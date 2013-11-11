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
* Description:   Id list request
*
*/



#ifndef C_CGLXIDLISTREQUEST_H
#define C_CGLXIDLISTREQUEST_H

// INCLUDES
#include "glxrequest.h"
#include <glxmediaid.h>
#include <mpxfilter.h>
#include <mpxcollectionpath.h>

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxIdListRequest class 
 *
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxIdListRequest : public CGlxRequest
	{
public:
	IMPORT_C static CGlxIdListRequest* NewL(TGlxMediaId aContainerId, 
			                        const TUid& aCollectionPluginUid, 
			          CMPXFilter* aFilter, CMPXCollectionPath& aPath);

	virtual ~CGlxIdListRequest();
	
	inline const TGlxMediaId& ContainerId() const;
	
	/**
	 * Return the CollectionPath passed in on construction
	 * @return Collection path passed in on construction
	 */
	inline CMPXCollectionPath& CollectionPath();
	
private:
	CGlxIdListRequest(const TUid& aCollectionPluginUid, TGlxMediaId aContainerId, CMPXCollectionPath& aPath);

private:
	TGlxMediaId iMediaId;

	/**
	 * Path to populate
	 */
	CMPXCollectionPath& iPath;
	};

#include "glxidlistrequest.inl"

#endif // C_CGLXIDLISTREQUEST_H

