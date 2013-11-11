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
* Description:  Test Harness for VideoHelix Playback Plugin
 *
*/

// Version : %version: 16 %


#ifndef __VHPPTESTCASE_H__
#define __VHPPTESTCASE_H__

//  INCLUDES

#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#include <mpxplaybackpluginobserver.h>
#include <mpxplaybackplugin.h>
#include <mpxplaybackpluginversion2.h>

#include <coreapplicationuisdomainpskeys.h>
#include <ctsydomainpskeys.h>
#include <mmf/common/mmferrors.h>

#include <AccessoryServer.h>
#include <AccessoryControl.h>
#include <AccPolGIDHeader.h>
#include <AccPolGenericID.h>

#include <mpxvideoplaybackdefs.h>
#include "timeoutController.h"

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

//
//  FORWARD DECLARATIONS
//
class CMpxVideoPlayerUtility;
class CDlMgrTestClass;
class CMPXVideoAccessoryObserver;

// CLASS DECLARATION


enum TDlMgrStifEvents
{
    EConnectedToDownload,
    EFileMoving
};

typedef struct
{
    TInt  iEvent;
    TInt  iData;
    TInt  iError;
} TCallbackEvent;

typedef CArrayPtrFlat<TCallbackEvent> CCallbackEventArray;

NONSHARABLE_CLASS( MStifTestObserver )
{
    public:
        virtual void HandleUtilityEvent( TCallbackEvent* aEvent ) = 0;
};

/**
*  CVHPPTestClass test
*  class for STIF Test Framework TestScripter.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CVHPPTestClass ) : public CScriptBase,
                                      public MTimeoutObserver,
                                      public MMPXPlaybackPluginObserver,
                                      public MStifTestObserver
{
    public:  // Constructors and destructor

        /*
         * Two-phased constructor.
         */
        static CVHPPTestClass* NewL( CTestModuleIf& aTestModuleIf );

        /*
         * Destructor.
         */
        virtual ~CVHPPTestClass();

        void HandleUtilityEvent( TCallbackEvent* aEvent );

    public: // Functions from base classes

        /*
         * From CScriptBase Runs a script line.
         * @param aItem Script line containing method name and parameters
         * @return Symbian OS error code
         */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /*
         * C++ default constructor.
         */
        CVHPPTestClass( CTestModuleIf& aTestModuleIf );

        /*
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        // ---------------------------------------------------------------------
        //  Test Cases
        // ---------------------------------------------------------------------
        virtual TInt InitializeL( CStifItemParser& aItem );
        virtual TInt InitializeHandleL( CStifItemParser& aItem );
        virtual TInt InitializeLinkL( CStifItemParser& aItem );
        virtual TInt InitializeSdpL( CStifItemParser& aItem );
        virtual TInt IssueGeneralCommandL( CStifItemParser& aItem );
        virtual TInt IssuePlaybackCommandL( CStifItemParser& aItem );
        virtual TInt IssueVideoCommandL( CStifItemParser& aItem );
        virtual TInt IssueSeekingCommandL( CStifItemParser& aItem );
        virtual TInt SetPropertyL( CStifItemParser& aItem );
        virtual TInt GetPropertyL( CStifItemParser& aItem );
        virtual TInt GetSubPlayerNamesL( CStifItemParser& aItem );
        virtual TInt SelectSubPlayerL( CStifItemParser& aItem );
        virtual TInt SubPlayerName( CStifItemParser& aItem );
        virtual TInt SubPlayerIndex( CStifItemParser& aItem );
        virtual TInt GetMediaL( CStifItemParser& aItem );
        virtual TInt CancelRequest( CStifItemParser& aItem );
        virtual TInt SetPlaybackCompleteL( CStifItemParser& aItem );
        virtual TInt SetUnexpectedMmfEventL( CStifItemParser& aItem );
        virtual TInt ConnectToDownloadL( CStifItemParser& aItem );
        virtual TInt PauseDownloadL( CStifItemParser& aItem );
        virtual TInt ResumeDownloadL( CStifItemParser& aItem );
        virtual TInt CancelDownloadL( CStifItemParser& aItem );
        virtual TInt RetrievePdlStatusL( CStifItemParser& aItem );
        virtual TInt ChangeAspectRatioL( CStifItemParser& aItem );

        virtual TInt AlarmOn( CStifItemParser& aItem );

        virtual TInt PhoneCallRejected( CStifItemParser& aItem );
        virtual TInt VoiceCallAccepted( CStifItemParser& aItem );
        virtual TInt VideoCallAccepted( CStifItemParser& aItem );
        virtual TInt PlayduringVideoCall( CStifItemParser& aItem );
        virtual TInt PlayduringVoiceCall( CStifItemParser& aItem );
        virtual TInt EndPhoneCall();
        virtual TInt SetTvOutConnectedL( CStifItemParser& aItem );
        virtual TInt SetTvOutDisconnectedL();
        virtual TInt ConnectTvOutL( CStifItemParser& aItem );
        virtual TInt DisconnectTvOutL();
        virtual TInt SetDrmProtectedL( CStifItemParser& aItem );
        virtual TInt IssueSeekedToEndCommandL( CStifItemParser& aItem );
        virtual TInt HandleVolumeL( CStifItemParser& aItem );
        virtual TInt InitializeWithPositionL( CStifItemParser& aItem );
        virtual TInt InitializeLinkWithPositionL( CStifItemParser& aItem );
        virtual TInt InitializeHandleWithPositionL( CStifItemParser& aItem );
        virtual TInt InitializeStreamingWithSdpFileHandleL( CStifItemParser& aItem  );
        virtual TInt RetrieveFileNameAndModeL( CStifItemParser& aItem );

        // ---------------------------------------------------------------------
        //  Helper Functions
        // ---------------------------------------------------------------------
        TInt ReadFileInitializationParameters( CStifItemParser& aItem, TDes& aFilename );
        TInt ReadInitializationErrors( CStifItemParser& aItem );
        void CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd );
        void WriteControllerError( const TDesC& aFilename, TInt aData );
        void PreparePluginL();
        TRect ReadRect( CStifItemParser& aItem );
        TInt ParsePlaybackCommand( CStifItemParser& aItem, TInt& aCmd );
        void CreateVideoPlaybackCommandL( CStifItemParser& aItem,
                                          TMPXVideoPlaybackCommand aCmd );
        void SendPdlCustomCommandL( TMPXPlaybackPdCommand aCustomCmd );
        void CreateBackgroundCommand( TBool aBackground );
        TInt SetVolumeSteps(  TInt aVolumeSteps  );


        //----------------------------------------------------------------------
        //  MMPXPlaybackPluginObserver Implementation
        //----------------------------------------------------------------------
        virtual void HandlePluginEvent( TEvent aEvent,
                                        TInt aData,
                                        TInt aError );

        virtual void HandlePlaybackMessage( const CMPXMessage& aMsg )
        {
            HandlePlaybackMessage(const_cast<CMPXMessage*>(&aMsg), KErrNone);
        }

        virtual void HandlePlaybackMessage( CMPXMessage* aMsg,
                                            TInt aErr );

        //----------------------------------------------------------------------
        //  MMPXPlaybackEngineObserver Implementation
        //----------------------------------------------------------------------
        virtual void HandleProperty( TMPXPlaybackProperty aProperty,
                                     TInt aValue,
                                     TInt aError );

        virtual void HandleSubPlayerNames( TUid aPlayer,
                                           const MDesCArray* aSubPlayers,
                                           TBool aComplete,
                                           TInt aError );

        virtual void HandleMedia( const CMPXMedia& aMedia,
                                  TInt aError )
        {
            HandleMedia( const_cast<CMPXMedia*>(&aMedia), aError );
        }

        virtual void HandleMedia( CMPXMedia* aMedia, TInt aError );

        virtual void HandlePlaybackCommandComplete( CMPXCommand* aCommandResult,
                                                    TInt aError );


        //----------------------------------------------------------------------
        //  MTimeoutObserver Implementation
        //----------------------------------------------------------------------
        void HandleTimeout( TInt error );

        //----------------------------------------------------------------------
        //  Other Methods
        //----------------------------------------------------------------------
        void AddExpectedEvent( TCallbackEvent* event );
        void AddErrorEvent( TInt aError );
        void ProcessEvent( TCallbackEvent* aCallback );
        void ProcessErrorEvent( TInt aError );

        void CleanupTempFiles();

    private:
        CTestModuleIf&                iTestModuleIf;
        CMPXPlaybackPluginVersion2*   iPlaybackPlugin;
        CStifLogger*                  iLog;
        CSimpleTimeout*               iTimeoutController;

        CMpxVideoPlayerUtility*       iPlayerUtility;
        CDlMgrTestClass*              iDlMgrTester;
        CMPXVideoAccessoryObserver*   iAccObserver;


        CCallbackEventArray*          iExpectedCallbackArray;
        TInt                          iExpectedError;
        TBool                         iFirstPlayCmd;
        TBool                         iAllowPositionCallback;
};

#endif  //  __VHPPTESTCASE_H__

// End of File
