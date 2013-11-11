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
 * Description: Main test class definition for UnieditorPluginLoader
 */

#include <QtTest/QtTest>
#include "debugtraces.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
#include "testunieditorpluginloader.h"
#include "testunieditorpluginloader.ini"

//---------------------------------------------------------------
// TestUnieditorPluginLoader::initTestCase
//---------------------------------------------------------------
void TestUnieditorPluginLoader::initTestCase()
{
	//register user defined object to meta system.
	qRegisterMetaType<long int> ("long int");
		
	//Instantiate UnieditorPluginLoader and verify if it is correctly Instantited. 
	unieditorPluginLoader = new UniEditorPluginLoader;
	QVERIFY(unieditorPluginLoader != NULL);
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::init
//---------------------------------------------------------------
void TestUnieditorPluginLoader::init()
{
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::createMMS
//---------------------------------------------------------------
void TestUnieditorPluginLoader::createMMS()
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
	
	messageType = ConvergedMessage::Mms;
	msg.setMessageType(messageType);
	
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
	
	UniEditorPluginInterface* pluginInterface = unieditorPluginLoader->getUniEditorPlugin(messageType);
	
	//Get a valid MMS message ID and verify that it is valid.
	msgId = pluginInterface->convertTo(&msg);
	QVERIFY(msgId != -1);
	QDEBUG_WRITE("MMS Successfully Created in Drafts Folder");
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::testMMS
//---------------------------------------------------------------
void TestUnieditorPluginLoader::testMMS()
{
	//check if draft-folder signal was received...this means message was created in draft
	
	UniEditorPluginInterface* pluginInterface = unieditorPluginLoader->getUniEditorPlugin(messageType);
	
	//Validate the MMS message with all the values set before. 
	ConvergedMessage draftMsg = *(pluginInterface->convertFrom(msgId));
	QVERIFY(draftMsg.subject().compare(QString(TEST_MSG_SUBJECT)) == 0);
	QVERIFY(draftMsg.messageType() == ConvergedMessage::Mms);
	QVERIFY(QString(TEST_SENDER).contains(draftMsg.toAddressList()[0]->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_ALIAS).contains(draftMsg.toAddressList()[0]->alias(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_CC).contains(draftMsg.ccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_BCC).contains(draftMsg.bccAddressList()[0]->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_SENDER).contains(draftMsg.fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(draftMsg.attachments().count() == 1);
	QVERIFY(draftMsg.attachments()[0]->attachmentType() == ConvergedMessageAttachment::EAttachment);
	QVERIFY(draftMsg.attachments()[0]->filePath().contains(QString(TEST_ATTACHMENT).mid(QString(TEST_ATTACHMENT).indexOf(QString("Sample.txt"), 0, Qt::CaseInsensitive)), Qt::CaseInsensitive) == true);
	QVERIFY(draftMsg.location() == ConvergedMessage::Draft);
	QVERIFY(draftMsg.priority() == ConvergedMessage::Normal);
	QVERIFY(draftMsg.direction() == ConvergedMessage::Outgoing);
	QDEBUG_WRITE("MMS Successfully Verified at Drafts Folder");
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::createSMS
//---------------------------------------------------------------
void TestUnieditorPluginLoader::createSMS()
{
    //Create a Converged Message instance. 
    QString subject  = TEST_MSG_SUBJECT;
    
    qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
	
	QString sender(TEST_SENDER);
	ConvergedMessageAddress address(sender);
	
	//Instantiate a Converged Message object and set service a SMS
	ConvergedMessage msg;
	
	messageType = ConvergedMessage::Sms;
	msg.setMessageType(messageType);
	
	//Set Subject
	msg.setSubject(subject);
	
	//Set Body
	msg.setBodyText(QString(TEST_MSG_BODY));
	
	//Set Timestamp and verify
	msg.setTimeStamp(timeStamp);
	QVERIFY(timeStamp == msg.timeStamp());
	
	//Set recipient. 
	msg.addToRecipient(address);
	
	//Set priority.
	msg.setPriority(ConvergedMessage::Normal);
	
	//Adding From Address
	QString recipientAddress(TEST_SENDER);
	ConvergedMessageAddress recipientAdd(recipientAddress);
	msg.addFromRecipient(recipientAdd);
	
	//Set Property and Verify
	msg.setProperty(ConvergedMessage::Unread);
	QVERIFY(msg.properties() == ConvergedMessage::Unread);
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
	
	UniEditorPluginInterface* pluginInterface = unieditorPluginLoader->getUniEditorPlugin(messageType);
	
	//Get a valid SMS message ID and verify that it is valid.
	msgId = pluginInterface->convertTo(&msg);
	QVERIFY(msgId != -1);
	QDEBUG_WRITE("SMS Successfully Created in Drafts Folder");
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::testSMS
//---------------------------------------------------------------
void TestUnieditorPluginLoader::testSMS()
{
	//check if draft-folder signal was received...this means message was created in draft
	
	UniEditorPluginInterface* pluginInterface = unieditorPluginLoader->getUniEditorPlugin(messageType);
	
	//Validate the SMS message with all the values set before. 
	ConvergedMessage draftMsg = *(pluginInterface->convertFrom(msgId));
	QVERIFY(draftMsg.subject().compare(QString(TEST_MSG_SUBJECT)) == 0);
	QVERIFY(draftMsg.bodyText().compare(QString(TEST_MSG_BODY)) == 0);
	QVERIFY(draftMsg.messageType() == ConvergedMessage::Sms);
	QVERIFY(QString(TEST_SENDER).contains(draftMsg.toAddressList()[0]->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_SENDER).contains(draftMsg.fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(draftMsg.location() == ConvergedMessage::Draft);
	QVERIFY(draftMsg.priority() == ConvergedMessage::Normal);
	QVERIFY(draftMsg.direction() == ConvergedMessage::Outgoing);
	QDEBUG_WRITE("SMS Successfully Verified at Drafts Folder");
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::cleanup
//---------------------------------------------------------------
void TestUnieditorPluginLoader::cleanup()
{
}

//---------------------------------------------------------------
// TestUnieditorPluginLoader::cleanupTestCase
//---------------------------------------------------------------
void TestUnieditorPluginLoader::cleanupTestCase()
{
	//Cleanup
	delete unieditorPluginLoader;//SMS/MMS Plugin Loader
}

//---------------------------------------------------------------
// main
// main entry point
//---------------------------------------------------------------
int main(int argc, char *argv[])
    { 
	int ret = -1;
	QCoreApplication app(argc, argv);
	QObject* tc = new TestUnieditorPluginLoader();
	
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
