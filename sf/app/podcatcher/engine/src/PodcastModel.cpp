/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <commdb.h>
#include "PodcastModel.h"
#include "FeedEngine.h"
#include "SettingsEngine.h"
#include "ShowEngine.h"
#include "connectionengine.h"
#include "podcastutils.h"
#include "Podcatcher.pan"

#include <cmdestination.h>
#include <cmmanager.h>
#include <bautils.h>
#include <aknserverapp.h>  // MAknServerAppExitObserver
#include <DocumentHandler.h>


const TInt KDefaultGranu = 5;
_LIT(KDBFileName, "podcatcher.sqlite");
_LIT(KDBTemplateFileName, "podcatcher.sqlite.template");

EXPORT_C CPodcastModel* CPodcastModel::NewL()
{
	CPodcastModel* self = new (ELeave) CPodcastModel;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CPodcastModel::~CPodcastModel()
{
	
	delete iFeedEngine;
	delete iSettingsEngine;
	delete iShowEngine;

	delete iIapNameArray;
	iIapIdArray.Close();
	
	delete iSNAPNameArray;
	iSNAPIdArray.Close();
	delete iCommDB;	
	sqlite3_close(iDB);
	iFsSession.Close();
	iActiveShowList.ResetAndDestroy();
	iActiveShowList.Close();
	delete iConnectionEngine;
	iCmManager.Close();
	delete iImageHandler;
	delete iDocHandler;
}

CPodcastModel::CPodcastModel()
{	
}

void CPodcastModel::ConstructL()
{
	DP("CPodcastModel::ConstructL BEGIN");
	User::LeaveIfError(iFsSession.Connect());
	iCommDB = CCommsDatabase::NewL (EDatabaseTypeUnspecified);
	iIapNameArray = new (ELeave) CDesCArrayFlat(KDefaultGranu);
	iSNAPNameArray = new (ELeave) CDesCArrayFlat(KDefaultGranu);

	iImageHandler = CImageHandler::NewL(FsSession(), *this);
	iDocHandler = CDocumentHandler::NewL(CEikonEnv::Static()->Process());

	TRAPD(err,iCmManager.OpenL());
	DP1("iCmManager.OpenL(),err=%d;", err);
	
	if (err == KErrNone)
		{
		UpdateIAPListL();
		UpdateSNAPListL();
		}
	
	iSettingsEngine = CSettingsEngine::NewL(*this);
	iConnectionEngine = CConnectionEngine::NewL(*this);	
	
	TRAP(err, OpenDBL());
	
	if (err != KErrNone)
		{
		ResetDB();
		
		TRAP(err, OpenDBL());
		
		if (err != KErrNone)
			{
			Panic(EPodcatcherPanicDB);
			}
		
		}
	
	iFeedEngine = CFeedEngine::NewL(*this);
	iShowEngine = CShowEngine::NewL(*this);

	DP("CPodcastModel::ConstructL END");
}



EXPORT_C void CPodcastModel::UpdateIAPListL()
	{
	iIapNameArray->Reset();
	iIapIdArray.Reset();	   

	CCommsDbTableView* table = iCommDB->OpenTableLC (TPtrC (IAP)); 
	TInt ret = table->GotoFirstRecord ();
	TPodcastIAPItem IAPItem;
	TBuf <KCommsDbSvrMaxFieldLength> bufName;
	while (ret == KErrNone) // There was a first record
		{
		table->ReadUintL(TPtrC(COMMDB_ID), IAPItem.iIapId);
		table->ReadTextL (TPtrC(COMMDB_NAME), bufName);
		table->ReadTextL (TPtrC(IAP_BEARER_TYPE), IAPItem.iBearerType);
		table->ReadTextL (TPtrC(IAP_SERVICE_TYPE), IAPItem.iServiceType);

		iIapIdArray.Append(IAPItem);
		iIapNameArray->AppendL(bufName); 		 
		ret = table->GotoNextRecord();
		}
	CleanupStack::PopAndDestroy(); // Close table
	}

EXPORT_C void CPodcastModel::UpdateSNAPListL()
{
	DP("CPodcastModel::UpdateSNAPListL BEGIN");
	iSNAPNameArray->Reset();
	iSNAPIdArray.Reset();
	
	RCmDestination destination;
	TPodcastIAPItem IAPItem;
	
	RArray<TUint32> destArray;
	CleanupClosePushL(destArray);
	iCmManager.AllDestinationsL(destArray);
	
	TInt cnt = destArray.Count();
	DP1("destArray.Count==%d", cnt);
	for(TInt loop = 0;loop<cnt;loop++)
		{
		destination = iCmManager.DestinationL (destArray[loop]);
		CleanupClosePushL(destination);
		if(!destination.IsHidden())
			{
			IAPItem.iIapId = destArray[loop];
			HBufC* name = destination.NameLC();
			DP1(" destination.NameLC==%S", name);
			iSNAPNameArray->AppendL(*name);
			CleanupStack::PopAndDestroy(name);
			iSNAPIdArray.Append(IAPItem);
			}
		CleanupStack::PopAndDestroy();//close destination
		}
	CleanupStack::PopAndDestroy();// close destArray

	DP("CPodcastModel::UpdateSNAPListL END");
}

EXPORT_C CDesCArrayFlat* CPodcastModel::IAPNames()
{
	return iIapNameArray;
}

EXPORT_C RArray<TPodcastIAPItem>& CPodcastModel::IAPIds()
{
	return iIapIdArray;
}

EXPORT_C CDesCArrayFlat* CPodcastModel::SNAPNames()
{
	return iSNAPNameArray;
}

EXPORT_C RArray<TPodcastIAPItem>& CPodcastModel::SNAPIds()
{
	return iSNAPIdArray;
}


RFs& CPodcastModel::FsSession()
	{
	return iFsSession;
	}

EXPORT_C void CPodcastModel::SetPlayingPodcast(CShowInfo* aPodcast)
{
	iPlayingPodcast = aPodcast;
}

EXPORT_C CShowInfo* CPodcastModel::PlayingPodcast()
{
	return iPlayingPodcast;
}

EXPORT_C CFeedEngine& CPodcastModel::FeedEngine()
{
	return *iFeedEngine;
}
	
EXPORT_C CShowEngine& CPodcastModel::ShowEngine()
{
	return *iShowEngine;
}

EXPORT_C CSettingsEngine& CPodcastModel::SettingsEngine()
{
	return *iSettingsEngine;
}

EXPORT_C CConnectionEngine& CPodcastModel::ConnectionEngine()
{
	return *iConnectionEngine;
}

EXPORT_C void CPodcastModel::PlayPausePodcastL(CShowInfo* aPodcast, TBool /* aPlayOnInit */) 
	{
	DP("CPodcastModel::PlayPausePodcastL BEGIN");
	TRAPD(err, LaunchFileEmbeddedL(aPodcast->FileName()));
	
	if (err == KErrNone)
		{
		aPodcast->SetPlayState(EPlayed);
		iShowEngine->UpdateShowL(*aPodcast);
		}
	else
		{
		User::Leave(err);
		}
	DP("CPodcastModel::PlayPausePodcastL END");
	}

EXPORT_C CFeedInfo* CPodcastModel::ActiveFeedInfo()
{
	return iActiveFeed;
}

EXPORT_C void CPodcastModel::SetActiveFeedInfo(CFeedInfo* aFeedInfo)
{
	iActiveFeed = aFeedInfo;
}

EXPORT_C RShowInfoArray& CPodcastModel::ActiveShowList()
{
	return iActiveShowList;
}

void CPodcastModel::SetActiveShowList(RShowInfoArray& aShowArray)
{
	iActiveShowList.ResetAndDestroy();
	TInt cnt = aShowArray.Count();

	for(TInt loop = 0;loop < cnt; loop++)
	{
		iActiveShowList.Append(aShowArray[loop]);
	}
}

EXPORT_C void CPodcastModel::DropDB()
	{
	if (iDB != NULL)
		{
		sqlite3_close(iDB);
		iDB = NULL;
		}
	
	TFileName dbFileName;
	dbFileName.Copy(iSettingsEngine->PrivatePath());
	dbFileName.Append(KDBFileName);

	// remove the old DB file
	if (BaflUtils::FileExists(iFsSession, dbFileName))
		{
		BaflUtils::DeleteFile(iFsSession, dbFileName);
		}
	}

void CPodcastModel::ResetDB()
	{
	DP("CPodcastModel::ResetDB BEGIN");
	
	DropDB();
	
	TFileName dbFileName;
	dbFileName.Copy(iSettingsEngine->PrivatePath());
	dbFileName.Append(KDBFileName);

	// remove the old DB file
	if (BaflUtils::FileExists(iFsSession, dbFileName))
		{
		BaflUtils::DeleteFile(iFsSession, dbFileName);
		}

	// copy template to new DB
	TFileName dbTemplate;
	TFileName temp;
	dbTemplate.Copy(_L("z:"));
	temp.Copy(iSettingsEngine->PrivatePath());
	dbTemplate.Append(temp);
	dbTemplate.Append(KDBTemplateFileName);
	
	DP1("Copy template DB from: %S", &dbTemplate);
	DP1("Copy template DB to: %S", &dbFileName);
	
	BaflUtils::CopyFile(iFsSession, dbTemplate,dbFileName);
	iIsFirstStartup = ETrue;
	DP("CPodcastModel::ResetDB END");
	}


void CPodcastModel::OpenDBL()
	{
	DP("CPodcastModel::OpenDBL BEGIN");
	
	if (iDB != NULL)
		{
		sqlite3_close(iDB);
		iDB = NULL;
		}
	
	TFileName dbFileName;
	dbFileName.Copy(iSettingsEngine->PrivatePath());
	dbFileName.Append(KDBFileName);
		
	if (!BaflUtils::FileExists(iFsSession, dbFileName))
		{
		User::Leave(KErrNotFound);
		}
	
	if (iDB == NULL) {	
		// open DB
		TBuf8<KMaxFileName> filename8;
		filename8.Copy(dbFileName);
		int rc = sqlite3_open((const char*) filename8.PtrZ(), &iDB);
		if(rc != SQLITE_OK){
			User::Leave(KErrCorrupt);
		}

		// do a test query 
		sqlite3_stmt *st;
		rc = sqlite3_prepare_v2(iDB,"select count(*) from feeds" , -1, &st, (const char**) NULL);
		if( rc==SQLITE_OK )
			{
			Cleanup_sqlite3_finalize_PushL(st);
			rc = sqlite3_step(st);
					
			if (rc != SQLITE_ROW)
				{
				User::Leave(KErrCorrupt);
				}
			CleanupStack::PopAndDestroy(); // st
			}
		else
			{
			User::Leave(KErrCorrupt);
			}
		}

	DP("CPodcastModel::OpenDBL END");	
	}

sqlite3* CPodcastModel::DB()
{
	return iDB;
}

void CPodcastModel::SetProxyUsageIfNeededL(RHTTPSession& aSession)
	{
	TBool useProxy = EFalse;
	HBufC* serverName = NULL;
	TUint32 port = 0;
	
	TRAPD(err,GetProxyInformationForConnectionL(useProxy, serverName, port));
	if (err == KErrNone && useProxy)
		{
		CleanupStack::PushL(serverName);
		TBuf8<128> proxyAddr;
		proxyAddr.Append(*serverName);
		proxyAddr.Append(':');
		proxyAddr.AppendNum(port);
				
		RStringF prxAddr = aSession.StringPool().OpenFStringL(proxyAddr);
		CleanupClosePushL(prxAddr);
		THTTPHdrVal prxUsage(aSession.StringPool().StringF(HTTP::EUseProxy,RHTTPSession::GetTable()));

		aSession.ConnectionInfo().SetPropertyL(
						aSession.StringPool().StringF(HTTP::EProxyUsage,RHTTPSession::GetTable()), 
						aSession.StringPool().StringF(HTTP::EUseProxy,RHTTPSession::GetTable()));

		aSession.ConnectionInfo().SetPropertyL(aSession.StringPool().StringF(HTTP::EProxyAddress,RHTTPSession::GetTable()), prxAddr); 
		
		CleanupStack::PopAndDestroy(&prxAddr);
		CleanupStack::PopAndDestroy(serverName);
		}
	}


void CPodcastModel::GetProxyInformationForConnectionL(TBool& aIsUsed, HBufC*& aProxyServerName, TUint32& aPort)
	{
	TInt iapId = GetIapId();
	CCommsDbTableView* table = iCommDB->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(COMMDB_ID), iapId);
	
	TUint32 iapService;
	HBufC* iapServiceType;
	table->ReadUintL(TPtrC(IAP_SERVICE), iapService);
	iapServiceType = table->ReadLongTextLC(TPtrC(IAP_SERVICE_TYPE));
	
	CCommsDbTableView* proxyTableView = iCommDB->OpenViewOnProxyRecordLC(iapService, *iapServiceType);
	TInt err = proxyTableView->GotoFirstRecord();
	if( err != KErrNone)
		{
		User::Leave(KErrNotFound);	
		}

	proxyTableView->ReadBoolL(TPtrC(PROXY_USE_PROXY_SERVER), aIsUsed);
	if(aIsUsed)
		{
		HBufC* serverName = proxyTableView->ReadLongTextLC(TPtrC(PROXY_SERVER_NAME));
		proxyTableView->ReadUintL(TPtrC(PROXY_PORT_NUMBER), aPort);
		aProxyServerName = serverName->AllocL();
		CleanupStack::PopAndDestroy(serverName);
		}
		
	CleanupStack::PopAndDestroy(proxyTableView);
	CleanupStack::PopAndDestroy(iapServiceType);
	CleanupStack::PopAndDestroy(table);
	}
	
TInt CPodcastModel::GetIapId()
	{
	_LIT(KSetting, "IAP\\Id");
	TUint32 iapId = 0;
	iConnectionEngine->Connection().GetIntSetting(KSetting, iapId);
	return iapId;
	}

EXPORT_C void CPodcastModel::GetShowsDownloadingL()
	{
	iActiveShowList.ResetAndDestroy();
	iShowEngine->GetShowsDownloadingL(iActiveShowList);
	}

EXPORT_C void CPodcastModel::GetShowsByFeedL(TUint aFeedUid)
	{
	iActiveShowList.ResetAndDestroy();
	iShowEngine->DeleteOldShowsByFeedL(aFeedUid);
	iShowEngine->CheckForDeletedShows(aFeedUid);
	iShowEngine->GetShowsByFeedL(iActiveShowList, aFeedUid);
	}

EXPORT_C void CPodcastModel::MarkSelectionPlayedL()
	{
	for (int i=0;i<iActiveShowList.Count();i++) {
		if(iActiveShowList[i]->PlayState() != EPlayed) {
			iActiveShowList[i]->SetPlayState(EPlayed);
			iShowEngine->UpdateShowL(*iActiveShowList[i]);
		}
	}
	}

TInt CPodcastModel::FindActiveShowByUid(TUint aUid)
	{
	for (int i=0;i<iActiveShowList.Count();i++) {
		if (iActiveShowList[i]->Uid() == aUid) {
			return i;
		}
	}
	
	return KErrNotFound;
	}

EXPORT_C TBool CPodcastModel::IsFirstStartup()
	{
	return iIsFirstStartup;
	}


void CPodcastModel::ImageOperationCompleteL(TInt /*aError*/, TUint /*aHandle*/, CPodcastModel& /*aPodcastModel*/)
	{
	
	}

EXPORT_C CImageHandler& CPodcastModel::ImageHandler()
	{
	return *iImageHandler;
	}

void CPodcastModel::LaunchFileEmbeddedL(const TDesC& aFilename)
    {
    //Set the exit observer so HandleServerAppExit will be called
    iDocHandler->SetExitObserver(this);   
 
    TDataType emptyDataType = TDataType();
    //Open a file embedded
    iDocHandler->OpenFileEmbeddedL(aFilename, emptyDataType);             
    }
 
void CPodcastModel::HandleServerAppExit(TInt aReason)
    {
    //Handle closing the handler application
    MAknServerAppExitObserver::HandleServerAppExit(aReason);
    }

