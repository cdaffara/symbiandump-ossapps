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
* Description: Music Player Settings Manager.
*
*/

#include <xqsettingsmanager.h>

#include "mpsettingsmanager.h"
#include "mpsettingsmanagerdefs.h"
#include "mptrace.h"

/*!
    \fn void shuffleChanged(bool shuffle)

    This signal is emitted when the \a shuffle setting was changed.
 */

/*!
    \fn void repeatChanged(bool repeat)

    This signal is emitted when the \a repeat setting was changed.
 */
 
/*!
    \class MpSettingsManager
    \brief Music Player Settings manager is the singleton responsible to
     store persistent Music Player settings.

    Music Player Settings Manager is a class that provides access to Music 
    player settings trough static getters and settes as well as singnals 
    and slots via the singleton instance.
*/


/*!
 Constructs the MP Settings Manager.
 */
MpSettingsManager::MpSettingsManager():
    mSettingsManager (new XQSettingsManager(this))
{
    TX_ENTRY
    connect(mSettingsManager, 
            SIGNAL(valueChanged(XQSettingsKey, QVariant)), 
            this, 
            SLOT(valueChanged(XQSettingsKey, QVariant)));
    //first read from CenRep
    XQSettingsKey firstStartupProfileKey(XQSettingsKey::TargetCentralRepository,
            KMPCenRepSettingsFeature,
            KMPCenRepSettingFirstStartupKey);
    mFirstStartup = mSettingsManager->readItemValue(firstStartupProfileKey).toInt();
    if ( mFirstStartup ) {
        mSettingsManager->writeItemValue(firstStartupProfileKey, 0);
    }

    XQSettingsKey suffleProfileKey(XQSettingsKey::TargetCentralRepository,
            KMPCenRepSettingsFeature,
            KMPCenRepSettingShuffleKey);
    mShuffle = mSettingsManager->readItemValue(suffleProfileKey).toInt();
    mSettingsManager->startMonitoring(suffleProfileKey);

    XQSettingsKey repeatProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingRepeatKey);
    mRepeat = mSettingsManager->readItemValue(repeatProfileKey).toInt();
    mSettingsManager->startMonitoring(repeatProfileKey);
    
    XQSettingsKey presetProfileKey(XQSettingsKey::TargetCentralRepository, 
             KMPCenRepSettingsFeature, 
             KMPCenRepSettingPresetIdKey);
    mPreset = mSettingsManager->readItemValue(presetProfileKey).toInt();
    mSettingsManager->startMonitoring(presetProfileKey);

    XQSettingsKey InspireMeKey(XQSettingsKey::TargetCentralRepository,
             KMPCenRepSettingsFeature,
             KMPCenRepSettingInspireMeKey);
    mInspireMe = mSettingsManager->readItemValue(InspireMeKey).toInt();
    mSettingsManager->startMonitoring(InspireMeKey);
    
    XQSettingsKey SongDetailsGbKey(XQSettingsKey::TargetCentralRepository,
             KMPCenRepSettingsFeature,
             KMPCenRepSettingSongDetailsGBKey);
    mSongDetailsGb = mSettingsManager->readItemValue(SongDetailsGbKey).toInt();
    mSettingsManager->startMonitoring(SongDetailsGbKey);

    XQSettingsKey showMtpInfo(XQSettingsKey::TargetCentralRepository,
            KMPCenRepSettingsFeature,
            KMusicPlayerShowMtpInfoKey);
    mShowMtpInfo = mSettingsManager->readItemValue(showMtpInfo).toInt();
    mSettingsManager->startMonitoring(showMtpInfo);

    XQSettingsKey mtpInfoUrl(XQSettingsKey::TargetCentralRepository,
            KMPCenRepSettingsFeature,
            KMusicPlayerMtpInfoUrlKey);
    mMtpInfoUrl = mSettingsManager->readItemValue(mtpInfoUrl, XQSettingsManager::TypeString).toString();
    TX_EXIT
}

/*!
 Destructs the settings manager.
 */
MpSettingsManager::~MpSettingsManager()
{
    delete mSettingsManager;
}

/*!
 Returns the singleton instance to the settings manager.
 */
MpSettingsManager * MpSettingsManager::instance()
{
    static MpSettingsManager instance;
    return &instance;
}

/*!
 Returns whether this is first startup or not.
 */
bool MpSettingsManager::firstStartup()
{
    return instance()->mFirstStartup;
}

/*!
 Returns the shuffle setting.
 */
bool MpSettingsManager::shuffle()
{
    return instance()->mShuffle;
}

/*!
 Returns the repeat setting.
 */
bool MpSettingsManager::repeat()
{
    return instance()->mRepeat;
}

/*!
 Returns the preset setting.
 */
int MpSettingsManager::preset()
{
    return instance()->mPreset;
}

/*!
 Returns the inspireMe setting.
 */
bool MpSettingsManager::inspireMe()
{
    return instance()->mInspireMe;
}

/*!
 Returns the SongDetails group Box setting.
 */
bool MpSettingsManager::songDetailsGb()
{
    return instance()->mSongDetailsGb;
}

/*!
 Returns wheter mtp info should be showed.
 */
bool MpSettingsManager::showMtpInfo()
{
    return instance()->mShowMtpInfo;
}

/*!
 Returns mtp info url.
 */
QString MpSettingsManager::mtpInfoUrl()
{
    return instance()->mMtpInfoUrl;
}


/*!
 Slot to be called when a setting is changed.
 */
void MpSettingsManager::valueChanged(const XQSettingsKey& key, 
        const QVariant& value)
{
    TX_ENTRY
    switch ( key.key() ) {
        case KMPCenRepSettingShuffleKey:
            mShuffle = value.toInt();
            TX_LOG_ARGS("shuffle changed to "<< mShuffle);
            emit shuffleChanged( mShuffle );
            break;
        case KMPCenRepSettingRepeatKey:
            mRepeat = value.toInt();
            TX_LOG_ARGS("Repeat changed to "<< mRepeat);
            emit repeatChanged( mRepeat );
            break;
        case KMPCenRepSettingPresetIdKey:
            mPreset = value.toInt();
            TX_LOG_ARGS("Preset changed to "<< mPreset);
            emit presetChanged( mPreset );
            break;
        case KMPCenRepSettingInspireMeKey:
            mInspireMe = value.toInt();
            TX_LOG_ARGS("Inspire Me changed to "<< mInspireMe);
            emit InspireMeChanged( mInspireMe );
            break;
        case KMPCenRepSettingSongDetailsGBKey:
            mSongDetailsGb = value.toInt();
            TX_LOG_ARGS("SongDetailsGB changed to "<< mSongDetailsGb);
            emit SongDetailsGbChanged( mSongDetailsGb );
            break;
        case KMusicPlayerShowMtpInfoKey:
            mShowMtpInfo = value.toInt();
            TX_LOG_ARGS("Show MTP info changed to "<< mShowMtpInfo);
            break;
        default :
            TX_LOG_ARGS(" unhandled cenrep key: " << key.key() << 
                    "; should never get here.");
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called to request an update on the \a shuffle setting.
 */
void MpSettingsManager::setShuffle(bool shuffle)
{
    TX_STATIC_ENTRY_ARGS("shuffle=" << shuffle);
    XQSettingsKey suffleProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingShuffleKey);
    instance()->mSettingsManager->writeItemValue(
            suffleProfileKey, 
            shuffle ? 1 : 0);
    TX_EXIT
}

/*!
 Slot to be called to request an update on the \a repeat setting.
 */
void MpSettingsManager::setRepeat(bool repeat)
{
    TX_STATIC_ENTRY_ARGS("repeat=" << repeat);
    XQSettingsKey repeatProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingRepeatKey);
    instance()->mSettingsManager->writeItemValue(
            repeatProfileKey, 
            repeat ? 1 : 0);
    TX_EXIT
}

/*!
 Slot to be called to request an update on the \a preset setting.
 */
void MpSettingsManager::setPreset(int preset)
{
    TX_STATIC_ENTRY_ARGS("preset=" << preset);
    XQSettingsKey presetProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingPresetIdKey);
    instance()->mSettingsManager->writeItemValue(
            presetProfileKey, preset);
    TX_EXIT
}

/*!
 Slot to be called to request an update on the \a InspireMe setting.
 */
void MpSettingsManager::setInspireMe(bool inspireme)
{
    TX_STATIC_ENTRY_ARGS("Inspire Me=" << inspireme);
    XQSettingsKey inspireMeProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingInspireMeKey);
    instance()->mSettingsManager->writeItemValue(
            inspireMeProfileKey, 
            inspireme ? 1 : 0);
    TX_EXIT
}

/*!
 Slot to be called to request an update on the \a SongDetails Group Box setting.
 */
void MpSettingsManager::setSongDetailsGb(bool songdetails)
{
    TX_STATIC_ENTRY_ARGS("SongDetailsGb=" << songdetails);
    XQSettingsKey songDetailsGbProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingSongDetailsGBKey);
    instance()->mSettingsManager->writeItemValue(
            songDetailsGbProfileKey, 
            songdetails ? 1 : 0);
    TX_EXIT
}

/*!
 Slot to be called to stop showing mtp info.
 */
void MpSettingsManager::stopShowingMtpInfo()
{
    TX_STATIC_ENTRY_ARGS("preset=" << preset);
    XQSettingsKey showMtpInfo(XQSettingsKey::TargetCentralRepository,
            KMPCenRepSettingsFeature,
            KMusicPlayerShowMtpInfoKey);
    instance()->mSettingsManager->writeItemValue(showMtpInfo, 0);
    TX_EXIT
}

