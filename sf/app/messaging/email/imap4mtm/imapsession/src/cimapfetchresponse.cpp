// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

// this class
#include "cimapfetchresponse.h"

// user includes
#include "cimapbodystructure.h"
#include "cimaprfc822headerfields.h"

	
EXPORT_C CImapFetchResponse* CImapFetchResponse::NewL()
// static method
// no non-trivial construction is needed as objects are constructed on demand.
	{
	return new(ELeave)CImapFetchResponse();
	}

CImapFetchResponse::~CImapFetchResponse()
	{
	delete iHeaderFields;
	delete iBodyStructure;		
	delete iBodyStructureData;
	}

/**
Returns the header information.
@return A pointer to the header information object or NULL.  Ownership is NOT transferred.
*/	
EXPORT_C CImapRfc822HeaderFields* CImapFetchResponse::HeaderFields()
	{
	return iHeaderFields;
	}

/**
Returns the body structure.
@return A pointer to the body structure or NULL.  Ownership is NOT transferred.
*/
EXPORT_C CImapBodyStructure* CImapFetchResponse::BodyStructure()
	{
	return iBodyStructure;
	}

/**
Sets the bodystructure and its data - and takes ownership of both parameters.
This operation will panic if the bodystructure or data are already set.
@param aBodyStructure The root body structure object
@param aBodyStructureData The data buffer that the bodystructure object points into
*/
void CImapFetchResponse::SetBodyStructure(CImapBodyStructure* aBodyStructure, HBufC8* aBodyStructureData)
	{
	if(iBodyStructure != NULL)
		return;
	// Don't allow data overwrite
	ASSERT(iBodyStructure == NULL);
	ASSERT(iBodyStructureData == NULL);
	
	// BOTH parameters must be non-null (otherwise they are inconsistent)
	ASSERT(aBodyStructure != NULL);
	ASSERT(aBodyStructureData != NULL);
	
	iBodyStructure = aBodyStructure;
	iBodyStructureData = aBodyStructureData;
	}
	
/**
Sets the header fields object and takes ownership of it.
This operation will panic if the header fields object is already set.
@param aHeaderFields The header fields object, representing RFC822 header information.
*/
void CImapFetchResponse::SetHeaderFields(CImapRfc822HeaderFields* aHeaderFields)
	{
	// Don't allow data overwrite
	ASSERT(iHeaderFields == NULL);
	
	iHeaderFields = aHeaderFields;
	}

/**
Copies the supplied message flag and uid info into this object
@param the supplied message flag and uid info
*/
void CImapFetchResponse::SetMessageFlagInfo(const TMessageFlagInfo& aMessageFlagInfo)
	{
	iMessageFlagInfo = aMessageFlagInfo;
	}

/**
@return The Uid of the message that was fetched.
*/
EXPORT_C TUint CImapFetchResponse::MessageUid() const
	{
	return iMessageFlagInfo.MessageUid();
	}

/**
@return The message flags information for this response.
*/
EXPORT_C TMessageFlagInfo& CImapFetchResponse::MessageFlagInfo()
	{
	return iMessageFlagInfo;
	}
