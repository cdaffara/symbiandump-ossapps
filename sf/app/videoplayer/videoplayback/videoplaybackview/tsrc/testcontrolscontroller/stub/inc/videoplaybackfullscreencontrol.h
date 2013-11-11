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
* Description:  Implementation of VideoPlaybackFullScreenControl
*
*/

// Version : %version:  4 %



#ifndef VIDEOPLAYBACKFULLSCREENCONTROL_H_
#define VIDEOPLAYBACKFULLSCREENCONTROL_H_

#include <qobject>

#include <videoplaybackcontrol.hrh>
#include <mpxplaybackframeworkdefs.h>

class HbWidget;
class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

class VideoPlaybackFullScreenControl : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackFullScreenControl( VideoPlaybackControlsController* controller,
                                        TVideoPlaybackControls index,
                                        HbWidget* widget,
                                        TUint controlproperties );

        virtual ~VideoPlaybackFullScreenControl();

    public:

        /**
        * Set visibility of each control
        */
        virtual void setVisibility( TMPXPlaybackState aState );

        /**
        * return control index
        */
        TVideoPlaybackControls controlIndex();

        /**
        * set changed volume
        */
        bool volumeChanged( int aVolume );

        /**
        * set changed duration
        */
        bool durationChanged( int duration );

        /**
        * set changed volume
        */
        bool positionChanged( int position );

        /**
        * Set changed position
        */
        bool aspectRatioChanged( int aspectRatio );

        /**
        * set changed volume
        */
        bool setDownloadSize( int size );

        /**
        * set changed state
        */
        void updateState( TMPXPlaybackState state );

        /*
         *  UpdateDownloadPosition
         *  updates the download ratio on the progress bar
         */
        bool updateDownloadPosition( int size );

        /*
         *  Update the controls with the file details
         */
        virtual void updateControlsWithFileDetails( VideoPlaybackViewFileDetails *details );

        virtual void setVisible( bool visible );

        virtual bool isVisible();

        virtual void updateControlProperties( TUint properties );

        void resetControl();

    public:
        int                       mPosition;
        int                       mDuration;
        int                       mAspectRatio;
        int                       mDownloadSize;
        int                       mDownloadPosition;
        int                       mVolume;

        bool                      mVisible;
        TMPXPlaybackState         mState;
        TMPXPlaybackState         mVisibilityState;

        VideoPlaybackControlsController* mController;
        HbWidget                 *mControl;
        TVideoPlaybackControls    mControlIndex;
        TUint                     mProperties;
        bool                      mControlReset;
};

#endif /*VIDEOPLAYBACKFULLSCREENCONTROL_H_*/

