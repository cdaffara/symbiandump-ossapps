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
* Description:  Implementation of VideoPlaybackDetailsPlaybackWindow
*
*/

// Version : %version:   2 %



#ifndef VIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_
#define VIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_

#include <qobject>

class VideoPlaybackControlsController;

class VideoPlaybackDetailsPlaybackWindow : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackDetailsPlaybackWindow( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackDetailsPlaybackWindow();

    private:
        VideoPlaybackControlsController *mController;
};

#endif /*VIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_*/

