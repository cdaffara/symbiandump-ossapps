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
* Description:  Interface used for scaling images to certain size.
*
*/



#ifndef VTENGIMAGESCALER_H
#define VTENGIMAGESCALER_H

//  INCLUDES
#include <gdi.h>

// FORWARD DECLARATIONS
class CFbsBitmap;

/**
* Image scaler observer.
*
*  @lib videoteleng.lib
*  @since Series 60 2.8
*/
class MVtEngScalerObserver
    {
    public:

        /**
        * Called when scaling is ready.
        * @param aBitmap a scaled bitmap
        * @param aError Symbian OS error code
        */
        virtual void ScalingCompleted( 
            CFbsBitmap* aBitmap,
            TInt aError ) = 0;
    };

/**
* Image scaler.
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngImageScaler
    {
    public: // new functions

        /**
        * Used to start the scaling of an image.
        * @param aSourceIndex the index of the source to use.
        */
        virtual void ScaleL( 
            TInt aSourceIndex ) = 0;

        /**
        * Configures the scaler.
        * @param aDisplayMode which kind of display
        * @param aSize the width and height of the display
        * @return ETrue if previous scaling was cancelled
        */
        virtual TBool ConfigureL(
            TDisplayMode aDisplayMode,
            TSize aSize,
            TInt aSourceHandle1,
            TInt aSourceHandle2 ) = 0;

        /**
        * Deletes the scaling object.
        */
        virtual void Release() = 0;

        /**
        * Possibility to cancel the scaling.
        * @return error code of canceling.
        */
        virtual TInt Cancel() = 0;
    };

/**
* Image scaler factory.
*
*  @lib videoteleng.lib
*  @since Series 60 2.8
*/
class VtEngScalerFactory
    {
    public:
        /**
        * Factory method for creating scaler
        * @param aObserver a callback interface
        */
        static MVtEngImageScaler* CreateScalerL(
            MVtEngScalerObserver& aObserver );
    };

#endif  // VTENGIMAGESCALER_H

// End of File
