// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <btsdp.h>

#include "pbapserver.h"
#include "btaccesshostlog.h"

// PBAP Profile Description UUID
const TUint16 KPbapProfileUUID	= 0x1130;

const TUint16 KPbapProfileVersion			= 0x0100;
const TUint8 KPbapLocalPhonebookRepository 	= 0x01;

const TSdpAttributeID KPbapSdpAttrIdSupportedRepositories = 0x0314;

_LIT8(KPbapProviderName,	"Symbian OS");
_LIT8(KPbapServiceName,		"Phonebook access PSE");



void CPbapServer::RegisterWithSdpL(TUint aRfcommChannel)
	{
	LOG_FUNC
	// must not call this function repeatedly without freeing record between calls
	if(iSdpRecordHandle)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicSDPRecordExists));
		User::Leave(KErrAlreadyExists);
		}
	if (iSdpRecordHandle)
		{
		return;
		}
		
	// connect to SDP server and database
	User::LeaveIfError(iSdpServer.Connect());
	User::LeaveIfError(iSdpDatabase.Open(iSdpServer));
	
	// create the record
	iSdpDatabase.CreateServiceRecordL(TUUID(KPbapPseUUID), iSdpRecordHandle);
		
	CSdpAttrValue* attrVal;
	CSdpAttrValueDES* attrValDES;

	// protocol Descriptor List
	attrValDES = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(attrValDES);		
	attrValDES
		->StartListL()
		->BuildDESL()
			->StartListL()
			->BuildUUIDL(TUUID(KL2CAPUUID))
			->EndListL()
		->BuildDESL()
			->StartListL()
			->BuildUUIDL(TUUID(KRFCommUUID))
			->BuildUintL(TSdpIntBuf<TUint8>(aRfcommChannel)) // channel read from obex
			->EndListL()
		->BuildDESL()
			->StartListL()
			->BuildUUIDL(TUUID(KObexProtocolUUID))
			->EndListL()	
		->EndListL();
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
	CleanupStack::PopAndDestroy(attrValDES);


	// language
	attrValDES = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
		->BuildUintL(TSdpIntBuf<TUint16>(KLanguageEnglish))
		->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdCharacterEncodingUTF8))
		->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdBasePrimaryLanguage))
		->EndListL();
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdLanguageBaseAttributeIDList, *attrValDES);
	CleanupStack::PopAndDestroy(attrValDES);


	// BT Profile Description
	attrValDES = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
		->BuildDESL()
			->StartListL()
			->BuildUUIDL(TUUID(KPbapProfileUUID))
			->BuildUintL(TSdpIntBuf<TUint16>(KPbapProfileVersion))
			->EndListL()
		->EndListL();
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdBluetoothProfileDescriptorList, *attrValDES);
	CleanupStack::PopAndDestroy(attrValDES);

	// Browse Group List
	attrValDES = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
		->BuildUUIDL(TUUID(TUint16(KPublicBrowseGroupUUID))) // Public Browse Group
		->EndListL();
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdBrowseGroupList, *attrValDES);
	CleanupStack::PopAndDestroy(attrValDES);

	// provider name
	attrVal = CSdpAttrValueString::NewStringL(KPbapProviderName);
	CleanupStack::PushL(attrVal);
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetProviderName, *attrVal);
	CleanupStack::PopAndDestroy(attrVal);


	// service name
	attrVal = CSdpAttrValueString::NewStringL(KPbapServiceName);
	CleanupStack::PushL(attrVal);
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName, *attrVal);
	CleanupStack::PopAndDestroy(attrVal);


	// supported repositories (currently no support for SIM repositories)
	TUint8 supportedRepositories = KPbapLocalPhonebookRepository;
		
	attrVal = CSdpAttrValueUint::NewUintL(TSdpIntBuf<TUint8>(supportedRepositories));
	CleanupStack::PushL(attrVal);
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KPbapSdpAttrIdSupportedRepositories, *attrVal);
	CleanupStack::PopAndDestroy(attrVal);
	// availablity
	UpdateAvailability(EPbapAvailable);
	}

void CPbapServer::UpdateAvailability(TPbapAvailability aAvailability)
	{
	// Non-leaving version of UpdateAvailability, called in place of Leaving
	// version, as the service availabilty is an optional part of PBAP and a
	// failure to update the SDP does not affect the functionality of the
	// profile as a whole
	TRAP_IGNORE(DoUpdateAvailabilityL(aAvailability));
	}

void CPbapServer::DoUpdateAvailabilityL(TPbapAvailability aAvailability)
	{
	iSdpDatabase.DeleteAttribute(iSdpRecordHandle, KSdpAttrIdServiceAvailability);
	CSdpAttrValue* attrVal = CSdpAttrValueUint::NewUintL(TSdpIntBuf<TUint8>(aAvailability));
	CleanupStack::PushL(attrVal);
	iSdpDatabase.UpdateAttributeL(iSdpRecordHandle, KSdpAttrIdServiceAvailability, *attrVal);
	CleanupStack::PopAndDestroy(attrVal);
	}

void CPbapServer::ReleaseSdpRegistration()
	{
	LOG_FUNC
	if(iSdpRecordHandle)
		{
		// delete the pbap sdp record
		iSdpDatabase.DeleteRecord(iSdpRecordHandle);		
		iSdpRecordHandle=0;
		}
	iSdpDatabase.Close();
	iSdpServer.Close();	
	}
