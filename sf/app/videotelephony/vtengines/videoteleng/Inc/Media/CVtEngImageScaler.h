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
* Description:  Scaler for bitmaps.
*
*/



#ifndef CVTENGIMAGESCALER_H
#define CVTENGIMAGESCALER_H

//  INCLUDES
#include "MVtEngImageScaler.h"
#include <cvtimagescaler.h>

// FORWARD DECLARATIONS
class MVtEngScalerObserver;
class CVtImageScaler;
class CVtImageBitmap;

/**
* Image scaler implementation class.
*
*  @lib videoteleng.lib
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CVtEngImageScaler ) : public CBase,
    public MVtImageScalerObserver,
    public MVtEngImageScaler
    {
    public: // constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver scaling finished callback.
        * @return scaler
        */
        static MVtEngImageScaler* NewL(
            MVtEngScalerObserver& aObserver );

        /**
        * Destructor
        */
        virtual ~CVtEngImageScaler();

    private: // constructor
        
        /**
        * C++ constructor.
        * @param aObserver scaling finished callback.
        */
        CVtEngImageScaler(
            MVtEngScalerObserver& aObserver );


        /**
        * Symbian constructor that may leave.
        */
        void ConstructL();

    public: // From MVtEngImageScaler.

        /**
        * @see MVtEngImageScaler.
        */
        virtual void ScaleL(
            TInt aSourceIndex );

        /**
        * @see MVtEngImageScaler
        */
        virtual TBool ConfigureL(
            TDisplayMode aDisplayMode,
            TSize aSize, 
            TInt aSourceHandle1,
            TInt aSourceHandle2 );

        /**
        * @see MVtEngImageScaler.
        */
        virtual void Release();

        /**
        * @see MVtEngImageScaler.
        */
        virtual TInt Cancel();

    public: // From MVtImageScalerObserver.

        /**
        * @ see MVtImageScalerObserver.
        */
        virtual void ScalingFinished( TInt aError );

    private:
        /**
        * EFirstBitmap 
        *  Use the first bitmap.
        * ESecondBitmap 
        *  Use the second bitmap.
        */
        enum 
	        {
            EFirstBitmap  = 0,
            ESecondBitmap = 1
            };

        /**
        * Fetches the used scaling method from locally.
        * variated properties, and stores it into iScalingMethod.
        */
        void GetScalingMethodL();

    private:

        // Observer receiving scaled bitmaps.
        MVtEngScalerObserver& iObserver;

        // Actual component doing the scaling.
        CVtImageScaler* iVtScaler;

        // Target bitmaps.
        RPointerArray<CVtImageBitmap> iTargets;

        // Source bitmaps.
        RPointerArray<CVtImageBitmap> iSources;

        // Bitmap index in targets.
        TInt                iTargetBitmapInUse;

        // Display mode.
        TDisplayMode        iMode;

        // Scaled size.
        TSize               iSize;

        // Method of scaling
        CVtImageScaler::TQuality    iScalingMethod;
    };

#endif  // CVTENGIMAGESCALER_H

// End of File
