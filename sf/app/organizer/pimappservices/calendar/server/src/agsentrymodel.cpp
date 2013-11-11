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

#include "agsentrymodel.h"

#include "agsalarm.h"
#include "agsasyncdelete.h"
#include "agsattachmentindex.h"
#include "agscategoryindex.h"
#include "agscategorylist.h"
#include "agsentrymanager.h"
#include "agmentry.h"
#include "agsextractor.h"
#include "agsfileconverter.h"
#include "agsiterator.h"
#include "agsstreamidset.h"
#include "agssort.h"
#include "agmutil.h"
#include "agmattendee.h"
#include "agmcategory.h"
#include "agssortinstance.h"
#include "agsindex.h"
#include "agmcontent.h"
#include "agsfilemanager.h"
#include "agsmain.h"
#include "agssess.h"
#include "agstzruleindex.h"
#include <calnotification.h>
#include "agsinstanceiterator.h"

#include "agmdebug.h"

#include <e32math.h>
#include <e32property.h>
#include <s32file.h>


// Compact threshold dependent on the number of operations
const TInt KCompactOperationsThreshold = 32;

/** This is called when opening an agenda file
@internalComponent
 */
CAgnEntryModel* CAgnEntryModel::NewL(CAgnServFile* aAgnServerFile)
	{
	CAgnEntryModel* self = new (ELeave) CAgnEntryModel();
	
	CleanupStack::PushL(self);
	self->ConstructL(aAgnServerFile);
	CleanupStack::Pop();
	
	return (self);
	}

/** Constructs a CAgnEntryModel object
@internalComponent
 */
void CAgnEntryModel::ConstructL(CAgnServFile* aAgnServerFile)

	{
	iUpdateAlarm = ETrue;
	iModelStreamIdSet = CAgnModelStreamIdSet::NewL();
	iEntryManager = CAgnEntryManager::NewL();
	iNextLocalUidValue = 1;
	iNextAttachmentUid = 1;

	if ( aAgnServerFile )
		{//When opening a file
		iAgnServerFile = aAgnServerFile;
		iSimpleEntryTable = CAgnSimpleEntryTable::NewL(*this);
        iExtractor = new (ELeave) TAgnInstanceExtractor(*iSimpleEntryTable);
		iCategoryIndex = CAgnCategoryIndex::NewL();
		iAttachmentIndex = new (ELeave) CAgnAttachmentIndex;
		CreateAlarmForServerL();
		}
	
	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError( iFs.ShareProtected() );
    
	iIndexFileIsDirty = ETrue; 	// for safety assume that the index
					// file is out of date. We can correct this
					// when we read the file

	iIndexFileIsPresent = ETrue; // Assume the index file is present. 
	                             // First attempt to read it will update this
	                             // if it is not there.
	iTzRuleIndex = NULL;
	}

/** 
Frees all resources owned by the agenda model, prior to its destruction. 

@internalComponent
@capability None
*/
CAgnEntryModel::~CAgnEntryModel()
	{
	ResetRollback();
	delete iCategoryIndex;
	delete iAttachmentIndex;
	delete iTzRuleIndex;
	delete iExtractor;	
	delete iSimpleEntryTable;
	delete iAlarm;
	delete iModelStreamIdSet;
	delete iEntryManager;
	delete iCalConverter;
	iFs.Close();
	}

const CAgnServFile& CAgnEntryModel::AgnServFile()
	{
	return *iAgnServerFile;
	}

/**
 * Resets delete rollback array before the callback
 */	
void CAgnEntryModel::ResetDeleteRollbackArray()
	{
	iDeleteRollbackArray.ResetAndDestroy();
	}

/** Load up the stream network
 */
void CAgnEntryModel::DoOpenL(const TStreamId& aModelStreamId)
	{
	delete iCalConverter;
	iCalConverter = NULL;
	
	// Check if a calendar converter is needed (old version file)
	//
	TAgnVersion fileVersion;
	CalCommon::TCalFileVersionSupport status;
	iAgnServerFile->GetFileVersionSupportStatusL(fileVersion,status);

	if ( status == CalCommon::EFileNeedsConverting )
		{
		if ( iAgnServerFile->IsReadOnly() )
			{
			User::Leave(KErrAccessDenied);
			}

		iCalConverter = CalFileVersionUtils::CreateConverterL(fileVersion, *iAgnServerFile);
		iModelStreamIdSet->LoadL(StreamStore(), aModelStreamId, *iCalConverter);
		}
	else
		{
		iTzRuleIndex = CAgnTzRuleIndex::NewL(*iAgnServerFile->Dictionary(), *iAgnServerFile->StoreL());
		iModelStreamIdSet->LoadL(StreamStore(), aModelStreamId);
		//If the Calendar file needs to be converted, CheckTzDbModificationL should be called after the file conversion is completed (in method LoadNewStreamStoreL). 
		TAgnVersion curVersion = CalFileVersionUtils::CurrentFileVersion();
		if(status == CalCommon::EFileIsCurrentVersion && !(fileVersion == curVersion))
			{
			//access to here means the format of index file has been changed but not to the rest
			//Changing the index file format does not cause DC but we need to ignore the old index files
			iIndexFileIsDirty = EFalse;
			iIndexFileIsPresent = EFalse;
			// Mark the existing index file as dirty and delete them.
			// Trap the leave to keep things running, but there is nothing
			// we can do if the file can't be deleted.
			TRAP_IGNORE(MarkIndexFileAsDirtyL());
			
			//Change the version of file to be current version.
			TRAP_IGNORE(iModelStreamIdSet->ChangeFileVersionL(StreamStore(), aModelStreamId, curVersion));
			}
		}
	
	GetFileIdL();
	InternalizeNextUidValuesL();
	InternalizeEntryManagerL();
	}

void CAgnEntryModel::CheckTzDbModificationL()
	{
	if(iTzRuleIndex)
		{
		iTzRuleIndex->CheckTzDbModificationL(*iAgnServerFile);
		}
	}

/** Opens an existing model store, whose root ID is aModelStreamId in the store aStore.

@capability None
@param aStore The store in which the model's data is stored.
@param aId The root stream ID of the store aStore.

@internalComponent
*/
void CAgnEntryModel::OpenL(CStreamStore& aStore, const TStreamId& aModelStreamId) 
	{	
	Reset();
	
	iEntryManager->SetStore(aStore);
	DoOpenL(aModelStreamId);
	}

TStreamId CAgnEntryModel::CreateL(CStreamStore& aStore) 
	{
	Reset();

	iEntryManager->SetStore(aStore);

	TStreamId headStreamId = iModelStreamIdSet->CreateL(aStore, CalFileVersionUtils::CurrentFileVersion());

	// save the next unique id value to the store
	iNextLocalUidValue = 1;
	iNextAttachmentUid = 1;
	ExternalizeNextUidValuesL();

	// save the entry store object to the store
	ExternalizeEntryManagerL();
	
	TTime creationDate; 
	creationDate.UniversalTime();
	iFileId = creationDate.Int64();
	TInt threeDidgit=Math::Random() % 1000;
	iFileId = iFileId - threeDidgit;// Ensure the file ID is unique even when two files have been created at the same time.
	ExternalizeFileIdL(aStore, iModelStreamIdSet->FileInformationStreamId());
	StreamStore().CommitL();

	return (headStreamId);
	}

void CAgnEntryModel::ExternalizeFileIdL(CStreamStore& aStore, const TStreamId& aStreamId) const
	{
	RStoreWriteStream out;
	// save file id
	out.ReplaceLC(aStore, aStreamId);
	out << iFileId;
	out.CommitL();
	CleanupStack::PopAndDestroy(); //out
	}

/**
Adds aEntry to the store and returns its resulting entry id.
@capability None
*/
TAgnEntryId CAgnEntryModel::DoAddEntryL(CAgnEntry& aEntry)
	{
	// If local UID is not set or if it's already used, then the entry is assigned a new local uid.
	TBool useNextLocalUid = EFalse;
	if (aEntry.LocalUid() == 0 || iSimpleEntryTable->GetEntry(aEntry.LocalUid()) != NULL)
		{
		useNextLocalUid = ETrue;
        while (iSimpleEntryTable->GetEntry(++iNextLocalUidValue) != NULL)
            {
            }
       	aEntry.SetLocalUid(iNextLocalUidValue);
		}

	TAgnEntryId nullId;
	aEntry.SetEntryId(nullId);

	StoreExternalAttributesL(aEntry);
	
	CopyAttachmentFileToDifferentPlaceL(aEntry);

	//Add the tz rules in the entry to tz rule index before it is stored in entry manager.
	if(iTzRuleIndex)
		{
		iTzRuleIndex->AddTzRuleL(aEntry);
		}
	
	TStreamId newStreamId = iEntryManager->AddEntryL(aEntry);
	if ( newStreamId != KNullStreamId )
		{
		iModelStreamIdSet->EntryStreamIdSet().AddL(newStreamId);
		}
	
	TAgnEntryId entryId = aEntry.EntryId();
	
	if ( useNextLocalUid )
		{
		ExternalizeNextUidValuesL();			
		}
	
	iEntryManager->StoreBuffersL();
	ExternalizeEntryManagerL();
	
	UpdateIndexL(aEntry, NULL, EAdd);

	// Don't commit on add. CommitL is called from CalInterimAPI after a number have been added.
		
	return (entryId);
	}

//It could copy the attachment to a different drive if it is the same as the existing one but with a different drive name
//or, it could copy the attachment form a different calendar file folder.
void CAgnEntryModel::CopyAttachmentFileToDifferentPlaceL(CAgnEntry& aEntry)
    {
    const TInt KAttachCount = aEntry.AttachmentCount();
    for (TInt ii = 0; ii < KAttachCount; ++ii)
        {
        CAgnAttachment& attach = aEntry.Attachment(ii);
        if (attach.Uid() && attach.Type() == CCalContent::EDispositionInline)
            {
            CAgnAttachmentFile* attachFile = static_cast<CAgnAttachmentFile*>(&attach);
            if(IsAttachmentFileFromSameSessionL(attachFile->FileName()))
                {
                const TDesC& existFileName = iAttachmentIndex->FileName(attach.Uid());
                if(existFileName != KNullDesC())
                    {//There exists an attachment with same uid 
                    TDriveName oldDrive = existFileName.Left(2);
                    TDriveName newDrive = attachFile->Drive();
                    if(oldDrive.CompareF(newDrive) != 0)//drive is different
                        {
                    //Copy file to a new drive with the same name
                        HBufC* newfileName = existFileName.AllocLC();
                        newfileName->Des().Replace(0,2,newDrive);
                        iAgnServerFile->CopyFileL(existFileName, newfileName->Des());
                        attachFile->SetFileNameL(*newfileName);
                        CleanupStack::PopAndDestroy(newfileName);
                        }
                    }
                }
            else if (iAgnServerFile->FileExistsL(attachFile->FileName())) 
                { //Attachment has been copied from a different calendar session
                TParsePtrC parseOriginalFile(attachFile->FileName()); 
                HBufC* fileName = GenerateFilenameLC(parseOriginalFile.Drive(), parseOriginalFile.NameAndExt());
                iAgnServerFile->CopyFileL(attachFile->FileName(), fileName->Des());
                attachFile->SetFileNameL(fileName->Des());
                CleanupStack::PopAndDestroy(fileName);
                attachFile->SetUid(iNextAttachmentUid++);
                }
            }
        }
    }
//Find out if the attachment is copied from a different server session
TBool CAgnEntryModel::IsAttachmentFileFromSameSessionL(const TDesC& aAttachmentFile)
    {
    TBool ret = ETrue;
    if(aAttachmentFile.FindF(iAgnServerFile->PrivatePath()) != KErrNotFound)

        {//The file name is something, for example,
        //c:\private\private\10003a5c\calendar_filename_a\0\attachmentfilename
        //We need to find out the calendar name where the attachment belongs to see if it is the same file as the current Calendar file. 
        TInt lengthPath = iAgnServerFile->PrivatePath().Length();
        const TInt lengthDrive = 2;
        //tempRemovedPath1 =  calendar_filename_a\0\attachmentfilename
        TPtrC tempRemovedPath1(aAttachmentFile.Mid(lengthPath + lengthDrive));
        //tempRemovedPath2 =  calendar_filename_a
        TPtrC tempRemovedPath2(tempRemovedPath1.Left(tempRemovedPath1.Locate('\\')));
        //calName = calendar_filename
        TPtrC calName(tempRemovedPath2.Left(tempRemovedPath2.LocateReverse('_')));
        TParsePtrC parseFile(iAgnServerFile->FileName()); 
        if(calName.CompareF(parseFile.NameAndExt()) != 0)
            {
            ret = EFalse;
            }
        }
    return ret;
    }

// aEntry is the entry that has been changed. If the change is an update, then aOriginalEntry gives 
// details about the original entry.
void CAgnEntryModel::NotifyChangeL(const CAgnServerSession& aSession, CAgnEntry* aEntry, 
									 MCalChangeCallBack2::TChangeType aChangeType, CAgnInstanceInfo* aOriginalEntry)
	{
	TAgnChange change;
	change.iOperationType = aChangeType;
	
	if ( aEntry )
		{
		change.iEntryType = aEntry->Type();
		change.iEntryId = aEntry->LocalUid();
		change.iRepeatRule = aEntry->RptDef();
		change.iStartTimeOfEntryUtc = aEntry->StartTime().UtcL();
		change.iEndTimeOfEntryUtc = aEntry->ValidToTimeLocalL();
			
		if ( aOriginalEntry )
			{
			change.iOriginalRepeatRule = aOriginalEntry->RptDef();
			change.iOriginalStartTimeUtc = aOriginalEntry->StartTimeUtc();
			change.iOriginalEndTimeUtc = aOriginalEntry->EndTimeUtc();
			}
		else
			{
			change.iOriginalRepeatRule = NULL;
			change.iOriginalStartTimeUtc = Time::NullTTime();
			change.iOriginalEndTimeUtc = Time::NullTTime();
			}
		}
	else
		{
		change.iEntryType = CCalEntry::EAppt;
		change.iEntryId = 0;
		change.iStartTimeOfEntryUtc = Time::NullTTime();
		change.iEndTimeOfEntryUtc = Time::NullTTime();
		change.iRepeatRule = NULL;
		change.iOriginalStartTimeUtc = Time::NullTTime();
		change.iOriginalEndTimeUtc = Time::NullTTime();
		change.iOriginalRepeatRule = NULL;
		}
	
	change.iSession = const_cast<CAgnServerSession*>(&aSession);
	GetFileIdL();
	change.iFileId = iFileId;
	NotifySessionsOfChangeL(change);
	}

void CAgnEntryModel::NotifySessionsOfChangeL(const TAgnChange& aChange)
	{
	RPointerArray<CAgnServerSession> sessions;
	CleanupClosePushL(sessions);
	iAgnServerFile->Server().FetchSessionsL(sessions);
	const TInt count = sessions.Count();
	for ( TInt i = 0; i < count; ++i )
		{
		sessions[i]->AddChangeL(aChange);
		}
	CleanupStack::PopAndDestroy(&sessions); // sessions.Close()
	}

void CAgnEntryModel::NotifyPublishAndSubscribeL(TAgnChangeFilter& aChangeFilter)
	{
	// if publish and subcribe is enabled...	
	if ( aChangeFilter.PubSubEnabled() )
		{
		// ...get and set the Publish and Subscribe data
		TCalPubSubData calPubSubData;
		calPubSubData.iFileNameHash = iAgnServerFile->FileNameHash();
		calPubSubData.iTimeOfChangeUtc.UniversalTime(); 
		
		TPckgBuf<TCalPubSubData> calBuf(calPubSubData);
		
		// publish the update
		if ( aChangeFilter.TodoChanged() )
			{
			User::LeaveIfError(RProperty::Set(KCalPubSubCategory, ECalPubSubTodoNotification, calBuf));
			}
		else
			{
			User::LeaveIfError(RProperty::Set(KCalPubSubCategory, ECalPubSubEventNotification, calBuf));
			}
		
		aChangeFilter.SetPubSubChange(TAgnChangeFilter::ENoChange);
		}
	}

/**
Add a entry to the file. This function decides whether a new entry should be added or if an existing entry 
should be replaced. It also handles the adding of child entries.
@internalComponent
@return the TAgnEntryId of the newly added entry.
@param aEntry The entry being stored.
@param aChangeFilter Specifies the notification filter for the session adding the entry.
@leave KErrArgument If aEntry is a parent but contains a recurrence Id.
@leave KErrNotFound If aEntry is a child entry whose parent entry doesn't exist in the calendar store.
@leave KErrArgument If aEntry is a child entry with a different time mode (fixed / floating) to its parent entry.
*/
TAgnEntryId CAgnEntryModel::StoreL(CAgnEntry& aEntry, TAgnChangeFilter* aChangeFilter)
	{
	iChangeFilter = aChangeFilter;
	TAgnEntryId returnId;

	if ( aEntry.GsDataType() == CGsData::EParent )
		{
		_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Entry to be stored is a parent");)
		_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Checking if an entry with the same GUID already exists");)
		
		// it is a parent entry
		CAgnEntry* existingEntryToReplace = FetchEntryL(aEntry.Guid());

		if (existingEntryToReplace == NULL && aEntry.LocalUid() != 0)
			{
			//This is used for the case when sync a updated entry from remote sync server.
			//Some servers may filter out GUID assigned by client and only info we can get
			//to update a entry is by using its local ID.
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Attempting to fetch using localUid to fetch the entry from the server");)
			existingEntryToReplace = FetchEntryL(aEntry.LocalUid());
			}
				
		if ( existingEntryToReplace)
			{
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: An entry with the same GUID already exists");)

			CleanupStack::PushL(existingEntryToReplace);
			if (existingEntryToReplace->Type() == aEntry.Type())
				{
				_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Existing entry type matches incoming entry's type");)

				// if this entry is the same type as the existing entry, update the existing entry
				aEntry.SetEntryId(existingEntryToReplace->EntryId());
				aEntry.SetLocalUid(existingEntryToReplace->LocalUid());
				
				_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Setting Ids using the existing entry: LocalUid= %d, EntryId= %d", aEntry.LocalUid(),aEntry.EntryId().Value());)
				
			        const RArray<TGsChildRefData>& KChildIds = existingEntryToReplace->ChildIds();
			        const TInt KCount = KChildIds.Count();
			        for ( TInt ii = 0; ii < KCount; ++ii )
			            {
			            aEntry.AddChildIdL(KChildIds[ii]);
			            }

				aEntry.SetLastModifiedDate();
	
				UpdateEntryL(aEntry, iChangeFilter, ETrue);

				returnId = aEntry.EntryId();
				}
			else 
				{
				_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Existing entry types is different to incoming entry's type");)
				_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Delete the existing entry and add the incoming entry as a new one");)
				
				// if the entry is a different type, add the new one first and delete only later
                // This change is needed if the entry having attachments, it will be shared by two 
                //entries, so that attachments will not be deleted, while deleting it later. 
				returnId = AddEntryL(aEntry);
				DeleteEntryL(*existingEntryToReplace, ETrue, iChangeFilter);
				}

			CleanupStack::PopAndDestroy(existingEntryToReplace);
			}
		else
			{
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Matching existing entry not found. Adding a new entry");)
			returnId = AddEntryL(aEntry);
			}
		}
	else
		{
		_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Entry to be stored is a child");)
		//if this is a child entry, fetch the parent from the child's GUID (if present) or Local UID
		CAgnEntry* parentEntry = NULL;
		if ( aEntry.Guid() != KNullDesC8 )
			{
			parentEntry = FetchEntryL(aEntry.Guid());
			}
		else
			{
			parentEntry = FetchEntryL(aEntry.ParentId());
			}
		
		if(parentEntry==NULL)
			{
			#if defined (__CAL_BASIC_LOGGING__) || (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING)
				AgmDebug::DebugLog("StoreL: KErrNotFound: Parent entry with the GUID or localUid doesnt exist");	
			#endif
	
			User::Leave(KErrNotFound);
			}
		
		CleanupStack::PushL(parentEntry);

		if ( parentEntry->TimeMode() != aEntry.TimeMode() ||  parentEntry->Type() != aEntry.Type() )
			{
			#if defined (__CAL_BASIC_LOGGING__) || (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
				AgmDebug::DebugLog("StoreL: KErrArgument: Child entry with different time mode or type from the parent entry is not allowed");
			#endif
			
			// don't allow a child entry to be different time mode or type from the parent entry
			User::Leave(KErrArgument);
			}
		
		if ( ! aEntry.RecurrenceId().IsSet())
			{
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Recurrence ID not set on the incoming entry. Setting it using the parent");)
			aEntry.SetRecurrenceIdFromParentL(*parentEntry);
			}
		
		// We allow only repeating parents to have children that attempt to change a parent 
		// schedule.(If the user's intention is to modify a non-repeating parent, the entire
		// parent should be replaced with a new entry).
		const CAgnRptDef* KParentRptDef = parentEntry->RptDef();
		
		if(KParentRptDef==NULL)
			{
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Parent is not repeating. Only repeating parents are allowed to have children");)
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: If the user's intention is to modify a non-repeating parent, the entire parent should be replaced with a new entry");)
		
			User::Leave(KErrArgument);
			}
				
		if ( aEntry.RptDef() )
			{
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Incoming entry is a repeating child entry");)
			returnId = AddRepeatingChildEntryUpdateParentRuleL(*parentEntry, aEntry);
			}
		else	
			{	
			_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Incoming entry is a non-repeating child entry");)
			returnId = AddNonRepeatingChildEntryUpdateParentExceptionsL(*parentEntry, aEntry);						
			}
		
		CleanupStack::PopAndDestroy(parentEntry);
		}
		_DBGLOG_VERBOSE(AgmDebug::DebugLog("StoreL: Entry added successfully");)
		_DBGLOG_ENTRY(AgmDebug::DebugLogEntryL(aEntry, EDumpEntryAll);)

	return (returnId);
	}

TStreamId CAgnEntryModel::WriteDescriptorToStreamL(const TDesC& aString)
	{
	if (aString.Length() > 0)
		{
		RStoreWriteStream out;
		TStreamId id = out.CreateLC(StreamStore());
		out.WriteUint32L(aString.Length());
		out << aString;
		out.CommitL();
		CleanupStack::PopAndDestroy(); //out
		return id;
		}
	return KNullStreamId;
	}

/** 
Store those entry properties which are stored in a separate stream: summary, description, alarm action.
@capability WriteUserData
*/
void CAgnEntryModel::StoreExternalAttributesL(CAgnEntry& aEntry)
	{
	TStreamId id = WriteDescriptorToStreamL(aEntry.Description());
	aEntry.SetDescriptionStreamId(id);
	
	id = WriteDescriptorToStreamL(aEntry.Summary());
	aEntry.SetSummaryStreamId(id);

	if ( aEntry.AlarmAction() )
		{
		RStoreWriteStream out;
		id = out.CreateLC(StreamStore());
		out << *aEntry.AlarmAction();
		out.CommitL();
		CleanupStack::PopAndDestroy(); //out
		aEntry.SetAlarmActionStreamId(id);
		}
	}

void CAgnEntryModel::DeleteExternalAttributesL(CAgnEntry& aEntry)
	{
	if ( aEntry.DescriptionStreamId() != KNullStreamId )
		{
		StreamStore().DeleteL(aEntry.DescriptionStreamId());
		aEntry.SetDescriptionStreamId(KNullStreamId);
		}

	if ( aEntry.SummaryStreamId() != KNullStreamId )
		{
		StreamStore().DeleteL(aEntry.SummaryStreamId());
		aEntry.SetSummaryStreamId(KNullStreamId);
		}

	if ( aEntry.AlarmActionStreamId() != KNullStreamId )
		{
		StreamStore().DeleteL(aEntry.AlarmActionStreamId());
		aEntry.SetAlarmActionStreamId(KNullStreamId);
		}
	}

void CAgnEntryModel::UpdateExternalAttributesL(CAgnEntry& aEntry)
	{
	// 3 scenarios where we do something for updating notes text
	TStreamId id = KNullStreamId;

	switch ( aEntry.DescriptionChange() )
		{
		case CAgnEntry::EAgnDataDeleted:
			DeleteTextStreamL(aEntry.DescriptionStreamId());
			aEntry.SetDescriptionStreamId(KNullStreamId);
			break;

		case CAgnEntry::EAgnDataAdded:
			id = StoreTextL(aEntry.Description());
			aEntry.SetDescriptionStreamId(id);
			break;

		case CAgnEntry::EAgnDataUpdated:
			UpdateTextL(aEntry.Description(), aEntry.DescriptionStreamId());
			break;

		default://no changes
			break;
		}

	// 3 scenarios where we do something for updating summary text
	switch ( aEntry.SummaryChange() )
		{
		case CAgnEntry::EAgnDataDeleted:
			DeleteTextStreamL(aEntry.SummaryStreamId());
			aEntry.SetSummaryStreamId(KNullStreamId);
			break;

		case CAgnEntry::EAgnDataAdded:
			id = StoreTextL(aEntry.Summary());
			aEntry.SetSummaryStreamId(id);
			break;

		case CAgnEntry::EAgnDataUpdated:
			UpdateTextL(aEntry.Summary(), aEntry.SummaryStreamId());
			break;

		default://no changes
			break;
		}

	// 3 scenarios where we do something for updating alarm action
	switch ( aEntry.AlarmActionChange() )
		{
		case CAgnEntry::EAgnDataDeleted:
			DeleteAlarmActionStreamL(aEntry.AlarmActionStreamId());
			aEntry.SetAlarmActionStreamId(KNullStreamId);
			break;

		case CAgnEntry::EAgnDataAdded:
			id = StoreAlarmActionL(*(aEntry.AlarmAction()));
			aEntry.SetAlarmActionStreamId(id);
			break;

		case CAgnEntry::EAgnDataUpdated:
			UpdateAlarmActionL(*(aEntry.AlarmAction()), aEntry.AlarmActionStreamId());
			break;

		default://no changes
			break;
		}
	}


TAgnEntryId CAgnEntryModel::AddEntryL(CAgnEntry& aEntry)
	{
	TAgnEntryId id;

	id = DoAddEntryL(aEntry);

	NotifyingL(MCalChangeCallBack2::EChangeAdd, aEntry, NULL);

	if (iUpdateAlarm && aEntry.HasAlarm())
		{
		iAlarm->FindAndQueueNextAlarmL(EFalse);
		}
		
	return (id);
	}


// Update all the indexes which are in RAM after an entry operation (add, update, delete)
void CAgnEntryModel::UpdateIndexL(CAgnEntry& aEntry, CAgnEntry* aOldEntry, TUpdateIndex aUpdateIndex)
	{
	// set guid hash before adding / updating an entry in the GUID hash index
	if ( aEntry.GsDataType() == CGsData::EParent && ! aEntry.GuidHash() )
		{
		aEntry.SetGuidHash( GenerateHash8L(aEntry.Guid()) );
		if ( aOldEntry )
			{
			aOldEntry->SetGuidHash( aEntry.GuidHash() );					
			}				
		}	
	
 	switch( aUpdateIndex )
 		{
 		case EAdd:
			{
			AppendRollbackArrayL(aEntry, ETrue);

			AddEntryToIndexesL(aEntry);
			}
 			break;

 		case EDelete:
 			{
			AppendRollbackArrayL(aEntry, EFalse);

			iSimpleEntryTable->DeleteEntry(aEntry.EntryId()); 				

 			iCategoryIndex->DeleteEntryL(aEntry.EntryId());
 			iAttachmentIndex->DeleteEntryL(aEntry);
 			} 			
 			break;
 
 		case EUpdate:
			{
			__ASSERT_ALWAYS(aOldEntry, Panic(EAgmErrUpdateInvalid));
			AppendRollbackArrayL(*aOldEntry, EFalse);
			iSimpleEntryTable->DeleteEntry(aOldEntry->EntryId());
			
			AppendRollbackArrayL(aEntry, ETrue);
			
			AddEntryToIndexesL(aEntry);
			}
 			break;
 			
 		case EBuildIndex:
 			{
			AppendRollbackArrayL(aEntry, ETrue);
			
			if(iTzRuleIndex)
				{
				iTzRuleIndex->FetchTzRuleL(aEntry);
				}
			AddEntryToIndexesL(aEntry);
			}
 			break;
 			
 		default:
 			Panic(EAgmErrInvalidIndexUpdate);
 			break;
 		}
 		// Increment the operations counter which triggers compacting upon reaching a threshold
 	++iOperationsCounter;
	}

// Add an entry to all indexes in RAM
void CAgnEntryModel::AddEntryToIndexesL(CAgnEntry& aEntry)
	{
	aEntry.SetCollectionId(iAgnServerFile->CollectionId());
	iSimpleEntryTable->AddEntryL(aEntry);

	iAgnServerFile->CategoryList().AddEntryL(aEntry);
	iCategoryIndex->UpdateEntryL(aEntry.EntryId(), aEntry);
	iAttachmentIndex->AddEntryL(aEntry);
	}

/**
Returns true if the entry is found to have any children which are repeating.
@internalComponent
@return TBool.
@param aEntry The entry.
*/
TBool CAgnEntryModel::EntryHasRepeatingChildrenL(const CAgnEntry& aParentEntry)
	{
	const RArray<TGsChildRefData>& KChildIds = aParentEntry.ChildIds();

	const TInt KIdCount = KChildIds.Count();
	
	TBool repeatingChildExists = EFalse;
	
	for ( TInt pos = 0; ! repeatingChildExists && pos < KIdCount; ++pos )
		{
		CAgnEntry* childOfParentEntry = FetchEntryL(KChildIds[pos].ChildId());
		if (childOfParentEntry)
			{
			repeatingChildExists = (childOfParentEntry && childOfParentEntry->RptDef());

			delete childOfParentEntry;
			}
		}

	return (repeatingChildExists);
	}

/**
Deletes any non-repeating children within a specified range. This is called when adding a repeating child entry to 
delete existing non-repeating child entries - those that appear either before or after the repeating child entry's recurrence ID,
depending on the recurrence range.
@internalComponent
@param aParentEntry The parent entry
@param aRecId Indicates the recurrence ID of the repeating child entry
@param aRange Indicates "this and prior" or "this and future" to delete child entries on one side of the recurrence ID.
*/
void CAgnEntryModel::DeleteNonRepeatingChildrenOutsideRangeL(const CAgnEntry& aParentEntry, const TAgnCalendarTime& aRecId, CalCommon::TRecurrenceRange aRange)
	{
	const RArray<TGsChildRefData>& KChildIds = aParentEntry.ChildIds();

	const TInt KCount(KChildIds.Count());
	
	for ( TInt i = 0; i < KCount; ++i )
		{
		// Check if any child falls outside parent's rpt rule
		if ( (aRecId > KChildIds[i].RecurrenceId() && aRange == CalCommon::EThisAndPrior) ||
		     (aRecId < KChildIds[i].RecurrenceId() && aRange == CalCommon::EThisAndFuture) )
			{
			// Remove child if it is non-repeating
			TCalLocalUid chIdRemove(KChildIds[i].ChildId());
			CAgnEntry* chIdEntry = FetchEntryL(chIdRemove);
			if (chIdEntry)
				{
				CleanupStack::PushL(chIdEntry);
				
				__ASSERT_ALWAYS( ! chIdEntry->RptDef(), Panic(EAgmErrAddingSecondRepeatingChildEntry));
				DeleteEntryL(*chIdEntry, ETrue, iChangeFilter);
					
				CleanupStack::PopAndDestroy(chIdEntry);
				}
			}
		}
	}

/**
Adds a repeating child to a repeating parent, and performs the necessary adjustments to the parent's repeat rule.
The explanation of how the parent's rpt rule is trimmed is given in the code comments.
@internalComponent
@param aParentEntry The parent entry
@param aRepeatingChild The child to be added
@leave KErrArgument If either aParentEntry or aRepeatingChild are non-repeating
@leave KErrArgument If aRepeatingChild's RecurrenceId is aParentEntry's first or last instance
@leave KErrNotSupported If aRepeatingChild's Range is ECurrentInstance
@leave KErrArgument If aRepeatingChild's Range is an unexpected value
*/
TAgnEntryId CAgnEntryModel::AddRepeatingChildEntryUpdateParentRuleL(CAgnEntry& aParentEntry, CAgnEntry& aRepeatingChild)
	{
	// Ensure both parent and child are repeating
	__ASSERT_ALWAYS((aRepeatingChild.RptDef() && aParentEntry.RptDef()), User::Leave(KErrArgument));
	TAgnCalendarTime childRecId = aRepeatingChild.RecurrenceId();
	
	TAgnEntryId retId;
	
	CAgnRptDef* parentRptDef = aParentEntry.RptDef();
	
	TTime recIdLocal(childRecId.LocalL());
    TDateTime recIdLocalDateTime(recIdLocal.DateTime());
    	
	CalCommon::TRecurrenceRange range = aRepeatingChild.RecurrenceRange();

	if(range == CalCommon::EThisOnly)
		{
		// if the range specified is EThisOnly, attempt to recalculate the recurrence range 
		// so that the maximum number of "overlapping" entries (schedules running concurrently in the 
		// parent and child) is removed	
		TTime dtStartLocal = aParentEntry.StartTime().LocalL();
		TDateTime dtStartLocalDateTime =(dtStartLocal.DateTime()); 
	 	if (dtStartLocalDateTime.Year() == recIdLocalDateTime.Year() && 
           dtStartLocalDateTime.Month() == recIdLocalDateTime.Month() && 
           dtStartLocalDateTime.Day() == recIdLocalDateTime.Day())
			{
			// Check whether the recurrence ID of the child is of the same day as the 
            // first instance of the parent.  If so, assume range is ThisAndPrior
            // to avoid the parent repeating rule being completely trimmed.
            //
            // Note: It is possible that this is a previously trimmed parent, as a result of 
            //       the trimming, completely embedded by the child.  Therefore, no rejection of 
            //       such child/parent range is performed to ensure an Import->Export->Import will 
            //       not be failed.  In the rare occurrence where the child entry completely embeds
            //       the parent entry and has RecID on the same day as start/end of parent entry, it will 
            //       be allowed through.
			range = CalCommon::EThisAndPrior;
			}
		else 
			{
			TTime dtUntilLocal = parentRptDef->LastInstanceL().LocalL();
			
			TDateTime dtUntilLocalDateTime =(dtUntilLocal.DateTime()); 
			if (dtUntilLocalDateTime.Year() == recIdLocalDateTime.Year() && 
                dtUntilLocalDateTime.Month() == recIdLocalDateTime.Month() && 
                dtUntilLocalDateTime.Day() == recIdLocalDateTime.Day())
	  			{
 	 			// Check whether the recurrence ID of the child is of the same day as the 
                // last instance of the parent. If so, assume range is ThisAndFuture
                // instances to avoid the parent repeating rule being completely trimmed.                
                //
                // Note: It is possible that this is a previously trimmed parent, as a result of 
                //       the trimming, completely embedded by the child.  Therefore, no rejection of 
                //       such child/parent range is performed to ensure an Import->Export->Import will 
                //       not be failed.  In the rare occurrence where the child entry completely 
                //       embeds the parent entry and has RecID on the same day as start/end of parent 
                //       entry, it will be allowed through.
				range = CalCommon::EThisAndFuture;
				}
			else
				{
				// Reject the child entry if it completely alters the parent entry.  This should be done by 
    			// replacing the parent schedule instead.
    			//
    			// An exception is when the parent entry's start/end date is the same as the RecID.  That
    			// could be the result of the parent being previously trimmed.
				__ASSERT_ALWAYS(parentRptDef->LastInstanceL() > aRepeatingChild.RptDef()->LastInstanceL() ||
				              aRepeatingChild.RptDef()->FirstInstanceL() > aParentEntry.StartTime(),
                              User::Leave(KErrArgument));
   				// if the RecId is neither at the start/end of the parent's rpt range, 
    			// then base on the distance of the beginning & end of the child recurreance range 
    			// from the RecId, determine if the range should be assumed as ThisAndFuture 
    			// or a ThisAndPrior scenario.
    			// 
    			// As this usage is most likely ThisAndFuture, so even if the distance from 
    			// beginning & end of the child repeat range is the same, assume CurrentAndFuture.                                
				TTimeIntervalDays afterRecurrence = (aRepeatingChild.RptDef()->LastInstanceL().UtcL()).DaysFrom(childRecId.UtcL());
				TTimeIntervalDays beforeRecurrence = childRecId.UtcL().DaysFrom(aRepeatingChild.RptDef()->FirstInstanceL().UtcL());
				range = (afterRecurrence >= beforeRecurrence) ?
							CalCommon::EThisAndFuture : CalCommon::EThisAndPrior;
				}
			}
		aRepeatingChild.SetRecurrenceRangeL(range);
		}

	// Trim parent's repeat rule to fall in line with the new child entry,
	// and deal with any exceptions and sporadic dates that fall within the
	// affected period.

	switch (range)
		{
		case CalCommon::EThisAndFuture:
			{
			// Reject if RecurrenceId falls on 1st instance of parent's (original) rpt rule
			// If the User's intention is to modify the entire parent schedule, they should replace the
			// existing parent with a new parent entry (by submitting an entry with no RecId specified).
			TTime firstInstanceParentUtc(Time::NullTTime());
			parentRptDef->NudgeNextInstanceUtcL(aParentEntry.StartTime().UtcL(), firstInstanceParentUtc);

			// Ensure child's RecurrenceId is not the parent's first instance.
			__ASSERT_ALWAYS(childRecId.UtcL() > firstInstanceParentUtc, User::Leave(KErrArgument));

			// Store child entry
			retId = AddChildEntryL(aRepeatingChild, aParentEntry);
			childRecId = aRepeatingChild.RecurrenceId(); // recurrence ID may have been updated if it was an imported rec ID with no time
			
			// Set parent's rpt-rule end date to child's RecId (already verified RecId points to a genuine instance on the parent).
			if (parentRptDef->RRule())
				{
				parentRptDef->SetUntilTime(childRecId);
				}
			}
			break;

		case CalCommon::EThisAndPrior:
			{						
			// Reject if RecurrenceId falls on last instance of parent's (original) rpt rule
			// If the User's intention is to modify the entire parent schedule, they should replace the
			// existing parent with a new parent entry (by submitting an entry with no RecId specified).
			TTime lastInstanceParentUtc(Time::NullTTime());
			parentRptDef->NudgePreviousInstanceUtcL(parentRptDef->LastInstanceL().UtcL(), lastInstanceParentUtc);
			
			// Ensure child's RecurrenceId is not the parent's last instance.
			__ASSERT_ALWAYS(childRecId.UtcL() < lastInstanceParentUtc, User::Leave(KErrArgument));
			
			// Store child entry 
			retId = AddChildEntryL(aRepeatingChild, aParentEntry);
			childRecId = aRepeatingChild.RecurrenceId(); // recurrence ID may have been updated if it was an imported rec ID with no time

			// Move parent's start time to child's RecId (already verified RecId points to a genuine instance on the parent).
			aParentEntry.MoveStartTimeLocalL(childRecId.LocalL());
			}
			break;

		default:
			{
			User::Leave(KErrArgument);	
			}
			break;
		}
	
	// add an exception to the parent on the child's recurrence ID
	aParentEntry.RptDef()->AddExceptionL(childRecId);
	
	// Remove any exceptions on the parent that fall in the discarded range
	aParentEntry.RptDef()->PruneExceptionsL();

	// Remove any sporadic dates on the parent that fall in the discarded range
	aParentEntry.PruneRDatesL(childRecId, range);
		
	// Commit parent to store
	aParentEntry.SetLastModifiedDate();
	UpdateEntryL(aParentEntry, iChangeFilter, EFalse);
	
	// Delete any non-repeating children that fall in the discarded range
	DeleteNonRepeatingChildrenOutsideRangeL(aParentEntry, childRecId, range);
	
	return (retId);
	}
	

/**
Adds a non-repeating child to a repeating parent, and then adds an exception to the parent's exception
list for the occurrence given by the child's RecId.
@internalComponent
@param aParentEntry The parent entry
@param aNonRepeatingChild The child to be added
@leave KErrArgument If parent entry does not have a repeat definition
*/
TAgnEntryId CAgnEntryModel::AddNonRepeatingChildEntryUpdateParentExceptionsL(CAgnEntry& aParentEntry, CAgnEntry& aNonRepeatingChild)
	{
	const TInt KNumchildrenBefore = aParentEntry.ChildIds().Count();
	// Add\Update children entry
	TAgnEntryId retId = AddChildEntryL(aNonRepeatingChild, aParentEntry);
	const TInt KNumchildrenAfter = aParentEntry.ChildIds().Count();
	
	TBool addexception = !aParentEntry.RptDef()->FindException(aNonRepeatingChild.RecurrenceId());
	if ( KNumchildrenAfter > KNumchildrenBefore || addexception )
		{
		//only add exception and update parent if a child has been added otherwise a existing child has been updated.
		aParentEntry.RptDef()->AddExceptionL(aNonRepeatingChild.RecurrenceId());
		aParentEntry.SetLastModifiedDate();
		UpdateEntryL(aParentEntry, iChangeFilter, EFalse);
		}
	
	return (retId);
	}


TAgnEntryId CAgnEntryModel::AddChildEntryL(CAgnEntry& aChild, CAgnEntry& aParent)
	{
  	TAgnCalendarTime entryRecId = aChild.RecurrenceId();
 	CAgnRptDef* parentRptDef = aParent.RptDef();
 	__ASSERT_ALWAYS(parentRptDef, Panic(EAgmErrAddingChildEntryToNonRepeatingParent));
		
	// Microsoft export recurrence id with date but no time. Test if current 
	// recurrenceId is an instance of parent entry so that when the time is 
	// not specified in recurrenceId we can find the right occurence from the parent.
	if ( ! parentRptDef->IsAnInstanceL(entryRecId.LocalL()) )
		{
		// If a midnight recurrence Id is not floating, and is not an instance of the parent entry, then
		// it means it is imported without the time and timezone information. Therefore, it has to be
		// converted to a correct UTC time here before nudging the instance.
		TDateTime recIdDateTime = entryRecId.UtcL().DateTime();
		if (recIdDateTime.Hour() == 0 && recIdDateTime.Minute() == 0 && 
			aChild.TimeMode() != MAgnCalendarTimeMode::EFloating)
			{
			entryRecId.SetUtcL(parentRptDef->ConvertFromRepeatLocalToUtcL(entryRecId.UtcL()));
			}

		TTime actualInstanceTimeUtc;
		parentRptDef->NudgeNextInstanceUtcL(entryRecId.UtcL(), actualInstanceTimeUtc);
		// If recurrenceId is not an instance of the parent then nudge to next occurence if an instance can be found for the same date. 
		if ( actualInstanceTimeUtc != Time::NullTTime() )
			{
			const TTime KActualInstanceTimeRptLocal = parentRptDef->ConvertFromUtcToRepeatLocalL(actualInstanceTimeUtc);
			const TTime KRecurrenceIdRptLocal = parentRptDef->ConvertFromUtcToRepeatLocalL(entryRecId.UtcL());
			
			const TDateTime KActualInstanceDateTimeRptLocal = KActualInstanceTimeRptLocal.DateTime();
			const TDateTime KRecurrenceIdDateTimeRptLocal = KRecurrenceIdRptLocal.DateTime();
			
			// Only change recurrenceId if nudged date is the same as the recurrenceId date
			if ( KRecurrenceIdDateTimeRptLocal.Year() == KActualInstanceDateTimeRptLocal.Year() &&
				 KRecurrenceIdDateTimeRptLocal.Month() == KActualInstanceDateTimeRptLocal.Month() &&
				 KRecurrenceIdDateTimeRptLocal.Day() == KActualInstanceDateTimeRptLocal.Day() )
				{
				if (aParent.TimeMode() == MAgnCalendarTimeMode::EFloating)
					{
					entryRecId.SetFloatingL(KActualInstanceTimeRptLocal);
					}
				else
					{
					entryRecId.SetUtcL(actualInstanceTimeUtc);
					}
				aChild.UpdateRecurrenceIdL(entryRecId);
				}
			else
				{
				User::Leave(KErrNotFound); // If leave occurs here then there is no event found for given day by recurrenceId
				}
			}
		else
			{
			User::Leave(KErrNotFound); // leave occurs here because the next instance cannot be found
			}
		}
	
	aChild.SetParentId(aParent.LocalUid());
// First check if the same child entry already exists in the store (one with the same RecId).
	// If so we delete it and replace it with the new child.
	// This allows the client to 'update' a child entry without having to destroy the entire
	// associated set.

	CAgnEntry* existingChildEntry = FetchEntryL(aParent.Guid(), entryRecId);

	TAgnEntryId retId;
	TBool addChild(ETrue);

	if ( existingChildEntry )
		{			
		CleanupStack::PushL(existingChildEntry);
		if ( existingChildEntry->Type() == aChild.Type() )
			{// Update existing child entry
			aChild.SetLocalUid(existingChildEntry->LocalUid());
			aChild.SetEntryId(existingChildEntry->EntryId());
			UpdateEntryL(aChild, iChangeFilter, EFalse);
			retId = aChild.EntryId();
			addChild = EFalse;
			}
		else
			{
			// Delete existing child entry
			aChild.SetLocalUid(existingChildEntry->LocalUid());
			DeleteEntryL(*existingChildEntry, EFalse, iChangeFilter);
			}
		CleanupStack::PopAndDestroy(existingChildEntry);
		}

	if ( addChild )
		{
		if ( aChild.RptDef() )
			{
			// Check there are no other repeating childs on this parent.
			// (Limit to only ONE rpt rule change to a parent entry)
			__ASSERT_ALWAYS(!EntryHasRepeatingChildrenL(aParent), User::Leave(KErrNotSupported));
			}

		// add the new child entry
		retId = AddEntryL(aChild);
			
		// Add the occurrence given by the RecId to the parent's Exception List.
		TGsChildRefData child(aChild.LocalUid(), entryRecId);
		aParent.AddChildIdL(child);
		}

	return (retId);
	}


CAgnEntry* CAgnEntryModel::FetchEntryL(const TDesC8& aGuid) const	
 	{
 	#ifdef __CAL_VERBOSE_LOGGING__
 		{
 		TBuf<KMaxGuidBufLength> guidBuf;
		guidBuf.Copy(aGuid);
		AgmDebug::DebugLog("FetchEntryL: Fetching entry with GUID='%S'",&guidBuf);
		}
	#endif
	
 	RArray<TAgnEntryId> candidateMatches;
 	CleanupClosePushL(candidateMatches);
 	
	iSimpleEntryTable->FindByHashL(GenerateHash8L(aGuid), candidateMatches);
 	
	CAgnEntry* entry = NULL;
 	const TInt KCount = candidateMatches.Count();
 	
 	for ( TInt i = 0; i < KCount; ++i )
 		{
		CAgnEntry* candidateEntry = FetchEntryL(candidateMatches[i]);
		__ASSERT_ALWAYS(candidateEntry, User::Leave(KErrNotFound)); // entry table contains an entry not in the store
		
		CleanupStack::PushL(candidateEntry);
		
		if ( candidateEntry->Guid() == aGuid )
			{
			entry = candidateEntry;
			CleanupStack::Pop(candidateEntry);
			break;
			}

		CleanupStack::PopAndDestroy(candidateEntry);
 		}
 		
 	CleanupStack::PopAndDestroy(); //candidateMatches.Close();
 
	return (entry);
 	}


CAgnEntry* CAgnEntryModel::FindChildFromParentL(const CAgnEntry& aParent, const TAgnCalendarTime& aRecurrenceId)	const
	{
	CAgnEntry* returnEntry = NULL;
	
	// Get Child ids
	const RArray<TGsChildRefData>& KChildIds = aParent.ChildIds();

	// Check if we have a match amongst the children of this parent
	const TInt KCount = KChildIds.Count();

	for ( TInt pos = 0; pos < KCount; ++pos )
		{
		const TGsChildRefData& KChildData = KChildIds[pos];
		
		if ( KChildData.RecurrenceId() == aRecurrenceId )
			{
			returnEntry = FetchEntryL(KChildData.ChildId());

			break;
			}
		}
	
	return (returnEntry);
	}

/** Save the contents of iNextLocalUidValue and iNextAttachmentUid to the store
@internalComponent
*/
void CAgnEntryModel::ExternalizeNextUidValuesL() const
	{
	ExternalizeNextUidValuesL(StreamStore(), iModelStreamIdSet->NextLocalUidValueStreamId());
	}

void CAgnEntryModel::ExternalizeNextUidValuesL(CStreamStore& aStreamStore, const TStreamId& aStreamId) const
	{
	RStoreWriteStream stream;

	stream.ReplaceLC(aStreamStore, aStreamId);
	stream.WriteUint32L(iNextLocalUidValue);
	stream.WriteUint32L(iNextAttachmentUid);

	stream.CommitL();

	CleanupStack::PopAndDestroy();
	}

void CAgnEntryModel::InternalizeNextUidValuesL()
	{
	RStoreReadStream in;
	in.OpenLC(StreamStore(), iModelStreamIdSet->NextLocalUidValueStreamId());
	if (iCalConverter)
		{
		iCalConverter->InternalizeNextUidValuesL(in);
		}
	else
		{
		iNextLocalUidValue = in.ReadUint32L();
		iNextAttachmentUid = in.ReadUint32L();
		}

	CleanupStack::PopAndDestroy(&in);
	}
	
/** Sets whether or not to use buffered deletion.

Buffered deletion means that when entries are deleted they are only 
marked as being deleted in the internal memory buffer and the file 
is not updated until either every entry in the buffer is marked as being 
deleted or a commit/flush is called.
@param aSetting ETrue for buffered deletion, EFalse for non-buffered deletion.
@capability None
*/
void CAgnEntryModel::SetBufferedDeleting(TBool aSetting)
	{ 
	iEntryManager->SetBufferedDeleting(aSetting); 
	}

/**
Flush out the entry store
*/
void CAgnEntryModel::FlushL()
	{
	iEntryManager->FlushBuffersL();
	}

/** Resets any file specific data before opening a new calendar file.
@internalAll
*/
void CAgnEntryModel::Reset()
	{
	iFileId = 0;
	
	if (iModelStreamIdSet)
		{
		iModelStreamIdSet->Reset();
		}
	
	if (iEntryManager)
		{
		iEntryManager->Reset();
		}
		
	if (iSimpleEntryTable)
		{
		iSimpleEntryTable->Reset();
		}
	}

/** Save the entry manager and its data to the store
@internalAll
*/
void CAgnEntryModel::ExternalizeEntryManagerL() const
	{
	RStoreWriteStream stream;
	stream.ReplaceLC(StreamStore(),iModelStreamIdSet->EntryManagerStreamId());
	stream << *iEntryManager;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	}

void CAgnEntryModel::InternalizeEntryManagerL()
	{
	RStoreReadStream in;
	in.OpenLC(StreamStore(), iModelStreamIdSet->EntryManagerStreamId());
	in >> *iEntryManager;

	// Sanity Check here. If the entry stream set is empty, then the store stream ids should
	// all be zero - reset them here to ensure that if the file has been partially corrupted, we can
	// recover from this.
	if ( iModelStreamIdSet->EntryStreamIdSet().Count() == 0 )
		{
		iEntryManager->Reset();
		}

	CleanupStack::PopAndDestroy(&in);
	}

void CAgnEntryModel::ResetIndexes()
	{
	// Reset indexes, returns EFalse if file is empty
	iSimpleEntryTable->Reset();
	iCategoryIndex->Reset();
	iAttachmentIndex->Reset();
	iModelStreamIdSet->EntryStreamIdSet().ResetIteratorToStart();
	if (iModelStreamIdSet->EntryStreamIdSet().Count() == 0)
		{
		iNumStreamsProcessed = 0;
		}
	}

// This method is used for generating a filename for the index file from the
// calendar filename. 
TBool CAgnEntryModel::GenerateIndexFileName(TFileName& aFileName) 
	{
	aFileName = FileName();
	if ((aFileName.Length() + KIdxFilePostFixLength) > KMaxFileName)
		{
		iIndexFileIsDirty = ETrue;
		iIndexFileIsPresent = EFalse;
		return EFalse;
		}
	aFileName.Append(KIdxFilePostFix);
	return ETrue;
	}


// This method marks the index file as dirty (i.e. out of sync with the indices 
// in RAM) by deleting it. A flag is kept internally to allow us to know that the
// file needs to be rebuilt and to no try to delete the file more than once.
void CAgnEntryModel::MarkIndexFileAsDirtyL()
	{

    if(IsIndexFileAvailableL())
        {		
            TFileName idxfilename;
            if (!GenerateIndexFileName(idxfilename))
                {
                User::Leave(KErrBadName);
                }           
            iFs.Delete(idxfilename); // ignore return as there is nothing we can do with it           
        }        
	iIndexFileIsDirty = ETrue;
	}
	
// This method allows clients of the model to determine if the index file is
// dirty and therefore in need of being rewritten with the current data.
TBool CAgnEntryModel::IsIndexFileDirty() const
	{
	return iIndexFileIsDirty;
	}

TCalCollectionId CAgnEntryModel::CollectionId() const
	{
	return iAgnServerFile->CollectionId();
	}

// This method reads the indices from the index file.
// It returns:
//		ETrue - indices successfully read from file
//      EFalse - indices not read from file (file may be missing, or there
//				 may have been errors trying to read the file.
// It Leaves when any of the index InternalizeL functions Leave.
// Overall description:
//		1. Attempt to open the file
//		2. If file is present InternalizeL all indices and return true
//		4. If no file, or errors in opening or streaming return false
TBool CAgnEntryModel::LoadIndexFileL()
	{
	TFileName idxfilename;
	if (!GenerateIndexFileName(idxfilename))
		{
		User::Leave(KErrBadName);
		}
	
	RFile idxFile;
	TInt errReadIdx = idxFile.Open(iFs, idxfilename, EFileRead);
	CleanupClosePushL(idxFile);
	
	if (errReadIdx == KErrNone)    // we have a file
		{
		RFileReadStream idxStream;
		idxStream.Attach(idxFile);
		CleanupClosePushL(idxStream);
		
		TInt internalizeErr = KErrNone;
		
		TRAP(internalizeErr, iSimpleEntryTable->InternalizeL(idxStream, iTzRuleIndex));
		if (internalizeErr != KErrNone)
			{
			// clear any entries that may have been added to the table
			// before leaving
			iSimpleEntryTable->Reset();
			User::Leave(internalizeErr);
			}
		TRAP(internalizeErr, iCategoryIndex->InternalizeL(idxStream));
		if (internalizeErr != KErrNone)
			{
			// clear any entries in this index or the entry table
			// before leaving
			iSimpleEntryTable->Reset();
			iCategoryIndex->Reset();
			User::Leave(internalizeErr);
			}
		TRAP(internalizeErr, iAttachmentIndex->InternalizeL(idxStream));
		if (internalizeErr !=KErrNone)
			{
			// clear any entries in this index, the category index
			// and the entry table before leaving
			iAttachmentIndex->Reset();
			iSimpleEntryTable->Reset();
			iCategoryIndex->Reset();
			User::Leave(internalizeErr);
			}

		CleanupStack::PopAndDestroy(2); //idxStream, idxFile
		iIndexFileIsDirty = EFalse;
		return ETrue;  // we have successfully read the index file
		}
	else if (errReadIdx == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(); // idxFile
		iIndexFileIsDirty = ETrue;  // the index file needs to be created/updated
		iIndexFileIsPresent = EFalse; 	// so we won't try to find the file every time
										// DoBuildIndexL() is called
		return EFalse; // no file to read
		}
		
	// if we get here, then there was an error reading the file for some
	// reason other than it not being present. We'll mark it as DIRTY (i.e. delete it).
	// MarkIndexFileAsDirtyL will try to delete the file if errors occur.
	CleanupStack::PopAndDestroy(&idxFile);
	MarkIndexFileAsDirtyL();
	iIndexFileIsPresent = EFalse;	
	return EFalse;
	}

// This method check the index file is exist.
// It returns:
//      ETrue - File is already exist
//      EFalse - File is not available
TBool CAgnEntryModel::IsIndexFileAvailableL()
    {
    TFileName idxfilename;
    if (!GenerateIndexFileName(idxfilename))
        {
        User::Leave(KErrBadName);
        }
   
    RFile idxFile;
    TInt errReadIdx = idxFile.Open(iFs, idxfilename, EFileRead);
    CleanupClosePushL(idxFile);
    if((errReadIdx == KErrNotFound))
        {
            CleanupStack::PopAndDestroy();
            return EFalse;
        }
    CleanupStack::PopAndDestroy(&idxFile);
    return ETrue;
}

// This method attempts to save all indices to the index file.
// If any errors are encountered it will Leave.
// Clients of this method should TRAP the Leave and possibly
// mark the file as dirty or try to delete it.
void CAgnEntryModel::SaveIndexFileL()
	{
	TFileName idxfilename;
	if (!GenerateIndexFileName(idxfilename))
		{
		User::Leave(KErrBadName);
		}
	
	RFile idxFile;
	TInt errWriteIdx = idxFile.Replace(iFs, idxfilename, EFileWrite);
	User::LeaveIfError(errWriteIdx);
	CleanupClosePushL(idxFile);
	
	
	RFileWriteStream idxStream;
	idxStream.Attach(idxFile);
	CleanupClosePushL(idxStream);

	iSimpleEntryTable->ExternalizeL(idxStream);
	iCategoryIndex->ExternalizeL(idxStream);
	iAttachmentIndex->ExternalizeL(idxStream);

	CleanupStack::PopAndDestroy(2); //idxStream, idxFile
	
	iIndexFileIsDirty = EFalse;
	}
	
TBool CAgnEntryModel::DoLoadIndexFile()
	{
	// Check to see if we have a valid index file that we can read
	TBool readPassed = EFalse;
	TRAPD(idxErr, readPassed = LoadIndexFileL());
	if ((readPassed) && (idxErr == KErrNone))
		{
		// We successfully read the prebuilt index.
		return ETrue;   
		}
	else
		{
		// something bad happened to the index file
		// we need to delete it because it couldn't be read
		// To ensure that it is deleted we need to mark the index 
		// file as "not dirty".
		iIndexFileIsDirty = EFalse;
		iIndexFileIsPresent = EFalse;
		// trap the leave to keep things running, but there is nothing
		// we can do if the file can't be deleted.
		TRAP_IGNORE(MarkIndexFileAsDirtyL());
		}
	return EFalse;
	}


TInt CAgnEntryModel::DoIndexBuildStepL()
	{
	// Check to see if we have a valid index file that we can read
	// before trying to build all the indices.
	
	if (iIndexFileIsPresent)
		{
		if (DoLoadIndexFile())
			{
			// We successfully read the prebuilt index.
			// There is no need to go any further.
			// The 0 below indicates that there is nothing left to do.
			return KAgnPercentageComplete;   
			}
		}
	// otherwise, there is no file or the file is dirty (out of sync), 
	// continue to build indexes
	
	TInt retVal = 0;

	FOREVER
		{
		TStreamId streamId(0);

		if ( ! iModelStreamIdSet->EntryStreamIdSet().At(streamId) ) // returns value in streamId
			{
			retVal = KAgnPercentageComplete; // indicate completion if not more streams
			break;
			}

		RStoreReadStream in;
		in.OpenLC(StreamStore(), streamId);
		
		in.ReadInt8L();		// discard buffer type information
		
		if ( iCalConverter )
			{
			// Read entry from a calendar file whose
			// version is not the current one.
			
			iCalConverter->InternalizeEntriesL(in);
			}
		else
			{	
			// Read entry from a calendar file - current version
			const TUint8 KCount = in.ReadUint8L();
			
			CAgnEntry* entry = NULL;	
			for ( TInt ii = 0; ii < KCount; ++ii )
				{
				entry =	CAgnEntry::NewL(in);
				CleanupStack::PushL(entry);
				
				UpdateIndexL(*entry, NULL, EBuildIndex);
				CleanupStack::PopAndDestroy(entry);
				}
			}

		CleanupStack::PopAndDestroy();		// in

		iSimpleEntryTable->Commit();

		if ( ! iModelStreamIdSet->EntryStreamIdSet().Next() )
		 	{
		 	retVal = KAgnPercentageComplete; // no more streams to process
			break;
			}

		// check iNumStreamsProcessed is valid - this number is used to calculate percentage complete
		++iNumStreamsProcessed;
		__ASSERT_ALWAYS(iNumStreamsProcessed <= iModelStreamIdSet->EntryStreamIdSet().Count(), User::Leave(KErrCorrupt));

		// After every second stream is processed, calculate the percentage complete and return
		if ( iNumStreamsProcessed % 2 == 0 )
			{
			// coverity[check_return] coverity[unchecked_value]
			TInt percentage = (iNumStreamsProcessed * KAgnPercentageComplete) / iModelStreamIdSet->EntryStreamIdSet().Count();
			retVal = (percentage < 1 ? 1 : percentage); // percentage complete must be at least 1 (returning 0 indicates index building complete)
			break;
			}
		}

	if (retVal == KAgnPercentageComplete)
		{
		// A return value of 0 indicates that the indexes have been
		// completely built. We will save them to file now so that
		// no future errors will cause this information to need to 
		// be built again.
		TRAPD (saveErr, SaveIndexFileL());
		if (saveErr != KErrNone)
			{
			// We couldn't save the index file, so we'll mark it as dirty
			TRAPD(ignore,MarkIndexFileAsDirtyL());
			User::LeaveIfError(ignore);			
			}
		}
	
	return (retVal);
	}


void CAgnEntryModel::BuildIndexCompleteL()
	{
	if ( iCalConverter )
		{
		RestoreCategoriesL();
		iCalConverter->CompleteConversionL();
		delete iCalConverter;
		iCalConverter = NULL;
		}
		
	CreateAlarmForServerL();
	iAlarm->DeleteAllAlarms();
	iAlarm->FindAndQueueNextAlarmL(EFalse);
	}

 
TInt CAgnEntryModel::MatchExactText(const TDesC& aTextField, const TDesC& aSearchText)
	{
	return aTextField.Match(aSearchText);
	}

TInt CAgnEntryModel::MatchFoldedText(const TDesC& aTextField, const TDesC& aSearchText)
	{
	return aTextField.MatchC(aSearchText);
	}

TBool CAgnEntryModel::MatchSearchTextL(MatchTextFnPtr aMatchTextFunction, CAgnEntry& aEntry, const TDesC& aSearchText, const TAgnFilter& aFilter)
	{ 
	// always search summary
	TInt pos = aMatchTextFunction(aEntry.Summary(), aSearchText);

	if ( pos == KErrNotFound && aFilter.IsEntryLocationSearched() )
		{
		pos = aMatchTextFunction(aEntry.Location(), aSearchText);
		}
	if ( pos == KErrNotFound && aFilter.IsEntryDescriptionSearched() )
		{
		pos = aMatchTextFunction(aEntry.Description(), aSearchText);
		}

	CAgnAttendee* organizer = aEntry.Organizer();
	const TInt KNumAttendees = aEntry.AttendeeCount();

	if ( organizer && pos == KErrNotFound )
		{
		if ( aFilter.IsOrganizerAddressSearched() )
			{
			pos = aMatchTextFunction(organizer->Address(), aSearchText);
			}
		if ( pos == KErrNotFound && aFilter.IsOrganizerSentByAddressSearched() )
			{
			pos = aMatchTextFunction(organizer->SentBy(), aSearchText);
			}
		if ( pos == KErrNotFound && aFilter.IsOrganizerCommonNameSearched() )
			{
			pos = aMatchTextFunction(organizer->CommonName(), aSearchText);
			}
		}
		
	for ( TInt i = 0; pos == KErrNotFound && i < KNumAttendees; ++i)
		{
		CAgnAttendee& attendee = aEntry.FetchAttendee(i);

		if ( aFilter.IsAttendeeAddressSearched() )
			{
			pos = aMatchTextFunction(attendee.Address(), aSearchText);
			}
		if ( pos == KErrNotFound && aFilter.IsAttendeeSentByAddressSearched() )
			{
			pos = aMatchTextFunction(attendee.SentBy(), aSearchText);
			}
		if ( pos == KErrNotFound && aFilter.IsAttendeeCommonNameSearched() )
			{
			pos = aMatchTextFunction(attendee.CommonName(), aSearchText);
			}
		}

	return (pos >= 0);
	}

TBool CAgnEntryModel::MatchFullEntryL(const TAgnEntryId& aEntryId, const TFindInstanceParams& aSearchParams)
	{
	TBool match(ETrue);
	if(aSearchParams.iSearchString.Length() > 0)
		{
		CAgnEntry* entry = FetchEntryL(aEntryId);
		__ASSERT_ALWAYS(entry, User::Leave(KErrNotFound));

		CleanupStack::PushL(entry);
		if(!MatchSearchTextL(*entry, aSearchParams.iSearchString, aSearchParams.iFilter))
			{
			match = EFalse;
			}
		CleanupStack::PopAndDestroy(entry);
		}
	return match;
	}

TBool CAgnEntryModel::MatchSearchTextL(CAgnEntry& aEntry, const TDesC& aSearchText, const TAgnFilter& aFilter)
	{
	TBool matchText = ETrue;
	
	if ( aSearchText.Length() > 0 )
		{
		TBuf<256> searchString;
		_LIT(KWildCard, "*");
		searchString.Append(KWildCard);
		searchString.Append(aSearchText);
		searchString.Append(KWildCard);
		
		if ( aEntry.Summary() == KNullDesC && aEntry.SummaryStreamId() != KNullStreamId )
			{
			HBufC* summary = RestoreTextL(aEntry.SummaryStreamId());
			aEntry.SetSummary(summary);			
			}
			
		// load description if required
		if ( aEntry.Description() == KNullDesC && aEntry.DescriptionStreamId() != KNullStreamId && aFilter.IsEntryDescriptionSearched() )
			{
			HBufC* description = RestoreTextL(aEntry.DescriptionStreamId());
			aEntry.SetDescription(description);			
			}

		MatchTextFnPtr matchFn = &MatchFoldedText;
		if ( aFilter.IsExactTextOnlySearch() )
 			{
 			matchFn = &MatchExactText;
 			}
 		
 		matchText = MatchSearchTextL(matchFn, aEntry, searchString, aFilter);
		}
		
	return (matchText);
	}

void CAgnEntryModel::FindInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aParameters)
	{
	iExtractor->FindInstancesL(aInstances, aParameters);
	
#if defined (__CAL_INSTANCE_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
	LogInstanceSearchL(aParameters, aInstances);
#endif	
	}

void CAgnEntryModel::LogInstanceSearchL(const TFindInstanceParams& aParameters, const CArrayFix<TAgnSortInstance>& aInstances) const
	{
	TAgnFilter iFilter;
	TTime dbgStartTime;
	TTime iEndTime;
	TBuf<KAgnMaxSearchStringLength> iSearchString;
	
	TAgnFilter debugFilter = aParameters.iFilter;
	
	TBuf<64> filterBuf;
	
	if (debugFilter.AreTimedApptsIncluded() && 
		debugFilter.AreRemindersIncluded() &&
		debugFilter.AreEventsIncluded() && 
		debugFilter.AreAnnivsIncluded() && 
		debugFilter.AreCompletedTodosIncluded() && 
		debugFilter.AreIncompletedTodosIncluded() )
		{
		filterBuf.Copy(_L("All entries"));
		}
	else
		{
		if (debugFilter.AreTimedApptsIncluded())
			{
			filterBuf.Append(_L("Appts,"));
			}
		if (debugFilter.AreRemindersIncluded())
			{
			filterBuf.Append(_L("Reminders,"));
			}
		if (debugFilter.AreEventsIncluded())
			{
			filterBuf.Append(_L("Events,"));
			}
		if (debugFilter.AreAnnivsIncluded())
			{
			filterBuf.Append(_L("Annivs,"));
			}
		if (debugFilter.AreCompletedTodosIncluded() && debugFilter.AreIncompletedTodosIncluded())
			{
			filterBuf.Append(_L("TODOs,"));
			}
		else if (debugFilter.AreIncompletedTodosIncluded())
			{
			filterBuf.Append(_L("Incomplete TODOs,"));
			}
		else if (debugFilter.AreCompletedTodosIncluded())
			{
			filterBuf.Append(_L("Complete TODOs,"));
			}
		}
	
	if (aParameters.iSearchString.Length() > 0)
		{
		TBuf<KAgnMaxSearchStringLength> searchBuf;
		searchBuf.Copy(aParameters.iSearchString);
		AgmDebug::DebugLog("Searching for text %S", &searchBuf);
		}
	
	TBuf<KMinTTimeStrLength> startTimeBuf;
	TBuf<KMinTTimeStrLength> endTimeBuf;
	
	AgmDebug::TTimeStrL(aParameters.iRangeStart.LocalL(),startTimeBuf);
	AgmDebug::TTimeStrL(aParameters.iRangeEnd.LocalL(),endTimeBuf);
	
	const TInt KInstanceCount(aInstances.Count());
	
	AgmDebug::DebugLog("FindInstancesL: Range: Start - %S, End - %S, Filter '%S'", &startTimeBuf, &endTimeBuf, &filterBuf);
	AgmDebug::DebugLog("Found %d instances", KInstanceCount);
	
	for ( TInt i = 0; i < KInstanceCount; ++i )
		{
		TBuf<KMinTTimeStrLength> instanceTimeBuf;
		AgmDebug::TTimeStrL(aInstances[i].InstanceIdL().Date().LocalL(), instanceTimeBuf);
		AgmDebug::DebugLog("Found instance: %S", &instanceTimeBuf);
		}

	}

void CAgnEntryModel::CreateAlarmForServerL()
	{
	if ( ! iAlarm )
		{
		iAlarm = CAgnAlarm::NewL(this, NULL);
		}
	}

/**
Get list of ids of alarmed entries in the next 60 days.
FindInstanceL is used to find instances of alarmed entries.
*/
void CAgnEntryModel::NextAlarmForServerL(const TTime& aNow, CArrayFixFlat<TAgnSortInstance>& aAlarmedIds)
	{
	if ( ! AgnDateTime::IsValidAgendaTTime(aNow) || ! iSimpleEntryTable || iAgnServerFile->IsFileDisabled())
		{
		return;
		}
	
	CArrayFixSeg<TAgnSortInstance>* dayInfoList = new(ELeave) CArrayFixSeg<TAgnSortInstance>(4);
	CleanupStack::PushL(dayInfoList);

	TFindInstanceParams searchParams;
	searchParams.iUndatedTodoTimeLocal = aNow;
    searchParams.iFilter = TAgnFilter(CalCommon::EIncludeAppts|CalCommon::EIncludeReminder|CalCommon::EIncludeEvents|
										CalCommon::EIncludeAnnivs|CalCommon::EIncludeIncompletedTodos|CalCommon::EIncludeAlarmedOnly, CalCommon::EFoldedTextSearch);
    searchParams.iSearchString = KNullDesC();
    searchParams.iRangeStart.SetLocalL(AgnDateTime::ResetToMidnight(aNow) - TTimeIntervalDays(1)); // alarms can be up to 24 hours after start time so check this
    searchParams.iRangeEnd.SetLocalL(aNow + TTimeIntervalDays(2));
        
	iExtractor->FindInstancesL(*dayInfoList, searchParams);
	TAgnDaySortKey sortKey(AgnDateTime::MaxDate(), searchParams.iUndatedTodoTimeLocal);
	dayInfoList->Sort(sortKey);

	
    searchParams.iFilter = TAgnFilter(CalCommon::EIncludeAppts|CalCommon::EIncludeReminder|CalCommon::EIncludeEvents|
			CalCommon::EIncludeAnnivs|CalCommon::EIncludeIncompletedTodos|CalCommon::EIncludeAlarmedOnly|
			CalCommon::EIncludeRptsNextInstanceOnly, CalCommon::EFoldedTextSearch);
	
	// check the next month if no alarmed instances found
	const TTime KLimit = aNow + TTimeIntervalDays(60);

	while ( dayInfoList->Count() == 0 && searchParams.iRangeStart.LocalL() < KLimit )
 		{
	    searchParams.iRangeStart.SetLocalL(searchParams.iRangeEnd.LocalL());
	    searchParams.iRangeEnd.SetLocalL(searchParams.iRangeStart.LocalL() + TTimeIntervalDays(10));
	    
	    iExtractor->FindInstancesL(*dayInfoList, searchParams);
	    TAgnDaySortKey sortKey1(AgnDateTime::MaxDate(), searchParams.iUndatedTodoTimeLocal);
		dayInfoList->Sort(sortKey1);
		}

	UpdateAlarmListL(aAlarmedIds, *dayInfoList, aNow);

	CleanupStack::PopAndDestroy(); //dayInfoList 
	}

/**
Examine the contents of aDayInfoList and see if the any of the contained alarm instances should be added to
or replace the contents of aAlarmedIds
@internalComponent
*/

void CAgnEntryModel::UpdateAlarmListL(CArrayFixFlat<TAgnSortInstance>& aAlarmedIds,CArrayFixSeg<TAgnSortInstance>& aDayInfoList, const TTime& aNow)
	{
	TTime nextAlarmLocal(AgnDateTime::MaxDate());
	
	for ( TInt ii = aDayInfoList.Count() - 1; ii >= 0; --ii )
		{
		TAgnSortInstance sortInstance = aDayInfoList[ii];
		
		if ( sortInstance.SimpleEntry().Type() == CCalEntry::ETodo || sortInstance.iStartTimeLocal >= AgnDateTime::ResetToMidnight(aNow) )
			{
			TTime alarmTimeLocal(sortInstance.InstanceAlarmDateTime());
	
			if ( alarmTimeLocal <= aNow && sortInstance.SimpleEntry().RptDef() )
				{				
				TTime nextInstance;
				
				while ( sortInstance.SimpleEntry().RptDef()->NudgeNextInstanceL(sortInstance.InstanceDate(), nextInstance, ETrue) && nextInstance <= aNow)
					{
					sortInstance.SetL(nextInstance, aNow);
									
					if ( sortInstance.iStartTimeLocal < AgnDateTime::MaxDate() )
						{
						alarmTimeLocal = sortInstance.InstanceAlarmDateTime();
						}
					}
				}
						
			if ( AgnDateTime::IsValidAgendaTTime(alarmTimeLocal) )
				{
				if ( alarmTimeLocal > aNow && alarmTimeLocal < nextAlarmLocal )
					{
					aAlarmedIds.Reset();
					aAlarmedIds.AppendL(sortInstance);
					nextAlarmLocal = alarmTimeLocal;
					}
				else
					{
					if ( alarmTimeLocal == nextAlarmLocal )
						{
						aAlarmedIds.AppendL(sortInstance);
						}
					}
				}
			}
		}
	}

/** Schedules a list of alarms whose dateTime meets the following criteria:
alarmTime >= aCurrentTime AND alarmTime <= aCurrentTime + 30 days
@internalComponent
*/
 void CAgnEntryModel::NextFewAlarmsForServerL(const TTime& aStartDateTime,const TTime& aEndDateTime,
											 CArrayFixFlat<TAgnSortInstance>& aAlarmedIds,const TInt aMaxNumberOfAlarms)
	{
	if(iAgnServerFile->IsFileDisabled())
	    {
	    return;
	    }
	if ( AgnDateTime::IsValidAgendaTTime(aStartDateTime) && AgnDateTime::IsValidAgendaTTime(aEndDateTime) &&
		 aEndDateTime>=aStartDateTime )
		{					
		CArrayFixSeg<TAgnSortInstance>* dayInfoList = new(ELeave) CArrayFixSeg<TAgnSortInstance>(4);
		CleanupStack::PushL(dayInfoList);

		TFindInstanceParams searchParams;
		searchParams.iUndatedTodoTimeLocal = searchParams.iRangeStart.LocalL();
	    searchParams.iFilter = TAgnFilter(CalCommon::EIncludeAppts|CalCommon::EIncludeReminder|CalCommon::EIncludeEvents|
											CalCommon::EIncludeAnnivs|CalCommon::EIncludeIncompletedTodos|CalCommon::EIncludeAlarmedOnly|
											CalCommon::EIncludeRptsNextInstanceOnly, CalCommon::EFoldedTextSearch);
	    searchParams.iSearchString = KNullDesC();
	    searchParams.iRangeStart.SetLocalL(aStartDateTime - TTimeIntervalDays(1)); // alarms can be up to 24 hours after start time so check this
	    searchParams.iRangeEnd.SetLocalL(aEndDateTime);
	    
		iExtractor->FindInstancesL(*dayInfoList, searchParams);
		TAgnAlarmSortKey aKey;
		dayInfoList->Sort(aKey);

		AddToAlarmListL(aAlarmedIds, *dayInfoList, aStartDateTime, aEndDateTime, aMaxNumberOfAlarms);

		CleanupStack::PopAndDestroy(dayInfoList);
		}
	}


void CAgnEntryModel::AddToAlarmListL(CArrayFixFlat<TAgnSortInstance>& aAlarmedIds,CArrayFixSeg<TAgnSortInstance>& aDayInfoList, const TTime& aStartDateTime,
									 const TTime& aEndDateTime,const TInt )
//
// Examine the contents of aDayInfoList add to aAlarmedIds
//
	{
	const TInt KDayListCount(aDayInfoList.Count());
	for (TInt i = 0; i < KDayListCount; ++i)
		{
 		const TAgnSortInstance KSortInstance = aDayInfoList[i];
 		const TTime KAlarmInstance(KSortInstance.InstanceAlarmDateTime());

		if (KAlarmInstance > aStartDateTime && KAlarmInstance <= aEndDateTime)
			{
			aAlarmedIds.AppendL(KSortInstance);						
			}
 		}
	}

void CAgnEntryModel::FindAndQueueNextFewAlarmsL()
	{
	if ( iAlarm )
		{
		iAlarm->FindAndQueueNextFewAlarmsL();
		}
	}

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
void CAgnEntryModel::DeleteAlarmsAndRequeueSessionAlarmL()
	{
	if ( iAlarm )
		{
		iAlarm->DeleteAlarmsAndRequeueSessionAlarmL();
		}
	}
#endif

/**
Return next time (from aStartDate) on which an instance exists
@internalComponent
*/
void CAgnEntryModel::NextPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams) const
	{
	iExtractor->NextPossibleInstancesL(aInstances, aSearchParams);
	}
/**
Return previous time (from aStartDate) on which an instance exists
@internalComponent
*/
void CAgnEntryModel::PreviousPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams) const
	{
	iExtractor->PreviousPossibleInstancesL(aInstances, aSearchParams);
	}


// Place the uids of entries that have a last changed data greater than aDate and which meet the selection
// criteria specified in aFilter into the aUids array.
//
void CAgnEntryModel::GetEntryUidsSinceDateL(const TTime& aTime, RArray<TCalLocalUid>& aUniqueIdList)
	{
	iSimpleEntryTable->FindByLastModifiedDateUtcL(aTime, aUniqueIdList);		
	}


/**
Get the file ID of the currently open Agenda file.  
This is unique to the file.
@capability None
*/
const TInt64& CAgnEntryModel::GetFileIdL()
	{
	if ( iFileId == 0 )
		{
		RStoreReadStream in;
		in.OpenLC(StreamStore(),iModelStreamIdSet->FileInformationStreamId());
		TInt64 fileId=0;
		in >> fileId;
		CleanupStack::PopAndDestroy(); //in
		iFileId = fileId;
		}
		
	return iFileId;
	}


HBufC* CAgnEntryModel::RestoreTextL(const TStreamId& aStream)
	{
	CStreamStore& store = StreamStore();
	RStoreReadStream in;
	in.OpenLC(store, aStream);
	TInt textLength = in.ReadUint32L();
	HBufC* text = HBufC::NewL(in, textLength);
	CleanupStack::PopAndDestroy(); //in
	return text;
	}

TStreamId CAgnEntryModel::StoreTextL(const TDesC& aText)
	{
	CStreamStore& store = StreamStore();
	RStoreWriteStream out;
	TStreamId id = out.CreateLC(store);
	out.WriteUint32L(aText.Length());
	out << aText;
	out.CommitL();
	CleanupStack::PopAndDestroy(); //out
	return id;
	}

/*
 * Updates the notes text stored in the specified stream. Null Descriptors are expected to be 
 * handled client side
 */
void CAgnEntryModel::UpdateTextL(const TDesC& aText, const TStreamId& aStream)
	{
	__ASSERT_DEBUG(aText.Length() != 0, Panic(EAgmNullDescriptor));

	CStreamStore& store = StreamStore();
	RStoreWriteStream out;
	out.ReplaceLC(store, aStream);
	out.WriteUint32L(aText.Length());
	out << aText;
	out.CommitL();
	CleanupStack::PopAndDestroy(); //out
	}

/*
 * Deletes the specified stream holding notes data.
 * 
 */
void CAgnEntryModel::DeleteTextStreamL(const TStreamId& aStream)
	{
	StreamStore().DeleteL(aStream);
	}	
	
	
CAgnContent* CAgnEntryModel::RestoreAlarmActionL(const TStreamId& aStream)
	{
	CStreamStore& store = StreamStore();
	RStoreReadStream in;
	in.OpenLC(store, aStream);

	CAgnContent* alarmAction = new (ELeave) CAgnContent;
	CleanupStack::PushL(alarmAction);
	in >> *alarmAction;
	CleanupStack::Pop(alarmAction);

	CleanupStack::PopAndDestroy(); //in
	return alarmAction;
	}


TStreamId CAgnEntryModel::StoreAlarmActionL(const CAgnContent& aAlarmAction)
	{
	CStreamStore& store = StreamStore();
	RStoreWriteStream out;
	TStreamId id = out.CreateLC(store);
	out << aAlarmAction;
	out.CommitL();
	CleanupStack::PopAndDestroy(); //out

	return id;
	}

/*
 * Updates the rich alarm data stored in the specified stream.  
 */
void CAgnEntryModel::UpdateAlarmActionL(const CAgnContent& aAlarmAction, const TStreamId& aStream)
	{
	CStreamStore& store = StreamStore();
	RStoreWriteStream out;
	out.ReplaceLC(store, aStream);
	out << aAlarmAction;
	out.CommitL();
	CleanupStack::PopAndDestroy(); //out
	}

/*
 * Deletes the specified stream holding rich alarm data.
 * 
 */
void CAgnEntryModel::DeleteAlarmActionStreamL(const TStreamId& aStream)
	{
	StreamStore().DeleteL(aStream);
	}

const TDesC& CAgnEntryModel::FileName() const
	{
	return iAgnServerFile->FileName();
	}

void CAgnEntryModel::SetUpdateAlarmL(TBool aUpdateAlarm)
	{
	iUpdateAlarm = aUpdateAlarm;
	
	if ( iUpdateAlarm )
		{
		iAlarm->FindAndQueueNextAlarmL(EFalse);
		}
	}


/** Commits any changes both to file and internally that have occurred to the model. 

This function does not empty the buffers. Call FlushL to do that.
@capability None
*/
void CAgnEntryModel::DoCommitL()
	{ 
	
	if(!AgnServFile().IsBackupRestoreLock())
		{
		iModelStreamIdSet->CommitL(StreamStore());
		StreamStore().CommitL();
		iSimpleEntryTable->Commit();
		iAttachmentIndex->CommitL(*iAgnServerFile);
		}
	
	// Trigger compacting after a certain number of operations on the model
	if(iOperationsCounter >= KCompactOperationsThreshold)
		{
		// Initiate synchronous compact
		iAgnServerFile->CompactFileL();
		// Reset operations counter
		iOperationsCounter=0;
		}
	}

// Commits all changes to file.
void CAgnEntryModel::CommitL()
	{
	DoCommitL();
	ResetRollback();
	}

// Called after multiple entries have been deleted.
// This can fail at any time and must roll back, so notification cannot happen until the changes are committed to file.
// This function does the commits then notifies, using the rollback array to find which entries have been deleted.
void CAgnEntryModel::CommitAndNotifyDeletesL(TAgnChangeFilter& aChangeFilter)
	{
	DoCommitL();
	iChangeFilter = &aChangeFilter;

	const TInt KDeleteCount = iDeleteRollbackArray.Count();
	for (TInt i = 0; i < KDeleteCount; ++i)
		{
		CAgnEntry* deletedEntry = iDeleteRollbackArray[i];
		NotifyingL(MCalChangeCallBack2::EChangeDelete, *deletedEntry, NULL);
		
		if(iTzRuleIndex)
			{
			//Remove the tz rule from tz rule index
			//we have to do it after CAgnEntryModel::NotifyingL that is indirectly using the
			//tz rule in aEntry.
			iTzRuleIndex->RemoveTzRuleL(*deletedEntry);
			}
		}

	StreamStore().CommitL();
	ResetRollback();
	}

// Add an entry to a rollback array. 
// If aAdd is ETrue it is added to the add rollback array (for add operations)
// If aAdd is EFalse it is added to the delete rollback array (for delete operations)
void CAgnEntryModel::AppendRollbackArrayL(const CAgnEntry& aEntry, TBool aAdd)
	{
	if ( aAdd )
		{
		iAddRollbackArray.AppendL(aEntry.EntryId());
		}
	else
		{
		CAgnEntry* entryCopy = aEntry.CloneL();
		CleanupStack::PushL(entryCopy);
		iDeleteRollbackArray.AppendL(entryCopy);
		CleanupStack::Pop(entryCopy);
		}
	}

// Reset rollback arrays
void CAgnEntryModel::ResetRollback()
	{
	iAddRollbackArray.Reset();
	iDeleteRollbackArray.ResetAndDestroy();
	}

/**
Rollback indexes in RAM. This is done by deleting all added entries and re-adding all deleted entries
@internalComponent
*/
void CAgnEntryModel::RollbackIndexesL()
	{
	// delete all added entries
	for ( TInt ii = iAddRollbackArray.Count() - 1; ii >= 0; --ii )
		{
		const TAgnEntryId& KEntryIdToDelete = iAddRollbackArray[ii];
		
		CAgnSimpleEntry* entryToDelete = iSimpleEntryTable->GetEntry(KEntryIdToDelete);
		if(entryToDelete != NULL)
			{
			iSimpleEntryTable->DeleteEntry(KEntryIdToDelete);
			}
		
		iAddRollbackArray.Remove(ii);
		}

	iAddRollbackArray.Reset();
	
	if(iTzRuleIndex)
		{
		//Rollback the reference count of tz rules in tz rule index
		TRAPD(ret, iTzRuleIndex->RollBackL());
		if(ret != KErrNotReady)
			{
			User::LeaveIfError(ret);
			}
		}

	// re-add all deleted entries
	for ( TInt ii = iDeleteRollbackArray.Count() - 1; ii >= 0; --ii )
		{
		CAgnEntry* entryToAdd = iDeleteRollbackArray[ii];

		// Check the entry has a guid hash
		if ( entryToAdd->GsDataType() == CGsData::EParent && ! entryToAdd->GuidHash() )
			{
			entryToAdd->SetGuidHash( GenerateHash8L(entryToAdd->Guid()) );
			}	

		// If the entry has already been added, delete it to prevent an error from re-adding the same entry.
		// This can happen if the delete operation fails at a certain point.
		const TAgnEntryId& KEntryIdToAdd = entryToAdd->EntryId();
		if (iSimpleEntryTable->GetEntry(KEntryIdToAdd))
			{
			iSimpleEntryTable->DeleteEntry(KEntryIdToAdd);
			}
		
		if(iTzRuleIndex)
			{
			//Fetch back the tz rule
			iTzRuleIndex->FetchTzRuleL(*entryToAdd);
			}
		
		// re-add the entry
		AddEntryToIndexesL(*entryToAdd);
		
		iDeleteRollbackArray.Remove(ii);
		delete entryToAdd;
		}

	iDeleteRollbackArray.Reset();
	
	iAttachmentIndex->Rollback();
	}
	
/** Reverts the model to the state it was in after CommitL() or RollbackL() was 
last called. This reverts changes to the file and to the indexes held in RAM.

This means that it deletes all entries which have been added, and reinstates 
all entries which have been deleted.

Note that this function is only called when an operation has failed. After this function is complete, there will be a leave in the place where 
Rollback was called.
	
@internalComponent
*/
void CAgnEntryModel::Rollback()
	{
	iEntryManager->Reset();

	StreamStore().Revert();

	TRAPD(ret,iModelStreamIdSet->RollbackL());
	__ASSERT_DEBUG(ret==KErrNone, Panic(EAgmErrRollbackFailed));

	TRAP(ret,RollbackIndexesL());
	}


void CAgnEntryModel::NotifyingL(MCalChangeCallBack2::TChangeType aChangeType, CAgnEntry& aEntry, CAgnInstanceInfo* aOriginalEntry)
	{
	if ( iChangeFilter )
		{
		if ( iChangeFilter->ChangeBroadcastEnabled() )
			{
			NotifyChangeL((iChangeFilter->Session()), &aEntry, aChangeType, aOriginalEntry);
			}
		else
			{
			iChangeFilter->SetChangeMadeWhileDisabled(ETrue);
			}

		if ( aEntry.Type() == CCalEntry::ETodo )
			{
			iChangeFilter->SetPubSubChange(TAgnChangeFilter::ETodoChanged);
			}
		else
			{
			iChangeFilter->SetPubSubChange(TAgnChangeFilter::EEventChanged);
			}

		NotifyPublishAndSubscribeL(*iChangeFilter);
		}
	}

/*
Delete aEntry from the store. If it has a positive replicated count however then mark it as having
been deleted and update it instead.
@capability WriteUserData
@capability ReadUserData
*/
void CAgnEntryModel::DeleteEntryL(CAgnEntry& aEntry, TBool aCascadeDelete, TAgnChangeFilter* aChangeFilter)
	{
	#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		{
		TBuf<KMaxGuidBufLength> guidBuf;
		guidBuf.Copy(aEntry.Guid());
		AgmDebug::DebugLog("DeleteEntryL: Deleting entry with local UID=%d, GUID=%S", aEntry.LocalUid(), &guidBuf);
		}
	#endif

	iChangeFilter = aChangeFilter;
	
	if ( aCascadeDelete )
		{
		if ( aEntry.GsDataType() == CGsData::EParent )
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("DeleteEntryL: Deleting children");)
			
			DeleteChildrenL(aEntry);
			}
		else
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("DeleteEntryL: Updating Parent");)
			UpdateParentL(aEntry);
			}
		}

	DoDeleteEntryL(aEntry);
	
	NotifyingL(MCalChangeCallBack2::EChangeDelete,aEntry, NULL);

	if ( iUpdateAlarm && aEntry.HasAlarm() )
		{
		iAlarm->FindAndQueueNextAlarmL(EFalse);
		iAlarm->DeleteEntriesAlarmL(aEntry.EntryId());	
		}
	
	if(iChangeFilter && iTzRuleIndex)
		{
		//Remove the tz rule from tz rule index
		//we have to do it after CAgnEntryModel::NotifyingL that is indirectly using the
		//tz rule in aEntry.
		iTzRuleIndex->RemoveTzRuleL(aEntry);
		}
	}


void CAgnEntryModel::DeleteChildrenL(CAgnEntry& aParent)
	{//Get Child ids
	__ASSERT_DEBUG(aParent.GsDataType() == CGsData::EParent, Panic(EAgmErrNotParentEntry));
	
	CAgnEntry* parent = FetchEntryL(aParent.EntryId());
	if (parent != NULL)
		{
		CleanupStack::PushL(parent);
		
		const RArray<TGsChildRefData>& KIds = parent->ChildIds();
		// delete each child entry 
		for ( TInt i = KIds.Count() - 1; i >= 0; --i )
			{
			const TCalLocalUid& KChildId = KIds[i].ChildId();
			CAgnEntry* childEntry = FetchEntryL(KChildId);  // pass flag so as not to tell parent  
			if (childEntry)
				{
				CleanupStack::PushL(childEntry);

				DeleteEntryL(*childEntry, EFalse, iChangeFilter); // don't propogate the delete back to this parent

				CleanupStack::PopAndDestroy(childEntry);
				}
			
			aParent.RemoveChildId(KChildId);
			}
		
		CleanupStack::PopAndDestroy(parent);
		}
	}


void CAgnEntryModel::UpdateParentL(CAgnEntry& aChild)
	{
	// It should be used in server side so that the notification of updating a parent is not sent
	__ASSERT_DEBUG(aChild.GsDataType() == CGsData::EChild, Panic(EAgmErrNotChildEntry));

	// get parent and update
	CAgnEntry* parentEntry = FetchEntryL(aChild.ParentId());
	__ASSERT_ALWAYS(parentEntry, User::Leave(KErrNotFound));
	CleanupStack::PushL(parentEntry);

	parentEntry->RemoveChildId(aChild.LocalUid());

	UpdateEntryL(*parentEntry, iChangeFilter, EFalse);

	CleanupStack::PopAndDestroy(parentEntry);	
	}


/**
Delete aEntry from the store. If the entry is a todo then its id is removed from its
todo list.
*/
void CAgnEntryModel::DoDeleteEntryL(CAgnEntry& aEntry)
	{
	DeleteExternalAttributesL(aEntry);
	
	TAgnEntryId id = aEntry.EntryId();
	TStreamId streamId = iEntryManager->DeleteEntryL(id);

	if ( streamId != KNullStreamId )
		{
		__ASSERT_DEBUG(streamId == aEntry.EntryId().StreamId(), Panic(EAgmErrWrongEntryDeleted));
		StreamStore().DeleteL(streamId);
		iModelStreamIdSet->EntryStreamIdSet().DeleteL(streamId);
		}
	
	if ( ! iEntryManager->BufferedDeleting() || iEntryManager->BufferHasBeenStored() ) // during tidying only commit when the buffer has been written
		{
		iEntryManager->StoreBuffersL();
		ExternalizeEntryManagerL();
		// Don't commit on delete. CommitL is called from CalInterimAPI after a number have been added.
		}
	
	UpdateIndexL(aEntry, NULL, EDelete);
	}


TBool CAgnEntryModel::EntryHasNoChildrenAndNoValidInstancesL(CAgnEntry& aEntry) const
	{
	TInt instances(1);
	TInt exceptions(0);
	if ( aEntry.RptDef() )
		{
		// Purely based on repeat rule, does not include exceptions' count
		instances = aEntry.RptDef()->InstanceCountL();
		const RArray<TAgnCalendarTime>* KExceptionList = aEntry.RptDef()->Exceptions();
		if (KExceptionList)
			{
			exceptions += KExceptionList->Count();
			}
		
		__ASSERT_ALWAYS(instances >= exceptions, User::Leave(KErrCorrupt));
		}
	
	TBool entryHasNoChild = ((aEntry.GsDataType() == CGsData::EChild) || aEntry.ChildIds().Count() == 0);
	return (entryHasNoChild && instances == exceptions);
	}

/*
@capability ReadUserData
@capability WriteUserData
*/
void CAgnEntryModel::UpdateEntryL(CAgnEntry& aEntry, TAgnChangeFilter* aChangeFilter, TBool aDeleteChildren)
	{
	TAgnEntryId originalId = aEntry.EntryId();
	
	if (originalId.IsNullId()) 
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: EntryId is null. Must be a newly created entry");)
		
		// Only parent entries can be updated
		if(aEntry.GsDataType() != CGsData::EParent)
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: KErrArgument: Only parent entries can be updated");)
			User::Leave(KErrArgument);
			}

		RPointerArray<CAgnEntry> entriesWithThisGuid;
		CleanupResetAndDestroyPushL(entriesWithThisGuid);
		FetchEntriesL(aEntry.Guid(), entriesWithThisGuid);
		
		// Only an existing entry with the same guid can be updated
		if(entriesWithThisGuid.Count() == 0)
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: KErrNotFound: Only an existing entry with the same guid can be updated");)
			User::Leave(KErrNotFound);
			}
		
		CAgnEntry* existingParent = entriesWithThisGuid[0];
		aEntry.SetLocalUid(existingParent->LocalUid());
		aEntry.SetEntryId(existingParent->EntryId());
		originalId = existingParent->EntryId();
		
		_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: Setting Ids: LocalUid - %d, EntryId - %d", aEntry.LocalUid(),aEntry.EntryId().Value());)
		CleanupStack::PopAndDestroy(&entriesWithThisGuid);
		}
		
	//client server calls needs to be updated
	iChangeFilter = aChangeFilter;
	
	if (EntryHasNoChildrenAndNoValidInstancesL(aEntry))
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: Deleting invalid entry - No children and no valid instances");)
		DeleteEntryL(aEntry, EFalse, iChangeFilter);
		return;
		}
	
	if ( aDeleteChildren )
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: Deleting children");)
		DeleteChildrenL(aEntry);
		}
	
	CAgnEntry* oldEntry = FetchEntryL(originalId);
	__ASSERT_DEBUG(oldEntry, User::Leave(KErrNotFound));
	CleanupStack::PushL(oldEntry);
	TBool hadAlarm = EFalse;
	if ( oldEntry )
		{
		hadAlarm = oldEntry->HasAlarm();
		}
	
	CAgnInstanceInfo* instanceInfoBefore = CAgnInstanceInfo::NewLC(*oldEntry);

	TRAPD(ret, DoUpdateEntryL(aEntry, oldEntry));

	if ( ret != KErrNone )
		{
		aEntry.SetEntryId(originalId);
		_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: DoUpdateEntryL failed: Leaving with error - %d",ret);)
		User::Leave(ret);
		}

	NotifyingL(MCalChangeCallBack2::EChangeModify, aEntry, instanceInfoBefore);
	
	if(iChangeFilter && iTzRuleIndex)
        {
        //Remove the tz rule from tz rule index
        //we have to do it after CAgnEntryModel::NotifyingL that is indirectly using the
        //tz rule in oldEntry.
        iTzRuleIndex->RemoveTzRuleL(*oldEntry);
        }
  
	
	CleanupStack::PopAndDestroy(instanceInfoBefore);
	CleanupStack::PopAndDestroy(oldEntry);
	
	// Delete the old alarm and if a new alarm exists it will be added by findAndQueue
	// If a todo is completed or an event updated, while alarm is snoozed, 
	// then the snoozed alarm has to be deleted separately, and deletion is not handled by findAndQueue
	if (hadAlarm)
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("UpdateEntryL: Deleting the alarm on the existing entry");)

		iAlarm->DeleteEntriesAlarmL(aEntry.EntryId());
		}
	if (iUpdateAlarm && aEntry.HasAlarm())
		{
		iAlarm->FindAndQueueNextAlarmL(EFalse);
		}
	}

/** Update an entry in the store. 
@capability ReadUserData
*/
void CAgnEntryModel::DoUpdateEntryL(CAgnEntry& aEntry, CAgnEntry* aOldEntry)
	{
	TStreamId newStreamId;
	UpdateExternalAttributesL(aEntry);

	if (aOldEntry)
		{
		DoUpdateAttachmentsL(aEntry, *aOldEntry);
		}
	
	if(iTzRuleIndex)
		{
		__ASSERT_DEBUG(aOldEntry, Panic(EAgmErrNullPointer));
		iTzRuleIndex->UpdateTzRuleL(*aOldEntry, aEntry);
		}
	
	TStreamId oldStreamId = iEntryManager->UpdateEntryL(aEntry, newStreamId);

	if ( oldStreamId != KNullStreamId )
		{
		StreamStore().DeleteL(oldStreamId);
		iModelStreamIdSet->EntryStreamIdSet().DeleteL(oldStreamId);
		}

	if ( newStreamId != KNullStreamId )
		{
		iModelStreamIdSet->EntryStreamIdSet().AddL(newStreamId);			
		}

	iEntryManager->StoreBuffersL();
	ExternalizeEntryManagerL();
	
	UpdateIndexL(aEntry, aOldEntry, EUpdate);
	}

// Called when an entry is updated
// This compares the new entry with the old one to see if any attachments have changed drive (by calling CCalAttachmentFile::SetDrive).
void CAgnEntryModel::DoUpdateAttachmentsL(CAgnEntry& aNewEntry, CAgnEntry& aOldEntry)
	{//This method will move the attachment to a different drive if it has been reset by the user.
	const TInt KOldAttachmentCount = aOldEntry.AttachmentCount();
	const TInt KNewAttachmentCount = aNewEntry.AttachmentCount();
	
	for (TInt oldEntryIndex = 0; oldEntryIndex < KOldAttachmentCount; ++oldEntryIndex)
		{
		CAgnAttachment& oldAttach = aOldEntry.Attachment(oldEntryIndex);
		if (oldAttach.Type() == CCalContent::EDispositionInline && oldAttach.Uid() != 0)
			{
			CAgnAttachmentFile& oldAttachFile = static_cast<CAgnAttachmentFile&>(oldAttach);
			TDriveName oldDrive = oldAttachFile.Drive();
			
			for (TInt newEntryIndex = 0; newEntryIndex < KNewAttachmentCount; ++newEntryIndex)
				{
				CAgnAttachmentFile& newAttachFile = static_cast<CAgnAttachmentFile&>(aNewEntry.Attachment(newEntryIndex));
				if (newAttachFile.Uid() == oldAttach.Uid() && newAttachFile.Drive() != oldDrive)
					{
					HBufC* newfilename = oldAttachFile.FileName().AllocLC();
					newfilename->Des().Replace(0,2,newAttachFile.Drive());
					iAgnServerFile->MoveFileL(oldAttachFile.FileName(), newfilename->Des());
					CleanupStack::PopAndDestroy(newfilename);

					break;
					}
				}
			}
		}
	}

void CAgnEntryModel::MoveAttachmentToDriveL(CAgnAttachmentFile& aOldFileAttachment, CAgnAttachmentFile& aNewFileAttachment)
	{
	TParsePtrC parse(aOldFileAttachment.FileName());
	HBufC* fileName = GenerateFilenameLC(aNewFileAttachment.Drive(), parse.NameAndExt());
	TPtr pFilename(fileName->Des());
	iAgnServerFile->MoveFileL(aOldFileAttachment.FileName(), pFilename);
	aNewFileAttachment.SetFileNameL(*fileName);
	CleanupStack::PopAndDestroy(fileName);
		
	#if defined (__CAL_ATTACH_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		HBufC8* attachId = aNewFileAttachment.ContentId().AllocLC();
		AgmDebug::DebugLog("Moving attachment to drive: Old filename: %S to New filename with drive: %S", &aOldFileAttachment.FileName(), &pFilename);
		AgmDebug::DebugLog("Attachment: UId - %S, Old filename size - %d, New filename size - %d", attachId, aOldFileAttachment.Size(), aNewFileAttachment.Size());
		CleanupStack::PopAndDestroy(attachId);
	#endif
	}

const CAgnSimpleEntry* CAgnEntryModel::GetSimpleEntryFromIndexes(const TAgnEntryId& aEntryId)
	{
	return iSimpleEntryTable->GetEntry(aEntryId);
	}

const CAgnSimpleEntry* CAgnEntryModel::GetSimpleEntryFromIndexes(TCalLocalUid aUniqueId)
	{
	return iSimpleEntryTable->GetEntry(aUniqueId);
	}

/** 
Gets an entry based on its entry ID.

@internalComponent
@capability ReadUserData
@param aId The entry ID of the entry to retrieve.
@return Pointer to the entry. 
*/
CAgnEntry* CAgnEntryModel::FetchEntryL(const TAgnEntryId& aId) const
	{
	CAgnEntry* entry = iEntryManager->FetchEntryL(aId);
	if (entry)
		{
		CleanupStack::PushL(entry);

		if(iTzRuleIndex)
			{
			iTzRuleIndex->FetchTzRuleL(*entry);
			}
		
		_DBGLOG_ENTRY(AgmDebug::DebugLog("FetchEntryL: Fetched entry with Stream Id %d, Partial Id %d",aId.Value(), aId.PartialId());)

		if ( entry->GsDataType() == CGsData::EChild )
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("FetchEntryL: Entry fetched is a child entry, Parent Id - %d", entry->ParentId());)
			
			// if a child entry has been fetched, get the recurrence ID and range stored with the parent
			CAgnEntry* parent = FetchEntryL(entry->ParentId());

			__ASSERT_ALWAYS(parent, User::Leave(KErrCorrupt)); // child without parent entry in DB!
			CleanupStack::PushL(parent);
			
			entry->SetRecurrenceIdFromParentL(*parent);
			
			CleanupStack::PopAndDestroy(parent);
			}

		_DBGLOG_ENTRY(AgmDebug::DebugLogEntryL(*entry, EDumpEntryAll);)
		CleanupStack::Pop(entry);
		}
	return (entry);
	}

/** 
Gets an entry based on its unique ID.

@internalComponent
@capability ReadUserData
@param aId The unique ID of the entry to retrieve.
@return Pointer to the entry. 
*/
CAgnEntry* CAgnEntryModel::FetchEntryL(TCalLocalUid aUniqueId) const
	{
	_DBGLOG_ENTRY(AgmDebug::DebugLog("FetchEntryL: Attempting to fetch simple entry with LocalUid='%d'", aUniqueId);)
	
	// find entry in indexes to get the entry ID
	CAgnSimpleEntry* simpleEntry = iSimpleEntryTable->GetEntry(aUniqueId);
	CAgnEntry* entry = NULL;
	
	if ( simpleEntry )
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("FetchEntryL: Entry found, fetching the full entry with Local ID %d",simpleEntry->EntryId().Value());)
		
		// fetch the full entry from the entry ID
		entry = FetchEntryL(simpleEntry->EntryId());			
		}
	#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
	else
		{
		AgmDebug::DebugLog("FetchEntryL: Entry not found");
		}
	#endif

	
	return ( entry );
	}

/** 
Gets entries based on GUID.

@internalComponent
@capability ReadUserData
@param aGuid The GUID of the entry to retrieve
@param aList The list of CAgnEntry objects
*/
void CAgnEntryModel::FetchEntriesL(const TDesC8& aGuid, RPointerArray<CAgnEntry>& aList) const
	{
	CAgnEntry* parentEntry = FetchEntryL(aGuid);
	
	#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		TBuf<KMaxGuidBufLength> guidBuf;
		guidBuf.Copy(aGuid);
		AgmDebug::DebugLog("FetchEntriesL:  Using GUID='%S'", &guidBuf);
	#endif

	if ( parentEntry )
		{
		#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
			TBuf<KMaxGuidBufLength> guidBuf;
			guidBuf.Copy(aGuid);
			AgmDebug::DebugLog("FetchEntriesL: Found a parent entry with GUID='%S'", &guidBuf);
		#endif

		CleanupStack::PushL(parentEntry);
		aList.AppendL(parentEntry);
		CleanupStack::Pop(parentEntry);
		
		// Fetch the children and add them to the array
		const RArray<TGsChildRefData>& KIds = parentEntry->ChildIds();
		
		const TInt KCount = KIds.Count();
		
		_DBGLOG_ENTRY(AgmDebug::DebugLog("FetchEntriesL: Parent entry has %d child(ren)", KCount);)

		for ( TInt i = 0; i < KCount; ++i )
			{
			CAgnEntry* childEntry = FetchEntryL(KIds[i].ChildId());  // pass flag so as not to tell parent to update its child list 
			if (childEntry)
				{
				CleanupStack::PushL(childEntry);
				aList.AppendL(childEntry);
				CleanupStack::Pop(childEntry);
				}
			}
		}  // parentEntry
	}


CAgnEntry* CAgnEntryModel::FetchEntryL(const TDesC8& aGuid, const TAgnCalendarTime& aRecurrenceId) const	
	{
	CAgnEntry* returnEntry = NULL;
	CAgnEntry* parentEntry = FetchEntryL(aGuid);
	
	if ( parentEntry )
		{
		CleanupStack::PushL(parentEntry);
		returnEntry = FindChildFromParentL(*parentEntry, aRecurrenceId); 
		CleanupStack::PopAndDestroy(parentEntry);  	
		}
		
	return returnEntry;
	}

TBool CAgnEntryModel::AreIndexesBuilt() const
	{
	if (iAgnServerFile)
		{
		return iAgnServerFile->AreIndexesBuilt();
		}
	return EFalse;
	}


void CAgnEntryModel::RestoreCategoriesL()
	{
	if ( iCalConverter )
		{
		iCalConverter->InternalizeCategoriesL();
		}
	}

void CAgnEntryModel::OpenAttachmentFileL(RFile& aFile, TInt aAttachmentUid) const
	{
	const CAgnAttachmentIndexItem* const item = iAttachmentIndex->Attachment(aAttachmentUid);
	// item is owned by iAttachmentIndex
	
	if ( item )
		{
		iAgnServerFile->OpenFileL(aFile, item->FileName());
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}


void CAgnEntryModel::CreateNewFileL(RFile& aFile, const TDesC& aFileName)
	{
	iAgnServerFile->CreateNewFileL(aFile, aFileName);
	}

// Called when a binary data attachment is stored. 
// At this stage, a new file has been created, and the file handle returned to the client.
// The data is written to file from the client side, then this function is called to update the metadata of the entry containing the attachment.
void CAgnEntryModel::UpdateAttachmentDetailsL(TCalLocalUid aLocalUid, TInt aAttachmentIndex, const TDesC& aFileName, TInt aAttachmentSize)
	{
	_DBGLOG_ATTACH(AgmDebug::DebugLog("UpdateAttachmentDetailsL: Local Uid %d, Attachment Index %d, FileName %S, Attachment Size %d", aLocalUid, aAttachmentIndex, &aFileName, aAttachmentSize);)
	
	CAgnEntry* entry = FetchEntryL(aLocalUid);
	
	if ( entry )
		{
		CleanupStack::PushL(entry);
		
		CAgnAttachmentFile* attachment = static_cast<CAgnAttachmentFile*>(&entry->Attachment(aAttachmentIndex));

		if ( attachment && attachment->FileName().Length() <= KMaxDriveName)
			{
			attachment->SetFileNameL(aFileName);
			attachment->SetSize(aAttachmentSize);
			attachment->SetUid(iNextAttachmentUid);
			++iNextAttachmentUid;
			UpdateEntryL(*entry, NULL, EFalse);
			
			ExternalizeNextUidValuesL();
			}
			
		CleanupStack::PopAndDestroy(entry);
		}
	}

TInt CAgnEntryModel::TransferFileFromClientL(RFile& aAttachfileHandle, CAgnAttachmentFile& aAttachFile, CAgnEntry& aEntry, TBool aIsSameDrive)
	{
	RBuf originalFileName;
	originalFileName.CreateL(KMaxFileName);
	CleanupClosePushL(originalFileName);
	aAttachfileHandle.FullName(originalFileName);
	TInt size;
	User::LeaveIfError(aAttachfileHandle.Size(size));
				
	// Generate  attachment filename
	
	TParsePtrC parseOriginalFile(originalFileName); 
	HBufC* fileName = GenerateFilenameLC(aAttachFile.FileName(), parseOriginalFile.NameAndExt());
	TPtr attachFilename(fileName->Des());
		
	_DBGLOG_ATTACH(AgmDebug::DebugLog("TransferFileFromClientL: Transferring file from: %S of size %d to %S", &originalFileName, size, &attachFilename);)
	if(aIsSameDrive)
		{
		User::LeaveIfError(aAttachfileHandle.Rename(attachFilename)); // move the file to calendar area
		}
	else
		{
		aAttachfileHandle.Close();
 		iAgnServerFile->MoveFileL(originalFileName, attachFilename);			
		}
	aAttachFile.SetFileNameL(attachFilename);
	aAttachFile.SetSize(size);
	aAttachFile.SetUid(iNextAttachmentUid++);
	
	// Don't call UpdateEntryL as we only need to update the Calendar db file
	TRAPD(err, UpdateEntryL(aEntry, NULL, EFalse));
	if (err != KErrNone)
		{
		// if the entry failed to update, move the attachment back
		if (aIsSameDrive)
			{
			User::LeaveIfError(aAttachfileHandle.Rename(originalFileName));	
			}
		else
			{
			iAgnServerFile->MoveFileL(fileName->Des(), originalFileName);		
			}	
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(2, &originalFileName);
	ExternalizeNextUidValuesL();
	return iNextAttachmentUid-1;
	}
TInt CAgnEntryModel::MoveFileToServerL(TCalLocalUid aLocalUid, TInt aAttachmentIndex)
	{
	TInt ret = 0;
	TRAPD(err, ret = DoMoveFileToServerL(aLocalUid, aAttachmentIndex));
	if(err != KErrNone)	
		{
		iAttachmentFileHandle.Close();
		User::Leave(err);	
		}
	return ret;
	}
	
TInt CAgnEntryModel::DoMoveFileToServerL(TCalLocalUid aLocalUid, TInt aAttachmentIndex)
	{
	_DBGLOG_ATTACH(AgmDebug::DebugLog("DoMoveFileToServerL: Local Uid %d, Attachment Index %d", aLocalUid, aAttachmentIndex);)
	
	TCalAttachmentUid attachUid(0);

	CAgnEntry* entry = FetchEntryL(aLocalUid);
	__ASSERT_ALWAYS(entry, User::Leave(KErrCorrupt));
	CleanupStack::PushL(entry);

	CAgnAttachment& attach = entry->Attachment(aAttachmentIndex);
	__ASSERT_ALWAYS(attach.Type() == CCalContent::EDispositionInline, User::Leave(KErrCorrupt));

	CAgnAttachmentFile& attachFile = static_cast<CAgnAttachmentFile&>(attach);
	__ASSERT_ALWAYS(attachFile.Drive().CompareF(KDefaultAttachmentDrive()),User::Leave(KErrCorrupt) );//Drive is the default one

	attachUid = TransferFileFromClientL(iAttachmentFileHandle,attachFile, *entry, EFalse);

	CleanupStack::PopAndDestroy(entry);
	return attachUid;
	}
	
TInt CAgnEntryModel::TransferAttachmentFileToServerL(RFile& aFile, TCalLocalUid aLocalUid, TInt aAttachmentIndex)
	{
	_DBGLOG_ATTACH(AgmDebug::DebugLog("TransferAttachmentFileToServerL: Local Uid %d, Attachment Index %d", aLocalUid, aAttachmentIndex);)
	
	TCalAttachmentUid attachUid(0);
	CAgnEntry* entry = FetchEntryL(aLocalUid);
	__ASSERT_ALWAYS(entry, User::Leave(KErrCorrupt));
	CleanupStack::PushL(entry);

	CAgnAttachment& attach = entry->Attachment(aAttachmentIndex);
	__ASSERT_ALWAYS(attach.Type() == CCalContent::EDispositionInline, User::Leave(KErrCorrupt));

	CAgnAttachmentFile& attachFile = static_cast<CAgnAttachmentFile&>(attach);

	if(!attachFile.Drive().CompareF(KDefaultAttachmentDrive()))//Drive is the default one
		{
		attachUid = TransferFileFromClientL(aFile, attachFile, *entry, ETrue);
		}
	else
		{//Client need to close the handle in order to move the original file to the drive specified.
		User::LeaveIfError(iAttachmentFileHandle.Duplicate(aFile));	
		}

	CleanupStack::PopAndDestroy(entry);
	return attachUid;
	}

// Generate a filename for an attachment on the specified drive.
HBufC* CAgnEntryModel::GenerateFilenameLC(const TDesC& aDrive, const TDesC& aFileName)
	{
	_LIT(KCalDirectory, "\\");
	
	// file name is "X:\\private\\10003a5b\\calendarfilename_a\\Y\\filename"
	// where X is the drive specified (KDefaultAttachmentDrive if none is set)
	// and Y is the folder number calculated from the attachment ID
	const TInt KNumberOfAttachmentsPerFolder = 32;
	//Restricting the Attachemnt folder name length to 2 chars, where it allows to add max0-99 folders
	//by considering the KMaxFileName Length is allowed 220 Chars	
	const TInt KMaxNumOfAttachmentFolders = 100;
	// 8 to cover attachment folder name and trailing number if there exists a same file name
	const TInt KExtraFileNameLength = 8;
	TInt fileNameLength = iAgnServerFile->FileName().Length() + KExtraFileNameLength  + aFileName.Length();
	TPtrC fileNamePtr (aFileName);
	if(fileNameLength > KMaxFileName)
	    {
	    const TInt KMinFileNameLength = 8;
	    fileNameLength = iAgnServerFile->FileName().Length() + KExtraFileNameLength +  KMinFileNameLength;
	    if(fileNameLength > KMaxFileName)
	        {
	        User::Leave(KErrBadName);
	        }
	    else
	        {
	        fileNamePtr.Set(aFileName.Left(KMinFileNameLength - 2));//save 2 for trailling number
	        }
	    }
	HBufC* fileName = HBufC::NewLC(fileNameLength);
	TPtr folderNamePtr = fileName->Des();
	iAgnServerFile->GetAttachmentFolderNameL(folderNamePtr);
	
	// if the drive has been set already, set it on the filename
	if ( aDrive.Length() >= 1 )
		{
		folderNamePtr.Replace(0, 1, aDrive.Left(1));
		}
	else
		{
		folderNamePtr.Replace(0, 1, KDefaultAttachmentDrive().Left(1));
		}
	
	const TInt KFolderNumber = iNextAttachmentUid / KNumberOfAttachmentsPerFolder;
	if (KFolderNumber >= KMaxNumOfAttachmentFolders)
		{
		User::LeaveIfError(KErrDirFull);
		}
	folderNamePtr.AppendNum(KFolderNumber);
	folderNamePtr.Append(KCalDirectory);
    TPtr fullFileNamePtr(folderNamePtr);
	fullFileNamePtr.Append(fileNamePtr);	
	TBool uniqueFilenameGenerated = EFalse;
	TInt count = 0;
	while ( ! uniqueFilenameGenerated )
	    {
	    if ( !iAgnServerFile->FileExistsL(fullFileNamePtr) )
            {
            uniqueFilenameGenerated = ETrue;
            }
	    else
	        {
	        TParsePtrC parse(fileNamePtr);
	        fullFileNamePtr = folderNamePtr;
	        fullFileNamePtr.Append(parse.Name());
	        fullFileNamePtr.AppendNum(count++);
	        fullFileNamePtr.Append(parse.Ext());
	        }
	    }
	
	iAgnServerFile->CreateDirL(fullFileNamePtr);
	return fileName;
	}

// Generate a filename for an attachment on the specified drive.
HBufC* CAgnEntryModel::GenerateRandomFilenameLC(const TDesC& aDrive)
    {
    _LIT(KCalDirectory, "\\");
    
    // file name is "X:\\private\\10003a5b\\calendarfilename_a\\Y\\filename"
    // where X is the drive specified (KDefaultAttachmentDrive if none is set)
    // and Y is the folder number calculated from the attachment ID
    const TInt KNumberOfAttachmentsPerFolder = 32;
    const TInt KNumCharsInFileName = 8; 
    //Restricting the Attachemnt folder name length to 2 chars, where it allows to add max0-99 folders
    //by considering the KMaxFileName Length is allowed 220 Chars   
    const TInt KMaxNumOfAttachmentFolders = 100;

    // 16 to cover attachment folder name and extension - could be 'foldername\\888\\filename.xxx'
    const TInt KFileNameLength = iAgnServerFile->FileName().Length() + 32 + KNumCharsInFileName;
    
    HBufC* fileName = HBufC::NewLC(KFileNameLength);
    TPtr fileNamePtr = fileName->Des();
    
    iAgnServerFile->GetAttachmentFolderNameL(fileNamePtr);
    
    // if the drive has been set already, set it on the filename
    if ( aDrive.Length() >= 1 )
        {
        fileNamePtr.Replace(0, 1, aDrive.Left(1));
        }
    else
        {
        fileNamePtr.Replace(0, 1, KDefaultAttachmentDrive().Left(1));
        }
    
    const TInt KFolderNumber = iNextAttachmentUid / KNumberOfAttachmentsPerFolder;
    if (KFolderNumber >= KMaxNumOfAttachmentFolders)
        {
        User::LeaveIfError(KErrDirFull);
        }
    fileNamePtr.AppendNum(KFolderNumber);
    fileNamePtr.Append(KCalDirectory);
    
    TBool uniqueFilenameGenerated = EFalse;
    
    TTime time;
    time.UniversalTime();
    TInt64 seed = time.Int64() + iNextAttachmentUid;
    
    while ( ! uniqueFilenameGenerated )
        {
        // Generate a random filename
        for ( TInt i = 0; i < KNumCharsInFileName; ++i, ++seed )
            {
            TChar randomChar = (Math::Rand(seed) % 26) + 'a';
            fileNamePtr.Append(randomChar);
            }

        // check that there is not an existing filename with the same name (very unlikely!)
        if ( iAgnServerFile->FileExistsL(fileNamePtr) )
            {
            // Remove this file name from the descriptor if the file exists already, and generate a new name
            fileNamePtr.SetLength(fileNamePtr.Length() - KNumCharsInFileName);
            }
        else
            {
            uniqueFilenameGenerated = ETrue;
            }
        }
    
    iAgnServerFile->CreateDirL(fileNamePtr);
    return fileName;
    }

const RArray<TCalLocalUid>* CAgnEntryModel::GetEntriesWithAttachment(TCalAttachmentUid aAttachmentUid) const
	{
	_DBGLOG_ATTACH(AgmDebug::DebugLog("GetEntriesWithAttachment: Attachment Uid %d", aAttachmentUid);)
	
	const CAgnAttachmentIndexItem* const item = iAttachmentIndex->Attachment(aAttachmentUid);
	
	if ( item && item->Entries().Count())
		{
		return &item->Entries();
		}
		
	return NULL;
	}


void CAgnEntryModel::GetSortedAttachmentsL(RArray<TCalAttachmentUid>& aAttachmentIds, CCalAttachmentManager::TSortOrder aSortType)
	{
	RPointerArray<CAgnAttachmentIndexItem> sortedAttachments;
	CleanupClosePushL(sortedAttachments);
	iAttachmentIndex->GetSortedIndexL(aSortType, sortedAttachments);

	const TInt KAttachmentCount = sortedAttachments.Count();
	for ( TInt i = 0; i < KAttachmentCount; ++i)
		{
		aAttachmentIds.AppendL(sortedAttachments[i]->Uid());
		}

	CleanupStack::PopAndDestroy(&sortedAttachments); 
	}


CAgnAttachment* CAgnEntryModel::FetchAttachmentByIdL(TCalAttachmentUid aAttachUid)
	{
	_DBGLOG_ATTACH(AgmDebug::DebugLog("FetchAttachmentByIdL: Attachment Uid %d", aAttachUid);)
	
	const CAgnAttachmentIndexItem* const item = iAttachmentIndex->Attachment(aAttachUid);
	CAgnAttachment* attachmentToWrite = NULL;
	
	if ( item )
		{
		const RArray<TCalLocalUid>& KEntries = item->Entries();

		if ( KEntries.Count() == 0 )
			{
			// corrupt index - remove the attachment and return NULL
			iAttachmentIndex->RemoveAttachment(aAttachUid);
			}
		else
			{
			const TCalLocalUid& Kid = KEntries[0];
			CAgnEntry* entry = FetchEntryL(Kid);

			if ( entry )
				{
				CleanupStack::PushL(entry);

				for ( TInt i = 0; i < entry->AttachmentCount(); ++i )
					{
					if ( entry->Attachment(i).Uid() == aAttachUid )
						{
						attachmentToWrite = AttachmentFactory::CloneL(entry->Attachment(i));

						break;
						}
					}

				CleanupStack::PopAndDestroy(entry);
				}
			}
		}

	return attachmentToWrite;
	}

const TDesC8& CAgnEntryModel::GetEntryGuidL(CAgnEntry& aEntry) const
	{
	if(aEntry.Guid() == KNullDesC8)
		{//it is a child and its uid has not been loaded
		CAgnEntry* parentEntry = FetchEntryL(aEntry.ParentId());
		__ASSERT_DEBUG(parentEntry, Panic(EAgmErrChildWithoutParent));
		if(parentEntry)
			{
			CleanupStack::PushL(parentEntry);
			HBufC8* guidHbuf = parentEntry->Guid().AllocL();
			CleanupStack::PopAndDestroy(parentEntry);	
			aEntry.SetGuid(guidHbuf);
			}
		}
	return aEntry.Guid();
	}


/**
Returns A reference to the stream store in which the model's data is stored. 
@internalComponent
*/
CStreamStore& CAgnEntryModel::StreamStore() const
	{
	return iEntryManager->StreamStore();
	}

CCalAsyncDelete* CAgnEntryModel::CreateAsyncDeleteL(TAgnChangeFilter& aChangeFilter)
	{
	return CCalAsyncDelete::NewL(*this, aChangeFilter, *iSimpleEntryTable);
	}

void CAgnEntryModel::LoadNewStreamStoreL(CStreamStore& aStore, const TStreamId& aModelStreamId, CAgnEntryManager& aEntryManager, CAgnTzRuleIndex& aTzRuleIndex)
	{
	iEntryManager->SetStore(aStore);

	iEntryManager->CopyStreamIds(aEntryManager);
	iModelStreamIdSet->LoadL(aStore, aModelStreamId);
	
	if(iTzRuleIndex)
		{
		delete iTzRuleIndex; 
		}
	
	iTzRuleIndex = &aTzRuleIndex;
	iTzRuleIndex->CheckTzDbModificationL(*iAgnServerFile);
	}

TBool CAgnEntryModel::StreamsAreEmpty() const
	{
	return (iModelStreamIdSet->EntryStreamIdSet().Count() == 0);
	}

TAgnEntryIter* CAgnEntryModel::CreateEntryIterL() const
	{
	return new (ELeave) TAgnEntryIter(iModelStreamIdSet->EntryStreamIdSet(), *iEntryManager);
	}

CAgnCategoryIndex& CAgnEntryModel::CategoryIndex() const
/** Gets the category index.

@return The agenda model category index object. */
	{
	return ( *iCategoryIndex );
	}

TTime CAgnEntryModel::TzRulesLastModifiedDateL()
	{
	return iTzRuleIndex->TzRulesLastModifiedDateL();
	}

void CAgnEntryModel::HandleTzRulesChangeL(const TTime& aTime)
    {
    iTzRuleIndex->HandleTzRulesChangeL(aTime);
    }

CAgnAlarm& CAgnEntryModel::Alarm()
    {
    return *iAlarm;
	}
