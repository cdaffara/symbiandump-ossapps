/* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*       test the functions in cpprofilemodel class 
*/

#include "ut_cpprofilemodel.h"


#include <QtTest/QtTest>

#include <QtCore/QStringList>
#include <cpprofilemodel.h>
#include <hbpushbutton.h>
#include <hbtranslator.h>
#include <profile.hrh>

/*!
    \class TestCpProfileModel \n
    \brief describe the test case's goal, like: \n
      class name: cpprofilemodel \n
      class's description: \n
      type of test case: unit test\n 
      test cases' number totally: \n
 */

void TestCpProfileModel::initTestCase()
{
    //translate the hbTrId text in control panel.
    HbTranslator translator("control_panel");
    translator.loadCommon();
    ringTonePath1 = QString("C:") + QDir::separator() + QString("resource") + QDir::separator() + QString("cptestdata") + QDir::separator() + QString("sounds") + QDir::separator() + QString("testsound.aac");
    ringTonePath2 = QString("C:") + QDir::separator() + QString("resource") + QDir::separator() + QString("cptestdata") + QDir::separator() + QString("sounds") + QDir::separator() + QString("testsound2.aac");
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: CpProfileModel(QObject *parent = 0); \n
     2. Case Descrition: verify the constructor can work correctly. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> parent = 0; \n&nbsp;&nbsp;
        <2> parent = new QObject(); \n
     4. Expected result: \n&nbsp;&nbsp;
        no crash \n
 */
void TestCpProfileModel::testConstructor()
{
    QObject *pObject = new QObject();
    
    //test constructor without parent.
    CpProfileModel *profileModel = new CpProfileModel(0);
    QVERIFY( profileModel != 0 );
    delete profileModel;
    profileModel = 0;
    // test constructor with parent.
    profileModel = new CpProfileModel(pObject);
    QVERIFY( profileModel != 0 );
    
    delete pObject;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString profileName(int profileId)const; \n
     2. Case Descrition: verify that it can return the corresponding profile name when using valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId,\n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId,\n
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> return QString( "General" ) \n &nbsp;&nbsp;
        <2> return QString( "Meeting" ) \n
 */
/*void TestCpProfileModel::testProfileNameWithValidProfileID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    QString profileName1 = profileModel->profileName(EProfileWrapperGeneralId);
    QVERIFY( profileName1 == QString( "General" ) );
    
    QString profileName2 = profileModel->profileName(EProfileWrapperMeetingId);
    QVERIFY( profileName2 == QString("Meeting") );
    
    delete profileModel;
}*/
/*!
     Test Case Description:\n 
     1. Fucntion Name: QString profileName(int profileId)const; \n
     2. Case Descrition: verify that it doesn't crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -80, \n &nbsp;&nbsp;
        <3> profileId = 888, \n &nbsp;&nbsp;
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> return QString() \n &nbsp;&nbsp;
        <2> return QString() \n &nbsp;&nbsp;
        <3> return QString() \n &nbsp;&nbsp;
        <4> return QString() \n &nbsp;&nbsp;
 */
void TestCpProfileModel::testProfileNameWithInvalidProfileID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    QString profileName1 = profileModel->profileName(EProfileWapperStart);
    QVERIFY( profileName1 == QString() );
    
    QString profileName2 = profileModel->profileName(EPRofileWrapperEnd);
    QVERIFY( profileName2 == QString() );
    
    QString profileName3 = profileModel->profileName( -80 );
    QVERIFY( profileName3 == QString() );
    
    QString profileName4 = profileModel->profileName( 888 );
    QVERIFY( profileName4 == QString() );
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QStringList profileNames()const; \n
     2. Case Descrition: Verify that the profile name list can be get correctly with this function \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        none \n
     4. Expected result: \n &nbsp;&nbsp;
         \n
 */
/*void TestCpProfileModel::testProfileNames()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    QStringList profilesNames = profileModel->profileNames();
    
    // Verify the right profile names are returned.
    QVERIFY ( profilesNames.count() == 2 ); 
    QVERIFY( profilesNames.contains("General", Qt::CaseInsensitive));
    QVERIFY( profilesNames.contains("Meeting", Qt::CaseInsensitive));    
    
    delete profileModel;
}*/

/*!
     Test Case Description:\n 
     1. Fucntion Name: int activateProfile(int profileId); \n
     2. Case Descrition: Verify that the profile cannot be actived with the invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -6 \n &nbsp;&nbsp;
        <4> profileId = 356 \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return KErrNotFound \n &nbsp;&nbsp;
        <2> return KErrNotFound \n &nbsp;&nbsp;
        <3> return KErrNotFound \n &nbsp;&nbsp;
        <4> return KErrNotFound \n 
 */
void TestCpProfileModel::testActivateProfileWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    int retErr1 = profileModel->activateProfile(EProfileWapperStart);
    QVERIFY( retErr1 == KErrNotFound );
    
    int retErr2 = profileModel->activateProfile(EPRofileWrapperEnd);
    QVERIFY( retErr2 == KErrNotFound );
    
    int retErr3 = profileModel->activateProfile( -6 );
    QVERIFY( retErr3 == KErrNotFound );
    
    int retErr4 = profileModel->activateProfile( 356 );
    QVERIFY( retErr4 == KErrNotFound );
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: int activateProfile(int profileId); \n
     2. Case Descrition: Verify that the profile can be actived with the valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId,\n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId,\n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> the current active profile ID is EProfileWrapperGeneralId \n &nbsp;&nbsp;
        <2> the current active profile ID is EProfileWrapperMeetingId \n
 */
void TestCpProfileModel::testActivateProfileWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->activateProfile(EProfileWrapperGeneralId);
    QVERIFY( profileModel->activeProfileId() == EProfileWrapperGeneralId );
    
    profileModel->activateProfile(EProfileWrapperMeetingId);
    QVERIFY( profileModel->activeProfileId() == EProfileWrapperMeetingId );
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: int activeProfileId() const;\n
     2. Case Descrition: Verify that it can return the right profile ID or Error code. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> set an active valid profile \n &nbsp;&nbsp;
        <2> set an active invaild profile \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right ID \n &nbsp;&nbsp;
        <2> no crash \n
 */
void TestCpProfileModel::testActiveProfileId()
{
    CpProfileModel *profileModel = new CpProfileModel();   
    
    profileModel->activateProfile(EProfileWrapperGeneralId);
    QVERIFY( profileModel->activeProfileId() == EProfileWrapperGeneralId );
    
    profileModel->activateProfile(-8);
    // set profile failed, so the active profileID is still the previous ID, EProfileWrapperGeneralId.
    QVERIFY( profileModel->activeProfileId() == EProfileWrapperGeneralId );  
    
    delete profileModel;
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: void profileSettings(int profileId, CpProfileSettings& profileSettings);\n
     2. Case Descrition: get profile settings,\n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileID = EProfileWrapperGeneralId, profileSettings = CpProfileSettings; \n &nbsp;&nbsp;
        <2> profileID = EProfileWrapperMeetingId, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
        <3> profileID = EProfileWapperStart, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
        <4> profileID = EPRofileWrapperEnd, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
        <5> profileID = int, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        no crash \n
 */
void TestCpProfileModel::testProfileSettings()
{
    CpProfileSettings profileSettings;    
    CpProfileModel *profileModel = new CpProfileModel();
    profileSettings.mKeyTouchScreenVibra = 0;
    profileSettings.mKeyTouchScreenTone = 2;
    // test with valid profile ID.
    profileModel->setProfileSettings( EProfileWrapperGeneralId, profileSettings );
    profileModel->profileSettings( EProfileWrapperGeneralId, profileSettings );
    QVERIFY( profileSettings.mKeyTouchScreenTone == 2);
    
    profileModel->setProfileSettings( EProfileWrapperMeetingId, profileSettings );
    profileModel->profileSettings( EProfileWrapperMeetingId, profileSettings );
    QVERIFY( profileSettings.mKeyTouchScreenVibra == 0);
    // test with invalid profile ID.
    profileModel->profileSettings( EProfileWapperStart, profileSettings );

    profileModel->profileSettings( EPRofileWrapperEnd, profileSettings );

    profileModel->profileSettings( 98, profileSettings );    
    
    delete profileModel;
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: int setProfileSettings(int profileId, CpProfileSettings& profileSettings );\n
     2. Case Descrition: Set profile settings from center repository keys \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileID = EProfileWrapperGeneralId, profileSettings = CpProfileSettings; \n &nbsp;&nbsp;
        <2> profileID = EProfileWrapperMeetingId, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
        <3> profileID = EProfileWapperStart, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
        <4> profileID = EPRofileWrapperEnd, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
        <5> profileID = 98, profileSettings = CpProfileSettings;  \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        no crash \n
 */
void TestCpProfileModel::testSetProfileSettings()
{
    CpProfileSettings profileSettings;
    profileSettings.mKeyTouchScreenVibra = 4;
    profileSettings.mKeyTouchScreenTone = 3;
    CpProfileModel *profileModel = new CpProfileModel();
    // test with valid profile ID.
    profileModel->setProfileSettings( EProfileWrapperGeneralId, profileSettings );
    profileModel->profileSettings( EProfileWrapperGeneralId, profileSettings );
    QVERIFY( profileSettings.mKeyTouchScreenTone == 3);
    
    profileModel->setProfileSettings( EProfileWrapperMeetingId, profileSettings );
    profileModel->profileSettings( EProfileWrapperMeetingId, profileSettings );
    QVERIFY( profileSettings.mKeyTouchScreenVibra == 4);
    
    // test with invalid profile ID.
    profileModel->setProfileSettings( EProfileWapperStart, profileSettings );
    profileModel->setProfileSettings( EPRofileWrapperEnd, profileSettings );
    profileModel->setProfileSettings( 98, profileSettings );    
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString ringTone() const;\n
     2. Case Descrition: Verify that it return the right active ring tone path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> set ringtone for profiles.  \n &nbsp;&nbsp;
        <2> no ringtone is set. \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right path \n &nbsp;&nbsp;
        <2> return QString() \n
 */
void TestCpProfileModel::testRingToneOfActive()
{
    CpProfileModel *profileModel = new CpProfileModel();
    // set a ringtone for profiles.
    profileModel->setRingTone(ringTonePath1);
    QString retRingTonePath1 = profileModel->ringTone();
    QVERIFY( retRingTonePath1 == ringTonePath1 );
    // no ringtone is set.
    profileModel->setRingTone(QString());
    QString retRingTonePath2 = profileModel->ringTone();
    QVERIFY( retRingTonePath2 == QString() );
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setRingTone(const QString& filePath);\n
     2. Case Descrition: Verify that it can set ring tone successfully with valid sound path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> soundPath = QString(), \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->ringTone() == soundPath \n
 */
void TestCpProfileModel::testSetRingToneAllWithValidPath()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setRingTone(ringTonePath1);
    QVERIFY( profileModel->ringTone() == ringTonePath1 );
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setRingTone(const QString& filePath);\n
     2. Case Descrition: Verify that no crash when setting ring tone with invalid sound path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(), \n &nbsp;&nbsp;
        <2> path = QString(XX), XX is an invalid path \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->ringTone() == QString() \n &nbsp;&nbsp;
        <2> profileModel->ringTone() == path \n &nbsp;&nbsp;
 */
void TestCpProfileModel::testSetRingToneAllWithInvalidPath()
{
    QString inValidPath = "Z:/InvalidSoundPath";
    CpProfileModel *profileModel = new CpProfileModel();
    // using empty path.
    profileModel->setRingTone( QString() );    
    QVERIFY( profileModel->ringTone() == QString() );
    // using an invalid path.
    profileModel->setRingTone( QString( inValidPath ) ); 
    QString retStr = profileModel->ringTone();    
    QVERIFY( profileModel->ringTone() == inValidPath );
    
    delete profileModel;
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: int void setMasterVolume(int volume);\n
     2. Case Descrition: Verify that the valid volume can be set correctly \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> volume = int X, X = {1,2,3,4,5,6,7,8,9,10} \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->masterVolume() == X. \n
 */
void TestCpProfileModel::testSetMasterWithValidVolume()
{
    CpProfileModel *profileModel = new CpProfileModel();    

    for( int i = EProfileRingingVolumeLevel1; i <= EProfileRingingVolumeLevel10; i++ )
        {
            profileModel->setMasterVolume(i);
            QVERIFY( profileModel->masterVolume() == i );
        }
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: int void setMasterVolume(int volume);\n
     2. Case Descrition: Verify that no crash when using  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> volume = -8 \n &nbsp;&nbsp;
        <2> volume = 230 \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash and the master volume is not changed. \n &nbsp;&nbsp;
        <2> no crash and the master volume is not changed. \n 
 */
void TestCpProfileModel::testSetMasterWithInvalidVolume()
{
    CpProfileModel *profileModel = new CpProfileModel(); 
    int oldVolume = profileModel->masterVolume();
    
    profileModel->setMasterVolume( -8 );
    QVERIFY( profileModel->masterVolume() == oldVolume );
    
    profileModel->setMasterVolume( 230 );
    QVERIFY( profileModel->masterVolume() == oldVolume );
    
    delete profileModel;        
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: int masterVolume() const;\n
     2. Case Descrition: Verify that it returns the right master volume for device. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> setMasterVolume( EProfileRingingVolumeLevel3 ) \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> returnMasterVolume == EProfileRingingVolumeLevel3 \n
 */
void TestCpProfileModel::testMasterVolume()
{
    CpProfileModel *profileModel = new CpProfileModel();
    profileModel->setMasterVolume( EProfileRingingVolumeLevel3 );
    
    int returnMasterVolume = profileModel->masterVolume();
    QVERIFY( returnMasterVolume == EProfileRingingVolumeLevel3 );
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
         void setMasterVibra(bool isVibra);\n         
     2. Case Descrition: Verify that the master vibra's status can be set successfully. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> isVibra = true; \n  &nbsp;&nbsp;
        <2> isVibra = false; \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->masterVibra() == true. \n &nbsp;&nbsp;
        <1> profileModel->masterVibra() == false. \n
 */
void TestCpProfileModel::testSetMasterVibra()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setMasterVibra(true);    
    QVERIFY( profileModel->masterVibra() == true );
    
    profileModel->setMasterVibra(false);
    QVERIFY( profileModel->masterVibra() == false );    
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
         bool masterVibra() const;\n         
     2. Case Descrition: Verify that it returns the correct master vibra's status. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> isVibra = true; \n  &nbsp;&nbsp;
        <2> isVibra = false; \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->masterVibra() == true. \n &nbsp;&nbsp;
        <1> profileModel->masterVibra() == false. \n
 */
void TestCpProfileModel::testMasterVibra()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setMasterVibra(true);    
    QVERIFY( profileModel->masterVibra() == true );
    
    profileModel->setMasterVibra(false);
    QVERIFY( profileModel->masterVibra() == false );    
    
    delete profileModel;
}
/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        void setSilenceMode(bool isSlience);\n
     2. Case Descrition: Verify the right slicence mode can be set. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> isSlience = true; \n  &nbsp;&nbsp;
        <2> isSlience = false; \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->silenceMode() == true. \n &nbsp;&nbsp;
        <1> profileModel->silenceMode() == false. \n
 */
void TestCpProfileModel::testSetSilenceMode()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setSilenceMode(true);
    QVERIFY( profileModel->silenceMode() == true );

    profileModel->setSilenceMode(false);
    QVERIFY( profileModel->silenceMode() == false ); 
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        bool silenceMode() const;\n &nbsp;&nbsp;
        void setSilenceMode(bool isSlience);\n
     2. Case Descrition: Verify it get the correct silence mode of device. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> isSlience = true; \n  &nbsp;&nbsp;
        <2> isSlience = false; \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->silenceMode() == true. \n &nbsp;&nbsp;
        <1> profileModel->silenceMode() == false. \n
 */
void TestCpProfileModel::testSilenceMode()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setSilenceMode(true);
    QVERIFY( profileModel->silenceMode() == true );

    profileModel->setSilenceMode(false);
    QVERIFY( profileModel->silenceMode() == false ); 
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString ringTone(int profileId)const; \n
     2. Case Descrition: Verify that it can return the right ringtone when using valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, set ringtone with ringTonePath1 \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, set ringtone with ringTonePath2 \n        
     4. Expected result: \n&nbsp;&nbsp;
        <1> profileModel->ringTone(EProfileWrapperGeneralId) == ringTonePath1  \n &nbsp;&nbsp;
        <2> profileModel->ringTone(EProfileWrapperMeetingId) == ringTonePath2  \n     
 */
void TestCpProfileModel::testRingToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setRingTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->ringTone(EProfileWrapperGeneralId) == ringTonePath1 );
    
    profileModel->setRingTone( EProfileWrapperMeetingId, ringTonePath2 );
    QVERIFY( profileModel->ringTone(EProfileWrapperMeetingId) == ringTonePath2 );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString ringTone(int profileId)const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 100,\n &nbsp;&nbsp;       
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash and return QString() \n &nbsp;&nbsp;
        <2> no crash and return QString() \n &nbsp;&nbsp;
        <3> no crash and return QString() \n &nbsp;&nbsp;
        <4> no crash and return QString() \n 
 */
void TestCpProfileModel::testRingToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    // set ring tone for all profile
    profileModel->setRingTone(ringTonePath1);
    
    QVERIFY( profileModel->ringTone( EProfileWapperStart ) == QString() );
    QVERIFY( profileModel->ringTone( EPRofileWrapperEnd ) == QString() );
    QVERIFY( profileModel->ringTone( -9 ) == QString() );
    QVERIFY( profileModel->ringTone( 100 ) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setRingTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it can set the profile ringtone successfully with valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, filePath = QString(ringTonePath) \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, filePath = QString() \n 
     4. Expected result: \n&nbsp;&nbsp;
        <1> profileModel->ringTone(EProfileWrapperGeneralId) == ringTonePath \n &nbsp;&nbsp;
        <2> profileModel->ringTone(EProfileWrapperMeetingId) == QString() \n 
 */
void TestCpProfileModel::testSetRingToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setRingTone(EProfileWrapperGeneralId, ringTonePath2);
    QVERIFY( profileModel->ringTone(EProfileWrapperGeneralId) == ringTonePath2 );
    
    profileModel->setRingTone(EProfileWrapperMeetingId, QString());
    QVERIFY( profileModel->ringTone(EProfileWrapperMeetingId) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setRingTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -19,\n &nbsp;&nbsp;
        <4> profileId = 101,\n
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n &nbsp;&nbsp;
        <4> no crash \n 
 */
void TestCpProfileModel::testSetRingToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setRingTone(EProfileWapperStart, ringTonePath2);
    
    profileModel->setRingTone(EPRofileWrapperEnd, ringTonePath2);
    
    profileModel->setRingTone(-19, ringTonePath2);
    
    profileModel->setRingTone(101, ringTonePath2);
    
    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString messageTone(int profileId) const; \n
     2. Case Descrition: Verify that it can return the right message tone when using valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, set ringtone with soundPath1 \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, set ringtone with soundTonePath2 \n  
     4. Expected result: \n&nbsp;&nbsp;
        <1> profileModel->messageTone(EProfileWrapperGeneralId) == soundPath1  \n &nbsp;&nbsp;
        <2> profileModel->messageTone(EProfileWrapperMeetingId) == soundPath2  \n  
 */
void TestCpProfileModel::testMessageToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setMessageTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->messageTone(EProfileWrapperGeneralId) == ringTonePath1 );
    
    profileModel->setMessageTone( EProfileWrapperMeetingId, ringTonePath2 );
    QVERIFY( profileModel->messageTone(EProfileWrapperMeetingId) == ringTonePath2 );

    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString messageTone(int profileId) const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 59,\n 
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash and return QString() \n &nbsp;&nbsp;
        <2> no crash and return QString() \n &nbsp;&nbsp;
        <3> no crash and return QString() \n &nbsp;&nbsp;
        <4> no crash and return QString() \n 
 */
void TestCpProfileModel::testMessageToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();

    QVERIFY( profileModel->messageTone( EProfileWapperStart ) == QString() );
    QVERIFY( profileModel->messageTone( EPRofileWrapperEnd ) == QString() );
    QVERIFY( profileModel->messageTone( -9 ) == QString() );
    QVERIFY( profileModel->messageTone( 59 ) == QString() );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setMessageTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it can set the message tone successfully with valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, filePath = QString(soundPath) \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, filePath = QString() \n 
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> profileModel->messageTone(EProfileWrapperGeneralId) == ringTonePath \n &nbsp;&nbsp;
        <2> profileModel->messageTone(EProfileWrapperMeetingId) == QString() \n 
 */
void TestCpProfileModel::testSetMessageToneWithValidID()
{
    QString soundPath2 = QString("C:/unavailable path");
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setMessageTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->messageTone(EProfileWrapperGeneralId) == ringTonePath1 );
    // set an unavailable path.
    profileModel->setMessageTone( EProfileWrapperGeneralId, soundPath2 );
    QVERIFY( profileModel->messageTone(EProfileWrapperGeneralId) == soundPath2 );
    
    profileModel->setMessageTone( EProfileWrapperMeetingId, QString() );
    QVERIFY( profileModel->messageTone(EProfileWrapperMeetingId) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setMessageTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -100,\n &nbsp;&nbsp;
        <4> profileId = 100,\n 
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n &nbsp;&nbsp;
        <4> no crash \n 
 */
void TestCpProfileModel::testSetMessageToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setMessageTone( EProfileWapperStart, ringTonePath1 );
//    QVERIFY( profileModel->messageTone(EProfileWapperStart) == QString() );

    profileModel->setMessageTone( EPRofileWrapperEnd, QString() );
//    QVERIFY( profileModel->messageTone(EPRofileWrapperEnd) == QString() );
    
    profileModel->setMessageTone( -100, ringTonePath1 );
//    QVERIFY( profileModel->messageTone(-100) == QString() );
    
    profileModel->setMessageTone( 100, QString() );
//    QVERIFY( profileModel->messageTone(100) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString emailTone(int profileId) const; \n
     2. Case Descrition: Verify that it can return the right email tone when using valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, set ringtone with soundPath1 \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, set ringtone with soundTonePath2 \n  
     4. Expected result: \n&nbsp;&nbsp;
        <1> profileModel->emailTone(EProfileWrapperGeneralId) == soundPath1  \n &nbsp;&nbsp;
        <2> profileModel->emailTone(EProfileWrapperMeetingId) == soundPath2  \n  
 */
void TestCpProfileModel::testEmailToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setEmailTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->emailTone(EProfileWrapperGeneralId) == ringTonePath1 );
    
    profileModel->setEmailTone( EProfileWrapperMeetingId, ringTonePath2 );
    QVERIFY( profileModel->emailTone(EProfileWrapperMeetingId) == ringTonePath2 );

    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString emailTone(int profileId) const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 59,\n 
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash and return QString() \n &nbsp;&nbsp;
        <2> no crash and return QString() \n &nbsp;&nbsp;
        <3> no crash and return QString() \n &nbsp;&nbsp;
        <4> no crash and return QString() \n 
 */
void TestCpProfileModel::testEmailToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();

    QVERIFY( profileModel->emailTone( EProfileWapperStart ) == QString() );
    QVERIFY( profileModel->emailTone( EPRofileWrapperEnd ) == QString() );
    QVERIFY( profileModel->emailTone( -9 ) == QString() );
    QVERIFY( profileModel->emailTone( 59 ) == QString() );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setEmailTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it can set the email tone successfully with valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, filePath = QString(soundPath) \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, filePath = QString() \n 
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> profileModel->emailTone(EProfileWrapperGeneralId) == soundPath \n &nbsp;&nbsp;
        <2> profileModel->emailTone(EProfileWrapperMeetingId) == QString() \n 
 */
void TestCpProfileModel::testSetEmailToneWithValidID()
{
    QString soundPath2 = QString("C:/unavailable path");
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setEmailTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->emailTone(EProfileWrapperGeneralId) == ringTonePath1 );
    // set an unavailable path.
    profileModel->setEmailTone( EProfileWrapperGeneralId, soundPath2 );
    QVERIFY( profileModel->emailTone(EProfileWrapperGeneralId) == soundPath2 );
    
    profileModel->setEmailTone( EProfileWrapperMeetingId, QString() );
    QVERIFY( profileModel->emailTone(EProfileWrapperMeetingId) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setEmailTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -100,\n &nbsp;&nbsp;
        <4> profileId = 100,\n 
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n &nbsp;&nbsp;
        <4> no crash \n 
 */
void TestCpProfileModel::testSetEmailToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setEmailTone( EProfileWapperStart, ringTonePath1 );
//    QVERIFY( profileModel->emailTone(EProfileWapperStart) == QString() );

    profileModel->setEmailTone( EPRofileWrapperEnd, QString() );
//    QVERIFY( profileModel->emailTone(EPRofileWrapperEnd) == QString() );
    
    profileModel->setEmailTone( -100, ringTonePath1 );
//    QVERIFY( profileModel->emailTone(-100) == QString() );
    
    profileModel->setEmailTone( 100, QString() );
//    QVERIFY( profileModel->emailTone(100) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString reminderTone(int profileId) const; \n
     2. Case Descrition: Verify that it can return the right reminder tone when using valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, set ringtone with soundPath1 \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, set ringtone with soundTonePath2 \n  
     4. Expected result: \n&nbsp;&nbsp;
        <1> profileModel->reminderTone(EProfileWrapperGeneralId) == soundPath1  \n &nbsp;&nbsp;
        <2> profileModel->reminderTone(EProfileWrapperMeetingId) == soundPath2  \n  
 */
void TestCpProfileModel::testReminderToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setReminderTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->reminderTone(EProfileWrapperGeneralId) == ringTonePath1 );
    
    profileModel->setReminderTone( EProfileWrapperMeetingId, ringTonePath2 );
    QVERIFY( profileModel->reminderTone(EProfileWrapperMeetingId) == ringTonePath2 );

    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString reminderTone(int profileId) const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 59,\n 
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash and return QString() \n &nbsp;&nbsp;
        <2> no crash and return QString() \n &nbsp;&nbsp;
        <3> no crash and return QString() \n &nbsp;&nbsp;
        <4> no crash and return QString() \n 
 */
void TestCpProfileModel::testReminderToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();

    QVERIFY( profileModel->reminderTone( EProfileWapperStart ) == QString() );
    QVERIFY( profileModel->reminderTone( EPRofileWrapperEnd ) == QString() );
    QVERIFY( profileModel->reminderTone( -9 ) == QString() );
    QVERIFY( profileModel->reminderTone( 59 ) == QString() );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setReminderTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it can set the reminder tone successfully with valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, filePath = QString(soundPath) \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, filePath = QString() \n 
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> profileModel->reminderTone(EProfileWrapperGeneralId) == soundPath \n &nbsp;&nbsp;
        <2> profileModel->reminderTone(EProfileWrapperMeetingId) == QString() \n 
 */
void TestCpProfileModel::testSetReminderToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setReminderTone( EProfileWrapperGeneralId, ringTonePath1 );
    QVERIFY( profileModel->reminderTone(EProfileWrapperGeneralId) == ringTonePath1 );
    // set an unavailable path.
    profileModel->setReminderTone( EProfileWrapperGeneralId, ringTonePath2 );
    QVERIFY( profileModel->reminderTone(EProfileWrapperGeneralId) == ringTonePath2 );
    
    profileModel->setReminderTone( EProfileWrapperMeetingId, QString() );
    QVERIFY( profileModel->reminderTone(EProfileWrapperMeetingId) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setReminderTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -100,\n &nbsp;&nbsp;
        <4> profileId = 100,\n 
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n &nbsp;&nbsp;
        <4> no crash \n 
 */
void TestCpProfileModel::testSetReminderToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setReminderTone( EProfileWapperStart, ringTonePath1 );
//    QVERIFY( profileModel->reminderTone(EProfileWapperStart) == QString() );

    profileModel->setReminderTone( EPRofileWrapperEnd, QString() );
//    QVERIFY( profileModel->reminderTone(EPRofileWrapperEnd) == QString() );
    
    profileModel->setReminderTone( -100, ringTonePath1 );
//    QVERIFY( profileModel->reminderTone(-100) == QString() );
    
    profileModel->setReminderTone( 100, QString() );
//    QVERIFY( profileModel->reminderTone(100) == QString() );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString notificationTone(int profileId) const; \n
     2. Case Descrition: Verify that it can return the right notification tone when using valid profile ID. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, isActive = true \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, isActive = false \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->notificationTone(EProfileWrapperGeneralId) == true  \n &nbsp;&nbsp;
        <2> profileModel->notificationTone(EProfileWrapperMeetingId) == false  \n  
 */
void TestCpProfileModel::testNotificationToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setNotificationTone( EProfileWrapperGeneralId, true );
    QVERIFY( profileModel->notificationTone(EProfileWrapperGeneralId) == true );
    
    profileModel->setNotificationTone( EProfileWrapperMeetingId, false );
    QVERIFY( profileModel->notificationTone(EProfileWrapperMeetingId) == false );

    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: QString notificationTone(int profileId) const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 59,\n 
     4. Expected result: \n&nbsp;&nbsp;
        <1> no crash and return QString() \n &nbsp;&nbsp;
        <2> no crash and return QString() \n &nbsp;&nbsp;
        <3> no crash and return QString() \n &nbsp;&nbsp;
        <4> no crash and return QString() \n 
 */
void TestCpProfileModel::testNotificationToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();

    QVERIFY( profileModel->notificationTone( EProfileWapperStart ) == false );
    QVERIFY( profileModel->notificationTone( EPRofileWrapperEnd ) == false );
    QVERIFY( profileModel->notificationTone( -9 ) == false );
    QVERIFY( profileModel->notificationTone( 59 ) == false );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: void setNotificationTone(int profileId, const QString& filePath); \n
     2. Case Descrition: Verify that it can set the notification tone successfully with valid profile ID. \n
     3. Input Parameters:  \n&nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, isActive = true \n &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, isActive = false \n 
     4. Expected result: \n&nbsp;&nbsp;\n &nbsp;&nbsp;
        <1> profileModel->notificationTone(EProfileWrapperGeneralId) == true \n &nbsp;&nbsp;
        <2> profileModel->notificationTone(EProfileWrapperMeetingId) == false \n 
 */
void TestCpProfileModel::testSetNotificationTone()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setNotificationTone( EProfileWrapperGeneralId, true );
    QVERIFY( profileModel->notificationTone(EProfileWrapperGeneralId) == true );

    profileModel->setNotificationTone( EProfileWrapperGeneralId, false );
    QVERIFY( profileModel->notificationTone(EProfileWrapperGeneralId) == false );

    delete profileModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        void setKeyTouchScreenTone(int profileId, int level);\n
     2. Case Descrition: Verify that the tone of the valid profile can be set with valid level value. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, level = int X (X = 0,1,2,3,4,5); \n  &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, level = int X; \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->keyTouchScreenTone(EProfileWrapperGeneralId) == X. \n &nbsp;&nbsp;
        <2> profileModel->keyTouchScreenTone(EProfileWrapperMeetingId) == X. \n
 */
void TestCpProfileModel::testSetKeyTouchScreenToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    int i = 0;
    for ( ; i <= 5; i++ ) {
        profileModel->setKeyTouchScreenTone( EProfileWrapperGeneralId, i );
        QVERIFY( profileModel->keyTouchScreenTone( EProfileWrapperGeneralId ) == i );
        
        profileModel->setKeyTouchScreenTone( EProfileWrapperMeetingId, i );
        QVERIFY( profileModel->keyTouchScreenTone( EProfileWrapperMeetingId ) == i );
    }
    
    profileModel->setKeyTouchScreenTone( EProfileWrapperGeneralId, 12 );
    int b = profileModel->keyTouchScreenTone( EProfileWrapperGeneralId );
    
    profileModel->setKeyTouchScreenTone( EProfileWrapperMeetingId, -12 );
    int c = profileModel->keyTouchScreenTone( EProfileWrapperMeetingId );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        void setKeyTouchScreenTone(int profileId, int level);\n
     2. Case Descrition: Verify that it does not crash with invalid level value. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWapperStart, level = int X \n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd, level = int X \n &nbsp;&nbsp;
        <3> profileId = -8, level = int X \n &nbsp;&nbsp;
        <4> profileId = 99, level = int X \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n &nbsp;&nbsp;
        <4> no crash \n 
 */
void TestCpProfileModel::testSetKeyTouchScreenToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setKeyTouchScreenTone( EProfileWapperStart, 4 );    
    
    profileModel->setKeyTouchScreenTone( EPRofileWrapperEnd, 2 );

    profileModel->setKeyTouchScreenTone( -8, 4 );
    
    profileModel->setKeyTouchScreenTone( 99, 3 );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        int keyTouchScreenTone(int profileId) const; \n
     2. Case Descrition: Verify that the tone can be get with valid profile ID. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId \n  &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right tone level. \n &nbsp;&nbsp;
        <2> return the right tone level. \n 
 */
void TestCpProfileModel::testKeyTouchScreenToneWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setKeyTouchScreenTone( EProfileWrapperGeneralId, 4 );
    QVERIFY( profileModel->keyTouchScreenTone( EProfileWrapperGeneralId ) == 4);
    
    profileModel->setKeyTouchScreenTone( EProfileWrapperMeetingId, 5 );
    QVERIFY( profileModel->keyTouchScreenTone( EProfileWrapperMeetingId ) == 5);
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        int keyTouchScreenTone(int profileId) const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 100,\n     
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash and return 0 \n &nbsp;&nbsp;
        <2> no crash and return 0 \n &nbsp;&nbsp;
        <3> no crash and return 0 \n &nbsp;&nbsp;
        <4> no crash and return 0 \n 
 */
void TestCpProfileModel::testKeyTouchScreenToneWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    QVERIFY( profileModel->keyTouchScreenTone( EProfileWapperStart ) == 0);
    QVERIFY( profileModel->keyTouchScreenTone( EPRofileWrapperEnd ) == 0);
    QVERIFY( profileModel->keyTouchScreenTone( -9 ) == 0);
    QVERIFY( profileModel->keyTouchScreenTone( 100 ) == 0);
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        void setKeyTouchScreenVibra(int profileId, int level);\n
     2. Case Descrition: Verify that the vibra mode of the valid profile can be set with valid level value. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId, level = int X (X = 0,1,2,3,4,5); \n  &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId, level = int X; \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> profileModel->keyTouchScreenVibra(EProfileWrapperGeneralId) == X. \n &nbsp;&nbsp;
        <2> profileModel->keyTouchScreenVibra(EProfileWrapperMeetingId) == X. \n
 */
void TestCpProfileModel::testSetKeyTouchScreenVibraWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    int i = 0;
    for ( ; i <= 5; i++ ) {
        profileModel->setKeyTouchScreenVibra( EProfileWrapperGeneralId, i );
        QVERIFY( profileModel->keyTouchScreenVibra( EProfileWrapperGeneralId ) == i );
        
        profileModel->setKeyTouchScreenVibra( EProfileWrapperMeetingId, i );
        QVERIFY( profileModel->keyTouchScreenVibra( EProfileWrapperMeetingId ) == i );
    }
    
    profileModel->setKeyTouchScreenVibra( EProfileWrapperGeneralId, 12 );
    int b = profileModel->keyTouchScreenVibra( EProfileWrapperGeneralId );
    
    profileModel->setKeyTouchScreenVibra( EProfileWrapperMeetingId, -12 );
    int c = profileModel->keyTouchScreenVibra( EProfileWrapperMeetingId );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        void setKeyTouchScreenVibra(int profileId, int level);\n
     2. Case Descrition: Verify that it does not crash with the invalid level value. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWapperStart, level = int X \n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd, level = int X \n &nbsp;&nbsp;
        <3> profileId = -8, level = int X \n &nbsp;&nbsp;
        <4> profileId = 99, level = int X \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n &nbsp;&nbsp;
        <4> no crash \n 
 */
void TestCpProfileModel::testSetKeyTouchScreenVibraWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setKeyTouchScreenVibra( EProfileWapperStart, 4 );    
    
    profileModel->setKeyTouchScreenVibra( EPRofileWrapperEnd, 2 );

    profileModel->setKeyTouchScreenVibra( -8, 4 );
    
    profileModel->setKeyTouchScreenVibra( 99, 3 );
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        int keyTouchScreenVibra(int profileId) const; \n
     2. Case Descrition: Verify that the vibra value can be get with valid profile ID. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWrapperGeneralId \n  &nbsp;&nbsp;
        <2> profileId = EProfileWrapperMeetingId \n  
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right key touch screen vibra's value. \n &nbsp;&nbsp;
        <2> return the right key touch screen vibra's value. \n 
 */
void TestCpProfileModel::testKeyTouchScreenVibraWithValidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    profileModel->setKeyTouchScreenVibra( EProfileWrapperGeneralId, 4 );
    QVERIFY( profileModel->keyTouchScreenVibra( EProfileWrapperGeneralId ) == 4);
    
    profileModel->setKeyTouchScreenVibra( EProfileWrapperMeetingId, 5 );
    QVERIFY( profileModel->keyTouchScreenVibra( EProfileWrapperMeetingId ) == 5);
    
    delete profileModel;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        int keyTouchScreenVibra(int profileId) const; \n
     2. Case Descrition: Verify that it does not crash when using invalid profile ID. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> profileId = EProfileWapperStart,\n &nbsp;&nbsp;
        <2> profileId = EPRofileWrapperEnd,\n &nbsp;&nbsp;
        <3> profileId = -9,\n &nbsp;&nbsp;
        <4> profileId = 100,\n     
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash and return 0 \n &nbsp;&nbsp;
        <2> no crash and return 0 \n &nbsp;&nbsp;
        <3> no crash and return 0 \n &nbsp;&nbsp;
        <4> no crash and return 0 \n 
 */
void TestCpProfileModel::testKeyTouchScreenVibraWithInvalidID()
{
    CpProfileModel *profileModel = new CpProfileModel();
    
    QVERIFY( profileModel->keyTouchScreenVibra( EProfileWapperStart ) == 0);
    QVERIFY( profileModel->keyTouchScreenVibra( EPRofileWrapperEnd ) == 0);
    QVERIFY( profileModel->keyTouchScreenVibra( -9 ) == 0);
    QVERIFY( profileModel->keyTouchScreenVibra( 100 ) == 0);
    
    delete profileModel;    
}

/*!
    Descrition of what you will do in this function
 */
void TestCpProfileModel::cleanupTestCase()
{
    // release all test data
    QCoreApplication::processEvents();
}

QTEST_MAIN(TestCpProfileModel)
