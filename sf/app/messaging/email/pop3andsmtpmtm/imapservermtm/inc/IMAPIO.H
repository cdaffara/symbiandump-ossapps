// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CImapIO header
// 
//

#if !defined(__IMAPIO_H__)
#define __IMAPIO_H__

#include <e32base.h>
#include <e32cons.h>
#include <mentact.h>
#include <imsk.h>
#include <iapprefs.h>

// Receive buffer size (max to read in one go). Should really be MTU related,
// as that's what we'll get in a chunk.
const TInt KReceiveBuffer=2048;

// Transmit buffer to keep data static when sending
const TInt KTransmitBuffer=1024;

// What we return with if we find an EOL and have been asked to do a partial
// read
const TInt KErrFoundEOL=1;

// States of async machine
/**
@internalComponent
@released
*/
enum TIOState
	{
	EIOStateDisconnected,
	EIOStateConnectQueued,
	EIOStateConnected,
	EIOStateReadQueued,
	EIOStateWriteQueued,
	EIOStateDummyQueued
	};

// States of reply parser
/**
@internalComponent
@released
*/
enum TIOParserState
	{
	EIOStateAtomWait,
	EIOStateInAtom,
	EIOStateWaitLF,
	EIOStateLiteralLength,
	EIOStateLiteralSkip,
	EIOStateLiteralFetch
	};

class CImapAtom : public CBase
/**
@internalComponent
@released
*/
	{
public:
	CImapAtom();
	~CImapAtom();

	void		Set(const TDesC8& aAtom);
	void		AddChild(CImapAtom *aNewChild);
	void		AddNext(CImapAtom *aNewNext);

	// Get child/next pointers
	CImapAtom*	Child();
	CImapAtom*	Next();

	// As above, but with leave if null
	CImapAtom*	ToChildL();
	CImapAtom*	ToNextL();

	TPtrC8		Atom();
	TBool		Compare(const TDesC8& aValue);
	TBool       CompareTail(const TDesC8& aValue);
	TInt		Value(TUint& aValue);
	TInt		Value(TInt& aValue);
	TInt		Value(TInt32& aValue);
	void		FixupL(const HBufC8 *aNewBuffer, const TText8 *aOldBuffer);
	TPtrC8		AtomNoAngleBrackets();

private:
	TPtrC8		iAtom;
	CImapAtom*	iChild;
	CImapAtom*	iNext;
	};


class CImapIO : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	~CImapIO();

	static CImapIO*	NewLC(TInt aId);
	static CImapIO*	NewL(TInt aId);

	// Queue a connect
	void ConnectL(TRequestStatus& aStatus, const TDesC& aHost, const TUint aPort, const CImIAPPreferences& aPrefs, TBool aSSLWrappedSocket);

	// Queue a line read
	TInt GetReply(TRequestStatus& aStatus);
	TInt GetReply(TRequestStatus& aStatus, const TInt aFetchSize, const TBool aPartialReturn);

	// Queue a line write
	TInt Send(TRequestStatus& aStatus, TRefByValue<const TDesC8> aFmt,...);
	void SendL(TRequestStatus& aStatus, TRefByValue<const TDesC8> aFmt,...);
	void SendWithTimeoutL(TRequestStatus& aStatus, TInt aTimeout, TRefByValue<const TDesC8> aFmt,...);

	// Logging
	void PerformLogging(TBool aLogging);

	// Queue a dummy read for connection status notification
	void QueueDummy(TRequestStatus& aStatus);

	// Get the root atom of the parse tree
	CImapAtom*	RootAtom();

	// Get bytes in/out totals
	TInt RXbytes(const TBool aReset);
	TInt TXbytes(const TBool aReset);

	// Disconnect
	void Disconnect();

	// Security
	void SetTLSResponseL();
	
	// From IMSK
	void LogText(const TDesC8& aString);	
	void LogText(TRefByValue<const TDesC8> aFmt,...);
	
/**
	@fn				TInt GetIAPValue(TUint32& aIap)
	Intended Usage	:	Gets the value of the currently connecting/connected IAP
	@param			aIap will be value of the currently connecting/connected IAP or KErrNotFound if an error occurs 
	@return			KErrNone if succesful, KErrNotFound is the session does not exist or a system-wide error code.

	*/
	TInt GetIAPValue(TUint32& aIap);

/**
	@fn				GetRConnectionName()
	Intended Usage	:	On return, the unique name of the RConnection.
	@since			9.1
	@return			KErrNone if succesful, or another of the system-wide error codes. 
	*/	
	TInt GetRConnectionName(TName &aName);
	
/**
	@fn				GetLastSocketActivityTimeout(TUint32& aTimeout)
	Intended Usage	:	Returns the last socket activity timeout value
	@since			9.1
	@param			aTimeout is a return argument containing the timeout if it was found
	@post				aTimeout will be filled with the timeout value
	@return			Returns KErrNone, KErrNotFound or KErrBadHandle
	*/
	TInt GetLastSocketActivityTimeout(TUint32& aTimeout);

/**
	@fn				TInt GetConnectionStage()
	Intended Usage	:	Gets the stage of the connection process as defined in nifvar.h and csdprog.h
	@since			7.0s
	@return         The current connection stage, KErrNotFound is the session does not exist or a system-wide error code.

	*/
	TInt GetConnectionStage();
	
/**
	@fn				SetPrimaryTextServerSession(CImTextServerSession* aPrimaryTextServerSession)
	Intended Usage	:	Sets the local primaryTextSeverSession from PrimarySession's TextServerSession.
						Going to be use on the secondary session
	@since			9.2
	@param			aPrimaryTextServerSession is to store the primary session's TextServerSession
	*/	
	void SetPrimaryTextServerSession(CImTextServerSession* aPrimaryTextServerSession);

/**
	@fn				GetTextServerSession()
	Intended Usage	:	Returns CImTextServerSession
	@since			9.2
	@return			CImTextServerSession 
	*/	
	CImTextServerSession* GetTextServerSession();

private:
	CImapIO(TInt aId);
	void ConstructL();
	void DoComplete(TInt& /*aStatus*/);
	void DoRunL();
	void DoCancel();
	void ProcessBufferL();
	void QueueRead();
	void BufferAppendL(const TChar aChar);

	void PushL(CImapAtom *aItem);
	CImapAtom* PopL();

	void AddAtomL();
	void AddAtomL(const TInt);
	TInt Send(TRequestStatus& aStatus, const TDesC8& aCommand);
	TInt SendWithTimeout(TRequestStatus& aStatus, TInt aTimeout, const TDesC8& aCommand);

private:
	TIOState				iState;
	TIOParserState			iParserState;
	TBool					iParserQuoted;
	TBool					iGotEscape;
	TInt					iLineStatus;
	TInt					iLiteralSkip;
	TInt					iLiteralLength;
	TInt					iBytesRead;
	TInt					iBytesToRead;
	TBool					iReturnPartialLine;

	HBufC8*					iBuffer;
	TInt					iBufferSize;
	TBuf8<KReceiveBuffer>	iReceive;
	TBuf8<KTransmitBuffer>	iTransmit;
	TInt					iLen;
	TBuf8<1>				iDummyBuffer;

	CImapAtom*				iRootAtom;
	CImapAtom*				iAtom;
	TPtrC8					iCurrentAtom;
	TInt					iAtomStart;

	CArrayPtrFlat<CImapAtom>	iAtomStack;
	TBool					iNextIsChild;

	// Byte I/O totals
	TInt					iTXbytes;
	TInt					iRXbytes;

	// Actual connection
	CImTextServerSession*	iSession;

	// Extra logging
	CImLog*					iImapLog;

	// Atom array
	RPointerArray<CImapAtom> iAtomArray;

	TInt					iID;
	
	// Take ownership of primary session's Textserversession object
	// Only going to be set on the secondary session 
	CImTextServerSession* 	iPrimaryTextServerSession;
	
	};

#endif
