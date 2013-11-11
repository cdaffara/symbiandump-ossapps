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
#include <QString>

CpProfileModelPrivate::CpProfileModelPrivate()
{
}

CpProfileModelPrivate::~CpProfileModelPrivate()
{
}

/*
 * Get the result of the initiation
 */
int CpProfileModelPrivate::initiationFlag()
{
    return -1;
}

/*
 * Get profile name with its id
 */
QString CpProfileModelPrivate::profileName(int profileId)
{
    Q_UNUSED(profileId);
    return "";
}

/*
 * Set profile name with \a profileId.
 */
int CpProfileModelPrivate::setProfileName(int profileId, const QString &name)
{
    Q_UNUSED(profileId);
    Q_UNUSED(name);
    return -1;
}

/*
 * Activate a profile with its id, return the operation code.
 */
int CpProfileModelPrivate::activateProfile(int profileId)
{
    Q_UNUSED(profileId);
    return -1;
}

/*
 * Get active profile's id
 */
int CpProfileModelPrivate::activeProfileId()
{
    return -1;
}

/*
 * Get path and file name of ring tone file
 */
QString CpProfileModelPrivate::ringTone()
{
    return "";
}

/*
 * Set path and file to ring tone
 */
int CpProfileModelPrivate::setRingTone(const QString& filePath)
{
    Q_UNUSED(filePath);
	return -1;
}

/*
 * Get path and file name of message tone file
 */
QString CpProfileModelPrivate::messageTone()
{
    return "";
}

/*
 * Set path and file to message tone
 */
int CpProfileModelPrivate::setMessageTone(const QString& filePath)
{
    Q_UNUSED(filePath);
	return -1;
}

/*
 * Get path and file name of email tone file
 */
QString CpProfileModelPrivate::emailTone()
{
    return "";
}

/*
 * Set path and file to email tone
 */
int CpProfileModelPrivate::setEmailTone(const QString& filePath)
{
    Q_UNUSED(filePath);
	return -1;
}

/*
 * Get path and file name of calendar event tone file
 */
QString CpProfileModelPrivate::calendarTone()
{
    return "";
}

/*
 * Set path and file to calendar event tone
 */
void CpProfileModelPrivate::setCalendarTone(const QString& filePath)
{
    Q_UNUSED(filePath);
}

/*
 * Get path and file name of clock alarm tone file
 */
QString CpProfileModelPrivate::alarmTone()
{
    return "";
}

/*
 * Set path and file to clock alarm tone
 */
void CpProfileModelPrivate::setAlarmTone(const QString& filePath)
{
    Q_UNUSED(filePath);
}

/*
 * Get the value of master volume
 */
int CpProfileModelPrivate::ringVolume()
{
    return -1;
}

/*
 * Set master volume, the value should be between 1-10
 */
void CpProfileModelPrivate::setRingVolume(int volume)
{
    Q_UNUSED(volume);
}

/*
 * Activate master volume to beep
 */
void CpProfileModelPrivate::activateBeep()
{;
}

/*
 * Get beep status in master volume
 */
bool CpProfileModelPrivate::isBeep()
{
    return false;
}

/*
 * Activate master volume to silent
 */
void CpProfileModelPrivate::activateSilent()
{
}

/*
 * Get silent status in master volume
 */
bool CpProfileModelPrivate::isSilent()
{
    return false;
}

/*
 * Get master vibra's status
 */
bool CpProfileModelPrivate::vibraStatus()
{
    return false;
}

/*
 * Set master vibra's status
 */
void CpProfileModelPrivate::setVibraStatus(bool status)
{
    Q_UNUSED(status);
}

/*
 * Get keypad' volume
 */
int CpProfileModelPrivate::keyVolume()
{
    return -1;
}

/*
 * Set keypad's volume, 
 * the value of the volume should be between 0-3
 */
void CpProfileModelPrivate::setKeyVolume(int volume)
{
    Q_UNUSED(volume);
}

/*
 * Get screen tone's volume
 */
int CpProfileModelPrivate::screenVolume()
{
    return -1;
}

/*
 * Set screen tone's volume, 
 * the value of the volume should be between 0-3
 */
void CpProfileModelPrivate::setScreenVolume(int volume)
{
    Q_UNUSED(volume);
}

/*
 * Get screen vibra's level
 */
int CpProfileModelPrivate::screenVibra()
{
    return -1;
}

/*
 * Set screen vibra's level, 
 * the value of the level should be between 0-3
 */
void CpProfileModelPrivate::setScreenVibra(int volume)
{
    Q_UNUSED(volume);
}


// End of file
