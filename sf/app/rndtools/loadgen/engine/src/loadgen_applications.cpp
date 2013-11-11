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


// INCLUDE FILES
#include "loadgen_applications.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <e32math.h>
#include "loadgen_traces.h"

_LIT(KThreadName, "Applications %d");

const TInt KDefaultStartDelay = 500;
const TInt KThreadNameLength = 64;
const TInt KDescriptorBufSize = 256;
const TInt KPriorityBufSize = 16;
const TInt KScanCodeStart = 32;
    
// ===================================== MEMBER FUNCTIONS =====================================

CAppLauncher* CAppLauncher::NewL( TApplicationsAttributes& aAttributes, TInt aReferenceNumber )
    {
    CAppLauncher* self = new(ELeave) CAppLauncher( aAttributes, aReferenceNumber );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------------------------

CAppLauncher::~CAppLauncher()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CAppLauncher::CAppLauncher(  
           TApplicationsAttributes& aAttributes, TInt aReferenceNumber ) : 
           iAttributes( aAttributes )
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CAppLauncher::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadApplications;
    
    TBuf<KThreadNameLength> threadName;
    threadName.Format( KThreadName, iAttributes.iId );
    
    // create a thread
    User::LeaveIfError( iThread.Create( threadName, ThreadFunction, 
                                        KDefaultStackSize*2, KMinHeapSize, 
                                        KKilo * KMinHeapSize, (TAny*) &iAttributes ) );
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CAppLauncher::ThreadFunction( TAny* aThreadArg )
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install( pS );

    // start generating load, pass pointer to arguments
    GenerateLoad( *( ( TApplicationsAttributes* ) aThreadArg ) );

    delete pS;
    delete pC;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CAppLauncher::GenerateLoad( TApplicationsAttributes& aAttributes )
    {
    CAppLauncherManager* appLauncherManager = NULL;
    TRAPD( err, appLauncherManager = CAppLauncherManager::NewL( aAttributes ) );
    if ( err == KErrNone ) 
        {
        CActiveScheduler::Start();
        }
    delete appLauncherManager;
    }

// --------------------------------------------------------------------------------------------

void CAppLauncher::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CAppLauncher::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CAppLauncher::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority( CLoadGenUtils::SettingItemToThreadPriority( iAttributes.iPriority ) );
    }
    
// --------------------------------------------------------------------------------------------

void CAppLauncher::Close()
    {
    CLoadBase::Close();
    
    if ( iThread.ExitReason() == 0 ) // check if the thread is still alive
        {
        // signal the thread that it needs to close
        iThread.RequestComplete( iAttributes.iDeathStatus, KErrCancel );

        // wait the thread to die
        TRequestStatus waiter;
        iThread.Logon( waiter );
        User::WaitForRequest( waiter );
        iThread.Close();
        }
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CAppLauncher::Description()
    {
    TBuf<KDescriptorBufSize> buf;
    TBuf<KPriorityBufSize> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription( iAttributes.iPriority, prioBuf );
    
    _LIT(KAppLauncherEntry, "[%d] Appls prio=%S max apps=%d heartbeat=%dms random=%d%%");
    buf.Format( KAppLauncherEntry, iAttributes.iId, &prioBuf, iAttributes.iMaxOpen, 
                iAttributes.iHeartBeat, iAttributes.iRandomVariance );
   
    return TPtrC( buf );
    }               

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CAppLauncherManager* CAppLauncherManager::NewL( TApplicationsAttributes& aAttributes )
    {
    CAppLauncherManager* self = new(ELeave) CAppLauncherManager(aAttributes);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------------------------

CAppLauncherManager::CAppLauncherManager( TApplicationsAttributes& aAttributes ) :
            CActive( EPriorityStandard ), iAttributes( aAttributes )
    {
    iAppEventType = EApplicationsLaunchApplication;
    }

// --------------------------------------------------------------------------------------------
    
CAppLauncherManager::~CAppLauncherManager()
    {
    if ( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }
    delete iLauncherEngine;
    Cancel();
    
    iWsSession.Close();
    }

// --------------------------------------------------------------------------------------------
 
void CAppLauncherManager::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    // create instance to application launcher
    iLauncherEngine = CLauncherEngine::NewL( iAttributes );
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    // init
    User::LeaveIfError( iWsSession.Connect() );
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iPeriodicTimer->Start( KDefaultStartDelay, 
                           CLoadGenUtils::MilliSecondsToMicroSeconds( 
                               iAttributes.iLaunchingInterval, iAttributes.iRandomVariance ),
                           TCallBack( PeriodicTimerCallBack, this ) );
    }
    
// --------------------------------------------------------------------------------------------
 
void CAppLauncherManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CAppLauncherManager::DoCancel()
    {
    }

// --------------------------------------------------------------------------------------------

TInt CAppLauncherManager::PeriodicTimerCallBack( TAny* aAny )
    {
    CAppLauncherManager* self = static_cast<CAppLauncherManager*>( aAny );

    TRAPD( err, self->SimulateEventL() );
    if ( KErrNone != err )
        {
        return err;
        }
    return KErrNone;
    }
    
// --------------------------------------------------------------------------------------------
 
void CAppLauncherManager::SimulateEventL()
    {    
    // Randomly start new applications.
    // After maximum applications launched stop one application.
    // And after that start launching again.
    if ( iAppEventType == EApplicationsLaunchApplication && 
         iAttributes.iMaxOpen == iLauncherEngine->AppLaunchCounter() )
        {
        iAppEventType = EApplicationsCloseApplication;
        }
    if ( iAppEventType == EApplicationsCloseApplication && 
         iAttributes.iMaxOpen -1 == iLauncherEngine->AppLaunchCounter() )
        {
        iAppEventType = EApplicationsLaunchApplication;
        }
    
    // Launch or stop
    if ( iAppEventType == EApplicationsLaunchApplication )
        {
        iLauncherEngine->StartAppLaunchingL();
        }  
    else
        {
        iLauncherEngine->StopApplication( ETrue );
        }
    }
    
// --------------------------------------------------------------------------------------------

#include <avkon.hrh>
#include <e32std.h>
#include <w32std.h>
#include <apgtask.h>
#include <bautils.h>
#include <s32file.h>
#include <apgwgnam.h> 

// ---------------------------------------------------------------------------

CLauncherEngine* CLauncherEngine::NewL( TApplicationsAttributes& aAttributes )
    {
    CLauncherEngine* self = new(ELeave) CLauncherEngine( aAttributes );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------

CLauncherEngine::CLauncherEngine( TApplicationsAttributes& aAttributes ) : 
                        iAttributes( aAttributes )
    {
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::ConstructL()
    {
    LOGSTRING("LoadGen: CLauncherEngine::ConstructL");
    iAppKeyPressManager = CAppKeyPressManager::NewL( iAttributes );
    iAppLaunchCounter = 0;
    User::LeaveIfError( iTimer.CreateLocal() );

    User::LeaveIfError( iLs.Connect() );
    User::LeaveIfError( iWs.Connect() );

    }

// ---------------------------------------------------------------------------

CLauncherEngine::~CLauncherEngine()
    {
    LOGSTRING("LoadGen: CLauncherEngine::~CLauncherEngine");
    StopApplication( EFalse );
    iWs.Close();
    iLs.Close();
    iTimer.Close();
    delete iAppKeyPressManager;
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::StartAppLaunchingL()
    {
    LOGSTRING("LoadGen: CLauncherEngine::StartAppLaunchingL");
    if ( iAppLaunchCounter == iAttributes.iMaxOpen )
        {
        return;
        }

    // start launching
    TBool launched( EFalse );
    while ( !launched )
        {
        launched = LaunchApplicationL();
        }
    }

// ---------------------------------------------------------------------------

TBool CLauncherEngine::LaunchApplicationL()
    {
    LOGSTRING("LoadGen: CLauncherEngine::LaunchApplicationL");
    TBool result( EFalse );
    // get the uid of the current app
    iCurrentAppUid = KNullUid;
    TApaAppInfo appInfo;
    
    // get application list size
    TInt appsListSize = iAttributes.iAppsArray->MdcaCount();
    // get random application number in the list
    TInt appTolaunch = CLoadGenUtils::RandomNumber( 0,  appsListSize - 1 );
    // get file name of the application to be launched
    TFileName launchFileName = iAttributes.iAppsArray->MdcaPoint( appTolaunch );
    iLs.GetAllApps();
    while ( iLs.GetNextApp( appInfo ) == KErrNone )
        {
        // get file name in  current appInfo
        TFileName appFileName = appInfo.iFullName;
        // get AppUid
        if ( appFileName.CompareF( launchFileName ) == 0 )
            {
            iCurrentAppUid = appInfo.iUid;
            break;
            }
        }

    if ( iCurrentAppUid != KNullUid )
        {        
        // parse the filename
        TParse nameParser;
        nameParser.SetNoWild( iAttributes.iAppsArray->MdcaPoint( appTolaunch ), NULL, NULL );
        iCurrentAppNameAndExt.Copy( nameParser.Drive() );
        iCurrentAppNameAndExt.Append( nameParser.NameAndExt() );
           
        // do not try to launch these apps
        if ( iCurrentAppUid != KNullUid &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch).FindF(_L("\\loadgen.")) == KErrNotFound &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\Phone." ) ) == KErrNotFound &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch).FindF( _L("\\Startup.") ) == KErrNotFound &&  
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\SplashScreen.") ) == KErrNotFound &&  
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\cameraapp.") ) == KErrNotFound &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\eshell.") ) == KErrNotFound &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\ConnTest.") ) == KErrNotFound &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\Launcher.") ) == KErrNotFound &&
              
#ifdef __WINS__
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\DebugAgent.") ) == KErrNotFound &&
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\redirect.") ) == KErrNotFound &&              
#endif              
              iAttributes.iAppsArray->MdcaPoint( appTolaunch ).FindF( _L("\\VoiceRecorder.") ) == KErrNotFound )            
            {
        
            // check if the app is already running
            TApaTaskList taskList( iWs );
            TApaTask thisTask = taskList.FindApp( iCurrentAppUid );
            if ( !thisTask.Exists( ))
                {
                // check the program's capabilities
                TApaAppCapabilityBuf buf;
                iLs.GetAppCapability( buf, iCurrentAppUid );
                TApaAppCapability cap = buf();
        
                // if it's embeddable only, don't launch if setting is enabled
                // if it's hidden, don't launch if setting is enabled
                if ( cap.iEmbeddability != TApaAppCapability::EEmbeddableOnly &&
                     !cap.iAppIsHidden )
                    {
                    LOGSTRING2("launchFileName = %S", &launchFileName);
                    
                    DoLaunchApplicationL();
                    result = ETrue;
                    }            
                } 
            }
        }
    return result;
    }


// ---------------------------------------------------------------------------

void CLauncherEngine::DoLaunchApplicationL()
    {
    LOGSTRING("LoadGen: CLauncherEngine::DoLaunchApplicationL");

    // Find the task with uid3
    TApaTaskList tasklist( iWs );
    TApaTask task=tasklist.FindApp( iCurrentAppUid );

    if ( !task.Exists() )
        // Task doesn't exist, launch a new instance of an application
        {
        TApaAppInfo appInfo;
        User::LeaveIfError( iLs.GetAppInfo( appInfo, iCurrentAppUid ) );

        CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL( appInfo.iFullName );

        // app is launched in background
        cmdLine->SetCommandL( EApaCommandBackground );       

        // start the app
        User::LeaveIfError( iLs.StartApp( *cmdLine ) );
        iAppKeyPressManager->AddNewApplicationUidToKeyEventsL( iCurrentAppUid );
        iAppLaunchCounter++;

        CleanupStack::PopAndDestroy( cmdLine );
        }
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::StopApplication( TBool aRandomApplication )
    {
    LOGSTRING("LoadGen: CLauncherEngine::StopApplication");

    // remove key pressing instance
    TUid appToDelete = iAppKeyPressManager->KillApplication( aRandomApplication );
    if ( appToDelete != KNullUid )
        {
        TApaTaskList taskList( iWs );
        TApaTask thisTask = taskList.FindApp( appToDelete );
    
        if ( thisTask.Exists() )
            {
            // since the application is still open, let's close it
            thisTask.EndTask();
            }
        iAppLaunchCounter--;
        }
    
    // return if only one application requested
    if ( aRandomApplication )
        {
        return;
        }
    
    // remove all launched applications because load is requested to be closed
    while ( appToDelete != KNullUid )
        {
        appToDelete = iAppKeyPressManager->KillApplication( aRandomApplication );
            
        if ( appToDelete != KNullUid )
            {
            TApaTaskList taskList( iWs );
            TApaTask thisTask = taskList.FindApp( appToDelete );
        
            if ( thisTask.Exists() )
                {
                // since the application is still open, let's close it
                thisTask.EndTask();
                }
            iAppLaunchCounter--;
            }
        }
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CAppKeyPressManager* CAppKeyPressManager::NewL( TApplicationsAttributes& aAttributes )
    {
    LOGSTRING("LoadGen: CAppKeyPressManager::NewL");
    CAppKeyPressManager* self = new(ELeave) CAppKeyPressManager( aAttributes );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------------------------

CAppKeyPressManager::CAppKeyPressManager( TApplicationsAttributes& aAttributes ) :
                                            iAttributes( aAttributes )
    {
    LOGSTRING("LoadGen: CAppKeyPressManager::CAppKeyPressManager");
    }

// --------------------------------------------------------------------------------------------
    
CAppKeyPressManager::~CAppKeyPressManager()
    {
    LOGSTRING("LoadGen: CAppKeyPressManager::~CAppKeyPressManager");
    if ( iKeyPresses.Count() != 0 )
        {
        iKeyPresses.ResetAndDestroy();
        }

    iKeyPresses.Close();
    }

// --------------------------------------------------------------------------------------------
 
void CAppKeyPressManager::ConstructL()
    {
    LOGSTRING("LoadGen: CAppKeyPressManager::ConstructL");
    }
    
   
// --------------------------------------------------------------------------------------------
void CAppKeyPressManager::AddNewApplicationUidToKeyEventsL( TUid aUid )
    {
    LOGSTRING("LoadGen: CAppKeyPressManager::AddNewApplicationUidToKeyEventsL");
    CApplicationKeyPresses* applicationKP = CApplicationKeyPresses::NewL( aUid,
                                                                    iAttributes );
    CleanupStack::PushL( applicationKP );
    User::LeaveIfError( iKeyPresses.Append( applicationKP ) );
    CleanupStack::Pop( applicationKP );
    }

// --------------------------------------------------------------------------------------------
TUid CAppKeyPressManager::KillApplication( TBool aRandomApplication )
    {
    TUid applicationUid = KNullUid;
    TInt appToDelete = iKeyPresses.Count() - 1 ;
    // remove the newest application if not random 
    if ( aRandomApplication )
        {
         appToDelete = CLoadGenUtils::RandomNumber( 0, iKeyPresses.Count() - 1 );
        }
    if ( iKeyPresses.Count() )
        {
        // get random application and delete it
        
        applicationUid = iKeyPresses[appToDelete]->ApplicationUid();
        delete iKeyPresses[appToDelete];
        iKeyPresses.Remove( appToDelete );
        }
    return applicationUid;
    }
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CApplicationKeyPresses* CApplicationKeyPresses::NewL( TUid aUid, TApplicationsAttributes&  
        aAttributes )
    {
    LOGSTRING("LoadGen: CApplicationKeyPresses::NewL");
    CApplicationKeyPresses* self = new(ELeave) CApplicationKeyPresses( aUid, aAttributes );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------------------------

CApplicationKeyPresses::CApplicationKeyPresses( TUid aUid, 
                                                        TApplicationsAttributes& aAttributes ) :
                                                iUid( aUid ), iAttributes( aAttributes ) 
    {
    LOGSTRING("LoadGen: CApplicationKeyPresses::CApplicationKeyPresses");
    }

// --------------------------------------------------------------------------------------------
    
CApplicationKeyPresses::~CApplicationKeyPresses()
    {
    LOGSTRING("LoadGen: CApplicationKeyPresses::~CApplicationKeyPresses");
    if ( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }
    
    iWsSession.Close();
    }

// --------------------------------------------------------------------------------------------
 
void CApplicationKeyPresses::ConstructL()
    {
    LOGSTRING("LoadGen: CApplicationKeyPresses::ConstructL");
    // init
    User::LeaveIfError( iWsSession.Connect() );
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    if ( iAttributes.iKeyPressType )
        {
        iPeriodicTimer->Start( KDefaultStartDelay,
                               CLoadGenUtils::MilliSecondsToMicroSeconds( 
                                       iAttributes.iHeartBeat,
                                       iAttributes.iRandomVariance ),
                                TCallBack( PeriodicTimerCallBack, this ) );
        }
    }
    
// --------------------------------------------------------------------------------------------

TInt CApplicationKeyPresses::PeriodicTimerCallBack( TAny* aAny )
    {
    CApplicationKeyPresses* self = static_cast<CApplicationKeyPresses*>( aAny );
    self->SimulateKeyEvent();
    return KErrNone;
    }
    
// --------------------------------------------------------------------------------------------
 
void CApplicationKeyPresses::SimulateKeyEvent()
    {
    LOGSTRING("LoadGen: CApplicationKeyPresses::SimulateKeyEvent");
    // for arrow key events
    
    TInt wgId(0);
    CApaWindowGroupName::FindByAppUid( iUid, iWsSession, wgId );
    // generate a random arrow key event
    TWsEvent event;
    event.SetType( EEventKey );
    event.Key()->iCode = CLoadGenUtils::RandomNumber( EKeyLeftArrow, EKeyDownArrow );
    event.Key()->iScanCode = event.Key()->iCode - KScanCodeStart;
    event.Key()->iModifiers = 0;
    event.Key()->iRepeats = 0;
    iWsSession.SendEventToWindowGroup( wgId, event );
    }

// End of File
