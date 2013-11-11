// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__IMPREFR_H__)
#define __IMPREFR_H__

#if !defined (__IMPCMTM_H__)
#include "IMPCMTM.H"
#endif

#if !defined(__IMAPSET_H__)
#include "IMAPSET.H"
#endif

// package buffer for progress info.
/**
@internalTechnology
@released
*/
typedef TPckgBuf<TImap4CompoundProgress> TImap4ProgressBuf;


/**
@internalComponent
@released
*/
enum EImapOpFlags 
	{
	KImapOpFlagNone                         = 0x0001,
   KImapOpFlagStartBatch                   = 0x0002,
   KImapOpFlagCancelBackgroundSynchronise  = 0x0004,
   KImapOpFlagConnect                      = 0x0008,
   KImapOpFlagGenINBOX                     = 0x0010,
   KImapOpFlagFullSync                     = 0x0020,
	KImapOpFlagDisconnect                   = 0x0040,
   KImapOpFlagMarkAllRead                  = 0x0080,
   KImapOpFlagMarkAllUnRead                = 0x0100,
   KImapOpFlagEndBatch                     = 0x0200
	};

const TInt KImapGeneralFirstCommand = 0x1000;

/**
@internalComponent
@released
*/
enum EGeneralOperations
    {
    KImapGeneralConnecting = KImapGeneralFirstCommand,
    KImapGeneralGettingNewMail,
    KImapGeneralMarkingFolders,
    KImapGeneralDisconnecting
    };

// create an active object to send the message
class CImap4ClientMtm;

class CImap4RefreshMBox : public CMsvOperation
/**
@internalComponent
@released
*/
	{
public: // Constructors/destructors
	IMPORT_C static CImap4RefreshMBox* NewLC(CImap4ClientMtm* aBaseMtm, TMsvId aServiceId,TRequestStatus& aObserverRequestStatus, TInt aOperations);
	IMPORT_C static CImap4RefreshMBox* NewL(CImap4ClientMtm* aBaseMtm, TMsvId aServiceId,TRequestStatus& aObserverRequestStatus, TInt aOperations);
	IMPORT_C ~CImap4RefreshMBox();
public: // Provide info
	IMPORT_C const TDesC8& ProgressL();
public: // Do work
    IMPORT_C void StartL();
public: //Derived
	void RunL();
protected:
	void DoCancel();
	void ConstructL();
protected:
   void DoStartBatchL();
   void DoEndBatchL();
   void DoConnectL();
	void DoDisconnectL();
   void DoGenerateInboxL();
   void DoFolderSyncL();
	void DoCancelBackgroundOperationL();
	void DoSubscriptionSyncL();
	void DoFullSyncL();
   void DoMarkAllAsReadL();
private:
   void NewOp();
   void WaitForOp(CMsvOperation* aNewOperation);
   CImap4RefreshMBox(CImap4ClientMtm* aBaseMtm, TMsvId aServiceId,TRequestStatus& aObserverRequestStatus, TInt aOperations);
protected:
	  // Data members defined by this class
	CImap4ClientMtm* iBaseMtm;
	TMsvId iServiceId;
	TInt iOperations;
	CMsvEntry* iEntry;
	CMsvEntrySelection* iSelection;
	TImap4CompoundProgress iProgress;
	TImap4ProgressBuf iProgressBuf;
   TInt iCurrentOperation;
   TInt iErrorCode;
	CMsvOperation* iCurrentOp;
	};

#endif
