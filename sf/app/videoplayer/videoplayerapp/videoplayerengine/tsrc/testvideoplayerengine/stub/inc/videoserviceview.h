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
* Description:  Declaration of VideoServiceView
*
*/

// Version : %version: 2 %

#ifndef VIDEOSERVICEVIEW_H
#define VIDEOSERVICEVIEW_H

#include <QObject>

#include "videoplayerengine.h"

// FORWARD DECLARATIONS
class VideoServices;

class VideoServiceView : public QObject
{

    Q_OBJECT

    public:
        VideoServiceView( VideoServices* parent, VideoPlayerEngine* engine );
        virtual ~VideoServiceView();

    public slots:
        void view( QString filePath );

    private:
        VideoPlayerEngine*   mEngine;
        VideoServices*       mServiceApp;
};

#endif
