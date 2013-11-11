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

#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <cpsettingformitemdata.h>
#include <apgtask.h>
#include "deviceupdatesdata.h"


DeviceUpdateData::DeviceUpdateData(CpItemDataHelper &itemDataHelper,
													   const QString &text /*= QString()*/,
													   const QString &description /*= QString()*/,
													   const HbIcon &icon /*= HbIcon()*/,
													   const HbDataFormModelItem *parent /*= 0*/)
													   : CpSettingFormEntryItemData(itemDataHelper,
													   text,
													   description,
													   icon,
													   parent)
{
	mproc = NULL;
}

DeviceUpdateData::~DeviceUpdateData()
{
	CloseDeviceUpdatesUi();
	if (mproc)
	{
		delete mproc;
		mproc = NULL;
	}
}

// -----------------------------------------------------------------------------
// DeviceUpdateData::OpenDmAppL()
// -----------------------------------------------------------------------------
//
void DeviceUpdateData::LaunchDeviceUpdatesUi()
	{	
		
		RWsSession ws;
    User::LeaveIfError( ws.Connect() );
    CleanupClosePushL( ws );
    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KUidSmlSyncApp );    
    if ( task.Exists() )
    {    	    
        task.BringToForeground();        
    }
    else
    {
 				// Create DM UI Process
    		if(!mproc)
    			mproc = new QProcess();
    		if(mproc->state() != QProcess::Running)
    		{
    			QString app = QLatin1String("deviceupdates");
    			QStringList args;
    			args<< QLatin1String("-cp");
    			mproc->start(app, args);
    			mproc->waitForStarted();
    		}     
    }
    CleanupStack::PopAndDestroy();  // ws    		
	}

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::CloseDmUi
// closes DM Ui
// ---------------------------------------------------------------------------------------------	
void DeviceUpdateData:: CloseDeviceUpdatesUi()
{
	if(( mproc )&&(mproc->state() == QProcess::Running))
  	{
    			mproc->close();
    }

}	

void DeviceUpdateData::onLaunchView()
{
	LaunchDeviceUpdatesUi();
}

CpBaseSettingView *DeviceUpdateData::createSettingView() const
{
		return 0;
}
