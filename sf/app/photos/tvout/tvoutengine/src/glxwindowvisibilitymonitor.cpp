/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Class that monitors tv window visibility
*
*/



//  CLASS HEADER
#include "glxwindowvisibilitymonitor.h"

//  EXTERNAL INCLUDES
#include <w32std.h>             // for TWsEvent
#include <AknDef.h>             // for KAknFullOrPartialForegroundLost
#include <AknWsEventObserver.h> // for EventMonitor
#include <aknappui.h>

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxpanic.h>
#include "glxtv.h"              // for MGlxWindowVisibilityObserver



//-----------------------------------------------------------------------------
// Return new object
//-----------------------------------------------------------------------------
//
CGlxWindowVisibilityMonitor* CGlxWindowVisibilityMonitor::NewL(
                               MGlxWindowVisibilityObserver& aVisibilityObserver ) 
    {
    GLX_LOG_INFO("CGlxWindowVisibilityMonitor::NewL");
    CGlxWindowVisibilityMonitor* self = new ( ELeave ) 
                CGlxWindowVisibilityMonitor( aVisibilityObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
CGlxWindowVisibilityMonitor::~CGlxWindowVisibilityMonitor()
    {
    GLX_LOG_INFO("~CGlxWindowVisibilityMonitor");
    }


//-----------------------------------------------------------------------------
// Close
// To overcome code-scanner high rated warning
//-----------------------------------------------------------------------------
//
void CGlxWindowVisibilityMonitor::Close()
    {
    iAknEventMonitor->Enable( EFalse );
    iAknEventMonitor->RemoveObserver( this );
    }

//-----------------------------------------------------------------------------
// Default C++ constructor
//-----------------------------------------------------------------------------
//
CGlxWindowVisibilityMonitor::CGlxWindowVisibilityMonitor
                           ( MGlxWindowVisibilityObserver& aVisibilityObserver ) 
                            :iVisibilityObserver ( aVisibilityObserver )
    {
    GLX_LOG_INFO("CGlxWindowVisibilityMonitor");
    }


//-----------------------------------------------------------------------------
// Symbian second phase constructor
//-----------------------------------------------------------------------------
//
void CGlxWindowVisibilityMonitor::ConstructL()
    {
    GLX_LOG_INFO("CGlxWindowVisibilityMonitor::ConstructL");
    // Register for visibility events
    iAknEventMonitor = 
       static_cast<CAknAppUiBase*>(CCoeEnv::Static()->AppUi())->EventMonitor();
    iAknEventMonitor->Enable( ETrue );
    iAknEventMonitor->AddObserverL( this );
    }


//-----------------------------------------------------------------------------
// From class MAknWsEventObserver
// Propagates window server visibility events to the observer
//-----------------------------------------------------------------------------
//
void CGlxWindowVisibilityMonitor::HandleWsEventL( const TWsEvent& aEvent, 
                                                CCoeControl* /*aDestination*/ )
    {
    GLX_LOG_INFO("CGlxWindowVisibilityMonitor::HandleWsEventL");
	TInt eventType = aEvent.Type();

    if ( eventType == EEventWindowVisibilityChanged )
        {
        // check the state 
        TUint visible = aEvent.VisibilityChanged()->iFlags;
        // notify the observer
        if ( visible & TWsVisibilityChangedEvent::EFullyVisible )
            {
            GLX_LOG_INFO("Visibility Event - EFullyVisible");
            iVisibilityObserver.HandleWindowVisibilityChangedL( ETvDisplayIsVisible ); 
            }
        else if ( visible & TWsVisibilityChangedEvent::ENotVisible )
            {
            GLX_LOG_INFO("Visibility Event - ENotVisible");
            iVisibilityObserver.HandleWindowVisibilityChangedL( ETvDisplayNotVisible ); 
            }
         else if (visible & TWsVisibilityChangedEvent::EPartiallyVisible)   
            {
            GLX_LOG_INFO("Visibility Event - EPartiallyVisible");
            iVisibilityObserver.HandleWindowVisibilityChangedL( ETvDisplayNotVisible ); 
            }
        }
    }





