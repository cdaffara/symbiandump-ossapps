/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for video frames.
*
*/



#ifndef MVTENGFRAMEOBSERVER_H
#define MVTENGFRAMEOBSERVER_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
*  Observer interface for video frames.
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngFrameObserver
    {
    public:

        enum TFrameType
            {
            ELocalVideoFrame,
            ERemoteVideoFrame
            };

        virtual void vtHandleFrameL( TFrameType aType,
            CFbsBitmap* aBitmap ) = 0;

        /**
        * Sets last drawn frame. This frame will be stored by UI
        * and will be drawn when DSA is temporarily disabled (e.g.
        * when a menu is opened)
        */
        virtual void vtSetFrame( TFrameType aType,
            CFbsBitmap* aBitmap ) = 0;

    };

#endif      // MVTENGFRAMEOBSERVER_H

// End of File
