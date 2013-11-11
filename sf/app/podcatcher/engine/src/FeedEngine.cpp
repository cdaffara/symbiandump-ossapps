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

#include "FeedEngine.h"
#include <f32file.h>
#include <bautils.h>
#include <s32file.h>
#include "SettingsEngine.h"
#include "ShowEngine.h"
#include <e32hashtab.h>
#include "OpmlParser.h"
#include "PodcastUtils.h"
#include <utf.h>
#include "Podcatcher.pan"
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>


_LIT(KFeedParseStorePath, "feeds\\");


CFeedEngine* CFeedEngine::NewL(CPodcastModel& aPodcastModel)
	{
	CFeedEngine* self = new (ELeave) CFeedEngine(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CFeedEngine::ConstructL()
	{
	iParser = new (ELeave) CFeedParser(*this, iPodcastModel.FsSession());
	
	iFeedClient = CHttpClient::NewL(iPodcastModel, *this);
	iFeedTimer.ConstructL();
	
	TInt err = KErrNone;
	TInt feedCount = 0;
	
	TRAP(err, feedCount = DBGetFeedCountL());
    if (err == KErrNone && feedCount > 0)
    	{
		DP("Loading feeds from DB");
		TRAP(err, DBLoadFeedsL());
    	}		

    
	if (err != KErrNone || iPodcastModel.IsFirstStartup()) {
		TFileName defaultFile = iPodcastModel.SettingsEngine().DefaultFeedsFileName();
		DP1("Loading default feeds from %S", &defaultFile);
		if (BaflUtils::FileExists(iPodcastModel.FsSession(), defaultFile)) {
			ImportFeedsL(defaultFile);
		}
	} 
	
	// clean out feeds temp directory
	TFileName feedTempPath;
	feedTempPath.Copy (iPodcastModel.SettingsEngine().PrivatePath ());
	feedTempPath.Append(KFeedParseStorePath);
	feedTempPath.Append(_L("*"));

	BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(), feedTempPath);
	BaflUtils::DeleteFile(iPodcastModel.FsSession(),feedTempPath);
    
    TFileName importFile = iPodcastModel.SettingsEngine().ImportFeedsFileName();
    if (BaflUtils::FileExists(iPodcastModel.FsSession(), importFile)) {
    	DP("Importing feeds");
    	TRAP_IGNORE(ImportFeedsL(importFile));
		}
    
    
    // offline profile support
    iRepository = CRepository::NewL( KCRUidProfileEngine );
    
	RunFeedTimer();
	}

CFeedEngine::CFeedEngine(CPodcastModel& aPodcastModel)
		: iClientState(EIdle),
		  iFeedTimer(this),
		  iPodcastModel(aPodcastModel),
		  iDB(*aPodcastModel.DB())
	{
	}

CFeedEngine::~CFeedEngine()
	{
	iObservers.Close();
	
	iFeedsUpdating.Close();
	iSortedFeeds.ResetAndDestroy();
	iSearchResults.ResetAndDestroy();
	
	delete iParser;
	delete iFeedClient;
	delete iOpmlParser;
	//
	delete iRepository;
	}

/**
 * Returns the current internal state of the feed engine4
 */
EXPORT_C TClientState CFeedEngine::ClientState()
	{
	return iClientState;
	}


/**
 * Returns the current updating client UID if clientstate is != ENotUpdateing
 * @return TUint
 */
EXPORT_C TUint CFeedEngine::ActiveClientUid()
	{
	if(iActiveFeed != NULL)
		{
		return iActiveFeed->Uid();
		}
	return 0;	
	}

void CFeedEngine::RunFeedTimer()
	{
	iFeedTimer.Cancel();

	if (iPodcastModel.SettingsEngine().UpdateAutomatically() != EAutoUpdateOff)
		{
		TInt interval = iPodcastModel.SettingsEngine().UpdateFeedInterval();
	
		if (interval != 0)
			{
			iFeedTimer.SetPeriod(interval);
			iFeedTimer.RunPeriodically();
			}
		}
	}

EXPORT_C void CFeedEngine::UpdateAllFeedsL(TBool aAutoUpdate)
	{
	if (iClientState != EIdle)
		{
		User::Leave(KErrInUse);
		}
	// check for offline profile, done by polling here, perhpas CenRep notifier woudl be better
	TInt currentProfileId = 0; // general profile
	TBool isOfflineProfile = EFalse;
	// Get ID of current profile
	if(KErrNone == iRepository->Get( KProEngActiveProfile, currentProfileId ) ) // better to assume online then leave if CRep fails
		{
		// Check value to determine the active profile if no error
		if ( 5 == currentProfileId)
			{
			// current profile is the offline profile
			isOfflineProfile = ETrue;
			}
	    }
	//
	iAutoUpdatedInitiator = aAutoUpdate;
	if ((iFeedsUpdating.Count() > 0) || (isOfflineProfile)) // cancel update if in offline mode
		{
		DP("Cancelling update");
		iFeedClient->Stop();
		iFeedsUpdating.Reset();
		return;
		}

	TInt cnt = iSortedFeeds.Count();
	for (int i=0;i<cnt;i++)
		{
		iFeedsUpdating.Append(iSortedFeeds[i]->Uid());
		}

	UpdateNextFeedL();
	}

EXPORT_C void CFeedEngine::CancelUpdateAllFeeds()
	{
	if(iClientState != EIdle)
		{
		iCancelRequested = ETrue;
		iFeedsUpdating.Reset();
		iFeedClient->Stop();
		}
	}

void CFeedEngine::UpdateNextFeedL()
	{
	DP1("UpdateNextFeed. %d feeds left to update", iFeedsUpdating.Count());
	
	if (iClientState != EIdle)
		{
		User::Leave(KErrInUse);
		}
	
	// reset active feed, will be set again in UpdateFeedL if needed
	iActiveFeed = NULL;
	
	if (iFeedsUpdating.Count() > 0)
		{
		CFeedInfo *info = GetFeedInfoByUid(iFeedsUpdating[0]);
		iFeedsUpdating.Remove(0);
		
		if (info == NULL)
			{
			UpdateNextFeedL();
			}
		else
			{
			TBool result = EFalse;
			//DP2("** UpdateNextFeed: %S, ID: %u", &(info->Url()), info->Uid());
			TRAPD(error, result = UpdateFeedL(info->Uid()));
			
			if (error != KErrNone || !result)
				{
				DP("Error while updating all feeds");
				for (TInt i=0;i<iObservers.Count();i++) 
					{
					TRAP_IGNORE(iObservers[i]->FeedUpdateAllCompleteL(iAutoUpdatedInitiator?MFeedEngineObserver::EFeedAutoUpdate:MFeedEngineObserver::EFeedManualUpdate));
					}
				}
			}
		}
	else
		{
		iClientState = EIdle;
		for (TInt i=0;i<iObservers.Count();i++) 
			{
			TRAP_IGNORE(iObservers[i]->FeedUpdateAllCompleteL(iAutoUpdatedInitiator?MFeedEngineObserver::EFeedAutoUpdate:MFeedEngineObserver::EFeedManualUpdate));
			}
		}
	}

EXPORT_C TBool CFeedEngine::UpdateFeedL(TUint aFeedUid)
	{
	DP("FeedEngine::UpdateFeedL BEGIN");
	
	if (iActiveFeed)
		{
		User::Leave(KErrInUse);
		}
	
	iActiveFeed = GetFeedInfoByUid(aFeedUid);
	
	iCancelRequested = EFalse;

	if (iActiveFeed->LastUpdated() == 0)
		{
		newFeed = ETrue;	
		}
	
	showsAdded = 0;
	
	iActiveFeed->SetLastError(KErrNone);
	DBUpdateFeedL(*iActiveFeed);
	
	iUpdatingFeedFileName.Copy (iPodcastModel.SettingsEngine().PrivatePath ());
	iUpdatingFeedFileName.Append(KFeedParseStorePath);
	BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(), iUpdatingFeedFileName);
	
	_LIT(KFileNameFormat, "%lu.xml");
	iUpdatingFeedFileName.AppendFormat(KFileNameFormat, aFeedUid);
	
	iClientState = EUpdatingFeed;
			
	for (TInt i=0;i<iObservers.Count();i++)
		{
		TRAP_IGNORE(iObservers[i]->FeedDownloadStartedL(iAutoUpdatedInitiator?MFeedEngineObserver::EFeedAutoUpdate:MFeedEngineObserver::EFeedManualUpdate, iActiveFeed->Uid()));
		}

	if(iFeedClient->GetL(iActiveFeed->Url(), iUpdatingFeedFileName, iPodcastModel.SettingsEngine().SpecificIAP()))
		{
		
		DP("FeedEngine::UpdateFeedL END, return ETrue");
		return ETrue;
		}
	else
		{
		DP("FeedEngine::UpdateFeedL END, return EFalse");
		return EFalse;
		}
	}

void CFeedEngine::NewShowL(CShowInfo& aItem)
	{
	HBufC* description = HBufC::NewLC(KMaxDescriptionLength);
	TPtr ptr(description->Des());
	ptr.Copy(aItem.Description());
	PodcastUtils::CleanHtmlL(ptr);

	aItem.SetDescriptionL(*description);
	CleanupStack::PopAndDestroy(description);

	if (newFeed) {
		// for new feeds, set all shows played
		aItem.SetPlayState(EPlayed);
		// except the first one
		if (showsAdded == 0) {
			aItem.SetPlayState(ENeverPlayed);	
		}
	}
	
	TRAPD(err, iPodcastModel.ShowEngine().AddShowL(aItem));

	if (err == KErrNone && aItem.PlayState() == ENeverPlayed && 
			iPodcastModel.SettingsEngine().DownloadAutomatically()) 
		{
		iPodcastModel.ShowEngine().AddDownloadL(aItem);
		}
	
	showsAdded++;
	}

void CFeedEngine::GetFeedImageL(CFeedInfo *aFeedInfo)
	{
	DP("GetFeedImage");

	TFileName filePath;
	filePath.Copy(iPodcastModel.SettingsEngine().BaseDir());
	
	// create relative file name
	TFileName relPath;
	relPath.Copy(aFeedInfo->Title());
	relPath.Append('\\');

	TFileName fileName;
	PodcastUtils::FileNameFromUrl(aFeedInfo->ImageUrl(), fileName);
	relPath.Append(fileName);
	PodcastUtils::EnsureProperPathName(relPath);
	
	// complete file path is base dir + rel path
	filePath.Append(relPath);
	aFeedInfo->SetImageFileNameL(filePath, NULL);

	if(iFeedClient->GetL(aFeedInfo->ImageUrl(), filePath, ETrue))
		{
			iClientState = EUpdatingImage;
		}
	}

EXPORT_C TBool CFeedEngine::AddFeedL(const CFeedInfo&aItem) 
	{
	DP2("CFeedEngine::AddFeed, title=%S, URL=%S", &aItem.Title(), &aItem.Url());
	for (TInt i=0;i<iSortedFeeds.Count();i++) 
		{
		if (iSortedFeeds[i]->Uid() == aItem.Uid()) 
			{
			DP1("Already have feed %S, discarding", &aItem.Url());			
			return EFalse;
			}
		}
	
	TLinearOrder<CFeedInfo> sortOrder( CFeedEngine::CompareFeedsByTitle);
	CFeedInfo* newItem = aItem.CopyL();
	CleanupStack::PushL(newItem);
	User::LeaveIfError(iSortedFeeds.InsertInOrder(newItem, sortOrder));
	CleanupStack::Pop(newItem);
	

	// Save the feeds into DB
	DBAddFeedL(aItem);
	return ETrue;
	}

void CFeedEngine::DBAddFeedL(const CFeedInfo& aItem)
	{
	DP2("CFeedEngine::DBAddFeed BEGIN, title=%S, URL=%S", &aItem.Title(), &aItem.Url());
	
	CFeedInfo *info;
	
	TRAPD(err, info = DBGetFeedInfoByUidL(aItem.Uid()));
	
	if (err == KErrNone && info) {
		delete info;
		User::Leave(KErrAlreadyExists);
	}

	HBufC* titleBuf = HBufC::NewLC(KMaxLineLength);
	TPtr titlePtr(titleBuf->Des());
	titlePtr.Copy(aItem.Title());
	PodcastUtils::SQLEncode(titlePtr);
	
	HBufC* descBuf = HBufC::NewLC(KMaxLineLength);
	TPtr descPtr(descBuf->Des());
	descPtr.Copy(aItem.Description());
	PodcastUtils::SQLEncode(descPtr);
	
	_LIT(KSqlStatement, "insert into feeds (url, title, description, imageurl, imagefile, link, built, lastupdated, uid, feedtype, customtitle, lasterror) values (\"%S\",\"%S\", \"%S\", \"%S\", \"%S\", \"%S\", \"%Ld\", \"%Ld\", \"%u\", \"%u\", \"%u\", \"%d\")");
	iSqlBuffer.Format(KSqlStatement,
			&aItem.Url(), titleBuf, descBuf, &aItem.ImageUrl(), &aItem.ImageFileName(), &aItem.Link(),
			aItem.BuildDate().Int64(), aItem.LastUpdated().Int64(), aItem.Uid(), EAudioPodcast, aItem.CustomTitle(), aItem.LastError());

	CleanupStack::PopAndDestroy(descBuf);
	CleanupStack::PopAndDestroy(titleBuf);
	
	sqlite3_stmt *st;
	 
	int rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st, (const void**) NULL);
	
	if (rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);

		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrCorrupt);
			}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	DP("CFeedEngine::DBAddFeed END");
	}

EXPORT_C void CFeedEngine::RemoveFeedL(TUint aUid) 
	{
	if (iActiveFeed && iActiveFeed->Uid() == aUid)
		{
		User::Leave(KErrInUse);
		}
	
	for (int i=0;i<iSortedFeeds.Count();i++) 
		{
		if (iSortedFeeds[i]->Uid() == aUid) 
			{
			iPodcastModel.ShowEngine().DeleteAllShowsByFeedL(aUid);

			CFeedInfo* feedToRemove = iSortedFeeds[i];
			
			//delete the image file if it exists
			if ((feedToRemove->ImageFileName().Length() >0)
					&& BaflUtils::FileExists(iPodcastModel.FsSession(), feedToRemove->ImageFileName()))
				{
				iPodcastModel.FsSession().Delete(feedToRemove->ImageFileName());
				}
				
			//delete the folder. It has the same name as the title.
			TFileName filePath;
			filePath.Copy(iPodcastModel.SettingsEngine().BaseDir());
			filePath.Append(feedToRemove->Title());
			filePath.Append('\\');
			iPodcastModel.FsSession().RmDir(filePath);

			iSortedFeeds.Remove(i);
			delete feedToRemove;
			
			DP("Removed feed from array");
			
			// now remove it from DB
			DBRemoveFeedL(aUid);
		}
	}
}


void CFeedEngine::DBRemoveFeedL(TUint aUid)
	{
	DP("CFeedEngine::DBRemoveFeed BEGIN");
	_LIT(KSqlStatement, "delete from feeds where uid=%u");
	iSqlBuffer.Format(KSqlStatement, aUid);

	sqlite3_stmt *st;
	 
	int rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st,	(const void**) NULL);
	
	if (rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);

		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrNotFound);
			}
		
		CleanupStack::PopAndDestroy(); //st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	DP("CFeedEngine::DBRemoveFeed END");
	}

void CFeedEngine::DBUpdateFeedL(const CFeedInfo &aItem)
	{
	DP2("CFeedEngine::DBUpdateFeed BEGIN, title=%S, URL=%S", &aItem.Title(), &aItem.Url());
	
	HBufC* titleBuf = HBufC::NewLC(KMaxLineLength);
	TPtr titlePtr(titleBuf->Des());
	titlePtr.Copy(aItem.Title());
	PodcastUtils::SQLEncode(titlePtr);
	
	HBufC* descBuf = HBufC::NewLC(KMaxLineLength);
	TPtr descPtr(descBuf->Des());
	descPtr.Copy(aItem.Description());
	PodcastUtils::SQLEncode(descPtr);
	
	_LIT(KSqlStatement, "update feeds set url=\"%S\", title=\"%S\", description=\"%S\", imageurl=\"%S\", imagefile=\"%S\", link=\"%S\", built=\"%Lu\", lastupdated=\"%Lu\", feedtype=\"%u\", customtitle=\"%u\", lasterror=\"%d\" where uid=\"%u\"");
	iSqlBuffer.Format(KSqlStatement,
			&aItem.Url(), titleBuf, descBuf, &aItem.ImageUrl(), &aItem.ImageFileName(), &aItem.Link(),
			aItem.BuildDate().Int64(), aItem.LastUpdated().Int64(), EAudioPodcast, aItem.CustomTitle(), aItem.LastError(), aItem.Uid());

	CleanupStack::PopAndDestroy(descBuf);
	CleanupStack::PopAndDestroy(titleBuf);
	
	sqlite3_stmt *st;
	 
	int rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st, (const void**) NULL);
	
	if (rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		
		if (rc != SQLITE_DONE)
			{
			User::Leave(KErrNotFound);
			}
		CleanupStack::PopAndDestroy(); //st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	DP("CFeedEngine::DBUpdateFeed END");
	}

void CFeedEngine::ParsingCompleteL(CFeedInfo *item)
	{
	TBuf<KMaxLineLength> title;
	title.Copy(item->Title());
	item->SetTitleL(title); // if this leaves we are out of memory
	}


EXPORT_C void CFeedEngine::AddObserver(MFeedEngineObserver *observer)
	{
	iObservers.Append(observer);
	}

EXPORT_C void CFeedEngine::RemoveObserver(MFeedEngineObserver *observer)
	{
	TInt index = iObservers.Find(observer);
	
	if (index > KErrNotFound)
		{
		iObservers.Remove(index);
		}
	}

void CFeedEngine::Connected(CHttpClient* /*aClient*/)
	{
	}

void CFeedEngine::Progress(CHttpClient* /*aHttpClient*/, TInt /*aBytes*/, TInt /*aTotalBytes*/)
	{	
}

void CFeedEngine::CompleteL(CHttpClient* /*aClient*/, TInt aError)
	{
	DP2("CFeedEngine::CompleteL BEGIN, iClientState=%d, aSuccessful=%d", iClientState, aError);

	switch(iClientState)
		{		
		case EUpdatingFeed: 
		{
		iClientState = EIdle;
		switch (aError)
			{
			case KErrCancel:						
				{
				iFeedsUpdating.Reset();
				}
				break;
			case KErrCouldNotConnect:
				iFeedsUpdating.Reset();
				break;
			default:
				{
				if (!iCancelRequested) {
					iActiveFeed->SetLastError(aError);
					TTime time;
					time.HomeTime();
					iActiveFeed->SetLastUpdated(time);
	
					if( aError == KErrNone)
						{
						// Parse the feed. We need to trap this call since it could leave and then
						// the whole update chain will be broken
						// change client state
						TRAPD(parserErr, iParser->ParseFeedL(iUpdatingFeedFileName, iActiveFeed, iPodcastModel.SettingsEngine().MaxListItems()));
	
						if(parserErr)
							{
							// we do not need to any special action on this error.
							iActiveFeed->SetLastError(parserErr);
							DP1("CFeedEngine::Complete()\t Failed to parse feed. Leave with error code=%d", parserErr);
							}

						// delete the downloaded XML file as it is no longer needed
						BaflUtils::DeleteFile(iPodcastModel.FsSession(),iUpdatingFeedFileName);			
	
						// if the feed has specified a image url. download it if we dont already have it
						if((iActiveFeed->ImageUrl().Length() > 0))
							{
							if ( (iActiveFeed->ImageFileName().Length() == 0) || 
									(iActiveFeed->ImageFileName().Length() > 0 && 
											!BaflUtils::FileExists(iPodcastModel.FsSession(), 
													iActiveFeed->ImageFileName()) )
							)
								{
								TRAPD(error, GetFeedImageL(iActiveFeed));
								if (error)
									{
									// we have failed in a very early stage to fetch the image.
									// continue with next Feed update	
									iActiveFeed->SetLastError(parserErr);
									iClientState = EIdle;							
									}
								}	
							}
						}
					else
						{
						// even if it fails, this will allow us to move on
						iClientState = EIdle;
						}
					}
				iCancelRequested = EFalse;
				}break;
			}
			DBUpdateFeedL(*iActiveFeed);
			NotifyFeedUpdateComplete(iActiveFeed->Uid(), aError);

			// we will wait until the image has been downloaded to start the next feed update.
			if (iClientState == EIdle)
				{
				UpdateNextFeedL();	
				}
			}break;
		case EUpdatingImage:
			{
			// change client state to not updating
			iClientState = EIdle;
			if(aError == KErrNone)
				{
				// now the image has been downloaded, so we set it again in the FeedInfo to start
				// converting it
				HBufC *fileNameCopy = iActiveFeed->ImageFileName().AllocLC();
				TRAP_IGNORE(iActiveFeed->SetImageFileNameL(*fileNameCopy, &iPodcastModel));
				CleanupStack::PopAndDestroy(fileNameCopy);
				}
			DBUpdateFeedL(*iActiveFeed);
			NotifyFeedUpdateComplete(iActiveFeed->Uid(), aError);
			UpdateNextFeedL();
			}break;
		case ESearching: 
			{
			iClientState = EIdle;
	
			DP2("Search complete, results in %S with error %d", &iSearchResultsFileName, aError);
			if(aError == KErrNone)
				{
				if (!iOpmlParser) 
					{
					iOpmlParser = new COpmlParser(*this, iPodcastModel.FsSession());
					}
	
				DP("Parsing OPML");
				iOpmlParser->ParseOpmlL(iSearchResultsFileName, ETrue);
				}
			else
				{
				NotifyOpmlParsingCompleteL(aError, 0);
				}
			
			BaflUtils::DeleteFile(iPodcastModel.FsSession(), iSearchResultsFileName);
			}break;
		case EIdle:
			UpdateNextFeedL();	
			break;
		default:
			Panic(EPodcatcherPanicFeedEngineState);
			break;
		}
	DP("CFeedEngine::CompleteL END");
	}

void CFeedEngine::NotifyFeedUpdateComplete(TInt aFeedUid, TInt aError)
	{
	DP("CFeedEngine::NotifyFeedUpdateComplete");	
	for (TInt i=0;i<iObservers.Count();i++) 
		{
		TRAP_IGNORE(iObservers[i]->FeedDownloadFinishedL(MFeedEngineObserver::EFeedAutoUpdate, aFeedUid, aError));
		}
	}

void CFeedEngine::Disconnected(CHttpClient* /*aClient*/)
	{
	}

void CFeedEngine::DownloadInfo(CHttpClient* /*aHttpClient */, int /*aTotalBytes*/)
	{	
	}

EXPORT_C void CFeedEngine::ImportFeedsL(const TDesC& aFile)
	{
	TFileName opmlPath;
	opmlPath.Copy(aFile);
	
	if (!iOpmlParser) {
		iOpmlParser = new COpmlParser(*this, iPodcastModel.FsSession());
	}
	
	iOpmlParser->ParseOpmlL(opmlPath, EFalse);
	}

EXPORT_C TBool CFeedEngine::ExportFeedsL(TFileName& aFile)
	{
	RFile rfile;
	TFileName privatePath;
	iPodcastModel.FsSession().PrivatePath(privatePath);
	TInt error = rfile.Temp(iPodcastModel.FsSession(), privatePath, aFile, EFileWrite);
	if (error != KErrNone) 
		{
		DP("CFeedEngine::ExportFeedsL()\tFailed to open file");
		return EFalse;
		}
	CleanupClosePushL(rfile);
	
	HBufC* templ = HBufC::NewLC(KMaxLineLength);
	templ->Des().Copy(KOpmlFeed());
	
	HBufC* line = HBufC::NewLC(KMaxLineLength);
	HBufC* xmlUrl = HBufC::NewLC(KMaxURLLength);		
	HBufC* htmlUrl = HBufC::NewLC(KMaxURLLength);		
	HBufC* desc = HBufC::NewLC(KMaxDescriptionLength);
	HBufC* title = HBufC::NewLC(KMaxTitleLength);

	HBufC8* utf8line = CnvUtfConverter::ConvertFromUnicodeToUtf8L(KOpmlHeader());

	rfile.Write(*utf8line);
	delete utf8line;
	
	for (int i=0; i<iSortedFeeds.Count(); i++)
		{
		CFeedInfo *info = iSortedFeeds[i];
		DP1("Exporting feed '%S'", &iSortedFeeds[i]->Title());
		
	// XML URL
		TPtr ptrXml(xmlUrl->Des());
		if (info->Url() != KNullDesC)
			{
			ptrXml.Copy(info->Url());
			PodcastUtils::XMLEncode(ptrXml);
			}
		
	// Description	
		TPtr ptrDesc(desc->Des());
		ptrDesc.Zero();
		if (info->Description() != KNullDesC) {
			ptrDesc.Copy(info->Description());
			PodcastUtils::XMLEncode(ptrDesc);
		}
		
	// Title	
		TPtr ptrTitle(title->Des());
		ptrTitle.Zero();

		if (info->Title() != KNullDesC) {
			ptrTitle.Copy(info->Title());
			PodcastUtils::XMLEncode(ptrTitle);
		}
		
	// HTML URL	
		TPtr ptrHtmlUrl(htmlUrl->Des());
		ptrHtmlUrl.Zero();

		if (info->Link() != KNullDesC) {
			ptrHtmlUrl.Copy(info->Link());
			PodcastUtils::XMLEncode(ptrHtmlUrl);
		}	
	// Write line to OPML file
		line->Des().Format(*templ, title, desc, xmlUrl, htmlUrl);
		utf8line = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*line);
		rfile.Write(*utf8line);
		delete utf8line;
		}

	utf8line = CnvUtfConverter::ConvertFromUnicodeToUtf8L(KOpmlFooter());
	rfile.Write(*utf8line);
	delete utf8line;
		
	CleanupStack::PopAndDestroy(7);//destroy 6 bufs & close rfile
	
	return ETrue;
	}

EXPORT_C CFeedInfo* CFeedEngine::GetFeedInfoByUid(TUint aFeedUid)
	{
	TInt cnt = iSortedFeeds.Count();
	for (TInt i=0;i<cnt;i++)
		{
		if (iSortedFeeds[i]->Uid() == aFeedUid)
			{
			return iSortedFeeds[i];
			}
		}
	
	return NULL;
	}
		
EXPORT_C const RFeedInfoArray& CFeedEngine::GetSortedFeeds()
{
	TLinearOrder<CFeedInfo> sortOrder( CFeedEngine::CompareFeedsByTitle);

	iSortedFeeds.Sort(sortOrder);
	return iSortedFeeds;
}

TInt CFeedEngine::CompareFeedsByTitle(const CFeedInfo &a, const CFeedInfo &b)
	{
		//DP2("Comparing %S to %S", &a.Title(), &b.Title());
		return a.Title().CompareF(b.Title());
	}

EXPORT_C void CFeedEngine::GetStatsByFeedL(TUint aFeedUid, TUint &aNumShows, TUint &aNumUnplayed)
	{
	//DP1("CFeedEngine::GetStatsByFeed, aFeedUid=%u", aFeedUid);
	DBGetStatsByFeedL(aFeedUid, aNumShows, aNumUnplayed);
	}

void CFeedEngine::DBGetStatsByFeedL(TUint aFeedUid, TUint &aNumShows, TUint &aNumUnplayed)
	{
	//DP1("CFeedEngine::DBGetStatsByFeed, feedUid=%u", aFeedUid);
	_LIT(KSqlStatement, "select count(*) from shows where feeduid=%u");
	iSqlBuffer.Format(KSqlStatement, aFeedUid);

	sqlite3_stmt *st;
	 
	int rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st,	(const void**) NULL);
	
	if( rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
	  	rc = sqlite3_step(st);
	  	
	  	if (rc == SQLITE_ROW)
	  		{
	  		aNumShows = sqlite3_column_int(st, 0);
	  		}
	  	else
	  		{
			User::Leave(KErrNotFound);
	  		}
	  	CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
		  
	_LIT(KSqlStatement2, "select count(*) from shows where feeduid=%u and playstate=0");
	iSqlBuffer.Format(KSqlStatement2, aFeedUid);

	rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st,	(const void**) NULL);
		
	if(rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
	  	rc = sqlite3_step(st);
	  	
	  	if (rc == SQLITE_ROW)
	  		{
	  		aNumUnplayed = sqlite3_column_int(st, 0);
	  		}
	  	else
	  		{
			User::Leave(KErrNotFound);
	  		}
	  	CleanupStack::PopAndDestroy(); // st
	}
}

TUint CFeedEngine::DBGetFeedCountL()
	{
	DP("DBGetFeedCount BEGIN");
	sqlite3_stmt *st;
	int rc = sqlite3_prepare_v2(&iDB,"select count(*) from feeds" , -1, &st, (const char**) NULL);
	TUint size = 0;
	 
	if( rc==SQLITE_OK )
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
			
		if (rc == SQLITE_ROW)
			{
	  		size = sqlite3_column_int(st, 0);
	  		}
		else
			{
	  		User::Leave(KErrCorrupt);
	  		}
		CleanupStack::PopAndDestroy(); // st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	DP1("DBGetFeedCount END=%d", size);
	return size;
}

void CFeedEngine::DBLoadFeedsL()
	{
	DP("DBLoadFeeds BEGIN");
	iSortedFeeds.Reset();
	CFeedInfo *feedInfo = NULL;
	
	_LIT(KSqlStatement, "select url, title, description, imageurl, imagefile, link, built, lastupdated, uid, feedtype, customtitle, lasterror from feeds");
	iSqlBuffer.Format(KSqlStatement);

	sqlite3_stmt *st;
	 
	TLinearOrder<CFeedInfo> sortOrder( CFeedEngine::CompareFeedsByTitle);

	int rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st,	(const void**) NULL);
	
	if (rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		while(rc == SQLITE_ROW) 
			{
			feedInfo = CFeedInfo::NewLC();
			
			const void *urlz = sqlite3_column_text16(st, 0);
			TPtrC16 url((const TUint16*)urlz);
			feedInfo->SetUrlL(url);

			const void *titlez = sqlite3_column_text16(st, 1);
			TPtrC16 title((const TUint16*)titlez);
			feedInfo->SetTitleL(title);

			const void *descz = sqlite3_column_text16(st, 2);
			TPtrC16 desc((const TUint16*)descz);
			feedInfo->SetDescriptionL(desc);

			const void *imagez = sqlite3_column_text16(st, 3);
			TPtrC16 image((const TUint16*)imagez);
			feedInfo->SetImageUrlL(image);

			const void *imagefilez = sqlite3_column_text16(st, 4);
			TPtrC16 imagefile((const TUint16*)imagefilez);
			if (imagefile.Length() > 0)
				{
				feedInfo->SetImageFileNameL(imagefile, &iPodcastModel);
				}
			
			const void *linkz = sqlite3_column_text16(st, 5);
			TPtrC16 link((const TUint16*)linkz);
			feedInfo->SetDescriptionL(link);
					
			sqlite3_int64 built = sqlite3_column_int64(st, 6);
			TTime buildtime(built);
			feedInfo->SetBuildDate(buildtime);

			sqlite3_int64 lastupdated = sqlite3_column_int64(st, 7);
			TTime lastupdatedtime(lastupdated);
			feedInfo->SetLastUpdated(lastupdatedtime);
			
			sqlite3_int64 customtitle = sqlite3_column_int64(st, 10);
			if (customtitle)
				{
				feedInfo->SetCustomTitle();
				}
			
			sqlite3_int64 lasterror = sqlite3_column_int(st, 11);
			feedInfo->SetLastError(lasterror);
			
			TLinearOrder<CFeedInfo> sortOrder( CFeedEngine::CompareFeedsByTitle);

			iSortedFeeds.InsertInOrder(feedInfo, sortOrder);
			
			CleanupStack::Pop(feedInfo);
				
			rc = sqlite3_step(st);
			}	
		CleanupStack::PopAndDestroy();//st
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	DP("DBLoadFeeds END");
	}

CFeedInfo* CFeedEngine::DBGetFeedInfoByUidL(TUint aFeedUid)
	{
	DP("CFeedEngine::DBGetFeedInfoByUid BEGIN");
	CFeedInfo *feedInfo = NULL;
	_LIT(KSqlStatement, "select url, title, description, imageurl, imagefile, link, built, lastupdated, uid, feedtype, customtitle, lasterror from feeds where uid=%u");
	iSqlBuffer.Format(KSqlStatement, aFeedUid);

	sqlite3_stmt *st;
	
	int rc = sqlite3_prepare16_v2(&iDB, (const void*)iSqlBuffer.PtrZ() , -1, &st,	(const void**) NULL);
	
	if (rc==SQLITE_OK)
		{
		Cleanup_sqlite3_finalize_PushL(st);
		rc = sqlite3_step(st);
		if (rc == SQLITE_ROW)
			{
			feedInfo = CFeedInfo::NewLC();
			
			const void *urlz = sqlite3_column_text16(st, 0);
			TPtrC16 url((const TUint16*)urlz);
			feedInfo->SetUrlL(url);

			const void *titlez = sqlite3_column_text16(st, 1);
			TPtrC16 title((const TUint16*)titlez);
			feedInfo->SetTitleL(title);

			const void *descz = sqlite3_column_text16(st, 2);
			TPtrC16 desc((const TUint16*)descz);
			feedInfo->SetDescriptionL(desc);

			const void *imagez = sqlite3_column_text16(st, 3);
			TPtrC16 image((const TUint16*)imagez);
			feedInfo->SetImageUrlL(image);

			const void *imagefilez = sqlite3_column_text16(st, 4);
			TPtrC16 imagefile((const TUint16*)imagefilez);
			feedInfo->SetDescriptionL(imagefile);
			
			const void *linkz = sqlite3_column_text16(st, 5);
			TPtrC16 link((const TUint16*)linkz);
			feedInfo->SetDescriptionL(link);
					
			sqlite3_int64 built = sqlite3_column_int64(st, 6);
			TTime buildtime(built);
			feedInfo->SetBuildDate(buildtime);

			sqlite3_int64 lastupdated = sqlite3_column_int64(st, 7);
			TTime lastupdatedtime(lastupdated);
			feedInfo->SetLastUpdated(lastupdatedtime);
			
			sqlite3_int64 customtitle = sqlite3_column_int64(st, 10);
			if (customtitle) {
				feedInfo->SetCustomTitle();
			}
			
			TInt lasterror = sqlite3_column_int(st, 11);
			feedInfo->SetLastError(lasterror);
						
			CleanupStack::Pop(feedInfo);
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		CleanupStack::PopAndDestroy();//st	
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	DP("CFeedEngine::DBGetFeedInfoByUid END");
	return feedInfo;
}

EXPORT_C void CFeedEngine::UpdateFeedInfoL(CFeedInfo *aItem)
	{
	if (iActiveFeed && iActiveFeed->Uid() == aItem->Uid())
		{
		User::Leave(KErrInUse);
		}
	
	DBUpdateFeedL(*aItem);
	}

EXPORT_C void CFeedEngine::SearchForFeedL(TDesC& aSearchString)
	{
	DP1("FeedEngine::SearchForFeedL BEGIN, aSearchString=%S", &aSearchString);
	
	if (iClientState != EIdle) {
		User::Leave(KErrInUse);
	}
	TBuf<KMaxURLLength> ssBuf;
	ssBuf.Copy(aSearchString);
	PodcastUtils::ReplaceString(ssBuf, _L(" "), _L("%20"));
	// prepare search URL
	HBufC* templ = HBufC::NewLC(KMaxLineLength);
	templ->Des().Copy(KSearchUrl());
		
	HBufC* url = HBufC::NewLC(KMaxURLLength);		
	url->Des().Format(*templ, &ssBuf);

	DP1("SearchURL: %S", url);
	
	// crate path to store OPML search results
	iPodcastModel.FsSession().PrivatePath(iSearchResultsFileName);
	
	iSearchResultsFileName.Append(KSearchResultsFileName);
	iSearchResults.ResetAndDestroy();
	// run search
	if(iFeedClient->GetL(*url, iSearchResultsFileName, iPodcastModel.SettingsEngine().SpecificIAP()))
		{
		iClientState = ESearching;
		}
	else
		{
		iClientState = EIdle;
		User::Leave(KErrAbort);
		}
	
	CleanupStack::PopAndDestroy(url);
	CleanupStack::PopAndDestroy(templ);
		
	DP("FeedEngine::SearchForFeedL END");
	}

EXPORT_C void CFeedEngine::AddSearchResultL(CFeedInfo *item)
	{
	DP1("CFeedEngine::AddSearchResultL, item->Title()=%S", &(item->Title()));
	iSearchResults.AppendL(item);
	}

EXPORT_C const RFeedInfoArray& CFeedEngine::GetSearchResults()
	{
	return iSearchResults;
	}


EXPORT_C void CFeedEngine::OpmlParsingCompleteL(TInt aError, TUint aNumFeedsAdded)
	{
	NotifyOpmlParsingCompleteL(aError, aNumFeedsAdded);
	}

void CFeedEngine::NotifyOpmlParsingCompleteL(TInt aError, TUint aNumFeedsAdded)
	{
	for (TInt i=0;i<iObservers.Count();i++) 
		{
		iObservers[i]->OpmlParsingComplete(aError, aNumFeedsAdded);
		}
	}
