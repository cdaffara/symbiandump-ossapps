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
# Description:   Main test class for CS Preview Plugin
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
#include "test_pluginpreview.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::Delete
//  
// -----------------------------------------------------------------------------
//
void Ctest_pluginpreview::Delete() 
    {          
	
	
    }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::RunMethodL
// 
// -----------------------------------------------------------------------------
//
TInt Ctest_pluginpreview::RunMethodL( CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        		
        ENTRY( "Example", Ctest_pluginpreview::ExampleL ),
        ENTRY( "CreateCCsPreviewPluginL",Ctest_pluginpreview::CreateCCsPreviewPluginL),
	    ENTRY( "MmsMsgPluginValidationL", Ctest_pluginpreview::MmsMsgPluginValidationL),
	    ENTRY( "SendMmsMessageL", Ctest_pluginpreview::SendMmsMessageL)
              
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
TInt Ctest_pluginpreview::ExampleL( CStifItemParser& aItem )
    {
	
    // Print to UI
    _LIT( Ktestplugin, "testpluginpreview" );
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
// Ctest_pluginpreview::CreateCCsPreviewPluginL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctest_pluginpreview::CreateCCsPreviewPluginL( CStifItemParser& /*aItem*/ )
	{
	
	 // Print to UI
 	 _LIT( Ktestplugin, "CreateCCsPreviewPlugin" );
   	 _LIT( KFunction, "In CreateCCsPreviewPlugin" );
     TestModuleIf().Printf( 0, Ktestplugin, KFunction );
	 // Print to log file
	 iLog->Log( KFunction );
	 iPlugin = CCsPreviewPlugin::NewL(this);
	 iPlugin->GetConversationsL();
     return KErrNone;
 	 
	}


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::ThumbnailPreviewReady()
// callback
// -----------------------------------------------------------------------------
//
void Ctest_pluginpreview::ThumbnailPreviewReady(MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/)
	{

	}


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::ThumbnailReady()
// 
// -----------------------------------------------------------------------------
//
void Ctest_pluginpreview::ThumbnailReady(TInt aError,MThumbnailData& aThumbnail, TThumbnailRequestId aId)
	{
	    
	}


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::RestoreReady()
// 
// -----------------------------------------------------------------------------
//
void Ctest_pluginpreview::RestoreReady(TInt /*aParseResult*/, TInt /*aError*/)
	{

	}


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::HandleCachingError
// Caching error event from Plugins
// -----------------------------------------------------------------------------
//

void Ctest_pluginpreview::HandleCachingError(const TInt /*aError*/)
	{
    		Signal(KErrGeneral);
    }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::HandleCachingCompleted
// Caching complete event from Plugins
// -----------------------------------------------------------------------------
//

void Ctest_pluginpreview::HandleCachingCompleted()
       {
   
	
   
       }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::MmsMsgPluginValidationL
// Validates the initial Caching of sms plugin
// -----------------------------------------------------------------------------
//
TInt Ctest_pluginpreview::MmsMsgPluginValidationL( CStifItemParser& /*aItem*/ )
    {
	// Print to UI
	_LIT(Ktestplugin, "MmsPluginValidationL" );
	_LIT( KFunction, "In MmsPluginValidationL" );
	TestModuleIf().Printf( 0, Ktestplugin, KFunction );
	// Print to log file
	iLog->Log( KFunction );

	CleanAllFoldersL();
    CreateMmsMessageL(KMsvGlobalInBoxIndexEntryId);
    CreateMmsMessageL(KMsvSentEntryId);
    CreateMmsMessageL(KMsvDraftEntryId);
    CreateMmsMessageL(KMsvGlobalOutBoxIndexEntryId);;

    iPlugin = CCsPreviewPlugin::NewL(this);
    iPlugin->GetConversationsL();

    return KErrNone;
    
    }


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::CleanAllFoldersL
// Cleans messages from all folders
// -----------------------------------------------------------------------------

void Ctest_pluginpreview::CleanAllFoldersL()
    {
	CleanMessagesInFolderL(KMsvGlobalInBoxIndexEntryId);
	CleanMessagesInFolderL(KMsvGlobalOutBoxIndexEntryId);
	CleanMessagesInFolderL(KMsvDraftEntryId);
 	// CleanMessagesInFolderL(KMsvSentEntryId);
    }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::CreateMmsMessageL
// Creates message 
// -----------------------------------------------------------------------------
//

void Ctest_pluginpreview::CreateMmsMessageL(const TMsvId aFolderId,TBool aUnReadFlag)
	{
		
	// create a new object to access an existing entry
	CMsvEntry* msvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(msvEntry);
	
	TMsvId defaultServiceId = 0;
	defaultServiceId = iMmsMtm->DefaultServiceL();
	
	_LIT( KMMSBody, " HELLO !!!");
	
	CleanupStack::Pop(msvEntry);
	iMmsMtm->SetCurrentEntryL(msvEntry);
	iMmsMtm->CreateMessageL(defaultServiceId);
	iMmsMtm->SetSubjectL(KMMSBody);
	
	// set some mms defaults
	iMmsMtm->SetMessageClass(EMmsClassPersonal);
	iMmsMtm->SetExpiryInterval(86400);
	iMmsMtm->SetDeliveryTimeInterval(3600);
	iMmsMtm->SetMessagePriority(EMmsPriorityLow);
	iMmsMtm->SetSenderVisibility(EMmsMaximumSenderVisibility);
	iMmsMtm->SetDeliveryReport(EMmsDeliveryReportYes);
	iMmsMtm->SetReadReply(EMmsReadReplyYes);        
	
	_LIT(KPhoneNumberString,"234567");
	TBuf<15> phoneNumberString;
	phoneNumberString.Copy(KPhoneNumberString);
	_LIT(KPhoneNumberString1,"mine<98450>");
	_LIT(KResultPhoneNumberString,"98450");
	
	if (aFolderId == KMsvGlobalInBoxIndexEntryId)
	{
		iMmsMtm->SetSenderL(KResultPhoneNumberString);
	}
	
	iMmsMtm->AddAddresseeL(KResultPhoneNumberString);
	
	CMsvStore* store = msvEntry->EditStoreL();
	CleanupStack::PushL(store);
	
	// set body as an attachment
	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
	CleanupStack::PushL(mimeHeaders);
	mimeHeaders->SetContentTypeL( _L8("image/jpg") );
	mimeHeaders->SetContentSubTypeL( _L8("image/jpg") );
	mimeHeaders->SetMimeCharset( KMmsUtf8 );
	mimeHeaders->SetSuggestedFilenameL( _L("body.jpg") );    
	         
	MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
	CleanupStack::PushL(attachmentInfo);
	attachmentInfo->SetAttachmentNameL( _L("body.txt") );
	attachmentInfo->SetMimeTypeL( _L8("image/jpg") );
	mimeHeaders->StoreL( *attachmentInfo );
	    
	RFile textFile;

	CMsvOperationActiveSchedulerWait* waiter = CMsvOperationActiveSchedulerWait::NewLC();
	attachMan.CreateAttachmentL(_L("body.jpg"), textFile, attachmentInfo, waiter->iStatus );
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
	 TMsvEntry messageEntry = iMmsMtm->Entry().Entry();        
	
	 // set the description field same as the message subject
	 messageEntry.iDescription.Set( KMMSBody );

	// save the changes done above
	iMmsMtm->Entry().ChangeL(messageEntry); 
	
	TFileName attachmentFile( _L("c:\\pictures\\picture123.jpg") );	
	HandleAttachementL(store,attachmentFile );
	
	// save the changes made to the message store
	store->CommitL();
	CleanupStack::PopAndDestroy(store);   
	               
	// save the message      
	iMmsMtm->SaveMessageL();
	   
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
	  	
	iMmsMtm->Entry().ChangeL(messageEntry);
	      
	}


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::CreateMmsMessageL
// Creates message 
// -----------------------------------------------------------------------------
//

void Ctest_pluginpreview:: HandleAttachementL(CMsvStore* store,TFileName& sourceFileName )
	{
    
	// attachment settings
	MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
	CleanupStack::PushL(attachmentInfo);
	attachmentInfo->SetAttachmentNameL( sourceFileName);        
	attachmentInfo->SetMimeTypeL(_L8("image/jpg") );

	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
	CleanupStack::PushL(mimeHeaders);
    mimeHeaders->SetContentTypeL( _L8("image/jpg") );
    mimeHeaders->SetContentSubTypeL( _L8("image/jpg") );
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
// Ctestsmsplugin::SendMmsMessageL
// Send ccs message using RSendAs api
// -----------------------------------------------------------------------------
//

TInt Ctest_pluginpreview::SendMmsMessageL( CStifItemParser& /*aItem*/ )
    {
	// Print to UI
	_LIT( Ktestplugin, "SendCcsMessageL" );
	_LIT( KFunction, "In SendCcsMessageL" );
	TestModuleIf().Printf( 0, Ktestplugin, KFunction );
	// Print to log file
	iLog->Log( KFunction );
	
	RSendAs sendas;
    CleanupClosePushL(sendas);
    TInt res = sendas.Connect();
    User::LeaveIfError(res);
    
   	RSendAsMessage message;
    CleanupClosePushL(message);
    TRAP(res, message.CreateL(sendas,KUidMsgTypeMultimedia));
    User::LeaveIfError(res);

   // TBuf<512> MsgText;
    //MsgText.Copy(_L("Message being sent"));
    //message.SetBodyTextL(MsgText);
    TBuf<15> RecAdd;
    RecAdd.Copy(_L("123456789"));
    message.AddRecipientL(RecAdd,RSendAsMessage::ESendAsRecipientTo );

   	message.SendMessageAndCloseL();
   	CleanupStack::Pop(1); // RMessage
   	CleanupStack::PopAndDestroy(1); // RSendAs

   	return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::CleanMessagesInFolderL
// Cleans messages from the specified folder
// -----------------------------------------------------------------------------

void Ctest_pluginpreview::CleanMessagesInFolderL(TMsvId aBoxId,TBool aDeleteOne)
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



