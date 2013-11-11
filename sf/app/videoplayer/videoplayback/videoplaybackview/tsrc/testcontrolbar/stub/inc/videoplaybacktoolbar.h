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
* Description:  Implementation of VideoPlaybackToolBar
*
*/

// Version : %version:  3 %



#ifndef VIDEOPLAYBACKTOOLBAR_H_
#define VIDEOPLAYBACKTOOLBAR_H_

#include <QWidget>
#include <mpxplaybackframeworkdefs.h>



class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;


class VideoPlaybackToolBar : public QWidget
{
    Q_OBJECT

    public:
        VideoPlaybackToolBar( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackToolBar();
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void initialize();
        void updateWithFileDetails( VideoPlaybackViewFileDetails* details );
        void durationChanged( int duration );
        void positionChanged( int position );
        void setVisible( bool visible );
        void resetControl();

    public:
        VideoPlaybackControlsController *mController;
        VideoPlaybackViewFileDetails *mFileDetails;
        int mAspectRatio;
        int mPosition;
        int mDuration;
        bool mVisible;
        bool mReset;
        TMPXPlaybackState mState;
};

#endif /*VIDEOPLAYBACKTOOLBAR_H_*/
