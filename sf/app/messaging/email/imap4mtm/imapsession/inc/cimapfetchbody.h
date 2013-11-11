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

#ifndef __CIMAPFETCHBODY_H__
#define __CIMAPFETCHBODY_H__

#include <e32std.h>

#include "cimapcommand.h"
#include "cimapmailstore.h"
#include "tmessageflaginfo.h"

//forward declaration
class CImapSession;
class CImapSettings;
class CImapMimeHeaderFieldsParser;
class CFetchBodyInfo;
class CImapFetchBodyResponse;

/**
@internalTechnology
@prototype
*/
class CImapFetchBody : public CImapCommand, public MMailStoreObserver, public MChunkOutOfOrderObserver
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
		Waiting for a literal that will be delivered to the body  parser.
		*/
		EStateBodyLiteral,
		/**
		Processing the data for BODY[x.MIME]
		*/
		EStateMime,
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
	// Construction and Destruction
	static CImapFetchBody* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, TUint aMessageUid, TBool aPeek, CFetchBodyInfo& aFetchBodyInfo, CImapFetchBodyResponse& aFetchBodyResponse, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapSession& aParent);
	~CImapFetchBody();

	// Overrides CImapCommand
	void Cancel();

	// Fetch Body specific
	TBool IsStoreOperationComplete();
	
	// Overrides CImapCommand::SendDataCnfL
	virtual void SendDataCnfL();
	
private:
	CImapFetchBody(CImapFolderInfo* aSelectedFolderData, TInt aLogId, TUint aMessageUid, TBool aPeek, CFetchBodyInfo& aFetchBodyInfo, CImapFetchBodyResponse& aFetchBodyResponse, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapSession& aParent);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	CImapCommand::TParseBlockResult DoParseBlockL(const TDesC8& aData);
	TInt NumberOfTaggedResponsesExpected() const;

	// from MMailStoreObserver
	void StoreOperationComplete(TMsvId aId,TInt aErrorCode);

	// from MChunkOutOfOrderObserver.		
	void  EnableSendFetch();
	
	void SendMessageL();
	TInt CalculateChunk(TInt aPos);
	TInt TotalRequestsRequired(TInt aSize);
	
	TParseBlockResult ProcessStartL();
	TParseBlockResult ProcessDataItemsL();
	void ProcessStartOfMimeL();
	void ProcessRestOfMimeL(const TDesC8& aData);

	TBool GetAndStoreNextPart();
	void ProcessBodyL();
	void ProcessFlagsL();
	void ProcessUidL();
	void ProcessBodyLiteralL(const TDesC8& aData);
	void WriteMimeHeadersToCafL();
	void StoreBodyDataL(TInt aExtraFetchRequest);

private:
	//the uid of the message on the server - used for SEND operations
	TUint iMessageUid;
	//whether we want to peek, controls the /seen flag
	TBool iPeek;
	//the size of the data we want to retrieve
	TInt32 iSizeToFetch;	
	//the total number of requests we need to issue to receive all of the data
	TInt iTotalRequests;
	//running count of requests that have been issued
	TInt iRequestCount;
	//the chunk where the data currently being parsed will be stored
	TInt iCurrentChunk;
	//the output stream used to send the commands
	MOutputStream* iOutStream;
	//the size of the literal being stored
	TInt iLiteralSize;
	//the maximum fetch size
	TUint iMaxFetchSize;
	//the number of commands that may be pipelined
	TUint iMaxOutstandingRequests;
	//gloabal settings information
	CImapSettings& iImapSettings;
	//info about this specific fetch
	CFetchBodyInfo& iFetchBodyInfo;
	//the gloabl mailstore object
	CImapMailStore& iImapMailStore;
	//the parent session
	CImapSession& iParent;
	//if the store operation is complete
	TBool iStoreComplete;
	// Mime header fields parser for BODY[x.MIME] response
	CImapMimeHeaderFieldsParser* iHeaderFieldsParser;
	/**
	Whether the UID data item was found in the untagged response.
	If the UID data item was NOT found then the response is an unsolicited FETCH response caused
	by a server event (e.g. a user deleting some messages from some other client such as a PC),
	rather than a direct response to our UID FETCH command - which would have included the UID DATA item.
	
	Non-UID FETCH responses should not be returned as the result of a FetchBody request.
	*/
	TBool iUidDataItemFoundInResponse;
	/**
	Stores the result of parsing the FLAGS data item and the UID data item.
	If both UID and FLAGS data items are included in the response, then the data in this
	object will be copied to the equivalent object in iFetchBodyResponse.
	If only the FLAGS data item included in the response, then this is an unsolicited FETCH response
	and the data is discarded (so as not to corrupt the data in iFetchBodyResponse)
	*/
	TMessageFlagInfo iMessageFlagInfo;
	// Stores fetch body response data
	CImapFetchBodyResponse& iFetchBodyResponse;
	// part thats being parsed
	TPtrC8 iCurrentPart;
	//used to track the state of the response parsing
	TState iState;
	// Stores current body data
	HBufC8* iBuf;
	// Indicates that we have received the MIME headers
	TBool iReceivedMimeHeaders;
	// When pipelining this is the amount of commands that are pipelined.
	TInt iOutstandingRequests;
	// Keeps track of the fetch responses received.
	TInt iResponseCount;
	// to keep track of all tags we expect to receive.
	RArray<TInt>iTagIds;
	// Indicates whether to send FETCH command to server.Sending FETCH command 
	// to server may be disabled if server sends out of order chunks.
	TBool iSendFetch;
	// When server responses with no body length, iUnexpectedFormat is set to ETrue.
	TBool iUnexpectedFormat;
	TBool iCancelled;
	};

#endif // __CIMAPFETCHBODY_H__
