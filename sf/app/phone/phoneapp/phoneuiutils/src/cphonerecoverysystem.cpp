/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Recovery system.
*
*/


// INCLUDE FILES
#include    <e32svr.h>

#include    "phoneui.pan"
#include    "cphonerecoverysystem.h"
#include    "ctelerecoverysystem.h"
#include    "cphonetimer.h"
#include    "phoneconstants.h"

const TInt KEmptySlot = 0;
const TInt KErrNoSpace = -1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneRecoverySystem::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneRecoverySystem* CPhoneRecoverySystem::Instance()
    {
    CPhoneRecoverySystem* instance = static_cast<CPhoneRecoverySystem*> 
        ( CCoeEnv::Static ( KUidRecoverySystemSingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhoneRecoverySystem::NewL() );
        if ( err )
            {
            Panic( EPhoneUtilsCouldNotCreateSingleton );    
            }
        }
    return instance;
    }

// ---------------------------------------------------------
// CPhoneRecoverySystem::CPhoneRecoverySystem
// ---------------------------------------------------------
//
CPhoneRecoverySystem::CPhoneRecoverySystem() :
    CCoeStatic( KUidRecoverySystemSingleton, EThread )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::NewL
// -----------------------------------------------------------------------------
//
CPhoneRecoverySystem* CPhoneRecoverySystem::NewL()
    {
    CPhoneRecoverySystem* self = new (ELeave) CPhoneRecoverySystem();
    return self;
    }

// -----------------------------------------------------------------------------
// CTeUtlFactoryImpl::CreateRecoverySystemL
// 
// -----------------------------------------------------------------------------
//
CTeleRecoverySystem* CPhoneRecoverySystem::CreateRecoverySystemL()
    {
    return CTeleRecoverySystem::NewL();
    }
    
// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::~CPhoneRecoverySystem
// -----------------------------------------------------------------------------
//
CPhoneRecoverySystem::~CPhoneRecoverySystem()
    {
    RemoveAllIdsFromContainer();
    delete iRecoverySystem;
    iRecoverySystem = NULL; 
    }


// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::AddL
// -----------------------------------------------------------------------------
//
EXPORT_C TRecoveryId CPhoneRecoverySystem::AddL( 
    TCallBack aCallBack, 
    TRecoveryPriority aPriority, 
    CTeleRecoverySystem::TRecoveryState aState )
    {
    if ( !iRecoverySystem )
        {
        iRecoverySystem = CreateRecoverySystemL();  
        }
        
    TRecoveryId id = iRecoverySystem->AddL( 
        aCallBack, 
        aPriority, 
        aState );

    try
        {
        AddIdToContainer( id ); 
        }
    catch( TInt exception )
        {
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsBufferOverflow ) );   
        }
    
    return id;
    }
    
// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::Add
// -----------------------------------------------------------------------------
//
TRecoveryId CPhoneRecoverySystem::Add(
    TCallBack aCallBack, 
    TRecoveryPriority aPriority, 
    CTeleRecoverySystem::TRecoveryState aState )
    {
    return iRecoverySystem->Add( 
        aCallBack, 
        aPriority, 
        aState );
    }

// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::Remove
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRecoverySystem::Remove( TRecoveryId aId )
    {
    CPhoneRecoverySystem* instance = static_cast<CPhoneRecoverySystem*> 
        ( CCoeEnv::Static( KUidRecoverySystemSingleton ) );
    
    if ( instance )
        {
        instance->RemoveId( aId );
        }
    }
    
// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::RemoveId
// -----------------------------------------------------------------------------
//
void CPhoneRecoverySystem::RemoveId( TRecoveryId aId )
    {
    if ( iRecoverySystem )
        {
        iRecoverySystem->Remove( aId );
        RemoveFromContainer( aId );
        }
    }
    
// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::SetState
// -----------------------------------------------------------------------------
//
void CPhoneRecoverySystem::SetState( TRecoveryId aId, CTeleRecoverySystem::TRecoveryState aState )
    {
    iRecoverySystem->SetState( 
        aId, 
        aState );
    }
    
// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::ResetPending
// -----------------------------------------------------------------------------
//
void CPhoneRecoverySystem::ResetPending()
    {
    iRecoverySystem->ResetPending();
    }
    
// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::RecoverNow
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhoneRecoverySystem::RecoverNow( TRecoveryId aId,
    TRecoveryPriority aPriority,
    TBool aAllSteps )
    {
    __ASSERT_DEBUG( iRecoverySystem, Panic( EPhoneUtilsInvariant ) );
    
    if ( !iRecoverySystem )
        {
        TRAPD( error, iRecoverySystem = CreateRecoverySystemL() )
        if( error != KErrNone )
            {
            return error;
            }
        }
    return iRecoverySystem->RecoverNow( aId, aPriority, aAllSteps );
    }

// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::EnablePreconditionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRecoverySystem::EnablePreconditionL()
    {
    __ASSERT_DEBUG( iRecoverySystem, Panic( EPhoneUtilsInvariant ) );
    
    if ( !iRecoverySystem )
        {
        iRecoverySystem = CreateRecoverySystemL();
        }
        
    iRecoverySystem->EnablePrecondition();
    }

// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::AddIdToContainer
// -----------------------------------------------------------------------------
//
void CPhoneRecoverySystem::AddIdToContainer( TRecoveryId aId )
    {
    __ASSERT_DEBUG( iIdContainer.Count() == KIdContainerSize, Panic( EPhoneUtilsInvariant ) );
    for( TInt i = 0; i < KIdContainerSize; i++ )
        {
        if( iIdContainer[ i ] == KEmptySlot )
            {
            iIdContainer[ i ] = aId;
            return;             
            }
        }
    
    // All slots checked, no space - throw exception    
    throw KErrNoSpace;      
    }

// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::RemoveFromContainer
// -----------------------------------------------------------------------------
//
void CPhoneRecoverySystem::RemoveFromContainer( TRecoveryId aId )
    {
    __ASSERT_DEBUG( iIdContainer.Count() == KIdContainerSize, Panic( EPhoneUtilsInvariant ) );
    for( TInt i = 0; i < KIdContainerSize; i++ )
        {
        if( iIdContainer[ i ] == aId )
            {
            iIdContainer[ i ] = KEmptySlot;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRecoverySystem::RemoveAllIdsFromContainer
// -----------------------------------------------------------------------------
//
void CPhoneRecoverySystem::RemoveAllIdsFromContainer()
    {
    __ASSERT_DEBUG( iIdContainer.Count() == KIdContainerSize, Panic( EPhoneUtilsInvariant ) );
    for( TInt i = 0; i < KIdContainerSize; i++ )
        {
        if( iIdContainer[ i ] != KEmptySlot )
            {
            RemoveId( iIdContainer[ i ] );
            }
        }       
    }

//  End of File  
