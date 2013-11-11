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
// t_unit_imapsession_server.cpp
// 
//

#include <test/ctefunitserver.h>
#include "t_imapmailstoreheaders.h"
#include "t_imapmailstorebodydata8.h"
#include "t_imapmailstorebodydata16.h"
#include "t_imapmailstoreattachments.h"
#include "t_imapmailstoreattachmentinfo.h"
#include "t_imapmailstorecancelingrequests.h"
#include "t_imapmailcancelstoreattachments.h"
#include "t_imapmailstoreplainbodydata.h"

		
_LIT(KServerName, "t_unit_imapmailstore_server");


GLDEF_C const TTestName ServerName()
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE;
 	ADD_TEST_SUITE(CTestImapMailStoreHeaders);
 	ADD_TEST_SUITE(CTestImapMailStoreBodyData8);
 	ADD_TEST_SUITE(CTestImapMailStoreBodyData16);
 	ADD_TEST_SUITE(CTestImapMailStoreAttachments);
 	ADD_TEST_SUITE(CTestImapMailStoreCancelingRequests);
 	ADD_TEST_SUITE(CTestImapMailStoreAttachmentInfo);
 	ADD_TEST_SUITE(CTestImapMailCancelStoreAttachments);
	ADD_TEST_SUITE(CTestImapMailStorePlainBodyData);
 	END_SUITE;
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep = NULL;
	/*
	if(aStepName == KSampleStep1)
		{
		testStep = new CSampleStep1(aServer);
		}
	*/
	return testStep;
	}
