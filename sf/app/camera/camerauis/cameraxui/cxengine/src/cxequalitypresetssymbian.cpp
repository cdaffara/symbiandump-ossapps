/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation and/or its subsidiary(-ies).
*
* Contributors:
*
*
* Description:
* Implementation file for CxeQualityPresetsSymbian class
*
*/

#include <algorithm>
#include <e32std.h> // For Symbian types used in mmsenginedomaincrkeys.h
#include <MmsEngineDomainCRKeys.h>
#include <imagingconfigmanager.h>

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxesettings.h"
#include "cxequalitydetails.h"
#include "cxequalitypresetssymbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxequalitypresetssymbianTraces.h"
#endif

// constants
namespace
{
    // Display IDs for cameras, used when requesting data from ICM
    const int    PRIMARY_CAMERA_DISPLAY_ID   = 2;
    const int    SECONDARY_CAMERA_DISPLAY_ID = 3;

    const int    ONE_MILLION    = 1000000;

    const QSize ASPECT_RATIO_SIZE_4BY3  = QSize(4,3);
    const QSize ASPECT_RATIO_SIZE_16BY9 = QSize(16, 9);
    const QSize ASPECT_RATIO_SIZE_11BY9 = QSize(11, 9);

    // ICM "camcorderVisible" parameter value below this means sharing aka mms quality.
    const int    MMS_QUALITY_CAMCORDERVISIBLE_LIMIT = 200;

    // Average video bit rate scaler
    const qreal  VIDEO_AVG_BITRATE_SCALER = 0.9;
    // Coefficient to estimate video metadata amount
    const qreal  VIDEO_METADATA_COEFF = 1.03;
    // Maximum video clip duration in seconds for all qualities
    const qint64 VIDEO_MAX_DURATION = 5400;
}


/* !
* Intializes ImagingConfigManager
*/
CxeQualityPresetsSymbian::CxeQualityPresetsSymbian(CxeSettings &settings)
    : mSettings(settings)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEQUALITYPRESETSSYMBIAN_1, "msg: e_CX_QUALITYPRESETS_NEW 1");

    TRAPD(err,  mIcm = CImagingConfigManager::NewL());

    if (err) {
        CX_DEBUG(("Error during ICM initialization error code = %d", err));
        mIcm = NULL;
    }

    OstTrace0(camerax_performance, CXEQUALITYPRESETSSYMBIAN_2, "msg: e_CX_QUALITYPRESETS_NEW 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/* !
* CxeQualityPresetsSymbian::~CxeQualityPresetsSymbian()
*/
CxeQualityPresetsSymbian::~CxeQualityPresetsSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mIcm;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
This function returns sorted list of image qualities from highest to lowest resolution.
i.e. first element in the list represent highest supported image resolution and so on.
@param cameraId The CameraIndex which defines which camera we are using primary/secondary.
Returns sorted list of image qualities in descending order.
*/
QList<CxeImageDetails> CxeQualityPresetsSymbian::imageQualityPresets(Cxe::CameraIndex cameraId)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEQUALITYPRESETSSYMBIAN_GETIMAGEPRESETS_IN, "msg: e_CX_QUALITYPRESETS_GETIMAGEPRESETS 1");

    QList<CxeImageDetails> presetList;

    if (!mIcm) {
        CX_DEBUG(("ICM not initialized, returning empty image qualities list"));
        return presetList;
    }
    int totalLevels = mIcm->NumberOfImageQualityLevels();

    CX_DEBUG(("Total image quality levels = %d", totalLevels));
    CArrayFixFlat<TUint>* levels = new CArrayFixFlat<TUint>(totalLevels);

    // Get camera display id based on camera index
    int displayId = cameraId == Cxe::SecondaryCameraIndex
                  ? SECONDARY_CAMERA_DISPLAY_ID : PRIMARY_CAMERA_DISPLAY_ID;

    TRAPD(err, mIcm->GetImageQualityLevelsL(*levels, displayId));

    if (err == KErrNone) {
        CX_DEBUG(( "Reading image quality sets one by one."));
        TImageQualitySet set;
        int numLevels = levels->Count();
        for(int i = 0; i < numLevels; i++) {
            mIcm->GetImageQualitySet(set, levels->At(i), displayId);
            if (set.iCamcorderVisible > 0) {
                // create new quality preset based on the quality set values
                CxeImageDetails newPreset = createImagePreset(set);

                // print debug prints
                debugPrints(newPreset);

                // append to the list of presets
                presetList.append(newPreset);
            }
        }
        CX_DEBUG(( "Sorting image qualities"));
        // Sorting result list according to height pixel size from highest to lowest
        // i.e. descending order keeping the highest resolution first.
        qSort(presetList.begin(), presetList.end(), qGreater<CxeImageDetails>());
    }

    delete levels;
    levels = NULL;

    OstTrace0(camerax_performance, CXEQUALITYPRESETSSYMBIAN_GETIMAGEPRESETS_OUT, "msg: e_CX_QUALITYPRESETS_GETIMAGEPRESETS 0");
    CX_DEBUG_EXIT_FUNCTION();
    return presetList;
}



/*!
This function returns sorted list of video qualities from highest to lowest resolution.
i.e. first element in the list represent highest supported video resolution and so on.
@param cameraId The CameraIndex which defines which camera we are using primary/secondary.
Returns sorted list if image qualities in descending order.
*/
QList<CxeVideoDetails> CxeQualityPresetsSymbian::videoQualityPresets(Cxe::CameraIndex cameraId)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEQUALITYPRESETSSYMBIAN_GETVIDEOPRESETS_IN, "msg: e_CX_QUALITYPRESETS_GETVIDEOPRESETS 1");

    QList<CxeVideoDetails> presetList;

    if (!mIcm) {
        CX_DEBUG(("ICM not initialized, returning empty video qualities list"));
        return presetList;
    }
    int totalLevels = mIcm->NumberOfVideoQualityLevels();

    CX_DEBUG(("Total video quality levels = %d", totalLevels));
    CArrayFixFlat<TUint>* levels= new CArrayFixFlat<TUint>(totalLevels);

    int displayId = cameraId == Cxe::SecondaryCameraIndex
                  ? SECONDARY_CAMERA_DISPLAY_ID : PRIMARY_CAMERA_DISPLAY_ID;

    TRAPD(err, mIcm->GetVideoQualityLevelsL(*levels, displayId));

    if (err == KErrNone) {
        CX_DEBUG(( "Reading video quality sets one by one."));
        TVideoQualitySet set;
        int numLevels = levels->Count();
        for(int i = 0; i < numLevels; i++) {
            mIcm->GetVideoQualitySet(set, levels->At(i), displayId);
          
            if (set.iCamcorderVisible > 0) {
                // create new quality preset
                CxeVideoDetails newPreset = createVideoPreset(set);

                // print debug prints
                debugPrints(newPreset);

                // append to the list of presets
                presetList.append(newPreset);
            }
        }
        CX_DEBUG(( "Sorting image qualities"));
        // Sorting result list according to height pixel size from highest to lowest
        // i.e. descending order keeping the highest resolution first.
        qSort(presetList.begin(), presetList.end(), qGreater<CxeVideoDetails>());
    }

    delete levels;
    levels = NULL;

    // Get the average video bitrate scaler
    TCamcorderMMFPluginSettings mmfPluginSettings;
    mIcm->GetCamcorderMMFPluginSettings(mmfPluginSettings);
    mCMRAvgVideoBitRateScaler = mmfPluginSettings.iCMRAvgVideoBitRateScaler;

    OstTrace0(camerax_performance, CXEQUALITYPRESETSSYMBIAN_GETVIDEOPRESETS_OUT, "msg: e_CX_QUALITYPRESETS_GETVIDEOPRESETS 0");
    CX_DEBUG_EXIT_FUNCTION();
    return presetList;
}



/*!
* Creates a new image preset based on TImageQualitySet values from ICM.
@ param set contains the ICM configuration data
@ returns CxeImageQuality struct
*/
CxeImageDetails CxeQualityPresetsSymbian::createImagePreset(const TImageQualitySet &set)
{
    CxeImageDetails newPreset;
    // set setting values from quality set
    newPreset.mWidth = set.iImageWidth;
    newPreset.mHeight = set.iImageHeight;
    newPreset.mEstimatedSize = set.iEstimatedSize;
    newPreset.mMpxCount = calculateMegaPixelCount(set.iImageWidth,
                                                  set.iImageHeight);
    newPreset.mPossibleImages = CxeImageDetails::UNKNOWN;
    newPreset.mImageFileExtension = toString(set.iImageFileExtension);
    newPreset.mAspectRatio = calculateAspectRatio(set.iImageWidth,
                                                  set.iImageHeight);

    return newPreset;
}



/*!
* Creates a new video preset based on TVideoQualitySet values from ICM.
*/
CxeVideoDetails CxeQualityPresetsSymbian::createVideoPreset(const TVideoQualitySet &set)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeVideoDetails newPreset;
    // set setting values from quality set
    newPreset.mWidth = set.iVideoWidth;
    newPreset.mHeight = set.iVideoHeight;

    newPreset.mRemainingTime = CxeVideoDetails::UNKNOWN;

    // Check if this is a sharing (mms) quality, and set size limit accordingly.
    if (set.iCamcorderVisible < MMS_QUALITY_CAMCORDERVISIBLE_LIMIT) {
        QVariant size;
        mSettings.get(KCRUidMmsEngine.iUid, KMmsEngineMaximumSendSize, Cxe::Repository, size);
        CX_DEBUG(("CxeQualityPresetsSymbian - Got MMS quality size limit: %d", size.toInt()));
        newPreset.mMaximumSizeInBytes = size.toInt();
    } else {
        // Zero means no limit.
        newPreset.mMaximumSizeInBytes = 0;
    }

    newPreset.mVideoBitRate = set.iVideoBitRate;
    newPreset.mAudioBitRate = set.iAudioBitRate;
    newPreset.mVideoFrameRate = set.iVideoFrameRate;
    newPreset.mVideoFileMimeType = toString(set.iVideoFileMimeType);
    newPreset.mPreferredSupplier = toString(set.iPreferredSupplier);
    newPreset.mVideoCodecMimeType = toString(set.iVideoCodecMimeType);
    newPreset.mAspectRatio = calculateAspectRatio(set.iVideoWidth,
                                                  set.iVideoHeight);

    // Convert FourCC value from TFourCC to ascii string
    const int KFourCCLength = 5; // 4 characters + '\0'
    TText8 fourCCBuf[KFourCCLength];
    TPtr8 fourCC(fourCCBuf, KFourCCLength);
    set.iAudioFourCCType.FourCC(&fourCC);
    fourCC.Append('\0');

    // set audiotype
    newPreset.mAudioType = toString(fourCCBuf);

    CX_DEBUG_EXIT_FUNCTION();
    return newPreset;
}


/*!
* Returns Aspect ratio of the image.
*/
qreal CxeQualityPresetsSymbian::avgVideoBitRateScaler()
{
    CX_DEBUG_IN_FUNCTION();
    // Get the average video bitrate scaler
    if (mIcm) {
        TCamcorderMMFPluginSettings mmfPluginSettings;
        mIcm->GetCamcorderMMFPluginSettings(mmfPluginSettings);
        mCMRAvgVideoBitRateScaler = mmfPluginSettings.iCMRAvgVideoBitRateScaler;
    }
    return mCMRAvgVideoBitRateScaler;
}


/*!
@ param width - image/video quality width
@ param height - image/video quality height
* Returns Aspect ratio of the image/video.
*/
Cxe::AspectRatio CxeQualityPresetsSymbian::calculateAspectRatio(int width, int height) const
{
    Cxe::AspectRatio aspectRatio = Cxe::AspectRatio4to3;

    // calculate delta error for the resolution against supported aspect ratio's
    int delta16by9 = abs((width * ASPECT_RATIO_SIZE_16BY9.height()) - (height * ASPECT_RATIO_SIZE_16BY9.width()));
    int delta11by9 = abs((width * ASPECT_RATIO_SIZE_11BY9.height()) - (height * ASPECT_RATIO_SIZE_11BY9.width()));
    int delta4by3  = abs((width * ASPECT_RATIO_SIZE_4BY3.height()) - (height * ASPECT_RATIO_SIZE_4BY3.width()));

    // get the closest aspect ratio
    int minValue = qMin(qMin(delta16by9, delta11by9), delta4by3);

    if (minValue == delta16by9) {
        aspectRatio = Cxe::AspectRatio16to9;
    } else if (minValue == delta11by9) {
        aspectRatio = Cxe::AspectRatio11to9;
    } else if (minValue == delta4by3) {
        aspectRatio = Cxe::AspectRatio4to3;
    }

    return aspectRatio;
}



/*!
* CxeQualityPresetsSymbian::calculateMegaPixelCount
@ param imageWidth refers to the image resolution width
@ param imageHeight refers to the image resolution height
@ returns megapixel count string
*/
QString
CxeQualityPresetsSymbian::calculateMegaPixelCount(int imageWidth, int imageHeight)
{
    QString mpxCountString;
    qreal size = imageWidth * imageHeight * 1.0;
    int mpxCount = (size/ONE_MILLION) * 10;

    if ((mpxCount % 10) == 0) {
        int value = mpxCount / 10;
        mpxCountString.setNum(value);
    } else {
        qreal value = mpxCount / 10.0;
        if ((mpxCount % 10) < 5) {
            mpxCountString.setNum(value, 'f', 1);
        } else {
            int temp = ceil(value);
            mpxCountString.setNum(temp);
        }
    }

    return mpxCountString;


}

/*!
* Get the available recording time with given video quality details and disk space.
* @param details Video quality details to use in calculation.
* @param space Available disk space to use in calculation.
* @return Available recording time estimate in seconds.
*/
int CxeQualityPresetsSymbian::recordingTimeAvailable(const CxeVideoDetails& details, qint64 space)
{
    CX_DEBUG_ENTER_FUNCTION();
    int time(0);

    // Maximum clip size may be limited for mms quality.
    // If mMaximumSizeInBytes == 0, no limit is specified.
    if (details.mMaximumSizeInBytes > 0 && details.mMaximumSizeInBytes < space) {
        space = details.mMaximumSizeInBytes;
    }

    // Use average audio/video bitrates to estimate remaining time
    qreal scaler(avgVideoBitRateScaler());
    if (scaler == 0) {
        // video bit rate scaler is 0, use the constant value
        scaler = VIDEO_AVG_BITRATE_SCALER;
    }

    bool muteSetting = mSettings.get<bool>(CxeSettingIds::VIDEO_MUTE_SETTING, false); // false = audio enabled

    int avgVideoBitRate = (details.mVideoBitRate * scaler);
    int avgAudioBitRate = muteSetting ? 0 : details.mAudioBitRate;

    quint32 averageBitRate = (quint32)((avgVideoBitRate + avgAudioBitRate) * VIDEO_METADATA_COEFF);
    quint32 averageByteRate = averageBitRate / 8;

    // 0 <= Remaining time <= KCamCMaxClipDurationInSecs
    qint64 remaining = std::max(qint64(0), space/averageByteRate);
    time = std::min(remaining, VIDEO_MAX_DURATION);

    CX_DEBUG(( "remaining time from algorithm: %d", time ));
    CX_DEBUG_EXIT_FUNCTION();
    return time;
}


/*!
Operator to sort values in ascending order.
@param s1 type of data to be sorted.
*/
bool CxeImageDetails::operator<(const CxeImageDetails& s1) const
{
    return mHeight < s1.mHeight;
}


/*!
Operator to sort values in ascending order.
@param s1 type of data to be sorted.
*/
bool CxeVideoDetails::operator<(const CxeVideoDetails& s1) const
{
    return mHeight < s1.mHeight;
}


/*
* Converts TUint8* to QString
*/
QString CxeQualityPresetsSymbian::toString(const TUint8* aData)
{
    return QString::fromLatin1((char*)aData);
}



/*!
* Helper method to enable debug prints.
@ param  Video quality preset values are printed out for debugging
*/
void CxeQualityPresetsSymbian::debugPrints(const CxeVideoDetails &preset)
{
    CX_DEBUG(("Video quality details"));
    CX_DEBUG(("Video resolution (%d,%d)", preset.mWidth, preset.mHeight));
    CX_DEBUG(("Audio bitrate = %d)", preset.mAudioBitRate));
    CX_DEBUG(("Video bitrate = %d)", preset.mVideoBitRate));
    CX_DEBUG(("Video frame rate = %f)", preset.mVideoFrameRate));
    CX_DEBUG(("Audio type: %s", preset.mAudioType.toAscii().data()));
    CX_DEBUG(("Video file MIME type: %s", preset.mVideoFileMimeType.toAscii().data()));
    CX_DEBUG(("Video preferred supplier: %s", preset.mPreferredSupplier.toAscii().data()));
    CX_DEBUG(("Video codec MIME type: %s", preset.mVideoCodecMimeType.toAscii().data()));
    QString aspectRatio;
    if (preset.mAspectRatio == Cxe::AspectRatio4to3) {
        aspectRatio = QString("4:3");
    } else if (preset.mAspectRatio == Cxe::AspectRatio16to9) {
        aspectRatio = QString("16:9");
    }
    CX_DEBUG(("Video aspect ratio: %s", aspectRatio.toAscii().data()));
}


/*!
* Helper method to enable debug prints.
@ param  Image quality preset values are printed out for debugging
*/
void CxeQualityPresetsSymbian::debugPrints(const CxeImageDetails &newPreset)
{
    CX_DEBUG(("Image quality details"));
    CX_DEBUG(("Image resolution (%d,%d)", newPreset.mWidth, newPreset.mHeight));
    CX_DEBUG(("Estimated size in bytes = %d)", newPreset.mEstimatedSize));
    CX_DEBUG(("Megapixels: %s", newPreset.mMpxCount.toAscii().data()));
    CX_DEBUG(("Image file extension: %s", newPreset.mImageFileExtension.toAscii().data()));

    QString aspectRatio;

    if (newPreset.mAspectRatio == Cxe::AspectRatio4to3) {
        aspectRatio = QString("4:3");
    } else {
        aspectRatio = QString("16:9");
    }
    CX_DEBUG(("Image aspect ratio: %s", aspectRatio.toAscii().data()));
}
