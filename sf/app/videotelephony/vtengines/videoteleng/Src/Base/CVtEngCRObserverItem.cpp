/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video call CR listener.
*
*/



// INCLUDE FILES
#include    "CVtEngCRObserverItem.h"
#include    <centralrepository.h>
#include    "MVtEngCRSettingObserver.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::CVtEngCRObserverItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngCRObserverItem::CVtEngCRObserverItem(
    MVtEngCRSettingObserver* aObserver,
    TUid aUid, TUint32 aKey, 
    CCenRepNotifyHandler::TCenRepKeyType aType  ) : 
    iObserver( aObserver ), iUid( aUid ), iKey( aKey ), iType( aType ),
    iNotifier( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngCRObserverItem* CVtEngCRObserverItem::NewL( 
    MVtEngCRSettingObserver* aObserver,
    TUid aUid, TUint32 aKey, 
    CCenRepNotifyHandler::TCenRepKeyType aType  )
    {
    __VTPRINTENTER( "CRListener.NewL" )
    CVtEngCRObserverItem* self = 
        new( ELeave ) CVtEngCRObserverItem( aObserver, aUid, aKey, aType );
    __VTPRINTEXIT( "CRListener.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::~CVtEngCRObserverItem
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngCRObserverItem::~CVtEngCRObserverItem()
    {
    __VTPRINTENTER( "CRListener.~" )
    if ( iNotifier )
    	{
    	iNotifier->StopListening();
    	}
    delete iNotifier;
    __VTPRINTEXIT( "CRListener.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Offer
// Checks if uid and id match this observer's.
// -----------------------------------------------------------------------------
//
TBool CVtEngCRObserverItem::Offer( const TUid aUid, const TUint32 aKey ) const
    {
    return aUid == iUid && iKey == aKey ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Offer
// Checks if uid matches this observer's.
// -----------------------------------------------------------------------------
//
TBool CVtEngCRObserverItem::Offer( const TUid aUid ) const
    {
    return aUid == iUid ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Offer
// Checks if id matches this observer's.
// -----------------------------------------------------------------------------
//
TBool CVtEngCRObserverItem::Offer( const TUint32 aKey ) const
    {
    return iKey == aKey ? ETrue : EFalse;
    }
        
// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::SetCenRepNotifier
// Sets the actual change handler to this object.
// -----------------------------------------------------------------------------
//
void CVtEngCRObserverItem::SetCenRepNotifier( CCenRepNotifyHandler* aNotifier )
    {
    iNotifier = aNotifier;
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::HandleNotifyInt
// Callback of changed int value.
// -----------------------------------------------------------------------------
//
void CVtEngCRObserverItem::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    __VTPRINTENTER( "CRListener.Int" )
    iObserver->HandleNotifyInt( iUid, aId, aNewValue );
    __VTPRINTEXIT( "CRListener.Int" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::HandleNotifyReal
// Callback of changed real value.
// -----------------------------------------------------------------------------
//
void CVtEngCRObserverItem::HandleNotifyReal( TUint32 aId, TReal aNewValue )
    { 
    __VTPRINTENTER( "CRListener.Real" )
    iObserver->HandleNotifyReal( iUid, aId, aNewValue );
    __VTPRINTEXIT( "CRListener.Real" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::HandleNotifyString
// Callback of changed string value.
// -----------------------------------------------------------------------------
//
void CVtEngCRObserverItem::HandleNotifyString( TUint32 aId, 
    const TDesC16& aNewValue )
    {
    __VTPRINTENTER( "CRListener.String" )
    iObserver->HandleNotifyString( iUid, aId, aNewValue );
    __VTPRINTEXIT( "CRListener.String" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::HandleNotifyBinary
// Callback of changed binary value.
// -----------------------------------------------------------------------------
//
void CVtEngCRObserverItem::HandleNotifyBinary( TUint32 /* aId */, 
    const TDesC8& /* aNewValue */ )
    {
    __VTPRINTENTER( "CRListener.Binary" )
    __VTPRINTEXIT( "CRListener.Binary" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::HandleNotifyGeneric
// Callback of a change in central repository.
// -----------------------------------------------------------------------------
//
void CVtEngCRObserverItem::HandleNotifyGeneric( TUint32 /* aId */ )
    {
    __VTPRINTENTER( "CRListener.Generic" )
    __VTPRINTEXIT( "CRListener.Generic" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::HandleNotifyError
// Callback of an error occurred.
// -----------------------------------------------------------------------------
//
#ifdef VTDEBUG
void CVtEngCRObserverItem::HandleNotifyError( TUint32 aId, TInt aError, 
    CCenRepNotifyHandler* /* aHandler */ )
#else    
void CVtEngCRObserverItem::HandleNotifyError( 
    TUint32 /* aId */, TInt /* aError */, CCenRepNotifyHandler* /* aHandler */ )
#endif // VTDEBUG  
    {
    __VTPRINT3( DEBUG_GEN, "CRListener.error id=%d,err=%d ", aId, aError )
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Uid
// Returns Uid for this observer.
// -----------------------------------------------------------------------------
//    
TUid CVtEngCRObserverItem::Uid() const
    {
    return iUid;
    }
    
// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Key
// Returns Key for this observer.
// -----------------------------------------------------------------------------
//    
TUint32 CVtEngCRObserverItem::Key() const
    {
    return iKey;
    }

// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Notifier
// Returns Notifier for this observer.
// -----------------------------------------------------------------------------
//    
CCenRepNotifyHandler* CVtEngCRObserverItem::Notifier() const
    {
    return iNotifier;
    }
  
// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Type
// Returns Type for this observer.
// -----------------------------------------------------------------------------
//    
CCenRepNotifyHandler::TCenRepKeyType CVtEngCRObserverItem::Type() const
    {
    return iType;
    }
  
// -----------------------------------------------------------------------------
// CVtEngCRObserverItem::Observer
// Returns true observer for this observer.
// -----------------------------------------------------------------------------
//    
MVtEngCRSettingObserver* CVtEngCRObserverItem::Observer() const
    {
    return iObserver;
    }
    
//  End of File
