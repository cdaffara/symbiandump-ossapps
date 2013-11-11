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
 * Description: Main test class definition for MMS Data Plugin
 */

#include <QtTest/QtTest>
#include <QtDebug>
#include <QTimer>
#include <QSignalSpy>
#include <mmsclient.h>
#include <mmssettings.h>
#include <CMsvMimeHeaders.h>
#include <f32file.h>
#include "debugtraces.h"
#include "convergedmessageaddress.h"
#include "testunidatamodelmmsplugin.h"
#include "testunidatamodelmmsplugin.ini"
#include "xqconversions.h"
#include "unidatamodelplugininterface.h"
#include "unidatamodelloader.h"

//---------------------------------------------------------------
// TestUniDataModelPlugin::initTestCase
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::initTestCase()
    {
    iObserver = new (ELeave) TestObserverMMS();

    // Create a new messaging server session..
    TRAPD(err,iMSession = CMsvSession::OpenSyncL(*iObserver));
    if(err)
        {
        TInt x = err;
        }
    
    // Create the client mtm registry
    CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL(*iMSession);

    // Get the MMS clientmtm
    iMmsClientMtm = static_cast<CMmsClientMtm*> (mtmReg->NewMtmL(KUidMsgTypeMultimedia));

    //Connecting to File Server Session
    User::LeaveIfError( iFs.Connect() );
    iFs.SetSessionPath( KRootPath );
    
    //Getting Service ID for MMS
    CMmsSettings* settings = CMmsSettings::NewL();
    settings->LoadSettingsL();
    iServiceId = settings->Service();
    
    iMessageId = 0;
    
    //Saving the Timestamp before Message is created
    retTimeStamp = QDateTime::currentDateTime();
    
    // Get the MMS data modelplugin
    pluginLoader = new UniDataModelLoader;
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    }

//---------------------------------------------------------------
// TestUniDataModelPlugin::init
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::init()
{
}

//---------------------------------------------------------------
// TestUniDataModelPlugin::testInboxMessage
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::testInboxMessage()
    {
	//Create the MMS First
	createMMS(KMsvGlobalInBoxIndexEntryIdValue,ETrue);
    
	QTest::qWait(50000);  //to ensure path completion
	
	//MMS Message created, will now be validated
	
	//Checking if Another Instance of MMS Data Plugin can be created form an existing one
	QObject* anotherInstance = pluginInterface->createInstance();
    QVERIFY(anotherInstance != NULL);
    delete anotherInstance;
    
    //Setting a Message ID, thereby populating the MMS message at the Plugin
    QVERIFY(pluginInterface->setMessageId(iMessageId) == KErrNone);

    //Verifying the MMS message size
    QVERIFY(pluginInterface->messageSize() == msgSize);
    
    //Verifying the MMS timestamp
    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);
    
    //Verifying is the MMP Plugin works file afetr it has been Reset and Restored
    pluginInterface->reset();
	pluginInterface->restore(*iMmsClientMtm);
	QVERIFY(pluginInterface->setMessageId(iMessageId) == KErrNone);
	
	//Subject Verification
    QVERIFY(pluginInterface->subject().compare(QString(TEST_MSG_SUBJECT)) == 0);
    
    //MMS message have no Body String
    QString body;
    pluginInterface->body(body);
    QVERIFY(body == QString(""));
    
    //Message type verification
    QVERIFY(pluginInterface->messageType().compare(QString("mms")) == 0);
    
    //Fetch the To List
    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);
    QVERIFY(recipientList.count() != 0);
    QVERIFY(recipientList[0]->address().compare(QString(TEST_MSG_RECIEPIENT1)) == 0);
    
    //Fetch the CC List
    ConvergedMessageAddressList recipientList2;
    pluginInterface->ccRecipientList(recipientList2);
    QVERIFY(recipientList2.count() != 0);
    QVERIFY(recipientList2[0]->address().compare(QString(TEST_MSG_RECIEPIENT2)) == 0);
    
    //Fetch the BCC List
    ConvergedMessageAddressList recipientList3;
    pluginInterface->bccRecipientList(recipientList3);
    QVERIFY(recipientList3.count() != 0);
    QVERIFY(recipientList3[0]->address().compare(QString(TEST_MSG_RECIEPIENT3)) == 0);
    
    //Fetch the From Address
    QString recepient(TEST_MSG_FROM1);
    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recepient == fromaddress);
    
    //Verifying Attachments
    UniMessageInfoList attList = pluginInterface->attachmentList();
    QVERIFY(attList.count() == 0);
    
    QVERIFY(pluginInterface->hasAttachment() == false);
    
    QVERIFY(pluginInterface->attachmentCount() == 0);
    
    QVERIFY(pluginInterface->messagePriority() == 1);

    //Verifying various objects added to the MMS Message during creation
    QVERIFY(pluginInterface->objectCount() == 2);
    
    UniMessageInfoList objectlist = pluginInterface->objectList();
    QVERIFY(!objectlist.isEmpty());
    
    QVERIFY(objectlist[1]->path().contains(QString("TestUnidataModelMMSPluginPhoto.jpg"), Qt::CaseInsensitive) == true);
	QVERIFY(objectlist[1]->mimetype().contains(QString("image/jpeg"), Qt::CaseInsensitive) == true);
    
    QVERIFY(pluginInterface->slideCount() == 1);
    
    QVERIFY(pluginInterface->slideContent(0)[0]->path().contains(QString("Story.txt"), Qt::CaseInsensitive) == true);
    QVERIFY(pluginInterface->slideContent(0)[0]->mimetype().contains(QString("text/plain"), Qt::CaseInsensitive) == true);
    
    //Verifying if the plugin returns the correct session, thus will correctly delete the MMS we just created
    CMsvSession* msvSession = pluginInterface->session();
    QVERIFY(msvSession != NULL);
    msvSession->RemoveEntry(iMessageId);
    
    qDebug() << "Verified Test MMS Successfully";    
    }

//---------------------------------------------------------------
// TestUniDataModelPlugin::cleanup
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::cleanup()
{
}

//---------------------------------------------------------------
// TestUniDataModelPlugin::cleanupTestCase
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::cleanupTestCase()
    {
	//Cleanup
	delete pluginInterface;
	delete pluginLoader;
    delete iMmsClientMtm;
    delete iMSession;
    delete iObserver;
    }

//---------------------------------------------------------------
// TestUniDataModelPlugin::createMMS
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::createMMS(TMsvId pId, TBool subjectField)
    {
    TMsvEntry indexEntry;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iMtm = KUidMsgTypeMultimedia;
    indexEntry.iServiceId = iServiceId;
    indexEntry.iDate.HomeTime();
    
    // Convert TTime to QDateTime , this will be used for comparing the time of mesage 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    TMsvId parentId = pId;
    CMsvEntry* entry = CMsvEntry::NewL(*iMSession,parentId,TMsvSelectionOrdering());
    CleanupStack::PushL(entry);

    entry->SetEntryL(parentId);
    entry->CreateL(indexEntry);
    entry->SetEntryL(indexEntry.Id());
    iMmsClientMtm->SwitchCurrentEntryL(entry->EntryId());

    CMsvStore* store = iMmsClientMtm->Entry().EditStoreL();
    CleanupStack::PushL(store);
      
    //MMS Message ID Saved
    iMessageId = indexEntry.Id();
    
    //Adding Subject
    if(subjectField)
        {
        QString subject(TEST_MSG_SUBJECT);
        HBufC* subj = XQConversions::qStringToS60Desc(subject);
        iMmsClientMtm->SetSubjectL(*subj);
        }

    //Adding Sender
    QString sender(TEST_MSG_FROM1);
    HBufC* addr = XQConversions::qStringToS60Desc(sender);
    
    if (addr)
        {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);
        
        iMmsClientMtm->SetSenderL(*addr);
        
        CleanupStack::PopAndDestroy(addr);
        }
    
    //Adding Recipient
    QString recipient(TEST_MSG_RECIEPIENT1);
    HBufC* addr2 = XQConversions::qStringToS60Desc(recipient);
    if (addr2)
        {
        CleanupStack::PushL(addr2);
        iMmsClientMtm->AddAddresseeL(EMsvRecipientTo,*addr2);
        CleanupStack::PopAndDestroy(addr2);
        }
    
    //Adding cCRecipient
    QString recipient2(TEST_MSG_RECIEPIENT2);
    HBufC* addr3 = XQConversions::qStringToS60Desc(recipient2);
    if (addr3)
        {
        CleanupStack::PushL(addr3);
        iMmsClientMtm->AddAddresseeL(EMsvRecipientCc,*addr3);
        CleanupStack::PopAndDestroy(addr3);
        }
      
    //Adding bCCRecipient
    QString recipient3(TEST_MSG_RECIEPIENT3);
	HBufC* addr4 = XQConversions::qStringToS60Desc(recipient3);
	if (addr4)
		{
		CleanupStack::PushL(addr4);
		iMmsClientMtm->AddAddresseeL(EMsvRecipientBcc,*addr4);
		CleanupStack::PopAndDestroy(addr4);
		}
	
    //Create Text Attachment
    TMsvAttachmentId attaId = 0;
    TFileName attachmentFile( _L("story.txt") );
    TBufC<12> story = _L( "Hello world!" );
    
    iMmsClientMtm->CreateTextAttachmentL(*store, attaId, story, attachmentFile, ETrue );
    qDebug() << "Text Attachment id" << attaId;
    
    //Image Attachment added to the message entry
    RFile attaFile;
	// Set filename of attachment
	TFileName name( KPictureFileName );
   
	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
	CleanupStack::PushL( mimeHeaders );
   
	// Set values to mime headers
	mimeHeaders->SetContentTypeL( _L8( "image") );
	mimeHeaders->SetContentSubTypeL( _L8( "jpeg" ) );
   
	_LIT8(KMimeType, "image/jpeg");
	TBufC8<10> mimeType(KMimeType);
	// CreateAttachment2L will set the content type to attachment Info
   
	// Open the attachment file for reading
	attaFile.Open( iFs, name, EFileShareReadersOnly | EFileRead );
	CleanupClosePushL(attaFile);
   
	CMsvAttachment* attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	// attaInfo ownerhip will be transferred to Attachment Manager.
	// It must not be pushed onto the cleanupStack before calling 
	// CreateAttachment2L.
   
	iMmsClientMtm->CreateAttachment2L(*store, attaFile,mimeType,*mimeHeaders,attaInfo,attaId);
	qDebug() << "Image Attachment id" << attaId;
	
	// Now Attachment Manager owns the attaInfo
	CleanupStack::Pop(); // attaFile.Close()
	CleanupStack::PopAndDestroy(); // mimeHeaders
        
    store->CommitL();
    
    CleanupStack::PopAndDestroy(store); // store
    
    iMmsClientMtm->SaveMessageL();
    
    //Saving the message size for future checking
    msgSize = iMmsClientMtm->MessageSize();
    
    CleanupStack::Pop(entry); // entry
    }

//---------------------------------------------------------------
// TestObserverMMS::HandleSessionEventL
//---------------------------------------------------------------
void TestObserverMMS::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                       TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}

//---------------------------------------------------------------
// main
// main entry point
//---------------------------------------------------------------
int main(int argc, char *argv[])
    { 
	int ret = -1;
	QCoreApplication app(argc, argv);
	QObject* tc = new TestUniDataModelMMSPlugin();
	
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
