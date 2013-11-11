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
* Description: Unit test for mpmpxpbframeworkwrapper
*
*/

#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>
#include <xqsharablefile.h>

#include "mpxaudioeffectengine.h"
#include "unittest_mpmpxplaybackframeworkwrapper.h"
#include "stub/inc/mpplaybackdata.h"
#include "stub/inc/mpxplaybackutility.h"
#include "stub/inc/mpsongdata.h"
#include "stub/inc/mpmpxembeddedplaybackhelper.h"

// Do this so we can access all member variables.
#define private public
#include "mpmpxplaybackframeworkwrapper.h"
#include "mpmpxplaybackframeworkwrapper_p.h"
#undef private

// This so we can test private functions
#include "mpmpxplaybackframeworkwrapper_p.cpp"

// Test data
struct TTestAttrs
    {
    const wchar_t* GeneralTitle;
    const wchar_t* MusicArtist;
    const TInt GeneralCount;
    const wchar_t* MusicAlbumArtFileName;
    const wchar_t* MusicAlbum;
    const wchar_t* MusicUri;
    const wchar_t* MimeType;
    };

const TTestAttrs KAllSongsTestData[] =
    {
     { L"Title 1", L"Artist 1", 1, L"e:\\data\\images\\art1.jpg", L"Album 1", L"Uri 1", L"audio/x-pn-realaudio"}
    ,{ L"Title 2", L"Artist 2", 1, L"e:\\data\\images\\art2.jpg", L"Album 2", L"Uri 2", L"audio/x-realaudio"}
    };

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpMpxPlaybackFrameworkWrapper tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmpxplaybackframeworkwrapper.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMpxPlaybackFrameworkWrapper::TestMpMpxPlaybackFrameworkWrapper()
    : mTest(0),
      iMediaTestData(0),
      mSongData(0)
{
}

TestMpMpxPlaybackFrameworkWrapper::~TestMpMpxPlaybackFrameworkWrapper()
{
    delete mTest;
    delete iMediaTestData;
    delete mSongData;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxPlaybackFrameworkWrapper::initTestCase()
{
    mSongData = new MpSongData();
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxPlaybackFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxPlaybackFrameworkWrapper::init()
{
    mTest = new MpMpxPlaybackFrameworkWrapper(MpCommon::KMusicPlayerUid, mSongData);
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxPlaybackFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpMpxPlaybackFrameworkWrapper::testConstructor()
{
    QVERIFY(mTestPrivate->iPlaybackUtility != 0);
    QVERIFY(mTestPrivate->iPlaybackData != 0);
}

/*!
 Tests playbackData
 */
void TestMpMpxPlaybackFrameworkWrapper::testPlaybackData()
{
    MpPlaybackData *playbackdata;
    playbackdata = mTest->playbackData();
    QCOMPARE(mTestPrivate->iPlaybackData, playbackdata);
}

/*!
 Tests stop.
 */
void TestMpMpxPlaybackFrameworkWrapper::testStop()
{
    mTest->stop();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdStop);
}

/*!
 Tests playPause.
 */
void TestMpMpxPlaybackFrameworkWrapper::testPlayPause()
{
    mTest->playPause();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdPlayPause);
}

/*!
 Tests skipForward.
 */
void TestMpMpxPlaybackFrameworkWrapper::testSkipForward()
{
    mTest->skipForward();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdNext);
}

/*!
 Tests skipBackward
 */
void TestMpMpxPlaybackFrameworkWrapper::testSkipBackward()
{
    mTest->skipBackward();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdPrevious);
}

/*!
 Tests setPosition
 */
void TestMpMpxPlaybackFrameworkWrapper::testSetPosition()
{
    mTest->setPosition(100);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, 100);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyPosition);
    mTest->setPosition(50);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, 50);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyPosition);
    mTest->setPosition(700);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, 700);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyPosition);
}

/*!
 Tests setShuffle
 */
void TestMpMpxPlaybackFrameworkWrapper::testSetShuffle()
{
    mTest->setShuffle(true);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRandomMode);
    mTest->setShuffle(false);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, TBool(EFalse));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRandomMode);
}

/*!
 Tests setRepeat
 */
void TestMpMpxPlaybackFrameworkWrapper::testSetRepeat()
{
    mTest->setRepeat(true);
    QCOMPARE(TMPXPlaybackRepeatMode(mTestPrivate->iPlaybackUtility->iValue), EPbRepeatAll);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRepeatMode);
    mTest->setRepeat(false);
    QCOMPARE(TMPXPlaybackRepeatMode(mTestPrivate->iPlaybackUtility->iValue), EPbRepeatOff);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRepeatMode);
}

/*!
 Tests handleProperty
 */
void TestMpMpxPlaybackFrameworkWrapper::testHandlePlaybackMessage()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(testMessage);
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral);

    //Update State
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EStateChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,0);
    mTestPrivate->iPlaybackUtility->iState = EPbStatePlaying;
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Playing);

    //Initialize complete
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EInitializeComplete);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,1);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandGeneralId),KMPXCommandIdPlaybackGeneral);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType),TInt(EPbCmdPlay));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandGeneralDoSync),TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandPlaybackGeneralNoBuffer),TBool(ETrue));

    //Property change
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EPropertyChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EPbPropertyPosition);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,50);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mPosition, 50);

    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EPbPropertyDuration);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,100);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mDuration, 100);

    //Media change
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EMediaChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,0);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iAttrs->Count(), 8);

    CleanupStack::PopAndDestroy(testMessage);
}

/*!
 Tests handlePlaybackError
 */
void TestMpMpxPlaybackFrameworkWrapper::testHandlePlaybackError()
{
    mTestPrivate->iPlaybackUtility->iReturnSource = true;
    mTestPrivate->DoHandlePlaybackErrorL(-20);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdNext);
    QCOMPARE(mTestPrivate->iPlaybackData->mCorrupted, true);
    QSignalSpy spy(mTest, SIGNAL(corruptedStop()));
    mTestPrivate->iPlaybackData->mCorrupted = false;
    mTestPrivate->iPlaybackUtility->iSetPlaylistEnd = true;
    mTestPrivate->DoHandlePlaybackErrorL(-20);
    QCOMPARE(mTestPrivate->iPlaybackData->mCorrupted, true);
    QCOMPARE(spy.count(), 1);    
}

/*!
 Tests handleProperty
 */
void TestMpMpxPlaybackFrameworkWrapper::testHandleProperty()
{
    for(TInt i = 0; i < 10; i++){
        mTestPrivate->HandlePropertyL(EPbPropertyPosition, i, KErrNone);
        mTestPrivate->HandlePropertyL(EPbPropertyDuration, i+1, KErrNone);
        QCOMPARE(mTestPrivate->iPlaybackData->mPosition, i);
        QCOMPARE(mTestPrivate->iPlaybackData->mDuration, i+1);
    }
    mTestPrivate->iPlaybackData->mPosition = 100;
    mTestPrivate->iPlaybackData->mDuration = 100;
    mTestPrivate->HandlePropertyL(EPbPropertyPosition, 10, KErrNotFound);
    mTestPrivate->HandlePropertyL(EPbPropertyDuration, 10, KErrNotFound);

    QCOMPARE(mTestPrivate->iPlaybackData->mPosition, 100);
    QCOMPARE(mTestPrivate->iPlaybackData->mDuration, 100);
}

/*!
 Tests handleMedia
 */
void TestMpMpxPlaybackFrameworkWrapper::testHandleMedia()
{
    loadTestData(0);
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(ETrue));
    mTestPrivate->iPlaybackData->mMediaReady = EFalse;
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(EFalse));

    loadTestData(1);
    mTestPrivate->iPlaybackData->mMediaReady = EFalse;
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(ETrue));
    const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].GeneralTitle));
    QCOMPARE(mTestPrivate->iPlaybackData->title(), QString::fromUtf16( title.Ptr(), title.Length() ));
    const TDesC& artist = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicArtist));
    QCOMPARE(mTestPrivate->iPlaybackData->artist(), QString::fromUtf16( artist.Ptr(), artist.Length() ));
    const TDesC& album = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicAlbum));
    QCOMPARE(mTestPrivate->iPlaybackData->album(), QString::fromUtf16( album.Ptr(), album.Length() ));
    const TDesC& albumArt = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicAlbumArtFileName));
    QCOMPARE(mTestPrivate->iPlaybackData->currentAAUri, QString::fromUtf16( albumArt.Ptr(), albumArt.Length() ));
    const TDesC& albumUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicUri));
    QCOMPARE(mTestPrivate->iPlaybackData->currentUri, QString::fromUtf16( albumUri.Ptr(), albumUri.Length() ));
    QVERIFY(mTestPrivate->iPlaybackData->mRealAudio);
    QCOMPARE(mTestPrivate->iPlaybackData->mId, 1);
    QCOMPARE(mTestPrivate->iPlaybackData->mAlbumId, 2);

    //Empty data
    cleanup();
    init();
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[1].GeneralCount);
    entries->SetTextValueL(KMPXMediaGeneralUri, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicUri)));
    CMPXMedia* emptyTestData = CMPXMedia::NewL(*entries);
    CleanupStack::PopAndDestroy(entries);
    CleanupStack::PushL(emptyTestData);

    mTestPrivate->iPlaybackData->mMediaReady = EFalse;
    mTestPrivate->HandleMediaL(*emptyTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(ETrue));
    const TDesC& musicUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicUri));
    QCOMPARE(mTestPrivate->iPlaybackData->title(), QString::fromUtf16( musicUri.Ptr(), musicUri.Length() ));
    QCOMPARE(mTestPrivate->iPlaybackData->artist(), QString());
    QCOMPARE(mTestPrivate->iPlaybackData->album(), QString());
    QCOMPARE(mTestPrivate->iPlaybackData->currentAAUri, QString::fromUtf16( musicUri.Ptr(), musicUri.Length() ));
    QCOMPARE(mTestPrivate->iPlaybackData->currentUri, QString::fromUtf16( musicUri.Ptr(), musicUri.Length() ));
    QVERIFY(!mTestPrivate->iPlaybackData->mRealAudio);

    CleanupStack::PopAndDestroy(emptyTestData);
}

/*!
 Tests handleMedia resulting from request from details
 */
void TestMpMpxPlaybackFrameworkWrapper::testHandleMediaDetails()
{
    mTestPrivate->iDetailsRequest = true;
    loadTestData(0);
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iSongData->iSetMedia, true);
}

/*!
 Tests retrieveSong
 */
void TestMpMpxPlaybackFrameworkWrapper::testRetrieveSongDetails()
{
    // Internal requests
    mTestPrivate->DoRetrieveSongDetailsL(false);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iAttrs->Count(), 8);

    // Request from Details view
    mTestPrivate->iDetailsRequest = false;
    mTest->retrieveSongDetails();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iAttrs->Count(), 18);
    QCOMPARE(mTestPrivate->iDetailsRequest, true);

    // Request from Details view - no source
    mTestPrivate->iDetailsRequest = false;
    mTestPrivate->iPlaybackUtility->iReturnSource = false;
    mTest->retrieveSongDetails();
    QCOMPARE(mTestPrivate->iDetailsRequest, false);
}

/*!
 Tests updateState
 */
void TestMpMpxPlaybackFrameworkWrapper::testUpdateState()
{
    mTestPrivate->iPlaybackUtility->iReturnSource = false;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::NotPlaying);
    mTestPrivate->iPlaybackUtility->iReturnSource = true;

    mTestPrivate->iPlaybackUtility->iState = EPbStatePlaying;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Playing);

    mTestPrivate->iPlaybackUtility->iState = EPbStatePaused;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Paused);

    mTestPrivate->iPlaybackUtility->iState = EPbStatePlaying;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Playing);

    mTestPrivate->iPlaybackUtility->iState = EPbStateStopped;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Stopped);
}

/*!
 Tests forceStop
 */
void TestMpMpxPlaybackFrameworkWrapper::testForceStop()
{
    mTestPrivate->ForceStopL();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandGeneralId),KMPXCommandIdPlaybackGeneral);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType),TInt(EPbCmdStop));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandGeneralDoSync),TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandPlaybackGeneralNoBuffer),TBool(ETrue));
}

/*!
 Tests setBalance
 */
void TestMpMpxPlaybackFrameworkWrapper::testSetBalance()
{
    int balance = 25;
    mTest->setBalance( balance );
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, balance);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyBalance);

    balance = 75;
    mTest->setBalance( balance );
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, balance);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyBalance);
}

/*!
 Tests applyAudioEffects
 */
void TestMpMpxPlaybackFrameworkWrapper::testApplyAudioEffects()
{
    mTest->applyAudioEffects();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iComandData, TInt(KAudioEffectsID));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbApplyEffect);
}

/*!
 Tests applyEqualizer
 */
void TestMpMpxPlaybackFrameworkWrapper::testApplyEqualizer()
{
    mTest->applyEqualizer();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iComandData, TInt(KEqualizerID));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbApplyEffect);
}

/*!
 Tests applyEqualizer
 */
void TestMpMpxPlaybackFrameworkWrapper::testPlay()
{
    //Play from filename
    QString fileName = QString("z:\\system\\data\\nullsound.mp3");
    mTest->play( fileName );
    QCOMPARE( mTestPrivate->iEmbeddedPlaybackHelper->iFileNameToPlay, fileName );

    //Play from Shareable file
    mTestPrivate->iPlaybackUtility->iInitialized = false;
    _LIT( KMPXNullSoundPath, "z:\\system\\data\\nullsound.mp3" );
    RFs fileServerSession;
    RFile file;
    TInt err( KErrNone );
    err = fileServerSession.Connect();
    if( err == KErrNone ) {
        err = file.Open( fileServerSession, KMPXNullSoundPath, EFileShareReadersOrWriters );
        if( err == KErrNone ) {
            XQSharableFile* sFile = new XQSharableFile(file);

            mTest->play( *sFile );
            QCOMPARE( mTestPrivate->iEmbeddedPlaybackHelper->iFileNameToPlay, sFile->fileName() );

            sFile->close();
            delete sFile;
            sFile = 0;
            fileServerSession.Close();
        }
        else {
            QWARN("Not able to Open test file");
        }
    }
    else {
        QWARN("Not able to create RF Session");
    }
    
    //Play empty filename
    QSignalSpy spy(mTest, SIGNAL(corruptedStop( bool )));
    QList<QVariant> arguments;
    QString emptyFileName = "";
    mTest->play( emptyFileName );
    QVERIFY( spy.count() == 1 );
    arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == true );

    //Play command
    cleanup();
    init();
    mTest->play();
    QCOMPARE( mTestPrivate->iPlaybackUtility->iCmd, EPbCmdPlay);
}

/*!
 Tests startSeekForward()
 */
void TestMpMpxPlaybackFrameworkWrapper::testStartSeekForward()
{
    mTest->startSeekForward();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdStartSeekForward);
}

/*!
 Tests stopSeeking()
 */
void TestMpMpxPlaybackFrameworkWrapper::testStopSeeking()
{
    mTest->stopSeeking();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdStopSeeking);
}

/*!
 Tests startSeekBackward()
 */
void TestMpMpxPlaybackFrameworkWrapper::testStartSeekBackward()
{
    mTest->startSeekBackward();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdStartSeekBackward);
}

/*!
 Tests HandleSubPlayerNamesL()
 */
void TestMpMpxPlaybackFrameworkWrapper::testHandleSubPlayerNamesL()
{
    //HandleSubPlayerNamesL does nothing, included just to cover all declared functions.
    TUid stubUid = {0x00000001};
    MDesCArray* stubArray = NULL;
    mTestPrivate->HandleSubPlayerNamesL(stubUid, stubArray, false, KErrNone);
    QVERIFY(mTestPrivate);
}

/*!
 Tests closeCurrentPlayback() / DoCloseCurrentPlaybackL()
 */
void TestMpMpxPlaybackFrameworkWrapper::testCloseCurrentPlayback()
{
    TMPXPlaybackCommand dummyCmd = EPbCmdPlay;
    mTestPrivate->iPlaybackUtility->iCmd = dummyCmd;

    //Simulate none file opened.
    mTestPrivate->iPlaybackUtility->iReturnSource = false;
    mTest->closeCurrentPlayback();
    QVERIFY(mTestPrivate->iPlaybackUtility->iCmd != EPbCmdClose);

    //Simulate some file already opened.
    mTestPrivate->iPlaybackUtility->iReturnSource = true;
    mTest->closeCurrentPlayback();
    QVERIFY(mTestPrivate->iPlaybackUtility->iCmd == EPbCmdClose);
}

/*!
 Tests pause.
 */
void TestMpMpxPlaybackFrameworkWrapper::testPause()
{
    mTest->pause();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdPause);
}

/*!
 Tests getMaxVolume.
 */
void TestMpMpxPlaybackFrameworkWrapper::testGetMaxVolume()
{
    mTest->getMaxVolume();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyMaxVolume);
}

/*!
 Tests getVolume.
 */
void TestMpMpxPlaybackFrameworkWrapper::testGetVolume()
{
    mTest->getVolume();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyVolume);
}

/*!
 Tests increaseVolume.
 */
void TestMpMpxPlaybackFrameworkWrapper::testIncreaseVolume()
{
    mTest->increaseVolume();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdIncreaseVolume);
}

/*!
 Tests decreaseVolume.
 */
void TestMpMpxPlaybackFrameworkWrapper::testDecreaseVolume()
{
    mTest->decreaseVolume();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdDecreaseVolume);
}

/*!
 Tests setVolume.
 */
void TestMpMpxPlaybackFrameworkWrapper::testSetVolume()
{
    int value = 30;
    mTest->setVolume( value );
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdSetVolume);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iComandData, value);
}

/*!
 Tests getMuteState.
 */
void TestMpMpxPlaybackFrameworkWrapper::testGetMuteState()
{
    mTest->getMuteState();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyMute);
}

/*!
 Tests mute.
 */
void TestMpMpxPlaybackFrameworkWrapper::testMute()
{
    mTest->mute();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdMuteVolume);
}

/*!
 Tests unmute.
 */
void TestMpMpxPlaybackFrameworkWrapper::testUnmute()
{
    mTest->unmute();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdUnMuteVolume);
}

/*!
 Used to load test data
 */
void TestMpMpxPlaybackFrameworkWrapper::loadTestData(TInt aPos)
{
    if(iMediaTestData){
        delete iMediaTestData;
        iMediaTestData=0;
    }
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralId, 1);
    entries->SetTObjectValueL<TInt>(KMPXMediaMusicAlbumId, 2);
    entries->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].GeneralTitle)));
    entries->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicArtist)));
    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[aPos].GeneralCount);
    entries->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicAlbumArtFileName)));
    entries->SetTextValueL(KMPXMediaMusicAlbum, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicAlbum)));
    entries->SetTextValueL(KMPXMediaGeneralUri, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicUri)));
    entries->SetTextValueL(TMPXAttribute(KMPXMediaGeneralMimeType), TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MimeType)));

    iMediaTestData = CMPXMedia::NewL(*entries);
    CleanupStack::PopAndDestroy(entries);
}

//end of file
