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

#include <xqsettingsmanager.h>
#include <xqcentralrepositoryutils.h>
#include <xqcentralrepositorysearchcriteria.h>
#include <qlist.h>

#include "ipssosextendedsettingsmanager.h"
#include "nmipssettingitems.h"
#include "ipssettingkeys.h"
#include "nmcommon.h"

/*!
    \class NmIpsSosExtendedSettingsManager
    \brief The class is used for manipulating extended POP3 and IMAP4 account settings stored
    in the Central Repository.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor    
    Creates the NmIpsSosExtendedSettingsManagerfor loading and saving extended mailbox settings.
    \param mailboxId Mailbox identifier.
*/
NmIpsSosExtendedSettingsManager::NmIpsSosExtendedSettingsManager(const NmId &mailboxId)
    : mMailboxId(mailboxId), mMailboxOffset(-1), mActiveProfileOffset(-1)
{
    mSettingsManager = new XQSettingsManager();

    calculateMailboxOffset();
}

/*!
    Destructor
*/
NmIpsSosExtendedSettingsManager::~NmIpsSosExtendedSettingsManager()
{
    delete mSettingsManager;
}

/*!     
    Reads extended mailbox setting.
    \param settingItem SettingItem enum of the setting to return.
    \param QVariant SettingValue of the found setting value.
    \return bool <true> when the setting item was read, otherwise <false>.
*/
bool NmIpsSosExtendedSettingsManager::readSetting(IpsServices::SettingItem settingItem,
                                               QVariant &settingValue) const
{
    bool ret(mMailboxOffset>=0);
    if(ret) {
        switch(settingItem) {
            case IpsServices::DownloadPictures:
                settingValue = readFromCenRep(IpsServices::EmailKeyPreferenceDownloadPictures);
                break;
            case IpsServices::MessageDivider:
                settingValue = readFromCenRep(IpsServices::EmailKeyPreferenceMessageDivider);
                break;
            case IpsServices::ReceptionActiveProfile:
                settingValue = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);
                break;
            case IpsServices::ReceptionUserDefinedProfile:
                settingValue = readFromCenRep(IpsServices::EmailKeyReceptionUserDefinedProfile);
                break;
            case IpsServices::ReceptionInboxSyncWindow:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionInboxSyncWindow);
                break;
            case IpsServices::ReceptionGenericSyncWindowInMessages:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                      IpsServices::EmailKeyReceptionGenericSyncWindowInMessages);
                break;
            case IpsServices::ReceptionWeekDays:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionWeekDays);
                break;
            case IpsServices::ReceptionDayStartTime:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionDayStartTime);
                break;
            case IpsServices::ReceptionDayEndTime:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionDayEndTime);
                break;
            case IpsServices::ReceptionRefreshPeriodDayTime:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionRefreshPeriodDayTime);
                break;
            case IpsServices::ReceptionRefreshPeriodOther:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionRefreshPeriodOther);
                break;
            case IpsServices::UserNameHidden:
                settingValue = readFromCenRep(IpsServices::EmailKeyUserNameHidden);
                break;
            case IpsServices::EmailNotificationState:
                settingValue = readFromCenRep(IpsServices::EmailKeyEMNState);
                break;
            case IpsServices::FirstEmnReceived:
                settingValue = readFromCenRep(IpsServices::EmailKeyFirstEmnReceived);
                break;
            case IpsServices::EmnReceivedNotSynced:
                settingValue = readFromCenRep(IpsServices::EmailKeyEmnReceivedNotSynced);
                break;
            case IpsServices::AoLastSuccessfulUpdateL:
                settingValue = readFromCenRep(IpsServices::EmailKeyAoLastSuccessfulUpdateL);
                break;
            case IpsServices::AoLastSuccessfulUpdateH:
                settingValue = readFromCenRep(IpsServices::EmailKeyAoLastSuccessfulUpdateH);
                break;
            case IpsServices::AoLastUpdateFailed:
                settingValue = readFromCenRep(IpsServices::EmailKeyAoLastUpdateFailed);
                break;
            case IpsServices::AoUpdateSuccessfulWithCurSettings:
                settingValue = readFromCenRep(IpsServices::EmailKeyAoUpdateSuccessfulWithCurSettings);
                break;
            default:
                ret = false;
                break;
            }
    }
    if(!settingValue.isValid()){
        ret = false;
        }
    
    return ret;
}
   
/*!     
    Writes extended mailbox setting.
    \param settingItem SettingItem enum of the setting to replace.
    \param settingValue QVariant of the new setting value.
    \return bool <true> when the setting item was written, otherwise <false>.
*/
bool NmIpsSosExtendedSettingsManager::writeSetting(IpsServices::SettingItem settingItem,
                                                const QVariant &settingValue)
{
    return writeSettingToCenRep(mActiveProfileOffset, settingItem, settingValue);
}

/*!     

*/
bool NmIpsSosExtendedSettingsManager::writeSetting(int profileMode,
    IpsServices::SettingItem settingItem, const QVariant &settingValue)
{
    quint32 profileOffset = convertToProfileOffset(profileMode);
    return writeSettingToCenRep(profileOffset, settingItem, settingValue);
}

/*!     
    Deletes all the extended settings of the mailbox.
*/
void NmIpsSosExtendedSettingsManager::deleteSettings()
{
    if(mMailboxOffset>=0) {

        // Find all the keys that match the criteria 0xXXXXXZZZ, where X=part of mailbox offset
        // and Z=don't care. This will give us all the keys for the particular mailbox.
        quint32 partialKey(mMailboxOffset);
        quint32 bitMask(0xFFFFF000);
        XQCentralRepositorySearchCriteria criteria(IpsServices::EmailMailboxSettingRepository,
                                                   partialKey, bitMask);
        // Find the keys.
        XQCentralRepositoryUtils utils(*mSettingsManager);
        QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

        // Delete the keys.
        foreach(XQCentralRepositorySettingsKey key, foundKeys) {
            utils.deleteKey(key);
            }
    }
}

/*!

*/
bool NmIpsSosExtendedSettingsManager::writeSettingToCenRep(qint32 profileOffset,
    IpsServices::SettingItem settingItem, const QVariant &settingValue)
{
    bool ret(profileOffset >= 0);
    if(ret) {
        switch(settingItem) {
            case IpsServices::DownloadPictures:
                ret = writeToCenRep(
                        IpsServices::EmailKeyPreferenceDownloadPictures, 
                        settingValue);
                break;
            case IpsServices::MessageDivider:
                ret = writeToCenRep(
                        IpsServices::EmailKeyPreferenceMessageDivider, 
                        settingValue);
                break;
            case IpsServices::ReceptionActiveProfile:
                ret = writeToCenRep(
                        IpsServices::EmailKeyReceptionActiveProfile, 
                        settingValue);
                calculateActiveProfileOffset();
                break;
            case IpsServices::ReceptionUserDefinedProfile:
                ret = writeToCenRep(
                        IpsServices::EmailKeyReceptionUserDefinedProfile, 
                        settingValue);
                break;
            case IpsServices::ReceptionInboxSyncWindow:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionInboxSyncWindow,
                        settingValue);
                break;
            case IpsServices::ReceptionGenericSyncWindowInMessages:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionGenericSyncWindowInMessages,
                        settingValue);
                break;
            case IpsServices::ReceptionWeekDays:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionWeekDays,
                        settingValue);
                break;
            case IpsServices::ReceptionDayStartTime:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionDayStartTime,
                        settingValue);
                break;
            case IpsServices::ReceptionDayEndTime:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionDayEndTime,
                        settingValue);
                break;
            case IpsServices::ReceptionRefreshPeriodDayTime:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionRefreshPeriodDayTime,
                        settingValue);
                break;
            case IpsServices::ReceptionRefreshPeriodOther:
                ret = writeToCenRep(
                        profileOffset + 
                        IpsServices::EmailKeyReceptionRefreshPeriodOther,
                        settingValue);
                break;
            case IpsServices::UserNameHidden:
                ret = writeToCenRep(
                        IpsServices::EmailKeyUserNameHidden, settingValue);
                break;
            case IpsServices::EmailNotificationState:
                ret = writeToCenRep(
                        IpsServices::EmailKeyEMNState, settingValue);
                break;
            case IpsServices::FirstEmnReceived:
                ret = writeToCenRep(
                        IpsServices::EmailKeyFirstEmnReceived, settingValue);
                break;
            case IpsServices::EmnReceivedNotSynced:
                ret = writeToCenRep(
                        IpsServices::EmailKeyEmnReceivedNotSynced,
                        settingValue);
                break;
            case IpsServices::AoLastSuccessfulUpdateL:
                ret = writeToCenRep(
                        IpsServices::EmailKeyAoLastSuccessfulUpdateL,
                        settingValue);
                break;
            case IpsServices::AoLastSuccessfulUpdateH:
                ret = writeToCenRep(
                        IpsServices::EmailKeyAoLastSuccessfulUpdateH,
                        settingValue);
                break;
            case IpsServices::AoLastUpdateFailed:
                ret = writeToCenRep(
                        IpsServices::EmailKeyAoLastUpdateFailed,
                        settingValue);
                break;
            case IpsServices::AoUpdateSuccessfulWithCurSettings:
                ret = writeToCenRep(
                        IpsServices::EmailKeyAoUpdateSuccessfulWithCurSettings,
                        settingValue);
                break;
            default:
                ret = false;
                break;
            }
    }
    return ret;
}

/*!
    Creates a new key-value pair into cenrep
 */
bool NmIpsSosExtendedSettingsManager::createKeyValuePair(
            const IpsServices::SettingItem settingItem, 
            const QVariant &settingValue)
    {
    
    XQCentralRepositoryUtils* utils =
            new XQCentralRepositoryUtils(*mSettingsManager);
    
    XQCentralRepositorySettingsKey settingKey(
                  IpsServices::EmailMailboxSettingRepository,
                  mMailboxOffset + settingItem);
          
    bool ret = utils->createKey(settingKey, settingValue);
          
    
    delete utils;
    
    return ret;    
    }

/*!
    Reads a key value from the Central Repository.
    \param key Key identifier.
    \return The settings value for the given key.
*/
QVariant NmIpsSosExtendedSettingsManager::readFromCenRep(quint32 key) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
                                              mMailboxOffset + key);
    return mSettingsManager->readItemValue(settingKey);
}

/*!
    Writes a key value to the Central Repository.
    \param key Key identifier.
    \param value The settings value for the given key.
    \return Returns <true> if the value was succesfully written, <false> if not.
*/
bool NmIpsSosExtendedSettingsManager::writeToCenRep(quint32 key, const QVariant &value) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
                                              mMailboxOffset + key);
    return mSettingsManager->writeItemValue(settingKey, value);
}

void NmIpsSosExtendedSettingsManager::calculateMailboxOffset()
{
    // Find all the keys that match the criteria 0xZZZZZ000, where Z=don't care.
    // This will give us all the keys that hold the mailbox ids, e.g. 0x00001000, 0x00002000, etc.
    quint32 partialKey(0x00000000);
    quint32 bitMask(0x00000FFF);
    XQCentralRepositorySearchCriteria criteria(IpsServices::EmailMailboxSettingRepository,
                                               partialKey, bitMask);
    // Set the mailbox id is value criteria for the search.
    criteria.setValueCriteria((int)mMailboxId.id32());

    // Find the keys.
    XQCentralRepositoryUtils utils(*mSettingsManager);
    QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

    // We should only get one key as a result.
    if (foundKeys.count() == 1) {
        mMailboxOffset = foundKeys[0].key();
        calculateActiveProfileOffset();
    }
}

/*!
    Calculates the active reception schedule profile offset.
*/
void NmIpsSosExtendedSettingsManager::calculateActiveProfileOffset()
{
    QVariant activeProfile = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);
    mActiveProfileOffset = convertToProfileOffset(activeProfile.toInt());
}

/*!

*/
qint32 NmIpsSosExtendedSettingsManager::convertToProfileOffset(int profile)
{
    quint32 profileOffset(0);
    // Possible values are defined in ipssettingkeys.h
    switch(profile) {
        case 0:
            profileOffset = IpsServices::EmailProfileOffsetKUTD;
            break;
        case 1:
            profileOffset = IpsServices::EmailProfileOffsetSE;
            break;
        case 2:
            profileOffset = IpsServices::EmailProfileOffsetMF;
            break;
        case 3:
            profileOffset = IpsServices::EmailProfileOffsetUD;
            break;
        default:
            break;
    }
    return profileOffset;
}
