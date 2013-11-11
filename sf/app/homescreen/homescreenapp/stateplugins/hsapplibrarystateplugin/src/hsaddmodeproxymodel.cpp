/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: Proxy model for view in add to homescreen mode.
 *
 */

#include <QObject>
#include <canotifier.h>

#include "hscontentservice.h"
#include "hsaddmodeproxymodel.h"
#include "hsmenuitemmodel.h"
#include "hsapplibstateutils.h"

/*!
 \class HsAddModeProxyModel
 \ingroup group_hsmenustateplugin
 \brief Proxy Model for add to homescreen mode.
 Filtering cwrt widgets to not displaing them in add mode when they are alredy present on HS.
 \lib ?library
 \see QSortFilterProxyModel
 */

/*!
 Constructor.
 \param parent parent for object.
 */
HsAddModeProxyModel::HsAddModeProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mCwrtWidgetCache(),
    mQuery()
{
    initilizeCwrtWidgetCache();
    invalidateFilter();
    connect(HsContentService::instance(),
        SIGNAL(widgetAdded(const QString &, const QVariantHash &)),
        this, SLOT(updateCacheOnAddWidget(const QString&, const QVariantHash&)));
    connect(HsContentService::instance(),
        SIGNAL(widgetRemoved(const QString &, const QVariantHash &)),
        this, SLOT(updateCacheOnRemoveWidget(const QString&, const QVariantHash&)));
    // create notifier for a cwrt widgets with query created in initilizeCwrtWidgetCache
    CaNotifierFilter filter(mQuery);
    mNotifier.reset(CaService::instance()->createNotifier(filter));
    connect(mNotifier.data(),
            SIGNAL(entryChanged(const CaEntry&, ChangeType)),
            this,
            SLOT(updateEntryStatus(const CaEntry&, ChangeType)));
}

/*
 Destructor.
 */
HsAddModeProxyModel::~HsAddModeProxyModel()
{
}

void HsAddModeProxyModel::initilizeCwrtWidgetCache()
{
    QStringList entryTypeNames;
    entryTypeNames.append(Hs::applicationTypeName);
    mQuery.setEntryTypeNames(entryTypeNames);
    mQuery.setAttribute(Hs::swTypeKey, Hs::HS_CWRT_APP_TYPE);
    mQuery.setAttribute(Hs::widgetUriAttributeName,
                       Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE);
    QList< QSharedPointer<CaEntry> > entries =
            CaService::instance()->getEntries(mQuery);
    foreach (QSharedPointer<CaEntry> entry, entries) {
        QVariantHash preferences;
        QMap<QString, QString> attributes = entry->attributes();
        QMapIterator<QString, QString> i(attributes);
        while (i.hasNext()) {
            i.next();
            QString key(i.key());
            QString value(i.value());
            if (key.contains(Hs::widgetParam)) {
                preferences.insert(key.remove(Hs::widgetParam),value);
            }
        }
        int count(0);
        HsContentService::instance()->widgets(
                Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE,preferences,count);
        mCwrtWidgetCache.insert(entry->id(), count > 0);
    }

}

/*
 Reimplementated from QSortFilterProxyModel to forward signals from CaItemModel.
 \param sourceModel source model.
 */
void HsAddModeProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    connect(sourceModel,
            SIGNAL(scrollTo(int, QAbstractItemView::ScrollHint)),
            this,
            SIGNAL(scrollTo(int, QAbstractItemView::ScrollHint)));
    connect(sourceModel,
            SIGNAL(countChange()),
            this,
            SIGNAL(countChange()));
}

/*!
 Return entryid for given Cwrt with mini view widget.
 \param uri of a widget.
 \param preferences widget preferences.
 \retval int entry id or 0 for not cwrt with mini view widgets.
 */
int HsAddModeProxyModel::findCwrtWidgetEntryId(const QVariantHash &preferences)
{
    int result = 0;
    CaQuery query;
    QStringList entryTypeNames;
    query.setEntryTypeNames(entryTypeNames);
    query.setAttribute(Hs::swTypeKey, Hs::HS_CWRT_APP_TYPE);
    query.setAttribute(Hs::widgetUriAttributeName,
            Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE);
    foreach (QString key, preferences.keys()) {
        query.setAttribute(Hs::widgetParam+key,
            preferences.value(key).toString());
    }
    QList< QSharedPointer<CaEntry> > entries =
        CaService::instance()->getEntries(query);
    if (entries.count() > 0) {
        result = entries[0]->id();
    }
    return result;
}

/*
 Slot called when widget is added to homescreen.
 If widget is removed we checkhow many instance of this widget is left on HS.
 \param uri of a widget.
 \param preferences widget preferences.
 */
void HsAddModeProxyModel::updateCacheOnAddWidget(const QString &uri,
    const QVariantHash &preferences)
{
	if (uri == Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE) {
        int entryId = findCwrtWidgetEntryId(preferences);
        if (!mCwrtWidgetCache.value(entryId) ) {
            mCwrtWidgetCache.insert(entryId,true);
            invalidateFilter();
        }
    }
}

/*
 Slot called when widget is removed from homescreen.
 If widget is removed we checkhow many instance of this widget is left on HS.
 \param uri of a widget.
 \param preferences widget preferences.
*/
void HsAddModeProxyModel::updateCacheOnRemoveWidget(const QString &uri,
    const QVariantHash &preferences)
{
    if (uri == Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE) {
        int entryId = findCwrtWidgetEntryId(preferences);
        if (mCwrtWidgetCache.value(entryId)) {
            mCwrtWidgetCache.insert(entryId,false);
            invalidateFilter();
        }
    }
}

/*
 Reimplementated from QSortFilterProxyModel.
 \param source_row row in source model.
 \param source_parent source parent index.
 */
bool HsAddModeProxyModel::filterAcceptsRow(int source_row,
        const QModelIndex &source_parent) const
{
    QVariant entryId = sourceModel()->data(
            sourceModel()->index(source_row,0,source_parent),
            CaItemModel::IdRole);
    return !mCwrtWidgetCache.value(entryId.toInt());
}

/*
 Update cache if some cwrt widgets were install/uninstal/update or on mmc card.
 \param entry changed entry.
 \param changeType the type of the change. 
 */
void HsAddModeProxyModel::updateEntryStatus(const CaEntry &entry, ChangeType changeType)
{
    if (changeType == RemoveChangeType) {
        mCwrtWidgetCache.remove(entry.id());
    } else {
        mCwrtWidgetCache.insert(entry.id(),
            HsAppLibStateUtils::isCWRTWidgetOnHomeScreen(&entry));
    }
    invalidateFilter();
}

