// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CMMSACCOUNT_H__
#define __CMMSACCOUNT_H__


#include <e32base.h>

class CRepository;
class CMmsSettings;
class TMmsAccountId;
class MMmsSettingsObserver;


/** 
The maximum account name size

@publishedAll
@released
*/
const TInt KMmsAccountNameSize = 256;

     

/** 
MMS account ID 

@publishedAll
@released
*/
class TMmsAccountId 
 	{
public:
	/** MMS account Id */
   	TInt iMmsAccountId;

	/** MMS account Name */     
   	TBuf<KMmsAccountNameSize> iMmsAccountName;
   	};


/**
Stores MMS settings.

@publishedAll
@released
*/
class CMmsAccounts : public CActive
 	{
public:
  	IMPORT_C static CMmsAccounts* NewL( );
   	IMPORT_C static CMmsAccounts* NewLC( );
    IMPORT_C virtual ~CMmsAccounts();
 
    // Observer functions
    IMPORT_C void AddObserverL(MMmsSettingsObserver & aObserver);
    IMPORT_C void RemoveObserver(MMmsSettingsObserver & aObserver);

    IMPORT_C TMmsAccountId CreateMMSAccountL(const TDesC& aAccountName, const CMmsSettings& aSettings);
  	IMPORT_C TInt MaxMMSAccounts();
               
    IMPORT_C void GetMMSAccountsL(RArray<TMmsAccountId>& aAccountIds) const;
    IMPORT_C void DeleteMMSAccountL(const TMmsAccountId& aAccountId);
     
    IMPORT_C void LoadSettingsL(const TMmsAccountId& aAccountId, CMmsSettings& aSettings);          
    IMPORT_C void PopulateDefaultSettingsL(CMmsSettings& aSettings);

    IMPORT_C void SaveSettingsL(const TMmsAccountId& aAccountId, const CMmsSettings& aSettings) const;

    IMPORT_C TMmsAccountId DefaultMMSAccountL( ) const;
    IMPORT_C void SetDefaultMMSAccountL(const TMmsAccountId& aAccount);

private:
    CMmsAccounts();
	void ConstructL();
	void DoLoadSettingsL(TUint32 aAccountId, CMmsSettings& aSettings);
	void DoSaveSettingsL(TUint32 aAccountId, const CMmsSettings& aSettings, const TDesC& aAccountName) const;

    void DoCancel( );
    void RunL( );

	TUint GetNextEmptyAccountSlotL();

private:
	enum TMmsSettingsCenRepId
		{
		EMmsAccountName				= 0x00000000,
		EMmsSettingsVersion			= 0x00000001,
		EMmsApplicationID			= 0x00000002,
		EMmsAddress					= 0x00000003,
		EMmsCreationMode			= 0x00000004,
		EMmsSettingsFlags			= 0x00000005,
		EMmsProxyCount				= 0x00000006,
		EMmsNapIdCount				= 0x00000007,		
		EMmsAutomaticDownload		= 0x00000008,
		EMmsValidityPeriod			= 0x00000009,
		EMmsMaxDownloadSize			= 0x0000000A,
		EMmsMaxDownloadRetries		= 0x0000000B,
		EMmsDownloadRetryInterval	= 0x0000000C,
		EMmsMaxSendMsgSize			= 0x0000000D,
		EMmsDeviceContentClass		= 0x0000000E,
		EMmsMaxImageHeight			= 0x0000000F,
		EMmsMaxImageWidth			= 0x00000010,
		
		EMmsProxyList				= 0x00001000,
		EMmsNapIdList				= 0x00002000,
		};

private:
   	CRepository* iRepository;
    RPointerArray<MMmsSettingsObserver> iObservers;
    };     
     
#endif // __CMMSACCOUNT_H__
