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

#ifndef __BTCMTM_H__
#define __BTCMTM_H__

#include <obexclientmtm.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#endif


/* This defines the maximum length that the connection password can be for BlueTooth.  The PDU used for this
   has a payload of 16 bytes.
*/
#define KBlueToothObexPasswordLength 16
const TInt KBlueToothObexDeviceAddressLength = 6; 	// 6 byte device address

extern const TUint8 KObexConnectionIDHeader;

class CBtClientMtm : public CObexClientMtm
/**
Bluetooth Client MTM.

Provides client-side functionality for bluetooth messaging. This is a thin implementation over the top of
CObexClientMtm.

@publishedPartner
@released
*/
	{
public:

	/**
	 * Canonical NewL factory function. 
	 *
	 * @param aRegisteredMtmDll Reference to registration data for MTM DLL.
	 * @param aMsvSession Reference to CMsvSession of the client requesting the object.	
	 * @return Pointer to a new, constructed CBtClientMtm
	 * @leave Leaves if no memory is available.
	 */
	IMPORT_C static CBtClientMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);

	// Contains the timeout values for connect and put operations
	struct STimeouts
		{
		TInt iConnectTimeout; //< Connection attempt timeout in microseconds
		TInt iPutTimeout;	  //< Put attempt timeout in microseconds
		};
	
	// Structure for parameter needed for sending via Bluetooth
	struct SBtcCmdSendParams
		{
		STimeouts iTimeouts; //< Structure containing connect and put operation timeouts
		TInt iRemoteObexPort; //< The value of the remote obex port
		TDesC* iConnectPassword; //<password sent to the remote server for a client initiated obex authentication challenge
		};
		
	// Internal version of SBtcCmdSendParams 
	struct SBtcCmdSendServerParams
		{
		STimeouts iTimeouts; //< Structure containing connect and put operation timeouts
		TInt iRemoteObexPort; //< The value of the remote obex port
		TBufC<KBlueToothObexPasswordLength> iConnectPassword; //<password sent to the remote server for a client initiated obex authentication challenge 
		};

	// --- RTTI functions ---
	
	/**
	 * Starts an asynchronous function as an active object. Only works for EBtcCmdSend.
	 *
	 * @param aFunctionId Identifier of the function to be invoked. Only supports EBtcCmdSend.
	 * @param aSelection Selection of message entries for the requested function to operate on.
	 * @param aParameter Buffer containing input and output parameters.
	 * @param aCompletionStatus Canonical TRequestStatus used for control of the active object.
	 * @return Pointer to a new asynchronously completing CMsvOperation. If failed, this is a completed operation with 
	 * status set to the relevant error code.
	 * @leave Leaves if no memory is available, or if the specified aFunctionId is unsupported.
	 */

	virtual CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus); //aParameter should package up SBtcCmdSendParams

	/**
	Add a Bluetooth addressee

	@param anAddressee Addressee field of the following format


	_LIT(KTestAddressee1, "XXX:\1Password:\2AA:\3BB");

	_LIT(KTestAddressee2, "XXX:\1Password:\2AA");

	_LIT(KTestAddressee3, "XXX:\1Password");

	_LIT(KTestAddressee4, "XXX");


	XXX		: BT device address (6 bytes)
	:\1		: Password tag sequence (4 bytes) [EBtClientMtmAddresseeFieldTypePassword]
	Password	: 0-16 characters (0-32 bytes)
	:\2		: Connection timeout tag sequence (4 bytes) [EBtClientMtmAddresseeFieldTypeConnectionTimeout]
	AA		: Connection timeout (1 TInt = 4 bytes) 
	:\3		: Put timeout tag sequence (4 bytes) [EBtClientMtmAddresseeFieldTypePutTimeout]
	BB		: Put timeout (1 TInt = 4 bytes)
	 
	@leave Leaves with KErrArgument if addresse badly formatted or corrupt.
	 */
	virtual void AddAddresseeL(const TDesC& anAddressee);

private:
	/*
	 * Addressee field tags
	 * Note: The EAlternativeConnectTimeout exists because in an older version of
	 * the code, we were looking for the wrong field tag in the addressee field to
	 * get the connect timeout value. We should have been looking for the ETimeout value
	 * from this Enum, but instead we were incorrectly using a constant defined elsewhere.
	 * The incorrectly used constant had a value of 0x08. In order to remain backward
	 * compatible, we now look for both constants.
	 */
	enum TBtClientMtmAddresseeFieldType
		{
		EDeviceAddress,
		EPassword,
		ETimeout,
		EPutTimeout,
		EAlternativeConnectTimeout = 0x08
		};
	
	/**
	 * Constructor--not for use by client applications
	 *
	 * @param aRegisteredMtmDll Registration data for MTM DLL.
	 * @param aMsvSession CMsvSession of the client requesting the object. 
	 */

	CBtClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);
	
	/**
	 * Deletes the old header, then creates a new CBtHeader.
	 *
	 * @leave KErrXXX System-wide error codes if allocation fails
	 */
	
	virtual void InitialiseHeaderL();
	
	TBool ParseDestinationFieldL(TUint16 aFieldTag, TDes8& aField, TPtrC& aFieldList);
	void ParseDestinationL(const TDesC& aFieldsToParse, TDes8& aDeviceAddress, 
						 TDes16& aPassword, TInt& aConnectTimeout, TInt& aPutTimeout);
	void CreateMessageOperationL(CMsvOperation*& aOperation, 
		const CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus);


#ifdef _DEBUG
	virtual void TestInvariant() const;
#endif //_DEBUG
	};


#endif // __BTCMTM_H__

