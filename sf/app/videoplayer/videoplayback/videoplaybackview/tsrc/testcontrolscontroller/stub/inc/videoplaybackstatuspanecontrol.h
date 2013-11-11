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
* Description:  Implementation of VideoPlaybackStatusPaneControl
*
*/

// Version : %version:  2 %



#ifndef VIDEOPLAYBACKSTATUSPANECONTROL_H_
#define VIDEOPLAYBACKSTATUSPANECONTROL_H_


#include "videoplaybackfullscreencontrol.h"


class VideoPlaybackControlsController;

class VideoPlaybackStatusPaneControl : public VideoPlaybackFullScreenControl
{
    Q_OBJECT

    public:
        VideoPlaybackStatusPaneControl( VideoPlaybackControlsController* controller, 
                                        TVideoPlaybackControls index,
                                        HbWidget* widget, 
                                        TUint controlproperties );

        virtual ~VideoPlaybackStatusPaneControl();

};

#endif /*VIDEOPLAYBACKSTATUSPANECONTROL_H_*/

