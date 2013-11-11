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
* Description:  Image Transforms subsystem.
*
*/


// INCLUDE FILES

#include "cvtimage.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtImage::Type() const
// -----------------------------------------------------------------------------
//
EXPORT_C CVtImage::TVtImageType CVtImage::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CVtImage::DisplayModeToVtDisplayMode( TDisplayMode aMode )
// -----------------------------------------------------------------------------
//
CVtImage::TVtDisplayMode CVtImage::DisplayModeToVtDisplayMode(
    TDisplayMode aMode )
    {
    TVtDisplayMode mode;

    switch( aMode )
        {
        case EColor4K:
            mode = EVtColor4K;
            break;

        case EColor64K:
            mode = EVtColor64K;
            break;

        case EColor16M:
            mode = EVtColor16M;
            break;

        case EColor16MU:
            mode = EVtColor16MU;
            break;

        case EColor16MA:
        case EColor16MAP:
            mode = EVtColor16MA;
            break;

        default:
            mode = EVtColorNone;
            break;
        }

    return mode;
    }

// -----------------------------------------------------------------------------
// CVtImage::CVtImage( TVtImageType aType )
// -----------------------------------------------------------------------------
//
CVtImage::CVtImage( TVtImageType aType )
: iType( aType )
    {
    }

// End of File


