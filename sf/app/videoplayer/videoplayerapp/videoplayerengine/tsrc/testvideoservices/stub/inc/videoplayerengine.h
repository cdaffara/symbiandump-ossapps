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
* Description:  Implementation of VideoPlayerEngine
*
*/

// Version : %version: da1mmcf#4 %

#ifndef VIDEOPLAYERENGINE_H
#define VIDEOPLAYERENGINE_H

#include <qobject.h>
#include <f32file.h>

//FORWARD CLASS DECLARATION

/**
 *  VideoPlayerEngine
 *
 */
class VideoPlayerEngine: public QObject
{
    Q_OBJECT

    public:
	
        /**
         * Constructor
         */
        VideoPlayerEngine( bool isService = false );

        /**
        * Destructor.
        */
        virtual ~VideoPlayerEngine();

        void initialize();
        void playMedia( QString filePath );
        void playMedia( RFile file );
        void setEmbedded();
        void playURI( QString uri );

    public slots:
        void handleCommand( int commandCode );

    private slots:
        void handleQuit();

    private:
        bool                      mIsService;
        bool                      mEmbedded;
};

#endif // VIDEOPLAYERENGINE_H

// EOF
