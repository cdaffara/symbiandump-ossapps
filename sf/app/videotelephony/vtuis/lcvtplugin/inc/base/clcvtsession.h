/*
* ==========================================================================
*  Name        : lcvtsession.h
*  Description : 
*  Version     :
*
*  Copyright ?2005 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==========================================================================
*/

#ifndef LCVTSESSION_H
#define LCVTSESSION_H

#include    <e32base.h>
#include    <w32std.h>
#include    <f32file.h>
#include    <e32property.h>
#include    <UikonInternalPSKeys.h>
#include    <centralrepository.h>
#include    <settingsinternalcrkeys.h>
#include    <AknQueryDialog.h>
#include    <gdi.h>

#include    <lcsession.h>
#include    <lcaudiocontrol.h> 
#include    <lcsessionobserver.h>
#include    <lcuiprovider.h>
#include    <lcengine.h>
#include    <cvtlogger.h>
#include    <mvtengeventobserver.h>
#include    <mvtengcommandobserver.h>
#include    <mvtengcommandhandler.h>
#include    <mvtengframeobserver.h>
#include    <mvtengaudio.h>
#include    <mvtengsessioninfo.h>
#include    <vtengevents.h>
#include    <mvtengmedia.h>

#include    <tvtengrendering.h>
#include    <tvtengrenderingdp.h>
#include    <tvtengrenderingnga.h>
#include    <tvtengrenderingdsa.h>


#include    "tlcvtcamerahandler.h"
#include    "clcvtactiveexec.h"
#include    "tlcvtstatebase.h"
#include    "lcvtindicatorcontroller.h"

#define     NGA_VIDEO_RENDER 

class CVtEngModel;
class CLcVtLocalVideoPlayer;
class CLcVtRemoteVideoPlayer;
class TLcVtStates;
class CLcVtCmdExecutor;


class CLcVtSession : public CBase, 
                     public MLcSession,
                     public MLcAudioControl,
                     public MLcVtActiveExec,
                     public MLcVtStateContext
    {
public:

    static CLcVtSession* NewL();
    
    ~CLcVtSession();

public: // From MLcSession
    
    TLcSessionState LcSessionState() const;
    
    void EstablishLcSessionL();
    
    void TerminateLcSessionL();    
    
    void SetLcSessionObserver( MLcSessionObserver* aObserver );
    
    void SetLcUiProvider( MLcUiProvider* aUiProvider );
    
    TBool IsBackgroundStartup();
    
    MLcVideoPlayer* RemoteVideoPlayer();
    
    MLcVideoPlayer* LocalVideoPlayer();   
    
    const TDesC& LocalDisplayName();
    
    const TDesC& RemoteDisplayName();
    
    TInt SetParameter( TInt aId, TInt aValue );
    
    TInt ParameterValue( TInt aId );    
    
    TInt SetForegroundStatus( TBool aIsForeground );
    
    const TDesC& RemoteDetails();
    
    void UpdateLcSessionL();    
    
    TBool SendDialTone( TChar aKey );
  
public: // from MLcAudioControl    
    
    TBool IsLcAudioMutedL();
    
    void MuteLcAudioL( TBool aMute );
    
    TBool IsLcMicMutedL();    
    
    void MuteLcMicL( TBool aMute );    
    
    TBool IsEnablingLcLoudspeakerAllowed();
    
    void EnableLcLoudspeakerL( TBool aEnabled ); 
    
    TBool IsLcLoudspeakerEnabled();
    
    TInt LcVolumeL(); 
    
    void SetLcVolumeL( TInt aValue );
    
    void IncreaseLcVolumeL();    
    
    void DecreaseLcVolumeL(); 
    
public: //from MLcVtActiveExec
    /**
    * @see MLcVtActiveExec::ActiveExecExecuteL.
    */
    TBool ActiveExecExecuteL(
        CLcVtActiveExec& aActiveExec,
        const TInt aState,
        TInt& aNextState,
        TRequestStatus& aRequest );

    /**
    * @see MLcVtActiveExec::ActiveExecContinue.
    */
    TBool ActiveExecContinue(
        CLcVtActiveExec& aActiveExec,
        TInt& aState,
        const TInt aError );

    /**
    * @see MLcVtActiveExec::ActiveExecCancel.
    */
    void ActiveExecCancel(
        CLcVtActiveExec& aActiveExec,
        const TInt aState );

    /**
    * @see MLcVtActiveExec::ActiveExecDone.
    */
    virtual void ActiveExecDone(
        CLcVtActiveExec& aActiveExec,
        const TInt aInitialState );    

public: //new function
    /**
    * @see MLcVtActiveExec::ActiveExecExecuteL.
    */
    TBool ActiveExecInitExecuteL(
        const TInt aState,
        TInt& aNextState,
        TRequestStatus& aRequest );

    /**
    * @see MLcVtActiveExec::ActiveExecContinue.
    */
    TBool ActiveExecInitContinue(
        TInt& aState,
        const TInt aError );

    /**
    * @see MLcVtActiveExec::ActiveExecCancel.
    */
    void ActiveExecInitCancel(
        const TInt aState );

    /**
    * @see MLcVtActiveExec::ActiveExecDone.
    */
    void ActiveExecInitDone(
        const TInt aInitialState );   
    
    /**
    * Executes command taking no parameters.
    * @param aCommand command to be performed.
    * @param aRequest request status.
    */
    void ActiveExecInitExecuteCommandL(
        const TVtEngCommandId aCommand,
        TRequestStatus& aRequest );

    /**
    * Executes command taking no parameters.
    * @param aCommand command to be performed.
    * @param aParam command parameters.
    * @param aRequest request status.
    */
    template < class T >
    void ActiveExecInitExecuteCommandL(
        const TVtEngCommandId aCommand,
        T& aParam,
        TRequestStatus& aRequest );
    
    /**
    * Executes prepare viewfinder command.
    * @param aRequest request status to be completed when finished.
    */
    void ActiveExecInitPrepareViewFinderL(
        TRequestStatus& aRequest );

    /**
    * Executes prepare remote render command.
    * @param aRequest request status to be completed when finished.
    */
    void ActiveExecInitPrepareRemoteRenderL(
        TRequestStatus& aRequest );    
    
    /**
    * Executes 'prepare camera' command.
    * @param aSource source to be prepared.
    * @param aRequest request status.
    */
    void ActiveExecInitPrepareCameraL(
        const MVtEngMedia::TMediaSource aSource,
        TRequestStatus& aRequest );

    /**
    * Executes 'set source' command.
    * @param aSource source to be selected.
    * @param aRequest request status.
    */
    void ActiveExecInitSetSourceL(
        const MVtEngMedia::TMediaSource aSource,
        TRequestStatus& aRequest );
        
    /**
    * Performs a command without any parameters.
    * @param aCommand command to be performed.
    */
    void ExecuteCmdL( const TVtEngCommandId aCommand );

    /**
    * Performs a command with a parameter.
    * @param aCommmand command to be performed.
    * @param aParam parameter.
    */
    template< class T >
    void ExecuteCmdL(
        const TVtEngCommandId aCommand,
        T& aParam );

    // ASYNCHRONOUS EXECUTION:

    /**
    * Performs a command without any parameters.
    * @param aExecutorPtr pointer to which returned value is stored.
    * @param aCommand command to be performed.
    * @param aStatus request status.
    * @return dialog.
    */
    CLcVtCmdExecutor* ExecuteCmdAsyncL(
        CLcVtCmdExecutor** aExecutorPtr,
        const TVtEngCommandId aCommand,
        TRequestStatus& aStatus );

    /**
    * Performs a command with a parameter.
    * @param aExecutorPtr pointer to which returned value is stored.
    * @param aCommmand command to be performed.
    * @param aParam parameter.
    * @param aStatus request status.
    * @return dialog.
    */
    template< class T >
    CLcVtCmdExecutor* ExecuteCmdAsyncL(
        CLcVtCmdExecutor** aExecutorPtr,
        const TVtEngCommandId aCommand,
        T& aParam,
        TRequestStatus& aStatus );

    /**
    * Performs a command.
    * @param aExecutorPtr pointer to which returned value is stored.
    * @param aCommand command.
    * @param aParams parameter, optional.
    * @param aStatus request status.
    * @return dialog.
    */
    CLcVtCmdExecutor* DoExecuteCmdAsyncL(
        CLcVtCmdExecutor** aExecutorPtr,
        const TVtEngCommandId aCommand,
        TDesC8* aParams,
        TRequestStatus& aStatus );   
    
    /**
    * Executes command in TRAP harnness.
    * @param aCommand command identifier.
    * @param aParam optional, parameters.
    * @return error code.
    */
    TInt Execute( const TVtEngCommandId aCommand, TDesC8* aParam );   
    
    /**
    * Handles layout changes basically calling HandleLayoutChangedL
    * in trap harness.
    * @return error code.
    */
    //TInt HandleLayoutChanged();

    /**
    * Handles layout changes.
    */
    void HandleLayoutChangedL();
    
public: //from MLcVtStateContext
    
    /** @see MLcVtStateContext::StartupPhase1L */
    void StartupPhase1L();

    /** @see MLcVtStateContext::ShutdownL */
    void ShutdownL();

    /** @see MLcVtStateContext::StartupPhase2L */
    void StartupPhase2L();

    /** @see MLcVtStateContext::ChangeApplicationFocus */
    void ChangeApplicationFocus( const TBool aForeground );

    /** @see MLcVtStateContext::ChangeState */
    void ChangeState( TLcVtStateBase* aState );

    /** @see MLcVtStateContext::StartTone */
    //void StartDtmfTone( const TChar& aTone );

    /** @see MLcVtStateContext::StopTone */
    //void StopDtmfTone();

    /** @see MLcVtStateContext::Model */
    CVtEngModel& Model();

    /** @see MLcVtStateContext::NumberSource*/
    TDesC* NumberSource() const;

    /** @see MLcVtStateContext::RefreshL */
    void RefreshL( const TInt aRefreshFlags );
    
    /** @see MLcVtStateContext::RefreshStatesL */
    void  RefreshStatesL();

    /** @see MLcVtStateContext::DoExecuteCmdL */
    void DoExecuteCmdL(
        const TVtEngCommandId aCommand,
        TDesC8* aParams );

    /** @see MLcVtStateContext::DoHandleLayoutChangedL */
    void DoHandleLayoutChangedL();
    
    /** From MLcVtShutterObserver via MLcVtStateContext
    *  Exits the application.
    */
    void HandleShutdownReady();    

    /** @see MLcVtStateContext::SetCallIdL */
    void SetCallIdL( const TInt aCallId );

    /** @see MLcVtStateContext::SetCallNameL */
    TBool SetCallNameL( const TDesC& aName ); 
    
    /** @see MLcVtStateContext::SetCallNumberL */
    void SetCallNumberL( const TDesC& aNumber );
    
    /** @see MLcVtStateContext::StartShutdown */
    void StartShutdown();    
    
public: //new functions    
    
    /**
    * Static async callback for handling layout change.
    */
    //static TInt DelayedHandleLayoutChanged( TAny* aPtr );
    
    /**
    * Callback function to exit.
    * @param aAny pointer to instance of this class.
    * @return KErrNone always.
    */    
    static TInt DoExit( TAny* aAny );
    
    /**
    * Close UI Application.
    */   
    void CloseApp();
    
    /**
    * Handles engine reset.
    */
    void HandleEngineResetL();    

    /**
    * Handle plugin command.
    */    
    void HandleCommandL(const TInt acommand);
    
    /**
    * Sets given zoom step, if it allowed.
    * @param aZoomStep New zoom step to be set.
    */
    void SetZoomFactorL( TInt aZoomStep ); 
    
    /**
    * Updates rendering parameters.
    */
    void UpdateRenderingParametersL();    

    inline TLcVtStates& LcVtStates();

#ifndef NGA_VIDEO_RENDER
    void vtHandleFrameL( CFbsBitmap* aBitmap ); 
#endif  
    TBool IsFeatureSupported( CLcEngine::TLcFeature aLcFeature );


private:
     CLcVtSession();
    
    void ConstructL();    

#ifndef NGA_VIDEO_RENDER
    /**
    * connect to font & bitmap server.
    */    
    void ConnectFbsServerL();
    
    /**
    * disconnect from font & bitmap server.
    */    
    void DisconnectFbsServer();
#endif       

    // COMMAND HANDLING
    
    /**
    * Enables audio (unmute microphone).
    */
    void CmdEnableAudioL();

    /**
    * Disables audio (mute microphone).
    */
    void CmdDisableAudioL();
    
    /**
    * Switches used camera.
    * @param aPrimaryCamera ETrue if primary camera is to be selected,
    *        EFalse if secondary camera is to be selected.
    */
    void CmdUseCameraL( const TBool aPrimaryCamera );
    
    /**
    * Activates loudspeaker.
    */
    void CmdActivateLoudspeakerL();

    /**
    * Deactivates loudspeaker.
    */
    void CmdDeactivateLoudspeakerL();
    
    /**
    * Enables video.
    */
    void CmdEnableVideoL();
    
    /**
    * Disables video.
    */
    void CmdDisableVideoL();
    
#ifdef NGA_VIDEO_RENDER
    void CreateVideoWindowsL();
    void DestroyVideoWindows();
#endif

private:
    class CEventObserver;

private:
#ifdef NGA_VIDEO_RENDER    
    RWindow*  iRemoteVideoWindow;
    RWindow*  iLocalVideoWindow;
#else
    CFbsBitmap* iDummy;    
    CWindowGc*  iGc;
    TBool   iFbsStarted; 
    RWsSession* iWsSession;    
    CWsScreenDevice* iWsSessionScreenDev;
    RWindowGroup *iRootWin;  
#endif
    // Owned event observer.
    CEventObserver* iEventObserver;
    // Engine model.
    CVtEngModel* iModel;
    // plugin states (media, audio, boolean variables)
    TLcVtStates* iLcVtStates;

    TVtCameraHandler          iCameraHandler;
    CLcVtLocalVideoPlayer*    iLocalVideoPlayer;
    CLcVtRemoteVideoPlayer*   iRemoteVideoPlayer;
    
    MLcSessionObserver* iObserver;
    MLcUiProvider* iUiProvider;
  
    TLcSessionState iSessionState;
    
    // Owned active executor for initialisation / uninitialisation.
    CLcVtActiveExec*  iActiveExec;
    
    CLcVtCmdExecutor* iCmdExecutor;
    
    // Reference to request status.
    TRequestStatus* iShutdownRequest;
    
    TLcVtStateBase*  iState;
    
    // ASync callback.
    CAsyncCallBack* iAsyncCallback;
    
    // Async callback for layout change
    //CAsyncCallBack* iLayoutChangeCallback;

    // DTMF tone, used as parameter to KVtEngStartDtmfTone.
    TPckgBuf< TChar > iDtmfTone;  
                
    TBool iLoudspeakerEnabled;
    TBool iMute;
    
    RWindow* iMainWindow;
    
    RWsSession iRwSession;
    RWindowGroup* iRwGroup;
    CWsScreenDevice* iDev;

    HBufC* iRemoteDisplayName;
    HBufC* iRemoteDetails;

    TInt VideoSendingKey;
    
    LcVtIndicatorController* iIndicatorCtr;
    
    TBool iForeGroundStatus;
    };
    

TLcVtStates& CLcVtSession::LcVtStates()
    {
    return *iLcVtStates;
    }
#endif // LCVTSESSION_H

// end of file
