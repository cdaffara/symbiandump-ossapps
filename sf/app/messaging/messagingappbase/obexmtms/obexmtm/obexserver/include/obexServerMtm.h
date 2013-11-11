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
// $Workfile: obexServerMtm.h $
// $Author: Stevep $
// $Revision: 9 $
// $Date: 12/12/01 15:56 $
// 
//


#if !defined(__OBEXSERVERMTM_H__)
#define __OBEXSERVERMTM_H__


#if !defined(__MTSR_H__)
#include <mtsr.h>
#endif

#include <obexclientmtm.h>

class CObexServerSendOperation;
class CObexSrvMtm : public CBaseServerMtm
/**
@internalTechnology
@released
*/
	{
public:
	enum TObexMtmOperations
		{
		EObexMtmUiNullOp,
		EObexMtmUiObexSend
		};

public:
	
	/**
	 * Destructor: Cancels active object (this)s
	 */
	
	IMPORT_C ~CObexSrvMtm();

	IMPORT_C virtual void CopyToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	IMPORT_C virtual void MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	
	
	/**
	 * Copies a selection of entries from a local location to a remote location
	 * N.B. Leaves as not supported
	 *
	 * @param aSelection The collection of message index entries for which the copy is required
	 * @param aServiceId The entry ID of the service by which the entries should be transferred
	 * @param aStatus Indicates operation completion
	 */
	
	IMPORT_C virtual void CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	IMPORT_C virtual void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	IMPORT_C virtual void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	IMPORT_C virtual void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	IMPORT_C void DeleteL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	IMPORT_C virtual void DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	IMPORT_C virtual void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	IMPORT_C virtual void ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	//
	//STP now defined in the base classes
	//IMPORT_C virtual void StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus);
	//
	IMPORT_C virtual TBool CommandExpected();
	
	/**
	 * Obtains progress from the send operation and copies it into the iProgress member
	 * 
	 * @return a TBufPckg<TImSmtpProgress maintained by the instantiated CMsgImOutboxSend object
	 */

	IMPORT_C virtual const TDesC8& Progress();
	
	/**
	 * Sets message entry to be sent to aEntry
	 *
	 * @param aEntry message server entry (service)
	 */
	
	IMPORT_C void SetInitialEntry(CMsvServerEntry* aEntry);

protected:

	/**
	 * Constructor
	 *
	 * @param aRegisterMtmDll	registration data for MTM dll
	 * @param aEntry			Message Server entry to operate upon
	 */

	IMPORT_C CObexSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry);
	
	/**
	 * Second phase constructor adds Obex server Mtm (this) to the active scheduler
	 */

	IMPORT_C void ConstructL();
	
	/**
	 * Cancels the obex send operation
	 */
	
	IMPORT_C virtual void DoCancel();
	IMPORT_C virtual void DoRunL();
	IMPORT_C virtual void DoComplete(TInt aError);

protected:
	CMsvServerEntry*				iEntry;	//< entry to send
	CObexServerSendOperation*		iSendOperation;	//< used by the derived classes to hold the send operation of the appropriate derived type
	TRequestStatus*					iReportStatus;


private:
	TPckgBuf<TObexMtmProgress>	iProgress;	//<progress buffer holds a local copy of iSendOperation's progress
	};

#endif  // __OBEXSERVERMTM_H__
