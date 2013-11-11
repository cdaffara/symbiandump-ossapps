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

#include "DmTelServiceHandler.h"
#include <hbapplication.h>

DmTelServiceHandler::DmTelServiceHandler(QObject *parent):
    XQServiceProvider("com.nokia.services.devicemanager", parent)
{
    publishAll();
}

DmTelServiceHandler::~DmTelServiceHandler()
{
}

void DmTelServiceHandler::showVersionNumber()
{
// Create DM UI Process
RProcess rp;
TInt err = rp.Create(KDMUIProcess,KNullDesC);
User::LeaveIfError(err);  
rp.SetPriority(EPriorityForeground);
rp.Resume();    // logon OK - start the server

// quit the service provider app.
//QCoreApplication::quit();	
QCoreApplication::exit(0);		  
}
