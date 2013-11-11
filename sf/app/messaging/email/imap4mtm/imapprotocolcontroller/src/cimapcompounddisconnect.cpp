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

#include "cimapmailstore.h" 
#include "cimapcompounddisconnect.h"
#include "cimapfolder.h"
#include "cimapfolderinfo.h"
#include "cimapsession.h"
#include "cimapsessionmanager.h"
#include "cimapcompoundsyncservice.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

CImapCompoundDisconnect::CImapCompoundDisconnect( CImapSyncManager& aSyncManager,
						 				  		  CMsvServerEntry& aServerEntry,
						 				  		  CImapSettings& aImapSettings,
						 				  		  CImapSessionManager& aSessionManager,
						 				  		  CImapMailStore& aImapMailStore,
						 				  		  const RPointerArray<CImapSession>& aSessionList,
						 				  		  CImapOfflineControl& aImapOfflineControl,
						 				  		  TBool aDoDeletes  )
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings ),
   iSessionManager(aSessionManager), iSessionList(aSessionList), iImapMailStore(aImapMailStore),iImapOfflineControl(aImapOfflineControl), iDoDeletes(aDoDeletes)
	{
	}

CImapCompoundDisconnect::~CImapCompoundDisconnect()
	{
	delete iCompoundSync;
	}
	
CImapCompoundDisconnect* CImapCompoundDisconnect::NewL( CImapSyncManager& aSyncManager,
						 				  				CMsvServerEntry& aServerEntry,
						 				  				CImapSettings& aImapSettings, 
						   								CImapSessionManager& aSessionManager,
						   								CImapMailStore& aImapMailStore,
						   								const RPointerArray<CImapSession>& aSessionList,
						   								CImapOfflineControl& aImapOfflineControl,
						   								TBool aDoDeletes  )
	{
	CImapCompoundDisconnect* self = new ( ELeave ) CImapCompoundDisconnect( aSyncManager,
																			aServerEntry,
																			aImapSettings,
																			aSessionManager,
																			aImapMailStore,
																			aSessionList,
																			aImapOfflineControl,
																			aDoDeletes );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CImapCompoundDisconnect::ConstructL()
	{
	CActiveScheduler::Add(this);
	iCompoundSync = CImapCompoundSyncService::NewL(iSyncManager, 
										    	   iServerEntry, 
										    	   iImapSettings,
										    	   iImapMailStore,
										    	   iImapOfflineControl,
										    	   ETrue);
	}

/**
Starts tha compound object by setting the active object active
@param aStatus
@param aSessionManager Reference to the session manager that is used to disconnect
@param aSessionList List of all connected sessions that need to be disconnected
*/
void CImapCompoundDisconnect::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundDisconnect::StartOperation()");
	if(iImapSettings.DeleteEmailsWhenDisconnecting() && iDoDeletes)
		iNextStep = EInboxLateDeletes;
	else
		iNextStep = EDisconnect;
		
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundDisconnect::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case EInboxLateDeletes:  // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDisconnect1); // Sync'ing deletes
			iCompoundSync->StartOperation(iStatus, *iSession);
			iProgressState = TImap4GenericProgress::EDisconnecting;
			iNextStep = EDisconnect;
			SetActive();
			break;
			}
		case EDisconnect:  // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDisconnect2); // disconnecting
			iSessionManager.Disconnect( iStatus, iSessionList );
			iProgressState = TImap4GenericProgress::EDisconnecting;
			iNextStep = EFinished;
			SetActive();
			break;
			}
			
		case EFinished:  // finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundDisconnect::Completing OK");
			iProgressState = TImap4GenericProgress::EDisconnected;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		} // end switch (iCurrentStep)
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
void CImapCompoundDisconnect::DoCancel()
	{
	switch (iCurrentStep)
		{
		case EInboxLateDeletes:
			{
			iCompoundSync->Cancel();
			break;
			}
		case EDisconnect:
			{
			iProgressState = TImap4GenericProgress::EDisconnecting;
			iSessionManager.Cancel();
			break;
			}
		case EFinished:
			{
			// self-completed or no outstanding request.
			iProgressState = TImap4GenericProgress::EDisconnected;
			break;
			}
		case ESuspendedForMigrate:
		default :
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectCompoundCancelUnexpectedState));
			iProgressState = TImap4GenericProgress::EIdle;
			break;
			}
		} // end switch (iCurrentStep)

	// shouldn't be restarted after a migrate (in fact, should have 
	// just been cancelled normally), but if it is, then just cut to
	// the finish on resume.
	iNextStep = EFinished;

	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();
	}

	
void CImapCompoundDisconnect::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EDisconnect;
	aCompoundProgress.iGenericProgress.iState = iProgressState;
	
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}

/**
Handles NO/BAD responses according to current step.
Negative server responses are considered fatal during disconnect.

@return KErrGeneral
*/
TInt CImapCompoundDisconnect::ProcessNegativeServerResponse()
	{
	iProgressErrorCode = iStatus.Int();
	return KErrGeneral;
	}

/**
Protocol Controller shouldn't attempt to resume a disconnect!
So this completes with KErrDisconnected.
*/
void CImapCompoundDisconnect::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundDisconnect::Resuming");
	__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectCompoundCancelUnexpectedState));
	iStopForMigrate = EFalse;

	Queue(aStatus);
	Complete(KErrDisconnected);
	}
