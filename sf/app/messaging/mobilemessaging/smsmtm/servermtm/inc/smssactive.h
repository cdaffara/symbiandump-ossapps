// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SMSSACTIVE_H__
#define __SMSSACTIVE_H__

#ifdef _DEBUG
#undef _MSG_NO_LOGGING
#endif

#include <e32base.h>
#include <bif.h>
#include <flogger.h>

#ifdef _MSG_NO_LOGGING
#define SMSSLOG(a)
#else
#define SMSSLOG(a) a
#endif

class RFs;
class CSmsMessage;
class CMsvServerEntry;
class TMsvEntry;
class CSmsNumber;
class CSmsHeader;

class CSmssActive : public CActive
/**
@internalComponent
@released
*/
	{
public:
	CSmssActive(RFs& aFs, CMsvServerEntry& aServerEntry, TInt aPriority = CActive::EPriorityStandard);

protected:
	
	~CSmssActive();
	void Queue(TRequestStatus& aStatus);
	void RequestComplete(TRequestStatus* aStatus, TInt aError, TBool aSetActive = EFalse);

	TBool CanSendMessage(const TMsvEntry& aEntry) const;
	TBool CanSendMessageToRecipient(const TMsvEntry& aEntry, const CSmsNumber& aRcpt) const;
	TInt Complete(TInt aStatus);

	void ChangeEntryL(const TMsvEntry& aNewEntry); //< Calls iServerEntry.ChangeL
 	void StoreHeaderL(const CSmsHeader& aNewHeader);

private:

	virtual TInt RunError(TInt aError); //from CActive. Calls Complete(), then completes iReport if not IsActive().
	virtual void RunL(); //From CActive. Will leave if aStatus != KErrNone
	virtual void DoCancel(); //From CActive. Calls DoSmssCancel(), then calls Complete(KErrCancel)
	virtual void DoRunL() = 0; //Called by RunL() if aStatus == KErrNone
	virtual void DoComplete(TInt& aStatus) = 0;
	virtual void DoSmssCancel() = 0;

protected:

#ifndef _MSG_NO_LOGGING

	//16-bit	
	void FLog(const TDesC& aFile, const TDesC& aInputLine, TBool aIgnoreLineBreaks = ETrue);
	void FLog(const TDesC& aInputLine, TBool aIgnoreLineBreaks = ETrue);
	void FLogFormat(TRefByValue<const TDesC> aFormat, ...);
	void FLogFormat(const TDesC& aFile, TRefByValue<const TDesC> aFormat, ...);
	void FLogFormat(const TDesC& aFile, TRefByValue<const TDesC> aFormat, VA_LIST& aList);
	
	//8-bit
	void FLogFormat(TRefByValue<const TDesC8> aFormat, ...);
	void FLogFormat(const TDesC& aFile, TRefByValue<const TDesC8> aFormat, ...);
	void FLogFormat(const TDesC& aFile, TRefByValue<const TDesC8> aFormat, VA_LIST& aList);
	
	TInt FLogMessage(const TMsvEntry& aEntry, const CSmsMessage& aSmsMessage, TBioMsgIdType aBearer, const TDesC& aFile);
	TBool IsLogging(const TDesC& aFile);

private:
	void DoFLogMessageL(const TMsvEntry& aEntry, const CSmsMessage& aSmsMessage, TBioMsgIdType aBearer, const TDesC& aFile);
	TInt ConstructFlogger(const TDesC& aFile);
	void DoConstructFloggerL(const TDesC& aFile);
	RFileLogger iFlogger;
	HBufC* iLastLogFile;

#else
	TInt8 iBCSpacer[sizeof(RFileLogger)+sizeof(HBufC*)];
#endif


protected:
	RFs& iFs;
	CMsvServerEntry& iServerEntry;

private:
	TRequestStatus* iReport;
	};

#endif	// __SMSSACTIVE_H__
