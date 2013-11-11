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



#include <cmmssettings.h>



/** 
Allocates and constructs an MMS settings object.

Initialises all member data to their default values.

@return
The newly constructed MMS settings object.
*/
EXPORT_C CMmsSettings* CMmsSettings::NewL()
	{
	CMmsSettings* self = CMmsSettings::NewLC();
	CleanupStack::Pop(self); 
	return self;
	}

/** 
Allocates and constructs an MMS settings object.

Initialises all member data to their default values.

@return
The newly constructed MMS settings object.
*/
EXPORT_C CMmsSettings* CMmsSettings::NewLC()
	{
	CMmsSettings* self = new (ELeave) CMmsSettings();
	CleanupStack::PushL(self);
	return self;
	}

/**
Constructor.
*/
CMmsSettings::CMmsSettings()
:	iMmsSettingsFlags(0),
	iApplicationId(0),
	iCreationMode(ECreationModeFree),
	iAutomaticDownloadOptions(EAutomaticDownloadOff),
	iValidityPeriod(0),
	iMaxDownloadSize(0),
	iMaxDownloadRetries(0),
	iDownloadRetryInterval(0),
	iMaxSendMsgSize(0),
	iDeviceContentClass(0),
	iMaxImageHeight(0),
	iMaxImageWidth(0)
	{		
	}

/** 
Destructor. 
*/
EXPORT_C CMmsSettings::~CMmsSettings()
	{
	delete iAddress;
	iProxy.Close();
	iNapId.Close();
	}

/**
Returns the Application Id.

@return
Application Id
*/	
EXPORT_C TInt CMmsSettings::ApplicationID( ) const
	{
	return iApplicationId;
	}

/**
Sets the Application Id.

@param	aAppID
Application Id
*/		
EXPORT_C void CMmsSettings::SetApplicationID(const TInt aAppID)
	{
	iApplicationId = aAppID;
	}

/**
Returns the absolute MMS Proxy-Relay URL

@return
The absolute MMS Proxy-Relay URL
*/	
EXPORT_C TPtrC CMmsSettings::Address( ) const
	{
	if (iAddress)
		{
		return TPtrC(*iAddress);
		}
		
	return TPtrC();
	}

/**
Sets the absolute MMS Proxy-Relay URL

@param	aAddress
The absolute MMS Proxy-Relay URL
*/		
EXPORT_C void CMmsSettings::SetAddressL(const TDesC& aAddress)
	{
	HBufC* newAddress = aAddress.AllocL();
	delete iAddress;
	iAddress = newAddress;
	}

/**
Returns the Creation Mode.
Creation Mode specifies how an MMS client should allow insertion of media
items in an MMS.

@see TCreationMode

@return
The Creation Mode.
*/		
EXPORT_C TCreationMode CMmsSettings::CreationMode( ) const
	{
	return iCreationMode;
	}

/**
Sets the Creation Mode.
Creation Mode specifies how an MMS client should allow insertion of media
items in an MMS.

@see TCreationMode

@param	aCreationMode
The Creation Mode.
*/			
EXPORT_C void CMmsSettings::SetCreationModeL(TCreationMode aCreationMode)
	{
	iCreationMode = aCreationMode;
	}

/** 
Gets the number of Proxies stored in this object.

@return
The number of Proxies stored.
*/
EXPORT_C TInt CMmsSettings::ProxyCount( ) const
	{
	return iProxy.Count();
	}

/** 
Gets the Proxy at the specified index.

@param	aIndex
Index

@return
Proxy
*/	
EXPORT_C TUid CMmsSettings::GetProxy(TInt aIndex) const
	{
	return iProxy[aIndex];
	}

/** 
Appends a Proxy to the list.

@param	aProxy
Proxy to be added to the list
*/		
EXPORT_C void CMmsSettings::AddProxyL(const TUid aProxy)
	{
	iProxy.AppendL(aProxy);
	}

/** 
Removes a Proxy from the list.

@param	aIndex
Index
*/			
EXPORT_C void CMmsSettings::RemoveProxy(TInt aIndex)
	{
	iProxy.Remove(aIndex);
	}

/** 
Gets the number of Nap Ids stored in this object.

@return
The number of Nap Ids stored.
*/	
EXPORT_C TInt CMmsSettings::NapIdCount( ) const
	{
	return iNapId.Count();	
	}

/** 
Gets the Nap Id at the specified index.

@param	aIndex
Index

@return
Nap Id
*/		
EXPORT_C TUid CMmsSettings::GetNapId (TInt aIndex) const
	{
	return iNapId[aIndex];	
	}

/** 
Appends a Nap Id to the list.

@param	aNapId
Nap Id to be added to the list
*/			
EXPORT_C void CMmsSettings::AddNapIdL(const TUid aNapId)
	{
	iNapId.AppendL(aNapId);	
	}

/** 
Removes a Nap Id from the list.

@param	aIndex
Index
*/				
EXPORT_C void CMmsSettings::RemoveNapId(TInt aIndex)
	{
	iNapId.Remove(aIndex);	
	}

/**
Returns whether the Notification Receipt should be displayed immediately to the user.

@return
Returns ETrue if the Notification Receipt should be displayed to the user.
*/	
EXPORT_C TBool CMmsSettings::DisplayNotification( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsDisplayNotification)==ESmsSettingsDisplayNotification;
	}

/**
Specifies whether the Notification Receipt should be displayed immediately to the user.

@param aFlag
Specify ETrue if the Notification Receipt should be displayed to the user.
*/		
EXPORT_C void CMmsSettings::SetDisplayNotification(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsDisplayNotification) | (aFlag ? ESmsSettingsDisplayNotification : ESmsSettingsNoFlags);
	}

/**
Returns whether the message should be downloaded automatically.

@see TAutomaticDownloadOptions

@return
The automatic download option
*/		
EXPORT_C TAutomaticDownloadOptions CMmsSettings::AutomaticDownload( ) const
	{
	return iAutomaticDownloadOptions;
	}

/**
Sets the automatic download option

@see TAutomaticDownloadOptions

@param aDownloadOptions
Automatic download option to be set
*/			
EXPORT_C void CMmsSettings::SetAutomaticDownload(TAutomaticDownloadOptions aDownloadOptions)
	{
	iAutomaticDownloadOptions = aDownloadOptions;
	}

/**
Returns whether the user allowed to modify the creation mode or not.

@return
Returns ETrue if the user is allowed to modify the creation mode.
*/	
EXPORT_C TBool CMmsSettings::CreationModeReadOnly( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsCreationModeReadOnly)==ESmsSettingsCreationModeReadOnly;	
	}

/**
Specifies whether the user allowed to modify the creation mode or not.

@param aFlag
Specify ETrue if the user is allowed to modify the creation mode.
*/			
EXPORT_C void CMmsSettings::SetCreationModeReadOnly(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsCreationModeReadOnly) | (aFlag ? ESmsSettingsCreationModeReadOnly : ESmsSettingsNoFlags);	
	}

/**
Returns the Validity Period of the MMS message.

@return
Validity Period of the message
*/		
EXPORT_C TInt CMmsSettings::ValidityPeriod( ) const
	{
	return iValidityPeriod;	
	}
	
/**
Sets the Validity Period of the MMS message.

@param	aValidityPeriod
Validity Period of the message
*/			
EXPORT_C void CMmsSettings::SetValidityPeriod(const TInt aValidityPeriod)
	{
	iValidityPeriod = aValidityPeriod;
	}

/**
Returns the maximum size of the MMS message that can be downloaded automatically.

@return
Maximum size of the message that can be downloaded automatically
*/			
EXPORT_C TInt CMmsSettings::MaxDownloadSize( ) const
	{
	return iMaxDownloadSize;
	}

/**
Sets the maximum size of the MMS message that can be downloaded automatically.

@param	aSize
Maximum size of the message that can be downloaded automatically
*/			
EXPORT_C void CMmsSettings::SetMaxDownloadSize (const TInt aSize)
	{
	iMaxDownloadSize = aSize;
	}

/**
Returns the Priority value of the MMS message.

@return
Priority value
*/		
EXPORT_C TBool CMmsSettings::Priority( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsPriority)==ESmsSettingsPriority;		
	}

/**
Sets the Priority value of the MMS message.

@param	aFlag
Priority value
*/				
EXPORT_C void CMmsSettings::SetPriority (TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsPriority) | (aFlag ? ESmsSettingsPriority : ESmsSettingsNoFlags);		
	}

/**
Returns whether the user's number should be hidden when sending an MMS.

@return
Returns ETrue if the user's number should be hidden.
*/	
EXPORT_C TBool CMmsSettings::HideNumber( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsHideNumber)==ESmsSettingsHideNumber;			
	}
	
/**
Specifies whether the user's number should be hidden when sending an MMS.

@param aFlag
Specify ETrue if the user's number should be hidden.
*/		
EXPORT_C void CMmsSettings::SetHideNumber(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsHideNumber) | (aFlag ? ESmsSettingsHideNumber : ESmsSettingsNoFlags);			
	}

/**
Returns whether the Read Reports should be sent back to the network when the user receives an MMS.

@return
Returns ETrue if the Read Reports should be sent back to the network
*/	
EXPORT_C TBool CMmsSettings::ReadReport( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsReadReport)==ESmsSettingsReadReport;				
	}
	
/**
Specifies whether the Read Reports should be sent back to the network when the user receives an MMS.

@param aFlag
Specify ETrue if the Read Reports should be sent back to the network
*/		
EXPORT_C void CMmsSettings::SetReadReport(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsReadReport) | (aFlag ? ESmsSettingsReadReport : ESmsSettingsNoFlags);				
	}

/**
Returns whether the Delivery Reports should be sent back to the network when the user receives an MMS.

@return
Returns ETrue if the Delivery Reports should be sent back to the network
*/	
EXPORT_C TBool CMmsSettings::DeliveryReport( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsDeliveryReport)==ESmsSettingsDeliveryReport;					
	}
	
/**
Specifies whether the Delivery Reports should be sent back to the network when the user receives an MMS.

@param aFlag
Specify ETrue if the Delivery Reports should be sent back to the network
*/		
EXPORT_C void CMmsSettings::SetDeliveryReport(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsDeliveryReport) | (aFlag ? ESmsSettingsDeliveryReport : ESmsSettingsNoFlags);					
	}

/**
Returns whether the original MMS should be included when replying to an MMS

@return
Returns ETrue if the original MMS should be included when replying to an MMS
*/	
EXPORT_C TBool CMmsSettings::ReplyWithHistory( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsReplyWithHistory)==ESmsSettingsReplyWithHistory;						
	}
	
/**
Specifies whether the original MMS should be included when replying to an MMS

@param aFlag
Specify ETrue if the original MMS should be included when replying to an MMS
*/		
EXPORT_C void CMmsSettings::SetReplyWithHistory (TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsReplyWithHistory) | (aFlag ? ESmsSettingsReplyWithHistory : ESmsSettingsNoFlags);						
	}

/**
Returns the Allow Delivery Notification value

@return
Allow Delivery Notification value
*/	
EXPORT_C TBool CMmsSettings::AllowDeliveryNotification( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsAllowDeliveryNotification)==ESmsSettingsAllowDeliveryNotification;							
	}
	
/**
Sets the Allow Delivery Notification value

@param	aFlag
Allow Delivery Notification value
*/			
EXPORT_C void CMmsSettings::SetAllowDeliveryNotification(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsAllowDeliveryNotification) | (aFlag ? ESmsSettingsAllowDeliveryNotification : ESmsSettingsNoFlags);							
	}

/**
Returns whether to filter advertisment MMS messages when downloading automatically.

@return
Returns ETrue if advertisments should be filtered when downloading automatically.
*/	
EXPORT_C TBool CMmsSettings::FilterAdvertisements( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsFilterAdvertisements)==ESmsSettingsFilterAdvertisements;								
	}
	
/**
Specifies whether to filter advertisment MMS messages when downloading automatically.

@param aFlag
Specify ETrue if advertisments should be filtered when downloading automatically.
*/		
EXPORT_C void CMmsSettings::SetFilterAdvertisements(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsFilterAdvertisements) | (aFlag ? ESmsSettingsFilterAdvertisements : ESmsSettingsNoFlags);								
	}

/**
Returns the maximum download retries of an MMS message

@return
Maximum download retries
*/		
EXPORT_C TInt CMmsSettings::MaxDownloadRetries( ) const
	{
	return iMaxDownloadRetries;
	}
	
/**
Sets the maximum download retries of an MMS message

@param	aRetries
Maximum download retries
*/				
EXPORT_C void CMmsSettings::SetMaxDownloadRetries(const TInt aRetries)
	{
	iMaxDownloadRetries = aRetries;
	}

/**
Returns the download retry interval

@return
Download retry interval
*/		
EXPORT_C TInt CMmsSettings::DownloadRetryInterval( ) const
	{
	return iDownloadRetryInterval;
	}
	
/**
Sets the download retry interval

@param	aInterval
Download retry interval
*/			
EXPORT_C void CMmsSettings::SetDownloadRetryInterval(const TInt aInterval)
	{
	iDownloadRetryInterval = aInterval;
	}

/**
Returns the maximum MMS message size that can be sent

@return
The maximum MMS message size that can be sent
*/		
EXPORT_C TInt CMmsSettings::MaxSendMsgSize( ) const
	{
	return iMaxSendMsgSize;
	}

/**
Sets the maximum MMS message size that can be sent

@param	aSize
The maximum MMS message size that can be sent
*/				
EXPORT_C void CMmsSettings::SetMaxSendMsgSize (const TInt aSize)
	{
	iMaxSendMsgSize = aSize;
	}

/**
Returns whether to display MMS messages in full screen.

@return
Returns ETrue if MMS messages should be displayed in full screen.
*/	
EXPORT_C TBool CMmsSettings::FullScreenPreview( ) const
	{
	return (iMmsSettingsFlags & ESmsSettingsFullScreenPreview)==ESmsSettingsFullScreenPreview;								
	}
	
/**
Specifies whether to display MMS messages using full screen.

@param aFlag
Specify ETrue if MMS messages should be displayed in full screen.
*/		
EXPORT_C void CMmsSettings::SetFullScreenPreview(TBool aFlag)
	{
	iMmsSettingsFlags = (iMmsSettingsFlags & ~ESmsSettingsFullScreenPreview) | (aFlag ? ESmsSettingsFullScreenPreview : ESmsSettingsNoFlags);	
	}

/**
Returns the content class of the device

@return
The content class of the device
*/		
EXPORT_C TInt CMmsSettings::DeviceContentClass( ) const
	{
	return iDeviceContentClass;
	}

/**
Sets the content class of the device

@param	aClass
The content class of the device
*/					
EXPORT_C void CMmsSettings::SetDeviceContentClass(const TInt aClass)
	{
	iDeviceContentClass = aClass;
	}

/**
Returns the maximum image height to be used when sending an MMS message

@return
The maximum image height
*/		
EXPORT_C TInt CMmsSettings::MaxImageHeight( ) const
	{
	return iMaxImageHeight;
	}

/**
Sets the maximum image height to be used when sending an MMS message

@param	aHeight
The maximum image height
*/						
EXPORT_C void CMmsSettings::SetMaxImageHeight(const TInt aHeight)
	{
	iMaxImageHeight = aHeight;
	}

/**
Returns the maximum image width to be used when sending an MMS message

@return
The maximum image width
*/		
EXPORT_C TInt CMmsSettings::MaxImageWidth( ) const
	{
	return iMaxImageWidth;
	}

/**
Sets the maximum image width to be used when sending an MMS message

@param	aWidth
The maximum image width
*/							
EXPORT_C void CMmsSettings::SetMaxImageWidth(const TInt aWidth)
	{
	iMaxImageWidth = aWidth;
	}

/**
Copies the MMS settings stored in the specified object into this object.

@param	aMmsSettings
The MMS Settings to be copied.
*/
EXPORT_C void CMmsSettings::CopyL(const CMmsSettings& aMmsSettings)
	{
	iMmsSettingsFlags		= aMmsSettings.iMmsSettingsFlags;
	iApplicationId 			= aMmsSettings.iApplicationId;
	
	delete iAddress;
    iAddress = NULL;
	iAddress	=	aMmsSettings.Address().AllocL();
	
	iCreationMode 			= aMmsSettings.iCreationMode;
	
	TInt count = aMmsSettings.ProxyCount();
	iProxy.Reset();
	for(TInt loop=0; loop < count; ++loop)
		{
		iProxy.AppendL(aMmsSettings.GetProxy(loop));
		}

	count = aMmsSettings.NapIdCount();
	iNapId.Reset();
	for(TInt loop=0; loop < count; ++loop)
		{
		iNapId.AppendL(aMmsSettings.GetNapId(loop));
		}

	iAutomaticDownloadOptions = aMmsSettings.iAutomaticDownloadOptions;
    iValidityPeriod 		= aMmsSettings.iValidityPeriod;
	iMaxDownloadSize 		= aMmsSettings.iMaxDownloadSize;
	iMaxDownloadRetries 	= aMmsSettings.iMaxDownloadRetries;
	iDownloadRetryInterval 	= aMmsSettings.iDownloadRetryInterval;
	iMaxSendMsgSize 		= aMmsSettings.iMaxSendMsgSize;
	iDeviceContentClass 	= aMmsSettings.iDeviceContentClass;
	iMaxImageHeight 		= aMmsSettings.iMaxImageHeight;
	iMaxImageWidth			= aMmsSettings.iMaxImageWidth;	
	}

void CMmsSettings::RemoveAllProxies()
	{
	iProxy.Reset();
	}

void CMmsSettings::RemoveAllNapIds()
	{
	iNapId.Reset();	
	}
	
/**
Returns MMS settings flag

@return
MMS settings flag
*/     
TUint32 CMmsSettings::MmsSettingsFlags() const
	{
	return iMmsSettingsFlags;
	}

/**
Sets MMS settings flag

@param aMsgFlags
MMS settings flag
*/   
void CMmsSettings::SetMmsSettingsFlags(TUint32 aFlags)
	{
	iMmsSettingsFlags = aFlags;
	}
