// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: obextestheaderlist.cpp $
// 
//

// System includes
#include <btmtmcmds.h>      // TBtMtmAsyncCmds
#include <irmtmcmds.h>      // TIrMtmAsyncCmds
#include <msvuids.h>        // KUidMsvMessageEntry
#include <obexclientmtm.h>
#include <btcmtm.h>
#include <ircmtm.h>

// User includes
#include "obextestheaderlist.h"
#include "obextestutils.h"

CObexTestHeaderList* CObexTestHeaderList::NewL()
/**
 * Factory function for class initialisation.
 */
	{
	CObexTestHeaderList* self = new (ELeave) CObexTestHeaderList();
    CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
    return self;
	}

void CObexTestHeaderList::AutoExternaliseL(const TDesC& aFileName, const TDesC& aDesiredResult)
/**
 * Utility function for test harness use
 * creates a write stream in the file passed in and externalises the
 * header list and expected result descriptor
 * the file will be created if not present, and if it's present
 * it's contents will be removed.
 */
	{
	RFs fs;
	CleanupClosePushL(fs);		// PUSH
	User::LeaveIfError(fs.Connect());

	RFileWriteStream writeStream;
	writeStream.Replace(fs, aFileName, (EFileWrite | EFileShareExclusive | EFileStream));
	
	// externalise the 'aDesiredResult' descriptor.
	TUint32 bufLength = aDesiredResult.Length();
	writeStream.WriteUint32L(bufLength);
	
	if(bufLength)
		writeStream.WriteL(aDesiredResult);

	// write out the header list
	CObexHeaderList::ExternalizeL(writeStream);

	// save the new stream
	writeStream.CommitL();
	writeStream.Close();
	CleanupStack::PopAndDestroy(); // fs
	}

void CObexTestHeaderList::AutoInternaliseL(const TDesC& aFileName)
/**
 * Utility function for test harness use
 * opens a read stream in the file passed in and internalises the
 * header list and expected result descriptor.  
 */
	{
	RFs fs;
	CleanupClosePushL(fs);		// PUSH
	User::LeaveIfError(fs.Connect());

	RFileReadStream readStream;
	readStream.Open(fs, aFileName,(EFileRead | EFileShareAny | EFileStream));

	TUint32 bufLength = readStream.ReadUint32L();

	if(bufLength)
		{
		iExpectedResult = HBufC::NewL(bufLength);
		TPtr16 bufPtr = iExpectedResult->Des();
		readStream.ReadL(bufPtr, bufLength);
		}
	else
		{
		iExpectedResult = HBufC::NewL(1);
		}

	CObexHeaderList::InternalizeL(readStream);

	readStream.Close();
	CleanupStack::PopAndDestroy(); // fs
	}

const TDesC& CObexTestHeaderList::ExpectedResultL()
/**
 * returns a reference to the expected result descriptor created during an internalise operation.
 */
	{
	if(!iExpectedResult)
		User::Leave(KErrNotFound);

	return *iExpectedResult;
	}

CObexTestHeaderList::~CObexTestHeaderList()
/**
 * Default destructor
 */
	{
	delete iExpectedResult;
	}

TInt CObexTestHeaderList::GetCount()
/**
 * Returns the number of headers stored in the list belonging to the base class.
 */
	{
	return CObexHeaderList::Count();
	}

CObexHeader*& CObexTestHeaderList::GetHeaderByIndexL(TInt aIndex)
/**
 * Return a reference to a pointer to a particular obex header from the list, 
 * identified by array position.
 */
	{
	return CObexHeaderList::ObexHeaderL(aIndex);
	}

void CObexTestHeaderList::ConstructL()
/**
 * Second phase constructor.
 */
	{
	CObexHeaderList::ConstructL();
	}

CObexTestHeaderList::CObexTestHeaderList() : iExpectedResult(0)
{
}
