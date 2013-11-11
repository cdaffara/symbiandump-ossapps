/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DisplaySink subsystem.
*
*/


#ifndef TDISPLAYSINKPARAMSDSA_H
#define TDISPLAYSINKPARAMSDSA_H

// INCLUDE FILES

#include "cvtimagerotator.h"
#include "mdisplaysink.h"

// CLASS DECLARATION

/**
*  Displaysink initialization parameters for DisplaySink's Direct Screen Access
*  implementation.
*
*  @lib displaysink.lib
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( TDisplaySinkParamsDSA ) : public TDisplaySinkParams
    {
public:

    /**
    * C++ constructor
    */
    TDisplaySinkParamsDSA();

public:
    // Handle to first bitmap that is filled by the sink. 176x144 bitmap (QCIF)
    TInt iBitmap1Handle;

    // Handle to second bitmap that is filled by the sink. 176x144 bitmap (QCIF)
    TInt iBitmap2Handle;

    // Angle to which YUV image must be rotated BEFORE it is converted into bitmap.
    // If CVtImageRotator::ERotationNone is given, image will be converted as it
    // is received. Also make sure that iBitmap1Handle and iBitmap2Handle contains
    // correct dimensions AFTER rotation.
    CVtImageRotator::TRotationAngle iRotationAngle;
    };

#include "tdisplaysinkparamsdsa.inl"

#endif // TDISPLAYSINKPARAMSDSA_H

// End of File
