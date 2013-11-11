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
* Description:    Handles & propogates UI state change notifications.
*
*/



/**
 * @internal reviewed 24/08/2007 by D Holland
 */

// Class header
#include "glxtv.h"

// External includes
#include <w32std.h>                     // for TWsVisibilityChangedEvent

//  INTERNAL INCLUDES
#include <glxlog.h>                     // for debug logging 
#include <glxtracer.h>                  // for debug logging
#include <glxpanic.h>                   // for Gallery panic codes
#include <glxtvconstants.h>             // for ETvConnectionChanged, TV heights
#include <centralrepository.h>          // for CRepository
#ifdef __MARM
#include <GSServerEngine.h>
#endif

// GLXTVOUT INCLUDES
#include "mglxtvobserver.h"             // for MGlxTvObserver
#include "glxwindowvisibilitymonitor.h" // for CGlxWindowVisibilityMonitor
#include "glxtvconnectionmonitor.h"     // for CGlxTvConnectionMonitor

const TUid KCRUidTvoutSettings = {0x1020730B};

using namespace glxTvOut;

/**
*  CGlxTvOutCenRepMonitor
*  CGlxTv conainted class for observing changes in central 
*					 repository TV aspect Ratio value
* @author Loughlin Spollen
*/
NONSHARABLE_CLASS( CGlxTv::CGlxTvOutCenRepMonitor )
	                      : public CActive
	{
	public:  // Constructors and destructor
	    /**
	    * Symbian Constructor.
	    * @param The TV Observer
	    * @param The central repository
	    * @return constructed object
	    */
	    static CGlxTvOutCenRepMonitor* NewL(MGlxTvObserver& aTvObserver,
	                                    CRepository& aRepository);

	    /**
	    * Destructor.
	    */
	    ~CGlxTvOutCenRepMonitor();
	    
	private: 
	    /**
	    * C++ constructor.
	    * @param The TV Observer
	    * @param The central repository
	    * @return constructed object
	    */
	    CGlxTvOutCenRepMonitor(MGlxTvObserver& aTvObserver,
	                            CRepository& aRepository);
	    /**
	    * 2nd phase constructor
	    */
	    void ConstructL();
	    
	protected: // from CActive
	    /**
	    * @ref CActive::RunL
	    */
	    void RunL();

	    /**
	    * @ref CActive::DoCancel
	    */
	    void DoCancel();
	    
	    /**
	    * @ref CActive::RunError
	    */
	    TInt RunError(TInt aError);

	private:
	    
	    // Not Owned: the Glx TV observer
	    MGlxTvObserver& iTvObserver;
	     
		// Not Owned: the central repository API
	    CRepository& iRepository;       
	    
	    TUint iSettingsTVAspectRatio;
	    // the central repository identifier
	    TUid iRepositoryUid;
	}; 




// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
CGlxTv::CGlxTvOutCenRepMonitor* CGlxTv::CGlxTvOutCenRepMonitor::NewL
										        ( MGlxTvObserver& aTvObserver,
	                                                CRepository& aRepository )
    {
    TRACER("CGlxTv::CGlxTvOutCenRepMonitor::NewL()");
			    
    CGlxTvOutCenRepMonitor* self 
                = new(ELeave) CGlxTvOutCenRepMonitor( aTvObserver, aRepository );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }




// -----------------------------------------------------------------------------
// C++ constructor.
// -----------------------------------------------------------------------------
inline CGlxTv::CGlxTvOutCenRepMonitor::CGlxTvOutCenRepMonitor
                                                ( MGlxTvObserver& aTvObserver,
	                                              CRepository& aRepository )
                    				 : CActive( EPriorityStandard ),
                    				   iTvObserver ( aTvObserver ),
                 				       iRepository( aRepository )
    {
    TRACER("CGlxTv::CGlxTvOutCenRepMonitor::CGlxTvOutCenRepMonitor()");
    }



// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor 
// ----------------------------------------------------------------------------
void CGlxTv::CGlxTvOutCenRepMonitor::ConstructL()
    {
	TRACER("CGlxTv::CGlxTvOutCenRepMonitor::ConstructL");
    CActiveScheduler::Add( this );
#ifdef __MARM
    CGSServerEngine* gsServerEngine= CGSServerEngine::NewLC();
    iSettingsTVAspectRatio = gsServerEngine->AspectRatioL();
    CleanupStack::Pop(gsServerEngine);
#else
    iSettingsTVAspectRatio = 0;
#endif
    }

    

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CGlxTv::CGlxTvOutCenRepMonitor::~CGlxTvOutCenRepMonitor()
    {
    TRACER("CGlxTv::~CGlxTvOutCenRepMonitor()");
    Cancel();
    }



// ----------------------------------------------------------------------------
// CGlxTvOutCenRepMonitor::RunL
// From class CActive.
// ----------------------------------------------------------------------------
void CGlxTv::CGlxTvOutCenRepMonitor::RunL()
    {
    TRACER("CGlxTv::CGlxTvOutCenRepMonitor::RunL");
    // Check for errors
    User::LeaveIfError( iStatus.Int() );
    GLX_LOG_INFO("CGlxTvOutCenRepMonitor - RunL completed with Err Code"); 
    User::LeaveIfError( iRepository.NotifyRequest( iSettingsTVAspectRatio, iStatus ) );
    if (!IsActive())
        {
        SetActive();
        iTvObserver.HandleTvStatusChangedL( ETvConnectionChanged );
        }
    }



// ----------------------------------------------------------------------------
// CGlxTvOutCenRepMonitor::DoCancel
// From class CActive.
// ----------------------------------------------------------------------------
void CGlxTv::CGlxTvOutCenRepMonitor::DoCancel()
    {
    TRACER("CGlxTv::CGlxTvOutCenRepMonitor::DoCancel()");
    iRepository.NotifyCancel( iSettingsTVAspectRatio );  
    }



//-----------------------------------------------------------------------------
// CGlxTvOutCenRepMonitor::RunError
// From class CActive.
//-----------------------------------------------------------------------------
//
TInt CGlxTv::CGlxTvOutCenRepMonitor::RunError( TInt /*aError*/ )
    {
    TRACER("CGlxTv::CGlxTvOutCenRepMonitor::RunError()");
    return KErrNone;
    }
    

//-----------------------------------------------------------------------------
// Return new object
//-----------------------------------------------------------------------------
//
EXPORT_C CGlxTv* CGlxTv::NewL( MGlxTvObserver& aTvObserver ) 
    {
    TRACER("CGlxTv::NewL()");
    CGlxTv* self = new (ELeave) CGlxTv( aTvObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
    
    

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
EXPORT_C CGlxTv::~CGlxTv()
    {
    TRACER("CGlxTv::~CGlxTv()");
    StopMonitoringEnvironment();
    delete iTvConnectionMonitor;
    }




//-----------------------------------------------------------------------------
// Default C++ constructor
//-----------------------------------------------------------------------------
//
CGlxTv::CGlxTv( MGlxTvObserver& aTvObserver ) : iTvObserver( aTvObserver ),
                                                iAspectRatio( KErrUnknown )
    {
    TRACER("CGlxTv::CGlxTv()");
    }




//-----------------------------------------------------------------------------
// Symbian second phase constructor
//-----------------------------------------------------------------------------
//
void CGlxTv::ConstructL()
    {
    TRACER("CGlxTv::ConstructL()");
    // Don't create the visibility monitor until the TV Out cable is connected

    // Owned: The TV connection monitor
    iTvConnectionMonitor = CGlxTvConnectionMonitor::NewL( *this );

    if ( iTvConnectionMonitor->IsConnected() )
        {
        // Start monitoring the environment for changes
        MonitorEnvironmentL();

        // Calculate the TV Out screen buffer size
        CalcTvScreenSzL();
        }
    }




//-----------------------------------------------------------------------------
// returns the TV screen size 
//-----------------------------------------------------------------------------
//
EXPORT_C TSize CGlxTv::ScreenSizeL() const
    {
    TRACER("CGlxTv::ScreenSizeL()");
    return iSzInPixels;
    }




//-----------------------------------------------------------------------------
// Is the analog TV Connected
//-----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxTv::IsConnected() const
    {
    TRACER("CGlxTv::IsConnected()");
    return iTvConnectionMonitor->IsConnected();
    }
    
    
//-----------------------------------------------------------------------------
// Is the HDMI TV Connected
//-----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxTv::IsHDMIConnected() const
    {
    TRACER("CGlxTv::IsHDMIConnected()");
    return iTvConnectionMonitor->IsHDMIConnected();
    }
    
//-----------------------------------------------------------------------------
// Is the TV widescreen
//-----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxTv::IsWidescreen() const
    {
    TRACER("CGlxTv::IsWidescreen()");
    return iAspectRatio == KGlxTvAspectWide;
    }
    
    
//-----------------------------------------------------------------------------
// From class MGlxWindowVisibilityObserver.
// Called when window becomes fully visible or not visible.
//-----------------------------------------------------------------------------
//
void CGlxTv::HandleWindowVisibilityChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxTv::HandleWindowVisibilityChangedL()");
    iTvObserver.HandleTvStatusChangedL( aChangeType );
    }
    
    
    
//-----------------------------------------------------------------------------
// From class MGlxTvConnectionObserver.
// Called when TV connection state changes
//-----------------------------------------------------------------------------
//
void CGlxTv::HandleTvConnectionStatusChangedL(TTvChangeType aChangeType)
    {
	TRACER("CGlxTv::HandleTvConnectionStatusChangedL()");

	if (iTvConnectionMonitor->IsConnected())
		{
		// Start monitoring the environment for changes
		MonitorEnvironmentL();
		// Calculate the TV Out screen buffer
		CalcTvScreenSzL();
		}
	else
		{
		StopMonitoringEnvironment();
		}
	
	iTvObserver.HandleTvStatusChangedL(aChangeType);
	}
   
//-----------------------------------------------------------------------------
// Retrieve the TV display aspect ratio
//-----------------------------------------------------------------------------
//
void CGlxTv::CalcTvScreenSzL()
    {
    TRACER("CGlxTv::CalcTvScreenSzL()");

    // Retrieve the aspect ratio and the settings info from CenRep        
    iAspectRatio  = KErrNotFound;
   
#ifdef __MARM
    CGSServerEngine* gsServerEngine = CGSServerEngine::NewLC();
    iAspectRatio = gsServerEngine->AspectRatioL();
    CleanupStack::Pop(gsServerEngine);
	User::LeaveIfError( iAspectRatio );
#else
	iAspectRatio = 0;
#endif
    // note: Constants are defined in the GSServerEngine.cpp and not exported
    // they are defined locally in glxtvconstants.h
    switch( iAspectRatio )
        {
        case KGlxTvAspectWide:
            {
            iSzInPixels = TSize ( KGlxTvOutWidthWide, KGlxTvOutHeightWide) ;
            break;
            }
        case KGlxTvAspectNormal:
            {
            iSzInPixels = TSize ( KGlxTvOutWidth, KGlxTvOutHeight );
            break;
            }
        default:
            {
            User::Leave(KErrUnknown);
            }
        }
	}
    

//-----------------------------------------------------------------------------
// From class MonitorEnvironmentL.
// Called when TV connection state changes to connected
//-----------------------------------------------------------------------------
//
void CGlxTv::MonitorEnvironmentL()
    {
    TRACER("CGlxTv::MonitorEnvironmentL()");
  
    // Instantiate the central repository
    if (!iCenRep)
        {
        // The Uid is hardcoded here as the hrh file giving us the Uid has been depricated and 
        // moved to a private folder
        iCenRep = CRepository::NewL( KCRUidTvoutSettings );
        }
    
    // Monitor changes to the aspect ratio in the central repository
    if (!iTvDisplayAspectRatioMonitor)    
        {
        iTvDisplayAspectRatioMonitor 
                    = CGlxTvOutCenRepMonitor::NewL( iTvObserver, *iCenRep );
        }
    
    // create window visibility monitoring object
    if ( !iWindowVisibilityMonitor )
        {
        iWindowVisibilityMonitor = 
                                CGlxWindowVisibilityMonitor::NewL( *this );
        }
    }

    
//-----------------------------------------------------------------------------
// StopMonitoringEnvironment().
// Called when TV connection state changes to disconnected
//-----------------------------------------------------------------------------
//
void CGlxTv::StopMonitoringEnvironment()
    {
    TRACER("CGlxTv::StopMonitoringEnvironment()");
    delete iTvDisplayAspectRatioMonitor; // destroy before iCenRep - must cancel 
    iTvDisplayAspectRatioMonitor = NULL; // outstanding requests
    if (iWindowVisibilityMonitor)
        {
        iWindowVisibilityMonitor->Close();
        }
    delete iWindowVisibilityMonitor;
    iWindowVisibilityMonitor = NULL;
    delete iCenRep;
    iCenRep = NULL;
    }

    
// End of file
