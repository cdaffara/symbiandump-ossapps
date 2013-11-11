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
* Description:  Implementation of VideoPlaybackControlBar
*
*/

// Version : %version: 4 %



#ifndef VIDEOPLAYBACKCONTROLBAR_H_
#define VIDEOPLAYBACKCONTROLBAR_H_



#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

class VideoPlaybackControlBar : public HbWidget
{
    Q_OBJECT

    public:
        VideoPlaybackControlBar( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackControlBar();
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void updateWithFileDetails( VideoPlaybackViewFileDetails* details );
        void setVisibleToControlBar( bool visible );
        void durationChanged( int duration );
        void positionChanged( int position );
        void resetControl();

    public:
        VideoPlaybackControlsController *mController;
        int  mPosition;
        int  mDuration;
        int  mAspectRatio;
        bool mVisibility;
        bool mReset;
        TMPXPlaybackState mState;
        VideoPlaybackViewFileDetails *mFileDetails;
};

#endif /*VIDEOPLAYBACKCONTROLBAR_H_*/
