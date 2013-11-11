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
// Name of test harness: T_MIUT07
// Component: IMCM
// Owner: EH
// Brief description of test harness:
// Tests CImOffLineOperation.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT07\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT07.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT07.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT07.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT07.exe on the other platform
// 
//

#include "emailtestutils.h"

#include <offop.h>
#include "MIUTLOG.H"  // used for Logs

#include "CFakeStoreManager.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#endif

//
// TestScheduler implements an error handler that doesn't panic.
class TestScheduler : public CActiveScheduler
	{
	public:
		void Error (TInt aError) const;
	};

//
// Session observer, reports stuff it is notified about
class CDummySessionObserver : public CBase , public MMsvSessionObserver
	{
	public:
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};

//
class CDummyUndo : public MImUndoOffLineOperation 
	{
	public:
	    void UndoOffLineChangesL(const CImOffLineOperation& /*aDeleted*/, TMsvId /*aFolderId*/) {}; 
	};

//
class CDummyStoreObserver : public CBase, public MMsvStoreObserver
	{
	public:
		void HandleStoreEvent(TMsvStoreEvent /*aEvent*/, TMsvId /*aId*/) {};
	};

//
class CDerivedMsvStore : public CMsvStore
	{
	public:
		CDerivedMsvStore(MMsvStoreObserver& aObserver, RFs& aFs, TMsvId aId, MMsvStoreManager& aStoreManager)
			: CMsvStore(aObserver, aFs, aId, aStoreManager) {};
	public:
		static CMsvStore* MyOpenForReadL(MMsvStoreObserver& aObserver, RFs& aFs, MMsvStoreManager& aStoreManager, TMsvId aId)
			{
			return CMsvStore::OpenForReadL(aObserver, aFs, aStoreManager, aId);
			}
		static CMsvStore* MyOpenForWriteL(MMsvStoreObserver& aObserver, RFs& aFs, MMsvStoreManager& aStoreManager, TMsvId aId)
			{
			return CMsvStore::OpenForWriteL(aObserver, aFs, aStoreManager, aId);
			}
	};

//
// Globals needed for the test code
LOCAL_D RTest test(_L("T_OFFOP - Test CImOffLineOperation class"));
LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D TMsvEntry entry; 
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

#define KtheFirstCommand	_L8("FirstCommand")
#define KtheSecondCommand	_L8("SecondCommand")

//
// Client side classes
CDummySessionObserver*  theSessionObserver = NULL;
CMsvSession*			theSession         = NULL;
TMsvId					theServiceId       = KMsvLocalServiceIndexEntryId;
CImOperationQueueList*  theQueue           = NULL;

// Folders created, to test the lists of queued operations stored within folders.
CMsvEntrySelection*		theFolders		   = NULL; 
CMsvEntrySelection*		theSubfolders	   = NULL; 
CDummyUndo				theDummyUndoer;	


//

LOCAL_D const TUid theTechnologyTypeUid   = KUidMsgTypeIMAP4;

// Nr folders to be created.
const TInt KNrFolders         = 2;
const TInt KNrSubFolders      = 2;
const TInt KNrQueuedPerFolder = 5;
const TInt KNrToDelete        = 5;

//
// Data component file name for client
// The data component file is a file belonging to the
// MTM. a class CMtmGroupData is stored in it, specifying the
// UID's of the components belonging to the MTM.
// The CMsvSession class needs it to install the MTM..
// the function CreateDataComponentFileStoreL creates the file for this MTM.
#define DataComponentFileName _L("c:\\system\\mtm\\impc.dat")  
// Service we're connected to.
TFileName theRootDir = _L("C:\\Data\\impc\\");

//
const TMsvId	KDummyMessageId		 =0x01101;
const TMsvId	KDummyTargetFolderId =0x01102;
const TInt		KDummyCommandId      =0x0123;
unsigned char *theFirstCommand  = (unsigned char *)"FirstCommand";
unsigned char *theSecondCommand = (unsigned char *)"SecondCommand";


//
void TestScheduler::Error(TInt anError) const
	{
	test.Printf(TRefByValue<const TDesC>_L("Active object left with error code %d\n"), anError);
	CActiveScheduler::Stop();
	}

//
void CDummySessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	};

//

LOCAL_C TMsvId CreateFolderEntryL(CMsvEntry& aClientEntry)
//
// Create a folder entry below aClientEntry, and see if there was an error.
//
	{
	log->AppendComment(_L8("\tCreating Folder Entry"));
	TBool result = ETrue;
	TMsvEntry folderEntry;

	folderEntry.iDescription.Set(_L("IMAP4 Folder"));
	folderEntry.iDetails.Set(_L("IMAP4"));
	folderEntry.iType = KUidMsvFolderEntry;
	folderEntry.iServiceId = theServiceId;
	folderEntry.iSize = 0;
	folderEntry.iMtm = KUidMsgTypeIMAP4;

	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();
	CMsvOperation*  opert = aClientEntry.CreateL(folderEntry, wait->iStatus);
	CActiveScheduler::Start(); // operation complete
	TBool result1=(opert->iStatus.Int()==KErrNone);
	if (!result1)
		log->AppendError(_L8("\t\tiStatus does not equal 0"),-1);
	result&=result1;
	TBool result2=(opert->Mtm()==KUidMsvLocalServiceMtm);
	if (!result2)
		log->AppendError(_L8("\t\tiMtm does not equal KUidMsvLocalServiceMtm"),-1);
	result&=result2;
	TMsvLocalOperationProgress details;
	TPckgC<TMsvLocalOperationProgress> package(details);	
	package.Set(opert->ProgressL());
	TBool result3=(package().iType==TMsvLocalOperationProgress::ELocalNew);
	if (!result3)
		log->AppendError(_L8("\t\tiType does not equal ELocalNew"),-1);
	result&=result3;
	TBool result4=(package().iTotalNumberOfEntries==1);
	if (!result4)
		log->AppendError(_L8("\t\tiTotalNumberOfEntries does not equal 1"),-1);
	result&=result4;
	TBool result5=(package().iError==KErrNone);
	if (!result5)
		log->AppendError(_L8("\t\tiError not equal to KErrNone"),-1);
	result&=result5;
	TBool result6=(package().iNumberCompleted==1);
	if (!result6)
		log->AppendError(_L8("\t\tiNumberCompleted not equal to 1"),-1);
	result&=result6;
	TBool result7=(package().iNumberFailed==0);
	if (!result7)
		log->AppendError(_L8("\t\tiNumberFailed not equal to 0"),-1);
	result&=result7;
	TBool result8=(package().iNumberRemaining==0);
	if (!result8)
		log->AppendError(_L8("\t\tiNumberRemaining not equal to 0()"),-1);
	result&=result8;
	delete opert; opert=NULL;
	CleanupStack::PopAndDestroy();//wait

	log->AppendComment(_L8("\tCreated Folder Entry"));
	// The Id of the service entry
	return package().iId; 
	}

LOCAL_C TBool DeleteFolderEntryL(CMsvEntry& aClientEntry, TMsvId aIdToDelete)
//
// Delete a folder entry below aClientEntry, and see if there was an error.
//
	{
	log->AppendComment(_L8("\tDeleting Folder Entry"));
	TBool result = ETrue;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();
	CMsvOperation*  opert = aClientEntry.DeleteL(aIdToDelete, wait->iStatus);

	CActiveScheduler::Start(); // operation complete
	TBool result1=(opert->iStatus.Int()==KErrNone);
	if (!result1)
		log->AppendError(_L8("\t\tiStatus does not equal 0"),-1);
	result&=result1;
	TBool result2=(opert->Mtm()==KUidMsvLocalServiceMtm);
	if (!result2)
		log->AppendError(_L8("\t\tiMtm not equal to KUidMsvLocalServiceMtm"),-1);
	result&=result2;
	TMsvLocalOperationProgress details;
	TPckgC<TMsvLocalOperationProgress> package(details);	
	package.Set(opert->ProgressL());
	TBool result3=(package().iType==TMsvLocalOperationProgress::ELocalDelete);
	if (!result3)
		log->AppendError(_L8("\t\tiType not equal to ELocalDelete"),-1);
	result&=result3;
	TBool result4=(package().iTotalNumberOfEntries==package().iNumberCompleted);
	if (!result4)
		log->AppendError(_L8("\t\tiTotalNumberOfEntries not equal to iNumberCompleted"),-1);
	result&=result4;
	TBool result5=(package().iError==KErrNone);
	if (!result5)
		log->AppendError(_L8("\t\tiError does not equal KErrNone"),-1);
	result&=result5;
	TBool result6=(package().iNumberFailed==0);
	if (!result6)
		log->AppendError(_L8("\t\tiNumberFailed does not equal 0"),-1);
	result&=result6;
	TBool result7=(package().iNumberRemaining==0);
	if (!result7)
		log->AppendError(_L8("\t\tiNumberRemaining does not equal 0"),-1);
	result&=result7;
	delete opert; opert=NULL;
	CleanupStack::PopAndDestroy();//wait
	log->AppendComment(_L8("\tDeleted Folder Entry"));
	return result;
	}
			   
LOCAL_C CMsvEntrySelection* CreateFoldersL(TMsvId aRootId, TInt aNrFolders)
	{
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, aRootId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);

	TInt i;
	for ( i = 0 ; i < aNrFolders ; i ++ )
		{
		CreateFolderEntryL(*msventry);
		}

	CMsvEntrySelection* folders = msventry->ChildrenWithServiceL(theServiceId) ;
	CleanupStack::PopAndDestroy();//msventry
	log->AppendComment(_L8("\tCreated Folders"));
	return folders;
	}

LOCAL_C TBool testCreateFoldersL()
	{
	log->AppendComment(_L8("\tTesting Created Folders"));
	TBool result = ETrue;
	theFolders = CreateFoldersL(KMsvGlobalInBoxIndexEntryId, KNrFolders);
	TBool result1=(theFolders->Count() == KNrFolders);
	if (!result1)
		log->AppendError(_L8("\t\tThe number of folders created is incorrect"),-1);
	result&=result1;
	theSubfolders = CreateFoldersL((*theFolders)[0], KNrSubFolders);
	TBool result2=(theSubfolders->Count() == KNrSubFolders);
	if (!result2)
		log->AppendError(_L8("\t\tThe number of subfolders created is incorrect"),-1);
	result&=result;
	return result;
	}

LOCAL_C void StoreQueuedOperationsL(CMsvEntry& aEntry, MImOffLineOperationArray& aArray)
	{
	CMsvStore* store=aEntry.EditStoreL();
	CleanupStack::PushL(store);
    
    CImOffLineArrayStore externaliser(aArray);
    externaliser.StoreL(*store);
    store->CommitL();
    
    CleanupStack::PopAndDestroy();//store
	log->AppendComment(_L8("\tStored Queued Operations"));
	}

LOCAL_C CImOffLineOperationArray* BuildOfflineOperationArrayLC()
    {
	TInt i;
    CImOffLineOperationArray* array = CImOffLineOperationArray::NewL();
    CleanupStack::PushL(array);

    CImOffLineOperation operation;
    
    for (i=0;i<KNrQueuedPerFolder-2;i++)
        {
        array->AppendOperationL(operation);
        }

	TBuf8<100> scrap;
    scrap = _L8(theFirstCommand);
    operation.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    array->AppendOperationL(operation);

    scrap = _L8(theSecondCommand);
    operation.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
	array->AppendOperationL(operation);

    log->AppendComment(_L8("\tBuilt OffLine Operation Array"));
    return array;
    }

LOCAL_C TBool testOffLineArraysFirstFiveEqual(MImOffLineOperationArray& aArray1, MImOffLineOperationArray& aArray2)
    {
	log->AppendComment(_L8("\tTesting Offline Arrays are Equal"));
	TBool result = ETrue;
    TInt i;
    for (i=0;i<KNrQueuedPerFolder;i++)
        result &= (aArray1.Operation(i).Equals(aArray2.Operation(i)));
	if (!result)
		log->AppendError(_L8("\t\tOperations are not equal on the arrays"),-1);
	return result;
    }

LOCAL_C TBool testPopulateFoldersWithQueuedOperationsL()
	{
	log->AppendComment(_L8("\tPopulating Folders With Queued Operations"));
	TBool result = ETrue;
    TInt i;
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);

    CImOffLineOperationArray* array = BuildOfflineOperationArrayLC();

    // See to it that the operations came through ok...
    CImOffLineOperation operation;

    TBuf8<100> scrap;
    scrap = _L8(theFirstCommand);
    operation.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    TBool result1=(operation.Equals(array->Operation(KNrQueuedPerFolder-2)));
	if (!result1)
		log->AppendError(_L8("\t\tOperations are not equal"),-1);
	result&=result1;

    scrap = _L8(theSecondCommand);
    operation.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    TBool result2=(operation.Equals(array->Operation(KNrQueuedPerFolder-1)));
	if (!result2)
			log->AppendError(_L8("\t\tOperations are not equal"),-1);
	result&=result2;

    for (i=0;i<theFolders->Count();i++)
		{
		msventry->SetEntryL((*theFolders)[i]);
		StoreQueuedOperationsL(*msventry, *array);
		}

	for (i=0;i<theSubfolders->Count();i++)
		{
		msventry->SetEntryL((*theSubfolders)[i]);
		StoreQueuedOperationsL(*msventry, *array);
		}

    CleanupStack::PopAndDestroy();//array 
    CleanupStack::PopAndDestroy();//msventry
	return result;
	}

LOCAL_C TBool testReadBackOperationsL()
	{
	log->AppendComment(_L8("\tReading Back Operations"));
	TBool result = ETrue;
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);

    CImOffLineOperationArray* array = CImOffLineOperationArray::NewL();
    CleanupStack::PushL(array);

	msventry->SetEntryL((*theFolders)[0]);
	CMsvStore* store=msventry->ReadStoreL();
	CleanupStack::PushL(store);
    CImOffLineArrayStore internaliser(*array);
    internaliser.RestoreL(*store);
    CleanupStack::PopAndDestroy();//store

    CImOffLineOperationArray* built = BuildOfflineOperationArrayLC();
    result &= testOffLineArraysFirstFiveEqual(*array, *built);
    CleanupStack::PopAndDestroy();//built

    CleanupStack::PopAndDestroy();//array 
    CleanupStack::PopAndDestroy();//msventry
	return result;
	}


LOCAL_C TBool testReadBackQueuedOperationsL()
	{
	log->AppendComment(_L8("\tReading Back Queued Operations"));
	TBool result = ETrue;
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);

    CImQueuedList* array = CImQueuedList::NewL();
    CleanupStack::PushL(array);

    msventry->SetEntryL((*theFolders)[0]);
	CMsvStore* store=msventry->ReadStoreL();

    CleanupStack::PushL(store);
    CImOffLineArrayStore internaliser(*array);
    internaliser.RestoreL(*store);
    CleanupStack::PopAndDestroy();//store

    CImOffLineOperationArray* built = BuildOfflineOperationArrayLC();
    result &= testOffLineArraysFirstFiveEqual(*array, *built);
    CleanupStack::PopAndDestroy();//built

    CleanupStack::PopAndDestroy();//array 
    CleanupStack::PopAndDestroy();//msventry
	return result;
	}

LOCAL_C TBool testDeleteFoldersL()
	{
	log->AppendComment(_L8("\tDeleting Folders"));
	TBool result = ETrue;
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);
	TInt i;

	for ( i = 0 ; i < theFolders->Count() ; i ++ )
		{
		result &= DeleteFolderEntryL(*msventry, (*theFolders)[i]);
		}
	if (!result)
			log->AppendError(_L8("\t\tError in Deleting Folder Entry"),-1);
	CleanupStack::PopAndDestroy();//msventry

	delete theFolders;
	theFolders = NULL;
	delete theSubfolders;
	theSubfolders = NULL;
	return result;
	}

LOCAL_C TBool testClearInboxL()
	{
	TBool result = ETrue;
	log->AppendComment(_L8("\tClearing Inbox"));
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);
	theFolders = msventry->ChildrenWithServiceL(theServiceId) ;
	CleanupStack::PopAndDestroy();//msventry
	result &= testDeleteFoldersL();
	return result;
	}

LOCAL_C TBool CreateDataComponentFileStoreL()
//
// Test read file handling.
//
	{
// Open file

	CMtmDllInfoArray* mtmdllinfoarray=new(ELeave) CMtmDllInfoArray();
	CleanupStack::PushL(mtmdllinfoarray);

	CMtmDllInfo* mtmdllinfo1=CMtmDllInfo::NewL(_L("IMAP4 SERVER component"),TUidType(KDynamicLibraryUid,KUidMtmServerComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KImap4ServerMtmFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo1);

	CMtmDllInfo* mtmdllinfo2=CMtmDllInfo::NewL(_L("IMAP4 CLIENT component"),TUidType(KDynamicLibraryUid,KUidMtmClientComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KImap4ClientMtmFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo2);

	CMtmDllInfo* mtmdllinfo3=CMtmDllInfo::NewL(_L("IMAP4 UI component"),TUidType(KDynamicLibraryUid,KUidMtmUiComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KImap4DataLayerFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo3);

	CMtmDllInfo* mtmdllinfo4=CMtmDllInfo::NewL(_L("IMAP4 UI DATA component"),TUidType(KDynamicLibraryUid,KUidMtmUiDataComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KImap4DataLayerFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo4);

	TCapabilitySet capSet;
	capSet.SetEmpty();
	CleanupStack::Pop(mtmdllinfoarray); // next line takes ownership
	CMtmGroupData* mtmgroupdata=CMtmGroupData::NewL(KUidMsgTypeIMAP4, theTechnologyTypeUid, mtmdllinfoarray, capSet);
	CleanupStack::PushL(mtmgroupdata);

    testUtils->FileSession().Delete(DataComponentFileName);
	CFileStore* filestore = CPermanentFileStore::ReplaceLC(testUtils->FileSession(),DataComponentFileName,EFileShareExclusive|EFileStream|EFileWrite);
	TUidType uidtype(KPermanentFileStoreLayoutUid,KUidMsvDataComponent,KUidMsgTypeIMAP4);
	filestore->SetTypeL(uidtype);
	RStoreWriteStream out;
	TStreamId streamid=out.CreateLC(*filestore);							// Push to stack
	mtmgroupdata->ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy(); // out
	filestore->SetRootL(streamid);
	filestore->CommitL();
	CleanupStack::PopAndDestroy(); // filestore
	
	CleanupStack::PopAndDestroy();  // mtmgroupdata
	log->AppendComment(_L8("\tCreated Data Component File Store"));
	return ETrue;
	}


LOCAL_C TBool CreateClientRegistryL()
//
// Get access to the Client MTM object (CBaseMtm-derived).
//
	{
	TBool result = ETrue;
// Session observer. Needed to create a session
	theSessionObserver = new (ELeave) CDummySessionObserver;
	result &= (theSessionObserver != NULL);
	CleanupStack::PushL(theSessionObserver);
// Session. Needed to create a client registry.
	theSession=CMsvSession::OpenSyncL(*theSessionObserver);
	result &= (theSession != NULL);
	CleanupStack::PushL(theSession);

	CleanupStack::Pop(2);// theSessionObserver, theSession
	log->AppendComment(_L8("\tCreated Client Registry"));
	return result;
	}

LOCAL_C void DeleteClientRegistry()
	{
// Clean up
	delete theSession;			
	theSession         = NULL;
	delete theSessionObserver;	
	theSessionObserver = NULL;
	log->AppendComment(_L8("\tDeleted Client Registry"));
	}


LOCAL_C TBool testGetQueListL()
	{
	log->AppendComment(_L8("\tGetting Queue List"));
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	// Queue takes over	ownership of msventry
	theQueue = CImOperationQueueList::NewL(*msventry, &theDummyUndoer);
	CleanupStack::PushL(theQueue);
	
	CleanupStack::Pop();//theQueue
	log->AppendComment(_L8("\tGot Queue List"));
	return ETrue;
	}


//
// TQueuedOperationSwap and TQueuedOperationKey : Classes needed for
// a QuickSort in ExpungeDeletedOperationsL. They sort folders from lower
// to higher ids, and within each folder from high line numbers to low
// line numbers. That way the highest numbers get deleted first, maintaining
// the validity of the other removals.
//
class TQueuedOperationSwap  : public TSwap
	{
public:
	IMPORT_C TQueuedOperationSwap(CImQueuedList& aList);
	void Swap(TInt aLeft,TInt aRight) const;
private:	
	CImQueuedList& iList;
	};

class TQueuedOperationKey : public TKey
	{
public:
	IMPORT_C TQueuedOperationKey(CImQueuedList& aList);
	TInt Compare(TInt aLeft,TInt aRight) const;
	TAny *At(TInt anIndex) const;
private:	
	CImQueuedList& iList;
	};


EXPORT_C TQueuedOperationKey::TQueuedOperationKey(CImQueuedList& aList) 
    : iList(aList) 
    {
    }

TInt TQueuedOperationKey::Compare(TInt aLeft,TInt aRight) const
	{
	if (aLeft == aRight)
		return 0;
	if (iList[ aLeft ].FolderId() != iList[ aRight ].FolderId())
		return iList[ aLeft ].FolderId() - iList[ aRight ].FolderId();
	// Sort in descending order, so last lines will be deleted first
	TInt diff = iList[ aRight ].OperationIndex() - iList[ aLeft ].OperationIndex();
	if (diff == 0)
        log->AppendError(_L8("\t\tTwo identical lines not allowed in queued operations list"), -1);
	return diff;
	}
TAny *TQueuedOperationKey::At(TInt anIndex) const
	{
	return &iList[ anIndex ];
	}


LOCAL_C TBool testQueuedSortedCorrectlyL()
    {
	log->AppendComment(_L8("\tTesting Queue Sorted Correctly"));
	TBool result = ETrue;
    CImQueuedList *list=CImQueuedList::NewL();
    CleanupStack::PushL(list);

    TQueuedOperation op;
    TInt i;
    for (i=0;i<theQueue->Count();i++)
        {
        op.CopyL((*theQueue)[i]);
        list->AppendL(op);
        }

    // Sort the queued list.
    TQueuedOperationKey key((*list));
	TQueuedOperationSwap swap((*list));
	User::QuickSort((*list).CountOperations(),key,swap);

    TInt nr = 1;

    TInt line=KNrQueuedPerFolder+1;
    
    // Now check if folders grouped together (not strictly necessary, but would slow down otherwise).
    for (i=1;i<list->CountOperations();i++)
        {
        if ((*list)[i].FolderId() != (*list)[i-1].FolderId())
            {
            nr++;
            line=KNrQueuedPerFolder+1;
            }
        else
            {
            result &= (line>(*list)[i-1].OperationIndex()) ;
            line = (*list)[i-1].OperationIndex();
            result &= (line>(*list)[i].OperationIndex()) ;
            }
        }
    result &= (nr == (KNrFolders + KNrSubFolders));

    CleanupStack::PopAndDestroy(); //list
	if (result)
		log->AppendComment(_L8("\tQueue Sorted Correctly"));
	else
		log->AppendError(_L8("\t\tGetting Queue List"), -1);
	return result;
    }

LOCAL_C TBool StoreFirstL(MMsvStoreManager& aStoreManager, TMsvId aId)
    {
    CImOffLineOperationArray *array = CImOffLineOperationArray::NewL();
    CleanupStack::PushL(array);

    CImOffLineOperation op;

    // Store a operation
	CDummyStoreObserver dummy;
	CMsvStore* store = CDerivedMsvStore::MyOpenForWriteL(dummy, testUtils->FileSession(), aStoreManager, aId);
	CleanupStack::PushL(store);

    TBuf8<100> scrap;
    scrap = _L8(theFirstCommand);
    op.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    array->AppendOperationL(op);

    // Test insertion: insert this operation before the one already added.
    op.SetCopyWithinService(KDummyMessageId, KDummyTargetFolderId);
    array->InsertOperationL(op,0);

    CImOffLineArrayStore externaliser(*array);
    externaliser.StoreL(*store);

    store->CommitL();

    CleanupStack::PopAndDestroy(); //store
    CleanupStack::PopAndDestroy(); //array

	log->AppendComment(_L8("\tStored First"));
	return ETrue;
    }

LOCAL_C TBool RestoreAndTestL(MMsvStoreManager& aStoreManager, TMsvId aId)
    {
	TBool result = ETrue;
    CImOffLineOperationArray *array = CImOffLineOperationArray::NewL();
    CleanupStack::PushL(array);

    CImOffLineOperation op;

    // Store a operation
	CDummyStoreObserver dummy;
	CMsvStore* store = CDerivedMsvStore::MyOpenForReadL(dummy, testUtils->FileSession(), aStoreManager, aId);
	CleanupStack::PushL(store);

    CImOffLineArrayStore internaliser(*array);
    internaliser.RestoreL(*store);
    result &= (array->CountOperations() == 2);

    op.SetCopyWithinService(KDummyMessageId, KDummyTargetFolderId);
    result &= (op.Equals(array->Operation(0)));

    TBuf8<100> scrap;
    scrap = _L8(theFirstCommand);
    op.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);

    CImOffLineOperation op2;
    op2.CopyL(array->Operation(1));
    result &= (op.Equals(op2));

	CleanupStack::PopAndDestroy(); //store
	CleanupStack::PopAndDestroy(); //array

	log->AppendComment(_L8("\tRestored and Tested"));
	return result;
    }

LOCAL_C TBool testStoreRestoreWorkscorrectlyL()
    {
    TBool result = ETrue;
   	log->AppendComment(_L8("\tTesting if Store/Restore Works Correctly"));

    CFakeStoreManager* storeManager = CFakeStoreManager::NewL(testUtils->FileSession());
    CleanupStack::PushL(storeManager);
    TMsvId entryId = 100;
    storeManager->DeleteFileStoreL(entryId);
    result &= StoreFirstL(*storeManager, entryId);
    result &= RestoreAndTestL(*storeManager, entryId);

	return result;
    }

LOCAL_C TBool testDeleteQueuedL(TInt aOffset, TInt aStep, TInt aNr)
	{
	log->AppendComment(_L8("\tTesting Deleting from the Queue"));
	TBool result = ETrue;
	result &= (theQueue != NULL);
	result &= (aNr <= theQueue->Count()); 
	if (!result)
		log->AppendError(_L8("\t\tCannot delete more items than on the queue"), -1);
	TInt i;
	for (i=0;i<aNr;i++)
		{
		theQueue->DeleteL(aOffset);
		aOffset += aStep;
		aOffset %= theQueue->Count();
		}
	log->AppendComment(_L8("\tTested Deleting from the Queue"));
	return result;
	}

LOCAL_C TBool testExpungeQueListL()
	{
	TBool result = ETrue;
	result &= (theQueue != NULL);
	theQueue->ExpungeDeletedOperationsL();
	log->AppendComment(_L8("\tTested Expunge Queue List"));
	return result;
	}
 
LOCAL_C TBool testFreeQueListL()
	{
	TBool result = ETrue;
	result &= (theQueue != NULL);
	delete theQueue;
	theQueue = NULL;
	log->AppendComment(_L8("\tFreed Queue List"));
	return result;
	}

LOCAL_C TBool TestOperationL(CImOffLineOperation& aOp,CImOffLineOperation::TOffLineOpType aOpType,TMsvId  aMessageId,TMsvId  aTargetMessageId,TInt aMtmFunctionId, const TDesC8* aMtmParameters)
    {
	TBool result = ETrue;
    TBool result1=(aOp.OpType()    == aOpType);
	if (!result1)
		log->AppendError(_L8("\t\tOpType not identical"),-1);
	result&=result1;

    TBool result2=(aOp.MessageId() == aMessageId);
	if (!result2)
		log->AppendError(_L8("\t\tMessageId not identical"),-1);
    result&=result2;

    TBool result3=(aOp.TargetMessageId() == aTargetMessageId);
	if (!result3)
		log->AppendError(_L8("\t\tTargetMessageId not identical"),-1);
	result&=result3;
    
	TBool result4=(aOp.MtmFunctionId() == aMtmFunctionId);
	if (!result4)
		log->AppendError(_L8("\t\tMtmFunctionId not identical"),-1);
	result&=result4;
    
	if (aMtmParameters)
		{
		TBool result6=(aOp.MtmParameters() == *aMtmParameters);
		if (!result6)
			log->AppendError(_L8("\t\tMtmParameters not identical"),-1);
		result&=result6;
		}
	return result;
    }
 
LOCAL_C TBool testOffLineOperationMethodsL()
    {
	log->AppendComment(_L8("\tTesting OffLine Operation Methods"));
	TBool result = ETrue;;
    // Create a no operation
    CImOffLineOperation op;
    CImOffLineOperation op2;
    CImOffLineOperation op3;

//
// Testing Offline operation creation    
    log->AppendComment(_L8("\tTesting Offline operation creation"));
    TBool result1=TestOperationL(op,CImOffLineOperation::EOffLineOpNone,KMsvNullIndexEntryId, KMsvNullIndexEntryId,0,NULL);
    // Create a copy operation
    op.SetCopyToLocal(KDummyMessageId, KDummyTargetFolderId);
	log->AppendComment(_L8("\tCreate a copy operation"));
    result1&=TestOperationL(op,CImOffLineOperation::EOffLineOpCopyToLocal,KDummyMessageId, KDummyTargetFolderId,0,NULL);
    // Create a move operation
    op.SetMoveFromLocal(KDummyMessageId, KDummyTargetFolderId);
	log->AppendComment(_L8("\tCreate a move operation"));
    result1&=TestOperationL(op,CImOffLineOperation::EOffLineOpMoveFromLocal,KDummyMessageId, KDummyTargetFolderId,0,NULL);
    // Create a delete operation
    op.SetDelete(KDummyMessageId);
	log->AppendComment(_L8("\tCreate a delete operation"));
    result1&=TestOperationL(op,CImOffLineOperation::EOffLineOpDelete,KDummyMessageId, KMsvNullIndexEntryId,0,NULL);
    // Create a delete operation
    op.SetDelete(KDummyMessageId);
	log->AppendComment(_L8("\tCreate a delete operation"));
    result1&=TestOperationL(op,CImOffLineOperation::EOffLineOpDelete,KDummyMessageId, KMsvNullIndexEntryId,0,NULL);
    // Create a change operation
    op.SetChange(KDummyMessageId);
	log->AppendComment(_L8("\tCreate a change operation"));
    result1&=TestOperationL(op,CImOffLineOperation::EOffLineOpChange,KDummyMessageId, KMsvNullIndexEntryId,0,NULL);
    // Create a create operation
    op.SetCreate(KDummyMessageId);
	log->AppendComment(_L8("\tCreate a create operation"));
    result1&=TestOperationL(op,CImOffLineOperation::EOffLineOpCreate,KDummyMessageId, KMsvNullIndexEntryId,0,NULL);
    if (!result1)
		log->AppendError(_L8("\t\tError Creating Operations"),-1);
//
// Testing Equals, CopyL 

	log->AppendComment(_L8("\tTesting Equals/CopyL"));
    // Create a mtm-specific operation
    // Op takes over ownership of command.
    TBuf8<100> scrap;
    scrap = _L8(theFirstCommand);
    op.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    TBool result2=TestOperationL(op,CImOffLineOperation::EOffLineOpMtmSpecific,KDummyMessageId, KDummyTargetFolderId,KDummyCommandId,&scrap);

    scrap = _L8(theFirstCommand);
    op2.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    result2&=TestOperationL(op2,CImOffLineOperation::EOffLineOpMtmSpecific,KDummyMessageId, KDummyTargetFolderId,KDummyCommandId,&scrap);

    result2&=(op.Equals(op2));
	if (!result2)
		log->AppendError(_L8("\t\tError with Equals()"),-1);
	result&=result2;

    scrap = _L8(theSecondCommand);
    op2.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    TBool result3=TestOperationL(op2,CImOffLineOperation::EOffLineOpMtmSpecific,KDummyMessageId, KDummyTargetFolderId,KDummyCommandId,&scrap);

    result3&=(!op.Equals(op2));
	if (!result3)
		log->AppendError(_L8("\t\tError with Equals()"),-1);
	result&=result3;

    op3.CopyL(op2);

    TBool result4=( op3.Equals(op2));
	if (!result4)
		log->AppendError(_L8("\t\tError with Equals()"),-1);
	result&=result4;
    TBool result5=(!op3.Equals(op ));
	if (!result5)
		log->AppendError(_L8("\t\tError with Equals()"),-1);
	result&=result5;

	return result;
    }

LOCAL_C TBool testListedOffLineOperationMethodsL()
    {
	log->AppendComment(_L8("\tTesting Listed Offline Operation Methods"));
	TBool result = ETrue;
    // Create a no operation
    CImOffLineOperation op1;
    CImOffLineOperation op2;
    CImOffLineOperation op3;

//
// Testing Offline operation creation    
    
    // Create a copy operation
    op1.SetCopyWithinService(KDummyMessageId, KDummyTargetFolderId);
    TBuf8<100> scrap;
    scrap = _L8(theFirstCommand);
    op2.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);
    scrap = _L8(theSecondCommand);
    op3.SetMtmSpecificCommandL(KDummyMessageId, KDummyTargetFolderId, KDummyCommandId, scrap);

    // Creation
    TQueuedOperation *qop1=TQueuedOperation::NewL(KDummyTargetFolderId,0, op1);
    CleanupStack::PushL(qop1);
    TQueuedOperation *qop2=TQueuedOperation::NewL(KDummyTargetFolderId,0, op1);
    CleanupStack::PushL(qop2);
    TQueuedOperation *qop3=TQueuedOperation::NewL(KDummyTargetFolderId,1, op2);
    CleanupStack::PushL(qop3);
    TQueuedOperation *qop4=TQueuedOperation::NewL(KDummyTargetFolderId+1,0, op2);
    CleanupStack::PushL(qop4);
    TQueuedOperation *qop5=TQueuedOperation::NewL(KDummyTargetFolderId+1,0, op3);
    CleanupStack::PushL(qop5);

    // See if data arrived ok...
	TBool result1 = ETrue;
	result1 &= (qop3->FolderId()       == KDummyTargetFolderId);
	result1 &= (qop3->OperationIndex() == 1);
	result1 &= (qop3->Operation().Equals(op2));
	if (!result1)
		log->AppendError(_L8("\t\tData did not arrive OK"), -1);
	result&=result1;

    // CopyL 
	TBool result2 = ETrue;
    result2 &= (  *qop1 != *qop3 );
    result2 &= (  *qop1 != *qop4 );
    result2 &= (!(*qop1 != *qop2));
    result2 &= (  *qop4 != *qop5 );

    qop2->CopyL(*qop4);
    result2 &= (  *qop1 != *qop2 );
    result2 &= (!(*qop2 != *qop4));
	if (!result2)
		log->AppendError(_L8("\t\tError Copying"), -1);
	result&=result2;
    CleanupStack::Pop(5);//qop1 to qop5, not CBase derived
    delete qop1;
    delete qop2;
    delete qop3;
    delete qop4;
    delete qop5;
	return result;
    }

//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CleanMessageFolderL();
	testUtils->FileSession().Delete(_L("C:\\system\\mtm\\MTM Registry"));
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2); //testUtils, scheduler
	}

//

LOCAL_C void doMainL()
	{
	TBool result = ETrue;
	InitL();
	testUtils->GoClientSideL();
	testUtils->FileSession().SetSessionPath(_L("c:\\"));
	testUtils->FileSession().MkDir(_L("c:\\logs\\"));
	testUtils->FileSession().MkDir(_L("c:\\logs\\email\\"));

	log=CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"),EAppend);
	CleanupStack::PushL(log);

	log->AppendComment(_L8("********** T_MIUT07 Test CImOffLineOperation class **********"));
	log->AppendComment(_L8("\tInit() Completed"));

	testUtils->TestStart(1);
	test.Printf(_L("Create Data Component FileStore\n"));	// Test 1
	result &= CreateDataComponentFileStoreL();
	if (result)
		log->AppendComment(_L8("PASSED Test 1"));
	else
		log->AppendError(_L8("FAILED Test 1"),-1);
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	// Get the client mtm, by using the just created file
	test.Printf(_L("Create Client Registry\n"));			// Test 2
	result &= CreateClientRegistryL();
	if (result)
		log->AppendComment(_L8("PASSED Test 2"));
	else
		log->AppendError(_L8("FAILED Test 2"),-1);
	testUtils->TestFinish(2);
	
	User::After(5000000); //delay to allow MailInit to complete
	
	testUtils->TestStart(3);
	test.Printf(_L("Clear inbox\n"));						// Test 3
	result &= testClearInboxL();
	if (result)
		log->AppendComment(_L8("PASSED Test 3"));
	else
		log->AppendError(_L8("FAILED Test 3"),-1);
	testUtils->TestFinish(3);

	testUtils->TestStart(4);
	test.Printf(_L("Creating folders\n"));					// Test 4
	result &= testCreateFoldersL();
	if (result)
		log->AppendComment(_L8("PASSED Test 4"));
	else
		log->AppendError(_L8("FAILED Test 4"),-1);
	testUtils->TestFinish(4);

	testUtils->TestStart(5);
	test.Printf(_L("Testing offline operation access\n"));	// Test 5
	result &= testOffLineOperationMethodsL();
	if (result)
		log->AppendComment(_L8("PASSED Test 5"));
	else
		log->AppendError(_L8("FAILED Test 5"),-1);
	testUtils->TestFinish(5);

	testUtils->TestStart(6);
	test.Printf(_L("Testing listed offline operation access\n"));	// Test 6
	result &= testListedOffLineOperationMethodsL();
	if (result)
		log->AppendComment(_L8("PASSED Test 6"));
	else
		log->AppendError(_L8("FAILED Test 6"),-1);
	testUtils->TestFinish(6);

	testUtils->TestStart(7);
    test.Printf(_L("Testing externalisation/internalisation of offline operations\n")); //Test 7
	result &= testStoreRestoreWorkscorrectlyL();
	if (result)
		log->AppendComment(_L8("PASSED Test 7"));
	else
		log->AppendError(_L8("FAILED Test 7"),-1);
	testUtils->TestFinish(7);

	testUtils->TestStart(8);
    test.Printf(_L("Obtaining folders with no stores associated\n"));	// Test 8
	result &= testGetQueListL();
	result &= testFreeQueListL();
	if (result)
		log->AppendComment(_L8("PASSED Test 8"));
	else
		log->AppendError(_L8("FAILED Test 8"),-1);
	testUtils->TestFinish(8);

	testUtils->TestStart(9);
    test.Printf(_L("Testing queued operations access\n"));	// Test 9
    result &= testPopulateFoldersWithQueuedOperationsL();
	if (result)
		log->AppendComment(_L8("PASSED Test 9"));
	else
		log->AppendError(_L8("FAILED Test 9"),-1);
	testUtils->TestFinish(9);

	testUtils->TestStart(10);
    result &= testReadBackOperationsL();					// Test 10
	if (result)
		log->AppendComment(_L8("PASSED Test 10"));
	else
		log->AppendError(_L8("FAILED Test 10"),-1);
	testUtils->TestFinish(10);

	testUtils->TestStart(11);
    result &= testReadBackQueuedOperationsL();				// Test 11
	if (result)
		log->AppendComment(_L8("PASSED Test 11"));
	else
		log->AppendError(_L8("FAILED Test 11"),-1);
	testUtils->TestFinish(11);

	testUtils->TestStart(12);
    result &= testGetQueListL();							// Test 12
	result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	if (result)
		log->AppendComment(_L8("PASSED Test 12"));
	else
		log->AppendError(_L8("FAILED Test 12"),-1);
	testUtils->TestFinish(12);

	testUtils->TestStart(13);
	result &= testFreeQueListL();							// Test 13
	if (result)
		log->AppendComment(_L8("PASSED Test 13"));
	else
		log->AppendError(_L8("FAILED Test 13"),-1);
	testUtils->TestFinish(13);

	testUtils->TestStart(14);
	test.Printf(_L("Testing if queued list sorted correctly\n"));	// Test 14
	result &= testGetQueListL();
	result &= testQueuedSortedCorrectlyL();
    result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	result &= testFreeQueListL();
	if (result)
		log->AppendComment(_L8("PASSED Test 14"));
	else
		log->AppendError(_L8("FAILED Test 14"),-1);
	testUtils->TestFinish(14);

	testUtils->TestStart(15);
	result &= testGetQueListL();									// Test 15
    result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	result &= testExpungeQueListL();
	result &= testFreeQueListL();
	if (result)
		log->AppendComment(_L8("PASSED Test 15"));
	else
		log->AppendError(_L8("FAILED Test 15"),-1);
	testUtils->TestFinish(15);

	testUtils->TestStart(16);
	test.Printf(_L("Testing rewrite of queued operations\n"));		// Test 16
	result &= testGetQueListL();
	result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	testDeleteQueuedL(0,4,KNrToDelete);
	result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder-KNrToDelete);
	result &= testFreeQueListL();
	if (result)
		log->AppendComment(_L8("PASSED Test 16"));
	else
		log->AppendError(_L8("FAILED Test 16"),-1);
	testUtils->TestFinish(16);

	testUtils->TestStart(17);
	test.Printf(_L("Testing removal queued operations\n"));			// Test 17
	result &= testGetQueListL();
	result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	testDeleteQueuedL(0,4,KNrToDelete);
	result &= testExpungeQueListL();
	result &= testFreeQueListL();
	if (result)
		log->AppendComment(_L8("PASSED Test 17"));
	else
		log->AppendError(_L8("FAILED Test 17"),-1);
	testUtils->TestFinish(17);

	testUtils->TestStart(18);
	result &= testGetQueListL();									// Test 18
	result &= (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder-KNrToDelete);
	result &= testFreeQueListL();
	if (result)
		log->AppendComment(_L8("PASSED Test 18"));
	else
		log->AppendError(_L8("FAILED Test 18"),-1);
	testUtils->TestFinish(18);

	testUtils->TestStart(19);
	test.Printf(_L("Deleting folders\n"));							// Test 19
	result &= testDeleteFoldersL();
	if (result)
		log->AppendComment(_L8("PASSED Test 19"));
	else
		log->AppendError(_L8("FAILED Test 19"),-1);
	testUtils->TestFinish(19);

//
// Finished. Clean up all objects
	test.Printf(_L("Deleting used objects\n"));
	DeleteClientRegistry();
	
	if (result)
		{
		log->AppendComment(_L8("PASSED All tests Correctly"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("FAILED Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}
	
	log->AppendComment(_L8("*************    T_MIUT07 Tests Complete    *************"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(); //log

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_MIUT07 Test CImOffLineOperation class\n"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
