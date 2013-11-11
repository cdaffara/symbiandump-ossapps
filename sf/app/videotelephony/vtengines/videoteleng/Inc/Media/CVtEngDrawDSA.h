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



#ifndef CVTENGDRAWDSA_H
#define CVTENGDRAWDSA_H

//  INCLUDES

#include "CVtEngRemoteVideo.h"
#include "TVtEngRenderConfigDSA.h"

// FORWARDS

class TDisplaySinkParamsDSA;

/**
*  Direct Screen Access drawer
*
*  @lib videoteleng.lib
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( CVtEngDrawDSA ) : public CVtEngRemoteVideo::CVtEngDraw,
    public MVtEngScalerObserver
    {
    public: // constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver call back interface to Display Sink
        * @return instance of CVtEngDrawDSA
        */
        static CVtEngDrawDSA* NewL( MDisplaySinkObserver* aObserver );

        // Destructor
        ~CVtEngDrawDSA();

    private: // construction

        /**
        * C++ constructor as private.
        * @param aObserver callback interface for Display Sink
        */
        CVtEngDrawDSA( MDisplaySinkObserver* aObserver );

    private: // from CVtEngDraw

        /**
        * @see CVtEngDraw::BaseConstructL
        */
        void BaseConstructL();

        /**
        * @see CVtEngDraw::CreateSinkL
        */
        MDisplaySink* CreateSinkL();

        /**
        * @see CVtEngDraw::BaseRefreshL
        */
        void BaseRefreshL();

        /**
        * @see CVtEngDraw::BaseRefreshBitmapsL
        */
        void BaseRefreshBitmapsL();

        /**
        * @see CVtEngDraw::BaseDoCancel
        */
        void BaseDoCancel();

        /**
        * @see CVtEngDraw::BaseVerifyConfigL
        */
        void BaseVerifyConfigL();

        /**
        * @see CVtEngDraw::BaseRestartL
        */
        void BaseRestartL();

        /**
        * @see CVtEngDraw::BaseAbortNow
        */
        void BaseAbortNow();

        /**
        * @see CVtEngDraw::BaseStartDrawL
        */
        void BaseStartDrawL();

        /**
        * @see CVtEngDraw::BaseStopDraw
        */
        void BaseStopDraw();

        /**
        * @see CVtEngDraw::BaseSetConfigL
        */
        void BaseSetConfigL( const TVtEngRenderConfig& aConfig );

        /**
        * @see CVtEngDraw::BaseSetUIForegroundL
        */
        void BaseSetUIForegroundL( TBool aForeground );

        /**
        * @see CVtEngDraw::BaseVideoFrameSizeChangedL
        */
        void BaseVideoFrameSizeChangedL( const TSize& );

        /**
        * @see CVtEngDraw::BaseRequestLastFrame
        */        
        void BaseRequestLastFrame();
        
        /**
        * @see CVtEngDraw::Config
        */
        TVtEngRenderConfig& Config();

    private: // from CActive

        /**
        * Handles prepared bitmap from display sink.
        * @see CActive::RunL
        */
        void RunL();

     public: // from MVtEngScalerObserver

        /**
        * From MVtEngScalerObserver, this method is called when
        * scaling is finished.
        * @see MVtEngScalerObserver::ScalingCompleted
        */
        virtual void ScalingCompleted(
            CFbsBitmap* aBitmap, TInt aError );

    private: // New functions

        /**
        * Does the drawing.
        * @param aBitmap a bitmap to draw
        */
        void DrawBitmap( CFbsBitmap& aBitmap );

        /**
        * Fills TDisplaySinkParamsDSA struct
        */
        void GetSinkParams( TDisplaySinkParamsDSA& aSinkParams );

    private: // data

        // Bitmapts for remote video (double buffering).
        CFbsBitmap*          iBitmap1;
        CFbsBitmap*          iBitmap2;

        // Bitmap indexes indicating which.
        // bitmap was provided.
        MDisplaySink::TBitmapNo iBitmapNo;

        // If a bitmap has been set available.
        TBool                iBitmapSetAvail;

        // VT image scaler
        MVtEngImageScaler*   iImageScaler;

        // DSA configuration
        TVtEngRenderConfigDSA iConfig;
    };

#endif // CVTENGDRAWDSA_H