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
* Description:   Get request
*
*/



#ifndef GLXGETREQUEST_H_
#define GLXGETREQUEST_H_

// INCLUDES
#include "glxrequest.h"
#include <glxmediaid.h>
#include <mpxattribute.h>
#include <mpxfilter.h>

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxGetRequest class 
 *
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxGetRequest : public CGlxRequest
	{
public:
	virtual ~CGlxGetRequest();
    IMPORT_C static CGlxGetRequest* NewL(const RArray<TGlxMediaId>& aMediaIdArray, 
			const RArray<TMPXAttribute>& aAttributeArray,
			const RArray<TMPXAttribute>& aCpiAttributeArray,
			const TUid& aCollectionPluginUid, 
			CMPXFilter* aFilter=NULL, 
			const TBool aDoNotNeedDataSource = EFalse);	
public:
	inline const RArray<TGlxMediaId>& MediaIds() const;
	inline TArray<TMPXAttribute> Attributes() const;
	inline TArray<TMPXAttribute> CpiAttributes() const;
	inline TBool CpiAttributesSet() const;
	inline void AppendCpiAttributeL(const TMPXAttribute& aAttribute);
    
private:
	CGlxGetRequest(const RArray<TGlxMediaId>& aMediaIdArray, 
			const RArray<TMPXAttribute>& aAttributeArray,
			const RArray<TMPXAttribute>& aCpiAttributeArray,
			const TUid& aCollectionPluginUid, 
			const TBool aDoNotNeedDataSource);

private:
	RArray<TGlxMediaId> iMediaIdArray;
	RArray<TMPXAttribute> iAttributeArray;
	RArray<TMPXAttribute> iCpiAttributeArray;
	};

#include "glxgetrequest.inl"

#endif // GLXGETREQUEST_H_
