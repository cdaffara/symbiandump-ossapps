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

#include "cpprofilemodel.h"
#include "cpprofilemodel_p.h"
#include <QtCore/QStringList>

/*!
   Contructor
 */
CpProfileModel::CpProfileModel(QObject *parent /*=0*/):QObject(parent),
                                                       d_ptr(new CpProfileModelPrivate())
{
    d_ptr->initialize(this);    
}

/*!
   Destrutor
 */
CpProfileModel::~CpProfileModel()
{
    delete d_ptr;
}

/*!
   Get profile name with its id
 */
QString CpProfileModel::profileName(int profileId) const
{
    return d_ptr->profileName(profileId);
}

/*!
   Set profile id \a profileId 's name with \a name.
*/
void CpProfileModel::setProfileName(int profileId, const QString &name)
{
    d_ptr->setProfileName(profileId, name);
}

/*!
   get profile name list 
 */
QStringList CpProfileModel::profileNames()const
{
    return d_ptr->profileNames();
}
/*!
   Activate a profile with its id, return the result code.
 */
int CpProfileModel::activateProfile(int profileId)
{
    return d_ptr->activateProfile(profileId);
}

/*!
   Get active profile's id
 */
int CpProfileModel::activeProfileId() const
{
    return d_ptr->activeProfileId();
}

/*!
     Get profile settings, store in center repository keys 
 */
void CpProfileModel::profileSettings(int profileId, CpProfileSettings& profileSettings)
{
    d_ptr->profileSettings(profileId, profileSettings);
}

/*!
     Set profile settings from center repository keys
 */
void CpProfileModel::setProfileSettings(int profileId, CpProfileSettings& profileSettings )
{
    return d_ptr->setProfileSettings(profileId, profileSettings);
}

/*!
     Get ring tone of active profile
 */
QString CpProfileModel::ringTone() const
{
    return d_ptr->ringTone();
}

/*!
     Set ring tone for all profiles
 */
void CpProfileModel::setRingTone(const QString& filePath)
{
    d_ptr->setRingTone(filePath);
}

/*!
     Return ringing volume of device
 */
int CpProfileModel::masterVolume() const
{
    return d_ptr->masterVolume();
}

/*!
     Set device's ringing volume
 */
void CpProfileModel::setMasterVolume(int volume)
{
    return d_ptr->setMasterVolume(volume);
}

/*!
     Return the master vibra's status of device
 */
bool CpProfileModel::masterVibra() const
{
    return d_ptr->masterVibra();
}

/*!
     Set the master vibra's status of device
 */
void CpProfileModel::setMasterVibra(bool isVibra)
{
    d_ptr->setMasterVibra(isVibra);
}

/*!
     Return the silence mode of device
 */
bool CpProfileModel::silenceMode() const
{
    return d_ptr->silenceMode();
}

/*!
     Set silence mode for deivce
 */
void CpProfileModel::setSilenceMode(bool isSilence)
{
    d_ptr->setSilenceMode(isSilence);
}

/*!
   Get profile's ring tone, if the profile id is invalid, always return an empty string
   \param profileId profile's id
 */
QString CpProfileModel::ringTone(int profileId)const
{
    return d_ptr->ringTone(profileId);
}

/*!
   Set ring tone for a profile, if the profile id is invalid, nothing happens
   \param profileId profile's id
   \param filePath ring tone's path
 */
void CpProfileModel::setRingTone(int profileId, const QString& filePath)
{
    d_ptr->setRingTone(profileId, filePath);
}

/*!
   Get message tone's name, if the profile id is invalid, always return an empty string
   \param profileId profile's id
 */
QString CpProfileModel::messageTone(int profileId) const
{
    return d_ptr->messageTone(profileId);
}

/*!
   Set message tone for a profile, if the profile id is invalid, nothing happens
   \param profileId profile's id
   \param filePath message tone's path
 */
void CpProfileModel::setMessageTone(int profileId, const QString& filePath)
{
    d_ptr->setMessageTone(profileId, filePath);
}

/*!
   Get email tone's name, if the profile id is invalid, always return an empty string
   \param profileId profile's id
 */
QString CpProfileModel::emailTone(int profileId) const
{
    return d_ptr->emailTone(profileId);
}

/*!
   Set email tone for a profile, if the profile id is invalid, nothing happens
   \param profileId profile's id
   \param filePath message tone's path
 */
void CpProfileModel::setEmailTone(int profileId, const QString& filePath)
{
    d_ptr->setEmailTone(profileId,filePath);
}

/*!
   Get reminder tone's name, if the profile id is invalid, always return an empty string
   \param profileId profile's id
 */
QString CpProfileModel::reminderTone(int profileId) const
{
    return d_ptr->reminderTone(profileId);
}

/*!
   Set reminder tone for a profile, if the profile id is invalid, nothing happens
   \param profileId profile's id
   \param filePath reminder tone's path
 */
void CpProfileModel::setReminderTone(int profileId, const QString& filePath)
{
    d_ptr->setReminderTone(profileId,filePath);
}

/*!
   Get notification tone's status, if the profile id is invalid, always return false
   \param profileId profile's id
   \return return the status of notification tone, true value stands for the tone being on
 */
bool CpProfileModel::notificationTone(int profileId) const
{
    return d_ptr->notificationTone(profileId);
}

/*!
   Set notification tone's status for a profile, if the profile id is invalid, nothing happens
   \param profileId profile's id
   \return isActive the status of notification tone
 */
void CpProfileModel::setNotificationTone(int profileId, bool isActive)
{
    d_ptr->setNotificationTone(profileId, isActive);
}

/*!
     Get key & touch screen tone's value, if the profile id is invalid, always return 0
 */
int CpProfileModel::keyTouchScreenTone(int profileId) const
{
    return d_ptr->keyTouchScreenTone(profileId);
}

/*!
     set key & touch screen tone, if the profile id is invalid, nothing happens
     \param profileId identify the profile
     \param level 0-5
 */
void CpProfileModel::setKeyTouchScreenTone(int profileId, int level)
{
    d_ptr->setKeyTouchScreenTone(profileId,level);
}

/*!
   Get key & touch screen vibra's value, if the profile id is invalid, always return 0
   \param profileId identify the profile
 */
int CpProfileModel::keyTouchScreenVibra(int profileId) const
{
    return d_ptr->keyTouchScreenVibra(profileId);
}

/*!
   Set key touch screen vibar for a profile, if the profile id is invalid, nothing happens
   \param profileId identify the profile
   \param level 0-5
 */
void CpProfileModel::setKeyTouchScreenVibra(int profileId, int level)
{
    d_ptr->setKeyTouchScreenVibra(profileId,level);
}

// End of file

