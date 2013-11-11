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
* Description:  videoappuiengine test class declaration.
*
*/

// TODO: at the moment we are not able to get access to all private methods
//       to the class under test. We need to declare some own framework in order
//       to declare testclasses into friends of classes under test etc.
//
//      missing test functions:
//      - how to setup construction to fail for testing purposes

// Version : %version:  %

#ifndef __TESTVIDEOAPPUIENGINE_H__
#define __TESTVIDEOAPPUIENGINE_H__


// INCLUDES
#include <QtTest/QtTest>
#include <e32base.h>
#include <e32def.h>
#include <mpxmessage2.h>

enum TActivationCommands
{
    EProcessTail,
    EProcessFile,
    EProcessStandAlone
};

enum TStifFileTypes
{
    ELocalFile,
    ESdpFile,
    ERamFile,
    EUrl,
    EAsxFile,
    EMultiLinkFile
};

enum TMpxUtilityEvents
{
    EViewUtilityPreLoadView,
    EViewUtilityActivateView,
    EViewUtilityActivateViewUid,
    EViewUtilityActivatePdlView,
    EViewUtilitySetAsDefaultView,
    EViewUtilityActivatePreviousView,
    EPlaybackUtilityInitFileHandle,
    EPlaybackUtilityInitFileName,
    EPlaybackUtilityInitStreamingUrl,
    EPlaybackUtilityInitStreamingFileHandle,
    EPlaybackUtilityInitPlaylist,
    EPlaybackUtilityStartPdl,
    EPlaybackUtilityPlayCmd,
    EPlaybackUtilityPdlInstance,
    EPlaybackUtilityClose,
    ECollectionUtilityMedia,
    ECollectionUtilityOpen,
    EAppUiCmdExit,
    EPlaybackUtilityOpenPlaylist,
    EAsxAttributeSeekUpdate,
    EFunctionLeave
};

typedef struct
{
    TMpxUtilityEvents  iEvent;
    TUid               iUid;
    TFileName          iFileName;
    TInt               iDownloadId;
    TInt               iApId;
    TInt               iExtra;
} TCallbackEvent;

typedef CArrayPtrFlat<TCallbackEvent> CCallbackEventArray;

_LIT(KTestLocalFilename, "c:\\data\\videos\\test.3gp" ); 
_LIT(KTestRamFilename, "c:\\data\\videos\\test.ram" );
_LIT(KTestRtspUrl, "rtsp://testserver/test.3gp");


class MAppUiEngineUnitTestObserver 
{
    public:
    virtual void HandleUtilityEvent( TCallbackEvent* aEvent ) = 0;
};


class VideoPlaybackWrapper;
class CMpxVideoPlayerAppUiEngine;
class MMPXPlaybackUtility;
class CMPXCollectionUtility;

class TestVideoAppUiEngine : public QObject,
                             public MAppUiEngineUnitTestObserver
{

    Q_OBJECT

    public: 

        //
        //  Constructor
        //
        TestVideoAppUiEngine();

        //
        //  Destructor
        //
        virtual ~TestVideoAppUiEngine();       
    
        //
        //  MAppUiEngineStifTestObserver Implementation
        //
        void HandleUtilityEvent( TCallbackEvent* aEvent );
    
        //
        //  CreateMpxMessageLC Implementation
        //
        CMPXMessage* CreateMpxMessageLC( int aEvent, int aType, int aData );
    
      
    private slots:

        void init();
        void cleanup();
    
        void testPlaybackUtility();
        void testOpenFile();
        void testOpenMedia();
        void testDoHandlePlaybackMessage();
        void testPrepareCloseMpx();
        void testDoHandleCollectionMedia();
        void testHandlePlaybackMessage();
        void testHandleCollectionMessage();
        void testHandleCollectionMedia();

        // called after last test case executed
        void cleanupTestCase();

    signals:

        void aboutToQuit();

    private:

        CMpxVideoPlayerAppUiEngine*       mTestObject;    
        VideoPlaybackWrapper*             mPlaybackWrapper;
        MMPXPlaybackUtility*              mPlaybackUtility;
        CMPXCollectionUtility*            mCollectionUtility;
            
        int                               mError;      
        TCallbackEvent*                   mExpectedEvent;      
        bool                              mTestResult;      

};

#endif  // __TESTVIDEOAPPUIENGINE_H__

// End of file



