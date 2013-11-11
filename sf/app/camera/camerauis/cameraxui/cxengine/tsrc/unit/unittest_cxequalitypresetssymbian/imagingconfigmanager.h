/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fake implementation for the unittest_cxequalitypresetssymbian
*
*/


#ifndef IMAGINGCONFIGMANAGER_H
#define IMAGINGCONFIGMANAGER_H

#include <mmf\common\mmfutilities.h>

const TUint KMaxStringLength = 256;

/*
 * Video quality set structure
 */
class TVideoQualitySet
    {
    public:
        // Enum level for the Quality set, used as ID
        TUint iVideoQualitySetLevel;
        // Video file format mime type, e.g. "video/3gpp"
        TText8 iVideoFileMimeType[ KMaxStringLength ];
        // Video codec mime type, e.g. "video/mp4v-es"
        TText8 iVideoCodecMimeType[ KMaxStringLength ];
        // Video picture width in pixels (luminance), resolution width
        TInt iVideoWidth;
        // Video picture height in pixels (luminance), resolution height
        TInt iVideoHeight;
        // Video framerate in fps
        TReal iVideoFrameRate;
        // Video bitrate in bps
        TInt iVideoBitRate;
        // Video encoder UID
        TUid iVideoEncoderUID;
        // Random access point rate, in pictures per second. For example, to
        // request a random access point every ten seconds, set the value to
        // 0.1. Random access means in video case usually that an INTRA frame
        // is forced by the encoder to make the video accessible at that time
        // without depending on the previous frames
        TReal iRandomAccessRate;
        // Video's pixel aspect ratio numerator, e.g. in PAL 16:11 pixel
        // aspect ratio means that this value is set to 16
        TUint iVideoPixelAspectRatioNum;
        // Video's pixel aspect ratio denominator, e.g. in PAL 16:11 pixel
        // aspect ratio means that this value is set to 11
        TUint iVideoPixelAspectRatioDenom;
        // Preferred supplier for the MMF camcorder plug-in (e.g. "Nokia")
        TText8 iPreferredSupplier[ KMaxStringLength ];

        // Audio codec FourCC, e.g. " AMR"
        TFourCC iAudioFourCCType;
        // Audio bitrate in bps
        TInt iAudioBitRate;
        // Audio sampling rate in Hz
        TInt iAudioSamplingRate;
        // Number of audio channels; in practice mono(1) vs stereo(2)
        TInt iAudioChannels;
        // Audio encoder UID
        TUid iAudioEncoderUID;
        // Video recording A/V sync start delay (ms), only used by
        // CamcorderMMFPlugIn, but depends on the qualityset
        TInt iAVSyncStartDelay;
        // Video recording A/V sync resume delay (ms), only used by
        // CamcorderMMFPlugIn, but depends on the qualityset
        TInt iAVSyncResumeDelay;
        // A non-zero ID telling unique combination of camera (primary/
        // secondary) and/or display (cover UI/main display) and/or rotation
        // setting combined into a number for separating each setting set
        // type from others
        TUint iCameraDisplayId;
        // Camcorder specific field that can be either 1(true) or 0(false)
        // meaning if the current quality settings set should be shown on
        // Camcorder UI or not
        TUint iCamcorderVisible;
	};


/*
 * Image quality set structure
 */
class TImageQualitySet
    {
    public:
        // Enum level for the Quality set, used as ID
        TUint iImageQualitySetLevel;
        // Image file format mime type, e.g. "image/jpeg"
        TText8 iImageFileMimeType[ KMaxStringLength ];
        // Image picture width in pixels, resolution width
        TInt iImageWidth;
        // Image picture height in pixels, resolution height
        TInt iImageHeight;
        // Image extension, e.g. ".jpg"
        TText8 iImageFileExtension[ KMaxStringLength ];
        // Image encoder UID
        TUid iImageEncoderUID;
        // Compression quality, determines wether file size or image quality
        // is preferred over the other
        TInt iCompressionQuality;
        // Estimated image size, in bytes
        TInt iEstimatedSize;
        // A non-Zero ID telling unique combination of camera (primary/
        // secondary) and/or display (cover UI/main display) and/or rotation
        // setting combined into a number for separating each setting set type
        // from others
        TUint iCameraDisplayId;
        // Camcorder specific field that can be either 1(true) or 0(false)
        // meaning if the current quality settings set should be shown on
        // Camcorder UI or not
        TUint iCamcorderVisible;
    };

/*
 * Camcorder MMF Plug-in Specific settings structure
 *
 */
class TCamcorderMMFPluginSettings
	{
	public:
		// Video framerate in nightmode (fps)
		TReal iVideoNightFrameRate;
		// Video bitrate scaler for remaining time calculation during
		// first 3seconds.
		TReal iCMRAvgVideoBitRateScaler;
		// Video Complexity Setting
		TInt iVideoComplexitySetting;
		// Quality is more important in camcorder than delay
		TReal iCMRLatencyQualityTradeoff;
		// range is [0...100]
		TUint iCMRPictureQuality;
		// Try to maintain the frame-rate,
		// 1.0 means the picture quality is sacrificed
		// "all the way" to match the target fps
		TReal iCMRQualityTemporalTradeoff;
		// number of camera buffers to use when using HW accelerated encoder
		TUint iCMRNumCameraBuffers;
		// number of camera buffers to use when using SW encoder
		TUint iCMRNumCameraBuffersARM;
		// min number of output (bitstream) buffers
		TUint iCMRMinNumOutputBuffers;
	};

/**
 *  Fake API class for emulate imaging configurations and settings
 */
class CImagingConfigManager : public CBase
    {
    public:

        // two phase constructor
        static CImagingConfigManager* NewL();


        // Destructor
        ~CImagingConfigManager();

        // Returns number of video quality levels
        TInt NumberOfVideoQualityLevels() const;

        // Get video quality levels
        TInt GetVideoQualityLevelsL( CArrayFixFlat<TUint>& aLevels,
                                              TUint aCameraDisplayID = 0 );

        /// returns video quality set and symbian error code if any.
        TInt GetVideoQualitySet( TVideoQualitySet& aSet,
                                          TInt aLevel,
                                          TUint aCameraDisplayID = 0 );

        // Get all settings for Camcorder
        TInt GetCamcorderMMFPluginSettings(TCamcorderMMFPluginSettings& aSet) const;

        // returns number of image quality levels
        TInt NumberOfImageQualityLevels() const;

        // Get image quality levels
        TInt GetImageQualityLevelsL( CArrayFixFlat<TUint>& aLevels,
                                              TUint aCameraDisplayID = 0 );

        // Get image quality set
        TInt GetImageQualitySet( TImageQualitySet& aSet,
                                          TInt aLevel,
                                          TUint aCameraDisplayID = 0 );

    private:
        CImagingConfigManager();
        // helper method to init image quality set
        void initImageQualitySet(TImageQualitySet& aSet);
        void initVideoQualitySet(TVideoQualitySet& aSet);
    private:
        CArrayFixFlat< TVideoQualitySet >* iVideoQualitySets;
        CArrayFixFlat< TImageQualitySet >* iImageQualitySets;

    };

#endif //IMAGINGCONFIGMANAGER_H
