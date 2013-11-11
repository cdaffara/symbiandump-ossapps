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

#include "cpvolumeplugin.h"
#include "cpvolumegroupitemdata.h"
#include <cpitemdatahelper.h>

CpVolumePlugin::CpVolumePlugin()
{
}

CpVolumePlugin::~CpVolumePlugin()
{
}

QList<CpSettingFormItemData*> CpVolumePlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    
    CpVolumeGroupItemData *volumeGroupItem = new CpVolumeGroupItemData(itemDataHelper);

    return QList<CpSettingFormItemData*>() << volumeGroupItem ;
}

Q_EXPORT_PLUGIN2(cpvolumeplugin, CpVolumePlugin);


