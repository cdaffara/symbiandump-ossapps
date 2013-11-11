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


#ifndef DEVICEUPDATESPLUGIN_H_
#define DEVICEUPDATESPLUGIN_H_

#include <cpplugininterface.h>
#include <qobject.h>


class CpItemDataHelper;

class DeviceUpdatesPlugin : public QObject, public CpPluginInterface
{
	Q_OBJECT
  Q_INTERFACES(CpPluginInterface)

public:
		DeviceUpdatesPlugin();

    virtual ~DeviceUpdatesPlugin();
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
};

#endif /* DEVICEUPDATESPLUGIN_H_ */
