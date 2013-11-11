/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_DELETE_AGENDA_SERVERS_H__
#define __TEST_CALINTERIMAPI_DELETE_AGENDA_SERVERS_H__

#include "TestCalInterimApiSuiteStepBase.h"

/**
 * Teststep that checks to see if the agenda server has been started and 
 * closes it if it has. 
 */
class CTestCalInterimApiDeleteAgendaServers: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDeleteAgendaServers();
	~CTestCalInterimApiDeleteAgendaServers();
	
	TVerdict 		doTestStepPreambleL();
	TVerdict 		doTestStepL();

private:	

public:
	};

_LIT(KTestCalInterimApiDeleteAgendaServers,"TestCalInterimApiDeleteAgendaServers");

#endif // __TEST_CALINTERIMAPI_DELETE_AGENDA_SERVERS_H__
