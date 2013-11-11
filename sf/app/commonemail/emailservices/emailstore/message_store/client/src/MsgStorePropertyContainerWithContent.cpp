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
* Description:  Message store property container with content client implementation.
*
*/


#include "MsgStorePropertyContainerWithContent.h"
#include "MsgStoreSessionContext.h"

// -------------------------------------
// CMsgStorePropertyContainerWithContent
// -------------------------------------

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStorePropertyContainerWithContent::CMsgStorePropertyContainerWithContent( 
        CMsgStoreSessionContext& aSessionContext, 
        TMsgStoreId aId, 
        TMsgStoreId aParentId, 
        TMsgStoreId aMailBoxId ) :
	CMsgStoreWritablePropertyContainer( aSessionContext, aId, aParentId, aMailBoxId ), 
	iHasContent(EFalse)
	{
	} // end constructor

// ==========================================================================
// FUNCTION: ContentLengthL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainerWithContent::ContentLengthL()
	{
	return iContext.iSession.ContentLengthL( iId, iParentId );
	} // end ContentLengthL

// ==========================================================================
// FUNCTION: FetchContentToBufferL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::FetchContentToBufferL( TDes8& aBuffer, TUint aStartOffset )
	{
	iContext.iSession.FetchContentL( iId, iParentId, aBuffer, aStartOffset );
	} // end FetchContentToBufferL

// ==========================================================================
// FUNCTION: FetchContentToFileL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::FetchContentToFileL( const TDesC& aFilename )
	{
	iContext.iSession.FetchContentL( iId, iParentId, aFilename );
	} // end FetchContentToFileL

// ==========================================================================
// FUNCTION: RemoveContentL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::RemoveContentL()
	{
	iContext.iSession.RemoveContentL( iId, iParentId );
	iHasContent = EFalse;
	} // end RemoveContentL

// ==========================================================================
// FUNCTION: AppendToContentL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::AppendToContentL( const TDesC8& aBuffer )
	{
	iContext.iSession.AppendToContentL( iId, iParentId, aBuffer );
	iHasContent = ETrue;
	} // end AppendToContentL

// ==========================================================================
// FUNCTION: PrependToContentL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::PrependToContentL(
    const TDesC8& aBuffer )
    {
    iContext.iSession.PrependToContentL( iId, iParentId, aBuffer );
    iHasContent = ETrue;
    }

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::ReplaceContentL( const TDesC8& aBuffer )
	{
	iContext.iSession.ReplaceContentL( iId, iParentId, aBuffer );
	iHasContent = ETrue;
	} // end ReplaceContentL

// ==========================================================================
// FUNCTION: ReplaceContentWithFileL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::ReplaceContentWithFileL( const TDesC& aFilename )
	{
	iContext.iSession.ReplaceContentWithFileL( iId, iParentId, aFilename );
	iHasContent = ETrue;
	} // end ReplaceContentWithFileL
	
// ==========================================================================
// FUNCTION: OpenOriginalContentFileL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainerWithContent::OpenOriginalContentFileL( RFile& aFile )
    {
    iContext.iSession.OpenOriginalContentFileL( iId, iParentId, aFile );
    }

