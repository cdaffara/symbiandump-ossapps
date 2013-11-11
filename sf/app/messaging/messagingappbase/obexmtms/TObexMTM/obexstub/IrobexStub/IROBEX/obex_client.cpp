// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// obex_client.cpp [stub]
// oscarg, September 2003.
// This is a modified version of Obex_Client.cpp. 
// Modified file for BT/IR OBEX stub
// 
//

#include <utf.h>

#include "debug.h"
#include "obex.h"
#include "obexutil.h"
#include "obextestheaderlist.h"

// file to read desired result from:
_LIT(KConfigFileName, "C:\\msgtest\\obexsmoke\\stub\\conf.txt");

// possible parameters passed to the test state in the script file
_LIT(KResultSuccess,			"succeed"			);
_LIT(KResultConnectRetry,		"connect_retry"		);
_LIT(KResultConnectFail,		"connect_fail"		);
_LIT(KResultSendNextFail,		"send_next_fail"	);
_LIT(KResultSendCompleteFail,	"send_complete_fail");
_LIT(KResultDisconnectFail,	"disconnect_fail"	);
_LIT(KResultInitCancel,		"init_cancel"		);
_LIT(KResultConnectCancel,		"connect_cancel"	);
_LIT(KResultSendCancel,		"send_cancel"		);
_LIT(KResultSendNextCancel,	"send_next_cancel"	);
_LIT(KResultDisconnectCancel,	"disconnect_cancel"	);

// enumeration with possible results of send operation
enum TSendOpResult
	{
	ESendOpOk				= 0,
	ERetryAtConnectAttempt,
	EFailAtConnectAttempt,
	EFailAtSendNextObject,
	EFailAtSendComplete,
	EFailAtDisconnect,
	ECancelAtInitialise,
	ECancelAtConnect,
	ECancelAtSend,
	ECancelAtSendNext,
	ECancelAtDisconnect
	};

// we need 2 new variables but we don't want to modify the class 
// definitions in the .h files.
// we use 2 global variables in Thread Local Storage.
struct Globals
	{
	TInt iDesiredResult;		// desired result for the send operation
	TBool iFirstConnectAttempt; // used in "connect_retry"
	CObexTestHeaderList* iVerifyHeaders;
	};

// get a pointer to the struct holding the 2 global variables
Globals* TheGlobals()
	{
	return static_cast<Globals*>(Dll::Tls());
	}
//

// maps the descriptors representing a desired result to 
// the elements of TSendOpResult.
// At the moment, by default it assumes the operation 
// will succeed (this could be changed so that, for example,
// it left if the supplied parameter is invalid).
TInt ParseDesiredResult(const TDesC& aStr)
	{
	TSendOpResult ret = ESendOpOk;
	if (aStr.CompareC(KResultSuccess) == 0)
		{
		ret = ESendOpOk;
		}
	else if (aStr.CompareC(KResultConnectRetry) == 0)
		{
		ret = ERetryAtConnectAttempt;
		}
	else if (aStr.CompareC(KResultConnectFail) == 0)
		{
		ret = EFailAtConnectAttempt;
		}
	else if (aStr.CompareC(KResultSendNextFail) == 0)
		{
		ret = EFailAtSendNextObject;
		}
	else if (aStr.CompareC(KResultSendCompleteFail) == 0)
		{
		ret = EFailAtSendComplete;
		}
	else if (aStr.CompareC(KResultDisconnectFail) == 0)
		{
		ret = EFailAtDisconnect;
		}
	else if (aStr.CompareC(KResultInitCancel) == 0)
		{
		ret = ECancelAtInitialise;
		}
	else if (aStr.CompareC(KResultConnectCancel) == 0)
		{
		ret = ECancelAtConnect;
		}
	else if (aStr.CompareC(KResultSendCancel) == 0)
		{
		ret = ECancelAtSend;
		}
	else if (aStr.CompareC(KResultSendNextCancel) == 0)
		{
		ret = ECancelAtSendNext;
		}
	else if (aStr.CompareC(KResultDisconnectCancel) == 0)
		{
		ret = ECancelAtDisconnect;
		}

	return ret;
	}

//
// class CObexClient
//

CObexClient::CObexClient()
	{
	iPendingRequest = NULL;
	iCurrentOperation = EOpIdle;
	}

void CObexClient::ConstructL(TObexProtocolInfo& aObexProtocolInfoPtr)
	{

// uncomment to specify a desired result without reading it from file
//#define TESTING_WITHOUT_CONF_FILE
#ifdef TESTING_WITHOUT_CONF_FILE
	//iDesiredResult = ESendOpOk;
	//iDesiredResult = ERetryAtConnectAttempt;
	iDesiredResult = EFailAtConnectAttempt;
	//iDesiredResult = EFailAtSendNextObject;
#else
	Globals* globals = static_cast<Globals*> (Dll::Tls());
	if (globals == NULL)
		{
		globals = new(ELeave) Globals;
		Mem::FillZ(globals,sizeof(Globals));
		Dll::SetTls(globals);
		}
	globals->iFirstConnectAttempt = ETrue;


	// Internalise the expected result, and verify headers if present.
	CObexTestHeaderList* headerList = CObexTestHeaderList::NewL();
	CleanupStack::PushL(headerList);

	headerList->AutoInternaliseL(KConfigFileName);

	globals->iDesiredResult = ParseDesiredResult(headerList->ExpectedResultL());

	CleanupStack::Pop(headerList);
	globals->iVerifyHeaders = headerList;

#endif // #ifdef TESTING_WITHOUT_CONF_FILE

	if (globals->iDesiredResult == ECancelAtInitialise)
		{
		User::Leave(KErrCancel); //?
		}
	CObex::ConstructL(aObexProtocolInfoPtr); // parent's constructor
	}


EXPORT_C CObexClient::~CObexClient()
	{
	FLOG(_L("CObexClient Destructor\r\n"));
	Error(KErrCompletion);
	Globals* globals = static_cast<Globals*>(Dll::Tls());
	delete globals->iVerifyHeaders;
	if (globals != NULL)
		{
		delete globals;
		globals = NULL;
		Dll::SetTls(NULL);
		}
	}

EXPORT_C void CObexClient::UserPasswordL(const TDesC& /*aPassword*/)
	{
	}

EXPORT_C const CObexHeaderSet& CObexClient::GetPutFinalResponseHeaders()
{
   return iCurrentObject->HeaderSet();
}
  
EXPORT_C CObexClient* CObexClient::NewL(TObexProtocolInfo& aObexProtocolInfoPtr)
/**
	Creates a new CObexClient object. The passed protocol information object 
	specifies what protocol to use, currently only the TObexIrProtocolInfo 
	concrete class is supported. See the CObex section for a full description 
	of this class. 
**/
	{
	CObexClient* self = new(ELeave) CObexClient();
	CleanupStack::PushL(self);
	self->ConstructL(aObexProtocolInfoPtr);
	CleanupStack::Pop();
	return(self);
	}


EXPORT_C void CObexClient::Connect(CObexBaseObject& aObject, TRequestStatus& aStatus)
/**
	Simulates a connection to a remote machine.
	This just calls ClientCommandL, which is where things actually happen...
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Connect no password but some header data\r\n"));
		iChallenge = EFalse;
		OBEX_TRAP(Error, ClientCommandL(EOpConnect, static_cast<TAny*>(&aObject), aStatus));
		}
	}

EXPORT_C void CObexClient::ConnectL(CObexBaseObject& aObject, const TDesC& aPassword, 
								   TRequestStatus& aStatus)
/**
	Simulates a connection to a remote machine.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Connect with password and some header info\r\n"));
		iChallenge = ETrue;
		delete iChallPassword;
		iChallPassword = NULL;
		iChallPassword = HBufC8::NewL(aPassword.Size());
		TPtr8 ptr = iChallPassword->Des();
		CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, aPassword);
		iChallenge = ETrue;
		OBEX_TRAP(Error, ClientCommandL(EOpConnect, static_cast<TAny*>(&aObject), aStatus));
		}
	}


EXPORT_C void CObexClient::ConnectL(const TDesC& aPassword, TRequestStatus& aStatus)
/**
	Simulates a connection to a remote machine.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Connect with password but no header info\r\n"));
		delete iChallPassword;
		iChallPassword = NULL;
		iChallPassword = HBufC8::NewL(aPassword.Length());
		TPtr8 ptr = iChallPassword->Des();
		CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, aPassword);
		iChallenge = ETrue;
		OBEX_TRAP(Error, ClientCommandL(EOpConnect, NULL, aStatus));
		}
	}

EXPORT_C void CObexClient::Connect(TRequestStatus& aStatus)
/**
	Simulates a connection to a remote machine.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Connect no password no header\r\n"));
		iChallenge = EFalse;
		OBEX_TRAP(Error, ClientCommandL(EOpConnect, NULL, aStatus));
		}
	}

EXPORT_C void CObexClient::Disconnect(TRequestStatus& aStatus)
/**
	Simulates a disconnection from the remote machine.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Disconnect\r\n"));
		OBEX_TRAP (Error, ClientCommandL(EOpDisconnect, NULL, aStatus));
		}
	}

EXPORT_C void CObexClient::Put(CObexBaseObject& aObject, TRequestStatus& aStatus)
/**
	Simulates an attempt to OBEX put aObject onto the server.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Put\r\n"));

		OBEX_TRAP(Error, ClientCommandL(EOpPut, static_cast<TAny*>(&aObject), aStatus));
		}
	}

EXPORT_C void CObexClient::Get(CObexBaseObject& aObject, TRequestStatus& aStatus)
/**
	Simulates getting an object from the remote machine.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::Get\r\n"));

		OBEX_TRAP(Error, ClientCommandL(EOpGet, static_cast<TAny*>(&aObject), aStatus));
		}
	}

EXPORT_C void CObexClient::SetPath(TSetPathInfo& aPathInfo, TRequestStatus& aStatus)
/**
	Simulates changing the remote device's current path.
	In the OBEX stub it does not do anything.
**/
	{
	if(!AlreadyActive(aStatus))
		{
		FLOG(_L("CObexClient::SetPath\r\n"));

		OBEX_TRAP(Error, ClientCommandL(EOpSetPath, static_cast<TAny*>(&aPathInfo), aStatus));
		}
	}

EXPORT_C void CObexClient::Abort()
	{
	}

TBool CObexClient::AlreadyActive(TRequestStatus& aStatus)
//can't do more than one request at atime
	{
	if(iPendingRequest)
		{
		TRequestStatus* tempStatus = &aStatus;
		User::RequestComplete(tempStatus,KErrAccessDenied);
		return ETrue;
		}
	return EFalse;
	}


//

void CObexClient::ClientCommandL(TOperation aOp, TAny* aParam, TRequestStatus& aStatus)
	{
	SetRequest(aStatus, aOp);

	Globals* globals = TheGlobals();

	switch(aOp)
		{
	case EOpConnect:
		
		// emulate the callback from the obex client to the MTM code 
		// that occurs when the MTM has a password.

		if (iChallPassword)
			{
			SetConnectState(EWaitForUserInput);
			iCallBack->GetUserPasswordL(_L("Stub"));
			}
		switch(globals->iDesiredResult)
			{
		case ERetryAtConnectAttempt:
			// fail the 1st time, succeed the next one
			if (globals->iFirstConnectAttempt)
				{
				globals->iFirstConnectAttempt = EFalse;
				User::Leave(KErrTimedOut);
				}
			else
				{
				globals->iFirstConnectAttempt = ETrue;
				CompleteRequest(KErrNone);
				}
			break;
		case EFailAtConnectAttempt: 
			User::Leave(KErrCouldNotConnect);
			break;

		case ECancelAtConnect:
			CompleteRequest(KErrCancel);
			break;
		default:
			CompleteRequest(KErrNone);
			break;
			}

		break;
	case EOpDisconnect:
		// called by CObexClient::DisconnectL()
		// the SendOperation is in TObexMtmProgress::EMovedToSent state

		switch (globals->iDesiredResult)
			{
		case EFailAtDisconnect:
			CompleteRequest(KErrCouldNotDisconnect);
			break;
		case ECancelAtDisconnect:
			CompleteRequest(KErrCancel);
			break;
		default:
			CompleteRequest(KErrNone);
			break;
			}
		
		break;
	case EOpPut:
	case EOpGet:
		{
		TInt err = KErrNone;
		TBool match = ETrue;

		// called from CObexClient::Put()
		// the SendOperation is in TObexMtmProgress::ESendNextObject state
		

		// The current obex object which holds the headers.
		iCurrentObject = static_cast<CObexBaseObject*>(aParam);
		
		// Access the obex header lists.
		CObexTestHeaderList* verifyHeaderList = globals->iVerifyHeaders;
//		CObexHeaderList* verifyHeaderList = globals->iVerifyHeaders;
		CObexHeaderSet&	headerList = iCurrentObject->HeaderSet();
		
		// Holder for a header.
		CObexHeader* header = CObexHeader::NewL();
		CleanupStack::PushL(header);

		headerList.SetMask(NULL);

		// Verify each header in the verify header list,
		// with the headers in the obex object header list.
		for (TInt i = 0; i < verifyHeaderList->GetCount(); i++)
			{
				CObexHeader* verifyHeader = verifyHeaderList->GetHeaderByIndexL(i);

				// Find the header.
				headerList.First();	 // Reset to top of list.
				err = headerList.Find(verifyHeader->HI(), *header);
				if (err == KErrNone)
				{
					// Compare the HV.
					switch (header->Type())
					{
						case (0x00): //Unicode
							{
							if (verifyHeader->AsUnicode().Compare(header->AsUnicode()))
								match = EFalse;
							break;
							}
						case (0x01): // ByteSeq
							{
							if (verifyHeader->AsByteSeq().Compare(header->AsByteSeq()))
								match = EFalse;
							break;
							}
						case (0x02): // Byte
							{
							if (verifyHeader->AsByte() != header->AsByte())
								match = EFalse;
							break;
							}
						case (0x03): //FourByte
							{
							if (verifyHeader->AsFourByte() != header->AsFourByte())
								match = EFalse;
							break;
							}
						default:
							{
								match = EFalse;
								break;
							}
					}
				}
				else
					match = EFalse;
			}
		CleanupStack::Pop(header);
		delete header;


		switch(globals->iDesiredResult)
			{
			case EFailAtSendNextObject:
				//CompleteRequest(KErrGeneral);
				CompleteRequest(KErrCommsLineFail);
				break;
			case ECancelAtSend:
				CompleteRequest(KErrCancel);
				break;
			default:
				{
					if (match)
						CompleteRequest(KErrNone);

					// Header don't match fail.
					else
						CompleteRequest(KErrNotFound);
					
				break;
				}
			}
		}
		break;

	case EOpSetPath:

		break;
	default:
		User::Leave(KErrNotSupported);
		}

	}


void CObexClient::OnPacketReceive(TObexPacket& /*aPacket*/)
	{
	}


TInt CObexClient::ParseConnectPacket(TObexPacket& /*aPacket*/)
	{
	return KErrNone;
	}

void CObexClient::OnError(TInt aError)
	{
	CompleteRequest(aError);
	}



TInt CObexClient::PrepareConnectPacket(TObexPacket& /*aPacket*/)
	{
	return KErrNone;
	}

void CObexClient::OnTransportUp()
	{
	}

void CObexClient::OnTransportDown()
	{
	}

void CObexClient::SetRequest(TRequestStatus& aStatus, TOperation aOperation)
	{
	aStatus = KRequestPending;
	iPendingRequest = &aStatus;
	iCurrentOperation = aOperation;
	}

void CObexClient::CompleteRequest(const TInt aCompletion)
	{
	FLOG(_L("CObexClient::CompleteRequest\r\n"));
	if(iPendingRequest)
		User::RequestComplete(iPendingRequest, aCompletion);// Sets iPendingRequest = NULL
	iPendingRequest=NULL;
	iCurrentOperation = EOpIdle;
	}
