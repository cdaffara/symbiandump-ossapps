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

#ifndef PODCASTMODEL_H
#define PODCASTMODEL_H
#include <e32base.h>
#include <eikenv.h>
#include <commdb.h>
#include <CommDbConnPref.h>
#include <es_sock.h>
#include <http/rhttpsession.h>
#include <cmmanager.h>

#include "FeedInfo.h"
#include "ShowInfo.h"
#include "debug.h"
#include <sqlite3.h>
#include "ImageHandler.h"
#include <aknserverapp.h>  // MAknServerAppExitObserver
#include <DocumentHandler.h>

// SQLite leaks memory when sorting, so to test our own memory leaks we need to disable this
//#define DONT_SORT_SQL
class RCmManager;
class CFeedEngine;
class CShowEngine;
class CSettingsEngine;
class CCommsDatabase;
class CConnectionEngine;

class TPodcastIAPItem
{
public:
	TUint32 iIapId;
	TBuf<KCommsDbSvrMaxFieldLength> iServiceType;
	TBuf<KCommsDbSvrMaxFieldLength> iBearerType;
};

/**
 * This class handles application storage needs and ownership of audioplayer, resource lists etc.
 */
class CPodcastModel : public CBase, public MImageHandlerCallback, public MAknServerAppExitObserver
{
public:
	IMPORT_C static CPodcastModel* NewL();
	IMPORT_C ~CPodcastModel();
	IMPORT_C CFeedEngine& FeedEngine();
	IMPORT_C CShowEngine& ShowEngine();
	IMPORT_C CSettingsEngine& SettingsEngine();
	IMPORT_C CConnectionEngine& ConnectionEngine();
	IMPORT_C CShowInfo* PlayingPodcast();
	IMPORT_C void SetPlayingPodcast(CShowInfo* aPodcast);
	IMPORT_C void PlayPausePodcastL(CShowInfo * aPodcast, TBool aPlayOnInit = EFalse);
	IMPORT_C CFeedInfo* ActiveFeedInfo();
	IMPORT_C void SetActiveFeedInfo(CFeedInfo* aFeedInfo);
	
	RFs& FsSession();
	
	IMPORT_C RShowInfoArray& ActiveShowList();
	
	void SetActiveShowList(RShowInfoArray& aShowArray);
	
	IMPORT_C void UpdateIAPListL();
	IMPORT_C void UpdateSNAPListL();
	IMPORT_C CDesCArrayFlat* IAPNames();
	IMPORT_C RArray<TPodcastIAPItem>& IAPIds();
	IMPORT_C CDesCArrayFlat* SNAPNames();
	IMPORT_C RArray<TPodcastIAPItem>& SNAPIds();

	void SetProxyUsageIfNeededL(RHTTPSession& aSession);
	void GetProxyInformationForConnectionL(TBool& aIsUsed, HBufC*& aServerName, TUint32& aPort);
	TInt GetIapId();
	
	sqlite3* DB();
	IMPORT_C void DropDB();
	
	IMPORT_C void GetShowsDownloadingL();
	IMPORT_C void GetShowsByFeedL(TUint aFeedUid);
	IMPORT_C void MarkSelectionPlayedL();
	
	TInt FindActiveShowByUid(TUint aUid);
	IMPORT_C TBool IsFirstStartup();
	IMPORT_C CImageHandler& ImageHandler();
protected:
	CPodcastModel();
	void ConstructL();
	void ResetDB();
	void OpenDBL();
	// From ImageHandler
	void ImageOperationCompleteL(TInt aError, TUint aHandle, CPodcastModel& aPodcastModel);	

private:  // Functions from base classes

   /**
     * From MAknServerAppExitObserver.
     * Handles the exit of a connected server application.
     */ 
    void HandleServerAppExit(TInt aReason);

private:  // Private functions
    void LaunchFileEmbeddedL(const TDesC& aFilename);
    
private:	
   CShowInfo* iPlayingPodcast;
   
   CFeedEngine* iFeedEngine;
   CShowEngine* iShowEngine;
   CSettingsEngine *iSettingsEngine;
   CConnectionEngine* iConnectionEngine;
   RShowInfoArray iActiveShowList;
   CFeedInfo *iActiveFeed;   
   
   RFs iFsSession;
   RArray<TPodcastIAPItem> iIapIdArray;
   CDesCArrayFlat* iIapNameArray;
   
   RArray<TPodcastIAPItem> iSNAPIdArray;
   CDesCArrayFlat* iSNAPNameArray;
   CCommsDatabase* iCommDB;  
   
   sqlite3* iDB;
   RCmManager iCmManager;
   TBool iIsFirstStartup;
   CImageHandler* iImageHandler;
   CDocumentHandler* iDocHandler;
};

#endif // PODCASTMODEL_H
