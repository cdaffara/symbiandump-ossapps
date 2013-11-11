/*
#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:   Main test class for CS Message Plugin
#*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <msvsearchsortoperation.h>
#include <msvsearchsortquery.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <TXTRICH.H>
#include <SMUTHDR.H>
#include <smut.h>
#include <msvstd.h>
#include <rsendas.h>
#include <rsendasmessage.h>
#include <ecom\ECom.h>
#include "test_plugin.h"
#include "CCsPlugin.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctest_plugin::Delete
//  
// -----------------------------------------------------------------------------
//
void Ctest_plugin::Delete() 
    {          
	iResultConversationEntryList.ResetAndDestroy();
	iResultConversationEntryList.Close();

	 iTestConversationEntryList.ResetAndDestroy();
	 iTestConversationEntryList.Close();

	 if( iPlugin )
		 {
			 delete iPlugin;
			 iPlugin = NULL;
		 }
	 
	 if (iSession)
		 {
			 delete iSession;
			 iSession = NULL;
		 }
	}

// -----------------------------------------------------------------------------
// Ctest_plugin::RunMethodL
// 
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::RunMethodL( CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        		
        ENTRY( "Example", Ctest_plugin::ExampleL ),
        ENTRY( "CacheFromCcsPluginL", Ctest_plugin::CacheFromCcsPluginL ),
        ENTRY( "CreateCCsMsgL", Ctest_plugin::CreateCCsMsgL ),		
        
        ENTRY( "CreateCcsPluginL", Ctest_plugin::CreateCcsPluginL ),
        ENTRY( "SendCcsMessageL", Ctest_plugin::SendCcsMessageL ),
        ENTRY( "SmsMsgPluginValidationL", Ctest_plugin::SmsMsgPluginValidationL),
        ENTRY( "MmsMsgPluginValidationL",Ctest_plugin::MmsMsgPluginValidationL),
        ENTRY( "BioMsgPluginValidationL",Ctest_plugin::BioMsgPluginValidationL)
              
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// Ctestsmsplugin::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::ExampleL( CStifItemParser& aItem )
    {
	
    // Print to UI
    _LIT( Ktestplugin, "testplugin" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Ktestplugin, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Ktestplugin,
                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Ctest_plugin::CreateCCsMsgL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::CreateCCsMsgL( CStifItemParser& /*aItem*/ )
	{
	
	// Print to UI
    _LIT( Ktestplugin, "CreateCCsMsgL" );
    _LIT( KFunction, "In CreateCCsMsgL" );
    TestModuleIf().Printf( 0, Ktestplugin, KFunction );
	// Print to log file
	iLog->Log( KFunction );
	iPlugin = CCsMsg::NewL(this);
	iPlugin->GetConversationsL();
	
    return KErrNone;
 	 
	} 
 
// -----------------------------------------------------------------------------
// Ctest_plugin::CacheFromCcsPluginL
// Creates the ccs plugin and initiates the caching
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::CacheFromCcsPluginL( CStifItemParser& /*aItem*/ )
    {
	// Print to UI
	_LIT( Ktestplugin, "CacheFromCcsPluginL" );
	_LIT( KFunction, "In CacheFromCcsPluginL" );
	TestModuleIf().Printf( 0, Ktestplugin, KFunction );
		// Print to log file
	iLog->Log( KFunction );

    iTestConversationEntryList.ResetAndDestroy();
    iResultConversationEntryList.ResetAndDestroy();
    
    iPlugin = CCsMsg::NewL(this);
    iPlugin->GetConversationsL();
     
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::CreateCcsPluginL
// Creates ccs plugin.
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::CreateCcsPluginL( CStifItemParser& /*aItem*/ )
    {
	// Print to UI
	_LIT( Ktestplugin, "CreateCcsPluginL" );
	_LIT( KFunction, "In CreateCcsPluginL" );
	TestModuleIf().Printf( 0, Ktestplugin, KFunction );
			// Print to log file
	iLog->Log( KFunction );

    iTestConversationEntryList.ResetAndDestroy();
    iResultConversationEntryList.ResetAndDestroy();
    
    iPlugin = CCsMsg::NewL(this);

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctestsmsplugin::SendCcsMessageL
// Send ccs message using RSendAs api
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::SendCcsMessageL( CStifItemParser& /*aItem*/ )
    {
	// Print to UI
	_LIT( Ktestplugin, "SendCcsMessageL" );
	_LIT( KFunction, "In SendCcsMessageL" );
	TestModuleIf().Printf( 0, Ktestplugin, KFunction );
	// Print to log file
	iLog->Log( KFunction );

    iTestConversationEntryList.ResetAndDestroy();
    iResultConversationEntryList.ResetAndDestroy();

    RSendAs sendas;
    CleanupClosePushL(sendas);
    TInt res = sendas.Connect();
    User::LeaveIfError(res);
    
    RSendAsMessage message;
    CleanupClosePushL(message);
    TRAP(res, message.CreateL(sendas, KUidMsgTypeSMS));
    User::LeaveIfError(res);

    TBuf<512> MsgText;
    MsgText.Copy(_L("Message being sent"));
    message.SetBodyTextL(MsgText);
    TBuf<15> RecAdd;
    RecAdd.Copy(_L("123456789"));
    message.AddRecipientL(RecAdd,RSendAsMessage::ESendAsRecipientTo );


    message.SendMessageAndCloseL();
    CleanupStack::Pop(1); // RMessage
    CleanupStack::PopAndDestroy(1); // RSendAs

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ctest_plugin::CreateMmsMessageL
// Creates messages in the different folders and populates the
// iTestConversationEntryList
// -----------------------------------------------------------------------------
//
void Ctest_plugin::CreateMmsMessageL(const TMsvId aFolderId,
        TBool aUnReadFlag)
	{
		
	// create a new object to access an existing entry
	CMsvEntry* msvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msvEntry);
	
	TMsvId defaultServiceId = 0;
	defaultServiceId = iMmsClientMtm->DefaultServiceL();
	
	_LIT( KMMSBody, " Hi !!!");
	
	CleanupStack::Pop(msvEntry);
	iMmsClientMtm->SetCurrentEntryL(msvEntry);
	iMmsClientMtm->CreateMessageL(defaultServiceId);
	iMmsClientMtm->SetSubjectL(KMMSBody);
	
	// set some mms defaults
	iMmsClientMtm->SetMessageClass(EMmsClassPersonal);
	iMmsClientMtm->SetExpiryInterval(86400);
	iMmsClientMtm->SetDeliveryTimeInterval(3600);
	iMmsClientMtm->SetMessagePriority(EMmsPriorityLow);
	iMmsClientMtm->SetSenderVisibility(EMmsMaximumSenderVisibility);
	iMmsClientMtm->SetDeliveryReport(EMmsDeliveryReportYes);
	iMmsClientMtm->SetReadReply(EMmsReadReplyYes);        
	
	_LIT(KPhoneNumberString,"234567");
	TBuf<15> phoneNumberString;
	phoneNumberString.Copy(KPhoneNumberString);
	_LIT(KPhoneNumberString1,"mine<98450>");
	_LIT(KResultPhoneNumberString,"98450");
	
	if (aFolderId == KMsvGlobalInBoxIndexEntryId)
		{
	iMmsClientMtm->SetSenderL(KResultPhoneNumberString);
		}
	
	iMmsClientMtm->AddAddresseeL(KResultPhoneNumberString);
	
	CMsvStore* store = msvEntry->EditStoreL();
	CleanupStack::PushL(store);
	
	// set body as an attachment
	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
	CleanupStack::PushL(mimeHeaders);
	mimeHeaders->SetContentTypeL( _L8("text") );
	mimeHeaders->SetContentSubTypeL( _L8("plain") );
	mimeHeaders->SetMimeCharset( KMmsUtf8 );
	mimeHeaders->SetSuggestedFilenameL( _L("body.txt") );    
	         
	MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
	CleanupStack::PushL(attachmentInfo);
	attachmentInfo->SetAttachmentNameL( _L("body.txt") );
	attachmentInfo->SetMimeTypeL( _L8("text/plain") );
	mimeHeaders->StoreL( *attachmentInfo );
	    
	RFile textFile;

	CMsvOperationActiveSchedulerWait* waiter = CMsvOperationActiveSchedulerWait::NewLC();
	attachMan.CreateAttachmentL(_L("body.txt"), textFile, attachmentInfo, waiter->iStatus );
	CleanupStack::Pop(); // waiter
	CleanupStack::Pop(); // attachmentInfo
	CleanupStack::PushL(waiter);
	waiter->Start();
	CleanupStack::PopAndDestroy(waiter);    
	
	HBufC8* tempBuf = HBufC8::NewLC( 10 );
	tempBuf->Des().Copy( KMMSBody );
	textFile.Write( tempBuf->Des() );        
	textFile.Close();
	CleanupStack::PopAndDestroy(); //tempBuf
	    
	CleanupStack::PopAndDestroy(); //mimeHeaders

	// get the entry of the message
	 TMsvEntry messageEntry = iMmsClientMtm->Entry().Entry();        
	
	 // set the description field same as the message subject
	 messageEntry.iDescription.Set( KMMSBody );

	// save the changes done above
	iMmsClientMtm->Entry().ChangeL(messageEntry); 
	
	TFileName attachmentFile( _L("c:\\pictures\\picture123.txt") );	
	HandleAttachementL(store,attachmentFile );
	
	// save the changes made to the message store
	store->CommitL();
	CleanupStack::PopAndDestroy(store);   
	               
	// save the message      
	iMmsClientMtm->SaveMessageL();
	   
	   // final fine tuning
	messageEntry.SetAttachment(ETrue);
	messageEntry.iDate.HomeTime();
	messageEntry.SetVisible(ETrue);
	messageEntry.SetInPreparation(EFalse);
	
	if( aUnReadFlag )
      {
		  messageEntry.SetUnread(ETrue);
	      messageEntry.SetNew(ETrue);
	   }
	  else
	  {
		  messageEntry.SetUnread(EFalse);
		  messageEntry.SetNew(EFalse);
	  }
	  messageEntry.SetComplete(ETrue);
	  messageEntry.iServiceId = defaultServiceId;
	  messageEntry.iRelatedId = 0;
	      
	  if (aFolderId == KMsvDraftEntryId) 
	          messageEntry.SetReadOnly(EFalse);
	  else
	          messageEntry.SetReadOnly(ETrue);
     
	  messageEntry.iMtmData1 = KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;
	  	
	  iMmsClientMtm->Entry().ChangeL(messageEntry);
	      
	  }

void Ctest_plugin:: HandleAttachementL(CMsvStore* store,TFileName& sourceFileName )
	{
    
	// attachment settings
	MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
	CleanupStack::PushL(attachmentInfo);
	attachmentInfo->SetAttachmentNameL( sourceFileName);        
	attachmentInfo->SetMimeTypeL(_L8("text") );

	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL(mimeHeaders);
    mimeHeaders->SetContentTypeL( _L8("text") );
    mimeHeaders->SetContentSubTypeL( _L8("plain") );
    mimeHeaders->SetMimeCharset( KMmsUtf8 );
    mimeHeaders->StoreL( *attachmentInfo );
    CleanupStack::PopAndDestroy( mimeHeaders );
	 
	CMsvOperationActiveSchedulerWait* waiter = CMsvOperationActiveSchedulerWait::NewLC();
	TInt err;
	TRAP(err, attachMan.AddAttachmentL(sourceFileName, attachmentInfo, waiter->iStatus ));
	if (err == KErrNone)
	{
		 waiter->Start();
	     CleanupStack::PopAndDestroy(waiter);
	     CleanupStack::Pop(); // attachmentInfo, the ownership is transferred
	 }	
	 else
	 {
		 CleanupStack::PopAndDestroy(waiter);
	     CleanupStack::PopAndDestroy(); // attachmentInfo, ownership is not transferred (leave occurs)
	     _LIT( Ktestplugin, "HandleAttachment" );
	     _LIT( KFunction, "In HandleAttachment::Failed" );
	 }
		
	 }


// -----------------------------------------------------------------------------
// Ctest_plugin::CreateSmartMessageL
// Creates messages in the different folders and populates the
// iTestConversationEntryList
// -----------------------------------------------------------------------------
//
void Ctest_plugin::CreateSmartMessageL(const TMsvId aFolderId,TBool aUnReadFlag)
{

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);

    TMsvId defaultServiceId = 0;
    defaultServiceId = iSmsClientMtm->DefaultServiceL();

   // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    iSmsClientMtm->SetCurrentEntryL(msvEntry);    
    
    // create a new message
    iSmsClientMtm->CreateMessageL(defaultServiceId);
    
    // Add addressee
    _LIT(KPhoneNumberString,"234567");
    TBuf<15> phoneNumberString;
    phoneNumberString.Copy(KPhoneNumberString);

    _LIT(KPhoneNumberString1,"mine<98450>");
    _LIT(KResultPhoneNumberString,"98450");
    
    // set the from field to sms header
    if (aFolderId == KMsvGlobalInBoxIndexEntryId)
    {
		CSmsHeader& smsHeader = iSmsClientMtm->SmsHeader();
		smsHeader.SetFromAddressL(KResultPhoneNumberString);
    }
    
    // set body, the BIO message itself
    iSmsClientMtm->Body().Reset();
    iSmsClientMtm->Body().InsertL(0, _L("Business Card\nTester Mike\nThe Company Ltd.\nSoftware Engineer\ntel +358 66 1234567\n")); 

    // get the entry of the message
    TMsvEntry messageEntry = iSmsClientMtm->Entry().Entry();

    // TO-DO: Add support for all BIO messages, currently just insert a business card message ... :
    // set the message type
    // parameters.iBIOMessageType ...
    iSmsClientMtm->BioTypeChangedL(KMsgBioUidVCard);
    messageEntry.iBioType = KMsgBioUidVCard.iUid;
    
    if (aFolderId == KMsvGlobalInBoxIndexEntryId)
    {
        iMmsClientMtm->SetSenderL(KResultPhoneNumberString);
    }
    
    _LIT( KBioBody, " Hi !!!");
 
    messageEntry.iDescription.Set(KBioBody);
    
    // set correct MTM type
    messageEntry.iMtm= KUidBIOMessageTypeMtm;

   // save the changes done above
   iSmsClientMtm->Entry().ChangeL(messageEntry);

    // save the message     
   iSmsClientMtm->SaveMessageL();
        	
   // final fine tuning
   messageEntry.SetAttachment(EFalse);
   messageEntry.iDate.HomeTime();
   messageEntry.SetVisible(ETrue);
   messageEntry.SetInPreparation(EFalse);
   
   if( aUnReadFlag )
   	{
   		  messageEntry.SetUnread(ETrue);
   	      messageEntry.SetNew(ETrue);
   	 }
   else
   	 {
   		  messageEntry.SetUnread(EFalse);
   		  messageEntry.SetNew(EFalse);
   	  }

   messageEntry.SetComplete(ETrue);
   messageEntry.iServiceId = defaultServiceId;
   messageEntry.iRelatedId = 0;
   iSmsClientMtm->Entry().ChangeL(messageEntry);
  

}

// -----------------------------------------------------------------------------
// Ctest_plugin::CreateMessageL
// Creates messages in the different folders and populates the
// iTestConversationEntryList
// -----------------------------------------------------------------------------
//
void Ctest_plugin::CreateMessageL(const TMsvId aFolderId,
        TBool aUnReadFlag)
    {
    CCsConversationEntry *conversationEntry = CCsConversationEntry::NewL();
    CleanupStack::PushL(conversationEntry);

    //Create entry from this index entry
    iSmsClientMtm->SwitchCurrentEntryL( aFolderId );
    iSmsClientMtm->CreateMessageL(KUidMsgTypeSMS.iUid);

    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    //AddBody
    _LIT( KSMSBody, " Hi,Hello Phone Owner !!!");
    TBuf<160> shortDescription;
    shortDescription.Append(KSMSBody);
    CRichText& body = iSmsClientMtm->Body();
    body.Reset();
    body.InsertL( 0, KSMSBody );
    indexEntry.iDescription.Set(KSMSBody);

    // Add addressee
    _LIT(KPhoneNumberString,"234567");
    TBuf<15> phoneNumberString;
    phoneNumberString.Copy(KPhoneNumberString);

    _LIT(KPhoneNumberString1,"mine<98450>");
    _LIT(KResultPhoneNumberString,"98450");

    if( aFolderId == KMsvDraftEntryId )
        {
        iSmsClientMtm->AddAddresseeL(KPhoneNumberString,TPtrC());
        iSmsClientMtm->AddAddresseeL(KPhoneNumberString1,TPtrC());

        CCsConversationEntry *conversationEntry1 = CCsConversationEntry::NewL();
        conversationEntry->SetContactL(KPhoneNumberString);
        conversationEntry->SetEntryId( indexEntry.Id());
        conversationEntry->SetTimeStampL( indexEntry.iDate.Int64());
        conversationEntry->SetDescriptionL(shortDescription);
        conversationEntry->ChangeAttributes(ECsAttributeDraft, ECsAttributeNone);
        conversationEntry->SetConversationDir(ECsDirectionOutgoing);
        conversationEntry->SetSendState( ECsSendStateUnknown );
        conversationEntry->SetType( ECsSMS );

        conversationEntry1->SetContactL(KResultPhoneNumberString);
        conversationEntry1->SetEntryId( indexEntry.Id());
        conversationEntry1->SetTimeStampL( indexEntry.iDate.Int64());
        conversationEntry1->SetDescriptionL(shortDescription);
        conversationEntry1->ChangeAttributes(ECsAttributeDraft, ECsAttributeNone);
        conversationEntry1->SetConversationDir(ECsDirectionOutgoing);
        conversationEntry1->SetSendState( ECsSendStateUnknown );
        conversationEntry1->SetType( ECsSMS );

        CleanupStack::PushL(conversationEntry1);
        iTestConversationEntryList.AppendL(conversationEntry);
        iTestConversationEntryList.AppendL(conversationEntry1);
        CleanupStack::Pop(conversationEntry1);
        }
    else if( aFolderId == KMsvGlobalOutBoxIndexEntryId )
        {
        iSmsClientMtm->AddAddresseeL(KPhoneNumberString,TPtrC());
        indexEntry.SetSendingState( KMsvSendStateWaiting );

        conversationEntry->SetContactL(KPhoneNumberString);
        conversationEntry->SetEntryId( indexEntry.Id());
        conversationEntry->SetTimeStampL( indexEntry.iDate.Int64());
        conversationEntry->SetDescriptionL(shortDescription);
        conversationEntry->SetConversationDir(ECsDirectionOutgoing);
        conversationEntry->SetSendState( ECsSendStateWaiting );
        conversationEntry->SetType( ECsSMS );

        iTestConversationEntryList.AppendL(conversationEntry);
        }
    else if( aFolderId == KMsvSentEntryId )
        {
        CSmsHeader&  smsHeader = iSmsClientMtm->SmsHeader();
        smsHeader.SetFromAddressL(KPhoneNumberString);
        indexEntry.iDetails.Set( KPhoneNumberString );

        conversationEntry->SetContactL(KPhoneNumberString);
        conversationEntry->SetEntryId( indexEntry.Id());
        conversationEntry->SetTimeStampL( indexEntry.iDate.Int64());
        conversationEntry->SetDescriptionL(shortDescription);
        conversationEntry->SetConversationDir(ECsDirectionOutgoing);
        conversationEntry->SetSendState( ECsSendStateUnknown );
        conversationEntry->SetType( ECsSMS );

        iTestConversationEntryList.AppendL(conversationEntry);
        }
    // final fine tuning
    indexEntry.SetAttachment(EFalse);

    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    if ( aFolderId == KMsvGlobalInBoxIndexEntryId )
        {
        CSmsHeader&  smsHeader = iSmsClientMtm->SmsHeader();
        smsHeader.SetFromAddressL(KPhoneNumberString);
        indexEntry.iDetails.Set( KPhoneNumberString );

        if( aUnReadFlag )
            {
            indexEntry.SetUnread(ETrue);
            conversationEntry->SetSendState( ECsSendStateUnknown );
            conversationEntry->ChangeAttributes( ECsAttributeUnread, ECsAttributeNone );
            }
        else
            {
            conversationEntry->SetSendState( ECsSendStateUnknown );
            conversationEntry->ChangeAttributes( ECsAttributeNone, ECsAttributeUnread );
            }
        conversationEntry->SetContactL(KPhoneNumberString);
        conversationEntry->SetEntryId( indexEntry.Id());
        conversationEntry->SetTimeStampL( indexEntry.iDate.Int64());
        conversationEntry->SetDescriptionL(shortDescription);
        conversationEntry->SetConversationDir(ECsDirectionIncoming);
        conversationEntry->SetType( ECsSMS );

        iTestConversationEntryList.AppendL(conversationEntry);

        indexEntry.SetNew(ETrue);
        }

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

    if ( iTestConversationEntryList.Count() == 0 )
        {
    	CleanupStack::PopAndDestroy(conversationEntry);
        }
    else
        {
        CleanupStack::Pop(conversationEntry);
        }

    }


// -----------------------------------------------------------------------------
// Ctest_plugin::CacheFromCcsPluginValidationL
// Validates the initial Caching of sms plugin
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::SmsMsgPluginValidationL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT(Ktestplugin, "CacheFromCcsPluginValidationL" );
    _LIT( KFunction, "In CacheFromCcsPluginValidationL" );
    TestModuleIf().Printf( 0, Ktestplugin, KFunction );
    // Print to log file
    iLog->Log( KFunction );

    CleanAllFoldersL();
    CreateInitialCacheL();

    iPlugin = CCsMsg::NewL(this);
    iPlugin->GetConversationsL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::MmsPluginValidationL
// Validates the initial Caching of sms plugin
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::MmsMsgPluginValidationL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT(Ktestplugin, "MmsPluginValidationL" );
    _LIT( KFunction, "In MmsPluginValidationL" );
    TestModuleIf().Printf( 0, Ktestplugin, KFunction );
    // Print to log file
    iLog->Log( KFunction );

    CleanAllFoldersL();
   
    iTestConversationEntryList.ResetAndDestroy();
    iResultConversationEntryList.ResetAndDestroy();
    
    CreateMmsMessageL(KMsvGlobalInBoxIndexEntryId);
    CreateMmsMessageL(KMsvSentEntryId);
    CreateMmsMessageL(KMsvDraftEntryId);
    CreateMmsMessageL(KMsvGlobalOutBoxIndexEntryId);;

    iPlugin = CCsMsg::NewL(this);
    iPlugin->GetConversationsL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::BioMsgPluginValidationL
// Validates the initial Caching of sms plugin
// -----------------------------------------------------------------------------
//
TInt Ctest_plugin::BioMsgPluginValidationL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT(Ktestplugin, "BioMsgPluginValidationL" );
    _LIT( KFunction, "In BioMsgPluginValidationL" );
    TestModuleIf().Printf( 0, Ktestplugin, KFunction );
    // Print to log file
    iLog->Log( KFunction );

    CleanAllFoldersL();
   
    iTestConversationEntryList.ResetAndDestroy();
    iResultConversationEntryList.ResetAndDestroy();
    
    CreateSmartMessageL(KMsvGlobalOutBoxIndexEntryId);
    CreateSmartMessageL(KMsvSentEntryId);
    CreateSmartMessageL(KMsvDraftEntryId);
    CreateSmartMessageL(KMsvGlobalOutBoxIndexEntryId);
    
    iPlugin = CCsMsg::NewL(this);
    iPlugin->GetConversationsL();

    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::CleanAllFoldersL
// Cleans messages from all folders
// -----------------------------------------------------------------------------
void Ctest_plugin::CleanAllFoldersL()
    {
    CleanMessagesInFolderL(KMsvGlobalInBoxIndexEntryId);
    CleanMessagesInFolderL(KMsvGlobalOutBoxIndexEntryId);
    CleanMessagesInFolderL(KMsvDraftEntryId);
 // CleanMessagesInFolderL(KMsvSentEntryId);
    }
// -----------------------------------------------------------------------------
// Ctest_plugin::CreateInitialCacheL
// Populates the messages in different folders which are used
// to verify the caching logic of ccs plugin
// -----------------------------------------------------------------------------
void Ctest_plugin::CreateInitialCacheL()
    {
	
	iTestConversationEntryList.ResetAndDestroy();
    iResultConversationEntryList.ResetAndDestroy();
    
    CreateMessageL(KMsvGlobalInBoxIndexEntryId);
    CreateMessageL(KMsvSentEntryId);
    CreateMessageL(KMsvDraftEntryId);
    CreateMessageL(KMsvGlobalOutBoxIndexEntryId);
       
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::CleanMessagesInFolderL
// Cleans messages from the specified folder
// -----------------------------------------------------------------------------
void Ctest_plugin::CleanMessagesInFolderL(TMsvId aBoxId,TBool aDeleteOne)
    {
    iLog->Log(_L("In CleanMessagesInFolderL"));
    TInt count;
    CMsvEntry* cEntry = NULL;
    // delete messages from the specified box
    cEntry = iSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(aBoxId);
    // show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeSMS);
    CleanupStack::PushL(msvEntrySelection);

    if( aDeleteOne )
        {
        count =1;
        }
    else
        {
        count = msvEntrySelection->Count();
        }

    for ( int i = 0; i < count ; i++ )
        {
        CMsvEntry* entry = iSession->GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly(EFalse);
        entry->ChangeL(tEntry);
        cEntry->DeleteL( msvEntrySelection->At(i) );
        CleanupStack::PopAndDestroy( entry );
        }

    CleanupStack::PopAndDestroy(msvEntrySelection);
    CleanupStack::PopAndDestroy(cEntry);
    iLog->Log(_L("End of CleanMessagesInFolderL"));
    }




// -----------------------------------------------------------------------------
// Ctest_plugin::CachingCompleted.
// Caching error event from Plugins
// -----------------------------------------------------------------------------
//
void Ctest_plugin::CachingError(const TInt /*aError*/)
    {
    Signal(KErrGeneral);
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::CachingCompleted.
// Caching complete event from Plugins
// -----------------------------------------------------------------------------
//
void Ctest_plugin::CachingCompleted()
    {
    if( iTestConversationEntryList.Count() == 0 )
        {
        Signal(KErrNone);
        return;
        }
    if( iResultConversationEntryList.Count() != iTestConversationEntryList.Count() )
        {
        // Delete();
        Signal(KErrCancel);
        }
    else
        {
        TInt i;
        for( i=0; i < iResultConversationEntryList.Count() ; i++ )
            {
            if( Compare(iResultConversationEntryList[i],iTestConversationEntryList[i] )!=0 )
                {
                break;
                }
            }
        if( i == iResultConversationEntryList.Count() )
            {
            iTestConversationEntryList.ResetAndDestroy();
            iResultConversationEntryList.ResetAndDestroy();
            Signal(KErrNone);
            }
        else
            {
            Signal(KErrNotFound);
            }
        }
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::Compare
// Compares the conversation entry stored conversation entry with
// the conversation entry returned from sms plugin
// -----------------------------------------------------------------------------
TInt Ctest_plugin::Compare(CCsConversationEntry *aResult, CCsConversationEntry *aTest)
    {
    TInt comparison = 0;

    if( aResult->Contact()->Des().Compare(aTest->Contact()->Des()) )
        comparison++;

    if( aResult->EntryId() != aTest->EntryId() )
        comparison ++;

    if( aResult->TimeStamp() != aTest->TimeStamp() )
        comparison++;

    if( aResult->ConversationDir() != aTest->ConversationDir() )
        comparison++;

    if( aResult->GetSendState() != aTest->GetSendState() )
        comparison++;

    if( aResult->GetType() != aTest->GetType() )
        comparison++;

    return comparison;
    }


// -----------------------------------------------------------------------------
// Ctest_plugin::AddConversations.
// Handle Conversation event from Plugins
// -----------------------------------------------------------------------------
//
void Ctest_plugin::AddConversations(
        const RPointerArray<CCsConversationEntry>& aConversationEntryLists)
    {
    CCsConversationEntry *conversationEntry ;
   
    for ( TInt i=0; i < aConversationEntryLists.Count(); i++ )
        {
        TRAP_IGNORE(
                conversationEntry = aConversationEntryLists[i]->CloneL();
                iResultConversationEntryList.AppendL(conversationEntry);
        );
        }

    }

// -----------------------------------------------------------------------------
// Ctest_plugin::ModifyConversations.
// Update Conversation event from Plugins
// -----------------------------------------------------------------------------
//
void Ctest_plugin::ModifyConversations(
        const RPointerArray<CCsConversationEntry>& aConversationEntryLists)
    {
    if( iTestConversationEntryList.Count() == 0 )
        {
        Signal(KErrNone);
        return;
        }
    //Expecting update for one entry at a time
    TInt i;
    for( i=0; i < aConversationEntryLists.Count() ; i++ )
        {
        if( Compare(aConversationEntryLists[i],iTestConversationEntryList[i] )!=0 )
            {
            break;
            }
        }
    if( i == aConversationEntryLists.Count() )
        {
        iTestConversationEntryList.ResetAndDestroy();
        iResultConversationEntryList.ResetAndDestroy();
        Signal(KErrNone);
        }
    else
        {
        Signal(KErrNotFound);
        }
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::DeleteConversations.
// Delete Conversation event from Plugins
// -----------------------------------------------------------------------------
//

void Ctest_plugin::DeleteConversations(
        const RPointerArray<CCsConversationEntry>& aConversationEntryLists)
    {
    if( iTestConversationEntryList.Count() == 0 )
        {
        Signal(KErrNone);
        return;
        }
    //Expecting update for one entry at a time
    TInt i;
    for( i=0; i < aConversationEntryLists.Count() ; i++ )
        {
        if( aConversationEntryLists[i]->EntryId() != iTestConversationEntryList[i]->EntryId() )
            {
            break;
            }
        }
    if( i == aConversationEntryLists.Count() )
        {
        iTestConversationEntryList.ResetAndDestroy();
        iResultConversationEntryList.ResetAndDestroy();
        Signal(KErrNone);
        }
    else
        {
        Signal(KErrNotFound);
        }
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::RefreshConversations
// Notify server to refresh conversations.
// -----------------------------------------------------------------------------
//
void Ctest_plugin::RefreshConversations()
    {
	
    }
