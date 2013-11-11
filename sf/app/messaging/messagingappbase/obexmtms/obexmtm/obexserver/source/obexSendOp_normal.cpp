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
// $Workfile: obexSendOp_normal.cpp $
// $Author: Stevep $
// $Revision: 20 $
// $Date: 25/03/02 10:53 $
// 
//

//class include
#include <obexsendop.h>

#ifndef __OBEX_SEND_OP_FILE_DEBUG_MODE__

//System includes
#include <msvids.h>
#include <msventry.h>	//CMsvServerEntry
#include <btmsgtypeuid.h>	//KUidMsgTypeBt
#include <irmsgtypeuid.h>	//KUidMsgTypeIr
#include <obexclientmtm.h>
//#include <obexpindlguid.h> //KObexPinNotifierUid
#include <obexconstantsinternal.h>

//user includes
#define KObexPinNotifierUid 0	//STP TEMPORARY!

// error resolver compatible error numbers - Now taken from obex.h
//const TInt KErrIrObexClientNoDevicesFound = -5501;
//const TInt KErrIrObexClientPutPeerAborted = -5503;

// Following are named KErrIrObexClientXXXXX because they are using the irobex.dll (which 
// should really be called obex.dll now that it supports more than IR)
const TInt KErrIrObexClientFirstPutFailed = -5510;

// Obex Send Operation specific constants 
_LIT8(KObexClientLocalWho,"Epoc device");

//
//  CObexServerSendOperation....	CMsvOperation derived op that is intended to be owned by a CMsvReporterOperation or similar. 
//							When this completes, the CMsvReporterOperation completes too
//

void CObexServerSendOperation::BuildSpecificConstructL()
/**
 * Normal second phase constructor.
 */
	{
	}

void CObexServerSendOperation::BuildSpecificDestructor()
/**
 * Destructor. Cancel()s, deletes owned objects and Close()s the connection to the FileServer.
 */
	{
	}

EXPORT_C const TDesC8& CObexServerSendOperation::ProgressL()
/**
 * Returns current progress information.
 *
 * @return A reference to a TPckgC<TObexMtmProgress> package pointer descriptor containing progress information on this send operation.
 * @leave KErrXXX system wide error codes
 */
	{
	// State operation type (and progress).
	TObexMtmProgress progress;

	//cases are due to inconsistencies between S60 and Sym Obex TObexMtmProgress
	switch(iSendState)
		{
		case TObexMtmProgress::EInitialised:
			progress.iSendState = TObexMtmProgress::EInitialise;
			break;
		case TObexMtmProgress::EMovedToSent:
			progress.iSendState = TObexMtmProgress::ESendComplete;
			break;
		default:
			progress.iSendState = iSendState;
			break;
		}

	progress.iTotalEntryCount = iAttachmentEntryCount;
	progress.iEntriesDone = progress.iTotalEntryCount - (iNextAttachment+1);
	progress.iCurrentEntrySize = iObexObject? iObexObject->Length() : 0; 
	progress.iCurrentBytesTrans = iObexObject? iObexObject->BytesSent() : 0;

	if (progress.iSendState == TObexMtmProgress::EUserCancelled)
		{
		progress.iError = KErrCancel;
		}
	else
		{
		progress.iError = iStatus.Int();
		}

	// prepate package buffer for return value, we copy it to a data member so progress
	// data isn't destroyed when we go out of scope
	TPckgC<TObexMtmProgress> progressDes(progress); 
    iProgressPckg.Copy(progressDes); //important to use copy() not set(), set() just points to our local descriptor 
	return iProgressPckg;
	}

void CObexServerSendOperation::RealRunL()
/**
 * Implementation of the send operation state machine. Progresses as:
 * Initialise-->Connect-->ConnectAttemptComplete-->SendObject-->(SendNextObject-->)SendComplete-->Disconnected
 * The SendNextObject state is repeated for each attachment in excess of one. 
 * Also handles UserCancelled and SendError states by CompleteObserver()ing with appropriate error codes.
 * Leaves will be passed back to RunL and handled there. 
 *
 * @leave KErrXXX system wide error codes
 */
	{
	const TInt status=iStatus.Int();

	switch(iSendState)
		{
	case TObexMtmProgress::EInitialise:
		{
		// Initialise Obex client object.
		InitialiseObexClientL(); //to be implemented by derived classes and this is transport specific

		if(!iAsyncInit)
			{
			iSendState = TObexMtmProgress::EInitialised;
			SetActive();
			CompleteSelf(KErrNone);
			}
		else
			{
			iSendState = TObexMtmProgress::EInitialised;
			iStatus = KRequestPending;
			SetActive();
			}
		break;
		}

	case TObexMtmProgress::EInitialised:
		{
		// any work to be done in the second phase of initialisation can be done here by derived classes.
		SecondPhaseObexClientInitL();

		iObexClient->SetLocalWho(KObexClientLocalWho);

		// Set up call back for authentication challenge. If the server sends an
		// an authentication challenge GetUserPasswordL() will be called in this class
		iObexClient->SetCallBack(*this);
		
		// We do this here so that we can re-enter the connect phase until this timeout 
		// expires in the case where there is no device present.

		// Timeout for repeatedly trying to connect to remote OBEX device
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel();
		
		if(iConnectTimeout)
			iTimeoutTimer->After(iConnectTimeout);  
		
		// move to connect phase 
		iSendState = TObexMtmProgress::EConnect;
		SetActive();
		CompleteSelf(KErrNone);
		break;
		}

	case TObexMtmProgress::EConnect:
		{
		PreConnectOperations();

		// Attempt an OBEX connect. 
		// If Connect is not successful (see TObexMtmProgress::EConnectAttemptComplete
		// below) then we will return to this state and try to connect again, this will
		// continue until we successfully connect or iConnectTimeout expires.
		iSendState=TObexMtmProgress::EConnectAttemptComplete;
		iStatus=KRequestPending;
		SetActive();

		// Pass this active objects iStatus to OBEX Connect Request, so that when
		// Connect completes our RunL() will be called
		if (iConnectPassword)
			{
			//Only use password if its length > 0
			if (iConnectPassword->Length() > 0)
				{
				// connect with a normal authentication challenge for the remote obex server
				iObexClient->ConnectL(*iConnectPassword,iStatus);
				}
			else
				{
				// no authentication challenge
				iObexClient->Connect(iStatus);
				}
			}
		else
			{
			// no authentication challenge
			iObexClient->Connect(iStatus);
			}
		break;
		}

	case TObexMtmProgress::EConnectAttemptComplete:
		{
		// A connection _attempt_ has completed - see if it was sucessful.
		if (status == KErrNone)
			{
			// Ok - remote Obex device found and connected to
			if(iTimeoutTimer->IsActive())
				iTimeoutTimer->Cancel(); //cancel connect timeout
			iSendState=TObexMtmProgress::ESendObject; //Go to send state
			CompleteSelf(KErrNone);
			}
		else if( (status == KErrNotFound) || (status == KErrTimedOut) || (status == KErrIrObexClientNoDevicesFound) )
			{  
			// We need to determine if we should retry.
			// As long as we have our own timeout set, we should continue to retry indefinitely - 
			// the timout will guard us and we'll drop into the next case.
			if(iTimeoutTimer->IsActive())
				{
				// Connect failed. No remote device was found so we try again.
				// (KErrIrObexClientNoDevicesFound is an OBEX specific error)
				iSendState=TObexMtmProgress::EConnect; //return to connect state above
				CompleteSelf(KErrNone);
				}
			else
				{
				// We could not connect to any remote devices.  We had no local timeout
				// available, so we will not try again.
				iSendState=TObexMtmProgress::ESendError;
				CompleteSelf(KErrIrObexClientNoDevicesFound);
				}
			}
		else 
			{
			// KErrDisconnected = Found obex machine but had "Obex" IAS query failure.
			// status>0  means that 'status' obex machines have been found.
			if(iTimeoutTimer->IsActive())
				iTimeoutTimer->Cancel();
			iSendState=TObexMtmProgress::ESendError;
			CompleteSelf(status);
			}

		PostConnectOperations();

		SetActive();
		break;
		}

	case TObexMtmProgress::ESendObject:
		{
		// As this is the first object to send, perform any pre send operations required
		PreSendOperations();

		// Fall through to next state to do the send
		}

	case TObexMtmProgress::ESendNextObject:  // Multiple object support.
		{
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel(); // Cancel last timeout.

		// For multiObject, check success of previous put. (Single objects won't be affected).
		if(!CheckStatusOfLastObject (status, iSendState))
			{
			//Last message failed - CheckStatusOfLastObject has taken the necessary steps to notify
			break;
			}

		//Get the next object ready to send
		TInt err = PrepareCurrentObjectAndSetStateL();

		//Check the status of preparation
		if (err == KErrNone)
			{
			if(iPutTimeout)
				iTimeoutTimer->After(iPutTimeout); // Start a new one.

			// **** SEND THE OBJECT *****
			// Pass this active objects iStatus to OBEX Put Request, so that when
			// Put completes this RunL() will be called again
			iStatus = KRequestPending;

 			// added to mimic minimal header behaviour from 7.0 irmtm for Win98.
 			if(iLastSendAttempt)
 				iObexObject->SetHeaderMask( KObexHdrName | KObexHdrLength);			

			iObexClient->Put(*iObexObject, iStatus);
			}
		else
			{
			//Unsuccessful--report the error
			CompleteSelf(err);
			}

		SetActive();
		break;
		}

	case TObexMtmProgress::ESendComplete:
		{
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel(); //cancel iPutTimeout

		// Perform any post send operations required
		PostSendOperations();

		// Object "send(s)" have completed but latest might have completed with an error
		if(status != KErrNone)
			{  
			// Pass error code onto the owner of this messaging operation
			if( (status==KErrInUse) || (status==KErrArgument) )
				{
				// Have connected OK but couldn't send, so receiver might be dealing with
				// a previous connection
				CompleteSelf(KErrIrObexClientFirstPutFailed); //different to EMultiObjectSendError
				}
			else
				{
				CompleteSelf(status); // General error.
				}

			SetActive();
			iSendState=TObexMtmProgress::ESendError;
			return;
			}

		// we have to clean up the obex object to release the file lock.
		delete iObexObject;
		iObexObject = NULL;

		//Now attempt to move the successfully sent message to the sent folder
		iMoveEntrySelection = new (ELeave) CMsvEntrySelection;
		iMoveEntrySelection->AppendL(iMsvSendParent.Entry().Id());		//i.e. just the sent message
#ifdef MOVE_MESSAGE_TO_SENT
		User::LeaveIfError(iMsvSendParent.SetEntry(iMsvSendParent.Entry().Parent()));		//Move to the parent
		// move entry selection. The method is asynchronous and iStatus must be checked when
		// the request completes (in case TObexMtmProgress::EMovedToSent)
		iMsvSendParent.MoveEntriesL(*iMoveEntrySelection, KMsvSentEntryId, iStatus);
		iSendState = TObexMtmProgress::EMovedToSent;
		SetActive();
#else
		// Don't delete the entry here because the entry must be deleted even if the
		// operation fails or is cancelled.
		// The message will be deleted in the CompleteObserver function.
		iSendState = TObexMtmProgress::EMovedToSent;
		SetActive();
		// We need to kick ourselves into the next state because we're not waiting on anything.
		CompleteSelf(KErrNone);
#endif
		break;
		}

	case TObexMtmProgress::EMovedToSent:
		{
#ifdef MOVE_MESSAGE_TO_SENT
		// first check result of asynchronous MoveEntriesL, leave if not KErrNone
		User::LeaveIfError(iStatus.Int());
		iMsvSendParent.SetEntry(iMoveEntrySelection->At(0));	//Switch back to actual message
		TMsvEntry entry = iMsvSendParent.Entry();
		entry.SetVisible(ETrue);			// Moved OK. Make the entry visible and flag it as complete.
		entry.SetInPreparation(EFalse);
		User::LeaveIfError(iMsvSendParent.ChangeEntry(entry));		//Commit changes
		User::LeaveIfError(iMsvSendParent.SetEntry(KMsvNullIndexEntryId));	//Unlock the entry
#endif
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel();

		iStatus=KRequestPending;
		iObexClient->Disconnect(iStatus);
		
		// test code
//		User::WaitForRequest(iStatus);
		
		iSendState=TObexMtmProgress::EDisconnected;
		SetActive(); // Will call back when have completed disconnection.

		// Unfortunately, it is possible for this call to never complete if you send
		// a file that the peer does not understand. This occurs because some peers 
		// (i.e. Palm) don't seem able to handle this scenario correctly and don't 
		// drop the transport when they should. So we restart the 'connect' timeout to 
		// catch this error
		if(iConnectTimeout)
			iTimeoutTimer->After(iConnectTimeout);
		}
		break;

	case TObexMtmProgress::EDisconnected:
		{
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel();
		if(status != KErrNone)
			{
			// Pass error code onto the owner of this messaging operation
			iSendState=TObexMtmProgress::ESendError;
			SetActive();
			CompleteSelf(status);
			}
		else
			{
			// Completed OK - 
			// Signal MS that the CMsvOperation has ended, and clean up.
			CompleteObserverL();
			}
		break;
		}

	case TObexMtmProgress::EUserCancelled:
		{  
		// Signal that the CMsvOperation has been cancelled by the user.
		TObexMtmProgress& progress = iProgressPckg();
		progress.iError = KErrCancel;
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel();
		CompleteObserverL();
		break;
		}

	case TObexMtmProgress::ESendError:
		{
		// General error handling stage.
		if(iTimeoutTimer->IsActive())
			iTimeoutTimer->Cancel();
		CompleteObserverL();
		break;
		}


	default:
		Panic(EObexSendOperationUnknownSendState);
		}
	}

#endif	//__OBEX_SEND_OP_FILE_DEBUG_MODE__
