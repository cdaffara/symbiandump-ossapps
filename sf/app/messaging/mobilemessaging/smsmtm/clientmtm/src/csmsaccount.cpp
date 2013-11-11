// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <msvuids.h>
#include <msvids.h>
#include <msvschedulesettings.h>
#include <msvoffpeaktime.h>
#include <msvsenderroraction.h>
#include <msvsysagentaction.h>
#include <centralrepository.h>
#include <tmsvschedulesettingsutils.h>

#include "SMUTSET.H"
#include <csmsaccount.h>
#include <smut.h>

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	#include <msvapi.h>
#endif

const TUint32 KIncrementSC = 0x00000010;
const TInt KMaxSettingLength = 256;
const TInt KMaxServiceCenterId = 256;
const TInt KSmsSettingsVersion			= 1;
const TInt KSmsMessageSettingsVersion	= 1;


/** 
Allocates and constructs an SMS account object.

Initialises all member data to their default values.

@return
The newly constructed SMS account object.
*/
EXPORT_C CSmsAccount* CSmsAccount::NewL()
	{
	CSmsAccount* self = CSmsAccount::NewLC();
	CleanupStack::Pop(self);	
	return self;
	}

/** 
Allocates and constructs an SMS account object.

Initialises all member data to their default values.

@return
The newly constructed SMS account object.
*/
EXPORT_C CSmsAccount* CSmsAccount::NewLC()
	{
	CSmsAccount* self = new (ELeave) CSmsAccount();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Second phase construction.
*/
void CSmsAccount::ConstructL()
	{
	iRepository = CRepository::NewL(KUidMsgTypeSMS);
	}

/**
Constructor.
*/
CSmsAccount::CSmsAccount()
	{
	}

/** 
Destructor. 
*/
EXPORT_C CSmsAccount::~CSmsAccount()
	{
	delete iRepository;
	delete iMsvSession;
	}

/**
Initialises the SMS settings in CenRep:

1. Creates a SMS service entry in the message store

2. Resets the settings in CenRep to their default values

3. Populates aSmsSettings with default settings

@param	aSmsSettings
SMS service settings
*/
EXPORT_C void CSmsAccount::InitialiseDefaultSettingsL(CSmsSettings& aSmsSettings)
	{
	ResetSettings();
	CreateSmsServiceL();	
	LoadSettingsL(aSmsSettings);
	}

/**
Initialises the SMS settings in CenRep:

1. Creates a SMS service entry in the message store

2. Resets the settings in CenRep to their default values

3. Populates the supplied objects with default settings

@param	aScheduleSettings
Schedule settings

@param	aOffPeakTimes
Off Peak Times

@param	aErrorActions
Send Error Actions

@param	aSysAgentActions
System Agent Actions
*/
EXPORT_C void CSmsAccount::InitialiseDefaultSettingsL(CMsvScheduleSettings& aScheduleSettings, CMsvOffPeakTimes& aOffPeakTimes, CMsvSendErrorActions& aErrorActions, CMsvSysAgentActions& aSysAgentActions)
	{
	ResetSettings();
	CreateSmsServiceL();	
	LoadSettingsL(aScheduleSettings, aOffPeakTimes, aErrorActions, aSysAgentActions);
	}

/**
Creates SMS service entry in the message store
*/
TMsvId CSmsAccount::CreateSmsServiceL()
    {
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	CreateHeaderStoreL();	
#endif

    TMsvId serviceEntryId = KMsvNullIndexEntryId;
    CMsvEntry* root = SessionL().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(root);
    
    TRAPD(error, TSmsUtilities::ServiceIdL(*root, serviceEntryId));
    if (error == KErrNotFound)
        {
        TBuf<KMaxSettingLength> accountName;
        User::LeaveIfError(iRepository->Get(ESmsAccountNameId, accountName));
        
	    TMsvEntry entry;
	    entry.iMtm = KUidMsgTypeSMS;
	    entry.iType = KUidMsvServiceEntry;
	    entry.SetReadOnly(EFalse);
        entry.SetVisible(EFalse);
        entry.iDate.UniversalTime();
        entry.iDetails.Set(accountName);
        root->SetEntryL(KMsvRootIndexEntryId);
        root->CreateL(entry);
        serviceEntryId = entry.Id();
        }
    else
        {
		User::LeaveIfError(error);
        }
            
    CleanupStack::PopAndDestroy(root);    
        		
	User::LeaveIfError(iRepository->Set(ESmsServiceId, static_cast<TInt>(serviceEntryId)));		
    return serviceEntryId;
    }
    
/**
Resets the settings in CenRep to their default values.
*/
void CSmsAccount::ResetSettings()
	{
	iRepository->Reset();
	}	

/**
Loads SMS Service settings from CenRep:

@param	aSmsSettings
Sms service settings
*/
EXPORT_C void CSmsAccount::LoadSettingsL(CSmsSettings& aSmsSettings)
	{	
	TInt temp = 0;
				
	//Load SMS Message Settings	
	User::LeaveIfError(iRepository->Get(ESmsValidityPeriodId, temp));
	aSmsSettings.SetValidityPeriod(TTimeIntervalMinutes(temp));

	User::LeaveIfError(iRepository->Get(ESmsValidityPeriodFormatId, temp));
	aSmsSettings.SetValidityPeriodFormat(static_cast<TSmsFirstOctet::TSmsValidityPeriodFormat>(temp));

	User::LeaveIfError(iRepository->Get(ESmsAlphabetId, temp));
	aSmsSettings.SetCharacterSet(static_cast<TSmsDataCodingScheme::TSmsAlphabet>(temp));

	User::LeaveIfError(iRepository->Get(ESmsMsgFlagsId, temp));
	aSmsSettings.SetMessageFlags(static_cast<TUint32>(temp));

	User::LeaveIfError(iRepository->Get(ESmsMessageConversionId, temp));
	aSmsSettings.SetMessageConversion(static_cast<TSmsPIDConversion>(temp));

		
	// Load SMS Service Settings
	User::LeaveIfError(iRepository->Get(ESmsFlagsId, temp));
	aSmsSettings.SetSettingsFlags(static_cast<TUint32>(temp));

	User::LeaveIfError(iRepository->Get(ESmsStatusReportHandlingId, temp));
	aSmsSettings.SetStatusReportHandling(static_cast<CSmsSettings::TSmsReportHandling>(temp));

	User::LeaveIfError(iRepository->Get(ESmsSpecialMessageHandlingId, temp));
	aSmsSettings.SetSpecialMessageHandling(static_cast<CSmsSettings::TSmsReportHandling>(temp));

	User::LeaveIfError(iRepository->Get(ESmsCommDbActionId, temp));
	aSmsSettings.SetCommDbAction(static_cast<CSmsSettings::TSmsSettingsCommDbAction>(temp));

	User::LeaveIfError(iRepository->Get(ESmsDeliveryId, temp));
	aSmsSettings.SetDelivery(static_cast<TSmsDelivery>(temp));
	
	aSmsSettings.RemoveSCAddresses();
	User::LeaveIfError(iRepository->Get(ESmsSCAddressesCountId, temp));
	TInt count = temp;

	TUint32 scId = ESmsSCAddressesPartialId;	// 0x1000
	TUint32 scFieldId = 0;
	TBuf<KMaxSettingLength> scNumber;
	TBuf<KMaxSettingLength> scName;
	while(count--)
		{
		scFieldId = scId;			// 0x0 - address field
		User::LeaveIfError(iRepository->Get(scFieldId, scNumber));

		++scFieldId;	// 0x1 - name field, increment the last bit		
		User::LeaveIfError(iRepository->Get(scFieldId, scName));				
		
		aSmsSettings.AddServiceCenterL(scName, scNumber);   
		scId += KIncrementSC; // get the next SC in the list
		}

	if (aSmsSettings.ServiceCenterCount()>0)
		{		
		User::LeaveIfError(iRepository->Get(ESmsDefaultSCId, temp));
		aSmsSettings.SetDefaultServiceCenter(temp);
		}
						
	User::LeaveIfError(iRepository->Get(ESmsBearerActionId, temp));
	aSmsSettings.SetSmsBearerAction(static_cast<CSmsSettings::TSmsSettingsCommDbAction>(temp));

	User::LeaveIfError(iRepository->Get(ESmsBearerId, temp));
	aSmsSettings.SetSmsBearer(static_cast<CSmsSettings::TMobileSmsBearer>(temp));

	User::LeaveIfError(iRepository->Get(ESmsClass2FolderId, temp));
	aSmsSettings.SetClass2Folder(static_cast<TMsvId>(temp));

	User::LeaveIfError(iRepository->Get(ESmsDescriptionLengthId, temp));
	aSmsSettings.SetDescriptionLength(temp);
	TInt err = iRepository->Get(EMsgSMSTimeStampSettings, temp);
	if(KErrNone == err )
 		{
 		aSmsSettings.SetUseServiceCenterTimeStampForDate(temp);
 		}
	}

/**
Loads Schedule Send settings from CenRep:

@param	aScheduleSettings
Schedule settings

@param	aOffPeakTimes
Off Peak Times

@param	aErrorActions
Send Error Actions

@param	aSysAgentActions
System Agent Actions
*/
EXPORT_C void CSmsAccount::LoadSettingsL(CMsvScheduleSettings& aScheduleSettings, CMsvOffPeakTimes& aOffPeakTimes, CMsvSendErrorActions& aErrorActions, CMsvSysAgentActions& aSysAgentActions)
	{

	TMsvScheduleSettingsUtils::LoadScheduleSettingsL(aScheduleSettings, *iRepository);
	TMsvScheduleSettingsUtils::LoadOffPeakSettingsL(aOffPeakTimes, *iRepository);
	TMsvScheduleSettingsUtils::LoadSendErrorSettingsL(aErrorActions, *iRepository);
	TMsvScheduleSettingsUtils::LoadSysAgentSettingsL(aSysAgentActions, *iRepository);	
	}

/**
Saves SMS Service settings to CenRep:

@param	aSmsSettings
Sms service settings
*/
EXPORT_C void CSmsAccount::SaveSettingsL(const CSmsSettings& aSmsSettings) const
	{

	User::LeaveIfError(iRepository->StartTransaction(CRepository::EReadWriteTransaction));
	iRepository->CleanupRollbackTransactionPushL();
	iRepository->Set(ESmsSettingsVersionId, KSmsSettingsVersion);  // Errors are ignored as the CenRep transaction fails on error.
	
	// Save SMS Message Settings¬
	iRepository->Set(ESmsSettingsMesssageVersionId, KSmsMessageSettingsVersion);
	iRepository->Set(ESmsValidityPeriodId, aSmsSettings.ValidityPeriod().Int());
	iRepository->Set(ESmsValidityPeriodFormatId, static_cast<TInt>(aSmsSettings.ValidityPeriodFormat()));
	iRepository->Set(ESmsAlphabetId, static_cast<TInt>(aSmsSettings.CharacterSet()));
	iRepository->Set(ESmsMsgFlagsId, static_cast<TInt>(aSmsSettings.MessageFlags()));
	iRepository->Set(ESmsMessageConversionId, static_cast<TInt>(aSmsSettings.MessageConversion()));
	

	// Save SMS Service Settings
	iRepository->Set(ESmsFlagsId, static_cast<TInt>(aSmsSettings.SettingsFlags()));
	iRepository->Set(ESmsStatusReportHandlingId, static_cast<TInt>(aSmsSettings.StatusReportHandling()));	
	iRepository->Set(ESmsSpecialMessageHandlingId, static_cast<TInt>(aSmsSettings.SpecialMessageHandling()));		
	iRepository->Set(ESmsCommDbActionId, static_cast<TInt>(aSmsSettings.CommDbAction()));
	iRepository->Set(ESmsDeliveryId, static_cast<TInt>(aSmsSettings.Delivery()));
	iRepository->Set(ESmsDefaultSCId, aSmsSettings.DefaultServiceCenter());

	TInt count = aSmsSettings.ServiceCenterCount();	
	__ASSERT_ALWAYS(count<=KMaxServiceCenterId, User::Leave(KErrOverflow));
	iRepository->Set(ESmsSCAddressesCountId, count);

	// Save list of SC
	TUint32 scId = ESmsSCAddressesPartialId;	// 0x1000
	TUint32 scFieldId = 0;
	for(TInt loop=0; loop < count; ++loop)
		{
		scFieldId = scId;				// 0x0 - address field
		iRepository->Set(scFieldId, aSmsSettings.GetServiceCenter(loop).Address());

		++scFieldId;		// 0x1 - name field, increment the last bit
		iRepository->Set(scFieldId, aSmsSettings.GetServiceCenter(loop).Name());
				
		scId += KIncrementSC;		// get the next SC in the list
		}

	iRepository->Set(ESmsBearerActionId, static_cast<TInt>(aSmsSettings.SmsBearerAction()));
	iRepository->Set(ESmsBearerId, static_cast<TInt>(aSmsSettings.SmsBearer()));
	iRepository->Set(ESmsClass2FolderId, static_cast<TInt>(aSmsSettings.Class2Folder()));
	iRepository->Set(ESmsDescriptionLengthId, aSmsSettings.DescriptionLength());

	TInt temp = 0;
 	TInt err = iRepository->Get(EMsgSMSTimeStampSettings, temp);
 	if(KErrNone == err )
 		{
 		temp =  (aSmsSettings.UseServiceCenterTimeStampForDate()) ? 1: 0;
		iRepository->Set(EMsgSMSTimeStampSettings,temp);
 		}
	CleanupStack::Pop();
	TUint32 errorId = 0;
	User::LeaveIfError(iRepository->CommitTransaction(errorId));
	
	aSmsSettings.StoreToCommDbL();
	}

/**
Saves Schedule Send settings to CenRep:

@param	aScheduleSettings
Schedule settings

@param	aOffPeakTimes
Off Peak Times

@param	aErrorActions
Send Error Actions

@param	aSysAgentActions
System Agent Actions
*/
EXPORT_C void CSmsAccount::SaveSettingsL(const CMsvScheduleSettings& aScheduleSettings, const CMsvOffPeakTimes& aOffPeakTimes, const CMsvSendErrorActions& aErrorActions, const CMsvSysAgentActions& aSysAgentActions) const
	{
	
	User::LeaveIfError(iRepository->StartTransaction(CRepository::EReadWriteTransaction));
	iRepository->CleanupRollbackTransactionPushL();

	TMsvScheduleSettingsUtils::SaveScheduleSettingsL(aScheduleSettings, *iRepository);
	TMsvScheduleSettingsUtils::SaveOffPeakSettingsL(aOffPeakTimes, *iRepository);
	TMsvScheduleSettingsUtils::SaveSendErrorSettingsL(aErrorActions, *iRepository);
	TMsvScheduleSettingsUtils::SaveSysAgentSettingsL(aSysAgentActions, *iRepository);	

	CleanupStack::Pop();
	TUint32 errorId = 0;
	User::LeaveIfError(iRepository->CommitTransaction(errorId));
	}

CMsvSession& CSmsAccount::SessionL()
	{
	if (iMsvSession == NULL)
		{
		iMsvSession = CMsvSession::OpenSyncL(*this);		
		}
		
	return *iMsvSession;	
	}

void CSmsAccount::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* , TAny* , TAny* ) 
    {
  	if (aEvent == EMsvCloseSession)
    	{
		delete iMsvSession;
		iMsvSession = NULL;
    	}    	
    }


#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
/**
* Send the header structure to message Server to create  header table.
* @param  None.
* @return  None.
*/
void CSmsAccount::CreateHeaderStoreL()
	{
	CMsvHeaderStore* aHeader = CMsvHeaderStore::NewL(KUidMsgTypeSMS ,SessionL());
	CleanupStack::PushL(aHeader);
	
	_LIT (KSmsHeaderversion , "SmsHeadearversion");
	aHeader->AddFieldL(KSmsHeaderversion().AllocL(),EIntegerField);

	_LIT (KSmsRecipientCount , "RecipientCount");
	aHeader->AddFieldL(KSmsRecipientCount().AllocL(),EIntegerField);
	
	_LIT (KRecipients , "To");
	aHeader->AddFieldL(KRecipients().AllocL(),ETextField);
	
	_LIT (KSmsFlags , "SmsFlags");
	aHeader->AddFieldL(KSmsFlags().AllocL(),EIntegerField);
			
	_LIT (KTextBioMsgIdType,"BioMsgIdType");
	aHeader->AddFieldL(KTextBioMsgIdType().AllocL(),EIntegerField);
	
	aHeader->CreateStoreL();
	CleanupStack::PopAndDestroy(aHeader);  //aHeader
	}
#endif
