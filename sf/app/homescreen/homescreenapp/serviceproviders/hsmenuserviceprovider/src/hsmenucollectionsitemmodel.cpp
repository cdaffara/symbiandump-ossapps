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
 * Description: menucollectionsitemmodel.cpp
 *
 */

#include "caservice.h"
#include "caentry.h"
#include "hsapp_defs.h"
#include "hsmenucollectionsitemmodel.h"

/*!
 Constructor
 \param query
 \param parent
 */
HsMenuCollectionsItemModel::HsMenuCollectionsItemModel(
    const CaQuery &query, QObject *parent) :
    HsMenuItemModel(query, parent)
{

}

/*!
 Destructor
 */
HsMenuCollectionsItemModel::~HsMenuCollectionsItemModel()
{
}

/*!
 Returns data of particular collection
 \param index
 \param role
 \retval
 */
QVariant HsMenuCollectionsItemModel::data(const QModelIndex &index,
        int role) const
{
    HSMENUTEST_FUNC_ENTRY("HsMenuCollectionsItemModel::data");
    QVariant variant;
    if (role == Qt::DisplayRole && secondLineVisibility()) {
        QList<QVariant> text;
        QSharedPointer<CaEntry> item = entry(index);
        text << item->text();
        text << getSecondLine(item.data());
        variant =  QVariant(text);
    } else {
        variant = CaItemModel::data(index, role);
    }
    HSMENUTEST_FUNC_EXIT("HsMenuCollectionsItemModel::data");
    return variant;
}

/*!
 Returns names of applications that belong to a collection
 of count of applications that belong to a collection
 (used for certain dynamic collections)
 \param entry of an collection
 \retval string with applications names separated by ','
 no longer than 256 bytes
 */
QString HsMenuCollectionsItemModel::getSecondLine(const CaEntry *entry) const
{
    HSMENUTEST_FUNC_ENTRY("HsMenuCollectionsItemModel::getSecondLine");
    QString result;

    CaQuery query;
    query.setParentId(entry->id());
    query.setEntryRoles(ItemEntryRole);

    if (entry->entryTypeName() == Hs::collectionDownloadedTypeName) {
        query.setFlagsOn(RemovableEntryFlag | VisibleEntryFlag);
    } else {
        query.setFlagsOn(VisibleEntryFlag);
    }

    query.setFlagsOff(MissingEntryFlag);

    QList< QSharedPointer<CaEntry> > entries = CaService::instance()->getEntries(query);

    if (!entries.count()) {
        result = QString(hbTrId("txt_applib_dblist_downloaded_val_empty"));
    }//if
    else {
        int count(0);
        if (entry->entryTypeName() == Hs::collectionDownloadedTypeName) {
            for (int i = 0; i < entries.count(); ++i) {
                if ((entries[i]->flags() & UsedEntryFlag) != UsedEntryFlag) {
                    count++;
                }
            }
        }
        if (count) {
            result = hbTrId("txt_applib_dblist_val_ln_new_applications",
                            count);
        } else {
            for (int i = 0; i < entries.count(); ++i) {
                result.append(entries.at(i)->text());
                if (i < entries.count() - 1) {
                    // do not append comma if it is last element
                    result.append(",");
                }//if
                if (result.length() > 256) {
                    // we don't need to store more than 256 bytes
                    break;
                }
            }
        }
    }
    HSMENUTEST_FUNC_EXIT("HsMenuCollectionsItemModel::getSecondLine");
    return result;
}
