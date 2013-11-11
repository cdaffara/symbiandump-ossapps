/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhMenuExtensionWrapper class.
*
*/


#include "cphonemenuextensionwrapper.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::CPhMenuExtensionWrapper
// ---------------------------------------------------------------------------
//
CPhMenuExtensionWrapper::CPhMenuExtensionWrapper( 
        TUid aImplementationUid )
    : 
    iPluginUid( aImplementationUid )
    {

    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CPhMenuExtensionWrapper::ConstructL()
    {
    iPlugin = CreateMenuExtensionL();
    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::NewL
// ---------------------------------------------------------------------------
//
CPhMenuExtensionWrapper* CPhMenuExtensionWrapper::NewL( 
        TUid aImplementationUid )
    {
    CPhMenuExtensionWrapper* self = 
        CPhMenuExtensionWrapper::NewLC( aImplementationUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::NewLC
// ---------------------------------------------------------------------------
//
CPhMenuExtensionWrapper* CPhMenuExtensionWrapper::NewLC( 
        TUid aImplementationUid )
    {
    CPhMenuExtensionWrapper* self = 
        new( ELeave ) CPhMenuExtensionWrapper( aImplementationUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::~CPhMenuExtensionWrapper
// ---------------------------------------------------------------------------
//
CPhMenuExtensionWrapper::~CPhMenuExtensionWrapper()
    {
    delete iPlugin;
    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::MatchByUid
// ---------------------------------------------------------------------------
//
TBool CPhMenuExtensionWrapper::MatchByUid( 
        const TUid* aKey, const CPhMenuExtensionWrapper& aWrapper )
    {
    if ( aKey )
        {
        return ( aWrapper.Identifier() == *aKey );
        }
    else
        {
        return EFalse;
        }
    }



// ---------------------------------------------------------------------------
// From class CTelMenuExtension.
// ---------------------------------------------------------------------------
//
void CPhMenuExtensionWrapper::ModifyExtensionsCallMenuL( 
        const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
        CEikMenuPane& aMenuPane )
    {
    iPlugin->ModifyExtensionsCallMenuL( aCallArray, aMenuPane );
    }

// ---------------------------------------------------------------------------
// From class CTelMenuExtension.
// ---------------------------------------------------------------------------
//
void CPhMenuExtensionWrapper::ModifyExtensionsTouchPaneButtonsL( 
        const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
        MTelTouchPaneExtension& aTouchPane )
    {
    iPlugin->ModifyExtensionsTouchPaneButtonsL( aCallArray, aTouchPane );
    }

// ---------------------------------------------------------------------------
// From class CTelMenuExtension.
// ---------------------------------------------------------------------------
//
TBool CPhMenuExtensionWrapper::HandleCommandL( TInt aCommand )
    {
    return iPlugin->HandleCommandL( aCommand );
    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::CreateContactExtensionL
// Needed for testing with mock technique.
// ---------------------------------------------------------------------------
//
CTelMenuExtension* 
    CPhMenuExtensionWrapper::CreateMenuExtensionL() const
    {
    return CTelMenuExtension::NewL( iPluginUid );
    }


// ---------------------------------------------------------------------------
// CPhMenuExtensionWrapper::Identifier
// ---------------------------------------------------------------------------
//
TUid CPhMenuExtensionWrapper::Identifier() const
    {
    return iPluginUid;
    }
