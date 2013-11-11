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
* Description:  Message store observer handler client implementation.
*
*/


#include "MsgStoreObserverHandler.h"
#include "RMessageStoreSession.h"
//<cmail>
#include "MsgStoreObserver.h"
//</cmail>



// ------------------------
// CMsgStoreObserverHandler
// ------------------------

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreObserverHandler* CMsgStoreObserverHandler::NewL( RMessageStoreSession& aSession )
	{
    CMsgStoreObserverHandler* self = new( ELeave ) CMsgStoreObserverHandler( aSession );
    return self;
	} // end NewL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreObserverHandler::CMsgStoreObserverHandler( RMessageStoreSession& aSession ) : 
	CActive( EPriorityStandard ),
	iSession( aSession )
	{
	__LOG_CONSTRUCT( "MsgClient", "CMsgStoreObserverHandler" )
	//initialize variable to false.
    iObjectDeleted = EFalse;
	} // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreObserverHandler::~CMsgStoreObserverHandler()
	{
	Cancel();
	
	iMailBoxObservers.Close();
	iObservers.Close();
		
	//object is being deleted. Mark variable as true to cover case where delete
	//has come while RunL was running and "AccountEventNotify" was called and
	//has not completed.
	iObjectDeleted = ETrue;
	__LOG_DESTRUCT
	} // end destructor

// ==========================================================================
// FUNCTION: AddObserverL
// ==========================================================================
void CMsgStoreObserverHandler::AddObserverL( MMsgStoreObserver* aObserver )
	{
    __LOG_ENTER_SUPPRESS( "AddObserverL (system)" )
    __LOG_WRITE8_FORMAT1_INFO( "aObserver=%08x", aObserver )
    
	if( !IsAdded() )
		{		
    	CActiveScheduler::Add(this);    
		} // end if
		
	iObservers.AppendL( aObserver );
	
	if( (iObservers.Count() + iMailBoxObservers.Count()) == 1 )
		{
		iSession.StartObservingL();
		
		iSession.GetEvents( iStatus, iEventBuffer );
		SetActive();
		} // end if
		
    __LOG_WRITE8_FORMAT1_INFO( "System observers = %d", iObservers.Count() )
    
	} // end AddObserverL

// ==========================================================================
// FUNCTION: RemoveObserverL
// ==========================================================================
void CMsgStoreObserverHandler::RemoveObserverL( MMsgStoreObserver* aObserver )
	{
    __LOG_ENTER_SUPPRESS( "RemoveObserverL (system)" )
    __LOG_WRITE8_FORMAT1_INFO( "aObserver=%08x", aObserver )
    
	TBool found = EFalse;
	
	TInt index = 0;
	
	while( !found && index < iObservers.Count() )
		{
		if( iObservers[index] == aObserver )
			{
			found = ETrue;
			iObservers.Remove( index );
			}
		else
			{
			index++;
			} // end if		
		} // end while
	
	if( !found )
		{
		User::Leave( KErrNotFound );
		} // end if
		
	if( (iObservers.Count() + iMailBoxObservers.Count()) == 0 )
		{
		Cancel();
		} // end if
		
    __LOG_WRITE8_FORMAT1_INFO( "System observers = %d", iObservers.Count() )
    
	} // end RemoveObserverL


// ==========================================================================
// FUNCTION: AddObserverL
// ==========================================================================
void CMsgStoreObserverHandler::AddObserverL( MMsgStoreMailBoxObserver* aObserver, TMsgStoreId aMailBoxId )
	{
    __LOG_ENTER_SUPPRESS( "AddObserverL (mailbox)" )
    __LOG_WRITE8_FORMAT2_INFO( "maibBoxId=%d, aObserver=%08x", aMailBoxId, aObserver )
    
	if( !IsAdded() )
		{		
    	CActiveScheduler::Add(this);    
		} // end if

	TMailBoxObserverInfo info;
	info.iObserver = aObserver;
	info.iMailBoxId = aMailBoxId;
	
	iMailBoxObservers.AppendL( info );
	
	if( (iObservers.Count() + iMailBoxObservers.Count()) == 1 )
		{
		iSession.StartObservingL();
		
		iSession.GetEvents( iStatus, iEventBuffer );
		SetActive();
		} // end if
		
    __LOG_WRITE8_FORMAT1_INFO( "Mailbox observers = %d", iMailBoxObservers.Count() )
	} // end AddObserverL

// ==========================================================================
// FUNCTION: RemoveObserverL
// ==========================================================================
void CMsgStoreObserverHandler::RemoveObserverL( MMsgStoreMailBoxObserver* aObserver, TMsgStoreId aMailBoxId )
	{
    __LOG_ENTER_SUPPRESS( "RemoveObserverL (mailbox)" )
    __LOG_WRITE8_FORMAT2_INFO( "maibBoxId=%d, aObserver=%08x", aMailBoxId, aObserver )
    
	TBool found = EFalse;
	
	TInt index = 0;
	
	while( !found && index < iMailBoxObservers.Count() )
		{
		if( iMailBoxObservers[index].iObserver == aObserver && 
		    iMailBoxObservers[index].iMailBoxId == aMailBoxId )
			{
			found = ETrue;
			iMailBoxObservers.Remove( index );
			}
		else
			{
			index++;
			} // end if		
		} // end while
	
	if( !found )
		{
		User::Leave( KErrNotFound );
		} // end if
		
	if( ( iObservers.Count() + iMailBoxObservers.Count()) == 0 )
		{
		Cancel();
		} // end if
	
    __LOG_WRITE8_FORMAT1_INFO( "Mailbox observers = %d", iMailBoxObservers.Count() )
		
	} // end RemoveObserverL

// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CMsgStoreObserverHandler::RunL()
	{
	__LOG_ENTER( "RunL" )
	
    if( iStatus == KErrNone )
    	{
        TInt eventCount = iEventBuffer.Length() / sizeof(TMsgStoreEvent);
    	
		__LOG_WRITE8_FORMAT1_INFO( "RunL %i events received", eventCount )

        const TMsgStoreEvent* eventPtr = reinterpret_cast<const TMsgStoreEvent *>( iEventBuffer.Ptr() );
        
        for( TUint i = 0; i < eventCount; i++ )
            {
    		if( eventPtr->iType < KMsgStoreSystemEventMin )
    			{
    			//just in case the observer's handler method leaves, we still want to get next event    			
    			ModificationNotify( *eventPtr );
    			}		
    		else if ( eventPtr->iType < KMsgStoreAccountEventMin )
    			{
                //just in case the observer's handler method leaves, we still want to get next event                
    			SystemEventNotify( *eventPtr );								
    	    	}
    		else
    			{
                //just in case the observer's handler method leaves, we still want to get next event                
    			AccountEventNotify( *eventPtr );
    			}
    	    	
    	    eventPtr++;
            }
            
        //check that "this" object was not deleted by client during "AccountEventNotify".
        //If object has been deleted there is no need to get session events and activate itself.
        if ( !iObjectDeleted )
            {
            // Get the next event(s).       
            iSession.GetEvents(iStatus, iEventBuffer);
            SetActive();
            }
    	
	    }
    else
    	{
        __LOG_WRITE8_FORMAT1_ERROR( "error, iStatus=%d", iStatus.Int() );
    	} // end if

    __LOG_EXIT
	} // end RunL
	
// ==========================================================================
// FUNCTION: RunError
// ==========================================================================
TInt CMsgStoreObserverHandler::RunError( TInt __LOG_BLOCK(aError) )
    {
    // This should never happen if the client's are properly coded and do not
    // leave during ModificationNotify or SystemEventNotify.  Disregard the
    // error, if it occurs.
    
    __LOG_ENTER_SUPPRESS( "RunError" )
    
    __LOG_WRITE8_FORMAT1_ERROR( "aError=%i", aError )
    
    return KErrNone;
    
    } // end RunError

// ==========================================================================
// FUNCTION: ModificationNotify
// ==========================================================================
void CMsgStoreObserverHandler::ModificationNotify( const TMsgStoreEvent& aEvent )
	{
	__LOG_ENTER( "ModificationNotify" )
	
	TMsgStoreContainerType type;
	TBool                  notify = ETrue;
	
	switch( aEvent.iId & EMsgStoreContainerMask )
		{
		case EMsgStoreMailBoxBits:			
			type = EMsgStoreMailboxContainer;
			break;
		case EMsgStoreFolderBits:			
			type = EMsgStoreFolderContainer;
			break;
		case EMsgStoreMessageBits:			
			type = EMsgStoreMessageContainer;
			break;
		case EMsgStorePartBits:			
			type = EMsgStorePartContainer;
			break;
		default:
			// this shouldn't happen!
			__LOG_WRITE_ERROR( "Invalid type" );
			notify = EFalse;
			break;			
		}; // end switch
		
	if( notify )
		{
		for( TInt index = 0; index < iMailBoxObservers.Count(); index++ )
			{
			TMailBoxObserverInfo info = iMailBoxObservers[index];
			if ( aEvent.iMailBoxId == info.iMailBoxId )
				{
		        __LOG_WRITE8_FORMAT1_INFO( "Notifying event for MailboxId %d", aEvent.iMailBoxId )
				info.iObserver->ModificationNotify( aEvent.iMailBoxId,
												static_cast<TMsgStoreOperation>(aEvent.iType), 
												type,
												aEvent.iFlags,
												aEvent.iId, 
												aEvent.iParentId, 
												aEvent.iOtherId );
				}
			} // end for
		} // end if
		
    __LOG_EXIT		
	} // end ModificationNotify

// ==========================================================================
// FUNCTION: SystemEventNotify
// ==========================================================================
void CMsgStoreObserverHandler::SystemEventNotify( const TMsgStoreEvent& aEvent )
	{
	__LOG_ENTER_SUPPRESS( "SystemEventNotify" )
	
	__LOG_WRITE8_FORMAT1_INFO( "Notifying %d observers", iObservers.Count() )
	for( TInt index = 0; index < iObservers.Count(); index++ )
		{
		iObservers[index]->SystemEventNotify( static_cast<TMsgStoreSystemEvent>(aEvent.iType) );
		} // end for
		
	} // end SystemEventNotify

// ==========================================================================
// FUNCTION: AccountEventNotify
// ==========================================================================
void CMsgStoreObserverHandler::AccountEventNotify( const TMsgStoreEvent& aEvent )
	{
	__LOG_ENTER_SUPPRESS( "AccountEventNotify" )
	
	TPtrC16 name16;
	const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( aEvent.iAccountName.Ptr() );
	name16.Set( valuePtr16, aEvent.iAccountName.Length() / 2 );
	
	TPtrC16 newName16;
	valuePtr16 = reinterpret_cast<const TUint16*>( aEvent.iNewAccountName.Ptr() );
	newName16.Set( valuePtr16, aEvent.iNewAccountName.Length() / 2 );
	
    __LOG_WRITE8_FORMAT1_INFO( "Notifying %d observers", iObservers.Count() )
    
	for( TInt index = 0; index < iObservers.Count(); index++ )
		{

		
		
		iObservers[index]->AccountEventNotify( static_cast<TMsgStoreAccountEvent>(aEvent.iType),
				                               static_cast<TInt32>(aEvent.iId),
				                               name16,
				                               newName16,
                                               aEvent.iMailBoxId );
		} // end for
		
	} // end AccountEventNotify



// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CMsgStoreObserverHandler::DoCancel()
	{
    __LOG_ENTER( "DoCancel" )

    iSession.StopObserving();

    __LOG_EXIT
	} // end DoCancel
		
