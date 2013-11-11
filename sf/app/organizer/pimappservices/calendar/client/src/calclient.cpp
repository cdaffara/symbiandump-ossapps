// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calclient.h"

#include <s32mem.h>

#include "agmallocator.h"
#include "agmattachment.h"
#include "agmentry.h"
#include "agmtlsproxy.h"
#include "agmtzrules.h"
#include "agmrptdef.h"
#include "agmcategory.h"
#include "agmcontent.h"
#include "agmserv.h"
#include "agssignal.h"
#include "calattachmentmanagerimpl.h"
#include "calinstance.h"
#include "calentryimpl.h"
#include "agmsortcriteria.h"
#include "calsessionimpl.h"
#include "calcalendarinfoimpl.h"
#include "agmfilechangenotification.h"

_LIT(KAgendaMutex, "AgnMutex");

//
// RAgendaServ
//
void RAgendaServ::AddSessionL(const CCalSessionImpl& aSession, TInt& aSessionId)
	{
	__ASSERT_DEBUG(FindSession(aSession) == -1, User::Invariant());
	iCalSessions.AppendL(&aSession);
	aSessionId = ++iLastSessionId;
	}

TBool RAgendaServ::RemoveSession(const CCalSessionImpl& aSession)
	{
	TInt pos = FindSession(aSession);
	if (pos != KErrNotFound)
	    {
	    iCalSessions.Remove(pos);
	    }
	return (iCalSessions.Count()>0)?ETrue:EFalse;
	}

TInt RAgendaServ::FindSession(const CCalSessionImpl& aSession)
	{
	const TInt count = iCalSessions.Count();
	for(TInt ii=0; ii<count; ++ii)
		{
		if(&aSession == iCalSessions[ii])
			{
			return ii;
			}
		}
	return -1;
	}

void RAgendaServ::IncreaseRef()
	{
	iReferenceCount++;
	}

TInt RAgendaServ::DecreaseRef()
	{
	iReferenceCount--;
	__ASSERT_ALWAYS( iReferenceCount >= 0, User::Invariant());
	return iReferenceCount;
	}

RAgendaServ* RAgendaServ::NewL()
/** Constructs a new RAgendaServ, initialising all member data to NULL.

@return A pointer to the client interface to the agenda server. */
	{
	RAgendaServ* self = new (ELeave) RAgendaServ();
	
	TRAPD(err, self->ConstructL());
	if(err != KErrNone)
	    {
	    delete self;
	    User::Leave(err);
	    }
	
	return self;
	}

 // not derived from CBase, so have to set member data to NULL
RAgendaServ::RAgendaServ()
	: RSessionBase(), iBuffer(NULL),iLastSessionId(0)
	{
	}

void RAgendaServ::ConstructL()
	{
	iReferenceCount = 0;
	iSimpleEntryAllocator = 0;
	iSimpleEntryAllocator = new (ELeave) CAgnSimpleEntryAllocator;
	CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_Client);
    iBuffer = CBufFlat::NewL(KInitialBufferSize);
    iBuffer->ExpandL(0, KInitialBufferSize);
	}

RAgendaServ::~RAgendaServ()
/** The destructor frees all resources owned by the server, prior to its destruction. */
	{
	__ASSERT_ALWAYS( iReferenceCount == 0, User::Invariant());
	delete iBuffer;
	CAgnTlsProxy::Release(NULL);	
	delete iSimpleEntryAllocator;
	iCalSessions.Close();
	}

// Connect to the server

//
// CAgnServer
//

// Static function to create a new thread or process, depending upon the
// variant. For marm builds, the AGSVEXE.EXE program is run, which 
// creates the server objects in a new process

TInt RAgendaServ::StartServer()
	{
	TRequestStatus stat;
	TAgnSignal signal(stat);

	
	RProcess server;
	// Locate the server on all drives (Y-A, then Z)

	TInt ret = server.Create(KAgendaServerExe, signal.Command(), EOwnerThread);
	if (ret!=KErrNone)
		{
		return ret;
		}
	server.SetPriority(EPriorityForeground);

	server.Rendezvous(stat);
	
	if (stat!=KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}

	
	User::WaitForRequest(stat);
	server.Close();
	return stat.Int();
	}

TInt RAgendaServ::Connect()
/** Connects the client process to the agenda server. 

This function must be called before the server can be used.

@return An error code. KErrNone if successful. */
	{
	RMutex mutex;
	TInt ret = mutex.CreateGlobal(KAgendaMutex);
	if(ret != KErrNone && ret != KErrAlreadyExists)
		{
		return ret;
		}
	ret = mutex.OpenGlobal(KAgendaMutex);	
	if (ret != KErrNone && ret != KErrAlreadyExists)
		{
		return ret;
		}
	mutex.Wait();
	TInt r = DoConnect();
	if (r==KErrNotFound || r==KErrServerTerminated)
		{
		r=StartServer();
		if(r==KErrNone || r==KErrAlreadyExists)
			{
			r=DoConnect();
			}
		}
	mutex.Signal();
	mutex.Close();
	return r;
	}
	
TInt RAgendaServ::DoConnect()
	{
	return CreateSession(KAgendaServerName,Version());
	}


// Return the client side version no.
TVersion RAgendaServ::Version() const
	{
	return (TVersion(KAgnServerMajorVersion,KAgnServerMinorVersion,KAgnServerBuildVersion));
	}



//
// Server Utility Functions

void RAgendaServ::Panic(TAgnClientPanic aPanic) const
	{
	_LIT(KAgendaClient,"AgendaClient");
	User::Panic(KAgendaClient, aPanic);
	}

/** 
Checks the size of the incoming buffer, and if it is larger
than the size of the buffer already defined, allocates enough
space in aBuffer, and transfers the data from the server

@capability ReadUserData
*/
CBufFlat* RAgendaServ::CreateTransmitBufferL(TInt aBufferSize) const
	{
	CBufFlat* buffer = NULL;
	
	if (aBufferSize > KInitialBufferSize)
		{
		// Create a buffer with granularity of aBufferSize
		buffer = CBufFlat::NewL(aBufferSize);
		CleanupStack::PushL(buffer);

		// Extends the size of the bufferr by the required size
		buffer->ExpandL(0,aBufferSize);

		// Transfer the data from the server
		TPtr8 bufptr = buffer->Ptr(0);
		TIpcArgs args(&bufptr);
		User::LeaveIfError(SendReceive(ETransmitBuffer,args));
		CleanupStack::Pop(buffer);
		}
	return buffer;
	}


void RAgendaServ::ExternalizeFilterL(const TAgnFilter& aFilter, RWriteStream& aStream) const
	{
	aFilter.ExternalizeL(aStream);
	}


CAgnSimpleEntry* RAgendaServ::InternalizeSimpleEntryL(RReadStream& aStream) const
	{
	CCalEntry::TType entryType = static_cast<CCalEntry::TType>( aStream.ReadUint32L() );
		
	CAgnSimpleEntry* entry = iSimpleEntryAllocator->CreateSimpleEntryL(entryType);
	//to cleanup CAgnSortEntry pointer, if a leave happens during internalize.
	TCleanSimpleEntry simpleEntryCleanupItem(*entry, *iSimpleEntryAllocator);
	CleanupStack::PushL(TCleanupItem(TCleanSimpleEntry::DestroySimpleEntry,&simpleEntryCleanupItem));
	entry->InternalizeL(aStream, ETrue);
    CleanupStack::Pop(&simpleEntryCleanupItem);
	
	return ( entry );
	}

/**
@capability None
*/
void RAgendaServ::OpenAgendaL(const CCalSessionImpl& aSession, const TDesC& aFileName, CalCommon::TCalFileVersionSupport& aStatus, TInt64& aFileId, TCalCollectionId& aCallectionId)
	{
    CBufFlat* streamBuffer = CBufFlat::NewL(aFileName.Size()+ sizeof(TUint8));
    CleanupStack::PushL(streamBuffer);
    RBufWriteStream bufStream;
    bufStream.Open(*streamBuffer);
    CleanupClosePushL(bufStream);
    bufStream << aFileName;
    bufStream.WriteInt32L(aSession.SessionId());
    CleanupStack::PopAndDestroy(&bufStream);
    TPtr8 readPtr = streamBuffer->Ptr(0);
    TPckgBuf<CalCommon::TCalFileVersionSupport> statusPckg(aStatus);
    TPckgBuf<TInt64> fileId(aFileId);
    TPckgBuf<TCalCollectionId> shortFileId(aCallectionId);
    TIpcArgs args(&readPtr, &statusPckg, &fileId, &shortFileId);  
    TInt err = SendReceive(EOpenAgenda,args);
    CleanupStack::PopAndDestroy(streamBuffer);
    if(err != KErrAlreadyExists)
        {
        aStatus = statusPckg();
        User::LeaveIfError(err);
        aFileId = fileId();
        aCallectionId = shortFileId();
        }
	}
	
void RAgendaServ::WaitUntilLoadedL(TCalCollectionId aCallectionId)
	{
	TIpcArgs args(0, aCallectionId);//0 argument means not to report the progess
	TRequestStatus status = KRequestPending;
	SendReceive(EStartBuildIndex, args, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

/** 
Closes the file that is open in the current agenda server session. 
@capability None
*/
void RAgendaServ::CloseAgenda(TInt aSessionId)
	{
	TIpcArgs args(aSessionId);
	SendReceive(ECloseAgenda, args);
	}

/** 
Fetches a complete entry from the server
@capability ReadUserData
*/
CAgnEntry* RAgendaServ::FetchEntryL(TAgnEntryId& aId, TInt64 aFileId) const
	{
	TPckg<TAgnEntryId> entryId(aId);
	TPckgBuf<TInt> size;
	TPtr8 ptr = iBuffer->Ptr(0);

	if (aId.IsNullId())
		{
		return NULL;
		}
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&ptr, &entryId, &size, &fileId);
	User::LeaveIfError(SendReceive(EFetchEntry,args));

	return ReadEntryFromServerSessionL(size());	
	}

/** Fetches a complete entry from the server
@capability ReadUserData
*/
CAgnEntry* RAgendaServ::FetchEntryL(TCalLocalUid aId, TInt64 aFileId) const
	{
	TPckg<TCalLocalUid> entryId(aId);
	TPckgBuf<TInt> size;
	TPtr8 ptr = iBuffer->Ptr(0);
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&ptr, &entryId, &size, &fileId);
	User::LeaveIfError(SendReceive(EFetchEntryByUID,args));
	return ReadEntryFromServerSessionL(size());
	}


/* Internalize agenda entry from buffer 

@capability ReadUserData
@param aBufferSize size of buffer 
@return agenda entry internalized from buffer
*/
CAgnEntry* RAgendaServ::ReadEntryFromServerSessionL(TInt aBufferSize) const
	{
	if (aBufferSize==0) // if size = 0, there entry has already been deleted
		{
		return NULL;
		}

	RBufReadStream readStream;
	// Create a transfer buffer, and set where to read the data from
	CBufFlat* buffer = CreateTransmitBufferL(aBufferSize);
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	CAgnEntry* entry = CreateEntryFromBufferStreamL(readStream);
	readStream.Close();
	CleanupStack::PopAndDestroy(buffer);

	return entry;
	}

CAgnEntry* RAgendaServ::CreateEntryFromBufferStreamL(RReadStream& aReadStream) const
	{
	CCalEntry::TType entryType = (CCalEntry::TType)aReadStream.ReadUint32L();
	
	CAgnEntry* entry = CAgnEntry::NewL(entryType);
	CleanupStack::PushL(entry);
	entry->InternalizeFromBufferL(aReadStream);
	CleanupStack::Pop(entry);
	return entry;
	}

/** 
Get a lite entry from the server
(Lite-entries are just time information, with no text, etc)
@capability ReadUserData
*/
CAgnSimpleEntry* RAgendaServ::GetSimpleEntryL(const TAgnEntryId& aId, TCalCollectionId aCallectionId) const
	{
	TPckgBuf<TInt> size;

	TPtr8 ptr = iBuffer->Ptr(0);

	if (aId.IsNullId())
		{
		return NULL;
		}
	CBufFlat* streamBuffer = CBufFlat::NewL(sizeof(TAgnInstance));
	CleanupStack::PushL(streamBuffer);
	RBufWriteStream bufStream;
	bufStream.Open(*streamBuffer);
	CleanupClosePushL(bufStream);

	bufStream << aId;
	CleanupStack::PopAndDestroy(&bufStream);
	TPtr8 readPtr = streamBuffer->Ptr(0);
	TIpcArgs args(&ptr, &readPtr, &size, aCallectionId);
	User::LeaveIfError(SendReceive(EFetchSimpleEntry,args));
	CleanupStack::PopAndDestroy(streamBuffer);
	if (size() == 0)
		{
		User::Leave(KErrNotFound);
		}

	RBufReadStream readStream;

	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}

	CleanupStack::PushL(buffer);

	CAgnSimpleEntry* entry = NULL;

	if (readStream.ReadUint8L())
		{
		entry = InternalizeSimpleEntryL(readStream);
		entry->SetCollectionId(aCallectionId);
		}
	
	readStream.Close();
	
	CleanupStack::PopAndDestroy(buffer);
	
	// Don't leave here for a NULL entry could be a race with an entry delete
	return entry;
	}

/** 
Get a lite entry from the server
(Lite-entries are just time information, with no text, etc)
@capability ReadUserData
*/
CAgnSimpleEntry* RAgendaServ::GetSimpleEntryL(TCalCollectionId aCallectionId, TUint32 aId) const
	{
//	TPckg<TAgnEntryId> entryId(aId);
	TPckgBuf<TInt> size;

	TPtr8 ptr = iBuffer->Ptr(0);
	if (aId == 0)
		{
		return NULL;
		}
	CBufFlat* streamBuffer = CBufFlat::NewL(sizeof(TUint8)+sizeof(TUint32));
	CleanupStack::PushL(streamBuffer);
	RBufWriteStream bufStream;
	bufStream.Open(*streamBuffer);
	CleanupClosePushL(bufStream);

	bufStream << aId;
	CleanupStack::PopAndDestroy(&bufStream);
	TPtr8 readPtr = streamBuffer->Ptr(0);
	TIpcArgs args(&ptr, &readPtr, &size, aCallectionId);
	User::LeaveIfError(SendReceive(EFetchSimpleEntryByUID,args));
	CleanupStack::PopAndDestroy(streamBuffer);
	if (size() == 0)
		{
		User::Leave(KErrNotFound);
		}

	RBufReadStream readStream;

	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}

	CleanupStack::PushL(buffer);

	CAgnSimpleEntry* entry = NULL;

	if (readStream.ReadUint8L())
		{
		entry = InternalizeSimpleEntryL(readStream);
		entry->SetCollectionId(aCallectionId);
		}
	
	readStream.Close();
	
	CleanupStack::PopAndDestroy(buffer);
	
	// Don't leave here for a NULL entry could be a race with an entry delete
	return entry;
	}
	
/** Get an array of lite entries from the server

On return the instance ids and lite entries will have corresponding indices in the arrays 
(Lite-entries are just time information, with no text, etc)

@param aIds The instance ids of the lite entries to fetch
@param aLiteEntries on return will contain the fetched lite entries.

@capability ReadUserData
*/
void RAgendaServ::GetSimpleEntryL(const RArray<TAgnInstance>& aEntryIds, RPointerArray<CAgnSimpleEntry>& aLiteEntries) const
	{
	// only fetches KMaxNumToFetch at a time from the server
	const TInt KMaxNumToFetch(32);
	const TInt KBufferSize = (KMaxNumToFetch + 1) * sizeof(TUint32); // enough room to write KNumToFetch entries plus a count
	
	const TInt KEntryIdCount(aEntryIds.Count());
	for (TInt entriesFetchedSoFar(0) ; entriesFetchedSoFar < KEntryIdCount ;)
		{
		CBufFlat* instanceIdBuffer = CBufFlat::NewL(KBufferSize);
		CleanupStack::PushL(instanceIdBuffer);
		RBufWriteStream bufStream;
		bufStream.Open(*instanceIdBuffer);
			
		// Externalize up to KMaxNumToFetch instance ids
		// KNumToFetch will be the total number of fetched TAgnLiteEntrys fetched this time
		const TInt KNumToFetch = ((KEntryIdCount - entriesFetchedSoFar) < KMaxNumToFetch) ? KEntryIdCount : entriesFetchedSoFar + KMaxNumToFetch;
		bufStream.WriteUint32L(KNumToFetch - entriesFetchedSoFar);
		for (; entriesFetchedSoFar < KNumToFetch ; ++entriesFetchedSoFar)
			{
			bufStream << aEntryIds[entriesFetchedSoFar];
			}
			
		bufStream.CommitL();
		bufStream.Close();
		TPtr8 entryIds = instanceIdBuffer->Ptr(0);
		
		TPckgBuf<TInt> size;
		TPtr8 ptr = iBuffer->Ptr(0);
		
		// call the server
		TIpcArgs args(&ptr, &entryIds, &size);
		User::LeaveIfError(SendReceive(EFetchSimpleEntries, args));
			
		// open the read stream
		RBufReadStream readStream;
		CBufFlat* liteEntryBuffer = CreateTransmitBufferL(size());
		if (liteEntryBuffer)
			{
			readStream.Open(*liteEntryBuffer);
			}
		else
			{
			readStream.Open(*iBuffer);
			}
		CleanupStack::PushL(liteEntryBuffer);

		// Read the lite entries from the buffer 
		const TCalCollectionId KLiteEntryCount(readStream.ReadUint8L());
		for (TInt i(0) ; i < KLiteEntryCount ; ++i)
			{
			if (readStream.ReadUint8L())
				{
				CAgnSimpleEntry* ptr = InternalizeSimpleEntryL(readStream);
				ptr->SetCollectionId(readStream.ReadUint8L());
				TInt err = aLiteEntries.Append(ptr);
				if (err != KErrNone)
					{
					iSimpleEntryAllocator->Delete(ptr);
					User::Leave(err);
					}
				}
			}
		
		readStream.Close();
		
		CleanupStack::PopAndDestroy(liteEntryBuffer);
		CleanupStack::PopAndDestroy(instanceIdBuffer);
		}
	}

/** Get an array of lite entries from the server

On return the instance ids and lite entries will have corresponding indices in the arrays 
(Lite-entries are just time information, with no text, etc)

@param aUid The UID of the lite entries to fetch
@param aLiteEntries on return will contain the fetched lite entries.

@capability ReadUserData
*/
void RAgendaServ::FetchSimpleEntriesByGuidL(const TDesC8& aUid, RPointerArray<CAgnSimpleEntry>& aLiteEntries, RArray<TInt> aFileIds) const
	{
	const TInt countFile = aFileIds.Count();
	const TInt lenUid = aUid.Length();
	
	CBufFlat* inBuffer = CBufFlat::NewL(sizeof(countFile*8) + lenUid);
	CleanupStack::PushL(inBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*inBuffer);
	bufStream.WriteInt16L(lenUid);
	bufStream.WriteL(aUid, lenUid);
	bufStream.WriteInt16L(countFile);
	for(TInt ii=0; ii<countFile;++ii)
		{
		bufStream.WriteUint8L(aFileIds[ii]);
		}
	CleanupStack::PopAndDestroy(&bufStream);
	
	TPtr8 paramsPtr = inBuffer->Ptr(0);

	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);
	TIpcArgs args(&ptr, &size, &paramsPtr);
	User::LeaveIfError(SendReceive(EFetchSimpleEntriesByGuid, args));
	CleanupStack::PopAndDestroy(inBuffer);
	// open the read stream
	RBufReadStream readStream;
	CBufFlat* liteEntryBuffer = CreateTransmitBufferL(size());
	if (liteEntryBuffer)
		{
		readStream.Open(*liteEntryBuffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(liteEntryBuffer);

	// Read the lite entries from the buffer 
	const TUint32 KLiteEntryCount(readStream.ReadUint32L());
	for (TInt i(0) ; i < KLiteEntryCount ; ++i)
		{
		CAgnSimpleEntry* ptr = InternalizeSimpleEntryL(readStream);
		ptr->SetCollectionId(readStream.ReadUint8L());
		TInt err = aLiteEntries.Append(ptr);
		if (err != KErrNone)
			{
			iSimpleEntryAllocator->Delete(ptr);
			User::Leave(err);
			}
		}
	
	readStream.Close();
	
	CleanupStack::PopAndDestroy(liteEntryBuffer);
	}

/** Updates an existing entry on the server
@param aEntry
@param aTodoPosition
@capability WriteUserData
@capability ReadUserData
@leave
@internalComponent
*/
void RAgendaServ::UpdateEntryL(CAgnEntry& aEntry, TBool aDeleteChildren, TCalCollectionId aCallectionId)
	{
	/*
	Check that either (or both) the instance or the unique id of the entry
	passed is not null, otherwise, the update will not occur because the server
	will not know which of its entries it should update.
	*/
	__ASSERT_ALWAYS( !(aEntry.EntryId().IsNullId() && aEntry.LocalUid() == 0 && aCallectionId == NULL), User::Leave(KErrNotFound));
	
	aEntry.VerifyBeforeStoreL();
	
	RBufWriteStream bufStream;

	bufStream.Open(*iBuffer);

	bufStream.WriteUint32L(aEntry.Type());
	bufStream.WriteUint8L(aDeleteChildren);

	// Set regardless.
	aEntry.SetLastModifiedDate();

	// Externalize the entry details to the buffer
	// Any rich text components will be stored in the textStream buffer
	aEntry.ExternalizeToBufferL(bufStream);
	bufStream.CommitL();
	bufStream.Close();

	// Get the sizes of the buffers
	TInt size(iBuffer->Size());
	
	// Get the pointer of the buffers
	TPtr8 ptr = iBuffer->Ptr(0);
	
	// The entry id	
	TPckgBuf<TAgnEntryId> entryId;
	TIpcArgs args(size, &ptr, &entryId, aCallectionId);
	
	User::LeaveIfError(SendReceive(EUpdateEntry, args));	
	aEntry.SetEntryId(entryId());	
	}

/** 
Add a new enty to the server
@capability WriteUserData
@capability ReadUserData
*/
TAgnEntryId RAgendaServ::AddEntryL(CAgnEntry& aEntry, TInt64 aFileId)
	{
	aEntry.VerifyBeforeStoreL();
	
	RBufWriteStream bufStream;

	bufStream.Open(*iBuffer);
	
	bufStream.WriteUint32L(aEntry.Type());

	aEntry.ExternalizeToBufferL(bufStream);

	bufStream.CommitL();
	bufStream.Close();

	// Get the sizes of the buffers
	TInt size = iBuffer->Size();
	
	// Get the pointer of the buffers
	TPtr8 ptr = iBuffer->Ptr(0);
	
	// The entry Id and unique Id
	TPckgBuf<TAgnEntryParameters> entryParams;
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(size, &ptr, &entryParams, &fileId);	
	
	User::LeaveIfError(SendReceive(EAddEntry, args));
	
	aEntry.SetEntryId(entryParams().iEntryId);
	aEntry.SetLocalUid(entryParams().iLocalId);
	if (entryParams().iRecurrenceId != Time::NullTTime())
		{
		TAgnCalendarTime agnRecId;
		if (aEntry.TimeMode() == MAgnCalendarTimeMode::EFloating)
			{
			agnRecId.SetFloatingL(entryParams().iRecurrenceId);
			}
		else
			{
			agnRecId.SetLocalL(entryParams().iRecurrenceId);
			}
		aEntry.UpdateRecurrenceIdL(agnRecId);
		}
	aEntry.SetLastModifiedDateUtc(entryParams().iLastModifiedDateUtc);
	if (aEntry.TimeMode() != MAgnCalendarTimeMode::EFloating &&	aEntry.RptDef() != NULL	)
		{
		CAgnTzRules* tzRules = aEntry.RptDef()->AgnTzRules();
		if(tzRules != NULL)
			{
			tzRules->SetTzZoneStreamId(entryParams().iTzStreamId);
			tzRules->SetSystemTzRule(entryParams().iSystemTzRule);
			}
		}
	

	SendFileAttachmentHandlesL(aEntry, aFileId);

	return entryParams().iEntryId;
	}

/**
For all attachments on this entry, find out whether they are file attachments.
If it contains a file handle, then it must be transferred to the server through a separate IPC call (TransferFileHandleToServerL).
If it contains binary data, then a file handle must be fetched from the server, to which the binary data 
will be written directly (WriteBinaryDataToNewAttachmentFileL).
*/
void RAgendaServ::SendFileAttachmentHandlesL(CAgnEntry& aEntry, TInt64 aFileId)
	{
	const TInt KAttachmentCount = aEntry.AttachmentCount();
	for (TUint32 i = 0; i < KAttachmentCount; i++)
		{
		CAgnAttachment& attach = aEntry.Attachment(i);
		if (attach.Type() == CCalContent::EDispositionInline)
			{
			// This is a file attachment
			CAgnAttachmentFile* fileAttach = static_cast<CAgnAttachmentFile*>(&attach);
			if (fileAttach->IsFileHandleSet())
				{
				// This is a file attachment with a file handle
				TransferFileHandleToServerL(aEntry.LocalUid(), i, *fileAttach, aFileId);
				}
			else if (fileAttach->Uid() == 0 && fileAttach->Content().Length() > 0)
				{
				// This is a new file attachment (uid==0) with binary data
				WriteBinaryDataToNewAttachmentFileL(aEntry, i, aFileId);
				}
			}
		}
	}

/**
Store a new attachment with a file handle. The file handle is transferred to the Calendar server, which moves the file to 
the Calendar store.
On the server-side, this will also update the entry containing this attachment with the new attachment metadata.
@capability WriteUserData
*/
void RAgendaServ::TransferFileHandleToServerL(TCalLocalUid aLocalUid, TInt aAttachmentIndex, CAgnAttachmentFile& aFileAttachment, TInt64 aFileId)
	{
	TAttachmentData attachData;
	attachData.iLocalUid = aLocalUid;
	attachData.iAttachmentIndex = aAttachmentIndex;
	TPckg<TCalAttachmentUid> attachUid(0);
	
	CBufFlat* paramsBuffer = CBufFlat::NewL(sizeof(TInt64) + sizeof(TAttachmentData));
	CleanupStack::PushL(paramsBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*paramsBuffer);
	bufStream.WriteUint32L(attachData.iLocalUid);
	bufStream.WriteInt16L(attachData.iAttachmentIndex);
	bufStream << aFileId;
	CleanupStack::PopAndDestroy(&bufStream);
	 	
	TPtr8 paramsPtr = paramsBuffer->Ptr(0);
	TIpcArgs args(&paramsPtr, &attachUid);
	// Detach file handle from attachment, if TransferToServer or SendReceive(ETransferAttachmentFileToServer) fail,
	// then the fileHandle must not be deleted.
	RFile& fileHandle = const_cast<RFile&>(aFileAttachment.FileHandle()); 
	CleanupClosePushL(fileHandle);

	// note that the following line will panic if the RFs that contains 
	// this file handle is not shared by calling RFs::ShareProtected()
	User::LeaveIfError(aFileAttachment.FileHandle().TransferToServer(args, KTransferAttachmentToSrvFsArgIndex, KTransferAttachmentToSrvFileArgIndex));
	User::LeaveIfError(SendReceive(ETransferAttachmentFileToServer, args));//replace the file in the same drive
	CleanupStack::PopAndDestroy(&fileHandle);
	if(attachUid() == 0)
		{//When the Drive is different from the default one, the server adopt the handle and will move the attachment file after the file handle is closed.
		User::LeaveIfError(SendReceive(EMoveFileToServer, args));
		}
	CleanupStack::PopAndDestroy(paramsBuffer);
	aFileAttachment.SetHasFileHandle(EFalse);
	aFileAttachment.SetUid(attachUid());
	}

/**
Store a new attachment from binary data. A new file handle is created on the Calendar server, and this is transferred
to the client side so that the binary data can be written directly.
On the server-side, this will also update the entry containing this attachment with the new attachment metadata.
@capability WriteUserData
*/
void RAgendaServ::WriteBinaryDataToNewAttachmentFileL(CAgnEntry& aEntry, TInt aAttachmentIndex, TInt64 aFileId)
	{
	CAgnAttachmentFile* attachment = static_cast<CAgnAttachmentFile*>(&aEntry.Attachment(aAttachmentIndex));

	// package up data required to identify this attachment on server side.
	TAttachmentData data;
	data.iLocalUid = aEntry.LocalUid();
	data.iAttachmentIndex = aAttachmentIndex;
	if (attachment->FileName().Length() > 0)
		{
		data.iDrive = attachment->FileName().Left(1);
		}
	else
		{
		data.iDrive = KNullDesC;
		}

	TPckg<TAttachmentData> pckgData(data);
	TPckg<TInt> pckgSubSessionHandle(0);  // initialise	
	TPckgBuf<TInt64> fileId(aFileId);
	
	TIpcArgs args(&pckgData, &pckgSubSessionHandle, &fileId);
	
	TInt dataSize = attachment->Content().Length();
	args.Set(3, dataSize);

	// call server side to create new file and update entry
	const TInt KSessionHandle = SendReceive(ETransferFileToClientToWrite, args);
	User::LeaveIfError(KSessionHandle);

	// fetch file handle and write binary data to it
	RFile newFileHandle;
	CleanupClosePushL(newFileHandle);
	User::LeaveIfError(newFileHandle.AdoptFromServer(KSessionHandle, pckgSubSessionHandle()));
	User::LeaveIfError(newFileHandle.Write(attachment->Content()));
	CleanupStack::PopAndDestroy(&newFileHandle); // newFileHandle.Close();
	}
	
/**
Fetch file handle for a file attachment.
@capability ReadUserData
*/
void RAgendaServ::FetchFileHandleL(RFile& aFileHandle, TCalAttachmentUid aAttachmentUid, TInt64 aFileId)
	{
	TPckg<TInt> pckgSubSessionHandle(0);  // initialise
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(aAttachmentUid, &pckgSubSessionHandle, &fileId);

	TInt sessionHandle = SendReceive(ETransferAttachmentFileToClient, args);
	User::LeaveIfError(sessionHandle);
	User::LeaveIfError(aFileHandle.AdoptFromServer(sessionHandle, pckgSubSessionHandle()));
	}

/**
Fetch all file attachments in the order specified and add them to the iterator.
@capability ReadUserData
*/
void RAgendaServ::FetchAttachmentsL(CCalAttachmentIteratorImpl& aIteratorImpl, CCalAttachmentManager::TSortOrder aSortOrder, TInt64 aFileId)
	{
	TPtr8 bufptr = iBuffer->Ptr(0);
	TPckg<TInt> size(0);
	TPckgBuf<TInt64> fileId(aFileId);
	// make server request
	TIpcArgs args(&bufptr, &size, aSortOrder, &fileId);
	User::LeaveIfError(SendReceive(EFetchSortedAttachments, args));
	
	RBufReadStream readStream;
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	
	CleanupStack::PushL(buffer);
	// internalize array from readStream
	const TInt KCount = readStream.ReadUint32L();

	TCalAttachmentUid attachUid;
	for (TInt i = 0; i < KCount; ++i)
		{
		readStream >> attachUid;
		aIteratorImpl.AppendL(attachUid);
		}
	
	readStream.Close();
	CleanupStack::PopAndDestroy(buffer);
	}

/**
Fetch local UIDs of all entries that reference the attachment specified.
@capability ReadUserData
*/
void RAgendaServ::EntriesReferencingFileAttachmentL(RArray<TCalLocalUid>& aUids, const CAgnAttachment& aAttachment, TInt64 aFileId)
	{
	if (aAttachment.Uid() && aAttachment.Type() == CCalContent::EDispositionInline)
		{
		TPtr8 bufptr = iBuffer->Ptr(0);
		TPckg<TInt> size(0);
		TPckgBuf<TInt64> fileId(aFileId);
		TIpcArgs args(&bufptr, &size, aAttachment.Uid(), &fileId);
		User::LeaveIfError(SendReceive(EEntriesWithAttachment, args));
		
		RBufReadStream readStream;
		CBufFlat* buffer = CreateTransmitBufferL(size());
		if (buffer)
			{
			readStream.Open(*buffer);
			}
		else
			{
			readStream.Open(*iBuffer);
			}
		CleanupStack::PushL(buffer);

		// internalize local UIDs from readStream
		const TInt KCount = readStream.ReadUint32L();
		TCalLocalUid id;
		for (TInt i = 0; i < KCount; ++i)
			{
			readStream >> id;
			aUids.AppendL(id);
			}

		readStream.Close();
		CleanupStack::PopAndDestroy(buffer);
		}
	}

/**
Fetch an attachment by its attachment UID. Note attachment UIDs are not exposed to clients, and are just used internally.
@capability ReadUserData
*/
CAgnAttachment* RAgendaServ::FetchAttachmentByIdL(TCalAttachmentUid aAttachUid, TInt64 aFileId)
	{
	if (aAttachUid == 0)
		{
		return NULL;
		}
	
	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);

	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&ptr, &size, aAttachUid, &fileId);
	User::LeaveIfError(SendReceive(EFetchAttachmentById, args));

	if (size() == 0) // if size = 0, there entry has already been deleted
		{
		return NULL;
		}

	RBufReadStream readStream;
	// Create a transfer buffer, and set where to read the data from
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	CAgnAttachment* attachment = AttachmentFactory::NewAttachmentL(readStream);

	readStream.Close();
	
	CleanupStack::PopAndDestroy(buffer);
	return attachment;
	}

/** Finds previous instances in the calendar store

@param aParams The filter settings to use when fidning the previous instances
@param aInstances The previous instances.  Note that the instances will be inserted at the begining of the array
 
@capability ReadUserData
*/
void RAgendaServ::PreviousInstancesL(const RArray<TInt64> aFileIds, CArrayFix<TAgnInstance>& aInstances, TFindInstanceParams& aParams) const
	{
	const TInt count = aFileIds.Count();
	CBufFlat* paramsBuffer = CBufFlat::NewL(sizeof(TFindInstanceParams) + count*64);
	CleanupStack::PushL(paramsBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*paramsBuffer);
	aParams.ExternalizeL(bufStream);
	bufStream.WriteInt16L(count);
	for(TInt ii=0; ii<count;++ii)
		{
		bufStream << aFileIds[ii];
		}
	CleanupStack::PopAndDestroy(&bufStream);
	
	TPtr8 paramsPtr = paramsBuffer->Ptr(0);
	TInt paramsSize = paramsBuffer->Size();
	
	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TIpcArgs args(&ptr, &size, &paramsPtr, paramsSize);
	
	User::LeaveIfError(SendReceive(EPreviousInstances, args));
	
	CleanupStack::PopAndDestroy(paramsBuffer);
	
	RBufReadStream readStream;
	CleanupClosePushL(readStream);
	
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		CleanupStack::PushL(buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	
	const TInt KCount(readStream.ReadUint32L());

	for (TInt i(0) ; i < KCount; ++i)
		{
		TAgnInstance instance;
		readStream >> instance;
		aInstances.InsertL(i,instance);
		}

	if (buffer)
		{
		CleanupStack::PopAndDestroy(buffer);
		}
	CleanupStack::PopAndDestroy(&readStream);
	}

/** Finds the next instances in the calendar store

@param aParams The filter settings to use when finding the next instances
@param aInstances The next instances.  Note that the instances will be appended to the end of the array
 
@capability ReadUserData
*/
void RAgendaServ::NextInstancesL(const RArray<TInt64> aFileIds, CArrayFix<TAgnInstance>& aInstances, const TFindInstanceParams& aParams) const
	{
	// Write the find instance setting out to the buffer
	
	CBufFlat* paramsBuffer = CBufFlat::NewL(sizeof(TFindInstanceParams));
	CleanupStack::PushL(paramsBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*paramsBuffer);
	aParams.ExternalizeL(bufStream);
	bufStream.WriteInt16L(aFileIds.Count());
	for(TInt ii=0; ii<aFileIds.Count();++ii)
		{
		bufStream << aFileIds[ii];
		}
	CleanupStack::PopAndDestroy(&bufStream);
	
	TPtr8 paramsPtr = paramsBuffer->Ptr(0);
	
	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TIpcArgs args(&ptr, &size, &paramsPtr);
	
	User::LeaveIfError(SendReceive(ENextInstances, args));
	
	CleanupStack::PopAndDestroy(paramsBuffer);
	
	RBufReadStream readStream;
	CleanupClosePushL(readStream);
	
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		CleanupStack::PushL(buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	
	const TInt KCount(readStream.ReadUint32L());
	
	for (TInt i(0) ; i < KCount; ++i)
		{
		TAgnInstance instance;
		readStream >> instance;
		aInstances.AppendL(instance);
		}

	if (buffer)
		{
		CleanupStack::PopAndDestroy(buffer);
		}
	
	CleanupStack::PopAndDestroy(&readStream);
	}

// Entry Iterator Calls

/** 
Creates an iterator for iterating though all the entries in the file. 

This may be used for activities such as merging or finding. The function returns 
EFalse if no entries are available. If any entries are available, the iterator 
is set to the first one.

Although this API does not have a trailing L it can leave

@capability None
@return ETrue if there are entries in the file. EFalse if there are none. 
*/
TBool RAgendaServ::CreateEntryIteratorL(TInt64 aFileId)
	{
	// Creates a server side entry iterator
	TPckgBuf<TInt64> fileId(aFileId);
	TPckgBuf<TBool> ok;
	TIpcArgs args(&fileId, &ok);
	User::LeaveIfError(SendReceive(ECreateEntryIterator,args));
	return ok();
	}

/** 
Moves the entry iterator to the next entry. 

Returns EFalse if no more entries exist.

Although this API does not have a trailing L it can leave

@capability None
@return ETrue if there is are more entries. EFalse if there are no more entries. 
*/
TBool RAgendaServ::EntryIteratorNextL(TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TPckgBuf<TBool> ok;
	TIpcArgs args(&fileId, &ok);
	User::LeaveIfError(SendReceive(EEntryIteratorNext,args));
	return ok();
	}

/** 
Retrieves the ID for the entry at the current iterator position. 

The ID can be used to retrieve the entry using CAgnEntryModel::FetchEntryL().

@capability None
@return The ID for the entry at the current iterator position 
*/
CAgnEntry* RAgendaServ::FetchEntryByIteratorL(TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);

	TPckgBuf<TInt> size;
	TPtr8 ptr = iBuffer->Ptr(0);
	TIpcArgs args(&ptr, &size, &fileId);
	User::LeaveIfError(SendReceive(EEntryIteratorPosition,args));

	return ReadEntryFromServerSessionL(size());	
	}

/** 
Place the uids of entries that have a last changed data greater than aDate and which meet the selection
criteria specified in aFilter into the aUids array.
@capability None
*/
void RAgendaServ::GetEntryUidsSinceDateL(RArray<TCalLocalUid>& aUids, const TTime& aTime, TInt64 aFileId)
	{
	// package up filter size and filter 
	TPckgBuf<TTime> time(aTime);

	TPtr8 bufptr = iBuffer->Ptr(0);
	TPckg<TInt> size(0);
	TPckgBuf<TInt64> fileId(aFileId);
	// make server request
	TIpcArgs args(&bufptr, &size, &time, &fileId);
	User::LeaveIfError(SendReceive(EGetEntryUidsSinceDate, args));

	RBufReadStream readStream;
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	// internalize carrayfix from readStream
	const TInt KCount = readStream.ReadUint32L();
	TCalLocalUid id;
	for (TInt i = 0; i < KCount; i++)
		{
		readStream >> id;
		aUids.AppendL(id);
		}

	readStream.Close();
	CleanupStack::PopAndDestroy(buffer);
	}

/** 
Restore the notes text from the server
@capability ReadUserData
*/
HBufC* RAgendaServ::RestoreTextL(TStreamId aId, TInt64 aFileId)
	{
	__ASSERT_ALWAYS( (aId != KNullStreamId), Panic(EStreamIdError));

	TPckgBuf<TInt> size(0);
	TPckg<TStreamId> textId(aId);

	TPtr8 ptr(iBuffer->Ptr(0));
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&ptr, &textId, &size, &fileId);
	User::LeaveIfError(SendReceive(ERestoreText, args ));
	RBufReadStream readStream;
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	TInt textLength = readStream.ReadUint32L();
	HBufC* text = HBufC::NewL(readStream, textLength);
		
	CleanupStack::PopAndDestroy(buffer);
	return text;
	}

/** 
@capability ReadUserData
*/
CAgnContent* RAgendaServ::RestoreAlarmActionL(TStreamId aId, TInt64 aFileId)
	{
	__ASSERT_ALWAYS( (aId != KNullStreamId), Panic(EStreamIdError));

	TPckgBuf<TInt> size(0);
	TPckg<TStreamId> id(aId);
	TPckgBuf<TInt64> fileId(aFileId);
	
	TPtr8 ptr(iBuffer->Ptr(0));
	TIpcArgs args(&ptr, &id, &size, &fileId);
	User::LeaveIfError(SendReceive(ERestoreAlarmAction, args ));
	RBufReadStream readStream;
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	CAgnContent* alarmAction = new (ELeave) CAgnContent;
	CleanupStack::PushL(alarmAction);
	readStream >> *alarmAction;
	CleanupStack::Pop(alarmAction);

	CleanupStack::PopAndDestroy(buffer);
	
	return alarmAction;
	}


/**
Gets the number of allocated cells in the server space, debug only.
@capability None
*/	
TInt RAgendaServ::_DebugRequestAllocatedCellsL()
	{
	TPckg<TInt> numCells(0);	
	TIpcArgs args(&numCells);
 	User::LeaveIfError(SendReceive(EAgnResourceCount, args));
 	return numCells();
	}

/**
Gets the number of allocated cells in the server space, debug only.
@capability None
*/	
TInt RAgendaServ::_DebugRequestAllocatedHeapSizeL()
	{
	TPckg<TInt> heapSize(0);	
	TIpcArgs args(&heapSize);
 	User::LeaveIfError(SendReceive(EAgnHeapSizeCount, args));
 	return heapSize();
	}


/**
Set heap allocation failure according the arguments, debug only.
@param	aHeapAllocationFailureFlag	heap allocation failure flags (RAllocator::TAllocFail)
@param	aCount	Fail at allocation number aCount
@capability None
*/	
void RAgendaServ::_DebugSetHeapFailL(TInt aHeapAllocationFailureFlag, TInt aCount)
	{
	TIpcArgs args(aHeapAllocationFailureFlag, aCount);
	User::LeaveIfError(SendReceive(EAgnSetHeapFailure, args));
	}

//
//	Category access
//
//

/** 
Start building the Indexes asynchronously.
*/
void RAgendaServ::StartBuildIndex(TBool aNeedProgressReport, TRequestStatus& aStatus, TCalCollectionId aCallectionId)
	{
	TIpcArgs args(aNeedProgressReport, aCallectionId);
 	SendReceive(EStartBuildIndex, args, aStatus);
	}


/** 
Start tidying by ToDo list asynchronously.
@capability WriteUserData
@capability ReadUserData
*/
void RAgendaServ::StartTidyByDateL(TBool aReportProgress, TRequestStatus& aStatus,
								   const TAgnFilter& aFilter,
								   const TTime& aTodaysDate,
								   const CalCommon::TCalTimeRange& aCalTimeRange,
								   TCalCollectionId aCallectionId)
	{
	// Externalize parameters
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	aFilter.ExternalizeL(bufStream);
	TPckgBuf<TTime> timePckgBuf;
	timePckgBuf = aTodaysDate;
	bufStream.WriteL(timePckgBuf);
	timePckgBuf = aCalTimeRange.StartTime().TimeLocalL();
	bufStream.WriteL(timePckgBuf);
	timePckgBuf = aCalTimeRange.EndTime().TimeLocalL();
	bufStream.WriteL(timePckgBuf);
	bufStream.CommitL();
	bufStream.Close();
	TPtr8 parameterPtr = iBuffer->Ptr(0);
	
	// Pass the parameters to the agenda model server but don't start the
	// tidy operation.
	TIpcArgs args(&parameterPtr, aCallectionId);
 	SendReceive(ETidyByDateReadParams, args);

	// Start the tidy operation and notify us asynchronously when it is
	// complete
	TIpcArgs args1(aReportProgress, aCallectionId);
 	SendReceive(ETidyByDateStart, args1, aStatus);
	}

void RAgendaServ::RequestProgress(TRequestStatus& aStatus, TCalCollectionId aCallectionId)
	{
	TIpcArgs args(aCallectionId);
 	SendReceive(ERequestProgress, args, aStatus);
	}
/** 
returns the number of categories in the category list
@capability None
*/
TInt RAgendaServ::CategoryCountL(TInt64 aFileId)
	{
	TPckgBuf<TInt> count;
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&count, &fileId);
	User::LeaveIfError(SendReceive(EGetCategoryListCount, args));
	
	return count();
	}

/** 
returns a category created from the category at aIndex in the category list
@capability ReadUserData
@param aIndex, this is the index of the category in the category list
*/
CAgnCategory* RAgendaServ::CategoryL(TInt aIndex, TInt64 aFileId)
	{
	TPtr8 ptr(iBuffer->Ptr(0));
	TPckgBuf<TInt> size;
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&ptr, aIndex, &size, &fileId);
	User::LeaveIfError(SendReceive(EGetCategoryListItem, args));

	RBufReadStream readStream;
	// Create a transfer buffer, and set where to read the data from
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	CAgnCategory* category = CAgnCategory::NewL(readStream);
	CleanupStack::PopAndDestroy(buffer);
	
	return category;
	}

/** 
adds a category to the list 
@capability WriteUserData
@param aName, this is the name of the new category that we want to add to the category list
*/
void RAgendaServ::AddCategoryToListL(const TDesC& aName, TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&aName, &fileId);
	User::LeaveIfError(SendReceive(EAddCategoryToList, args));
	}

/** 
package up step size and category
@capability None
*/
void RAgendaServ::FilterCategoryL(CArrayFixSeg<TAgnEntryId>& aEntries, TInt64 aFileId)
	{
	TPckgBuf<TInt> size(0);
	TPtr8 ptr(iBuffer->Ptr(0));
	TPckgBuf<TInt64> fileId(aFileId);
	
	TIpcArgs args(&ptr, &size, &fileId);
	User::LeaveIfError(SendReceive(ECategoryFilter, args));

	RBufReadStream readStream;
	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);								
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	const TInt numberEntries = readStream.ReadInt16L();
	for(TInt a = 0; a < numberEntries; ++a)
		{
		TAgnEntryId entry;
		entry.InternalizeL(readStream);
		aEntries.AppendL(entry);
		}

	CleanupStack::PopAndDestroy(buffer);
	}

/**
@capability WriteUserData
*/
void RAgendaServ::StartCategoryTaskL(TBool aReportProgress, TRequestStatus& aStatus, const CAgnCategory& aCategory, CCalAsyncTaskManager::TAsyncAction aTask, TCalCollectionId aCallectionId)
	{
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);
	aCategory.ExternalizeL(bufStream);

	bufStream.CommitL();
	CleanupStack::Pop(&bufStream);
	TPtr8 ptr(iBuffer->Ptr(0));
	TIpcArgs args(&ptr, iBuffer->Size(), aTask, aCallectionId);
	User::LeaveIfError(SendReceive(ECategoryStart, args));
	TIpcArgs args1(aReportProgress, aTask, aCallectionId);
	SendReceive(ECategoryStartAsyn, args1, aStatus);
	}


void RAgendaServ::FilterCategorySynchronouslyL(const CAgnCategory& aCategory, TCalCollectionId aCallectionId)
	{
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);
	aCategory.ExternalizeL(bufStream);

	bufStream.CommitL();
	CleanupStack::Pop(&bufStream);
	
	TPtr8 ptr(iBuffer->Ptr(0));
	TIpcArgs args(&ptr, iBuffer->Size(), CCalAsyncTaskManager::EFilterCategory, aCallectionId);
	User::LeaveIfError(SendReceive(ECategoryStart, args));
	TRequestStatus status = KRequestPending;
	TIpcArgs args1(EFalse, CCalAsyncTaskManager::EFilterCategory, aCallectionId);
	SendReceive(ECategoryStartAsyn, args1, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

/**
@capability None
*/
void RAgendaServ::CancelTask(TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&fileId);
	SendReceive(ECancelTask, args);
	}

/**
@capability WriteUserData
*/
void RAgendaServ::CreateAgendaFileL(const TDesC& aFileName)
	{
	TIpcArgs args(&aFileName);
	User::LeaveIfError(SendReceive(ECreateAgendaFile,args));
	}

/**
@capability WriteUserData
*/
void RAgendaServ::DeleteAgendaFileL(const TDesC& aFileName, TInt aSessionId)
	{
	TIpcArgs args(&aFileName,aSessionId);
	User::LeaveIfError(SendReceive(EDeleteAgendaFile,args));
	}

CDesCArray* RAgendaServ::ListAgendaFilesL()
	{
	// Get a list of file names held by the server 
	// currently setup extractor
	// Argumnents: 0 : Data buffer 
	//			   1 : Size of buffer

	//create the buffer
	TPtr8 ptr = iBuffer->Ptr(0);

	TPckgBuf<TInt> size(0);
	TIpcArgs args(&ptr, &size);
	User::LeaveIfError(SendReceive(EGetListFileNames,args));
	RBufReadStream readStream;

	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else 
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	const TInt count = readStream.ReadUint8L();
	CDesCArray* fileNames=NULL;
	if(count!=0)
		{
		fileNames = new(ELeave) CDesCArrayFlat(count);
		CleanupStack::PushL(fileNames);
		for (TInt i=0; i<count; ++i)
			{
			TFileName fileName;
			TInt length=readStream.ReadInt8L();		
			readStream.ReadL(fileName,length);
			fileNames->AppendL(fileName);
			}

		CleanupStack::Pop(fileNames);
		}
	readStream.Close();

	CleanupStack::PopAndDestroy(buffer);
	return fileNames;
	}

TBool RAgendaServ::AgendaFileExistsL(const TDesC& aFileName)
	{
	TPckgBuf<TBool> isFileThere;
	TIpcArgs args(&isFileThere,&aFileName);
	User::LeaveIfError(SendReceive(EAgendaFileExists,args));
	return isFileThere();
	}

TInt RAgendaServ::SetUpdateAlarm(TBool aUpdateAlarm, TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(aUpdateAlarm, &fileId);
	return SendReceive(ESetUpdateAlarm,args); 
	}

void RAgendaServ::SetEnablePubSubNotificationsL(TBool aEnablePubSubNotification, TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(aEnablePubSubNotification, &fileId);
	User::LeaveIfError(SendReceive(ESetEnablePubSubNotification,args));
	}

void RAgendaServ::SetChangeNotificationParametersL(MCalChangeCallBack2::TChangeEntryType aEntryType, TBool aIncludeUndatedTodos, TTime aFilterStartTimeUtc, TTime aFilterEndTimeUtc, TInt64 aFileId)
    {
	CBufFlat* paramsBuffer = CBufFlat::NewL(sizeof(TInt64) + sizeof(TBool) + sizeof(MCalChangeCallBack2::TChangeEntryType));
	CleanupStack::PushL(paramsBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*paramsBuffer);
	bufStream.WriteInt16L(aEntryType);
	bufStream.WriteInt16L(aIncludeUndatedTodos);
	bufStream << aFileId;
	CleanupStack::PopAndDestroy(&bufStream);
	
	TPtr8 paramsPtr = paramsBuffer->Ptr(0);

	TPckg<TTime> start(aFilterStartTimeUtc);
	TPckg<TTime> end(aFilterEndTimeUtc);
	TIpcArgs args(&start, &end, &paramsPtr);
    User::LeaveIfError(SendReceive(ERequestChangeNotificationParameters, args));
	CleanupStack::PopAndDestroy(paramsBuffer);
    }

void RAgendaServ::RequestChangeNotification(TRequestStatus& aStatus, TInt aSessionId, TUint8 aNotificationType)
	{
	TIpcArgs args(aSessionId, aNotificationType);
	SendReceive(ERequestChangeNotification, args, aStatus);
	}

void RAgendaServ::CancelChangeNotification(TInt aSessionId, TUint8 aNotificationType)
	{
	TIpcArgs args(aSessionId, aNotificationType);
    SendReceive(ECancelChangeNotification, args);
	}

// Prevent notification of changes made to the agenda model from this
// client from being broadcast to other clients.
void RAgendaServ::DisableChangeBroadcast(TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&fileId);
    SendReceive(EDisableChangeBroadcast, args);	
	}

// Allow notification of changes made to the agenda model from this
// client from being broadcast to other clients.
void RAgendaServ::EnableChangeBroadcast(TInt64 aFileId)
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&fileId);
    SendReceive(EEnableChangeBroadcast, args);
	}

void RAgendaServ::GetFileChangesSinceLastNotificationL(RPointerArray<CAgnFileChangeInfo>& aCalFileChangeInfo, TInt aSessionId)
    {
    TPtr8 ptr = iBuffer->Ptr(0);
    TPckgBuf<TInt> size(0);
    TIpcArgs args(&ptr, &size, aSessionId);
    User::LeaveIfError(SendReceive(EGetFileChangesSinceLastNotification,args));
    RBufReadStream readStream;
    CleanupClosePushL(readStream);
    CBufFlat* buffer = CreateTransmitBufferL(size());
    if (buffer)
        {
        readStream.Open(*buffer);
        }
    else
        {
        readStream.Open(*iBuffer);
        }
    CleanupStack::PushL(buffer);
    const TInt count = readStream.ReadInt32L();
    aCalFileChangeInfo.ReserveL(aCalFileChangeInfo.Count() + count);
    for (TInt ii = 0; ii < count; ++ii)
        {       
        CAgnFileChangeInfo* info = CAgnFileChangeInfo::NewL(readStream);
        aCalFileChangeInfo.Append(info);
        }

    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PopAndDestroy(&readStream);
    }

/**
@capability ReadUserData
*/
void RAgendaServ::GetChangesSinceLastNotificationL(RArray<TCalChangeEntry>& aChangeItems, TCalCollectionId aCollectionId, TCalFileId& aFileId )
	{
	TPckgBuf<TCalFileId> fileId(aFileId);
	TPtr8 ptr = iBuffer->Ptr(0);
	TPckgBuf<TInt> size(0);
	TIpcArgs args(&ptr, &size, aCollectionId, &fileId);
	User::LeaveIfError(SendReceive(EGetChangesSinceLastNotification,args));
	RBufReadStream readStream;
    aFileId = fileId();
  	const TInt KBufferSize = size();
	CBufFlat* buffer = CreateTransmitBufferL(KBufferSize);
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}

	CleanupStack::PushL(buffer);
	
	// The buffer contains a number of change item (6 bytes in size) and nothing else.
	// Check that the buffer is divisible by the size of a change item.
	const TInt KSizeOfChangeItem = 6;
	if (KBufferSize % KSizeOfChangeItem == 0)
		{
		const TInt KItems = KBufferSize / KSizeOfChangeItem;

		TCalChangeEntry changeItem;
		for (TInt count=0; count < KItems; count++)
			{
			changeItem.iEntryId = readStream.ReadUint32L();
			changeItem.iChangeType = (MCalChangeCallBack2::TChangeType)readStream.ReadUint8L();
			CCalEntry::TType type = static_cast<CCalEntry::TType>(readStream.ReadUint8L());
			if (changeItem.iChangeType == MCalChangeCallBack2::EChangeUndefined)
				{
				changeItem.iEntryType = MCalChangeCallBack2::EChangeEntryAll;
				}
			else
				{
				if (type == CCalEntry::ETodo)
					{
					changeItem.iEntryType =
							MCalChangeCallBack2::EChangeEntryTodo;
					}
				else if (type == CCalEntry::ENote)
					{
					changeItem.iEntryType =
							MCalChangeCallBack2::EChangeEntryNote;
					}
				else
					{
					changeItem.iEntryType =
							MCalChangeCallBack2::EChangeEntryEvent;
					}
				}
			aChangeItems.AppendL(changeItem);
			}
		}
	else
		{
		// If the buffer is corrupt, return a dummy item to say that something has changed
		TCalChangeEntry changeItem;
		changeItem.iEntryId = 0;
		changeItem.iChangeType = MCalChangeCallBack2::EChangeUndefined;
		changeItem.iEntryType = MCalChangeCallBack2::EChangeEntryAll;
		aChangeItems.AppendL(changeItem);
		}

	readStream.Close();

	CleanupStack::PopAndDestroy(buffer);
	}

/**
@capability ReadUserData
*/
HBufC8* RAgendaServ::GetPropertyValueL(const TDesC& aFilename, TStreamId aStreamId) const
    {
    TPtr8 bufptr = iBuffer->Ptr(0);
    TPckg<TInt> size(0);
    TPckgC<TStreamId> streamId(aStreamId);
    
    // make server request
    TIpcArgs args(&bufptr, &size, &aFilename, &streamId);
    
    User::LeaveIfError(SendReceive(EGetPropertyValue, args));

    RBufReadStream readStream;
    CBufFlat* buffer = CreateTransmitBufferL(size());
    if (buffer)
        {
        readStream.Open(*buffer);
        }
    else
        {
        readStream.Open(*iBuffer);
        }
    CleanupStack::PushL(buffer);
    
    // internalize calendar info from readStream
    HBufC8* value = HBufC8::NewL(readStream, KMaxTInt);

    readStream.Close();
    CleanupStack::PopAndDestroy(buffer);
    
    return value;
    }

/**
@capability WriteUserData
*/
void RAgendaServ::SetCalendarInfoL(const TDesC& aFilename, const CCalCalendarInfoImpl& aCalendarInfoImpl) const
    {
    CBufFlat* paramsBuffer = CBufFlat::NewL(100);
    CleanupStack::PushL(paramsBuffer);
    RBufWriteStream bufStream;
    CleanupClosePushL(bufStream);
    bufStream.Open(*paramsBuffer);
    aCalendarInfoImpl.IpcExternalizeL(bufStream);
    CleanupStack::PopAndDestroy(&bufStream);
    
    TPtr8 paramsPtr = paramsBuffer->Ptr(0);

    TIpcArgs args(&aFilename, &paramsPtr);
    TInt error = SendReceive(ESetCalendarInfo, args);
    User::LeaveIfError(error);
    CleanupStack::PopAndDestroy(paramsBuffer);
    }

/**
@capability ReadUserData
*/
CCalCalendarInfoImpl* RAgendaServ::GetCalendarInfoLC(const TDesC& aFilename, CCalSessionImpl& aSessionImpl) const
    {
    TPtr8 bufptr = iBuffer->Ptr(0);
    TPckg<TInt> size(0);
    
    // make server request
    TIpcArgs args(&bufptr, &size, &aFilename);
    
    User::LeaveIfError(SendReceive(EGetCalendarInfo, args));

    RBufReadStream readStream;
    CBufFlat* buffer = CreateTransmitBufferL(size());
    if (buffer)
        {
        readStream.Open(*buffer);
        }
    else
        {
        readStream.Open(*iBuffer);
        }
    CleanupStack::PushL(buffer);
    
    // internalize calendar info from readStream
    CCalCalendarInfoImpl* info(CCalCalendarInfoImpl::NewL(aSessionImpl));
    CleanupStack::PushL(info);
    info->IpcInternalizeL(readStream);

    readStream.Close();
    CleanupStack::Pop(info);
    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PushL(info);
    return info;
    }

/**
@capability WriteUserData
*/
void RAgendaServ::DeleteEntryL(TAgnEntryId aEntryId, TCalCollectionId aCallectionId)
	{
	TPckg<TAgnEntryId> entryId(aEntryId);
	TIpcArgs args(&entryId, aCallectionId);
	User::LeaveIfError(SendReceive(EDeleteEntry,args));
	}

/**
@capability ReadUserData
*/
void RAgendaServ::FetchEntryByGuidL(const TDesC8& aGuid, RPointerArray<CAgnEntry>& aList, TInt64 aFileId) const
	{
	// Get all entries which have the same GUID 
	// currently setup extractor
	// Argumnents: 0 : Data buffer 
	//			   1 : Size of buffer
	//			   2 : GUID

	//create the buffer
	TPtr8 ptr = iBuffer->Ptr(0);

	TPckgBuf<TInt> size(0);
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&ptr, &size, &aGuid, &fileId);
	User::LeaveIfError(SendReceive(EFetchEntryByGuid,args));
	RBufReadStream readStream;

	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else 
		{
		readStream.Open(*iBuffer);
		}
	CleanupStack::PushL(buffer);

	const TInt KCount = readStream.ReadUint32L();
	
	for (TInt i = 0; i < KCount; ++i)
		{
		CAgnEntry* entry = CreateEntryFromBufferStreamL(readStream);
		CleanupStack::PushL(entry);
		aList.AppendL(entry);
		CleanupStack::Pop(entry);
		}

	readStream.Close();

	CleanupStack::PopAndDestroy(buffer);
	}

/**
@capability WriteUserData
*/
void RAgendaServ::DeleteEntryL(const RArray<TCalLocalUid>& aIds, TCalCollectionId aCallectionId)
	{
	// Create a write stream for this buffer
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);
	const TInt KCount = aIds.Count();
	writeStream.WriteUint32L(KCount);
		for (TInt ii = 0; ii < KCount; ++ii)
		{
		writeStream.WriteUint32L(aIds[ii]);
		}

	writeStream.CommitL();
	writeStream.Close();
	
	TInt size(iBuffer->Size());
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TIpcArgs args(size, &ptr, aCallectionId);
	User::LeaveIfError(SendReceive(EDeleteEntriesByLocalUid,args));
	}

/**
@capability WriteUserData
*/
void RAgendaServ::DeleteEntryByGuidL(const TDesC8& aGuid, TBool aCommitAndNotify, TInt64 aFileId) 
	{
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&aGuid, aCommitAndNotify, &fileId);
	User::LeaveIfError(SendReceive(EDeleteEntryByGuid, args));
	}

/**
@capability ReadUserData
*/
void RAgendaServ::FindInstancesL(RArray<TInt64>& aFileIds, CArrayFix<TAgnInstance>& aMatchedInstanceList, const TFindInstanceParams& aParams)
	{
	// Write the find instance setting out to the buffer
	CBufFlat* paramsBuffer = CBufFlat::NewL(sizeof(TFindInstanceParams));
	CleanupStack::PushL(paramsBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*paramsBuffer);
	aParams.ExternalizeL(bufStream);
	bufStream.WriteInt16L(aFileIds.Count());
	for(TInt ii=0; ii<aFileIds.Count();++ii)
		{
		bufStream << aFileIds[ii];
		}
	CleanupStack::PopAndDestroy(&bufStream);
	
	TPtr8 paramsPtr = paramsBuffer->Ptr(0);
	
	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TIpcArgs args(&ptr, &size, &paramsPtr);
	User::LeaveIfError(SendReceive(EFindInstances,args));
	
	CleanupStack::PopAndDestroy(paramsBuffer);
	
	RBufReadStream readStream;

	CBufFlat* buffer = CreateTransmitBufferL(size());
	if (buffer)
		{
		readStream.Open(*buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}

	if (buffer)
		{
		CleanupStack::PushL(buffer);
		}

	TInt items = readStream.ReadUint32L();

	for (TInt count=0; count<items; count++)
		{
		TAgnInstance aInstance;
		readStream >> aInstance;

		aMatchedInstanceList.AppendL(aInstance);
		}

	readStream.Close();

	if (buffer)
		{
		CleanupStack::PopAndDestroy(buffer);
		}
	}

/**
@capability WriteUserData
*/
void RAgendaServ::CommitL(TCalCollectionId aCallectionId)
	{
	TIpcArgs args(aCallectionId);
	User::LeaveIfError(SendReceive(ECommit, args));
	}
/**
@capability WriteUserData
*/
void RAgendaServ::Rollback(TCalCollectionId aCallectionId)
	{
	TIpcArgs args(aCallectionId);
	SendReceive(ERollback, args);
	}
/** Get the last modified time for tz rules from time zone server
	It is called when the client getting the last modified date for an entry since the 
	last modified time of an entry is not updated when tz database is updated.
@capability none
*/
TTime RAgendaServ::TzRulesLastModifiedDateL(TInt64 aFileId)
	{
	TPckgBuf<TTime> time;
	TPckgBuf<TInt64> fileId(aFileId);
	TIpcArgs args(&time, &fileId);
	User::LeaveIfError(SendReceive(ETzDbChangedTime, args));
	return time();
	}

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
/**
 * Create instance iterator with given find instance settings and sort
 * criteria
@capability ReadUserData
*/
TInt RAgendaServ::CreateInstanceIteratorL(const TFindInstanceParams& aFindParams, const CCalSortCriteria& aSortCriteria, RArray<TInt64>& aFileIds)
	{
	//Store find instance parameter into buffer
	CBufFlat* paramsBuffer = CBufFlat::NewL(sizeof(TFindInstanceParams));
	CleanupStack::PushL(paramsBuffer);
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*paramsBuffer);
	aFindParams.ExternalizeL(bufStream);
	bufStream.CommitL();
	CleanupStack::PopAndDestroy(&bufStream);
	
	TPtr8 paramsPtr = paramsBuffer->Ptr(0);
	
	//Store sort criteria into buffer
	RBufWriteStream sortCriteriaBufStream;
	sortCriteriaBufStream.Open(*iBuffer);
	CleanupClosePushL(sortCriteriaBufStream);
	aSortCriteria.SortCriteria().ExternalizeL(sortCriteriaBufStream);
	sortCriteriaBufStream.WriteInt16L(aFileIds.Count());
	const TInt count = aFileIds.Count();
	for(TInt ii=0; ii<count; ++ii)
		{
		sortCriteriaBufStream << aFileIds[ii];
		}

    sortCriteriaBufStream.CommitL();
	CleanupStack::PopAndDestroy(&sortCriteriaBufStream);

	// Get the sizes of the buffers
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TPckgBuf<TInt> iteratorId(KErrNotFound);
	TIpcArgs args(&paramsPtr, &ptr, &iteratorId);
	
	User::LeaveIfError(SendReceive(EInstanceIteratorCreate, args));
	
	CleanupStack::PopAndDestroy(paramsBuffer);
	
	return iteratorId();
	}
#endif

/**
 * Destroy instance iterator with specified iterator id
@capability ReadUserData
*/
void RAgendaServ::DestroyInstanceIterator(TInt aIteratorId)
	{
	TIpcArgs args(aIteratorId);
	
	//It's not neccessary/possible to check if the operation complete successfully
	//because the function may be called at application exiting time.
	Send(EInstanceIteratorDestroy, args);
	}
	
/**
 * Fetch the available instances start from specified index 
@capability ReadUserData
*/
void RAgendaServ::InstanceIteratorNextL(TInt aIteratorId, CArrayFix<TAgnInstance>& aInstances, TInt aIndex) const
	{
	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TIpcArgs args(&ptr, &size, aIteratorId, aIndex);
	User::LeaveIfError(SendReceive(EInstanceIteratorNext, args));

	LoadingInstancesFromBufferL(aInstances, size());
	}
	
/**
 * Fetch the previous instances which index are before given index  
@capability ReadUserData
*/
void RAgendaServ::InstanceIteratorPreviousL(TInt aIteratorId, CArrayFix<TAgnInstance>& aInstances, TInt aIndex) const
	{
	TPckgBuf<TInt> size(0);
	TPtr8 ptr = iBuffer->Ptr(0);
	
	TIpcArgs args(&ptr, &size, aIteratorId, aIndex);
	User::LeaveIfError(SendReceive(EInstanceIteratorPrevious, args));
	
	LoadingInstancesFromBufferL(aInstances, size());
	}
	
void RAgendaServ::LoadingInstancesFromBufferL(CArrayFix<TAgnInstance>& aInstances, TInt aBufSize) const
	{
	RBufReadStream readStream;
	CleanupClosePushL(readStream);
	
	CBufFlat* buffer = CreateTransmitBufferL(aBufSize);
	if (buffer)
		{
		readStream.Open(*buffer);
		CleanupStack::PushL(buffer);
		}
	else
		{
		readStream.Open(*iBuffer);
		}
	
	const TInt KCount(readStream.ReadUint32L());
	
	for (TInt i(0) ; i < KCount; ++i)
		{
		TAgnInstance instanceId;
		instanceId.InternalizeL(readStream);
		aInstances.AppendL(instanceId);
		}

	if (buffer)
		{
		CleanupStack::PopAndDestroy(buffer);
		}
	
	CleanupStack::PopAndDestroy(&readStream);
	}

/**
 * Get the number of instances stored in an iterator
@capability ReadUserData
*/
TInt RAgendaServ::InstanceIteratorCountL(TInt aIteratorId) const
	{
	TPckgBuf<TInt> count(0);
	TIpcArgs args(aIteratorId, &count);
	User::LeaveIfError(SendReceive(EInstanceIteratorCount, args));
	return count();
	}

/**
 * Find the index of the given instance in the iterator. 
@capability ReadUserData
*/
TInt RAgendaServ::InstanceIteratorLocateIndexL(TInt aIteratorId, TAgnInstance& aInstanceId)
	{
	TPckgBuf<TInt> index(0);
	TPckg<TAgnInstance> instanceId(aInstanceId);	
	TIpcArgs args(aIteratorId, &instanceId, &index);
	User::LeaveIfError(SendReceive(EInstanceIteratorLocateIndex, args));
	return index();
	}

void RAgendaServ::DeleteSimpleEntry(CAgnSimpleEntry* aSimpleEntry)
/** Deletes a sort entry from the sort entry allocator (a specialised memory handler 
for sort entry objects). 

A sort entry is an entry in an agenda index. The function frees the sort entry object 
and puts it on the free list.

@param aSimpleEntry Pointer to the simple entry to delete. */
	{
	iSimpleEntryAllocator->Delete(aSimpleEntry);
	}

CAgnSimpleEntry* RAgendaServ::CreateDumySimpleEntryL()
	{
	return iSimpleEntryAllocator->CreateSimpleEntryL(CCalEntry::EAppt);
	}

CAgnSimpleEntryAllocator& RAgendaServ::SimpleEntryAllocator() const
	{
	return *iSimpleEntryAllocator;
	}

/**
@capability none
*/
void RAgendaServ::__dbgClearTzClientCacheL(TBool aRestartCaching)
	{
	CAgnTlsProxy* agntlsproxy = CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_Client);
	agntlsproxy->__dbgClearTzClientCacheL(aRestartCaching);
	CAgnTlsProxy::Release(agntlsproxy);
	}


//Cleanup Callback function which deletes CAgnSortEntry * using CAgnSorEntryAllocator.
void TCleanSimpleEntry::DestroySimpleEntry(TAny* aPtr)
	{
	TCleanSimpleEntry* cleanSimpleEntry = static_cast<TCleanSimpleEntry*> (aPtr);
	cleanSimpleEntry->GetSimpleEntryAllocator().Delete(cleanSimpleEntry->GetSimpleEntry());
	}

TCleanSimpleEntry::TCleanSimpleEntry(CAgnSimpleEntry& aSimpleEntry, CAgnSimpleEntryAllocator& aAlloc)
				: iSimpleEntry(aSimpleEntry),iAlloc(aAlloc)
	{ 
	}

CAgnSimpleEntry* TCleanSimpleEntry::GetSimpleEntry()
	{
	return &iSimpleEntry;	
	}

CAgnSimpleEntryAllocator& TCleanSimpleEntry::GetSimpleEntryAllocator()
	{
	return iAlloc;	
	}


