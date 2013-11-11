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

// Version : %version:  4 %



#ifndef VIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define VIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>
#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackviewfiledetails.h"

// FORWARD DECLARATIONS


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
class VideoBasePlaybackView;

class VideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        VideoPlaybackControlsController( bool attachOperation );

        /**
        * Destructor.
        */
        virtual ~VideoPlaybackControlsController();

    public:

        /**
        * Command handling function.
        * Call HandleCommandL() of container
        */
        void handleCommand( TMPXVideoPlaybackViewCommandIds command, int value = 0 );

        /**
        * Reset or cancel timers for the controls
        */
        void resetDisappearingTimers( TTimerAction timerAction );

        /**
        * Return state
        */
        inline TMPXPlaybackState state();

        inline VideoBasePlaybackView* view();

        /**
        * Return file details
        */
        inline VideoPlaybackViewFileDetails* fileDetails();

        inline TPlaybackViewMode viewMode();

        void evaluateAndChangeViewMode( TPlaybackViewMode viewMode, bool transitionEffect = true );

        bool isAttachOperation();

    private slots:
        void attachVideo();
        void sendVideo();

    public:
        VideoPlaybackViewFileDetails   *mFileDetails;
        VideoBasePlaybackView          *mView;

        TMPXPlaybackState               mState;
        TPlaybackViewMode               mViewMode;
        TTimerAction                    mTimerAction;
        TMPXVideoPlaybackViewCommandIds mCommand;
        int                             mCommandValue;
        bool                            mIsAttachOperation;
        bool                            mAttachVideoDone;
        bool                            mSendVideoDone;
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
//   VideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
inline
TPlaybackViewMode VideoPlaybackControlsController::viewMode()
{
    return mViewMode;
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

#endif /*VIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
