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
#include <csmsaccount.h>

#include "convergedmessage.h"
#include "unidatamodelplugininterface.h"
#include "unidatamodelloader.h"
#include "testunidatamodelsmsplugin.h"
#include "testunidatamodelsmsplugin.ini"
_LIT(KUnixEpoch, "19700000:000000.000000");


//main entry point
int main(int argc, char *argv[])
    { 
    int ret = -1;
    QCoreApplication app(argc, argv);
    QObject* tc = new TestUniDataModelSMSPlugin();
    
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
// TestUniDataModelSMSPlugin::init
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::init()
{
    iObserver = new (ELeave) TestObserver();

    // Create a new messaging server session..
    iMSession = CMsvSession::OpenSyncL(*iObserver);
    // Create the client mtm registry
    iMtmReg = CClientMtmRegistry::NewL(*iMSession);

    // Get the SMS clientmtm
    iSmsClientMtm = static_cast<CSmsClientMtm*> (iMtmReg->NewMtmL(KUidMsgTypeSMS));
      
    TRAPD(err, iSmsClientMtm->DefaultServiceL()) ;     
            
    if( err == KErrNotFound)
        {
        CSmsAccount* account = CSmsAccount::NewL();
        CleanupStack::PushL(account);
        CSmsSettings* settings = CSmsSettings::NewL();
        CleanupStack::PushL(settings);
        account->InitialiseDefaultSettingsL(*settings);
        CleanupStack::PopAndDestroy(2);
        }
    messageId = 0;
    pluginLoader = new UniDataModelLoader();
    retTimeStamp = QDateTime::currentDateTime();
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::cleanup
//---------------------------------------------------------------
void TestUniDataModelSMSPlugin::cleanup()
{

    if (messageId > 0)
        iSmsClientMtm->Entry().DeleteL(messageId);

    delete iObserver;
    iObserver = NULL;
    
    delete iSmsClientMtm;
    iSmsClientMtm = NULL;
    
    delete iMtmReg;
    iMtmReg = NULL;
    
    delete iMSession;
    iMSession = NULL;
    
    delete pluginLoader;
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::validateMsg
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::createInboxSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId));

    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = XQConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recepient(TEST_MSG_FROM1);
    QString recepient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = XQConversions::qStringToS60Desc(recepient);
    HBufC* addr2 = XQConversions::qStringToS60Desc(recepient2);
    HBufC* alias1 = XQConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);
        CSmsHeader& smsHeader = iSmsClientMtm->SmsHeader();
        smsHeader.SetFromAddressL(*addr);
        messageId = indexEntry.Id();

        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::createOutboxSMS
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::createOutboxSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvGlobalOutBoxIndexEntryId));

    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = XQConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recepient(TEST_MSG_FROM1);
    QString recepient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = XQConversions::qStringToS60Desc(recepient);
    HBufC* addr2 = XQConversions::qStringToS60Desc(recepient2);
    HBufC* alias1 = XQConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        // If drafts/ outbox message , set the recipients 
        iSmsClientMtm->AddAddresseeL(*addr, TPtrC());
        iSmsClientMtm->AddAddresseeL(*addr2, *alias1);
        indexEntry.SetSendingState(KMsvSendStateWaiting);
        messageId = indexEntry.Id();
        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::createDraftsSMS
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::createDraftsSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvDraftEntryId));
    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = XQConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = XQConversions::qStringToS60Desc(recipient);
    HBufC* addr2 = XQConversions::qStringToS60Desc(recipient2);
    HBufC* alias1 = XQConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        // If drafts/ outbox message , set the recipients 
        iSmsClientMtm->AddAddresseeL(*addr, TPtrC());
        indexEntry.SetSendingState(KMsvSendStateWaiting);
        messageId = indexEntry.Id();

        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::createDraftsSMSNoRecipient
//---------------------------------------------------------------
void TestUniDataModelSMSPlugin::createDraftsSMSNoRecipient()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvDraftEntryId));
    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = XQConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    indexEntry.SetSendingState(KMsvSendStateWaiting);
    messageId = indexEntry.Id();

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::testInboxMessage
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::testInboxMessage()
{
    createInboxSMS();

    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    pluginInterface->setMessageId(messageId);

    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString origalias(TEST_MSG_ALIAS1);

    QVERIFY(recipientList.count() == 0);

    int size1 = pluginInterface->messageSize();

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recipient == fromaddress);
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::testOutboxMessage
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::testOutboxMessage()
{
    createOutboxSMS();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    // Get the data for the first message in DRAFTS    
    pluginInterface->setMessageId(messageId);
    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString origalias(TEST_MSG_ALIAS1);

    QVERIFY(recipientList.count() == 2);
    QVERIFY(recipient == recipientList[0]->address()
            && recipientList[0]->alias().isEmpty());

    const QString& as = recipientList[1]->address();
    const QString& as1 = recipientList[1]->alias();
    QVERIFY(recipient2 == as);

    // ConvergedMessageAddress::alias return a QSting with a space
    origalias.append(' ');
    QVERIFY(origalias == as1);

    int size1 = pluginInterface->messageSize();

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(fromaddress.isEmpty());
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::testDraftsMessage
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::testDraftsMessage()
{
    createDraftsSMS();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    // Get the data for the first message in DRAFTS    
    pluginInterface->setMessageId(messageId);
    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);

    QVERIFY(recipientList.count() == 1);
    QVERIFY(recipient == recipientList[0]->address()
            && recipientList[0]->alias().isEmpty());

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(fromaddress.isEmpty());
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::testDraftsMessageNoRecipient
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::testDraftsMessageNoRecipient()
{
    createDraftsSMSNoRecipient();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    // Get the data for the first message in DRAFTS    
    pluginInterface->setMessageId(messageId);
    int size = pluginInterface->messageSize();

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QVERIFY(recipientList.count() == 0);

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(fromaddress.isEmpty());
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::testGetPluginWithoutLoading
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::testGetPluginWithoutLoading()
{
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);
    QVERIFY(pluginInterface == NULL);

    // Try to geta plugin for a differnt message type
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    QVERIFY(pluginInterface == NULL);
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::testUnusedAPIs
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::testUnusedAPIs()
{
	  pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    QVERIFY(pluginInterface->hasAttachment() == false);

    UniMessageInfoList attachmentlist = pluginInterface->attachmentList();
    QVERIFY(attachmentlist.isEmpty());

    QVERIFY(pluginInterface->objectCount() == 0);

    UniMessageInfoList objectlist = pluginInterface->objectList();
    QVERIFY(objectlist.isEmpty());

    QVERIFY(pluginInterface->slideCount() == 0);

    UniMessageInfoList slidelist = pluginInterface->slideContent(0);
    QVERIFY(slidelist.isEmpty());
    
    ConvergedMessageAddressList recipientList;
    
	pluginInterface->ccRecipientList(recipientList);
	QVERIFY(recipientList.count() == 0);
		
	pluginInterface->bccRecipientList(recipientList);
	QVERIFY(recipientList.count() == 0);
		
	 MsgPriority priority = pluginInterface->messagePriority();
    // Normal priority
    qDebug() << "prority" << priority;
    QVERIFY(priority == 1);
	int count = pluginInterface->attachmentCount();
    QVERIFY(count == 0);
		
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::HandleSessionEventL
//---------------------------------------------------------------

void TestObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                       TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}
