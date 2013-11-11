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
 * Description:    CGlxImageViewerManager implementation
 *
 */


#include "glximageviewermanager.h"
#include <glxsingletonstore.h>
#include <glxtracer.h>

#include <f32file.h>

EXPORT_C CGlxImageViewerManager* CGlxImageViewerManager::InstanceL()
    {
    TRACER("CGlxImageViewerManager::InstanceL()");
    return CGlxSingletonStore::InstanceL(&NewL);
    }

EXPORT_C void CGlxImageViewerManager::Close()
    {
    TRACER("CGlxImageViewerManager::DeleteInstance()");
    CGlxSingletonStore::Close(this);
    }

EXPORT_C HBufC* CGlxImageViewerManager::ImageUri()
    {
    TRACER("CGlxImageViewerManager::ImageUri()");    
    return iImageUri;
    }

EXPORT_C RFile64& CGlxImageViewerManager::ImageFileHandle()
    {
    TRACER("CGlxImageViewerManager::ImageFileHandle()");    
    return *iFile;
    }

EXPORT_C TBool CGlxImageViewerManager::IsPrivate()
    {
    TRACER("CGlxImageViewerManager::IsPrivate()");    
    return iIsPrivate;
    }

CGlxImageViewerManager::CGlxImageViewerManager()
    : iImageUri(NULL), iFile(NULL), iIsPrivate(EFalse)
    {
    TRACER("CGlxImageViewerManager::CGlxImageViewerManager()");
    // No implementation required
    }

CGlxImageViewerManager::~CGlxImageViewerManager()
    {
    TRACER("CGlxImageViewerManager::~CGlxImageViewerManager()");
    Reset();
    }

CGlxImageViewerManager* CGlxImageViewerManager::NewLC()
    {
    TRACER("CGlxImageViewerManager::NewLC()");
    CGlxImageViewerManager* self = new (ELeave) CGlxImageViewerManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CGlxImageViewerManager* CGlxImageViewerManager::NewL()
    {
    TRACER("CGlxImageViewerManager::NewL()");
    CGlxImageViewerManager* self = CGlxImageViewerManager::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

void CGlxImageViewerManager::ConstructL()
    {
    TRACER("CGlxImageViewerManager::ConstructL()");
    }

// ---------------------------------------------------------------------------
// SetImageUri
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxImageViewerManager::SetImageUriL(const TDesC& aFileName)
    {
    TRACER("void CGlxImageViewerManager::SetImageUriL()");    
    if ( iImageUri )
        {
        delete iImageUri;
        iImageUri = NULL;
        }
    if (aFileName.Length() == 0)
        {
        User::Leave(KErrNotSupported);    
        }
   	
    iImageUri = aFileName.AllocL();    
    }

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxImageViewerManager::Reset()
    {
    TRACER("void CGlxImageViewerManager::Reset()");
    if( iFile )
        {
        iFile->Close();
        }
    delete iFile;
    iFile = NULL;

    if ( iImageUri )
        {
        delete iImageUri;
        iImageUri = NULL;
        }
    iIsPrivate = EFalse;
    }

// ---------------------------------------------------------------------------
// SetImageFileHandleL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxImageViewerManager::SetImageFileHandleL(const RFile& aFileHandle)
    {
    TRACER("void CGlxImageViewerManager::SetImageFileHandleL()");
    _LIT( KPrivateFolder, "\\Private\\" );    // Platsec private folder  
    TFileName filePath;
    User::LeaveIfError( aFileHandle.FullName( filePath ) );
    SetImageUriL( filePath );
    TParsePtrC parse( filePath );
    if( parse.PathPresent() &&
        parse.Path().Length() > KPrivateFolder().Length() &&
        parse.Path().Left( KPrivateFolder().Length() ).CompareF( KPrivateFolder ) == 0 )
        {
        // File is in private folder; duplicate file handle
        iFile = new (ELeave) RFile64;
        User::LeaveIfError( iFile->Duplicate( aFileHandle ) );
        iIsPrivate = ETrue;
        }
    }
