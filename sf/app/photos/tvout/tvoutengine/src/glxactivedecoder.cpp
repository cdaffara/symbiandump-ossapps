/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    
*
*/

#include <glxtracer.h>
#include <glxlog.h>
#include "glxactivedecoder.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
CGlxHdmiDecoderAO* CGlxHdmiDecoderAO::NewL(CGlxHdmiSurfaceUpdater* aHdmiSurfaceUpdater)
    {
    TRACER("CGlxHdmiDecoderAO::NewL()");
    CGlxHdmiDecoderAO* self = new (ELeave) CGlxHdmiDecoderAO(aHdmiSurfaceUpdater);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CGlxHdmiDecoderAO()
// -----------------------------------------------------------------------------
CGlxHdmiDecoderAO::CGlxHdmiDecoderAO(CGlxHdmiSurfaceUpdater* aHdmiSurfaceUpdater):
    CActive(CActive::EPriorityStandard-1),iHdmiSurfaceUpdater(aHdmiSurfaceUpdater)
    {
    TRACER("CGlxHdmiDecoderAO::CGlxHdmiDecoderAO()");
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// ~CGlxHdmiDecoderAO
// Cancel the outstanding request
// -----------------------------------------------------------------------------
CGlxHdmiDecoderAO::~CGlxHdmiDecoderAO()
    {
    TRACER("CGlxHdmiDecoderAO::~CGlxHdmiDecoderAO()");  
    }
    
// -----------------------------------------------------------------------------
// RunL
// Calls iHdmiSurfaceUpdater->HandleRunL() which is expected to handle the call
// -----------------------------------------------------------------------------
void CGlxHdmiDecoderAO::RunL()
    {
    TRACER("CGlxHdmiDecoderAO::RunL()");
    if(iStatus == KErrUnderflow)
        {
        GLX_LOG_INFO("CGlxHdmiDecoderAO::RunL() - call continueconvert");
        iDecoder->ContinueConvert(&iStatus);
        }
    else
        {
        GLX_LOG_INFO("CGlxHdmiDecoderAO::RunL() - call handlerunL");
        iHdmiSurfaceUpdater->HandleRunL(iStatus);
        }
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
void CGlxHdmiDecoderAO::DoCancel()
    {
    TRACER("CGlxHdmiDecoderAO::DoCancel()");
    iDecoder->Cancel();
    }

// -----------------------------------------------------------------------------
// ConvertImageL
// -----------------------------------------------------------------------------
void CGlxHdmiDecoderAO::ConvertImageL(CFbsBitmap& aBitmap,
                                       CImageDecoder* aDecoder)
    {
    TRACER("CGlxHdmiDecoderAO::ConvertImageL()");
    iDecoder = aDecoder;
    iDecoder->Convert(&iStatus,aBitmap); 
    SetActive();
    }
    


