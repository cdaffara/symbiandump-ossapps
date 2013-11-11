// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// smutsimparam.cpp
//
#include <smutsimparam.h>

#include <mmlist.h>

#include "SMCMMAIN.H"
#include <smscmds.h>
#include "SMUTSET.H"

/**
The UID for the stream into which the SIM parameters are stored.

@internalComponent
@deprecated
Should be moved to the cpp file for this class.
*/
const TUid KUidMsvSimSmsParams = {0x100081BC};

/**
Creates an operation that reads the SMS parameters from the SIM.

@param	aMtm
The UID for the SMS MTM.

@param	aService
The SMS Service settings Id.

@param	aMsvSession
A message server session.

@param	aObserverRequestStatus
The observer status to be completed.

@return
The operation encapsulating reading the SIM parameters. 

@post
The SMS parameters are stored in iSimParams.

@internalComponent
*/
CSmsSimParamOperation* CSmsSimParamOperation::ReadSimParamsL(TUid aMtm, TMsvId aService, CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus)
	{
	CSmsSimParamOperation* self = new (ELeave) CSmsSimParamOperation(aMtm, aService, aMsvSession, aObserverRequestStatus);
	CleanupStack::PushL(self);
	
	self->iSimParams = CMobilePhoneSmspList::NewL();
	self->iState = EStateReadingSimParams;
	self->DoReadSimParamsL();
	
	CleanupStack::Pop(self);
	return self;
	}

/**
Creates an operation that writes the SMS parameters to the SIM.

@param	aList
The list of SIM parameters to be written.

@param	aMtm
The UID for the SMS MTM.

@param	aService
The SMS Service settings Id.

@param	aMsvSession
A message server session.

@param	aObserverRequestStatus
The observer status to be completed.

@return
The operation encapsulating writing the SIM parameters. 

@post
The SMS parameters are stored in the SIM.

@internalComponent
*/
CSmsSimParamOperation* CSmsSimParamOperation::WriteSimParamsL(const CMobilePhoneSmspList& aList, TUid aMtm, TMsvId aService, CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus)
	{
	CSmsSimParamOperation* self = new (ELeave) CSmsSimParamOperation(aMtm, aService, aMsvSession, aObserverRequestStatus);
	CleanupStack::PushL(self);
	
	self->iSimParams = CMobilePhoneSmspList::NewL();
	self->iState = EStateWritingSimParams;
	self->DoWriteSimParamsL(aList);
	
	CleanupStack::Pop(self);
	return self;
	}

/** 
Gets the operation completion/error code for reading/writing the SMS parameters.

It returns a packaged TInt (TIntBuf) and not a packaged TSmsProgress.

The code can be intrepreted as follows:

KErrNone - the SMS parameters were successfully read/writte from/to the SIM

KErrNotSupported - the telephone handset and/or TSY module does not support 
reading/writing the SMS parameters.

KErrNotFound - no SMS parameters where found on the SIM.

Other system-wide error codes - may indicate a comms, telephony or memory error.

@return
A TIntBuf containing the operation completion/error code.
*/
EXPORT_C const TDesC8& CSmsSimParamOperation::ProgressL()
	{
	iProgressBuf = TIntBuf(iError);
	return iProgressBuf;
	}

/**
Constructor.
*/
CSmsSimParamOperation::CSmsSimParamOperation(TUid aMtm, TMsvId aService, CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus)
:	CMsvOperation(aMsvSession, EPriorityStandard, aObserverRequestStatus),
	iError(KErrNone)
	{
	iService = aService;
	iMtm = aMtm;
	CActiveScheduler::Add(this);
	}

/**
Destructor.
*/
CSmsSimParamOperation::~CSmsSimParamOperation()
	{
	Cancel();
	delete iSimParams;
	delete iOperation;
	}

/**
Retrieves the read SIM parameters from the stream in the SMS Service entry.

The read SIM parameters are placed in the member data iSimParams.

@pre
The async read SIM parameters operation is complete.

@post
The parameters are retrived from the stream in the SMS Service entry and stored
in iSimParams.

@leave	KErrNotFound
There were no SIM parameters to read from the SIM.
*/
void CSmsSimParamOperation::DoRunReadSimParamsL()
	{
	CMsvEntry* entry = iMsvSession.GetEntryL(iService);
	CleanupStack::PushL(entry);
			
	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	RestoreSimParamsL(*store, *iSimParams);

	const TInt count = iSimParams->Enumerate();

	if (!count)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(2, entry); //entry, store
	}

/**
Completes the observer with the value in iError.
*/
void CSmsSimParamOperation::Complete()
	{
	TRequestStatus* status = &iObserverRequestStatus;
	User::RequestComplete(status, iError);
	}

/**
Reads the SIM parameters from the SIM.

A read SIM parameters command is transferred to the SMS server MTM. It is this
operation that does the actual reading of SIM parameters.

@post
A ESmsMtmCommandReadSimParams comand operation is started.
*/
void CSmsSimParamOperation::DoReadSimParamsL()
	{
	TransferCommandL(ESmsMtmCommandReadSimParams);
	}

/**
Writes the SMS parameters to the SIM.

The new parameters to be stored are written to a stream in the SMS Service entry.
A write SIM parameters command is transferred to the SMS server MTM. It is this
operation that does the actual writing of SIM parameters.

@param	aList
A list wit the new SIM parameters.

@post
The new SIM parameters are stored in the message store and a ESmsMtmCommandWriteSimParams
comand operation is started.
*/
void CSmsSimParamOperation::DoWriteSimParamsL(const CMobilePhoneSmspList& aList)
	{
	CMsvEntry* entry = iMsvSession.GetEntryL(iService);
	CleanupStack::PushL(entry);
		
	CMsvStore* store = entry->EditStoreL();
	CleanupStack::PushL(store);

	StoreSimParamsL(*store, aList);

	CleanupStack::PopAndDestroy(2, entry); 

	TransferCommandL(ESmsMtmCommandWriteSimParams);
	}

/**
Requests the specified asynchronous command from the SMS server MTM.

The command Ids are defined by TSmsMtmCommand.

@param	aCommandId
The Id of the requested command.

@post
The requested command is started.

@see	TSmsMtmCommand
*/
void CSmsSimParamOperation::TransferCommandL(TInt aCommandId)
	{
	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);
	sel->AppendL(iService);

	delete iOperation;
	iOperation = NULL;
	iOperation = iMsvSession.TransferCommandL(*sel, aCommandId, TPtrC8(), iStatus);

	CleanupStack::PopAndDestroy(sel);
	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

/**
Gets a list of Service Centre addresses.

This function can only be used after a read/write SIM parameters operation has
completed. If the CSmsSimParamOperation::ProgressL API returns any an error
code other than KErrNone, or the SIM parameters contain no Service Centre 
addresses, then this function returns an empty list.

@return
The list of SMS parameters that only contain Service Centre addresses. A pointer
to the list object is left on the cleanup stack.
*/
EXPORT_C CMobilePhoneSmspList* CSmsSimParamOperation::ServiceCentersLC() const
	{
	TInt count = 0;
	count = iSimParams->Enumerate();

	CMobilePhoneSmspList* list = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(list);

	// Order important
	for (TInt i = 0; i < count; ++i)
		{
		const RMobileSmsMessaging::TMobileSmspEntryV1& smspEntry = iSimParams->GetEntryL(i);

		if (smspEntry.iValidParams & RMobileSmsMessaging::KSCAIncluded)
			{
			list->AddEntryL(smspEntry);
			}
		}
	return list;
	}

/**
Stores the contents of aList into the message store.

The supplied SIM parameters are stored in the KUidMsvSimSmsParams stream of the
message entry store specified. This should be the SMS Service entry. No checking
is performed to verify that the store is for the SMS service entry.

@param	aStore
The message entry store to write the parameters into.

@param	aList
The SMS parameters to be written.

@post
The contents of aList are stored in the message store. 

@internalTechnology
*/
EXPORT_C void CSmsSimParamOperation::StoreSimParamsL(CMsvStore& aStore, const CMobilePhoneSmspList& aList)
	{
	aStore.Remove(KUidMsvSimSmsParams);
	
	RMsvWriteStream writeStream;
	writeStream.AssignLC(aStore, KUidMsvSimSmsParams);

	writeStream << aList;
	writeStream.CommitL();
	aStore.CommitL();

	CleanupStack::PopAndDestroy(&writeStream);
	}

/**
Restores the SIM parameters from message store.

The SIM parameters are restored from the KUidMsvSimSmsParams stream of the 
message entry store specified. This should be the SMS Service entry. No checking
is performed to verify that the store is for the SMS service entry.

@param	aStore
The message entry store to read the parameters from.

@param	aList
An output argument to conatin the restored SMS parameters.

@leave	KErrNotFound
The stream did not exist in the specified message entry store.

@post
The output argument aList contains the restored SIM parameters.

@internalTechnology
*/
EXPORT_C void CSmsSimParamOperation::RestoreSimParamsL(CMsvStore& aStore, CMobilePhoneSmspList& aList)
	{
	if (aStore.IsPresentL(KUidMsvSimSmsParams))
		{
		RMsvReadStream readStream;
		readStream.OpenLC(aStore, KUidMsvSimSmsParams);

		readStream >> aList;
		readStream.Close();

		CleanupStack::PopAndDestroy(&readStream); 
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

/*
 *	Methods from CActive
 */

/**
Handles completed async operations.

If the async operation was to read the SIM parameters, then the SIM parameters 
are read from the appropriate stream in the SMS Service entry and placed in the
parameter member iSimParams.

If the async operation was to read the SIM parameters, then there is nothing 
more to do.

For both types of operation the observer for this operation is completed.

@pre
The async operation is complete

@post
The operation is complete

@leave	KErrNotFound
There were no SIM parameters to read from the SIM.

@panic	SMCM	3
There was an supported command.
*/
void CSmsSimParamOperation::RunL()
	{
	iError = iStatus.Int();

	if (iError == KErrNone)
		{
		TSmsProgressBuf progressBuf;
		progressBuf.Copy(iOperation->ProgressL());
		TSmsProgress progress = progressBuf();

		iError = progress.iError;

		if (iError == KErrNone)
			{
			switch (iState)
				{
				case EStateReadingSimParams:
					{
					DoRunReadSimParamsL();
					break;
					}
				case EStateWritingSimParams:
					{
					break;
					}
				default:
					{
					Panic(ESmscPanicUnexpectedCommand);
					}
				}
			}
		}
	
	Complete();
	}

/**
Cancels any pending async request.

The observer is completed with an value of KErrCancel.
*/
void CSmsSimParamOperation::DoCancel()
	{
	if (iOperation)
		{
		iOperation->Cancel();
		}

	iError = KErrCancel;
	Complete();
	}

/**
Handles leaves from RunL().

The observer is completed with the leave code aError.

@param	aError
The leave code from RunL().

@return
A value of KErrNone.
*/
TInt CSmsSimParamOperation::RunError(TInt aError)
	{
	iError = aError;
	Complete();
	return KErrNone;
	}
