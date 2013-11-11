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
// $Workfile: IRS.CPP $
// $Author: Stevep $
// $Revision: 8 $
// $Date: 25/03/02 8:49 $
// 
//

//class include
#include "IRS.H"

//system includes
#include <msventry.h>
#include <ircmtm.h>	// For the send command

//user includes
#include <irheader.h>
#include "sendopn.h"
const TInt KErrIrObexClientSubsequentPutFailed = -5511;

EXPORT_C CIrSrvMtm* CIrSrvMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry*  aEntry)
/**
 * NewL factory function. Calls ReleaseLibrary() if construction fails.
 *
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aEntry Context on which to operate.
 * @return Pointer to a newly constructed CIrSrvMtm.
 * @leave KErrXXX System-wide error codes
 */
	{
	CIrSrvMtm* mysvrmtm = new CIrSrvMtm(aRegisteredMtmDll, aEntry);
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

CIrSrvMtm::CIrSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry):
	CObexSrvMtm(aRegisteredMtmDll, aEntry)
/**
 * Constructor. Calls CObexSrvMtm's constructor in initialisation list.
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aEntry Context on which to operate.
 */
	{
	__DECLARE_NAME(_S("CIrSrvMtm"));
	}

/**
 * Destructor. Deletes the header member
 */

EXPORT_C CIrSrvMtm::~CIrSrvMtm()
	{
	delete iHeader;
	}

EXPORT_C void CIrSrvMtm::StartCommandL (CMsvEntrySelection& aSelection, 
										TInt aCommand, 
										const TDesC8& aParameter, 
										TRequestStatus& aStatus)
/**
 * Instantiates and initiates a CIrServerSemdOperation class to begin sending the specified OBEX object
 * via Infrared. Called in response to InvokeAsyncFunctionL() in the client MTM, and passes through
 * all the arguments passed in to that function. Only supports aCommand==EIrMtmCmdSend
 * @param aSelection Entry to operate on.
 * @param aCommand Command to start. Only EIrMtmCmdSend is supported.
 * @param aParameter Package buffer containing timeout, port and password info needed for the send operation.
 * @param aStatus Used to notify observer of completion.
 * @leave KErrNotSupported if aCommand!=EIrMtmCmdSend
 * @leave KErrXXX if aCommand!= EIrMtmCmdSend
 */
	{
	switch (aCommand)
		{
		//The only supported command is EIrcCmdSend; switch statement used for potential future extension
		case CIrClientMtm::EIrcCmdSend:
			{
			TPckgBuf<CIrClientMtm::STimeouts> sendParamsBuf;
			sendParamsBuf.Copy(aParameter);
			iTimeouts = sendParamsBuf();
						
			//Zeroth selection is the TMsvId of the entry to send.
			iEntry = iServerEntry->NewEntryL(aSelection[0]);
			
			TMsvEntry messageEntry=iEntry->Entry();
			messageEntry.SetSendingState(KMsvSendStateSending);
			iEntry->ChangeEntry(messageEntry);

			//Construct a header 
			delete iHeader;
			iHeader = NULL;
			iHeader = CIrHeader::NewL();

			//Get the entry's store
			CMsvStore* store = iEntry->ReadStoreL();
			CleanupStack::PushL(store);
			
			//read the header from the store, and destroy the store
			iHeader->RestoreL(*store);
			CleanupStack::PopAndDestroy();	//store

			iProtocolInfo = iHeader->IrProtocolInfo();

			iFirstSendAttempt = ETrue;

			aStatus = KRequestPending;
			iReportStatus = &aStatus;

			SendL();

			break;
			}
		default:
			User::Leave(KErrNotSupported);
		}

	}

/**
 * Creates a server send operation to perform the send
 */


void CIrSrvMtm::SendL()
	{
	// Password is not supported for IR
	TPtrC connectPassword;
	// connectPassword.Set(sendParamsBuf().iConnectPassword);

	//Now make the send operation, which will start automatically. aStatus is passed in to
	//report completion.

	delete iSendOperation;
	iSendOperation = NULL;
    
    //If NewL() leaves , the progress should be recorded 
	
	iSendOperation = CIrServerSendOperation::NewL(*iEntry, iProtocolInfo,
		iTimeouts.iConnectTimeout, iTimeouts.iPutTimeout, &connectPassword, iStatus, !iFirstSendAttempt);

	SetActive();
	}

/**
 * Obtains progress information, which, if valid, results in calling Send()
 */

EXPORT_C void CIrSrvMtm::DoRunL()
	{
	// If the first attempt to send the message fails then retry with OBEX::IrXfer

	TPckgBuf<TObexMtmProgress> package;
	package.Copy(iSendOperation->ProgressL());
	TInt error = package().iError;
	
	if ((error != KErrNone) && (iFirstSendAttempt))
		{
		// If we have made only one send attempt so far, we will retry now

		// If the error code indicates that we have not found the OBEX class
		// name, change to use the OBEX:IrXfer class name as some older
		// implementations only understand that one.
		if (error == KErrIrObexClientPeerDoesNotHaveObex)
			{
			iProtocolInfo.iClassName = KIrXferClassName;
			}

		iFirstSendAttempt = EFalse;
       if(error == KErrIrObexClientSubsequentPutFailed)
			{
			User::RequestComplete(iReportStatus, KErrNone);
			}
		else
			{
			SendL();
			}
		}
	else
		{
		// Always report with a no error, any failure is recorded in the progress.
		User::RequestComplete(iReportStatus, KErrNone);
		}
	}
