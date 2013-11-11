/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CBtDiscover class definition. Bluetooth device discovery and SDP.
*
*/


#include "cbtdiscover.h"
#include "clog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * @brief Instantiate new CBtDiscover object.
 */
CBtDiscover* CBtDiscover::NewL()
	{
	CBtDiscover* self = new (ELeave) CBtDiscover();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}


/**
 * @brief Two phase construction.
 */
void CBtDiscover::ConstructL()
	{
	iSdpSearchPattern = CSdpSearchPattern::NewL();
	iMatchList = CSdpAttrIdMatchList::NewL();
	}

/**
 * @brief Constructor.
 */
CBtDiscover::CBtDiscover() :
	iCurrentSDPAttrID(0),
	iCurrentServiceUUID(0),
	iFoundSTS(EFalse),
	iWantedService(ETrue),
	iDoingSDP(EFalse),
	iFindingDevices(EFalse),
	iAgent(NULL),
	iRequestStatus(NULL),
	iSdpSearchPattern(NULL),
	iMatchList(NULL),
	iContentTypes(NULL),
	iPrinterModel(NULL),
	iBPPPort(0),
	iSTSPort(0),
	iOPPPort(0)
	{
	}

/**
 * @brief Destructor.
 */
CBtDiscover::~CBtDiscover()
	{
	LOG("CBtDiscover::~CBtDiscover begin");
	Stop();
	if(iSdpSearchPattern)
		delete iSdpSearchPattern;
	if(iMatchList)
		delete iMatchList;
	if(iContentTypes)
		delete iContentTypes;
	if(iPrinterModel)
		delete iPrinterModel;
	if(iAgent)
		delete iAgent;
	LOG("CBtDiscover::~CBtDiscover end");
	}

/**
 * @brief Destructor.
 */
void CBtDiscover::Reset()
	{
	LOG("CBtDiscover::Reset begin");

	StopSDP(); // resets also public member variables and iDoingSDP flag

	iBPPPort = 0;
	iSTSPort = 0;
	iOPPPort = 0;

	if(iPrinterModel)
		delete iPrinterModel;
	iPrinterModel = NULL;

	if(iSdpSearchPattern)
		iSdpSearchPattern->Reset();
	
	// No can do if remove fails -> ignore leave
	if(iMatchList)
		TRAP_IGNORE( iMatchList->RemoveL(TAttrRange(0x0000, 0xFFFF)) );
	
	if(iContentTypes)
		delete iContentTypes;
	iContentTypes = NULL;
	

	if( iRequestStatus )
	{
		if(KRequestPending == iRequestStatus->Int())
			User::RequestComplete(iRequestStatus, KErrNone);
	}
	iRequestStatus = NULL;

	LOG("CBtDiscover::Reset end");
	}

/**
  * @brief Starts the asynchronous search for Bluetooth devices.
  *
  * Starts the asynchronous search for Bluetooth devices. The Active Object containing
  * this object, CRsBtDiscoverEngine, is notified after the first device is found.
  * @param aStatus Status variable from CRsBtDiscoverEngine, receives notification when asynchronous call completes.
  */
void CBtDiscover::Start(TRequestStatus &aStatus)
	{
	LOG("[CBtDiscover::Start]\t begin");
	aStatus = KErrNotSupported;

	TInt err = iSocketServ.Connect();
	LOG1("[CBtDiscover::Start]\t iSocketServ.Connect() err: %d", err);
	if( KErrNone == err )
	{
		err = iHostResolver.Open( iSocketServ, KBTAddrFamily, KBTLinkManager );
		LOG1("[CBtDiscover::Start]\t iHostResolver.Open() err: %d", err);
	}

	if( KErrNone == err )
	{
		iFindingDevices = ETrue;
		iSockAddr.SetIAC( KGIAC );
		iSockAddr.SetAction( KHostResInquiry | KHostResName | KHostResIgnoreCache );
		iHostResolver.GetByAddress( iSockAddr, iNameEntry, aStatus );
	}
	else
	{
		iFindingDevices = EFalse;
		iSocketServ.Close();
		iHostResolver.Close();
	}

	LOG("[CBtDiscover::Start]\t end");
	}

/**
  * @brief Continues the asynchronous search for Bluetooth devices.
  *
  * Continues the asynchronous search for Bluetooth devices. The Active Object containing
  * this object, CRsBtDiscoverEngine, is notified after the next device is found.
  * @param aStatus Status variable from CRsBtDiscoverEngine, receives notification when asynchronous call completes.
  */
void CBtDiscover::GetNextDevice(TRequestStatus &aStatus)
	{
	LOG("[CBtDiscover::GetNextDevice]\t");
	iHostResolver.Next(iNameEntry, aStatus);
	}

/**
  * @brief Stop the asynchronous search for Bluetooth devices.
  */
void CBtDiscover::Stop()
	{
	LOG1("[CBtDiscover::Stop]\t begin with iFindingDevices: %d", iFindingDevices);

	if(iFindingDevices)
		iHostResolver.Cancel();
	iFindingDevices = EFalse;

	Reset();

	iHostResolver.Close();
	iSocketServ.Close();

	LOG("[CBtDiscover::Stop]\t end");
	}

/**
  * @brief Is an asynchronous search for Bluetooth devices in progress?
  * @return True, a search is in progress, or False, no search is in progress
  */
TBool CBtDiscover::IsFindingDevices() const
	{
	return iFindingDevices;
	}

/**
  * @brief Get the last Bluetooth device found by the search.
  * @return The last Bluetooth device found by the search.
  */
TNameRecord CBtDiscover::GetNameRecord() const
	{
	return iNameEntry();
	}

/**
  * @brief Store Bluetooth device information, in preparation for doing an SDP query.
  */
void CBtDiscover::SetNameRecord(TNameRecord aNameRecord)
	{
	TNameEntry nameEntry(aNameRecord);
	iNameEntry = nameEntry;
	}

/**
  * @brief Starts the asynchronous request for the SDP record.
  *
  * Starts the asynchronous request for the SDP record from a Bluetooth device.
  * Specifically the device is tested for the BPP and OPP services.
  * @param aStatus Status variable from CRsBtDiscoverEngine, receives notification when asynchronous call completes.
  * @param aServiceMask Bitmask to require (only) certain services. All marked by default.
  */
void CBtDiscover::GetSupportedServicesL(TRequestStatus &aStatus, TUint32 aServiceMask)
	{
	LOG("[CBtDiscover::GetSupportedServicesL]\t begin");

	Reset();
	iRequestStatus = &aStatus;
	*iRequestStatus = KRequestPending;	

	iMatchList->RemoveL(TAttrRange(0x0000, 0xFFFF));
	iMatchList->AddL(TAttrRange(KSdpAttrIdServiceClassIDList));
	iMatchList->AddL(TAttrRange(KSdpAttrIdProtocolDescriptorList));
	iMatchList->AddL(TAttrRange(KSdpAttrIdAdditionalProtocolDescriptorLists));
	iMatchList->AddL(TAttrRange(KSdpAttrIdDocFormatsSupported));
	iMatchList->AddL(TAttrRange(KSdpAttrIdModelID));
	iMatchList->AddL(TAttrRange(KSdpAttrIdMaxWidth));
	iMatchList->AddL(TAttrRange(KSdpAttrIdMaxLen));

	iSdpSearchPattern->Reset();

	if(KBtMaskObjectPush & aServiceMask)
		iSdpSearchPattern->AddL(KBTSDPOBEXObjectPush); // 0x1105
	if(KBtMaskDirectPrinting & aServiceMask)
		iSdpSearchPattern->AddL(KBTSDPDirectPrinting); // 0x1118
	if(KBtMaskPrintingStatus & aServiceMask)
		iSdpSearchPattern->AddL(KBTSDPPrintingStatus); // 0x1123
	if(KBtMaskL2CAP & aServiceMask)
		iSdpSearchPattern->AddL(KBTSDPL2CAP);
	if(KBtMaskRFCOMM & aServiceMask)
		iSdpSearchPattern->AddL(KBTSDPRFCOMM);
	if(KBtMaskObex & aServiceMask)
		iSdpSearchPattern->AddL(KBTSDPObex);

	StartSDPL();

	LOG("[CBtDiscover::GetSupportedServicesL]\t end");
	}

/**
  * @brief Initiate the SDP request started in VerifyServiceSupportL.
  */
void CBtDiscover::StartSDPL()
	{
	StopSDP();
	TBTDevAddr btAddr = ((TInquirySockAddr)iNameEntry().iAddr).BTAddr();
	iAgent = CSdpAgent::NewL(*this, btAddr);
	iAgent->SetRecordFilterL(*iSdpSearchPattern);
	iAgent->SetAttributePredictorListL(*iMatchList);
	iAgent->NextRecordRequestL();
	iDoingSDP = ETrue;
	}

/**
  * @brief Stop the asynchronous request for the SDP record from a Bluetooth device.
  */
void CBtDiscover::StopSDP()
	{
	LOG1("[CBtDiscover::StopSDP] begin with iAgent address: %d", TInt(iAgent));
	iDoingSDP = EFalse;

	iCurrentSDPAttrID = 0;
    iCurrentServiceUUID = 0;
	iFoundSTS = EFalse;
	iWantedService = ETrue;

	if( iAgent )
		{
		iAgent->Cancel();
		delete iAgent;
		iAgent = NULL;
		}
	LOG("[CBtDiscover::StopSDP] end");
	}

/**
  * @brief Is an asynchronous request for the SDP record in progress?.
  *
  * Is an asynchronous request for the SDP record from a Bluetooth device in progress?
  * @return True, a request is in progress, or False, no request is in progress
  */
TBool CBtDiscover::IsDoingSDP() const
	{
	return iDoingSDP;
	}

/**
  * The CSdpAgent object calls this to notify that the attributes from an SDP record are ready for parsing.
  * @param aAttrValue Structured attribute(s) of SDP record, parsed with TBTAttribValVisitor
  */
void CBtDiscover::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID aAttrID, CSdpAttrValue *aAttrValue)
	{
	TRAPD( err, AttributeRequestResultL( aAttrID, aAttrValue ) );
	if(KErrNone != err)
		LOG1("[CBtDiscover::AttributeRequestResult]\t AttributeRequestResultL leaves with: %d", err);
	}

void CBtDiscover::AttributeRequestResultL( TSdpAttributeID aAttrID, CSdpAttrValue* aValue )
	{
	LOG1("[CBtDiscover::AttributeRequestResultL]\t *** Device: %S: ", &GetDeviceName() );
	LOG2("CBtDiscover::AttributeRequestResultL aAttrID: %d, aValue->Type(): %d", aAttrID, aValue->Type());

	iCurrentSDPAttrID = aAttrID;
	CBTAttribValVisitor* valVisitor = NULL;

	switch( aAttrID )
		{
		case KSdpAttrIdServiceClassIDList:
			iCurrentServiceUUID = 0; // reset
		case KSdpAttrIdProtocolDescriptorList:
		case KSdpAttrIdAdditionalProtocolDescriptorLists:
		case KSdpAttrIdDocFormatsSupported:
		case KSdpAttrIdModelID:
		case KSdpAttrIdMaxWidth:
		case KSdpAttrIdMaxLen:
			valVisitor = CBTAttribValVisitor::NewLC( *this );
			aValue->AcceptVisitorL( *valVisitor ); // goes in CBTAttribValVisitor::VisitAttributeValueL 
			CleanupStack::PopAndDestroy( valVisitor );
			break;
		default:
			break;
		}

	delete aValue;

	LOG("CBtDiscover::AttributeRequestResultL end");
	}


/**
  * The CSdpAgent object calls this to notify that the SDP record request has been completed.
  */
void CBtDiscover::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
	{
	LOG1("CBtDiscover::NextRecordRequestComplete aError: %d", aError);
	LOG1("CBtDiscover::NextRecordRequestComplete aHandle: %d", aHandle);
	LOG1("CBtDiscover::NextRecordRequestComplete aTotalRecordsCount: %d", aTotalRecordsCount);
	TInt err( aError );

	if( KErrNone == aError  && aTotalRecordsCount > 0)
	{
		TRAP( err, iAgent->AttributeRequestL( aHandle, KSdpAttrIdServiceClassIDList ) );
		LOG1("CBtDiscover::NextRecordRequestComplete err: %d", err);
	}

	if( KErrNone != err || !aTotalRecordsCount )
	{
		if( KErrEof == err || !aTotalRecordsCount )
		{
			err = KErrNone;
		}
		StopSDP();
		LOG("CBtDiscover::NextRecordRequestComplete User::RequestComplete");
		User::RequestComplete( iRequestStatus, err );	
	}

	// reset
	iCurrentServiceUUID = 0; 
	iWantedService = ETrue;
	iCurrentSDPAttrID = 0;
	LOG("CBtDiscover::NextRecordRequestComplete end");
	}

/**
  * The CSdpAgent object calls this to notify that the current SDP record has no more attributes.
  * @param aError Standard Symbian error code, KErrNone indicates success.
  */
void CBtDiscover::AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
{
	LOG("[CBtDiscover::AttributeRequestComplete]\t Begin.");
	if( KErrNone != aError || !iWantedService || 0 == iCurrentSDPAttrID)
	{
		LOG1("[CBtDiscover::AttributeRequestComplete]\t err: %d", aError);
		iCurrentSDPAttrID = 0;
		LOG("[CBtDiscover::AttributeRequestComplete]\t Not wanted service. Get next record.");
		TRAPD( err, iAgent->NextRecordRequestL() );
		if(KErrNone != err)
		{
			NextRecordRequestComplete(err, aHandle, 0);
		}
	}
	else
	{
		LOG("[CBtDiscover::AttributeRequestComplete]\t Wanted service. Get next attribute.");
		TRAPD( err, GetNextAttributeL(aHandle) );
		if( KErrNone != err )
			LOG1("[CBtDiscover::AttributeRequestComplete]\t err: %d", err);
	}
	iCurrentSDPAttrID = 0;
	LOG("[CBtDiscover::AttributeRequestComplete]\t End.");
}

/**
  * @brief Get BPP port.
  * @return Port (i.e. Bluetooth channel) of the BPP service on the printer. Returns -1 if port is unknown.
  */
TInt CBtDiscover::GetBPPPort() const
	{
	return iBPPPort;
	}

/**
  * @brief Get Status port.
  * @return Port (i.e. Bluetooth channel) of the status service on the printer. Returns -1 if port is unknown.
  */
TInt CBtDiscover::GetSTSPort() const
	{
	return iSTSPort;
	}

/**
  * @brief Get OPP port.
  * @return Port (i.e. Bluetooth channel) of the OPP service on the printer. Returns -1 if port is unknown.
  */
TInt CBtDiscover::GetOPPPort() const
	{
	return iOPPPort;
	}

/**
  * @brief Set the BPP Port.
  *
  * Called from TBTAttribValVisitor to set the Port (i.e. Bluetooth channel).
  * Also used to indicate that the BPP service was successfully detected in the SDP response.
  */
void CBtDiscover::SetBPPPort(TInt aPort)
	{
	LOG1("CBtDiscover::SetBPPPort aPort: %d", aPort);
	iBPPPort = aPort;
	}

/**
  * @brief Set the Status Port.
  *
  * Called from TBTAttribValVisitor to set the Port (i.e. Bluetooth channel).
  * Also used to indicate that the Status service was successfully detected in the SDP response.
  */
void CBtDiscover::SetSTSPort(TInt aPort)
	{
	LOG1("CBtDiscover::SetSTSPort aPort: %d", aPort);
	iSTSPort = aPort;
	}
/**
  * @brief Set the OPP Port.
  *
  * Called from TBTAttribValVisitor to set the Port (i.e. Bluetooth channel).
  * Also used to indicate that the OPP service was successfully detected in the SDP response.
  */
void CBtDiscover::SetOPPPort(TInt aPort)
	{
	LOG1("CBtDiscover::SetOPPPort aPort: %d", aPort);
	iOPPPort = aPort;
	}

/**
  * @brief Bluetooth device supports BPP?.
  *
  * Does the SDP response for the current Bluetooth device indicate that BPP is supported?
  * Checks to make sure that a Port (i.e. Bluetooth channel) for the BPP service was determined,
  * AND that the device supports the Mime/Multiplex document type.
  * @return True, BPP is supported, or False, BPP is not supported
  */
TBool CBtDiscover::IsBPPSupported() const
	{
	_LIT(KMimeXhtmlType, "application/vnd.pwg-xhtml-print+xml");
	_LIT(KMimeMultiplexType, "application/vnd.pwg-multiplexed");
	LOG1("CBtDiscover::IsBPPSupported iBPPPort: %d", iBPPPort);
	return ( iBPPPort && (SupportsContentType( KMimeMultiplexType ) || SupportsContentType( KMimeXhtmlType )) );
	}

/**
  * @brief Bluetooth device supports OPP?.
  *
  * Does the SDP response for the current Bluetooth device indicate that OPP is supported?
  * @return True if OPP is supported. False if OPP is not supported.
  */
TBool CBtDiscover::IsOPPSupported() const
	{
	LOG1("CBtDiscover::IsOPPSupported iOPPPort: %d", iOPPPort);
	if(iOPPPort)
		return ETrue;
	else
		return iOPPPort;
	}

/**
  * @brief Obtain Bluetooth device class.
  *
  * @return Bluetooth device class. Used by CRsBtDiscoverEngine to do the first check
  * whether the current Bluetooth device is likely to support BPP or OPP.
  * @sa KBTMinDevClassPrinter
  */
TBTDeviceClass CBtDiscover::GetDeviceClass() const
	{
	TInquirySockAddr sockAddr = (TInquirySockAddr)iNameEntry().iAddr;
	TBTDeviceClass devClass(sockAddr.MajorServiceClass(),
		sockAddr.MajorClassOfDevice(),
		sockAddr.MinorClassOfDevice());

	return devClass;
	}

/**
  * @brief Obtain Bluetooth device name.
  *
  * @return Friendly name of current Bluetooth device.
  */
const TDesC& CBtDiscover::GetDeviceName() const
	{
	return iNameEntry().iName;
	}

/**
  * @brief Sets supported content types.
  *
  * Called from TBTAttribValVisitor to set the supported content types.
  * @param aString String of comma separated mime-types.
  */
void CBtDiscover::SetContentTypesL(const TPtrC8 &aString)
	{
#ifdef _DEBUG

	LOG1("CBtDiscover::SetContentTypesL %d", aString.Length());
	
	TInt start = 0;
	TInt end = aString.Locate(',');
	TPtrC8 ptr;
	while(end > 0 && start <= aString.Length())
	{
		ptr.Set(aString.Mid(start, end));
		LOG81("[CBtDiscover::SetContentTypesL]\t ptr: \"%S\"", &ptr);
		++end;
		start += end;
		if(start > aString.Length())
			break;
		
		TPtrC8 left = aString.Mid(start);
		end = left.Locate(',');
		if(KErrNotFound == end)
			end = aString.Length() - start;
	}

#endif
	delete iContentTypes;
	iContentTypes = NULL;
	iContentTypes = HBufC::NewL(aString.Length());
	TPtr bufPtr(iContentTypes->Des());
	bufPtr.Copy(aString);
	}

/**
  * @brief Ask if device supports a content type.
  *
  * Searches the string of supported content types found in the SDP record of the current Bluetooth device.
  *
  * @param aContType Mime-type to search for.
  *
  * @return True, if the passed in content type was found, False, if it was not found
  */
TBool CBtDiscover::SupportsContentType(const TDesC &aContType) const
	{
	LOG1("CBtDiscover::SupportsContentType aContType: %S", &aContType);
	TPtr bufPtr(iContentTypes->Des());

	if( bufPtr.Length() == 0 )
		{
		LOG("CBtDiscover::SupportsContentType length == 0");
		return EFalse;
		}

	if( bufPtr.Find(aContType) == KErrNotFound )
		{
		LOG("CBtDiscover::SupportsContentType Find == KErrNotFound");
		return EFalse;
		}
	else
		{
		LOG("CBtDiscover::SupportsContentType Found");
		return ETrue;
		}
	}

/**
  * @brief Called from TBTAttribValVisitor to set the printer model.
  * @param aString String of semi-colon separated printer details.
  */
void CBtDiscover::SetPrinterModelL(const TPtrC8 &aString)
	{
	LOG81("CBtDiscover::SetPrinterModelL aString: %S", &aString);

	if(iPrinterModel)
		delete iPrinterModel;
	iPrinterModel = NULL;
	
	_LIT8(KModelString, ";MDL:");
	TInt pos = aString.Find( KModelString );
	LOG1("[CBtDiscover::SetPrinterModelL]\t pos: %d", pos);
	if( pos != KErrNotFound )
		{
		TPtrC8 tmpStr = aString.Right(aString.Length() - pos - 5);
		pos = tmpStr.Locate(';');
		if( pos != KErrNotFound )
			{
			iPrinterModel = HBufC::NewL(pos);
			iPrinterModel->Des().Copy(tmpStr.Left(pos));
			}
		}

	LOG("CBtDiscover::SetPrinterModelL end");
	}

/**
 * @brief Obtain Printer Model.
 *
 * Gets a TDesC& to the printer model previously set with SetPrinterModelL(),
 * or KNullDesC if no printer model has been set.
 *
 * @return String with the printer model.
 */
const TDesC& CBtDiscover::GetPrinterModel() const
	{
	if (iPrinterModel)
		return *iPrinterModel;
	else
		return KNullDesC;
	}

void CBtDiscover::SetVendor( TPrinter::TPrinterVendor aVendor )
	{
	LOG1("CBtDiscover::SetVendor aVendor: %d", aVendor);
	iVendor = aVendor;
	}

TPrinter::TPrinterVendor CBtDiscover::GetVendor() const
	{
	LOG1("CBtDiscover::GetVendor return: %d", iVendor);
	return iVendor;
	}

void CBtDiscover::GetNextAttributeL(TSdpServRecordHandle aHandle)
	{
	LOG1("[CBtDiscover::GetNextAttributeL]\t iCurrentSDPAttrID: 0x%X", iCurrentSDPAttrID);

	TSdpAttributeID nextId;

	switch( iCurrentSDPAttrID )
	{
		case KSdpAttrIdServiceClassIDList:
			nextId = KSdpAttrIdProtocolDescriptorList;
			break;
		case KSdpAttrIdProtocolDescriptorList:
			nextId = KSdpAttrIdAdditionalProtocolDescriptorLists;
			break;
		case KSdpAttrIdAdditionalProtocolDescriptorLists:
			nextId = KSdpAttrIdDocFormatsSupported;
			break;
		case KSdpAttrIdDocFormatsSupported:
			nextId = KSdpAttrIdModelID;
			break;
		case KSdpAttrIdModelID:
			nextId = KSdpAttrIdMaxWidth;
			break;
		case KSdpAttrIdMaxWidth:
			nextId = KSdpAttrIdMaxLen;
			break;
		default:
			// all interested attributes received, at tleast asked
			//Request 0xFFFFFF to indicate all done
			nextId = KSdpAttrIdAllDone;
			break;
	}

	iAgent->AttributeRequestL( aHandle, nextId );

	LOG1("[CBtDiscover::GetNextAttributeL]\t end. nextId: 0x%X", nextId);
	}



/************************************************************************/
/* visitor */
/************************************************************************/

CBTAttribValVisitor* CBTAttribValVisitor::NewLC( CBtDiscover& aDiscover )
	{
	CBTAttribValVisitor* obj = new (ELeave) CBTAttribValVisitor( aDiscover );
	CleanupStack::PushL( obj );
	return obj;
	}

/**
  * @param aDiscover Stores a pointer to the CBtDiscover object, in order to call
  * CBtDiscover::SetPort and CBtDiscover::SetContentTypes once those attributes have been
  * found.
  */
CBTAttribValVisitor::CBTAttribValVisitor(CBtDiscover& aDiscover) : iDiscover(aDiscover)
	{
	}

/**
  * @brief Called for each of the individual attributes in the SDP record being parsed.
  * @param aValue Value of the attribute.
  * @param aType Type of the attribute.
  */
void CBTAttribValVisitor::VisitAttributeValueL(CSdpAttrValue& aValue, TSdpElementType aType)
	{
	LOG1("[CBTAttribValVisitor::VisitAttributeValueL]\t Begin. aType: %d", aType);

	switch ( aType )
		{
		case ETypeUUID:
			CheckUuid(aValue.UUID());
			break;
		case ETypeUint:
			CheckUint(aValue.Uint());
			break;
		case ETypeString:
			switch (iDiscover.iCurrentSDPAttrID)
				{
				case KSdpAttrIdDocFormatsSupported:
					iDiscover.SetContentTypesL(aValue.Des());
					break;
				case KSdpAttrIdModelID:
					CheckVendor( aValue.Des() );
					iDiscover.SetPrinterModelL(aValue.Des());
					break;
				default:
					break;
				}
			break;
		default:
			break;
		}
	}

/**
  * @brief Indicates the beginning of an attribute list in the SDP record.
  */
void CBTAttribValVisitor::StartListL(CSdpAttrValueList& /*aList*/)
	{
	LOG("CBTAttribValVisitor::StartListL");
	}

/**
  * @brief Indicates the end of an attribute list in the SDP record.
  */
void CBTAttribValVisitor::EndListL()
	{
	LOG("CBTAttribValVisitor::EndListL");
	}

CBTAttribValVisitor::~CBTAttribValVisitor()
	{
	}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CheckUuid
//
//--------------------------------------------------------------------------------------------
void CBTAttribValVisitor::CheckUuid( const TUUID& aUuid )
{
	LOG("[CBTAttribValVisitor::CheckUuid]\t begin");

	// We're interested only about UUIDs received at KSdpAttrIdServiceClassIDList
	if (KSdpAttrIdServiceClassIDList != iDiscover.iCurrentSDPAttrID)
	{
		LOG1("[CBTAttribValVisitor::CheckUuid]\t Wrong attribute ID: 0x%X",iDiscover.iCurrentSDPAttrID);
		return;
	}

#ifdef _DEBUG
	TBuf<20> uidstr;
	_LIT(KUidForm, "0x%02X%02X%02X%02X");
	uidstr.Format(KUidForm, aUuid[0], aUuid[1], aUuid[2], aUuid[3]);
	LOG1("[CBTAttribValVisitor::CheckUuid]\t ETypeUUID: %S", &uidstr);
#endif

	TUUID uidOpp(KBTSDPOBEXObjectPush);
	TUUID uidBpp(KBTSDPDirectPrinting);
	TUUID uidSts(KBTSDPPrintingStatus);
	
	if(aUuid == uidOpp)
		iDiscover.iCurrentServiceUUID = KBTSDPOBEXObjectPush;
	if(aUuid == uidBpp)
		iDiscover.iCurrentServiceUUID = KBTSDPDirectPrinting;
	if(aUuid == uidSts)
		iDiscover.iFoundSTS = ETrue;

	LOG("[CBTAttribValVisitor::CheckUuid]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CheckUint
//
//--------------------------------------------------------------------------------------------
void CBTAttribValVisitor::CheckUint( const TUint& aValue )
{
	LOG("[CBTAttribValVisitor::CheckUint]\t begin");
	
	switch(iDiscover.iCurrentSDPAttrID)
	{
		case KSdpAttrIdProtocolDescriptorList:
			if (KBTSDPDirectPrinting == iDiscover.iCurrentServiceUUID)
			{
				LOG1("[CBTAttribValVisitor::CheckUint]\t  iDiscover->SetBPPPort(%d)", aValue);
				iDiscover.SetBPPPort(aValue);
			}
			if (KBTSDPOBEXObjectPush == iDiscover.iCurrentServiceUUID)
			{
				LOG1("[CBTAttribValVisitor::CheckUint]\t  iDiscover->SetOPPPort(%d)", aValue);
				iDiscover.SetOPPPort(aValue);
			}
			break;
		case KSdpAttrIdAdditionalProtocolDescriptorLists:
			if(iDiscover.iFoundSTS )
			{
				LOG1("[CBTAttribValVisitor::CheckUint]\t iDiscover->SetSTSPort(%d)", aValue);
				iDiscover.SetSTSPort(aValue);
				iDiscover.iFoundSTS = EFalse;
			}
			break;
		case KSdpAttrIdMaxWidth:
			LOG1("[CBTAttribValVisitor::CheckUint]\t KSdpAttrIdMaxWidth: %d", aValue);
			break;
		case KSdpAttrIdMaxLen:
			LOG1("[CBTAttribValVisitor::CheckUint]\t KSdpAttrIdMaxLen: %d", aValue);
			break;
		default:
			break;
	}

	LOG("[CBTAttribValVisitor::CheckUint]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CheckVendor
//
//--------------------------------------------------------------------------------------------
void CBTAttribValVisitor::CheckVendor( const TDesC8&  aVendor  )
	{
	LOG("[CBTAttribValVisitor::CheckVendor]\t Begin");
	iDiscover.SetVendor( TPrinter::EVendorNone );
	// At the moment this functionality is not used, but no Vendor is set.
	HBufC8* vendorName = aVendor.Alloc();
	if( vendorName )
	{
		LOG81("[CBTAttribValVisitor::CheckVendor]\t vendorName: %S", vendorName);
		TPtr8 ptr = vendorName->Des();
		ptr.LowerCase();
		_LIT8( KHp, "mfg:hp;" );

		TInt res = vendorName->Find( KHp );
		LOG1("[CBTAttribValVisitor::CheckVendor]\t res: %d", res);
		
		if( res != KErrNotFound ) 
			iDiscover.SetVendor( TPrinter::EVendorHp );
	}
	delete vendorName;
	LOG("[CBTAttribValVisitor::CheckVendor]\t End");
	}

//  End of File
