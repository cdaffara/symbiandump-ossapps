// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimapcompoundcreate.h"
#include "cimapfolder.h"
#include "cimapsyncmanager.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

CImapCompoundCreate::CImapCompoundCreate(CImapSyncManager& aSyncManager,
						 				 CMsvServerEntry& aServerEntry,
						 				 CImapSettings& aImapSettings,
						 				 const TMsvId aParent,
						 				 const TBool aIsFolder) :
	CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings),
	iParentId(aParent), iIsFolder(aIsFolder)
	
	{
	
	}

CImapCompoundCreate::~CImapCompoundCreate()
	{
	delete iLeafName;
	iFolderList.ResetAndDestroy();
	}
	
CImapCompoundCreate* CImapCompoundCreate::NewL(CImapSyncManager& aSyncManager,
						 				  	   CMsvServerEntry& aServerEntry,
						 				  	   CImapSettings& aImapSettings,
											   const TMsvId aParent, 
						  					   const TDesC& aLeafName, 
						  					   const TBool aIsFolder)
	{
	CImapCompoundCreate* self = new (ELeave) CImapCompoundCreate(aSyncManager,
																 aServerEntry,
																 aImapSettings,
																 aParent,
																 aIsFolder);
	CleanupStack::PushL(self);
	self->ConstructL(aLeafName);
	CleanupStack::Pop(self);
	return self;
	}

void CImapCompoundCreate::ConstructL(const TDesC& aLeafName)
	{
	iLeafName = aLeafName.AllocL();
	
	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

	
void CImapCompoundCreate::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCreate::StartOperation()");
	iNextStep = ECreate;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise.
*/	
TBool CImapCompoundCreate::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ECreate:		// asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCreateFolder);
			// Make the folder/mailbox name to create
			HBufC* newFolderPath = MakeFolderNameL(iParentId, *iLeafName, iIsFolder);
			CleanupStack::PushL(newFolderPath);
			
			iSession->CreateL(iStatus, *newFolderPath);

			CleanupStack::PopAndDestroy(newFolderPath);

			iProgressState = TImap4GenericProgress::EBusy;
			iNextStep = ECreateLocal;
			SetActive();
			break;
			}
		
		case ECheckFolderExists:
			{
			// This state allows to check if a folder already exists remotely.
			// It is entered if a migration occurs during the create state,
			// and is used to determine if the CREATE succeeded, even though we
			// did not receive a response prior to the operation being cancelled
			// to allow the migration to occur.
			__LOG_TEXT(iSession->LogId(), "CImapSessionManager::ECheckFolderExists");

			// Make the folder/mailbox name to create
			HBufC* newFolderPath = MakeFolderNameL(iParentId, *iLeafName, iIsFolder);
			CleanupStack::PushL(newFolderPath);
			
			iFolderList.ResetAndDestroy();
			iSession->ListL(iStatus, KNullDesC(), *newFolderPath, iFolderList);
			
			CleanupStack::PopAndDestroy(newFolderPath);

			iNextStep = EProcessFolderCheck;
			SetActive();
			break;
			}

		case EProcessFolderCheck:
			{
			if (iFolderList.Count()==0)
				{
				// the folder does not exist remotely - (re)send the CREATE
				iNextStep = ECreate;
				}
			else
				{
				// the folder exists on the remote server - create a local copy
				iNextStep = ECreateLocal;
				}
			return EFalse;
			}
		
		case ECreateLocal: 	// synchronous
			{	
			CreateLocalFolderL();
			iNextStep = EFinished;
			// fall through
			}
			
		case EFinished:		// finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCreate::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, 
			   TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		}
	return EFalse;
	}

/**
May be called in case of a genuine cancel or a cancel for migrate.
Following a genuine cancel, the compound operation will be deleted.
Following a cancel for migrate, the compound operation will be resumed,
so the iNextState is updated here to ensure the operation is
correctly restarted.

In either case, CMsgActive::DoCancel() is called to complete the
user request with KErrCancel.

Note that if the default CMsgActive::DoComplete() is overridden,
then that must also be modified to handle either case described above.
*/
void CImapCompoundCreate::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ECreate:
		case ECheckFolderExists:
			{
			// outstanding request is on session
			iSession->Cancel();
			iNextStep = ECheckFolderExists;
			break;
			}
		case EProcessFolderCheck:
		case ECreateLocal:
		case EFinished:
			{
			// self-completed or no outstanding request.
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse,
			   TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundCancelUnexpectedState));
			break;
			}
		} // end switch (iCurrentStep)
	
	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();
	}

/**
Builds the full IMAP pathname for the folder to be created.

@param aParent   id of the parent TMsvEntry
@param aLeafName name of the folder to be created
@param aIsFolder ETrue if the folder is to be a folder rather than a mailbox
@return a heap descriptor containing the full pathname.  Ownership is transferred to the caller.
*/
HBufC* CImapCompoundCreate::MakeFolderNameL(const TMsvId aParent, 
										  const TDesC& aLeafName,
										  const TBool aIsFolder)
	{
	
	// parent path is empty if creating a folder under the service entry.
	HBufC* parentpath = NULL;
	// Creates the ImapFolder without subscribe.. (EFalse is passed for not to subscribe folder)
	CImapFolder* folder = iSyncManager.GetFolderL(aParent, EFalse);
	if (aParent!=iImapSettings.ServiceId())
		{
		// Puts parentpath on the cleanupstack
		parentpath = folder->FullFolderPathL().AllocLC();
		}
	else
		{
		// otherwise parentpath is empty
		parentpath = KNullDesC().AllocLC();
		}

	const TDesC& pathSeparator = iImapSettings.PathSeparator();			
	HBufC* newFolderPath = NULL;
			
	if (*parentpath==KNullDesC)
		{
		// creating a root entry folder
		if (aIsFolder)
			{
			// +1 for trailing heirarchy path separator
			newFolderPath = HBufC16::NewLC(aLeafName.Length()+1);
			TPtr16 ptr(newFolderPath->Des());
			ptr.Copy(aLeafName);
			ptr.Append(pathSeparator);
			}
		else
			{
			// Root level mailbox
			newFolderPath = aLeafName.AllocLC();
			}
		}
	else
		{
		// creating a subfolder
		if (aIsFolder)
			{
			// +1 for heirarchy separator, +1 for trailing heirarchy path separator
			newFolderPath = HBufC16::NewLC(parentpath->Length()+aLeafName.Length()+2);
			TPtr16 ptr(newFolderPath->Des());
			ptr.Copy(*parentpath);
			ptr.Append(pathSeparator);
			ptr.Append(aLeafName);
			ptr.Append(pathSeparator);
			}
		else
			{
			// +1 for heirarchy separator
			newFolderPath = HBufC16::NewLC(parentpath->Length()+aLeafName.Length()+1);
			TPtr16 ptr(newFolderPath->Des());
			ptr.Copy(*parentpath);
			ptr.Append(pathSeparator);
			ptr.Append(aLeafName);
			}
		}
		
	CleanupStack::Pop(newFolderPath);
	CleanupStack::PopAndDestroy(parentpath);
	
	return newFolderPath;
	}


/**
Creates local mirror of the newly created folder.

*/
void CImapCompoundCreate::CreateLocalFolderL()
	{
	TMsvEmailEntry message;
	SetEntryL(iParentId);
	// No need to set iMtmData1-3 or iSize to 0 as this is done on construction.
	
	message.iType=KUidMsvFolderEntry;
	message.iMtm=KUidMsgTypeIMAP4;
	message.iServiceId=iImapSettings.ServiceId();
	message.SetValidUID(EFalse);
	message.SetMailbox(ETrue); // Default to creating a mailbox
	message.SetComplete(ETrue);
	message.iDetails.Set(*iLeafName);
	iServerEntry.CreateEntry(message);
	// Save the created id for the progress response
	iNewFolderId=message.Id();
	}


void CImapCompoundCreate::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	// Create does not set iOperation, it just sets iState to EBusy 
	// when doing the creation
	aCompoundProgress.iGenericProgress.iState = iProgressState;
	aCompoundProgress.iGenericProgress.iReturnedMsvId = iNewFolderId;
	
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}


/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is reported in the 
progress report, and the operation completes.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundCreate::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ECreate:
		case ECheckFolderExists:
			// Skip creation of local folder...
			iNextStep = EFinished;
			break;
		case EProcessFolderCheck:
		case ECreateLocal:
		case EFinished:
		case ESuspendedForMigrate:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundUnexpectedState);
			break;
			}
		
		} // end switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}

/**
Called to resume the compound operation following a bearer migration.
*/
void CImapCompoundCreate::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCreate::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundUnexpectedState));
	iStopForMigrate = EFalse;

	// Switch on next step
	switch (iNextStep)
		{
		case ECreate:
		case ECheckFolderExists:
			{
			// Restart the operation
			CompleteSelf();
			break;
			}
		
		case ECreateLocal:
		case EFinished:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		} // end switch (iNextStep)
	Queue(aStatus);
	}
