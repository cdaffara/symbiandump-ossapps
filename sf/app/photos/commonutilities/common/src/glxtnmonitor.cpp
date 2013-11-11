/*
* ============================================================================
*  Name        : glxscreensavermonitor.cpp
*  Part of     : GLX / Collection Data Source Framework
*  Description : 
*  Version     : %version:  1 % 
*
*  Copyright © 2006-2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

#include "glxtnmonitor.h"
#include <glxlog.h>

//#include <ScreenSaverInternalPSKeys.h>
#include <e32debug.h>

const TUid KTAGDPSNotification = { 0x2001FD51 };
//const TInt KForceBackgroundGeneration = 0x00000010;
const TInt KItemsleft = 0x00000008;

CGlxTNMonitor::CGlxTNMonitor( MGlxTNObserver *observer )
: CActive( CActive::EPriorityHigh ),
  iObserver( observer )
{    
}

// ---------------------------------------------------------------------------
// ~CGlxPauseMonitor.
// Destructor.
// ---------------------------------------------------------------------------
//
CGlxTNMonitor::~CGlxTNMonitor()
{
	Cancel();
	iScreenSaverProperty.Close();
}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxTNMonitor* CGlxTNMonitor::NewL( MGlxTNObserver *observer  )
{
    //GLX_LOG_ENTRY_EXIT("CGlxTNMonitor::NewL");    
    CGlxTNMonitor* self = new (ELeave) CGlxTNMonitor( observer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// ---------------------------------------------------------------------------
// ConstructL.
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CGlxTNMonitor::ConstructL()
{
    iScreenSaverProperty.Attach( KTAGDPSNotification, KItemsleft );
	CActiveScheduler::Add( this );
    RunL();
}

void CGlxTNMonitor::RunL()
{	
	GLX_LOG_ENTRY_EXIT("CGlxTNMonitor::RunL");    
    // resubscribe before processing new value to prevent missing updates
	iScreenSaverProperty.Subscribe( iStatus );
    SetActive();
	//User::WaitForRequest(iStatus);
    TInt intValue;
	TInt err = iScreenSaverProperty.Get( intValue ) ;
	if ( err != KErrNotFound ){
	    iObserver->updateTNCount( intValue );
	}
}
    
// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxTNMonitor::DoCancel()
{
	iScreenSaverProperty.Cancel();
}

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CGlxTNMonitor::RunError( TInt )
{
    GLX_LOG_ENTRY_EXIT("CGlxTNMonitor::RunError");       
    return KErrNone;
}


