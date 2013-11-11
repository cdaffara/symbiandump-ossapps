/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Subscribe source implementation for getting contact status  
 *              notification from publisher.
 *
 */
#include "contactsubscriber.h"
#include "mylocationlogger.h"
#include <locationservicedefines.h>

const TUid KContactPropertyCat={0x20022EF9};
enum TMyPropertyKeys {EMyPropertyInteger=0x1, EMyPropertyType=0x2};
const TInt KBufferSize=16;

// -----------------------------------------------------------------------------
// CContactSubscriber::NewL()
// -----------------------------------------------------------------------------
//
CContactSubscriber* CContactSubscriber::NewL(MNotifyChange* aNotifyChange)
{
    CContactSubscriber* self = CContactSubscriber::NewLC(aNotifyChange);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CContactSubscriber::NewLC()
// -----------------------------------------------------------------------------
//
CContactSubscriber* CContactSubscriber::NewLC(
        MNotifyChange* aNotifyChange)
{
    CContactSubscriber* self = new (ELeave) CContactSubscriber(
            aNotifyChange);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CContactSubscriber::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactSubscriber::ConstructL()
{
    __TRACE_CALLSTACK;
    CActiveScheduler::Add(this);
    TInt ret = iProperty.Attach(KContactPropertyCat, EMyPropertyInteger);
    if (KErrNone == ret)
    {
        SubscribeChangeNotiFication();
    }
}


// -----------------------------------------------------------------------------
// CContactSubscriber::SubscribeChangeNotiFication()
// start subscribe for contact entry
// -----------------------------------------------------------------------------
//
void CContactSubscriber::SubscribeChangeNotiFication()
{
    __TRACE_CALLSTACK;
    if (IsActive())
      {
          Cancel();
      }
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);   
    SetActive();
}

// -----------------------------------------------------------------------------
// CContactSubscriber::CContactSubscriber()
// Default constructor .
// -----------------------------------------------------------------------------
//
CContactSubscriber::CContactSubscriber(MNotifyChange* aNotifyChange) :
    CActive(EPriorityStandard),
    iNotifyChange(*aNotifyChange)
    
{
}
// -----------------------------------------------------------------------------
// CContactSubscriber::~CContactSubscriber()
// default destuctor.
// -----------------------------------------------------------------------------
//
CContactSubscriber::~CContactSubscriber()
{
    __TRACE_CALLSTACK;
    Cancel();
    iProperty.Close();
    
}
// -----------------------------------------------------------------------------
// CContactSubscriber::RunL()
// Assyncronous request handler , on completion of notification
// -----------------------------------------------------------------------------
//
void CContactSubscriber::RunL()
{
    __TRACE_CALLSTACK;
    SubscribeChangeNotiFication();
    // property updated, get new value 
    TBuf8 <KBufferSize> value; 
    TPtrC8  id; 
    TPtrC8  sourceType; 
    TPtrC8  addressCount; 
    
    if ( KErrNone == iProperty.Get( value ) )
    { 
        TInt pos =  value.Locate(TChar('-')); 
        id.Set( value.Left(pos) ); 
        
        TPtrC8 ptr = value.Right(3); 
        sourceType.Set(ptr.Left(1)); 
        addressCount.Set(ptr.Right(1)); 
        
        TInt appId = -1, addressType = -1, addressTypeCount = -1; 
        TLex8 lex(id); 
        lex.Val(appId); 
        
        TLex8 lex1(sourceType); 
        lex1.Val( addressType ); 
        
        TLex8 lex2(addressCount); 
        lex2.Val(addressTypeCount); 
        
        iNotifyChange.GetChangeNotificationL( appId, addressType,addressTypeCount ); 
    } 
}
// -----------------------------------------------------------------------------
// CContactSubscriber::DoCancel()
// Handels the error condition on assynchronous request
// -----------------------------------------------------------------------------
//
void CContactSubscriber::DoCancel()
{
    iProperty.Cancel();
}

//End of file

