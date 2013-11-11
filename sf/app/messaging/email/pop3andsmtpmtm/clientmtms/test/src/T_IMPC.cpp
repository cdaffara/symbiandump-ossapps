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
// Name of test harness: T_IMPC
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests IMAP4 Query Capabilities; Creates/stores/restores IMAP4 service; 
// checks for connection; creates folders; gives folders queued operations 
// (populates folders with queued operations); tests queued operations access;
// tests rewrite of queued operations; tests removal queued of operations; 
// tests deleting folders.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMPC\T_IMCM.LOG
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMPC.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMPC.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMPC.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMPC.exe on the other platform
// define to mark stuff still to do. Should be switched on for the last compile.
// 
//

#include "emailtestutils.h"
#include <mtmuids.h>
#include <miutlog.h>
#include <impcmtm.h>
#include <offop.h>
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
//
// Globals needed for the test code
LOCAL_D RTest test(_L("T_IMPC test harness"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

// a define to allow disabling user interaction in general, for this test exe.
#define RECEIVE_KEYPRESS test.Getch();

//
// Client side classes
CClientMtmRegistry*     theClientRegistry  = NULL;
CDummySessionObserver*  theSessionObserver = NULL;
CMsvSession*			theSession         = NULL;
CBaseMtm*		        theClientMtm       = NULL;
TMsvId					theServiceId       = KMsvNullIndexEntryId;

// Folders created, to test the lists of queued operations stored within folders.
CMsvEntrySelection*		theFolders		   = NULL; 
CMsvEntrySelection*		theSubfolders	   = NULL; 

_LIT(KServerMtmFileName, "z:\\system\\libs\\imps.dll");
_LIT(KClientMtmFileName, "z:\\system\\libs\\imcm.dll");
// The UI MTMs are not used here
_LIT(KUiMtmFileName, "z:\\system\\libs\\dummyuimtm.dll");
_LIT(KUiDataMtmFileName, "z:\\system\\libs\\dummyuidatamtm.dll");

const TUid KUidEmailTechnologyGroup=	{0x10001671};
LOCAL_D const TUid theTechnologyTypeUid   = KUidEmailTechnologyGroup; 

// Nr folders to be created.
const TInt KNrFolders = 2;
const TInt KNrSubFolders = 2;
const TInt KNrQueuedPerFolder = 5;

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

_LIT(KTestSettingsServerAddress,"2.2.2.2");

const TInt KTestSettingsPort = 111;
_LIT8(KTestSettingsFolderPath,"\\a\\folder\\path\\");

//
void TestScheduler::Error(TInt anError) const
	{
	test.Printf(_L("Active object left with error code %d\n"), anError);
	CActiveScheduler::Stop();
	}

//
void CDummySessionObserver::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
	{
	switch (aEvent)
		{
		case EMsvEntriesCreated:
			{
			TMsvId newid    = *(TMsvId*)aArg1;
			TMsvId parentid = *(TMsvId*)aArg2;
			test.Printf(_L("SESSION EVENT : EMsvEntriesCreated id=%d, parentid=%d\n"),newid,parentid);
			}
			break;
		case EMsvEntriesChanged:
			{
			TMsvId id    = *(TMsvId*)aArg1;
			test.Printf(_L("SESSION EVENT : EMsvEntriesChanged id=%d\n"),id);
			}
			break;
		case EMsvEntriesDeleted:
			{
			CMsvEntrySelection *selection=(CMsvEntrySelection *)aArg1;
			test.Printf(_L("SESSION EVENT : EMsvEntriesDeleted (%d items)\n"),selection->Count());
			}
			break;
		case EMsvEntriesMoved:
			{
			CMsvEntrySelection *selection=(CMsvEntrySelection *)aArg1;
			test.Printf(_L("SESSION EVENT : EMsvEntriesMoved (%d items)\n"),selection->Count());
			}
			break;
		case EMsvMtmGroupInstalled:
			{
			TUid id=*(TUid *)aArg2;
			test.Printf(_L("SESSION EVENT : EMsvMtmGroupInstalled (uid=%d)\n"),id);
			}
			break;
		case EMsvMtmGroupDeInstalled:
			{
			TUid id=*(TUid *)aArg2;
			test.Printf(_L("SESSION EVENT : EMsvMtmGroupDeInstalled (uid=%d)\n"),id);
			}
			break;
		default:
			break;
		}
	};

//

LOCAL_C TMsvId FindTxtServiceEntry(CMsvEntry& aRootEntry)	
//
// Try to find a service entry for our TXT message types
// return the id if it already exists. Otherwise returns KMsvNullIndexEntryId.
//
	{
	TMsvId result = KMsvNullIndexEntryId;
	// Get the direct children
	aRootEntry.SetEntryL(KMsvRootIndexEntryId);
	CMsvEntrySelection *selection = aRootEntry.ChildrenWithMtmL(KUidMsgTypeIMAP4); 
	test (selection != NULL);
	CleanupStack::PushL(selection);
	// Now return the first found entry
	TInt nrChildren = selection->Count();
	test.Printf (_L("%d txt service entries\n"), nrChildren);
	if (nrChildren >0)
		result = (*selection)[0];
	CleanupStack::PopAndDestroy(1);	//selection
	return result;
	}


LOCAL_C TMsvId CreateServiceEntryL(CMsvEntry& aClientEntry, TInt aError)
//
// Create a service entry below aClientEntry, and see if there was an error.
//
	{
	TMsvEntry serviceEntry;
	serviceEntry.iDescription.Set(_L("IMAP4 Service Entry"));
	serviceEntry.iDetails.Set(_L("IMAP4"));
	serviceEntry.iType = KUidMsvServiceEntry;
	serviceEntry.iSize = 0;
	serviceEntry.iMtm = KUidMsgTypeIMAP4;
	aClientEntry.SetEntryL(KMsvRootIndexEntryId);
	
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();
	CMsvOperation*  opert = aClientEntry.CreateL(serviceEntry, wait->iStatus);
	CActiveScheduler::Start(); // operation complete
	test(opert->iStatus.Int()==KErrNone);
	test(opert->Mtm()==KUidMsvLocalServiceMtm);
	TMsvLocalOperationProgress details;
	TPckgC<TMsvLocalOperationProgress> package(details);	
	package.Set(opert->ProgressL());
	test(package().iType==TMsvLocalOperationProgress::ELocalNew);
	test(package().iTotalNumberOfEntries==1);
	test(package().iError==aError);
	if (aError)
		{
		test(package().iNumberCompleted==0);
		test(package().iNumberFailed==1);
		}
	else
		{
		test(package().iNumberCompleted==1);
		test(package().iNumberFailed==0);
		*(TMsvId*)&serviceEntry = package().iId;
		}
	test(package().iNumberRemaining==0);
	delete opert; opert=NULL;
	CleanupStack::PopAndDestroy(); // wait


	// Store the service entry body (service settings)
	// Get the store
	aClientEntry.SetEntryL(serviceEntry.Id());
	CMsvStore* store=NULL;
	TRAPD(err,store=aClientEntry.EditStoreL());
	CleanupStack::PushL(store);
	// Store settings
	
	CleanupStack::PopAndDestroy();          //store

	// The Id of the service entry
	return serviceEntry.Id(); 
	}

LOCAL_C TMsvId CreateFolderEntryL(CMsvEntry& aClientEntry)
//
// Create a folder entry below aClientEntry, and see if there was an error.
//
	{
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
	test(opert->iStatus.Int()==KErrNone);
	test(opert->Mtm()==KUidMsvLocalServiceMtm);
	TMsvLocalOperationProgress details;
	TPckgC<TMsvLocalOperationProgress> package(details);	
	package.Set(opert->ProgressL());
	test(package().iType==TMsvLocalOperationProgress::ELocalNew);
	test(package().iTotalNumberOfEntries==1);
	test(package().iError==KErrNone);
	test(package().iNumberCompleted==1);
	test(package().iNumberFailed==0);
	test(package().iNumberRemaining==0);
	delete opert; opert=NULL;
	CleanupStack::PopAndDestroy();//wait

	// The Id of the service entry
	return package().iId; 
	}

LOCAL_C void DeleteFolderEntryL(CMsvEntry& aClientEntry, TMsvId aIdToDelete)
//
// Delete a folder entry below aClientEntry, and see if there was an error.
//
	{
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();
	CMsvOperation*  opert = aClientEntry.DeleteL(aIdToDelete, wait->iStatus);

	CActiveScheduler::Start(); // operation complete
	test(opert->iStatus.Int()==KErrNone);
	test(opert->Mtm()==KUidMsvLocalServiceMtm);
	TMsvLocalOperationProgress details;
	TPckgC<TMsvLocalOperationProgress> package(details);	
	package.Set(opert->ProgressL());
	test(package().iType==TMsvLocalOperationProgress::ELocalDelete);
	test(package().iTotalNumberOfEntries==package().iNumberCompleted);
	test(package().iError==KErrNone);
	test(package().iNumberFailed==0);
	test(package().iNumberRemaining==0);
	delete opert; opert=NULL;
	CleanupStack::PopAndDestroy();//wait
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
	return folders;
	}

LOCAL_C void testCreateFoldersL()
	{
	theFolders = CreateFoldersL(KMsvGlobalInBoxIndexEntryId, KNrFolders);
	test (theFolders->Count() == KNrFolders);
	theSubfolders = CreateFoldersL((*theFolders)[0], KNrSubFolders);
	test (theSubfolders->Count() == KNrSubFolders);
	}

LOCAL_C void StoreQueuedOperationsL(CMsvEntry& aEntry, MImOffLineOperationArray& aArray)
	{
	CMsvStore* store=aEntry.EditStoreL();
	CleanupStack::PushL(store);
    
    CImOffLineArrayStore externaliser(aArray);
    externaliser.StoreL(*store);
    store->CommitL();
    
    CleanupStack::PopAndDestroy();//store
	}

LOCAL_C void testPopulateFoldersWithQueuedOperationsL()
	{
    TInt i;
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);

    CImOffLineOperationArray* array = CImOffLineOperationArray::NewL();
    CleanupStack::PushL(array);
//    array->Reset();
    CImOffLineOperation operation;
    for (i=0;i<KNrQueuedPerFolder;i++)
        {
        array->AppendOperationL(operation);
        }
    
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

    CleanupStack::PopAndDestroy(2);// array, msventry
	}

LOCAL_C void testDeleteFoldersL()
	{
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);
	TInt i;

	for ( i = 0 ; i < theFolders->Count() ; i ++ )
		{
		DeleteFolderEntryL(*msventry, (*theFolders)[i]);
		}

	CleanupStack::PopAndDestroy();//msventry

	delete theFolders;
	theFolders = NULL;
	delete theSubfolders;
	theSubfolders = NULL;
	}

LOCAL_C void testClearInboxL()
	{
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);
	theFolders = msventry->ChildrenWithServiceL(theServiceId) ;
	CleanupStack::PopAndDestroy();//msventry
	testDeleteFoldersL();
	}

//
// Generic test setup (cleanup stack, file system, scheduler)
// testSetup ensures the services that would normally be present
// under the Operating system are created, so the test code can use it.

LOCAL_C void GetMtmDetails(TUid aMtmId)
	{
	TBool isPresent=theClientRegistry->IsPresent(aMtmId);
	__ASSERT_DEBUG(isPresent!=EFalse, User::Panic(_L("reg test 2"),0));
	if (isPresent)
		{
		TPtrC name( theClientRegistry->RegisteredMtmDllInfo(aMtmId).HumanReadableName());
		TInt ordinal = theClientRegistry->RegisteredMtmDllInfo(aMtmId).iEntryPointOrdinalNumber;
		test.Printf(_L("%S  (ordinal %d)\n"), &name, ordinal);
		}
	}

LOCAL_C void UseClientRegistry()
	{
	TInt numMtms;

	numMtms = theClientRegistry->NumRegisteredMtmDlls();
	for (TInt ii=0; ii<numMtms; ii++)
		{
		TUid mtmId = theClientRegistry->MtmTypeUid(ii);
		GetMtmDetails(mtmId);
		}
	}

LOCAL_C void CreateDataComponentFileStoreL()
//
// Test read file handling.
//
	{
// Open file

	CMtmDllInfoArray* mtmdllinfoarray=new(ELeave) CMtmDllInfoArray();
	CleanupStack::PushL(mtmdllinfoarray);

	CMtmDllInfo* mtmdllinfo1=CMtmDllInfo::NewL(_L("Email"),TUidType(KDynamicLibraryUid,KUidMtmServerComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KServerMtmFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo1);

	CMtmDllInfo* mtmdllinfo2=CMtmDllInfo::NewL(_L("Email"),TUidType(KDynamicLibraryUid,KUidMtmClientComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KClientMtmFileName,3,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo2);

	CMtmDllInfo* mtmdllinfo3=CMtmDllInfo::NewL(_L("Email"),TUidType(KDynamicLibraryUid,KUidMtmUiComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KUiMtmFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo3);

	CMtmDllInfo* mtmdllinfo4=CMtmDllInfo::NewL(_L("Email"),TUidType(KDynamicLibraryUid,KUidMtmUiDataComponent,TUid::Uid(KUidMtmDefaultSpecificVal)),KUiDataMtmFileName,1,TVersion(2,0,0));
	mtmdllinfoarray->AddMtmDllInfoL(mtmdllinfo4);

	TCapabilitySet capSet;
	capSet.SetEmpty();
	CleanupStack::Pop(mtmdllinfoarray); // next line takes ownership
	CMtmGroupData* mtmgroupdata=CMtmGroupData::NewL(KUidMsgTypeIMAP4, theTechnologyTypeUid, mtmdllinfoarray, capSet);
	CleanupStack::PushL(mtmgroupdata);

	test.Printf(_L("Number of dlls %d\n"),mtmgroupdata->MtmDllInfoArray().Count());

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
	CleanupStack::PopAndDestroy(2, mtmgroupdata); // filestore, mtmgroupdata
	}

LOCAL_C void CreateClientRegistryL()
//
// Get access to the Client MTM object (CBaseMtm-derived).
//
	{
// Session observer. Needed to create a session
	theSessionObserver = new (ELeave) CDummySessionObserver;
	test(theSessionObserver != NULL);
	CleanupStack::PushL(theSessionObserver);
// Session. Needed to create a client registry.
	theSession=CMsvSession::OpenSyncL(*theSessionObserver);
	test(theSession != NULL);
	CleanupStack::PushL(theSession);

    TInt err=theSession->InstallMtmGroup(DataComponentFileName);
    test(err==KErrNone || err==KErrAlreadyExists);

// Client registry. Needed to get the MTM component
	theClientRegistry=CClientMtmRegistry::NewL(*theSession,KMsvDefaultTimeoutMicroSeconds32);
	test(theClientRegistry != NULL);
	test(theClientRegistry->IsPresent(KUidMsgTypeIMAP4));
	CleanupStack::PushL(theClientRegistry);

// CMsvEntry, needed to create the root entry, and then the TXT service entry
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvRootIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msventry);
// First try to find an already existing service node
	theServiceId = FindTxtServiceEntry(*msventry);
// Otherwise add it
	if (theServiceId == KMsvNullIndexEntryId)
		{
		test.Printf(_L("Creating service entry\n"));
        theServiceId  = CreateServiceEntryL(*msventry,KErrNone);
		}
	else
		{
		test.Printf(_L("Service entry found\n"));
		}
	// 
	test (theServiceId != KMsvNullIndexEntryId);
	//
    test.Printf(_L("Create mtm\n"));
	theClientMtm=(CImap4ClientMtm*)theClientRegistry->NewMtmL(KUidMsgTypeIMAP4);

// Step to the level of the service entry.
	msventry->SetEntryL(theServiceId);
	CleanupStack::Pop(); //msventry is owned by theClientMtm
	theClientMtm->SetCurrentEntryL(msventry);  // the object msventry points to is now owned by the mtm
// Store the settings for the service entry
    theClientMtm->SaveMessageL();
	test.Printf(_L("Text mtm type uid %d\n"),theClientMtm->Type());
	CleanupStack::Pop(3);// Client registry, session Observer & session
    }

LOCAL_C void DeleteClientRegistry()
	{
// Clean up
	delete theClientMtm;		theClientMtm	   = NULL;
	delete theClientRegistry;	theClientRegistry  = NULL;
	delete theSession;			theSession         = NULL;
	delete theSessionObserver;	theSessionObserver = NULL;
	}

LOCAL_C void testStoreServiceEntryL()
	{
	theClientMtm->SaveMessageL();
	}

/*LOCAL_C void testRestoreServiceEntryL()
	{
	theClientMtm->LoadMessageL();
	}
*/
LOCAL_C void testClearServiceEntry()
	{
    CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
    imap4Settings->CopyL(((CImap4ClientMtm*)theClientMtm)->Imap4Settings());
    imap4Settings->Reset();
    ((CImap4ClientMtm*)theClientMtm)->SetImap4SettingsL(*imap4Settings);
    delete imap4Settings;
	}

// Server MTM Communication Test Functions //

LOCAL_C void testCheckForConnectionL(TBool aShouldBeConnected)
	{
    test(theClientMtm != NULL);
	CMsvEntrySelection* aSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection);
	aSelection->AppendL(theServiceId);
    TBool result = aShouldBeConnected;

	TBuf8<256> parameter;
    ((CImap4ClientMtm*)theClientMtm)->InvokeSyncFunctionL(KIMAP4MTMIsConnected, *aSelection, parameter);
	TImap4GenericProgress temp;	
	TPckgC<TImap4GenericProgress> paramPack(temp);

	const TDesC8& progBuf = parameter;	
	paramPack.Set(progBuf);
	TImap4GenericProgress progress=paramPack();	

	result = progress.iErrorCode;
	if (result==KErrNone)
		test.Printf(_L("Connected to internet               "));
	else
		test.Printf(_L("Not connected to internet           "));

    CleanupStack::PopAndDestroy();//aSelection
    
    User::LeaveIfError((result!=0) != (aShouldBeConnected!=0));
    }

CImOperationQueueList *theQueue=NULL;

LOCAL_C void testGetQueListL()
	{
	CMsvEntry* msventry=CMsvEntry::NewL(*theSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	// Queue takes over	ownership of msventry
	theQueue = ((CImap4ClientMtm*)theClientMtm)->QueueListL(*msventry);
	CleanupStack::PushL(theQueue);

//	test.Printf(_L("%d operations queued\n"),theQueue->Count());
	
	CleanupStack::Pop();//theQueue
	}

LOCAL_C void testDeleteQueuedL(TInt aOffset, TInt aStep, TInt aNr)
	{
	test (theQueue != NULL);
	test (aNr <= theQueue->Count()); 
	TInt i;
	for (i=0;i<aNr;i++)
		{
		theQueue->DeleteL(aOffset);
		aOffset += aStep;
		aOffset %= theQueue->Count();
		}
	}

LOCAL_C void testExpungeQueListL()
	{
	test (theQueue != NULL);
	theQueue->ExpungeDeletedOperationsL();
	}

LOCAL_C void testFreeQueListL()
	{
	test (theQueue != NULL);
	delete theQueue;
	theQueue = NULL;
	}

LOCAL_C void testSetServiceSettings(TInt aService)
	{
    CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
    imap4Settings->CopyL(((CImap4ClientMtm*)theClientMtm)->Imap4Settings());
	switch (aService)
		{
		case 0:
            imap4Settings->SetServerAddressL(_L("128.2.10.131"));
			imap4Settings->SetPort(143);
			imap4Settings->SetLoginNameL(testUtils->MachineName());
			imap4Settings->SetPasswordL(testUtils->MachineName());
			imap4Settings->SetFolderPathL(_L8(""));
            break;
        default:
        case 1:
			imap4Settings->SetServerAddressL(_L("someothername"));
			imap4Settings->SetPort(111);
			imap4Settings->SetLoginNameL(testUtils->MachineName());
			imap4Settings->SetPasswordL(testUtils->MachineName());
			imap4Settings->SetFolderPathL(_L8("someothername"));
			break;
		}
    ((CImap4ClientMtm*)theClientMtm)->SetImap4SettingsL(*imap4Settings);
    delete imap4Settings;
    }

LOCAL_C TInt testVerifyServiceSettingsL()
	{
    CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
    imap4Settings->CopyL(((CImap4ClientMtm*)theClientMtm)->Imap4Settings());
    
    TInt result=0;
	result += imap4Settings->ServerAddress().Compare(_L("128.2.10.131"));
	result += imap4Settings->Port() - (143);
	result += imap4Settings->LoginName().Compare(testUtils->MachineName());
	result += imap4Settings->Password().Compare(testUtils->MachineName());
	result += imap4Settings->FolderPath().Compare(_L8(""));
    delete imap4Settings;
    return result;
	}

LOCAL_C void testCapabilitiesL()
	{
	// check the capabilities
	TInt response=0;
	// 
	// supported
	test(theClientMtm->QueryCapability(KUidMtmQueryMaxBodySize ,response)==KErrNone);
	test(response==0xffff); response=0;
	test(theClientMtm->QueryCapability(KUidMtmQueryMaxTotalMsgSize ,response)==KErrNone);
	test(response==0xffff); response=0;
	test(theClientMtm->QueryCapability(KUidMtmQuerySupportedBody ,response)==KErrNone);
//	test(theClientMtm->QueryCapability(KUidMtmQueryCanSendMsg ,response)==KErrNone);
	test(theClientMtm->QueryCapability(KUidMtmQueryCanReceiveMsg ,response)==KErrNone);
	test(theClientMtm->QueryCapability(KUidMtmQuerySupportAttachments ,response)==KErrNone);
	//
	// not supported
	test(theClientMtm->QueryCapability(KUidMtmQueryOffLineAllowed ,response)==KErrNotSupported);
	//
	// non-existant capability
	test(theClientMtm->QueryCapability(KUidMsgTypeIMAP4 ,response)==KErrNotSupported);
	}

#include <cemailaccounts.h>
#include <iapprefs.h>

LOCAL_C void testStoreSettingsL()
//
// Dummy settings for checking that StoreSettingsL work for the client mtm
//	
	{
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
	CleanupStack::PushL(imap4Settings);
	
	CImIAPPreferences* iapSettings = CImIAPPreferences::NewLC();
	
	accounts->PopulateDefaultImapSettingsL(*imap4Settings, *iapSettings);
	
	TImapAccount imapAccount = accounts->CreateImapAccountL(_L("ImapAccount"), *imap4Settings, *iapSettings, EFalse);
	((CImap4ClientMtm*)theClientMtm)->SwitchCurrentEntryL(imapAccount.iImapService);
	
    imap4Settings->CopyL(((CImap4ClientMtm*)theClientMtm)->Imap4Settings());
    imap4Settings->SetServerAddressL(KTestSettingsServerAddress);
	imap4Settings->SetPort(KTestSettingsPort);
	imap4Settings->SetLoginNameL(testUtils->MachineName());
	imap4Settings->SetPasswordL(testUtils->MachineName());
	imap4Settings->SetFolderPathL(KTestSettingsFolderPath);
    ((CImap4ClientMtm*)theClientMtm)->SetImap4SettingsL(*imap4Settings);
	CleanupStack::PopAndDestroy(3, accounts);
	TRAPD(err,((CImap4ClientMtm*)theClientMtm)->StoreSettingsL());
   	test(err==KErrNone);
	}

LOCAL_C TInt testVerifyStoreSettingsL()
//
// Comparison of the settings stored to see that the client commits after storing
//
	{
	TRAPD(err,((CImap4ClientMtm*)theClientMtm)->RestoreSettingsL());
	test(err==KErrNone);
    CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
	CleanupStack::PushL(imap4Settings);
    imap4Settings->CopyL(((CImap4ClientMtm*)theClientMtm)->Imap4Settings());
    
    TInt result=0;
	result += imap4Settings->ServerAddress().Compare(KTestSettingsServerAddress);
	result += imap4Settings->Port() - KTestSettingsPort;
	result += imap4Settings->LoginName().Compare(testUtils->MachineName());
	result += imap4Settings->Password().Compare(testUtils->MachineName());
	result += imap4Settings->FolderPath().Compare(KTestSettingsFolderPath);
    
	imap4Settings->Reset();
    ((CImap4ClientMtm*)theClientMtm)->SetImap4SettingsL(*imap4Settings);
	TRAP(err,((CImap4ClientMtm*)theClientMtm)->StoreSettingsL());
   	test(err==KErrNone);

   	TRequestStatus st;
   	TMsvId s =0;
   	TMsvPartList retPart=KMsvMessagePartBody;
   	TUint id=0;
   	TInt add=0;
   	
   	TBuf<1> des;
   	TBuf8<1> des8;
   		
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->ReplyL(s,retPart,st));
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->ForwardL(s,retPart,st));
   	((CImap4ClientMtm*)theClientMtm)->ValidateMessage(id);
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->AddAddresseeL(des));
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->AddAddresseeL(des));
   	((CImap4ClientMtm*)theClientMtm)->RemoveAddressee(add);
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->AddAttachmentL(des,des8,id,st));
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->AddAddresseeL(des));
	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->AddEntryAsAttachmentL(s,st));
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->AddEntryAsAttachmentL(s,st));
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->DefaultServiceL());
   	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->RemoveDefaultServiceL());
	TRAP_IGNORE(((CImap4ClientMtm*)theClientMtm)->ChangeDefaultServiceL(s));

	CleanupStack::PopAndDestroy(imap4Settings);
    return result;
	}

	
void TestForErrorL(TInt aError)
	{
	if (aError)
		{
		test.Printf(_L("Trap returned error %d\n"),aError);
        log->AppendError(_L8("Trap returned error"),aError);
        test.Getch();
		}
	test(aError==KErrNone);
	}

#define TEST(x)  TRAP(err,x);TestForErrorL(err)
#define TESTNEXT(x) test.Next(_L(x));log->AppendComment(_L8(x));log->AppendComment(_L8(""))

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);

	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();	

	testUtils->WriteComment(_L("IMAP4 Client MTM Tests"));
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********* T_IMPC Test IMAP4 Client MTM *********"));
	TBuf8<80> buf;

#if defined(__WINS__)	
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}

LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_IMPC Tests Complete    **********"));
	log->AppendComment(_L8(""));

	testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	User::After(5000000);
	}

LOCAL_C void doMainL()
    {
	InitL();
    TInt err=0;

// Create a file with the group data (the UIDs of the dll's that
// belong to this MTM group).
    
	TInt testNo = 1;
    TESTNEXT("Create Data Component FileStore");
	testUtils->TestStart(testNo++);
	TEST( CreateDataComponentFileStoreL()					);
	testUtils->TestFinish(testNo-1);
// Get the client mtm, by using the just created file
	TESTNEXT("Create Client Registry");
	testUtils->TestStart(testNo++);
	TEST( CreateClientRegistryL()							);
	testUtils->TestFinish(testNo-1);

// Show some information on the client side
	UseClientRegistry(); // Doesn't leave.

	testUtils->TestStart(testNo++);
    TEST( testCapabilitiesL()                               );
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Clear service entry");
	testUtils->TestStart(testNo++);
	TEST( testClearServiceEntry()							);
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Set service settings");
	TInt result = 0;
	testUtils->TestStart(testNo++);
	TEST( testSetServiceSettings(0)							);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( result = testVerifyServiceSettingsL()				);
	test (result == 0);
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Store service entry");
	testUtils->TestStart(testNo++);
	TEST( testStoreServiceEntryL()							);
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Set (incorrect) service settings");
	testUtils->TestStart(testNo++);
	TEST( testSetServiceSettings(1)							);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( result = testVerifyServiceSettingsL()				);
	test (result != 0);
	testUtils->TestFinish(testNo-1);

/*	TESTNEXT("Restore service entry");
	testUtils->TestStart(testNo++);
	TEST( testRestoreServiceEntryL()						);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( result = testVerifyServiceSettingsL()				);
	test (result == 0);
	testUtils->TestFinish(testNo-1);
*/
	// Test of StoreSettingsL so that the store don't get corrupted.
	TESTNEXT("Testing storing settings");
	testUtils->TestStart(testNo++);
	TEST( testStoreSettingsL()								);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( result = testVerifyStoreSettingsL()				);
	test( result == 0);
	testUtils->TestFinish(testNo-1);

    // Testing passing on commands to the server....
	TESTNEXT("Check that there is no connection yet");
    testUtils->TestStart(testNo++);
	TEST( testCheckForConnectionL(EFalse));
	testUtils->TestFinish(testNo-1);

	test.Next(_L("Creating folders"));
	testUtils->TestStart(testNo++);
	TEST( testClearInboxL()					);
	testUtils->TestFinish(testNo-1);

	test.Next(_L("Creating folders"));
	testUtils->TestStart(testNo++);
	TEST( testCreateFoldersL()							);
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Give folders queued operations");
	testUtils->TestStart(testNo++);
	TEST( testPopulateFoldersWithQueuedOperationsL()	);
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Testing queued operations access");
	testUtils->TestStart(testNo++);
	TEST( testGetQueListL()								);
	test (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( testExpungeQueListL()							);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( testFreeQueListL()							);
	testUtils->TestFinish(testNo-1);

#define KNrToDelete 5

	TESTNEXT("Testing rewrite of queued operations");
	testUtils->TestStart(testNo++);
	TEST( testGetQueListL()								);
	test (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	testDeleteQueuedL(0,4,KNrToDelete);
	test (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder-KNrToDelete);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( testFreeQueListL()							);
	testUtils->TestFinish(testNo-1);

	TESTNEXT("Testing removal queued operations");
	testUtils->TestStart(testNo++);
	TEST( testGetQueListL()								);
	test (theQueue->Count() == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder);
	testDeleteQueuedL(0,4,KNrToDelete);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( testExpungeQueListL()							);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( testFreeQueListL()							);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo++);
	TEST( testGetQueListL()								);
	TInt nr = theQueue->Count();
	test (nr == (KNrFolders + KNrSubFolders)*KNrQueuedPerFolder-KNrToDelete);
	testUtils->TestFinish(testNo-1);
	testUtils->TestStart(testNo++);
	TEST( testFreeQueListL()							);
	testUtils->TestFinish(testNo-1);

	test.Next(_L("Deleting folders"));
	testUtils->TestStart(testNo++);
	TEST( testDeleteFoldersL()							);
	testUtils->TestFinish(testNo-1);

	// Finished. Clean up all objects
	TESTNEXT("Deleting used objects");
	testUtils->TestStart(testNo++);
	TEST( DeleteClientRegistry()						);
	testUtils->TestFinish(testNo-1);

	Closedown();
    }

GLDEF_C TInt E32Main()
	{	
	TInt err;
	__UHEAP_MARK;
    test.Start(_L("T_IMPC Test harness"));
	theCleanup=CTrapCleanup::New();
    TRAP(err,doMainL());		
    test(err == KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
