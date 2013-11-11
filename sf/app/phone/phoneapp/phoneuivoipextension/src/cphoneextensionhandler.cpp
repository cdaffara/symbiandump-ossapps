/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneExtensionHandler class.
*
*/

// INCLUDES

#include <eikmenup.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include "phoneresourceids.h"
#include <avkon.rsg>
#include <StringLoader.h>

#include "cphoneextensionhandler.h"
#include "cphonemainresourceresolver.h"
#include "cphonelogger.h"
#include "phoneappvoipcommands.hrh"
#include "phonerssvoip.h"
#include "cphonemenuextensionwrapper.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneExtensionHandler::CPhoneExtensionHandler()
    {
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::~CPhoneStateIdleVoIP()
// Destructor
// -----------------------------------------------------------
//
CPhoneExtensionHandler::~CPhoneExtensionHandler()
    {    
    iPlugins.ResetAndDestroy();
    REComSession::FinalClose();
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::ConstructL()
// Constructor
// -----------------------------------------------------------
//
void CPhoneExtensionHandler::ConstructL()
    {
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::NewL()
// Constructor
// -----------------------------------------------------------
//
CPhoneExtensionHandler* CPhoneExtensionHandler::NewL( )
    {
    CPhoneExtensionHandler* self = new (ELeave) CPhoneExtensionHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneExtensionHandler::HandleCommandL( 
        TUint aServiceId,
        TInt aCommand )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneExtensionHandler::HandleCommandL" );
    
    CTelMenuExtension* interface = MenuExtensionL( aServiceId );
    
    return interface->HandleCommandL( aCommand );
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::ModifyInCallMenuL
// -----------------------------------------------------------
//
void CPhoneExtensionHandler::ModifyInCallMenuL(
        TUint aServiceId,
        RArray<CTelMenuExtension::TCallInfo>& aArray,
        TInt aResourceId, 
        CEikMenuPane& aMenuPane )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneExtensionHandler::ModifyInCallMenuL" );
    
    if ( aResourceId != R_AVKON_MENUPANE_FEP_DEFAULT &&
        aResourceId != R_AVKON_MENUPANE_EDITTEXT_DEFAULT &&
        aResourceId != R_AVKON_MENUPANE_LANGUAGE_DEFAULT &&
        R_PHONEUI_NEW_CALL_MENU_VOIP != aResourceId )
        {

        // Load xSP ECOM plugin 
        CTelMenuExtension* interface = MenuExtensionL( aServiceId );
        
        __PHONELOG( EBasic, PhoneUIVoIPExtension, 
            "CPhoneExtensionHandler::ModifyInCallMenuL, plugin exists" );
        // Add unattended transfer option to menu
        AddUnattendedTransferMenuItemL( aMenuPane );
        
        // Let plugin modify menu pane
        interface->ModifyExtensionsCallMenuL( aArray, aMenuPane );
        }
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::ResolveImplementationUidL
// -----------------------------------------------------------
//
TUid CPhoneExtensionHandler::ResolveImplementationUidL( 
        TUint32 aServiceId, TServicePropertyName aPropertyName )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneExtensionHandler::ResolveImplementationUidL" );
    
    TUid implUid = { 0 };
    CSPEntry* entry = CSPEntry::NewLC();

    CSPSettings* spSettings = CSPSettings::NewLC(); 
    
    User::LeaveIfError(
        spSettings->FindEntryL( aServiceId, *entry ) ); 

    CleanupStack::PopAndDestroy( spSettings ); 

    const CSPProperty* property = NULL;
    User::LeaveIfError( entry->GetProperty( property, aPropertyName ) );
    
    if ( property ) 
        {
        TInt temp(0);
        User::LeaveIfError( property->GetValue( temp ) );
        implUid.iUid = temp;
        }

    CleanupStack::PopAndDestroy( entry );
    return implUid;
    }

// -----------------------------------------------------------
// CPhoneExtensionHandler::AddUnattendedTransferMenuItem
// -----------------------------------------------------------
//
void CPhoneExtensionHandler::AddUnattendedTransferMenuItemL( 
        CEikMenuPane& aMenuPane )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneExtensionHandler::AddUnattendedTransferMenuItemL" );
    
    HBufC* string = StringLoader::LoadLC( 
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneVoIPUnattendedTransferOption ) );
    CEikMenuPaneItem::SData data;
    data.iCascadeId=0;
    data.iText = *string;
    data.iFlags=0;
    data.iCommandId = EPhoneInCallCmdUnattendedTransfer;
 
    // Try to find index of EPhoneInCallCmdGoToIdle command
    // unattended transfer item will be placed above it if found
    TInt index = KErrNotFound;
    TInt menuItemCount = aMenuPane.NumberOfItemsInPane();
    for ( TInt i= 0 ; i < menuItemCount; i++ )
        {
        if ( aMenuPane.MenuItemCommandId( i ) == EPhoneInCallCmdGoToIdle )
            {
            index = i;
            break;
            }
        }
   
    if ( KErrNotFound < index )
        {
        aMenuPane.InsertMenuItemL( data, index );  
        }
    else
        {
        aMenuPane.AddMenuItemL( data ); 
        }
   
    CleanupStack::PopAndDestroy( string );   
    }

// ----------------------------------------------------------
// CPhoneExtensionHandler::MenuExtensionL
// (other items were commented in a header).
// -----------------------------------------------------------
//
CTelMenuExtension* CPhoneExtensionHandler::MenuExtensionL( TUint aServiceId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneExtensionHandler::MenuExtensionL" );
    
    TUid pluginUid = ResolveImplementationUidL( aServiceId, 
        EPropertyCallMenuHandlerPluginId );
    
    CPhMenuExtensionWrapper* wrapper = NULL;
    TInt pluginInd = 
        iPlugins.Find( pluginUid, CPhMenuExtensionWrapper::MatchByUid );
    if ( KErrNotFound == pluginInd )
        {
        wrapper = CPhMenuExtensionWrapper::NewLC( pluginUid );
        iPlugins.AppendL( wrapper );
        CleanupStack::Pop( wrapper );
        }
    else
        {
        wrapper = iPlugins[pluginInd];
        }
    
    __ASSERT_ALWAYS( NULL != wrapper, User::Leave( KErrNotFound ) );
    return wrapper;
    }


// End of File
