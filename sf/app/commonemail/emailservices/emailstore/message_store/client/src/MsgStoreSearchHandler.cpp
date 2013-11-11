/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store search handler client implementation.
*
*/



#include "MsgStoreSearchHandler.h"
//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreMailBox.h"
//</cmail>
#include "RMessageStoreSession.h"

const TInt KInitialPropertiesTransferBufferLength = 10000;

// ----------------------
// CMsgStoreSearchHandler
// ----------------------

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreSearchHandler* CMsgStoreSearchHandler::NewL( RMessageStoreSession&    aSession,
                                                      RMsgStoreSearchCriteria& aSearchCriteria, 
                                                      MMsgStoreSearchClient&   aSearchClient )
    {
    CMsgStoreSearchHandler* self = new(ELeave) CMsgStoreSearchHandler( aSession, aSearchCriteria, aSearchClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    } // end NewL
    
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreSearchHandler::CMsgStoreSearchHandler( RMessageStoreSession& aSession,
                                                RMsgStoreSearchCriteria& aSearchCriteria, 
                                                MMsgStoreSearchClient& aSearchClient ) :
    CActive( EPriorityStandard ),
    iSession( aSession ),
    iSearchCriteria( aSearchCriteria ),
    iSearchClient( aSearchClient )
    {
    __LOG_CONSTRUCT( "MsgClient", "CMsgStoreSearchHandler" ) 
    
    CActiveScheduler::Add(this);    
    } // end constructor
                               
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreSearchHandler::ConstructL()    
    {
    iMatchBuffer[0].CreateL( KInitialPropertiesTransferBufferLength );
    iMatchBuffer[1].CreateL( KInitialPropertiesTransferBufferLength );
    
    iSession.PrepareSearchL( iSearchCriteria );
    
    iSession.GetMatchesL( iStatus, iMatchBuffer[iMatchBufferIndex] );
    SetActive();
    } // end ConstructL

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreSearchHandler::~CMsgStoreSearchHandler()
    {
    Cancel();
    
    iMatchBuffer[0].Close();
    iMatchBuffer[1].Close();
    
    if( iThisObjectHasBeenDeleted )
        {
        *iThisObjectHasBeenDeleted = ETrue;
        } // end if
        
    __LOG_DESTRUCT
    } // end destructor
    

// ==========================================================================
// FUNCTION: CancelSearch
// ==========================================================================
void CMsgStoreSearchHandler::CancelSearch()
    {
    __LOG_ENTER( "CancelSearch" )
    
    iSession.CancelSearch();
    Cancel();
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CMsgStoreSearchHandler::RunL()
    {
    __LOG_ENTER( "RunL" )
    
    RBuf8& currentMatchBuffer = iMatchBuffer[iMatchBufferIndex];
    
    if( iStatus == KErrNone && currentMatchBuffer.Length() > 0 )
        {
        // Switch to the other match buffer for the next call to the session.  Make this call before
        // calling the search client with matches, just in case the client cancels the search during
        // the callback.  This will cause DoCancel to be called during this object's destruction, which
        // will cancel the server-side search.
        iMatchBufferIndex = (iMatchBufferIndex + 1) % 2;
        iSession.GetMatchesL( iStatus, iMatchBuffer[iMatchBufferIndex] );
        SetActive();                                      

        // Client's may cancel the search during callbacks.  This flag mechanism is used to detect that case,
        // and avoid using any heap variables that may no longer be valid.
        TBool thisObjectHasBeenDeleted = EFalse;
        iThisObjectHasBeenDeleted = &thisObjectHasBeenDeleted;
            
        TBool atEndOfIds = (currentMatchBuffer.Length() == 0);
        TUint offset = 0;
        
        while( !thisObjectHasBeenDeleted && !atEndOfIds )
            {
            const TMsgStoreId& msgId = *reinterpret_cast<const TMsgStoreId *>( currentMatchBuffer.Ptr() + offset );
            offset += sizeof(TMsgStoreId);                    
            
            const TMsgStoreId& folderId = *reinterpret_cast<const TMsgStoreId *>( currentMatchBuffer.Ptr() + offset );
            offset += sizeof(TMsgStoreId);                    

            const TUint length = *reinterpret_cast<const TUint *>( currentMatchBuffer.Ptr() + offset );
            offset += sizeof(TUint);
            
            TPtrC8 propertyBuf( KNullDesC8 );
            if ( length > 0 )
                {
                propertyBuf.Set( currentMatchBuffer.Ptr() + offset, length );
                offset += length;
                }
            
            __LOG_WRITE8_FORMAT1_INFO( "match found (%i)", msgId )  
              
            if( offset >= currentMatchBuffer.Length() )
                {
                atEndOfIds = ETrue;                
                } // end if
                
            // Note that the client may cancel the search during this callback.                    
            CMsgStorePropertyContainer* matchMessage = CMsgStorePropertyContainer::NewL();
            CleanupStack::PushL( matchMessage );
            
            matchMessage->SetIds( msgId, folderId );
            matchMessage->DeserializeL( propertyBuf );
            
            CleanupStack::Pop( matchMessage );
            
            iSearchClient.MatchFound( matchMessage );
            
            //need to delete the message, the observer should take ownership of this pointer
            //and set the original to NULL
            delete matchMessage;
                                
            } // end while                          
        
        if( !thisObjectHasBeenDeleted )
            {
            iThisObjectHasBeenDeleted = NULL;
            } // end if
        }
    else
        {
        __LOG_WRITE8_FORMAT1_INFO( "Search completed, iStatus=%d", iStatus.Int() );
        
        iSearchClient.SearchCompleted();
        
        } // end if         

    // __LOG_EXIT was removed because it crashes in cases where thisObjectHasBeenDeleted is true        
    
    } // end RunL
    
// ==========================================================================
// FUNCTION: RunError
// ==========================================================================
TInt CMsgStoreSearchHandler::RunError( TInt __LOG_BLOCK(aError) )
    {
    __LOG_ENTER_SUPPRESS( "RunError" )
    
    __LOG_WRITE8_FORMAT1_ERROR( "aError=%i", aError )
    
    // Something unexpected failed.  Notify the client that search is complete.
    iSearchClient.SearchCompleted();
    
    return KErrNone;
    
    } // end RunError

// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CMsgStoreSearchHandler::DoCancel()
    {
    //do not call iSession.CancelSearch() here because these object are deleted
    //later than the time searches are canceled.  If we call it here
    //iSession.CancelSearch() will be sent to server again which may have side effects.
    } // end 
