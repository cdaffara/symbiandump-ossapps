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

#include "agsfileconverter.h"

#include <s32file.h>
#include <apparc.h>
#include <asshddefs.h>
#include <txtrich.h>
#include <vtzrules.h>

#include "agscategorylist.h"
#include "agsentrymanager.h"
#include "agmcontent.h"
#include "agmdate.h"
#include "agmentry.h"
#include "agsentrymodel.h"
#include "agsstreamidset.h"
#include "agmutil.h"
#include "agsver.h"
#include "agmcategory.h"
#include "agmattendee.h"
#include "agsfilemanager.h"
#include "agsmain.h"
#include "calcommonimpl.h"
#include "agstzruleindex.h"

const TInt KMajorVersionNumber = 1;
const TInt KMinorVersionNumber = 2;

const TInt KBuildNumber = 2;	// Build 1.2.2 is current build
const TInt KBuildCompatibleVersion = 1;
//
const TInt KOldMinorVersionNumber = 1;
const TInt KOldBuildV94 = 211;		// build 1.1.211 is v9.4 compacted version
const TInt KOldBuildV92 = 210;		// build 1.1.210 is v9.2
const TInt KOldBuildV91 = 209;		// build 1.1.209 is v9.1
									// build 1.1.208 is v8.0 and v8.1
									// build 1.1.207 is 6.0 & 6.1 (prior builds 204-206 were dev releases)

// ---------------------------- CalFileVersionUtils -------------------------

CAgnCalendarConverter* CalFileVersionUtils::CreateConverterL(const TAgnVersion& aFileVersion, CAgnServFile& aAgnServerFile)
/** @internalComponent */
	{
	CAgnCalendarConverter* converter = NULL;
	
	if (aFileVersion == TAgnVersion(TVersion(KMajorVersionNumber, KOldMinorVersionNumber, KOldBuildV91))) 
		{
		// file format 1.1.209 used in 9.1
		converter = CAgnCalendarConverter209::NewL(aAgnServerFile);
		}
	else if (aFileVersion == TAgnVersion(TVersion(KMajorVersionNumber, KOldMinorVersionNumber, KOldBuildV92))) 
		{
		// file format 1.1.210 used in 9.2
		converter = CAgnCalendarConverter210::NewL(aAgnServerFile);
		}
	else if (aFileVersion == TAgnVersion(TVersion(KMajorVersionNumber, KOldMinorVersionNumber, KOldBuildV94)))
		{
		// file format 1.1.211 used in 9.4 compacted file version
		converter = CAgnCalendarConverter211::NewL(aAgnServerFile);
		}
	else if (aFileVersion == CurrentFileVersion() )
		{
		converter = NULL;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	
	return converter;
	}

void CalFileVersionUtils::FileVersionSupportedL(const TAgnVersion& aFileVersion, CalCommon::TCalFileVersionSupport& aStatus)
	{
	if ( aFileVersion == CurrentFileVersion() ||
			aFileVersion == TAgnVersion(TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildCompatibleVersion)))
		{
		// The the file was created by an agenda model
		// of the current version
		//
		aStatus = CalCommon::EFileIsCurrentVersion;
		}
	else
		{
		RArray<TAgnVersion> supportedFileVersions;
		CleanupClosePushL(supportedFileVersions);
		supportedFileVersions.AppendL(TAgnVersion(TVersion(KMajorVersionNumber, KOldMinorVersionNumber, KOldBuildV94)));
		supportedFileVersions.AppendL(TAgnVersion(TVersion(KMajorVersionNumber, KOldMinorVersionNumber, KOldBuildV92)));
		supportedFileVersions.AppendL(TAgnVersion(TVersion(KMajorVersionNumber, KOldMinorVersionNumber, KOldBuildV91)));
		
		// The file version is not current but if it is in the 
		// list of versions supported by this agenda model it can
		// be converted to current version.
		//
		aStatus = CalCommon::EUnsupportedFileVersion;
		for ( TInt i = 0; i < supportedFileVersions.Count(); ++i )
			{
			if ( supportedFileVersions[i] == aFileVersion )
				{
				aStatus = CalCommon::EFileNeedsConverting;
				break;
				}
			}
		CleanupStack::PopAndDestroy(&supportedFileVersions);
		}
	}

TAgnVersion CalFileVersionUtils::CurrentFileVersion()
	{ 
	return (TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildNumber));
	}

// ---------------------------- CAgnCalendarConverter -------------------------

CAgnCalendarConverter::CAgnCalendarConverter()
	{
	}
	
CAgnCalendarConverter::~CAgnCalendarConverter()
/** @internalComponent */
	{
	if(iOutputTzRuleIndex)
		{
		delete iOutputTzRuleIndex; 
		}
	
	delete iDictionary;
	delete iOutputStreamStore;
	delete iOutModelStreamIdSet;
	delete iOutEntryManager;
	}

void CAgnCalendarConverter::ConstructL(CAgnServFile& aAgnServerFile)
/** @internalComponent */
	{ 
	iOutModelStreamIdSet = CAgnModelStreamIdSet::NewL();
	iOutEntryManager = CAgnEntryManager::NewL();
	iAgnServerFile = &aAgnServerFile;
	
	// Get the original (old version) stream store from the CAgnServFile
	//
	iInputStreamStore = iAgnServerFile->StoreL();
	__ASSERT_ALWAYS(iInputStreamStore, User::Leave(KErrCorrupt));
	
	// Create the output stream store
	//
	TParsePtrC fullFileName(iAgnServerFile->FileName());
	HBufC* fileName = HBufC::NewLC(fullFileName.Drive().Length() + 
								   fullFileName.NameAndExt().Length() +
								   KUpdatedAgendaFileExtension().Length());
	TPtr fileNamePtr(fileName->Des());
	fileNamePtr.Append(fullFileName.Drive());
	fileNamePtr.Append(fullFileName.NameAndExt());
	fileNamePtr.Append(KUpdatedAgendaFileExtension);
	
	// Create the output stream store where converted information will be kept
	// This code copied from CAgnServerSession::CreateAgendaFileL() and CAgnEntryModel::CreateL(CStreamStore& aStore)
	//
	iOutputStreamStore = iAgnServerFile->Server().FileMgr()->CreateAgendaFileLC(fileNamePtr);
	CleanupStack::Pop(iOutputStreamStore);
	CleanupStack::PopAndDestroy(fileName);
	iOutputStreamStore->SetTypeL(TUidType(KPermanentFileStoreLayoutUid, KUidAppDllDoc, KUidAgnApp));
	
    iOutEntryManager->SetStore(*iOutputStreamStore);
        
	// Create the network of stream Ids needed by the model
	TStreamId headstreamId = iOutModelStreamIdSet->CreateL(*iOutputStreamStore, CalFileVersionUtils::CurrentFileVersion());
	
	// Create the stream dictionary for iOutputStreamStore and keep headstreamId in it
	//
	iDictionary = CStreamDictionary::NewL();
	iDictionary->AssignL(KUidAgnModel, headstreamId);
	
    // Set stream dictionary in stream store
	TApaAppIdentifier dummyApp(KUidAgnModel, KNullDesC());
	CApaProcess::WriteRootStreamL(*iOutputStreamStore,*iDictionary, dummyApp);
	
	iOutputTzRuleIndex = CAgnTzRuleIndex::NewL(*iDictionary, *iOutputStreamStore);
	}

CFileStore& CAgnCalendarConverter::InputStreamStore() const
	{
	return *iInputStreamStore;
	}

CFileStore& CAgnCalendarConverter::OutputStreamStore() const
	{
	return *iOutputStreamStore;
	}

CAgnServFile& CAgnCalendarConverter::AgnServerFile() const
	{
	return *iAgnServerFile;
	}

CStreamDictionary& CAgnCalendarConverter::Dictionary() const
	{
	return *iDictionary;
	}

void CAgnCalendarConverter::AddToIndexL(CAgnEntry& aEntry) const
	{
	iAgnServerFile->Model()->UpdateIndexL(aEntry, NULL, CAgnEntryModel::EAdd);
	}

void CAgnCalendarConverter::SetNextLocalUidValue(TInt aValue) const
	{
	iAgnServerFile->Model()->iNextLocalUidValue = aValue;
	}

void CAgnCalendarConverter::SetModelStreamIds(const TAgnVersion& aVersion,
											  const TStreamId& aEntrySetStreamId,
											  const TStreamId& aNextLocalUidValueStreamId,
											  const TStreamId& aEntryManagerStreamId,
											  const TStreamId& aFileInformationStreamId,
											  CAgnModelStreamIdSet& aModelStreamIdSet) const
	{
	aModelStreamIdSet.iVersion = aVersion;
	aModelStreamIdSet.iEntrySetStreamId = aEntrySetStreamId;
	aModelStreamIdSet.iNextLocalUidValueStreamId = aNextLocalUidValueStreamId;
	aModelStreamIdSet.iEntryManagerStreamId = aEntryManagerStreamId;
	aModelStreamIdSet.iFileInformationStreamId = aFileInformationStreamId;
	}
						   
/**
This method is executed as the last step of a calendar file conversion.
Externalizes to the new file store the new CAgnEntryManager and CAgnModelStreamIdSet.
Calls ReplaceAgendaFile() to update the Model and its ServerFile with the products of the
conversion.
@internalComponent */
void CAgnCalendarConverter::CompleteConversionL()
	{
	// Copy iFileId and iNextUniqueIdValue to the Converter's output stream store
	//
	iAgnServerFile->Model()->ExternalizeFileIdL(*iOutputStreamStore, iOutModelStreamIdSet->FileInformationStreamId());
	iAgnServerFile->Model()->ExternalizeNextUidValuesL(*iOutputStreamStore, iOutModelStreamIdSet->NextLocalUidValueStreamId());
	iOutputStreamStore->CommitL();

	// Commit the streamIdSet and the EntryManager to the output store
	//
	RStoreWriteStream stream;
	stream.ReplaceLC(*iOutputStreamStore,iOutModelStreamIdSet->EntryManagerStreamId());
	stream << *iOutEntryManager;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	
	iOutModelStreamIdSet->CommitL(*iOutputStreamStore);
	iOutputStreamStore->CommitL();
	
	delete iDictionary;
	iDictionary = NULL;
	delete iOutputStreamStore;
	iOutputStreamStore = NULL;
	
	// Set new calendar file and dictionary in CAgnServFile
	// (this deletes the old file, and hands over the ownership of iOutputTzRuleIndex)
	//
	iAgnServerFile->ReplaceConvertedAgendaFileL(*iOutEntryManager, *iOutputTzRuleIndex);
	iOutputTzRuleIndex = NULL;
	}
	
/**
This method stores in CAgnCalendarConverter::iOutputStreamStore the entry that has been internalized
from an old version file. 

The persistence layer objects owned by the CAgnCalendarConverter (iOutEntryManager and iOutModelStreamIdSet)
will also be updated to register the new entry.
@internalComponent */
void CAgnCalendarConverter::SaveRestoredEntryL(CAgnEntry& aEntry) const
	{
	
	// Externalize outline attributes first
	TStreamId id = KNullStreamId;
	RStoreWriteStream out;
	
	id = WriteDescriptorToStreamL(aEntry.Description());
	aEntry.SetDescriptionStreamId(id);
	
	id = WriteDescriptorToStreamL(aEntry.Summary());
	aEntry.SetSummaryStreamId(id);
	
	if(aEntry.AlarmAction())
		{
		RStoreWriteStream out;
		id = out.CreateLC(*iOutputStreamStore);
		out << *aEntry.AlarmAction();
		out.CommitL();
		CleanupStack::PopAndDestroy(); //out
		aEntry.SetAlarmActionStreamId(id);
		}

	iOutputTzRuleIndex->AddTzRuleL(aEntry);
	TStreamId newStreamId = iOutEntryManager->AddEntryL(aEntry); // This changes the entry/instance ID

	if (newStreamId != KNullStreamId)
		{
		// This puts the stream id in the array but doesn't commit it
		// To reduce file acces, the ModelStreamIdSet is only commited when all
		// the entries have been converted.
		iOutModelStreamIdSet->EntryStreamIdSet().AddL(newStreamId);
		}
	}

TStreamId CAgnCalendarConverter::WriteDescriptorToStreamL(const TDesC& aString) const
/** @internalComponent */
	{
	if (aString.Length() > 0)
		{
		RStoreWriteStream out;
		TStreamId id = out.CreateLC(*iOutputStreamStore);
		out.WriteUint32L(aString.Length());
		out << aString;
		out.CommitL();
		CleanupStack::PopAndDestroy(); //out
		return id;
		}
	return KNullStreamId;
	}
	
// CAgnCalendarConverter209 //

CAgnCalendarConverter209::CAgnCalendarConverter209()
	{
	}
	
CAgnCalendarConverter209* CAgnCalendarConverter209::NewL(CAgnServFile& aAgnServerFile)
	{
	CAgnCalendarConverter209* converter = new (ELeave) CAgnCalendarConverter209();
	CleanupStack::PushL(converter);
	converter->ConstructL(aAgnServerFile);
	CleanupStack::Pop(converter);
	return converter;
	}
	
void CAgnCalendarConverter209::InternalizeEntriesL(RReadStream& aStream)
/** @internalComponent */
	{
	// The layout of the calendar file in 1.1.x puts a set of calendar entries first, followed by their 
	// extended data. For this reason, the entire set must be loaded into memory before they can be stored.
	RPointerArray<CAgnEntry> entryArray(TAgnEntryId::EMaxNumEntriesPerStream);
	CleanupResetAndDestroyPushL(entryArray);
	
	RArray<TBool> isExtendedArray;
	CleanupClosePushL(isExtendedArray);
	
	const TUint8 KCount = aStream.ReadUint8L();
	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		// Read in entries
		TBool isExtended = EFalse;
		CAgnEntry* entry = InternalizeEntryL(aStream, isExtended);
		CleanupStack::PushL(entry);
		entryArray.AppendL(entry);
		CleanupStack::Pop(entry);
		
		isExtendedArray.AppendL(isExtended); // isExtended is set to ETrue if the entry has extended data (to be loaded later)
		}
	
	for ( TInt i = 0; i < KCount; ++i )
		{
		// Now read in the extended attributes if necessary
		if (isExtendedArray[i])
			{
			InternalizeExtendedEntryL(aStream, *entryArray[i]);
			}

		// The old aEntry has been internalized, so add it to the current version of the file
	    SaveRestoredEntryL(*entryArray[i]);
	    
	    // add this entry to the index
	    AddToIndexL(*entryArray[i]);
		}

	CleanupStack::PopAndDestroy(&isExtendedArray);
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CAgnCalendarConverter209::InternalizeNextUidValuesL(RReadStream& aStream)
	{
	SetNextLocalUidValue(aStream.ReadUint32L());
	}

/**
This method internalizes the CAgnModelStreamIdSet according to the file version.
It also copies to the Converter's own stream store the contents of 
aModelStreamIdSet's streams that must be preserved
during the conversion: iNextUniqueIdValue and iFileId.

(Currently, it works for both 9.1 and 9.2 to 9.3)
@internalComponent */
void CAgnCalendarConverter209::InternalizeModelStreamIdSetL(RReadStream& aStream, CAgnModelStreamIdSet& aModelStreamIdSet)
	{
	// 9.2/9.1 was:                                     9.3 is:
	// iVersion                                         iVersion
	// iTodoListListStreamId
	// iDeletedTodoListListStreamId
	// iEntrySetStreamId                                iEntrySetStreamId
	// iFormatLayerStreamId
	// iNextLocalUidValueStreamId                       iNextLocalUidValueStreamId
	// iObserverControllerStreamId
	// iLastSynchronizedDateStreamId
	// iEntryStoreStreamId                              iEntryManagerStreamId
	// iFileInformationStreamId                         iFileInformationStreamId
		
	// Get the 9.1/9.2 bytes. Those that exit in 9.3, set them in the CAgnModelStreamIdSet.
	// Skip the rest.
	TStreamId disposableStreamId;
	
	TAgnVersion version;
	aStream >> version;
	
	//Skip: iTodoListStreamId, iDeletedTodoListListStreamId
	aStream >> disposableStreamId;
	aStream >> disposableStreamId;
	
	TStreamId entrySetStreamId;
	aStream >> entrySetStreamId;
	
	//Skip: iFormatLayerStreamId
	aStream >> disposableStreamId;
	
	TStreamId nextLocalUidValueStreamId;
	aStream >> nextLocalUidValueStreamId;
	
	//Skip: iObserverControllerStreamId, iLastSynchronizedDateStreamId;
	aStream >> disposableStreamId;
	aStream >> disposableStreamId;
	
	TStreamId entryManagerStreamId;
	aStream >> entryManagerStreamId;
	
	TStreamId fileInformationStreamId;
	aStream >> fileInformationStreamId;
	
	SetModelStreamIds(version, entrySetStreamId, nextLocalUidValueStreamId, entryManagerStreamId, fileInformationStreamId, aModelStreamIdSet);
	}

/**
Categories have not changed from 9.1 to 9.3.
Just copy them from the original file to the new file.

@internalComponent */
void CAgnCalendarConverter209::InternalizeCategoriesL()
	{
	TStreamId streamId = AgnServerFile().WriteCategoryListL(OutputStreamStore());
	AgnServerFile().AddStreamToDictionaryL(KUidAgnCategoryList, streamId, Dictionary(), OutputStreamStore());
	}

CAgnEntry* CAgnCalendarConverter209::InternalizeEntryL(RReadStream& aStream, TBool& aHasExtended)
	{
	enum TAgnEntryTypeV9192 // the entry type enumeration in v9.1 and v9.2
		{
		EAgnAppt,
		EAgnTodo,
		EAgnEvent,
		EAgnAnniv
		};
	
	const TInt KAgnExtendedDataFlag = 0x80;
    // Remove 9.1's/9.2's KAgnExtendedDataFlag (0x80)
    // from the entry type
    //
    TUint8 typeData = aStream.ReadUint8L();
    
	if (typeData & KAgnExtendedDataFlag)
		{
		const TUint8 KVersion = aStream.ReadUint8L();
		__ASSERT_ALWAYS(KVersion !=0, User::Leave(KErrCorrupt));  //version mumbers started from 1
		if (KVersion > 1)
			{
			const TUint8 KLength = aStream.ReadUint8L();
			for (TInt i=0; i<KLength; ++i)
				{
				aStream.ReadUint8L(); //skip additional data becouse we don't support it yet
				}
			}
		typeData &= ~KAgnExtendedDataFlag;
		}
		
	const TAgnEntryTypeV9192 KV9192Type = static_cast<TAgnEntryTypeV9192>(typeData);
	
	CCalEntry::TType entryType = CCalEntry::EAppt;

	TAgnEntryId entryId;
	entryId.InternalizeL(aStream); //TUInt32 iId
	TUint16 attributes = aStream.ReadUint16L(); 
	
	switch (KV9192Type)
		{
		case EAgnAppt:
			if (attributes & EBit9)
				{
				entryType = CCalEntry::EReminder; // is a day note
				}
			else
				{
				entryType = CCalEntry::EAppt;
				}
			break;
		case EAgnAnniv:
			entryType = CCalEntry::EAnniv;
			break;
		case EAgnEvent:
			entryType = CCalEntry::EEvent;
			break;
		case EAgnTodo:
			entryType = CCalEntry::ETodo;
			break;
		default:
			User::Leave(KErrCorrupt);
		}
	
    if (attributes & EBit13)
    	{
    	aHasExtended = ETrue;
    	}
    else
    	{
    	aHasExtended = EFalse;
    	}
    	
    CCalEntry::TStatus status = CCalEntry::ENullStatus;
    // Get status from attributes' bits 
    //
    if (attributes & (EBit14 | EBit15 | EBit16))
    	{
    	// Bits 14,15,16 on.
    	status = CCalEntry::EVCalDelegated;
    	}
    else if (attributes & (EBit14 | EBit15))
    	{
    	// Bits 14, 15 on
    	if (entryType == CCalEntry::ETodo)
    		{
    		status = CCalEntry::ETodoCompleted;
    		}
    	}
    else if (attributes & (EBit14 | EBit16))
    	{
    	//Bits 14,16 on
    	status = CCalEntry::EVCalDeclined;
    	}
    else if (attributes & (EBit15 | EBit16))
    	{
    	// Bits 15, 16 on
    	status = CCalEntry::EVCalSent;
    	}
    else if (attributes & EBit14)
   	 	{
    	if (entryType != CCalEntry::ETodo)
    		{
    		status = CCalEntry::EConfirmed;
    		}
   	 	}
    else if (attributes & EBit15)
    	{
    	status = CCalEntry::EVCalNeedsAction;
    	}
    else if (attributes & EBit16)
    	{
    	status = CCalEntry::EVCalAccepted;
    	}
    else if(attributes & EBit11)
    	{
    	if (entryType != CCalEntry::ETodo)
    		{
    		status = CCalEntry::ETentative;
    		}
    	}
 
    // read GS data
   	enum TGsTypeV9192
		{
		/** Not a GS Entry (created using AgnModel APIs directly, not CalInterimAPI. */
		ENonGsEntry,
		/** Parent Entry. */
		EGsRootParent,
		/** Not used. */
		EGsParent,
		/** Child Entry. */
		EGsLeaf
		};
			
	CAgnEntry* entry = NULL;
	
	const TGsTypeV9192 KCalTypeV9192 = static_cast<TGsTypeV9192> (aStream.ReadInt8L());
	switch (KCalTypeV9192)
		{
		case EGsRootParent:
			{
		    aStream.ReadUint32L(); // ignore guid hash
		    HBufC8* guid = HBufC8::NewLC(aStream, KAgnEntryMaxGuidLength);   
		    
		    aStream.ReadInt8L(); // read other class type 
		    
		    entry = CAgnEntry::NewL(entryType, guid, CCalEntry::EMethodNone, 0); // dummy values for method and sequence number
			CleanupStack::Pop(guid);
			CleanupStack::PushL(entry);
			
		    TAgnCalendarTime recurId;

		    const TInt KCount = aStream.ReadInt32L(); 
		    for (TInt i = 0; i < KCount; ++i)
				{
				TCalLocalUid childId = aStream.ReadUint32L();	
				aStream >> recurId;
				
				TGsChildRefData childData(childId, recurId);
				entry->AddChildIdL(childData);
				}
		
			// set correct value for method and sequence number
			TInt32 seqNum = aStream.ReadInt32L();
			entry->SetSequenceNumber(seqNum);
			
			CCalEntry::TMethod method = static_cast<CCalEntry::TMethod> (aStream.ReadInt8L());
			entry->SetMethod(method);

			aStream.ReadUint32L(); // parent ID (Null for a parent)
			}
			break;
			
		case EGsLeaf:
			{
			TInt32 seqNum = aStream.ReadInt32L();
			CCalEntry::TMethod method = static_cast<CCalEntry::TMethod> (aStream.ReadInt8L());
			TCalLocalUid parentId = aStream.ReadUint32L(); 
			
		    entry = CAgnEntry::NewL(entryType, NULL, method, seqNum, TAgnCalendarTime(), CalCommon::EThisOnly);
		    CleanupStack::PushL(entry);
		    entry->SetParentId(parentId);
			}
			break;

		case EGsParent: // fall through
		case ENonGsEntry:
			// this is a non-GS entry, get the GUID from the unique ID, entry == NULL
			break;
		
		default:
			{
			User::Leave(KErrCorrupt);
			}
		}
    
    // Read in entry's unique Id
    //
   	TCalLocalUid localUid;
    aStream >> localUid;
    
    // create entry now we have all the information
	
	if (!entry) // create non-GS entry from unique ID
		{
    	TBuf8<32> uniqueIdBuf;
	    uniqueIdBuf.AppendNum(localUid);
	    HBufC8* guid = uniqueIdBuf.AllocLC();
		entry = CAgnEntry::NewL(entryType, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);
		}
		
    entry->SetStatus(status);
    entry->SetLocalUid(localUid);

    // TReplicationData was next in 9.1/9.2, with the following data:
    //            TStatus iStatus;
    //            TBool iHasBeenDeleted;
    //            TUint iCount;
    //            TAgnCalendarTime iLastChangedDateUtc
    //
	entry->SetReplicationStatusL(static_cast<CCalEntry::TReplicationStatus>(aStream.ReadInt8L()));

    TBool hasBeenDeleted = aStream.ReadUint8L();// Not used in 9.3 but checked later
    aStream.ReadUint8L();//skip iCount
    TAgnCalendarTime lastChangedDate;
	aStream >> lastChangedDate;
	entry->SetLastModifiedDateUtc(lastChangedDate.UtcL());
    
    // CAgnRptDef
    //
    // In 9.1/9.2, the second bit of the 'attributes' 
    // indicates presence/absence of a repeat definition
    // 
    CArrayFixFlat<TAgnCalendarTime>* exceptions = NULL;
    if (attributes & EBit2)
    	{
		exceptions = InternalizeRptDefL(aStream, *entry);
		}
	
	TInt32 alarmPreTime = InternalizeAlarmL(aStream, attributes, *entry);
	InternalizeSummaryL(aStream, hasBeenDeleted, *entry);
	InternalizeTypeSpecificDataL(aStream, attributes, *entry);
	AddExceptionsL(*entry, exceptions); // must add exceptions after start date has been set
	delete exceptions;
	
	// if the entry is floating, the until date must be converted to a floating TAgnCalendarTime
	if (entry->TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		if (entry->RptDef() && entry->RptDef()->RRule())
			{
			if (AgnDateTime::IsValidAgendaTTime(entry->RptDef()->RRule()->UntilTimeL().UtcL()))
				{
				TAgnCalendarTime agnUntilTime;
				agnUntilTime.SetFloatingL(entry->RptDef()->RRule()->UntilTimeL().UtcL());
				entry->RptDef()->SetUntilTime(agnUntilTime);
				}
			}
		}
	
	// if the entry has an alarm, calculate the offset from the pretime used in v9.1/v9.2
	if (alarmPreTime != KMaxTInt32)
		{
		if (KV9192Type == EAgnTodo)
			{
			entry->SetAlarmOffset(CalculateAlarmOffsetL(alarmPreTime, entry->EndTime().LocalL()));
			}
		else
			{
			entry->SetAlarmOffset(CalculateAlarmOffsetL(alarmPreTime, entry->StartTime().LocalL()));
			}
		}
	
	CleanupStack::Pop(entry);
	
	return entry;
	}

CArrayFixFlat<TAgnCalendarTime>* CAgnCalendarConverter209::InternalizeRptDefL(RReadStream& aStream, CAgnEntry& aEntry) const
	{
	CAgnRptDef* rptDef = CAgnRptDef::NewL(aEntry);
	CleanupStack::PushL(rptDef);

	InternalizeRepeatRuleL(aStream, *rptDef);
	CArrayFixFlat<TAgnCalendarTime>* exceptions = InternalizeExceptionsL(aStream);
	CleanupStack::PushL(exceptions);
	InternalizeSporadicDatesL(aStream, *rptDef);
		
	InternalizeTimeZoneL(aStream, *rptDef);	
			
	aStream.ReadUint8L(); // ignore show next only
	
	aEntry.SetRptDefL(*rptDef);
	
	CleanupStack::Pop(exceptions);
	CleanupStack::PopAndDestroy(rptDef);
	return exceptions;
	}

void CAgnCalendarConverter209::AddExceptionsL(CAgnEntry& aEntry, CArrayFixFlat<TAgnCalendarTime>* aExceptions) const
	{
	// must add exceptions after sporadic dates
	if (aExceptions && aEntry.RptDef())
		{
		const TInt KCount = aExceptions->Count();
		if (KCount > 0)
			{
			for (TInt i = 0; i < KCount; ++i)
				{
				aEntry.RptDef()->AddExceptionL((*aExceptions)[i]);
				}
			}
		}
	}

void CAgnCalendarConverter209::InternalizeRepeatRuleL(RReadStream& aStream, CAgnRptDef& aRptDef) const
	{
    TBool hasRepeatRule = aStream.ReadUint8L();

	// TAgnRpt 
	//
	// This data member of CAgnRptDef is different in 9.3 
    //
	if (hasRepeatRule)
		{
		TAgnRpt* rule = NULL;
		TAgnRpt::TType rptType = static_cast<TAgnRpt::TType>(aStream.ReadUint8L());
		
		// Intenalize data in base class for repeat definitions
		// (TAgnRpt)
		//
		TInt64 timeInt64;
		aStream >> timeInt64; // ignore start time
		aStream >> timeInt64;
		TAgnCalendarTime untilTime;
		untilTime.SetUtcL(timeInt64); // not right if the entry is floating, this must be fixed once the time mode is known

		TUint16 interval;
		aStream >> interval;
		
		// skip iRptNextOnly and iRptForever (not used in 9.3)
		aStream.ReadUint8L();
		aStream.ReadUint8L();
		
		// Internalize data in classes derived from TAgnRpt
		//
		switch(rptType)
			{
			case TAgnRpt::EDaily:
				//Nothing else to internalize for rpt rule
				rule = new (ELeave) TAgnDailyRpt(aRptDef);
				CleanupStack::PushL(rule);
				break;
			case TAgnRpt::EWeekly:
				{
				rule = new (ELeave) TAgnWeeklyRpt(aRptDef);
				CleanupStack::PushL(rule);
				TAgnWeeklyRpt* weeklyRule = static_cast<TAgnWeeklyRpt*>(rule);
				TUint8 weeklyRptDays = aStream.ReadUint8L();
				if (weeklyRptDays & EBit1)
					{
					weeklyRule->SetDay(EMonday);	
					}
				if (weeklyRptDays & EBit2)
					{
					weeklyRule->SetDay(ETuesday);
					}
				if (weeklyRptDays & EBit3)
					{
					weeklyRule->SetDay(EWednesday);
					}
				if (weeklyRptDays & EBit4)
					{
					weeklyRule->SetDay(EThursday);
					}
				if (weeklyRptDays & EBit5)
					{
					weeklyRule->SetDay(EFriday);
					}
				if (weeklyRptDays & EBit6)
					{
					weeklyRule->SetDay(ESaturday);
					}
				if (weeklyRptDays & EBit7)
					{
					weeklyRule->SetDay(ESunday);
					}

				TDay firstDayOfWeek = static_cast<TDay>(aStream.ReadUint8L());
				weeklyRule->SetFirstDayOfWeek(firstDayOfWeek);
				}
				break;
			case TAgnRpt::EMonthlyByDates:
				{
				rule = new (ELeave) TAgnMonthlyByDatesRpt(aRptDef);
				CleanupStack::PushL(rule);
				TInt32 monthlyRptDates;
				aStream >> monthlyRptDates;
				for (TInt i = 0; i <= 30; ++i) // check 31 dates
					{
					TInt dateToCheck = 1 << i;
					if (monthlyRptDates & dateToCheck)
						{
						static_cast<TAgnMonthlyByDatesRpt*>(rule)->SetDate(i);
						}
					}
				}
				break;
			case TAgnRpt::EMonthlyByDays:
				{
				rule = new (ELeave) TAgnMonthlyByDaysRpt(aRptDef);
				CleanupStack::PushL(rule);
				TAgnMonthlyByDaysRpt* monthlyRule = static_cast<TAgnMonthlyByDaysRpt*>(rule);
				
				TInt weekInMonth = 0;
				while (weekInMonth <= 4) // 5 weeks in month (1st, 2nd, 3rd, 4th, last)
					{
					TUint8 monthlyRptDays = aStream.ReadUint8L();
					if (monthlyRptDays & EBit1)
						{
						monthlyRule->SetDay(EMonday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));	
						}
					if (monthlyRptDays & EBit2)
						{
						monthlyRule->SetDay(ETuesday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));
						}
					if (monthlyRptDays & EBit3)
						{
						monthlyRule->SetDay(EWednesday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));
						}
					if (monthlyRptDays & EBit4)
						{
						monthlyRule->SetDay(EThursday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));
						}
					if (monthlyRptDays & EBit5)
						{
						monthlyRule->SetDay(EFriday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));
						}
					if (monthlyRptDays & EBit6)
						{
						monthlyRule->SetDay(ESaturday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));
						}
					if (monthlyRptDays & EBit7)
						{
						monthlyRule->SetDay(ESunday, static_cast<TAgnRpt::TWeekInMonth>(weekInMonth));
						}
					++weekInMonth;
					}
				}
				break;
			case TAgnRpt::EYearlyByDay:
				{
				rule = new (ELeave) TAgnYearlyByDayRpt(aRptDef);
				CleanupStack::PushL(rule);
				TAgnRpt::TWeekInMonth weekInMonth = static_cast<TAgnRpt::TWeekInMonth>(aStream.ReadUint8L());
				TDay day = static_cast<TDay>(aStream.ReadUint8L());
				TDateTime untilDateTime = untilTime.LocalL().DateTime();
				static_cast<TAgnYearlyByDayRpt*>(rule)->FindStartDayL(day, weekInMonth, untilDateTime.Month(), untilDateTime.Year());
				}
				break;
			case TAgnRpt::EYearlyByDate:
				rule = new (ELeave) TAgnYearlyByDateRpt(aRptDef);
				CleanupStack::PushL(rule);
				break;
			default:
				User::Leave(KErrCorrupt);
			}
		rule->SetInterval(interval);
		
		aRptDef.SetRRuleL(*rule);
		aRptDef.SetUntilTime(untilTime);
		CleanupStack::PopAndDestroy(rule);
		}
	}

CArrayFixFlat<TAgnCalendarTime>* CAgnCalendarConverter209::InternalizeExceptionsL(RReadStream& aStream) const
	{
	CArrayFixFlat<TAgnCalendarTime>* exceptionArray = NULL;
	if (aStream.ReadUint8L())
		{
		const TInt KExceptionArrayGranularity = 4;
		
		exceptionArray = new (ELeave) CArrayFixFlat<TAgnCalendarTime>(KExceptionArrayGranularity);
		CleanupStack::PushL(exceptionArray);
		aStream >> *exceptionArray;
		CleanupStack::Pop(exceptionArray);
		}
	return exceptionArray;
	}

void CAgnCalendarConverter209::InternalizeSporadicDatesL(RReadStream& aStream, CAgnRptDef& aRptDef) const
	{
	const TInt KCount = aStream.ReadUint16L();
	if (KCount > 0)
		{
		TAgnCalendarTime time;
		for (TInt i = 0; i < KCount; ++i)
			{
			aStream >> time;
			aRptDef.AddSporadicDateL(time);
			}
		}
	}

void CAgnCalendarConverter209::InternalizeTimeZoneL(RReadStream& aStream, CAgnRptDef& aRptDef) const
	{
	// Time Zone  
	TBool hasTimeZone = aStream.ReadUint8L();
	if (hasTimeZone)
		{
		CTzRules* tzRules = CTzRules::NewL(aStream);
		CleanupStack::PushL(tzRules);
		aRptDef.SetTimeZoneL(*tzRules);
		CleanupStack::PopAndDestroy(tzRules);
		}
	}

TInt32 CAgnCalendarConverter209::InternalizeAlarmL(RReadStream& aStream, TInt16 aAttributes, CAgnEntry& aEntry) const
	{
	// In 9.1/9.2, the eigth bit of the 'attributes' 
	// indicates presence/absence of a entry symbol
	//
	if (aAttributes & EBit8)
		{
		aStream.ReadInt16L();//Skip entry symbol (not used in 9.3)
		}
		
	// Alarm data
	//
	// In 9.1/9.2, the bits 1 (EIsCrossedOut) and 5(EHasAlarm)
	// of 'attributes' are used to tell if alarm information
	// has been externalized or not
	//
	TInt32 alarmPreTime = KMaxTInt32;
	if ( ! (aAttributes & EBit1)) 
		{
		// Entry is not a completed todo
		if (aAttributes & EBit5) 
			{
			// Entry has alarm info
			HBufC* alarmSoundName = HBufC::NewLC(aStream, KMaxAlarmSoundNameLength);
			if (alarmSoundName->Length() > 0)
				{
				aEntry.SetAlarmSoundNameL(*alarmSoundName);
				}
			CleanupStack::PopAndDestroy(alarmSoundName);
				
			alarmPreTime = aStream.ReadInt32L();
			// alarm offset is converted as an offset later once entry time is known
			}
		}
	return alarmPreTime;
	}

void CAgnCalendarConverter209::InternalizeSummaryL(RReadStream& aStream, TBool aHasBeenDeleted, CAgnEntry& aEntry) const
	{
	// Rich Text
	// In 9.1/9.2 may contain several streamIds (Styles,MarkupData,text);
	// It may be stored inline or offline.
	// In 9.3, only the text is wanted for the 'Summary'.
	// 
	if ( ! aHasBeenDeleted)
		{
		TBool isStoredInline = TBool(aStream.ReadInt8L());
		
		// Create auxiliary RichText object
		CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
		CleanupStack::PushL(paraFormatLayer);
		
		CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
		CleanupStack::PushL(charFormatLayer);
		
		CRichText *auxRichText = CRichText::NewL(paraFormatLayer,charFormatLayer,CEditableText::EFlatStorage,32);
		CleanupStack::PushL(auxRichText);
		// Populate the rich text object
		//
		if (isStoredInline)
			{
			auxRichText->InternalizeStyleDataL(aStream);
			TBool hasMarkupData=TBool(aStream.ReadUint8L());
			if (hasMarkupData)
				{
				auxRichText->InternalizeMarkupDataL(aStream);
				}
			auxRichText->InternalizePlainTextL(aStream);
			}
		else
			{
			// Rich Text stored offline
			TStreamId richTextStreamId;
			aStream >> richTextStreamId;
			if (richTextStreamId != KNullStreamId)
				{
				RStoreReadStream richTextStream;
				CleanupClosePushL(richTextStream);
				richTextStream.OpenL(InputStreamStore(),richTextStreamId);
				CPersistentStore* embeddedStore = CEmbeddedStore::FromLC(richTextStream);
				auxRichText->RestoreL(*embeddedStore,embeddedStore->Root());
				CleanupStack::PopAndDestroy(embeddedStore);
				CleanupStack::PopAndDestroy(&richTextStream); // richTextStream.Close()
				}
			}
		
		// Get the text if there's any and leave it in the entry
		//
		if (auxRichText->DocumentLength() > 0)
			{
			HBufC* summaryBuffer = HBufC::NewLC(auxRichText->DocumentLength());
			TPtr summaryBufPtr(summaryBuffer->Des());
			auxRichText->Extract(summaryBufPtr);
			
			// Set the summary in the entry
			// 
			aEntry.SetSummary(summaryBuffer);
			CleanupStack::Pop(summaryBuffer);
			}
		
		CleanupStack::PopAndDestroy(auxRichText);
		CleanupStack::PopAndDestroy(charFormatLayer);
		CleanupStack::PopAndDestroy(paraFormatLayer);
		}
	else
		{
		// Deleted entry has no rich text. No Summary.
		aEntry.SetSummaryStreamId(KNullStreamId);
		}
	}

void CAgnCalendarConverter209::InternalizeTypeSpecificDataL(RReadStream& aStream, TInt16 aAttributes, CAgnEntry& aEntry) const
	{
	//
	// Restore data dependent on entry type (stored with StoreSpecificsL in 9.1/2)
	//
	TAgnCalendarTime time;
	
	const CCalEntry::TType KEntryType = aEntry.Type();
	switch(KEntryType)
		{
		case CCalEntry::EAppt:
		case CCalEntry::EReminder:
		case CCalEntry::EEvent:
		case CCalEntry::EAnniv:
			{
			aStream >> time;

			TAgnCalendarTime endTime;
			aStream >> endTime;;
			
			if ( ! endTime.IsSet())
				{
				aEntry.SetStartAndEndTimeL(time, time);
				}
			else
				{
				aEntry.SetStartAndEndTimeL(time, endTime);
				}
				
			if (KEntryType == CCalEntry::EAnniv || KEntryType == CCalEntry::EEvent)
				{
				aStream >> time; // Events and Anniversaries have a display time, not used in 9.3
				if (KEntryType == CCalEntry::EAnniv)
					{
					aStream.ReadInt16L(); // Skip iBaseYear
					aStream.ReadUint8L(); // Skip iDisplayAs
					aStream.ReadInt8L(); // Skip iHasBaseYear
					}
				}
		    break;
			}
		    
		case CCalEntry::ETodo:
			{
		    aStream.ReadInt32L(); // Skip iTodoListId
		    aStream >> time; // Was iDueDate in 9.1/2
		    if(aAttributes & EBit1)
		    	{
		    	// Is a crossed out (completed) Todo
		    	//
		    	TAgnCalendarTime completedDate;
		    	aStream >> completedDate;
		    	aEntry.SetCompletedDateUtcL(completedDate.UtcL());
		    	}
		    else
		    	{
		    	// This is not a completed Todo
		    	aEntry.SetCompletedDateUtcL(Time::NullTTime());
		    	}
		    
		    if (time.IsSet()) // = end time
		    	{
		    	TInt duration = aStream.ReadUint16L();
		    	TTimeIntervalDays intervalDays(duration);	    
		    	TAgnCalendarTime startTime;
		    	if (time.TimeMode() == MAgnCalendarTimeMode::EFloating)
		    		{
		    		startTime.SetFloatingL(time.LocalL() - intervalDays);
		    		}
		    	else
		    		{
		    		startTime.SetLocalL(time.LocalL() - intervalDays);
		    		}
				aEntry.SetStartAndEndTimeL(startTime, time);
		    	}
		    
         	aStream.ReadUint8L();// Skip iAlarmFrom
		    aStream >> time; // Not used in 9.3
         	aEntry.SetPriority(aStream.ReadUint8L()); // For Todos, use this priority; for other entry types, take priority from the extended entry
		    aStream.ReadUint8L(); //Skip iDisplayDueDateAs
			}
		    break;
		}
	}

/** Algorithm to calculate the alarm offset from the 'pretime' and the 'alarm origin'.
This algorithm is used in v9.1 and v9.2 code. The alarm origin is the instance time corresponding to the alarm.
The pretime is the value saved to file, it is a combination of the minute of the day (0-1440) and the number of
days before the event that the alarm occurs.
@internalComponent */
TInt32 CAgnCalendarConverter209::CalculateAlarmOffsetL(TInt32 aPreTime, const TTime& aAlarmOrigin) const
	{
	// copied from TAgnAlarmPreTime::AlarmInstanceDateTime and TAgnAlarmPreTime::AlarmTime
	TDateTime alarmOriginDateTime = aAlarmOrigin.DateTime();
	TInt alarmOriginTimeOfDay = alarmOriginDateTime.Hour()*60 + alarmOriginDateTime.Minute();
	
	TUint daysWarning = (aPreTime - alarmOriginTimeOfDay) / KAgnMinutesInADay;
	TUint time = KAgnMinutesInADay - ((aPreTime - alarmOriginTimeOfDay) % KAgnMinutesInADay);
	if (time == KAgnMinutesInADay)
		{
		time = 0;
		if (daysWarning > 0)
			{
			daysWarning -= 1;
			}
		}

	TTime alarmTime = aAlarmOrigin - TTimeIntervalDays(daysWarning);
	TDateTime alarmDateTime = alarmTime.DateTime();
	alarmDateTime.SetHour(time / 60);
	alarmDateTime.SetMinute(time % 60);
	TTimeIntervalMinutes offset;
	aAlarmOrigin.MinutesFrom(TTime(alarmDateTime), offset);
	return offset.Int();
	}

void CAgnCalendarConverter209::InternalizeExtendedEntryL(RReadStream& aStream, CAgnEntry& aEntry)
/** @internalComponent */
	{
    // Internalize information that in 9.1/9.2 lives in the Extended entry
    //
    TInt id = aStream.ReadUint32L(); //Not in 9.3, but needed later in this method
    TInt size = aStream.ReadUint32L();
    
    // Skip GlobalId
    TBuf<32> dummyBuffer;
    if (size)
    	{
    	aStream.ReadL(dummyBuffer,size);
    	}
    
    // Populate location
    //
	size = aStream.ReadUint32L();
	if (size)
		{
		HBufC* location = HBufC::NewLC(size);
		TPtr bufPtr = location->Des();
		aStream.ReadL(bufPtr,size);
		aEntry.SetLocationL(*location);
		CleanupStack::PopAndDestroy();
		}
    
    // Populate attendee list
    // 
    size = aStream.ReadUint32L();
	
	RPointerArray<CAgnAttendee> attendees;
	CleanupClosePushL(attendees);
	
	for (TInt count=0; count < size; count++)
		{
		TBool organizer = EFalse;
		CAgnAttendee* attendee = InternalizeAttendeeL(aStream, organizer);
		if (organizer)
			{
			aEntry.SetOrganizerL(attendee);
			}
		else
			{
			aEntry.AddAttendeeL(attendee);
			}
		attendees.AppendL(attendee);
		}
	
	// populate the phoneowner/organizer fields 
	//
	size = aStream.ReadUint32L();
	
	if(size > 0 && size <= attendees.Count())
		{
		aEntry.SetPhoneOwnerL(attendees[size-1]);
		}
	
	size = aStream.ReadUint32L();
	
	if(size > 0 && size <= attendees.Count())
		{
		CAgnAttendee* organizer = attendees[size-1]->CloneL();
		CleanupStack::PushL(organizer);
		aEntry.SetOrganizerL(organizer);
		CleanupStack::Pop(organizer);
		}
	CleanupStack::PopAndDestroy(&attendees); // attendees.Close()
	
	// Populate DT stamp
	//
	TAgnCalendarTime dtStamp;
	aStream >> dtStamp;
	aEntry.SetDTStampUtcL(dtStamp.UtcL());
	
	TInt expansion = aStream.ReadUint8L(); // read number of bytes in expansion 
	// Only try to internalize category information if it exists
	if (expansion>0)
		{
		
		// Read in categories
		//
		size = aStream.ReadUint32L();

		for (TInt ccount=0; ccount<size; ccount++)
			{
			CAgnCategory* category = InternalizeCategoryL(aStream);
			CleanupStack::PushL(category);
			aEntry.AddCategoryL(category);
			CleanupStack::Pop(category);
			}
		
		// Skip iBackgroundSymbolColor
		aStream.ReadUint8L();
		aStream.ReadUint8L();
		aStream.ReadUint8L();
		
		// If it's the new file format read in the Stream Id for the notes text
		const TInt KExtendedAttributesIdv1_0 = 0x110000F2; // from 9.1 / 9.2
		
		if (id == KExtendedAttributesIdv1_0) 
			{
			// Set the priority for entries that are not Todos (for Todos it's been set already)
			if (aEntry.Type() != CCalEntry::ETodo)
				{
				aEntry.SetPriority(aStream.ReadUint8L());
				}
			else
				{
				// Skip the priority
				aStream.ReadUint8L();
				}
			
			// 	9.1/2 "Notes" field is "Description" in 9.3
			TStreamId descriptionStreamId;
			aStream >> descriptionStreamId;
			
			if(descriptionStreamId != KNullStreamId) 
				{
				// Put the description in the aEntry
				//
				RStoreReadStream in;
				in.OpenLC(InputStreamStore(), descriptionStreamId);
				TInt textLength = in.ReadUint32L();
				if(textLength > 0)
					{
					HBufC* text = HBufC::NewL(in, textLength);
					aEntry.SetDescription(text);
					}
				CleanupStack::PopAndDestroy(); //in
				}
	
			// Ignore iNotesClipboardStreamId
			TStreamId clipboardStreamId;
			aStream >> clipboardStreamId;	
			}
		}
	}

CAgnCategory* CAgnCalendarConverter209::InternalizeCategoryL(RReadStream& aStream) const
	{
	CCalCategory::TCalCategoryType categoryType = static_cast<CCalCategory::TCalCategoryType>(aStream.ReadUint8L());
	
	TInt size = aStream.ReadUint32L();
	HBufC* extendedCategoryName = HBufC::NewLC(size);
	TPtr bufPtr = extendedCategoryName->Des();
	aStream.ReadL(bufPtr,size);

	CAgnCategory* category = NULL;
	if (categoryType == CCalCategory::ECalExtended || extendedCategoryName->Length() > 0)
		{
		category = CAgnCategory::NewL(*extendedCategoryName);
		}
	else
		{
		category = CAgnCategory::NewL(categoryType);
		}
	CleanupStack::PopAndDestroy(extendedCategoryName);
	return category;
	}

CAgnAttendee* CAgnCalendarConverter209::InternalizeAttendeeL(RReadStream& aStream, TBool& aOrganizer) const
	{
	CAgnAttendee::TAgnRole agnRole = static_cast<CAgnAttendee::TAgnRole>(aStream.ReadUint8L());
	aStream.ReadUint8L();// Skip iStatus
	TBool responseRequested = aStream.ReadUint8L();
	aStream.ReadUint8L(); // ignore expect value
	CCalAttendee::TCalRole calRole = static_cast<CCalAttendee::TCalRole>(aStream.ReadUint8L());
	CCalAttendee::TCalStatus calStatus = static_cast<CCalAttendee::TCalStatus>(aStream.ReadUint8L());
		
	//	read in Address
	TInt size = aStream.ReadUint32L();
	HBufC* address = HBufC::NewLC(size);
	TPtr bufPtr = address->Des();
	aStream.ReadL(bufPtr,size);
	
	size = aStream.ReadUint32L();
	HBufC* commonName = HBufC::NewLC(size);
	TPtr bufPtr2 = commonName->Des();
	aStream.ReadL(bufPtr2,size);	

	size = aStream.ReadUint32L();
	HBufC* sentBy = HBufC::NewLC(size);
	TPtr bufPtr3 = sentBy->Des();
	aStream.ReadL(bufPtr3,size);

	CAgnAttendee* attendee = CAgnAttendee::NewL(*address, *sentBy);
	CleanupStack::PushL(attendee);
	attendee->SetCommonNameL(*commonName);
	
	attendee->SetResponseRequested(responseRequested);
	attendee->SetRole(agnRole);
	if (agnRole == CAgnAttendee::EOrganizer)
		{
		aOrganizer = ETrue;
		}
	attendee->SetCalStatus(calStatus);
	
	CleanupStack::Pop(attendee);
	CleanupStack::PopAndDestroy(sentBy);
	CleanupStack::PopAndDestroy(commonName);
	CleanupStack::PopAndDestroy(address);

	return attendee;
	}
	
// CAgnCalendarConverter210 //

CAgnCalendarConverter210::CAgnCalendarConverter210()
	{
	}

CAgnCalendarConverter210* CAgnCalendarConverter210::NewL(CAgnServFile& aAgnServerFile)
	{
	CAgnCalendarConverter210* converter = new (ELeave) CAgnCalendarConverter210();
	CleanupStack::PushL(converter);
	converter->ConstructL(aAgnServerFile);
	CleanupStack::Pop(converter);
	return converter;
	}

void CAgnCalendarConverter210::InternalizeExtendedEntryL(RReadStream& aStream, CAgnEntry& aEntry)
	{
	CAgnCalendarConverter209::InternalizeExtendedEntryL(aStream, aEntry);
	InternalizeAlarmActionL(aStream, InputStreamStore(), aEntry);
	}

/** 
Internalize alarm action
@internalComponent */
void CAgnCalendarConverter210::InternalizeAlarmActionL(RReadStream& aStream, CStreamStore& aInputStreamStore, CAgnEntry& aEntry)
	{
	TStreamId alarmActionStreamId;
	aStream >> alarmActionStreamId;
	if ( alarmActionStreamId != KNullStreamId)
		{
       	RStoreReadStream in;
	   	in.OpenLC(aInputStreamStore, alarmActionStreamId);
		CAgnContent* alarmAction = new (ELeave) CAgnContent;
		CleanupStack::PushL(alarmAction);
		in >> *alarmAction;
		aEntry.SetAlarmAction(alarmAction);
		CleanupStack::Pop(alarmAction);
		CleanupStack::PopAndDestroy(); //in
  		}
	}

// CAgnCalendarConverter211 //
CAgnCalendarConverter211* CAgnCalendarConverter211::NewL(CAgnServFile& aAgnServerFile)
	{
	CAgnCalendarConverter211* converter = new (ELeave) CAgnCalendarConverter211();
	CleanupStack::PushL(converter);
	converter->ConstructL(aAgnServerFile);
	CleanupStack::Pop(converter);
	return converter;
	}

CAgnCalendarConverter211::CAgnCalendarConverter211()
	{
	}

CAgnCalendarConverter211::~CAgnCalendarConverter211()
	{
	delete iInputTzRuleIndex;
	}

void CAgnCalendarConverter211::ConstructL(CAgnServFile& aAgnServerFile)
	{
	CAgnCalendarConverter::ConstructL(aAgnServerFile);
	iInputTzRuleIndex = CAgnTzRuleIndex::NewL(*aAgnServerFile.Dictionary(), *aAgnServerFile.StoreL());
	}

void CAgnCalendarConverter211::InternalizeTimeZoneL(RReadStream& aStream, CAgnRptDef& aRptDef) const
	{
 	TBool isSystemRule = aStream.ReadUint8L();
 	TStreamId streamId;
 	aStream >> streamId;
 	
 	if(streamId != KNullStreamId)
 		{
 		CTzRules* tzRule = iInputTzRuleIndex->FindTzRuleByStreamIdL(streamId, isSystemRule);
 		CleanupStack::PushL(tzRule);
 		aRptDef.SetTimeZoneL(*tzRule);
 		CleanupStack::PopAndDestroy(tzRule);
 		}
	}


