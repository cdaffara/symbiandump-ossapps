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

#ifndef SHOWENGINE_H_
#define SHOWENGINE_H_

#include <e32base.h>
#include <APGCLI.H>
#include "constants.h"
#include "ShowInfo.h"
#include "PodcastModel.h"
#include "HttpClient.h"
#include "ShowEngineObserver.h"
#include "MetaDataReader.h"
#include <sqlite3.h>
#ifdef ENABLE_MPX_INTEGRATION    
#include "mpxcollectionhelper.h"
#include "mpxcollectionhelperobserver.h"
#endif

struct TDownload
	{
	TUint iIndex;
	TUint iUid;
	};
#ifdef ENABLE_MPX_INTEGRATION    
class CShowEngine : public CBase, public MHttpClientObserver, public MMetaDataReaderObserver,
					public MMPXCollectionHelperObserver
#else
class CShowEngine : public CBase, public MHttpClientObserver, public MMetaDataReaderObserver
#endif
{
public:
	IMPORT_C static CShowEngine* NewL(CPodcastModel& aPodcastModel);
	IMPORT_C virtual ~CShowEngine();
	
public:
	IMPORT_C void AddDownloadL(CShowInfo& info);
	IMPORT_C void RemoveDownloadL(TUint aUid);
	IMPORT_C void RemoveAllDownloadsL();

	IMPORT_C void SuspendDownloads();
	IMPORT_C void ResumeDownloadsL();

	IMPORT_C TInt GetNumDownloadingShows();
	IMPORT_C CShowInfo* ShowDownloading();
	IMPORT_C CShowInfo* GetShowByUidL(TUint aShowUid);
	IMPORT_C CShowInfo* GetNextShowByTrackL(CShowInfo* aShowInfo);
	
	// show access methods
	IMPORT_C void GetAllShowsL(RShowInfoArray &aArray);
	IMPORT_C void GetShowsByFeedL(RShowInfoArray &aArray, TUint aFeedUid);
	IMPORT_C void GetShowsDownloadedL(RShowInfoArray &aArray);
	IMPORT_C void GetNewShowsL(RShowInfoArray &aArray);
	IMPORT_C void GetShowsDownloadingL(RShowInfoArray &aArray);
	IMPORT_C CShowInfo* DBGetShowByFileNameL(TFileName aFileName);
	
	IMPORT_C void AddShowL(const CShowInfo& item);
	IMPORT_C void DeletePlayedShowsL(RShowInfoArray &aShowInfoArray);
	IMPORT_C void DeleteAllShowsByFeedL(TUint aFeedUid,TBool aDeleteFiles=ETrue);
	IMPORT_C void DeleteShowL(TUint aShowUid, TBool aRemoveFile=ETrue);
	IMPORT_C void DeleteOldShowsByFeedL(TUint aFeedUid);
	
	IMPORT_C void AddObserver(MShowEngineObserver *observer);
	IMPORT_C void RemoveObserver(MShowEngineObserver *observer);

	IMPORT_C void NotifyShowListUpdatedL();
	IMPORT_C void UpdateShowL(CShowInfo& aInfo);

	IMPORT_C void GetMimeType(const TDesC& aFileName, TDes& aMimeType);

	IMPORT_C void CheckForDeletedShows(TUint aFeedUid);
	IMPORT_C CMetaDataReader& MetaDataReader();
	IMPORT_C void MoveDownloadUpL(TUint aUid);
	IMPORT_C void MoveDownloadDownL(TUint aUid);

private:
	// from HttpClientObserver, dont have to be public
	void Connected(CHttpClient* aClient);
	void Disconnected(CHttpClient* aClient);
	void CompleteL(CHttpClient* aClient, TInt aError);
	void Progress(CHttpClient* aHttpClient, int aBytes, int aTotalBytes);
	void DownloadInfo(CHttpClient* aClient, int aSize);
	void FileError(TUint aError);
	// from MetaDataReaderObserver
	void ReadMetaDataL(CShowInfo& aShowInfo);
	void ReadMetaDataCompleteL();
	// from MMPXCollectionHelperObserver
    void HandleAddFileCompleteL( TInt /*aErr*/ ) {}  

private:
	CShowEngine(CPodcastModel& aPodcastModel);
	void ConstructL();

	void GetShowL(CShowInfo *info);

	void NotifyDownloadQueueUpdatedL();
	void NotifyShowDownloadUpdatedL(TInt aBytesOfCurrentDownload, TInt aBytesTotal);
	void NotifyShowFinishedL(TInt aError);
	void DownloadNextShowL();

	static TInt CompareShowsByDate(const CShowInfo &a, const CShowInfo &b);
	static TBool CompareShowsByUid(const CShowInfo &a, const CShowInfo &b);
	static TInt CompareShowsByTitle(const CShowInfo &a, const CShowInfo &b);
	static TInt CompareShowsByTrackNo(const CShowInfo &a, const CShowInfo &b);
	
	void AddShowToMpxCollection(CShowInfo &aShowInfo);

private:
	// DB methods
	CShowInfo* DBGetShowByUidL(TUint aUid);
	void DBFillShowInfoFromStmtL(sqlite3_stmt *st, CShowInfo* showInfo);
	void DBAddShowL(const CShowInfo& aItem);
	void DBUpdateShowL(CShowInfo& aItem);
	void DBGetShowsByFeedL(RShowInfoArray& aShowArray, TUint aFeedUid);
	void DBGetAllShowsL(RShowInfoArray& aShowArray);
	void DBGetNewShowsL(RShowInfoArray& aShowArray);
	void DBGetDownloadedShowsL(RShowInfoArray& aShowArray);
	void DBDeleteAllShowsByFeedL(TUint aFeedUid);
	void DBDeleteOldShowsByFeedL(TUint aFeedUid);
	void DBDeleteShowL(TUint aUid);
	void DBRemoveAllDownloadsL();
	void DBRemoveDownloadL(TUint aUid);
	void DBGetAllDownloadsL(RShowInfoArray& aShowArray);
	TUint DBGetDownloadsCountL();
	void DBAddDownloadL(TUint aUid);
	CShowInfo* DBGetNextDownloadL();
	void DBSwapDownloadsL(TDownload aFirstDL, TDownload aSecondDL);
	
private:
	CHttpClient* iShowClient;			
		
	CPodcastModel& iPodcastModel;

	// observers that will receive callbacks
    RArray<MShowEngineObserver*> iObservers;

	// The show we are currently downloading
	CShowInfo* iShowDownloading;
    TUint iDownloadErrors;
    
    CMetaDataReader* iMetaDataReader;
    
    RApaLsSession iApaSession;
	TBuf8<512> iRecogBuffer;
	
	sqlite3& iDB;
    TBuf<KDefaultSQLDataBufferLength> iSqlBuffer;
#ifdef ENABLE_MPX_INTEGRATION    
    MMPXCollectionHelper* iCollectionHelper;
#endif
};

#endif /*SHOWENGINE_H_*/

