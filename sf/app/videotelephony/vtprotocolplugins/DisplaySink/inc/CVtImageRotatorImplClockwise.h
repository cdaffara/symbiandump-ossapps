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




#ifndef CVTIMAGEROTATORIMPLCLOCKWISE_H
#define CVTIMAGEROTATORIMPLCLOCKWISE_H

// INCLUDE FILES

#include "CVtImageRotatorImpl.h"

// FORWARD DECLARATIONS

class CVtImage;

// CLASS DECLARATIONS

/**
*  Image scaler implementation.
*
*  @lib videosource.lib
*/
class CVtImageRotatorImplClockwise : public CVtImageRotatorImpl
    {
    public:
        /**
        * C++ constructor
        * @param "aAngle" Selected rotation angle.
        */
        CVtImageRotatorImplClockwise( const CVtImageRotator::TRotationAngle& aAngle );
        
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
        * The rotation implementation.
        */
        void DoRotate();
    
        /**
        * The rotation implementation for YUV planes.
        * @param "aSource" Constant pointer to source plane.
        * @param "aTarget" Pointer to target plane.
        * @param "aWidth" Width of the plane in pixels.
        * @param "aHeight" Height of the plane in pixels.
        * @param "aBytesPerRow" Bytes per row in plane.
        */
        void RotatePlane( 
            const TUint8* aSource, 
            TUint8* aTarget, 
            TInt aSourceWidth, 
            TInt aSourceHeight, 
            TInt aSourceBytesPerRow,
            TInt aTargetHeight,
            TInt aTargetBytesPerRow );
    };

#endif // CVTIMAGEROTATORIMPLCLOCKWISE_H
            
// End of File


