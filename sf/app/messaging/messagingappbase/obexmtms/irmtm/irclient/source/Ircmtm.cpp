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
// $Workfile: IRCMTM.CPP $
// $Author: Stevep $
// $Revision: 9 $
// $Date: 23/01/02 14:21 $
// 
//

//class include
#include "IRCMTM.H"

//system includes
#include <e32std.h>
#include <e32base.h>
//#include <eikenv.h>
#include <txtrich.h>	// CRichText
#include <mtmuids.h>	// KUidMtmQueryCanSendMsg
#include <msvreg.h>		// CRegisteredMtmDll
#include <mtmdef.h>		// KUidMtmQueryxxx & TMsvPartList flags
#include <msvuids.h>	// KUidMsvMessageEntry
#include <irmtmcmds.h>	// EIrMtmCmdSend

//user includes
#include <irheader.h>
#include "irmsgtypeuid.h" //KUidMsgTypeBt
#include "CIRCancelObserver.h"
#include <cobexsendoperation.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#include <mtmuidsdef.hrh>
#endif

EXPORT_C CIrClientMtm* CIrClientMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
/**
 * Canonical NewL factory function. 
 *
 * @param aRegisteredMtmDll Reference to registration data for MTM DLL.
 * @param aMsvSession Reference to CMsvSession of the client requesting the object.
 * @return Pointer to a new, constructed CIrClientMtm
 * @leave Leaves if no memory is available.
 */
	{
	CIrClientMtm* self = new(ELeave) CIrClientMtm(aRegisteredMtmDll, aMsvSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CIrClientMtm::CIrClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
: CObexClientMtm(aRegisteredMtmDll, aMsvSession, KUidMsgTypeIrUID)
/**
 * Constructor
 *
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aMsvSession CMsvSession of the client requesting the object.
 */
	{
	}

void CIrClientMtm::InitialiseHeaderL()
/**
 * Deletes the old header, then creates a new CIrHeader.
 *
 *  @leave KErrXXX System-wide error codes if allocation fails
 */
	{
	delete iHeader;
	iHeader = 0;

	iHeader = CIrHeader::NewL();
	}

CMsvOperation* CIrClientMtm::InvokeAsyncFunctionL(TInt aFunctionId, 
												    const CMsvEntrySelection& aSelection,
													TDes8& aParameter, 
												    TRequestStatus& aCompletionStatus)
/**
 * Starts an asynchronous messaging function as an active object. Only works for EIrcCmdSend.
 *
 * @param aFunctionId Identifier of the function to be invoked. Only supports EIrcCmdSend.
 * @param aSelection Selection of message entries for the requested function to operate on.
 * @param aParameter Buffer containing input and output parameters.
 * @param aCompletionStatus Canonical TRequestStatus used for control of the active object.
 * @return Pointer to a new asynchronously completing CMsvOperation. If failed, this is a completed operation with 
 * status set to the relevant error code.
 * @leave KErrXXX System-wide error codes
 * @leave KErrNotSupported if aFunctionId != EIrcCmdSend
 */
	{
	CMsvOperation* operation = NULL;
	switch (aFunctionId)
		{
		case KMTMStandardFunctionsSendMessage:
			{
			CObexSendOperation* send = new(ELeave) CObexSendOperation(Session(), aCompletionStatus);
			CleanupStack::PushL(send);
			CIRCancelObserver* observer = CIRCancelObserver::NewL(send->iStatus, Session(), aSelection[0]);
			CleanupStack::PushL(observer);
			CMsvOperation* op = Session().TransferCommandL(aSelection, EIrcCmdSend, aParameter,
															observer->Status());
			observer->SetOperation(op);
			send->Start(observer);
			CleanupStack::Pop(2, send); // send, observer
			operation = send;
			break;
			}
	case EIrcCmdSend:
			{
			CIRCancelObserver* cancelObserver = CIRCancelObserver::NewL(aCompletionStatus, Session(), aSelection[0]);
			CleanupStack::PushL(cancelObserver);
			CMsvOperation* op = Session().TransferCommandL(aSelection,	aFunctionId, aParameter,
											cancelObserver->Status());
			cancelObserver->SetOperation(op);
			CleanupStack::Pop(cancelObserver);
			operation = cancelObserver;
			break;
			}
	default:
		User::Leave(KErrNotSupported);
		}
	return operation; // ownership of operation is passed to caller
	}
