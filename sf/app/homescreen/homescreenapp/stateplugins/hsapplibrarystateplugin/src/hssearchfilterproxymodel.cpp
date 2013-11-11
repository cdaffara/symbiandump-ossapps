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
 * Description: Proxy model for search and highlight items view.
 *
 */

#include <QObject>
#include <hbcolorscheme.h>
#include <caitemmodel.h>

#include "hssearchfilterproxymodel.h"

/*!
 \class HsSearchFilterProxyModel
 \ingroup group_hsmenustateplugin
 \brief Proxy Model for highlight search string.
 Highlight search string in model item title.
 \lib ?library
 \see QSortFilterProxyModel
 */

/*!
 Constructor.
 \param parent parent for object.
 */
HsSearchFilterProxyModel::HsSearchFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mInSettingFilterString(false)
{
}

/*
 Destructor.
 */
HsSearchFilterProxyModel::~HsSearchFilterProxyModel()
{
}

/*
The data() method is overridden to have a place where the highlighting
 is added to the item data. In this implementation HTML-tags for 
 background color are added to the beginning and end of the match 
 in the data method using HbColorScheme.
 \param index source parent index.
 \param role item role.
 */
QVariant HsSearchFilterProxyModel::data(const QModelIndex &index, 
        int role) const
{
    QColor highlight = HbColorScheme::color("qtc_lineedit_marker_normal");
    QColor color = HbColorScheme::color("qtc_lineedit_selected");
    
    if (role == Qt::DisplayRole && !mFilterCriteria.isEmpty()) {
        QStringList textList = 
                QSortFilterProxyModel::data(index, role).toStringList();
        if(!textList[0].isEmpty())
        {
            QString start = QString(
                    TAG_START).arg( highlight.name().toUpper() ).arg(color.name().toUpper());
            int pos = textList[0].indexOf(mFilterCriteria, 0,Qt::CaseInsensitive);
            textList[0].insert(pos, start);
            textList[0].insert(pos + start.length() + mFilterCriteria.length(), TAG_END);
        }
        
        return QVariant(textList);
    }
        return QSortFilterProxyModel::data(index, role);

}
/*
SetFilterString is the way to set the filtering criteria that is used
 in highlight filtering. 
 QSortFilterProxyModel does not allow overriding its own filtering 
 criteria setting methods, so a separate method is added in the 
 HighlightFilterProxyModel. 
 This method stores the current filtering criteria and it can be 
 used in the data() method in highlighting the matches. 
 The method also calls the QSortFilterProxyModel method for setting 
 the filterCriteria. This triggers the base class to do the actual 
 filtering.
 \param index source parent index.
 \param role item role.
 */
void HsSearchFilterProxyModel::setFilterString(const QString &filterCriteria)
{
    mInSettingFilterString = true;
    mFilterCriteria = filterCriteria;
    setFilterRegExp(
            QRegExp("(^|\\b)" + filterCriteria, Qt::CaseInsensitive));
    mInSettingFilterString = false;
}

QString HsSearchFilterProxyModel::filterString() const
{
    return mFilterCriteria;
}

/*
 Reimplementated from QSortFilterProxyModel.
 The filterAcceptsRow method is overridden to send a dataChanged signal 
 to the list view item
 \param source_row row in source model.
 \param source_parent source parent index.
 */
bool HsSearchFilterProxyModel::filterAcceptsRow (int source_row, 
        const QModelIndex &source_parent) const
{
    bool retVal = QSortFilterProxyModel::filterAcceptsRow(source_row, 
            source_parent);
    if (mInSettingFilterString && retVal) {
        QModelIndex mi = mapFromSource(sourceModel()->index(source_row, 0, 
                source_parent));
        emit const_cast<HsSearchFilterProxyModel*>(this)->dataChanged(mi, mi);
    }
    return retVal;
}



