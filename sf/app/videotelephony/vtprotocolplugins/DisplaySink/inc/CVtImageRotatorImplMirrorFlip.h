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




#ifndef CVTIMAGEROTATORIMPLMIRRORFLIP_H
#define CVTIMAGEROTATORIMPLMIRRORFLIP_H

// INCLUDE FILES

#include "CVtImageRotatorImpl.h"

// FORWARD DECLARATIONS

class CVtImage;
class CVtImageIYUV;

// CLASS DECLARATIONS

/**
*  Image scaler implementation.
*
*  @lib videosource.lib
*/
class CVtImageRotatorImplMirrorFlip : public CVtImageRotatorImpl
    {
    public:
        /**
        * C++ constructor
        * @param "aAngle" Selected rotation angle.
        */
        CVtImageRotatorImplMirrorFlip( const CVtImageRotator::TRotationAngle& aAngle );

        /**
        * Rotates the image.
        * @param "aContinue" When method returns, this parameter will contain
        * ETrue if new call is needed (i.e. scaling is not yet finished) or
        * EFalse if scaling is complete.
        * @return KErrNone or one of the system wide error codes.
        */
        TInt Rotate( TBool& aContinue );

        /**
        * Source and target image validation method.
        * @param "aSource" Constant reference to source image.
        * @param "aTarget" Constant reference to target image.
        * @exception If source or target is not valid, this method leaves
        * with KErrNotSupported.
        */
        void ValidateSourceTargetL( 
            const CVtImage& aSource, 
            CVtImage& aTarget );

        /**
        * Method for checking specific rotation angle support.
        * @param "aAngle" Rotation type that will be needed.
        * @return ETrue if aAngle is one of the supported rotation types
        * EFalse otherwise.
        */
        TBool SupportsRotationAngle( const CVtImageRotator::TRotationAngle& aAngle );

    private:

        /**
        * Mirrors image horizontally from iSource to iTarget.
        */
        void Mirror();
    
        /**
        * Mirrors image horizontally from aTarget to aTarget.
        */
        void Mirror( CVtImage& aTarget );

        /**
        * Mirrors IYUV image from iSource to iTarget.
        */
        void MirrorIYUV
            ( 
            const CVtImageIYUV& aSource, 
            CVtImageIYUV& aTarget 
            );

        /**
        * Mirrors one plane of IYUV image from iSource to iTarget.
        */
        void MirrorPlane
            ( 
            TUint8* aSource,
            TUint8* aTarget,
            TInt aWidth,
            TInt aHeight,
            TInt aBytesPerRow
            );

        /**
        * Flips image vertically from iSource to iTarget.
        */
        void Flip();
           
        /**
        * Flips image vertically from aTarget to aTarget.
        */
        void Flip( CVtImage& aTarget );

        /**
        * Flips IYUV image vertically from iSource to iTarget.
        */
        void FlipIYUV
            ( 
            const CVtImageIYUV& aSource, 
            CVtImageIYUV& aTarget 
            );

        /**
        * Flips one plane of IYUV.
        */
        void FlipPlane
            ( 
            TUint8* aSource, 
            TUint8* aTarget, 
            TInt aHeight, 
            TInt aBytesPerRow 
            );
    };

#endif // CVTIMAGEROTATORIMPLMIRRORFLIP_H
            
// End of File


