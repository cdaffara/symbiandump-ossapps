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
* Description:  Implementation of VideoPlaybackControlPolicy
*
*/

// Version : %version:  2 %



#ifndef VIDEOPLAYBACKCONTROLPOLICY_H_
#define VIDEOPLAYBACKCONTROLPOLICY_H_

// INCLUDES
#include <qobject>

#include "videoplaybackcontrol.hrh"
#include "videoplaybackcontrolscontroller.h"

// FORWARD DECLARATIONS
class VideoPlaybackViewFileDetails;

// DATA TYPES

// CLASS DECLARATION
class VideoPlaybackControlPolicy : public QObject
{
    Q_OBJECT

    public:

        /**
        * Two-phased constructor.
        */
        VideoPlaybackControlPolicy();

        /**
        * Destructor.
        */
        virtual ~VideoPlaybackControlPolicy();

    public:
        /**
        * Sets the control properties per policy
        */
        void setControlProperties( TVideoPlaybackControls controlIndex,
                                   TUint& properties,
                                   VideoPlaybackViewFileDetails *details,
                                   TPlaybackViewMode viewMode );
};

#endif /*MPXVIDEOPLAYBACKCONTROLPOLICY_H_*/

// End of File
