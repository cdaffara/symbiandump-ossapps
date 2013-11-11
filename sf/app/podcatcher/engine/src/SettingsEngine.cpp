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

#include <bautils.h>
#include <s32file.h>
#include "SettingsEngine.h"
#include "FeedEngine.h"
#include "ShowEngine.h"

const TUid KMainSettingsStoreUid = {0x1000};
const TUid KMainSettingsUid = {0x1002};
const TUid KExtraSettingsUid = {0x2001};
const TInt KMaxParseBuffer = 1024;

CSettingsEngine::CSettingsEngine(CPodcastModel& aPodcastModel) : iPodcastModel(aPodcastModel)
	{
	}

CSettingsEngine::~CSettingsEngine()
	{
	TRAP_IGNORE(SaveSettingsL());	
	}

CSettingsEngine* CSettingsEngine::NewL(CPodcastModel& aPodcastModel)
	{
	CSettingsEngine* self = new (ELeave) CSettingsEngine(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSettingsEngine::ConstructL()
	{
	DP("CSettingsEngine::ConstructL BEGIN");
	// default values
	iUpdateFeedInterval = KDefaultUpdateFeedInterval;
	iDownloadAutomatically = EFalse;
	iUpdateAutomatically = EAutoUpdateOff;
	iMaxListItems = KDefaultMaxListItems;
	iIap = 0;
	
	// Check that our basedir exist. Create it otherwise;
	GetDefaultBaseDirL(iBaseDir);
	DP1("Base dir: %S", &iBaseDir);
	
	// load settings
	TRAPD(loadErr, LoadSettingsL());
	if (loadErr != KErrNone)
		{
		DP1("CSettingsEngine::ConstructL\tLoadSettingsL returned error=%d", loadErr);
		DP("CSettingsEngine::ConstructL\tUsing default settings instead");
	
		TRAPD(error,SaveSettingsL());
		if (error != KErrNone) 
			{
			DP1("error saving: %d", error);
			}
		}
	DP("CSettingsEngine::ConstructL END");	
	}

void CSettingsEngine::GetDefaultBaseDirL(TDes & /*aBaseDir*/)
	{
	CDesCArray* disks = new(ELeave) CDesCArrayFlat(10);
	CleanupStack::PushL(disks);

	BaflUtils::GetDiskListL(iPodcastModel.FsSession(), *disks);
	
	#ifdef __WINS__
		iBaseDir.Copy(KPodcastDir3);
		CleanupStack::PopAndDestroy(disks);
		return;
	#endif
	
	DP1("Disks count: %u", disks->Count());
	
	for (int i=0;i<disks->Count();i++) {
		TPtrC ptr = disks->operator[](i);
		DP2("Disk %u: %S", i, &ptr);
	}

	if (disks->Count() == 1)  // if only one drive, use C:
		{
		iBaseDir.Copy(KPodcastDir3);
		} 
	else // else we use the flash drive
		{
		iBaseDir.Copy(KPodcastDir1);
		DP1("Trying podcast dir '%S'", &iBaseDir);
				
		TRAPD(err, BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(), iBaseDir));
		
		if (err != KErrNone)
			{
			DP("Leave in EnsurePathExistsL");
			iBaseDir.Copy(KPodcastDir2);
			DP1("Trying podcast dir '%S'", &iBaseDir);
			TRAPD(err, BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(), iBaseDir));
			if (err != KErrNone) 
				{
				DP("Leave in EnsurePathExistsL");
				iBaseDir.Copy(KPodcastDir3);
				DP1("Trying podcast dir '%S'", &iBaseDir);
				TRAPD(err, BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(), iBaseDir));
				if (err != KErrNone)
					{
					DP("Leave in EnsurePathExistsL");
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(disks);
	}

void CSettingsEngine::LoadSettingsL()
	{
	DP("CSettingsEngine::LoadSettingsL\t Trying to load settings");
	
	// Create path for the config file
	TFileName configPath;
	configPath.Copy(PrivatePath());
	configPath.Append(KConfigFile);

	DP1("Checking settings file: %S", &configPath);
	
	CDictionaryFileStore* store = CDictionaryFileStore::OpenLC(iPodcastModel.FsSession(), configPath, KMainSettingsStoreUid);

	if( store->IsPresentL(KMainSettingsUid) )
	{
		RDictionaryReadStream stream;
		stream.OpenLC(*store, KMainSettingsUid);
		
		TInt len = stream.ReadInt32L();
		stream.ReadL(iBaseDir, len);
		iUpdateFeedInterval = stream.ReadInt32L();
		iUpdateAutomatically = static_cast<TAutoUpdateSetting>(stream.ReadInt32L());
		iDownloadAutomatically = stream.ReadInt32L();
		
		
		iDownloadSuspended = stream.ReadInt32L(); // was iMaxSimultaneousDownloads
		iIap = stream.ReadInt32L();		
		
		TInt low = stream.ReadInt32L();
		TInt high = stream.ReadInt32L();
		iUpdateFeedTime = MAKE_TINT64(high, low);
		TInt dummy;		
		dummy = stream.ReadInt32L(); // was iSelectOnlyUnplayed
		dummy = stream.ReadInt32L(); // was iSeekStepTime
				
		CleanupStack::PopAndDestroy(1); // readStream and iniFile
		DP("CSettingsEngine::LoadSettingsL\t Settings loaded OK");
	}
	CleanupStack::PopAndDestroy();// close store
}

EXPORT_C void CSettingsEngine::SaveSettingsL()
	{
	DP("CSettingsEngine::SaveSettingsL BEGIN");

	TFileName configPath;
	configPath.Copy(PrivatePath());
	configPath.Append(KConfigFile);

	CDictionaryFileStore* store = CDictionaryFileStore::OpenLC(iPodcastModel.FsSession(), configPath, KMainSettingsStoreUid);

	RDictionaryWriteStream stream;
	stream.AssignLC(*store, KMainSettingsUid);
		
	stream.WriteInt32L(iBaseDir.Length());
	stream.WriteL(iBaseDir);
	stream.WriteInt32L(iUpdateFeedInterval);
	stream.WriteInt32L(iUpdateAutomatically);
	stream.WriteInt32L(iDownloadAutomatically);
	stream.WriteInt32L(iDownloadSuspended);
	stream.WriteInt32L(iIap);
	
	stream.WriteInt32L(I64LOW(iUpdateFeedTime.Int64()));
	stream.WriteInt32L(I64HIGH(iUpdateFeedTime.Int64()));
	stream.WriteInt32L(0); // was iSelectOnlyUnplayed
	stream.WriteInt32L(0); // was iSeekStepTime

	stream.CommitL();
	store->CommitL();
	CleanupStack::PopAndDestroy(2); // stream and store
	DP("CSettingsEngine::SaveSettingsL END");
	}

TFileName CSettingsEngine::DefaultFeedsFileName()
	{
	TFileName defaultFeeds;
	TFileName temp;
	defaultFeeds.Copy(_L("z:"));
	temp.Append(PrivatePath());
	temp.Append(KDefaultFeedsFile);
	defaultFeeds.Append(temp);
	DP1("Default feeds file: %S", &defaultFeeds);
	return defaultFeeds;
	}

TFileName CSettingsEngine::ImportFeedsFileName()
	{
	TFileName importFeeds;
	importFeeds.Append(BaseDir());
	importFeeds.Append(KImportFeedsFile);
	return importFeeds;
	}


TFileName CSettingsEngine::PrivatePath()
	{
	TFileName privatePath;
	iPodcastModel.FsSession().PrivatePath(privatePath);

	TRAP_IGNORE(BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(), privatePath));
	return privatePath;
	}

TInt CSettingsEngine::MaxListItems() 
	{
	return iMaxListItems;
	}

EXPORT_C TFileName& CSettingsEngine::BaseDir()
	{
	return iBaseDir;
	}

EXPORT_C void CSettingsEngine::SetBaseDir(TFileName& aFileName)
	{
	TInt length = aFileName.Length();
	if (length > 0) 
		{
		if (aFileName[length-1] != '\\') 
			{
			aFileName.Append(_L("\\"));
			}
		}
	iBaseDir = aFileName;
	}

EXPORT_C TInt CSettingsEngine::UpdateFeedInterval() 
	{
	return iUpdateFeedInterval;
	}

EXPORT_C void CSettingsEngine::SetUpdateFeedInterval(TInt aInterval)
	{
	iUpdateFeedInterval = aInterval;
	iPodcastModel.FeedEngine().RunFeedTimer();
	}

EXPORT_C TAutoUpdateSetting CSettingsEngine::UpdateAutomatically() 
	{
	if (iUpdateAutomatically == EAutoUpdateOff || 
		iUpdateAutomatically == EAutoUpdatePeriod1 ||
		iUpdateAutomatically == EAutoUpdatePeriod2 ||
		iUpdateAutomatically == EAutoUpdatePeriod3 ||
		iUpdateAutomatically == EAutoUpdatePeriod4)
		return iUpdateAutomatically;
	else
		return EAutoUpdateOff;
	}

EXPORT_C void CSettingsEngine::SetUpdateAutomatically(TAutoUpdateSetting aAutoSetting)
	{
	iUpdateAutomatically = aAutoSetting;
	
	if (aAutoSetting > 0) {
		SetUpdateFeedInterval(aAutoSetting);
	}
	}

EXPORT_C TBool CSettingsEngine::DownloadAutomatically() 
	{
	return iDownloadAutomatically;
	}

EXPORT_C void CSettingsEngine::SetDownloadAutomatically(TBool aDownloadAuto)
	{
	iDownloadAutomatically = aDownloadAuto;
	}

EXPORT_C TTime CSettingsEngine::UpdateFeedTime()
	{
	return iUpdateFeedTime;
	}

EXPORT_C void CSettingsEngine::SetUpdateFeedTime(TTime aUpdateTime)
	{
	iUpdateFeedTime = aUpdateTime;
	iPodcastModel.FeedEngine().RunFeedTimer();
	}

EXPORT_C TInt CSettingsEngine::SpecificIAP()
	{
	if (iIap == 0 || iIap == -1) {
		return iIap;
	}
	
	for (int i=0;i<iPodcastModel.IAPIds().Count();i++) {
		if (((TInt)iPodcastModel.IAPIds()[i].iIapId) == (iIap&KUseIAPMask)) {
			return iIap;
		}

	}
	
	for (int i=0;i<iPodcastModel.SNAPIds().Count();i++) {
			if (((TInt)iPodcastModel.SNAPIds()[i].iIapId) == (iIap&KUseIAPMask)) {
				return iIap;
			}

		}
	
	return iPodcastModel.IAPIds()[0].iIapId;
	}

EXPORT_C void CSettingsEngine::SetSpecificIAP(TInt aIap)
	{
	iIap = aIap;
	}

EXPORT_C TBool CSettingsEngine::DownloadSuspended()
	{
	return iDownloadSuspended;
	}

EXPORT_C void CSettingsEngine::SetDownloadSuspended(TBool aSuspended)
	{
	DP1("CSettingsEngine::SetDownloadSuspended, aSuspended=%d", aSuspended);
	iDownloadSuspended = aSuspended;
	}
