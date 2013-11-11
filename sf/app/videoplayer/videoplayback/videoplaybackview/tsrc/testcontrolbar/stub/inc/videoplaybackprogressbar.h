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
* Description:  Implementation of VideoPlaybackProgressBar
*
*/

// Version : %version:  3 %



#ifndef VIDEOPLAYBACKPROGRESSBAR_H_
#define VIDEOPLAYBACKPROGRESSBAR_H_

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

class VideoPlaybackProgressBar : public HbWidget
{
    Q_OBJECT

    public:
        VideoPlaybackProgressBar( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackProgressBar();
        void initialize();
        void updateWithFileDetails( VideoPlaybackViewFileDetails* details );
        void updateState( TMPXPlaybackState state );
        void resetControl();

    public:
        void durationChanged( int duration );
        void positionChanged( int position );

    public:
        VideoPlaybackControlsController *mController;
        VideoPlaybackViewFileDetails *mFileDetails;
        TMPXPlaybackState mState;
        bool mReset;
        int mDuration;
        int mPosition;
};

#endif /*VIDEOPLAYBACKPROGRESSBAR_H_*/

