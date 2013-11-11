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
* Description:
*
*/
#include <mmf/common/mmferrors.h>
#include <NokiaAudioPreference.h>
#include <coedef.h>
#include <QStringList>
#include <hxmetadatakeys.h>
#include <StereoWideningBase.h>

#include "irqmetadata.h"
#include "irqenums.h"
#include "irqmmfadapter.h"
#include "irqlogger.h"

//Constants
const TUid KUidController        = { 0x101F8514 }; // Helix Video controller UID
const TInt KConnectingTime       = 30*1000000;     // 30 seconds
const TInt KVolumeMinPercentage  = 0;              // Minimum volume percentage
const TInt KVolumeMaxPercentage  = 100;            // Maximum volume percentage
const TInt KLoadingCompletePercentage = 100;       // Loading Complete percentage
const int KDefaultStereoLevel = 100;            // Default stereo level

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::IRQMMFAdapter
//  Constructor
//  Initialize viriants
// ---------------------------------------------------------------------------
//
IRQMMFAdapter::IRQMMFAdapter() :
    iVideoPlayer(NULL)
    ,iQMetaData(NULL)
    ,iPrepareTimer(NULL), iStereoEffect(NULL)
{
    LOG_METHOD;
    iPlayState = EStopped;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::~IRQMMFAdapter
//  Destructor
//  Clean up resources
// ---------------------------------------------------------------------------
//
IRQMMFAdapter::~IRQMMFAdapter()
{
    LOG_METHOD;
    destroyPlayer();

    delete iQMetaData;
    iQMetaData = NULL;

    if (iPrepareTimer)
    {
        if (iPrepareTimer->IsActive())
        {
            iPrepareTimer->Cancel();
        }
        delete iPrepareTimer;
        iPrepareTimer = NULL;
    }
    
    delete iStereoEffect;
    iStereoEffect = NULL;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::playStation
//  IRQPlayerAdapterInterface method
//  Play url via specific access point id
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::playStation(const QString &aUrl, int aApId)
{
    LOG_METHOD;
    TRAPD(error, playL(aUrl, aApId));
    if (NULL == iQMetaData)
    {
        LOG("Error Occured(EIRQErrorOutOfMemory)");
        emit errorOccured(EIRQErrorOutOfMemory);
    }

    if (KErrNone != error)
    {
        LOG_FORMAT("Error Occured = %d", (int)error);
        emit errorOccured(EIRQPlayerErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::playL
//  Play url via specific access point id
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::playL(const QString &aUrl, int aApId)
{
    LOG_METHOD;
    
    // Save stream Url
    if (NULL == iQMetaData)
    {
        iQMetaData = new (ELeave) IRQMetaData();
    }
    else
    {
        // Clear MetaData
        iQMetaData->clear();
    }
    iQMetaData->setStreamUrl(aUrl);

    // Transfer url from QString to TDesC
    TPtrC stationUrl(reinterpret_cast<const TUint16*>(aUrl.utf16()));

    // Create player if it doesn't exist
    if (NULL == iVideoPlayer)
    {
        createPlayerL();
    }

    // If the status is not stopped, clean up last playback resources
    stop();

    // Open url
    LOG("CVideoPlayerUtility::OpenUrlL(stationUrl, aApId, KNullDesC8, KUidController)");
    iVideoPlayer->OpenUrlL(stationUrl, aApId, KNullDesC8, KUidController);
    iPlayState = EOpenning;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::stop
//  IRQPlayerAdapterInterface method
//  Stop playback, call Close() to clean up allocated resources
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::stop()
{
    LOG_METHOD;
    
    if (iVideoPlayer && EStopped != iPlayState)
    {
        LOG("Stop the prepare timeout timer");
        if (iPrepareTimer)
        {
            if (iPrepareTimer->IsActive())
            {
                iPrepareTimer->Cancel();
            }
        }

        LOG("CVideoPlayerUtility::Stop()");
        LOG("CVideoPlayerUtility::Close()");
        iVideoPlayer->Stop();
        iVideoPlayer->Close();
        iPlayState = EStopped;
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::setVolume
//  IRQPlayerAdapterInterface method
//  Set volume to player
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::setVolume(int aVolume)
{
    LOG_METHOD;
    
    if (iVideoPlayer && iPlayState > EOpenning)
    {
        // aVolume is a percentage
        if (aVolume < KVolumeMinPercentage)
        {
            aVolume = KVolumeMinPercentage;
        }
        else if (aVolume > KVolumeMaxPercentage)
        {
            aVolume = KVolumeMaxPercentage;
        }
        int volume = aVolume*iVideoPlayer->MaxVolume()/KVolumeMaxPercentage;

        LOG_FORMAT("CVideoPlayerUtility::SetVolumeL(%d)", volume);
        TRAPD(error, iVideoPlayer->SetVolumeL(volume));
        if (KErrNone != error)
        {
            LOG_FORMAT("Error Occured = %d", (int)error);
            emit errorOccured(EIRQPlayerErrorGeneral);
        }
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::getVolume
//  IRQPlayerAdapterInterface method
//  Get current volume value from player
// ---------------------------------------------------------------------------
//
int IRQMMFAdapter::getVolume()
{
    LOG_METHOD;
    
    int volume = KVolumeMinPercentage;

    if (iVideoPlayer && iPlayState > EOpenning)
    {
        // Return a percentage
        volume = iVideoPlayer->Volume()*KVolumeMaxPercentage/iVideoPlayer->MaxVolume();
    }
    return volume;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::getPlayerInstance
//  IRQPlayerAdapterInterface method
//  Get audio player instance
// ---------------------------------------------------------------------------
//
void* IRQMMFAdapter::getPlayerInstance()
{
    return (void*)iVideoPlayer;
}

void IRQMMFAdapter::enableStereoEffect()
{
    LOG_METHOD;
    if (IRQPlayerAdapterInterface::EPlaying != iPlayState)
    {
        return;
    }

    TRAP_IGNORE(enableStereoEffectL());
}

void IRQMMFAdapter::disableStereoEffect()
{
    LOG_METHOD;
    if (iStereoEffect)
    {
        if (iStereoEffect->IsEnabled())
        {
            TRAPD(error, iStereoEffect->DisableL());
            if (KErrNone != error)
            {
                LOG_FORMAT("Error Occured = %d", (int)error);
                emit errorOccured(EIRQPlayerErrorSetStereoFailed);
            }
            delete iStereoEffect;
            iStereoEffect = NULL;
        }
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoOpenComplete
//  Callback function, MVideoPlayerUtilityObserver method
//  Called after calling CVideoPlayerUtility::OpenUrlL()
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoOpenComplete(TInt aError)
{
    LOG_METHOD;
    if (KErrNone == aError)
    {
        if (NULL == iPrepareTimer)
        {
            TRAPD(error, iPrepareTimer = CPeriodic::NewL(CPeriodic::EPriorityStandard));
            if (KErrNone != error)
            {
                emit errorOccured(EIRQErrorOutOfMemory);
                return;
             }
        }

        // Prepare to playback
        LOG("CVideoPlayerUtility::Prepare()");
        iVideoPlayer->Prepare();
        iPlayState = EConnecting;

        // Start a timer to check preparation status
        if (iPrepareTimer->IsActive())
        {
            // Cancel the previous request if pending
            iPrepareTimer->Cancel();
        }
        TTimeIntervalMicroSeconds32 interval(KConnectingTime);
        iPrepareTimer->Start(interval,interval,
                             TCallBack(IRQMMFAdapter::isPrepareCompleted,this));
        LOG_FORMAT("Start the prepare timeout timer = %d sec", (int)interval.Int()/1000000);
    }
    else
    {
        LOG_FORMAT("Error Occured = %d", (int)aError);
        emit errorOccured(EIRQPlayerErrorConnectingFailed);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoPrepareComplete
//  Callback function, MVideoPlayerUtilityObserver method
//  Called after calling CVideoPlayerUtility::Prepare. Since some audio types
//  are not explicitly(hxmmffourccmap.cpp), they are not retrieved here.
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoPrepareComplete(TInt aError)
{
    LOG_METHOD;
    // Cancel the previous request if pending
    LOG("Stop the prepare timeout timer");
    if (iPrepareTimer->IsActive())
    {
        iPrepareTimer->Cancel();
    }

    if (KErrNone == aError)
    {
        // Get volume from preset
        int volumeval = KVolumeMaxPercentage/2;
        emit volumeExpected(volumeval);
        setVolume(volumeval);

        // Save bit rate
        int bitrate = 0;
        TRAPD(error, bitrate = iVideoPlayer->AudioBitRateL());
        if (KErrNone == error)
        {
            iQMetaData->setBitrate(bitrate/1000);
        }

        // Send signal ConnectionEstablished
        LOG_FORMAT("bit rate = %d", iQMetaData->getBitrate());
        emit connectionEstablished(iQMetaData->getBitrate());

        // Set specific event to get meta data from player
        setMetadataEventConfig();

        // Start playback
        LOG("CVideoPlayerUtility::Play()");
        iVideoPlayer->Play();
        iPlayState = EBuffering;
    }
    else if (KErrServerBusy == aError)
    {
        LOG("Error Occured(EIRQPlayerErrorServerFull)");
        emit errorOccured(EIRQPlayerErrorServerFull);
    }
    else
    {
        LOG_FORMAT("Error Occured = %d", (int)aError);
        emit errorOccured(EIRQPlayerErrorConnectingFailed);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoPlayComplete
//  Callback function, MVideoPlayerUtilityObserver method
//  Notification that video playback has completed. This is not called if play
//  back is explicitly stopped by calling Stop. Moreover, radio station stream
//  has no end. So it should be NEVER called.
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoPlayComplete(TInt aError)
{
    LOG_METHOD;
    if (KErrNone != aError)
    {
        LOG_FORMAT("Error Occured = %d", (int)aError);
        emit errorOccured(EIRQPlayerErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoEvent
//  Callback function, MVideoPlayerUtilityObserver method
//  Handle events from player.
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoEvent(TMMFEvent const & aEvent)
{
    LOG_METHOD;
    if (KMMFEventCategoryVideoPlayerGeneralError == aEvent.iEventType)
    {
        LOG("Event Type = KMMFEventCategoryVideoPlayerGeneralError");
        switch (aEvent.iErrorCode)
        {
            case KErrHardwareNotAvailable:
                LOG("Event Error Code = KErrHardwareNotAvailable");
                emit errorOccured(EIRQPlayerErrorAudioDeviceLost);
                break;                
            case KErrMMAudioDevice:
                // Higher priority application has taken over the
                // audio device. --> Do stop.
                LOG("Event Error Code = KErrMMAudioDevice");
                emit errorOccured(EIRQPlayerErrorAudioDeviceLost);
                break;
            case KErrDisconnected:
                LOG("Event Error Code = KErrDisconnected");
                emit errorOccured(EIRQPlayerErrorConnectionLost);
                break;
            case KErrTimedOut:  
                LOG("Event Error Code = KErrTimedOut");  
                emit errorOccured(EIRQPlayerErrorTimeOut);
                break;
            case KErrServerBusy:  
                LOG("Event Error Code = KErrServerBusy");  
                emit errorOccured(EIRQPlayerErrorServerFull);
                break;                                            
            default:
                LOG_FORMAT("Event Error Code = %d", (int)aEvent.iErrorCode);
                emit errorOccured(EIRQPlayerErrorGeneral);			
                break;
        }
    }
    else if (KMMFRefreshMetaData == aEvent.iEventType)
    {
        LOG("Event Type = KMMFRefreshMetaData");
        LOG_FORMAT( "Event Error Code = %d", (int)aEvent.iErrorCode);
        // Get refreshed meta data
        TRAPD(error, getRefreshedMetaDataL(aEvent.iErrorCode));
        if (KErrNone != error)
        {
            LOG_FORMAT("Get MetaData Error Occured = %d", (int)error);
            emit errorOccured(EIRQPlayerErrorGeneral);
        }
    }
    else
    {
        LOG_FORMAT("Event Type = %d", (int)aEvent.iEventType.iUid);
        LOG_FORMAT("Event Error Code = %d", (int)aEvent.iErrorCode);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoFrameReady
//  Callback function, MVideoPlayerUtilityObserver method
//  For video stream only, never called
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError)
{
    LOG_METHOD;
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvloLoadingStarted
//  Callback function, MVideoLoadingObserver method
//  Start buffering after CVideoPlayerUtility::Play() is called
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvloLoadingStarted()
{
    LOG_METHOD;
    // Get buffering progress and send it to application
    int percentageComplete = 0;

    LOG("CVideoPlayerUtility::GetVideoLoadingProgressL()");
    TRAPD(error, iVideoPlayer->GetVideoLoadingProgressL(percentageComplete));

    if (KErrNone == error)
    {
        // Send signal to UpdateProgress
        LOG_FORMAT("Loading Percentage Complete = %d", percentageComplete);
        emit percentageBuffered(percentageComplete);
    }
    else
    {
        LOG_FORMAT("Error Occured = %d", (int)error);
        emit errorOccured(EIRQPlayerErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvloLoadingComplete
//  Callback function, MVideoLoadingObserver method
//  Send 100% buffering status out
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvloLoadingComplete()
{
    LOG_METHOD;
    iPlayState = EPlaying;

    // Send signal to update progress, 100%
    emit percentageBuffered(KLoadingCompletePercentage);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::getRefreshedMetaData
//  Get refreshed meta data according to the index
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::getRefreshedMetaDataL(TInt index)
{
    LOG_METHOD;
    if (iQMetaData)
    {
        LOG("CVideoPlayerUtility::MetaDataEntryL()");
        CMMFMetaDataEntry* pMetadataEntry = iVideoPlayer->MetaDataEntryL(index);

        QString entryName = QString::fromUtf16(pMetadataEntry->Name().Ptr(),
                                               pMetadataEntry->Name().Length());

        // If the meta data is the same as last, we don't need to report it.
        if (iLastArtistSongName == entryName)
        {
            return;
        }
        else
        {
            iLastArtistSongName = entryName;
        }

        // Artist, song name
        if (entryName == HXAuthor)
        {
            QString songArtist = QString::fromUtf16(pMetadataEntry->Value().Ptr(),
                                                    pMetadataEntry->Value().Length());
            iQMetaData->setArtistSongName(songArtist);

            // Send signal HandleMetaDataReceived
            emit metaDataReceived(*iQMetaData);
        }
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::createPlayer
//  Create player instance
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::createPlayerL()
{
    LOG_METHOD;
    
    LOG("CVideoPlayerUtility::NewL()");
    // Create player instance
    iVideoPlayer = CVideoPlayerUtility2::NewL(*this,KAudioPriorityAudioPlaybackStreaming ,
                                             (TMdaPriorityPreference)KAudioPrefRealOneStreaming);
    LOG("CVideoPlayerUtility::RegisterForVideoLoadingNotification()");
    // Register loading notification
    iVideoPlayer->RegisterForVideoLoadingNotification(*this);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::destroyPlayer
//  Destroy player
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::destroyPlayer()
{
    LOG_METHOD;
    delete iVideoPlayer;
    iVideoPlayer = NULL;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::SetMetadataEventConfig
//  Enable meta data event
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::setMetadataEventConfig()
{
    LOG_METHOD;
    TMMFMessageDestinationPckg  destinationPckg(KUidInterfaceMMFControllerMetadataEventMsg);
    TPckgBuf<TBool>             metadataEventPckg(EMMFEnableMetadataEvent);

    //  Enable meta data event.
    LOG("CVideoPlayerUtility::CustomCommandSync(EMMFSetMetadataEventConfig)");
    iVideoPlayer->CustomCommandSync(destinationPckg,
                                    EMMFSetMetadataEventConfig,
                                    metadataEventPckg,
                                    KNullDesC8);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::isPrepareCompleted
//  Static function for periodic call
// ---------------------------------------------------------------------------
//
TInt IRQMMFAdapter::isPrepareCompleted(TAny* aPtr)
{
    LOG_METHOD;
    LOG("Prepare timeout timer call back function");
    IRQMMFAdapter* self = static_cast<IRQMMFAdapter*>(aPtr);
    if (self)
    {
        self->checkPrepare();
    }
    return KErrNone;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::checkPrepare
//  Check if the preparation is complete
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::checkPrepare()
{
    LOG_METHOD;
    LOG("Stop the prepare timeout timer");
    if (iPrepareTimer->IsActive())
    {
        // Cancel the previous request if pending
        iPrepareTimer->Cancel();
    }

    if (EConnecting == iPlayState)
    {
        LOG("Error Occured(EIRQPlayerErrorConnectingFailed)");
        emit errorOccured(EIRQPlayerErrorConnectingFailed);
        stop();
    }
}

void IRQMMFAdapter::enableStereoEffectL()
{
    LOG_METHOD;
    TUint stereoLevel = KDefaultStereoLevel;

    if (!iStereoEffect)
    {
        CVideoPlayerUtility* playerInstance = (CVideoPlayerUtility*)getPlayerInstance();
        iStereoEffect = CStereoWidening::NewL(*playerInstance, EFalse, stereoLevel);
    }

    if (!iStereoEffect->IsEnabled())
    {
        iStereoEffect->EnableL();
        iStereoEffect->SetStereoWideningLevelL(stereoLevel);
        iStereoEffect->ApplyL();
    }
}
