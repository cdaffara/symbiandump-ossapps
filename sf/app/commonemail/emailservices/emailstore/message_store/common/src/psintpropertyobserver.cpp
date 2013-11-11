/*
* Copyright (c) 2000-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Int P/S key value observer implementation.
*
*/

#include "psintpropertyobserver.h"

 CPSIntPropertyWatcher* CPSIntPropertyWatcher::NewL( MPSPropertyWatcherObserver* aObserver )
     {
     CPSIntPropertyWatcher* self = CPSIntPropertyWatcher::NewLC( aObserver );
     CleanupStack::Pop( self );
     return self;
     }
 
 CPSIntPropertyWatcher* CPSIntPropertyWatcher::NewLC( MPSPropertyWatcherObserver* aObserver )
    {
    CPSIntPropertyWatcher* self = new (ELeave)CPSIntPropertyWatcher();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;
    }
 
 CPSIntPropertyWatcher::~CPSIntPropertyWatcher()
     {
     Cancel();
     iProperty.Close();
     }

 void CPSIntPropertyWatcher::StartL( TUid aCategory, TUint aKey, 
                                           TUint32 aValueWhenToTrigger, 
                                           TBool aDefineIfNeeded,  
                                           TSecurityPolicy aReadPolicy,
                                           TSecurityPolicy aWritePolicy )
     {
     // Define PS property?
     if ( aDefineIfNeeded )
         {
         // Try defining first.
         TInt err = iProperty.Define( aCategory, aKey, RProperty::EInt, aReadPolicy, aWritePolicy );
         if ( KErrNone != err && 
              KErrAlreadyExists != err && 
              KErrPermissionDenied != err )
             {
             User::Leave( err );
             }
         }

     // Property already exists, attach.
     User::LeaveIfError( iProperty.Attach( aCategory, aKey ) );
     
     // Lets subscribe for change notifications
     iProperty.Subscribe( iStatus );
     SetActive();
     iObservedValue = aValueWhenToTrigger;
     }
 
 void CPSIntPropertyWatcher::Stop()
     {
     Cancel();
     iProperty.Close();
     }
        

 CPSIntPropertyWatcher::CPSIntPropertyWatcher( ): CActive( EPriorityStandard )
    {
    
    }

void CPSIntPropertyWatcher::ConstructL( MPSPropertyWatcherObserver* aObserver )
    {
    iObserver = aObserver;
    User::LeaveIfNull( iObserver );
    CActiveScheduler::Add( this );
    }

// CActive methods
void CPSIntPropertyWatcher::RunL()
    {
    // It's canceled?
    if ( iStatus.Int() == KErrCancel )
        {
        // We are done
        return;
        }
    
    // Value has been changed?
    if ( iStatus.Int() == KErrNone )
        {
        // Reinitiate the subscription so that 
        // we wont loose anymore updates.
        iProperty.Subscribe( iStatus );
        SetActive();
        
        // Get current value        
        TInt value = 0;
        iProperty.Get( value );
        if ( iObservedValue == static_cast<TUint32>( value ) )
            {
            // Cancel subscription.
            Cancel();
            
            // Notify observer.
            if ( iObserver )
                {
                iObserver->OnPSValueChangedToRequired();
                }
            }
        }
    }

void CPSIntPropertyWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

