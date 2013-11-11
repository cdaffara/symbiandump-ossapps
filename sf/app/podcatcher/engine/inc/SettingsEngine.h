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

#ifndef SETTINGSENGINE_H_
#define SETTINGSENGINE_H_

#include <e32base.h>
#include "PodcastModel.h"

_LIT(KPodcastDir1, "F:\\Podcasts\\"); // memory card on phone with flash disk
_LIT(KPodcastDir2, "E:\\Podcasts\\"); // memory card on phone without flash disk
_LIT(KPodcastDir3, "C:\\Podcasts\\");

_LIT(KConfigImportFile, "config.cfg");
_LIT(KDefaultFeedsFile, "defaultfeeds.xml");
_LIT(KImportFeedsFile, "feeds.xml");

_LIT(KConfigFile, "config.db");

// constants
const TInt KSettingsUid = 1000;
const TInt KDefaultUpdateFeedInterval = 60;
const TInt KDefaultMaxListItems = 100;
const TInt32 KUseIAPFlag =0x01000000;
const TInt32 KUseIAPMask =0x00FFFFFF;

enum TAutoUpdateSetting
	{
	EAutoUpdateOff,
	EAutoUpdatePeriod1=60,
	EAutoUpdatePeriod2=360,
	EAutoUpdatePeriod3=720,
	EAutoUpdatePeriod4=1440
	};

class CSettingsEngine : public CBase
	{
	public:
		static CSettingsEngine* NewL(CPodcastModel& aPodcastModel);
		virtual ~CSettingsEngine();
	

		TFileName DefaultFeedsFileName();
		TFileName ImportFeedsFileName();
		TFileName PrivatePath();

		TInt MaxListItems();

		IMPORT_C TFileName& BaseDir();
		IMPORT_C void SetBaseDir(TFileName& aFileName);

		IMPORT_C TInt UpdateFeedInterval();
		IMPORT_C void SetUpdateFeedInterval(TInt aInterval);

		IMPORT_C TInt MaxSimultaneousDownloads();
		IMPORT_C void SetMaxSimultaneousDownloads(TInt aMaxDownloads);
		
		IMPORT_C TAutoUpdateSetting UpdateAutomatically();	
		IMPORT_C void SetUpdateAutomatically(TAutoUpdateSetting aAutoOn);
		
		IMPORT_C TBool DownloadAutomatically();
		IMPORT_C void SetDownloadAutomatically(TBool aAutoDownloadOn);
		
		IMPORT_C TBool DownloadSuspended();
		IMPORT_C void SetDownloadSuspended(TBool aSuspended);

		IMPORT_C TTime UpdateFeedTime();
		IMPORT_C void SetUpdateFeedTime(TTime aTime);
		
		IMPORT_C TInt SpecificIAP();
		IMPORT_C void SetSpecificIAP(TInt aIap);
			
		IMPORT_C void SaveSettingsL();

	private:
		CSettingsEngine(CPodcastModel& aPodcastModel);
		void ConstructL();
		void LoadSettingsL();
		void GetDefaultBaseDirL(TDes &aBaseDir);


	private:
		// the settings we serialize
		TFileName iBaseDir;
		TInt iUpdateFeedInterval;
		TAutoUpdateSetting iUpdateAutomatically;
		TBool iDownloadAutomatically;
		TInt iIap;
		TInt iMaxListItems;
		TTime iUpdateFeedTime;
		TBool iDownloadSuspended;

		// Other member variables		
		CPodcastModel &iPodcastModel; 	// reference to the model
	};

#endif /*SETTINGSENGINE_H_*/
