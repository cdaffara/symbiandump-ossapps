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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version:  5 %



#ifndef MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>

#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackviewfiledetails.h"

// FORWARD DECLARATIONS
class VideoPlaybackDocumentLoader;
class VideoPlaybackControlsController;
class VideoPlaybackControlConfiguration;



// CLASS DECLARATION

class VideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        VideoPlaybackControlsController( VideoPlaybackViewFileDetails *details );

        /**
        * Destructor.
        */
        virtual ~VideoPlaybackControlsController();

    public:
        /**
        * Return file details
        */
        inline VideoPlaybackViewFileDetails* fileDetails();

        VideoPlaybackDocumentLoader* layoutLoader();

    public:
        /**
        * Initialize controller
        */
        void initializeController();

        bool shouldShowRNLogo();

    public slots:
        void controlsListUpdated();

    public:
        VideoPlaybackViewFileDetails          *mFileDetails;

        VideoPlaybackControlConfiguration     *mControlsConfig;

        VideoPlaybackDocumentLoader           *mLoader;
};

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::fileDetails
// -------------------------------------------------------------------------------------------------
//
inline
VideoPlaybackViewFileDetails* VideoPlaybackControlsController::fileDetails()
{
    return mFileDetails;
}

#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
