/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Image Transforms subsystem.
*
*/




#ifndef CVTIMAGESCALERIMPL_H
#define CVTIMAGESCALERIMPL_H

// INCLUDE FILES

#include <e32base.h>
#include <bitmaptransforms.h>

// FORWARD DECLARATIONS

class CVtImage;

// CLASS DECLARATIONS

/**
*  Image scaler implementation.
*
*  @lib videosource.lib
*/
class CVtImageScalerImpl : public CActive
    {
    protected:

        /**
         * Constructor.
         */
        CVtImageScalerImpl();

    public:

        /**
         * Destructor.
         */
        ~CVtImageScalerImpl();

        /**
        * Sets scaler params.
        * @param "aParams" Scaling params, including source and target images.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void SetSourceTargetL( const CVtImage& aSource, CVtImage& aTarget );

        /**
        * Method to check whether bitmap heap locking is needed or not during
        * scaling process.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return ETrue if bitmap heap lock is needed, EFalse otherwise.
        */
        TBool NeedHeapLock() const;

        /**
        * Pure virtual scaling method.
        * @param "aContinue" When method returns, this parameter will contain
        * ETrue if new call is needed (i.e. scaling is not yet finished) or
        * EFalse if scaling is complete.
        * @return KErrNone or one of the system wide error codes.
        */
        virtual TInt Scale( TBool& aContinue ) = 0;

    protected:

        /**
         * Scales using CBitmapScaler, which is slow but works with any
         * Symbian color mode.
         * @param aQuality Scaling quality @see CBitmapScaler::TQualityAlgorithm
         * @exception In error situation leaves with one of the system wide
         * error codes.
         */
        void ScaleWithBitmapScalerL(
            const CBitmapScaler::TQualityAlgorithm aQuality );

    private:

        /**
        * Pure virtual source and target image validation method.
        * @param "aSource" Constant reference to source image.
        * @param "aTarget" Constant reference to target image.
        * @exception If source or target is not valid, this method leaves
        * with KErrNotSupported.
        */
        virtual void ValidateSourceTargetL(
            const CVtImage& aSource,
            CVtImage& aTarget ) = 0;

        /**
        * Method for validating image size.
        * @param "aSize" Size to be validated.
        * @param "aMinSize" Minimum allowed size. If this is not given a default
        * size of TSize( 1, 1 ) is used.
        * @exception If size is not valid within given parameters, this
        * method leaves with KErrNotSupported.
        */
        void LeaveIfNotValidSizeL(
            const TSize& aSize,
            const TSize aMinSize = TSize( 1, 1 ) );

    private: // from CActive

        /**
         * @see CActive::RunL
         */
        void RunL();

        /**
         * @see CActive::DoCancel
         */
        void DoCancel();

    protected:

        // Source for scaling
        const CVtImage* iSource; // not owned

        // Target image (will hold scaled source)
        CVtImage* iTarget; // not owned

        // Bitmap scaler instance
        CBitmapScaler* iScaler;

        // Active scheduler waiter object
        CActiveSchedulerWait iActiveSchedulerWait;

    };

#endif // CVTIMAGESCALERIMPL_H

// End of File


