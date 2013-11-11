/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include <e32keys.h>
#include <aknViewAppUi.h>
#include <coemop.h>
#include <aknnavi.h>
#include <aknstaticnotedialog.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <AknDef.h>
#include <imageprintapp.rsg>

#include "cimageprintsettingscontainer.h"
#include "imageprintapp.hrh"
#include "cpropertiesboxmodel.h"
#include "cimageprintappui.h"
#include "imageprint.h"
#include "cimageprintdoc.h"
#include "cdiscoverydlgmanager.h"
#include "cimageprintengine.h"
#include "ciffactory.h"
#include "clog.h"
#include "printapputil.h"
#include "midleprinteventregisterif.h"
#include "cimgpprintutils.h"
#include "muimode.h"

//  CONSTRUCTION
CImagePrintSettingsContainer* CImagePrintSettingsContainer::NewL(
    MPrintSettings* aSettings,
    MObjectProvider* aProvider,
    TRect aClientRect,
    CImagePrintAppUi* aAppUi,
    MUIMode* aUIMode )
    {
    CImagePrintSettingsContainer* self =
        new ( ELeave ) CImagePrintSettingsContainer( aSettings, aAppUi, aUIMode );
    CleanupStack::PushL( self );
    self->ConstructL( aProvider, aClientRect );
    CleanupStack::Pop( self );

    return self;
    }

// Default constructor
CImagePrintSettingsContainer::CImagePrintSettingsContainer(
    MPrintSettings* aSettings,
    CImagePrintAppUi* aAppUi,
    MUIMode* aUIMode ) :
    iPrintSettings( aSettings ),
    iAppUi( aAppUi ),
    iUIMode( aUIMode )
    {
    }

// 2nd phase constructor
void CImagePrintSettingsContainer::ConstructL(
    MObjectProvider* aProvider,
    const TRect& aRect )
    {
    // Create a window for this application view
    SetMopParent( aProvider );
    CreateWindowL();
    CImagePrintDoc* doc = static_cast<CImagePrintDoc*>( iAppUi->Document() );
    iModel = CPropertiesBoxModel::NewL( *( doc->Engine() ), iPrintSettings, this );

    // Initialize list box outlook
    iListBox = new ( ELeave ) CAknSettingStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iEikonEnv->EikAppUi()->AddToStackL( iListBox );

    // Set the model & observer
    iListBox->SetListBoxObserver( iModel );
    iListBox->Model()->SetItemTextArray( iModel );
    // List box won't delete model, the Controller object will take care of it
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set the windows size
    SetRect( aRect );
    iListBox->SetSize( iListBox->MinimumSize() );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
	}

// Destructor
CImagePrintSettingsContainer::~CImagePrintSettingsContainer()
    {
	if ( iUSBConnectionWaitNote )
		{
		delete iUSBConnectionWaitNote;
		}
    iEikonEnv->EikAppUi()->RemoveFromStack( iListBox );
    delete iListBox;
    delete iModel;
    }


// Return the amount of compound controls
TInt CImagePrintSettingsContainer::CountComponentControls() const
    {
    return 1;
	}

// Gets the specified component of a compound control.
CCoeControl* CImagePrintSettingsContainer::ComponentControl(
    TInt aIndex ) const
    {
    CCoeControl* cntrl = 0;

    if ( aIndex == 0 )
        {
        cntrl = iListBox;
        }

    return cntrl;
    }

// Draws the empty view
void CImagePrintSettingsContainer::Draw(
    const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear();
    }

// Starts editing the active item
void CImagePrintSettingsContainer::EditItemL()
    {
    iModel->HandleListBoxEventL( iListBox,
                                 MEikListBoxObserver::EEventEnterKeyPressed );
                                 
    ChangeStatusPaneTextL( iModel->NumberOfCopies() );
    }

void CImagePrintSettingsContainer::DoDiscoveryL( TInt aProtocol, TBool /*aHidden*/ )
	{
	TBool ret = iUIMode->ChangeDefaultPrinterL( aProtocol );
	}

void CImagePrintSettingsContainer::ChangeStatusPaneTextL( const TInt aCopiesOfImage )
    {    
     iAppUi->SetNavipaneTextL( aCopiesOfImage );
   }

void CImagePrintSettingsContainer::UpdateListBox()
	{
	iListBox->SizeChanged();
	}

void CImagePrintSettingsContainer::UpdateListSize( TRect /*aRect*/ )
    {
    iListBox->SetSize( iListBox->MinimumSize() );
    }

void CImagePrintSettingsContainer::SizeChanged()
	{
	iListBox->SetSize( iListBox->MinimumSize() );
	}

void CImagePrintSettingsContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect = iAppUi->ClientRect();
        SetRect( rect );
        UpdateListSize( rect );
        DrawNow();
        }
    }

//  End of File
