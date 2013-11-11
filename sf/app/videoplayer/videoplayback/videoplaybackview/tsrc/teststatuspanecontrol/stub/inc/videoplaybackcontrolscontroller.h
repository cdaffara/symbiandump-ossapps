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
* Description:  Implementation of VideoPlaybackControlsController
*
*/

// Version : %version:  6 %



#ifndef VIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define VIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>

#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "videoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackviewfiledetails.h"

// FORWARD DECLARATIONS
class VideoBasePlaybackView;
class VideoPlaybackDocumentLoader;
class VideoPlaybackFullScreenControl;
class VideoPlaybackControlsController;


// DATA TYPES

enum TTimerAction
{
    ETimerCancel,
    ETimerReset
};

enum TPlaybackViewMode
{
    EFullScreenView,
    EDetailsView,
    EAudioOnlyView
};


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
                VideoPlaybackViewFileDetails *details );

        /**
        * Destructor.
        */
        virtual ~VideoPlaybackControlsController();

    public:
        /**
        * Reset or cancel timers for the controls
        */
        void resetDisappearingTimers( TTimerAction timerAction );

        /*
        * Return state
        */
        inline TMPXPlaybackState state();

        /**
        * Return file details
        */
        inline VideoPlaybackViewFileDetails* fileDetails();

        VideoPlaybackDocumentLoader* layoutLoader();

        inline VideoBasePlaybackView* view();

        inline bool isFileDetailsAdded();

        void evaluateAndChangeViewMode( TPlaybackViewMode viewMode, bool transitionEffect = true );

        TPlaybackViewMode viewMode();

    public:
        /**
        * Initialize controller
        */
        void initializeController();

        /**
        * Create controls
        */
        void createControls();

        /**
        * Update controls
        */
        void handleStateChange( TMPXPlaybackState newState );

        /**
        * Update Control's visibility
        */
        void updateControlsVisibility();

    public:
        VideoBasePlaybackView               *mView;
        VideoPlaybackViewFileDetails          *mFileDetails;

        QList<VideoPlaybackFullScreenControl*> mControls;

        VideoPlaybackDocumentLoader           *mLoader;

        TMPXPlaybackState                      mState;
        TPlaybackViewMode                      mViewMode;
        TTimerAction                           mTimerAction;
        bool                                   mFileDetailsAdded;
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
//   VideoPlaybackControlsController::view
// -------------------------------------------------------------------------------------------------
//
inline
VideoBasePlaybackView* VideoPlaybackControlsController::view()
{
    return mView;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::view
// -------------------------------------------------------------------------------------------------
//
inline
bool VideoPlaybackControlsController::isFileDetailsAdded()
{
    return mFileDetailsAdded;
}

#endif /*VIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
