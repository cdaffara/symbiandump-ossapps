/*
* Copyright (c) 20101 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of VideoPlaybackControlConfiguration
*
*/

// Version : %version:  3 %



#ifndef VIDEOPLAYBACKCONTROLCONFIGURATION_H_
#define VIDEOPLAYBACKCONTROLCONFIGURATION_H_

// INCLUDES
#include <qobject>

#include <mpxplaybackframeworkdefs.h>
#include "videoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

// FORWARD DECLARATIONS
class VideoPlaybackControlsController;

// DATA TYPES

// CLASS DECLARATION
class VideoPlaybackControlConfiguration : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        VideoPlaybackControlConfiguration( VideoPlaybackControlsController* controller );

        /**
        * Destructor.
        */
        virtual ~VideoPlaybackControlConfiguration();

    public:
        /**
        * Update control list
        */
        void updateControlList( TVideoPlaybackControlCommandIds event );

        /**
        * Return control list
        */
        QList<TVideoPlaybackControls>& controlList();

        void updateControlsWithFileDetails();

        /**
        * Create control list
        */
        void createControlList();

    private:

        /**
        * Delete controls from list
        */
        void deleteControlFromList( TVideoPlaybackControls control );

        /**
        * Add controls to list
        */
        void addControlToList( TVideoPlaybackControls control );

    signals:
        void controlListUpdated();

    public:
        
        TInt mState;
        bool mUpdateControlsWithFileDetails;
        
    private:
        VideoPlaybackControlsController *mControlsController;
        QList<TVideoPlaybackControls>    mControlsList;
};

#endif /*VIDEOPLAYBACKCONTROLCONFIGURATION_H_*/

// End of File
