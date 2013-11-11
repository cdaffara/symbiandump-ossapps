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
* Description: Music Player volume slider.
*
*/

#ifndef MPVOLUMESLIDER_H
#define MPVOLUMESLIDER_H

#include <QObject>
#include "mpcommondefs.h"

class MpEngine;
class HbVolumeSliderPopup;

class MpVolumeSlider : public QObject
{
    Q_OBJECT

public:

    explicit MpVolumeSlider( QObject *parent = 0 );
    virtual ~MpVolumeSlider();
    void showVolumeSlider();

public slots:

    // To handle Engine signals
    void handleVolumeProperty( MpCommon::MpVolumeProperty property, int value );

    // To handle VolumeSlider signals
    void sliderPressed();
    void sliderReleased();
    void muteIconClicked();
    void volumeSliderChanged( int value );

private:

    void initializeVolumeSliderPopup();

private:
    Q_DISABLE_COPY(MpVolumeSlider)

    MpEngine                    *mMpEngine;             // not owned
    HbVolumeSliderPopup         *mVolumeSliderPopup;    // owned
    int                          mEngineVolumeMax;
    int                          mEngineVolumeLevel;
    bool                         mEngineMuted;
    int                          mEngineDataCompleted;  // 0x01 volume max, 0x02 volume level, 0x04 volume mute
    bool                         mSliderPressed;
    int                          mSliderStep;           // VolumeLevelMax / DefaultSteps
};

#endif // MPVOLUMESLIDER_H
