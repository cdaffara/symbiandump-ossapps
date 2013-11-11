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
// Header file for the TS_SYNC.CPP - test programme for Phonebook Synchroniser
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef __TE_SYNC_H__
#define __TE_SYNC_H__

#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include <e32keys.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include "phbksync.h"
#include "TE_SyncBase.h"


class CTestOpen : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestOpen();
	};

class CTestDoSynchronisation : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestDoSynchronisation();
	};

class CTestWriteContactToICCTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCTest();
	};

class CTestWriteContactToICCOverwriteTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCOverwriteTest();
	};

class CTestWriteContactToICCNoSlotTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCNoSlotTest();
	};

class CTestWriteContactToICCInvalidSlotTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCInvalidSlotTest();
	};

class CTestWriteContactToICCBlankNumTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCBlankNumTest();
	};

class CTestWriteContactToICCBlankNameTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCBlankNameTest();
	};

class CTestWriteContactToICCExceedMaxNameTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCExceedMaxNameTest();
	};

class CTestWriteContactToICCExceedMaxNumTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCExceedMaxNumTest();
	};

class CTestWriteContactToICCMaxNumTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCMaxNumTest();
	};

class CTestWriteContactToICCMaxNameTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCMaxNameTest();
	};

class CTestWriteContactToICCNumLettersTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCNumLettersTest();
	};

class CTestWriteContactToICCObsecureTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteContactToICCObsecureTest();
	};

class CTestValidateContact : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestValidateContact();
	};

class CTestValidateContactAccessLockedTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestValidateContactAccessLockedTest();
	};

class CTestValidateContactInvalidUIDTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestValidateContactInvalidUIDTest();
	};

class CTestUpdateLookUpTableFailure : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestUpdateLookUpTableFailure();
	};

class CTestUpdateLookUpTableSuccess : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestUpdateLookUpTableSuccess();
	};

class CTestGetSyncMode : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestGetSyncMode();
	};

class CTestSetSyncMode : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestSetSyncMode();
	};

class CTestDeleteContactFromICC : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestDeleteContactFromICC();
	};

class CTestDeleteContactInvalidIdFromICC : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestDeleteContactInvalidIdFromICC();
	};

class CTestGetContactFormat : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestGetContactFormat();
	};

class CTestGetPhoneBookCacheState : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestGetPhoneBookCacheState();
	};

class CTestVersionTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestVersionTest();
	};

class CTestGetLastSyncError : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestGetLastSyncError();
	};

class CTestNotifyCacheStateChange : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestNotifyCacheStateChange();
	};

class CTestGetPhoneBookId : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestGetPhoneBookId();
	};

class CTestReset : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestReset();
	};

class CTestCancelWriteRequest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestCancelWriteRequest();
	};

class CTestCancelDeleteRequest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestCancelDeleteRequest();
	};

class CTestCancelDoSyncRequest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestCancelDoSyncRequest();
	};


class CTestSlotHandlingTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestSlotHandlingTest();
	};

class CTestWriteToSlotTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestWriteToSlotTest();
	};

class CTestConnectionTest : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestConnectionTest();
	};

class CTestFailedPhoneStartup : public CSyncTestStep
	{
	public:
		virtual enum TVerdict doTestStepL();
		CTestFailedPhoneStartup();
	};

#endif
