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




#ifndef CVTIMAGEROTATORIMPL_H
#define CVTIMAGEROTATORIMPL_H

// INCLUDE FILES

#include "cvtimagerotator.h"

// FORWARD DECLARATIONS

class CVtImage;

// CLASS DECLARATIONS

/**
*  Image scaler implementation.
*
*  @lib videosource.lib
*/
class CVtImageRotatorImpl : public CBase
    {
    public:

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
        * Pure virtual rotation method.
        * @param "aContinue" When method returns, this parameter will contain
        * ETrue if new call is needed (i.e. scaling is not yet finished) or
        * EFalse if scaling is complete.
        * @return KErrNone or one of the system wide error codes.
        */
        virtual TInt Rotate( TBool& aContinue ) = 0;

        /**
        * Pure virtual method for checking specific rotation angle support.
        * @param "aAngle" Rotation type that will be needed.
        * @return ETrue if aAngle is one of the supported rotation types
        * EFalse otherwise.
        */
        virtual TBool SupportsRotationAngle( const CVtImageRotator::TRotationAngle& aAngle ) = 0;

        /**
        * Sets rotation angle to be used.
        * @param "aAngle" Rotation type that will be needed.
        */
        void SetAngle( const CVtImageRotator::TRotationAngle& aAngle );

    protected:

        /**
        * C++ constructor.
        * @param "aAngle" Rotation type that will be needed.
        */
        CVtImageRotatorImpl( const CVtImageRotator::TRotationAngle& aAngle );

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

    protected:

        // Source for scaling
        const CVtImage* iSource; // not owned

        // Target image (will hold scaled source)
        CVtImage* iTarget; // not owned

        // Selected rotation angle
        CVtImageRotator::TRotationAngle iAngle;
    };

#endif // CVTIMAGEROTATORIMPL_H

// End of File


