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
* Description: 
*
*/
#include <eikenv.h>
#include "application.h"
#include "enginewrapper.h"

#define KEY_CAPTURE_PRIORITY 100

SGApplication::SGApplication(int argc, char *argv[])  
    : HbApplication(argc, argv) 
     {
	 
	
     }

	 
SGApplication::~SGApplication()  
{
		
}	
	 
// ---------------------------------------------------------------------------

#if defined(Q_WS_S60)
#if defined(HB_QT_S60_EVENT_FILTER)
bool SGApplication::s60EventFilter(TWsEvent *aEvent)
#else
bool SGApplication::symbianEventFilter(const QSymbianEvent *aEvent)
#endif // (HB_QT_S60_EVENT_FILTER)
{
    // pass event to engine, if engine returns true(event was not handled),
    // forward it.
    if(!iEngine->handleCaptureCommands(aEvent)) 
        {
        return false;
        }
    else
        {
#if defined(HB_QT_S60_EVENT_FILTER)
        return HbApplication::s60EventFilter(aEvent);
#else
		return HbApplication::symbianEventFilter(aEvent);
#endif		
        }
    
    }
#endif

// ---------------------------------------------------------------------------
