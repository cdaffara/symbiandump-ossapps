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
* Description:  Implementation of VideoPlaybackFileDetailsWidget
*
*/

// Version : %version:  2 %



#ifndef VIDEOPLAYBACKFILEDETAILSWIDGET_H_
#define VIDEOPLAYBACKFILEDETAILSWIDGET_H_

#include <qobject>

class VideoPlaybackControlsController;


class VideoPlaybackFileDetailsWidget : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackFileDetailsWidget( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackFileDetailsWidget();
        
    private:        
        VideoPlaybackControlsController *mController;
};

#endif /*VIDEOPLAYBACKFILEDETAILSWIDGET_H_*/

