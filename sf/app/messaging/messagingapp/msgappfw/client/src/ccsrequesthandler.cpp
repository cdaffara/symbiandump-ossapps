/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is the client side interface for the conversation
*               server.
*
*/


// INCLUDE FILES

// SYSTEM INCLUDES
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
#include <mcsresultsobserver.h>
#include <mcsconversationlistchangeobserver.h>
#include <mcsconversationchangeobserver.h>
#include <mcscachingstatusobserver.h>
#include <ccsrequesthandler.h>
#include <rcssession.h>
#include <ccsnotificationhandler.h>

//USER INCLUDES
#include "ccsdebug.h"
#include "ccsconversationchangeobserver.h"

const TInt  KSmallIpcBuffer = 256; //256 bytes
const TInt KBigIpcBuffer = 2048; // 2K

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSRequestHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CCSRequestHandler* CCSRequestHandler::NewL()
    {
    PRINT ( _L("Enter CCSRequestHandler::NewL") );

    CCSRequestHandler* self = NewLC( );
    CleanupStack::Pop( self );

    PRINT ( _L("End CCSRequestHandler::NewL") );

    return( self ) ;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CCSRequestHandler* CCSRequestHandler::NewLC()
    {
    PRINT ( _L("Enter CCSRequestHandler::NewLC") );

    CCSRequestHandler* self = new ( ELeave ) CCSRequestHandler();
    CleanupStack::PushL( self );
    self->ConstructL();

    PRINT ( _L("End CCSRequestHandler::NewLC") );

    return self;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::CCSAsyncRequestHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CCSRequestHandler::CCSRequestHandler()
: CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CCSRequestHandler::ConstructL()
    {
    PRINT ( _L("Enter CCSRequestHandler::ConstructL") );

    User::LeaveIfError( iSession.Connect() );
    
    // Notification handler
    iNotificationHandler = CCsNotificationHandler::NewL(this);
    
    // Observers
    iResultsObserver = NULL;
    iCachingStatusObserver = NULL;
    iConversationListChangeObserver = NULL;
    iConversationChangeObserver = NULL;

    PRINT ( _L("End CCSRequestHandler::ConstructL") );
    }


// -----------------------------------------------------------------------------
// CCSRequestHandler::~CCSRequestHandler()
// Destructor.
// -----------------------------------------------------------------------------
CCSRequestHandler::~CCSRequestHandler()
    {
    PRINT ( _L("Enter CCSRequestHandler::~CCSRequestHandler") );

    Cancel(); 

    // Cleanup
    if ( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
   
    if ( iResultsBuffer )
        {
        delete iResultsBuffer;
        iResultsBuffer = NULL;
        }
    
    if ( iNotificationHandler )
        {
        delete iNotificationHandler;
        iNotificationHandler = NULL;
        }
    
    if( iListResultsBuffer )
        {
        delete iListResultsBuffer;
        iListResultsBuffer=NULL;
        }
    if( iConvResultsBuffer )
        {
        delete iConvResultsBuffer;
        iConvResultsBuffer=NULL;
        }
    
    // Close the session
    iSession.Close();

    PRINT ( _L("End CCSRequestHandler::~CCSRequestHandler") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetConversationListResults()
// This shall handle all entry list result from server
//
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationListResults()
    {
    PRINT( _L("Enter CCSRequestHandler::HandleGetConversationListResults") );
    
    TInt error = KErrNone;
    RPointerArray<CCsClientConversation> clientConversationList;

    // Parse the results
    RDesReadStream resultStream(iListResultsBuffer->Des());
    TInt listCount = 0;

    TRAP(error, 
            resultStream.PushL();
            listCount = resultStream.ReadUint16L();
            resultStream.Pop()
            );

    if(error != KErrNone)
        {
        //call panic
        }
    
    // Speed up the allocation
    if( listCount > 0 )
        {
        clientConversationList.Reserve(listCount);
        }

    for (TInt iloop = 0 ; iloop < listCount; iloop++)
        {
        TRAP(error,
                resultStream.PushL();
                CCsClientConversation* clientConversation = CCsClientConversation::NewL();
                CleanupStack::PushL(clientConversation);
                clientConversation->InternalizeL(resultStream);
                clientConversationList.AppendL(clientConversation);
                CleanupStack::Pop(clientConversation);
                resultStream.Pop()
                );

        if(error != KErrNone)
            {
            //call panic
            }
        }
    
    // Clear all the internal buffers
    // Cleanup
    delete iListResultsBuffer;
    iListResultsBuffer = NULL;
        
    resultStream.Close();

    // Pass the results to the observer    
    if ( iResultsObserver )
        {
        iResultsObserver->ConversationList(clientConversationList);
        }
    
    clientConversationList.ResetAndDestroy();
    
    PRINT( _L("End CCSRequestHandler::HandleGetConversationListResults") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetConversationResults()
// This shall handle GetCoversation results async from server
//
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationResults()
    {
    PRINT( _L("Enter CCSRequestHandler::HandleGetConversationResults") );

    TInt error = KErrNone;

    RPointerArray<CCsConversationEntry> ConversationEntryList;
    TInt totalConversationCount(0);
    
    // Parse the results
    RDesReadStream resultStream(iConvResultsBuffer->Des());
    TInt  conversationEntryCount = 0;

    TRAP(error, resultStream.PushL();

    conversationEntryCount = resultStream.ReadInt32L();

    //Read total conversation count
   totalConversationCount = resultStream.ReadInt32L();

    resultStream.Pop());

    if(error != KErrNone)
        {
        //call panic
        }
    // Speed up the allocation
    if( conversationEntryCount > 0 )
        {
        ConversationEntryList.Reserve(conversationEntryCount);
        }
    
    PRINT1 ( _L("Number of conversation entries = %d"), conversationEntryCount );

    // conversation entries
    for ( TInt iloop = 0; iloop < conversationEntryCount; iloop++ )
        {
        TRAP(error,
                resultStream.PushL();

        CCsConversationEntry* conversationEntry = CCsConversationEntry::NewL();
        CleanupStack::PushL(conversationEntry);

        conversationEntry->InternalizeL(resultStream);
        ConversationEntryList.AppendL(conversationEntry);

        CleanupStack::Pop(conversationEntry);
        resultStream.Pop());

        if(error != KErrNone)
            {
            //call panic
            }
        }

    // Cleanup
    if ( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
    delete iConvResultsBuffer;
    iConvResultsBuffer = NULL;
        
    resultStream.Close();

    // Pass the results to the observer
    if ( iResultsObserver )
        {
        iResultsObserver->Conversations(ConversationEntryList, totalConversationCount);
        }
        
    // cleanup heap data
    ConversationEntryList.ResetAndDestroy();

    PRINT_TIMESTAMP ("End CCSRequestHandler::HandleGetConversationResults");
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetEntryListOverflow()
// Handle the buffer overflow error for get entry list results
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationListOverflow()
    {
    PRINT ( _L("Enter CCSRequestHandler::HandleGetConversationListOverflow") );

	if ( !iListResultsBuffer )
        {
		return;
		}
    TInt error = KErrNone;

    // New buffer size is now stored in results buffer
    RDesReadStream stream(iListResultsBuffer->Des());
    TInt bufferSize = 0;

    TRAP(error, 
            stream.PushL();
            bufferSize = stream.ReadInt32L();
            stream.Pop()
            );

    if(error != KErrNone)
        {
        //call panic
        }

    stream.Close();

    // Delete and recreate the results buffer
    if ( iListResultsBuffer)
        {
        delete iListResultsBuffer;
        iListResultsBuffer = NULL;
        }

    // Buffer created for the new size
    TRAP(error, 
            iListResultsBuffer = HBufC8::NewL(bufferSize);
            iSession.GetConversationListL(iListResultsBuffer->Des(),iStatus)
            );

    if(error != KErrNone)
        {
        //call panic
        }
    SetActive();
    
    PRINT ( _L("End CCSRequestHandler::HandleGetEntryListOverflow") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetConversationOverflow()
// Handle the buffer overflow error for get conversation results
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationOverflow()
    {
    PRINT ( _L("Enter CCSRequestHandler::HandleGetConversationOverflow") );

	   if ( !iConvResultsBuffer )
        {
				return;
				}
    TInt error = KErrNone;

    // New buffer size is now stored in results buffer
    RDesReadStream stream(iConvResultsBuffer->Des());
    TInt bufferSize = 0;

    // Read the buffer size and create a new buffer
    TRAP(error, 
            stream.PushL();
            bufferSize = stream.ReadInt32L();
            stream.Pop()
            );

    if(error != KErrNone)
        {
        //call panic
        }

    stream.Close();

    // Delete and recreate the results buffer
    if ( iConvResultsBuffer )
        {
        delete iConvResultsBuffer;
        iConvResultsBuffer = NULL;
        }

    // Buffer created for the new size
    TRAP(error, 
            iConvResultsBuffer = HBufC8::NewL(bufferSize);
            iSession.SendNewBufferGetConversationL(iConvResultsBuffer->Des(),iStatus)
            );

    if(error != KErrNone)
        {
        //call panic
        }

    SetActive();
    
    PRINT ( _L("End CCSRequestHandler::HandleGetConversationListOverflow") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestResultsEventL()
// Add conversation result Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestResultsEventL(
        MCsResultsObserver* aObserver)
    {
    iResultsObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveResultsEventL()
// Remove conversation result observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveResultsEventL
(MCsResultsObserver* /*aObserver*/)
    {
    iResultsObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestConversationListChangeEventL()
// Add conversation list change event Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestConversationListChangeEventL(
        MCsConversationListChangeObserver* aObserver)
    {
    iConversationListChangeObserver = aObserver;
    
    // Register to server.
    iNotificationHandler->RequestConversationListChangeEventL();  
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveConversationListChangeEventL()
// Remove conversation list change event Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveConversationListChangeEventL(
        MCsConversationListChangeObserver* /*aObserver*/)
    {
    // Before removing observer, make sure it was added earlier.
    if( iConversationListChangeObserver )
        {
        iConversationListChangeObserver = NULL;
    
        // De-register from the server           
        iNotificationHandler->RemoveConversationListChangeEventL();  
        }
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestConversationChangeEventL()
// Add conversation change event Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestConversationChangeEventL(
        MCsConversationChangeObserver* aObserver,
        CCsClientConversation* aClientConversation)
    {
    CCsConversationChangeObserver* conChgObserver = 
            CCsConversationChangeObserver::NewL();
    CleanupStack::PushL(conChgObserver);
    conChgObserver->SetClientConversationL(*aClientConversation);
    conChgObserver->SetConversationChangeObserverL(aObserver);
    iConversationChangeObserver = conChgObserver;
    CleanupStack::Pop(conChgObserver);

    iNotificationHandler->RequestConversationChangeEventL(aClientConversation);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveConversationChangeEventL()
// Remove conversation change event observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveConversationChangeEventL(
        MCsConversationChangeObserver* /*aObserver*/,
        CCsClientConversation* aClientConversation)
    {
    if ( iConversationChangeObserver )
        {
        delete iConversationChangeObserver;
        iConversationChangeObserver = NULL;
        if(aClientConversation)
            {
            iNotificationHandler->RemoveConversationChangeEventL(aClientConversation);
            }
         }
    }
// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestCachingStatusEventL()
// Add caching status Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestCachingStatusEventL(
        MCsCachingStatusObserver* /*aObserver*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveCachingStatusEventL()
// Remove caching status Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveCachingStatusEventL(
        MCsCachingStatusObserver* /*aObserver*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
void CCSRequestHandler::RunL()
    { 
    switch ( iStatus.Int() )
        {
        case EGetConversationListOperationComplete:
            HandleGetConversationListResults();
            break;

        case EGetConversationListBufferOverflow:
            HandleGetConversationListOverflow();
            break;

        case EGetConversationOperationComplete:
            HandleGetConversationResults();
            break;

        case EGetConversationBufferOverflow:
            HandleGetConversationOverflow();
            break;

        default:
            HandleErrorL(iStatus.Int());
            break;
        }
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleErrorL()
// Send the error code to the client.
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleErrorL(TInt /*aErrorCode*/)
    {
    // Not supported
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
void CCSRequestHandler::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::Version()
// Recovers the conversation server version.
// -----------------------------------------------------------------------------
EXPORT_C TVersion CCSRequestHandler::Version() const
    {
    return (iSession.Version());
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::ShutdownServerL()
// Shuts down the conversation server.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::ShutdownServerL()
    {
    return (iSession.ShutdownServerL());
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationListL()
// Get Recent Conversation Entry list with display name
// for all stored conversation entry IDs.
// This API can be used to prepare conversation list view.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetConversationListL()
    {    
    PRINT( _L("Enter CCSRequestHandler::GetConversationListL") );

    // ------------------------- Results Buffer ---------------------

    // Create a buffer to store the search results.
    if(iListResultsBuffer)
        {
        delete iListResultsBuffer;
        iListResultsBuffer = NULL;
        }
    
	// coverity[size_error][buffer_alloc]
	iListResultsBuffer = HBufC8::NewL(KBufferMaxLen);
    
    // Send the ASYNC request
    iSession.GetConversationListL(iListResultsBuffer->Des(), 
            iStatus);
    SetActive();

    PRINT( _L("End CCSRequestHandler::GetConversationListL") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationUnreadListL()
// Get Recent unread Conversation Entry list with display name
// for all stored conversation entry IDs.
// This API can be used to prepare conversation list view.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetConversationUnreadListL(RPointerArray<
        CCsClientConversation>* aClientConversationList)
    {
    PRINT( _L("Enter CCSRequestHandler::GetConversationUnreadListL") );

    TInt resultBufferSize = KBufferMaxLen;
    TInt isOverflow = EFalse;
    TInt error = KErrNone;

    HBufC8* overflow = HBufC8::NewL(sizeof(TInt));
    CleanupStack::PushL(overflow);

    do
        {

        // 1. Create fresh Result buffer.
        if (iResultsBuffer)
            {
            delete iResultsBuffer;
            iResultsBuffer = NULL;
            }
        iResultsBuffer = HBufC8::NewL(resultBufferSize);

        // 2. Send the SYNC request.
        iSession.GetConversationUnreadListL(overflow->Des(),
                iResultsBuffer->Des());

        // 3. Read Overflow status.
        RDesReadStream overflowStream(overflow->Des());

        TRAP(error,
                overflowStream.PushL();
                isOverflow = overflowStream.ReadUint16L();
                overflowStream.Pop());

        overflowStream.Close();

        // 4. If Overflow, reset the result buffer size.
        if (isOverflow)
            {
            // New buffer size is now stored in results buffer
            RDesReadStream stream(iResultsBuffer->Des());

            TRAP(error,
                    stream.PushL();
                    resultBufferSize = stream.ReadInt32L();
                    stream.Pop());

            stream.Close();
            }
        }
    while (isOverflow);

    CleanupStack::PopAndDestroy(overflow);

    // 5. No Overflow, Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    TInt listCount = 0;

    TRAP(error,
            resultStream.PushL();
            listCount = resultStream.ReadUint16L();
            resultStream.Pop()
    );

    for (TInt iloop = 0; iloop < listCount; iloop++)
        {
        TRAP(error,
                resultStream.PushL();
                CCsClientConversation* clientConversation = CCsClientConversation::NewL();
                CleanupStack::PushL(clientConversation);
                clientConversation->InternalizeL(resultStream);
                aClientConversationList->AppendL(clientConversation);
                CleanupStack::Pop(clientConversation);
                resultStream.Pop());
        }

    // Clear all the internal buffers
    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Close();

    PRINT( _L("End CCSRequestHandler::GetConversationUnreadListL") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationsL()
// Get Conversation Entry list for given Conversation Entry ID to prepare
// convresation view.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetConversationsL(
        CCsClientConversation*  aClientConversation,
        TInt aKnownIndex, 
        TInt aPageSize)
    {
    PRINT( _L("Enter CCSRequestHandler::GetConversationsL") );

    // check if the aclientconversation is null then return
    if ( aClientConversation == NULL)
        {
        return;
        }

    //-------------------Contact-----------------------------

    // Tmp buffer
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();
    
    //Write requested details in buffer.
    dataStream.WriteInt32L(aKnownIndex);
    dataStream.WriteInt32L(aPageSize);
    
    // Write the Client Conversation in the stream
    aClientConversation->ExternalizeL(dataStream);
    dataStream.CommitL();
    
    //------------------Input Buffer--------------------------
    // Create a HBufC8 for IPC
    iBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(iBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    CleanupStack::PopAndDestroy(2, dataBuf);

    // ------------------------- Results Buffer ---------------------

    // Create a buffer to store the search results.
    if(iConvResultsBuffer)
        {
        delete iConvResultsBuffer;
        iConvResultsBuffer = NULL;
        }
    iConvResultsBuffer = HBufC8::NewL(KBufferMaxLen);

    // Send the ASYNC request
    iSession.GetConversationsL(iBuffer->Des(),
                               iConvResultsBuffer->Des(),
                               iStatus);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetCachingStatusL()
// Get caching status
// Synchronous
// -----------------------------------------------------------------------------
EXPORT_C TUint8 CCSRequestHandler::GetCachingStatusL()
    {
    PRINT ( _L("Enter CCSRequestHandler::GetCachingStatusL") );
    
    // ------------------------- Results Buffer ---------------------
    // Create a buffer to store the results.
    if(iResultsBuffer)
        {
        delete iResultsBuffer;
        iResultsBuffer = NULL;
        }
    iResultsBuffer = HBufC8::NewL(KBufferMaxLen);

    // --------------------------------------------------------------

    // Send the request
    iSession.GetCachingStatusL(iResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    resultStream.PushL();

    TUint8 cachestatus = resultStream.ReadUint8L();

    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Pop();
    resultStream.Close();

    PRINT( _L("End CCSRequestHandler::GetCachingStatusL") );
    return cachestatus;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetTotalUnreadCountL()
// Get total unread count
// Synchronous
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCSRequestHandler::GetTotalUnreadCountL()
    {
    PRINT ( _L("Enter CCSRequestHandler::GetTotalUnreadCountL") );

    // ------------------------- Results Buffer ---------------------
    // Create a buffer to store the results.
    if(iResultsBuffer)
        {
        delete iResultsBuffer;
        iResultsBuffer = NULL;
        }
    iResultsBuffer = HBufC8::NewL(KBufferMaxLen);

    // --------------------------------------------------------------

    // Send the request
    iSession.GetTotalUnreadCountL(iResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    resultStream.PushL();

    TUint32 unreadCount = resultStream.ReadUint32L();

    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Pop();
    resultStream.Close();

    PRINT( _L("End CCSRequestHandler::GetTotalUnreadCountL") );
    return unreadCount;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleAddConversationList()
// Process add conversation list event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleAddConversationList(HBufC8* aResultsBuffer)
    {
    PRINT( _L("Enter CCSRequestHandler::HandleAddConversationList") );

    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }

    // Cleanup
    resultStream.Close();

    // Pass the results to the observer
    if ( iConversationListChangeObserver )
        {
        iConversationListChangeObserver->
        AddConversationList(*clientConversation);
        }

    delete clientConversation;
    
    PRINT( _L("End CCSRequestHandler::HandleAddConversationList") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleDeleteConversationList()
// Process delete conversation lsit event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleDeleteConversationList(HBufC8* aResultsBuffer)
    {
    PRINT( _L("Enter CCSRequestHandler::HandleDeleteConversationList") );

    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }

    // Cleanup
    resultStream.Close();

    // Pass the results to the observer
    if ( iConversationListChangeObserver )
        {
        iConversationListChangeObserver->
        DeleteConversationList(*clientConversation);
        }
        
    delete clientConversation;

    PRINT( _L("End CCSRequestHandler::HandleDeleteConversationList") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandlePartialDeleteConversationList()
// Process partial delete conversation lsit event received from server
// -----------------------------------------------------------------------------

void CCSRequestHandler::HandlePartialDeleteConversationList(HBufC8* aResultsBuffer)
{
    PRINT( _L("Enter CCSRequestHandler::HandlePartialDeleteConversationList") );

    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }

    // Cleanup
    resultStream.Close();
    
    // Pass the results to the observer
    if ( iConversationListChangeObserver )
        {
        CleanupStack::PushL( clientConversation );
        iConversationListChangeObserver->
        PartialDeleteConversationList(*clientConversation);
        }
    
    CleanupStack::PopAndDestroy();// clientConversation*/

    PRINT( _L("End CCSRequestHandler::HandlePartialDeleteConversationList") );
}

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleModifyConversationList
// Process modify conversation lsit event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleModifyConversationList(HBufC8* aResultsBuffer)
    {
    PRINT( _L("Enter CCSRequestHandler::HandleModifyConversationList") );
    
    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }
    
    // Cleanup
    resultStream.Close();

    // Pass the results to the observer
    if ( iConversationListChangeObserver )
        {
        iConversationListChangeObserver->
        ModifyConversationList(*clientConversation);
        }
        
    delete clientConversation;

    PRINT( _L("End CCSRequestHandler::HandleModifyConversationList") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleAddConversation
// Process add conversation event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleAddConversation(HBufC8* aResultsBuffer)
    {
    PRINT( _L("Start CCSRequestHandler::HandleAddConversation") );

    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }

    // Cleanup
    resultStream.Close();

    // Pass the results to the observer
    if ( iConversationChangeObserver )
        {
        MCsConversationChangeObserver* observer = 
                iConversationChangeObserver->ConversationChangeObserver();
        observer->AddConversation(*(clientConversation->
                GetConversationEntry()));
        }
     
    delete clientConversation;

    PRINT( _L("End CCSRequestHandler::HandleAddConversation") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleDeleteConversation
// Process delete conversation event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleDeleteConversation(HBufC8* aResultsBuffer)
    {
    PRINT( _L("Start CCSRequestHandler::HandleDeleteConversation") );

    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }

    // Cleanup
    resultStream.Close();

    // Pass the results to the observer
    if ( iConversationChangeObserver )
        {
        MCsConversationChangeObserver* observer = 
                iConversationChangeObserver->ConversationChangeObserver();
        observer->DeleteConversation(*(clientConversation->
                GetConversationEntry()));
        }
    
    delete clientConversation;

    PRINT( _L("End CCSRequestHandler::HandleDeleteConversation") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleModifyConversation
// Process Modify conversation event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleModifyConversation(HBufC8* aResultsBuffer)
    {
    PRINT( _L("Start CCSRequestHandler::HandleModifyConversation") );
    TInt error = KErrNone;

    // perpare client conversation
    CCsClientConversation* clientConversation = NULL;
    RDesReadStream resultStream(aResultsBuffer->Des());

    TRAP(error,
        resultStream.PushL();
        clientConversation = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConversation);
        // Parse the results
        clientConversation->InternalizeL(resultStream);
        CleanupStack::Pop(clientConversation);
        resultStream.Pop());

    if ( error != KErrNone )
        {
        // Ignore this conversation
        }

    // Cleanup
    resultStream.Close();

    // Pass the results to the observer
    if ( iConversationChangeObserver )
        {
        MCsConversationChangeObserver* observer = 
                iConversationChangeObserver->ConversationChangeObserver();
        observer->ModifyConversation(*(clientConversation->
                GetConversationEntry()));
        }
    
    delete clientConversation;

    PRINT( _L("End CCSRequestHandler::HandleModifyConversation") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleCachingStarted
// Process caching started event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleCachingStarted(HBufC8* /*aResultsBuffer*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleCachingCompleted
// Process caching completed event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleCachingCompleted(HBufC8* /*aResultsBuffer*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleCachingError
// Process caching error event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleCachingError(HBufC8* /*aResultsBuffer*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::DeleteConversationL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::DeleteConversationL(TInt aConversationId)        
    {
    PRINT( _L("Enter CCSRequestHandler::DeleteConversationL") );
    iSession.DeleteConversationL(aConversationId);
    PRINT( _L("End CCSRequestHandler::DeleteConversationL") );
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleRefreshConversationList
// Process refresh from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleRefreshConversationList(HBufC8* /*aResultsBuffer*/)
    {
    // Pass the results to the observer
    if ( iConversationListChangeObserver )
       {
       iConversationListChangeObserver->RefreshConversationList();
       }
    }

// -----------------------------------------------------------------------------
// Ccsrequesthandler::HandleRefreshConversationList
// Process refresh from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleRefreshConversation(HBufC8* /*aResultsBuffer*/)
    {
    // Pass the results to the observer
    if ( iConversationChangeObserver )
       {
       MCsConversationChangeObserver* observer = 
               iConversationChangeObserver->ConversationChangeObserver();
       observer->RefreshConversation();
       }
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationIdL()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCSRequestHandler::GetConversationIdL(TInt aContactId)        
    {    
    TInt conversationId = -1;
    
    // Create a buffer to store the results.
    if(iResultsBuffer)
      {
      delete iResultsBuffer;
      iResultsBuffer = NULL;
      }
    iResultsBuffer = HBufC8::NewL(KSmallIpcBuffer);
      
    // Send the request
    iSession.GetConversationIdL(aContactId, iResultsBuffer->Des());
    
    // Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    resultStream.PushL();     
    conversationId = resultStream.ReadInt32L();

    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Pop();
    resultStream.Close();
    
    return conversationId;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationIdFromAddressL()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCSRequestHandler::GetConversationIdFromAddressL(TDesC& aContactAddress)        
    {    
    TInt conversationId = -1;
    
    // Create a buffer to store the results.
    if(iResultsBuffer)
      {
      delete iResultsBuffer;
      iResultsBuffer = NULL;
      }
    iResultsBuffer = HBufC8::NewL(KSmallIpcBuffer);
      
    // Send the request
    iSession.GetConversationIdFromAddressL(aContactAddress,iResultsBuffer->Des());            
    
    // Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    resultStream.PushL();     
    conversationId = resultStream.ReadInt32L();

    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Pop();
    resultStream.Close();
    
    return conversationId;
    }
	
// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationFromConversationIdL()
// -----------------------------------------------------------------------------
EXPORT_C CCsClientConversation* CCSRequestHandler::GetConversationFromConversationIdL(TInt aConversationId)
{
    // Create a buffer to store the results.
    if(iResultsBuffer)
    {
        delete iResultsBuffer;
        iResultsBuffer = NULL;
    }
    iResultsBuffer = HBufC8::NewL(KBigIpcBuffer);

    // Send the request
    iSession.GetConversationFromConversationIdL(aConversationId, iResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    resultStream.PushL();
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    CleanupStack::PushL(clientConversation);
    clientConversation->InternalizeL(resultStream);
    CleanupStack::Pop(clientConversation);

    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Pop();
    resultStream.Close();

    return clientConversation;
}

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationFromMessageIdL()
// -----------------------------------------------------------------------------
EXPORT_C CCsClientConversation* CCSRequestHandler::GetConversationFromMessageIdL(TInt aMessageId)        
{    
    // Create a buffer to store the results.
    if(iResultsBuffer)
    {
        delete iResultsBuffer;
        iResultsBuffer = NULL;
    }
    iResultsBuffer = HBufC8::NewL(KBigIpcBuffer);

    // Send the request
    iSession.GetConversationFromMessageIdL(aMessageId, iResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(iResultsBuffer->Des());
    resultStream.PushL();
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    CleanupStack::PushL(clientConversation);
    clientConversation->InternalizeL(resultStream);
    CleanupStack::Pop(clientConversation);
    
    // Cleanup
    delete iResultsBuffer;
    iResultsBuffer = NULL;
    resultStream.Pop();
    resultStream.Close();

    return clientConversation;
}

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetMessagingHistoryL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetMessagingHistoryL(TInt aContactId,
        TInt aKnownIndex, 
        TInt aPageSize)
    {
    TInt conversationId = GetConversationIdL(aContactId);
    if ( conversationId == -1 )
        User::Leave(KErrNotFound);
    
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    clientConversation->SetConversationEntryId(conversationId);
    //set dummy entry
    CCsConversationEntry *entry = CCsConversationEntry::NewL();
    clientConversation->SetConversationEntryL(entry);
    GetConversationsL(clientConversation,aKnownIndex,aPageSize);
    
    //delete 
    delete clientConversation;
    if( entry )
        delete entry;  
        
    return;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationIdL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::ClearMessagingHistoryL(TInt aContactId)
    {
    TInt conversationId = GetConversationIdL(aContactId);
    if ( conversationId == -1 )
        User::Leave(KErrNotFound);
    
    // Done in background
    DeleteConversationL(conversationId);
    return;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::MarkConversationReadL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::MarkConversationReadL(TInt aConversationId)        
    {
    iSession.MarkConversationReadL(aConversationId);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::MarkMessagingHistoryReadL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::MarkMessagingHistoryReadL(TInt aContactId)        
    {
    TInt conversationId = GetConversationIdL(aContactId);
    if ( conversationId == -1 )
        User::Leave(KErrNotFound);
      
    MarkConversationReadL(conversationId);
    return;
    }

// End of File
