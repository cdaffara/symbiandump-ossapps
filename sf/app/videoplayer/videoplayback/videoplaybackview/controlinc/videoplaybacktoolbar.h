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
* Description:  Implementation of VideoPlaybackToolBar
*
*/

// Version : %version: 6 %



#ifndef VIDEOPLAYBACKTOOLBAR_H_
#define VIDEOPLAYBACKTOOLBAR_H_

#include <QObject>

#include <MMFScalingCustomCommandConstants.h>
#include <mpxplaybackframeworkdefs.h>


class QTimer;
class HbIcon;
class HbAction;
class HbToolBar;
class HbToolButton;
class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

// DATA TYPES

enum TButton
{
    E1stButton,
    E2ndButton,
    E3rdButton,
    E4thButton,
    E5thButton,
    EButtonCount    // Should always be the last value
};

enum TButtonIcon
{
    ERWIcon,
    EPlayIcon,
    EPauseIcon,
    EFFIcon,
    ENaturalIcon,
    EStretchIcon,
    EZoomIcon,
    EDetailsIcon,
    EAttachIcon,
    EShareIcon,
    EIconCount    // Should always be the last value
};

enum TSeekingState
{
    ENotSeeking,
    EFastForwarding,
    ERewinding
};

const int KMPXFastForward = 30;
const int KMPXRewind = -10;

class VideoPlaybackToolBar : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackToolBar( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackToolBar();

    public:
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void initialize();
        void updateWithFileDetails( VideoPlaybackViewFileDetails* details );
        void positionChanged( int position );
        void durationChanged( int duration );
        void setVisible( bool visible );
        void resetControl();

    private slots:
        void playPause();
        void changeAspectRatio();
        void handleButtonPressed();
        void handleRWButtonPressed();
        void handleFFButtonPressed();
        void ffPressing();
        void rwPressing();
        void ffReleased();
        void rwReleased();
        void openDetailsView();
        void retrieveButtons();

    private:
        void setAspectRatioButton();

    private:
        VideoPlaybackControlsController *mController;
        HbToolBar                       *mToolBar;
        QTimer                          *mSeekStartTimer;
        QTimer                          *mRetrieveButtonTimer;

        QList<HbIcon*>                   mButtonIcons;
        QList<HbAction*>                 mButtonActions;
        QList<HbToolButton*>             mButtons;

        TSeekingState                    mSeekingState;

        bool    mInitialized;
        int     mPosition;
        int     mDuration;
        int     mAspectRatio;
};

#endif /*VIDEOPLAYBACKTOOLBAR_H_*/
