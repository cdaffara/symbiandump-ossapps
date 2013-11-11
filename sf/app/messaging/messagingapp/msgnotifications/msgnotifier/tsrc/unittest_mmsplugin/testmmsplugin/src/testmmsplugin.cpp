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
 * Description: - Main test class for Msgnotification -indicator plugin, 
 *                                                   -notification dialog plugin
 */

#include "testmmsplugin.h"
#include <QtTest/QtTest>
#include "debugtraces.h"
#include <QTimer>
#include <QSignalSpy>
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "unieditormmsplugin.h"
#include "mmstestbed.h"
#include "testmmsplugin.ini"
#include <QSignalSpy>



//factory method to create objects.
QObject* getObject(QString className)
{
    if(className == "TestMmsPlugin" )
    {
        return new TestMmsPlugin;
    }
 	else
	{
		return 0;
	}
}

//main entry point
int main(int argc, char *argv[])
    { 
    int ret = -1;
	QCoreApplication app(argc, argv);
	
	//the configuration file.
	QFile data("c:/testmmsplugin.cfg");
    if (data.open(QFile::ReadOnly)) 
        {
		QTextStream in(&data);
		while(!in.atEnd())
			{
			QString name = in.readLine();
			QObject* tc = getObject(name);
			
			if(tc)
				{
				ret =  QTest::qExec(tc, argc, argv);
				delete tc;
				}
			}
        }
	return ret;

    }






//---------------------------------------------------------------
// TestMmsPlugin::initTestCase
//---------------------------------------------------------------

void TestMmsPlugin::initTestCase()
{
    qRegisterMetaType<long int> ("long int"); 
    msgPlugin = new UniEditorMmsPlugin();
    QVERIFY(msgPlugin != 0);
    mmsId = -1;
    mmstestbed = new MmsTestBed;
	QVERIFY(mmstestbed != 0);
	if(!mmstestbed)
	{
		QSKIP("Mmstestbed creation failed", SkipAll); 
	}

    spy_draft  = new QSignalSpy(mmstestbed,SIGNAL(entryCreatedInDraft(long int)));
    spy_outbox = new QSignalSpy(mmstestbed,SIGNAL(entryMovedToOutbox(long int)));
    spy_sent   = new QSignalSpy(mmstestbed,SIGNAL(entryMovedToSent(long int)));

}

//---------------------------------------------------------------
// TestMmsPlugin::init
//---------------------------------------------------------------

void TestMmsPlugin::init()
{
}

//---------------------------------------------------------------
// TestMmsPlugin::testSendMMS
//---------------------------------------------------------------

void TestMmsPlugin::testSendMMS()
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
	
	//Adding CC Address
	QString ccAddress(TEST_CC);
	ConvergedMessageAddress ccAdd(ccAddress);
	msg.addCcRecipient(ccAdd);
	
	//Adding BCC Address
	QString bccAddress(TEST_BCC);
	ConvergedMessageAddress bccAdd(bccAddress);
	msg.addBccRecipient(bccAdd);
	
	mmstestbed->cleanAll();
	mmstestbed->setConnectionLocal(true);
	long int id = msgPlugin->convertTo(&msg);
	
	QVERIFY(id != 0);
	msgPlugin->send(id);
	QTest::qWait(5000); 
	long int idRec;
	
	if( 1 == spy_draft->count())
		{
		void * temp = const_cast<void*>(spy_draft->at(0).at(0).data());
		idRec = *reinterpret_cast< long int(*)>(temp);
		QDEBUG_WRITE("passed: spy_draft.count");
		}
	else
		{
		QFAIL("testSendReceiveMMS: Failed to create message in Draft");
		mmstestbed->setConnectionLocal(false);
		return;
		}
	if( 1 == spy_outbox->count())
		{
		mmstestbed->fromOutboxToMmsc();
		QDEBUG_WRITE("passed: spy_outbox.count");
		}
	else
		{
		QFAIL("testSendReceiveMMS: Failed to move message to Outbox");
		mmstestbed->setConnectionLocal(false);
		return;
		}

	if( 1 == spy_sent->count())
		{
		void * temp = const_cast<void*>(spy_sent->at(0).at(0).data());
		long int sentmsgid = *reinterpret_cast< long int(*)>(temp);
		QCOMPARE(sentmsgid, idRec);
		mmstestbed->fromMmscToInbox();
		QDEBUG_WRITE("passed: spy_sent.count");
		}
	else
		{
		QFAIL("testSendReceiveMMS: Failed to move message to Sent folder");
		mmstestbed->setConnectionLocal(false);
		return;
		}
}

//---------------------------------------------------------------
// TestMmsPlugin::testMmsReceived
//---------------------------------------------------------------
void TestMmsPlugin::testMmsReceived()
{
        QTest::qWait(5000);
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
    delete msgPlugin;
    delete mmstestbed;
}

