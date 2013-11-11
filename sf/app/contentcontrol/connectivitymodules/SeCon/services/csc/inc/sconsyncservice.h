/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CSConSyncService header file.
*
*/


#ifndef _SCONSYNCSERVICE_H_
#define _SCONSYNCSERVICE_H_

#include <e32base.h>
#include <apgcli.h>     // RApaLsSession
#include <SyncMLClientDS.h>


class TApaAppInfo;
class RApaLsSession;
class CRepository;


class TSConSolutionInfo
	{
	public:
	TSConSolutionInfo():iTime(0){}
	public:
	TFileName 	iSolutionName;
	TUint		iUid;
	TTime		iTime;
	};

/**
 *  This class provides information about active sync solutions.
 *
 *  @code
 *   CSConSyncService *syncService = CSConSyncService::NewLC();
 *   SConSolutionInfo solutionInfo;
 *   syncService->GetSolutionL( KSeconCalendarUid, solutionInfo );
 *   CleanupStack::PopAndDestroy( syncService );
 *  @endcode
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CSConSyncService ) : public CBase
	{
	public:
	static CSConSyncService* NewLC();
	~CSConSyncService();
	
	/**
     * Populate SolutionInfo according to content type.
     *
     * @since S60 v5.0
     * @param aSeconContentType Contenttype to be readed
     * @param aSolutionInfo contains founded solution info,
     * 				        solution info is empty if not found
     * @return none
     */
	void GetSolutionL( const TUint aSeconContentType, TSConSolutionInfo& aSolutionInfo );
	
	private:
	
	CSConSyncService();
	void ConstructL();
	
	// Return correct providerId according to content type.
	TSmlDataProviderId ProviderId( const TUint32 aSeconContentType ) const;
	
	// Parse time from text
	TInt ParseTimestamp( TTime& aTime, const TDesC& aTimestamp ) const;
	
	// Get application caption.
	void GetApplicationCaptionL( TDes& aCaption, const TUid aAppUid );
	
	// Search UID from installed applications, return app caption
	void GetInstPackageCaptionL( TDes& aCaption, const TUid aPackageUid );
	
	// Gets last used profile for selected provider
	void GetLastUsedProfileNameL( TDes& aProfileName, TTime& aTime,
			const TSmlDataProviderId& aDataProviderId );
	
	// Gets last sync time for certain profile and provider.
	void GetLastSyncTimeL( TTime& aTime, const TSmlProfileId aSyncProfileId,
			const TSmlDataProviderId aDataProviderId );
	
	// Gets provider task id from profile
	void GetProviderTaskL( TSmlTaskId& aProviderTask,
			RSyncMLDataSyncProfile& aSyncProfile,
			const TSmlDataProviderId aDataProviderId ) const;
	
	// Gets latest historyJob where task is correct.
	const CSyncMLHistoryJob* LatestHistoryJob( RSyncMLHistoryLog& aHistoryLog,
			TInt aTaskId ) const;
	
	// Returns ETrue if task exists in HistoryJob.
	TBool TaskExist( const CSyncMLHistoryJob* aHistoryJob, TInt aTaskId ) const;
	
	private: // data
	TBool			iApaSessionConnected;
	RApaLsSession 	iApaSession;
	TBool			iSyncSessionOpened;
	RSyncMLSession 	iSyncSession;
	CRepository* 	iRepository;
	};

#endif // _SCONSYNCSERVICE_H_
