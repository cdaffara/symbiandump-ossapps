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
// btcmtm.cpp
//

//class include
#include <btcmtm.h>

//system includes
#include <e32std.h>
#include <e32base.h>
#include <txtrich.h>	// CRichText

#include <mtmuids.h>	// KUidMtmQueryCanSendMsg
#include <msvreg.h>		// CRegisteredMtmDll
#include <mtmdef.h>		// KUidMtmQueryxxx & TMsvPartList flags
#include <msvuids.h>	// KUidMsvMessageEntry
#include "btmtmcmds.h"	//EBtMtmCmdSend

//user includes
#include <btheader.h>
#include "btmsgtypeuid.h" //KUidMsgTypeBt
#include <cobexsendoperation.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#include <mtmuidsdef.hrh>
#endif

const TUint8 KObexConnectionIDHeader = 0xCB;

CBtClientMtm::CBtClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
: CObexClientMtm(aRegisteredMtmDll, aMsvSession, KUidMsgTypeBt)
/**
 * Constructor--not for use by client applications
 *
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aMsvSession CMsvSession of the client requesting the object. 
 */
	{
	}

void CBtClientMtm::InitialiseHeaderL()
/**
 * Deletes the old header, then creates a new CBtHeader.
 *
 * @leave KErrXXX System-wide error codes if allocation fails
 */
	{
	delete iHeader;
	iHeader = 0;

	iHeader = CBtHeader::NewL();
	}

EXPORT_C CBtClientMtm* CBtClientMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
/**
 * Canonical NewL factory function. 
 *
 * @param aRegisteredMtmDll Reference to registration data for MTM DLL.
 * @param aMsvSession Reference to CMsvSession of the client requesting the object.
 * @return Pointer to a new, constructed CBtClientMtm
 * @leave Leaves if no memory is available.
 */
	{
	CBtClientMtm* self = new(ELeave) CBtClientMtm(aRegisteredMtmDll, aMsvSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMsvOperation* CBtClientMtm::InvokeAsyncFunctionL(TInt aFunctionId, 
												    const CMsvEntrySelection& aSelection,
													TDes8& aParameter, 
												    TRequestStatus& aCompletionStatus)
/**
 * Starts an asynchronous function as an active object. Only works for EBtcCmdSend.
 *
 * @param aFunctionId Identifier of the function to be invoked. Only supports EBtcCmdSend and
 * KMTMStandardFunctionsSendMessage.
 * @param aSelection Selection of message entries for the requested function to operate on.
 * @param aParameter Buffer containing input and output parameters.
 * @param aCompletionStatus Canonical TRequestStatus used for control of the active object.
 * @return Pointer to a new asynchronously completing CMsvOperation. If failed, this is a completed operation with 
 * status set to the relevant error code.
 * @leave Leaves if no memory is available, or if the specified aFunctionId is unsupported.
 */	{
	__TEST_INVARIANT_VIRTUAL

	CMsvOperation* op = NULL;
	switch (aFunctionId)
		{
	case KMTMStandardFunctionsSendMessage:
			{
			// parameter ignored, it's assumed that the 1st addressee will contain all the
			// required information to send this item. Only the 1st item in the selection
			// list is sent.
			CreateMessageOperationL(op, aSelection, aCompletionStatus);
			break;
			}
	case EBtMtmCmdSend:
			{
			/*
			In order to get our password over the client-server boundary without breaking the client interface
			we will repack the client side package buffer into a server package buffer.
			*/
			TPckgBuf<CBtClientMtm::SBtcCmdSendServerParams> serverParams;

			TPckgBuf<CBtClientMtm::SBtcCmdSendParams>& clientParams = (TPckgBuf<CBtClientMtm::SBtcCmdSendParams>&)aParameter;

			/* at this point, it may be worth checking the length of the supplied password
			and leaving with KErrArgument if it's longer than allowed */
			if(!clientParams().iConnectPassword || clientParams().iConnectPassword->Length() > KBlueToothObexPasswordLength)
				User::Leave(KErrArgument);
				
			serverParams().iTimeouts = clientParams().iTimeouts;
			serverParams().iRemoteObexPort = clientParams().iRemoteObexPort;
			serverParams().iConnectPassword = *(clientParams().iConnectPassword);

			op = Session().TransferCommandL(aSelection,	aFunctionId, serverParams,
											aCompletionStatus);
			break;
			}
	default:
		User::Leave(KErrNotSupported);
		}
	return(op); // ownership of op is passed to caller
	}


void CBtClientMtm::CreateMessageOperationL(
	CMsvOperation*& aOperation, const CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus)
	{		
	// must have at least 1 addressee (if more than 1 the others are ignored)
	if (AddresseeList().Count() == 0)
		{
		User::Leave(KErrArgument);
		}
	
	// extract and parse addressee information
	TPckgBuf<CBtClientMtm::SBtcCmdSendServerParams> serverParams;
	serverParams().iRemoteObexPort = 0;
	
	// Set some default values for the timeouts in case they have not been added to
	// the addressee.
	serverParams().iTimeouts.iConnectTimeout = 0;
	serverParams().iTimeouts.iPutTimeout = 0;

	// address already saved when addressee was added (and we know there is at least one)
	TBuf8<KBlueToothObexDeviceAddressLength> addressIgnored; 
	TBuf<KBlueToothObexPasswordLength> password;
	ParseDestinationL(AddresseeList()[0], addressIgnored, password, 
		serverParams().iTimeouts.iConnectTimeout, 
		serverParams().iTimeouts.iPutTimeout);

	serverParams().iConnectPassword = password;
			
	// obex send operation wrapper, supporting standard progress
	CObexSendOperation* send = new(ELeave) CObexSendOperation(Session(), aCompletionStatus);
	CleanupStack::PushL(send);
	// start the send operation
	CMsvOperation* op = 
		Session().TransferCommandL(aSelection,	EBtMtmCmdSend, serverParams, send->iStatus);
	CleanupStack::PushL(op);
	send->Start(op);
	CleanupStack::Pop(2, send); // op, send
	aOperation = send;
	}

void CBtClientMtm::AddAddresseeL(const TDesC& anAddressee)
	{
	// check addressee parses ok - checks all fields (that are there)
	TBuf8<KBlueToothObexDeviceAddressLength> address;
	TBuf<KBlueToothObexPasswordLength> ignored1;
	TInt ignored2(0);
	TInt ignored3(0);
	ParseDestinationL(anAddressee, address, ignored1, ignored2, ignored3);
	
	// parses ok, try to save it - fails if there is already an addressee
	CObexClientMtm::AddAddresseeL(anAddressee);	

	// parses and saves ok, store device address in header
	iHeader->SetAddrL(address);
	}

/**
 * Parse bluetooth addressee field
 *
 * @param aFieldTag Field tag ID (see TBtClientMtmAddresseeFieldType).
 * @param aField The next field (descriptor buffer of the correct size for the given field).
 * @param aFieldList Addressee field list.
 * @leave Leaves with KErrArgument if parameter or formatting incorrect.
 */
TBool CBtClientMtm::ParseDestinationFieldL(TUint16 aFieldTag, TDes8& aField, TPtrC& aFieldList)
	{
	TBool isDeviceAddress = (aFieldTag == EDeviceAddress);

	aField.Zero();
	TInt length = aFieldList.Length();
	if (!isDeviceAddress)
		{
		// end of field list
		if (length == 0)
			{
			return EFalse;
			}
		// expecting tag
		if (length < 2 || aFieldList[0] != ':' || aFieldList[1] != aFieldTag)
			{
			User::Leave(KErrArgument);
			}
		// remove tag and marker
		aFieldList.Set(aFieldList.Right(aFieldList.Length() - 2));	
		}

	// locate next tag		
	TInt fieldLength = aFieldList.Locate(':');
	if (fieldLength == KErrNotFound)
		{
		fieldLength = aFieldList.Length();
		}

	// twice as many bytes
	TInt fieldLength8 = fieldLength << 1;
	if (fieldLength8 > aField.MaxLength())
		{
		User::Leave(KErrArgument);
		}
	
	// copy field	
	aField.Copy((TUint8*)aFieldList.Ptr(), fieldLength8);

	// remove field from field list
	aFieldList.Set(aFieldList.Right(aFieldList.Length() - fieldLength));	
	
	return ETrue;
	}	
	
/**
 * Parse bluetooth addressing information from addressee list.
 *
 * @param aDeviceAddress Device address (6 bytes).
 * @param aPassword Password (max length 16).
 * @param aConnectTimeout Connection timeout.
 * @param aPutTimeout Put timeout.
 * @leave Leaves with KErrArgument if parameter or formatting incorrect.
 */
void CBtClientMtm::ParseDestinationL(const TDesC& aFieldsToParse, TDes8& aDeviceAddress, 
								   TDes16& aPassword, TInt& aConnectTimeout, TInt& aPutTimeout)
	{
	TPtrC16 fields(aFieldsToParse);

	// get device address
	if (ParseDestinationFieldL(EDeviceAddress, aDeviceAddress, fields))
		{
		// get password
		TPtr8 password((TUint8*)aPassword.Ptr(), aPassword.MaxLength()<<1);
		if (ParseDestinationFieldL(EPassword, password, fields))
			{
			aPassword.SetLength(password.Length()>>1);
			// get connection timeout
			TPckg<TInt> connectTimeout(aConnectTimeout);
			TBool parsedOk = EFalse;

			// The connect timeout can have a field tag defined by ETimeout, or a
			// field tag defined by EAlternativeConnectTimeout. This is because an
			// older version of code looked for the wrong tag (EAlternativeConnectTimeout),
			// and we need to remain backward compatible with it.
			TRAPD(err, parsedOk = ParseDestinationFieldL(ETimeout, connectTimeout, fields));

			if (err == KErrArgument)
				{
				parsedOk = ParseDestinationFieldL(EAlternativeConnectTimeout, connectTimeout, fields);
				}
			else
				{
				User::LeaveIfError(err);
				}

			if (parsedOk)
				{
				// get put timeout
				TPckg<TInt> putTimeout(aPutTimeout);
				ParseDestinationFieldL(EPutTimeout, putTimeout, fields);
				}
			}
		}
	}



#ifdef _DEBUG
void CBtClientMtm::TestInvariant() const
	{
	__ASSERT_DEBUG(iMsvEntry &&
				   iHeader &&
				   (iMsvEntry->Entry().iMtm == KUidMsgTypeBt) &&
				   (iMsvEntry->Entry().iType == KUidMsvMessageEntry),
				   User::Invariant());
	}
#endif //_DEBUG

