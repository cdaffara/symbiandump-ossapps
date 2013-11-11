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
* Description:  VideoPlaybackViewPlugin
*
*/

// Version : %version:  7 %


#ifndef VIDEOPLAYBACKVIEWPLUGIN_H
#define VIDEOPLAYBACKVIEWPLUGIN_H

#include <QObject>
#include <mpxviewpluginqt.h>


// FORWARD DECLARATION
class VideoPlaybackView;

/**
 *  MPX playback view plugin definition.
 *
 */
class VideoPlaybackViewPlugin : public MpxViewPlugin
{
    Q_OBJECT

    public:

        /**
         * C++ default constructor.
         */
        VideoPlaybackViewPlugin();

        /**
         * Destructor.
         *
         */
        virtual ~VideoPlaybackViewPlugin();

        // from QViewPlugin
        void createView();

        void destroyView();

        void activateView();

        void deactivateView();

        QGraphicsWidget* getView();
	
    signals:
        void command( int aCommand );    

    public slots:
        void orientationChange(Qt::Orientation orientation);
        void back();

    public:
	VideoPlaybackView* mView;
	bool               mViewActivated;
};

#endif //VIDEOPLAYBACKVIEWPLUGIN_H

// End of file

