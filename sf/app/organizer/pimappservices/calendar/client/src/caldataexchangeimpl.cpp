// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "caldataexchangeimpl.h"

#include <ecom/ecom.h>

#include "calcommonimpl.h"
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calentry.h>
#include <calsession.h>
#include "calsessionimpl.h"

CCalDataExchangeImpl::CCalDataExchangeImpl(CCalSession& aSession)
	: iSession(aSession)
	{
	iSession.Impl().IncrementReferenceCount();
	iDllLoadFailed = EFalse;
	}
	
CCalDataExchangeImpl::~CCalDataExchangeImpl()
	{
	iSession.Impl().DecrementReferenceCount();
	delete iVCalConverter;
	REComSession::FinalClose();
	}

void CCalDataExchangeImpl::LoadDllL()
	{
	
	iVCalConverter = CAgnVersit::NewL();
	User::LeaveIfNull(iVCalConverter);
	}
	
void CCalDataExchangeImpl::ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	// Call Import with no flags set
	ImportL(aDataFormat, aReadStream, aCalEntryArray, 0, KMaxTInt);
	}

void CCalDataExchangeImpl::ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, TInt aFlags)
	{
	ImportL(aDataFormat, aReadStream, aCalEntryArray, aFlags, KMaxTInt);	
	}

void CCalDataExchangeImpl::ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, TInt aFlags, TInt aNumEntries)
	{
	__ASSERT_ALWAYS(aDataFormat == KUidVCalendar, User::Leave(KErrNotSupported));
	
	RPointerArray<CCalEntry> calEntryArray;
	CleanupResetAndDestroyPushL(calEntryArray);
	
	TBool reachedEndOfStream = EFalse;
	TInt entryCount = 0;
	TInt err = KErrNone;
	
	while (!reachedEndOfStream && entryCount < aNumEntries)
		{
		TRAP(err, ImportVCalL(aFlags, aReadStream, calEntryArray));
		const TInt KCount = calEntryArray.Count();

		entryCount += KCount;
		
		if (err == KErrEof)
			{
			reachedEndOfStream = ETrue;
			}
		else
			{
			User::LeaveIfError(err);
			}
			
		for (TInt i = 0; i < KCount; ++i)
			{
			CCalEntry* calEntry = calEntryArray[i];
			aCalEntryArray.AppendL(calEntry);						// aCalEntryArray takes ownership of calEntry
			calEntryArray[i]=NULL;
			}
			
		calEntryArray.Reset();
		}
		
	CleanupStack::PopAndDestroy(&calEntryArray);
	}  
	
void CCalDataExchangeImpl::ExportL(TUid aDataFormat, RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	__ASSERT_ALWAYS(aDataFormat == KUidVCalendar, User::Leave(KErrNotSupported));	
	
	if (iDllLoadFailed)
		 {
		 User::Leave(KErrNotFound);
		 }

	if (iVCalConverter == NULL)
		 {
	     LoadDllL();
		 }

	iVCalConverter->ExportVCalL(aWriteStream, aCalEntryArray, Versit::EUTF8CharSet);
	}

void CCalDataExchangeImpl::ImportAsyncL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver, TInt aFlags)
	{
	__ASSERT_ALWAYS(aDataFormat == KUidVCalendar, User::Leave(KErrNotSupported));
	if (iDllLoadFailed)
		{
		 User::Leave(KErrNotFound);
		}
		
	if (iVCalConverter == NULL)
		{
		LoadDllL();
		}
	iVCalConverter->ImportVCalAsyncL(aFlags, aReadStream, aCalEntryArray,aObserver);
	}

void CCalDataExchangeImpl::ExportAsyncL(TUid aDataFormat, RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver)
	{
	__ASSERT_ALWAYS(aDataFormat == KUidVCalendar, User::Leave(KErrNotSupported));
	if (iDllLoadFailed)
		{
		 User::Leave(KErrNotFound);
		}
		
	if (iVCalConverter == NULL)
		{
		LoadDllL();
		}

	iVCalConverter->ExportVCalAsyncL(aWriteStream, aCalEntryArray, aObserver, Versit::EUTF8CharSet); // takes ownership of entrylist
	}

void CCalDataExchangeImpl::ImportVCalL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray)
	{
	if (iDllLoadFailed)
		{
		 User::Leave(KErrNotFound);
		}
		
	if (iVCalConverter == NULL)
		{
		LoadDllL();
		}
	iVCalConverter->ImportVCalL(aFlags,aReadStream,aEntryArray);
	}
	
/** Signals the destruction of the plug-in to ECom. */
EXPORT_C CAgnVersit::~CAgnVersit()
/**
@internalAll
*/
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

CAgnVersit* CAgnVersit::NewL()
	{
	RImplInfoPtrArray	implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);
	REComSession::ListImplementationsL(KUidEComAgnVersitInterface,implInfoArray);
	//leave if impliInfoArray is empty
	const TInt count = implInfoArray.Count();
	__ASSERT_ALWAYS(count > 0, User::Leave(KErrNotFound));
	// Load the first implementation found for KUidEComAgnVersitInterface which
	// should be KUidEComAgnVersitDefaultImplementation unless it is replaced by licensees
	const TUid firstImplementationFound = implInfoArray[0]->ImplementationUid();
	CleanupStack::PopAndDestroy(&implInfoArray);
    TAny* ptr = REComSession::CreateImplementationL(firstImplementationFound,
								_FOFF(CAgnVersit,iDtor_ID_Key));
	return reinterpret_cast<CAgnVersit*>(ptr); 
	}

