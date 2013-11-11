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

#include "ctestimapsession.h"
#include "ctestimapcommand.h"
#include "ctestimapcancelandflush.h"
#include "ctestimapsessioncreatesequenceset.h"
#include "ctestimapidle.h"
#include "ctestimapservereventwait.h"
#include "ctestimaplogout.h"
#include "ctestimapselect.h"
#include "ctestimapexpunge.h"
#include "ctestimapcreate.h"
#include "ctestimapdelete.h"
#include "ctestimaprename.h"
#include "ctestimapsearch.h"
#include "ctestimapstore.h"
#include "ctestimapcopy.h"
#include "ctestimapnoop.h"
#include "ctestimaplist.h"
#include "ctestimaplsub.h"
#include "ctestimapfetchflags.h"
#include "ctestimapatombuilder.h"
#include "ctestimapbodystructurebuilder.h"
#include "ctestimapfetchbody.h"
#include "ctestimapfetchsinglebodystructure.h"	

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
#include "ctestimapauthenticate.h"
#endif
#include <test/ctefunitserver.h>

_LIT(KServerName, "t_unit_imapsession_server");


GLDEF_C const TTestName ServerName()
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE;
	ADD_TEST_SUITE(CTestImapSession);
 	ADD_TEST_SUITE(CTestImapCommand);
 	ADD_TEST_SUITE(CTestImapCancelAndFlush);
 	ADD_TEST_SUITE(CTestImapSessionCreateSequenceSet);
 	ADD_TEST_SUITE(CTestImapIdle);
 	ADD_TEST_SUITE(CTestImapServerEventWait);
 	ADD_TEST_SUITE(CTestImapLogout);
 	ADD_TEST_SUITE(CTestImapSelect);
	ADD_TEST_SUITE(CTestImapExpunge);
	ADD_TEST_SUITE(CTestImapCreate);
	ADD_TEST_SUITE(CTestImapDelete);
	ADD_TEST_SUITE(CTestImapRename);
	ADD_TEST_SUITE(CTestImapSearch);
 	ADD_TEST_SUITE(CTestImapStore);
 	ADD_TEST_SUITE(CTestImapCopy);
 	ADD_TEST_SUITE(CTestImapNoop);
 	ADD_TEST_SUITE(CTestImapList);
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	ADD_TEST_SUITE(CTestImapAuthenticate);		
#endif
	ADD_TEST_SUITE(CTestImapLsub);
 	ADD_TEST_SUITE(CTestImapFetchFlags);
 	ADD_TEST_SUITE(CTestImapAtomBuilder);
 	ADD_TEST_SUITE(CTestImapBodyStructureBuilder);
	ADD_TEST_SUITE(CTestImapFetchBody);
	ADD_TEST_SUITE(CTestImapFetchSingleBodyStructure);
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
