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
* Description:   imagereader header file
*
*/


#ifndef GLXIMAGEREADER_H_
#define GLXIMAGEREADER_H_

#include <imageconversion.h>
#include "glximageviewermanager.h"
class CFbsBitmap;

/**
 *  CGlxImageReader
 *
 *  Image Attributes Reader
 *
 *  @lib glxmedialists.lib
 */
class CGlxImageReader : public CBase
    {

private:
    static CGlxImageReader* NewLC();
    CGlxImageReader();
    void ConstructL();
    void GetFileTypeL(TDataType aMimeType);

public:
    static CGlxImageReader* NewL();
    ~CGlxImageReader();
    TInt GetDRMRightsL(TInt aAttribute);
    TInt GetFrameCount();
    TSize GetDimensions();

private:
    CImageDecoder*                      iImageDecoder;
    CGlxImageViewerManager*             iImgViewerMgr;
    };

#endif /* GLXIMAGEREADER_H_ */
