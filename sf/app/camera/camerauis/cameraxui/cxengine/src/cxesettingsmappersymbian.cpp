/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QRegExp>
#include <ecamadvsettingsuids.hrh>

#include "cxeautofocuscontrol.h"
#include "cxesettingsmappersymbian.h"
#include "cxutils.h"
#include "cxenamespace.h"

using namespace Cxe;



/*!
    Map Cxe::Whitebalance value to its corresponding CCamera value.
*/
CCamera::TWhiteBalance CxeSettingsMapperSymbian::Map2CameraWb(
        Cxe::Whitebalance whiteBalance )
{
    CCamera::TWhiteBalance wb(CCamera::EWBAuto);

    switch (whiteBalance) {
        case WhitebalanceSunny:
            wb = CCamera::EWBDaylight;
            break;
        case WhitebalanceCloudy:
            wb = CCamera::EWBCloudy;
            break;
        case WhitebalanceIncandescent:
            wb = CCamera::EWBTungsten;
            break;
        case WhitebalanceFluorescent:
            wb = CCamera::EWBFluorescent;
            break;
        case WhitebalanceAutomatic:
        default:
            wb = CCamera::EWBAuto;
            break;
    }
    return wb;
}


/*!
    Map Cxe::FlashMode to its corresponding CCamera value.
*/
CCamera::TFlash CxeSettingsMapperSymbian::Map2CameraFlash(
        Cxe::FlashMode flashMode)
{
    CCamera::TFlash flash(CCamera::EFlashAuto);

    switch (flashMode) {
        case FlashAntiRedEye:
            flash = CCamera::EFlashRedEyeReduce;
            break;
        case FlashOn:
            flash = CCamera::EFlashForced;
            break;
        case FlashOff:
            flash = CCamera::EFlashNone;
            break;
        case FlashAuto:
        default:
            flash = CCamera::EFlashAuto;
            break;
    }
    return flash;
}


/*!
    Map Cxe::ExposureMode to its corresponding CCamera value.
*/
CCamera::TExposure CxeSettingsMapperSymbian::Map2CameraExposureMode(
        Cxe::ExposureMode exposureMode)
{
    CCamera::TExposure expo(CCamera::EExposureAuto);

    switch (exposureMode) {
        case ExposureNight:
            expo = CCamera::EExposureNight;
            break;
        case ExposureBacklight:
            expo = CCamera::EExposureBacklight;
            break;
        case ExposureSport:
            expo = CCamera::EExposureSport;
            break;
        case ExposureAuto:
        default:
            expo = CCamera::EExposureAuto;
            break;
    }

    return expo;
}


/*!
    Map Cxe::ExposureMode to its corresponding CCamera value.
*/
CCamera::CCameraImageProcessing::TEffect
        CxeSettingsMapperSymbian::Map2CameraEffect(Cxe::Colortone colorTone)
{
    CCamera::CCameraImageProcessing::TEffect effect(
            CCamera::CCameraImageProcessing::EEffectNone);

    switch(colorTone) {
        case ColortoneBlackAndWhite:
            effect = CCamera::CCameraImageProcessing::EEffectMonochrome;
            break;
        case ColortoneSepia:
            effect = CCamera::CCameraImageProcessing::EEffectSepia;
            break;
        case ColortoneNegative:
            effect = CCamera::CCameraImageProcessing::EEffectNegative;
            break;
        case ColortoneVivid:
            effect = CCamera::CCameraImageProcessing::EEffectVivid;
            break;
        case ColortoneNormal:
        default:
            effect = CCamera::CCameraImageProcessing::EEffectNone;
            break;
    }
    return effect;
}

/*!
    Map CxeAutoFocusControl::Mode to its corresponding CCamera value.
*/
CCamera::CCameraAdvancedSettings::TFocusRange
CxeSettingsMapperSymbian::Map2CameraAutofocus(CxeAutoFocusControl::Mode afMode)
{
    CCamera::CCameraAdvancedSettings::TFocusRange value;

    switch (afMode) {
        case CxeAutoFocusControl::Macro:
            value = CCamera::CCameraAdvancedSettings::EFocusRangeMacro;
            break;
        case CxeAutoFocusControl::Portrait:
            value = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
            break;
        case CxeAutoFocusControl::Infinity:
            value = CCamera::CCameraAdvancedSettings::EFocusRangeInfinite;
            break;
        case CxeAutoFocusControl::Hyperfocal:
            value = CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal;
            break;
        case CxeAutoFocusControl::Auto:     // Fall through
        default:
            value = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
            break;
    }
    return value;
}



/*!
    Map Cxe::DeviceOrientation to MCameraOrientation.
*/
MCameraOrientation::TOrientation
        CxeSettingsMapperSymbian::Map2CameraOrientation(Cxe::DeviceOrientation
                                                        uiOrientation)
{
    MCameraOrientation::TOrientation cameraOrientation;

    switch (uiOrientation) {
        case Orientation90:
            cameraOrientation = MCameraOrientation::EOrientation90;
            break;
        case Orientation180:
            cameraOrientation = MCameraOrientation::EOrientation180;
            break;
        case Orientation270:
            cameraOrientation = MCameraOrientation::EOrientation270;
            break;
        case Orientation0: // default
        default:
            cameraOrientation = MCameraOrientation::EOrientation0;
            break;
        }
    return cameraOrientation;
}

/*!
    Helper data structure to define how MIME types are mapped to
    MCameraUseCaseHint enumerations.
*/
class CxeCodecMapping
{
public:
    CxeCodecMapping(const char *regexp, MCameraUseCaseHint::TVideoCodec codec,
                    MCameraUseCaseHint::TVideoProfile profile) :
    mRegExp(regexp),
    mCodec(codec),
    mProfile(profile)
    {
    // No implementation needed
    }

public:
    QString mRegExp;
    MCameraUseCaseHint::TVideoCodec mCodec;
    MCameraUseCaseHint::TVideoProfile mProfile;
};

/*!
    Map video codec MIME type from ICM to enumerations used by the
    MCameraUseCaseHint custom interface

    \param[in] videoDetails CxeVideoDetails defining the current video quality.
    \param[out] codec       Reference where to store the video codec info.
                            MCameraUseCaseHint::ECodecUnknown if not known.
    \param[out] profile     Reference where to store the video profile info.
                            MCameraUseCaseHint::EProfileUnknown if not known.
*/
void CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(
        const CxeVideoDetails &videoDetails,
        MCameraUseCaseHint::TVideoCodec &codec,
        MCameraUseCaseHint::TVideoProfile &profile)
{
    typedef QPair<MCameraUseCaseHint::TVideoCodec,
                  MCameraUseCaseHint::TVideoProfile> CxeCodecAndProfile;

    codec = MCameraUseCaseHint::ECodecUnknown;
    profile = MCameraUseCaseHint::EProfileUnknown;

    // Create a mapping table for mapping from the MIME type string
    // to MCameraUseCaseHint codec and profile enums using QRegExp. Specific
    // rules should be before more generic ones because the map is iterated in
    // order.
    QList<CxeCodecMapping> mappingTable;

    mappingTable << CxeCodecMapping("^video/H263-2000",
                                    MCameraUseCaseHint::ECodecH263,
                                    MCameraUseCaseHint::EProfileH263P0L10)

                 << CxeCodecMapping("^video/H264.*profile-level-id=42801E",
                                    MCameraUseCaseHint::ECodecH264,
                                    MCameraUseCaseHint::EProfileH264BpL3)

                 << CxeCodecMapping("^video/H264.*profile-level-id=42801F",
                                    MCameraUseCaseHint::ECodecH264,
                                    MCameraUseCaseHint::EProfileH264BpL3_1)

                 << CxeCodecMapping("^video/H264", // Other H.264 profile
                                    MCameraUseCaseHint::ECodecH264,
                                    MCameraUseCaseHint::EProfileUnknown)

                 << CxeCodecMapping("^video/mp4v-es.*profile-level-id=2",
                                    MCameraUseCaseHint::ECodecMpeg4,
                                    MCameraUseCaseHint::EProfileMPEG4SpL2)

                 << CxeCodecMapping("^video/mp4v-es.*profile-level-id=3",
                                    MCameraUseCaseHint::ECodecMpeg4,
                                    MCameraUseCaseHint::EProfileMPEG4SpL3)

                 << CxeCodecMapping("^video/mp4v-es.*profile-level-id=4",
                                    MCameraUseCaseHint::ECodecMpeg4,
                                    MCameraUseCaseHint::EProfileMPEG4SpL4a)

                 << CxeCodecMapping("^video/mp4v-es", // Other MPEG-4 profile
                                    MCameraUseCaseHint::ECodecMpeg4,
                                    MCameraUseCaseHint::EProfileUnknown);

    bool found = false;
    for (int i = 0; i < mappingTable.count() && !found; i++) {
        QRegExp regExp(mappingTable[i].mRegExp, Qt::CaseInsensitive);
        if (regExp.indexIn(videoDetails.mVideoCodecMimeType) >= 0) {
            found = true;
            codec = mappingTable[i].mCodec;
            profile = mappingTable[i].mProfile;

            CX_DEBUG(("Matched codec %d, profile 0x%x for '%s'", codec, profile,
                     videoDetails.mVideoCodecMimeType.toAscii().constData()));
        }
    }

    if (!found) {
        CX_DEBUG(("No codec/profile found for '%s'",
                 videoDetails.mVideoCodecMimeType.toAscii().constData()));
    }
}

// end of file
