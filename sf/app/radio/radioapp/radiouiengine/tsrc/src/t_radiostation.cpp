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
* Description: radiostation test implementation
*
*/

#include <e32base.h>
#include <eikenv.h>
#include "t_radiostation.h"
#include "radiopresetstorage.h"
#include "radiostation.h"
#include "radiostationmodel.h"
#include "radiohistorymodel.h"
#include "radioenginewrapper.h"
#include "radiouiengine.h"
#include "radioengineutils.h"
#include "radiologger.h" //Radio::connect
#include "trace.h"

// Constants
const uint KTestFrequency1 = 89000000;
const uint KTestFrequency2 = 89500000;
const uint KTestFrequency3 = 90000000;
const uint KTestFrequency4 = 90500000;
const uint KTestFrequency5 = 91000000;
const uint KTestFrequency6 = 91500000;
const uint KTestGenre1 = 1;
const uint KTestGenre2 = 2;
const QString KTestStationName1 = "Radio Noice";
const QString KTestStationName2 = "RN RENAMED";
const QString KTestUrl1 = "http://qt.nokia.com";
const QString KTestUrl2 = "http://www.radionoice.com";
const QString KTestFrequencyString1 = "89000000";
const QString KTestFrequencyString2 = "89500000";
const QString KTestFrequencyString3 = "90000000";
const QString KTestFrequencyString6 = "91500000";
const QString KTestArtist1 = "Eläkeläiset";
const QString KTestArtist2 = "Jope";
const QString KTestArtist3 = "Motorhead";
const QString KTestTitle1 = "Humppa^2";
const QString KTestTitle2 = "´åäö´ ^&%¤^";
const QString KTestTitle3 = "Dancing queen";
const QString KTestRadioTextRadioText = "NOW! Metallica - Enter sandman in MusicStore for free";
const QString KTestRadioTextPlusArtist = "Metallica";
const QString KTestRadioTextPlusTitle = "Enter sandman";
const QString KTestRadioTextPlusUrl = "www.metallica.com";
const QString KTestRadioTextPlusUnsupportedTag = "*#*#*#";
const QString KTestDynamicPSText = "MAKKARAA";

/*!
 *
 */
int main(int /* argc*/, char *argv[])
{
    FUNC_LOG;
    TestRadioUiEngine* tv = new TestRadioUiEngine();

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiouiengine.txt";

    int res = QTest::qExec(tv, 3, pass);

    INFO_1( "Main, result value %i", res );
    delete tv;
    return res;
}


TestRadioUiEngine::TestRadioUiEngine()
{
    FUNC_LOG;
}


void TestRadioUiEngine::tunedToFrequency( uint /* frequency */, int /* commandSender */)
{
    FUNC_LOG;
}


void TestRadioUiEngine::seekingStarted( Seek::Direction /* direction */)
{
    FUNC_LOG;
}
    
void TestRadioUiEngine::radioStatusChanged( bool /* radioIsOn */)
{
    FUNC_LOG;
}
    
void TestRadioUiEngine::rdsAvailabilityChanged( bool /* available */)
{
    FUNC_LOG;
}
    
void TestRadioUiEngine::volumeChanged( int /* volume */)
{
    FUNC_LOG;
}
    
void TestRadioUiEngine::muteChanged( bool /* muted */)
{
    FUNC_LOG;
}

void TestRadioUiEngine::audioRouteChanged( bool /* loudspeaker */)
{
    FUNC_LOG;
}

void TestRadioUiEngine::scanAndSaveFinished()
{
    FUNC_LOG;
}

    
void TestRadioUiEngine::headsetStatusChanged( bool /* connected */)
{
    FUNC_LOG;
}
        
void TestRadioUiEngine::skipPrevious()
{
    FUNC_LOG;
}
    
void TestRadioUiEngine::skipNext()
{
    FUNC_LOG;
}

/*!
 * Destructor
 */
TestRadioUiEngine::~TestRadioUiEngine()
{
    FUNC_LOG;
    mEngineWrapper.reset( NULL );
    mPresetStorage.reset( NULL );
    delete mSchedulerTimer;
    delete mScheduler;
    RadioEngineUtils::Release();
}

/*!
 * called before each testfunction is executed
 */
void TestRadioUiEngine::init()
{
    FUNC_LOG;
}

/*!
 * called after every testfunction
 */
void TestRadioUiEngine::cleanup()
{
    FUNC_LOG;
}

/*!
 * called before the first testfunction is executed
 */
void TestRadioUiEngine::initTestCase()
{
    FUNC_LOG;
    // Workaround for the below panic, occured after porting to 10.1 
    // Main Panic E32USER-CBase 44
    // Create and install the active scheduler
    mScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install( mScheduler );
    TInt err( KErrNone);
    TRAP( err, mSchedulerTimer = CSchedulerStopAndStartTimer::NewL( *this ) );
    QVERIFY2(  KErrNone == err, "CSchedulerStopAndStartTimer not constructed!" );     
    mSchedulerTimer->StartTimer( 1000000, CSchedulerStopAndStartTimer::ETimerIdCreateMUT );
}

/*!
 * called after the last testfunction was executed
 */
void TestRadioUiEngine::cleanupTestCase()
{
    FUNC_LOG;
    Radio::disconnect( &mUiEngine->stationModel(),  SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
        this,    SLOT(dataChanged(const QModelIndex, const QModelIndex)) );
    
    Radio::disconnect( &mUiEngine->stationModel(),           SIGNAL(stationDataChanged(RadioStation)),
        this,    SLOT(stationDataChanged(RadioStation)) );
    
    Radio::disconnect( &mUiEngine->stationModel(),           SIGNAL(favoriteChanged(RadioStation)),
        this,    SLOT(favoriteChanged(RadioStation)) );
    
    Radio::disconnect( &mUiEngine->historyModel(),           SIGNAL(itemAdded()),
        this,    SLOT(itemAdded()) );
    mSchedulerTimer->StartTimer( 1000000, CSchedulerStopAndStartTimer::ETimerIdDeleteMUT );
}

/*!
 * 
 */
void TestRadioUiEngine::dataChanged(const QModelIndex /* topLeft */, const QModelIndex /* bottomRight */)
{
    FUNC_LOG;
	mEnteredSlots |= DataChanged;
}

/*!
 * 
 */
void TestRadioUiEngine::stationDataChanged( RadioStation /* addedStation */ )
{
    FUNC_LOG;
	mEnteredSlots |= StationDataChanged;	
}

/*!
 * 
 */
void TestRadioUiEngine::favoriteChanged( RadioStation /* addedStation */)
{
    FUNC_LOG;
	mEnteredSlots |= FavoriteChanged;
}

/*!
 * 
 */
void TestRadioUiEngine::itemAdded()
{
    FUNC_LOG;
    mEnteredSlots |= ItemAdded;
}


/*!
 * Testing of implicit sharing a.k.a. copy-on-write
 */
void TestRadioUiEngine::testImplicitSharing()
{	
    FUNC_LOG;
	RadioStation t_RadioStation_1;
	
	int originalPresetIndex = t_RadioStation_1.presetIndex();
	// before any values assigned into any data field
	QVERIFY2(( originalPresetIndex == RadioStation::SharedNull ), "API:Radiostation init failed 1");
	t_RadioStation_1.setName("Noice");
	originalPresetIndex = t_RadioStation_1.presetIndex();
	// once some value assigned into some other data field
	QVERIFY2(( originalPresetIndex == RadioStation::Invalid ), "API:Radiostation init failed 2");
		
	t_RadioStation_1.setFrequency( KTestFrequency1 );
	//const int newIndex = mModel->findUnusedPresetIndex();
	t_RadioStation_1.setPresetIndex( 2 );
	t_RadioStation_1.setGenre(1);
	t_RadioStation_1.setUrl("http://qt.nokia.com");			
	
	RadioStation* t_RadioStation_2 = new RadioStation(t_RadioStation_1);
	// test that changing the content of copied data doesn't cause
	// detach in the copying data structure
	uint originalFrequency = t_RadioStation_1.frequency();
    t_RadioStation_1.setFrequency( originalFrequency + 1 );
    // should be detached
	bool detached = t_RadioStation_2->isDetached();
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ freq 1");
	
	// test that changing the content of data sets detach true
	originalFrequency = t_RadioStation_2->frequency();
	t_RadioStation_2->setFrequency( originalFrequency + 1 );
	// should be detached
	detached = t_RadioStation_2->isDetached();	
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ freq 2");	
	delete t_RadioStation_2;
	t_RadioStation_2 = NULL;
	
	//-------------------------------------------------------------------------
	// test that changing the content of copied data doesn't cause
	// detach in the copying data structure
	t_RadioStation_2 = new RadioStation(t_RadioStation_1);
	// should not be detached
	detached = t_RadioStation_2->isDetached();
	QVERIFY2(!detached, "API:Radiostation Implicit sharing/ preset index 1");
	originalPresetIndex = t_RadioStation_1.presetIndex();
	t_RadioStation_1.setPresetIndex( originalPresetIndex + 1 );
	// should be detached
	detached = t_RadioStation_2->isDetached();
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ preset index 2");
	
	// test that changing the content of data sets detach true
	originalPresetIndex = t_RadioStation_2->presetIndex();
	//newIndex = mModel->findUnusedPresetIndex();
	t_RadioStation_2->setPresetIndex( originalPresetIndex + 1 );
	// should be detached
	detached = t_RadioStation_2->isDetached();
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ preset index 3");	
    delete t_RadioStation_2;
    t_RadioStation_2 = NULL;
    
    // test that changing the content of copied data doesn't cause
    // detach in the copying data structure
    t_RadioStation_2 = new RadioStation(t_RadioStation_1);
    // should not be detached
    detached = t_RadioStation_2->isDetached();
    QVERIFY2(!detached, "API:Radiostation Implicit sharing/ name 1");
    //QString originalName = t_RadioStation_1.name(); 
    t_RadioStation_1.setName("RadioOne");
    // should be detached
    detached = t_RadioStation_2->isDetached();
    QVERIFY2(detached, "API:Radiostation Implicit sharing/ name 2");
    // test that changing the content of data sets detach true
    t_RadioStation_2->setName("RadioTwo");
    // should be detached
    detached = t_RadioStation_2->isDetached();
    QVERIFY2(detached, "API:Radiostation  Implicit sharing/ name 3");
}

/*!
 * Testing of change flags set by RadioStation class
 */
void TestRadioUiEngine::testChangeFlags()
{
    FUNC_LOG;
	RadioStation t_RadioStation;
		
    QVERIFY2(!t_RadioStation.isValid(), "API:Radiostation Init failure");
    t_RadioStation.setUserDefinedName("");
	t_RadioStation.setUserDefinedName("Radio Noice");
	t_RadioStation.setFrequency( KTestFrequency1 );
	//const int newIndex = mModel->findUnusedPresetIndex();
	t_RadioStation.setPresetIndex( 2 );
	t_RadioStation.setGenre( KTestGenre1 );
	t_RadioStation.setUrl( KTestUrl1 );	
	
	t_RadioStation.resetChangeFlags();
	bool persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 1");	
    t_RadioStation.setUserDefinedName("Radio Noice+");
	bool nameChanged = t_RadioStation.hasDataChanged( RadioStation::NameChanged );	
	QVERIFY2(nameChanged, "API:Radiostation Change flags/ NameChanged");	
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	QVERIFY2(persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 2");	
	t_RadioStation.resetChangeFlags();
			
	uint originalGenre = t_RadioStation.genre();
	t_RadioStation.setGenre( originalGenre + 1 );
	bool genreChanged = t_RadioStation.hasDataChanged( RadioStation::GenreChanged );
	QVERIFY2(genreChanged, "API:Radiostation Change flags/ GenreChanged");	
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	QVERIFY2(persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 3");	
	t_RadioStation.resetChangeFlags();
	
	QString originalUrl = t_RadioStation.url();
	t_RadioStation.setUrl(originalUrl);
	// previous url set so no url or persistent data change flags should be set
	bool urlChanged = t_RadioStation.hasDataChanged( RadioStation::UrlChanged );
	QVERIFY2(!urlChanged, "API:Radiostation Change flags/ UrlChanged");
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 4");	
	t_RadioStation.resetChangeFlags();
	
	QString originalRadioText = t_RadioStation.radioText();
	t_RadioStation.setRadioText( originalRadioText + "buy sausage" );
	bool radioTextChanged = t_RadioStation.hasDataChanged( RadioStation::RadioTextChanged );
	QVERIFY2(radioTextChanged, "API:Radiostation Change flags/ RadioTextChanged");
	// radio text not stored into cenrep
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 5");
	t_RadioStation.resetChangeFlags();
	t_RadioStation.setRadioText( t_RadioStation.radioText() );
	// because current radio text is reset change flags must not be affected
	radioTextChanged = t_RadioStation.hasDataChanged( RadioStation::RadioTextChanged );
	QVERIFY2(!radioTextChanged, "API:Radiostation Change flags/ RadioTextChanged");
	t_RadioStation.resetChangeFlags();
	
	QVERIFY2((t_RadioStation.psType()==RadioStation::Unknown), "API:Radiostation PS type check");	
	t_RadioStation.setPsType( RadioStation::Dynamic );
	bool psTypeChanged = t_RadioStation.hasDataChanged( RadioStation::PsTypeChanged );
	QVERIFY2(psTypeChanged, "API:Radiostation Change flags/ PsTypeChanged");
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	// PS type not stored as persistent data
	QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 6");
	t_RadioStation.resetChangeFlags();
	
	t_RadioStation.setFavorite(t_RadioStation.isFavorite());
	bool favouriteChanged = t_RadioStation.hasDataChanged( RadioStation::FavoriteChanged );
    QVERIFY2(!favouriteChanged, "API:Radiostation Change flags/ FavoriteChanged");    
    persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 7");
    t_RadioStation.resetChangeFlags();
    
    // initially there should not be PI code defined
    QVERIFY2(!t_RadioStation.hasPiCode(), "API:Radiostation Pi code check");
    // non-clear channel
    t_RadioStation.setPiCode(0xC004, RadioRegion::Default); //88.1 CBEE-FM - Chatham, ON
    bool piCodeChanged = t_RadioStation.hasDataChanged( RadioStation::PiCodeChanged );
    QVERIFY2(piCodeChanged, "API:Radiostation Change flags/ PiCodeChanged");
    persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 8");
}

/*!
 * Testing of call sign integer to char conversion
 */
void TestRadioUiEngine::TestCallSignChar()
{
    FUNC_LOG;
    /*
	for(uint i = 0; i < KLastCallSignCharCode; i++)
	{
	    RadioStation t_RadioStation;
	    bool passed =  t_RadioStation.callSignChar(i) == static_cast<char>( 'A' + i ); 	    	
	    QVERIFY2(passed, "API:Radiostation TestCallSignChar 1");	
	}
	RadioStation t_RadioStation;
	bool passed = t_RadioStation.callSignChar(KLastCallSignCharCode + 1) == static_cast<char>( '?' );
	QVERIFY2(passed, "API:Radiostation TestCallSignChar 2");
	*/
}

/*!
 * Testing of PI code to call sign conversion
 */
void TestRadioUiEngine::testPICodeToCallSign()
{
    FUNC_LOG;
	/*
    RadioStation t_RadioStation;
	
	// boundary values, two typical values and three chars call sign case
	QString callSign = t_RadioStation.piCodeToCallSign( KKxxxCallSignPiFirst - 1 );
	QVERIFY2((callSign==""), "API:Radiostation Call sign <");
	callSign = t_RadioStation.piCodeToCallSign( KKxxxCallSignPiFirst );
	QVERIFY2((callSign=="KAAA"), "API:Radiostation Call sign KAAA");
	callSign = t_RadioStation.piCodeToCallSign( 0x243F );
	QVERIFY2((callSign=="KHRJ"), "API:Radiostation Call sign KHRJ");
	callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiFirst - 1 );
	QVERIFY2((callSign=="KZZZ"), "API:Radiostation Call sign KZZZ");
	callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiFirst );
	QVERIFY2((callSign=="WAAA"), "API:Radiostation Call sign WAAA");
	callSign = t_RadioStation.piCodeToCallSign( 0x74B9 );
    QVERIFY2((callSign=="WMDT"), "API:Radiostation Call sign WMDT");
    callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiLast );
    QVERIFY2((callSign=="WZZZ"), "API:Radiostation Call sign WZZZ");
    callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiLast + 1 );
    QVERIFY2((callSign=="KEX"), "API:Radiostation Call sign KEX");
    callSign = t_RadioStation.piCodeToCallSign( 0x99B5 );
    QVERIFY2((callSign=="WJZ"), "API:Radiostation Call sign WJZ");    
    callSign = t_RadioStation.piCodeToCallSign( 0x99C0 );
    QVERIFY2((callSign==""), "API:Radiostation Call sign ????");
    */
}

/*!
 * Testing of RadioStationModel initial state
 */
void TestRadioUiEngine::testRadioStationModelInit()
{
    FUNC_LOG;
	//TODO:: Check why ASSERT fails when mModel->rowCount() == 0 
	if( mUiEngine->stationModel().rowCount()>0 )
    {	    
		mUiEngine->stationModel().removeAll();
		QVERIFY2((mUiEngine->stationModel().rowCount()==0), "API:RadioStationModel removeAll()");
	}
		 
	RadioStation foundStation;
	// no stations in the model in this phase
	QVERIFY2(!(mUiEngine->stationModel().findFrequency( KTestFrequency1, foundStation )), 
		"API:RadioStationModel findFrequency 1");	
}

/*!
 * Testing of RadioStationModel addStation method and resulting RadioStationModel signals
 */
void TestRadioUiEngine::testAddStation1()
{
    FUNC_LOG;
	RadioStation station;
	station.setFrequency( KTestFrequency1 );
	station.setFrequency( KTestFrequency1 );
	station.setGenre( KTestGenre1 );
	station.setGenre( KTestGenre1 );
	station.setUrl( KTestUrl1 );    
	station.setType( RadioStation::LocalStation );
	station.setType( RadioStation::LocalStation );
	station.setName("");
	station.setName( KTestStationName1 );
	station.setDynamicPsText( KTestDynamicPSText );
	station.setPiCode( 0xC004, RadioRegion::America ); //88.1 CBEE-FM - Chatham, ON
    QVERIFY2(!(station.dynamicPsText().compare(KTestDynamicPSText)), "API:RadioStationModel addStation 1");
    station.setDynamicPsText( KTestDynamicPSText );
	// check that adding station increases model row count
	mExpectedStationCount = mUiEngine->stationModel().rowCount() + 1;	
	mStationToBeAdded = station.name();
	mUiEngine->stationModel().addStation( station );
	QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 2");
	bool correctSignalsReceived = mEnteredSlots.testFlag( DataChanged );
	
	bool frequencyUpdated = station.frequency() == KTestFrequency1;
	QVERIFY2(frequencyUpdated, "API:RadioStationModel addStation 1");
	// check that correct signals received
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel addStation 3");
	mEnteredSlots = NoSlotsEntered;
	
	// check that added station can be found by frequency
    int index( KErrNotFound );
    index = mUiEngine->stationModel().indexFromFrequency( KTestFrequency1 );
    QVERIFY2(index != KErrNotFound, "Added station not found from model!");
    
    RadioStation readStation ( mUiEngine->stationModel().stationAt( index ) );    
    QVERIFY2( !readStation.name().compare(KTestStationName1), "Added station's name not found from model!");
}

/*!
 * Testing of RadioStationModel saveStation method and resulting RadioStationModel signals
 */
void TestRadioUiEngine::testSaveStation1()
{
    FUNC_LOG;
	RadioStation newStation1;
    // check that find by frequency works
    // this is needed for preset index to be updated into newStation1
    QVERIFY2((mUiEngine->stationModel().findFrequency( KTestFrequency1, newStation1 )), 
        "API:RadioStationModel findFrequency 2");
    newStation1.setType( RadioStation::Favorite );
    mEnteredSlots = NoSlotsEntered;
    mUiEngine->stationModel().saveStation( newStation1 );    
    bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
            mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );;
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 1");
    mEnteredSlots = NoSlotsEntered;
    
    newStation1.setGenre(newStation1.genre()+1);
    mUiEngine->stationModel().saveStation( newStation1 );
    correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged  ) && mEnteredSlots.testFlag( DataChanged );
    // check that correct signals received
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 2");
    mEnteredSlots = NoSlotsEntered;
}

/*!
 * Testing of RadioStationModel addStation method and rowCount methods
 */
void TestRadioUiEngine::testAddStation2()
{
    FUNC_LOG;
    RadioStation newStation2;
	newStation2.setFrequency( KTestFrequency2 );
	newStation2.setGenre( KTestGenre2 );
	newStation2.setUrl( KTestUrl2 );
    newStation2.setType( RadioStation::LocalStation | RadioStation::Favorite );
    newStation2.setName("Radio ice");
    // check that adding station increases model row count
    mExpectedStationCount = mUiEngine->stationModel().rowCount()+1;
    mStationToBeAdded = newStation2.name();
    mUiEngine->stationModel().addStation( newStation2 );
    
    QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 3");
    mEnteredSlots = NoSlotsEntered;
}

/*!
 * Testing of RadioStationModel saveStation method special cases
 */
void TestRadioUiEngine::testSaveStation2()
{
    FUNC_LOG;
    RadioStation newStation1;
    // this is needed for preset index to be updated into newStation1
    QVERIFY2((mUiEngine->stationModel().findFrequency( KTestFrequency1, newStation1 )), 
        "API:RadioStationModel findFrequency 2");
    
	newStation1.setFrequency( KTestFrequency2 );	
	mExpectedStationCount = mUiEngine->stationModel().rowCount();
	// updating existing station data must not increase station count
	mUiEngine->stationModel().saveStation( newStation1 );
	QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel saveStation 1");    
    // because frequency tried to be updated no signals should be received either
	bool correctSignalsReceived = mEnteredSlots.testFlag( DataChanged );
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 2");
    mEnteredSlots = NoSlotsEntered;
    
    // original frequency resumed
    newStation1.setFrequency( KTestFrequency1 );
    newStation1.setGenre(newStation1.genre()+1);
    mExpectedStationCount = mUiEngine->stationModel().rowCount();
    // now that frequency resumed signal should be received also
    mUiEngine->stationModel().saveStation( newStation1 );
    correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && mEnteredSlots.testFlag( DataChanged );
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 3");
    // updating existing station data must not increase station count
    QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel saveStation 2");
    
    mExpectedStationCount = mUiEngine->stationModel().rowCount();
    mStationToBeAdded = newStation1.name();
    // adding station must fail because the frequency is the same as previously used frequency
    mUiEngine->stationModel().addStation( newStation1 );
    QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 3");
}

/*!
 * Testing of RadioStationModel addStation method special cases
 */
void TestRadioUiEngine::testAddStation3()
{
    FUNC_LOG;
    RadioStation newStation3;
    newStation3.setFrequency( KTestFrequency2 );
    newStation3.setGenre(3);
    newStation3.setUrl("http://www.radio4noice.com");
    newStation3.setType( RadioStation::LocalStation | RadioStation::Favorite );
    newStation3.setName("Radio e");
    // adding station must fail because frequency is the same as previously used frequency
    mExpectedStationCount = mUiEngine->stationModel().rowCount();
    mStationToBeAdded = newStation3.name();
    mUiEngine->stationModel().addStation( newStation3 );    
    QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 4");
    // adding station must success because the station frequency is different now
    newStation3.setFrequency( KTestFrequency2 + 1 );
    mExpectedStationCount = mUiEngine->stationModel().rowCount() + 1;
    mUiEngine->stationModel().addStation( newStation3 );
    QVERIFY2((mUiEngine->stationModel().rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 5");
    // test that station can be found by frequency range
    QList<RadioStation> stations;
    stations = mUiEngine->stationModel().stationsInRange( KTestFrequency1, KTestFrequency3 );
    QVERIFY2((stations.count()==3), "API:RadioStationModel stationsInRange");
}

/*!
 * Testing of RadioStationModel saveStation method special cases
 */
void TestRadioUiEngine::testSaveStation3()
{
    FUNC_LOG;
    RadioStation newStation3;
    RadioStation foundStation;
    QVERIFY2(mUiEngine->stationModel().findFrequency( KTestFrequency2 + 1, newStation3 ),			
        "API:RadioStationModel findFrequency 4");
    
    newStation3.setFrequency( KTestFrequency3 );
    // because frequency or preset index don't have change flag the frequency must not be updated 
    mUiEngine->stationModel().saveStation( newStation3 );

    QVERIFY2(!(mUiEngine->stationModel().findFrequency( KTestFrequency3, foundStation )),			
        "API:RadioStationModel findFrequency 3");

    newStation3.setGenre( newStation3.genre() + 1 );
    // allthough genre changed so the frequency update must not become effective  
    mUiEngine->stationModel().saveStation( newStation3 );	

    QVERIFY2(!(mUiEngine->stationModel().findFrequency( KTestFrequency3, foundStation )),			
        "API:RadioStationModel findFrequency 4");
    

    mEnteredSlots = NoSlotsEntered;
    foreach( const RadioStation& station, mUiEngine->stationModel().list() ) 
    {
        if ( station.frequency() == KTestFrequency2 ) 
        {        
            mUiEngine->stationModel().setFavoriteByFrequency( KTestFrequency2, !station.isFavorite() );
        }
    }
    bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
        mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel findFrequency 5");
}



/*!
 * Test that stations are stored into RadioStationModel in ascending frequency order  
 */
void TestRadioUiEngine::testSortByFrequency()
{
    FUNC_LOG;
	int role = RadioRole::RadioStationRole;
	int previousFrequency(0); // int not initialized zero as default
	
	for (int i = 0; i< mUiEngine->stationModel().rowCount(); i++ )
	{
		QModelIndex index = mUiEngine->stationModel().index( i, 0 );
		QVariant stationData = mUiEngine->stationModel().data( index, role );
		RadioStation station = stationData.value<RadioStation>();
		// stations must be obtainded at ascending frequency order
		QVERIFY2((station.frequency()>previousFrequency), "API:RadioStationModel testSortByFrequency");
		previousFrequency = station.frequency();
	}
}

/*!
 * Test that preset indexes match
 */
void TestRadioUiEngine::testFindPresetIndex()
{
    FUNC_LOG;
	RadioStation station;
	for (int i = 0; i< mUiEngine->stationModel().rowCount(); i++ )
    {
		QVERIFY2((mUiEngine->stationModel().findPresetIndex(i) != RadioStation::NotFound), 
	        "API:RadioStationModel testFindPresetIndex 1");
		QVERIFY2((mUiEngine->stationModel().findPresetIndex( i, station ) != RadioStation::NotFound), 
			"API:RadioStationModel testFindPresetIndex 2");
		QVERIFY2((station.presetIndex() == i), "API:RadioStationModel testFindPresetIndex 3");
	}
}

/*!
 * Test that preset can be removed from model by frequency
 */
void TestRadioUiEngine::testRemoveByFrequency()
{
    FUNC_LOG;
	RadioStation station;
	int initialStationCount( mUiEngine->stationModel().rowCount() );
	int presetIndex(0);
	
	// find valid preset index
	for(int i = 0; i<mUiEngine->stationModel().rowCount(); i++)
	{
		presetIndex = mUiEngine->stationModel().findPresetIndex( i, station );
		if(presetIndex!=RadioStation::NotFound)
			break;
	}
	QVERIFY2((presetIndex != RadioStation::NotFound), "API:RadioStationModel testRemoveByFrequency 1");
		
	mUiEngine->stationModel().removeByFrequency( station.frequency() );	
	QVERIFY2((mUiEngine->stationModel().rowCount()==(initialStationCount-1)), "API:RadioStationModel testRemoveByFrequency 2");
}

/*!
 * Test that station can be removed from model by preset index 
 */
void TestRadioUiEngine::testRemoveByPresetIndex()
{
    FUNC_LOG;
	RadioStation station;
	station.setFrequency( KTestFrequency4 );
	mStationToBeAdded = "";
	mExpectedStationCount = mUiEngine->stationModel().rowCount() + 1;
	mUiEngine->stationModel().addStation( station );
	// for updating station preset index
	QVERIFY2((mUiEngine->stationModel().findFrequency( KTestFrequency4, station )), 
			"API:RadioStationModel testRemoveByPresetIndex 1");
	
	int initialStationCount( mUiEngine->stationModel().rowCount() );
	mUiEngine->stationModel().removeByPresetIndex( station.presetIndex() );    
    QVERIFY2((mUiEngine->stationModel().rowCount()==(initialStationCount-1)), "API:RadioStationModel testRemoveByPresetIndex 2");
}

/*!
 * Test RadioStationModel method removeStation
 */
void TestRadioUiEngine::testRemoveStation()
{
    FUNC_LOG;
	RadioStation station;
	station.setFrequency( KTestFrequency5 );
	mStationToBeAdded = "";
	mExpectedStationCount = mUiEngine->stationModel().rowCount() + 1;
	mUiEngine->stationModel().addStation( station );
	int initialStationCount( mUiEngine->stationModel().rowCount() );
	// for updating station preset index
	QVERIFY2(mUiEngine->stationModel().findFrequency( KTestFrequency5, station ),			
	        "API:RadioStationModel testRemoveStation");
	
	mUiEngine->stationModel().removeStation( station );	
	QVERIFY2((mUiEngine->stationModel().rowCount()==(initialStationCount-1)), "API:RadioStationModel testRemoveStation");
}


/*!
 * Test setting and unsetting station type favourite
 */
void TestRadioUiEngine::testSetFavorite()
{	
    FUNC_LOG;
	RadioStation station;	
	station.setFrequency( KTestFrequency6 );
	station.setType( RadioStation::Favorite );	
    QVERIFY2(station.isFavorite(), "API:RadioStationModel testSetFavorite 1");
	mUiEngine->stationModel().addStation( station );
	mEnteredSlots = NoSlotsEntered;	
	
	mUiEngine->stationModel().setFavoriteByFrequency( KTestFrequency6, false );	
	bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
	    mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel testSetFavorite 2");
	
	mUiEngine->stationModel().findFrequency( KTestFrequency6, station );
	QVERIFY2(!station.isFavorite(), "API:RadioStationModel testSetFavorite 3");
	mEnteredSlots = NoSlotsEntered;
	
	mUiEngine->stationModel().setFavoriteByPreset( station.presetIndex(), true );
	correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
		mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel testSetFavorite 4");
    
	// test toggling the favorite 
	int index = mUiEngine->stationModel().indexFromFrequency( KTestFrequency6 );
	mUiEngine->stationModel().setFavoriteByFrequency( KTestFrequency6, ETrue );
    RadioStation station1 = mUiEngine->stationModel().stationAt( index );
    QVERIFY2(station1.isFavorite(), "API:RadioStationModel testToggleFavourite");

}

/*!
 * Test renaming station
 */
void TestRadioUiEngine::testRenameStation()
{	
    FUNC_LOG;
	RadioStation station;
	int presetIndex(0);
	// find valid preset index
    for( int i = 0; i<mUiEngine->stationModel().rowCount(); i++ )
	{
		if( mUiEngine->stationModel().findPresetIndex( i, station ) != RadioStation::NotFound )
		{
			presetIndex = i;
			break;
		}
	}   
    QString initialStationName( station.name() );
    mEnteredSlots = NoSlotsEntered;
	mUiEngine->stationModel().renameStation( presetIndex, initialStationName + "Renamed" );	
	bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
        mEnteredSlots.testFlag( DataChanged );
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel testRenameStation 1");	
    QVERIFY2(initialStationName!=mUiEngine->stationModel().stationAt(presetIndex).name(), "API:RadioStationModel testRenameStation 2");
        
    station.setUserDefinedName( KTestStationName2 );
    QVERIFY2(station.isRenamed(), "API:RadioStationModel testRenameStation 1");
    
}

/*!
 * Test setting radio text
 */
void TestRadioUiEngine::testSetRadioTextPlus()
{
    FUNC_LOG;
    RadioStation station;
    mUiEngine->stationModel().findFrequency( KTestFrequency6, station );
    station.setRadioText( "" );
    station.setRadioText( KTestRadioTextRadioText );
    station.setRadioText( KTestRadioTextRadioText );
    station.setRadioTextPlus( RtPlus::Artist, KTestRadioTextPlusArtist );
    station.setRadioTextPlus( RtPlus::Title, KTestRadioTextPlusTitle );
    station.setRadioTextPlus( RtPlus::Homepage, KTestRadioTextPlusUrl );
    station.setRadioTextPlus( RtPlus::Homepage + 1, KTestRadioTextPlusUnsupportedTag );
    mUiEngine->stationModel().saveStation( station );
    bool effective = station.radioText().contains( KTestRadioTextPlusArtist, Qt::CaseSensitive );
    QVERIFY2(effective, "API:RadioStationModel testSetRadioTextPlus 1");
    effective = station.radioText().contains( KTestRadioTextPlusTitle, Qt::CaseSensitive );
    QVERIFY2(effective, "API:RadioStationModel testSetRadioTextPlus 2");
    effective = !station.url().compare( KTestRadioTextPlusUrl );
    QVERIFY2(effective, "API:RadioStationModel testSetRadioTextPlus 3");
}

/*!
 * Test RadioStationModel API
 */	
void TestRadioUiEngine::testRadioStationModel()
{
    FUNC_LOG;
	testRadioStationModelInit();
	testAddStation1();
	testSaveStation1();
	testAddStation2();
	testSaveStation2();
	testAddStation3();
	testSaveStation3();
	testSortByFrequency();
	testFindPresetIndex();
	testRemoveByFrequency();
	testRemoveByPresetIndex();
	testRemoveStation();
	testSetFavorite();
	testRenameStation();
	testSetRadioTextPlus();
}
/*!
 * Test RadioHistoryModel API
 */
void TestRadioUiEngine::testhistoryModel()
    {
    FUNC_LOG;
    testHistoryModelInit();
    testHistoryModelAddItem();
    testAddRadioTextPlus();
    testClearRadioTextPlus();
    }

/*!
 * Testing of RadioHistoryModel initial state
 */
void TestRadioUiEngine::testHistoryModelInit()
{
    FUNC_LOG;
    //TODO:: Check why ASSERT fails when mUiEngine->historyModel().rowCount() == 0 
    if( mUiEngine->historyModel().rowCount()>0 )
    {       
        mUiEngine->historyModel().removeAll( EFalse );
    }
    QVERIFY2((mUiEngine->historyModel().rowCount()==0), "API:RadioHistoryModel removeAll()");           
}

/*!
 *
 */
void TestRadioUiEngine::testHistoryModelAddItem()
{
    FUNC_LOG;
    RadioStation station;
    mUiEngine->stationModel().findFrequency( KTestFrequency6, station );
    int expectedHistoryItemCount( mUiEngine->historyModel().rowCount() + 1 );
    mEnteredSlots = NoSlotsEntered;
    mUiEngine->historyModel().addItem( KTestArtist1, KTestTitle1, station );    
    // check that item count increases
    QVERIFY2((mUiEngine->historyModel().rowCount()==expectedHistoryItemCount), "API:RadioHistoryModel addItem() 2");
    
    // check that artist/title stored into the model conforms to the one read from the model
    QModelIndex index = mUiEngine->historyModel().index( 0, 0 );
    QStringList stringList = mUiEngine->historyModel().data( index, Qt::DisplayRole ).value<QStringList>();        
    QString artistTitle = stringList.at(0);    
    QVERIFY2(!(artistTitle.compare(KTestArtist1+" - "+KTestTitle1)), "API:RadioHistoryModel addItem() 3");
    
    
    expectedHistoryItemCount = mUiEngine->historyModel().rowCount() + 1;
    // trying to add an item that allready exists must increase the item count
    mUiEngine->historyModel().addItem( KTestArtist1, KTestTitle1, station );
    INFO_1( "mUiEngine->historyModel().rowCount()== %i", mUiEngine->historyModel().rowCount() );
    QVERIFY2((mUiEngine->historyModel().rowCount()==expectedHistoryItemCount), "API:RadioHistoryModel addItem() 4");
}

/*!
 *
 */
void TestRadioUiEngine::testAddRadioTextPlus()
{   
    FUNC_LOG;
    RadioStation station;
    mUiEngine->stationModel().findFrequency( KTestFrequency6, station );
    mUiEngine->historyModel().addRadioTextPlus( RtPlus::Artist, KTestArtist1, station );
    mUiEngine->historyModel().addRadioTextPlus( RtPlus::Title, KTestTitle1, station );
}

/*!
 *
 */
void TestRadioUiEngine::testClearRadioTextPlus()
{   
    FUNC_LOG;
    mUiEngine->historyModel().clearRadioTextPlus();
}

/*!
 * Test RadioHistoryModelItem API
 */
void TestRadioUiEngine::testHistoryModelItem()
{
    FUNC_LOG;
    RadioHistoryItem* item = new RadioHistoryItem( KTestArtist3, KTestTitle3 );
    item->setArtist( KTestArtist3 );
    QVERIFY2(!(item->artist().compare(KTestArtist3)), "API:testHistoryModelItem setArtist()");
    item->setTitle( KTestTitle3 );
    QVERIFY2(!(item->title().compare(KTestTitle3)), "API:testHistoryModelItem setTitle()");
    QVERIFY2( item->isValid(), "API:testHistoryModelItem isValid() 1");
    item->reset();
    QVERIFY2(!item->isValid(), "API:testHistoryModelItem isValid() 2");
    delete item;
    item = NULL;
}

void TestRadioUiEngine::Timeout( TUint aTimerId )
    {
    FUNC_LOG;    
    if ( CSchedulerStopAndStartTimer::ETimerIdCreateMUT == aTimerId )
        {
        INFO("ETimerIdCreateMUT elapsed");
        }
    else if ( CSchedulerStopAndStartTimer::ETimerIdDeleteMUT == aTimerId )
        {
        INFO("ETimerIdDeleteMUT elapsed");
        }
    else if ( CSchedulerStopAndStartTimer::ETimerIdRunMUT == aTimerId )
        {
        INFO("ETimerIdRunMUT elapsed");
        }
    else
        {
        INFO("Unknown timer elapsed");        
        }
    }

void TestRadioUiEngine::CreateMUT()
    {
    FUNC_LOG;
    RadioEngineUtils::InitializeL();
    mUiEngine = new RadioUiEngine;
    mUiEngine->init();
    
    mEngineWrapper.reset( new RadioEngineWrapper( mUiEngine->stationModel().stationHandlerIf() ) );
    mEngineWrapper->init();
    mPresetStorage.reset( new RadioPresetStorage() );
    mUiEngine->stationModel().initialize( mPresetStorage.data(), mEngineWrapper.data() );
    
    //TODO:: Check why ASSERT fails when mModel->rowCount() == 0 
    if(mUiEngine->stationModel().rowCount()>0)
    {
        mUiEngine->stationModel().removeAll(); //ASSERT: \"last >= first\" in file qabstractitemmodel.cpp, line 2110    
    }    

    Radio::connect( &mUiEngine->stationModel(),  SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
        this,    SLOT(dataChanged(const QModelIndex, const QModelIndex)) );
    
    Radio::connect( &mUiEngine->stationModel(),           SIGNAL(stationDataChanged(RadioStation)),
        this,    SLOT(stationDataChanged(RadioStation)) );
    
    Radio::connect( &mUiEngine->stationModel(),           SIGNAL(favoriteChanged(RadioStation)),
        this,    SLOT(favoriteChanged(RadioStation)) );
    
    Radio::connect( &mUiEngine->historyModel(),           SIGNAL(itemAdded()),
        this,    SLOT(itemAdded()) );
    mSchedulerTimer->StartTimer( 1000000, CSchedulerStopAndStartTimer::ETimerIdRunMUT );
    }

void TestRadioUiEngine::DeleteMUT()
    {
    FUNC_LOG;
    if ( NULL != mUiEngine )
        {
        delete mUiEngine;
        mUiEngine = NULL;        
        }
    }
