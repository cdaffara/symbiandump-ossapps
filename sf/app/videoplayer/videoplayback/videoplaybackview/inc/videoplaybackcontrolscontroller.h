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
* Description:  Implementation of VideoPlaybackControlsController
*
*/

// Version : %version: da1mmcf#29 %



#ifndef VIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define VIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <QObject>
#include <QPixmap>

#include <xqserviceutil.h>
#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "videoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackviewfiledetails.h"

// FORWARD DECLARATIONS
class QTimer;
class QString;
class HbAction;
class ThumbnailManager;
class ShareUi;
class VideoBasePlaybackView;
class VideoPlaybackVolumeControl;
class VideoPlaybackControlPolicy;
class VideoPlaybackDocumentLoader;
class VideoPlaybackFullScreenControl;
class VideoPlaybackControlsController;
class VideoPlaybackControlConfiguration;
class VideoServices;

// DATA TYPES

enum TTimerAction
{
    ETimerCancel,
    ETimerReset
};

enum TThumbNailState
{
    EThumbNailEmpty,
    EThumbNailNotAvailable,
    EThumbNailRequsted,
    EThumbNailSet
};

enum TPlaybackViewMode
{
    EFullScreenView,
    EDetailsView,
    EAudioOnlyView
};

const int KControlsTimeOut = 4000;
const int KRNLogoTimeOut = 600;

const QString KPLAYBACKVIEW_DOCML = ":/videoplaybackview/videoplaybackview.docml";


// CLASS DECLARATION

class VideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        VideoPlaybackControlsController(
                VideoBasePlaybackView *view,
                CMPXVideoViewWrapper *viewWrapper,
                VideoPlaybackViewFileDetails *details );

        /**
        * Destructor.
        */
        virtual ~VideoPlaybackControlsController();

    public:

        /**
        * Handle event from container
        */
        void handleEvent( TVideoPlaybackControlCommandIds event, int value = 0 );

        /**
        * Command handling function.
        * Call HandleCommandL() of container
        */
        void handleCommand( TMPXVideoPlaybackViewCommandIds command, int value = 0 );

        /*
        * Return ETrue if TV-out cable gets connected
        */
        inline bool isTvOutConnected();

        /**
        * Reset or cancel timers for the controls
        */
        void resetDisappearingTimers( TTimerAction timerAction );

        /**
        * Return state
        */
        inline TMPXPlaybackState state();

        /**
        * Return file details
        */
        inline VideoPlaybackViewFileDetails* fileDetails();

        //
        //  Add the file details to the controls controller when available
        //
        void addFileDetails( VideoPlaybackViewFileDetails* details );

        inline VideoPlaybackDocumentLoader* layoutLoader();

        inline VideoBasePlaybackView* view();

        void updateVideoRectDone();

        inline TPlaybackViewMode viewMode();

        void evaluateAndChangeViewMode(
                TPlaybackViewMode viewMode = EFullScreenView, bool transitionEffect = false );

        inline bool isAttachOperation();

        inline bool isFileDetailsAdded();

        bool isRNLogoBitmapInControlList();

        bool shouldShowRNLogo();

        inline bool isService();

        inline int volumeSteps();

    private:
        /**
        * Initialize controller
        */
        void initializeController();

        /**
        * Update controls
        */
        void handleStateChange( TMPXPlaybackState newState );

        /**
        * Show Controls and reset the timers
        */
        void showControls();

        /**
        * Return ETrue if any control is visible
        */
        bool isVisible();

        /**
        * Append a control based on control index
        */
        void appendControl( TVideoPlaybackControls controlIndex );

        /**
        * Set changed volume
        */
        void volumeChanged( int volume );

        /**
        * Set changed duration
        */
        void durationChanged( int duration);

        /**
        * Set changed position
        */
        void positionChanged( int position );

        /**
        * Set changed position
        */
        void aspectRatioChanged( int aspectRatio );

        /*
         *  Sets the download size on the progress bar
         */
        void setDownloadSize( int size );

        /*
         *  Updates the download ratio on the progress bar
         */
        void updateDownloadPosition( int newSize );

        /**
        * Set changed state on the controls
        */
        void updateState();

        /**
        * Check whether this clip is real format or not
        */
        bool realFormat( QString filename );

        /**
        * Check whether this clip is real format or not for streaming/live streaming
        */
        bool realFormatForStreaming( const TDesC& des );

        /**
        * Check whether this clip is real format or not for local/progressive donwload
        */
        bool realFormatForLocal();

        /**
        * Handle errors
        */
        void handleErrors();

        /**
		* Return true if control is visible
		*/
		bool isSoftKeyVisible();

		void updateVideoRect(  bool transitionEffect = true );

		void showVolumeControls();

		void setDefaultBitmap();

		void generateThumbNail();

		void resetControls();

    signals:

        /**
        * Signals the file path of the current video to VideoServices
        *
        * @param filePath of the video
        */
        void attachVideoPath( const QString& );

    private slots:
        void hideAllControls();
        void skipToNextVideoItem();
        void skipToPreviousVideoItem();
        void handleTappedOnScreen();
        void handleThumbnailReady( QPixmap tnData, void *internal , int id, int error );
        void controlsListUpdated();
        void attachVideo();
        void sendVideo();
        void handleRNLogoVisibleChanged();
        void handleRNLogoTimeout();
        void handleOrientationChanged( Qt::Orientation orientation );

    private:
        VideoBasePlaybackView                     *mView;
        CMPXVideoViewWrapper                      *mViewWrapper;
        VideoPlaybackViewFileDetails              *mFileDetails;

        QList<VideoPlaybackFullScreenControl*>     mControls;

        VideoPlaybackControlPolicy                *mControlsPolicy;
        VideoPlaybackControlConfiguration         *mControlsConfig;

        QTimer                                    *mControlsTimer;
        QTimer                                    *mRNLogoTimer;

        VideoPlaybackDocumentLoader               *mLoader;
        VideoPlaybackVolumeControl                *mVolumeControl;

        ThumbnailManager                          *mThumbnailManager;
        VideoServices                             *mVideoServices;

        bool                                       mViewTransitionIsGoingOn;
        bool                                       mIsAttachOperation;
        bool                                       mFileDetailsAdded;
        bool                                       mShowControlsWhenInHorizontal;

        TThumbNailState                            mThumbNailState;

        TMPXPlaybackState                          mState;
        TPlaybackViewMode                          mViewMode;
        Qt::Orientation                            mOrientation;

        ShareUi                                    *mShareUi;
};

// INLINE METHODS
// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::state
// -------------------------------------------------------------------------------------------------
//
inline
TMPXPlaybackState VideoPlaybackControlsController::state()
{
    return mState;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::fileDetails
// -------------------------------------------------------------------------------------------------
//
inline
VideoPlaybackViewFileDetails* VideoPlaybackControlsController::fileDetails()
{
    return mFileDetails;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::isTvOutConnected
// -------------------------------------------------------------------------------------------------
//
inline
bool VideoPlaybackControlsController::isTvOutConnected()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::isTvOutConnected(%d)"),
            mFileDetails->mTvOutConnected);

    return mFileDetails->mTvOutConnected;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
inline
VideoPlaybackDocumentLoader* VideoPlaybackControlsController::layoutLoader()
{
    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::view
// -------------------------------------------------------------------------------------------------
//
inline
VideoBasePlaybackView* VideoPlaybackControlsController::view()
{
    return mView;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
inline
TPlaybackViewMode VideoPlaybackControlsController::viewMode()
{
    return mViewMode;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::isAttachOperation
// -------------------------------------------------------------------------------------------------
//
inline
bool VideoPlaybackControlsController::isAttachOperation()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::isAttachOperation() ret %d"),
        mIsAttachOperation );

    return mIsAttachOperation;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
inline
bool VideoPlaybackControlsController::isFileDetailsAdded()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::isFileDetailsAdded() ret %d"),
        mFileDetailsAdded );

    return mFileDetailsAdded;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::isService
// -------------------------------------------------------------------------------------------------
//
inline
bool VideoPlaybackControlsController::isService()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::isService()"));

    return XQServiceUtil::isService();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::volumeSteps
// -------------------------------------------------------------------------------------------------
//
inline
int VideoPlaybackControlsController::volumeSteps()
{
    int volumeSteps = mViewWrapper->VolumeSteps();
    MPX_DEBUG(_L("VideoPlaybackControlsController::volumeSteps() steps = %d"), volumeSteps);

    return volumeSteps;
}

#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_*/

// End of File
