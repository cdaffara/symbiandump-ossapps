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
* Description:  Container store delete handler implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreDeleteHandler.h"
#include "ContainerStore.h"
#include "ContainerStoreDefs.h"

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CDeleteHandler* CDeleteHandler::NewL( CContainerStore&               aContainerStore,
                                      MBackgroundOperationsObserver& aBackgroundOperationsObserver, 
                                      TInt                           aPriority )
    {
    CDeleteHandler* self = new(ELeave) CDeleteHandler( aContainerStore, aBackgroundOperationsObserver, aPriority );
    return self;
    } // end NewL
    
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CDeleteHandler::CDeleteHandler( CContainerStore&               aContainerStore,
                                MBackgroundOperationsObserver& aBackgroundOperationsObserver, 
                                TInt                           aPriority ) :
    CActive( aPriority ),
    iContainerStore( aContainerStore ),
    iBackgroundOperationsObserver( aBackgroundOperationsObserver )
    {    
    __LOG_CONSTRUCT( "msg", "CDeleteHandler" )

   	CActiveScheduler::Add(this);
	} // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CDeleteHandler::~CDeleteHandler()
    {
    __LOG_DESTRUCT
        
    Cancel();
    
    iStack.Close();
    } // end destructor

// ==========================================================================
// FUNCTION: Start
// ==========================================================================
void CDeleteHandler::Start()
    {
    if( !IsActive() )
       {
        iBackgroundOperationsObserver.BackgroundOperationsInProgress();         
        iState = ERetryDelete;
        Reschedule();
        } // end if
    } // end Start

// ==========================================================================
// FUNCTION: Reschedule
// ==========================================================================
void CDeleteHandler::Reschedule()
    {
    // Schedule next delete (yield, in case higher priority active objects have something to do).
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );      
    } // end Reschedule

// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CDeleteHandler::RunL()
    {    
    if( DoNextDelete() )
        {
        Reschedule();
        }
    } // end RunL

// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CDeleteHandler::DoCancel()
    {
    // Nothing to do here.
    } // end DoCancel
    
// ==========================================================================
// FUNCTION: FindContainerToDeleteL
// ==========================================================================
void CDeleteHandler::FindContainerToDeleteL()
    {
    TBool done(EFalse);
    
    TContainerId containerId = KToBeDeletedContainers;
    if ( iState == ERetryDelete )
        {
        containerId = KToRetryDeleteContainers;
        }
    
    // Traverse down to the lowest first child in the hierarchy and delete it.
    while( !done )
        {
        // Look for children of the current lowest container in the hierarchy.
        TStackNode newNode;
        
        if( iStack.Count() == 0 )
            {            
            iContainerStore.FirstChildL( containerId, newNode.iId, newNode.iBookmark );
            }
        else
            {
            iContainerStore.FirstChildL( iStack[iStack.Count()-1].iId, newNode.iId, newNode.iBookmark );
            } // end if
    
        // If there are no children to the lowest container then we are done for now.  Otherwise, add the
        // first child to the stack and continue trying to traverse lower.
        if( newNode.iId == KContainerInvalidId )
            {
            done = ETrue;
            }
        else
            {
            iStack.AppendL( newNode );
            } // end if
            
        } // end while
    
    } // end FindContainerToDeleteL

// ==========================================================================
// FUNCTION: DoNextDelete
// ==========================================================================
TBool CDeleteHandler::DoNextDelete()
    {
    __LOG_ENTER( "DoNextDeleteL" )
    
    // Delete the lowest node in the hierarchy.        
    TRAPD( result, FindContainerToDeleteL() );

    TBool more ( EFalse );    
        
    if( result == KErrNone )
        {        
        if( iStack.Count() > 0 )
            {
            TRAP( result, iContainerStore.DeleteIndividualContainerL( iStack[iStack.Count()-1].iBookmark ) );
        
            if( result == KErrNone )
                {
                iStack.Remove( iStack.Count() - 1 );
                more = ETrue;          
                }
            else
                {
                __LOG_WRITE8_FORMAT1_ERROR( "Unexpected error in DeleteIndividualContainerL %i", result );
                
                // Do not reschedule.  The delete handler will resumed when the server is restarted or when
                // another item is deleted.  Hopefully the unexpected error is transient and will clear up by
                // then.
                } // end if
                
            } // end if
        }
    else
        {
        __LOG_WRITE8_FORMAT1_ERROR( "Unexpected error in FindContainerToDeleteL %i", result );
        
        // Do not reschedule.  The delete handler will resumed when the server is restarted or when
        // another item is deleted.  Hopefully the unexpected error is transient and will clear up by
        // then.
        
        } // end if

    if( !more )
        {   
        if ( iState == ERetryDelete )
            {
            //next state
            iState = EDeleteFromSortingTable;
            more = ETrue;
            }
        else if(iState == EDeleteFromSortingTable)
            {
            //check sorting table for containers that have been marked for deletion
            if(!iContainerStore.DeleteFromSortingTable())
                {
                //if nor more then set to next state
                iState = EDelete;
                }
            more = ETrue;
            }
        else
            {
            iBackgroundOperationsObserver.BackgroundOperationsCompleted();         
            }
        } // end if

    __LOG_EXIT                    
    return more;
    
    } // end DoNextDelete
