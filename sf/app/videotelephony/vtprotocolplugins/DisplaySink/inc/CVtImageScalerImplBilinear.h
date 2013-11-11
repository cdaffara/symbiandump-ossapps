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





#ifndef CVTIMAGESCALERIMPLBILINEAR_H
#define CVTIMAGESCALERIMPLBILINEAR_H

// INCLUDE FILES

#include "CVtImageScalerImpl.h"

// CLASS DECLARATIONS

/**
*  Bilinear image scaler implementation.
*
*  @lib videosource.lib
*/
class CVtImageScalerImplBilinear : public CVtImageScalerImpl
    {
    private: // from CVtImageScalerImpl

        /**
        * From CVtImageScalerImpl. Check CVtImageScalerImpl for description.
        */
        TInt Scale( TBool& aContinue );

    private:

        /**
        * Validate given parameters.
        */
        void ValidateSourceTargetL(
            const CVtImage& aSource,
            CVtImage& aTarget );

        /**
        * Initializes internal variables for scaling.
        */
        void Initialize();

        /**
        * Scales images in displaymode EColor4K.
        */
        void Scale4K();

        /**
        * Scales images in displaymode EColor64K.
        */
        void Scale64K();

        /**
        * Scales images in displaymode EColor16M.
        */
        void Scale16M();

        /**
        * Scales images in displaymode EColor16MU.
        */
        void Scale16MU();

        /**
        * Scales images in displaymode EColor16MA.
        */
        void Scale16MA();

        /**
        * Fast scaling when target is double the source size for
        * 4K and 64K display modes.
        */
        void Scale2x4K64K( TUint32 aMask );

        /**
        * Fast scaling when target is double the source size for
        * 16M display mode.
        */
        void Scale2x16M();

        /**
        * Fast scaling when target is double the source size for
        * 16MU and 16MA display modes.
        */
        void Scale2x16MU16MA();

    private:

        // Target width ratio to source width
        TUint32 iU;

        // Target height ratio to source height
        TUint32 iV;
    };

#endif // CVTIMAGESCALERIMPLBILINEAR_H

// End of File


