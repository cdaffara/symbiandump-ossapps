// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//class include
#include "ObexSdpUtils.h"
//system includes
#include <es_sock.h>


//
// Bluetooth Assigned Numbers
// The following constants come from the Bluetooth Specification Version 1.0B
//

const TUint32 KObexProtocolUUIDValue = 0x0008;
const TUint32 KObexObjectPushUUIDValue = 0x1105;
const TUint32 K_L2CAP_UUID_Value = 0x0100;
const TUint32 K_RFCOMM_UUID_Value = 0x0003;

const TUint16 KServiceClassIdAttributeId = 0x0001;
const TUint16 KProtocolDescriptorListAttributeId = 0x0004;
const TUint16 KBluetoothProfileDescriptorListAttributeId = 0x0009;
const TUint16 KSupportedFormatsListAttributeId = 0x0303;


//
// The following structures are used to validate the record we receive from a remote
// device
//

const TInt KAttributes = 4;
const TInt KMaxComponents = 8;
const TInt KTAttributeComponentMembers = 3;

const TUint16 KExpectedAttributes[KAttributes] =
	{
	KServiceClassIdAttributeId,
	KProtocolDescriptorListAttributeId,
	KBluetoothProfileDescriptorListAttributeId,
	KSupportedFormatsListAttributeId
	};

const TUint32 KExpectedComponents[KAttributes][KMaxComponents][KTAttributeComponentMembers] = 
	{
		// KServiceClassIdAttributeId 
		{
			{CObexSdpUtils::EUint, ETrue, KServiceClassIdAttributeId},
			{CObexSdpUtils::EAnythingUntilNextExpectedValue, EFalse, 0},
			{CObexSdpUtils::EUUID, ETrue, KObexObjectPushUUIDValue},
			{CObexSdpUtils::EAnythingUntilEnd, EFalse, 0}
		},
		// KProtocolDescriptorListAttributeId
		{ 
			{CObexSdpUtils::EUint, ETrue,  KProtocolDescriptorListAttributeId},
			{CObexSdpUtils::EAnythingUntilNextExpectedValue, EFalse, 0},
			{CObexSdpUtils::EUUID, ETrue,  K_L2CAP_UUID_Value},
			{CObexSdpUtils::EUintPossible, ETrue,  K_RFCOMM_UUID_Value}, //NOT MANDATORY TO RECEIVE THIS UINT
			{CObexSdpUtils::EUUID, ETrue,  K_RFCOMM_UUID_Value},
			{CObexSdpUtils::EUint, EFalse, 0}, //REMOTE OBEX PORT NUMBER
			{CObexSdpUtils::EUUID, ETrue,  KObexProtocolUUIDValue},
			{CObexSdpUtils::EAnythingUntilEnd, EFalse, 0}
		},
		// KBluetoothProfileDescriptorListAttributeId - NOT MANDATORY ATTRIBUTE BUT IF PRESENT INDICATES THAT REMOTE DEVICE CONFORMS 100% TO OBEX OBJECT PUSH PROFILE
		{
			{CObexSdpUtils::EUint, ETrue,  KBluetoothProfileDescriptorListAttributeId},
			{CObexSdpUtils::EAnythingUntilNextExpectedValue, EFalse, 0},
			{CObexSdpUtils::EUUID, ETrue,  KObexObjectPushUUIDValue},
			{CObexSdpUtils::EUint, EFalse, 0}, //VERSION OF OBEX OBJECT PUT SUPPORTED
			{CObexSdpUtils::EAnythingUntilEnd, EFalse, 0}
		},
		// KSupportedFormatsListAttributeId
		{
			{CObexSdpUtils::EUint, ETrue,  KSupportedFormatsListAttributeId},
			{CObexSdpUtils::EUintListUntilEnd, EFalse, 0}, // FORMATS SUPPORTED - LIST OF UINTS OF UNSPECIFIED LENGTH
			{CObexSdpUtils::ENoComponent, EFalse, 0}
		}
	};


//
// CObexSdpUtils
//


EXPORT_C CObexSdpUtils* CObexSdpUtils::NewL(MObexSdpUtilsObserver& aObserver)
/**
 * Leave safe constructor
 *
 * @param aObserver Used to indicate results of SDP query
 */
	{
	CObexSdpUtils* self = new(ELeave) CObexSdpUtils(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CObexSdpUtils* CObexSdpUtils::NewLC(MObexSdpUtilsObserver& aObserver)
/**
 * Leave safe constructor (which adds the newly created object to the cleanup stack)
 * 
 * @param aObserver Used to indicate results of SDP query
 */
	{
	CObexSdpUtils* self = new(ELeave) CObexSdpUtils(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CObexSdpUtils::ConstructL()
/**
 * Necessary initial construction - Creates various structures used to check 
 * results obtained through CSdpAgent
 */
	{
	// Set up structures containing expected components
	iAttributeArray = new(ELeave) CArrayPtrFlat<CAttribute>(1); //granularity of 1

	TInt attrCount;
	for (attrCount=0; attrCount<KAttributes; attrCount++)
		{
		CObexSdpUtils::CAttribute* attr = new(ELeave) CAttribute;
		CleanupStack::PushL(attr);
		attr->iAttributeComponentArray = new(ELeave) CArrayFixFlat<TAttributeComponent>(1); //granularity of 1

		TInt compCount = -1;
		do
			{
			compCount++;
			TAttributeComponent comp;
			comp.iType = (TType)KExpectedComponents[attrCount][compCount][0];
			comp.iSpecificValue = KExpectedComponents[attrCount][compCount][1];
			comp.iValue = KExpectedComponents[attrCount][compCount][2];
			attr->iAttributeComponentArray->AppendL(comp);
			} while (KExpectedComponents[attrCount][compCount][0]!=ENoComponent);

		attr->iAttributeId = KExpectedAttributes[attrCount];
		iAttributeArray->AppendL(attr);
		CleanupStack::Pop(attr);
		}
	}


CObexSdpUtils::CObexSdpUtils(MObexSdpUtilsObserver& aObserver) : iObserver(aObserver), iIsActiveSDPQuery(EFalse)

/**
 * Simple constructor
 *
 * @param aObserver Used to indicate results of SDP query
 */
	{
	}

CObexSdpUtils::~CObexSdpUtils()
/**
 * Destructor. This can be called before the SDP Query completes to safely cancel the 
 * query - N.B. this is the only way to cancel a pending query
 */
	{
	if (iIsActiveSDPQuery)
		// if we are in the middle of a query
		{
			HaltQueryWithError(KErrAbort); 
			//iObserver.RemoteBtObexQueryResult(KErrAbort,KErrNotFound,EFalse,KErrNotFound,iSupportedFormatsList);
			//iSdpAgent->Cancel();
		}

		if(iSdpAgent)
		// if we have started a SDP agent
   		{
			delete iSdpAgent;	
		
		}
	
	if(iAttributeArray)
	    iAttributeArray->ResetAndDestroy();
	delete iAttributeArray;
	}


EXPORT_C void CObexSdpUtils::RemoteBtObexQueryL(TBTDevAddr aBtDevAddr)
/**
 * Perform SDP query on a remote bluetooth device. Result is returned through the MObexSdpUtilsObserver 
 * observer class. The query can be cancelled at any time by destroying this CObexSdpUtils 
 * object.
 *
 * @param aBtDevAddr The address of the bluetooth device
 */
	{
		if(iSdpAgent)
		{
			iSdpAgent->Cancel();
		}
		else
		{
			iSdpAgent = CSdpAgent::NewL(*this,aBtDevAddr);
		}

	// Set up search for SDP records containing the OBEX Object Push Service 
	// UUID.
	// We also specify that the record must contain L2CAP and RFCOMM UUIDS as
	// these are mandatory according to the BT Spec
	CSdpSearchPattern* searchPattern = CSdpSearchPattern::NewL();
	CleanupStack::PushL(searchPattern);
	searchPattern->AddL(TUUID(KObexObjectPushUUIDValue));
	searchPattern->AddL(TUUID(K_L2CAP_UUID_Value));
	searchPattern->AddL(TUUID(K_RFCOMM_UUID_Value));
	iSdpAgent->SetRecordFilterL(*searchPattern); //makes COPY of searchPattern
	CleanupStack::PopAndDestroy(searchPattern);

	// Start the search
	iSdpRecordCount=0;
	iSdpAgent->NextRecordRequestL(); //Will result in call to NextRecordRequestComplete()
	iIsActiveSDPQuery = ETrue; //Keep record of the fact that we are mid query
	}

void CObexSdpUtils::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
/** 
 * Got an SDP record from the remote device, so check for errors and then request the 
 * first attribute.
 *
 * @param aError Error code 
 * @param aHandle Handle to the service recorde
 * @param aTotalRecordsCount The total number of records
 */
	{
	iSdpRecordCount++;

	if (aError)
		{
		// return the error that has prevented us from continuing with our query
		HaltQueryWithError(aError);
		return;
		}

	if(iSdpRecordCount>aTotalRecordsCount)
		{
		// return KErrEof because we have gone through all the records and 
		// the port hasn't been found
		HaltQueryWithError(KErrEof);
		return;
		}

	// Got a record so let's request our first attribute
	iNotThisSdpRecord = EFalse;
	iRemoteObexPort = KErrNotFound;
	iFullComplianceWithObexObjectPush = EFalse;
	iObexObjectPushProfileVersion = KErrNotFound;
	iSupportedFormatsList.SetLength(0);

	iAttribute = -1; 
	RequestNextAttribute(aHandle); //will increment iAttribute to 0
	}

void CObexSdpUtils::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID /*aAttrID*/, CSdpAttrValue* /*aAttrValue*/)
/**
 * The overload of AttributeRequestL() that we are using in NextRecordRequestComplete()
 * should NOT result in this function being called, if it does then halt the query with an 
 * error.
 *
 * @param aHandle The handle to the service record
 * @param aAttrID The SDP Attribute ID
 * @param aAttrValue The SDP Attribute Value
 */
	{
	// The overload of AttributeRequestL() that we are using in NextRecordRequestComplete
	// should NOT result in this function being called
	HaltQueryWithError(KErrUnknown);
	}

void CObexSdpUtils::AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
/**
 * Called after we have got all the attribute components for current attribute being 
 * processed. If everything is OK get the next attribute or record.
 * 
 * @param aHandle The handle to the service record
 * @param aError The error code
 */
	{
	if (!aError && !iNotThisSdpRecord && iAttribute>=iAttributeArray->Count()-1 && iExpectedType==EUintListUntilEnd)
		{
		// Query is complete - got all the attributes and we have finished in the middle
		// of compiling the list of supported obex formats (we can not know in advance the 
		// no. of formats a remote device supports)
		iObserver.RemoteBtObexQueryResult(KErrNone,iRemoteObexPort,iFullComplianceWithObexObjectPush,iObexObjectPushProfileVersion,iSupportedFormatsList);
		HaltQuery();
		}
	else
		{
		// See if we should continue looking at the attributes for this record
		if (iNotThisSdpRecord //there was a problem with attribute UUIDs and Uints
		    || (iExpectedType==EAnythingUntilNextExpectedValue) //we should have got whatever we expected next!
		    || (iExpectedType==EUint) //we should always get the Uint we are looking for
			|| (iExpectedType==EUUID) //we should always get the UUID we are looking for 
			|| (aError)) //an error has occurred, note that not finding an attribute does NOT result in KErrNotFound
			{
			// following is an exception to the rules above...
			if (iCurrentAttributeId==KBluetoothProfileDescriptorListAttributeId) //it OK to not find what we expect in for attribute because attribute is not mandatory
				{
				// therefore everything is OK with the record so far - Request the next attribute
				RequestNextAttribute(aHandle);
				}
			else
				{
				// this record didn't satisfy our query so try the next one
				TRAPD(err,iSdpAgent->NextRecordRequestL()); //results in call to NextRecordRequestComplete()
				if (err)
					{
					HaltQueryWithError(err);
					}
				}
			}
		else
			{
			// Everything is OK with the record so far - Request the next attribute
			RequestNextAttribute(aHandle);
			}
		}
	}

MSdpElementBuilder* CObexSdpUtils::BuildUUIDL(const TUUID& aUUID)
/**
 * Got a UUID attribute component. Check it is what we expected to receive. Get the next attribute component.
 *
 * @param aUUID The Bluetooth UUID of attribute
 */
	{
	if(iNotThisSdpRecord
	   || iExpectedType==EAnythingUntilEnd) //don't care what we get for current attribute anymore!
		{
		// Not interested in this UUID
		return this;
		}

	if (iExpectedType==EAnythingUntilNextExpectedValue)
		{
		if (iNextExpectedType==EUUID && aUUID==TUUID(iNextExpectedValue))
			{
			// Got the "next" value we are looking for so set current expected to "next"
			// and carry on!
			NextAttributeComponent();
			}
		else
			{
			// Not interested in this UUID
			return this;
			}
		}

	iCurrentUUID = aUUID;

	if (iExpectedType!=EUUID)
		{
		if (iExpectedType==EUintPossible)
			{
			// It was possible to receive a Uint but we got a UUID instead, this is
			// perfectly valid. Go the the next component and call this method
			// recursively to ensure next component is processed with this UUID
			NextAttributeComponent();
			BuildUUIDL(aUUID);
			return this; //to prevent us missing an attribute
			}
		else
			{
			// Shouldn't have received a UUID!
			iNotThisSdpRecord = ETrue;
			}
		}
	else if (iExpectingSpecific)
		{
		if (aUUID != TUUID(iExpectedValue))
			{
			// Haven't got what we expected!
			iNotThisSdpRecord = ETrue;
			}
		else if (aUUID==TUUID(KObexObjectPushUUIDValue) && iCurrentAttributeId==KBluetoothProfileDescriptorListAttributeId)
			{
			// Remote device complies fully with obex object push profile
			iFullComplianceWithObexObjectPush = ETrue;;
			}
		}
	else
		{
		// When dealing with UUIDs we are ALWAYS expecting a specific UUID.
		// If we end up here then something has gone wrong
		iNotThisSdpRecord = ETrue;
		}

	NextAttributeComponent();
	return this;
	}


MSdpElementBuilder* CObexSdpUtils::BuildUintL(const TDesC8& aUint)
/**
 * Got a Uint attribute componet. Check it is what we expected to receive. Get the next attribute component.
 *
 * @param aUint Attribute as Uint contained in a descriptor
 */
	{
	if(iNotThisSdpRecord
	   || iExpectedType==EAnythingUntilEnd) //don't care what we get for current attribute anymore!
		{
		// Not interested in this Uint
		return this;
		}

	TUint32 uint = UInt32(aUint);

	if (iExpectedType==EAnythingUntilNextExpectedValue)
		{
		if (iNextExpectedType==EUint && uint==iNextExpectedValue)
			{
			// Got the "next" value we are looking for so set current expected to "next"
			// and carry on!
			NextAttributeComponent();
			}
		else
			{
			// Not interested in this Uint
			return this;
			}
		}

	if (iExpectedType==ENoComponent || iExpectedType==EUUID)
		{
		// We should have not found a Uint
		iNotThisSdpRecord = ETrue;
		}
	else if (iExpectingSpecific)
		{
		if (uint != iExpectedValue)
			{
			// haven't got what we expected!
			iNotThisSdpRecord = ETrue;
			}
		}
	else
		{
		// Not expecting a specific value so the value we've got is unknown so it's
		// a piece of info we are trying to discover

		if (iCurrentAttributeId==KProtocolDescriptorListAttributeId && iCurrentUUID==TUUID(K_RFCOMM_UUID_Value))
			{
			// Got the port number!
			iRemoteObexPort = uint;
			}
		else if (iCurrentAttributeId==KBluetoothProfileDescriptorListAttributeId && iCurrentUUID==TUUID(KObexObjectPushUUIDValue))
			{
			// Got the Obex Object Push Profile Version
			iObexObjectPushProfileVersion = uint;
			}
		else if (iCurrentAttributeId==KSupportedFormatsListAttributeId && iCurrentUUID==TUUID(NULL))
			{
			// Got an Obex format supported by the remote device
			TInt length = iSupportedFormatsList.Length();
			if (length<KMaxObexSupportedFormats)
				{
				iSupportedFormatsList.SetLength(length+1);
				iSupportedFormatsList[length]=(TUint8)uint;
				}
			else
				{
				HaltQueryWithError(KErrTooBig);
				}
			return this; //don't go to next expected component because a list doesn't
						 //have a defined length
			}
		}

		NextAttributeComponent();
		return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildUnknownL(TUint8 /*aType*/, TUint8 /*aSizeDesc*/, const TDesC8& /*aData*/)
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildNilL()
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildIntL(const TDesC8& /*aInt*/)
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildBooleanL(TBool /*aBool*/)
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildStringL(const TDesC8& /*aString*/)
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildDESL()
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildDEAL()
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::StartListL()
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::EndListL()
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

MSdpElementBuilder* CObexSdpUtils::BuildURLL(const TDesC8& /*aURL*/)
/**
 * Not required for our SDP query, so we provide an empty implementation.
 */
	{
	return this;
	}

TUint32 CObexSdpUtils::UInt32(const TDesC8& aUint)
/**
 * Convert descriptor containing Uint to a numeric Uint.
 *
 * The Uints can be 1, 2, 4, 8 or 16 bytes long in theory. It is doubtful 
 * that the 8 or 16 byte variety would actually be used in our Obex Object 
 * Push Service case. The attribute Id will always be a 2 byte Uint but it 
 * is unclear from the spec what size the the port number should be. It is 
 * implied that it should only be 1 byte but in the EPOC implementation, it 
 * is returned as whatever size is it registered as (there is no type/size 
 * checking carried out in the SDP server).
 *
 * @param aUint The Uint as a descriptor
 */
	{
	TUint32 uint=0;

	switch(aUint.Length())
		{
	case 1:
		uint = (TUint32)aUint[0];
		break;
	case 2:
		uint = (TUint32)(BigEndian::Get16(aUint.Ptr()));
		break;
	case 4:
	default: 
		// for our query, if larger Uids are received they may be truncated
		uint = BigEndian::Get32(aUint.Ptr());
		break;
		}

	return uint;
	}

void CObexSdpUtils::HaltQuery()
/**
 * Halt the current SDP query by destroying the CSdpAgent.
 */
	{
		iSdpAgent->Cancel();
		iIsActiveSDPQuery = EFalse;
	}

void CObexSdpUtils::HaltQueryWithError(TInt aError)
/**
 * Halt the current SDP query by destroying the CSdpAgent. Inform MObexSdpUtilsObserver 
 * observer that query has completed with an error.
 * 
 * @param aError The error code
 */
	{
	iObserver.RemoteBtObexQueryResult(aError,KErrNotFound,EFalse,KErrNotFound,iSupportedFormatsList);
	HaltQuery();
	}

void CObexSdpUtils::RequestNextAttribute(TSdpServRecordHandle aHandle)
/**
 * Get next attribute if we are expecting another attribute.
 * 
 * @param aHandle The handle to the service records
 */
	{
	iAttribute++;
	if (iAttribute >= iAttributeArray->Count())
		{
		// Shouldn't be requesting another attribute as have no expectation
		// for another attribute
		HaltQueryWithError(KErrNotFound);
		}
	else
		{
		iCurrentUUID=TUUID(NULL);

		iAttributeComponent=-1;
		NextAttributeComponent(); //will increment iAttributeComponent

		iCurrentAttributeId = (*iAttributeArray)[iAttribute]->iAttributeId;
		TRAPD(err,iSdpAgent->AttributeRequestL(this,aHandle,TSdpAttributeID(iCurrentAttributeId))); //results in multiple calls to BuildUUIDL() & BuildUintL(), ending with 1 call to AttributeRequestComplete()
		if (err)
			{
			HaltQueryWithError(err);
			}
		}
	}

void CObexSdpUtils::NextAttributeComponent()
/**
 * Get next attribute component if we are expecting another attribute component. Set 
 * internal expectations for the next attribute component we receive.
 */
	{
	iAttributeComponent++;
	if (iAttributeComponent >= (*iAttributeArray)[iAttribute]->iAttributeComponentArray->Count())
		{
		// Shouldn't be requesting another attribute component as we have no 
		// expectation for another attribute component
		iNotThisSdpRecord = ETrue;
		return;
		}

	// Set expectations for the next attribute component we receive
	iExpectedType = (*(*iAttributeArray)[iAttribute]->iAttributeComponentArray)[iAttributeComponent].iType;
	iExpectingSpecific = (*(*iAttributeArray)[iAttribute]->iAttributeComponentArray)[iAttributeComponent].iSpecificValue;
	iExpectedValue = (*(*iAttributeArray)[iAttribute]->iAttributeComponentArray)[iAttributeComponent].iValue;

	if (iExpectedType==EAnythingUntilNextExpectedValue)
		{
		iNextExpectedType = (*(*iAttributeArray)[iAttribute]->iAttributeComponentArray)[iAttributeComponent+1].iType;
		iNextExpectedValue = (*(*iAttributeArray)[iAttribute]->iAttributeComponentArray)[iAttributeComponent+1].iValue;
		}
	}


CObexSdpUtils::CAttribute::CAttribute()
/**
 * Simple constructor
 */
	{
	}

CObexSdpUtils::CAttribute::~CAttribute()
/**
 * Destructor deletes array of attributes
 */
	{
	delete iAttributeComponentArray;
	}
