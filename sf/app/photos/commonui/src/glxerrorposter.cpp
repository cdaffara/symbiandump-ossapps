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
* Description:   Singleton that posts errors to the framework
*
*/



#include "glxerrorposter.h"

#include <glxsingletonstore.h>

#include <glxlog.h>

// ---------------------------------------------------------------------------
// Provides instance of error poster and increments ref count
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxErrorPoster* CGlxErrorPoster::InstanceL()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::InstanceL" );

    return CGlxSingletonStore::InstanceL( &NewL );
    }

// ---------------------------------------------------------------------------
// Removes instance of error poster and decrements ref count
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxErrorPoster::Close()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::Close" );

    CGlxSingletonStore::Close( this );
    }

// ---------------------------------------------------------------------------
// Two phase constructor.
// ---------------------------------------------------------------------------
//
CGlxErrorPoster* CGlxErrorPoster::NewL()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::NewL" );

    CGlxErrorPoster* self = new ( ELeave ) CGlxErrorPoster();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CGlxErrorPoster::CGlxErrorPoster() :
        CActive(EPriorityStandard)
    {
    GLX_LOG_INFO( "CGlxErrorPoster::CGlxErrorPoster" );

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxErrorPoster::~CGlxErrorPoster()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::~CGlxErrorPoster" );

    Cancel();
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CGlxErrorPoster::ConstructL()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::ConstructL" );
    }

// ---------------------------------------------------------------------------
// Post an error
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxErrorPoster::PostError(TInt aError)
    {
    GLX_LOG_INFO1( "CGlxErrorPoster::PostError %d" , aError);

    iLatestError = aError;
    if ( !IsActive() )
        {
        TRequestStatus* requestStatus = &iStatus;
        User::RequestComplete( requestStatus, KErrNone );
        SetActive();
        }
  }

// ---------------------------------------------------------------------------
// CActive::RunL
// ---------------------------------------------------------------------------
//
void CGlxErrorPoster::RunL()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::RunL" );

    TInt error = iLatestError;
    iLatestError = KErrNone;
    User::Leave( error );
    }

// ---------------------------------------------------------------------------
// CActive::DoCancel
// ---------------------------------------------------------------------------
//
void CGlxErrorPoster::DoCancel()
    {
    GLX_LOG_INFO( "CGlxErrorPoster::DoCancel" );
    // No need to do anything 
    // CActive::Cancel() will wait for the request to complete
    }
