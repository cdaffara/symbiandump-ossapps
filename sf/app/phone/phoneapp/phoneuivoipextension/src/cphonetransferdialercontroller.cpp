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
* Description: Implementation of CPhoneTransferDialerController class.
*
*/


// INCLUDES
#include <e32base.h>

#include <akntoolbar.h>
#include <AknsItemID.h>
#include <StringLoader.h>
#include "phoneresourceids.h"
#include "phoneui.pan"
#include "phonelogger.h"
#include <data_caging_path_literals.hrh>
#include "cphonetransferdialercontroller.h"
#include "cphonemainresourceresolver.h"
#include "phonerssvoip.h"
#include "phoneappcommands.hrh"
#include "phoneappvoipcommands.hrh"


// CONSTANTS
_LIT ( KPhoneMifFileName, "phoneui.mif" );

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//  
CPhoneTransferDialerController::CPhoneTransferDialerController()
    {
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::~CPhoneTransferDialerController()
// Destructor
// -----------------------------------------------------------
//
CPhoneTransferDialerController::~CPhoneTransferDialerController()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::~CPhoneTransferDialerController()");
    delete iNumberEntryPromptText;
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::NewL()
// Constructor
// -----------------------------------------------------------
//
CPhoneTransferDialerController* CPhoneTransferDialerController::NewL()
    {
    CPhoneTransferDialerController* self = new (ELeave) 
        CPhoneTransferDialerController();
  
    return self;
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::Initialize
// -----------------------------------------------------------
//
void CPhoneTransferDialerController::InitializeL( CAknToolbar& aToolbar )
    {  
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::InitializeL()"); 
    
    if ( !iIsInitialized )
        {    
        __PHONELOG1( 
                EAll, 
                PhoneUIVoIPExtension, 
                "Toolbar control count:=%d", 
                aToolbar.CountComponentControls());
        iToolbar = &aToolbar;
        iIsInitialized = ETrue;
        }
   
    iNumberEntryIsEmpty = ETrue;
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::CbaResourceId
// -----------------------------------------------------------
//
TInt CPhoneTransferDialerController::CbaResourceId() const
    {
    return EPhoneVoIPTransferDialerSoftkeys;
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::MenuResourceId
// -----------------------------------------------------------
//
TInt CPhoneTransferDialerController::MenuResourceId() const
    {
    if ( iNumberEntryIsEmpty )
        {
        return EPhoneVoIPTransferDialerNumberEntryEmptyMenubar;
        }
    else
        {
        return EPhoneVoIPTransferDialerNumberEntryNotEmptyMenubar;
        }
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::NumberEntryPromptTextL
// -----------------------------------------------------------
//
const TDesC& CPhoneTransferDialerController::NumberEntryPromptTextL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::NumberEntryPromptTextL()");
    if ( !iNumberEntryPromptText )
        {
        iNumberEntryPromptText = StringLoader::LoadL( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneVoIPTransferAddress ) );
        }  
    
    return *iNumberEntryPromptText;
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::HandleNumberEntryIsEmpty
// -----------------------------------------------------------
//
void CPhoneTransferDialerController::HandleNumberEntryIsEmpty( TBool aIsEmpty )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::HandleNumberEntryIsEmpty()");
    __ASSERT_DEBUG( iToolbar, Panic( EPhoneCtrlInvariant ) );
    iToolbar->SetItemDimmed( EPhoneCmdTransferDialerOk, aIsEmpty, EFalse );
    iToolbar->SetItemDimmed( EPhoneCmdTransferDialerSearch, !aIsEmpty, EFalse );
    iToolbar->SetItemDimmed( EPhoneDialerCmdClear, aIsEmpty, EFalse );
    iNumberEntryIsEmpty = aIsEmpty;
    }

// -----------------------------------------------------------
// CPhoneTransferDialerController::ShowButtons
// -----------------------------------------------------------
//
void CPhoneTransferDialerController::ShowButtons( TBool aShow )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::ShowButtons()");
    __ASSERT_DEBUG( iToolbar, Panic( EPhoneCtrlInvariant ) );
    if ( aShow )
        {
        iToolbar->SetItemDimmed( EPhoneCmdTransferDialerOk, ETrue, EFalse ); 
        iToolbar->SetItemDimmed( EPhoneCmdTransferDialerSearch, EFalse, EFalse ); 
        iToolbar->SetItemDimmed( EPhoneDialerCmdClear, ETrue, EFalse ); 
        }
    }

// ---------------------------------------------------------------------------
// PhoneTransferDialerController::CreateButtonLC
// ---------------------------------------------------------------------------
//
CAknButton* CPhoneTransferDialerController::CreateButtonLC( 
    TInt aNormalIconId,
    TInt aNormalMaskId,
    const TDesC& aTooltipText,
    const TAknsItemID& aSkinIconId ) const
    {   
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::CreateButtonLC()");
    TFileName mifPath( KDriveZ );
    mifPath.Append( KDC_APP_BITMAP_DIR );
    mifPath.Append( KPhoneMifFileName );

    CAknButton* button = CAknButton::NewLC( 
        mifPath,
        aNormalIconId,
        aNormalMaskId,
        -1, -1, // dimmed
        -1, -1, // pressed
        -1, -1, // hover
        KNullDesC,
        aTooltipText, // help
        0, // flags
        0, // state flags
        aSkinIconId );
                                                                 
    button->SetFocusing( EFalse );
    button->SetBackground( iToolbar );

    return button;
    }

// ---------------------------------------------------------------------------
// CPhoneTransferDialerController::GetTooltipText
//
// Tooltip texts for toolbar buttons
// ---------------------------------------------------------------------------
//
HBufC* CPhoneTransferDialerController::GetTooltipTextL( TInt aCommandId ) const
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneTransferDialerController::GetTooltipTextL()");
    TInt resourceId = 0;
    HBufC* tooltip = NULL;
    switch( aCommandId )
        {                            
        case EPhoneCmdTransferDialerOk:
            resourceId = R_VOIP_DIALER_TOOLTIP_ACCEPT;
            break;    
        case EPhoneCmdTransferDialerSearch:
            resourceId = R_VOIP_DIALER_TOOLTIP_SEARCH;
            break;                      
       default:
           tooltip = KNullDesC().Alloc();
           break;
       }
           
    if ( resourceId )
        {
        tooltip = StringLoader::LoadL( resourceId, CCoeEnv::Static() );
        }
    return tooltip;
    }

// ---------------------------------------------------------------------------
// CPhoneTransferDialerController::SkinId
// ---------------------------------------------------------------------------
//    
TAknsItemID CPhoneTransferDialerController::SkinId( TInt /*aIconIndex*/ ) const
    {
    TAknsItemID skinId;
    skinId = KAknsIIDNone; 
    return skinId;  
    }
