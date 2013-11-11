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

#ifndef __CIMAPFETCHBODYSTRUCTUREBASE_H__
#define __CIMAPFETCHBODYSTRUCTUREBASE_H__

#include "cimapcommand.h"
#include "tmessageflaginfo.h"

class CImapFetchResponse;
class MOutputStream;
class CImapBodyStructureBuilder;
class CImapRfc822HeaderFieldsParser;

/**
@internalTechnology
@prototype
*/
class CImapFetchBodyStructureBase : public CImapCommand
	{
private:
	enum TState
		{		
		/**
		Looking for the next data item(s), in the current line.
		This state can transition to EStateBodyStructureLiteral or EStateHeaderFieldsLiteral if 
		either of these data items are found, or EStateComplete if no data items are found.
		*/
		EStateDataItemLine,
		/**
		Waiting for a literal that will be delivered to the body structure parser.
		This state will ALWAYS transition to EStateBodyStructureLine, so that the parser can 
		search for the closing BODYSTRUCTURE bracket which is always delivered on a line.
		*/
		EStateBodyStructureLiteral,
		/**
		Waiting for a line that will be delivered to the body structure parser.
		This state can transition to EStateBodyStructureLiteral or EStateDataItemLine
		depending on whether the parser requests more data (which will be literal).
		*/
		EStateBodyStructureLine,
		/**
		Waiting for a literal that will be delivered to the header fields parser.
		This state will ALWAYS transition to EStateRequestDataItemLine, as header fields are always 
		delivered as a block of literal data followed by a return to the data items line.
		*/
		EStateHeaderFieldsLiteral,
		/**
		This state occurs after EStateHeaderFieldsLiteral.  It means that a data item line
		is expected, but it has yet to be fetched from the input stream.
		This differs to EStateDataItemLine, which means that the current block of data
		still has data items in it.
		*/
		EStateFetchNextDataItemLine,
		/**
		Either all data has been parsed, or the data was not recognised.
		*/
		EStateComplete,
		};

public:
	~CImapFetchBodyStructureBase();

protected:
	CImapFetchBodyStructureBase(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aHeaderFields);

	// From CImapCommand
	TParseBlockResult DoParseBlockL(const TDesC8& aData);

	TBool UidDataItemFoundInResponse();

private:
	// From CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);	

	TParseBlockResult ProcessStartL();
	TParseBlockResult ProcessDataItemsL();
	void ProcessFlagsL();
	void ProcessUidL();
	void ProcessBodyStructureL();
	void ProcessBodyStructureL(const TDesC8& aData);
	void ProcessHeaderFieldsL();
	void ProcessHeaderFieldsL(const TDesC8& aData);
	TBool GetAndStoreNextPart();

protected:
	const TDesC8* iSequenceSet;
	CImapFetchResponse* iFetchResponse;

private:
	/**
	Whether the UID data item was found in the untagged response.
	If the UID data item was NOT found then the response is an unsolicited FETCH response caused
	by a server event (e.g. a user deleting some messages from some other client such as a PC),
	rather than a direct response to our UID FETCH command - which would have included the UID DATA item.
	
	Non-UID FETCH responses should not be returned as the result of a FetchBodystructure request.
	*/
	TBool iUidDataItemFoundInResponse;
	/**
	Stores the result of parsing the FLAGS data item and the UID data item.
	If both UID and FLAGS data items are included in the response, then the data in this
	object will be copied to the equivalent object in iFetchResponse.
	If only the FLAGS data item included in the response, then this is an unsolicited FETCH response
	and the data is discarded (so as not to corrupt the data in iFetchResponse)
	*/
	TMessageFlagInfo iMessageFlagInfo;
	TPtrC8 iCurrentPart;
	const TDesC8& iHeaderFields;
	TState iState;
	CImapBodyStructureBuilder* iBodyStructureBuilder;
	CImapRfc822HeaderFieldsParser* iHeaderFieldsParser;
	};

#endif // __CIMAPFETCHBODYSTRUCTUREBASE_H__
