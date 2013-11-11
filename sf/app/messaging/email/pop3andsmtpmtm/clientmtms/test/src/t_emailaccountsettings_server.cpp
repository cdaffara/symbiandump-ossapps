// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <test/ctefunitserver.h>

#include "ctestimapsyncdownloadrules.h"
#include "ctestimapsyncdownloadrulessettings.h"
#include "ctestimaptransportbuffersizes.h"
#include "ctestsmtpmobilitysettings.h"
#include "ctestsmtpmobilityaccounts.h"
#include "ctesttlsssldomainsetting.h"

_LIT(KServerName, "t_emailaccountsettings_server");

GLDEF_C const TTestName ServerName()
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE;
	ADD_TEST_SUITE(CTestImapSyncDownloadRules);
	ADD_TEST_SUITE(CTestImapSyncDownloadRulesSettings);
	ADD_TEST_SUITE(CTestImapTransportBufferSizes);
	ADD_TEST_SUITE(CTestSmtpMobilitySettings);
	ADD_TEST_SUITE(CTestSmtpMobilityAccounts);
	ADD_TEST_SUITE(CTestTlsSslDomainSetting);
 	END_SUITE;
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	return NULL;
	}
