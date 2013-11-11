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
* Description: Image Viewer Manager header file : CCGlxImageViewerManager declaration
*
*/


#ifndef GLXIMAGEVIEWERMANAGER_H
#define GLXIMAGEVIEWERMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class RFile;
class RFile64;

/**
 *  CGlxImageViewerManager
 * 
 */
class CGlxImageViewerManager : public CBase
    {
public: // Constructors and destructor

    /**
     * Destructor.
     */
    ~CGlxImageViewerManager();

    /**
     * Singleton constructor.
     */
    IMPORT_C static CGlxImageViewerManager* InstanceL();
    
public:
    IMPORT_C HBufC* ImageUri();
    IMPORT_C RFile64& ImageFileHandle();
    IMPORT_C TBool IsPrivate();
    IMPORT_C void Close( void );
    IMPORT_C void SetImageUriL(const TDesC& aFileName);
    IMPORT_C void SetImageFileHandleL(const RFile& aFileHandle);
    IMPORT_C void Reset();

private:

    /**
     * Two-phased constructor.
     */
    static CGlxImageViewerManager* NewL();

    /**
     * Two-phased constructor.
     */
    static CGlxImageViewerManager* NewLC();

    /**
     * Constructor for performing 1st stage construction
     */
    CGlxImageViewerManager();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:
    /// image file uri in case of Photos starting as Image viewer 
    HBufC* iImageUri;

    /// Handle to image file which will be shown in Image viewer
    RFile64* iFile;

    /// Flag that stores if the image file is in a private folder. 
    /// ETrue means the image is private, else EFalse 
    TBool iIsPrivate;
    };

#endif // GLXIMAGEVIEWERMANAGER_H
