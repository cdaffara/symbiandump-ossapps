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
 */
#include <QtTest/QtTest>
#include <QtDebug>
#include <QTimer>
#include <QSignalSpy>
#include <xqconversions.h>
#include <bsp.h>
#include <biodb.h> 
#include <biouids.h>
#include <biocmtm.h>
#include "convergedmessageaddress.h"

#include "testunidatamodelloader.h"
#include <unidatamodelloader.h>
#include "testunidatamodelloader.ini"
#include "unidatamodelplugininterface.h"
#include <convergedmessage.h>
#include <unieditorplugininterface.h>

const TInt KTelephoneNumberMaxLength = 32;

// vCalender
_LIT(KUnixEpoch, "19700000:000000.000000");
#define KBioUidValue  0x10001262

const TUid KBioUidValueUid = {KBioUidValue};


//---------------------------------------------------------------
//Main entry point
//---------------------------------------------------------------
int main(int argc, char *argv[])
    { 
    int ret = -1;
    QCoreApplication app(argc, argv);
    QObject* tc = new TestUniDataModelLoader();
    
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

//---------------------------------------------------------------
// TestUniDataModelLoader::init
//---------------------------------------------------------------
void TestUniDataModelLoader::initTestCase()
{
	iObserver = new (ELeave) MsgObserver();

	// Create a new messaging server session..
	iMSession = CMsvSession::OpenSyncL(*iObserver);

	iMsvEntry = iMSession->GetEntryL(KMsvRootIndexEntryId);
	newEntryId = 0;
	pluginLoader = new UniDataModelLoader();

	retTimeStamp = QDateTime::currentDateTime();
	int err = iFs.Connect();
	QVERIFY(err == 0);
	iBioDb = CBIODatabase::NewL(iFs);
	iFs.SetSessionPath(_L("c:\\"));
	inbox = EFalse;
}

//---------------------------------------------------------------
// TestUniDataModelLoader::init
//---------------------------------------------------------------
void TestUniDataModelLoader::init()
{
    
}

//---------------------------------------------------------------
// TestUniDataModelLoader::testBioMessage
//---------------------------------------------------------------
void TestUniDataModelLoader::testBioMessage()
{
	inbox = ETrue;
	pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
	
	QVERIFY( pluginInterface != NULL );
	
	QString msgType = pluginInterface->messageType();
			
	qDebug() <<"msgType" << msgType;
	QCOMPARE(msgType, QString("biomessage"));
	
	qDebug() <<"UnidataModel Loader for Biomessage Created Successfully";
}

//---------------------------------------------------------------
// TestUniDataModelLoader::testMMSMessage
//---------------------------------------------------------------
void TestUniDataModelLoader::testMMSMessage()
{
	qRegisterMetaType<long int> ("long int");
	QString service = TEST_SERVICE_NAME_MMS;
	QString subject  = TEST_MSG_SUBJECT;
	qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
		
	QString sender(TEST_SENDER);
	ConvergedMessageAddress address(sender);
	ConvergedMessageAttachmentList attachmentList;  
	
	QString attachment2Path = TEST_ATTACHMENT4;
		ConvergedMessageAttachment* attachment2 = 
				new ConvergedMessageAttachment(attachment2Path, ConvergedMessageAttachment::EAttachment);
		
	QString attachmentPath = TEST_ATTACHMENT5;
	ConvergedMessageAttachment* attachment = 
			new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
			
	attachmentList.append(attachment);
	attachmentList.append(attachment2);
	ConvergedMessage msg;
	msg.setMessageType(ConvergedMessage::Mms);
	msg.setSubject(subject);
	msg.setTimeStamp(timeStamp);
	msg.addToRecipient(address);
	msg.addAttachments(attachmentList);
	msg.setPriority(ConvergedMessage::Normal);
		//Adding alias
	ConvergedMessageAddress address1;
	address1.setAlias(QString(TEST_MSG_RECIEPIENT));
	msg.addToRecipient(address1);
  
	pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
	pluginInterface->setMessageId(newEntryId);

	QVERIFY( pluginInterface != NULL ); 
	
	QString msgType = pluginInterface->messageType();
		   
	qDebug() <<"msgType" << msgType;
	QCOMPARE(msgType, QString("mms"));
	
	qDebug() <<"UnidataModel Loader for MMS Created Successfully";
} 

//---------------------------------------------------------------
// TestUniDataModelLoader::testSMSMessage
//---------------------------------------------------------------
void TestUniDataModelLoader::testSMSMessage()
{
	qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
	QString sender(TEST_SENDER);
	ConvergedMessageAddress address(sender);
	ConvergedMessage msg;
	msg.setMessageType(ConvergedMessage::Sms);
	msg.setTimeStamp(timeStamp);
	msg.addToRecipient(address);
	msg.setBodyText( TEST_MSG_BODY);
	msg.setPriority(ConvergedMessage::Normal);
	//Adding alias
	ConvergedMessageAddress address1;
	address1.setAlias(QString(TEST_MSG_RECIEPIENT));
	msg.addToRecipient(address1);
			
	pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);
	
	QVERIFY( pluginInterface != NULL );
	
	QString msgType = pluginInterface->messageType();
   
	qDebug() <<"msgType" << msgType;
	QCOMPARE(msgType, QString("sms"));
	
	qDebug() <<"UnidataModel Loader for SMS Created Successfully";
}

//---------------------------------------------------------------
// TestUniDataModelLoader::cleanup
//---------------------------------------------------------------
void TestUniDataModelLoader::cleanup()
{

}

//---------------------------------------------------------------
// TestUniDataModelLoader::cleanupTestCase
//---------------------------------------------------------------
void TestUniDataModelLoader::cleanupTestCase()
{
	delete iMsvEntry;
	iMsvEntry = NULL;
	delete iObserver;
	iObserver = NULL;
	delete iMSession;
	iMSession = NULL;
	delete pluginLoader;
}

//---------------------------------------------------------------
// TestUniDataModelLoader::HandleSessionEventL
//---------------------------------------------------------------
void MsgObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                      TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}
