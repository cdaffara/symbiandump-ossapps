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
#include <HbInstance>
#include "irhbapplication.h"

IRHbApplication::IRHbApplication(int &argc, char *argv[], Hb::ApplicationFlags flags) 
                : HbApplication(argc, argv, flags)
{
    
}

void IRHbApplication::initMattiAutomation()
{
    HbMainWindow *window = hbInstance->allMainWindows().first();
    connect(window, SIGNAL(viewReady()), this, SLOT(handleAppReady())); 
}

void IRHbApplication::handleAppReady()
{
    HbMainWindow *window = hbInstance->allMainWindows().first();
    if (NULL != window->currentView())
    {
        emit applicationReady();
        disconnect(window, SIGNAL(viewReady()), this, SLOT(handleAppReady()));
    }
}
