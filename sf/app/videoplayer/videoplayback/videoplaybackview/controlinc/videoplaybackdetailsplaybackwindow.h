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
* Description:  Implementation of VideoPlaybackDetailsPlaybackWindow
*
*/

// Version : %version:  9 %



#ifndef VIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_
#define VIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class HbPushButton;
class VideoPlaybackControlsController;

class VideoPlaybackDetailsPlaybackWindow : public HbWidget
{
    Q_OBJECT

    public:
        VideoPlaybackDetailsPlaybackWindow( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackDetailsPlaybackWindow();
        void initialize();
        void updateState( TMPXPlaybackState state );

    private slots:
        void play();

    private:
        VideoPlaybackControlsController *mController;
        HbPushButton                        *mPlayButton;

        bool                                 mInitialized;
};

#endif /*VIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_*/

