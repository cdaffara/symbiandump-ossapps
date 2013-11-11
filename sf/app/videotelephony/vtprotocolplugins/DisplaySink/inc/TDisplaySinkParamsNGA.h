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
* Description:  Displaysink initialization parameters for DisplaySink's NGA implementation.
*
*/

#ifndef TDISPLAYSINKPARAMSNGA_H
#define TDISPLAYSINKPARAMSNGA_H

// INCLUDE FILES

#include "mdisplaysink.h"
#include <pixelformats.h>
//#include <videorenderer.h>
#include <videoframebuffer.h>

// CLASS DECLARATION

/**
*  Displaysink initialization parameters for DisplaySink's NGA
*  implementation.
*
*  @lib displaysink.lib
*  @since Series 60 7.0
*/
NONSHARABLE_CLASS( TDisplaySinkParamsNGA ) : public TDisplaySinkParams
    {
public:
    // Pointer's pointer to first posting buffer  that is filled by the
    // sink.
    TUint8**  iSurfaceBuffer1;    
    //TVideoFrameBuffer** iSurfaceBuffer1;

    // Pointer's pointer to second posting buffer  that is filled by the
    // sink.
    //TVideoFrameBuffer** iSurfaceBuffer2;
    TUint8**  iSurfaceBuffer2;

    // Posting format to be used
    TUidPixelFormat iSurfaceFormat;

    // Frame size in pixels
    TSize iFrameSize;
    };

#endif // TDISPLAYSINKPARAMSNGA_H

// End of File
