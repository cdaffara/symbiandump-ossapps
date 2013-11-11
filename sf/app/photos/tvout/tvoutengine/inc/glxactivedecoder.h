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
* Description:    This is the decoder class used to decode Images to HDMI quality.
*
*/


#ifndef GLXACTIVEDECODER_H_
#define GLXACTIVEDECODER_H_

// INCLUDES
#include <aknapp.h>
#include <imageconversion.h>
#include "glxhdmisurfaceupdater.h"

// Fwd decleration
class CImageDecoder;

/**
 * Class Description
 * This is the decoder class used to decode Images to HDMI quality.
 * 
 * Clients should create an instance of this class from CGlxHdmiSurfaceUpdater , with an 
 * instance of it and call ConvertImageL() with approprriate parameters
 */        

class CGlxHdmiDecoderAO : public CActive
    {
public:
    
    /**
     * NewL()
     * @param CGlxHdmiSurfaceUpdater
     * @return CGlxHdmiDecoderAO object
     */
    static CGlxHdmiDecoderAO* NewL(CGlxHdmiSurfaceUpdater* aHdmiSurfaceUpdater);
    
    /**
     * Destructor
     */
    ~CGlxHdmiDecoderAO();
    
    /**
     * ConvertImageL() 
     * This calls the asyncronous service request to ICL convert
     * @param - Destination Bitmap
     * @param - Image decoder 
     */
    void ConvertImageL(CFbsBitmap& iBitmap,CImageDecoder* aDecoder);
    
protected:/// from CActive
    void RunL();
    void DoCancel();
    
private:
    /**
     * Constructor
     */
    CGlxHdmiDecoderAO(CGlxHdmiSurfaceUpdater* aHdmiSurfaceUpdater);

private:
    /// not owned
    CGlxHdmiSurfaceUpdater* iHdmiSurfaceUpdater; 
    /// Image decoder instance
    CImageDecoder* iDecoder;
    };

#endif /* GLXACTIVEDECODER_H_ */
