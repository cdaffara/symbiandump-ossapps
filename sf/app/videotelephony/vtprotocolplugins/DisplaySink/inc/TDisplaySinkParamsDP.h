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
* Description:  DisplaySink subsystem.
*
*/


#ifndef TDISPLAYSINKPARAMSDP_H
#define TDISPLAYSINKPARAMSDP_H

// INCLUDE FILES

#include "mdisplaysink.h"
#include <Posting_Surface.h>

// CLASS DECLARATION

/**
*  Displaysink initialization parameters for DisplaySink's Display Posting
*  implementation.
*
*  @lib displaysink.lib
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( TDisplaySinkParamsDP ) : public TDisplaySinkParams
    {
public:
    // Pointer's pointer to first posting buffer  that is filled by the
    // sink.
    CPostingSurface::TPostingBuff** iPostingBuffer1;

    // Pointer's pointer to second posting buffer  that is filled by the
    // sink.
    CPostingSurface::TPostingBuff** iPostingBuffer2;

    // Posting format to be used
    CPostingSurface::TPostingFormat iPostingFormat;

    // Frame size in pixels
    TSize iFrameSize;
    };

#endif // TDISPLAYSINKPARAMSDP_H

// End of File
