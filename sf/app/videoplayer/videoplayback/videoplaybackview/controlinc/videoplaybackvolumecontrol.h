/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of VideoPlaybackVolumeControl
*
*/

// Version : %version:  1 %



#ifndef VIDEOPLAYBACKVOLUMECONTROL_H_
#define VIDEOPLAYBACKVOLUMECONTROL_H_


// INCLUDES
#include <QObject>

// FORWARD DECLARATIONS
class QTimer;
class HbVolumeSliderPopup;
class VideoPlaybackControlsController;

class VideoPlaybackVolumeControl : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackVolumeControl( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackVolumeControl();

    public:
        void volumeChanged( int volume );
        void setVisible( bool visible );
        bool isVisible();

    private:
        void initialize();
        void setVolume( int volume );

    private slots:
        void handleSliderValueChanged( int volume );
        void handleMuteIconClicked();
        void handleSliderPressed();
        void handleSliderReleased();
        void handleDraggingTimerTimeOut();

    private:
        VideoPlaybackControlsController *mController;
        HbVolumeSliderPopup             *mVolumePopup;
        QTimer                          *mDraggingHandlerTimer;

        QList<int>  mExpectedVolList;
        int         mVolume;
        int         mDraggingVolume;
        int         mVolumeNormalizer;
        bool        mMutedByMuteIcon;
        bool        mDragging;
};

#endif /*VIDEOPLAYBACKVOLUMECONTROL_H_*/
