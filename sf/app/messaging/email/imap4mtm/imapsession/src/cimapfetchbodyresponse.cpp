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
#include "cimapfetchbodyresponse.h"
#include "cimapmimeheaderfields.h"

	
EXPORT_C CImapFetchBodyResponse* CImapFetchBodyResponse::NewL()
	{
	return new(ELeave)CImapFetchBodyResponse();
	}

CImapFetchBodyResponse::~CImapFetchBodyResponse()
	{
	delete iHeaderFields;
	}

/**
Returns the header information.
@return A pointer to the header information object or NULL.  Ownership is NOT transferred.
*/	
EXPORT_C CImapMimeHeaderFields* CImapFetchBodyResponse::HeaderFields()
	{
	return iHeaderFields;
	}

/**
Sets the header fields object and takes ownership of it.
This operation will panic if the header fields object is already set.
@param aHeaderFields The header fields object, representing RFC822 header information.
*/
void CImapFetchBodyResponse::SetHeaderFields(CImapMimeHeaderFields* aHeaderFields)
	{
	if (iHeaderFields != NULL)
		{
		delete iHeaderFields;
		}

	iHeaderFields = aHeaderFields;
	}

/**
Copies the supplied message flag and uid info into this object
@param the supplied message flag and uid info
*/
void CImapFetchBodyResponse::SetMessageFlagInfo(const TMessageFlagInfo& aMessageFlagInfo)
	{
	iMessageFlagInfo = aMessageFlagInfo;
	}

/**
@return The message flags information for this response.
*/
EXPORT_C TMessageFlagInfo& CImapFetchBodyResponse::MessageFlagInfo()
	{
	return iMessageFlagInfo;
	}
