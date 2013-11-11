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
// $Workfile: obexServerMtm.cpp $
// $Author: Stevep $
// $Revision: 8 $
// $Date: 25/01/02 16:16 $
// 
//

//class include
#include <obexservermtm.h>
//system includes
#include <msventry.h>
#include <msvids.h>
//user includes
#include <obexsendop.h>


EXPORT_C CObexSrvMtm::CObexSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry):
	CBaseServerMtm(aRegisteredMtmDll, aEntry)
/**
 * Constructor
 *
 * @param aRegisterMtmDll	registration data for MTM dll
 * @param aEntry			Message Server entry to operate upon
 */
	{
	__DECLARE_NAME(_S("CObexSrvMtm"));
	iSendOperation = 0;
	}


EXPORT_C void CObexSrvMtm::ConstructL()
/**
 * Second phase constructor adds Obex server Mtm (this) to the active scheduler
 */	
	{
	User::LeaveIfError(iServerEntry->SetEntry(KMsvNullIndexEntryId));
	CActiveScheduler::Add(this);
	}

EXPORT_C CObexSrvMtm::~CObexSrvMtm()
/**
 * Destructor: Cancels active object (this)s
 */
	{
	Cancel();	//classes derived from CActive MUST call Cancel()
	delete iEntry;
	delete iSendOperation;
	}



EXPORT_C void CObexSrvMtm::SetInitialEntry(CMsvServerEntry* aEntry)
/**
 * Sets message entry to be sent to aEntry
 *
 * @param aEntry message server entry (service)
 */
	{
	iEntry = aEntry;  // MUST copy as gain ownership - delete or will stay locked.
	}






EXPORT_C void CObexSrvMtm::CopyFromLocalL(const CMsvEntrySelection& /*aSelection*/,
								 TMsvId /*aServiceId*/, /* Info for protocol....*/ 
								 TRequestStatus& /*aStatus*/)
/**
 * Copies a selection of entries from a local location to a remote location
 * N.B. Leaves as not supported
 *
 * @param aSelection The collection of message index entries for which the copy is required
 * @param aServiceId The entry ID of the service by which the entries should be transferred
 * @param aStatus Indicates operation completion
 */
	{
	User::Leave(KErrNotSupported);
	return;
	}


EXPORT_C const TDesC8& CObexSrvMtm::Progress()
/**
 * Obtains progress from the send operation and copies it into the iProgress member
 * 
 * @return a TBufPckg<TImSmtpProgress maintained by the instantiated CMsgImOutboxSend object
 */
	{
	if (iSendOperation)
		{
		//Get a local copy of the returned package buffer, which may be deleted at any moment...
		TRAP_IGNORE(iProgress.Copy(iSendOperation->ProgressL()));
		}

	//
	return iProgress;
	}



EXPORT_C void CObexSrvMtm::DoCancel()
/**
 * Cancels the obex send operation
 */
	{
	iSendOperation->Cancel();   
	User::RequestComplete(iReportStatus,KErrCancel);
	}


EXPORT_C void CObexSrvMtm::DoRunL()
/**
 * 
 */
	{
	User::RequestComplete( iReportStatus, KErrNone );
	}

EXPORT_C void CObexSrvMtm::DoComplete(TInt /*aError*/)
/**
 *
 */
	{
	}

EXPORT_C TBool CObexSrvMtm::CommandExpected()
/**
 * 
 */
	{
	return EFalse;
	}

//
//  StartCommandL()
//
/*STP pure virtual in the base class.
EXPORT_C void CObexSrvMtm::StartCommandL(CMsvEntrySelection& aSelection,
                                TInt aCommand,
                                const TDesC8& aParameter,
                                TRequestStatus& aStatus)
	{
	iReport=&aStatus;
	User::RequestComplete(iReport, KErrNone);   // iOutboxSend actually does nothing.
	}
*/

//*****************************************************************************
//The rest of the commands are unsupported
//*****************************************************************************

// Messages are sent via the InvokeAsync command method on the client MTM.


EXPORT_C void CObexSrvMtm::MoveFromLocalL(const CMsvEntrySelection& /*aSelection*/,
                                 TMsvId /*aServiceId*/,
                                 TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::CopyToLocalL(const CMsvEntrySelection& /*aSelection*/,
                               TMsvId /*aDestination*/, 
                               TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::MoveToLocalL(const CMsvEntrySelection& /*aSelection*/,
                               TMsvId /*aDestination*/,
                               TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::CopyWithinServiceL(const CMsvEntrySelection& /*aSelection*/,
                                     TMsvId /*aDestination*/, TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::MoveWithinServiceL(const CMsvEntrySelection& /*aSelection*/,
                                     TMsvId /*aDestination*/,
                                     TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::DeleteL(const CMsvEntrySelection& /*aSelection*/, TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::DeleteAllL(const CMsvEntrySelection& /*aSelection*/,
                             TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::CreateL(TMsvEntry /*aNewEntry*/,
                          TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}

EXPORT_C void CObexSrvMtm::ChangeL(TMsvEntry /*aNewEntry*/,
                          TRequestStatus& aStatus)
/**
 * Function not supported
 */
	{
	iReportStatus=&aStatus;
	User::RequestComplete(iReportStatus,KErrNotSupported);
	}







