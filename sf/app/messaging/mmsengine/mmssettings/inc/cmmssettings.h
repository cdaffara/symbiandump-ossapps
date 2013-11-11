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

#ifndef __CMMSSETTINGS_H__
#define __CMMSSETTINGS_H__


#include <e32base.h>


class CRepository;


/**
Automatic Download Options.

@publishedAll
@released
*/
enum TAutomaticDownloadOptions
 	{
 	/** Automatic Download Option Off */
	EAutomaticDownloadOff = 0,
 	/** Automatic Download Option On */
	EAutomaticDownloadOn,
	/** Automatic Download Only When Roaming */
	EAutomaticDownloadOnlyWhenRoaming
	};


/**
Creation Mode Options.

@publishedAll
@released
*/
enum TCreationMode
 	{
	/** Creation Mode Free */
	ECreationModeFree = 0,
	/** Creation Mode Warning */
	ECreationModeWarning,
	/** Creation Mode Restricted */
	ECreationModeRestricted
	};


/**
Run-time MMS configuration settings.

Messaging clients should use an instance of this class to specify
and retrieve configuration settings.

This class should be used with CMmsAccounts to store and restore settings.

@see CMmsAccounts

@publishedAll
@released
*/
class CMmsSettings : public CBase
     {
public:
	IMPORT_C static CMmsSettings* NewL( );
	IMPORT_C static CMmsSettings* NewLC( );
    IMPORT_C ~CMmsSettings( );
    IMPORT_C void CopyL(const CMmsSettings& aMmsSettings);    

    IMPORT_C TInt ApplicationID( ) const;
	IMPORT_C void SetApplicationID(const TInt aAppID);

	IMPORT_C TPtrC Address( ) const;
   	IMPORT_C void SetAddressL(const TDesC& aAddress);
	
	IMPORT_C TCreationMode CreationMode( ) const;
   	IMPORT_C void SetCreationModeL(TCreationMode aCreationMode);

    IMPORT_C TInt ProxyCount( ) const;
    IMPORT_C TUid GetProxy(TInt aIndex) const;
    IMPORT_C void AddProxyL(const TUid aProxy);
    IMPORT_C void RemoveProxy(TInt aIndex);

    IMPORT_C TInt NapIdCount( ) const;
    IMPORT_C TUid GetNapId (TInt aIndex) const;
    IMPORT_C void AddNapIdL(const TUid aNapId);
    IMPORT_C void RemoveNapId(TInt aIndex);

    IMPORT_C TBool DisplayNotification( ) const;
    IMPORT_C void SetDisplayNotification(TBool aFlag);

    IMPORT_C TAutomaticDownloadOptions AutomaticDownload( ) const;
    IMPORT_C void SetAutomaticDownload(TAutomaticDownloadOptions aDownloadOptions);

    IMPORT_C TBool CreationModeReadOnly( ) const;
    IMPORT_C void SetCreationModeReadOnly(TBool aFlag);

    IMPORT_C TInt ValidityPeriod( ) const;
    IMPORT_C void SetValidityPeriod(const TInt aValidityPeriod);

    IMPORT_C TInt MaxDownloadSize( ) const;
    IMPORT_C void SetMaxDownloadSize (const TInt aSize);

    IMPORT_C TBool Priority( ) const;
    IMPORT_C void SetPriority (TBool aFlag);

    IMPORT_C TBool HideNumber( ) const;
    IMPORT_C void SetHideNumber(TBool aFlag);

    IMPORT_C TBool ReadReport( ) const;
    IMPORT_C void SetReadReport(TBool aFlag);

    IMPORT_C TBool DeliveryReport( ) const;
    IMPORT_C void SetDeliveryReport(TBool aFlag);

    IMPORT_C TBool ReplyWithHistory( ) const;
    IMPORT_C void SetReplyWithHistory (TBool aFlag);

    IMPORT_C TBool AllowDeliveryNotification( ) const;
    IMPORT_C void SetAllowDeliveryNotification(TBool aFlag);

    IMPORT_C TBool FilterAdvertisements( ) const;
    IMPORT_C void SetFilterAdvertisements(TBool aFlag);

    IMPORT_C TInt MaxDownloadRetries( ) const;
    IMPORT_C void SetMaxDownloadRetries(const TInt aRetries);

    IMPORT_C TInt DownloadRetryInterval( ) const;
    IMPORT_C void SetDownloadRetryInterval(const TInt aInterval);

    IMPORT_C TInt MaxSendMsgSize( ) const;
    IMPORT_C void SetMaxSendMsgSize (const TInt aSize);

    IMPORT_C TBool FullScreenPreview( ) const;
    IMPORT_C void SetFullScreenPreview(TBool aFlag);

    IMPORT_C TInt DeviceContentClass( ) const;
    IMPORT_C void SetDeviceContentClass(const TInt aClass);

    IMPORT_C TInt MaxImageHeight( ) const;
    IMPORT_C void SetMaxImageHeight(const TInt aHeight);

    IMPORT_C TInt MaxImageWidth( ) const;
    IMPORT_C void SetMaxImageWidth(const TInt aWidth);   
    
	void RemoveAllProxies();
	void RemoveAllNapIds();
	
	TUint32 MmsSettingsFlags() const;
	void SetMmsSettingsFlags(TUint32 aFlags);
	    
private:
    CMmsSettings();

	enum TMmsSettingsFlags
		{
		ESmsSettingsNoFlags					= 0x00000000,
		ESmsSettingsDisplayNotification		= 0x00000002,
		ESmsSettingsCreationModeReadOnly	= 0x00000004,
		ESmsSettingsPriority				= 0x00000008,
		ESmsSettingsHideNumber				= 0x00000010,
		ESmsSettingsReadReport				= 0x00000020,
		ESmsSettingsDeliveryReport			= 0x00000040,
		ESmsSettingsReplyWithHistory		= 0x00000080,
		ESmsSettingsAllowDeliveryNotification	= 0x00000100,
		ESmsSettingsFilterAdvertisements	= 0x00000200,
		ESmsSettingsFullScreenPreview		= 0x00000400
		};

private:

	TUint32 iMmsSettingsFlags;
	TInt iApplicationId;
	HBufC* iAddress;
	TCreationMode iCreationMode;
	RArray<TUid> iProxy;
	RArray<TUid> iNapId;
	TAutomaticDownloadOptions iAutomaticDownloadOptions;
	TInt iValidityPeriod;
	TInt iMaxDownloadSize;
	TInt iMaxDownloadRetries;
	TInt iDownloadRetryInterval;
	TInt iMaxSendMsgSize;
	TInt iDeviceContentClass;
	TInt iMaxImageHeight;
	TInt iMaxImageWidth;
   	};
   	
#endif // __CSMSACCOUNT_H__
