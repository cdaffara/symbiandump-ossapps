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
* Description: Unit test for MpSettingsManager
*
*/

#include <QSignalSpy>
#include <qnamespace.h>
#include "mpsettingsmanagerdefs.h"


#include "unittest_mpsettingsmanager.h"
#include "stub/inc/xqsettingskey.h"
#include "stub/inc/xqsettingsmanager.h"


// Do this so we can access all member variables.
#define private public
#include "mpsettingsmanager.h"
#undef private

//This so we can test private functions
#include "../../src/mpsettingsmanager.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestMpSettingsManager tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsettingsmanager.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpSettingsManager::TestMpSettingsManager()
{
}

TestMpSettingsManager::~TestMpSettingsManager()
{

}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSettingsManager::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSettingsManager::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSettingsManager::init()
{

}

/*!
 Called after every testfunction.
 */
void TestMpSettingsManager::cleanup()
{

}

/*!
 TestConstructor.
 */
void TestMpSettingsManager::testConstructor()
{
    MpSettingsManager *test;
    test = new MpSettingsManager();
    QVERIFY(test);
    QVERIFY(test->mSettingsManager);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingFirstStartupKey),1);
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingFirstStartupKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingFirstStartupKey),0);
    QCOMPARE(test->mFirstStartup, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingShuffleKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingShuffleKey), 1);
    QCOMPARE(test->mShuffle, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mRepeat, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingPresetIdKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingPresetIdKey),1);
    QCOMPARE(test->mPreset, 1);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingInspireMeKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingInspireMeKey),1);
    QCOMPARE(test->mInspireMe, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingSongDetailsGBKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingSongDetailsGBKey),1);
    QCOMPARE(test->mSongDetailsGb, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMusicPlayerShowMtpInfoKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMusicPlayerShowMtpInfoKey),1);
    QCOMPARE(test->mShowMtpInfo, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMusicPlayerMtpInfoUrlKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMusicPlayerMtpInfoUrlKey),0);
    QCOMPARE(test->mMtpInfoUrl, QString("testText"));

    delete test;
    test = 0;
}

/*!
 TestInstanceConstruction.
 */
void TestMpSettingsManager::testInstanceConstruction()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    QVERIFY(test);
    QVERIFY(test->mSettingsManager);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingFirstStartupKey),1);
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingFirstStartupKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingFirstStartupKey),0);
    QCOMPARE(test->mFirstStartup, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingShuffleKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingShuffleKey), 1);
    QCOMPARE(test->mShuffle, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mRepeat, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingPresetIdKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingPresetIdKey),1);
    QCOMPARE(test->mPreset, 1);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingInspireMeKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingInspireMeKey),1);
    QCOMPARE(test->mInspireMe, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingSongDetailsGBKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingSongDetailsGBKey),1);
    QCOMPARE(test->mSongDetailsGb, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMusicPlayerShowMtpInfoKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMusicPlayerShowMtpInfoKey),1);
    QCOMPARE(test->mShowMtpInfo, true);

    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMusicPlayerMtpInfoUrlKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMusicPlayerMtpInfoUrlKey),0);
    QCOMPARE(test->mMtpInfoUrl, QString("testText"));

    QCOMPARE(test, MpSettingsManager::instance()); // test that instance returns the singleton
}

/*!
testFirstStartupGet
 */
void TestMpSettingsManager::testFirstStartupGet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();

    QCOMPARE(test->firstStartup(), true);

}

/*!
testShuffleGetAndSet
 */
void TestMpSettingsManager::testShuffleGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mShuffle = true;
    MpSettingsManager::setShuffle(false);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingShuffleKey),QVariant(false));
    QCOMPARE(MpSettingsManager::shuffle(), true); //should not chaqnge until stored in cenrep

  
    test->mShuffle = false;
    test->setShuffle(true);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingShuffleKey),QVariant(true));
    QCOMPARE(MpSettingsManager::shuffle(), false);//should not chaqnge until stored in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingShuffleKey),2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingShuffleKey),0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingShuffleKey), 0);
}

/*!
testRepeatGetAndSet
 */
void TestMpSettingsManager::testRepeatGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mRepeat = true;
    MpSettingsManager::setRepeat(false);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingRepeatKey),QVariant(false));
    QCOMPARE(MpSettingsManager::repeat(), true); //should not chaqnge until stored in cenrep

  
    test->mRepeat = false;
    test->setRepeat(true);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingRepeatKey),QVariant(true));
    QCOMPARE(MpSettingsManager::repeat(), false);//should not chaqnge until stored in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingRepeatKey),2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingRepeatKey),0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingRepeatKey), 0);
}

/*!
testPresetGetAndSet
 */
void TestMpSettingsManager::testPresetGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mPreset = 1;
    MpSettingsManager::setPreset(0);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingPresetIdKey),QVariant(0));
    QCOMPARE(MpSettingsManager::preset(), 1); //should not chaqnge until stored in cenrep

  
    test->mPreset = 0;
    test->setPreset(1);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingPresetIdKey),QVariant(1));
    QCOMPARE(MpSettingsManager::preset(), 0);//should not chaqnge until stored in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingPresetIdKey),2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingPresetIdKey),0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingPresetIdKey), 0);
}

/*!
testInspireMeGetAndSet
 */
void TestMpSettingsManager::testInspireMeGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mInspireMe = true;
    MpSettingsManager::setInspireMe(false);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingInspireMeKey),QVariant(false));
    QCOMPARE(MpSettingsManager::inspireMe(), true); //should not chaqnge until stored in cenrep


    test->mInspireMe = false;
    test->setInspireMe(true);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingInspireMeKey),QVariant(true));
    QCOMPARE(MpSettingsManager::inspireMe(), false);//should not chaqnge until stored in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingInspireMeKey), 2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingInspireMeKey), 0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingInspireMeKey), 0);
}

/*!
testSongDetailsGbGetAndSet
 */
void TestMpSettingsManager::testSongDetailsGbGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mSongDetailsGb = true;
    MpSettingsManager::setSongDetailsGb(false);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingSongDetailsGBKey),QVariant(false));
    QCOMPARE(MpSettingsManager::songDetailsGb(), true); //should not chaqnge until stored in cenrep


    test->mSongDetailsGb = false;
    test->setSongDetailsGb(true);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingSongDetailsGBKey),QVariant(true));
    QCOMPARE(MpSettingsManager::songDetailsGb(), false);//should not chaqnge until stored in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingSongDetailsGBKey), 2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingSongDetailsGBKey), 0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingSongDetailsGBKey), 0);
}

/*!
testShowMtpInfoGetAndStop
 */
void TestMpSettingsManager::testShowMtpInfoGetAndStop()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mShowMtpInfo = true;
    MpSettingsManager::stopShowingMtpInfo();
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMusicPlayerShowMtpInfoKey),QVariant(false));
    QCOMPARE(MpSettingsManager::showMtpInfo(), true); //should not chaqnge until stored in cenrep

  
    test->mShowMtpInfo = false;
    QCOMPARE(MpSettingsManager::showMtpInfo(), false);

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMusicPlayerShowMtpInfoKey), 1);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMusicPlayerShowMtpInfoKey), 0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMusicPlayerShowMtpInfoKey), 0);
}

/*!
testMtpInfoUrlGet
 */
void TestMpSettingsManager::testMtpInfoUrlGet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mMtpInfoUrl = QString("http:\\nokia.com");
    QCOMPARE(MpSettingsManager::mtpInfoUrl(), QString("http:\\nokia.com"));

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMusicPlayerMtpInfoUrlKey), 0);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMusicPlayerMtpInfoUrlKey), 0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMusicPlayerMtpInfoUrlKey), 0);
}

/*!
 testValueChangedShuffleCase.
 */ 
void TestMpSettingsManager::testValueChangedShuffleCase()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();

    
    XQSettingsKey suffleProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingShuffleKey);
    test->mShuffle = true;
    test->valueChanged(suffleProfileKey, QVariant(0));
    QCOMPARE(test->mShuffle, false);

    test->mShuffle = false;
    QSignalSpy spy( test, SIGNAL(shuffleChanged(bool) ) );
    test->mSettingsManager->emitValueChanged(suffleProfileKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(true) );
    QCOMPARE(test->mShuffle, true);
}

/*!
 testValueChangedRepeatCase.
 */ 
void TestMpSettingsManager::testValueChangedRepeatCase()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
      
    XQSettingsKey repeatProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingRepeatKey);
    test->mRepeat = true;
    test->valueChanged(repeatProfileKey, QVariant(0));
    QCOMPARE(test->mRepeat, false);

    test->mRepeat = false;
    QSignalSpy spy( test, SIGNAL(repeatChanged(bool) ) );
    test->mSettingsManager->emitValueChanged(repeatProfileKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(true) );
    QCOMPARE(test->mRepeat, true);

}

/*!
 testValueChangedPresetCase.
 */ 
void TestMpSettingsManager::testValueChangedPresetCase()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
      
    XQSettingsKey presetProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingPresetIdKey);
    test->mPreset = 1;
    test->valueChanged(presetProfileKey, QVariant(0));
    QCOMPARE(test->mPreset, 0);

    test->mPreset = 0;
    QSignalSpy spy( test, SIGNAL(presetChanged(int) ) );
    test->mSettingsManager->emitValueChanged(presetProfileKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(1) );
    QCOMPARE(test->mPreset, 1);

}

/*!
 testValueChangedInspireMe.
 */ 
void TestMpSettingsManager::testValueChangedInspireMe()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
      
    XQSettingsKey InspireMeKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingInspireMeKey);
    test->mInspireMe = true;
    test->valueChanged(InspireMeKey, QVariant(0));
    QCOMPARE(test->mInspireMe, false);

    test->mInspireMe = false;
    QSignalSpy spy( test, SIGNAL(InspireMeChanged(bool) ) );
    test->mSettingsManager->emitValueChanged(InspireMeKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(true) );
    QCOMPARE(test->mInspireMe, true);
}

/*!
 testValueChangedSongDetailsGb.
 */ 
void TestMpSettingsManager::testValueChangedSongDetailsGb()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
      
    XQSettingsKey SongDetailsGbKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingSongDetailsGBKey);
    test->mSongDetailsGb = true;
    test->valueChanged(SongDetailsGbKey, QVariant(0));
    QCOMPARE(test->mSongDetailsGb, false);

    test->mSongDetailsGb = false;
    QSignalSpy spy( test, SIGNAL(SongDetailsGbChanged(bool) ) );
    test->mSettingsManager->emitValueChanged(SongDetailsGbKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(true) );
    QCOMPARE(test->mSongDetailsGb, true);
}

/*!
 testValueChangedShowMtpInfo.
 */ 
void TestMpSettingsManager::testValueChangedShowMtpInfo()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
      
    XQSettingsKey showMtpInfo(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMusicPlayerShowMtpInfoKey);
    test->mShowMtpInfo = true;
    test->valueChanged(showMtpInfo, QVariant(0));
    QCOMPARE(test->mShowMtpInfo, false);

    test->mShowMtpInfo = false;
    test->mSettingsManager->emitValueChanged(showMtpInfo, QVariant(1));
    QCOMPARE(test->mShowMtpInfo, true);
}
// End of file

