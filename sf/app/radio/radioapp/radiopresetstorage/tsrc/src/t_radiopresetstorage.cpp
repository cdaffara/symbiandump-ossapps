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
#include "t_radiopresetstorage.h"
#include "radiopresetstorage.h"
#include "radiostation.h"
#include "radiostation_p.h"
#include "trace.h"

// Constants
const uint KTestFrequency1 = 89000000;
const uint KTestFrequency2 = 89500000;
const uint KTestFrequency3 = 90000000;
const uint KTestFrequency4 = 90500000;
const int KInvalidPresetIndex = -2;
const uint KFirstPresetIndex = 0;
const uint KSecondPresetIndex = 1;
const uint KThirdPresetIndex = 2;
const uint KFourthPresetIndex = 3;
const uint KArbitraryPresetIndex = 14;
const uint KFrequencyStep = 100000;

/*!
 *
 */
int main(int /* argc*/, char *argv[])
{
    FUNC_LOG;
    TestRadioPresetStorage tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiopresetstorage.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


TestRadioPresetStorage::TestRadioPresetStorage()
{
    FUNC_LOG;
}


/*!
 * Destructor
 */
TestRadioPresetStorage::~TestRadioPresetStorage()
{
    FUNC_LOG;
}

/*!
 * called before each testfunction is executed
 */
void TestRadioPresetStorage::init()
{
    FUNC_LOG;
}

/*!
 * called after every testfunction
 */
void TestRadioPresetStorage::cleanup()
{
    FUNC_LOG;
}

/*!
 * called before the first testfunction is executed
 */
void TestRadioPresetStorage::initTestCase()
{
    FUNC_LOG;
    mPresetStorage.reset( new RadioPresetStorage() );
    // this is the range used in this test module
    for(int i = 0; i <= mPresetStorage->maxNumberOfPresets(); i++)
    {
        mPresetStorage->deletePreset( i );        
    }    
    QVERIFY2(( mPresetStorage->presetCount() == 0 ), "API:RadioPresetStorage initTestCase 1");
    INFO_1("mPresetStorage->maxNumberOfPresets() == %i", mPresetStorage->maxNumberOfPresets() );
}

/*!
 * called after the last testfunction was executed
 */
void TestRadioPresetStorage::cleanupTestCase()
{
    FUNC_LOG;
	
}

/*!
 * Test saving preset 
 */
void TestRadioPresetStorage::testSavePreset()
{
    FUNC_LOG;
    int test = mPresetStorage->maxNumberOfPresets();
    int initialPresetCount( 0 );
    //int initialPresetindex( KFirstPresetIndex );  
     RadioStation station;  
    station.setFrequency( KTestFrequency1 );
    station.setPresetIndex( KFirstPresetIndex );
    QVERIFY2( mPresetStorage->savePreset( *station.data_ptr() ), "Preset save failed! 1");    
    INFO_1("mPresetStorage->presetCount() = %i", mPresetStorage->presetCount() );
    QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 1 ), "API:RadioPresetStorage testSavePreset 1");
    INFO_1("mPresetStorage->nextPreset() = %i", mPresetStorage->nextPreset( KFirstPresetIndex ) );
    QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KErrNotFound ), "API:RadioPresetStorage testSavePreset 2");
    
    station.setFrequency( KTestFrequency2 );
    station.setPresetIndex( KSecondPresetIndex );
    QVERIFY2( mPresetStorage->savePreset( *station.data_ptr() ), "Preset save failed! 2");    
    QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 2 ), "API:RadioPresetStorage testSavePreset 3");
    QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KSecondPresetIndex ), "API:RadioPresetStorage testSavePreset 4");
    
    // check that saving preset succeeds if preset index and frequency are allready in use
    station.setFrequency( KTestFrequency2 );
    station.setPresetIndex( KSecondPresetIndex );
    QVERIFY2( mPresetStorage->savePreset( *station.data_ptr() ), "Preset save failed! 3");    
    QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 2 ), "API:RadioPresetStorage testSavePreset 5");
    
    // check that saving preset success when new preset index and frequency are not in use
    station.setFrequency( KTestFrequency3 );
    station.setPresetIndex( KThirdPresetIndex );
    QVERIFY2( mPresetStorage->savePreset( *station.data_ptr() ), "Preset save failed! 4");    
    QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 3 ), "API:RadioPresetStorage testSavePreset 6");
    
    // check that saving with out of boundary preset index succeeds
    station.setFrequency( KTestFrequency4 );
    station.setPresetIndex( KInvalidPresetIndex );  
    QVERIFY2( mPresetStorage->savePreset( *station.data_ptr() ), "Preset save failed! 5");    
    INFO_1("mPresetStorage->presetCount() = %i", mPresetStorage->presetCount() );
    QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 4 ), "API:RadioPresetStorage testSavePreset 7");

    // check that saving with arbitrary preset index succeeds
    station.setFrequency( KTestFrequency4 );
    station.setPresetIndex( KArbitraryPresetIndex );  
    QVERIFY2( mPresetStorage->savePreset( *station.data_ptr() ), "Preset save failed! 6");    
    INFO_1("mPresetStorage->presetCount() = %i", mPresetStorage->presetCount() );
    QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 5 ), "API:RadioPresetStorage testSavePreset 8");
}

/*!
 * Test reading preset 
 */
void TestRadioPresetStorage::testReadPreset()
{
    FUNC_LOG;
    RadioStation station;
    RadioStationIf* preset = static_cast<RadioStationIf*>( station.data_ptr() );
    QVERIFY2( mPresetStorage->readPreset( KFirstPresetIndex, *preset ),  "Failed to read a preset");
    QVERIFY2(( preset->frequency() == KTestFrequency1 ), "API:RadioPresetStorage testReadPreset 1");
    
    RadioStation station1;
    RadioStationIf* preset1 = static_cast<RadioStationIf*>( station1.data_ptr() );
    QVERIFY2( mPresetStorage->readPreset( KSecondPresetIndex, *preset1 ),  "Failed to read a preset");
    QVERIFY2(( preset1->frequency() == KTestFrequency2 ), "API:RadioPresetStorage testReadPreset 2");  
    
    RadioStation station2;
    RadioStationIf* preset2 = static_cast<RadioStationIf*>( station2.data_ptr() );
    QVERIFY2( mPresetStorage->readPreset( KThirdPresetIndex, *preset2 ),  "Failed to read a preset");
    QVERIFY2(( preset2->frequency() == KTestFrequency3 ), "API:RadioPresetStorage testReadPreset 3");
    
    RadioStation station3;
    RadioStationIf* preset3 = static_cast<RadioStationIf*>( station3.data_ptr() );
    QVERIFY2( mPresetStorage->readPreset( KFourthPresetIndex, *preset3 ),  "Failed to read a preset");
    QVERIFY2(( preset3->frequency() == KTestFrequency4 ), "API:RadioPresetStorage testReadPreset 4");

    RadioStation stationArbitrary;
    RadioStationIf* presetArbitrary = static_cast<RadioStationIf*>( stationArbitrary.data_ptr() );
    QVERIFY2( mPresetStorage->readPreset( KArbitraryPresetIndex, *presetArbitrary ),  "Failed to read a preset");
    QVERIFY2(( presetArbitrary->frequency() ==  KTestFrequency4 ), "API:RadioPresetStorage testReadPreset 6");

}

/*!
 * Test deleting preset
 */
void TestRadioPresetStorage::testDeletePreset()
{   
    FUNC_LOG;
    int initialPresetCount( mPresetStorage->presetCount() );
    
    QVERIFY2(( mPresetStorage->firstPreset() == KFirstPresetIndex ), "API:RadioPresetStorage testDeletePreset 1");
    QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KSecondPresetIndex ), "API:RadioPresetStorage testDeletePreset 2");
    mPresetStorage->deletePreset( KSecondPresetIndex );
    QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KThirdPresetIndex ), "API:RadioPresetStorage testDeletePreset 3");
    QVERIFY2(( mPresetStorage->presetCount() == (initialPresetCount - 1)), "API:RadioPresetStorage testDeletePreset 4");       
    
    RadioStation station;
    RadioStationIf* preset = static_cast<RadioStationIf*>( station.data_ptr() );
    QVERIFY2( EFalse == mPresetStorage->readPreset( KSecondPresetIndex, *preset ), "Reading of deleted preset succeeded!" );
    
    mPresetStorage->deletePreset( KFirstPresetIndex );
    QVERIFY2(( mPresetStorage->firstPreset() == KThirdPresetIndex ), "API:RadioPresetStorage testDeletePreset 6");
    mPresetStorage->deletePreset( KThirdPresetIndex );
    mPresetStorage->deletePreset( KFourthPresetIndex );
    mPresetStorage->deletePreset( KArbitraryPresetIndex );

    
    QVERIFY2(( mPresetStorage->presetCount() == 0), "API:RadioPresetStorage testDeletePreset 7");
}
  
/*!
 * Test filling preset storage and handling boundary values
 */
void TestRadioPresetStorage::testStressTest()
{
    FUNC_LOG;
    for(int i = 0; i < mPresetStorage->maxNumberOfPresets(); i++)
    {
        RadioStation station;
        station.setFrequency( 87500000 + i*KFrequencyStep );
        station.setPresetIndex( i );  
        mPresetStorage->savePreset( *station.data_ptr() );
    }
    QVERIFY2(( mPresetStorage->presetCount() == mPresetStorage->maxNumberOfPresets()), "API:RadioPresetStorage testStressTest 1");
    int test = mPresetStorage->presetCount();
    RadioStation station;
    station.setFrequency( 87500000 + mPresetStorage->maxNumberOfPresets()*KFrequencyStep + KFrequencyStep);
    station.setPresetIndex( mPresetStorage->maxNumberOfPresets() );  
    mPresetStorage->savePreset( *station.data_ptr() );
    int r = mPresetStorage->presetCount();
    // preset count must not increase
    QVERIFY2(( mPresetStorage->presetCount() == mPresetStorage->maxNumberOfPresets()), "API:RadioPresetStorage testStressTest 2");
    
    for(int i = 0; i <= mPresetStorage->maxNumberOfPresets(); i++)
    {
        mPresetStorage->deletePreset( i );        
    }  

    // test deleting with invalid preset index
    mPresetStorage->deletePreset( KInvalidPresetIndex );
    QVERIFY2(( mPresetStorage->presetCount() == 0), "API:RadioPresetStorage testStressTest 3");
}
