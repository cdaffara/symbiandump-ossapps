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
#include "cpkeyscreenplugin.h"
#include "cpkeyscreenview.h"
#include <cpsettingformentryitemdataimpl.h>

CpKeyScreenPlugin::CpKeyScreenPlugin()
{
}

CpKeyScreenPlugin::~CpKeyScreenPlugin()
{
}

QList<CpSettingFormItemData*> CpKeyScreenPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    CpSettingFormItemData* itemData = new CpSettingFormEntryItemDataImpl<CpKeyScreenView>(
            CpSettingFormEntryItemData::ListEntryItem,  // item type
			itemDataHelper,
			hbTrId("txt_cp_list_keys_screen"),  // text
			QString(""),                       // description
			"qtg_large_key_screen");            // icon name
    return QList<CpSettingFormItemData*>() << itemData;
}

Q_EXPORT_PLUGIN2(cpkeyscreenplugin, CpKeyScreenPlugin);
