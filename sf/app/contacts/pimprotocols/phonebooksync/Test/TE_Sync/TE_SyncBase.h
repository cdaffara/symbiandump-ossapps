// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent 
*/


#ifndef _TE_SYNCSUITE_H_
#define _TE_SYNCSUITE_H_


#include "f32file.h"
#include "c32comm.h"

#include <e32base.h>
#include <e32test.h>
#include <phbksync.h>
#include <cntdb.h>
#include <cntdef.h>
#include <etelmm.h>
#include <simtsy.h>
#include <connect/sbeclient.h>
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

using namespace conn;


_LIT(KNameFormat,"AAAAAAAAAAAAAAAAA%03d");
_LIT(KNumberFormat,"00000000000000000%03d");
_LIT(KModifiedName,"ABC");
_LIT(KTestName,"Name");
_LIT(KTestNumber,"02079460099");


//
// Test macros
//
_LIT(KTestCheckFailText, "Failed: Got %d while expecting %d.");
_LIT(KTestCheckConditionFailText, "Failed: Condition was false.");
_LIT(KTestCheckStringFailText, "Failed: Got \"%S\" while expecting \"%S\".");

#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckFailText, a, b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKSTRL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckStringFailText, &(a), &(b)); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckFailText, a, b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckStringFailText, &(a), &(b)); \
        SetTestStepResult(EFail); \
        } \
    }


class CSyncTestStep : public CTestStep
{
public:
	class TContactICCEntry
	/**
	 * This class is used to store the contacts phonebook data (supplied in CContactICCEntry)
	 * in the Phonebook Server internal format.
	 */
		{
	public:
		TContactICCEntry();
		void Reset();
	public:
		enum 
		{
		KMaxPhBkNameSize=241
		};

		TBuf16<KMaxPhBkNameSize> iName;
		TBuf16<RMobilePhone::KMaxMobileTelNumberSize> iNumber;
		TInt iSlotNum;
		RMobilePhone::TMobileTON iTON;
		TContactItemId iContactUID;
		};

	typedef TPckg<TContactICCEntry> TContactICCEntryPckg;

public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	void WriteContactToICCL(TContactICCEntry& iccEntry, TRequestStatus& aStatus);
	void AddTextFieldToIccContactL(TStorageType aType, TFieldType aFieldType, TUid aMapping, TDesC& aField, CContactICCEntry* aIccEntry);
	TContactItemId GetValidUIDFromContactsDbL();

	void ConfigurePhbkSyncToIdleL();
	void ConfigurePhbkSyncToFullL();

public:
	RPhoneBookSession  iSession;

private:
	CActiveScheduler*  iScheduler;
	CSBEClient*  iSecureBackupEngine;
};


#endif
