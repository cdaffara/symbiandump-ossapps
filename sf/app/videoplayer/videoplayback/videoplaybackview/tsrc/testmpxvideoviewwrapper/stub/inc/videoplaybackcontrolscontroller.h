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

// Version : %version: 9 %



#ifndef VIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define VIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>

#include "mpxvideo_debug.h"
#include "videoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class CMPXVideoViewWrapper;
class VideoBasePlaybackView;
class VideoPlaybackControlsController;
class VideoPlaybackViewFileDetails;

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

        //
        //  Handle event from container
        //
        void handleEvent( TVideoPlaybackControlCommandIds event, int value = 0 );

        //
        //  Add the file details to the controls controller when available
        //
        void addFileDetails( VideoPlaybackViewFileDetails* details );

        //
        //  updateVideoRectDone
        //
        void updateVideoRectDone();

        bool isRNLogoBitmapInControlList();

    public:
        VideoBasePlaybackView        *mView;
        CMPXVideoViewWrapper           *mViewWrapper;
        VideoPlaybackViewFileDetails   *mFileDetails;
        TVideoPlaybackControlCommandIds mReceivedEvent;

        int   mValue;
        bool  mRNLogoVisible;
        bool  mFileDetailsAdded;
};

#endif /*VIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
