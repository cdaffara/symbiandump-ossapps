/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamDynMenu class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamdynmenu.h"    
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamDynMenu::TPhoneCmdParamDynMenu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamDynMenu::TPhoneCmdParamDynMenu() :
    TPhoneCommandParam(),
    iResourceId( 0 ), 
    iDynMenu ( NULL )
    {
    iParamId = EPhoneParamIdMenu;
    }

// ---------------------------------------------------------
// TPhoneCmdParamDynMenu::SetResourceId
// Sets the menu resource id.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamDynMenu::SetResourceId( 
    TInt aResourceId )
    {
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneUtilsParameterNotInitialized ) );
    iResourceId = aResourceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamDynMenu::SetDynMenu
// Sets the menu pointer
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamDynMenu::SetDynMenu( TAny* aDynMenu )
    {
    __ASSERT_DEBUG( aDynMenu, Panic( EPhoneUtilsParameterNotInitialized ) );
    iDynMenu = aDynMenu;
    }

// ---------------------------------------------------------
// TPhoneCmdParamDynMenu::ResourceId
// Returns the Menu bar resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamDynMenu::ResourceId() const
   {
    __ASSERT_DEBUG( iResourceId, Panic( EPhoneUtilsInvariant ) );
   return iResourceId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamDynMenu::DynMenu
// Returns the current view ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TAny* TPhoneCmdParamDynMenu::DynMenu()
   {
    __ASSERT_DEBUG( iDynMenu, Panic( EPhoneUtilsInvariant ) );
   return iDynMenu;
   }

// End of file
