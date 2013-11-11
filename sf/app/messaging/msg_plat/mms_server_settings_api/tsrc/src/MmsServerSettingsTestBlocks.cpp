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
* Description:   Test Case file for testing the MmsServersettings API
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MmsServerSettingsTest.h"
#include <mmsclient.h>

#include <mmsaccount.h>
#include <mmssettings.h>
#include <mmsregisteredapplications.h>
#include <MsvScheduleSettings.h>
#include <MsvOffPeakTime.h>
#include <MsvSendErrorAction.h>
#include <msvsysagentaction.h>
#include <msvstd.hrh>   // KMsvNullIndexEntryIdValue
#include <msvstd.h>

// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS
_LIT( KTestApplicationId1, "com.test.messenger1" );
_LIT( KTestApplicationId2, "com.test.messenger2" );
_LIT( KTestApplicationId3, "com.test.messenger3" );
_LIT( KUserFriendlyName1, "friend1" );
_LIT( KUserFriendlyName2, "friend2" );
_LIT( KUserFriendlyName3, "friend3" );


_LIT( KBuffer,"This is a string of length 1024                                                                                                                                                                                                                              ok");
// MACROS


// LOCAL CONSTANTS AND MACROS
//
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMmsServerSettingsTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CMmsServerSettingsTest::ExampleL ),
        ENTRY( "CMmsAccount::LoadSettingsL", CMmsServerSettingsTest::CMmsAccountLoadSettingsL ),
        ENTRY( "CMmsAccount::SaveSettingsL", CMmsServerSettingsTest::CMmsAccountSaveSettingsL ),
        ENTRY( "LoadScheduleSendSettingsL", CMmsServerSettingsTest::CMmsAccountLoadScheduleSendSettingsL ),
        ENTRY( "LoadScheduleReceiveSettingsL", CMmsServerSettingsTest::CMmsAccountLoadScheduleReceiveSettingsL ),
        ENTRY( "CMmsSettings::Reset", CMmsServerSettingsTest::CMmsSettingsReset ),
        ENTRY( "CMmsSettings::CopyL", CMmsServerSettingsTest::CMmsSettingsCopyL ),
        ENTRY( "CMmsSettings::LoadSettingsL", CMmsServerSettingsTest::CMmsSettingsLoadSettingsL ),
        ENTRY( "CMmsSettings::SaveSettingsL", CMmsServerSettingsTest::CMmsSettingsSaveSettingsL ),
        ENTRY( "CMmsSettings::AddAccessPointL", CMmsServerSettingsTest::CMmsSettingsAddAccessPointL ),
        ENTRY( "CMmsSettings::DeleteAccessPointL", CMmsServerSettingsTest::CMmsSettingsDeleteAccessPointL ),
        ENTRY( "CMmsSettings::AccessPoint", CMmsServerSettingsTest::CMmsSettingsAccessPointL ),
        ENTRY( "CMmsSettings::ValidateSettings", CMmsServerSettingsTest::CMmsSettingsValidateSettingsL ),
        ENTRY( "CMmsSettings::RestoreFactorySettingsL", CMmsServerSettingsTest::CMmsSettingsRestoreFactorySettingsL ),
        ENTRY( "CMmsSettings::CreateNewServiceL", CMmsServerSettingsTest::CMmsSettingsCreateNewServiceL ),
        ENTRY( "CMmsSettings::CreateFolderEntryL", CMmsServerSettingsTest::CMmsSettingsCreateFolderEntryL ),
        ENTRY( "CMmsSettings::FolderEntryL", CMmsServerSettingsTest::CMmsSettingsFolderEntryL ),
        ENTRY( "CMmsRegApps::LoadRegisteredApplicationsL", CMmsServerSettingsTest::LoadRegisteredApplicationsL ),
        ENTRY( "CMmsRegApps::NumberOfRegisteredApplicationsL", CMmsServerSettingsTest::NumberOfRegisteredApplicationsL ),
        ENTRY( "CMmsRegApps::RegisteredL", CMmsServerSettingsTest::RegisteredL ),
        ENTRY( "CMmsRegApps::RegisterL", CMmsServerSettingsTest::RegisterL ),
        ENTRY( "CMmsRegApps::UnRegisterL", CMmsServerSettingsTest::UnRegisterL ),
        ENTRY( "CMmsRegApps::UserFriendlyName", CMmsServerSettingsTest::UserFriendlyNameL ),
        ENTRY( "CMmsRegApps::RegisteredApplicationList", CMmsServerSettingsTest::RegisteredApplicationListL ),
        
        
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KMmsServerSettingsTest, "MmsServerSettingsTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KMmsServerSettingsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KMmsServerSettingsTest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsAccountLoadSettingsL
// Executes the LoadSettingsL method of CMmsAccount class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsAccountLoadSettingsL( CStifItemParser& /*aItem*/ )   
	{
	TInt error;
    TBool localMode = iMmsSettings->LocalMode();
    
    TRAP( error, iMmsAccount->LoadSettingsL( *iMmsSettings ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("CMmsAccount::Load settings passed ") );
        }
    else
        {
        iLog->Log( _L("CMmsAccount::Load Settings failed with error %d"), error );
        }
   	return KErrNone;	
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsAccountSaveSettingsL
 // Executes the SaveSettingsL method of CMmsAccount class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsAccountSaveSettingsL( CStifItemParser& /*aItem*/ )   
	{
	TInt error;
    TBool localMode = iMmsSettings->LocalMode();
    TRAP( error, iMmsAccount->SaveSettingsL( *iMmsSettings ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("CMmsAccount::Save Settings passed ") );
        }
    else
        {
        iLog->Log( _L("CMmsAccount::Save Settings failed with error %d"), error );
        }
   
	return error;	
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsAccountLoadScheduleSendSettingsL
// Executes the LoadScheduleSendSettingsL method of CMmsAccount class 
// -----------------------------------------------------------------------------
//	
TInt CMmsServerSettingsTest::CMmsAccountLoadScheduleSendSettingsL( CStifItemParser& /*aItem*/ )  
	{
	TInt error;

	CMsvScheduleSettings* scheduleSettings = CMsvScheduleSettings::NewL();
    CleanupStack::PushL( scheduleSettings );
    
    CMsvOffPeakTimes* offPeak = new (ELeave) CMsvOffPeakTimes();
    CleanupStack::PushL( offPeak );
    
    CMsvSendErrorActions* errorActions = CMsvSendErrorActions::NewL();
    CleanupStack::PushL( errorActions );
    
    CMsvSysAgentActions* sysAgentActions = new (ELeave) CMsvSysAgentActions();
    CleanupStack::PushL( sysAgentActions );
    
    TRAP( error, iMmsAccount->LoadScheduleSendSettingsL(
        *scheduleSettings,
        *offPeak,
        *errorActions,
        *sysAgentActions ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("Load Scheduled Send Settings passed") );
        }
    else
        {
        iLog->Log( _L("Load Scheduled Send Settings failed with error %d"), error );
        }
        CleanupStack::PopAndDestroy( sysAgentActions );
	    sysAgentActions = NULL;
	    CleanupStack::PopAndDestroy( errorActions );
	    errorActions = NULL;
	    CleanupStack::PopAndDestroy( offPeak );
	    offPeak = NULL;
	    CleanupStack::PopAndDestroy( scheduleSettings );
	    scheduleSettings = NULL;  
          return error;
      }
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsAccountLoadScheduleReceiveSettingsL
// Executes the LoadScheduleReceiveSettingsL method of CMmsAccount class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsAccountLoadScheduleReceiveSettingsL( CStifItemParser& /*aItem*/ )  
	{
	TInt error;
	CMsvScheduleSettings* scheduleSettings = CMsvScheduleSettings::NewL();
    CleanupStack::PushL( scheduleSettings );
    
    CMsvOffPeakTimes* offPeak = new (ELeave) CMsvOffPeakTimes();
    CleanupStack::PushL( offPeak );
    
    CMsvSendErrorActions* errorActions = CMsvSendErrorActions::NewL();
    CleanupStack::PushL( errorActions );
    
    CMsvSysAgentActions* sysAgentActions = new (ELeave) CMsvSysAgentActions();
    CleanupStack::PushL( sysAgentActions );
    TRAP( error, iMmsAccount->LoadScheduleReceiveSettingsL(
        *scheduleSettings,
        *offPeak,
        *errorActions,
        *sysAgentActions ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("Load Scheduled Receive Settings passed") );
        }
    else
        {
        iLog->Log( _L("Load Scheduled Receive Settings failed with error %d"), error );
        }

    // these are not needed any more    
    CleanupStack::PopAndDestroy( sysAgentActions );
    sysAgentActions = NULL;
    CleanupStack::PopAndDestroy( errorActions );
    errorActions = NULL;
    CleanupStack::PopAndDestroy( offPeak );
    offPeak = NULL;
    CleanupStack::PopAndDestroy( scheduleSettings );
    scheduleSettings = NULL;   
    return error;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsReset
// Executes the Reset method of CMmsSettings class 
// -----------------------------------------------------------------------------
//	
TInt CMmsServerSettingsTest::CMmsSettingsReset( CStifItemParser& /*aItem*/ )
	{
	
	iMmsSettings->Reset();
	iLog->Log(_L("CMmsSettings_Reset passed"));
	return KErrNone;
			
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsCopyL
// Executes the CopyL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsCopyL( CStifItemParser& /*aItem*/ )
	{
//	TInt error;
	TBool localMode = iMmsSettings->LocalMode();
	CMmsSettings* settings = CMmsSettings::NewL();
	iMmsSettings->CopyL(*settings);
	settings->AddAccessPointL(2,1);
	settings->AddAccessPointL(4,2);	
	iMmsSettings->CopyL(*settings);
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsLoadSettingsL
// Executes the LoadSettingsL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsLoadSettingsL( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("In LoadSettingsL"));
	TInt error;
	TBool localMode = iMmsSettings->LocalMode();
    TRAP( error, iMmsSettings->LoadSettingsL());
    iMmsSettings->AddAccessPointL(2,1);
	iMmsSettings->AddAccessPointL(4,2);
	TRAP( error, iMmsSettings->SaveSettingsL()); //Save the settings for saving the accesspoints.
	TRAP( error, iMmsSettings->LoadSettingsL());
	iMmsSettings->DeleteAccessPointL(1); //Delete the accesspoint that we have added
	iMmsSettings->DeleteAccessPointL(0);
    if ( error == KErrNone )
        {
        iLog->Log( _L("CMmsSettings::Load settings passed ") );
        }
    else
        {
        iLog->Log( _L("CMmsSettings::Load Settings failed with error %d"), error );
        }
  	return KErrNone;	
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsSaveSettingsL
// Executes the SaveSettingsL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//

TInt CMmsServerSettingsTest::CMmsSettingsSaveSettingsL( CStifItemParser& /*aItem*/ )
	{
	TInt error;
	TBool localMode = iMmsSettings->LocalMode();
    TRAP( error, iMmsSettings->SaveSettingsL());
    if ( error == KErrNone )
        {
        iLog->Log( _L("CMmsSettings::Save settings passed ") );
        }
    else
        {
        iLog->Log( _L("CMmsSettings::Save Settings failed with error %d"), error );
        }
    return KErrNone;	
	
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsAddAccessPointL
// Executes the AddAccessPointL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//	
TInt CMmsServerSettingsTest::CMmsSettingsAddAccessPointL( CStifItemParser& /*aItem*/ )
	{
	TInt error;
	TRAP(error,iMmsSettings->AddAccessPointL(0,0)); // Illegal accesspoint
	TRAP(error,iMmsSettings->AddAccessPointL(1,-1)); //Illegal index
	TRAP(error,iMmsSettings->AddAccessPointL(1,2)); //This will insert the value at 0 location -> No error
	TRAP(error,iMmsSettings->AddAccessPointL(2,1)); //Normal case
	TRAP(error,iMmsSettings->AddAccessPointL(2,2)); //Inserting same accesspoint again
	TRAP( error, iMmsSettings->SaveSettingsL());
	iMmsSettings->DeleteAccessPointL(1); //Delete the accesspoint that we have added
	iMmsSettings->DeleteAccessPointL(0);
	
	if(error == KErrNone || error == KErrArgument)
	{
		return KErrNone;
	}
	return error;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsDeleteAccessPointL
// Executes the DeleteAccessPointL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsDeleteAccessPointL( CStifItemParser& /*aItem*/ )
	{
	TInt error;
	TRAP(error,iMmsSettings->AddAccessPointL(2,0));
	TRAP( error, iMmsSettings->SaveSettingsL());
	TRAP(error,iMmsSettings->DeleteAccessPointL(-1)); //Deletion from index -1
	TRAP(error,iMmsSettings->DeleteAccessPointL(0));  //Deletion from index 0
	TRAP(error,iMmsSettings->DeleteAccessPointL(5));  //Array out of bounds (we have inserted only 2 accesspoints in the earlier test case)
	if(error == KErrNone || error == KErrArgument)
		{
		return KErrNone;
		}
	return error;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsAccessPoint
// Executes the AccessPoint method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsAccessPointL( CStifItemParser& /*aItem*/ )
	{
	TInt accesspoint;
	TInt error;
	TRAP(error,iMmsSettings->AddAccessPointL(1,0));
	TRAP( error, iMmsSettings->SaveSettingsL());
	accesspoint = iMmsSettings->AccessPoint(0); // Valid
	accesspoint = iMmsSettings->AccessPoint(-1);
	accesspoint = iMmsSettings->AccessPoint(5); //Array out of bounds
	iMmsSettings->DeleteAccessPointL(0); //Delete the accesspoint that we have added
	if(accesspoint == KErrNone || accesspoint == KErrNotFound)
	    {
	    return KErrNone;
	    }
	return accesspoint;
	
		
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsValidateSettings
// Executes the ValidateSettings method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsValidateSettingsL( CStifItemParser& /*aItem*/ )
	{
	TInt error;
	iMmsSettings->ValidateSettings(); //This passes Normally
	
	iMmsSettings->SetExpiryInterval( -1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetExpiryInterval( KMmsMaxExpiry + 10 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetExpiryInterval( 0 ); 
    iMmsSettings->ExpiryInterval();
    
    iMmsSettings->SetSenderVisibility( TMmsMessageSenderVisibility( -1 ) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetSenderVisibility( TMmsMessageSenderVisibility(EMmsMaximumSenderVisibility + 10) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetSenderVisibility( EMmsSenderVisibilityDefault );
    iMmsSettings->SenderVisibility();
    
    iMmsSettings->SetDeliveryReportWanted( KMmsYes - 1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetDeliveryReportWanted( KMmsNo + 1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetDeliveryReportWanted( KMmsYes );
    iMmsSettings->DeliveryReportWanted();
    
    iMmsSettings->SetDeliveryReportSendingAllowed( KMmsYes - 1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetDeliveryReportSendingAllowed( KMmsNo + 1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetDeliveryReportSendingAllowed( KMmsYes );
    iMmsSettings->DeliveryReportSendingAllowed();
    
    iMmsSettings->SetReadReplyReportWanted( KMmsYes - 1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetReadReplyReportWanted( KMmsNo + 1 );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetReadReplyReportWanted( KMmsYes );
    iMmsSettings->ReadReplyReportWanted();
    
    iMmsSettings->SetReceivingModeHome( TMmsReceivingMode (EMmsReceivingMin - 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetReceivingModeHome( TMmsReceivingMode (EMmsReceivingMax + 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetReceivingModeHome( EMmsReceivingAutomatic );
    iMmsSettings->ReceivingModeHome();
    
    iMmsSettings->SetReceivingModeForeign( TMmsReceivingMode (EMmsReceivingMin - 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetReceivingModeForeign( TMmsReceivingMode (EMmsReceivingMax + 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetReceivingModeForeign( EMmsReceivingManual );
    iMmsSettings->ReceivingModeForeign();
    
    iMmsSettings->SetMessagePriority( TMmsMessagePriority(EMmsMinimumPriority - 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetMessagePriority( TMmsMessagePriority(EMmsMaximumPriority + 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetMessagePriority( EMmsPriorityNormal );
    iMmsSettings->MessagePriority();
    
    iMmsSettings->SetMessageClass( TMmsMessageClass(EMmsMimimumMessageClass - 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetMessageClass( TMmsMessageClass(EMmsMaximumMessageClass + 1) );
    error = iMmsSettings->ValidateSettings();
    iMmsSettings->SetMessageClass( EMmsClassPersonal );
    iMmsSettings->MessageClass();
    
    iMmsSettings->SetLocalMode( ETrue) ;
    iMmsSettings->ValidateSettings();
    
    iMmsSettings->SetLocalMode( EFalse);
    iMmsSettings->ValidateSettings();
    
    TRAP(error,iMmsSettings->AddAccessPointL(2,1));
    TRAP(error,iMmsSettings->AddAccessPointL(3,2));
    TRAP( error, iMmsSettings->SaveSettingsL());
    iMmsSettings->ValidateSettings();
    iMmsSettings->DeleteAccessPointL(1); //Delete the accesspoint that we have added
	iMmsSettings->DeleteAccessPointL(0);
    
    
    
	return KErrNone;
		
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsRestoreFactorySettingsL
// Executes the RestoreFactorySettingsL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsRestoreFactorySettingsL( CStifItemParser& /*aItem*/ )
	{

	TInt error;
	TRAP(error,iMmsSettings->RestoreFactorySettingsL( *iSession, EMmsFactorySettingsLevelNormal ));	
	return error;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsCreateNewServiceL
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsCreateNewServiceL( CStifItemParser& /*aItem*/ )
	{
	TInt error;
	TRAP(error,iMmsSettings->CreateNewServiceL(*iSession));
	return error;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsCreateFolderEntryL
// Executes the CreateFolderEntryL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsCreateFolderEntryL( CStifItemParser& /*aItem*/ )
	{
	TInt error;
	// 0x1008 is the "Documents" folder.
	TMsvId dummyFolder = KMsvNullIndexEntryIdValue;
    _LIT( KDummyFolder1, "ForTest" );
    TMsvId dummyFolder1 = KMsvNullIndexEntryIdValue; 
    error = iMmsSettings->CreateFolderEntryL( *iSession, 0x1008, KDummyFolder1, dummyFolder1 );
    iLog->Log( _L("Create Folder Entry, returned %d"), error );
    error = iMmsSettings->CreateFolderEntryL( *iSession, 0x1008, KDummyFolder1, dummyFolder ); 
    iLog->Log( _L("Create Duplicate Folder Entry, returned %d"), error );
    return KErrNone;
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsSettingsFolderEntryL
// Executes the FolderEntryL method of CMmsSettings class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::CMmsSettingsFolderEntryL( CStifItemParser& /*aItem*/ )
	{
	// 0x1008 is the "Documents" folder.
	TMsvId dummyFolder = KMsvNullIndexEntryIdValue;
    _LIT( KDummyFolder, "Test" );
    iMmsSettings->CreateFolderEntryL( *iSession, 0x1008, KDummyFolder, dummyFolder );
     TMsvId folder = iMmsSettings->FolderEntryL( *iSession, 0x1008, KDummyFolder);
     if( dummyFolder == folder)
     {
     return KErrNone;	
     }
     return KErrGeneral;
     
	}
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::LoadRegisteredApplicationsL
// Executes the LoadRegisteredApplicationsL method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::LoadRegisteredApplicationsL( CStifItemParser& /*aItem*/)
    {
    TInt error;
    TRAP(error,iMmsRegisteredApps->LoadRegisteredApplicationsL());
    iMmsRegisteredApps->RegisterL( KTestApplicationId1); //Try creating one and check again
    iMmsRegisteredApps->RegisterL( KTestApplicationId2);
    TRAP(error,iMmsRegisteredApps->LoadRegisteredApplicationsL()); 
    iMmsRegisteredApps->UnregisterL( KTestApplicationId1);
    iMmsRegisteredApps->UnregisterL( KTestApplicationId2);
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::NumberOfRegisteredApplicationsL
// Executes the NumberOfRegisteredApplicationsL method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::NumberOfRegisteredApplicationsL( CStifItemParser& /*aItem*/)
    {
    iMmsRegisteredApps->NumberOfRegisteredApplicationsL();
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::RegisteredL
// Executes the LoadScheduleSendSettingsL method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::RegisteredL( CStifItemParser& /*aItem*/)
    {
    iMmsRegisteredApps->RegisterL( KTestApplicationId1);
    TBool ret = iMmsRegisteredApps->RegisteredL(KTestApplicationId1);
    iMmsRegisteredApps->UnregisterL( KTestApplicationId1);
    if(ret)
	    {
	    return KErrNone;	
	    }
	return KErrGeneral;
    
    }
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::RegisterL
// Executes the RegisterL method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::RegisterL( CStifItemParser& /*aItem*/)
    {
    RBuf myRBuf;
	TInt maxSizeOfData = 1030;
	if ((myRBuf.Create(maxSizeOfData)) == KErrNone)
		{
		CleanupClosePushL(myRBuf);
		iLog->Log(_L("Rbuf Creation succeeded "));
		for(TInt i = 0;i<4;i++)
			{
			myRBuf.Append(KBuffer());	
			}
		myRBuf.Append(_L("1024 ok"));
		}
	TInt length = myRBuf.Length();
    iMmsRegisteredApps->RegisterL(KTestApplicationId1,KUserFriendlyName1); //Will be created 
    iMmsRegisteredApps->RegisterL(KTestApplicationId1,myRBuf); //Throws an error
    iMmsRegisteredApps->RegisterL(myRBuf,KUserFriendlyName1); //Throws an error 
    iMmsRegisteredApps->RegisterL(KTestApplicationId2); //Registers ..
    iMmsRegisteredApps->UnregisterL(KTestApplicationId1); //Unregistering the applications that we have registered
    iMmsRegisteredApps->UnregisterL(KTestApplicationId2);
    CleanupStack::PopAndDestroy(&myRBuf);
     
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::UnRegisterL
// Executes the UnRegisterL method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::UnRegisterL( CStifItemParser& /*aItem*/)
    {
    TInt error;
   
    TRAP(error,iMmsRegisteredApps->UnregisterL(KTestApplicationId1));
    iMmsRegisteredApps->RegisterL(KTestApplicationId1,KUserFriendlyName1);
    iMmsRegisteredApps->RegisterL(KTestApplicationId2,KUserFriendlyName2);
    iMmsRegisteredApps->RegisterL(KTestApplicationId3,KUserFriendlyName3);
    TRAP(error,iMmsRegisteredApps->UnregisterL(KTestApplicationId2));
    TRAP(error,iMmsRegisteredApps->UnregisterL(KTestApplicationId3));
    TRAP(error,iMmsRegisteredApps->UnregisterL(KTestApplicationId1));
    TRAP(error,iMmsRegisteredApps->UnregisterL(KTestApplicationId1));
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::UserFriendlyName
// Executes the UserFriendlyName method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::UserFriendlyNameL( CStifItemParser& /*aItem*/)
    {
    TInt error;
    TInt error1;
    TRAP(error,iMmsRegisteredApps->RegisterL(KTestApplicationId1,KUserFriendlyName1));
    TRAP(error,iMmsRegisteredApps->RegisterL(KTestApplicationId2));
    error1 = iMmsRegisteredApps->UserFriendlyName(KTestApplicationId1).Compare(KUserFriendlyName1);
    TRAP(error,iMmsRegisteredApps->UnregisterL(KTestApplicationId2));
    iMmsRegisteredApps->UserFriendlyName(KTestApplicationId2);
    iMmsRegisteredApps->UnregisterL(KTestApplicationId1);
    if(error1 == 0 )
        {
        return KErrNone;
        }
    return error1;
        
    }// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::RegisteredApplicationList
// Executes the RegisteredApplicationList method of CMmsRegisteredApplication class 
// -----------------------------------------------------------------------------
//
TInt CMmsServerSettingsTest::RegisteredApplicationListL( CStifItemParser& /*aItem*/)
    {
    iMmsRegisteredApps->RegisterL(KTestApplicationId1,KUserFriendlyName1);
    iMmsRegisteredApps->RegisterL(KTestApplicationId2,KUserFriendlyName2);
    iMmsRegisteredApps->RegisteredApplicationList();
    iMmsRegisteredApps->UnregisterL(KTestApplicationId1);
    iMmsRegisteredApps->UnregisterL(KTestApplicationId2);
    
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
