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
* Description:  ECom interface for email interface factory inline implementation
*
*/

#include "emailclientapi.hrh"

// LOCAL FUNCTIONS

inline void ResetAndDestroyCleanup( TAny* aAny )
    {
    RImplInfoPtrArray* ptrArray = reinterpret_cast<RImplInfoPtrArray*>( aAny );
    ptrArray->ResetAndDestroy();
    }

inline void CleanupResetAndDestroyPushL( RImplInfoPtrArray& aArray )
    {
    TCleanupItem item( &ResetAndDestroyCleanup, &aArray );
    CleanupStack::PushL( item );
    }

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CEmailInterfaceFactory::NewL
// Two-phased constructor
// ----------------------------------------------------------------------------
//
inline CEmailInterfaceFactory* CEmailInterfaceFactory::NewL( )
    {
    // Find implementation for our interface.
    RImplInfoPtrArray implArray;
    EmailInterface::CleanupResetAndDestroyPushL( implArray );
    const TUid ifUid = {KEmailClientFactoryApiUid};
    REComSession::ListImplementationsL(
                            ifUid,
                            implArray );
    // there should be only one impl so take first
    TAny* interface = NULL;
    if ( implArray.Count() )
        {
        const TUid uid = implArray[0]->ImplementationUid();        
        interface = REComSession::CreateImplementationL( 
            uid, _FOFF( CEmailInterfaceFactory, iDtor_ID_Key ) );
        }
    CleanupStack::PopAndDestroy(); // implArray
        
    return reinterpret_cast<CEmailInterfaceFactory*>( interface );
    }

// ----------------------------------------------------------------------------
// CEmailInterfaceFactory::~CEmailInterfaceFactory
// Destructor
// ----------------------------------------------------------------------------
//
inline CEmailInterfaceFactory::~CEmailInterfaceFactory()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

template<class T>
inline void CleanupResetAndRelease<T>::PushL( RPointerArray<T>& aItem ) { 
     TCleanupItem item( &CleanupResetAndRelease<T>::ResetAndRelease, &aItem );
     CleanupStack::PushL( item );
 }
template<class T>
inline void CleanupResetAndRelease<T>::ResetAndRelease( TAny* aPtr )
     {
     RPointerArray<T>* array = reinterpret_cast<RPointerArray<T>* >( aPtr );
     for ( TInt i = 0; i < array->Count(); i++ )
         {
         T* item = (*array)[i];
         item->Release();
         }
     array->Reset();
     }

// End Of File
