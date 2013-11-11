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
// $Workfile: btsmtm.cpp $
// $Author: Stevep $
// $Revision: 11 $
// $Date: 25/03/02 8:49 $
// 
//

//class include
#include "btsmtm.h"

//system includes
#include <msventry.h>
#include "btmtmcmds.h"	//EBtMtmCmdSend

//user includes
#include <btheader.h>
#include "btsendop.h"
#include <btcmtm.h>

EXPORT_C CBtSrvMtm* CBtSrvMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry*  aEntry)
/**
 * NewL factory function. Calls ReleaseLibrary() if construction fails.
 *
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aEntry Context on which to operate.
 * @return Pointer to a newly constructed CBtSrvMtm.
 * @leave KErrXXX System-wide error codes
 */
	{
	CBtSrvMtm* mysvrmtm = new CBtSrvMtm(aRegisteredMtmDll, aEntry);
	if (mysvrmtm==NULL)
		{
		aRegisteredMtmDll.ReleaseLibrary();
		User::Leave(KErrNoMemory);
		}
	CleanupStack::PushL(mysvrmtm);
	mysvrmtm->ConstructL();
	CleanupStack::Pop();
	return mysvrmtm;
	}

CBtSrvMtm::CBtSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry):
	CObexSrvMtm(aRegisteredMtmDll, aEntry)
/**
 * Constructor. Calls CObexSrvMtm's constructor in initialisation list.
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aEntry Context on which to operate.
 */
	{
	__DECLARE_NAME(_S("CBtSrvMtm"));
	}


EXPORT_C void CBtSrvMtm::StartCommandL (CMsvEntrySelection& aSelection, 
										TInt aCommand, 
										const TDesC8& aParameter, 
										TRequestStatus& aStatus)
/**
 * Instantiates and initiates a CBtServerSemdOperation class to begin sending the specified OBEX object
 * via Bluetooth. Called in response to InvokeAsyncFunctionL() in the client MTM, and passes through
 * all the arguments passed in to that function. Only supports aCommand==CBtClientMtm::EBtcCmdSend
 *
 * @param aSelection Entry to operate on.
 * @param aCommand Command to start. Only CBtClientMtm::EBtcCmdSend is supported.
 * @param aParameter Package buffer containing timeout, port and password info needed for the send operation.
 * @param aStatus Used to notify observer of completion.
 * @leave KErrNotSupported if aCommand!=CBtClientMtm::EBtcCmdSend
 * @leave KErrXXX System-wide error codes
 */
	{
	switch (aCommand)
		{
		//The only supported command is EBtcCmdSend; switch statement used for potential future extension
		case EBtMtmCmdSend:
			{
			TPckgBuf<CBtClientMtm::SBtcCmdSendServerParams> sendParamsBuf;
			sendParamsBuf.Copy(aParameter);
			iTimeouts = sendParamsBuf().iTimeouts;
		
			// we're going to find out the port number ourselves in the send operation
			TPtrC connectPassword(sendParamsBuf().iConnectPassword);
			
			//Zeroth selection is the TMsvId of the entry to send.
			iEntry = iServerEntry->NewEntryL(aSelection[0]);

			//Construct a header
			CBtHeader* header = CBtHeader::NewL();
			CleanupStack::PushL(header);

			//Get the entry's store
			CMsvStore* store = iEntry->ReadStoreL();
			CleanupStack::PushL(store);

			//read the header from the store, and destroy the store
			header->RestoreL(*store);
			CleanupStack::PopAndDestroy();	//store

			aStatus = KRequestPending;
			//remember the request status for the observer
			iReportStatus = &aStatus;

			iStatus = KRequestPending;
			//Now make the send operation, which will start automatically. iStatus is passed in to
   			//report completion.
			iSendOperation = CBtServerSendOperation::NewL(*iEntry, header->BtProtocolInfo(),
				iTimeouts.iConnectTimeout, iTimeouts.iPutTimeout, &connectPassword, iStatus);
			SetActive();

			CleanupStack::PopAndDestroy();	//header
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		}

	}
