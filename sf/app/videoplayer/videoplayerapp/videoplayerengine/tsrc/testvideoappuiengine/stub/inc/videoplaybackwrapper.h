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
* Description:  Stub implementation of VideoPlaybackWrapper
*
*/

// Version : %version: 2 %



#ifndef VIDEOPLAYBACKWRAPPER_H_
#define VIDEOPLAYBACKWRAPPER_H_

#include <qobject>
#include <qstring>
#include <qfile>

#include <mpxhbvideocommondefs.h>

class VideoPlaybackWrapper : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackWrapper();
        virtual ~VideoPlaybackWrapper();
        int playMedia( QString aFileName );

    public:
        void openPlaybackView() ;
        static int GetInstanceCount();
        void lateInit();

    signals:
        void handlePlaybackView( int viewId );

    private:
        static void Increment();
        static void Decrement();
};

#endif /* VIDEOPLAYBACKWRAPPER_H_ */
