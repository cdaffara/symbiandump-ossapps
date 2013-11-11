/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDES
#include "testmsginit.h"
#include "debugtraces.h"
#include "msgsimnumberdetector.h"
#include "coutboxobserver.h"
#include "startupmonitor.h"
#include "coutboxsendoperation.h"
#include "cmobilesignalstrengthhandler.h"
#include "testoutboxobserver_stub.h"
#include "testoutboxsender_stub.h"
#include "testsimnumdetector_stub.h"

#include <csmsaccount.h>
#include <e32property.h>
#include <startupdomainpskeys.h>


void TestMsgInit::initTestCase()
	{

	}

void TestMsgInit::init()
	{
	qDebug("TestMsgInit::init enter");
	
	iTestMsgSimOperation = CTestMsgSimOperation::NewL();
	QVERIFY2(iTestMsgSimOperation != NULL, "CTestMsgSimOperation init failed.");
	
	qDebug("TestMsgInit::init exit");
	}

void TestMsgInit::validateMobileSignalStrength()
	{
	qDebug("TestMsgInit::validateMobileSignalStrength enter");
	
	CTestOutboxObserver* mOutBoxObserver = CTestOutboxObserver::NewL();
	QVERIFY2(mOutBoxObserver != NULL, "CTestOutboxObserver init failed.");
	CleanupStack::PushL(mOutBoxObserver);
	
	iSignalStrengthHandler = CMobileSignalStrengthHandler::NewL();
	QVERIFY2(iSignalStrengthHandler != NULL, "CMobileSignalStrengthHandler init failed.");
	
	if (iSignalStrengthHandler->ObservingSupported()) 
	{
		iSignalStrengthHandler->SetSignalStrengthObserverL(mOutBoxObserver);
		iSignalStrengthHandler->StartObservingL();
	}
	
	CleanupStack::PopAndDestroy(mOutBoxObserver);
	//QTest::qWait(1000);
	if(NULL !=iSignalStrengthHandler)
		{
		delete iSignalStrengthHandler;
		iSignalStrengthHandler = NULL;
		}
	qDebug("TestMsgInit::validateMobileSignalStrength exit");
	}

void TestMsgInit::validateOutboxSenderOperation()
	{
	qDebug("TestMsgInit::validateOutboxSenderOperation enter");
	
	CTestOutboxSender* mTestOutboxSender = CTestOutboxSender::NewL(*(iTestMsgSimOperation->iMsvSession));
	QVERIFY2(mTestOutboxSender != NULL, "CTestOutboxSender init failed.");
	CleanupStack::PushL(mTestOutboxSender);
	
	CMsvSingleOpWatcher* msingleOpWatcher = CMsvSingleOpWatcher::NewL(*mTestOutboxSender);
	QVERIFY2(msingleOpWatcher != NULL, "CMsvSingleOpWatcher init failed.");
	CleanupStack::PushL(msingleOpWatcher);

	iMsvOperation = COutboxSendOperation::NewL(*(iTestMsgSimOperation->iMsvSession), msingleOpWatcher->iStatus);
	QVERIFY2(iMsvOperation != NULL, "COutboxSendOperation init failed.");
	
	//QTest::qWait(1000);
	if(NULL !=iMsvOperation)
		{
		delete iMsvOperation;
		iMsvOperation = NULL;
		}
	CleanupStack::PopAndDestroy(2); //mTestOutboxSender, msingleOpWatcher
	
	qDebug("TestMsgInit::validateOutboxSenderOperation exit");
	}

void TestMsgInit::validateOutboxObserver()
	{
	qDebug("TestMsgInit::validateOutboxObserver enter");
	
	// Observes the OUTBOX for any offline messages...
	iOutBoxObserver = COutboxObserver::NewL();
	QVERIFY2(iOutBoxObserver != NULL, "COutboxObserver init failed.");
	    
	// Start the Auto-send AO, to handle offline SMS messages
	iOutBoxObserver->HandleMsvSessionReadyL(*(iTestMsgSimOperation->iMsvSession));
	//QTest::qWait(1000);
	
	if(NULL !=iOutBoxObserver)
		{
		delete iOutBoxObserver;
		iOutBoxObserver = NULL;
		}
	qDebug("TestMsgInit::validateOutboxObserver exit");
	}

 void TestMsgInit::validateSimNumDetector()
	{
	qDebug("TestMsgInit::validateSimNumDetector enter");
	 
	TInt startupState = 0;
	TInt modifiedState = ESwStateNormalRfOn;
	TInt status = 0;
	
	//Load current settings
	CSmsSettings* smsSettings = CSmsSettings::NewLC();
	CSmsAccount* smsAccount = CSmsAccount::NewLC();
	smsAccount->LoadSettingsL(*smsSettings);

	// Remove all old SMSC's configured
	TInt numSCAddresses = smsSettings->ServiceCenterCount();

	qDebug() << "numSCAddresses:" << numSCAddresses;

	for (TInt j = numSCAddresses; j > 0; j--)
		{
		smsSettings->RemoveServiceCenter(j - 1);
		}

	// Save settings
	TInt maxTries(5);
	TBool done(EFalse);
	while (maxTries && !done)
		{
		TRAPD( err, smsAccount->SaveSettingsL( *smsSettings ) );
		if (err == KErrNone)
			{
		qDebug("CMsgSimOperation::CreateDefaultSettingsL settings saved");

			done = ETrue;
			}
		else if (err == KErrLocked)
			{
		qDebug("CMsgSimOperation::CreateDefaultSettingsL KErrLocked");

			// Wait a while and retry.
			User::After(100000); // 0.1 seconds
			maxTries--;
			}
		else
			{
			User::Leave(err);
			}
		}

	QVERIFY2((iSimHandler = new MsgSimNumDetector()) != NULL, "Message settings engine init failed.");
	status = RProperty::Get( KPSUidStartup, KPSGlobalSystemState, startupState );
	if(ESwStateNormalRfOn != startupState)
		{
		RProperty::Set( KPSUidStartup, KPSGlobalSystemState, modifiedState );
		}
	qDebug() << "startupState:" << startupState;
	qDebug() << "modifiedState:" << modifiedState;
	RProperty::Set(KPSUidStartup, KPSGlobalSystemState, startupState);
	
	qDebug() << "startupState:" << startupState;
	qDebug() << "modifiedState:" << modifiedState;
	CleanupStack::PopAndDestroy(2); // smsAccount, smsSettings
	//QTest::qWait(1000);
	if(NULL !=iSimHandler)
		{
		delete iSimHandler;
		iSimHandler = NULL;
		}
	
	qDebug("TestMsgInit::validateSimNumDetector Exit");
	}
 
void TestMsgInit::cleanup()
	{
	if(NULL !=iTestMsgSimOperation)
		{
		delete iTestMsgSimOperation;
		iTestMsgSimOperation = NULL;
		}	
	}

void TestMsgInit::cleanupTestCase()
	{
	
	}

//main entry point
int main(int argc, char *argv[])
    { 
    int ret = -1;
    QCoreApplication app(argc, argv);
    QObject* tc = new TestMsgInit();
    
#ifdef __WINSCW__	
		char *new_argv[3]; 
		QString str = "C:\\data\\" + QFileInfo(QCoreApplication::applicationFilePath()).baseName() + ".log";	
		QByteArray bytes = str.toAscii();	
		char arg1[] = "-o";	
		new_argv[0] = argv[0];	
		new_argv[1] = arg1;	
		new_argv[2] = bytes.data();	
		ret = QTest::qExec(tc, 3, new_argv);	
#else	
		ret = QTest::qExec(tc, argc, argv);	
#endif
	delete tc;
    return ret;
    }

