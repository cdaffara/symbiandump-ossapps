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

// Version : %version: ou1cpsw#23 %

#ifndef VIDEOPLAYERENGINE_H
#define VIDEOPLAYERENGINE_H

#include <qobject.h>
#include <mpxviewpluginqt.h>

#include <f32file.h>

#include "mpxhbvideocommondefs.h"
#include "videoplayerappexport.h"

//FORWARD CLASS DECLARATION

class MpxViewPlugin;
class VideoServices;
class VideoPlaybackWrapper;
class AfActivityStorage;
class VideoIadUpdateWrapper;

/**
 *  VideoPlayerEngine
 * 
 */
class VIDEOPLAYERAPP_DLL_EXPORT VideoPlayerEngine : public QObject
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
    
    public:
        void initialize();
        void playMedia( QString filePath );
		void playURI( QString uri );
        void playMedia( RFile file );
        void setEmbedded();
    signals:
        void applicationReady();
    
    public slots:
        void handleCommand( int commandCode );

    private slots:
        void handleQuit();
        void serviceQuit();

    private:
        void activateView( MpxHbVideoCommon::MpxHbVideoViewType viewType );

        void connectView();
        void disconnectView();
    
        void switchView();    
    
        void loadPluginAndCreateView( MpxHbVideoCommon::MpxHbVideoViewType viewType );
        
        void doDelayedLoad();
        
        void createMissingViews();
        
        void createPlaybackView();
        
        void setCurrentView();
        
        bool isPlayServiceInvoked();  
        
        bool shouldExit();
        
        bool shouldActivateCollectionView();
        
        void handlePlaybackFailure(int errorCode);
        
        void checkForUpdates();

    private:
        bool                      mIsService;
        bool                      mEmbedded;
        bool                      mDelayedLoadDone;
        bool                      mIsPlayService;

        MpxViewPlugin*            mCurrentViewPlugin;
        MpxViewPlugin*            mPlaybackViewPlugin; 
        MpxViewPlugin*            mCollectionViewPlugin; 
        MpxViewPlugin*            mFileDetailsViewPlugin;
        

        VideoPlaybackWrapper      *mPlaybackWrapper;

        VideoServices*            mVideoServices;
        
        AfActivityStorage*        mActivityStorage;

        VideoIadUpdateWrapper*    mIadUpdateWrapper;
};

#endif // VIDEOPLAYERENGINE_H

// EOF
