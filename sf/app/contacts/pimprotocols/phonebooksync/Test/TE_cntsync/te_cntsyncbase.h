// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_CNTSYNCBASE_H_
#define _TE_CNTSYNCBASE_H_


#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

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

#include "te_cntsyncutils.h"

using namespace conn;


const TInt KMaxICCSlots = 100;
const TInt KMaxICCSlotsTest6 = 20;
const TInt KNumOfUsedSlotstest6 = 5;
const TInt KMaxPhBkNameSize = 214;
const TInt KTest24ICCSlotsUsed = (22*5); 
const TInt KTest25ICCSlotsUsed = (3*5);
const TInt KTest26ICCSlotsUsed = 3;
const TInt KTest29ICCSlotsUsed = (1*5);

const TInt KTest1ICCSlotsADN = 5;
const TInt KTest1ICCSlotsSDN = 5;
const TInt KTest1ICCSlotsLND = 5;
const TInt KTest1ICCSlotsUsim = 5;
const TInt KTest1ICCSlotsFDN = 5;
const TInt KTest1MaxSlots = 25;
const TInt KTest2ICCSlots = 25;
const TInt KTest2ICCSlotsADN = 5;
const TInt KTest2ICCSlotsSDN = 5;
const TInt KTest2ICCSlotsLND = 5;
const TInt KTest2ICCSlotsUsim = 5;
const TInt KTest2ICCSlotsFDN = 5;
const TInt KTest6ICCSlotsADN = 5;
const TInt KTest6ICCSlotsSDN = 5;
const TInt KTest6ICCSlotsLND = 5;
const TInt KTest6ICCSlotsUsim = 5;
const TInt KTest6ICCSlotsFDN = 5;
const TInt KTest9ICCSlotsADN = 18;
const TInt KTest9ICCSlotsSDN = 18;
const TInt KTest9ICCSlotsLND = 18;
const TInt KTest9ICCSlotsUsim = 18;
const TInt KTest9ICCSlotsFDN = 18;
const TInt KTest13ICCSlots = 255;
const TInt KTest13ICCSlotsADN = 150;
const TInt KTest13ICCSlotsSDN = 150;
const TInt KTest13ICCSlotsLND = 150;
const TInt KTest13ICCSlotsUsim = 20;
const TInt KTest13ICCSlotsFDN = 150;


_LIT(KNameFormat,"%c%03d");
_LIT(KNumberFormat,"%03d");
_LIT(KEmailFormat,"E%03d@symbianfoundation");
_LIT(KModifiedName,"ABC");
_LIT(KTestName,"Example");
_LIT(KTestNumber,"020 79460006");
_LIT(KTestEMail,"example@symbianfoundation");

_LIT(KTestPersonWithInternationalNumber, "PersonWithInternationalNumber");
_LIT(KTestPersonWithInternationalNumber2, "PersonWithInternationalNumber2");
_LIT(KTestInternationalNumber1, "+447700900649");
_LIT(KTestInternationalNumber2, "+447700900650");

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


class CCntSyncTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	void DoSyncL();
	void DoSyncFailL();
	void WaitForSyncToCompleteL();
	void SetSimTsyTestNumberL(TInt aTestNumber);
	void SignalSimTsyToReduceTimersL();
	CContactDatabase* OpenOrCreateContactDatabaseL();
	void ConfigurePhbkSyncToIdleL();
	void ConfigurePhbkSyncToFullL();
	void CheckSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode);
	void SetSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode);
	void CheckCacheStateL(RPhoneBookSession::TSyncState aState);

protected:
	RPhoneBookSession  iSession;
	CContactDatabase*  iDb;
	CActiveScheduler*  iScheduler;
	CSBEClient*  iSecureBackupEngine;
	};

#endif
