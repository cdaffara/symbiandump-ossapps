/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <QAbstractListModel>

#include "cxutils.h" // debug
#include "cxuienums.h"
#include "cxuisettingsinfo.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuisettingradiobuttonlistmodel.h"



/*!
* CxuiSettingRadioButtonListModel::CxuiSettingRadioButtonListModel
*/
CxuiSettingRadioButtonListModel::CxuiSettingRadioButtonListModel() :
   QAbstractListModel(),
   mListBoxType(CxuiSettingRadioButtonList::SingleLine)

{
}



/*!
* CxuiSettingRadioButtonListModel::CxuiSettingRadioButtonListModel
*/
void CxuiSettingRadioButtonListModel::resetModel()
{
    CX_DEBUG_ENTER_FUNCTION();

    // starts reseting the model
    beginResetModel();

    mItems.clear();
    mListBoxType = CxuiSettingRadioButtonList::SingleLine;

    // ends reseting the model
    endResetModel();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Reads data
*/
QVariant CxuiSettingRadioButtonListModel::data(const QModelIndex &index, int role) const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxuiSettingRadioButtonListModel: index.row() = %d", index.row()));

    QVariant data;

    if (!index.isValid()) {
        CX_DEBUG(("[WARNING] CxuiSettingRadioButtonListModel: index.isValid() not true!"));
    } else if (index.row() >= rowCount(index)) {
        CX_DEBUG(("[WARNING] CxuiSettingRadioButtonListModel: row too large, row count %d!", rowCount(index)));
    } else if (role == Qt::DisplayRole) { // Happy case
        QStringList list;
        QString setting = mItems.at(index.row());
        CX_DEBUG(("CxuiSettingRadioButtonListModel: data [%s]", setting.toAscii().constData()));

        if (mListBoxType == CxuiSettingRadioButtonList::TwoLine) {
            CX_DEBUG(("CxuiSettingRadioButtonListModel: Listbox type is TwoLineListBox"));
            // two line list box
            // get the two strings
            QStringList lines = setting.split(CxUiSettings::NEW_LINE_CHAR);
            // Split returns always atleast a single element list.
            list << lines[0];
            if (lines.size() > 1) {
                list << lines[1];
            }
        } else {
            CX_DEBUG(("CxuiSettingRadioButtonListModel: Listbox type is SingleLine"));
            list << setting;
        }

        data = QVariant(list);
    } else {
        // No action
    }

    CX_DEBUG_EXIT_FUNCTION();
    return data;
}

/*!
Returns number of rows in the radio button setting page.
*/
int CxuiSettingRadioButtonListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mItems.count();
}


/*!
Sets the items visible in the radio button list
*/
void CxuiSettingRadioButtonListModel::setItems(QStringList items)
{
    CX_DEBUG_ENTER_FUNCTION();
    mItems = items;

    // Notify radio button list that the data content has changed.
    if (mItems.size() > 0) {
        const QModelIndex start = index(0, 0);
        const QModelIndex end = index(mItems.size() - 1, 0);
        emit dataChanged(start, end);
    }

    // Relayouting needed.
    emit layoutChanged();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
Sets the items visible in the radio button list
*/
QStringList CxuiSettingRadioButtonListModel::items() const
{
    return mItems;
}

/*!
Sets the items visible in the radio button list
*/
void
CxuiSettingRadioButtonListModel::setListBoxType(int type)
{
   mListBoxType = type;
}

int CxuiSettingRadioButtonListModel::listBoxType() const
{
   return mListBoxType;
}
