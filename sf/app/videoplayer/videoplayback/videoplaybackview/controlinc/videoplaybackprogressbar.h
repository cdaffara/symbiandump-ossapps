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
* Description:  Implementation of VideoPlaybackProgressBar
*
*/

// Version : %version: da1mmcf#17 %



#ifndef VIDEOPLAYBACKPROGRESSBAR_H_
#define VIDEOPLAYBACKPROGRESSBAR_H_

#include <QObject>
#include <hbextendedlocale.h>
#include <mpxplaybackframeworkdefs.h>

class QTimer;
class HbProgressSlider;
class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

class VideoPlaybackProgressBar : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackProgressBar( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackProgressBar();
        void initialize();
        void updateWithFileDetails( VideoPlaybackViewFileDetails* details );
        void updateState( TMPXPlaybackState state );

    public:
        void durationChanged( int duration );
        void positionChanged( int position );
        void resetControl();

    private slots:
        void handleSliderPressed();
        void handleSliderMoved( int value );
        void handleSliderReleased();
        void handleSeekingTimeout();

    private:
        QString valueToReadableFormat( int value );
        void updatePostion( int position );
        void setEnableProgressSlider( bool enable );

    private:
        VideoPlaybackControlsController *mController;
        HbProgressSlider                *mProgressSlider;

        int     mDuration;
        int     mDraggingPosition;
        int     mSetPosition;

        bool    mNeedToResumeAfterSetPosition;
        bool    mInitialized;
        bool    mSliderDragging;
        bool    mLongTimeFormat;
        bool    mLiveStreaming;

        QTimer *mSeekingTimer;
        HbExtendedLocale mLocale;
};

#endif /*VIDEOPLAYBACKPROGRESSBAR_H_*/

