/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Device updates plug-in for control panel
*
*/


#include <cpitemdatahelper.h>
#include "deviceupdatesplugin.h"
#include "deviceupdatesdata.h"



DeviceUpdatesPlugin::DeviceUpdatesPlugin()
{
	
}

DeviceUpdatesPlugin::~DeviceUpdatesPlugin()
{
	
}

QList<CpSettingFormItemData*> DeviceUpdatesPlugin::createSettingFormItemData(CpItemDataHelper& itemDataHelper) const
{
   
	return QList<CpSettingFormItemData*>()
					<< new DeviceUpdateData(itemDataHelper, QString("Device updates"),
                                          QString("Device updates plugin"), HbIcon(":/resources/qtg_large_device_update.svg"));
}

Q_EXPORT_PLUGIN2( deviceupdatesplugin, DeviceUpdatesPlugin)
