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
* Description:  This class provide API to read resource from resource file
*
*/


// INCLUDE FILES
#include <barsread.h>
#include <mpxuser.h>
#include <mpxlog.h>
#include "mpxresource.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Factory function
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXResource* CMPXResource::NewL(const TDesC& aResourceFile)
    {
    CMPXResource* resObj = NewLC(aResourceFile);
    CleanupStack::Pop(resObj);
    return resObj;
    }

// ----------------------------------------------------------------------------
// Factory function
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXResource* CMPXResource::NewLC(const TDesC& aResourceFile)
    {
    MPX_DEBUG1("CMPXResource::NewLC");
    CMPXResource* self = new (ELeave) CMPXResource();
    CleanupStack::PushL (self);
    self->ConstructL(aResourceFile);
    return self;
    }

// ----------------------------------------------------------------------------
// Decrements the reference count, and delete the object if it is 0
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXResource::Release()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave
// ----------------------------------------------------------------------------
//
CMPXResource::CMPXResource()
    {
    }

// ----------------------------------------------------------------------------
// Contruct the object
// ----------------------------------------------------------------------------
//
void CMPXResource::ConstructL(const TDesC& aResourceFile)
    {
    // In order to support installation of individual plugin.
    // aResourceFile must be a final name.
    // All parse should be done in the plugin side.
    MPX_DEBUG1("CMPXResource::ConstructL");
    User::LeaveIfError(iFs.Connect());
    MPX_DEBUG2("Open resource file %S", &aResourceFile);
    iResourceFile.OpenL(iFs, aResourceFile);
    iResourceFile.ConfirmSignatureL(0);
    MPX_DEBUG1("CMPXResource::ConstructL End");
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXResource::~CMPXResource()
    {
    iResourceFile.Close();
    iFs.Close();
    }

// ----------------------------------------------------------------------------
// Read array of descriptors
// ----------------------------------------------------------------------------
//
EXPORT_C CDesCArrayFlat* CMPXResource::ReadDesCArrayL(TInt aResourceId)
    {
    CDesCArrayFlat* descArray = ReadDesCArrayLC(aResourceId);
    CleanupStack::Pop(descArray);
    return descArray;
    }

// ----------------------------------------------------------------------------
// Read array of descriptors, leave on cleanup stack
// ----------------------------------------------------------------------------
//
EXPORT_C CDesCArrayFlat* CMPXResource::ReadDesCArrayLC(TInt aResourceId)
    {
    //MPX_DEBUG1("CMPXResource::ReadDesCArrayLC");
    TResourceReader resReader;
    HBufC8* readBuffer = iResourceFile.AllocReadLC(aResourceId);
    resReader.SetBuffer(readBuffer);
    CDesCArrayFlat* descArray = resReader.ReadDesCArrayL();
    CleanupStack::PopAndDestroy(readBuffer);
    //MPX_DEBUG1("CMPXResource::ReadDesCArrayLC End");
    CleanupStack::PushL(descArray);
    return descArray;
    }

// ----------------------------------------------------------------------------
// Get a heap descriptor from the resource file
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXResource::ReadHBufCL(TInt aResourceId)
    {
    //MPX_DEBUG1("CMPXResource::ReadHBufCL");
    HBufC8* readBuffer = iResourceFile.AllocReadLC(aResourceId);
    TResourceReader resReader;
    resReader.SetBuffer(readBuffer);
    //resource type has to be LBUF
    HBufC* hbuf = resReader.ReadHBufCL();
    CleanupStack::PopAndDestroy(readBuffer);
    return hbuf;
    }

// ----------------------------------------------------------------------------
// Read array of menu items
// ----------------------------------------------------------------------------
//
EXPORT_C CDesCArrayFlat* CMPXResource::ReadMenuArrayL(TInt aResourceId, RArray<TInt>& aIdArray)
    {
    CDesCArrayFlat* menuArray = ReadMenuArrayLC(aResourceId, aIdArray);
    CleanupStack::Pop(menuArray);
    return menuArray;
    }

// ----------------------------------------------------------------------------
// Read array of menu items, leave on cleanup stack
// ----------------------------------------------------------------------------
//
EXPORT_C CDesCArrayFlat* CMPXResource::ReadMenuArrayLC(TInt aResourceId, RArray<TInt>& aIdArray)
    {
    //MPX_DEBUG1("CMPXResource::ReadMenuArrayLC");
    TResourceReader resReader;
    HBufC8* readBuffer = iResourceFile.AllocReadLC(aResourceId);
    resReader.SetBuffer(readBuffer);
    TInt count(resReader.ReadInt16());
    CDesCArrayFlat* descArray = new (ELeave) CDesCArrayFlat(count);
    for (TInt k = 0; k < count; k++)
              {
              aIdArray.AppendL (resReader.ReadInt32());
              HBufC* hbuf = resReader.ReadHBufCL();
              CleanupStack::PushL(hbuf);
              descArray->AppendL (*hbuf);
              CleanupStack::PopAndDestroy(hbuf);
        }
    CleanupStack::PopAndDestroy(readBuffer);
    //MPX_DEBUG1("CMPXResource::ReadDesCArrayLC End");
    CleanupStack::PushL(descArray);
    return descArray;
    }

// End of File
