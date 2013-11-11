/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __TESTCALINTERIMAPISTARTUPSTEP_H__
#define __TESTCALINTERIMAPISTARTUPSTEP_H__

#include "TestCalInterimApiSuiteServer.h"
#include "TestCalInterimApiSuiteStepBase.h"

_LIT(KTestStartUpExeStep,"TestStartUpExeStep");
_LIT(KStartupActionTag,  "StartupAction");
_LIT(KLaunch,            "LaunchProcess");
_LIT(KTerminate,         "TerminateProcess");
_LIT(KCheck,             "CheckProcess");
_LIT(KStartupnameTag,    "StartupExeName");
_LIT(KExitTypeTag,       "StartUpExitType");
_LIT(KExitReasonTag,     "StartUpExitReason");
_LIT(KModeTag,           "Mode");


class CTestStartUpExeStep : public CTestCalInterimApiSuiteStepBase 
    {
public:        
    CTestStartUpExeStep(CTestCalInterimApiSuite& aTestServerPtr);
    ~CTestStartUpExeStep();
	TVerdict doTestStepPreambleL();
    TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
	
private:
    TPtrC iStartupAction;
    TInt iExitReasonFromIni;
    TPtrC iAgnServerExeName;  
    TPtrC iServerMode;
    TExitType iExitTypeFromIni;
    CTestCalInterimApiSuite* iTestServerPtr;      
    };
    
#endif