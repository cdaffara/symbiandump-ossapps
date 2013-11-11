/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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
#include "cppersonalizationgroupitemdata.h"
#include <cplogger.h>
#include <hbdataformmodelitem.h>
#include <cpsettingformentryitemdataimpl.h>
#include <cpbasesettingview.h>
#include <cpitemdatahelper.h>
#include <hbmessagebox.h>
#include <cpprofilemodel.h>
#include "cppersonalizationadvanceview.h"
#include "cppersonalizationentryitemdata.h"


CpPersonalizationGroupItemData::CpPersonalizationGroupItemData(HbDataFormModelItem::DataItemType type,
                                                               const QString &label,
                                                               const QString &configFile,
                                                               const HbDataFormModelItem *parent):
                                                               CpCategorySettingFormItemData(type, label, configFile, parent)
                                                               
{
    CPFW_LOG("CpPersonalizationGroupItemData::CpPersonalizationGroupItemData(), START");
	CPFW_LOG("CpPersonalizationGroupItemData::CpPersonalizationGroupItemData(), END");
}

CpPersonalizationGroupItemData::~CpPersonalizationGroupItemData()
{
}

void CpPersonalizationGroupItemData::beforeLoadingConfigPlugins(CpItemDataHelper &/*itemDataHelper*/)
{

    // keep this interface for development in the futrue
    CPFW_LOG("CpPersonalizationGroupItemData::beforeLoadingConfigPlugins(), START");

	CPFW_LOG("CpPersonalizationGroupItemData::beforeLoadingConfigPlugins(), END");
}

void CpPersonalizationGroupItemData::afterLoadingConfigPlugins(CpItemDataHelper &itemDataHelper)
{
    CPFW_LOG("CpPersonalizationGroupItemData::afterLoadingConfigPlugins(), START");
    CpSettingFormEntryItemData *advanceSettingItem = 
            new CpSettingFormEntryItemDataImpl<CpPersonalizationAdvanceView>(CpSettingFormEntryItemData::ButtonEntryItem,
            		itemDataHelper, hbTrId("txt_cp_button_advanced_settings"));
    
    advanceSettingItem->setContentWidgetData("textAlignment", QVariant( Qt::AlignHCenter | Qt::AlignVCenter) );
    advanceSettingItem->setContentWidgetData("objectName", "advanceSettingButton" );
    appendChild(advanceSettingItem);
    CPFW_LOG("CpPersonalizationGroupItemData::afterLoadingConfigPlugins(), END");
}

