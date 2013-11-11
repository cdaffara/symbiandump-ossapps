/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*       test the functions in cppluginconfigreader class 
*/

#ifndef UT_CPPROFILEMODEL_H_ 
#define UT_CPPROFILEMODEL_H_

class CpPluginConfig;
#include <QObject>

class TestCpProfileModel :public QObject
{
    Q_OBJECT
private slots:
    // init function
    void initTestCase();
    
    void testConstructor(); // test the constructor.    

    //void testProfileNameWithValidProfileID(); // test the profileName() function with valid profile ID. 
    void testProfileNameWithInvalidProfileID(); // test the profileName() function with invalid profile ID.
    //void testProfileNames(); // test the profileNames() function.
   
    void testActivateProfileWithInvalidID(); //test the activateProfile() function with invalid profile ID.
    void testActivateProfileWithValidID(); // test the activateProfile() function with valid profile ID.
    void testActiveProfileId(); // test the activeProfileId() function.
    
    void testProfileSettings(); // test the profileSettings() function.
    void testSetProfileSettings(); // test the setProfileSettings() function.
    
    void testRingToneOfActive(); // test the ringTone() function.
    void testSetRingToneAllWithValidPath(); // test the setRingTone() function with valid sound path.
    void testSetRingToneAllWithInvalidPath(); // test the setRingTone() function with invalid sound path.
    
    void testSetMasterWithValidVolume(); // test the setMasterVolume() function with valid volume value.
    void testSetMasterWithInvalidVolume(); // test the setMasterVolume() function with invalid volume value.
    void testMasterVolume(); // test the masterVolume() function.
    
    void testSetMasterVibra(); // test the setMasterVibra() functions.
    void testMasterVibra(); // test the masterVibra() functions.
    
    void testSetSilenceMode(); // test the setSilenceMode() functions.
    void testSilenceMode(); // test the silenceMode() functions.
    
    void testRingToneWithValidID(); // test the ringTone(int profileId) function with valid profile ID.
    void testRingToneWithInvalidID(); // test the ringTone(int profileId) function with invalid profile ID.
    void testSetRingToneWithValidID(); // test the  setRingTone(int profileId, const QString& filePath) function with valid profile ID.
    void testSetRingToneWithInvalidID(); // test the  setRingTone(int profileId, const QString& filePath) function with invalid profile ID.
    
    void testMessageToneWithValidID(); // test MessageTone() function with valid profile ID.
    void testMessageToneWithInvalidID(); // test MessageTone() function with valid profile ID.
    void testSetMessageToneWithValidID(); // test the setMessageTone() function with valid profile ID.
    void testSetMessageToneWithInvalidID(); // test the setMessageTone() function with invalid profile ID.
    
    void testEmailToneWithValidID(); // test emailTone() function with valid profile ID.
    void testEmailToneWithInvalidID(); // test emailTone() function with valid profile ID.
    void testSetEmailToneWithValidID(); // test the setEmailTone() function with valid profile ID.
    void testSetEmailToneWithInvalidID(); // test the setEmailTone() function with invalid profile ID.
    
    void testReminderToneWithValidID(); // test reminderTone() function with valid profile ID.
    void testReminderToneWithInvalidID(); // test reminderTone() function with valid profile ID.
    void testSetReminderToneWithValidID(); // test the setReminderTone() function with valid profile ID.
    void testSetReminderToneWithInvalidID(); // test the setReminderTone() function with invalid profile ID.
    
    void testNotificationToneWithValidID(); // test notificationTone() function with valid profile ID.
    void testNotificationToneWithInvalidID(); // test notificationTone() function with valid profile ID.
    void testSetNotificationTone(); // test the setNotificationTone() function with valid profile ID.
    
    void testSetKeyTouchScreenToneWithValidID(); // test the setKeyTouchScreenTone()function with valid profile ID.
    void testSetKeyTouchScreenToneWithInvalidID(); // test the setKeyTouchScreenTone()function with invalid profile ID.
    void testKeyTouchScreenToneWithValidID(); // test the keyTouchScreenTone()function with valid profile ID.
    void testKeyTouchScreenToneWithInvalidID(); // test the keyTouchScreenTone()function with invalid profile ID.
    
    void testSetKeyTouchScreenVibraWithValidID(); // test the setKeyTouchScreenVibra()function with valid profile ID.
    void testSetKeyTouchScreenVibraWithInvalidID(); // test the setKeyTouchScreenVibra()function with invalid profile ID.
    void testKeyTouchScreenVibraWithValidID(); // test the keyTouchScreenVibra()function with valid profile ID.
    void testKeyTouchScreenVibraWithInvalidID(); // test the keyTouchScreenVibra()function with invalid profile ID.

    void cleanupTestCase();    
private:
   QString ringTonePath1;
   QString ringTonePath2;
};
#endif /* UT_CPPROFILEMODEL_H_ */
