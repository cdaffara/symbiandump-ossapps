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
 * Description: : Main test class definition for MMS Plugin
 */

#include <QtTest/QtTest>
#include <QTimer>
#include <QSignalSpy>
#include "debugtraces.h"
#include "testmmsplugin.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "unieditormmsplugin.h"
#include "mmstestbed.h"
#include "testmmsplugin.ini"

//---------------------------------------------------------------
// TestMmsPlugin::initTestCase
//---------------------------------------------------------------
void TestMmsPlugin::initTestCase()
{
	//Instantiate UniEditorMmsPlugin and verify if it is correctly Instantited. 
	msgPlugin = new UniEditorMmsPlugin();
	QVERIFY(msgPlugin != NULL);
	
	//Verify if MMS service is Validated. 
    QCOMPARE(msgPlugin->validateService(), 1);
    QCOMPARE(msgPlugin->isServiceValid(), 1);
    msgPlugin->setEncodingSettings(ETrue, ESmsEncodingNone, -1);
    QString str("Hello");
    TInt numOfRemainingChars = 1;
    TInt numOfPDUs = 1;
    TBool unicodeMode = ETrue;
    TSmsEncoding smsEncoding = ESmsEncodingNone;
    QVERIFY(msgPlugin->getNumPDUs(str, numOfRemainingChars, numOfPDUs, unicodeMode, smsEncoding) == true);
    
    //Verify MmsTestBed Instance. 
    mmstestbed = new MmsTestBed;
	QVERIFY(mmstestbed != NULL);
	
	//register user defined object to meta system.
	qRegisterMetaType<long int> ("long int");
	
	//set up signalspy to listen to signals emitted by mmstestbed
	spy_draft = new QSignalSpy(mmstestbed, SIGNAL(entryCreatedInDraft(long int)));
	spy_outbox = new QSignalSpy(mmstestbed, SIGNAL(entryMovedToOutbox(long int)));
	spy_sent  = new QSignalSpy(mmstestbed, SIGNAL(entryMovedToSent(long int)));
	spy_inbox = new QSignalSpy(mmstestbed, SIGNAL(entryCreatedInInbox(long int)));
}

//---------------------------------------------------------------
// TestMmsPlugin::init
//---------------------------------------------------------------
void TestMmsPlugin::init()
{
}

//---------------------------------------------------------------
// TestMmsPlugin::createMMSWithTextAttachment
//---------------------------------------------------------------
void TestMmsPlugin::createMMSWithTextAttachment()
{
    //Create a Converged Message instance. 
    QString subject  = TEST_MSG_SUBJECT;
	qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
	
	QString sender(TEST_SENDER);
	ConvergedMessageAddress address(sender);
	ConvergedMessageAttachmentList attachmentList;    

	//Add a text attachment to attachment list. 
	QString attachmentPath = TEST_ATTACHMENT5;
	ConvergedMessageAttachment* attachment = 
		new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
	
	attachmentList.append(attachment);
	
	//Instantiate a Converged Message object and set service a MMS
	ConvergedMessage msg;
	msg.setMessageType(ConvergedMessage::Mms);
	
	//Set Subject
	msg.setSubject(subject);
	
	msg.setTimeStamp(timeStamp);
	
	//Set recipient. 
	msg.addToRecipient(address);
	
	//Add attachments' list
	msg.addAttachments(attachmentList);
	msg.setPriority(ConvergedMessage::Normal);
	
	//Adding CC Address
	QString ccAddress(TEST_CC);
	ConvergedMessageAddress ccAdd(ccAddress);
	msg.addCcRecipient(ccAdd);
	
	//Adding BCC Address
	QString bccAddress(TEST_BCC);
	ConvergedMessageAddress bccAdd(bccAddress);
	msg.addBccRecipient(bccAdd);
	
	//Adding From Address
	QString recipientAddress(TEST_SENDER);
	ConvergedMessageAddress recipientAdd(recipientAddress);
	msg.addFromRecipient(recipientAdd);
	
	//start send and prepare to test send-success or fail
	mmstestbed->cleanAll();
	mmstestbed->setConnectionLocal(true);
	
	//Get a valid MMS message ID and verify that it is valid.
	mmsMsgId = msgPlugin->convertTo(&msg);
	QVERIFY(mmsMsgId != -1);
}

//---------------------------------------------------------------
// TestMmsPlugin::testMMSWithTextAttachment
//---------------------------------------------------------------
void TestMmsPlugin::testMMSWithTextAttachment()
{
	long int mmsDraftMsgId;
	
	//check if draft-folder signal was received...this means message was created in draft 
	if( 1 <= spy_draft->count())
	{
		//compare the msgid and verify with the ID given by MMS plugin 
		void * temp = const_cast<void*>(spy_draft->at(0).at(0).data());
		mmsDraftMsgId = *reinterpret_cast< long int(*)>(temp);
		QVERIFY(mmsDraftMsgId == mmsMsgId);
		
		//Validate the MMS message with all the values set before. 
		ConvergedMessage* draftMsg = msgPlugin->convertFrom(mmsDraftMsgId);
		QVERIFY(draftMsg->subject().compare(QString(TEST_MSG_SUBJECT)) == 0);
		QVERIFY(draftMsg->messageType() == ConvergedMessage::Mms);
		QVERIFY(QString(TEST_SENDER).contains(draftMsg->toAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_CC).contains(draftMsg->ccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_BCC).contains(draftMsg->bccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_SENDER).contains(draftMsg->fromAddress()->address(), Qt::CaseInsensitive) == true);
		QVERIFY(draftMsg->attachments().count() == 1);
		QVERIFY(draftMsg->attachments()[0]->attachmentType() == ConvergedMessageAttachment::EAttachment);
		QVERIFY(draftMsg->attachments()[0]->filePath().contains(QString(TEST_ATTACHMENT5).mid(QString(TEST_ATTACHMENT5).indexOf(QString("Sample.txt"), 0, Qt::CaseInsensitive)), Qt::CaseInsensitive) == true);
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to create message in Draft");
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
	}
}

//---------------------------------------------------------------
// TestMmsPlugin::sendMMSWithTextAttachment
//---------------------------------------------------------------
void TestMmsPlugin::sendMMSWithTextAttachment()
{
	//Send the MMS message and verify if it was sent successfully. 
	bool sent = msgPlugin->send(mmsMsgId);
	QVERIFY(sent == true);
}

//---------------------------------------------------------------
// TestMmsPlugin::testSentMMSWithTextAttachment
//---------------------------------------------------------------
void TestMmsPlugin::testSentMMSWithTextAttachment()
{
	QTest::qWait(10000);  //to ensure path completion
	
	//check if outbox-folder signal was received...this means message was moved to outbox for send    
	if( 1 <= spy_outbox->count())
	{
		//compare the msgid
		void * temp = const_cast<void*>(spy_outbox->at(0).at(0).data());
		long int outboxId = *reinterpret_cast< long int(*)>(temp);
		QCOMPARE(outboxId, mmsMsgId);
		
		//push the message from outbox to mmsc(sent folder)
		mmstestbed->fromOutboxToMmsc();
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to move message to Outbox");
		
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
		return;
	}

	//check if sent-folder signal was received....this means message was successfully sent scheduled.
	if( 1 <= spy_sent->count())
	{
		//check for send-path
		void * temp = const_cast<void*>(spy_sent->at(0).at(0).data());
		long int sentmsgid = *reinterpret_cast< long int(*)>(temp);
		QCOMPARE(sentmsgid, mmsMsgId);
		
		//issue a fetch request on the sent message, to test the receive path
		mmstestbed->fromMmscToInbox();
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to move message to Sent folder");
	
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
		return;
	}
	
	//check if inbox-folder signal was received....this means message was successfully received at the inbox folder.
	if( spy_inbox->count() <= 0)
	{
		QFAIL("testSendReceiveMMS: Failed to move message to Inbox folder");
		
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
	}
}

//---------------------------------------------------------------
// TestMmsPlugin::createMMSWithTextAndImageAttachment
//---------------------------------------------------------------
void TestMmsPlugin::createMMSWithTextAndImageAttachment()
{
    //Create a Converged Message instance. 
    QString subject  = TEST_MSG_SUBJECT;
	qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
	
	QString sender(TEST_SENDER);
	ConvergedMessageAddress address(sender);
	ConvergedMessageAttachmentList attachmentList;    

	//Add a text attachment to attachment list. 
	QString attachmentPath1 = TEST_ATTACHMENT5;
	ConvergedMessageAttachment* attachment = 
		new ConvergedMessageAttachment(attachmentPath1, ConvergedMessageAttachment::EInline);
	
	//Add an image attachment to attachment list. 
	QString attachmentPath2 = TEST_ATTACHMENT4;
    ConvergedMessageAttachment* attachmentImage = 
        new ConvergedMessageAttachment(attachmentPath2, ConvergedMessageAttachment::EAttachment);
    
    attachmentList.append(attachment);
    attachmentList.append(attachmentImage);
    
    //Instantiate a Converged Message object and set service a MMS
	ConvergedMessage msg;
	msg.setMessageType(ConvergedMessage::Mms);
	msg.setSubject(subject);
	msg.setTimeStamp(timeStamp);
	msg.addToRecipient(address);
	
	//Add attachments' list
	msg.addAttachments(attachmentList);
	msg.setPriority(ConvergedMessage::Normal);
	
	//Adding CC Address
	QString ccAddress(TEST_CC);
	ConvergedMessageAddress ccAdd(ccAddress);
	msg.addCcRecipient(ccAdd);
	
	//Adding BCC Address
	QString bccAddress(TEST_BCC);
	ConvergedMessageAddress bccAdd(bccAddress);
	msg.addBccRecipient(bccAdd);
	
	//Adding From Address
	QString recipientAddress(TEST_SENDER);
	ConvergedMessageAddress recipientAdd(recipientAddress);
	msg.addFromRecipient(recipientAdd);
	
	//start send and prepare to test send-success or fail
	mmstestbed->cleanAll();
	mmstestbed->setConnectionLocal(true);
	
	//Get a valid MMS message ID and verify that it is valid.
	mmsMsgId = msgPlugin->convertTo(&msg);
	QVERIFY(mmsMsgId != -1);
}

//---------------------------------------------------------------
// TestMmsPlugin::testMMSWithTextAndImageAttachment
//---------------------------------------------------------------
void TestMmsPlugin::testMMSWithTextAndImageAttachment()
{
	long int mmsDraftMsgId;
	
	//check if draft-folder signal was received...this means message was created in draft 
	if( 1 <= spy_draft->count())
	{
		//compare the msgid and verify with the ID given by MMS plugin 
		void * temp = const_cast<void*>(spy_draft->at(1).at(0).data());
		mmsDraftMsgId = *reinterpret_cast< long int(*)>(temp);
		QVERIFY(mmsDraftMsgId == mmsMsgId);
		
		//Validate the MMS message with all the values set before. 
		ConvergedMessage* draftMsg = msgPlugin->convertFrom(mmsDraftMsgId);
		QVERIFY(draftMsg->subject().compare(QString(TEST_MSG_SUBJECT)) == 0);
		QVERIFY(draftMsg->messageType() == ConvergedMessage::Mms);
		QVERIFY(QString(TEST_SENDER).contains(draftMsg->toAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_CC).contains(draftMsg->ccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_BCC).contains(draftMsg->bccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_SENDER).contains(draftMsg->fromAddress()->address(), Qt::CaseInsensitive) == true);
		QVERIFY(draftMsg->attachments().count() == 1);
		QVERIFY(draftMsg->attachments()[0]->attachmentType() == ConvergedMessageAttachment::EAttachment);
		QVERIFY(draftMsg->attachments()[0]->filePath().contains(QString(TEST_ATTACHMENT4).mid(QString(TEST_ATTACHMENT4).indexOf(QString("SmileyFace.gif"), 0, Qt::CaseInsensitive)), Qt::CaseInsensitive) == true);	
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to create message in Draft");
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
	}
}

//---------------------------------------------------------------
// TestMmsPlugin::sendMMSWithTextAndImageAttachment
//---------------------------------------------------------------
void TestMmsPlugin::sendMMSWithTextAndImageAttachment()
{
	//Send the MMS message and verify if it was sent successfully. 
	bool sent = msgPlugin->send(mmsMsgId);
	QVERIFY(sent == true);
}

//---------------------------------------------------------------
// TestMmsPlugin::testSentMMSWithTextAndImageAttachment
//---------------------------------------------------------------
void TestMmsPlugin::testSentMMSWithTextAndImageAttachment()
{
	QTest::qWait(10000);  //to ensure path completion
	
	//check if outbox-folder signal was received...this means message was moved to outbox for send    
	if( 1 <= spy_outbox->count())
	{
		//compare the msgid
		void * temp = const_cast<void*>(spy_outbox->at(1).at(0).data());
		long int outboxId = *reinterpret_cast< long int(*)>(temp);
		QCOMPARE(outboxId, mmsMsgId);
		
		//push the message from outbox to mmsc(sent folder)
		mmstestbed->fromOutboxToMmsc();
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to move message to Outbox");
		
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
		return;
	}

	//check if sent-folder signal was received....this means message was successfully sent scheduled.
	if( 1 <= spy_sent->count())
	{
		//check for send-path
		void * temp = const_cast<void*>(spy_sent->at(1).at(0).data());
		long int sentmsgid = *reinterpret_cast< long int(*)>(temp);
		QCOMPARE(sentmsgid, mmsMsgId);
		
		//issue a fetch request on the sent message, to test the receive path
		mmstestbed->fromMmscToInbox();
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to move message to Sent folder");
	
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
		return;
	}
	
	//check if inbox-folder signal was received....this means message was successfully received at the inbox folder.
	if( spy_inbox->count() <= 0)
	{
		QFAIL("testSendReceiveMMS: Failed to move message to Inbox folder");
		
		//reset connection to 'global on, local off'
		mmstestbed->setConnectionLocal(false);
	}
}

//---------------------------------------------------------------
// TestMmsPlugin::cleanup
//---------------------------------------------------------------
void TestMmsPlugin::cleanup()
{
}

//---------------------------------------------------------------
// TestMmsPlugin::cleanupTestCase
//---------------------------------------------------------------
void TestMmsPlugin::cleanupTestCase()
{
	//Cleanup
	delete spy_draft;//Signal for Draft Folder
	delete spy_outbox;//Signal for Outbox Folder
	delete spy_sent;//Signal for Sent Folder
	delete spy_inbox;//Signal for inbox Folder
    delete msgPlugin;//MMS Plugin
    delete mmstestbed;//MMS Testbed
}

//---------------------------------------------------------------
// main
// main entry point
//---------------------------------------------------------------
int main(int argc, char *argv[])
    { 
	int ret = -1;
	QCoreApplication app(argc, argv);
	QObject* tc = new TestMmsPlugin();
	
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

//End of File
