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
#include <QTest>
#include <ecam.h>
#include <ecamadvsettingsuids.hrh>
#include <ecamadvsettings.h>
#include <ECamOrientationCustomInterface2.h>

#include "cxenamespace.h"
#include "cxesettingsmappersymbian.h"
#include "cxequalitydetails.h"
#include "unittest_cxesettingsmappersymbian.h"


// Enums


UnitTestCxeSettingsMapperSymbian::UnitTestCxeSettingsMapperSymbian()
{
}

UnitTestCxeSettingsMapperSymbian::~UnitTestCxeSettingsMapperSymbian()
{
}

// Run before each individual test case
void UnitTestCxeSettingsMapperSymbian::init()
{
}

// Run after each individual test case
void UnitTestCxeSettingsMapperSymbian::cleanup()
{
}

void UnitTestCxeSettingsMapperSymbian::testMap2CameraAutofocus()
{
    CCamera::CCameraAdvancedSettings::TFocusRange result;
    CxeAutoFocusControl::Mode initialData = CxeAutoFocusControl::Auto;

    // case 0: check with autofocus auto value
    result = CxeSettingsMapperSymbian::Map2CameraAutofocus(initialData);
    QVERIFY(result == CCamera::CCameraAdvancedSettings::EFocusRangeAuto);

    // case 1: try with valid ui autofocus value
    initialData = CxeAutoFocusControl::Hyperfocal;
    result = CxeSettingsMapperSymbian::Map2CameraAutofocus(initialData);
    QVERIFY(result == CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal);

}

void UnitTestCxeSettingsMapperSymbian::testMap2CameraEffect()
{
    CCamera::CCameraImageProcessing::TEffect result;
    Cxe::Colortone initialData = static_cast<Cxe::Colortone>(99999);

    // case 0: check with unknown value
    result = CxeSettingsMapperSymbian::Map2CameraEffect(initialData);
    QVERIFY(result == CCamera::CCameraImageProcessing::EEffectNone);

    // case 1: try with valid ui color tone value
    initialData = Cxe::ColortoneSepia;
    result = CxeSettingsMapperSymbian::Map2CameraEffect(initialData);
    QVERIFY(result == CCamera::CCameraImageProcessing::EEffectSepia);

    // case 2: try with direct value which is invalid
    result = CxeSettingsMapperSymbian::Map2CameraEffect(
            static_cast<Cxe::Colortone>(-1));
    QVERIFY(result == CCamera::CCameraImageProcessing::EEffectNone);

}

void UnitTestCxeSettingsMapperSymbian::testMap2CameraExposureMode()
{
    CCamera::TExposure result;
    Cxe::ExposureMode initialData = static_cast<Cxe::ExposureMode>(-1);

    // case 0: check with unknown value
    result = CxeSettingsMapperSymbian::Map2CameraExposureMode(initialData);
    QVERIFY(result == CCamera::EExposureAuto);

    // case 1: try with valid ui exposure value
    initialData = Cxe::ExposureNight;
    result = CxeSettingsMapperSymbian::Map2CameraExposureMode(initialData);
    QVERIFY(result == CCamera::EExposureNight);

    // case 2: try with direct value which is invalid
    result = CxeSettingsMapperSymbian::Map2CameraExposureMode(
        static_cast<Cxe::ExposureMode>(99999));
    QVERIFY(result == CCamera::EExposureAuto);
}

void UnitTestCxeSettingsMapperSymbian::testMap2CameraFlash()
{
    CCamera::TFlash result;
    Cxe::FlashMode initialData = static_cast<Cxe::FlashMode>(-1);

    // case 0: check with unknown value
    result = CxeSettingsMapperSymbian::Map2CameraFlash(initialData);
    QVERIFY(result == CCamera::EFlashAuto);

    // case 1: try with valid ui flash value
    initialData = Cxe::FlashOn;
    result = CxeSettingsMapperSymbian::Map2CameraFlash(initialData);
    QVERIFY(result == CCamera::EFlashForced);

    // case 2: try with direct value which is invalid
    result = CxeSettingsMapperSymbian::Map2CameraFlash(
            static_cast<Cxe::FlashMode>(99999));
    QVERIFY(result == CCamera::EFlashAuto);
}


void UnitTestCxeSettingsMapperSymbian::testMap2CameraOrientation()
{
    MCameraOrientation::TOrientation result;
    Cxe::DeviceOrientation initialData = Cxe::Orientation0;

    // case 0: check with unknown value
    result = CxeSettingsMapperSymbian::Map2CameraOrientation(initialData);
    QVERIFY(result == MCameraOrientation::EOrientation0);

    // case 1: try with valid ui camera orientation value
    initialData = Cxe::Orientation90;
    result = CxeSettingsMapperSymbian::Map2CameraOrientation(initialData);
    QVERIFY(result == MCameraOrientation::EOrientation90);
}

void UnitTestCxeSettingsMapperSymbian::testMap2CameraWb()
{
    CCamera::TWhiteBalance result;
    Cxe::Whitebalance initialData = static_cast<Cxe::Whitebalance>(-1);

    // case 0: check with unknown value
    result = CxeSettingsMapperSymbian::Map2CameraWb(initialData);
    QVERIFY(result == CCamera::EWBAuto);

    // case 1: try with valid ui whitebalance value
    initialData = Cxe::WhitebalanceSunny;
    result = CxeSettingsMapperSymbian::Map2CameraWb(initialData);
    QVERIFY(result == CCamera::EWBDaylight);

    // case 2: try with direct value which is invalid
    result = CxeSettingsMapperSymbian::Map2CameraWb(
            static_cast<Cxe::Whitebalance>(99999));
    QVERIFY(result == CCamera::EWBAuto);
}

void UnitTestCxeSettingsMapperSymbian::testMap2UseCaseHintVideoParameters()
{
    CxeVideoDetails details;
    MCameraUseCaseHint::TVideoCodec codec = MCameraUseCaseHint::ECodecUnknown;
    MCameraUseCaseHint::TVideoProfile profile = MCameraUseCaseHint::EProfileUnknown;

    details.mVideoCodecMimeType = "video/H263-2000";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecH263);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileH263P0L10);

    details.mVideoCodecMimeType = "video/mp4v-es; profile-level-id=2";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecMpeg4);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileMPEG4SpL2);

    details.mVideoCodecMimeType = "video/mp4v-es; profile-level-id=3";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecMpeg4);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileMPEG4SpL3);

    details.mVideoCodecMimeType = "video/mp4v-es; profile-level-id=4";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecMpeg4);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileMPEG4SpL4a);

    details.mVideoCodecMimeType = "video/mp4v-es; profile-level-id=foobar";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecMpeg4);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileUnknown);

    details.mVideoCodecMimeType = "video/H264; profile-level-id=42801E";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecH264);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileH264BpL3);

    details.mVideoCodecMimeType = "video/H264; profile-level-id=42801F";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecH264);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileH264BpL3_1);

    details.mVideoCodecMimeType = "video/H264; profile-level-id=foobar";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecH264);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileUnknown);

    details.mVideoCodecMimeType = "invalid";
    CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(details, codec, profile);
    QCOMPARE(codec, MCameraUseCaseHint::ECodecUnknown);
    QCOMPARE(profile, MCameraUseCaseHint::EProfileUnknown);
}

// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeSettingsMapperSymbian);
