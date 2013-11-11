/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  LiveComm Videotelephony Plugin session Class
*
*/

#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>
#include <featmgr.h>
 
#include <w32std.h>
#include <coecntrl.h>
#include <eikenv.h>
#include <eikappui.h>
#include <apgcli.h>
#include <centralrepository.h> 

#include "clcvtsession.h"

#include "cvtengmodel.h"

#include "clcvtvideoplayerbase.h"
#include "clcvtlocalvideoplayer.h"
#include "clcvtremotevideoplayer.h"
#include "tlcvtstates.h"
#include "clcvtcmdexecutor.h"
#include "mlcvtenginecommandmanager.h"
#include "lcvtplugincommand.h"
#include "lcvtutility.h"

// User selectable Call answer status values
const TInt KAllwaysQuery = 0;
const TInt KAllwaysAllow = 2;

// Default call index.
const TInt KVtUiDefaultCallId = 1;

// Granularity of command observer array.
const TInt KLcVtCommandObserverArrayGranularity = 5;



// Enumerates states for CLcVtActiveExec.
enum
    {    
    // Operation finished. Must equal to zero.
    EVtSessionNone,
    
    // Active execution states for iActiveExec:
    
    //     Startup (prepare engine):
    EVtSessionStartup = 100,                                    // 100
    //     Initialise engine.
    EVtSessionStartupInitEngine = EVtSessionStartup,            // 100
    //     Prepare viewfinder.
    EVtSessionStartupPrepareViewfinder,                         // 101
    //     Prepare remote render.
    EVtSessionStartupPrepareRemoteRender,                       // 102
    //     Startup Finish 
    EVtSessionStartupFinish,                                    // 103
    
    //     Shutdown (reset engine):
    EVtSessionShutdown = 300,                                   // 300
    //     Hide application.
    EVtSessionShutdownHideApplication = EVtSessionShutdown,     // 300
    //     Reset engine.
    EVtSessionShutdownResetEngine,                              // 301
    
        
    //     Answered:
    EVtSessionAnswered = 500,                                   // 500    
    //     Start remote render.
    EVtSessionAnsweredStartRemoteRender = EVtSessionAnswered,   // 500    
    //     Prepare Camere
    EVtSessionAnsweredDoPrepareCamera,                          // 501
    //     Select camera as source.
    EVtSessionAnsweredSetlectCamera,                            // 502
    //     Select blank image as source.
    EVtSessionAnsweredSetlectNone,
    //     Start view finder
    EVtSessionAnsweredStartViewFinder,                          // 504
    //     Show Application 
    EVtSessionAnsweredBringToForeground,                        // 505
    
    //EVtSessionAnswered = 500,                                   // 500
    //EVtSessionAnsweredBringToForeground = EVtSessionAnswered,                        // 505
    
    //     Finish.
    EVtSessionAnsweredFinish                                    // 506    
    };


/**
 * Encapsulates event handling.
 * @since Series 60 2.6
 */    
class CLcVtSession::CEventObserver
    : public CBase,
      public MLcVtEngineCommandManager,
      public MVtEngEventObserver,
#ifndef NGA_VIDEO_RENDER
      public MVtEngFrameObserver,
#endif
      public MVtEngCommandObserver
    {    
    public: // Constructors and destructors
     
        /**
        * Two-phased constructor.
        * @param aSession plugin session.
        */        
        static CEventObserver* NewL(CLcVtSession& aSession);
        
        /**
        * Destructor.
        */        
        ~CEventObserver();

    public: // Functions from base classes

        /**
        * @see MLcVtEngineCommandManager::AddObserverL.
        */
        void AddObserverL( MVtEngCommandObserver& aObserver );

        /**
        * @see MLcVtEngineCommandManager::RemoveObserver.
        */
        void RemoveObserver( MVtEngCommandObserver& aObserver );
        
        /**
        * @see MVtEngEventObserver::HandleVtEventL.
        */        
        void HandleVtEventL( TInt aEvent );
        
        /**
        * @see MVtEngCommandObserver::HandleVTCommandPerformedL.
        */        
        void HandleVTCommandPerformedL(
                TVtEngCommandId aCommand,
                const TInt aError );
#ifndef NGA_VIDEO_RENDER        
        /**
        * @see MVtEngFrameObserver::vtHandleFrameL.
        */        
        void vtHandleFrameL( TFrameType aType, CFbsBitmap* aBitmap );
        
        /**
        * @see MVtEngFrameObserver::vtSetFrame
        */        
        void vtSetFrame( TFrameType aType, CFbsBitmap* aBitmap );                 
#endif                                       
    private:
        
        /**
        * Constructor.
        * @param aSession plugin session.
        */
        CEventObserver(CLcVtSession& aSession );
        
        /**
        * Symbian OS constructor.
        */        
        void ConstructL();        
          
    private:        
        
        // Ref to plugin session.
        CLcVtSession& iSession;    
        
        // ETrue when command events are being sent.
        TBool iInCommandPerformed;

        // Owned array of observers.
        RPointerArray< MVtEngCommandObserver > iCommandObservers;
        
    };

         
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtSession* CLcVtSession::NewL()
    {
    CLcVtSession* self = new ( ELeave ) CLcVtSession();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }     

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtSession::CLcVtSession( ) 
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CLcVtSession::ConstructL()
    {
    
    __VTPRINTENTER( "CLcVtSession.ConstructL" )  
      
    __VTPRINT( DEBUG_MEDIA , "FeatureManager::InitializeLibL()" )
    FeatureManager::InitializeLibL(); 
        
    __VTPRINT( DEBUG_MEDIA , "iCameraHandler.SetSession(this)" )
    iCameraHandler.SetSession(this);
    __VTPRINT( DEBUG_MEDIA , "iRemoteVideoPlayer = CLcVtRemoteVideoPlayer_NewL" )
    iRemoteVideoPlayer = CLcVtRemoteVideoPlayer::NewL(this, *this);
    __VTPRINT( DEBUG_MEDIA , "iLocalVideoPlayer  = CLcVtLocalVideoPlayer_NewL" )
    iLocalVideoPlayer  = CLcVtLocalVideoPlayer::NewL(this, *this, iCameraHandler);  

#ifndef NGA_VIDEO_RENDER 
    iFbsStarted = EFalse;   
            
    __VTPRINT( DEBUG_MEDIA , "iWsSession = &(CCoeEnv::Static()->WsSession())" )
    iWsSession = &(CCoeEnv::Static()->WsSession());
    __VTPRINT( DEBUG_MEDIA , "iWsSessionScreenDev = CCoeEnv::Static()->ScreenDevice()" )
    iWsSessionScreenDev = CCoeEnv::Static()->ScreenDevice();
    __VTPRINT( DEBUG_MEDIA , "iRootWin = &(CCoeEnv::Static()->RootWin())" )
    iRootWin = &(CCoeEnv::Static()->RootWin());
    
    __VTPRINT( DEBUG_MEDIA , "ConnectFbsServerL()" )
    ConnectFbsServerL();    
#endif
    __VTPRINT( DEBUG_MEDIA , "iEventObserver = CEventObserver_NewL" )
    iEventObserver = CEventObserver::NewL( *this );

    iModel = CVtEngModel::NewL( *iEventObserver, *iEventObserver );

    __VTPRINT( DEBUG_MEDIA , "iLcVtStates_new" )
    iLcVtStates = new ( ELeave ) TLcVtStates( *iModel );
    __VTPRINT( DEBUG_MEDIA , "iLcVtStates->Update()" )
    iLcVtStates->Update();    
    
    __VTPRINT( DEBUG_MEDIA , "new_iActiveExec" )
    iActiveExec =
        new ( ELeave ) CLcVtActiveExec( CActive::EPriorityHigh );  
        
    __VTPRINT( DEBUG_MEDIA , "SetCallIdL( KVtUiDefaultCallId )" )
    SetCallIdL( KVtUiDefaultCallId );
    
    __VTPRINT( DEBUG_MEDIA , "iAsyncCallback" )
    iAsyncCallback =
        new ( ELeave ) CAsyncCallBack (
            TCallBack( &DoExit, this ),
            CActive::EPriorityStandard );   
#if 0    
    __VTPRINT( DEBUG_MEDIA , "iLayoutChangeCallback" )
    iLayoutChangeCallback =
        new ( ELeave ) CAsyncCallBack(
            TCallBack( &DelayedHandleLayoutChanged, this ),
            CActive::EPriorityStandard );   
#endif    
    CRepository* repository = CRepository::NewL( KCRUidTelephonySettings );

    if ( repository->Get( KSettingsVTVideoSending, VideoSendingKey ) == KErrNone )
        {
        __VTPRINT2( DEBUG_GEN, "CLcVtSession.IsFeatureSupported get key=%d", VideoSendingKey )
        }
    else
        {
        __VTPRINT( DEBUG_GEN, "CLcVtSession.IsFeatureSupported set key to default KAllwaysAllow" )
        VideoSendingKey = KAllwaysAllow;
        }
    delete repository;
    
    iIndicatorCtr = new  ( ELeave ) LcVtIndicatorController();
    
    iForeGroundStatus = EFalse;
    
    __VTPRINTEXIT( "CLcVtSession.ConstructL" )
    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtSession::~CLcVtSession()
    {
    __VTPRINTENTER( "CLcVtSession.~CLcVtSession" )  

    //delete iLayoutChangeCallback;
    delete iAsyncCallback;

#ifndef NGA_VIDEO_RENDER    
    DisconnectFbsServer();
#endif
    delete iActiveExec;
    delete iLcVtStates;

    delete iModel;
    
    delete iEventObserver;
    iEventObserver = NULL;
    
#ifdef NGA_VIDEO_RENDER
    DestroyVideoWindows();
#endif

    delete iLocalVideoPlayer;
    delete iRemoteVideoPlayer;
    
    delete iRemoteDisplayName;
    delete iRemoteDetails;
    if (iIndicatorCtr) {
        iIndicatorCtr->disableActiveCallIndicator();
        delete iIndicatorCtr;
    }    
    __VTPRINTEXIT( "CLcVtSession.~CLcVtSession" )
    FeatureManager::UnInitializeLib();  
    }

#ifndef NGA_VIDEO_RENDER
void CLcVtSession::ConnectFbsServerL()
    {
    // VT use bitmaps viewfinder now, So we start FBS server for proto demo  
    // and later we will use Qt image object to implement bitmaps view finder or we will use direct view finder    
    TInt err( KErrNone );
    // Start Font&Bitmap server    
    err = FbsStartup();
    if ( err == KErrNone )
        {
       err = RFbsSession::Connect();
       if( err == KErrNone )
           {
           iFbsStarted = ETrue;
           }            
        }
    
    iDummy = new ( ELeave ) CFbsBitmap();  
    //CleanupStack::PushL( iDummy );
    User::LeaveIfError( iDummy->Create( TSize( 4096, 1 ), EColor64K ) );      
    
    iGc = new (ELeave) CWindowGc( iWsSessionScreenDev );
    //CleanupStack::PushL( iGc );
    User::LeaveIfError( iGc->Construct());
    
    //CleanupStack::Pop(2);
    }


void CLcVtSession::DisconnectFbsServer()
    {
    delete iGc;
    delete iDummy;    
    if ( iFbsStarted )
        {
        RFbsSession::Disconnect();
        iFbsStarted = EFalse;        
        }    
    }
#endif
#ifdef NGA_VIDEO_RENDER

_LIT(KVTWindowGroupName,"32VTPluginVideoWindow");

void CLcVtSession::CreateVideoWindowsL()
    {
    __VTPRINTENTER( "CLcVtSession.CreateVideoWindowsL" ) 
    User::LeaveIfError( iRwSession.Connect() );  

    iRwGroup = new (ELeave) RWindowGroup( iRwSession ) ;        
    iRwGroup->Construct( (TUint32)iRwGroup, EFalse );
    
    iRwGroup->SetName( KVTWindowGroupName );
    iRwGroup->SetOrdinalPosition( 1 , ECoeWinPriorityNeverAtFront );
    iRwGroup->AutoForeground(EFalse);
    
    iDev = new (ELeave) CWsScreenDevice( iRwSession );
    iDev->Construct( ); 
    
    iRemoteVideoWindow = new (ELeave) RWindow( iRwSession );
    iRemoteVideoWindow->Construct( *iRwGroup, (TUint32)iRemoteVideoWindow );
    
    TRect RMRect = RemoteVideoPlayer()->LcWindow()->LcWindowRect();
    iRemoteVideoWindow->SetExtent( RMRect.iTl, RMRect.Size() );
    iRemoteVideoWindow->SetOrdinalPosition( 1, ECoeWinPriorityNeverAtFront );
    iRemoteVideoWindow->Activate();
    
    iLocalVideoWindow = new (ELeave) RWindow( iRwSession );
    iLocalVideoWindow->Construct( *iRwGroup, (TUint32)iLocalVideoWindow );
    
    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();
    iLocalVideoWindow->SetExtent( VFRect.iTl, VFRect.Size() );
    iLocalVideoWindow->SetOrdinalPosition( 1, ECoeWinPriorityNeverAtFront );
    iLocalVideoWindow->Activate();   
        
    iRwSession.Flush();  

    __VTPRINTEXIT( "CLcVtSession.CreateVideoWindowsL" )
    }

void CLcVtSession::DestroyVideoWindows()
    {
    
    delete iDev;
    iDev = NULL;
    delete iRemoteVideoWindow;
    iRemoteVideoWindow = NULL;
    delete iLocalVideoWindow;
    iLocalVideoWindow = NULL;
    delete iRwGroup;
    iRwGroup = NULL;
    iRwSession.Close();
    }

#endif


void CLcVtSession::SetLcSessionObserver( MLcSessionObserver* aObserver )
    {
    iObserver = aObserver;
    }

void CLcVtSession::SetLcUiProvider( MLcUiProvider* aUiProvider )
    {
    iUiProvider = aUiProvider;
    }
    
TBool CLcVtSession::IsBackgroundStartup()
    {
    return ETrue;
    }
  
TBool CLcVtSession::SendDialTone( TChar aKey )
    {
    iDtmfTone = aKey;
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.SendDialTone: %d", iDtmfTone() )
    if ( Execute( KVtEngStartDtmfTone, &iDtmfTone ) != KErrNone )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

void CLcVtSession::EstablishLcSessionL()
    {
    __VTPRINTENTER( "CLcVtSession.EstablishLcSessionL" )
    
    TLcVtStateBase::SetInitialStateL( *this, *iLcVtStates );
    
    __VTPRINTEXIT( "CLcVtSession.EstablishLcSessionL" )
    }

void CLcVtSession::TerminateLcSessionL()
    {
    __VTPRINTENTER( "CLcVtSession.TerminateLcSessionL" )    
    HandleCommandL( EPluginCmdEndActiveCall );
    __VTPRINTEXIT( "CLcVtSession.TerminateLcSessionL" )
    }

MLcVideoPlayer* CLcVtSession::RemoteVideoPlayer()
    {
    return iRemoteVideoPlayer;
    }

MLcVideoPlayer* CLcVtSession::LocalVideoPlayer()
    {
    return iLocalVideoPlayer;
    }

MLcSession::TLcSessionState CLcVtSession::LcSessionState() const
    {
    return iSessionState;
    }

const TDesC& CLcVtSession::LocalDisplayName()
    {
    return KNullDesC;
    }
const TDesC& CLcVtSession::RemoteDisplayName()
    {
    if ( iRemoteDisplayName )
        {
        return *iRemoteDisplayName;
        }
    return KNullDesC;
    }
    
TInt CLcVtSession::SetParameter( TInt aId, TInt aValue )
    {
     if (aId == 0)
         iMainWindow = reinterpret_cast<RWindow*>(aValue);
         
    return 0;
    }

TInt CLcVtSession::ParameterValue( TInt aId )
    {
    return 0;
    }

const TDesC& CLcVtSession::RemoteDetails()
    {
    if ( iRemoteDetails )
        {
        return *iRemoteDetails;
        }
    return KNullDesC;
    }

void CLcVtSession::UpdateLcSessionL()
    {     
    __VTPRINTENTER( "CLcVtSession.UpdateLcSessionL" )
    
    if(iLcVtStates->ExecState() != TLcVtStates::EExecStateRunning )
        {
        __VTPRINTEXIT( "CLcVtSession.UpdateLcSessionL -- State Not OK" )
        return;
        }
#if 0    
    if(iLocalVideoPlayer->IsOrientationChanged())
        HandleLayoutChanged();
    else
        UpdateRenderingParametersL();
#endif
    UpdateRenderingParametersL();
    __VTPRINTEXIT( "CLcVtSession.UpdateLcSessionL" )
    }
    
// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::IsLcAudioMutedL()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
void CLcVtSession::MuteLcAudioL( TBool aMute )
    {
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::IsLcMicMutedL()
    {    
    TBool result = !(iLcVtStates->AudioState().IsAudio());
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.IsLcMicMutedL = %d", result )
    return result;  
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
void CLcVtSession::MuteLcMicL( TBool aMute )
    {   
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.MuteLcMicL aMute =  %d", aMute )

    if(aMute)
        {      
        HandleCommandL(EPluginCmdDisableAudio);
        }
    else
        {
        HandleCommandL(EPluginCmdEnableAudio);     
        }
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::IsEnablingLcLoudspeakerAllowed()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
void CLcVtSession::EnableLcLoudspeakerL( TBool aEnabled )
    {
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.EnableLcLoudspeakerL aEnabled =  %d", aEnabled )    
    
    if(aEnabled)
        {
        HandleCommandL(EPluginCmdActivateLoudspeaker);
        }
    else
        {
        HandleCommandL(EPluginCmdDeactivateLoudspeaker);
        }
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::IsLcLoudspeakerEnabled()
    {
    TBool result = !(iLcVtStates->AudioState().CanActivateLoudspeaker());
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.IsLcLoudspeakerEnabled = %d", result )
    return result;    
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
TInt CLcVtSession::LcVolumeL()
    {
    MVtEngAudio& audio = iModel->Audio();
    MVtEngAudio::TAudioRoutingState audioRouting;
    User::LeaveIfError( audio.GetRoutingState( audioRouting ) );
    const TInt volume( audio.OutputVolume(
        !( audioRouting == MVtEngAudio::EAudioLoudspeaker ) ) );
    
    return volume;
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
void CLcVtSession::SetLcVolumeL( TInt aValue )
    {   
    __VTPRINTENTER( "CLcVtSession.SetLcVolumeL" )
    MVtEngAudio& audio = iModel->Audio();
    const TInt HandsetVolume( audio.OutputVolume( ETrue ) );    
    const TInt HandsfreeVolume( audio.OutputVolume( EFalse ) );    
    
    MVtEngAudio::TVtEngOutputVolume volume;
    volume.iHandsetVolume = HandsetVolume;
    volume.iHandsfreeVolume = HandsfreeVolume;

    MVtEngAudio::TAudioRoutingState audioRouting;
    User::LeaveIfError( audio.GetRoutingState( audioRouting ) );    
    if ( audioRouting == MVtEngAudio::EAudioHandset )
        {
        volume.iHandsetVolume = aValue;
        }
    else
        {
        volume.iHandsfreeVolume = aValue;    
        }

    ExecuteCmdL( KVtEngSetAudioVolume, volume );
    __VTPRINTEXIT( "CLcVtSession.SetLcVolumeL" )
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
void CLcVtSession::IncreaseLcVolumeL()
    {
    ExecuteCmdL( KVtEngIncreaseAudioVolume );
    }

// -----------------------------------------------------------------------------
// From MLcAudioControl
// -----------------------------------------------------------------------------
//
void CLcVtSession::DecreaseLcVolumeL()
    {
    ExecuteCmdL( KVtEngDecreaseAudioVolume );
    }

CVtEngModel& CLcVtSession::Model()
    {
    return *iModel;
    }


void CLcVtSession::HandleCommandL(const TInt aCommand)
    {
    
    __VTPRINTENTER( "CLcVtSession.HandleCommandL" )
    
    if ( iLcVtStates->ExecState() != TLcVtStates::EExecStateRunning )
        {
        __VTPRINTEXIT( "CLcVtSession.HandleCommandL NOT ready" )
        return;
        }
    
    if (  iState->HandleCommandL( aCommand ) == TLcVtStateBase::EEventHandled )
        {
        __VTPRINTEXIT( "CLcVtSession.HandleCommandL <silently ignoring> 0" )
        return;
        }   
        
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.HandleCommandL command =  %d", aCommand )
    
    switch ( aCommand )
        {
        case EPluginCmdEnableAudio:
            CmdEnableAudioL();
            break;
            
        case EPluginCmdDisableAudio:
            CmdDisableAudioL();
            break;
            
        case EPluginCmdUsePrimaryCamera:
            CmdUseCameraL( ETrue );
            break;
            
        case EPluginCmdUseSecondaryCamera:
            CmdUseCameraL( EFalse );
            break;
            
        case EPluginCmdEnableVideo:
            CmdEnableVideoL();
            break;
            
        case EPluginCmdDisableVideo:
            CmdDisableVideoL();
            break;
            
        case EPluginCmdActivateLoudspeaker:
            CmdActivateLoudspeakerL();
            break;
            
        case EPluginCmdDeactivateLoudspeaker:
            CmdDeactivateLoudspeakerL();
            break;
            
        }
    
    __VTPRINTEXIT( "CLcVtSession.HandleCommandL" )
    
    iLcVtStates->Update();
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CmdEnableAudioL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdEnableAudioL()
    {
    ExecuteCmdL( KVtEngUnmuteOutgoingAudio );
    iLcVtStates->Update();
    }


// -----------------------------------------------------------------------------
// CLcVtSession::CmdDisableAudioL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdDisableAudioL()
    {
    ExecuteCmdL( KVtEngMuteOutgoingAudio );
    iLcVtStates->Update();
    }


// -----------------------------------------------------------------------------
// CLcVtSession::CmdUseCameraL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdUseCameraL( const TBool aPrimaryCamera )
    {

    MVtEngMedia& media = iModel->Media();

    MVtEngMedia::TMediaSource selectedSource =
        ( aPrimaryCamera ) ?
            MVtEngMedia::EMediaCameraPri : MVtEngMedia::EMediaCameraSec;

    TInt available = 0;
    LcVtUtility::GetOutgoingMediaState( media, available );
    if ( available & MVtEngMedia::EMediaVideo )
        {
        ExecuteCmdL( KVtEngStopViewFinder );
        
        TRAPD( err, ExecuteCmdL( KVtEngSetSource, selectedSource ) );

        if ( err == KErrNone )
            {
            ExecuteCmdL( KVtEngStartViewFinder );
            }
        else
            {
            //ShowOtherCameraNotUsableNoteL();
            }
        }
    else
        {
        /*
        MVtEngMedia::TPrepareCameraParams params;
        params.iMediaSource = selectedSource;
        params.iInitialize = EFalse;
        TRAPD( err, ExecuteCmdL( KVtEngPrepareCamera, params ) );
        if ( err != KErrNone )
            {
            ShowOtherCameraNotUsableNoteL();
            }
            */
        }
    
    }


// -----------------------------------------------------------------------------
// CLcVtSession::CmdEnableVideoL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdEnableVideoL()
    {
    // if outgoing video is frozen
    MVtEngMedia& media = iModel->Media();
    if ( LcVtUtility::GetFreezeState( media ) )
        {
        ExecuteCmdL( KVtEngUnfreeze );
        // swap images if needed
        //RestoreViewFinderL();
        }
    else
        {
        ExecuteCmdL( KVtEngStopViewFinder );
        //iUplinkWindow->SetStreamBitmap( NULL );
        MVtEngMedia::TMediaSource source = MVtEngMedia::EMediaCamera;
        ExecuteCmdL( KVtEngSetSource, source );
        MVtEngMedia::TCameraId id;
        if ( iModel->Media().GetCurrentCameraId( id ) == KErrInUse )
            {
            //ShowCameraInUseNoteL();
            }
        ExecuteCmdL( KVtEngStartViewFinder );
        }
    // update VB settings
    //UpdateVBSettingL();
    iLcVtStates->Update();
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CmdDisableVideoL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdDisableVideoL()
    {
    ExecuteCmdL( KVtEngStopViewFinder );

    //iUplinkWindow->SetStreamBitmap( NULL );

    TInt err = KErrNotFound;
    if ( LcVtUtility::HasStillImage( iModel->Media() ) )
        {
        __VTPRINT( DEBUG_GEN, "CLcVtSession.CmdDisableVideoL.HasStill" )
        MVtEngMedia::TMediaSource source = MVtEngMedia::EMediaStillImage;
        TRAP( err, ExecuteCmdL( KVtEngSetSource, source ) );
        }
    if ( ( err != KErrNone ) )
        {
        __VTPRINT( DEBUG_GEN, "CLcVtSession.CmdDisableVideoL.MediaNone" )
        MVtEngMedia::TMediaSource source = MVtEngMedia::EMediaNone;
        ExecuteCmdL( KVtEngSetSource, source );
        }

    ExecuteCmdL( KVtEngStartViewFinder );
    iLcVtStates->Update();
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CmdActivateLoudspeakerL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdActivateLoudspeakerL()
    {
    MVtEngAudio::TVtEngRoutingSetting audioSetting =
        MVtEngAudio::EActivateHandsfree;
    ExecuteCmdL( KVtEngSetAudioRouting, audioSetting );
    iLcVtStates->Update();
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CmdDeactivateLoudspeakerL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CmdDeactivateLoudspeakerL()
    {
    MVtEngAudio::TVtEngRoutingSetting audioSetting =
        MVtEngAudio::EDeactivateHansfree;
    ExecuteCmdL( KVtEngSetAudioRouting, audioSetting );
    iLcVtStates->Update();
    }


// -----------------------------------------------------------------------------
// CLcVtSession::SetZoomFactorL
// -----------------------------------------------------------------------------
//
void CLcVtSession::SetZoomFactorL( TInt aZoomStep )
    {
    __VTPRINTENTER( "CLcVtSessionCLcVtSession.SetZoomFactorL" )
    MVtEngMedia& media = iModel->Media();
    if ( LcVtUtility::IsZoomAllowed( media ) )
        {
        ExecuteCmdL( KVtEngSetZoomStep, aZoomStep );
        }
    __VTPRINTEXIT( "CLcVtSession.SetZoomFactorL" )
    }

#ifndef NGA_VIDEO_RENDER
//local video display, VT use bitmaps vf now, and maybe replace by direct vf later
void CLcVtSession::vtHandleFrameL( CFbsBitmap* aBitmap )
    {
    __VTPRINTENTER( "CLcVtSession.vtHandleFrameL" )   
    
    /*    
    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();
    const TPoint KVFcordinate( VFRect.iTl.iX,  VFRect.iTl.iY );    
    iMainWindow->Invalidate();
    iMainWindow->BeginRedraw();
    iGc->Activate( *iMainWindow );
    aBitmap->BeginDataAccess();
    aBitmap->EndDataAccess();
    iGc->BitBlt( KVFcordinate , aBitmap );
    iGc->Deactivate();
    iMainWindow->EndRedraw();
    */
    
    /*
    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();    
    const TPoint KVFcordinate( VFRect.iTl.iX -2 ,  VFRect.iTl.iY - 168 );
    iLocalVideoWindow->Invalidate();
    iLocalVideoWindow->BeginRedraw();
    iGc->Activate( *iLocalVideoWindow );
    iGc->BitBlt( KVFcordinate , aBitmap );
    iGc->Deactivate();
    iLocalVideoWindow->EndRedraw();
    */   
        
    __VTPRINTEXIT("CLcVtSession.vtHandleFrameL")     
    }
#endif

TInt CLcVtSession::SetForegroundStatus( TBool aIsForeground )
{
    __VTPRINTENTER( "CLcVtSession.SetForegroundStatus" )
    __VTPRINT2( DEBUG_MEDIA , "    foreground: %d", aIsForeground )
    
    iForeGroundStatus = aIsForeground;

    if ( iRwGroup )
        {
        TInt priority = iForeGroundStatus ? ECoeWinPriorityNormal : ECoeWinPriorityNeverAtFront;
        iRwGroup->SetOrdinalPosition( 1 , priority );
        iRemoteVideoWindow->SetOrdinalPosition( 1 , priority );
        iLocalVideoWindow->SetOrdinalPosition( 1 , priority );
        iRwSession.Flush();
        }
    if ( aIsForeground )
       {
       iIndicatorCtr->disableActiveCallIndicator();
       }
    else {
       iIndicatorCtr->enableActiveCallIndicator();
       }

    __VTPRINTEXIT( "CLcVtSession.SetForegroundStatus" )
    
    return KErrNone;   
}

// -----------------------------------------------------------------------------
// CLcVtSession::ShutdownL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ShutdownL()
    {
    __VTPRINTENTER( "CLcVtSession.ShutdownL" )
    iActiveExec->Start( EVtSessionShutdown, *this );
    __VTPRINTEXIT( "CLcVtSession.ShutdownL" )
    }

// -----------------------------------------------------------------------------
// CLcVtSession::StartupPhase1L
// -----------------------------------------------------------------------------
//
void CLcVtSession::StartupPhase1L()
    {
    __VTPRINTENTER( "CLcVtSession.StartupPhase1L" )
    iActiveExec->Start( EVtSessionStartup, *this );
    __VTPRINTEXIT( "CLcVtSession.StartupPhase1L" )
    }


// -----------------------------------------------------------------------------
// CLcVtSession::StartupPhase2L
// -----------------------------------------------------------------------------
//
void CLcVtSession::StartupPhase2L()
    {
    __VTPRINTENTER( "CLcVtSession.StartupPhase2L" )
    iActiveExec->Start( EVtSessionAnswered, *this );
    __VTPRINTEXIT( "CLcVtSession.StartupPhase2L" )
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecExecuteL
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::ActiveExecExecuteL(
        CLcVtActiveExec& /*aActiveExec*/,
        const TInt aState,
        TInt& aNextState,
        TRequestStatus& aRequest )
    {
    return ActiveExecInitExecuteL(
            aState,
            aNextState,
            aRequest );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecContinue
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::ActiveExecContinue(
        CLcVtActiveExec& /*aActiveExec*/,
        TInt& aState,
        const TInt aError )
    {
    return ActiveExecInitContinue( aState, aError );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecCancel
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecCancel(
        CLcVtActiveExec& /*aActiveExec*/,
        TInt aState )
    {
    ActiveExecInitCancel( aState );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecDone
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecDone(
        CLcVtActiveExec& /*aActiveExec*/,
        const TInt aInitialState )
    {    
    ActiveExecInitDone( aInitialState );
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitCancel
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitCancel(
        const TInt /*aState*/ )
    {
    __VTPRINTENTER( "CLcVtSession.InitExecCancel" )
    
    delete iCmdExecutor;

    if ( iShutdownRequest )
        {
        User::RequestComplete( iShutdownRequest, KErrCancel );
        iShutdownRequest = NULL;
        }
    __VTPRINTEXIT( "CLcVtSession.InitExecCancel" )
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitDone
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitDone(
        const TInt aInitialState )
    {
    __VTPRINTENTER( "CLcVtSession.ExecInitDone" )
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.InitExecDone.ini=%d", aInitialState )
    // Operation succeeded
    
    // Now we have finished.
    if ( iState )
        {
        if ( aInitialState == EVtSessionAnswered )
            {
            TRAP_IGNORE( iState->LcVtStartupPhase2DoneL() );
			iSessionState = EOpen;
            iObserver->StateChanged( *this );             
            }
        else if ( aInitialState == EVtSessionShutdown )
            {
            TRAP_IGNORE( iState->ShutdownDoneL() );
            }
        else // EVtSessionStartup
            {
            TRAP_IGNORE( iState->LcVtStartupPhase1DoneL() );
            }
        }
    __VTPRINTEXIT( "CLcVtSession.ExecInitDone" )
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitExecuteL
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::ActiveExecInitExecuteL(
        const TInt aState,
        TInt& aNextState,
        TRequestStatus& aRequest )
    {
    __VTPRINTENTER( "CLcVtSession.ActiveExecInitExecuteL" )
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.ActiveExecInitExecuteL.st=%d", aState )

    TBool synch = EFalse;

    switch ( aState )
        {
        // Startup chain:
        case EVtSessionStartupInitEngine:
            //ChangeApplicationFocus(ETrue);
#if defined (__WINS__)
            ActiveExecInitExecuteCommandL( KVtEngInitializeEngineDiag, aRequest );
#else            
            ActiveExecInitExecuteCommandL( KVtEngInitializeEngine, aRequest );
#endif            
            aNextState = EVtSessionStartupPrepareViewfinder; 
            //aNextState = EVtSessionStartupFinish;            
            break;
            
        case EVtSessionStartupPrepareViewfinder:
            //iSessionState = EOpen;
            //iObserver->StateChanged( *this );          
                        
#ifdef NGA_VIDEO_RENDER
            CreateVideoWindowsL();
#endif           
            ActiveExecInitPrepareViewFinderL( aRequest );            
            aNextState = EVtSessionStartupPrepareRemoteRender;
            break;

        case EVtSessionStartupPrepareRemoteRender:
            ActiveExecInitPrepareRemoteRenderL( aRequest );
            aNextState = EVtSessionStartupFinish;
            break;
            
        case EVtSessionStartupFinish:
            aNextState = EVtSessionNone;
            synch = ETrue;
            break;
            
            
        // Shutdown chain:    
        case EVtSessionShutdownHideApplication:
            ChangeApplicationFocus( EFalse );
            synch = ETrue;
            aNextState = EVtSessionShutdownResetEngine;            
            break;
        case EVtSessionShutdownResetEngine:
            {
            aRequest = KRequestPending;

            MVtEngCommandHandler& command = iModel->CommandHandler();
            // There should not be any commands ongoing because
            // we have canceled all dialogs (and this should
            // be the only command without dialog).
            command.CancelCommand( command.PendingCommand() ); // ignore error
            command.ExecuteL( KVtEngResetEngine, NULL );
            iShutdownRequest = &aRequest;

            aNextState = EVtSessionNone;
            }            
            break;
        
        
            
        // Answered chain:  
        case EVtSessionAnsweredStartRemoteRender:            
            {
            ActiveExecInitExecuteCommandL( KVtEngStartRenderRemote, aRequest );
            //TRequestStatus* status = &aRequest;
            //User::RequestComplete( status, KErrNone );            

            MVtEngSessionInfo::TDirection direction;
            if ( iModel->Session().GetDirection( direction ) != KErrNone )
                {
                direction = MVtEngSessionInfo::EDirectionMT;
                }
            
            MVtEngMedia& media = iModel->Media();    
            if ( LcVtUtility::HasCameras( media ) )
                {
                if ( direction == MVtEngSessionInfo::EDirectionMT )
                    {
                        if ( VideoSendingKey == KAllwaysAllow)
                            {
                            aNextState = EVtSessionAnsweredDoPrepareCamera;
                            }
                        else
                            {
                            aNextState = EVtSessionAnsweredSetlectNone;
                            }
                    }
                else
                    {
                    //MVtEngSessionInfo::EDirectionMO
                    aNextState = EVtSessionAnsweredDoPrepareCamera;
                    }
                }
            else
                {
                aNextState = EVtSessionAnsweredSetlectNone;
                }
            }
            break;
            
        case EVtSessionAnsweredSetlectNone:
            {
            ActiveExecInitSetSourceL( MVtEngMedia::EMediaNone, aRequest );
            aNextState = EVtSessionAnsweredStartViewFinder;            
            }
            break;
        case EVtSessionAnsweredDoPrepareCamera:
            {
            MVtEngMedia::TMediaSource source = MVtEngMedia::EMediaCamera;
            ActiveExecInitPrepareCameraL( source, aRequest );
            aNextState = EVtSessionAnsweredSetlectCamera;
            }
            break;
            
        case EVtSessionAnsweredSetlectCamera:
            ActiveExecInitSetSourceL( MVtEngMedia::EMediaCamera, aRequest );
            aNextState = EVtSessionAnsweredStartViewFinder;
            break;
            
        case EVtSessionAnsweredStartViewFinder:
            ActiveExecInitExecuteCommandL( KVtEngStartViewFinder, aRequest );
            aNextState = EVtSessionAnsweredBringToForeground;            
            break;
            
        case EVtSessionAnsweredBringToForeground:
            {
            //CreateVideoWindowsL();
            
            ChangeApplicationFocus(ETrue);
            TRequestStatus* status = &aRequest;
            User::RequestComplete( status, KErrNone );            
            aNextState = EVtSessionAnsweredFinish;
            }
            break;
            
        case EVtSessionAnsweredFinish:
            {          
            //iRwGroup->SetOrdinalPosition( 1 , ECoeWinPriorityNormal );
            
            TBool handleAsForeground( ETrue );
            TPtrC8 params( reinterpret_cast< TUint8* >( &handleAsForeground ), sizeof( TBool ) );
            MVtEngCommandHandler& command = iModel->CommandHandler();            
            TRAPD( err, command.ExecuteL( KVtEngSetUIForeground, &params ));
            if(err != KErrNone)
                {
                err = KErrNone;
                }
            
            iLcVtStates->Update();
            
            iObserver->StateChanged( *iLocalVideoPlayer );
            iObserver->StateChanged( *iRemoteVideoPlayer );            
            
            synch = ETrue;
            aNextState = EVtSessionNone;
            }
            break;
        }
    
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.ActiveExecInitExecuteL.sync=%d", synch )
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.ActiveExecInitExecuteL.next=%d", aNextState )
    __VTPRINTEXITR( "CLcVtSession.ActiveExecInitExecuteL %d", synch )
    return synch;
    
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitContinue
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::ActiveExecInitContinue(
        TInt& aState,
        const TInt /*aError*/ )
    {
    __VTPRINTENTER( "CLcVtSession.InitExecCont" )
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.InitExecCont.st=%d", aState ) 

    const TBool cont = EFalse;
    __VTPRINT2( DEBUG_GEN, "VtUi.InitExecCont.cont=%d", cont )

    if ( !cont )
        {
        // Operation failed.
        if ( iState )
            {
            TRAP_IGNORE( iState->LcVtStartupFailedL() );
            }
        }
    __VTPRINTEXITR( "VtUi.InitExecCont %d", cont )
    return cont;
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitExecuteCommandL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitExecuteCommandL(
        const TVtEngCommandId aCommand,
        TRequestStatus& aRequest )
    {
    iCmdExecutor =
        ExecuteCmdAsyncL(
        &iCmdExecutor,
        aCommand,
        aRequest );
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitPrepareViewFinderL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitPrepareViewFinderL(
        TRequestStatus& aRequest )
    {
    
    __VTPRINTENTER( "CLcVtSession.ActiveExecInitPrepareViewFinderL" )
        
    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();    
    TSize size (VFRect.Size());
    
#ifdef NGA_VIDEO_RENDER  
    
    __VTPRINT3(DEBUG_GEN,  "CLcVtSession.ActiveExecInitPrepareViewFinderL size(%d,%d)", size.iWidth, size.iHeight )    
    
    iLocalVideoWindow->SetPosition( VFRect.iTl );    
    iLocalVideoWindow->SetSize( VFRect.Size() ); 
    
    iRwSession.Flush(); 
        
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iTl.iX: %d", VFRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iTl.iY: %d", VFRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iWidth: %d", VFRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iHeight: %d", VFRect.Height() )  
                   
    TVtEngRenderingOptionsNGA configViewfinder( *iLocalVideoWindow, iRwSession );
    iCmdExecutor =
          ExecuteCmdAsyncL(
              &iCmdExecutor,
              KVtEngPrepareViewFinder,
              configViewfinder,
              aRequest );   
    
    
    /*
    TPoint point( 0, 0 );
    TRect vfRect( point, size ); 
     
    TVtEngRenderingOptionsDSA configViewfinder(*iEventObserver, iRwSession ,*iDev, *iLocalVideoWindow, vfRect, vfRect);    

    iCmdExecutor =
          ExecuteCmdAsyncL(
              &iCmdExecutor,
              KVtEngPrepareViewFinderDSA,
              configViewfinder,
              aRequest );
              */                  
    
#else
    
    __VTPRINT3(DEBUG_GEN,  "CLcVtSession.ActiveExecInitPrepareViewFinderL size(%d,%d)", size.iWidth, size.iHeight )    
    
    TVtEngRenderingOptions configViewfinder(*iEventObserver, size);

    iCmdExecutor =
          ExecuteCmdAsyncL(
              &iCmdExecutor,
              KVtEngPrepareViewFinder,
              configViewfinder,
              aRequest );   
    
#endif    
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitPrepareRemoteRenderL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitPrepareRemoteRenderL(
        TRequestStatus& aRequest )
    {
    
    TRect RMRect = RemoteVideoPlayer()->LcWindow()->LcWindowRect();    
    const TSize KRMsize( RMRect.Size());
    TRect screen( KRMsize );
    TRect clip( KRMsize );        
    
#ifdef NGA_VIDEO_RENDER 
        
    iRemoteVideoWindow->SetPosition( RMRect.iTl );
    iRemoteVideoWindow->SetSize( RMRect.Size() );    
    
    iRwSession.Flush(); 
    
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iTl.iX: %d", RMRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iTl.iY: %d", RMRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iWidth: %d", RMRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iHeight: %d", RMRect.Height() )   
       
       
    TVtEngRenderingOptionsNGA configRemoteRender( *iRemoteVideoWindow, iRwSession );    
    iCmdExecutor =
        ExecuteCmdAsyncL(
            &iCmdExecutor,
            KVtEngPrepareRemoteRenderNGA,
            configRemoteRender,
            aRequest );
    
#else

    const TPoint KRMcordinate( RMRect.iTl.iX,  RMRect.iTl.iY );
    
    TVtEngRenderingOptionsDP configRemoteRender(*iEventObserver, *iWsSession ,*iWsSessionScreenDev, *iMainWindow, screen, clip, KRMcordinate );  

    iCmdExecutor =
        ExecuteCmdAsyncL(
            &iCmdExecutor,
            KVtEngPrepareRemoteRenderDP,
            configRemoteRender,
            aRequest );
    
#endif
    
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitPrepareCameraL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitPrepareCameraL(
        const MVtEngMedia::TMediaSource aSource,
        TRequestStatus& aRequest )
    {
    MVtEngMedia::TPrepareCameraParams params;
    params.iMediaSource = aSource;
    params.iInitialize = ETrue;
    iCmdExecutor =
        ExecuteCmdAsyncL(
            &iCmdExecutor,
            KVtEngPrepareCamera,
            params,
            aRequest );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ActiveExecInitSetSourceL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ActiveExecInitSetSourceL(
        const MVtEngMedia::TMediaSource aSource,
        TRequestStatus& aRequest )
    {
    MVtEngMedia::TMediaSource source =
        aSource;
    iCmdExecutor =
        ExecuteCmdAsyncL(
            &iCmdExecutor,
            KVtEngSetSource,
            source,
            aRequest );
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ExecuteCmdAsyncL
// -----------------------------------------------------------------------------
//
CLcVtCmdExecutor* CLcVtSession::ExecuteCmdAsyncL(
        CLcVtCmdExecutor** aDialogPtr,
        const TVtEngCommandId aCommand,
        TRequestStatus& aStatus )
    {
    return DoExecuteCmdAsyncL( aDialogPtr, aCommand, NULL, aStatus );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ExecuteCmdAsyncL
// -----------------------------------------------------------------------------
//
template< class T >
CLcVtCmdExecutor* CLcVtSession::ExecuteCmdAsyncL(
        CLcVtCmdExecutor** aDialogPtr,
        const TVtEngCommandId aCommand,
        T& aParam,
        TRequestStatus& aStatus )
    {
    TPtrC8 params( reinterpret_cast< TUint8* >( &aParam ), sizeof( T ) );
    return DoExecuteCmdAsyncL( aDialogPtr, aCommand, &params, aStatus );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::DoExecuteCmdAsyncL
// -----------------------------------------------------------------------------
//
CLcVtCmdExecutor* CLcVtSession::DoExecuteCmdAsyncL(
        CLcVtCmdExecutor** aDialogPtr,
        const TVtEngCommandId aCommand,
        TDesC8* aParams,
        TRequestStatus& aStatus )
    {
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.DoExecAsync.cmd=%d", aCommand )
    MVtEngCommandHandler& command = iModel->CommandHandler();
    const TInt caps = command.GetCommandCaps( aCommand );

    if ( caps >= KErrNone )
        {
        const TBool asynchronous =
            ( caps & MVtEngCommandHandler::EAttribAsync );

        if ( asynchronous  )
            {
            CLcVtCmdExecutor* executor =
                new ( ELeave ) CLcVtCmdExecutor(
                    aDialogPtr,
                    iModel->CommandHandler(),
                    *iEventObserver );
            executor->ExecuteCmdL( aCommand, aParams, &aStatus );
            return executor;
            }
        else
            {
            command.ExecuteL( aCommand, aParams );
            RefreshStatesL();
            TRequestStatus* status = &aStatus;
            User::RequestComplete( status, KErrNone );
            }
        }
    else
        {
        // Failed.
        User::Leave( caps );
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ExecuteCmdL
// -----------------------------------------------------------------------------
//
void CLcVtSession::ExecuteCmdL( const TVtEngCommandId aCommand )
    {
    DoExecuteCmdL( aCommand, NULL );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::ExecuteCmdL
// -----------------------------------------------------------------------------
//
template< class T >
void CLcVtSession::ExecuteCmdL(
        const TVtEngCommandId aCommand,
        T& aParam )
    {
    TPtrC8 params( reinterpret_cast< TUint8* >( &aParam ), sizeof( T ) );
    DoExecuteCmdL( aCommand, &params );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::DoExecuteCmdL
// -----------------------------------------------------------------------------
//
void CLcVtSession::DoExecuteCmdL(
        const TVtEngCommandId aCommand,
        TDesC8* aParams )
    {
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.DoExec.cmd=%d", aCommand )
    MVtEngCommandHandler& command = iModel->CommandHandler();
    const TInt caps = command.GetCommandCaps( aCommand );

    if ( caps >= KErrNone )
        {
        const TBool asynchronous =
            ( caps & MVtEngCommandHandler::EAttribAsync );

        if ( asynchronous  )
            {            
            CLcVtCmdExecutor* executor =
                new ( ELeave ) CLcVtCmdExecutor(
                    NULL,
                    iModel->CommandHandler(),
                    *iEventObserver );
            iUiProvider->BlockUi(ETrue);
            //executor->ExecuteCmdL( aCommand, aParams, NULL );
            TRAPD( err, executor->ExecuteCmdL( aCommand, aParams, NULL ) );
            iUiProvider->BlockUi(EFalse);
            if(err != KErrNone)
                User::Leave(err);
            
            }
        else
            {
            command.ExecuteL( aCommand, aParams );
            RefreshStatesL();
            }
        }
    else
        {
        // Failed.
        User::Leave( caps );
        }
    }


// -----------------------------------------------------------------------------
// CLcVtSession::Execute
// -----------------------------------------------------------------------------
//
TInt CLcVtSession::Execute( const TVtEngCommandId aCommand, TDesC8* aParam )
    {
    MVtEngCommandHandler& command = iModel->CommandHandler();
    TRAPD( err, command.ExecuteL( aCommand, aParam ) );
#ifdef VTDEBUG
    if ( err != KErrNone )
        {
        __VTPRINT3( DEBUG_GEN, "CLcVtSession.Execute.Nok.cmd=%d,err=%d", aCommand, err )
        }
#endif // VTDEBUG
    return err;
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ChangeApplicationFocus
// -----------------------------------------------------------------------------
//
void CLcVtSession::ChangeApplicationFocus( const TBool aForeground )
    {
    __VTPRINTENTER( "CLcVtSession.ChangeApplicationFocus" )
    if ( iUiProvider ){
        iUiProvider->HandleForegroundStatus(aForeground);
    }
    __VTPRINTEXIT( "CLcVtSession.ChangeApplicationFocus" )
    }


// -----------------------------------------------------------------------------
// CLcVtSession::ChangeState
// -----------------------------------------------------------------------------
//
void CLcVtSession::ChangeState( TLcVtStateBase* aState )
    {
    __VTPRINTENTER( "CLcVtSession.ChangeState" )
    iState = aState;
    __VTPRINTEXIT( "CLcVtSession.ChangeState" )
    }

#if 0
// -----------------------------------------------------------------------------
// CLcVtSession::StartDtmfTone
// -----------------------------------------------------------------------------
//
void CLcVtSession::StartDtmfTone( const TChar& aTone )
    {
    // should be called only through current application state
    iDtmfTone = aTone;
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.DTMF.Start.%d", iDtmfTone() )
    if ( Execute( KVtEngStartDtmfTone, &iDtmfTone ) != KErrNone )
        {
        iDtmfTone = 0;
        }
    }

// -----------------------------------------------------------------------------
// CLcVtSession::StopDtmfTone
// -----------------------------------------------------------------------------
//
void CLcVtSession::StopDtmfTone()
    {
    // should be called only through current application state
    if ( iDtmfTone() )
        {
        __VTPRINT( DEBUG_GEN, "CLcVtSession.DTMF.STOP" )
        (void) Execute( KVtEngStopDtmfTone, NULL );
        iDtmfTone = 0;
        }
    }
#endif
// -----------------------------------------------------------------------------
// CLcVtSession::SetCallIdL
// -----------------------------------------------------------------------------
//
void CLcVtSession::SetCallIdL( const TInt aCallId )
    {
    }

// -----------------------------------------------------------------------------
// CLcVtSession::SetCallNameL
// -----------------------------------------------------------------------------
//
TBool CLcVtSession::SetCallNameL( const TDesC& aName )
    {
    HBufC* name = aName.AllocL();
    delete iRemoteDisplayName;
    iRemoteDisplayName = name;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CLcVtSession::SetCallNameL
// -----------------------------------------------------------------------------
//
void CLcVtSession::SetCallNumberL( const TDesC& aNumber )
    {
    HBufC* number = aNumber.AllocL();
    delete iRemoteDetails;
    iRemoteDetails = number;
    }


// -----------------------------------------------------------------------------
// CLcVtSession::StartShutdown
// -----------------------------------------------------------------------------
//
void CLcVtSession::StartShutdown()
    {
    __VTPRINTENTER( "CLcVtSession.StartShutdown" )
    iSessionState = EClosing;
    if ( iObserver != NULL )
        {
        iObserver->StateChanged( *this ); 
        }
    //iUiProvider->BlockUi(ETrue);
    __VTPRINTEXIT( "CLcVtSession.StartShutdown" )
    }

// -----------------------------------------------------------------------------
// CLcVtSession::NumberSource
// -----------------------------------------------------------------------------
//
TDesC* CLcVtSession::NumberSource() const
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CLcVtSession::RefreshL
// -----------------------------------------------------------------------------
//
void CLcVtSession::RefreshL( const TInt aRefreshFlags )
    {
    }


// -----------------------------------------------------------------------------
// CLcVtSession::RefreshStatesL
// -----------------------------------------------------------------------------
//
void CLcVtSession::RefreshStatesL()
    {
    __VTPRINTENTER( "CLcVtSession.RefreshStatesL" )
    // If transaction (set of commands) is pending refresh in delayed until
    // all of them are processed.
    __VTPRINTEXIT( "CLcVtSession.RefreshStatesL" )
    }

#if 0
// -----------------------------------------------------------------------------
// CLcVtSession::HandleLayoutChanged
// -----------------------------------------------------------------------------
//
TInt CLcVtSession::HandleLayoutChanged()
    {
    __VTPRINTENTER( "CLcVtSession.HandleLayoutChanged" )
    iLayoutChangeCallback->CallBack();
    __VTPRINTEXIT( "CLcVtSession.HandleLayoutChanged" )
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLcVtSession::DelayedHandleLayoutChanged
// -----------------------------------------------------------------------------
//
TInt CLcVtSession::DelayedHandleLayoutChanged( TAny* aPtr )
    {
    __VTPRINTENTER( "CLcVtSession.DelayedHandleLayoutChanged" )
    CLcVtSession* self = reinterpret_cast< CLcVtSession* > ( aPtr );
    //self->iUiStates->SetDisableBlindSetting( ETrue );
    TRAPD( err, self->HandleLayoutChangedL() );
    //self->iUiStates->SetDisableBlindSetting( EFalse );
    //self->RefreshBlind();
    __VTPRINTENTER( "CLcVtSession.DelayedHandleLayoutChanged" )
    return err;
    }

// -----------------------------------------------------------------------------
// CLcVtSession::HandleLayoutChangedL
// -----------------------------------------------------------------------------
//
void CLcVtSession::HandleLayoutChangedL()
    {
    __VTPRINTENTER( "CLcVtSession.LayoutChg" )
    
    if ( iState )
    {
    iState->HandleLayoutChangedL();
    }
    
    /*
    if ( !iAllowVideoDlgShowed )
        {
        if ( iState )
            {
            iState->HandleLayoutChangedL();
            }
        }
    else
        {
        iUiStates->SetLayoutChangeNeeded( EFalse );
        DoHandleLayoutChangedL();
        }
        */
    __VTPRINTEXIT( "CLcVtSession.LayoutChg" )
    }

// -----------------------------------------------------------------------------
// CLcVtSession::DoHandleLayoutChangedL
// -----------------------------------------------------------------------------
//
void CLcVtSession::DoHandleLayoutChangedL()
    {
    __VTPRINTENTER( "CLcVtSession.DoLayoutChg" )
    
    // Fully update rendering parameters
    UpdateRenderingParametersL();
    // Notify engine about layout change
    //iLayoutChg = ETrue;
    TRAPD( error, ExecuteCmdL( KVtEngHandleLayoutChange ) );
    //iLayoutChg = EFalse;

    // Not ready error is allowed to happen (e.g. when sharing)
    if ( error && ( error != KErrNotReady ) )
        {
        User::Leave( error );
        }
    else
        {
        __VTPRINT( DEBUG_GEN,
            "CLcVtSession.DoLayoutChg KVtEngHandleLayoutChange == KErrNotReady (ok)" )
        }
        
    __VTPRINTEXIT( "CLcVtSession.DoLayoutChg" )
    }
#endif

// -----------------------------------------------------------------------------
// CLcVtSession::UpdateRenderingParametersL
// -----------------------------------------------------------------------------
//
#ifdef NGA_VIDEO_RENDER

void CLcVtSession::UpdateRenderingParametersL()
    {
    __VTPRINTENTER( "CLcVtSession.UpdateRenderingParametersL" )
            
    TInt priority = iForeGroundStatus ? ECoeWinPriorityNormal : ECoeWinPriorityNeverAtFront;
    
    iRwGroup->SetOrdinalPosition( 1 , priority );
    
    TRect RMRect = RemoteVideoPlayer()->LcWindow()->LcWindowRect();
    iRemoteVideoWindow->SetExtent( RMRect.iTl, RMRect.Size() ); 
    iRemoteVideoWindow->SetOrdinalPosition( 1 , priority );
    iRemoteVideoWindow->SetTransparencyAlphaChannel();
    
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iTl.iX: %d", RMRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iTl.iY: %d", RMRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iWidth: %d", RMRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iHeight: %d", RMRect.Height() )    
    
    
    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();
    iLocalVideoWindow->SetExtent( VFRect.iTl, VFRect.Size() );
    iLocalVideoWindow->SetOrdinalPosition( 1, priority );
    iLocalVideoWindow->SetTransparencyAlphaChannel();
    
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iTl.iX: %d", VFRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iTl.iY: %d", VFRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iWidth: %d", VFRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.VFRect.iHeight: %d", VFRect.Height() )    
    
    iRwSession.Flush();    

    __VTPRINTEXIT( "CLcVtSession.UpdateRenderingParametersL" )
    
    }


#else

void CLcVtSession::UpdateRenderingParametersL()
    {
    
    __VTPRINTENTER( "CLcVtSession.UpdateRenderingParametersL" )
    
    MVtEngMedia& media = iModel->Media();
    const TBool viewFinderStarted =
        media.RenderingStarted( MVtEngMedia::EMediaOutgoing );
    const TBool remoteRenderStarted =
        media.RenderingStarted( MVtEngMedia::EMediaIncoming );

    // Stop viewfinder & remote render.
    if( viewFinderStarted )
        {
        ExecuteCmdL( KVtEngStopViewFinder );
        }
    ExecuteCmdL( KVtEngStopRenderRemote );

    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();    
    TSize size (VFRect.Size());  
    
    TVtEngRenderingOptions configViewfinder(*iEventObserver,size);

    ExecuteCmdL( KVtEngPrepareViewFinder, configViewfinder );

    // Prepare remote render.
    TRect RMRect = RemoteVideoPlayer()->LcWindow()->LcWindowRect();    
    const TSize KRMsize( RMRect.Size());
    TRect screen( KRMsize );
    TRect clip( KRMsize );
    const TPoint KRMcordinate( RMRect.iTl.iX,  RMRect.iTl.iY );   
    
    TVtEngRenderingOptionsDP configRemoteRender(*iEventObserver, *iWsSession ,*iWsSessionScreenDev, *iMainWindow, screen, clip, KRMcordinate );  

    TRAPD ( err, ExecuteCmdL(
        KVtEngPrepareRemoteRenderDP,
        configRemoteRender ) );    
    
     if ( err != KErrNone )
        {
        if ( viewFinderStarted )
            {
            // Start viewfinder.
            ExecuteCmdL( KVtEngStartViewFinder );
            }
        User::Leave ( err );
        } 

    // Start remote render.
    TRAP_IGNORE( ExecuteCmdL( KVtEngStartRenderRemote ) );

    if ( viewFinderStarted )
        {
        // Start viewfinder.
        ExecuteCmdL( KVtEngStartViewFinder );
        }

    __VTPRINTEXIT( "CLcVtSession.UpdateRenderingParametersL" )
    
    }

#endif

// -----------------------------------------------------------------------------
// CLcVtSession::HandleEngineResetL
// -----------------------------------------------------------------------------
//
void CLcVtSession::HandleEngineResetL()
    {
    __VTPRINTENTER( "CLcVtSession.HandleEngineResetL" )
    if ( iShutdownRequest )
        {
        __VTPRINT( DEBUG_GEN, "  CLcVtSession.HandleEngineResetL shutdown" )
        User::RequestComplete( iShutdownRequest, KErrNone );
        iShutdownRequest = NULL;
        }
    __VTPRINTEXIT( "CLcVtSession.HandleEngineResetL" )
    }


// -----------------------------------------------------------------------------
// CLcVtSession::HandleShutdownReady
// -----------------------------------------------------------------------------
//
void CLcVtSession::HandleShutdownReady()
    {
    __VTPRINTENTER( "CLcVtSession.HandleShutdownReady" )

    if(iAsyncCallback->IsActive())
        {
        iAsyncCallback->Cancel();
        }

    iAsyncCallback->Set(
        TCallBack( &DoExit, this ) );
    iAsyncCallback->CallBack();
    __VTPRINTEXIT( "CLcVtSession.HandleShutdownReady" )
    }


// -----------------------------------------------------------------------------
// CLcVtSession::DoExit
// -----------------------------------------------------------------------------
//
TInt CLcVtSession::DoExit( TAny* aSession )
    {
    __VTPRINTENTER( "CLcVtSession.DoExit" )
    CLcVtSession* self = static_cast< CLcVtSession* >( aSession );
    self->iAsyncCallback->Cancel();
    // Before exiting ensure there are no pending actions.
    self->ActiveExecInitCancel( 0 );  
    self->CloseApp();    
    __VTPRINTEXIT( "CLcVtSession.DoExit" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CloseApp
// -----------------------------------------------------------------------------
//
void CLcVtSession::CloseApp()
    {
    __VTPRINTENTER( "CLcVtSession.CloseApp" )
    //iUiProvider->BlockUi(EFalse);
#if 0
    if(iEventObserver)
        {
         delete iEventObserver;
         iEventObserver = NULL;
        }
#endif
    iSessionState = EClosed;
    iObserver->StateChanged( *this ); 
    __VTPRINTEXIT( "CLcVtSession.CloseApp" )
    }

// Implementation of CLcVtSession::CEventObserver

// -----------------------------------------------------------------------------
// CLcVtSession::CEventObserver::CEventObserver
// -----------------------------------------------------------------------------
//
CLcVtSession::CEventObserver::CEventObserver( CLcVtSession& aSession )
    :iSession(aSession),
    iCommandObservers( KLcVtCommandObserverArrayGranularity )
    {        
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CEventObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CEventObserver::ConstructL()
    {                
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CEventObserver::NewL
// -----------------------------------------------------------------------------
//
CLcVtSession::CEventObserver* CLcVtSession::CEventObserver::NewL( CLcVtSession& aSession )
   {
   CEventObserver* self = new ( ELeave ) CEventObserver(aSession  );
   CleanupStack::PushL( self );
   self->ConstructL();
   CleanupStack::Pop( self );
   return self;
   }

// -----------------------------------------------------------------------------
// CLcVtSession::CEventObserver::~CEventObserver
// -----------------------------------------------------------------------------
//
CLcVtSession::CEventObserver::~CEventObserver()
   {
   iCommandObservers.Close();        
   }

// -----------------------------------------------------------------------------
// CLcVtSession::CEventObserver::AddObserverL
// -----------------------------------------------------------------------------
//
void CLcVtSession::CEventObserver::AddObserverL(
       MVtEngCommandObserver& aObserver )
    {
    User::LeaveIfError(
    iCommandObservers.Append( &aObserver ) );
    }

// -----------------------------------------------------------------------------
// CLcVtSession::CEventObserver::RemoveObserver
// -----------------------------------------------------------------------------
//
void CLcVtSession::CEventObserver::RemoveObserver(
        MVtEngCommandObserver& aObserver )
    {
    const TInt pos = iCommandObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        if ( !iInCommandPerformed )
            {
            iCommandObservers.Remove( pos );
            }
        else
            {
            iCommandObservers[ pos ] = NULL;
            }
        }
    }
    

void CLcVtSession::CEventObserver::HandleVtEventL( TInt aEvent )
    {
    __VTPRINTENTER( "CEventObserver.HandleVtEventL" )
    __VTPRINT2( DEBUG_GEN, "CEventObserver.HandleVtEventL event=%d ",aEvent );
    
    /** Allow application state to handle event first. It may also deny further
      * handling by indicating the event was handled */
    if ( iSession.iState &&
            iSession.iState->HandleVtEventL( aEvent )
            == TLcVtStateBase::EEventHandled )
        {
        __VTPRINTEXITR( "CEventObserver.HandleVtEventL %d skipped", aEvent )
        return;
        }

    if ( aEvent == KVtEngAudioOutputVolumeChanged )
        {
        __VTPRINTEXITR( "CEventObserver.HandleVtEventL %d skipped", aEvent )
        return;
        }
    iSession.iLcVtStates->Update();
    
    iSession.iObserver->Updated(*(iSession.iLocalVideoPlayer));
    iSession.iObserver->Updated(*(iSession.iRemoteVideoPlayer));    
    
    __VTPRINTEXIT( "CEventObserver.HandleVtEventL" )
    }
        
void CLcVtSession::CEventObserver::HandleVTCommandPerformedL(
        TVtEngCommandId aCommand,
        const TInt aError )
    {         
     __VTPRINTENTER( "CEventObserver.HandleVTCommandPerformedL" )
     __VTPRINT3( DEBUG_GEN, "CEventObserver.HandleVTCommandPerformedL cmd=%d , result=%d",aCommand, aError );   
     
     if ( iSession.iState &&
             iSession.iState->HandleVTCommandPerformedL( aCommand, aError ) ==
          TLcVtStateBase::EEventHandled )
         {
         // state didn't allow further processing of command completion
         __VTPRINTEXITR( "CEventObserver.HandleVTCommandPerformedL %d", 0 )
         return;
         }
     
     iSession.RefreshStatesL();

     iSession.iLcVtStates->Update();
     
     const TInt count = iCommandObservers.Count();
     TInt nullPosition = KErrNotFound;

     iInCommandPerformed = ETrue;

     // iInCommandPerformed equals ETrue while inside the following loop;
     // Exceptions must not be raised.
     for ( TInt index = 0; index < count; index++ )
         {
         MVtEngCommandObserver* obs = iCommandObservers[ index ];
         if ( obs )
             {
             TRAP_IGNORE( obs->HandleVTCommandPerformedL( aCommand, aError ) );
             }
         else
             {
             // We store only one position; eventually all NULL elements of the
             // array will be removed.
             nullPosition = index;
             }
         }
     iInCommandPerformed = EFalse;

     if ( nullPosition != KErrNotFound )
         {
         iCommandObservers.Remove( nullPosition );
         }

     
     // Specific handling of some commands:
     if ( aCommand == KVtEngResetEngine ) // Engine has been reset
         {
         //__ASSERT_ALWAYS( !aError, VtUiPanic::Panic( EVtUiPanicResetFailed ) );
         iSession.HandleEngineResetL();
         }
         
     
     __VTPRINTEXITR( "CEventObserver.HandleVTCommandPerformedL %d", 1 )
                  
    }

#ifndef NGA_VIDEO_RENDER
void CLcVtSession::CEventObserver::vtHandleFrameL(
         TFrameType aType,
         CFbsBitmap* aBitmap )
    {
    iSession.vtHandleFrameL( aBitmap);
    }

void CLcVtSession::CEventObserver::vtSetFrame(
        TFrameType aType,
        CFbsBitmap* aBitmap )
    {
    }
#endif

TBool CLcVtSession::IsFeatureSupported( CLcEngine::TLcFeature aLcFeature )
{
    __VTPRINT2( DEBUG_GEN, "CLcVtSession.IsFeatureSupported feature=%d", aLcFeature)
    TBool flag = EFalse;

    switch ( aLcFeature )
        {
        case CLcEngine::ELcSendVideoQuery:
            {
            //MO call: return false; MT call: return true if key = Ask first, return false if key = No or Yes.
            MVtEngSessionInfo::TDirection direction;
            if ( iModel->Session().GetDirection( direction ) != KErrNone )
                {
                direction = MVtEngSessionInfo::EDirectionMT;
                }
            __VTPRINT2( DEBUG_GEN, "CLcVtSession.IsFeatureSupported direction=%d", direction )

            if ( direction == MVtEngSessionInfo::EDirectionMT 
                 && VideoSendingKey == KAllwaysQuery )
                {
                flag = ETrue;
                }
            }
            break;

        default:
            break;
        }

    // return feature not supported if not handled
    return flag;
}

// End of file

/*
 * 
// -----------------------------------------------------------------------------
// CLcVtSession::UpdateLocalVideoRenderParamL
// -----------------------------------------------------------------------------
//
void CLcVtSession::UpdateLocalVideoRenderParamL()
    { 
    __VTPRINTENTER( "CLcVtSession.UpdateLocalVideoRenderParamL" )
    
    if(iLcVtStates->ExecState() != TLcVtStates::EExecStateRunning )
        {
        __VTPRINTEXIT( "CLcVtSession.UpdateLocalVideoRenderParamL -- not running state" )
        return;
        }
    
    MVtEngMedia& media = Model().Media();                
    const TBool viewFinderStarted =
        media.RenderingStarted( MVtEngMedia::EMediaOutgoing );
    
    if( viewFinderStarted )
        {
        ExecuteCmdL( KVtEngStopViewFinder );
        }  
    
    TRect VFRect = LocalVideoPlayer()->LcWindow()->LcWindowRect();    
    TSize size (VFRect.Size());  
    
    TVtEngRenderingOptions configViewfinder(*iEventObserver,size);

    ExecuteCmdL( KVtEngPrepareViewFinder, configViewfinder );
    
    if ( viewFinderStarted )
        {
        // Start viewfinder.
        ExecuteCmdL( KVtEngStartViewFinder );
        }    
    
    __VTPRINTEXIT( "CLcVtSession.UpdateLocalVideoRenderParamL" )    
    }

// -----------------------------------------------------------------------------
// CLcVtSession::UpdateRemoteVideoRenderParamL
// -----------------------------------------------------------------------------
//
void CLcVtSession::UpdateRemoteVideoRenderParamL()
    {    
    __VTPRINTENTER( "CLcVtSession.UpdateRemoteVideoRenderParamL" )
    
    if(iLcVtStates->ExecState() != TLcVtStates::EExecStateRunning )
        {
        __VTPRINTEXIT( "CLcVtSession.UpdateRemoteVideoRenderParamL -- not running state" )
        return;
        }    
    
    MVtEngMedia& media = Model().Media();
    const TBool remoteRenderStarted =
        media.RenderingStarted( MVtEngMedia::EMediaIncoming );
    
    ExecuteCmdL( KVtEngStopRenderRemote );
    
    TRect RMRect = RemoteVideoPlayer()->LcWindow()->LcWindowRect();    
    const TSize KRMsize( RMRect.Size());
    TRect screen( KRMsize );
    TRect clip( KRMsize );
    const TPoint KRMcordinate( RMRect.iTl.iX,  RMRect.iTl.iY );    
    
    __VTPRINT2( DEBUG_MEDIA , "    RMRect.iTl.iX: %d", RMRect.iTl.iX)
    __VTPRINT2( DEBUG_MEDIA , "    RMRect.iTl.iY: %d", RMRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    RMRect.iWidth: %d", RMRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    RMRect.iHeight: %d", RMRect.Height() )    
    
    TVtEngRenderingOptionsDP configRemoteRender(*iEventObserver, *iWsSession ,*iWsSessionScreenDev, *iMainWindow, screen, clip, KRMcordinate );  

    ExecuteCmdL(
        KVtEngPrepareRemoteRenderDP,
        configRemoteRender );    
    
    TRAP_IGNORE( ExecuteCmdL( KVtEngStartRenderRemote ) );
    
    __VTPRINTEXIT( "CLcVtSession.UpdateRemoteVideoRenderParamL" )

    }

// -----------------------------------------------------------------------------
// CLcVtSession::LayoutChangedL
// -----------------------------------------------------------------------------
//
void CLcVtSession::LayoutChangedL()
    {    
    __VTPRINTENTER( "CLcVtSession.LayoutChangedL" )
    
    if(iLcVtStates->ExecState() != TLcVtStates::EExecStateRunning )
        {
        __VTPRINTEXIT( "CLcVtSession.LayoutChangedL -- not running state" )
        return;
        }    
    
    TRAPD( error, ExecuteCmdL( KVtEngHandleLayoutChange ) );
    
    __VTPRINTEXIT( "CLcVtSession.LayoutChangedL" )
    }
 
 */
