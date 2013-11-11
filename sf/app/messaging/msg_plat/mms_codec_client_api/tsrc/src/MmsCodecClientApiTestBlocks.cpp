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
* Description:   Description
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <msvapi.h>
#include <msvids.h>
#include "MmsCodecClientApiTest.h"
#include "mmscodecclient.h"
#include "mmsconst.h"
#include "e32const.h"





// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS

#ifdef __WINSCW__
_LIT( KBinaryMessage, "\\mmsmessagebinaries\\m.mms");
_LIT( KBinaryMessage2, "\\mmsmessagebinaries\\multipartrelated.mms");
#else
_LIT( KBinaryMessage,  "%C:\\testing\\data\\mmsmessagebinaries\\m.mms");
_LIT( KBinaryMessage2, "%C:\\testing\\data\\mmsmessagebinaries\\multipartrelated.mms");
#endif
const TInt KMmsCodecClientChunkSize = 1024;
// MACROS


// LOCAL CONSTANTS AND MACROS


// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMmsCodecClientApiTest::Delete() 
    {
	//CleanAllL();
	
    }

// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CMmsCodecClientApiTest::ExampleL ),
        ENTRY( "CreateNewMessageEntryL", CMmsCodecClientApiTest::CreateNewMessageEntryL ),
        ENTRY( "AddMMTestL", CMmsCodecClientApiTest::AddMMTestL ),
        ENTRY( "RelaceMMTestL", CMmsCodecClientApiTest::RelaceMMTestL ),
        ENTRY( "RetreiveMMTestL", CMmsCodecClientApiTest::RetreiveMMTestL ),
        ENTRY( "SendMMTestL", CMmsCodecClientApiTest::SendMMTestL ),
        ENTRY( "MoveMMTestL", CMmsCodecClientApiTest::MoveMMTestL ),
        ENTRY( "SetFlagsTest", CMmsCodecClientApiTest::SetFlagsTestL ),
        ENTRY( "InitializeChunkedRetrievingTestL", CMmsCodecClientApiTest::InitializeChunkedRetrievingTestL ),
        ENTRY( "DiskSpaceBelowCriticalLevelTestL", CMmsCodecClientApiTest::DiskSpaceBelowCriticalLevelTestL ),
        ENTRY( "InitializeChunkedAddingTestL", CMmsCodecClientApiTest::InitializeChunkedAddingTestL ),
        ENTRY( "InitializeChunkedReplacingTestL", CMmsCodecClientApiTest::InitializeChunkedReplacingTestL ),
        ENTRY( "DeleteMM", CMmsCodecClientApiTest::DeleteMMTestL ),
        ENTRY( "CleanMessageStore", CMmsCodecClientApiTest::CleanAllL ),
        
        
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KMmsCodecClientApiTest, "MmsCodecClientApiTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KMmsCodecClientApiTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KMmsCodecClientApiTest, 
                                KParam, i, &string );
        i++;
        }
   
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::CreateNewMessageEntryL
// Calls the method CreateNewMessageEntryL in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::CreateNewMessageEntryL()
    {
    TInt errorCount =2;
    iLog->Log(_L(" In CreateNewMessageEntryL Test"));
    TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvDraftEntryId;
    TInt error1 = iCodecClient->CreateNewMessageEntryL(folder, messageId);
    if(error1 == KErrNone)
       {
       iLog->Log(_L("Created New entry in Drafts Folder successfully"));
       errorCount--;
       }
    //Try creating the same message entry again.. It should return KErrArgument and delete the entry
    error1 = iCodecClient->CreateNewMessageEntryL(folder, messageId);
    if(error1 == KErrArgument)
          {
          iLog->Log(_L("Creating same entry in Drafts Folder again returned KErrArg "));
          errorCount--;
          }
    CleanAllL();
    return errorCount;
    }
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::AddMMTestL
// Calls the method AddMML in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::AddMMTestL()
    {
    TInt errorCount = 3;
    TInt error1;
    TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvDraftEntryId;
    TUint32 flags = 0;
    TBool unread = EFalse;
    //We are trying to call the AddMML function before calling CreateNewMessageEntryL() function.
    //The size of the encode buffer is also zero 
    //It should return with KErrArgument
    TRAPD(error2, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error2 != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error for the call made to AddMML before calling CreateNewMessageEntry()"));
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrArgument)
            {
        iLog->Log(_L("AddMML Returned with KErrArgument for not calling CreateNewMessageEntryL()"));
        errorCount--;
            }
        }
        
    //We are trying to call the AddMML function after calling CreateNewMessageEntryL() function.
    //The size of the encode buffer is  zero 
    //It should return KErrArgument    
    error1 = iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iEncodeBuffer = CBufFlat::NewL(1);
    iEncodeBuffer->ResizeL(0);
    if(error1!=KErrNone)
    	{
    	return error1;
    	}
    TRAPD(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL failed with error %d"),error);
        }
    else
        {
        iWait->Start();
         if(iWait->iStatus.Int() == KErrArgument)
            {
        iLog->Log(_L("AddMMTestL returned KErrArgument,when the passed buffer size is zero"));
        errorCount--;
            }
        }
    //We are trying to call the AddMML function after calling CreateNewMessageEntryL() function.
    //The size of the encode buffer is not zero 
    //It should return with KErrNone or KErrNotSupported if decoding fails...
    
    error1 = iCodecClient->CreateNewMessageEntryL(folder, messageId);
    if(error1!=KErrNone)
    	{
    	CleanAllL();
    	return error1;
    	}
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
   #endif		
   iLog->Log(iFilePath);
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
   // iEncodeBuffer->ResizeL(10);
    
    TRAPD(error3, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error3 != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error3);
        
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            iLog->Log(_L("AddMMTestL with valid parameters returned %d"),iWait->iStatus.Int());
            errorCount--;
            }
        }
    CleanAllL();
    return errorCount;
    }
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::RelaceMMTestL
// Calls the method RelaceMML in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::RelaceMMTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvDraftEntryId;
    TMsvId outbox = KMsvGlobalOutBoxIndexEntryIdValue;
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt error;
    TInt returnValue;
    TInt errorCount = 4;
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    error = iCodecClient->CreateNewMessageEntryL(folder, messageId);
    if(error!=KErrNone)
    	{
    	return error;
    	}
    //iEncodeBuffer->ResizeL(10);
    TRAPD(error1, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    
    if(error1 != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error1);
        returnValue = error1;
        }
    else
        {
        iWait->Start();
        returnValue = iWait->iStatus.Int();
        iLog->Log(_L("AddMMTestL returned %d"),iWait->iStatus.Int());
           
        }
    if(!returnValue)
	    {
     	iLog->Log(_L("Calling ReplaceMML with valid parameters"));
   	   	TRAPD(error3,iCodecClient->ReplaceMML(messageId, *iEncodeBuffer, flags, unread, iWait->iStatus));
   	   	
	    if(error3 != KErrNone)
	    	{
	    	iLog->Log(_L("Call to ReplaceMML failed with %d"),error3);
	   		}
	   	else
	   		{
	   		iWait->Start();
	   		iLog->Log(_L("ReplaceMML returned with state %d"),iWait->iStatus.Int());	
	   		errorCount--;
	   		}
	    }
	    
    //Try to call the ReplaceMML function when the state is EEntryCreated...
    //Should return with KErrArgument and changes the state to EIdle
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
   // iEncodeBuffer->ResizeL(10);
    iLog->Log(_L("Calling ReplaceMML with Invalid state"));
    TRAPD(err,iCodecClient->ReplaceMML(messageId, *iEncodeBuffer, flags, unread, iWait->iStatus));
    
    if(err != KErrNone)
    	{
    	iLog->Log(_L("Call to ReplaceMML passed even when the state is EEntryCreated"));
   		}
   	else
   		{
   		iWait->Start();
   		iLog->Log(_L("ReplaceMML returned with state %d"),iWait->iStatus.Int());	
   		errorCount--;
   		}
   	// Call the ReplaceMML method with an empty buffer
	iEncodeBuffer->ResizeL(0);
	iLog->Log(_L("Calling ReplaceMML with empty buffer"));
	TRAPD(error2,iCodecClient->ReplaceMML(messageId, *iEncodeBuffer, flags, unread, iWait->iStatus));
	
    if(error2 != KErrNone)
    	{
    	iLog->Log(_L("Call to ReplaceMML passed even when the buffer size is zero"));
   		}
   	else
   		{
   		iWait->Start();
   		iLog->Log(_L("ReplaceMML returned with state %d"),iWait->iStatus.Int());	
   		errorCount--;
   		}
   	//Call ReplaceMML to replace a message in OutBox, it should return with error.
   	
   	iLog->Log(_L("Calling ReplaceMML to replace a message in outbox"));
   	iCodecClient->CreateNewMessageEntryL(outbox, messageId);
   //	iEncodeBuffer->ResizeL(10);
  	TRAPD(error3, iCodecClient->AddMML(*iEncodeBuffer, outbox, flags, unread, messageId, iWait->iStatus ) );
  	
    if(error3 != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error1);
        
        }
    else
        {
        iWait->Start();
        returnValue = iWait->iStatus.Int();
        iLog->Log(_L("AddMMTestL returned %d"),iWait->iStatus.Int());
        
        }
    TRAPD(err4,iCodecClient->ReplaceMML(messageId, *iEncodeBuffer, flags, unread, iWait->iStatus));
    
    if(err4 != KErrNone)
    	{
    	iLog->Log(_L("Call to ReplaceMML passed even when the state is EEntryCreated"));
   		}
   	else
   		{
   		iWait->Start();
   		iLog->Log(_L("ReplaceMML returned with state %d"),iWait->iStatus.Int());	
   		errorCount--;
   		}
   CleanAllL();
	return errorCount;	
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::RetreiveMMTestL
// Calls the method RetreiveMML in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::RetreiveMMTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvDraftEntryId;	
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt returnValue;
    TInt errorCount = 2;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage2;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage2,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    iLog->Log(_L("Calling RetrieveMML with invalid state"));
    TRAPD( error1, iCodecClient->RetrieveMML( messageId, *iEncodeBuffer, folder, flags, unread, iWait->iStatus ) );
    
    if ( error1 != KErrNone )
        {
        iLog->Log( _L("Retrieve Message entry failed, left with error: %d"), error1 );
        
        }
    else
        {
        iWait->Start();
        iLog->Log( _L("Retrieve Message entry  completed with status: %d"), iWait->iStatus.Int() );
        errorCount--;
        }
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    TRAPD(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error1);
        returnValue = error1;
        }
    else
        {
        iWait->Start();
        returnValue = iWait->iStatus.Int();
        iLog->Log(_L("AddMMTestL returned %d"),iWait->iStatus.Int());
        }
    if(!returnValue)
	    {
	    iLog->Log(_L("Calling RetrieveMML with valid parameters"));
	    TRAP( error, iCodecClient->RetrieveMML( messageId, *iEncodeBuffer, folder, flags, unread, iWait->iStatus ) );
	    if ( error != KErrNone )
	        {
	        iLog->Log( _L("Retrieve Message entry failed, left with error: %d"), error );
	        }
	    else
	        {
	        iWait->Start();
	        iLog->Log( _L("Retrieve Message entry  completed with status: %d"), iWait->iStatus.Int() );
            errorCount--;
	        }
	    }
	CleanAllL();
	return errorCount;
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::SendMMTestL
// Calls the method SendMML in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//

TInt CMmsCodecClientApiTest::SendMMTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvDraftEntryId;	
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt errorCount = 2;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    iLog->Log(_L("Calling SendMML with invalid state"));
    TRAPD( error1, iCodecClient->SendMML( messageId,iWait->iStatus ) );
    
    if ( error1 != KErrNone )
        {
        iLog->Log( _L("Sending Message entry failed, left with error: %d"), error1 );
        
        }
    else
        {
        iWait->Start();
         iLog->Log( _L("Sending Message entry  completed with status: %d"), iWait->iStatus.Int() );
         errorCount--;
         }
        
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    TRAPD(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error1);
        
        }
    else
        {
        iWait->Start();
        iLog->Log(_L("AddMMTestL returned %d"),iWait->iStatus.Int());
            
        }
    iLog->Log(_L("Calling SendMML with valid parameters"));
    TRAPD( error2, iCodecClient->SendMML( messageId,iWait->iStatus ) );
    
    if ( error2 != KErrNone )
        {
        iLog->Log( _L("Sending Message entry failed, left with error: %d"), error1 );
        
        }
    else
        {
        iWait->Start();
        iLog->Log( _L("Sending Message entry  completed with status: %d"), iWait->iStatus.Int() );
        errorCount--;
        }
    CleanAllL();	
	return errorCount;	
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::MoveMMTestL
// Calls the method MoveMML in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//

TInt CMmsCodecClientApiTest::MoveMMTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvGlobalInBoxIndexEntryId;
    TMsvId sentFolder = KMsvSentEntryId;	
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt errorCount = 5;
    TInt error;
    CMsvEntry* cEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage2;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage2,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    iLog->Log(_L("Calling MoveMML with invalid state"));
    TRAPD( error1, iCodecClient->MoveMML( messageId,KMsvDraftEntryId,iWait->iStatus ) );
    
    if ( error1 != KErrNone )
        {
        iLog->Log( _L("Moving Message entry failed, left with error: %d"), error1 );
        
        }
    else
        {
        iWait->Start();
		iLog->Log( _L("Moving Message entry  completed with status: %d"), iWait->iStatus.Int() );
		errorCount--;
         }
    iCodecClient->CreateNewMessageEntryL(folder, messageId);     
    TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        iLog->Log(_L("AddMMTestL returned %d"),iWait->iStatus.Int());
           
        }
    iLog->Log(_L("Moving message to Outbox"));
    TRAPD( error2, iCodecClient->MoveMML( messageId,KMsvGlobalOutBoxIndexEntryId,iWait->iStatus ) );
    
    if ( error2 != KErrNone )
        {
        iLog->Log( _L("Moving Message entry failed, left with error: %d"), error1 );
        
        }
    else
        {
        iWait->Start();
         iLog->Log( _L("Moving Message entry  completed with status: %d"), iWait->iStatus.Int() );
         errorCount--;
         }
    iLog->Log(_L("Calling MoveMML with samefolder as parent"));
    TRAPD( error4, iCodecClient->MoveMML( messageId,KMsvGlobalInBoxIndexEntryId,iWait->iStatus ) );
    
    if ( error4 != KErrNone )
        {
        iLog->Log( _L("Moving Message entry failed, left with error: %d"), error1 );
        
        }
    else
        {
        iWait->Start();
         iLog->Log( _L("Moving Message entry  completed with status: %d"), iWait->iStatus.Int() );
         errorCount--;
         }
         
    iLog->Log(_L("Moving Message to Sent folder"));
    TRAPD( error5, iCodecClient->MoveMML( messageId,KMsvSentEntryId,iWait->iStatus ) );
    
    if ( error5 != KErrNone )
        {
        iLog->Log( _L("Moving Message entry failed, left with error: %d"), error5 );
        
        }
    else
        {
        iWait->Start();
         iLog->Log( _L("Moving Message entry  completed with status: %d"), iWait->iStatus.Int() );
         errorCount--;
         }
    iCodecClient->CreateNewMessageEntryL(sentFolder, messageId);
    
    TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, sentFolder, flags, unread, messageId, iWait->iStatus ) );
    
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error1);
        
        }
    else
        {
        iWait->Start();
            iLog->Log(_L("AddMMTestL returned %d"),iWait->iStatus.Int());
            
        }
    iLog->Log(_L("Moving message to Inbox"));
    TRAPD( error3, iCodecClient->MoveMML( messageId,KMsvGlobalInBoxIndexEntryId,iWait->iStatus ) );
    
    if ( error3 != KErrNone )
        {
        iLog->Log( _L("Moving Message entry failed, left with error: %d"), error1 );
        }
    else
        {
        iWait->Start();
         iLog->Log( _L("Moving Message entry  completed with status: %d"), iWait->iStatus.Int() );
         errorCount--;
         }
    CleanupStack::PopAndDestroy(cEntry);   
    CleanAllL();  	
	return errorCount;	
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::SetFlagsTest
// Calls the method SetFlags() in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//

TInt CMmsCodecClientApiTest::SetFlagsTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
	TMsvId nullMessageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvGlobalInBoxIndexEntryId;
    TMsvId outbox = KMsvGlobalOutBoxIndexEntryId;
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt error;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    iLog->Log(_L("Calling SetFlags() with invalid state"));
    error = iCodecClient->SetFlags(messageId, flags, unread );
    iLog->Log(_L("Set Flags returned %d"),error);
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iLog->Log(_L("Calling SetFlags() with invalid state and Null MessageId"));
    error = iCodecClient->SetFlags(nullMessageId, flags, unread );
    iLog->Log(_L("Set Flags returned %d"),error);
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
	
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
        }
    iLog->Log(_L("Calling SetFlags() valid params"));
    error = iCodecClient->SetFlags(messageId, flags, unread );
    iLog->Log(_L("Set Flags returned with %d"),error);
    iCodecClient->CreateNewMessageEntryL(outbox, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, outbox, flags, unread, messageId, iWait->iStatus ) );
	
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
        }
    iLog->Log(_L("Calling SetFlags() for message in OutBox"));
    error = iCodecClient->SetFlags(messageId, flags, unread );
    iLog->Log(_L("Set Flags returned with %d"),error);
    CleanAllL();
    return KErrNone;	
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::InitializeChunkedRetrievingTestL
// Calls the method InitializeChunkedRetrievingL() in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::InitializeChunkedRetrievingTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvGlobalInBoxIndexEntryId;
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt error;
    TInt size = 1;
    TBool lastChunk = EFalse;   
    TPtrC8 dataPart;
    TInt pos = 0;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iCodecClient->NextDataPart( dataPart, lastChunk ); //Calling Next Data Part with invalid state
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);	
    iLog->Log(_L("Calling InitializeChunkedRetrievingL() with invalid state"));
    TRAP( error, iCodecClient->InitializeChunkedRetrievingL( messageId, folder,flags, unread, size, iWait->iStatus ) );
    
    if ( error == KErrNone )
        {
        if ( iWait->iStatus.Int() == KErrNone )
            {
             iLog->Log( _L("InitializeChunkedRetrievingL Failed, should have returned error"));
            }
        else
        	{
             iLog->Log(_L("InitializeChunkedRetrievingL returned error %d"),error);
        	}
        }
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
	
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();	
        iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
        }
    
	TRAP( error, iCodecClient->InitializeChunkedRetrievingL( messageId, folder,flags, unread, size, iWait->iStatus ) );
	
    if ( error == KErrNone )
        {
        iWait->Start();
        if ( iWait->iStatus.Int() == KErrNone )
            {
            while ( !lastChunk && error == KErrNone )
	            {
	            error = iCodecClient->GetNextDataPart( dataPart, lastChunk ); //This is a correct context of GetNextDataPart
	            if ( error != KErrNone )
	                {
	                iLog->Log(_L("GetNextDataPart returned error %d"), error );
	                
	                }
	            if ( dataPart.Length() > 0 && error == KErrNone )
	                {
	                if ( iEncodeBuffer->Size() < pos + dataPart.Length() )
	                    {
	                    iEncodeBuffer->ResizeL( pos + dataPart.Length() );
	                    }
	                iEncodeBuffer->Write( pos, dataPart );
	                pos += dataPart.Length();
	                }
	            error = iCodecClient->ReleaseData();   
	            if ( error != KErrNone )
	                {
	                iLog->Log(_L("ReleaseData returned error %d"), error );
	                }
	            }
             iLog->Log( _L("InitializeChunkedRetrievingL Passed"));
            }
        else
        	{
             iLog->Log(_L("InitializeChunkedRetrievingL returned with status %d"),iWait->iStatus.Int());
        	}
        }
    else
        {
        iLog->Log(_L("InitializeChunkedRetrievingL left with error %d"),error);	
        }
    // The pass case is executed again to Cover the decision " if(iEncodeBuffer)" in CUT
    messageId = KMsvNullIndexEntryId;
    iFilePath = KBinaryMessage;
#ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
#endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);	
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
       	iWait->Start();	
        iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
        }
    
	TRAP( error, iCodecClient->InitializeChunkedRetrievingL( messageId, folder,flags, unread, size, iWait->iStatus ) );
	
    if ( error == KErrNone )
        {
        iWait->Start();
        if ( iWait->iStatus.Int() == KErrNone )
            {
            while ( !lastChunk && error == KErrNone )
	            {
	            error = iCodecClient->GetNextDataPart( dataPart, lastChunk ); //This is a correct context of GetNextDataPart
	            if ( error != KErrNone )
	                {
	                iLog->Log(_L("GetNextDataPart returned error %d"), error );
	                
	                }
	            if ( dataPart.Length() > 0 && error == KErrNone )
	                {
	                if ( iEncodeBuffer->Size() < pos + dataPart.Length() )
	                    {
	                    iEncodeBuffer->ResizeL( pos + dataPart.Length() );
	                    }
	                iEncodeBuffer->Write( pos, dataPart );
	                pos += dataPart.Length();
	                }
	            error = iCodecClient->ReleaseData();   
	            if ( error != KErrNone )
	                {
	                iLog->Log(_L("ReleaseData returned error %d"), error );
	                }
	            }
             iLog->Log( _L("InitializeChunkedRetrievingL Passed"));
            }
        else
        	{
             iLog->Log(_L("InitializeChunkedRetrievingL returned with status %d"),iWait->iStatus.Int());
        	}
        }
    else
        {
        iLog->Log(_L("InitializeChunkedRetrievingL left with error %d"),error);	
        }
    CleanAllL();
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::DiskSpaceBelowCriticalLevelTestL
// Calls the method DiskSpaceBelowCriticalLevelL() in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//

TInt CMmsCodecClientApiTest::DiskSpaceBelowCriticalLevelTestL()
	{
    iCodecClient->DiskSpaceBelowCriticalLevelL(KMmsIndexEntryExtra);
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::InitializeChunkedAddingTestL
// Calls the method InitializeChunkedAddingL() in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::InitializeChunkedAddingTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvGlobalInBoxIndexEntryId;
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt error;
    TBool lastChunk = EFalse;   
    TPtrC8 dataPart;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
    iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
   #endif		
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);	
    iLog->Log(_L("Calling InitializeChunkedAddingL() with invalid state"));
    TRAP( error, iCodecClient->InitializeChunkedAddingL( KMsvDraftEntryId, messageId, flags, unread ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedAddingL Failed, should have returned error"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedAddingL returned error %d"),error);
    	}
        
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            //returnValue = iWait->iStatus.Int();
            iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
            }
        }
	TRAP( error, iCodecClient->InitializeChunkedAddingL( KMsvDraftEntryId, messageId, flags, unread ) );
	error = FeedDataInChunks( iCodecClient );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedAddingL passed"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedAddingL returned error %d"),error);
    	}
    TInt error1 = iCodecClient->GetNextDataPart( dataPart, lastChunk ); //Calling GetNextDataPart out of context...
    error1 = iCodecClient->ReleaseData(); //Calling Release Data Out of Context....
    
    // The pass case is executed again to Cover the decision " if(iEncodeBuffer)" in CUT
    messageId = KMsvNullIndexEntryId;
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            //returnValue = iWait->iStatus.Int();
            iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
            }
        }
	TRAP( error, iCodecClient->InitializeChunkedAddingL( KMsvDraftEntryId, messageId, flags, unread ) );
	error = FeedDataInChunks( iCodecClient );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedAddingL passed"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedAddingL returned error %d"),error);
    	}
    CleanAllL();
	return error;	
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::InitializeChunkedReplacingTestL
// Calls the method InitializeChunkedReplacingL() in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//
TInt CMmsCodecClientApiTest::InitializeChunkedReplacingTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvGlobalInBoxIndexEntryId;
    TMsvId outBox = KMsvGlobalOutBoxIndexEntryId;
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt error;
    TPtrC8 dataPart;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__  
	 iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
    #endif	 
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);	
    iLog->Log(_L("Calling InitializeChunkedReplacingL() with invalid state"));
    TRAP( error, iCodecClient->InitializeChunkedReplacingL(messageId, flags, unread ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedReplacingL Failed, should have returned error"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedReplacingL returned error %d"),error);
    	}
    iLog->Log(_L("Calling InitializeChunkedReplacingL() to replace message in Outbox"));   
    iCodecClient->CreateNewMessageEntryL(outBox, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, outBox, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            //returnValue = iWait->iStatus.Int();
            iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
            }
        }
	TRAP( error, iCodecClient->InitializeChunkedReplacingL( messageId, flags, unread ) );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedReplacingL Failed, should have returned error"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedReplacingL returned error %d"),error);
    	}
    iLog->Log(_L("Calling InitializeChunkedReplacingL() with valid parameters")); 
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            //returnValue = iWait->iStatus.Int();
            iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
            }
        }
	TRAP( error, iCodecClient->InitializeChunkedReplacingL( messageId, flags, unread ) );
	error = FeedDataInChunks( iCodecClient );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedReplacingL Passed"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedReplacingL returned error %d"),error);
    	}
    // The pass case is executed again to Cover the decision " if(iEncodeBuffer)" in CUT
    messageId = KMsvNullIndexEntryId;
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            //returnValue = iWait->iStatus.Int();
            iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
            }
        }
	TRAP( error, iCodecClient->InitializeChunkedReplacingL( messageId, flags, unread ) );
	error = FeedDataInChunks( iCodecClient );
    if ( error == KErrNone )
        {
        iLog->Log( _L("InitializeChunkedReplacingL Passed"));
        }
    else
    	{
         iLog->Log(_L("InitializeChunkedReplacingL returned error %d"),error);
    	}
    CleanAllL();
    return error;
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::DeleteMMTestL
// Calls the method DeleteMML() in CMmsCodecClient class.
// -----------------------------------------------------------------------------
//    
TInt CMmsCodecClientApiTest::DeleteMMTestL()
	{
	TMsvId messageId = KMsvNullIndexEntryId;
    TMsvId folder = KMsvGlobalInBoxIndexEntryId;
    TUint32 flags = 0;
    TBool unread = EFalse;
    TInt error = KErrNone;
    TPtrC8 dataPart;
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
    iFilePath = KBinaryMessage;
    #ifndef __WINSCW__	
	 iFilePath.Format(KBinaryMessage,69);/*69 Repesents E*/
    #endif	 
    FillBufferFromFileL( iFilePath,iFs,iEncodeBuffer);	
    iLog->Log(_L("Calling DeleteMM with invalid state"));
    iCodecClient->DeleteMM(messageId);
    if ( error == KErrNone )
        {
        iLog->Log( _L("DeleteMM Failed, should have left with error"));
        }
    else
    	{
         iLog->Log(_L("DeleteMM returned error %d"),error);
    	}
    iLog->Log(_L("Calling DeleteMM with valid parameters"));   
    iCodecClient->CreateNewMessageEntryL(folder, messageId);
	TRAP(error, iCodecClient->AddMML(*iEncodeBuffer, folder, flags, unread, messageId, iWait->iStatus ) );
    if(error != KErrNone)
        {
        iLog->Log(_L("AddMMTestL returned error %d "),error);
        }
    else
        {
        iWait->Start();
        if(iWait->iStatus.Int() == KErrNone || iWait->iStatus.Int() == KErrNotSupported)
            {
            //returnValue = iWait->iStatus.Int();
            iLog->Log(_L("AddMMTestL returned with %d"),iWait->iStatus.Int());
            }
        }
	error = iCodecClient->DeleteMM( messageId);
    if ( error == KErrNone )
        {
        iLog->Log( _L("DeleteMM Passed"));
        }
    else
    	{
         iLog->Log(_L("DeleteMM returned error %d"),error);
    	}
    CleanAllL();	
	return error;	
	}
// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::FeedDataInChunks
// Method used to retrieve the data in chunks.
// -----------------------------------------------------------------------------
//	
TInt CMmsCodecClientApiTest::FeedDataInChunks( CMmsCodecClient* aCodecClient )
    {
    
    TInt error = KErrNone;
    TInt chunkSize = KMmsCodecClientChunkSize;
    TInt size = iEncodeBuffer->Size();
    
    // Number of full chunks
    TInt chunkNumber = size / chunkSize;
    TInt remainder = size % chunkSize;
    
    TInt pos = 0;    
    TPtrC8 messagePtr( iEncodeBuffer->Ptr( 0 ) );
    
    TBool lastChunk = EFalse;
    TInt i = 0;
    for ( i = 0; i < chunkNumber && error == KErrNone; i++ )
        {
        pos = i * chunkSize;
        TPtrC8 ptr( iEncodeBuffer->Ptr( 0 ).Mid( pos, chunkSize ) );
        error = aCodecClient->NextDataPart( ptr, lastChunk );
        }
        
    // last chunk

    if ( remainder > 0 && error == KErrNone )
        {
        TPtrC8 ptr2( iEncodeBuffer->Ptr( size - remainder ) );
        if ( ptr2.Length() > 0 )
            {
            error = aCodecClient->NextDataPart( ptr2, lastChunk );
            }
        }
        
    lastChunk = ETrue;
    TPtrC8 ptr3;
    if ( error == KErrNone )
        {
        aCodecClient->NextDataPart( ptr3, lastChunk );
        }
    return error;    
    }

void CMmsCodecClientApiTest::FillBufferFromFileL(const TDesC& aFilePath,RFs& aFs,CBufFlat* aEncodeBuffer)
    {
    TInt error = KErrNone;
    TEntry orgEntry;
    error = aFs.Entry( aFilePath, orgEntry );
    TInt size = orgEntry.iSize;
    RFile inFile;
        
    if ( aEncodeBuffer == NULL )
        {
        aEncodeBuffer = CBufFlat::NewL( size );
        }
    else
        {
        aEncodeBuffer->ResizeL( 0 );
        aEncodeBuffer->ResizeL( size );
        }

    error = inFile.Open( aFs, aFilePath, EFileShareReadersOnly );
    TPtr8 ptr = aEncodeBuffer->Ptr( 0 );
    if ( error == KErrNone )
        {
        error = inFile.Read( ptr, size );
        inFile.Close();
        }
    else
        {
        iLog->Log(_L("*** - can't read file"));
        }
    User::LeaveIfError( error ); 
    }
void CMmsCodecClientApiTest::CleanAllL()
	{

	iLog->Log(_L("In Clean All"));
	CleanBoxL(KMsvGlobalInBoxIndexEntryId,*iSession);
    CleanBoxL(KMsvGlobalOutBoxIndexEntryId,*iSession);
    CleanBoxL(KMsvSentEntryId,*iSession);
    CleanBoxL(KMsvDraftEntryId,*iSession);
    iLog->Log(_L("End of Clean All"));
    	
	}
void CMmsCodecClientApiTest::CleanBoxL(TMsvId aBoxId, CMsvSession& aSession)
    {
    iLog->Log(_L("In CleanBoxL"));
    CMsvEntry* cEntry = NULL;
    // delete all messages from the specified box
    cEntry = aSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(aBoxId);
    // show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
    CleanupStack::PushL(msvEntrySelection);

    CMsvEntrySelection* selection = NULL;
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    selection = cEntry->ChildrenWithMtmL(KUidMsgMMSNotification);
    CleanupStack::PushL( selection );
    if ( selection->Count() > 0 )
        {
        msvEntrySelection->AppendL( selection->Back( 0 ), selection->Count() );
        }
    CleanupStack::PopAndDestroy(); // selection

    int i;
    for (i = 0; i < msvEntrySelection->Count(); ++i)
        {
        CMsvEntry* entry = aSession.GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly(EFalse);
        entry->ChangeL(tEntry);
        cEntry->DeleteL( msvEntrySelection->At(i) );
        CleanupStack::PopAndDestroy( entry );
        }

    CleanupStack::PopAndDestroy(msvEntrySelection);
    CleanupStack::PopAndDestroy(cEntry);
    iLog->Log(_L("End of CleanBoxL"));
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
