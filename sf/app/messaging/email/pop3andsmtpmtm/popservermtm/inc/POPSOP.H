// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// popservermtm module header
// @internalComponent
// @released
// 
//

#if !defined(_IMPPOP_H__)
#define _IMPPOP_H__

#include <badesca.h>
#include <e32std.h>
#include <e32base.h>    // For CMsgActive, TFileName
#include <imsk.h>		// ImSockets interface - For CImSocket() wrapper.
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
#include "cpopsaslauthhelper.h"
#endif

class CImRecvConvert;

//
// CImPop3Operation
//
class CImPop3Operation : public CMsgActive
	{
public:
	~CImPop3Operation();
	virtual TBool SetMessage(TMsvId);
	
	TBool PopCommandAccepted();
	// get necessary info from IMCV
	TMsvId EntryId() { return iNewEntryId; };
	TBool  EmptyHeaders() { return iEmptyHeaders; };
	TPtrC8  ImMsgId();

protected:
	CImPop3Operation();
	virtual void Construct(CImPop3Session*);
	virtual void Construct(CImPop3Session*, CImRecvConvert*);

	virtual void DoRunL();
	virtual void DoCancel();
	virtual void DoComplete(TInt&);

	TInt MessageNo(TMsvId);
	void QueueReadNext();
	void RetrieveMessageL();
	void CheckIfPending();
	void RemoveFirstDot();

protected:
	CImTextServerSession*			iTextServer;
	CImPop3Session*					iPopSession;

	TImMailBuffer					iTextServerResponse;

	TInt							iMessageNo;
	TBufC8<3>						iFullStopTerminator;
	TBool							iFirstLine;
	TBool							iCommandAccepted;
	CImRecvConvert*					iRecvConverter;

	TMsvId							iNewEntryId;
	TBool							iEmptyHeaders;
	HBufC8*							iImId;
	TBool							iParsedHeader;
	};

//
// CImPop3Stat
//
// STAT command returns no of messages and total size of remote mailbox
//
class CImPop3Stat : public CImPop3Operation
	{
public:
	static CImPop3Stat* NewL(CImPop3Session*);
	void Start(TRequestStatus&, TUint&, TUint&);
	
private:
	inline CImPop3Stat() {};
	virtual void DoRunL();
private:
	TUint*		iNoMessages;
	TUint*		iMailboxSize;
	};

//
// CImPop3List
//
// list get size of a single message or all messages in remote mailbox
//
class CImPop3List : public CImPop3Operation
	{
public:
	static CImPop3List* NewL(CImPop3Session*);
	void Start(TRequestStatus&, TUint*);
private:
	inline	CImPop3List() {};
	virtual void DoRunL();
	TInt	GetIndexAndSize(TLex8*);
private:
	TUint*		iSize;
	TUint		iArrayPos;
	TInt 		iNoResponses;
	};

//
// CImPop3Retr
//
// retrieve a specified message
//
class CImPop3Retr : public CImPop3Operation
	{
public:
	static CImPop3Retr* NewL(CImPop3Session*, CImRecvConvert*, RFs&);
	void StartL(TRequestStatus&);
	void SetMessageIndex(TInt);
	virtual TUint Progress();

private:
	inline CImPop3Retr(RFs& aFs) : iFs(aFs) {};
	void ConstructL(CImPop3Session *aPopSession, CImRecvConvert* aRecvConverter);
	void DoRunL();
private:
	TUint		iNoBytesRetrieved;
	RFs&		iFs;
	TInt		iCurrentDrive;
	};

//
// CImPop3Dele
//
// delete a specified message
//
class CImPop3Dele : public CImPop3Operation
	{
public:
	static CImPop3Dele* NewL(CImPop3Session*);
	void Start(TRequestStatus&);
private:
	inline CImPop3Dele() {};
	};

//
// CImPop3Noop
//
// carry out a no operation
//
class CImPop3Noop : public CImPop3Operation
	{
public:
	static CImPop3Noop* NewL(CImPop3Session*);
	void Start(TRequestStatus&);
private:
	inline CImPop3Noop() {};
	};

//
// CImPop3Rset
//
// carry out RSET command
//
class CImPop3Rset : public CImPop3Operation
	{
public:
	static CImPop3Rset* NewL(CImPop3Session*);
	void Start(TRequestStatus&);
private:
	inline CImPop3Rset() {};
	};

//
// CImPop3Top
//
// sends the TOP command to request email headers. 
//
class CImPop3Top : public CImPop3Operation
	{
public:
	static CImPop3Top* NewL(CImPop3Session*, CImRecvConvert* ,TBool aHeaderOnly=ETrue);
	void StartL(TRequestStatus&);
	TBool SetMessageAndLines(TMsvId, TInt);
	void SetMessageIndexAndLines(TInt, TInt);
	void SetStartAndEndMessageIndex(TInt, TInt);
	virtual TUint Progress();
private:
	inline CImPop3Top() {};

	void DoRunL();
	void ConstructL(CImPop3Session*, CImRecvConvert* ,TBool aHeaderOnly);


	TInt		iNoLines;
	TUint		iNoBytesRetrieved;
	TBool		iHeadersOnly;
	TInt		iStartMsg;
	TInt		iLastMulti;
	TBool 		iPipeliningSupported;
	};


// 
// CImPop3UidlMap
// 
// return unique POP id of a specified message or ids of all the remote messages
//
class CImPop3UidlMap : public CBase
	{
public:
	static CImPop3UidlMap* NewL(TInt aMsgCount);
	~CImPop3UidlMap();

	void BindL(TInt aMsgIx,const TDesC8& aUidl);
	TInt MsgNo(const TDesC8& aUidl) const;
	TPtrC8 At(TInt aMsgIx) const;
	inline TPtrC8 operator[](TInt aMsgIx) const;
	TInt MsgUidlNo(const TDesC8& aUidl,TInt& aPos) const;
	TInt MsgIndex(TInt aPos) const;
	TInt MsgCount() const;
	TPtrC8 MsgUidl(TInt aPos) const;
private:
	CImPop3UidlMap(TInt aMsgCount);
private:
	CDesC8ArrayFlat iUid;
	CArrayFixFlat<TInt> iMsgNo;
	};

inline TPtrC8 CImPop3UidlMap::operator[](TInt aMsgNo) const
	{return At(aMsgNo);}

//
//  CImPop3Uidl
//
//
class CImPop3Uidl : public CImPop3Operation
	{
public:
	static CImPop3Uidl* NewL(CImPop3Session*);
	void Start(TRequestStatus&, CImPop3UidlMap&);

private:
	inline CImPop3Uidl() {};
	
	void DoRunL();
	void GetIndexAndUidL(TLex8*);
private:
	CImPop3UidlMap*		iMsgUid;
	};
	
//
// CImPop3Capa
//
// returns the list of capabilities supported by the server
//
class CImPop3Capa : public CImPop3Operation
	{
public:
	static CImPop3Capa* NewL(CImPop3Session*);
	void Start(TRequestStatus&);
	TBool PipeliningSupport();
	void ParseSASLCapabilitise(const TDesC8& aData);
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
	void SetSaslAuthExtensionFlag();
	TUint SaslAuthExtensionFlag();
#endif
	
private:
	inline CImPop3Capa() {};
	virtual void DoRunL();

private:
	TBool iPipeliningSupported;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
	TUint iSupportedAuthProfiles; // for TPopAuthProfileFlags
	CPopAuthMechanismHelper::TPopAuthProfileFlag iCurrentAuthProfile;
#endif	
	};

#endif
