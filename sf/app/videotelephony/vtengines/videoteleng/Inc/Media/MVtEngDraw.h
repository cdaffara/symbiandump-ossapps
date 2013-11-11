/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote video handling class.
*
*/



#ifndef MVTENGDRAW_H
#define MVTENGDRAW_H

// INCLUDE

#include <e32base.h>

// FORWARD

class MDisplaySink;
class MDisplaySinkObserver;
class TVtEngRenderConfig;

/**
*  Drawer API.
*
*  @lib videoteleng.lib
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( MVtEngDraw )
    {
    public:

       /**
        * Base construction for derived classes.
        */
        virtual void BaseConstructL() = 0;

        /**
        * Instantiates display sink.
        */
        virtual MDisplaySink* CreateSinkL() = 0;

        /**
        * Refresh view.
        */
        virtual void BaseRefreshL() = 0;

        /**
        * Updates the target bitmaps and/or displaymode
        * and/or the sizes of bitmaps
        */
        virtual void BaseRefreshBitmapsL() = 0;

        /**
        * DoCancel method for inherited classes.
        */
        virtual void BaseDoCancel() = 0;

        /**
        * Configuration verifying routine for derived classes. If
        * new configuration is not supported, then may this method
        * leaves with appropriate error code.
        */
        virtual void BaseVerifyConfigL() = 0;

        /**
        * Called when DSA calls Restart() to start drawing again.
        */
        virtual void BaseRestartL() = 0;

        /**
        * Called when DSA calls AbortNow() to stop drawing.
        */
        virtual void BaseAbortNow() = 0;

        /**
        * Starts rendering.
        */
        virtual void BaseStartDrawL() = 0;

        /**
        * Stops rendering.
        */
        virtual void BaseStopDraw() = 0;

        /**
        * Sets configuration.
        */
        virtual void BaseSetConfigL( const TVtEngRenderConfig& aConfig ) = 0;

        /**
        * Called when UI foreground status changes.
        */
        virtual void BaseSetUIForegroundL( TBool aForeground ) = 0;

        /**
        * Called when incoming video frame size changes.
        */
        virtual void BaseVideoFrameSizeChangedL( const TSize& aTo ) = 0;
        
        /**
        * Request update last remote video frame through MVtEngFrameObserver::vtSetFrame.
        */
        virtual void BaseRequestLastFrame() = 0;        

    protected:

        /**
        * Returns current configuration settings.
        */
        virtual TVtEngRenderConfig& Config() = 0;

    };

/**
* Remote drawer factory.
*
*  @lib videoteleng.lib
*  @since Series 60 3.2
*/
class VtEngDrawFactory
    {
    public:
        /**
        * Factory method for creating drawer
        * @param aObserver a callback interface
        */
        static MVtEngDraw* CreateDrawerL( MDisplaySinkObserver* aObserver, TBool );
    };

#endif // MVTENGDRAW_H