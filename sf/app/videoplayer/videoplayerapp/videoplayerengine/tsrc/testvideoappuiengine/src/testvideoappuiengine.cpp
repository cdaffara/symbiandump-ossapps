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
* Description:  VideoAppUiEngine test class implementation.
*
*/

// Version : %version:  3 %

// INCLUDES
#include <QtTest/QtTest>
#include <qdebug.h>

#include <hbapplication.h>
#include <f32file.h>
#include <bautils.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>

#include "stub/inc/testutilities.h"

#include "mpxplaybackutilityimp_stub.h"
#include "mpxcollectionutilityimp_stub.h"

#include "stub/inc/hbinstance.h"
#include "mpxhbvideocommondefs.h"
#include "testvideoappuiengine.h"
#include "../stub/inc/videoplaybackwrapper.h"
#define private public
#include "mpxvideoplayerappuiengine.h"
#undef private
#include "mpxvideo_debug.h"




// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestVideoAppUiEngine tc;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoAppUiEngine.txt";

    return QTest::qExec(&tc, 3, pass);
}


TestVideoAppUiEngine::TestVideoAppUiEngine()
    : mTestObject( 0 )
{          
    MPX_DEBUG(_L("TestVideoAppUiEngine::TestVideoAppUiEngine()"));  
}

TestVideoAppUiEngine::~TestVideoAppUiEngine()
{    
    MPX_ENTER_EXIT(_L("TestVideoAppUiEngine::~TestVideoAppUiEngine()"));
    
    delete mTestObject;
    mTestObject = 0;
    
    delete mPlaybackWrapper;
    mPlaybackWrapper = 0;        
    
    TestUtilities* utility = reinterpret_cast<TestUtilities*>( UserSvr::DllTls(12345678) ); 
    utility->iPlaybackUtility->RemoveTestObserverL();
    
}


void TestVideoAppUiEngine::init()
{ 
    MPX_ENTER_EXIT(_L("TestVideoAppUiEngine::init()"));
    
    mTestResult = false;    
        
    mPlaybackWrapper = new VideoPlaybackWrapper();
        
    TRAPD( createErr, mTestObject = CMpxVideoPlayerAppUiEngine::NewL( mPlaybackWrapper ) );
    
    QVERIFY( ! createErr );
    
    QVERIFY( mTestObject->iCollectionUtility );
    
    TRAPD( initErr, mTestObject->LateInitL() );
    
    QVERIFY( ! initErr );       
    
    TestUtilities* utility = reinterpret_cast<TestUtilities*>( UserSvr::DllTls(12345678) );   
    
    TRAP_IGNORE( utility->iPlaybackUtility->AddTestObserverL( this ) );    
}

void TestVideoAppUiEngine::cleanup()
{
    MPX_ENTER_EXIT(_L("TestVideoAppUiEngine::cleanup()"));
    
    delete mTestObject;
    mTestObject = 0;
    
    delete mPlaybackWrapper;
    mPlaybackWrapper = 0;
    
    TestUtilities* utility = reinterpret_cast<TestUtilities*>( UserSvr::DllTls(12345678) ); 

    if ( utility )
    {
        utility->iPlaybackUtility->RemoveTestObserverL();
    }
    
}

void TestVideoAppUiEngine::cleanupTestCase()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::cleanupTestCase()"));  
    // all common cleanup here
}

void TestVideoAppUiEngine::testPlaybackUtility()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testPlaybackUtility()"));  
    
    init();
        
    QVERIFY(  mTestObject->PlaybackUtility() );
        
}
		
void TestVideoAppUiEngine::testOpenFile()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testOpenFile()")); 
    
    // Test for local file
    init();    
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitFileName;                    
    TRAPD( openFileErr, mTestObject->OpenFileL( KTestLocalFilename ) ); 
    QVERIFY( ! openFileErr );
    QVERIFY( mTestResult );
    
    // Test for streaming link  
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitStreamingUrl;                    
    TRAPD( openRamFileErr, mTestObject->OpenFileL( KTestRamFilename ) );  
    QVERIFY( ! openRamFileErr );
    QVERIFY( mTestResult );  
    
    // Test for a url
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitStreamingUrl;                    
    TRAPD( openUrlErr, mTestObject->OpenFileL( KTestRtspUrl ) );  
    QVERIFY( ! openUrlErr );
    QVERIFY( mTestResult );     
        
}

void TestVideoAppUiEngine::testOpenMedia()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testOpenMedia()")); 
    
    init(); 
        
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTextValueL( KMPXMediaGeneralUri, KTestLocalFilename );
    media->SetTObjectValueL( KMPXMediaGeneralId, TMPXItemId( 1, 2 ) );
    
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitPlaylist;
    
    TRAPD( err, mTestObject->OpenMediaL( *media ) ); 
    QVERIFY( ! err );
    QVERIFY( mTestResult );        
  
}

void TestVideoAppUiEngine::testDoHandlePlaybackMessage()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testDoHandlePlaybackMessage()")); 
    
    init();    
       
    QSignalSpy spy( mPlaybackWrapper, SIGNAL(handlePlaybackView(int)) );      
    QCOMPARE( spy.count(), 0 );     
        
    CMPXMessage* msg = CreateMpxMessageLC(TMPXPlaybackMessage::EPlayerChanged, 0, 0);    
    TRAPD( err, mTestObject->DoHandlePlaybackMessageL( *msg ));
    QVERIFY( ! err );
    
    QCOMPARE( spy.count(), 1 );
    spy.clear();        
    
}

void TestVideoAppUiEngine::testPrepareCloseMpx()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testPrepareCloseMpx()")); 
    
    init();
    
    TRAPD( err, mTestObject->PrepareCloseMpxL() ); 
    
    QVERIFY( ! err );
        
}

void TestVideoAppUiEngine::testDoHandleCollectionMedia()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testDoHandleCollectionMedia()")); 
    
    init(); 
        
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTextValueL( KMPXMediaGeneralUri, KTestLocalFilename );
    media->SetTObjectValueL( KMPXMediaGeneralId, TMPXItemId( 1, 2 ) );
    
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitPlaylist;
    
    TRAPD( err, mTestObject->DoHandelCollectionMediaL( *media ) ); 
    QVERIFY( ! err );
    QVERIFY( mTestResult ); 
    
}

void TestVideoAppUiEngine::testHandlePlaybackMessage()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testHandlePlaybackMessage()")); 
    
    init();    
       
    QSignalSpy spy( mPlaybackWrapper, SIGNAL(handlePlaybackView(int)) );      
    QCOMPARE( spy.count(), 0 );    
    
    mTestObject->iUpdateSeekInfo = ETrue;
        
    CMPXMessage* msg = CreateMpxMessageLC(TMPXPlaybackMessage::EPlayerChanged, 0, 0);    
    mTestObject->HandlePlaybackMessage( msg, KErrNone );    
    
    QCOMPARE( spy.count(), 1 );
    spy.clear();     
    
    QVERIFY( ! mTestObject->iUpdateSeekInfo );
}

void TestVideoAppUiEngine::testHandleCollectionMessage()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testHandleCollectionMessage()")); 
    
    init();    
    
    // Test for EMcItemOpened
    //
    CMPXMessage* msg = CreateMpxMessageLC(4, EMcPathChangedByOpen, EMcItemOpened);  // EPathChanged => 4  
    mTestObject->HandleCollectionMessage( msg, KErrNone );    
    QVERIFY( mTestObject->iCollectionUtility );    
    
    // Test for EMcContainerOpened
    //
    mTestObject->iMultilinkPlaylist = ETrue;
    
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitPlaylist;
    
    delete msg;
    msg = NULL;
    msg = CreateMpxMessageLC( 4, EMcPathChangedByOpen, EMcContainerOpened );
    mTestObject->HandleCollectionMessage( msg, KErrNone ); 
    
    QVERIFY( mTestResult );      
    
}

void TestVideoAppUiEngine::testHandleCollectionMedia()
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::testHandleCollectionMedia()")); 
    
    init(); 
        
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTextValueL( KMPXMediaGeneralUri, KTestLocalFilename );
    media->SetTObjectValueL( KMPXMediaGeneralId, TMPXItemId( 1, 2 ) );
    
    mExpectedEvent = new (ELeave) TCallbackEvent;
    mExpectedEvent->iEvent = EPlaybackUtilityInitPlaylist;
    
    TRAPD( err, mTestObject->HandleCollectionMediaL( *media, KErrNone ) ); 
    QVERIFY( ! err );
    QVERIFY( mTestResult );     
    
}

void TestVideoAppUiEngine::HandleUtilityEvent( TCallbackEvent* aEvent )
{
    MPX_DEBUG(_L("TestVideoAppUiEngine::HandleUtilityEvent()")); 
    
    if ( mExpectedEvent->iEvent == aEvent->iEvent )
    {
        switch ( aEvent->iEvent )
        {
            case EPlaybackUtilityInitFileName:
            {
                if ( aEvent->iFileName ==  KTestLocalFilename )
                {
                    mTestResult = true;
                }                
                break;
            }
            case EPlaybackUtilityInitStreamingUrl:
            {
                if ( aEvent->iFileName ==  KTestRamFilename )    
                {
                    mTestResult = true;
                }
                break;
            }
            case EPlaybackUtilityInitPlaylist:
            {
                mTestResult = true;   
                break;
            }
            case EAsxAttributeSeekUpdate:
            {
                mTestResult = true;
                break;
            }
            
        }
    
    }
    
    delete aEvent;    
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::CreateMpxMessageLC
// -------------------------------------------------------------------------------------------------
//
CMPXMessage*
TestVideoAppUiEngine::CreateMpxMessageLC( int aEvent, int aType, int aData )
{
    MPX_ENTER_EXIT(_L("TestVideoAppUiEngine::CreateMpxMessageLC()"));
    
    CMPXMessage* pbMessage = CMPXMessage::NewL();
    CleanupStack::PushL( pbMessage );

    pbMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
    pbMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, aEvent );
    pbMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, aType );
    pbMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, aData );

    return pbMessage;
}


// End of file



