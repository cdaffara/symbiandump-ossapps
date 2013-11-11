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

#ifndef __CIMAPCOMMAND_H__
#define __CIMAPCOMMAND_H__

#include <e32std.h>
#include <mentact.h>
#include <msvstd.h>

// Typedefs
typedef RArray<TPtrC8> RDesParts;

// Forward Declaration
class MOutputStream;
class CImapFolderInfo;

/**
Abstract base class for sending IMAP commands and receiving and parsing IMAP responses.
Each implementation of this class represents a single IMAP command together with its expected responses.
Each implementation can use this base class to handle unsolicited responses.

@internalTechnology
@prototype
*/
class CImapCommand : public CBase
	{
public:
	/**
	Identifies the tagged response code that was received for this message.
	*/
	enum TResponseCode 
		{
		/** 
		No tagged response has been received yet.
		The command is not yet complete.
		*/
		EImapResponseNone,
		/**
		A tagged OK response has been received.
		The command has completed successfully.
		Note that receipt of an UNTAGGED OK response should not cause iResponse code to be updated to this value.
		*/
		EImapResponseOk,
		/**
		A tagged NO response has been received.
		The server has completed the command by refusing to run it. 
		*/
		EImapResponseNo, 
		/**
		A tagged BAD response has been received.
		The server has completed the command because it did not understand the command and was not able to run it.
		*/
		EImapResponseBad 
		};
	
	/**
	The result of parsing a block of data.  
	This indicates how the block was parsed, and whether further data is expected.
	*/
	enum TParseBlockResult
		{
		/**
		The block was not recognised, so we need to do generic handling.
		*/
		ENotRecognised,
		/**
		The block was parsed, but more data (e.g. a literal block) is needed for this response.
		*/
		EResponseIncomplete,
		/**
		The block was parsed and the whole of an untagged response has now been received.
		No more data is expected for this response.
		but the command is still expecting to receive a tagged response.
		*/
		ECompleteUntagged,
		/**
		The block was parsed and the whole of a tagged response has now been received.
		No more data is expected for this command.
		The result code can be accessed using CImapCommand::Result();
		*/
		ECompleteTagged,
		};
	
	/**
	Indicates what kind of data is expected next (if any) and how to handle it.
	*/	
	enum TParseState
		{
		/**
		CImapCommand is waiting for the beginning of a new tagged or untagged response.
		A line of data is expected, starting with a tag, * or + character.
		*/
		EWaitStartResponse,
		/**
		The subclass is in the middle of parsing a response.
		It is expecting a block of literal data.
		*/
		EWaitLiteralParse,
		/**
		The subclass is in the middle of parsing a response.
		It is expecting a line of data.
		*/
		EWaitLineParse,
		/**
		CImapCommand is in the middle of discarding a response, 
		so that it can get to the start of the next response.
		It is expecting a block of literal data.
		*/
		EWaitLiteralIngore,
		/**
		CImapCommand is in the middle of discarding a response, 
		so that it can get to the start of the next response.
		It is expecting a line of data.
		*/
		EWaitLineIgnore,
		/**
		The final tagged response for this command has been received.
		No more data is expected.
		*/
		ECommandComplete
		};
		
	enum { KMinPartsForIsLineTagged = 2	};
	
	enum TTagType 
		{ 
		ETagged,
		EUntagged,
		EContinuation 
		};

	~CImapCommand();
	
	/**
	Sends the command represented by this object as a message to the IMAP server.
	@param aTagId Used to generate the IMAP tag that identifies the message.
	@param aStream The output stream on which the message will be sent.
	The provided output stream should already be set up and ready to use.
	*/
	virtual void SendMessageL(TInt aTagId, MOutputStream& aStream)=0;
	
	virtual void SendDataCnfL();
		
	TParseState ParseBlockL(const TDesC8& aData);
	
	virtual void WaitingForMoreDataL();
	
	virtual void Cancel();
	TBool Flushing() const;
	virtual TBool CanCompleteFlushNow() const;
	
	TResponseCode ResponseCode() const;
	TInt LiteralBytesExpected() const;
	
	static void CorruptDataL(TInt aLogId);
	void CorruptDataL();
	
	static HBufC8* EncodeMailboxNameForSendL(const TDesC16& aMailboxName);
	static TBool CheckForPrintableAtomSpecial(const TDesC8& aString, TInt& aCountQuotedSpecials);

	// Utility methods for parsing
	static TInt TagLength(TInt aTagId);
	static void GetDelimitedPartsL(TChar aDelimiter, const TDesC8& aData, RDesParts& aOutDelimitedPartsList, TInt aMaxParts = 0);
		
	TInt GetTagId() ;
	
protected:
	CImapCommand(CImapFolderInfo* aSelectedFolderData, TInt aLogId);

	/**
	This is called after whole line or literal block has been received.
	An implementation of this method should parse the supplied data and store the results
	in the subclass's data members.
	@param aData The data to be parsed.  This will either be a single line or single block of literal data.
			The first call to ParseMessageL() will always receive a line.
			Subsequent calls will receive whatever was requested in the return value of the previous call to this method.
	@return This indicates how the block was parsed, and whether further data is expected.
	*/	
	virtual TParseBlockResult DoParseBlockL(const TDesC8& aData) = 0;
	
	virtual TInt NumberOfTaggedResponsesExpected() const;
	void ParseStartResponseFlushL(const TDesC8& aData);	

	TTagType GetTagTypeL(TInt& aTagId);
	TPtrC8 GetNextPart();
	TPtrC8 PeekNextPart();
	TPtrC8 Remainder();
	TPtrC8 GetResponseTextCodeL();
	TResponseCode GetResponseStateCode();

	void EnterFlushingState();
	
	// Access Methods
	void SetParseState(TParseState aParseState);
	TParseState ParseState() const;
	
	void SetMessageFlagsChanged();
		
private:
	void ParseLineL(const TDesC8& aLine);
	void ParseLiteralL(const TDesC8& aLiteralBlock);

	TParseBlockResult ParseUnhandledBlockL(const TDesC8& aLine);
	TInt PeekLiteralRequest(const TDesC8& aLine);

protected:
	/**
	Stores the tag id supplied by SendMessage()
	Use iTagId to validate that tagged responses have the correct tag id.
	*/
	TInt iTagId;
	/**
	Holds the data to be sent on the output stream to the remote server.
	*/
	HBufC8* iOutputBuffer;
	
	TPtrC8 iUnparsedData;
	/**
	Pointer to the folder data object for the currently selected mailbox.
	The pointer can be NULL if no mailbox is currently selected.
	The pointer is NOT owned by this object.
	*/
	CImapFolderInfo* iSelectedFolderData;
	
	/**
	Stores the tag completion code of the response message
	*/
	TResponseCode iResponseCode;
	
	/**
	Most commands are complete only when the tagged response is received.
	However there are some scenarios where receipt of any response is sufficient 
	to complete the command - in which case this flag should be set to ETrue.
	*/
	TBool iCompleteOnAnyResponse;
	
	/**
	If a literal block is expected in the next call to ParseBlockL(), then
	the number of bytes expected is recorded here.
	0 indicates that a literal block is not expected.
	*/
	TInt iLiteralBytesExpected;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)			
	/**
	If message body is downloaded using FETCH BINARY.
	The number of bytes received for previous fetch is recorded here.
	This valued is used to make sure that there are no extra body fetch command are issued 
	*/
	TInt iPreviousLiteralBytesReceived;
#endif
	
	/**
	The log id of the session that owns this command object.
	*/
	TInt iLogId;
	
private:
	/**
	Indicates what kind of data is expected next (if any) and how to handle it.
	*/
	TParseState iParseState;
	
	/**
	When true, all incoming data is ignored until the last tagged response.
	*/
	TBool iFlushing;
	
	/**
	Counts the number of tagged responses that need to be flushed.
	Usually, this is just the one, but for pipelined commands many requests
	are made at a time, so many tagged responses need to be flushed after a cancel.
	*/
	TInt iTaggedResponsesToFlush;
	};

/**
Provides a migration path that replaces DoParseBlockL() with separate 
subclass methods for different kinds of incoming data.
*/
class CImapCommandEx : public CImapCommand
	{
protected:
	CImapCommandEx(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	
	void BaseParseTaggedResponseL(TInt aTagId);

	/**
	Called when a tagged response is received.
	iUnparsed data contains the incoming line, excluding the tag.
	@param aTagId The value of the incoming tag.
	@return Whether this tags response completes the command.  
			Unless multiple commands are being sent (e.g. in fetch body), this should return ETrue.
	*/
	virtual TBool ParseTaggedResponseL(TInt aTagId);
	virtual void ParseContinuationResponseL();

	virtual TParseBlockResult ParseUntaggedResponseL();
	virtual void ParseLiteralBlockL();
	virtual TBool ParseLineFollowingLiteralL();
	
private:
	TParseBlockResult ParseStartResponseL();

	// Implements CImapCommand
	TParseBlockResult DoParseBlockL(const TDesC8& aData);
	};

#endif // __CIMAPCOMMAND_H__
