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
*
*/

#include "cpprofilemodel_p.h"
#include "cpprofilemodel.h"
#include <cplogger.h>
#include <e32base.h>
#include <QString>
#include <QMap>
#include <MProfileEngineExtended2.h>
#include <MProfileExtended.h>
#include <MProfileName.h>
#include <MProfileTones.h>
#include <MProfileSetTones.h>
#include <MProfileExtraTones.h>
#include <MProfileSetExtraTones.h>
#include <MProfileExtraSettings.h>
#include <MProfileSetExtraSettings.h>
#include <MProfileFeedbackSettings.h>
#include <MProfileSetFeedbackSettings.h>
#include <MProfilesNamesArray.h>
#include <MProfileSetName.h>
#include <settingsinternalcrkeys.h>
#include <hbglobal.h>
#include <QtCore/QStringList>
#include <MProfileExtended2.h>
#include <MProfileSetExtraTones2.h>
#include <MProfileExtraTones2.h>
#include <MProfileVibraSettings.h>
#include <MProfileSetVibraSettings.h>
#include <TProfileToneSettings.h>
#include <hwrmvibrasdkcrkeys.h>
#include <centralrepository.h>
#include <XQConversions>
#include <profile.hrh>
/*
 * Constructor
 */
CpProfileModelPrivate::CpProfileModelPrivate()
    : mEngine(0),      
      q_ptr(0)
{
    
}

/*
 * Initialize the profile engine and available profile list for profile wrapper. 
 */
void CpProfileModelPrivate::initialize(CpProfileModel *parent)
{
    q_ptr = parent;
    CPFW_LOG("CpProfileModelPrivate::CpProfileModelPrivate(), START.");
    TRAP_IGNORE(
        mEngine = CreateProfileEngineExtended2L();
        /*
         * Currently, engine part will return all previous version of profile
         * so some invalid profile will be added in the new list, to avoid this 
         * use hard code to get the right list of profile. 
         */
        /*MProfilesNamesArray* nameList = mEngine->ProfilesNamesArrayLC();
        int profileCount = nameList->MdcaCount();
        for (int i = 0; i<profileCount; i++) {
            MProfileName *profileName = nameList->ProfileName(i);
            mProfileList.insert(profileName->Id(), mEngine->Profile2L(profileName->Id())); 
        }
        CleanupStack::PopAndDestroy(*nameList);*/           
    );
    mProfileList.append(static_cast<int>(EProfileWrapperGeneralId)); // general id 
    mProfileList.append(static_cast<int>(EProfileWrapperMeetingId)); // meeting id     
    CPFW_LOG("CpProfileModelPrivate::CpProfileModelPrivate(), END");
}    

/*
 * Destructor 
 */
CpProfileModelPrivate::~CpProfileModelPrivate()
{
    if (mEngine!=0) {
        mEngine->Release();
    }
	mProfileList.clear();
}

/*
 * Get profile name with its id
 */
QString CpProfileModelPrivate::profileName(int profileId) const
{
    CPFW_LOG("CpProfileModelPrivate::profileName(), START.");
    // Return an empty string if id is not valid.    
    if (!isValidProfile(profileId)) {
        return QString();
    }
    QString profileName;
    QT_TRAP_THROWING(        
        MProfile *profile = mEngine->Profile2L(profileId);
        const MProfileName &name = profile->ProfileName();        
        if (name.Name().Length() > 0) {
            profileName = XQConversions::s60DescToQString(name.Name());
        }
        profile->Release();
    )
    return profileName;    
    
}

/*
 * Set profile name with \a profileId.
 */
void CpProfileModelPrivate::setProfileName(int profileId, const QString &name)
{
    if(!isValidProfile(profileId)) {
        return;
    }
    HBufC *des = XQConversions::qStringToS60Desc( name );
    QT_TRAP_THROWING(
        CleanupStack::PushL(des);
        MProfileExtended *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        MProfileSetName &profileSetName = profileExtend->ProfileSetName();
        profileSetName.SetNameL(*des);
        mEngine ->CommitChangeL(*profileExtend);
        CleanupStack::Pop(2); // des, profileExtend
        profileExtend->Release();
    )    
}

/*
 * Get available profiles' name list.
 */
QStringList CpProfileModelPrivate::profileNames() const
{
    CPFW_LOG("CpProfileModelPrivate::profileNames(), START.");
    QStringList nameList;    
    foreach(int profileId, mProfileList) {               
        QT_TRAP_THROWING(        
            MProfile *profile = mEngine->Profile2L(profileId);
            const MProfileName &name = profile->ProfileName();
            if (name.Name().Length() > 0) {
                nameList.append(XQConversions::s60DescToQString(name.Name()));
            }
            profile->Release();
        )
    }
    CPFW_LOG("CpProfileModelPrivate::profileNames(), END.");
    return nameList;
}


/*
 * Activate a profile with its id, return the result.
 */
int CpProfileModelPrivate::activateProfile(int profileId)
{
    CPFW_LOG("CpProfileModelPrivate::activateProfile(), START.");
    // currently, only two profile remains: general and meeting,
    // But profile engine also support the old profile like:
    // silence, out ...
    // so filter the invalid profile id first.
    if (!isValidProfile(profileId)) {
        return KErrNotFound;
    }
    TRAPD( err, 
        mEngine->SetActiveProfileL( profileId );
    );
    CPFW_LOG("CpProfileModelPrivate::activateProfile(), END.");
    return err;
}

/*
 * Get active profile's id
 */
int CpProfileModelPrivate::activeProfileId() const
{
    return mEngine->ActiveProfileId();
}

/*
 * Return all profile settings according to profile's id
 */
void CpProfileModelPrivate::profileSettings(int profileId,
        CpProfileSettings& profileSettings)
{
    if (!isValidProfile(profileId)) {
        return;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        
        const MProfileTones &setTones = profileExtend->ProfileTones();
        const TProfileToneSettings &toneSettings = setTones.ToneSettings();
        const MProfileExtraTones2 &extTones = profileExtend->ProfileExtraTones2();
        const MProfileVibraSettings &vibraSettings =
                profileExtend->ProfileVibraSettings();
        const MProfileExtraSettings &extraSettings =
                profileExtend->ProfileExtraSettings();
        const MProfileFeedbackSettings &feedbackSettings =
                extraSettings.ProfileFeedbackSettings();
               
        profileSettings.mRingTone = XQConversions::s60DescToQString(setTones.RingingTone1());
        profileSettings.mMessageTone = XQConversions::s60DescToQString(setTones.MessageAlertTone());
        profileSettings.mEmailTone = XQConversions::s60DescToQString(extTones.EmailAlertTone());
        profileSettings.mReminderTone = XQConversions::s60DescToQString(extTones.ReminderTone());
        profileSettings.mNotificationTone = toneSettings.iWarningAndGameTones;
        
        // only use Keypad Volume as a base value for display in key & touch screen setting option
        profileSettings.mKeyTouchScreenTone = toneSettings.iKeypadVolume;
        profileSettings.mKeyTouchScreenVibra = feedbackSettings.TactileFeedback();
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();    
    );
}
/*
 *   set profile settings
 */
void CpProfileModelPrivate::setProfileSettings(int profileId, CpProfileSettings& profileSettings)
{
    if (!isValidProfile(profileId)) {
        return;
    }
    
    QT_TRAP_THROWING (
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId); 
        CleanupStack::PushL(profileExtend);
        
        MProfileSetTones &setTones = profileExtend->ProfileSetTones();
        TProfileToneSettings &toneSettings = setTones.SetToneSettings();
        MProfileSetExtraTones2 &setExtTones =
                profileExtend->ProfileSetExtraTones2();
        MProfileSetVibraSettings &setVibraSettings =
                profileExtend->ProfileSetVibraSettings();
        MProfileSetExtraSettings &extraSettings =
                profileExtend->ProfileSetExtraSettings();
        MProfileSetFeedbackSettings &setFeedbackSettings =
                extraSettings.ProfileSetFeedbackSettings();
        
        HBufC *setting = XQConversions::qStringToS60Desc(profileSettings.mRingTone);
        CleanupStack::PushL(setting);
        setTones.SetRingingTone1L(*setting);
        CleanupStack::PopAndDestroy();
        
        setting = XQConversions::qStringToS60Desc(profileSettings.mMessageTone);
        CleanupStack::PushL(setting);
        setTones.SetMessageAlertToneL(*setting);
        CleanupStack::PopAndDestroy();
        
        setting = XQConversions::qStringToS60Desc(profileSettings.mEmailTone);
        CleanupStack::PushL(setting);
        setExtTones.SetEmailAlertToneL(*setting);
        CleanupStack::PopAndDestroy();
        
        setting = XQConversions::qStringToS60Desc(profileSettings.mReminderTone);
        CleanupStack::PushL(setting);
        setExtTones.SetReminderToneL(*setting);
        CleanupStack::PopAndDestroy();

        toneSettings.iWarningAndGameTones
                        = profileSettings.mNotificationTone;
        // Change the keypad volume and touch screen tone together
        toneSettings.iKeypadVolume
                        = static_cast<TProfileKeypadVolume> (profileSettings.mKeyTouchScreenTone);    
        setFeedbackSettings.SetAudioFeedback(
                        static_cast<TProfileAudioFeedback> (profileSettings.mKeyTouchScreenTone));
        setFeedbackSettings.SetTactileFeedback(
                        static_cast<TProfileTactileFeedback> (profileSettings.mKeyTouchScreenVibra));
        
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::Pop(); // profileExtend
         )
}

/*
 * Get the active profile's ring tone name
 */
QString CpProfileModelPrivate::ringTone() const
{
    // return empty string when active profile id is invalid,
    // some old application still set the profile which is not available now,
    // this check can be removed when every application use a correct profile id    

    QString ringTone;
    if (!isValidProfile(activeProfileId())) {
        return ringTone;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(activeProfileId());
        CleanupStack::PushL(profileExtend);
        const MProfileTones &setTones = profileExtend->ProfileTones();
        
        ringTone = XQConversions::s60DescToQString(setTones.RingingTone1());
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
        )
    
    return ringTone;
}

/*
 * Set the ring tone for all profiles
 */
void CpProfileModelPrivate::setRingTone(const QString& filePath)
{   
    for (TInt i = 0; i < mProfileList.count(); ++i) {  
        QT_TRAP_THROWING(
            MProfileExtended2 *profileExtend = mEngine->Profile2L(mProfileList.at(i));
            CleanupStack::PushL(profileExtend);
            
            MProfileSetTones &setTones = profileExtend->ProfileSetTones();
            HBufC *ringTone =  XQConversions::qStringToS60Desc(filePath);
            CleanupStack::PushL(ringTone);
            setTones.SetRingingTone1L( *ringTone );
            mEngine ->CommitChangeL(*profileExtend);
            CleanupStack::PopAndDestroy();
            CleanupStack::Pop(); // profileExtend
            profileExtend->Release();
        )
    }     
}

/*
 *  Get the ringing volume value 
 */
int CpProfileModelPrivate::masterVolume() const
{
    int masterVolume = 0;
    QT_TRAP_THROWING(masterVolume = mEngine->MasterVolumeL();)
    return masterVolume;
}

/*
 * Set the ringing volume
 */
void CpProfileModelPrivate::setMasterVolume(int volume)
{
    // the volume range 1-10
    if (volume >= EProfileRingingVolumeLevel1 && volume <= EProfileRingingVolumeLevel10) {
        QT_TRAP_THROWING(mEngine->SetMasterVolumeL( volume );)
    }
}
/*
 * Get the master vibra's status   
 */
bool CpProfileModelPrivate::masterVibra() const
{
    bool masterVibra = false;
    QT_TRAP_THROWING(masterVibra = mEngine->MasterVibraL();)
    return masterVibra; 
}

/*
 * Set master vibra's status
 */
void CpProfileModelPrivate::setMasterVibra(bool isVibra)
{
    QT_TRAP_THROWING(mEngine->SetMasterVibraL( isVibra );)    
}

/*
 * Get the status of silence mode.
 */
bool CpProfileModelPrivate::silenceMode() const
{
    bool isSlience = false;
    QT_TRAP_THROWING(isSlience = mEngine->SilenceModeL();)
    return isSlience;
}

/*
 * Set the status of silence mode
 */
void CpProfileModelPrivate::setSilenceMode(bool isSilence)
{
    QT_TRAP_THROWING(mEngine->SetSilenceModeL( isSilence );)    
}

/*
 * Return the ring tone of a profile, if the profile id is invalid, always
 * return an empty string
 */
QString CpProfileModelPrivate::ringTone(int profileId) const
{
    QString ringTone;
    if(!isValidProfile(profileId)) {
        return ringTone;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend =  mEngine->Profile2L(profileId); 
        CleanupStack::PushL(profileExtend);
        
        const MProfileTones &setTones = profileExtend->ProfileTones();
        
        ringTone = XQConversions::s60DescToQString(setTones.RingingTone1());
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )    
    return ringTone;
}

/*
 * Set the ring tone for a profile, if the profile id is invalid, nothing happens
 */
void CpProfileModelPrivate::setRingTone(int profileId, const QString& filePath)
{
    if(!isValidProfile(profileId)) {
        return;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        
        MProfileSetTones &setTones = profileExtend->ProfileSetTones();
        HBufC *ringTone =  XQConversions::qStringToS60Desc(filePath);
        CleanupStack::PushL(ringTone);
        setTones.SetRingingTone1L(*ringTone);
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::PopAndDestroy(); //ringTone
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )            
}

/*
 * Get the message tone of a profile, if the profile id is invalid, always return 
 * an empty string 
 */
QString CpProfileModelPrivate::messageTone(int profileId) const
{
    QString messageTone;
    if(!isValidProfile(profileId)) {
        return messageTone;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId); 
        CleanupStack::PushL(profileExtend);
        
        const MProfileTones &setTones = profileExtend->ProfileTones();
        
        messageTone = XQConversions::s60DescToQString(setTones.MessageAlertTone());
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
    
    return messageTone;    
}

/*
 * Set the message tone of a profile, if the profile id is invalid, nothing happens
 */
void CpProfileModelPrivate::setMessageTone(int profileId, const QString& filePath)
{
    if(!isValidProfile(profileId)) {
        return;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        MProfileSetTones &setTones =
            profileExtend->ProfileSetTones();
        HBufC *messageTone = XQConversions::qStringToS60Desc(filePath);
        CleanupStack::PushL(messageTone);
        setTones.SetMessageAlertToneL(*messageTone);
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::PopAndDestroy();//messageTone
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )                
}

/*
 * Get the email tone of a profile, if profile id is invalid, return an empty string
 */
QString CpProfileModelPrivate::emailTone(int profileId) const
{
    QString emailTone;
    
    if(!isValidProfile(profileId)) {
        return emailTone;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);  
        CleanupStack::PushL(profileExtend);
        const MProfileExtraTones2 &extTones =
                profileExtend->ProfileExtraTones2();

        emailTone = XQConversions::s60DescToQString(extTones.EmailAlertTone());
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
    return emailTone;
}

/*
 * Set the email tone for a profile, if the profile id is invalid, nothing happens
 */
void CpProfileModelPrivate::setEmailTone(int profileId, const QString& filePath)
{
    if(!isValidProfile(profileId)) {
        return ;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        MProfileSetExtraTones2 &setExtTones =
                profileExtend->ProfileSetExtraTones2();
        HBufC *emailTone =  XQConversions::qStringToS60Desc(filePath);
        CleanupStack::PushL(emailTone);
        setExtTones.SetEmailAlertToneL(*emailTone);
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::PopAndDestroy();//emailtone;
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
}

/*
 * Get a reminder tone for a profile, if the profile id is invalid,
 * always return an emtpy string 
 */
QString CpProfileModelPrivate::reminderTone(int profileId) const
{
    QString reminderTone;
    if(!isValidProfile(profileId)) {
        return reminderTone;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        const MProfileExtraTones2 &extTones = profileExtend->ProfileExtraTones2();
        
        reminderTone = XQConversions::s60DescToQString(extTones.ReminderTone());
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
    return reminderTone;        
}

/*
 * Set a reminder tone for a profile, if the profile id is invalid,
 * nothing happens
 */
void CpProfileModelPrivate::setReminderTone(int profileId, const QString& filePath)
{
    if(!isValidProfile(profileId)) {
        return;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        
        MProfileSetExtraTones2 &setExtTones = profileExtend->ProfileSetExtraTones2();
        HBufC *reminderTone = XQConversions::qStringToS60Desc(filePath);
        CleanupStack::PushL(reminderTone);
        setExtTones.SetReminderToneL( *reminderTone );
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::PopAndDestroy(); //reminderTone;
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )            
}

/*
 * Get the status of notification tone, if the profile id is invalid,
 * always return false
 */
bool CpProfileModelPrivate::notificationTone(int profileId) const
{
    bool ret = false;
    if(!isValidProfile(profileId)) {
        return false;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        
        const MProfileTones &setTones = profileExtend->ProfileTones();
        const TProfileToneSettings &toneSettings = setTones.ToneSettings();
        ret = toneSettings.iWarningAndGameTones;
        
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
    return ret;
}

/*
 * Set the status of notification tone, if the profile id is
 * invalid, nothing happens
 */
void CpProfileModelPrivate::setNotificationTone(int profileId, bool isActive)
{
    if(!isValidProfile(profileId)) {
        return ;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        MProfileSetTones &setTones = profileExtend->ProfileSetTones();
        TProfileToneSettings &toneSettings = setTones.SetToneSettings();

        toneSettings.iWarningAndGameTones = isActive;

        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
}
/*
 * Get key & touch screen tone's value, if the profile id
 * is invalid, always return 0
 */
int CpProfileModelPrivate::keyTouchScreenTone(int profileId) const
{
    int level = 0;
    if(!isValidProfile(profileId)) {
        return level;
    }
    
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);

        const MProfileTones &setTones = profileExtend->ProfileTones();
        const TProfileToneSettings &toneSettings = setTones.ToneSettings();
        
        // Return only keypad volume, but touch tone volume will be synchronized in 
        // SetKeyTouchScreenTone(), these two settings also have the same default value
        // in cenrep key
        level = toneSettings.iKeypadVolume;  
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
    return level;
}
/*
 *   set key & touch screen tone, if the profile id
 *   is invalid, nothing happens
 */
void CpProfileModelPrivate::setKeyTouchScreenTone(int profileId, int level)
{   
    if(!isValidProfile(profileId)) {
        return ;
    }
    QT_TRAP_THROWING(

        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);
        MProfileSetTones &setTones =
                profileExtend->ProfileSetTones();
        TProfileToneSettings &toneSettings =
                setTones.SetToneSettings();
        
        MProfileSetExtraSettings &extraSettings =
                    profileExtend->ProfileSetExtraSettings();
        MProfileSetFeedbackSettings &setFeedbackSettings =
                    extraSettings.ProfileSetFeedbackSettings();
        
        // Update the key pad volume and touch tone volume together
        toneSettings.iKeypadVolume
                = static_cast<TProfileKeypadVolume> (level);
        
        setFeedbackSettings.SetAudioFeedback(
                static_cast<TProfileAudioFeedback> (level));
        
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )    
}

/*
 * Get key touch screen vibra's value of a profile, return 0 if the
 * profile id is invalid  
 */
int CpProfileModelPrivate::keyTouchScreenVibra(int profileId)const
{
    int level = 0;
    if(!isValidProfile(profileId)) {
        return level;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);    
        const MProfileExtraSettings &extraSettings =
                    profileExtend->ProfileExtraSettings();
        const MProfileFeedbackSettings &feedbackSettings =
                    extraSettings.ProfileFeedbackSettings();
        level = feedbackSettings.TactileFeedback();

        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )
    
    return level;
}

/*
 * Set key & touch screen vibra for a profile, 
 * if the profile id is invalid, nothing happens
 */
void CpProfileModelPrivate::setKeyTouchScreenVibra(int profileId, int level)
{
    if(!isValidProfile(profileId)) {
        return ;
    }
    QT_TRAP_THROWING(
        MProfileExtended2 *profileExtend = mEngine->Profile2L(profileId);
        CleanupStack::PushL(profileExtend);    

        MProfileSetExtraSettings &extraSettings =
                profileExtend->ProfileSetExtraSettings();
        MProfileSetFeedbackSettings &setFeedbackSettings =
                extraSettings.ProfileSetFeedbackSettings();
        setFeedbackSettings.SetTactileFeedback(
                static_cast<TProfileTactileFeedback> (level));
        mEngine->CommitChangeL(*profileExtend);
        CleanupStack::Pop(); // profileExtend
        profileExtend->Release();
    )                
}

/*
 * Judge the profile is valid or not 
 */

bool CpProfileModelPrivate::isValidProfile(int profileId) const
{
    return mProfileList.contains(profileId);
}

// End of file


