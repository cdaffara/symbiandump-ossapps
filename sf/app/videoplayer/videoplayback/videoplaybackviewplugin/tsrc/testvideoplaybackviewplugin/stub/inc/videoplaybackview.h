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
* Description:  Video playback view
*
*/

// Version : %version:  3 %



// This file defines the API for .dll

#ifndef __VIDEOPLAYBACKVIEW_H__
#define __VIDEOPLAYBACKVIEW_H__

//  Include Files
#include <hbview.h>


//  Class Definitions

class VideoPlaybackView : public HbView
{
    Q_OBJECT

    public:
        VideoPlaybackView();
        virtual ~VideoPlaybackView();

        void handleActivateView(); 
        
        void handleDeactivateView();
                
        void handleBack();
        
        void handleSoftkeyBack();
        
        void handlePluginError( int aError );
        
        void handlePdlStateChange( int aState );
        
        void handleStoppedState();
				
};

#endif  // __VIDEOPLAYBACKVIEW_H__

// EOF
