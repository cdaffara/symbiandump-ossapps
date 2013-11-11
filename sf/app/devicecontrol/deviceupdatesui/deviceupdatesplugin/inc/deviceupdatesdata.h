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
* Description:  Device updates data for control panel
*
*/

#ifndef	DEVICEUPDATESDATA_H_
#define	DEVICEUPDATESDATA_H_

#include <QtCore/QProcess>
#include <cpsettingformentryitemdata.h>

_LIT( KDMUIProcess, "\\deviceupdates.exe" );
_LIT( KDMUIName, "deviceupdates" );


const TUid KUidSmlSyncApp = { 0x101F6DE5 };

class DeviceUpdateData : public CpSettingFormEntryItemData
{
    Q_OBJECT
public:
    explicit DeviceUpdateData(CpItemDataHelper &itemDataHelper,
												const QString &text = QString(),
												const QString &description = QString(),
												const HbIcon &icon = HbIcon(),
												const HbDataFormModelItem *parent = 0);
    void LaunchDeviceUpdatesUi();
    void CloseDeviceUpdatesUi();
    virtual ~DeviceUpdateData();
public slots:
	void onLaunchView();
private:
	virtual CpBaseSettingView *createSettingView() const;
	
private:
		QProcess *mproc;

};
#endif//	DEVICEUPDATESVIEW_H_
