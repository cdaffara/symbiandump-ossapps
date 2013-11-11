/**
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
* Description: Unit test for mpvolumeslider
*
*/

#ifndef TESTMPVOLUMESLIDER_H
#define TESTMPVOLUMESLIDER_H

#include <QtTest/QtTest>
#include "mpcommondefs.h"

class MpVolumeSlider;

class TestMpVolumeSlider : public QObject
{
    Q_OBJECT

public:
    
    TestMpVolumeSlider();
    virtual ~TestMpVolumeSlider();

signals:
    void triggerVolumePropertyChanged( MpCommon::MpVolumeProperty property, int value );
    void triggerSliderPressed();
    void triggerSliderReleased();
    void triggerIconClicked();
    void triggerValueChanged( int value );

// from QtTest
public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

// internal
private slots:

    void testConstructor();
    void testShowVolumeSlider();
    void testHandleVolumeProperty();
    void testSliderPressed();
    void testSliderReleased();
    void testMuteIconClicked();
    void testVolumeSliderChanged();
    void testInitializeVolumeSliderPopup();

private:

    MpVolumeSlider           *mTest;


};

#endif  // TESTMPVOLUMESLIDER_H

