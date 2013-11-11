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

#include "cntserviceviewmanager.h"
#include "cntserviceviewfactory.h"
#include <cntabstractengine.h>
/*!
Constructor
*/
CntServiceViewManager::CntServiceViewManager( HbMainWindow *mainWindow,
    CntAbstractServiceProvider& aProvider ):
    CntDefaultViewManager(mainWindow)
{
    // ownership of the factory is given to the base class.
    setViewFactory( new CntServiceViewFactory( aProvider, engine().extensionManager() ) );
}

/*!
Destructor
*/
CntServiceViewManager::~CntServiceViewManager()
{
/*Remove all views*/
}

void CntServiceViewManager::closeApp()
{
    // Quit happens when returnValueDelivered signal from XQServiceProver
    // is caught from service handler.
    emit applicationClosed();
}

// end of file
