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
* Description:   Thumbnail request
*
*/



#ifndef C_GLXTHUMBNAILREQUEST_H
#define C_GLXTHUMBNAILREQUEST_H

// INCLUDES
#include <glxtnthumbnailrequest.h>

#include "glxrequest.h"
#include "glxmediaid.h"
#include <glxtnfileinfo.h>

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxThumbnailRequest class 
 *
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxThumbnailRequest : public CGlxRequest 
	{
public:
	IMPORT_C static CGlxThumbnailRequest* NewL(TGlxThumbnailRequest aThumbnailRequest,
					const TUid& aCollectionPluginUid, const TInt attributeId, CMPXFilter* aFilter = NULL); 
	virtual ~CGlxThumbnailRequest();

	IMPORT_C const TGlxMediaId& ItemId() const;
	IMPORT_C void ThumbnailRequest(TGlxThumbnailRequest& aTnRequest) const;	
    IMPORT_C TUint AttributeId() const;
    IMPORT_C void SetThumbnailInfo(CGlxtnFileInfo* aInfo);
    IMPORT_C CGlxtnFileInfo* ThumbnailInfo() const;

private:
	CGlxThumbnailRequest(TGlxThumbnailRequest aThumbnailRequest,
					const TUid& aCollectionPluginUid, const TInt attributeId);

private:
	TGlxThumbnailRequest iThumbnailRequest;
	const TInt iAttributeId;
	// owned
	CGlxtnFileInfo* iInfo;
	};


#endif // C_GLXTHUMBNAILREQUEST_H
