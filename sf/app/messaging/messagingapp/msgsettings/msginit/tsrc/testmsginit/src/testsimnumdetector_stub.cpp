/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     A class which takes care of reading the possible new
 *     service centres from SIM and adds them to pda-side Sms Settings.
 *
 */

// INCLUDE FILES

#include "coutboxobserver.h"
#include "testsimnumdetector_stub.h"
#include "startupmonitor.h"
#include "debugtraces.h"



// ================= MEMBER FUNCTIONS =======================

EXPORT_C CTestMsgSimOperation* CTestMsgSimOperation::NewL()
    {
    QDEBUG_WRITE("CTestMsgSimOperation::NewL enter")

    CTestMsgSimOperation* self = new (ELeave) CTestMsgSimOperation();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    QDEBUG_WRITE("CTestMsgSimOperation::NewL End")
    return self;
    }


CTestMsgSimOperation::CTestMsgSimOperation()
    {
    }

void CTestMsgSimOperation::ConstructL()
    {   
    QDEBUG_WRITE("CTestMsgSimOperation::ConstructL enter")

    // initialise
    iMsvSession = CMsvSession::OpenSyncL(*this);
   
    QDEBUG_WRITE("CTestMsgSimOperation::ConstructL exit")
    }

CTestMsgSimOperation::~CTestMsgSimOperation()
    {
    QDEBUG_WRITE("CTestMsgSimOperation::~CTestMsgSimOperation Enter") 
    if(NULL != iMsvSession)
    	{
		delete iMsvSession;
		iMsvSession = NULL;
    	}
    
    QDEBUG_WRITE("CTestMsgSimOperation::~CTestMsgSimOperation Exit") 
    }

void CTestMsgSimOperation::HandleSessionEventL(TMsvSessionEvent aEvent,
        TAny* /*aArg1*/, TAny* /*aArg2*/,
        TAny* /*aArg3*/)
    {
	QDEBUG_WRITE("CTestMsgSimOperation::HandleSessionEventL Enter") 

	QDEBUG_WRITE("CTestMsgSimOperation::HandleSessionEventL Exit") 
    }

void CTestMsgSimOperation::HandleStartupReadyL()
    {
    QDEBUG_WRITE("CTestMsgSimOperation::HandleStartupReadyL Enter") 
    // Boot ready, start the real SimOperation

    QDEBUG_WRITE("CTestMsgSimOperation::HandleStartupReadyL Exit") 
    }

//  End of File  


