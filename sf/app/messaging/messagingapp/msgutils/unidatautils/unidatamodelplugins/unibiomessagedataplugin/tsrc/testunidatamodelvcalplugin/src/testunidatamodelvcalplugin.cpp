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

#include "testunidatamodelvcalplugin.h"
#include <unidatamodelloader.h>
#include "testunidatamodelvcalplugin.ini"
#include "unidatamodelplugininterface.h"
#include <convergedmessage.h>

const TInt KTelephoneNumberMaxLength = 32;
_LIT(KBIOTxtFilePath, "C:\\test\\");
_LIT(KBIOvCalenderPrefix, "vcal*");
// vCalender
_LIT(KUnixEpoch, "19700000:000000.000000");
#define KBioUidValue  0x10001262

const TUid KBioUidValueUid =
{KBioUidValue};


//factory method to create objects.
QObject* getObject(QString className)
{
    if(className == "TestUniDataModelVCalPlugin" )
    {
        return new TestUniDataModelVCalPlugin;
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
	QFile data("c:/TestUniDataModelVCalPlugin.cfg");
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
// TestUniDataModelVCalPlugin::init
//---------------------------------------------------------------


void TestUniDataModelVCalPlugin::initTestCase()
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
	    drafts = EFalse;
}
void TestUniDataModelVCalPlugin::cleanupTestCase()
{
	    delete iMsvEntry;
	    iMsvEntry = NULL;
	    delete iObserver;
	    iObserver = NULL;
	    delete iMSession;
	    iMSession = NULL;
	    delete pluginLoader;
}

void TestUniDataModelVCalPlugin::init()
{
    
}

//---------------------------------------------------------------
// TestUniDataModelVCalPlugin::cleanup
//---------------------------------------------------------------
void TestUniDataModelVCalPlugin::cleanup()
{
  

}

void TestUniDataModelVCalPlugin::testInBoxVCalMessage()
{
		inbox = ETrue;
		GenerateMessagesL();

		pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
		pluginInterface->setMessageId(newEntryId);

		int count = pluginInterface->attachmentCount();
		QVERIFY(count == 1);

		UniMessageInfoList attachmentlist = pluginInterface->attachmentList();
		QVERIFY(!attachmentlist.isEmpty());

		UniMessageInfo* att = attachmentlist.at(0);
		QString path = att->path();

		QString recepient(TEST_MSG_FROM1);
		QString fromaddress;
		pluginInterface->fromAddress(fromaddress);
		QVERIFY(recepient == fromaddress);

		QString body;
		pluginInterface->body(body);
		QVERIFY(!body.isEmpty());

		// Compare input message , with the read message
		RFile origFile;
		int op = origFile.Open(iFs, currentFile, EFileRead);
		QVERIFY(op == 0);
    
		TBuf8<1000> readBuf;
		HBufC* readBuf16 = HBufC::NewL(2000);
    
		origFile.Read(readBuf);
		readBuf16->Des().Copy(readBuf);

		QString orig = XQConversions::s60DescToQString(*readBuf16);
		origFile.Close();

		HBufC* filepath = XQConversions::qStringToS60Desc(path);

		int size = pluginInterface->messageSize();

		MsgPriority priority = pluginInterface->messagePriority();
    	// Normal priority
		qDebug() << "prority" << priority;
		QVERIFY(priority == 1);

		QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

		ConvergedMessageAddressList recipientList;
		pluginInterface->toRecipientList(recipientList);
		QVERIFY(recipientList.count() == 0);
		qDebug() << "recipientList.count" << recipientList.count();
    
		if (inbox)
           {
               iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
               qDebug() << "SetEntry" ;
           }

    
}

void TestUniDataModelVCalPlugin::testDraftsVCalMessage()
{
		drafts = ETrue;
		GenerateMessagesL();

		pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
		pluginInterface->setMessageId(newEntryId);

		int count = pluginInterface->attachmentCount();
		QVERIFY(count == 1);

		UniMessageInfoList attachmentlist = pluginInterface->attachmentList();
		QVERIFY(!attachmentlist.isEmpty());

		UniMessageInfo* att = attachmentlist.at(0);
		QString path = att->path();

		QString fromaddress;
		pluginInterface->fromAddress(fromaddress);
		QVERIFY(!fromaddress.isEmpty()); //added ! shweta

		QString body;
		pluginInterface->body(body);
		QVERIFY(!body.isEmpty());

		// Compare input message , with the read message
		RFile origFile;
		int op = origFile.Open(iFs, currentFile, EFileRead);
		QVERIFY(op == 0);
    
		TBuf8<1000> readBuf;
		HBufC* readBuf16 = HBufC::NewL(2000);

		origFile.Read(readBuf);
		readBuf16->Des().Copy(readBuf);

		QString orig = XQConversions::s60DescToQString(*readBuf16);
		origFile.Close();
    
		int size = pluginInterface->messageSize();

		MsgPriority priority = pluginInterface->messagePriority();
		// Normal priority
		qDebug() << "prority" << priority;
		QVERIFY(priority == 1);

		QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

        QString recepient(TEST_MSG_FROM1);
        ConvergedMessageAddressList recipientList;
        ConvergedMessageAddress alias(recepient);
      
        recipientList.append(&alias);
        pluginInterface->toRecipientList(recipientList);
        
        QVERIFY( recipientList.count() != 0 );
        qDebug() << "recipientList.count()"<<recipientList.count() ;
        
        const QString& copy = recipientList[0]->address();
        qDebug() << "copy = "<<recipientList[0]->address();
        QVERIFY(recepient == copy);
        qDebug() << "Receipient Verified";

        if (drafts)
               {
                   iMsvEntry->SetEntryL(KMsvDraftEntryId);
                   qDebug() << "SetEntry" ;
               }
  }



void TestUniDataModelVCalPlugin::testUnusedAPIs()
{
		pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);

		QVERIFY(pluginInterface->objectCount() == 0);

		UniMessageInfoList objectlist = pluginInterface->objectList();
		QVERIFY(objectlist.isEmpty());

		QVERIFY(pluginInterface->slideCount() == 0);

		UniMessageInfoList slidelist = pluginInterface->slideContent(0);
		QVERIFY(slidelist.isEmpty());
    
		QVERIFY(pluginInterface->hasAttachment() == true );
    
		ConvergedMessageAddressList recipientList;
    
		pluginInterface->ccRecipientList(recipientList);
		QVERIFY(recipientList.count() == 0);
		
		pluginInterface->bccRecipientList(recipientList);
		QVERIFY(recipientList.count() == 0);
}

CMsvEntrySelection* TestUniDataModelVCalPlugin::GenerateMessagesL()
{
		HBufC* aFileDirectory = KBIOTxtFilePath().AllocL();
		TMsvId messageId;
		TBIOMessageType currentMsgType;

		CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
		CleanupStack::PushL(selection);

		TInt err = iFs.GetDir(aFileDirectory->Des(),
                          KEntryAttMatchMask,
                          ESortByName,
                          iDir);
		if (err == KErrPathNotFound)
			{
				TInt makeDirErr = iFs.MkDirAll(aFileDirectory->Des());
				makeDirErr == KErrNone ? User::Leave(KErrNotFound)
                : User::Leave(makeDirErr);
			}
		else if (err != KErrNone)
			{
				User::Leave(err);
			}

		// Set the session path for the RFs
		SetSessionPath(aFileDirectory->Des());
		if (iDir)
			{
				delete iDir;
				iDir = NULL;
			}
        
		User::LeaveIfError(iFs.GetDir(_L("*.txt"), KEntryAttNormal, ESortByName, iDir));
		TInt count = iDir->Count();
		if (count == 0)
			{
				User::Leave(KErrNotFound); // No files to process
			}

		TBuf<60> outputBuf;

		for (TInt loop = 0; loop < count; loop++)
			{
				TEntry as = (*iDir)[0];
				currentFile = (as.iName);
				// Not processed all the messages - so keep the current state
				iFilesProcessed++; // Here because need to update the counter promptly
				currentMsgType = SetMessageType(currentFile);
				if (currentMsgType != ENoMessage) // skip any dodgy filenames
					{
						messageId = CreateBIOEntryFromFileL(currentFile, currentMsgType);
						selection->AppendL(messageId);
						TPtrC tempPtr = (currentFile.Des());

					}
			}
		CleanupStack::Pop(); // selection
		
		return selection;
}

TBIOMessageType TestUniDataModelVCalPlugin::SetMessageType(
                                                           const TDesC& aFileName)
{

	if (aFileName.MatchF(KBIOvCalenderPrefix) == 0)
		{
			return EBiovCalenderMessage;
		}

    return ENoMessage;
}

TMsvId TestUniDataModelVCalPlugin::CreateBIOEntryFromFileL(
                                                           const TDesC& aFilename,
                                                           TBIOMessageType aMessageType)
{
		//First use the filename to get the message body, 
		// then convert '\n' to the paragraph delimiters used in proper SMS 
		HBufC* tempBuffer;

		tempBuffer = ReadFromFileL(aFilename);
		CleanupStack::PushL(tempBuffer);

		newEntryId = CreateBIOEntryL(*tempBuffer, aMessageType);

		// Finished with our local descriptors - free up some memory
		CleanupStack::PopAndDestroy();
    
		return newEntryId;
}

void TestUniDataModelVCalPlugin::SetSessionPath(const TDesC& aSessionPath)
{
		iFs.SetSessionPath(aSessionPath);
}

HBufC* TestUniDataModelVCalPlugin::ReadFromFileL(const TDesC& aFile)
{
		RFile file;
		TBuf8<1024> lineBuffer;
		TInt err = KErrNone;

		err = file.Open(iFs, aFile, EFileStreamText | EFileRead | EFileShareAny);

		if (err != KErrNone) // Didn't find the file, so leave - should only get valid filenames!
			{
				User::Leave(KErrNotFound);
			}

		HBufC* bioBuf = HBufC::NewLC(65535); // Create a new descriptor on the heap.
		HBufC* copyBuffer = HBufC::NewLC(1024);

		do // Read in the text from file, and also check if there is a name field:
			{
				err = file.Read(lineBuffer);// Read upto 256 chars, '\n' and all...
				//err = ReadLineL(file,lineBuffer);
				if (err == KErrNone) // Made a valid read,
					if (lineBuffer.Length() == 0) // but read 0 chars
                err = KErrEof; // so set err value to end processing

				if (err == KErrNone)
					{	
						copyBuffer->Des().Copy(lineBuffer);// Copy, and overwrite existing text
						if ( (bioBuf->Length() + copyBuffer->Length())
								> bioBuf->Des().MaxLength())
							{
								bioBuf = bioBuf->ReAllocL(bioBuf->Length()
										+ copyBuffer->Length());
							}
						bioBuf->Des().Append(*copyBuffer);
						//bioBuf->Des().Append(_L("\n"));
					}
    }while (err != KErrEof);

    CleanupStack::PopAndDestroy(); // Destroy the copyBuffer.
    CleanupStack::Pop();// Remove the bioBuf.

    file.Close();
    return bioBuf;
}

TMsvId TestUniDataModelVCalPlugin::CreateBIOEntryL(TDesC& aText,
                                                   TBIOMessageType aMessageType)
{
		// Ensure that we have a valid service ID to work with:
		TMsvId iBioServiceId;

		iBioServiceId = SetBIOServiceIdL();

		HBufC* localBuffer = aText.AllocL();
		CleanupStack::PushL(localBuffer);

		TPtr messDes = localBuffer->Des();

		if (aMessageType != EBiovCardMessage && aMessageType
            != EBiovCalenderMessage)
			{
				// convert \r\n to \n since this is what is expected from SMS when not vCard data
				for (TInt i = 0; i < messDes.Length(); i++)
					{
						if (messDes[i] == (TText) '\r' && i < messDes.Length() - 1
								&& messDes[i + 1] == (TText) '\n')
							messDes.Delete(i, 1);
					}
			}

		//  Create and fill a CRichText object for the jobbie:
		CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
		CleanupStack::PushL(paraFormatLayer);
		CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
		CleanupStack::PushL(charFormatLayer);
		CRichText* richText = CRichText::NewL(paraFormatLayer, charFormatLayer);
		CleanupStack::PushL(richText);

		TInt pos = richText->DocumentLength();
		richText->InsertL(pos, messDes);

		TMsvEntry newBioEntry;
		newBioEntry.SetNew(ETrue);
		newBioEntry.SetComplete(EFalse);
		newBioEntry.SetUnread(ETrue);
		newBioEntry.SetVisible(ETrue);
		newBioEntry.SetReadOnly(EFalse);
		newBioEntry.SetFailed(EFalse);
		newBioEntry.SetOperation(EFalse);
		newBioEntry.SetMultipleRecipients(EFalse);
		newBioEntry.SetAttachment(EFalse);
		newBioEntry.iMtm = KUidBIOMessageTypeMtm;
		newBioEntry.iType = KUidMsvMessageEntry;
		newBioEntry.iServiceId = iBioServiceId;
		TTime now;
		now.UniversalTime();
		newBioEntry.iDate = now;

		TTime unixEpoch(KUnixEpoch);
		TTimeIntervalSeconds seconds;
		TTime timeStamp = newBioEntry.iDate;
		timeStamp.SecondsFrom(unixEpoch, seconds);
		retTimeStamp.setTime_t(seconds.Int());

		newBioEntry.iDescription.Set(richText->Read(0, richText->DocumentLength()));
		TBufC<KTelephoneNumberMaxLength> telNumber;

		QString recepient(TEST_MSG_FROM1);
		tempNumber = XQConversions::qStringToS60Desc(recepient);

		telNumber = tempNumber->Des();
		newBioEntry.iDetails.Set(telNumber);

		SetForMtmTypeL(newBioEntry, aMessageType);

		newBioEntry.iSize = richText->DocumentLength();// msgSize;
		CreateBioEntryClientSideL(newBioEntry, *richText);

		CleanupStack::PopAndDestroy(4); // richText, charFormatLayer, paraFormatLayer, localBuffer
		return newBioEntry.Id();
}

void TestUniDataModelVCalPlugin::CreateBioEntryClientSideL(TMsvEntry& aEntry,
                                                           CRichText& aBody)
{
		if (inbox)
			{
				iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
			}
		else if (drafts)
			{
				iMsvEntry->SetEntryL(KMsvDraftEntryId);
			}
		//  Get the global inbox.   

		iMsvEntry->CreateL(aEntry);
		iMsvEntry->SetEntryL(aEntry.Id());

		// Save all the changes
		CMsvStore* store = iMsvEntry->EditStoreL();
		CleanupStack::PushL(store);

		if (store->HasBodyTextL())
			{
				store->DeleteBodyTextL();
			}

		CPlainText* pText = CPlainText::NewL();
		CleanupStack::PushL(pText);

		if (inbox)
			{
				CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *pText);
				CleanupStack::PushL(smsHeader);
				smsHeader->SetFromAddressL(*tempNumber);
				smsHeader->StoreL(*store);
				
			}
		else if (drafts)
			{

			QString recepient(TEST_MSG_FROM1);
			QString recepient2(TEST_MSG_FROM2);
			QString alias(TEST_MSG_ALIAS1);

			HBufC* addr = XQConversions::qStringToS60Desc(recepient);
			HBufC* addr2 = XQConversions::qStringToS60Desc(recepient2);
			HBufC* alias1 = XQConversions::qStringToS60Desc(alias);

			CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *pText);
			CleanupStack::PushL(smsHeader);

			CSmsNumber* number1 = CSmsNumber::NewL(); // create the instance
			CleanupStack::PushL(number1);

			number1->SetNameL(TPtrC());
			number1->SetAddressL(*addr);
			smsHeader->Recipients().AppendL(number1);
			CleanupStack::Pop(number1);

			CSmsNumber* number2 = CSmsNumber::NewL();
			CleanupStack::PushL(number2);

			number2->SetNameL(*alias1);
			number2->SetAddressL(*addr2);

			smsHeader->Recipients().AppendL(number2);
			CleanupStack::Pop(number2);
			smsHeader->StoreL(*store);

			}

    store->StoreBodyTextL(aBody);
    store->CommitL();

    CleanupStack::PopAndDestroy(3); //store - close the store

    aEntry.SetComplete(ETrue);
    // Update the entry
    iMsvEntry->ChangeL(aEntry);
    iMsvEntry->SetEntryL(KMsvRootIndexEntryId);
}

TMsvId TestUniDataModelVCalPlugin::SetBIOServiceIdL()
{
		//  Haven't found an entry so create a BIO Message service:
		TMsvEntry bioServiceEntry;
		bioServiceEntry.iMtm = KUidBIOMessageTypeMtm;
		bioServiceEntry.iType = KUidMsvServiceEntry;
		bioServiceEntry.SetVisible(EFalse);
		bioServiceEntry.iDate.UniversalTime();
		bioServiceEntry.iDescription.Set(_L("BIO Message Service ")); // Is there such a thing?
		bioServiceEntry.iDetails.Set(_L("BIO Message Service"));

		TMsvId newBIOServiceId;

		iMsvEntry->SetEntryL(KMsvRootIndexEntryId);
		iMsvEntry->CreateL(bioServiceEntry); // Needs to be a child of the root!
		newBIOServiceId = bioServiceEntry.Id();

    return newBIOServiceId;
}

void TestUniDataModelVCalPlugin::SetForMtmTypeL(TMsvEntry& aEntry,
                                                TBIOMessageType aType)
{
		TInt index;

		aEntry.iBioType = KUidBIOVCalenderMsg;

		// Set other TMsvEntry fields if we have a BIO message type
		if (aType != ENoMessage)
			{
				aEntry.SetMtmData2(0);
				aEntry.SetMtmData3(0);
			}

		// set description from BIF file
		iBioDb->GetBioIndexWithMsgIDL(TUid::Uid(aEntry.iBioType), index);
		if (index == KErrNotFound)
			User::Leave(KErrNotFound);
		aEntry.iDescription.Set(iBioDb->BifReader(index).Description());
}

void MsgObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                      TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}
