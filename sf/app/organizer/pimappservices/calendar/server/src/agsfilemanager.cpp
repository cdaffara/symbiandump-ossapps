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

#include "agsfilemanager.h"

#include "agsasyncdelete.h"
#include "agscategoryindex.h"
#include "agscategorylist.h"
#include "agsentrymanager.h"
#include "agsfileconverter.h"
#include "agsfilesearch.h"
#include "agsentrymodel.h"
#include "agsattachmentindex.h"
#include "agmutil.h"
#include "agsmain.h"
#include "agspermanentdata.h"
#include "agssess.h"
#include "calcommonimpl.h"
#include "agmdebug.h"
#include "agstzruleindex.h"
#include "agmcalendarinfo.h"

#include <apparc.h>
#include <s32file.h>

const TInt KAgnCompactionThresholdInBytes=10240;

//
// CAgnServFile
//
 
// Make sure the active object has the same priority as the server
// If it's higher, the client will be blocked when the server
// does an extended operation. If lower, the server won't ever
// get around to the extended tasks, unless it is left alone by
// all clients.
CAgnServFile::CAgnServFile(RFs& aFs, CAgnServer& aAgnServer) :
	CActive(CActive::EPriorityStandard),
	iAgnServer(aAgnServer),
	iFs(aFs),
	iActiveStep(CCalAsyncTaskManager::ENoAction),
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iShutdownNotification(EFalse),
#endif
	iIsFileDisabled(EFalse)
	{
	CActiveScheduler::Add(this);
	iRefCount = 0;
	}


CAgnServFile* CAgnServFile::NewL(RFs& aFs, CAgnServer& aAgnServer)
	{
	CAgnServFile* self = new(ELeave)CAgnServFile(aFs, aAgnServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAgnServFile::ConstructL()
	{
	const TInt KMessageVectorGranularity = 4; // unlikely to be 4 messages simultaneously
	iMessageVector = new (ELeave) CArrayFixFlat<TAgnMessageToComplete> (KMessageVectorGranularity);
	iFileShutdownDelayTimer = CAgnServFileShutdownDelayTimer::NewL(*this);
	}

CAgnServFile::~CAgnServFile()
	{
	CloseAgendaImmediately();
	
	delete iFileName;
	delete iMessageVector;
	delete iCategoryList;
	delete iAsyncDelete;
	delete iFileShutdownDelayTimer;
	}
	
CAgnTlsProxy* CAgnServFile::TimeZoneConverter() const
	{
	return iAgnServer.TimeZoneConverter();
	}


void CAgnServFile::Start()
//
// Set the object to the active state and make a request
//
	{
	if ( ! IsActive())
		{
		SetActiveAndMakeRequest();
		}
	}

void CAgnServFile::RunL()
	{
	if (iActiveStep == CCalAsyncTaskManager::ENoAction)
		{
		return;
		}

	TBool completed = EFalse;
	TRAPD(err, completed = DoStepL());
	if (err != KErrNone )
		{
		DoTaskCompleteL(err, iSession);
		}
	else if ( ! completed)
		{
		// The current step is not complete, reschedule the active object
		// do some more work.
		SetActiveAndMakeRequest();
		}
	}

TBool CAgnServFile::HasServerSession() const
	{
	return (iSession != NULL);
	}

const TDesC& CAgnServFile::PrivatePath() const
	{
	if (iAgnServer.FileMgr())
		{
		return iAgnServer.FileMgr()->PrivatePath();
		}
	return KNullDesC();
	}

void CAgnServFile::DoCancel()
	{
	// do nothing
	}


void CAgnServFile::SetActiveAndMakeRequest()
//
// Set the object to the active state and make a request
//
	{
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}

const TDesC& CAgnServFile::FileName() const
	{
	if (iFileName)
		{
		return *iFileName;
		}
	return KNullDesC();
	}

void CAgnServFile::GetAttachmentFolderNameL(TDes& aFolderName)
	{
	GetAttachmentFolderNameL(FileName(), aFolderName);
	}
	
void CAgnServFile::GetAttachmentFolderNameL(const TDesC& aFileName, TDes& aFolderName)
	{
	// extension added to calender file name to give folder name for storing attachments
	_LIT(KCalAttachmentsExtension, "_a");
	_LIT(KCalDirectory, "\\");

	__ASSERT_ALWAYS(aFileName.Length() + KCalAttachmentsExtension().Length() + KCalDirectory().Length() <= aFolderName.MaxLength(), User::Leave(KErrArgument));

	aFolderName.Copy(aFileName);
	aFolderName.Append(KCalAttachmentsExtension);
	aFolderName.Append(KCalDirectory);
	}

void CAgnServFile::OpenAgendaL(const TDesC& aFilename, CalCommon::TCalFileVersionSupport& aStatus)
	{
	_DBGLOG_BASIC(AgmDebug::DebugLog("Opening Calendar File - '%S'",&aFilename);)

	if(aFilename.Length() > KMaxFileName)
		{
		_DBGLOG_BASIC(AgmDebug::DebugLog("KErrArgument:  Calendar filename length is greater than max filename length permitted");)
		User::Leave(KErrArgument);
		}

	// Check the file exists
	if (FileExistsL(aFilename) == EFalse)
		{
		_DBGLOG_BASIC(AgmDebug::DebugLog("KErrNotFound: Filename - %S not found", &aFilename);)
		User::Leave(KErrNotFound);
		}
	
	// If the file is in ROM, should leave
	if (aFilename.Length() > 2)
		{
		TDriveName driveName;
		driveName.Append(aFilename[0]);
		driveName.UpperCase();
		TDriveUnit drive(driveName);
		TDriveInfo driveInfo;
		User::LeaveIfError(iFs.Drive(driveInfo, drive));
		if (driveInfo.iDriveAtt == KDriveAttRom)
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("KErrNotSupported: The file is in ROM. This is not supported");)
			User::Leave(KErrNotSupported);
			}
		}
	
	iFileName = aFilename.AllocL();

	iReadOnly = FileIsReadOnlyL(FileName());
	CreateFileStoreL(aFilename);
	
	// Check that the file version is compatible
	// with the entry model's version
	TAgnVersion version;
	GetFileVersionSupportStatusL(version, aStatus);
	
	if(aStatus == CalCommon::EUnsupportedFileVersion)
		{
		User::Leave(KErrNotSupported);
		}
	
	// Create a model for this file
	CreateModelForFileL();
	}

void CAgnServFile::ReopenAgendaAfterRestoreL()
	{
	//We only build index if it was built before restore started.
	TBool toBuiildIndex = iIndexesBuilt;
	CreateFileStoreL(*iFileName);//iIndexesBuilt is set to EFalse in this function
	CreateModelForFileL();
	if (toBuiildIndex)
		{
		while(iIndexesBuilt ==0)
			{
			DoBuildIndexStepL();
			}
		TRAPD(err, iModel->BuildIndexCompleteL());
		if(err != KErrNone && err != KErrServerBusy)
			{
			User::Leave(err);
			}
		iModel->CommitL();
		}
	Model()->CheckTzDbModificationL();
	if(RefreshTzRules())
		{
		TPckgBuf<NTzUpdate::TTzRulesChange> pubSubBuf;
		if(KErrNone == RProperty::Get(NTzUpdate::KPropertyCategory, NTzUpdate::ETzRulesChange, pubSubBuf))
			{
			TTime tzChange = pubSubBuf().iUTCTimeOfRulesChange;
			Model()->HandleTzRulesChangeL(tzChange);
			}

		SetRefreshTzRules(EFalse);
		}
	}

void CAgnServFile::CreateFileStoreL(const TDesC& aFilename)
	{
	CFileStore* store = NULL;
	iDictionary = CApaProcess::ReadRootStreamLC(iFs, store, aFilename, EFileRead);
	CleanupStack::Pop(); // dictionary
	CleanupStack::PushL(store);
	iModelStreamId = iDictionary->At(KUidAgnModel);
	if (iModelStreamId == KNullStreamId)
		{
		_DBGLOG_BASIC(AgmDebug::DebugLog("KErrCorrupt: Corrupt model stream");)
		User::Leave(KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(store);

	// Create the permanent file store
	//      
	if (iReadOnly)
		{
		iStore =  CPermanentFileStore::OpenL(iFs,aFilename,EFileShareAny|EFileRead);
		}
	else
		{
		iStore =  CPermanentFileStore::OpenL(iFs,aFilename,EFileShareAny|EFileRead|EFileWrite);
		}

	CAgnCalendarInfo* info(NULL);
	TStreamId calendarInfoStreamId = iDictionary->At(KUidAgnCalendarInfo);

	if (calendarInfoStreamId != KNullStreamId)
		{
		// Calendar info has been set on this file
		RStoreReadStream readStream;
		readStream.OpenLC(*iStore, calendarInfoStreamId);
		info = CAgnCalendarInfo::NewL();
		CleanupStack::PushL(info);
		info->FileInternalizeL(readStream);
		info->SetIsValid(ETrue);
		iIsFileDisabled = (!info->Enabled());
		CleanupStack::PopAndDestroy(info);
		CleanupStack::PopAndDestroy(&readStream);
		}
	}

void CAgnServFile::GetFileVersionSupportStatusL(TAgnVersion& aFileVersion, CalCommon::TCalFileVersionSupport& aStatus)
	{
	RStoreReadStream readStream;
	readStream.OpenLC(*(StoreL()),iModelStreamId);
	readStream >> aFileVersion;
	CleanupStack::PopAndDestroy(&readStream);
	CalFileVersionUtils::FileVersionSupportedL(aFileVersion, aStatus);
	}

void CAgnServFile::CreateModelForFileL()
	{
	// Create a model for this file
	iModel = CAgnEntryModel::NewL(this);
	
	// Set the model to run in server mode
	// Open the requested file
	iModel->OpenL(*iStore, iModelStreamId);
	iIndexesBuilt = EFalse;
	iProgressTotal = 0; 
	if (!iCategoryList)
		{
		iCategoryList = CAgnCategoryList::NewL(*this);
		}
	iModel->CategoryIndex().SetCategoryList(iCategoryList);
	
	// we've got here, so the file isn't on the z:\ drive
	SaveCategoryListL();
	SetFileNameHashL();
	}

void CAgnServFile::SetFileNameHashL()
	{
	// There is a file associated with the model
	TParsePtrC parse(FileName());
	TPtrC drive(parse.Drive());
	TPtrC name(parse.Name());
	HBufC* fileNameAndDriveOnly = HBufC::NewLC(drive.Length() + name.Length());
	TPtr ptr = fileNameAndDriveOnly->Des();
	ptr.Append(drive);
	ptr.Append(name);
	iFileNameHash = FoldAndGenerateHashL(*fileNameAndDriveOnly);
	CleanupStack::PopAndDestroy(fileNameAndDriveOnly);
	}

TUint32 CAgnServFile::FileNameHash() const
	{
	return iFileNameHash;
	}
	
void CAgnServFile::SaveCategoryListL()
	{
	TStreamId streamId = DictionaryLookup(KUidAgnCategoryList);

	if (streamId == KNullStreamId)
		{
		// If the category list stream does not exist, create it.
		CreateCategoryListL();
		}
	else
		{
		// category stream exists, we are re-opening the file
		InternalizeCategoryListL(streamId);
		}
	
	iModel->RestoreCategoriesL();
	}


void CAgnServFile::CreateCategoryListL() const
	{
	TStreamId streamId = WriteCategoryListL(*iStore);
	AddStreamToDictionaryL(KUidAgnCategoryList, streamId);
	}

TStreamId CAgnServFile::WriteCategoryListL(CStreamStore& aStore) const
	{
	RStoreWriteStream writeStream;
	TStreamId streamId = writeStream.CreateLC(aStore);
	iCategoryList->ExternalizeL(writeStream);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	return streamId;
	}

void CAgnServFile::InternalizeCategoryListL(const TStreamId& aStreamId)
	{
	RStoreReadStream readStream;
	readStream.OpenLC(*iStore,aStreamId);
	iCategoryList->InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	}


void CAgnServFile::ExternalizeCategoryListL()
	{
	TStreamId streamId = DictionaryLookup(KUidAgnCategoryList);
	RStoreWriteStream writeStream;
	writeStream.ReplaceLC(*iStore,streamId);
	iCategoryList->ExternalizeL(writeStream);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	User::LeaveIfError(iStore->Commit());
	}


CAgnCategoryList& CAgnServFile::CategoryList() const
	{
	return *iCategoryList;
	}

void CAgnServFile::StartBuildIndex(TAgnMessageToComplete& aMessageToComplete)
		
	{
	if (iIndexesBuilt)
		{
		// The indexes have already been bulit.
		aMessageToComplete.Message().Complete(KErrNone);
		return;
		}
	
	// Add this request to the async. index build request queue.
	//
	TRAPD(err, AddAsyncRequesterL(aMessageToComplete));
	if (err != KErrNone)
		{
		aMessageToComplete.Message().Complete(err);
		}
	else if (iActiveStep != CCalAsyncTaskManager::EBuildIndex)
		{
		DoStartBuildIndex();
		}
	}

void CAgnServFile::RequestProgressL(TAgnMessageToComplete& aMessageToComplete)
		
	{
	if (iActiveStep == CCalAsyncTaskManager::ENoAction)
		{
		// there are no progress to report
		aMessageToComplete.Message().Complete(KErrNone);
		return;
		}

	// Add this request to the async. index build request queue.
	//
	TRAPD(err, AddAsyncRequesterL(aMessageToComplete));
	if (err != KErrNone)
		{
		aMessageToComplete.Message().Complete(err);
		// If are already building the indexes then don't broadcast the error.
		}
	}
	
void CAgnServFile::DoStartBuildIndex()
	{
	// If we are already building the index then there is nothing
	// more to do, this request will be completed when the index has been
	// built.
	//

	// Start building the index.
	iModel->ResetIndexes();
	if ( ! iModel->StreamsAreEmpty())
		{			
		// Report % complete and continue building index.
		iActiveStep = CCalAsyncTaskManager::EBuildIndex;
		iIndexesBuilt = EFalse;
		iProgressTotal = 0;
		Start();
		}
	else
		{
		// Agenda is empty, nothing to do for indexes. Complete immediately.
		iIndexesBuilt = ETrue;
		CompleteRequests(ETrue, KErrNone, iSession);
		}	
	}

TBool CAgnServFile::AreIndexesBuilt() const
	{
	return iIndexesBuilt;
	}

// static function which can be added to a cleanup item on cleanup stack
void CAgnServFile::CloseAgenda(TAny* aFile)
	{
	CAgnServFile* file = static_cast<CAgnServFile*>(aFile);
	file->CloseAgendaImmediately();
	delete file;
	}

void CAgnServFile::DoSaveIndexFile()
	{
	if (iModel == NULL)
		{
		return;
		}
	if (iIndexesBuilt && iModel->IsIndexFileDirty())
		{
		TInt trapErr = KErrNone;
		TRAP(trapErr, iModel->SaveIndexFileL());
		if (trapErr != KErrNone)
			{
			// if we can't save the index file, we need to
			// make sure that it is marked as dirty
			TRAP_IGNORE(iModel->MarkIndexFileAsDirtyL());
			}
		}
	}	

/** 
* Closes the calendar file in the current agenda server session immediately or after a delay.
* The delay is triggered using a timer owned by the calendar file. If a session then tries to access the same calendar file, the delay is cancelled and the file is not closed
* @param aCloseAgendaWithDelay ETrue if a delay is required before the closure of the calendar file, EFalse if the calendar file is to be closed immediately
*/
void CAgnServFile::CloseAgenda(TBool aCloseImmediately)
	{
	__ASSERT_DEBUG(iRefCount>=0,User::Invariant());
	
	if (iRefCount>0)
		{
		// Decrease the reference count	
		--iRefCount;
		
		if(iRefCount == 0)
			{
			
			if(!aCloseImmediately) // Decide whether we need to trigger the file close timer 
				{
				Cancel();
				// Kick off the timer delay before closing the file
				iFileShutdownDelayTimer->Start();
				}
			else 
				{
				// Close the file immediately
				iFileShutdownDelayTimer->DoCloseAgenda();
				}
			}
		}
	}
	
void CAgnServFile::CloseAgendaImmediately()
	{
	// We're now sure that we are closing the agenda file.
	// We need to bring the cached index file up to date.
	DoSaveIndexFile();	
	Cancel();
	
	//The alarms can also be queued by a shutdown notification from the system state manager.
	//If the alarms have been queued then we do not want to do so again, so checking here.
	if (iStore && iModel
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
		&& !iShutdownNotification
#endif
		)
		{
		//queue next 10 alarms within 31 days since it is the last client
		TRAP_IGNORE(iModel->FindAndQueueNextFewAlarmsL());	
		}
	
	if (IsActive())
		{
		Deque();
		}

	// Make sure we're not still compacting
	iCompactor.Close();

	// No more references, so actually close the file
	delete iModel;
	iModel = NULL;

	// Make sure the compactor is shut down before deleting the store
	delete iStore;
	iStore = NULL;

	delete iDictionary;
	iDictionary = NULL;
	}



// Add to the file session reference count
void CAgnServFile::AddReference()
	{
	++iRefCount;
	iFileShutdownDelayTimer->Cancel();
	}

// Getter - file session reference count
TInt CAgnServFile::ReferenceCount() const
	{
	return iRefCount;
	}


CFileStore* CAgnServFile::StoreL() const
	{
	if(!iStore)
		{
		//It shouldn't happen unless the client go for server function directly
		User::Leave(KErrLocked);
		}
	return iStore;
	}

CStreamDictionary* CAgnServFile::Dictionary() const
	{
	return iDictionary;
	}

CAgnEntryModel* CAgnServFile::Model() const
	{
	return iModel;
	}


TStreamId CAgnServFile::DictionaryLookup(TUid aUid) const
	{
	return iDictionary->At(aUid);
	}

TBool CAgnServFile::IsLocked() const
	{
	TBool ret = EFalse;
	if(iActiveStep != CCalAsyncTaskManager::ENoAction)
	    {
	    ret = ETrue;
	    }
	else
	    {
	    ret = iLocked;
	    }
	
	return ret;
	}

void CAgnServFile::SetLock(TBool aLocked)
    {
    iLocked = aLocked;
    }

TBool CAgnServFile::IsReadOnly() const
	{
	return iReadOnly;
	}


TBool CAgnServFile::DoStepL()
	{
	TBool completed = EFalse;

	TInt progress = 0;
	switch (iActiveStep)
		{
		case CCalAsyncTaskManager::EBuildIndex:
			_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("DoStepL: Agenda Serv File: Async Build Index step");)
			progress = DoBuildIndexStepL();
			break;

		case CCalAsyncTaskManager::EDeleteEntry:
			_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("DoStepL: Agenda Serv File: Async Delete Entries");)
			progress = iAsyncDelete->DoDeleteStepL();
			break;

		case CCalAsyncTaskManager::EFilterCategory:
			_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("DoStepL: Agenda Serv File: Async Filter Category");)
		case CCalAsyncTaskManager::EDeleteCategory:
			_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("DoStepL: Agenda Serv File: Async Delete Category");)
			
			if (iIndexesBuilt == EFalse)
				{
				_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("DoStepL: Agenda Serv File: Indexes are not built");)
			
				// check the entry model's version
				TAgnVersion version;
				CalCommon::TCalFileVersionSupport status;
				GetFileVersionSupportStatusL(version, status);
			
				if (status != CalCommon::EFileIsCurrentVersion)
					{
					_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("KErrNotSupported: Indexes must be built before any category operations can be done on a non-current file");)
							
					// indexes must be built before any category operations can be done on a non-current file
					User::Leave(KErrNotSupported);
					}
				}

			progress = iModel->CategoryIndex().DoStepCategoryL(iActiveStep);
			break;
			
		default:	
			completed = EFalse;	// return Complete status
		}

	iProgressTotal = progress;

	if (iProgressTotal == KAgnPercentageComplete)
		{
		completed = DoTaskCompleteL(KErrNone, iSession);//complete
		}
	else
		{
		CompleteRequests(EFalse, iProgressTotal, iSession);
		}
			
	return completed;
	}


TInt CAgnServFile::DoBuildIndexStepL()
	{
	TInt indexProgress=1;//0 implies finished
	if (iIndexesBuilt)
		{
		// Index building is already completed.
		return KAgnPercentageComplete;
		}

	// Some some work on building the index, 0 means it is finished otherwise
	// the percentage progress from 1 to 100 is returned.
	indexProgress = iModel->DoIndexBuildStepL();

	// If the file has been completely loaded, then we can set that the category
	// list has had its counters incremented for all entries in this file
	if (indexProgress == KAgnPercentageComplete)
		{
		iIndexesBuilt = ETrue;
		}

	return indexProgress;
	}

TInt CAgnServFile::DoCompactL()
	{
	TInt ret = EFalse;
	TRAPD(err, ret = DoCompactorStepL());
	if (err != KErrNone)
		{
		// Compactor error, so stop it
		iCompactionStage = ENothing;
		iCompactor.Close();
		User::Leave(err);
		}

	if (ret)
		{
		// Finished compacting, so get rid of the CIdle
		iCompactionStage = ENothing;
 		return ETrue;
		}

	return EFalse;
	}


TBool CAgnServFile::DoCompactorStepL()
	{
	// Attempt to compact the file
	if (iCompactionStage==ENothing)
		{
		iCompactor.OpenL(*iStore, iNextCompactEffort);
		iCompactionStage=EReclaiming;
		return EFalse;
		}

	// Set the compaction stage to ENothing, so if the next step
	iCompactor.NextL(iNextCompactEffort);
	if (iNextCompactEffort == 0)
		{// Stage is finished
		CStreamStore& store=*iStore;
		if (iCompactionStage==EReclaiming)
			{
			// Decide if there is enough free space to compact - 
			// The threshold is the greater of KAgnCompactionThresholdInBytes (10240 bytes)
			// or 1/10th of the file size
			TInt totalSize=0;
			User::LeaveIfError(iStore->File().Size(totalSize));
			TInt threshold = KAgnCompactionThresholdInBytes;
			if (threshold < totalSize/10)
				{
				threshold = totalSize/10;	
				}
			if (iCompactor.Available() < threshold)
				// There is space to reclaim, but not enough to make it worthwhile, so stop the active object
				{
				iCompactor.Close();
				iCompactionStage = ENothing;
				return ETrue;
				}
			// There is enough space to reclaim to make it worthwhile, so kick it off
			store.CommitL();
			iCompactor.Close();
			iCompactor.CompactL(store, iNextCompactEffort);
			iCompactionStage=ECompacting;
			}
		else if (iCompactionStage==ECompacting)
			{// Compaction is all done, so stop
			store.CommitL();
			iCompactor.Close();
			iCompactionStage=ENothing;
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CAgnServFile::IsInterestedSession(CAgnServerSession* aSession)
	{
	const TInt count = iMessageVector->Count();
	
	for (TInt ii=0; ii< count; ++ii)
		{
		if(&iMessageVector->At(ii).Session() == aSession)
			{
			return ETrue;
			}
		}
	
	return EFalse;
	}

void CAgnServFile::CancelTaskL(CAgnServerSession* aSession)
    {
    Cancel();
    CompleteRequests(ETrue, KErrCancel, aSession);
    }
// If an error occurs during a index build or deletetion request
// then complete it and broadcast the error as the progress count.
//
TBool CAgnServFile::DoTaskCompleteL(TInt aErr, CAgnServerSession* aSession)
	{
	if(aErr == KErrCancel && !IsInterestedSession(aSession))
		{
		return EFalse;
		}
	
	TBool complete = ETrue;
	switch(iActiveStep)
		{
		case CCalAsyncTaskManager::EDeleteEntry:
			{
			iActiveStep = CCalAsyncTaskManager::ENoAction;
			if(aErr != KErrNone)
				{
				iModel->Rollback();
				}
			else
				{
				iModel->CommitL();
				CompactFileL();
				// Indicate that there is more processing to do.
				}
			
			iModel->SetBufferedDeleting(EFalse);
			delete iAsyncDelete;
			iAsyncDelete = NULL;
			TInt64 fileid = Model()->GetFileIdL();
			TRAP_IGNORE(iSession->BulkChangeCompletedL(fileid, aErr)); // transmits error to client
			iSession = NULL;
			}
			break;
		case CCalAsyncTaskManager::EDeleteCategory:
			iActiveStep = CCalAsyncTaskManager::ENoAction;
			if(aErr != KErrNone)
				{
				if(!iModel->CategoryIndex().FirstStep())
					{
					TInt err=0;
					TRAP(err,CategoryList().RollBackL());
					TRAP(err,iModel->CategoryIndex().RollBackDeleteCategoryL());
					__ASSERT_DEBUG(err==KErrNone, Panic(EAgmErrRollbackFailed));
					}
				}
			else
				{
				TRAPD(err, iModel->CommitL());
				if (err != KErrNone)
					{
					iModel->Rollback();
					aErr = err;
					}
				}
			break;
		case CCalAsyncTaskManager::EBuildIndex:
			if (aErr == KErrNone)
				{
				iModel->BuildIndexCompleteL();
				iModel->CommitL();
				iActiveStep = CCalAsyncTaskManager::ENoAction;
				}
			else if (aErr == KErrOverflow || aErr == KErrEof)
				{
				// the file is corrupt
				aErr = KErrCorrupt;
				}
			break;
		}
	CompleteRequests(ETrue, aErr, aSession);
	return complete;	
	}

TInt CAgnServFile::AddAsyncRequester(TAgnMessageToComplete& aMessageToComplete)
	{
	// Try to add this async request to the queue
	// and complete the message if it fails
	
	TRAPD(err, AddAsyncRequesterL(aMessageToComplete));
	
	if (err != KErrNone)
		{
		aMessageToComplete.Message().Complete(err);
	    iSession = NULL;        	
		}

	return err;
	}

void CAgnServFile::AddStreamToDictionaryL(const TUid& aUid, const TStreamId& aStreamId) const
	{
	AddStreamToDictionaryL(aUid, aStreamId, *iDictionary, *iStore);
	}

void CAgnServFile::AddStreamToDictionaryL(const TUid& aUid, const TStreamId& aStreamId, 
			CStreamDictionary& aDictionary, CPersistentStore& aStore) const
	{
	aDictionary.AssignL(aUid, aStreamId);

	// Re-write the dictionary stream
	RStoreWriteStream stream;
	TStreamId streamId = aStore.Root();
	stream.ReplaceLC(aStore, streamId);
	stream<< aDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // dictionary stream
	iStore->CommitL();
	}

void CAgnServFile::CompactFileL()
	{
	_DBGLOG_BASIC(AgmDebug::DebugLog("Agenda Serv File: Synchronous Compact file");)
		
   	while (!DoCompactL())
		{
		}
	}

void CAgnServFile::TidyByDateSetup(CAgnServerSession& aSession,
								   const TAgnFilter& aFilter,
								   const TTime& aUndatedTodoDate,
								   const TTime& aStartDate,
								   const TTime& aEndDate)
	{
	iSession = &aSession;
	iTidyByDateFilter = aFilter;
	iTidyByDateUndatedTodoDate = aUndatedTodoDate;
	iTidyByDateStartDate = aStartDate;
	iTidyByDateEndDate = aEndDate;	
	}


void CAgnServFile::TidyByDateStartL(TAgnMessageToComplete& aMessageToComplete, TAgnChangeFilter& aChangeFilter)
	{
	if(iAsyncDelete)
		{
		iModel->SetBufferedDeleting(EFalse);
		delete iAsyncDelete;
		iAsyncDelete = NULL;
		}
		
	iAsyncDelete = iModel->CreateAsyncDeleteL(aChangeFilter);

	iModel->CommitL(); // commit changes before beginning delete
	
	// Ask the async delete object to set itself up.
	// this will create a list of all the entries to delete
	// and tell us if there is any work to be done.
	TBool entriesToDelete = iAsyncDelete->SetUpDeleteL(iTidyByDateFilter,
														iTidyByDateUndatedTodoDate,
														iTidyByDateStartDate,
														iTidyByDateEndDate);

	if (!entriesToDelete)
		{
		// There are no entries that match the filter
		// so delete the active delete object and
		// complete the request. 
		delete iAsyncDelete;
		iAsyncDelete = NULL;
		aMessageToComplete.Message().Complete(KErrNone);
		iSession = NULL;
		}
	else
		{
		// There are entries to delete so add the request message
		// to our list of messages so that we can complete
		// it later when the async delete is finished.
		TInt error = AddAsyncRequester(aMessageToComplete);

		if (error != KErrNone)
			{
			// We were unable to add the request message our list.
			// The message has been completed with the
			// correct error code so we just need to clean up.
			delete iAsyncDelete;
			iAsyncDelete = NULL;
			iSession = NULL;
			}
		else
			{
			// Start performing the tidy via the RunL function of this Active Object.
			iModel->SetBufferedDeleting(ETrue);
			iActiveStep = CCalAsyncTaskManager::EDeleteEntry;
			Start();
			}
		}
	}

void CAgnServFile::CategoryTaskStartL(TAgnMessageToComplete& aMessageToComplete, CCalAsyncTaskManager::TAsyncAction aTask)
	{
	if ( AddAsyncRequester(aMessageToComplete) == KErrNone )
		{
		iActiveStep = aTask;
		Start();
		}
	}

void CAgnServFile::CreateDirL(const TDesC& aFileName) const
	{
	CreateDirL(iFs, aFileName);
	}

TBool CAgnServFile::FileExistsL(const TDesC& aFileName) const
	{
	return FileExistsL(iFs, aFileName);
	}

void CAgnServFile::DeleteFileL(const TDesC& aFileName) const
	{
	DeleteFileL(iFs, aFileName);
	}

void CAgnServFile::MoveFileL(const TDesC& aSource, const TDesC& aDestination) const
	{
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Move(aSource, aDestination, CFileMan::ERecurse));
	CleanupStack::PopAndDestroy(fileMan);
	}

void CAgnServFile::CopyFileL(const TDesC& aSource, const TDesC& aDestination) const
	{
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(aSource, aDestination, CFileMan::ERecurse));
	CleanupStack::PopAndDestroy(fileMan);
	}

// Return EFalse if file does not exist, or ETrue if it does. Leaves if file status cannot be determined.
TBool CAgnServFile::FileIsReadOnlyL(const TDesC& aFileName) const
	{
	TUint attrib;
	User::LeaveIfError(iFs.Att(aFileName, attrib));
	return attrib & KEntryAttReadOnly;
	}
	
// ensures a directory exists for this filename. If directory already exists, nothing happens
/*static*/ void CAgnServFile::CreateDirL(RFs& aFs, const TDesC& aDirectory)
	{
	TInt err = aFs.MkDirAll(aDirectory);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	}

// Return EFalse if file does not exist, or ETrue if it does. Leaves if file status cannot be determined.
/*static*/ TBool CAgnServFile::FileExistsL(RFs& aFs, const TDesC& aFileName)
	{
	TUint dummy;
	TInt err = aFs.Att(aFileName, dummy);
	if (err == KErrNotFound || err == KErrPathNotFound)
		{
		return EFalse;
		}
	User::LeaveIfError(err);
	return ETrue;
	}
	
// Deletes file but does not leave if it cannot be found
/*static*/ void CAgnServFile::DeleteFileL(RFs& aFs, const TDesC& aFileName)
	{
	TInt err = aFs.Delete(aFileName);
	if (err != KErrNotFound && err != KErrPathNotFound)
		{
		User::LeaveIfError(err);
		}
	}

/**
Opens a file with the specified name. Used for creating attachment files.
*/
void CAgnServFile::OpenFileL(RFile& aFileHandle, const TDesC& aFileName)
	{
	User::LeaveIfError(aFileHandle.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
	}

/**
Creates a new file with the specified name. Used for creating attachment files.
*/
void CAgnServFile::CreateNewFileL(RFile& aFileHandle, const TDesC& aFileName)
	{
	User::LeaveIfError(aFileHandle.Create(iFs, aFileName, EFileRead | EFileWrite | EFileShareExclusive));
	}

void CAgnServFile::ReplaceConvertedAgendaFileL(CAgnEntryManager& aEntryManager, CAgnTzRuleIndex& aTzRuleIndex)
	{
	// close the agenda file but don't delete the model since indexes are already built
	CAgnEntryModel* tmpModel = iModel;
	iModel = NULL;

	CloseAgendaImmediately();

	iModel = tmpModel;

	// delete the existing file and rename the new one in its place
	HBufC* convertedFileName = HBufC::NewLC(FileName().Length() + KUpdatedAgendaFileExtension().Length());
	TPtr convertedFileNamePtr(convertedFileName->Des());
	convertedFileNamePtr.Append(FileName());
	convertedFileNamePtr.Append(KUpdatedAgendaFileExtension);
	
	DeleteFileL(FileName());
	User::LeaveIfError(iFs.Rename(convertedFileNamePtr, FileName()));
	
	CleanupStack::PopAndDestroy(convertedFileName);
	
	// re-open the agenda stream store, but don't rebuild indexes as no need
	CFileStore* store = NULL;
	iDictionary = CApaProcess::ReadRootStreamLC(iFs, store, FileName(), EFileRead);
	CleanupStack::Pop(); // dictionary
    CleanupStack::PushL(store);
	iModelStreamId = iDictionary->At(KUidAgnModel);
	if (iModelStreamId == KNullStreamId)
		{
		User::Leave(KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(store);
	
	iStore = CPermanentFileStore::OpenL(iFs,FileName(),EFileShareAny|EFileRead|EFileWrite);
	
	aTzRuleIndex.ReloadStore(*iDictionary, *iStore);
	iModel->LoadNewStreamStoreL(*iStore, iModelStreamId, aEntryManager, aTzRuleIndex);
	
	// Calculate the file name hash key
	// for publish and subscribe notifications
	SetFileNameHashL();
	}

HBufC8* CAgnServFile::GetPropertyValueLC(TStreamId aStreamId)
    {
    // Calendar info has been set on this file
    RStoreReadStream readStream;
    readStream.OpenLC(*iStore, aStreamId);
    HBufC8* value = HBufC8::NewL(readStream, KMaxTInt);
    CleanupStack::PopAndDestroy(&readStream);
    CleanupStack::PushL(value);
    return value;
    }

TBool CAgnServFile::SetCalendarInfoL(const CAgnCalendarInfo& aCalendarInfo)
    {
	TBool fileInfoExist = EFalse;
	
    // Fetch the existing calendar info
    TStreamId calendarInfoStreamId = iDictionary->At(KUidAgnCalendarInfo);
    TStreamId newStreamId;
    
    RStoreWriteStream writeStream;
    
    if (calendarInfoStreamId == KNullStreamId)
        {
        // Write all the properties to seperate streams
        aCalendarInfo.ExternalizePropertiesL(*iStore, NULL);
    
        // The calendar info stream does not exist so create it
        newStreamId = writeStream.CreateLC(*iStore);
        }
    else
        {
        // Read the current metadata
        RStoreReadStream readStream;
        readStream.OpenLC(*iStore, calendarInfoStreamId);
        CAgnCalendarInfo* info(CAgnCalendarInfo::NewL());
        CleanupStack::PushL(info);
        info->FileInternalizeL(readStream);
        CleanupStack::Pop(info);
        CleanupStack::PopAndDestroy(&readStream);
        CleanupStack::PushL(info);
        
        // Write all the properties to seperate streams
        aCalendarInfo.ExternalizePropertiesL(*iStore, info);
        
        CleanupStack::PopAndDestroy(info);
        
        writeStream.ReplaceLC(*iStore, calendarInfoStreamId);
		
		fileInfoExist = ETrue;
        }
    
    
    // Update the values etc 
    aCalendarInfo.FileExternalizeL(writeStream);
    
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(&writeStream);
    
    if (calendarInfoStreamId == KNullStreamId)
        {
        AddStreamToDictionaryL(KUidAgnCalendarInfo, newStreamId);
        }
		
    iIsFileDisabled = (!aCalendarInfo.Enabled());
    TParsePtrC parser(FileName());
    TFileName fileName = parser.Drive();
    fileName.Append(parser.NameAndExt());
    if(iIsFileDisabled)
        {
        iAgnServer.AlarmServer().SetAlarmStatusForCalendarFile(fileName, EAlarmStatusDisabled); 
        }
    else
        {
        QueueAlarmsImmediately();
        }
    
    User::LeaveIfError(iStore->Commit());
    return fileInfoExist;
    }
	
TBool CAgnServFile::IsCalendarInfoExistL() const
    {
    TStreamId calendarInfoStreamId = iDictionary->At(KUidAgnCalendarInfo);
    return (calendarInfoStreamId != KNullStreamId)?ETrue:EFalse;
    }
	
CAgnCalendarInfo* CAgnServFile::GetCalendarInfoLC() const
    {
    CAgnCalendarInfo* info(CAgnCalendarInfo::NewL());
    CleanupStack::PushL(info);
    
    TStreamId calendarInfoStreamId = iDictionary->At(KUidAgnCalendarInfo);
    
    if (calendarInfoStreamId != KNullStreamId)
        {
        // Calendar info has been set on this file
        RStoreReadStream readStream;
        readStream.OpenLC(*iStore, calendarInfoStreamId);
        info->FileInternalizeL(readStream);
        CleanupStack::PopAndDestroy(&readStream);
        info->SetIsValid(ETrue);
        }
    else
        {
        // There is no calendar info for this file so return an invalid version
        info->SetIsValid(EFalse);
        }
    
    // make sure we set the file on the info that we are returning
    TParsePtrC parse(*iFileName);
    TFileName shortFileName(parse.Drive());
    shortFileName.Append(parse.NameAndExt());
    info->SetFileNameL(shortFileName);
    
    return info;
    }

void CAgnServFile::AddAsyncRequesterL(TAgnMessageToComplete& aMessageToComplete)
	{
	// It is only possible to have more than one client in the message queue
	// if the asych task is building index
	iMessageVector->AppendL(aMessageToComplete);
	if ( (iActiveStep != CCalAsyncTaskManager::ENoAction) &&
			(iActiveStep != CCalAsyncTaskManager::EBuildIndex) )
		{
		__ASSERT_DEBUG(iMessageVector->Count()==1, Panic(EAgmErrTwoAsyncTasksStarted));
		}
	iSession = &aMessageToComplete.Session();
 	}

void CAgnServFile::CompleteRequests(TBool aIsCompleted, TInt aCompleteCode, CAgnServerSession* aSession)
	{
	if(aIsCompleted)
		{
		iActiveStep = CCalAsyncTaskManager::ENoAction;
		}

	TInt count = iMessageVector->Count();
	for (TInt i = count -1 ; i >= 0 ; i--)
		{
		if (aCompleteCode == KErrCancel)
			{//One client should only cancel its own building index task
			if (aSession == &iMessageVector->At(i).Session())
				{
				iMessageVector->At(i).Message().Complete(KErrCancel);
				iMessageVector->Delete(i);
				if (iMessageVector->Count() == 0)
					{
					// if this operation has failed with a KErrCancel error, cancel the active object
					iActiveStep = CCalAsyncTaskManager::ENoAction;
					}
				break;
				}
			}
		else
			{
			if (aIsCompleted || (iMessageVector->At(i)).ReportProgress())
				{
				(iMessageVector->At(i)).Message().Complete(aCompleteCode);	
				iMessageVector->Delete(i);
				}
		
			}
		}
	}

CAgnServer& CAgnServFile::Server() const
	{
	return iAgnServer;
	}

CAgnServerSession* CAgnServFile::ServerSession()
    {
    return iSession;
    }

void CAgnServFile::DoCloseAgenda()
	{
	CloseAgendaImmediately();
	// Closing the file (itself) is the last thing the file should do. The file is owned and closed by the file manager.
	// Do NOT attempt to use the file or the delay close timer object after closing the file
	iAgnServer.FileMgr()->CloseAgendaFile(this);
	}

// Getter - File close timer 
CAgnServFileShutdownDelayTimer& CAgnServFile::FileShutdownDelayTimer() const
	{
	__ASSERT_DEBUG(iFileShutdownDelayTimer, User::Invariant());
	return *iFileShutdownDelayTimer;
	}

void CAgnServFile::SetRefreshTzRules(TBool aSetRefresjTzRule)
    {
    iRefreshTzRules = aSetRefresjTzRule;
    }

TBool CAgnServFile::RefreshTzRules() const
    {
    return iRefreshTzRules;
    }

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
void CAgnServFile::QueueAlarmsImmediately()
	{
	if (iStore && iModel)
		{
		TRAP_IGNORE(iModel->FindAndQueueNextFewAlarmsL());	
		}
	}

void CAgnServFile::DeleteAlarmsAndRequeueSessionAlarm()
	{
	if (iStore && iModel)
		{
		TRAP_IGNORE(iModel->DeleteAlarmsAndRequeueSessionAlarmL());	
		}
	}

void CAgnServFile::SetShutdownFlag(TBool aNotificationFlag)
	{
	iShutdownNotification = aNotificationFlag;
	}
#endif


void CAgnServFile::TzRulesHaveChangedL()
	{
	RPointerArray<CAgnServerSession> sessions;//does not own
	CleanupClosePushL(sessions);
	iAgnServer.FetchSessionsL(sessions);
	const TInt count = sessions.Count();
	TInt64 fileId = iModel->GetFileIdL();
	TAgnChange change;
	change.iOperationType = MCalChangeCallBack2::EChangeTzRules;
	change.iFileId = fileId;
	for ( TInt i = 0; i < count; ++i )
		{
		sessions[i]->AddChangeL(change);
		}
	CleanupStack::PopAndDestroy(&sessions);
	}

void CAgnServFile::SetCollectionId(TCalCollectionId aCollectionId)
	{
	iCollectionId = aCollectionId;
	}

TUint8 CAgnServFile::CollectionId() const
	{
	return iCollectionId;
	}

TInt CAgnServFile::BackupReStoreChanged(MCalChangeCallBack2::TChangeType aChangeType)
	{
	TInt err = KErrNone;
	RFile file;         

	switch(aChangeType)
		{
		case MCalChangeCallBack2::EBackupStart:
			SetLock(ETrue);
			BackupRestoreLock(ETrue);
			(iStore->File()).Close();
			iStore->Detach();
			break;
		case MCalChangeCallBack2::EBackupEnd:
			if (iReadOnly)
				{
				err = file.Open(iFs,*iFileName,EFileShareAny|EFileRead);
				}
			else
				{
				err = file.Open(iFs,*iFileName,EFileShareAny|EFileRead|EFileWrite);
				}
			iStore->Reattach(file);
			SetLock(EFalse);
			BackupRestoreLock(EFalse);
			break;
		case MCalChangeCallBack2::ERestoreStart:
			SetLock(ETrue);
			Model()->MarkIndexFileAsDirtyL();
			BackupRestoreLock(ETrue);
			delete iStore;
			iStore = NULL;
			delete iDictionary;
			iDictionary = NULL;
			break;
		case MCalChangeCallBack2::ERestoreEnd:
			delete iModel;
			iModel = NULL;
			SetLock(EFalse);
			TRAP(err, ReopenAgendaAfterRestoreL());
			BackupRestoreLock(EFalse);
			break;
		default:
			User::Invariant();
			break;
		}
	return err;
	}

void CAgnServFile::BackupRestoreLock(TBool aLock)
	{
	iBackupRestoreLock = aLock;
	RPointerArray<CAgnServerSession> sessions;
	CleanupClosePushL(sessions);

	//Fetch all sessions
	TRAP_IGNORE (iAgnServer.FetchSessionsL(sessions));
	TInt countSessions = sessions.Count();
	for(TInt ii = 0; ii< countSessions; ++ii)
		{
		// lock the client
		sessions[ii]->LockClient(aLock);
		}
	CleanupStack::PopAndDestroy(&sessions); 
	}

TBool CAgnServFile::IsBackupRestoreLock() const
	{
	return iBackupRestoreLock;
	}


// aServeFile is NOT owned
CAgnServFileShutdownDelayTimer* CAgnServFileShutdownDelayTimer::NewL(CAgnServFile& aServFile)
	{
	CAgnServFileShutdownDelayTimer* self = new(ELeave) CAgnServFileShutdownDelayTimer(aServFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CAgnServFileShutdownDelayTimer::CAgnServFileShutdownDelayTimer(CAgnServFile& aServFile)
	: CTimer(EPriorityIdle), iServFile(aServFile)
	{
	}

/** 
Starts the timer to close the file. Usually the file will be closed after a delay of 5 seconds.
However if a backup or restore operation is in progress the file will be closed immediately as 
the backup engine needs access to the files.
*/
void CAgnServFileShutdownDelayTimer::Start()
	{
	if (iServFile.Server().BackupRestoreAgent().BackupInProgress() || 
		iServFile.Server().BackupRestoreAgent().RestoreInProgress())
		{
		Cancel();
		DoCloseAgenda();
		}
	// The timer shouldn't be active, in release mode we cope with it though to be 
	// more robust
	else if (!IsActive())
		{
		After(KServerShutdownDelay);
		}
	else
		{
		__ASSERT_DEBUG(!IsActive(), Panic(EAgmErrFileTimerAlreadyActive));
		}
	}

void CAgnServFileShutdownDelayTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}


void CAgnServFileShutdownDelayTimer::DoCloseAgenda()
	{
	iServFile.DoCloseAgenda();
	}

void CAgnServFileShutdownDelayTimer::CloseAgenda()
	{
	// Check if the timer is active. If the timer is active, it implies the last session of the file has closed and no other sessions are open
	// For example this is used to cancel the timer just before deleting the calendar file incase the timer is the only thing preventing the deletion
	if(IsActive())
		{
		Cancel();
		DoCloseAgenda();
		}
	}

void CAgnServFileShutdownDelayTimer::RunL()
	{
	if(iStatus == KErrNone)
		{
		DoCloseAgenda();
		}
	}

// CAgnServFileMgr //

CAgnServFileMgr::CAgnServFileMgr(RFs& aFs, CAgnServer& aAgnServer)
: iAgnServer(aAgnServer), iFs(aFs)
	{
	}


CAgnServFileMgr::~CAgnServFileMgr()
	{
	delete iPermanentData;
	
	// Make sure file list is empty
	if (iFileList)
		{
		for (TInt count = iFileList->Count() - 1; count >= 0; --count)
			{
			delete (*iFileList)[count];
			}
		delete iFileList;
		}
	}


CAgnServFileMgr* CAgnServFileMgr::NewL(RFs& aFs, CAgnServer& aAgnServer)
	{
	CAgnServFileMgr* self = new(ELeave) CAgnServFileMgr(aFs, aAgnServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAgnServFileMgr::ConstructL()
	{
	const TInt KFileListGranularity = 1; // usually only one Calendar file opened at any one time
	iFileList = new (ELeave) CArrayFixFlat<CAgnServFile*>(KFileListGranularity);
	User::LeaveIfError(iFs.PrivatePath(iPrivatePath));
	}

void CAgnServFileMgr::CreatePermanentDataL()
	{
	// Attempt to pre-create the permanent data if the server is in
	// non-transient mode.
	if (iAgnServer.ServerMode() == ENonTransientServer)
		{
		iPermanentData = CAgnPermanentData::NewL(iAgnServer, *this);
		// If StartDataCreationL() leaves then iPermanentData will still 
		// be valid, therefore it is not sensible to let the Leave panic 
		// the entire server; hence TRAP_IGNORE is used.
		TRAP_IGNORE(iPermanentData->StartDataCreationL());
		}
	}

CAgnServFile& CAgnServFileMgr::OpenAgendaL(const TDesC& aFilename, CAgnServer& aAgnServer, CalCommon::TCalFileVersionSupport& aStatus)
	{
	//Parse the filename
	HBufC* realfilename = ParseFilenameLC(aFilename);
	realfilename->Des().Fold();
	
	CAgnServFile* agnServFile = NULL;
	
	// Ownership not passed
	agnServFile = GetFile(*realfilename);
	if(agnServFile == NULL)
		{
		// New file - File has not yet been opened, so open it here
		agnServFile = CAgnServFile::NewL(iFs, aAgnServer);
		CleanupStack::PushL(TCleanupItem(CAgnServFile::CloseAgenda, agnServFile));

		agnServFile->OpenAgendaL(*realfilename, aStatus);
		CAgnServFile* servFile = agnServFile;
		TUint8 lastShortFileId = 0;
		TInt count = iFileList->Count();
		if(count>0)
			{
			lastShortFileId = (*iFileList)[count-1]->CollectionId();
			if(KMaxTUint8 == lastShortFileId)
				{
				User::Leave(KErrOverflow);
				}
			}
		servFile->SetCollectionId(lastShortFileId + 1);
		iFileList->AppendL(servFile);
		CleanupStack::Pop(); // CAgnServFile::CloseAgenda
		}
	else
		{
		// The file already exists so populate aStatus with its version
		// support status because this will be sent back to the client
		TAgnVersion version;
		agnServFile->GetFileVersionSupportStatusL(version, aStatus);
		}
		
	// This will increment the file reference counter and cancel the file shutdown timer
	agnServFile->AddReference();
	CleanupStack::PopAndDestroy(realfilename);
	return *agnServFile;
	}

/** 
* Closes the calendar file in the current agenda server session immediately or after a delay.
* The delay is triggered using a timer owned by the calendar file. If a session then tries to access the same calendar file, the delay is cancelled and the file is not closed
* @param aCloseImmediately EFalse if a delay is required before the closure of the calendar file, ETrue if the calendar file is to be closed immediately
* @param aServFile Calendar file to be closed
*/
TInt CAgnServFileMgr::CloseAgenda(CAgnServFile& aServFile, TBool aCloseImmediately)
	{
	// Close an instance of this store
	TInt fileCount = iFileList->Count();

	for (TInt count=0; count<fileCount; count++)
		{
		CAgnServFile* servFile = (*iFileList)[count];
		if (&aServFile == servFile)
			{
			servFile->CloseAgenda(aCloseImmediately);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

CFileStore* CAgnServFileMgr::CreateAgendaFileLC(const TDesC& aFileName)
	{
	HBufC* resolvedFileName = ParseFilenameLC(aFileName); //Resolve the filename
	CAgnServFile::CreateDirL(iFs, *resolvedFileName);

	// If the permanent data object is holding this file open, 
	// free it so the client can (re)create it.
	if (iPermanentData && iPermanentData->IsOnlyClientOfFile(*resolvedFileName))
		{
		iPermanentData->ReleaseFileL(*resolvedFileName);
		}	

	CFileStore* fileStore = CPermanentFileStore::ReplaceL(iFs, *resolvedFileName, EFileWrite); //Create the file
	CleanupStack::PopAndDestroy(resolvedFileName);
	CleanupStack::PushL(fileStore);
	return fileStore;
	}

void CAgnServFileMgr::DeleteAgendaFileL(const TDesC& aClientFileName)
	{
	//Parse the filename
	HBufC* realfilename = ParseFilenameLC(aClientFileName);
	realfilename->Des().Fold();
	
	// Ownership not passed
	CAgnServFile* agnServFile = GetFile(*realfilename);
	// Check and cancel the file close timer, if active, to enable file deletion
	if (agnServFile)
		{
		agnServFile->FileShutdownDelayTimer().CloseAgenda();
		}

	// If the permanent data object is holding this file open,
	// free it so the client can delete it.
	if (iPermanentData && iPermanentData->IsOnlyClientOfFile(*realfilename))
		{
		iPermanentData->ReleaseFileL(*realfilename);
		}
	
	if ( ! CAgnServFile::FileExistsL(iFs, *realfilename))
		{
		User::Leave(KErrNotFound);
		}
	
	const TInt KFolderNameLength = realfilename->Length() + 8;
	HBufC* folderName = HBufC::NewLC(KFolderNameLength);
	TPtr folderNamePtr = folderName->Des();
	
	CAgnServFile::GetAttachmentFolderNameL(*realfilename, folderNamePtr);
	
	// delete all attachments in the same directory as the calendar file
	CFileMan* fileMan = CFileMan::NewL(iFs);	
	TInt err = fileMan->RmDir(folderNamePtr);
	delete fileMan;
	
	if (err != KErrNotFound && err != KErrPathNotFound)
		{
		User::LeaveIfError(err);
		}

	CAgnServFile::DeleteFileL(iFs, *realfilename);
	
	const TInt KIndexFileNameLength = realfilename->Length() + KIdxFilePostFixLength;
	HBufC* indexFileName = HBufC::NewLC(KIndexFileNameLength);
	TPtr indexfilenamePtr = indexFileName->Des();
	indexfilenamePtr.Append(*realfilename);
	indexfilenamePtr.Append(KIdxFilePostFix());
	TInt delErr = iFs.Delete(indexfilenamePtr);
	// it is valid for the index file not to be present so we only
	// leave on other errors
	if ((delErr != KErrNotFound) && (delErr != KErrPathNotFound))
		{
		User::LeaveIfError(delErr);
		}
	CleanupStack::PopAndDestroy(indexFileName);
	CleanupStack::PopAndDestroy(folderName);
	CleanupStack::PopAndDestroy(realfilename);
	}

const TDesC& CAgnServFileMgr::PrivatePath() const
	{
	return iPrivatePath;
	}

CDesCArray* CAgnServFileMgr::ListAgendaFilesL() const
	{
	return CAgnFileScanner::ListAgendaFilesL(iFs,TUidType(KPermanentFileStoreLayoutUid, KUidAppDllDoc, KUidAgnApp), iPrivatePath);
	}

/** Parse the Agenda file name

The file name combination in unsecured data is a file name with full path. 
In secured data, it includes a drive name and a file name but not a full path.,
i.e. DriveName:FileName 

@param aFileNameDes The file name combination.
@return the full path of the Agenda file.
*/
HBufC* CAgnServFileMgr::ParseFilenameLC(const TDesC& aClientFileName) const
	{
	if (aClientFileName.Locate('\\')>=0)//there is a path included explicitly
		{
		User::Leave(KErrNotSupported);
		}

	HBufC* filename = NULL;
	const TInt KClientFileNameLength = aClientFileName.Length();
	if (KClientFileNameLength > KCalMaxFilePath)
		{
		User::Leave(KErrBadName);
		}	
	if (KClientFileNameLength >= 2 && aClientFileName[1] == KDriveDelimiter)
		{
		filename = HBufC::NewLC(aClientFileName.Length() + iPrivatePath.Length());
		TPtr filenamePtr1(filename->Des());
		filenamePtr1.Copy(aClientFileName.Left(2));
		filenamePtr1.Append(iPrivatePath);
		filenamePtr1.Append(aClientFileName.Right(KClientFileNameLength - 2));
		}
	else
		{
		TDriveUnit driveUnit(EDriveC);
		const TDesC* calendarFileName = &aClientFileName;
		
		if (KClientFileNameLength == 0)
			{//use the default agenda filename
			calendarFileName = &KDefaultAgendaFileName;
			}
		
		filename = HBufC::NewLC(KMaxDriveName + iPrivatePath.Length() + calendarFileName->Length());
		TPtr filenamePtr2(filename->Des());
		
		filenamePtr2.Copy(driveUnit.Name());
		filenamePtr2.Append(iPrivatePath);
		filenamePtr2.Append(*calendarFileName);
		}
	return filename;
	}

TBool CAgnServFileMgr::AgendaFileExistsL(const TDesC& aClientFileName) const
	{
	HBufC* realfilename = ParseFilenameLC(aClientFileName);
	TBool fileExists = CAgnServFile::FileExistsL(iFs, *realfilename);
	CleanupStack::PopAndDestroy(realfilename);
	return fileExists;
	}

TBool CAgnServFileMgr::FileCloseTimersRunning() const
	{
	const TInt KFileCount = iFileList->Count();

	for (TInt count=0; count<KFileCount; ++count)
		{
		CAgnServFile* servFile = (*iFileList)[count];
		if(servFile->FileShutdownDelayTimer().IsActive())
			{
			return ETrue;
			}
		}
	return EFalse;
	}

/** Closes all files that are scheduled for close immediately.

This function is needed when files that are currently scheduled for close need to be closed
immediately. For instance this is used in the backup and restore case where the files need to
be closed immediately because the backup engine needs to access them.
*/
void CAgnServFileMgr::CloseScheduledFilesImmediately()
	{
	for (TInt count = iFileList->Count()-1; count >= 0; --count)
		{
		CAgnServFile* servFile = (*iFileList)[count];
		if (servFile->FileShutdownDelayTimer().IsActive())
			{
			servFile->FileShutdownDelayTimer().Cancel();
			servFile->CloseAgendaImmediately();
			iAgnServer.ShutdownServer();
			delete servFile;
			iFileList->Delete(count);
			}
		}
	}
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
void CAgnServFileMgr::QueueAlarmsImmediatelyForShutdown()
	{
	for (TInt count = iFileList->Count()-1; count >= 0; --count)
		{
		CAgnServFile* servFile = (*iFileList)[count];
		if(servFile)
			{
			servFile->QueueAlarmsImmediately();
			servFile->SetShutdownFlag(ETrue);
			}
		}
	}

void CAgnServFileMgr::RequeueAlarmsForShutdownCancellation()
	{
	for (TInt count = iFileList->Count()-1; count >= 0; --count)
		{
		CAgnServFile* servFile = (*iFileList)[count];
		if(servFile)
			{
			servFile->DeleteAlarmsAndRequeueSessionAlarm();
			servFile->SetShutdownFlag(EFalse);
			}
		}
	}
#endif
// Remove the agenda file from the list and delete it
void CAgnServFileMgr::CloseAgendaFile(CAgnServFile* aServFile)
	{
	// Close an instance of this store
	const TInt KFileCount = iFileList->Count();
	
	#ifdef _DEBUG
	// Calendar file should exist in the list
	TBool fileFound = EFalse;
	#endif
	
	for (TInt count=0; count<KFileCount; ++count)
		{
		CAgnServFile* servFile = (*iFileList)[count];
		if (aServFile == servFile)
			{
			iFileList->Delete(count);
		#ifdef _DEBUG
			fileFound = ETrue;
		#endif
			break;
			}
		}

	// Calendar file should exist in the list whilst deletion and shouldnt have been removed earlier
	__ASSERT_DEBUG(fileFound, Panic(EAgmErrCalendarFileNotFound));
	
	// Close the calendar file
	delete aServFile;
	// Trigger server shutdown in case this was the last calendar file open by the server
	if(iFileList->Count() == 0)
		{
		iAgnServer.ShutdownServer();
		}
	}

// Iterate through the list of files owned by the file manager. Ownership is not passed
// @return Pointer to the matching file or NULL if it isnt found.
CAgnServFile* CAgnServFileMgr::GetFile(const TDesC& aFilename) const
	{
	// Check that the file is not already open
	const TInt KFileCount = iFileList->Count();
	
	for (TInt count=0; count<KFileCount; count++)
		{
		if (!aFilename.CompareF((*iFileList)[count]->FileName()))
			{
			// Doesnt take ownership
			CAgnServFile* agnServFile = (*iFileList)[count];
			return agnServFile;
			}
		}
	return NULL;
	}

CAgnServFile* CAgnServFileMgr::GetFileL(TInt64 aFileId) const
	{
	const TInt KFileCount = iFileList->Count();
	
	for (TInt count=0; count<KFileCount; count++)
		{
		if (aFileId == (*iFileList)[count]->Model()->GetFileIdL())
			{
			// Doesnt take ownership
			return  (*iFileList)[count];
			}
		}
	User::Leave(KErrNotFound);
	return NULL;
	}

CAgnServFile* CAgnServFileMgr::GetFileL(TCalCollectionId aCollectionId) const
	{
	const TInt KFileCount = iFileList->Count();
	
	for (TInt count=0; count<KFileCount; count++)
		{
		if (aCollectionId == (*iFileList)[count]->CollectionId())
			{
			// Doesnt take ownership
			return (*iFileList)[count];
			}
		}

	User::Leave(KErrNotFound);
	return NULL;
	}

TInt CAgnServFileMgr::Count()
    {
    return iFileList->Count();
    }

CAgnServFile* CAgnServFileMgr::File(TInt aIndex)
    {
    return (*iFileList)[aIndex];
    }

TInt64 CAgnServFileMgr::GetLongFileIdL(TCalCollectionId aCollectionId) const
	{
	const TInt KFileCount = iFileList->Count();
	
	for (TInt count=0; count<KFileCount; count++)
		{
		if (aCollectionId == (*iFileList)[count]->CollectionId())
			{
			// Doesnt take ownership
			return (*iFileList)[count]->Model()->GetFileIdL();
			}
		}
	return KNullFileId;
	}

void CAgnServFileMgr::BackupReStoreChanged(MCalChangeCallBack2::TChangeType aChange)
	{
	if (iFileList)
		{
		RPointerArray<CAgnServerSession> sessions;
		TRAP_IGNORE(iAgnServer.FetchSessionsL(sessions));
		const TInt countSessions = sessions.Count();
		TInt ii =0;
		if(aChange == MCalChangeCallBack2::EBackupStart
				|| aChange == MCalChangeCallBack2::ERestoreStart)
			{
			for (ii=0; ii<countSessions; ++ii)
				{
				sessions[ii]->BackupRestoreCancelTask();
				}
			}
		
		const TInt countFiles= iFileList->Count();
		for (TInt jj=0; jj<countFiles; ++jj)
			{
			CAgnServFile* servFile = (*iFileList)[jj];
			TInt64 oldFileId = KNullFileId;          
			TRAP_IGNORE(oldFileId = servFile->Model()->GetFileIdL());
			TInt err = servFile->BackupReStoreChanged(aChange);
			if(aChange == MCalChangeCallBack2::ERestoreEnd && err != KErrNone)
				{
				aChange = MCalChangeCallBack2::ERestoredFileCanNotBeOpened;
				}
			
			for(ii = 0; ii< countSessions; ++ii)
				{
				TRAP_IGNORE(sessions[ii]->BackupReStoreChangedL(oldFileId,
									*servFile, aChange));
				}
			
			if(aChange == MCalChangeCallBack2::ERestoredFileCanNotBeOpened)
				{
				servFile->CloseAgenda(ETrue);
				}
			}
		sessions.Close();
		}
	}

	TBool CAgnServFile::IsFileDisabled()
	{
	return iIsFileDisabled;
	}




