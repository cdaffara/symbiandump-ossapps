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

#include "cpprivacyplugin.h"
#include <cpcategorysettingformitemdata.h>
#include <cpitemdatahelper.h>

CpPrivacyPlugin::CpPrivacyPlugin()
{
    
}

CpPrivacyPlugin::~CpPrivacyPlugin()
{
    
}

QList<CpSettingFormItemData*> CpPrivacyPlugin::createSettingFormItemData(CpItemDataHelper &/*itemDataHelper*/) const
{
    CpCategorySettingFormItemData *itemData = 
            new CpCategorySettingFormItemData(
            HbDataFormModelItem::GroupItem,
            hbTrId("txt_cp_subhead_security"),
            QString("cpprivacyplugin.cpcfg") );
    return QList<CpSettingFormItemData*>() << itemData;
}

Q_EXPORT_PLUGIN2(cpprivacyplugin, CpPrivacyPlugin);
