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

#include "t_imapmailstore.h"
#include "cimaputils.h"

RTest test(KImapMailStoreTest);
	
CTestImapMailStore::CTestImapMailStore():
	iImapMailStore(NULL),iRequestCount(0),iServerEntry(NULL),iMsvServer(NULL),iSettings(NULL),iInfo(NULL)
	{	
	}
	
/**
Fills in a CImHeader object
@param aCImHeader the header to be filled.
*/
void CTestImapMailStore::CreateCImHeaderL(CImHeader& aCImHeader)
	{
	aCImHeader.Reset();
	aCImHeader.SetSubjectL(KMiutSubject);
	aCImHeader.SetImMsgIdL(KMiutMsgId);
	aCImHeader.SetFromL(KMiutFrom);
	aCImHeader.SetReplyToL(KMiutReplyTo);
	aCImHeader.ToRecipients().AppendL(KMiutToRecipients1());
	aCImHeader.ToRecipients().AppendL(KMiutToRecipients2);
	aCImHeader.CcRecipients().AppendL(KMiutCcRecipients1);
	aCImHeader.CcRecipients().AppendL(KMiutCcRecipients2);
	aCImHeader.BccRecipients().AppendL(KMiutBccRecipients);
	}

/**
Fills in a CImMimeHeader object
@param aCImMimeHeader the header to be filled.
*/
void CTestImapMailStore::CreateCImMimeHeaderL(CImMimeHeader& aCImMimeHeader)
	{
	aCImMimeHeader.SetContentTypeL(KImailHeader1);
	aCImMimeHeader.SetContentSubTypeL(KImailHeader2);
	aCImMimeHeader.SetContentDispositionL(KImailHeader9);
	aCImMimeHeader.SetContentDescriptionL(KImailHeader1);
	aCImMimeHeader.SetContentBaseL(KImailHeader3);
	aCImMimeHeader.SetContentLocationL(KImailHeader6);
	aCImMimeHeader.SetContentIDL(KImailHeader4);
	}

/**
Initialises the tests.
*/	
void CTestImapMailStore::SetupL()
//virtual 
	{
	//clear the messaging folder
	CEmailTestUtils* testUtils;
	testUtils = CEmailTestUtils::NewL(test);
	CleanupStack::PushL(testUtils);
	testUtils->FileSession().SetSessionPath(KCDrive);
	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();
	CleanupStack::PopAndDestroy(testUtils);

	
	//create a CMsvServerEntry
	iMsvServer = CMsvServer::NewL();

	// wait a couple of seconds
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;
	iServerEntry = CMsvServerEntry::NewL(*iMsvServer, KMsvRootIndexEntryId);

	CImapUtils::CreateL();
	
	//pass the CMsvServerEntry to the mailstore
	iImapMailStore= CImapMailStore::NewL(*iServerEntry);
	CreateMessageStructureL(KMsvGlobalOutBoxIndexEntryIdValue);	
	}

void CTestImapMailStore::TearDownL()
//virtual 
	{
	delete iImapMailStore;
	delete iServerEntry;
	delete iMsvServer;
	
	if(iSettings!=NULL)
		{
		delete iSettings;	
		}
	if(iInfo!=NULL)
		{
		delete iInfo;
		}

	CImapUtils::Delete();
	test.Close();
	}

/**
MMailStoreObserver method called by CImapMailStore.
*/	
void CTestImapMailStore::StoreOperationComplete(TMsvId /*aId*/, TInt /*aErrorCode*/)
	{
	--iRequestCount;
	
	__ASSERT_DEBUG(iRequestCount>=0, User::Invariant());
	if(iRequestCount==0)
		{
		CActiveScheduler::Stop();	
		}
	}


/**
From MChunkOutOfOrderObserver
*/	
void CTestImapMailStore::EnableSendFetch()
	{
	
	}
	
/**
Creates index entries for the tests.
@param aId	root entry.
*/	
void CTestImapMailStore::CreateMessageStructureL(TMsvId aId)
	{
	//message root entry
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypeIMAP4;
	iServerEntry->SetEntry(aId);
	iServerEntry->CreateEntry(entry1);
	iServerEntry->SetEntry(entry1.Id());
	iMsvMessageEntry=entry1.Id();
	
	//folder entry
	TMsvEntry folder;
	folder.iType = KUidMsvFolderEntry;
	folder.iMtm = KUidMsvLocalServiceMtm;
	folder.iServiceId = KMsvLocalServiceIndexEntryId;
	iServerEntry->SetEntry(iMsvMessageEntry);
	iServerEntry->CreateEntry(folder);
	iServerEntry->SetEntry(folder.Id());
	iFolderId=folder.Id();
	
	//attachment entry
	TMsvEntry attentry;
	attentry.iType = KUidMsvAttachmentEntry;
	attentry.iServiceId = KMsvLocalServiceIndexEntryId;
	attentry.iMtm = KUidMsgTypeIMAP4;

	attentry.iDetails.Set(KAttachmentDetails);
	attentry.iSize=100;
	iServerEntry->SetEntry(iFolderId);
	iServerEntry->CreateEntry(attentry);
	iServerEntry->SetEntry(attentry.Id());
	iAttMessageEntry=attentry.Id();	

	//body entry
	TMsvEntry bodyentry;
	bodyentry.iType = KUidMsvEmailTextEntry;
	bodyentry.iServiceId = KMsvLocalServiceIndexEntryId;
	bodyentry.iMtm = KUidMsgTypeIMAP4;
	bodyentry.iDetails.Set(KBodyDetails);
	iServerEntry->SetEntry(iFolderId);
	iServerEntry->CreateEntry(bodyentry);
	iServerEntry->SetEntry(bodyentry.Id());
	iBodyMessageEntry=bodyentry.Id();	
	}


CTestSuite* CTestImapMailStore::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	END_SUITE;
	}
	

