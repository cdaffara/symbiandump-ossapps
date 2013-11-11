/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Thumbnail Manager stub impl
*
*/

// Version : %version:  2 %

#include <fbs.h>
#include "tnmgr_stub.h"
#include "mpxvideo_debug.h"


/****************************************************
 * CThumbnailManager stub implementation
 ****************************************************/
CThumbnailManager::~CThumbnailManager()
{
}

CThumbnailManager* CThumbnailManager::NewL( MThumbnailManagerObserver& aObserver )
{
    MPX_ENTER_EXIT(_L("CThumbnailManager::NewL() - stub impl"));
    CThumbnailManager* self = new( ELeave )CThumbnailManager( aObserver );    
    return self;
}

CThumbnailManager::CThumbnailManager( MThumbnailManagerObserver&  aObserver )
: iObserver( aObserver )
{
    MPX_ENTER_EXIT(_L("CThumbnailManager::CThumbnailManager() - stub impl"));
}

TThumbnailRequestId CThumbnailManager::SetThumbnailL( CThumbnailObjectSource& /* aObjectSource */ )
{   
    MPX_ENTER_EXIT(_L("CThumbnailManager::SetThumbnailL() - stub impl"));
    return 0;
}


/****************************************************
 * CThumbnailObjectSource stub implementation
 ****************************************************/
CThumbnailObjectSource::~CThumbnailObjectSource()
{
}

CThumbnailObjectSource* CThumbnailObjectSource::NewL( CFbsBitmap*  aBitmap, const TDesC& aUri )
{
    MPX_ENTER_EXIT(_L("CThumbnailObjectSource::NewL() - stub impl"));
    CThumbnailObjectSource* self = new( ELeave )CThumbnailObjectSource( aBitmap, aUri );
    return self;
}

CThumbnailObjectSource::CThumbnailObjectSource( CFbsBitmap*  /* aBitmap */ , const TDesC& /* aUri */ )
{
}


