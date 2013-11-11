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

#include <QCoreApplication> 

#include "irmemorycollector.h"
#include "irapplication.h"
#include "irviewmanager.h"
 

IRMemoryCollector::IRMemoryCollector(IRApplication* aApplication) : iApplication(aApplication)
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

void IRMemoryCollector::aboutToQuit()
{
    IRViewManager *viewManager = iApplication->getViewManager();
    
    delete iApplication;
    iApplication = NULL;
        
    delete viewManager;  
}
