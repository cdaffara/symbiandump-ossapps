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
* Description:  
*
*/

#include "cntsettingsmodel.h"
#include <cntuids.h>
#include <qstringlist.h>
#include <xqsettingskey.h>
#include "cntdebug.h"

CntSettingsModel::CntSettingsModel() :
HbDataFormModel(),
mNameOrderkey(NULL),
mNameListRowSettingkey(NULL)
{
    // Create name ordering setting item
    createNameOrderSettingsItem();
    
    // Create names list row (1-row or 2-rows) setting item
    createNamesListRowSettingItem();
    
    connect(this, SIGNAL(dataChanged(QModelIndex , QModelIndex)), this, SLOT(handleDataChanged(QModelIndex , QModelIndex)));
}

void CntSettingsModel::createNameOrderSettingsItem()
{
    HbDataFormModelItem* ord = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem, hbTrId(
    "txt_phob_setlabel_name_display_order"));
    
    QStringList orderList;
    // This order should be maintained as per the UI spec
    orderList.append( hbTrId("txt_phob_setlabel_name_display_order_val_last_name") );
    orderList.append( hbTrId("txt_phob_setlabel_name_display_order_val_last_name_separator") );
    orderList.append( hbTrId("txt_phob_setlabel_name_display_order_val_first_nam") );
    
    ord->setContentWidgetData( "items", orderList );
    appendDataFormItem(ord, invisibleRootItem());
    mNameOrderkey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
                            KCRCntSettings.iUid,
                            KCntNameOrdering);
    int settingValue = mSettings.readItemValue(*mNameOrderkey, XQSettingsManager::TypeInt).toInt();

    if (settingValue == CntOrderLastFirst) {
        ord->setContentWidgetData("currentIndex", 0 );
    } else if (settingValue == CntOrderLastCommaFirst) {
        ord->setContentWidgetData("currentIndex", 1 );
    } else if (settingValue == CntOrderFirstLast) {
        ord->setContentWidgetData("currentIndex", 2 );
    }
}

void CntSettingsModel::createNamesListRowSettingItem()
{
    HbDataFormModelItem* rowSetting = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem, hbTrId(
        "txt_phob_setlabel_show"));
    
    QStringList rowSettingList;
    // This order should be maintained as per the UI spec
    rowSettingList.append( hbTrId("txt_phob_setlabel_show_val_name_only") );
    rowSettingList.append( hbTrId("txt_phob_setlabel_show_val_name_and_phonenumber") );

    rowSetting->setContentWidgetData( "items", rowSettingList );
    appendDataFormItem(rowSetting, invisibleRootItem());
    mNameListRowSettingkey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
                            KCRCntSettings.iUid,
                            KCntNameListRowSetting);
	int settingValue = mSettings.readItemValue(*mNameListRowSettingkey, XQSettingsManager::TypeInt).toInt();
    
    if (settingValue == CntOneRowNameOnly) {
         rowSetting->setContentWidgetData("currentIndex", 0 );
	} else if (settingValue == CntTwoRowsNameAndPhoneNumber) {
         rowSetting->setContentWidgetData("currentIndex", 1 );
    }
}

CntSettingsModel::~CntSettingsModel()
{
    delete mNameOrderkey;
    delete mNameListRowSettingkey;
}

void CntSettingsModel::handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight)
{
    Q_UNUSED(bottomRight)
    bool written(true);
            
    if (topLeft.row() == 0) {
        int selected = itemFromIndex(topLeft)->contentWidgetData( "currentIndex" ).toInt();
        if (selected == 0) {
            written = mSettings.writeItemValue(*mNameOrderkey, QVariant(CntOrderLastFirst));
        } else if (selected == 1) {
            written = mSettings.writeItemValue(*mNameOrderkey, QVariant(CntOrderLastCommaFirst));
        } else if (selected == 2) {
            written = mSettings.writeItemValue(*mNameOrderkey, QVariant(CntOrderFirstLast));
        }
        if (!written) {
            CNT_LOG_ARGS(QString("failed writting cenrep key"))
        }
    }
    else if (topLeft.row() == 1) {
        int selected = itemFromIndex(topLeft)->contentWidgetData( "currentIndex" ).toInt();
		if (selected == 0) {
            written = mSettings.writeItemValue(*mNameListRowSettingkey, QVariant(CntOneRowNameOnly));
        } else if (selected == 1) {
            written = mSettings.writeItemValue(*mNameListRowSettingkey, QVariant(CntTwoRowsNameAndPhoneNumber));
        }
        if (!written) {
            CNT_LOG_ARGS(QString("failed writting cenrep key"))
        }
    }
}
