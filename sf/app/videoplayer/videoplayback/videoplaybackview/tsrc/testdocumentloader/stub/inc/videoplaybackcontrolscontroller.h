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

// Version : %version:  3 %



#ifndef VIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define VIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>
#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "mpxhelixplaybackplugindefs.h"
#include "mpxcommonvideoplaybackview.hrh"

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

const QString KPLAYBACKVIEW_DOCML = ":/videoplaybackview/videoplaybackview.docml";

// CLASS DECLARATION

class VideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        VideoPlaybackControlsController();

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

        TPlaybackViewMode viewMode();

        void changeViewMode( TPlaybackViewMode viewMode, bool transitionEffect = true );

    public:

        TMPXPlaybackState                          mState;
        TPlaybackViewMode                          mViewMode;
        TTimerAction                            mTimerAction;
        TMPXVideoPlaybackViewCommandIds            mCommand;
        int                                        mCommandValue;
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
//   VideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
inline
TPlaybackViewMode VideoPlaybackControlsController::viewMode()
{
    return mViewMode;
}

#endif /*VIDEOPLAYBACKCONTROLSCONTROLLER_H_*/

// End of File
