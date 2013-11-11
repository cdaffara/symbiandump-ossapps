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
* Description:  Data types passed through Publish and Subscribe
*
*/


#ifndef TELINFORMATIONPSTYPES_H
#define TELINFORMATIONPSTYPES_H

// INCLUDES
#include <cntdef.h>
#include <phclttypes.h>

// CONSTANTS

// DATA TYPES

/**
* Information to be shown in Phone application title pane.
* Note: Size of the data must not exceed RProperty::KMaxPropertySize.
*/
struct TTelTitleDisplay
    {
    /**
    * Maximum length for display information.
    */
    enum { ETagMaxLength = 128 };
    
    /**
    * Display information buffer.
    */
    typedef TBuf<ETagMaxLength> TDisplayTag;
    
    // Operator information, off-line etc.
    TDisplayTag iDisplayTag;
    
    // Handle to operator logo, or 0 if not valid.
    TInt iLogoHandle;

    // Handle to operator logo mask bitmap, or 0 if not valid.
    TInt iLogoMaskHandle;

    // Handle to background image
    TInt iBackgroundImageHandle;

    // Handle to background mask image
    TInt iBackgroundImageMaskHandle;
    
    // Background redraw counter
    TUint iBackgroundImageRedrawCounter;

    typedef TPckg<TTelTitleDisplay> TDisplayInfoPckg;
    };


#endif    // TELINFORMATIONPSTYPES_H

// End of file