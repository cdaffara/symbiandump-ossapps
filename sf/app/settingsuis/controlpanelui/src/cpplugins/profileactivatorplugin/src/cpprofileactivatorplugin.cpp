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

#include "cpprofileactivatorplugin.h"
#include "cpprofileactivatorentryitem.h"

CpProfileActivatorPlugin::CpProfileActivatorPlugin()
{
}

CpProfileActivatorPlugin::~CpProfileActivatorPlugin()
{
}

QList<CpSettingFormItemData*> CpProfileActivatorPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    CpSettingFormItemData* itemData = new CpProfileActivatorEntryItem(
                                            itemDataHelper,
                                            hbTrId("txt_cp_dblist_profile"),
                                            " ",
                                            "qtg_large_profiles");
    return QList<CpSettingFormItemData*>() << itemData;
}

Q_EXPORT_PLUGIN2(cpprofileactivatorplugin, CpProfileActivatorPlugin);
