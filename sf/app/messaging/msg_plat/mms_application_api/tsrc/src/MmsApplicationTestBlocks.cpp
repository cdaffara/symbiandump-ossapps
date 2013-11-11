/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MmsApplicationTestBlocks implementation*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MmsApplicationTest.h" //Header of Application Adapter
#include "mmssettings.h"
#include "mmsapplicationadapter.h"
#include "mmsregisteredapplications.h"
#include "MTCLREG.H" //For CClientMtmRegistry
#include "mmsclient.h"
#include "msvids.h"
// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS
_LIT(KAppId1,"KAppId1");
_LIT(KAppId2,"KAppId2");
_LIT ( KUserFriendlyName, "friend" );
_LIT ( KUserFriendlyName1, "friend1" );


// MACROS


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsApplicationTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMmsApplicationTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMmsApplicationTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMmsApplicationTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CMmsApplicationTest::ExampleL ),
        ENTRY( "Register", CMmsApplicationTest::RegisterL ),
        ENTRY( "UnRegisterL", CMmsApplicationTest::UnRegisterL ),
        ENTRY( "RegisteredL", CMmsApplicationTest::RegisteredL ),
        ENTRY( "FolderIdL", CMmsApplicationTest::FolderIdL ),
        ENTRY( "ApplicationIdL", CMmsApplicationTest::ApplicationIdL ),
        ENTRY( "SetApplicationIdL", CMmsApplicationTest::SetApplicationIdL ),
        ENTRY( "ReplyToApplicationIdL", CMmsApplicationTest::ReplyToApplicationIdL ),
        ENTRY( "SetReplyToApplicationIdL", CMmsApplicationTest::SetReplyToApplicationIdL ),
        ENTRY( "RegisteredApplicationList", CMmsApplicationTest::RegisteredApplicationListL ),
        ENTRY( "DeleteL", CMmsApplicationTest::DeleteL ),
        ENTRY( "MmscUrlL", CMmsApplicationTest::MmscUrlL ),
        ENTRY( "RegisterWithNameL", CMmsApplicationTest::RegisterWithNameL ),
        ENTRY( "UserFriendlyName", CMmsApplicationTest::UserFriendlyNameL ),
        
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMmsApplicationTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmsApplicationTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KMmsApplicationTest, "MmsApplicationTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KMmsApplicationTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KMmsApplicationTest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
TInt CMmsApplicationTest::RegisterL( CStifItemParser& /*aItem */)
    {
    iLog->Log(_L("In RegisterL"));
    TMsvId folderId = iAppAdapter->FolderIdL(KAppId1);
    iAppAdapter->RegisterL(KAppId1,folderId);
    iAppAdapter->UnregisterL(KAppId2);
    iAppAdapter->UnregisterL(KAppId1);
    
    return KErrNone;
    }
TInt CMmsApplicationTest::UnRegisterL( CStifItemParser& /*aItem */)
    {
    iLog->Log(_L("In UnRegisterL"));
    TInt error;
    TMsvId folderId = iAppAdapter->FolderIdL(KAppId2);
    iAppAdapter->RegisterL(KAppId2,folderId);
    error = iAppAdapter->UnregisterL(KAppId2);
    return error;
    }
TInt CMmsApplicationTest::RegisteredL( CStifItemParser& /*aItem */)
    {
    TMsvId folderId;
    iLog->Log(_L("In RegisteredL"));
    iAppAdapter->RegisteredL(KAppId2);
    iAppAdapter->RegisterL(KAppId2,folderId);
    iAppAdapter->RegisteredL(KAppId2);
    iAppAdapter->UnregisterL(KAppId2);
    return KErrNone;
    }
TInt CMmsApplicationTest::FolderIdL()
    {
    iLog->Log(_L("In FolderIdL"));
    iAppAdapter->UnregisterL(KAppId1);
    iAppAdapter->FolderIdL(KAppId1);
    TMsvId folderId = KMsvNullIndexEntryId;
    iAppAdapter->RegisterL(KAppId2,folderId);
    TMsvId retFolderId = iAppAdapter->FolderIdL(KAppId2);
    iAppAdapter->UnregisterL(KAppId2);
    if(retFolderId == folderId)
	    {
	    return KErrNone;	
	    }
	 else
	 	{
	 	return KErrGeneral;
	 	}
     }
TInt CMmsApplicationTest::ApplicationIdL()
    {
    iLog->Log(_L("In ApplicationIdL"));
    TMsvId messageId = CreateMessageL();
    iAppAdapter->SetApplicationIdL(messageId,KAppId1);
    TPtrC applicId = iAppAdapter->ApplicationIdL(messageId);
    iAppAdapter->DeleteL(messageId);
    if(applicId.Compare(KAppId1) == 0)
	    {
	    return KErrNone;	
	    }
	 else
	 	{
	 	return KErrGeneral;
	 	}
    }
TInt CMmsApplicationTest::SetApplicationIdL()
    {
    iLog->Log(_L("In SetApplicationIdL"));
    TMsvId messageId = CreateMessageL();
    iAppAdapter->SetApplicationIdL(messageId,KAppId1);
    TPtrC applicId = iAppAdapter->ApplicationIdL(messageId);
    iAppAdapter->DeleteL(messageId);
    if(applicId.Compare(KAppId1) != 0)
	    {
	    return KErrGeneral;	
	    }
	 else
	 	{
	 	return KErrNone;
	 	}
    
    }
TInt CMmsApplicationTest::ReplyToApplicationIdL()
    {
    iLog->Log(_L("In ReplyToApplicationIdL"));
    TMsvId messageId = CreateMessageL();
    iAppAdapter->SetReplyToApplicationIdL(messageId,KAppId1);
    TPtrC replyToApplicId = iAppAdapter->ReplyToApplicationIdL(messageId);
    iAppAdapter->DeleteL(messageId);
    if(replyToApplicId.Compare(KAppId1) == 0)
	    {
	    return KErrNone;	
	    }
	 else
	 	{
	 	return KErrGeneral;
	 	}
    }
TInt CMmsApplicationTest::SetReplyToApplicationIdL()
    {
    iLog->Log(_L("In SetReplyToApplicationIdL"));
    TMsvId messageId = CreateMessageL();
    iAppAdapter->SetReplyToApplicationIdL(messageId,KAppId1);
    TPtrC replyToApplicId = iAppAdapter->ReplyToApplicationIdL(messageId);
    iAppAdapter->DeleteL(messageId);
    if(replyToApplicId.Compare(KAppId1) == 0)
	    {
	    return KErrNone;	
	    }
	 else
	 	{
	 	return KErrGeneral;
	 	}
    }
TInt CMmsApplicationTest::DeleteL()
    {
    iLog->Log(_L("In DeleteL"));
    TMsvId messageId = CreateMessageL();
    TInt error = iAppAdapter->DeleteL(messageId);
    iAppAdapter->DeleteL(messageId);
    return error;
    }
TInt CMmsApplicationTest::MmscUrlL()
    {
    iLog->Log(_L("In MmscUrlL"));
    
    HBufC* serverUri = NULL;
    TInt error = iAppAdapter->MmscUrlL(serverUri);
    CMmsSettings* mmsSettings = CMmsSettings::NewL(); 
    mmsSettings->LoadSettingsL();
    TInt accessPointCount = mmsSettings->AccessPointCount();
    if(!accessPointCount)
    {
    mmsSettings->AddAccessPointL(1,0);
    mmsSettings->AddAccessPointL(2,1);
    error = iAppAdapter->MmscUrlL(serverUri);	
    mmsSettings->DeleteAccessPointL(1);
    mmsSettings->DeleteAccessPointL(0);
    }
    else
    {
	    while(accessPointCount)
	    {
	    mmsSettings->DeleteAccessPointL(0);
	    accessPointCount--;	
	    }
	error = iAppAdapter->MmscUrlL(serverUri);
    }
 
    delete mmsSettings;
   
        return KErrNone;
    
    }
TInt CMmsApplicationTest::RegisteredApplicationListL()
    {
    TMsvId folderId;
    iLog->Log(_L("In RegisteredApplicationList"));
    iAppAdapter->RegisterL(KAppId1,folderId);
    iAppAdapter->RegisterL(KAppId2,folderId);
    iAppAdapter->RegisteredApplicationList(); //returns an arrry of registered applications
    iAppAdapter->UnregisterL(KAppId1);
    iAppAdapter->UnregisterL(KAppId2);
    return KErrNone;
    }
TInt CMmsApplicationTest::RegisterWithNameL()
    {
    TInt error;
    iLog->Log(_L("In RegisterWithNameL"));
    TMsvId folderId = KMsvNullIndexEntryId;
    error = iAppAdapter->RegisterWithNameL(KAppId1,KUserFriendlyName,folderId);
    iAppAdapter->RegisterWithNameL(KAppId1,KUserFriendlyName,folderId);
    iAppAdapter->RegisterWithNameL(KAppId1,KUserFriendlyName1,folderId);
    iAppAdapter->RegisterWithNameL(KAppId2,KUserFriendlyName1,folderId);
    iAppAdapter->UnregisterL(KAppId1);
    iAppAdapter->UnregisterL(KAppId2);
    return error;
    }
TInt CMmsApplicationTest::UserFriendlyNameL()
    {
    iLog->Log(_L("In User Friendly Name"));
    TMsvId folderId = KMsvNullIndexEntryId;
    iAppAdapter->RegisterWithNameL(KAppId1,KUserFriendlyName,folderId);
    iAppAdapter->RegisterL(KAppId2,folderId);
    TPtrC name = iAppAdapter->UserFriendlyName(KAppId1);
    iAppAdapter->UserFriendlyName(KAppId2);
    iAppAdapter->UnregisterL(KAppId1);
    iAppAdapter->UnregisterL(KAppId2);
    if(name.Compare(KUserFriendlyName) == 0)
        {
        return KErrNone;
        }
    return KErrGeneral;
    
    }

TMsvId CMmsApplicationTest::CreateMessageL()
    {
    if ( !iSession )
       {
       iSession = CMsvSession::OpenSyncL(*this);
       }
   CClientMtmRegistry* clientMtmRegistry = CClientMtmRegistry::NewL(*iSession);
   CleanupStack::PushL( clientMtmRegistry );
   CMmsClientMtm* mmsClient = (CMmsClientMtm *) clientMtmRegistry->NewMtmL(KUidMsgTypeMultimedia);
   CleanupStack::PushL( mmsClient );
   TMsvId serviceId = mmsClient->DefaultServiceL();
   mmsClient->SwitchCurrentEntryL( KMsvDraftEntryId );
   mmsClient->CreateMessageL( serviceId );
   TMsvId messageid = mmsClient->Entry().EntryId();
       
   _LIT( KSender, "+3581234567890" );
   _LIT( KRecipient1, "+3580987654321" );
   _LIT ( KMessageSubject, "Subject 1" );
   
   mmsClient->SetSenderL( KSender );
   
   mmsClient->AddAddresseeL( KRecipient1, TPtrC() ); // this becomes "To"
   
   mmsClient->SetSubjectL( KMessageSubject );
   
   // save the message 
   mmsClient->SaveMessageL();
   // make it visible and not under construction
   TMsvEntry entry = mmsClient->Entry().Entry();
   entry.SetInPreparation( EFalse );
   entry.SetVisible( ETrue );
   entry.iDate.HomeTime();
   mmsClient->Entry().ChangeL( entry );
   CleanupStack::PopAndDestroy(mmsClient);
   CleanupStack::PopAndDestroy(clientMtmRegistry);
   return messageid;
    }
void CMmsApplicationTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    //No Implementation
    }
// -----------------------------------------------------------------------------
// CMmsApplicationTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CMmsApplicationTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
