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

#include "ShowEngine.h"
#include "FeedEngine.h"
#include "FeedInfo.h"
#include <bautils.h>
#include <s32file.h>
#include "SettingsEngine.h"
#include <e32hashtab.h>
#include <httperr.h>
#include "debug.h"
#include "PodcastUtils.h"
#include <mpxcollectionhelperfactory.h>

const TUint KMaxDownloadErrors = 3;
const TInt KMimeBufLength = 100;

CShowEngine::CShowEngine(CPodcastModel& aPodcastModel) :
	iPodcastModel(aPodcastModel),
	iDB(*aPodcastModel.DB())
	{
	}

EXPORT_C CShowEngine::~CShowEngine()
	{	
	delete iShowClient;
	iObservers.Close();
	delete iShowDownloading;
	delete iMetaDataReader;
	iApaSession.Close();
#ifdef ENABLE_MPX_INTEGRATION  	
	if (iCollectionHelper)
		delete iCollectionHelper;
#endif
	}

EXPORT_C CShowEngine* CShowEngine::NewL(CPodcastModel& aPodcastModel)
	{
	CShowEngine* self = new (ELeave) CShowEngine(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CShowEngine::GetMimeType(const TDesC& aFileName, TDes& aMimeType)
	{
	aMimeType.Zero();
	RFile file;
	if (file.Open(iPodcastModel.FsSession(), aFileName, 0) == KErrNone)
		{
		if (file.Read(iRecogBuffer) == KErrNone)
			{
			TDataRecognitionResult result;
			if (iApaSession.RecognizeData(aFileName, iRecogBuffer, result)
					== KErrNone)
				{
				aMimeType.Copy(result.iDataType.Des());
				}

			}
		}
	file.Close();
	}

void CShowEngine::ConstructL()
	{	
	iShowClient = CHttpClient::NewL(iPodcastModel, *this);
	iShowClient->SetResumeEnabled(ETrue);
	iMetaDataReader = new (ELeave) CMetaDataReader(*this, iPodcastModel.FsSession());
	iMetaDataReader->ConstructL();
	User::LeaveIfError(iApaSession.Connect());
	}

EXPORT_C void CShowEngine::SuspendDownloads()
	{
	iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
	iShowClient->Stop();
	}

EXPORT_C void CShowEngine::ResumeDownloadsL()
	{
	DP("CShowEngine::ResumeDownloadsL BEGIN");
	if (iPodcastModel.SettingsEngine().DownloadSuspended())
		{
		iPodcastModel.SettingsEngine().SetDownloadSuspended(EFalse);
		iDownloadErrors = 0;
		DownloadNextShowL();
		}
	DP("CShowEngine::ResumeDownloadsL END");
	}

EXPORT_C void CShowEngine::RemoveAllDownloadsL()
	{
	if (!iPodcastModel.SettingsEngine().DownloadSuspended())
		{
		SuspendDownloads();
		}

	DBRemoveAllDownloadsL();
	NotifyDownloadQueueUpdatedL();
	}

EXPORT_C void CShowEngine::RemoveDownloadL(TUint aUid)
	{
	DP("CShowEngine::RemoveDownloadL BEGIN");

	TBool resumeAfterRemove = EFalse;
	// if trying to remove the present download, we first stop it
	if (!iPodcastModel.SettingsEngine().DownloadSuspended() && iShowDownloading != NULL
			&& iShowDownloading->Uid() == aUid)
		{
		DP("CShowEngine::RemoveDownload\t This is the active download, we suspend downloading");
		SuspendDownloads();
		
		// partial downloads should be removed
		BaflUtils::DeleteFile(iPodcastModel.FsSession(), iShowDownloading->FileName());

		resumeAfterRemove = ETrue;
		}

	CShowInfo *info = DBGetShowByUidL(aUid);
	if (info != NULL)
		{
		info->SetDownloadState(ENotDownloaded);
		DBUpdateShowL(*info);
		delete info;
		}
	
	DBRemoveDownloadL(aUid);

	if (resumeAfterRemove)
		{
		ResumeDownloadsL();
		}
	else
		{
		NotifyShowDownloadUpdatedL(-1, -1);
		NotifyDownloadQueueUpdatedL();
		}
	
	DownloadNextShowL();
	DP("CShowEngine::RemoveDownloadL END");
	}

void CShowEngine::Connected(CHttpClient* /*aClient*/)
	{

	}

void CShowEngine::Progress(CHttpClient* /*aHttpClient */, TInt aBytes,
		TInt aTotalBytes)
	{	
	iShowDownloading->SetShowSize(aTotalBytes);
	TRAP_IGNORE(NotifyShowDownloadUpdatedL(aBytes, aTotalBytes));
	}

void CShowEngine::Disconnected(CHttpClient* /*aClient */)
	{
	}

void CShowEngine::DownloadInfo(CHttpClient* aHttpClient, TInt aTotalBytes)
	{
	DP1("CShowEngine::DownloadInfo, About to download %d bytes", aTotalBytes);
	}

void CShowEngine::GetShowL(CShowInfo *info)
	{
	CFeedInfo *feedInfo = iPodcastModel.FeedEngine().GetFeedInfoByUid(
			info->FeedUid());
	
	TFileName filePath;
	filePath.Copy(iPodcastModel.SettingsEngine().BaseDir());
	
	TFileName relPath;
	relPath.Copy(feedInfo->Title());
	relPath.Append('\\');

	TFileName fileName;
	PodcastUtils::FileNameFromUrl(info->Url(), fileName);
	relPath.Append(fileName);
	PodcastUtils::EnsureProperPathName(relPath);

	// complete file path is base dir + rel path
	filePath.Append(relPath);
	info->SetFileNameL(filePath);

	User::LeaveIfError(iShowClient->GetL(info->Url(), filePath));
	}

EXPORT_C void CShowEngine::AddShowL(const CShowInfo& aItem)
	{
	DP1("CShowEngine::AddShowL, title=%S", &aItem.Title());
	CShowInfo *showInfo = DBGetShowByUidL(aItem.Uid());

	if (showInfo == NULL)
		{
		DBAddShowL(aItem);
		}
	else
		{
		delete showInfo;	
		User::Leave(KErrAlreadyExists);
		}	
	}

EXPORT_C void CShowEngine::AddObserver(MShowEngineObserver *observer)
	{
	iObservers.Append(observer);
	}

EXPORT_C void CShowEngine::RemoveObserver(MShowEngineObserver *observer)
	{
	TInt index = iObservers.Find(observer);

	if (index > KErrNotFound)
		{
		iObservers.Remove(index);
		}
	}

void CShowEngine::AddShowToMpxCollection(CShowInfo &aShowInfo)
	{
#ifdef ENABLE_MPX_INTEGRATION
	if (!iCollectionHelper)
		iCollectionHelper = CMPXCollectionHelperFactory::NewCollectionHelperL();
	
	// if this leaves, not much we can do anyway
	TRAP_IGNORE(iCollectionHelper->AddL(aShowInfo.FileName(), this));
#endif
	}

void CShowEngine::CompleteL(CHttpClient* /*aHttpClient*/, TInt aError)
	{
	if (iShowDownloading != NULL)
		{
		DP2("CShowEngine::CompleteL file=%S, aError=%d", &iShowDownloading->FileName(), aError);		
		if(aError != KErrCouldNotConnect)
			{
			if(aError == KErrDisconnected && iPodcastModel.SettingsEngine().DownloadSuspended())
				{
				// no error if disconnect happened because of suspended downloading
				}
			else
				{
				iShowDownloading->SetLastError(aError);
				}

			if (aError == KErrNone)
				{
				TBuf<KMimeBufLength> mimeType;
				GetMimeType(iShowDownloading->FileName(), mimeType);
				_LIT(KMimeAudio,"audio");
				_LIT(KMimeVideo,"video");
				if (mimeType.Left(5) == KMimeAudio)
					{
					iShowDownloading->SetShowType(EAudioPodcast);
					}
				else if (mimeType.Left(5) == KMimeVideo)
					{
					iShowDownloading->SetShowType(EVideoPodcast);
					}

				iShowDownloading->SetDownloadState(EDownloaded);
				DBUpdateShowL(*iShowDownloading);
				DBRemoveDownloadL(iShowDownloading->Uid());
				AddShowToMpxCollection(*iShowDownloading);				
				NotifyShowFinishedL(aError);
				iDownloadErrors = 0;
				delete iShowDownloading;
				iShowDownloading = NULL;
				}
			else
				{
				// 400 and 500 series errors are serious errors on which probably another download will fail
				if(aError >= HTTPStatus::EBadRequest && aError <= HTTPStatus::EBadRequest+200)
					{
					iShowDownloading->SetDownloadState(EFailedDownload);
					DBUpdateShowL(*iShowDownloading);
					DBRemoveDownloadL(iShowDownloading->Uid());
					NotifyShowFinishedL(aError);

					delete iShowDownloading;
					iShowDownloading = NULL;
					}
				else if (aError == KErrDiskFull)
					{
					// stop downloading immediately if disk is full
					iShowDownloading->SetDownloadState(EQueued);
					DBUpdateShowL(*iShowDownloading);
					iDownloadErrors = KMaxDownloadErrors;
					}
				else // other kind of error, missing network etc, reque this show
					{
					iShowDownloading->SetDownloadState(EQueued);
					DBUpdateShowL(*iShowDownloading);
					}

				NotifyDownloadQueueUpdatedL();
				iDownloadErrors++;
				if (iDownloadErrors >= KMaxDownloadErrors)
					{
					DP("Too many downloading errors, suspending downloads");
					iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
					NotifyShowFinishedL(aError);
					}
				}
			DownloadNextShowL();
			}		

		else
			{
			// Connection error
			if(iShowDownloading)
				{
				iShowDownloading->SetDownloadState(EQueued);
				DBUpdateShowL(*iShowDownloading);
				}
			iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
			NotifyShowFinishedL(aError);
			}
		}
	}

EXPORT_C CShowInfo* CShowEngine::ShowDownloading()
	{
	return iShowDownloading;
	}

EXPORT_C CShowInfo* CShowEngine::GetShowByUidL(TUint aShowUid)
	{
	return DBGetShowByUidL(aShowUid);
	}
CShowInfo* CShowEngine::DBGetShowByUidL(TUint aUid)
	{
	DP("CShowEngine::DBGetShowByUid");
	CShowInfo *showInfo = NULL;
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype from shows where uid=%u");
	iSqlBuffer.Format(KSqlStatement, aUid);

	sqlite3_stmt *st;

	//DP1("SQL: %S", &iSqlBuffer.Left(KSqlDPLen));

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		if (rc == SQLITE_ROW)
			{
			showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			CleanupStack::Pop(showInfo);
			}
		CleanupStack::PopAndDestroy();//st
		}

	return showInfo;
	}

EXPORT_C CShowInfo* CShowEngine::DBGetShowByFileNameL(TFileName aFileName)
	{
	DP("CShowEngine::DBGetShowByUid");
	CShowInfo *showInfo = NULL;
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype from shows where filename=\"%S\"");
	iSqlBuffer.Format(KSqlStatement, &aFileName);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		if (rc == SQLITE_ROW)
			{
			showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			CleanupStack::Pop(showInfo);
			}
		CleanupStack::PopAndDestroy();//st
		}

	return showInfo;
	}

void CShowEngine::DBGetAllShowsL(RShowInfoArray& aShowArray)
	{
	DP("CShowEngine::DBGetAllShows");
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype from shows");
	iSqlBuffer.Format(KSqlStatement);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		while (rc == SQLITE_ROW)
			{
			CShowInfo* showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			aShowArray.Append(showInfo);
			CleanupStack::Pop(showInfo);
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st
		}

	}

void CShowEngine::DBGetAllDownloadsL(RShowInfoArray& aShowArray)
	{
	DP("CShowEngine::DBGetAllDownloads");
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, shows.uid, showsize, trackno, pubdate, showtype, lasterror from downloads, shows where downloads.uid=shows.uid");
	iSqlBuffer.Format(KSqlStatement);

#ifndef DONT_SORT_SQL
	_LIT(KSqlSort, " order by dl_index");
	iSqlBuffer.Append(KSqlSort);
#endif
	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		while (rc == SQLITE_ROW)
			{
			CShowInfo* showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			aShowArray.Append(showInfo);
			CleanupStack::Pop(showInfo);
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	// delete downloads that don't have a show
	
	_LIT(KSqlStatement2, "delete from downloads where uid not in (select downloads.uid from shows, downloads where shows.uid=downloads.uid)");
	iSqlBuffer.Format(KSqlStatement2);

	rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,	&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

CShowInfo* CShowEngine::DBGetNextDownloadL()
	{
	DP("CShowEngine::DBGetNextDownload");
	CShowInfo *showInfo = NULL;
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, shows.uid, showsize, trackno, pubdate, showtype, lasterror from downloads, shows where downloads.uid=shows.uid");
	iSqlBuffer.Format(KSqlStatement);

#ifdef DONT_SORT_SQL
	_LIT(KSqlSort, " limit 1");
	iSqlBuffer.Append(KSqlSort);
#else
	_LIT(KSqlNoSort, " order by dl_index limit 1");
	iSqlBuffer.Append(KSqlNoSort);
#endif

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		if (rc == SQLITE_ROW)
			{
			showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			CleanupStack::Pop(showInfo);
			}
		else
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy();//st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	return showInfo;
	}

void CShowEngine::DBGetShowsByFeedL(RShowInfoArray& aShowArray, TUint aFeedUid)
	{
	DP1("CShowEngine::DBGetShowsByFeed BEGIN, feedUid=%u", aFeedUid);
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype, lasterror from shows where feeduid=%u");
	iSqlBuffer.Format(KSqlStatement, aFeedUid);

#ifndef DONT_SORT_SQL	
	_LIT(KSqlOrderByDate, " order by pubdate desc");
	iSqlBuffer.Append(KSqlOrderByDate);
#endif

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		while (rc == SQLITE_ROW)
			{
			CShowInfo* showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			aShowArray.Append(showInfo);
			CleanupStack::Pop(showInfo);
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	DP("CShowEngine::DBGetShowsByFeed END");
	}

TUint CShowEngine::DBGetDownloadsCountL()
	{
	DP("CShowEngine::DBGetDownloadsCount");

	_LIT(KSqlStatement, "select count(*) from downloads");
	iSqlBuffer.Format(KSqlStatement);

	sqlite3_stmt *st;
	TUint count = 0;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);

		if (rc == SQLITE_ROW)
			{
			count = sqlite3_column_int(st, 0);
			}
		else
			{
			User::Leave(KErrUnknown);
			}
		
		CleanupStack::PopAndDestroy(); //st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	return count;
	}

void CShowEngine::DBGetDownloadedShowsL(RShowInfoArray& aShowArray)
	{
	DP("CShowEngine::DBGetDownloadedShows");
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype, lasterror from shows where downloadstate=%u");
	iSqlBuffer.Format(KSqlStatement, EDownloaded);

#ifndef DONT_SORT_SQL
	_LIT(KSqlSort, " order by title");
	iSqlBuffer.Append(KSqlSort);
#endif	

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		while (rc == SQLITE_ROW)
			{
			CShowInfo* showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			aShowArray.Append(showInfo);
			CleanupStack::Pop(showInfo);
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBGetNewShowsL(RShowInfoArray& aShowArray)
	{
	DP("CShowEngine::DBGetNewShows");
	_LIT(KSqlStatement, "select url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype, lasterror from shows where playstate=%u");
	iSqlBuffer.Format(KSqlStatement, ENeverPlayed);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		while (rc == SQLITE_ROW)
			{
			CShowInfo* showInfo = CShowInfo::NewLC();
			DBFillShowInfoFromStmtL(st, showInfo);
			aShowArray.Append(showInfo);
			CleanupStack::Pop(showInfo);
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBDeleteOldShowsByFeedL(TUint aFeedUid)
	{
	DP("CShowEngine::DBDeleteOldShows");
	
	// what we do:
	// 1. sort shows by pubdate
	// 2. select the first MaxListItems shows
	// 3. delete the rest if downloadstate is ENotDownloaded
	
	_LIT(KSqlStatement,"delete from shows where feeduid=%u and downloadstate=0 and uid not in (select uid from shows where feeduid=%u order by pubdate desc limit %u)");
	iSqlBuffer.Format(KSqlStatement, aFeedUid, aFeedUid, iPodcastModel.SettingsEngine().MaxListItems());

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		sqlite3_finalize(st);
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	
	_LIT(KSqlStatement2, "delete from downloads where uid not in (select downloads.uid from shows, downloads where shows.uid=downloads.uid)");
	iSqlBuffer.Format(KSqlStatement2);

	rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,	&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		sqlite3_finalize(st);
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBFillShowInfoFromStmtL(sqlite3_stmt *st, CShowInfo* showInfo)
	{
	const void *urlz = sqlite3_column_text16(st, 0);
	TPtrC16 url((const TUint16*) urlz);
	showInfo->SetUrlL(url);

	const void *titlez = sqlite3_column_text16(st, 1);
	TPtrC16 title((const TUint16*) titlez);
	showInfo->SetTitleL(title);

	const void *descz = sqlite3_column_text16(st, 2);
	TPtrC16 desc((const TUint16*) descz);
	showInfo->SetDescriptionL(desc);

	const void *filez = sqlite3_column_text16(st, 3);
	TPtrC16 file((const TUint16*) filez);
	showInfo->SetFileNameL(file);

	sqlite3_int64 pos = sqlite3_column_int64(st, 4);
	TTimeIntervalMicroSeconds position(pos);
	showInfo->SetPosition(position);

	TUint playtime = sqlite3_column_int(st, 5);
	showInfo->SetPlayTime(playtime);

	TUint playstate = sqlite3_column_int(st, 6);
	showInfo->SetPlayState((TPlayState) playstate);

	TUint downloadstate = sqlite3_column_int(st, 7);
	showInfo->SetDownloadState((TDownloadState) downloadstate);

	TUint feeduid = sqlite3_column_int(st, 8);
	showInfo->SetFeedUid(feeduid);

	TUint uid = sqlite3_column_int(st, 9);
	showInfo->SetUid(uid);

	TUint showsize = sqlite3_column_int(st, 10);
	showInfo->SetShowSize(showsize);

	TUint trackno = sqlite3_column_int(st, 11);
	showInfo->SetTrackNo((TShowType) trackno);

	sqlite3_int64 pubdate = sqlite3_column_int64(st, 12);
	TTime timepubdate(pubdate);
	showInfo->SetPubDate(timepubdate);

	TUint showtype = sqlite3_column_int(st, 13);
	showInfo->SetShowType((TShowType) showtype);
	
	TInt lasterror = sqlite3_column_int(st, 14);
	showInfo->SetLastError(lasterror);
	}

void CShowEngine::DBAddShowL(const CShowInfo& aItem)
	{
	DP2("CShowEngine::DBAddShow, title=%S, URL=%S", &aItem.Title(), &aItem.Url());

	HBufC* titleBuf = HBufC::NewLC(KMaxLineLength);
	TPtr titlePtr(titleBuf->Des());
	titlePtr.Copy(aItem.Title());
	PodcastUtils::SQLEncode(titlePtr);

	HBufC* descBuf = HBufC::NewLC(KMaxLineLength);
	TPtr descPtr(descBuf->Des());
	descPtr.Copy(aItem.Description());
	PodcastUtils::SQLEncode(descPtr);
	
	_LIT(KSqlStatement, "insert into shows (url, title, description, filename, position, playtime, playstate, downloadstate, feeduid, uid, showsize, trackno, pubdate, showtype) values (\"%S\",\"%S\", \"%S\", \"%S\", \"%Lu\", \"%u\", \"%u\", \"%u\", \"%u\", \"%u\", \"%u\", \"%u\", \"%Lu\", \"%d\")");
	
	iSqlBuffer.Format(KSqlStatement, &aItem.Url(), &titlePtr, &descPtr,
			&aItem.FileName(), aItem.Position().Int64(), aItem.PlayTime(),
			aItem.PlayState(), aItem.DownloadState(), aItem.FeedUid(),
			aItem.Uid(), aItem.ShowSize(), aItem.TrackNo(),
			aItem.PubDate().Int64(), aItem.ShowType());

	CleanupStack::PopAndDestroy(descBuf);
	CleanupStack::PopAndDestroy(titleBuf);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);
	
	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrAlreadyExists);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBAddDownloadL(TUint aUid)
	{
	DP1("CShowEngine::DBAddDownload, aUid=%u", aUid);

	_LIT(KSqlStatement, "insert into downloads (uid) values (%u)");
	iSqlBuffer.Format(KSqlStatement, aUid);
	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBUpdateShowL(CShowInfo& aItem)
	{
	DP1("CShowEngine::DBUpdateShow, title='%S'", &aItem.Title());

	HBufC* titleBuf = HBufC::NewLC(KMaxLineLength);
	TPtr titlePtr(titleBuf->Des());
	titlePtr.Copy(aItem.Title());
	PodcastUtils::SQLEncode(titlePtr);

	HBufC* descBuf = HBufC::NewLC(KMaxLineLength);
	TPtr descPtr(descBuf->Des());
	descPtr.Copy(aItem.Description());
	PodcastUtils::SQLEncode(descPtr);

	_LIT(KSqlStatement, "update shows set url=\"%S\", title=\"%S\", description=\"%S\", filename=\"%S\", position=\"%Lu\", playtime=\"%u\", playstate=\"%u\", downloadstate=\"%u\", feeduid=\"%u\", showsize=\"%u\", trackno=\"%u\",pubdate=\"%Lu\", showtype=\"%d\", lasterror=\"%d\" where uid=\"%u\"");
	iSqlBuffer.Format(KSqlStatement, &aItem.Url(), &titlePtr, &descPtr,
			&aItem.FileName(), aItem.Position().Int64(), aItem.PlayTime(),
			aItem.PlayState(), aItem.DownloadState(), aItem.FeedUid(),
			aItem.ShowSize(), aItem.TrackNo(), aItem.PubDate().Int64(),
			aItem.ShowType(), aItem.LastError(), aItem.Uid());

	CleanupStack::PopAndDestroy(descBuf);
	CleanupStack::PopAndDestroy(titleBuf);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);		

		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBDeleteShowL(TUint aUid)
	{
	DP("CShowEngine::DBDeleteShow");

	_LIT(KSqlStatement, "delete from shows where uid=%u");
	iSqlBuffer.Format(KSqlStatement, aUid);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);

		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBDeleteAllShowsByFeedL(TUint aFeedUid)
	{
	DP("CShowEngine::DBDeleteAllShowsByFeed");

	_LIT(KSqlStatement, "delete from shows where feeduid=%u");
	iSqlBuffer.Format(KSqlStatement, aFeedUid);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);

		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBRemoveAllDownloadsL()
	{
	DP("CShowEngine::DBRemoveAllDownloads");

	_LIT(KSqlStatement, "delete from downloads");
	iSqlBuffer.Format(KSqlStatement);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		rc = sqlite3_step(st);
		sqlite3_finalize(st);
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	_LIT(KSqlStatement2, "update shows set downloadstate=0 where downloadstate=1");
	iSqlBuffer.Format(KSqlStatement2);

	rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1, &st,
			(const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	}

void CShowEngine::DBRemoveDownloadL(TUint aUid)
	{
	DP("CShowEngine::DBRemoveDownload");

	_LIT(KSqlStatement, "delete from downloads where uid=%u");
	iSqlBuffer.Format(KSqlStatement, aUid);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CShowEngine::DBSwapDownloadsL(TDownload aFirstDL, TDownload aSecondDL)
	{
	DP("CShowEngine::DBSwapDownloadsL");
	_LIT(KSqlStatement, "update downloads set uid=%u where dl_index=%d");
	
	iSqlBuffer.Format(KSqlStatement, aFirstDL.iUid, aSecondDL.iIndex);
	sqlite3_stmt *st;
	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);
	
	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	
	iSqlBuffer.Format(KSqlStatement, aSecondDL.iUid, aFirstDL.iIndex);
	
	rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);
	
	if (rc == SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrUnknown);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

EXPORT_C CShowInfo* CShowEngine::GetNextShowByTrackL(CShowInfo* aShowInfo)
	{
	CShowInfo* nextShow = NULL;
	RShowInfoArray array;
	DBGetShowsByFeedL(array, aShowInfo->FeedUid());
	TUint diff = KMaxTInt;
	for (TInt loop = 0; loop < array.Count(); loop++)
		{
		if (aShowInfo->TrackNo() < array[loop]->TrackNo())
			{
			if ((array[loop]->TrackNo() - aShowInfo->TrackNo()) < diff)
				{
				diff = array[loop]->TrackNo() - aShowInfo->TrackNo();
				nextShow = array[loop];
				}
			}
		}
	array.ResetAndDestroy();
	return nextShow;
	}

TBool CShowEngine::CompareShowsByUid(const CShowInfo &a, const CShowInfo &b)
	{
	return a.Uid() == b.Uid();
	}

TInt CShowEngine::CompareShowsByDate(const CShowInfo &a, const CShowInfo &b)
	{
	if (a.PubDate() > b.PubDate())
		{
		//		DP2("Sorting %S less than %S", &a.iTitle, &b.iTitle);
		return -1;
		}
	else if (a.PubDate() == b.PubDate())
		{
		//		DP2("Sorting %S equal to %S", &a.iTitle, &b.iTitle);
		return 0;
		}
	else
		{
		//		DP2("Sorting %S greater than %S", &a.iTitle, &b.iTitle);
		return 1;
		}
	}

TInt CShowEngine::CompareShowsByTrackNo(const CShowInfo &a, const CShowInfo &b)
	{
	if (a.TrackNo() < b.TrackNo())
		{
		return -1;
		}
	else if (a.TrackNo() == b.TrackNo())
		{
		return 0;
		}
	else
		{
		return 1;
		}
	}

TInt CShowEngine::CompareShowsByTitle(const CShowInfo &a, const CShowInfo &b)
	{
	if (a.Title() < b.Title())
		{
		//		DP2("Sorting %S less than %S", &a.iTitle, &b.iTitle);
		return -1;
		}
	else if (a.Title() == b.Title())
		{
		//		DP2("Sorting %S equal to %S", &a.iTitle, &b.iTitle);
		return 0;
		}
	else
		{
		//		DP2("Sorting %S greater than %S", &a.iTitle, &b.iTitle);
		return 1;
		}
	}

EXPORT_C void CShowEngine::DeletePlayedShowsL(RShowInfoArray &aShowInfoArray)
	{
	for (TInt i = 0; i < aShowInfoArray.Count(); i++)
		{
		if (aShowInfoArray[i]->PlayState() == EPlayed
				&& aShowInfoArray[i]->FileName().Length() > 0)
			{
			BaflUtils::DeleteFile(iPodcastModel.FsSession(), aShowInfoArray[i]->FileName());
			aShowInfoArray[i]->SetDownloadState(ENotDownloaded);
			DBUpdateShowL(*aShowInfoArray[i]);
			}
		}
	}

EXPORT_C void CShowEngine::DeleteAllShowsByFeedL(TUint aFeedUid, TBool aDeleteFiles)
	{
	RShowInfoArray array;
	DBGetShowsByFeedL(array, aFeedUid);

	const TInt count = array.Count();

	for (TInt i = count - 1; i >= 0; i--)
		{
		if (iShowDownloading && iShowDownloading->Uid() == array[i]->Uid())
			{
			// trying to delete the active download
			RemoveDownloadL(iShowDownloading->Uid());
			}
		
		// delete downloaded file
		if (array[i]->FileName().Length() > 0)
			{
			if (aDeleteFiles)
				{
				BaflUtils::DeleteFile(iPodcastModel.FsSession(), array[i]->FileName());
				}
			}
		}
	array.ResetAndDestroy();
	
	// delete all shows from DB
	DBDeleteAllShowsByFeedL(aFeedUid);

	// this will clear out deleted shows from the download queue
	DBGetAllDownloadsL(array);
	array.ResetAndDestroy();

	NotifyDownloadQueueUpdatedL();
	}

EXPORT_C void CShowEngine::DeleteOldShowsByFeedL(TUint aFeedUid)
	{
	DBDeleteOldShowsByFeedL(aFeedUid);
	}

EXPORT_C void CShowEngine::DeleteShowL(TUint aShowUid, TBool aRemoveFile)
	{

	CShowInfo *info = DBGetShowByUidL(aShowUid);

	if (info != NULL)
		{
		if (info->FileName().Length() > 0 && aRemoveFile)
			{
			BaflUtils::DeleteFile(iPodcastModel.FsSession(), info->FileName());
			}
		
		info->SetDownloadState(ENotDownloaded);
		DBUpdateShowL(*info);
		delete info;
		}
	}

EXPORT_C void CShowEngine::GetShowsByFeedL(RShowInfoArray& aShowArray, TUint aFeedUid)
	{
	DP("CShowEngine::GetShowsByFeed");
	DBGetShowsByFeedL(aShowArray, aFeedUid);
	}

EXPORT_C void CShowEngine::GetAllShowsL(RShowInfoArray &aArray)
	{
	DP("CShowEngine::GetAllShows");
	DBGetAllShowsL(aArray);
	}

EXPORT_C void CShowEngine::GetShowsDownloadedL(RShowInfoArray &aArray)
	{
	DP("CShowEngine::GetShowsDownloaded");
	DBGetDownloadedShowsL(aArray);
	}

EXPORT_C void CShowEngine::GetNewShowsL(RShowInfoArray &aArray)
	{
	DP("CShowEngine::GetNewShows");
	DBGetNewShowsL(aArray);
	}

EXPORT_C void CShowEngine::GetShowsDownloadingL(RShowInfoArray &aArray)
	{
	DP("CShowEngine::GetShowsDownloading");
	DBGetAllDownloadsL(aArray);
	}

EXPORT_C TInt CShowEngine::GetNumDownloadingShows()
	{
	TUint count = 0;
	TRAP_IGNORE(count = DBGetDownloadsCountL());
		
	return (const TInt) count;
	}

EXPORT_C void CShowEngine::AddDownloadL(CShowInfo& aInfo)
	{
	aInfo.SetDownloadState(EQueued);
	DBUpdateShowL(aInfo);
	DBAddDownloadL(aInfo.Uid());
	DownloadNextShowL();
	}

void CShowEngine::DownloadNextShowL()
	{
	DP("CShowEngine::DownloadNextShowL BEGIN");
	// Check if we have anything in the download queue
	const TInt count = DBGetDownloadsCountL();
	DP("CShowEngine::DownloadNextShow\tTrying to start new download");DP1("CShowEngine::DownloadNextShow\tShows in download queue %d", count);

	if (count > 0)
		{
		if (iPodcastModel.SettingsEngine().DownloadSuspended())
			{
			DP("CShowEngine::DownloadNextShow\tDownload process is suspended, ABORTING");
			// Inform the observers
			NotifyDownloadQueueUpdatedL();
			return;
			}
		else if (iShowClient->IsActive())
			{
			DP("CShowEngine::DownloadNextShow\tDownload process is already active.");
			// Inform the observers
			NotifyDownloadQueueUpdatedL();
			return;
			}
		else
			{
			if (iShowDownloading) {
				delete iShowDownloading;
			}
			
			// Start the download
			iShowDownloading = DBGetNextDownloadL();
			
			while(iShowDownloading != NULL)
				{
				DP1("CShowEngine::DownloadNextShow\tDownloading: %S", &(iShowDownloading->Title()));
				iShowDownloading->SetDownloadState(EDownloading);
				iShowDownloading->SetLastError(KErrNone);
				DBUpdateShowL(*iShowDownloading);
				// Inform the observers
				// important to do this after we change download state
				NotifyDownloadQueueUpdatedL();

				TRAPD(error,GetShowL(iShowDownloading));
				if (error == KErrNone)
					{
					break;
					}
				else
					{
					iShowDownloading->SetDownloadState(EFailedDownload);
					DBRemoveDownloadL(iShowDownloading->Uid());
					DBUpdateShowL(*iShowDownloading);
					CleanupStack::PopAndDestroy(iShowDownloading);
					
					iShowDownloading = DBGetNextDownloadL();
					if(iShowDownloading == NULL)
						{
						iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
						}
					}				
				}
			}
		}
	else
		{
		// Inform the observers
		NotifyDownloadQueueUpdatedL();
		iShowDownloading = NULL;DP("CShowEngine::DownloadNextShow\tNothing to download");
		}
	DP("CShowEngine::DownloadNextShowL END");
	}

void CShowEngine::NotifyDownloadQueueUpdatedL()
	{
	const TInt count = iObservers.Count();
	for (TInt i = 0; i < count; i++)
		{
		iObservers[i]->DownloadQueueUpdatedL(1, DBGetDownloadsCountL() - 1);
		}
	}

void CShowEngine::NotifyShowDownloadUpdatedL(TInt aBytesOfCurrentDownload, TInt aBytesTotal)
	{
	const TInt count = iObservers.Count();
	for (TInt i = 0; i < count; i++)
		{
			iObservers[i]->ShowDownloadUpdatedL(aBytesOfCurrentDownload, aBytesTotal);
		}
	}

void CShowEngine::NotifyShowFinishedL(TInt aError)
	{
	const TInt count = iObservers.Count();
		for (TInt i = 0; i < count; i++)
			{
				iObservers[i]->ShowDownloadFinishedL(iShowDownloading?iShowDownloading->Uid():0, aError);
			}
	}

EXPORT_C void CShowEngine::NotifyShowListUpdatedL()
	{
	for (TInt i = 0; i < iObservers.Count(); i++)
		{
		iObservers[i]->ShowListUpdatedL();
		}
	}

void CShowEngine::ReadMetaDataL(CShowInfo& aShowInfo)
	{
	//DP1("Read %S", &(aShowInfo->Title()));
	DBUpdateShowL(aShowInfo);
	}

void CShowEngine::ReadMetaDataCompleteL()
	{
	NotifyShowListUpdatedL();
	MetaDataReader().SetIgnoreTrackNo(EFalse);
	}

EXPORT_C void CShowEngine::UpdateShowL(CShowInfo& aInfo)
	{
	DBUpdateShowL(aInfo);
	}

EXPORT_C CMetaDataReader& CShowEngine::MetaDataReader()
	{
	return *iMetaDataReader;
	}

void CShowEngine::FileError(TUint /*aError*/)
	{
	iDownloadErrors = KMaxDownloadErrors;
	}

EXPORT_C void CShowEngine::CheckForDeletedShows(TUint aFeedUid)
	{
	RShowInfoArray shows;
	
	TRAPD(err, DBGetShowsByFeedL(shows, aFeedUid));
	
	if (err != KErrNone)
		{
		// probably a catastrophic error, but it doesn't
		// matter for this method
		return;
		}
	
	for (int i=0;i<shows.Count();i++)
		{
		if (shows[i]->DownloadState() == EDownloaded && shows[i]->FileName() != KNullDesC)
			{
			if(!BaflUtils::FileExists(iPodcastModel.FsSession(),shows[i]->FileName()))
				{
				// file doesn't exist anymore, assume it was deleted from outside
				DP1("Show %S does not exist on disk, flagging as non downloaded", &shows[i]->FileName());
				shows[i]->SetDownloadState(ENotDownloaded);
				shows[i]->SetPlayState(EPlayed);
				TRAP_IGNORE(DBUpdateShowL(*shows[i]));
				}
			}
		}
	}

EXPORT_C void CShowEngine::MoveDownloadUpL(TUint aUid)
	{
	DP("CShowEngine::MoveDownLoadUpL");
	_LIT(KSqlStatement, "select * from downloads");
	iSqlBuffer.Format(KSqlStatement);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		RArray<TDownload> downloads;
		CleanupClosePushL(downloads);
	
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		
		TInt selectedIdx = -1;
		while (rc == SQLITE_ROW && selectedIdx == -1)
			{
			TDownload download;
			
			download.iIndex = sqlite3_column_int(st, 0);
			download.iUid = sqlite3_column_int(st, 1);
			
			downloads.Append(download);
			
			if (download.iUid == aUid)
				{
				selectedIdx = downloads.Count()-1;
				}
			
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st, downloads
		
		// If the selected download was found in the database
		if (selectedIdx != -1)
			{
			// Swap the specified download with the one above it
			TDownload selectedDownload = downloads[selectedIdx];
			TDownload previousDownload = downloads[selectedIdx - 1];
			
			// SQL - Update index (with index of selected download) where uid is of previous download
			// and update index (with index of previous download) where uid if of selected download
			DBSwapDownloadsL(selectedDownload, previousDownload);
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		
		CleanupStack::PopAndDestroy(); // downloads
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

EXPORT_C void CShowEngine::MoveDownloadDownL(TUint aUid)
	{
	DP("CShowEngine::MoveDownLoadDownL");
	
	// An upside-down list will result in the download moving down
	_LIT(KSqlStatement, "select * from downloads order by dl_index desc");
	iSqlBuffer.Format(KSqlStatement);

	sqlite3_stmt *st;

	int rc = sqlite3_prepare16_v2(&iDB, (const void*) iSqlBuffer.PtrZ(), -1,
			&st, (const void**) NULL);

	if (rc == SQLITE_OK)
		{
		RArray<TDownload> downloads;
		CleanupClosePushL(downloads);
	
		rc = sqlite3_step(st);
		Cleanup_sqlite3_finalize_PushL(st);
		
		TInt selectedIdx = -1;
		while (rc == SQLITE_ROW && selectedIdx == -1)
			{
			TDownload download;
			
			download.iIndex = sqlite3_column_int(st, 0);
			download.iUid = sqlite3_column_int(st, 1);
			
			downloads.Append(download);
			
			if (download.iUid == aUid)
				{
				selectedIdx = downloads.Count()-1;
				}
			
			rc = sqlite3_step(st);
			}
		CleanupStack::PopAndDestroy();//st, downloads
		
		// If the selected download was found in the database
		if (selectedIdx != -1)
			{
			// Swap the specified download with the one above it
			TDownload selectedDownload = downloads[selectedIdx];
			TDownload previousDownload = downloads[selectedIdx - 1];
			
			// SQL - Update index (with index of selected download) where uid is of previous download
			// and update index (with index of previous download) where uid if of selected download
			DBSwapDownloadsL(selectedDownload, previousDownload);
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		
		CleanupStack::PopAndDestroy(); // downloads
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}
