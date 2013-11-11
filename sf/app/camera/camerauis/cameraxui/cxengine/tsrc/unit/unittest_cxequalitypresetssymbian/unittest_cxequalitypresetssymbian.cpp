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

#include <QTest>
#include <QList>

// "fake" system include
#include "imagingconfigmanager.h"

#include "cxefakesettings.h"
#include "unittest_cxequalitypresetssymbian.h"
#include "cxequalitypresetssymbian.h"
#include "cxequalitydetails.h"
#include "cxutils.h"
#include "cxenamespace.h"

UnitTestCxeQualityPresetsSymbian::UnitTestCxeQualityPresetsSymbian()
{
}


UnitTestCxeQualityPresetsSymbian::~UnitTestCxeQualityPresetsSymbian()
{
    cleanup();
}


void UnitTestCxeQualityPresetsSymbian::init()
{
    mSettings = new CxeFakeSettings();
    mQualityPresets = new CxeQualityPresetsSymbian(*mSettings);
}

void UnitTestCxeQualityPresetsSymbian::cleanup()
{
    delete mQualityPresets;
    mQualityPresets = NULL;

    delete mSettings;
    mSettings = NULL;
}

void UnitTestCxeQualityPresetsSymbian::testImageQualityPresets()
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<CxeImageDetails> imagePresets =
        mQualityPresets->imageQualityPresets(Cxe::PrimaryCameraIndex);

    QVERIFY(imagePresets.count() > 0);

    QCOMPARE(imagePresets[0].mWidth, 0);
    QCOMPARE(imagePresets[0].mHeight, 0);

    imagePresets.clear();
    imagePresets = mQualityPresets->imageQualityPresets(Cxe::SecondaryCameraIndex);
    QVERIFY(imagePresets.count() > 0);

    QCOMPARE(imagePresets[0].mWidth, 0);
    QCOMPARE(imagePresets[0].mHeight, 0);

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeQualityPresetsSymbian::testVideoQualityPresets()
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<CxeVideoDetails> videoPresets =
        mQualityPresets->videoQualityPresets(Cxe::PrimaryCameraIndex);

    QVERIFY(videoPresets.count() > 0);

    //QCOMPARE(videoPresets[0].mWidth, 640);
    //QCOMPARE(videoPresets[0].mHeight, 360);
    QCOMPARE(videoPresets[0].mWidth, 0);
    QCOMPARE(videoPresets[0].mHeight, 0);

    videoPresets.clear();
    videoPresets = mQualityPresets->videoQualityPresets(Cxe::SecondaryCameraIndex);
    QVERIFY(videoPresets.count() > 0);

    //QCOMPARE(videoPresets[0].mWidth, 640);
    //QCOMPARE(videoPresets[0].mHeight, 360);
    QCOMPARE(videoPresets[0].mWidth, 0);
    QCOMPARE(videoPresets[0].mHeight, 0);

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeQualityPresetsSymbian::testRecordingTimeAvailable()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    // first call videoQualityPresets
    // to fetch the quality values for video recording
    //mQualityPresets->videoQualityPresets(Cxe::PrimaryCameraIndex);

    int time(0);

    CxeVideoDetails details;
    details.mAudioBitRate = 100;
    details.mVideoBitRate = 100;

    // Check zero time when disk full.
    time = mQualityPresets->recordingTimeAvailable(details, qint64(0));
    QVERIFY(time == 0);

    // Common time limit is 90 minutes
    details.mMaximumSizeInBytes = qint64(1000*1000*1000);
    time = mQualityPresets->recordingTimeAvailable(details, qint64(1000*1000*1000));
    QVERIFY(time == 5400);

    // Normal case, no quality specific limit
    time = mQualityPresets->recordingTimeAvailable(details, qint64(1000*1000));

    // Correct calculation of the recording time available
    // requires implementation of the CImagingConfigManager::GetCamcorderMMFPluginSettings()
    // in tsrc\unit\system_include\imagingconfigmanager.cpp
    // Just using a default value for testing purposes
    QVERIFY(time == 5400);

    // Normal case, quality specifies limit
    details.mMaximumSizeInBytes = 10*1000;
    time = mQualityPresets->recordingTimeAvailable(details, qint64(1000*1000));
    QVERIFY(time > 0);
    QVERIFY(time < 5400);

    CX_DEBUG_EXIT_FUNCTION();
}


// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeQualityPresetsSymbian);

// End of file
