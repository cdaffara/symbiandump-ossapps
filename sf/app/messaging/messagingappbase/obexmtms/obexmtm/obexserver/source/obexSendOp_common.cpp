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
#include <obexsendop.h>

//System includes
#include <msvids.h>
#include <msventry.h>	//CMsvServerEntry
#include <btmsgtypeuid.h>	//KUidMsgTypeBt
#include <irmsgtypeuid.h>	//KUidMsgTypeIr
#include <obexheaderlist.h>

#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <apparc.h>		// CApaApplication::GenerateFileName
#include <apgcli.h>		// RApaLsSession   <-- data recognition.

//user includes

// Following are named KErrIrObexClientXXXXX because they are using the irobex.dll (which 
// should really be called obex.dll now that it supports more than IR)
const TInt KErrIrObexClientFirstPutFailed = -5510;
const TInt KErrIrObexClientSubsequentPutFailed = -5511;
const TInt KMaxBufSizeForRecognition = 1024;


EXPORT_C CObexServerSendOperation::CObexServerSendOperation(TUid aMsgTypeUid, CMsvServerEntry& aSendObj, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus)
	:CActive(CActive::EPriorityHigh),
		iAsyncInit(EFalse),
		iObserverRequestStatus(aObserverRequestStatus),
		iMsvSendParent(aSendObj), 
		iConnectTimeout(aConnectTimeoutMicroSeconds),
		iPutTimeout(aPutTimeoutMicroSeconds),
		iCancelWithoutCompleting(EFalse),
		iProgressPckg(TObexMtmProgress()),
		iMtm(aMsgTypeUid)
/**
 * Constructor.
 *
 *  @param aMsgTypeUid UID of message type
 *  @param aSendObj Reference to the object to send.
 *  @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
 *  @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
 *  @param aObserverRequestStatus TRequestStatus of owning active object.
 */
	{
	}

EXPORT_C CObexServerSendOperation::CObexServerSendOperation
                                   (TUid aMsgTypeUid, CMsvServerEntry& aSendObj,TInt aConnectTimeoutMicroSeconds,
                                    TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus,
                                    TBool aLastSendAttempt)
	:CActive(CActive::EPriorityHigh),
		iAsyncInit(EFalse),
		iLastSendAttempt(aLastSendAttempt),
		iObserverRequestStatus(aObserverRequestStatus),
		iMsvSendParent(aSendObj), 
		iConnectTimeout(aConnectTimeoutMicroSeconds),
		iPutTimeout(aPutTimeoutMicroSeconds),
		iCancelWithoutCompleting(EFalse),
		iProgressPckg(TObexMtmProgress()),
		iMtm(aMsgTypeUid)
		
/**
 * Constructor.
 *
 *  @param aMsgTypeUid UID of message type
 *  @param aSendObj Reference to the object to send.
 *  @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
 *  @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
 *  @param aObserverRequestStatus TRequestStatus of owning active object.
 *  @param aLastSendAttempt TBool flag to check for second send attempt and also for sending headers.  EFalse sends full headers, ETrue only sends name and size
 */
	{
	}


EXPORT_C void CObexServerSendOperation::ConstructL(const TDesC* aConnectPassword)
/**
 * Second phase constructor. Sets up connection to the FileServer, initialises attachments or filename list then 
 * starts sending process by initialising.
 *
 * @param aConnectPassword Pointer to the password to be used for authentication.
 * @leave Leaves if insufficient memory.
 * @leave Leaves if cannot connect to FileServer.
 */
	{
	if (aConnectPassword->Length())
		{
		// take our own copy of the connection password
		HBufC* password=aConnectPassword->AllocL();
		iConnectPassword=password;
		}

	BuildSpecificConstructL();

	

	TRequestStatus* observerStatus = &iObserverRequestStatus;
	*observerStatus = KRequestPending;
	


	// If no attachments have been found, try using the Attachment API
		// Try Attachment API...
		CMsvStore* store = iMsvSendParent.ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& manager = store->AttachmentManagerL();
		iAttachmentEntryCount = manager.AttachmentCount();
		iNextAttachment = iAttachmentEntryCount-1;
		CleanupStack::PopAndDestroy(store);


	iTimeoutTimer = CObexSendOpTimeout::NewL(this);

	CActiveScheduler::Add(this);
	iStatus = KRequestPending;
	SetActive();

	if ( iAttachmentEntryCount > 0 )
		{
		// START SENDING
		iSendState = TObexMtmProgress::EInitialise;
		CompleteSelf(KErrNone);
		}
	else
		{
		//Nothing to send
		iSendState = TObexMtmProgress::ESendError;
		CompleteSelf(KErrNone);
		}
	}

EXPORT_C CObexServerSendOperation::~CObexServerSendOperation()
/**
 * Destructor. Cancel()s, deletes owned objects and Close()s the connection to the FileServer.
 */
	{
	Cancel();
	delete iObexClient;
	delete iObexObject;
	delete iTimeoutTimer;
	delete iConnectPassword;
	delete iMoveOperation;
	delete iMoveEntrySelection;


	BuildSpecificDestructor();
	}

void CObexServerSendOperation::TimeOut()
/**
 * Called when the current operation times out. Causes the current operation to be cancelled, then reactivates with
 * the appropriate error code (KErrIrObexClientNoDevicesFound or KErrIrObexClientPutPeerAborted).
 */
	{
	switch (iSendState)
		{
	case TObexMtmProgress::EInitialise:
	case TObexMtmProgress::EConnect:
	case TObexMtmProgress::EConnectAttemptComplete:
		ActivateRunLWithError(KErrIrObexClientNoDevicesFound);
		break;
	case TObexMtmProgress::ESendObject:		// Next RunL: start first send (not started sending yet)
	case TObexMtmProgress::ESendNextObject:	// Next RunL: start next send (still sending current object)
	case TObexMtmProgress::ESendComplete:		// Next RunL: nothing left to send (still sending current object)
	case TObexMtmProgress::EDisconnected:		// Trying to disconnect
		ActivateRunLWithError(KErrIrObexClientPutPeerAborted);
		break;
	default:
		Panic(EObexSendOperationUnexpectedTimeout);
		}
	}

void CObexServerSendOperation::ActivateRunLWithError(TInt aError)
/**
 * Cancels the current operation, then reactivates with the given error code.
 *
 * @param aError Error code to be passed to CompleteSelf.
 */
{
	ExplicitCancel();
	
	// Ensure RunL() is called to handle the error
	iSendState=TObexMtmProgress::ESendError;
	SetActive(); // Schedule so can CompleteObserverL() using given error code when RunL() is called again
	CompleteSelf(aError);
}

void CObexServerSendOperation::ExplicitCancel()
/**
 * Cancel any pending obex operation without completing the observer. 
 */
	{
	// Cancel any pending obex operation without completing the observer (the observer
	// owns this messaging operation, it's request status is iObserverRequestStatus)
	iCancelWithoutCompleting = ETrue; //this prevents the observer from completing when DoCancel() is called by the active object framework
	Cancel();
	iCancelWithoutCompleting = EFalse;
	}

EXPORT_C void CObexServerSendOperation::DoCancel()
/**
 * Cancels the current operation, deletes the client and Cancel()s the timeout timer. Only completes the observer
 * (by a call to CompleteObserverL) if an external entity (i.e. the owner) has called Cancel().
 * Otherwise the observer is not completed.
 */
	{
	// Have to delete the Obex client here or it mucks up the active scheduler. 
	// Abort doesn't cut the mustard.
	delete iObexClient;
	iObexClient=NULL;

	// Delete the Obex object as we may try to delete the message store entry
	// when we complete the observer and this will fail as the iObexObject has the
	// associated attachment file open.
	delete iObexObject;
	iObexObject = NULL;

	iTimeoutTimer->Cancel();

	if(!iCancelWithoutCompleting)
		{
		// We will only get here if an external entity has called Cancel() on us, only
		// the user (owner) of this active object will do this. In most cases this
		// will be the direct result of a user action, e.g. cancelling a dialog
		iSendState = TObexMtmProgress::EUserCancelled;
		CompleteObserverL(); // complete the observer which owns this messaging op
		}
	}

EXPORT_C TBool CObexServerSendOperation::CompletingObserver(TInt aErrorCode)
/**
* This is the default implementation that allows the server mtm to continue normally.
* Tells the derived class that the base class is about to complete the observer.
* This is the first thing called when CompleteObserverL is called.
* Since the behaviour of CompleteObserverL is to clean up the message that it was trying to send,
* this calls gives the derived class an opportunity to either stop this deletion or recover any information
* synchronously from the message.
* If the derived class has no need to use this functionality, the default implementation allows deletion.
* @param aErrorCode The last error code encountered during operation
* @return TBool True delete the message
* @return TBool False DO NOT delete the message
*/
	{
	(void)aErrorCode;

	return ETrue;
	}

void CObexServerSendOperation::CompleteObserverL() //CMsvOperation completed.
/**
 * Complete the observer, reporting any error via the progress. THIS METHOD MUST BE CALLED ONCE ONLY.
 *
 */
	{
	ProgressL();
	TObexMtmProgress& progress = iProgressPckg();

	// If the message should be deleted then do it here.
#ifndef MOVE_MESSAGE_TO_SENT

	// this call gives the derived class an opportunity to control this behaviour.
	
	// note that the error passed into this is the final one which the server mtm wants to
	// report to the UI - occasionally, this will be KErrNone when in fact an Obex error
	// did occur.  To make sure that we tell the derived class exactly what happened, we
	// pass the current state of the progress indicator out to it.

	if(CompletingObserver(progress.iError))
		SynchronousEntryDelete();
#endif

	TRequestStatus* status = &iObserverRequestStatus;
	User::RequestComplete(status, KErrNone);
	}

void CObexServerSendOperation::CompleteSelf(TInt aError)
/**
 * This causes this active object's request to complete which means
 * RunL() will be called again if we are active (immediately if there 
 * are no higher priority active objects in the active scheduler).
 *
 * @param aError Error to be passed forward to the next step of the state machine
 */
	{
	// This causes this active objects request to complete which means
	// RunL() will be called again if we are active (immediately if there 
	// are no higher priority active objects in the active scheduler).
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aError);
	}

EXPORT_C void CObexServerSendOperation::RunL()
/**
 * Calls RealRunL(), and traps errors
 *
 * @leave Leaves with errors from RealRunL()
 */
	{
	TRAPD(err, RealRunL());

	if(err)
		{
		ExplicitCancel(); //don't complete the observer, just cancel sending activity

		// if RealRunL left in ESendError, sending it back would cause an endless loop!
		if (iSendState == TObexMtmProgress::ESendError)
			{
			iStatus = err;
			CompleteObserverL();

			// update iStatus "manually" so that it shows the REAL error (the error that 
			// made RealRunL leave, not the previous error that made it go to ESendError)
			// NOTE: if we left iStatus unchanged here, the observer would get the error
			// that caused the send operation to fail. If for any reason this were thought to be
			// better, just removing the following line would do.
			}
		else
			{
			iSendState = TObexMtmProgress::ESendError;

			// this is so that RunL-RealRunL are called immediately to handle the error
			// (the observer will be completed in RealRunL)

			SetActive();
			CompleteSelf(err);
			}

		User::Leave(err); // Allow system to display an error.
		}
	}

/**
 * This is not required to do anything in the base implementation.
 */

EXPORT_C void CObexServerSendOperation::SecondPhaseObexClientInitL()
	{
	}

EXPORT_C void CObexServerSendOperation::PreConnectOperations()
	{
	}

EXPORT_C void CObexServerSendOperation::PostConnectOperations()
	{
	}

EXPORT_C void CObexServerSendOperation::PreSendOperations()
	{
	}

EXPORT_C void CObexServerSendOperation::PostSendOperations()
	{
	}

TInt CObexServerSendOperation::SynchronousEntryDelete()
/**
 * Delete the outbox entry as operation has 'completed'.
 * Will be invisible&InPreparation anyway (MS delete will delete it the next 
 * time it starts).
 */
	{
	// Delete the outbox entry as operation has 'completed'.
	// Will be invisible&InPreparation anyway (MS delete will delete it the next 
	// time it starts).
	TInt err;
	const TMsvEntry& tMsvSentEntry = iMsvSendParent.Entry();
	TInt messageId = tMsvSentEntry.Id();
	err = iMsvSendParent.SetEntry(tMsvSentEntry.Parent());
	if (err == KErrNone)
		{
		err = iMsvSendParent.DeleteEntry(messageId);
		}
	return err;
 	}
	
void CObexServerSendOperation::InitialiseAttachmentL(CMsvServerEntry& aParent, TInt aWhichAttachment)
/**
 * Load an attachment into the obex sending buffer, and create a new Obex object of name TMsvEntry::iDetails.
 *
 * @param aParent Reference to CMsvServerEntry to be sent.
 * @param aWhichAttachment Zero-based index of attachment to send.
 * @leave KErrXXX system wide error codes
 */
	{
	// Load an attachment into the obex sending buffer

		// use the Attachment API to load the attachment files
		CMsvStore* store = aParent.ReadStoreL();
		CleanupStack::PushL(store);
		CMsvAttachment* attachment = store->AttachmentManagerL().GetAttachmentInfoL(aWhichAttachment);
		CleanupStack::PushL(attachment);
		LoadFileIntoObjectL(attachment->FilePath(), attachment->AttachmentName(), attachment->MimeType());
		
		// if any additional obex headers have been provided, set them
		// in the obex object
		TPtrC8 headerData;
		if( attachment->GetDesC8Attribute(KUidObexHeaders, headerData) == KErrNone )
			{
			// header data exists, try to build an obex header list
			CObexHeaderList* headerList = CObexHeaderList::NewLC();
			headerList->ImportFromAttachmentL(*attachment);
			headerList->AddHeadersToBaseObjectL(*iObexObject);
			CleanupStack::PopAndDestroy(headerList);
			}
			
		CleanupStack::PopAndDestroy(2, store); // attachment, store
	}

void CObexServerSendOperation::LoadFileIntoObjectL(const TDesC& aFileName, const TDesC& aObexName, const TDesC8& aMimeType)
	{
	// Load the file into our send data buffer.
	// Create a new Obex object from the file.

	// Check we actually have a file
	TEntry fileEntry;
	User::LeaveIfError( FileSession().Entry(aFileName, fileEntry) );
	if(fileEntry.IsDir())
		User::Leave(KErrNotSupported); //Can't send a directory.

	//Store the filename associated with the obex object
	iSendFile = aFileName;
	
	// Get the file length
	RFile file;
	User::LeaveIfError( file.Open(FileSession(), aFileName, EFileRead  | EFileShareReadersOnly) );
	CleanupClose<RFile>::PushL(file);
	TInt fileLength;
	User::LeaveIfError( file.Size(fileLength) );

	// To allow file type recognition, read the file into sendBody
	// only need the first KMaxBufSizeForRecognition bytes
	HBufC8* sendBody = HBufC8::NewLC(fileLength < KMaxBufSizeForRecognition ? fileLength : KMaxBufSizeForRecognition);
	TPtr8 sendBodyPtr = sendBody->Des();
	User::LeaveIfError( file.Read(sendBodyPtr) );
	CleanupStack::Pop(); //sendBodyPtr;
	CleanupStack::PopAndDestroy(); //file.Close();
	CleanupStack::PushL(sendBody);	//Must be a better way to do this!

	// Create an Obex object from the file.
	delete iObexObject;
	iObexObject=NULL;
	iObexObject = CObexFileObject::NewL(iSendFile);

	// Set the obex object name field 
	if(aObexName.Length()!=0)
		{
		iObexObject->SetNameL(aObexName); // Only *set* a header field if have some data, otherwise Obex will assert.
		}
	
	if( aMimeType.Length() > 0 )
		{
		iObexObject->SetTypeL(aMimeType);
		}
	else
		{
		// Attempt to recognise the datatype (MIME type) of the data in the send buffer
		// but only set obex object type field if we are pretty confident of the 
		// Recogniser's result
		RApaLsSession lsSess;
		if( lsSess.Connect() == KErrNone)
			{
			CleanupClosePushL(lsSess);

			TDataRecognitionResult result;
			User::LeaveIfError( lsSess.RecognizeData(aObexName, sendBodyPtr, result) );
			if (result.iConfidence>=CApaDataRecognizerType::EProbable)
				{
				iObexObject->SetTypeL(result.iDataType.Des8());	//only set a header field if have some data, otherwise Obex will assert.
				}
			CleanupStack::PopAndDestroy();	// lsSess;
			}
		}

	// Set obex object length field for benefit of remote device's UI
	iObexObject->SetLengthL(fileLength);

	// Set obex object time field
	TTime modifiedTime;
	FileSession().Modified(aFileName, modifiedTime);
	iObexObject->SetTimeL(modifiedTime);

	//Cleanup
	CleanupStack::PopAndDestroy();	//sendBody
	}


EXPORT_C void CObexServerSendOperation::GetUserPasswordL(const TDesC& /*aRelm*/)
/**
 * Called by the Obex Client when authentication is requested to pass the password back. If the password is invalid, this
 * call should succeed but the send operation as a whole will inevitably fail.
 *
 * @param aRelm ignored, but could be used to indicate which password to use.
 * @leave KErrXXX system wide error codes. Shouldn't leave just because the password is invalid.
 * @leave KErrIrObexConnectChallRejected, if password is found empty.
 */
	{
	//A "pusher" may not know in advance that the remote will issue an OBEX authentication challenge when connecting an OBEX session.  
	// As such if a password is not provided the MTM will return, KErrIrObexConnectChallRejected
	if(!iConnectPassword)
		{
		User::Leave(KErrIrObexConnectChallRejected);		
		}
	
	//Simply pass back the default user password
	iObexClient->UserPasswordL(*iConnectPassword);
	}


TBool CObexServerSendOperation::CheckStatusOfLastObject(TInt aStatus, TObexMtmProgress::TSendState aSendState)
/**
 * Checks the last object was sent correctly, and tries to action appropriate error feedback if not. Only to be called
 * from ESendObject/ESendNextObject or ESendComplete states. 
 *
 * @param aStatus Status of last object
 * @return ETrue if message was OK--EFalse if message failed and this function has taken the necessary action
 */
	{
	if(aStatus != KErrNone)
		{ 
		if ((aStatus==KErrInUse) || (aStatus==KErrArgument))
			{
			//May indicate a problem with sending the previous object, or an error with a previous connection still
			//being active--try and decide which
			if (aSendState == TObexMtmProgress::ESendNextObject)
				{
				// Have managed to send at least one object - receiver might not be 
				// able to accept multiple objects.
				CompleteSelf(KErrIrObexClientSubsequentPutFailed); //would be useful if this err was handled with a "Try sending one at a time" message to the user
				}
			else
				{
				// Have connected OK but couldn't send, so receiver might be dealing with
				// a previous connection
				CompleteSelf(KErrIrObexClientFirstPutFailed); //different to EMultiObjectSendError
				}
			}
		else
			{
			// May indicate an error with the first or subsequent object
			// Pass error code onto the owner of this messaging operation
			CompleteSelf(aStatus); // General error.
			}

		iSendState=TObexMtmProgress::ESendError;
		SetActive();
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

TInt CObexServerSendOperation::PrepareCurrentObjectAndSetStateL()
/**
 * Loads the next object to be sent, whether an attachment or a file in the file list.
 *
 * @return KErrXXX standard error code
 * @return KErrNotFound if there were neither attachments nor files in the file list
 * @leave KErrXXX system wide error codes
 */
	{
		// Have attachment(s) to be sent.
		InitialiseAttachmentL(iMsvSendParent, iNextAttachment); //Load data into send buffer.
		--iNextAttachment;
		if(iNextAttachment<0) //iNextAttachment is 0 for a 1 attachment entry.
			{
			// Send will be complete after the current object has been sent so
			// go to next state after the send operation completes
			iSendState=TObexMtmProgress::ESendComplete;
			}
		else
			{
			// Still have another object to send after current object has been
			// sent so return to this state after the send operation completes
			iSendState=TObexMtmProgress::ESendNextObject;
			}

		//Successfully sent an attachment specified object
		return KErrNone;
	}

void CObexServerSendOperation::MoveToSentAndSetActiveL()
/**
 * Moves the newly sent message to the global sent items folder, and sets active ready for its completion.
 *
 * @leave KErrXXX system wide error codes
 */
	{
	iMoveEntrySelection = new (ELeave) CMsvEntrySelection;
	iMoveEntrySelection->AppendL(iMsvSendParent.Entry().Id());		//i.e. just the sent message
	iMsvSendParent.SetEntry(iMsvSendParent.Entry().Parent());		//Move to the parent
	iMsvSendParent.MoveEntriesL(*iMoveEntrySelection, KMsvSentEntryId, iStatus);
	iSendState = TObexMtmProgress::EMovedToSent;
	SetActive();
	}

void CObexServerSendOperation::CleanupAfterMovedToSent()
/**
 * Restores after the message has been moved to the inbox, and marks the message as visible.
 */
	{
	iMsvSendParent.SetEntry(iMoveEntrySelection->At(0));	//Switch back to actual message
	TMsvEntry entry = iMsvSendParent.Entry();
	entry.SetVisible(ETrue);			// Moved OK. Make the entry visible and flag it as complete.
	entry.SetInPreparation(EFalse);
	iMsvSendParent.ChangeEntry(entry);		//Commit changes
	iMsvSendParent.SetEntry(KMsvNullIndexEntryId);	//Unlock the entry
	}


RFs& CObexServerSendOperation::FileSession()
/**
 * Returns a reference to the file session (RFs) of the message
 *
 * @return A reference to the file session of the the message 
 */
	{
	return iMsvSendParent.FileSession();
	}


//
//
// CObexSendOpTimeout 
//
//

CObexSendOpTimeout* CObexSendOpTimeout::NewL(CObexServerSendOperation* aSendOperation)
/**
 *Canonical NewL function, which also sets the owner operation.
 *
 *@param aSendOperation Obex send operation which will be "timed out" when the timer expires
 */
	{
	CObexSendOpTimeout* self=new (ELeave) CObexSendOpTimeout;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iSendOperation = aSendOperation;
	CleanupStack::Pop();
	return self;
	}

CObexSendOpTimeout::CObexSendOpTimeout()
	: CTimer(CActive::EPriorityStandard)
	  // Construct  zero-priority active object
/**
 * Constructor. Calls CTimer's constructor with priority EPriorityStandard
 */
	{};

void CObexSendOpTimeout::ConstructL()
/**
 * Second phase constructor. Calls CTimer::ConstructL(), and adds itself to the active scheduler
 *
 * @leave KErrXXX system wide error codes
 */
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this); 
	}

void CObexSendOpTimeout::RunL()
/**
 * Calls the TimeOut method of the associated CObexServerSendOperation when the timer expires
 *
 * @leave KErrXXX system wide error codes
 */
	{
	iSendOperation->TimeOut();
	}




//
//
// Panic
//
//

GLDEF_C void Panic(TObexSendOperationPanic aPanic)
	{
	_LIT(KObexSendOperationPanic,"ObexSendOp");
	User::Panic(KObexSendOperationPanic,aPanic);
	}

