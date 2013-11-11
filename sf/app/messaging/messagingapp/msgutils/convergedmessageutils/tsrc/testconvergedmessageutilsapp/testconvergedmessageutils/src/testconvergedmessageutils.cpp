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
 * Description: Main test class definition for ConvergedMessageUtils
 */

#include <QtTest/QtTest>
#include <QTimer>
#include <QSignalSpy>
#include "debugtraces.h"
#include "testconvergedmessageutils.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "unieditormmsplugin.h"
#include "mmstestbed.h"
#include "testconvergedmessageutils.ini"

//---------------------------------------------------------------
// TestConvergedMessageUtils::initTestCase
//---------------------------------------------------------------
void TestConvergedMessageUtils::initTestCase()
{
	//register user defined object to meta system.
	qRegisterMetaType<long int> ("long int");
		
	//Verify MmsTestBed Instance. 
	mmstestbed = new MmsTestBed;
	QVERIFY(mmstestbed != NULL);
	
	//Instantiate UniEditorMmsPlugin and verify if it is correctly Instantited. 
	msgPlugin = new UniEditorMmsPlugin;
	QVERIFY(msgPlugin != NULL);
	
	//set up signalspy to listen to signals emitted by mmstestbed
	spy_draft = new QSignalSpy(mmstestbed, SIGNAL(entryCreatedInDraft(long int)));
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::init
//---------------------------------------------------------------
void TestConvergedMessageUtils::init()
{
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::createMMS
//---------------------------------------------------------------
void TestConvergedMessageUtils::createMMS()
{
    //Create a Converged Message instance. 
    QString subject  = TEST_MSG_SUBJECT;
    
    qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
	
	QString sender(TEST_SENDER);
	ConvergedMessageAddress address(sender);
	ConvergedMessageAttachmentList attachmentList;    

	//Add a text attachment to attachment list. 
	QString attachmentPath = TEST_ATTACHMENT;
	ConvergedMessageAttachment* attachment = 
		new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
	
	attachmentList.append(attachment);
	
	//Instantiate a Converged Message object and set service a MMS
	ConvergedMessage msg;
	
	msg.setMessageType(ConvergedMessage::Mms);
	
	//Set Subject
	msg.setSubject(subject);
	
	//Set Timestamp and verify
	msg.setTimeStamp(timeStamp);
	QVERIFY(timeStamp == msg.timeStamp());
	
	//Set Alias to an address
	address.setAlias(QString (TEST_ALIAS));
	
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
	
	//Set Body Text and verify
	msg.setBodyText(QString(TEST_MSG_BODY));
	QVERIFY(msg.bodyText().compare(QString(TEST_MSG_BODY)) == 0);
	
	//Removing Body Text as MMS messages do not contain Body Text
	msg.setBodyText(QString(NULL));
	
	//Set Property and Verify
	msg.setProperty(ConvergedMessage::Attachment);
	msg.setProperty(ConvergedMessage::Unread);
	QVERIFY(msg.properties() == ConvergedMessage::Attachment | ConvergedMessage::Unread);
	QVERIFY(msg.hasAttachment() == true);
	QVERIFY(msg.isUnread() == true);
			
	//Set Location
	msg.setLocation(ConvergedMessage::Draft);
	
	//Set Sending State and Verify
	msg.setSendingState(ConvergedMessage::Waiting);
	QVERIFY(msg.sendingState() == ConvergedMessage::Waiting);
	
	//Set Direction
	msg.setDirection(ConvergedMessage::Outgoing);
	
	//Set Priority
	msg.setPriority(ConvergedMessage::Normal);
	
	//Set Sub Type and verify
	msg.setMessageSubType(ConvergedMessage::NokiaService);
	QVERIFY(msg.messageSubType() == ConvergedMessage::NokiaService);
	
	//Create another Converged Message
	ConvergedMessage msgCopyFirst(msg);
	ConvergedMessage msgCopySecond(*(msg.id()));//by Id
	QCOMPARE(msgCopySecond.id()->getId(), msg.id()->getId());//Verify if the copy was as expected
	
	//Create a Converged Message with serializing and deserializing and verify
	QFile file("c:\\test.txt");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);   // Serialize the data into the file
	msg.serialize(out);   // Serialize a string
	file.close();
 
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);    // read the data serialized from the file
	
	ConvergedMessage msgCopyThird;//Third Copy of the Actual Message
	msgCopyThird.deserialize(in);
	file.close();
	
	//Verify the 3rd Copy with the Original Copy
	QVERIFY(msgCopyThird.id()->getId() == msg.id()->getId());
	QCOMPARE(msgCopyThird.subject(), msg.subject());
	QVERIFY(msgCopyThird.messageType() == msg.messageType());
	
	//Get a valid MMS message ID and verify that it is valid.
	mmsMsgId = msgPlugin->convertTo(&msgCopyFirst);
	QVERIFY(mmsMsgId != -1);
	QDEBUG_WRITE("MMS Successfully Sent to Dratfs Folder");
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::testMMS
//---------------------------------------------------------------
void TestConvergedMessageUtils::testMMS()
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
		QVERIFY(QString(TEST_ALIAS).contains(draftMsg->toAddressList()[0]->alias(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_CC).contains(draftMsg->ccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_BCC).contains(draftMsg->bccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
		QVERIFY(QString(TEST_SENDER).contains(draftMsg->fromAddress()->address(), Qt::CaseInsensitive) == true);
		QVERIFY(draftMsg->attachments().count() == 1);
		QVERIFY(draftMsg->attachments()[0]->attachmentType() == ConvergedMessageAttachment::EAttachment);
		QVERIFY(draftMsg->attachments()[0]->filePath().contains(QString(TEST_ATTACHMENT).mid(QString(TEST_ATTACHMENT).indexOf(QString("Sample.txt"), 0, Qt::CaseInsensitive)), Qt::CaseInsensitive) == true);
		QVERIFY(draftMsg->location() == ConvergedMessage::Draft);
		QVERIFY(draftMsg->priority() == ConvergedMessage::Normal);
		QVERIFY(draftMsg->direction() == ConvergedMessage::Outgoing);
		QDEBUG_WRITE("MMS Successfully Verified at Drafts Folder");
	}
	else
	{
		QFAIL("testSendReceiveMMS: Failed to create message in Draft");
	}
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::testConvergedMessageIdUnusedMethods
//---------------------------------------------------------------
void TestConvergedMessageUtils::testConvergedMessageIdUnusedMethods()
{
	//Create a copy of ConvergedMessageId object from another object
	ConvergedMessageId msgId1;
	msgId1.setId(0x646);
	
	//Verify if the two objects match
	ConvergedMessageId msgId3;
	msgId3 = msgId1;
	QVERIFY(msgId3 == msgId1);
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::testConvergedMessageAttachmentUnusedMethods
//---------------------------------------------------------------
void TestConvergedMessageUtils::testConvergedMessageAttachmentUnusedMethods()
{
	//Set Attachment File Path and Type and Verify
	ConvergedMessageAttachment msgAttachment;
	msgAttachment.setFilePath(QString(TEST_ATTACHMENT));
	QCOMPARE(msgAttachment.filePath(), QString(TEST_ATTACHMENT));
	msgAttachment.setAttachmentType(ConvergedMessageAttachment::EAttachment);
	QVERIFY(msgAttachment.attachmentType() == ConvergedMessageAttachment::EAttachment);
	
	//Second Copy with same Contents
	ConvergedMessageAttachment msgAttachmentCopy;
	msgAttachmentCopy.setFilePath(QString(TEST_ATTACHMENT));
	msgAttachmentCopy.setAttachmentType(ConvergedMessageAttachment::EAttachment);
	
	//Verify if they are same
	QVERIFY(msgAttachmentCopy == msgAttachment);
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::cleanup
//---------------------------------------------------------------
void TestConvergedMessageUtils::cleanup()
{
}

//---------------------------------------------------------------
// TestConvergedMessageUtils::cleanupTestCase
//---------------------------------------------------------------
void TestConvergedMessageUtils::cleanupTestCase()
{
	//Cleanup
	mmstestbed->cleanAll();//Clean All messages from varios Folders
	delete spy_draft;//Signal for Draft Folder
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
	QObject* tc = new TestConvergedMessageUtils();
	
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
