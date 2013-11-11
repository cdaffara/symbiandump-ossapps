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

#include "ctestimapservermtmbusy.h"
#include "ctestimapservermtmcancelbackgsync.h"
#include "ctestimapservermtmcancelofflineop.h"
#include "ctestimapservermtmconnect.h"
#include "ctestimapservermtmconnectandsync.h"
#include "ctestimapservermtmcopyfromlocal.h"
#include "ctestimapservermtmcopytolocal.h"
#include "ctestimapservermtmcopywithinservice.h"
#include "ctestimapservermtmchange.h"
#include "ctestimapservermtmcreate.h"
#include "ctestimapservermtmdeleteall.h"
#include "ctestimapservermtmdisconnect.h"
#include "ctestimapservermtmisconnected.h"
#include "ctestimapservermtmfolderfullsync.h"
#include "ctestimapservermtmfullsync.h"
#include "ctestimapservermtminboxnewsync.h"
#include "ctestimapservermtmlocalunsubscribe.h"
#include "ctestimapservermtmlocalsubscribe.h"
#include "ctestimapservermtmmovefromlocal.h"
#include "ctestimapservermtmmovetolocal.h"
#include "ctestimapservermtmmovewithinservice.h"
#include "ctestimapservermtmpopulate.h"
#include "ctestimapservermtmselect.h"
#include "ctestimapservermtmsynchronise.h"
#include "ctestimapservermtmsynctree.h"
#include "ctestimapservermtmrenamefolder.h"
#include "ctestimapservermtmundeleteall.h"
#include "ctestimapservermtmunsupportedcmds.h"
#include "ctestimapservermtmwaitforbackg.h"

#include <test/ctefunitserver.h>

_LIT( KServerName, "t_unit_imapservermtm_server" );

GLDEF_C const TTestName ServerName( )
	{
	TTestName serverName( KServerName );
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL( )
	{
	User::SetJustInTime( ETrue );

	START_SUITE;
	ADD_TEST_SUITE( CTestImapServerMtmBusy );
	ADD_TEST_SUITE( CTestImapServerMtmCancelBackgSync );
	ADD_TEST_SUITE( CTestImapServerMtmCancelOfflineOp );
 	ADD_TEST_SUITE( CTestImapServerMtmConnect );
 	ADD_TEST_SUITE( CTestImapServerMtmConnectAndSync );
 	ADD_TEST_SUITE( CTestImapServerMtmCopyFromLocal );
 	ADD_TEST_SUITE( CTestImapServerMtmCopyToLocal );
 	ADD_TEST_SUITE( CTestImapServerMtmCopyWithinService );
 	ADD_TEST_SUITE( CTestImapServerMtmChange );
 	ADD_TEST_SUITE( CTestImapServerMtmCreate );
 	ADD_TEST_SUITE( CTestImapServerMtmDeleteAll );
 	ADD_TEST_SUITE( CTestImapServerMtmDisconnect );
 	ADD_TEST_SUITE( CTestImapServerMtmIsConnected );
 	ADD_TEST_SUITE( CTestImapServerMtmFolderFullSync );
 	ADD_TEST_SUITE( CTestImapServerMtmFullSync );
 	ADD_TEST_SUITE( CTestImapServerMtmInboxNewSync );
 	ADD_TEST_SUITE( CTestImapServerMtmPopulate );
 	ADD_TEST_SUITE( CTestImapServerMtmSelect );
 	ADD_TEST_SUITE( CTestImapServerMtmSynchronise );
 	ADD_TEST_SUITE( CTestImapServerMtmSyncTree );
 	ADD_TEST_SUITE( CTestImapServerMtmLocalSubscribe );
 	ADD_TEST_SUITE( CTestImapServerMtmLocalUnSubscribe );
 	ADD_TEST_SUITE( CTestImapServerMtmMoveFromLocal );
 	ADD_TEST_SUITE( CTestImapServerMtmMoveToLocal );
 	ADD_TEST_SUITE( CTestImapServerMtmMoveWithinService );
 	ADD_TEST_SUITE( CTestImapServerMtmRenameFolder );
 	ADD_TEST_SUITE( CTestImapServerMtmUndeleteAll );
 	ADD_TEST_SUITE( CTestImapServerMtmUnsupportedCmds );
 	ADD_TEST_SUITE( CTestImapServerMtmWaitForBackg );
 	END_SUITE;
	}

GLDEF_C CTestStep* CreateTEFTestStep( const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/ )
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
