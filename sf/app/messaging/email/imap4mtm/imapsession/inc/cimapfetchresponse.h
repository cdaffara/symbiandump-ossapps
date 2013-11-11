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


#ifndef __CIMAPFETCHRESPONSE_H__
#define __CIMAPFETCHRESPONSE_H__

#include <e32base.h>

#include "tmessageflaginfo.h"

// Forward Declarations
class CImapBodyStructure;
class CImapRfc822HeaderFields;


/**
Represents the results of a FETCH request.
The iHeaderInfo and iBodyStructure are only constructed on demand.
This is because they are large and are often not present in the FETCH response,
which is indicated by them remaining NULL after parsing.
@internalTechnology
@prototype
*/
class CImapFetchResponse : public CBase
	{
public:
	IMPORT_C static CImapFetchResponse* NewL();
	~CImapFetchResponse();
	
	IMPORT_C CImapRfc822HeaderFields* HeaderFields();
	IMPORT_C CImapBodyStructure* BodyStructure();
	IMPORT_C TUint MessageUid() const;
	IMPORT_C TMessageFlagInfo& MessageFlagInfo();
	
	// non-exported public methods
	void SetBodyStructure(CImapBodyStructure* aBodyStructure, HBufC8* aBodyStructureData);
	void SetHeaderFields(CImapRfc822HeaderFields* aHeaderFields);
	void SetMessageFlagInfo(const TMessageFlagInfo& aMessageFlagInfo);
		
private:	
	/**
	Stores any message flags information recieved for this response.
	*/
	TMessageFlagInfo iMessageFlagInfo;
	/**
	Stores RFC822 header information.  May be NULL.
	*/
	CImapRfc822HeaderFields* iHeaderFields;
	/**
	Stores a (potentially multipart) bodystructure.  May be NULL.
	*/
	CImapBodyStructure* iBodyStructure; 
	/**
	The data buffer that iBodyStructure points into.
	*/
	HBufC8* iBodyStructureData;
	};

/**
Implemented by CImapFolder and anything else that wants perform fetch multiple messages from a single request.
@internalTechnology
@prototype
*/
class MImapFetchStructureObserver
	{
public:
	/**
	Called each time a fetch response has been received and parsed.
	@param aImapFetchResponse stores all the fetch response information and passes ownership to the implementor.
	*/
	virtual void OnFetchLD(CImapFetchResponse* aImapFetchResponse) =0;
	
	/**
	Called to commit any headers that have been created in response to receiving fetch responses.
	*/
	virtual void OnFetchCommit() =0;
	};

#endif // __CIMAPFETCHRESPONSE_H__
