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
* Description:  Implementation of VideoPlayerApp
*
*/

// Version : %version: 1 %

#ifndef VIDEOPLAYERAPP_H
#define VIDEOPLAYERAPP_H

#include <hbapplication.h>

/**
 *  VideoPlayerApp
 * 
 */
class VideoPlayerApp : public HbApplication
{
    Q_OBJECT

    public:
        /**
         * Constructor
         */
        VideoPlayerApp(int &argc,
                       char *argv[],
                       Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags);

        /**
        * Destructor.
        */
        ~VideoPlayerApp();
    
    signals:
        
        /**
         * Emitted when application view has been drawn.
         */
        void applicationReady();
    
    public slots:
        
        /**
         * Should be connected to HbMainWindow's viewReady signal, so that
         * applicationReady signal is emitted at correct time.
         */
        void viewReadySlot();
};

#endif // VIDEOPLAYERAPP_H

// EOF
