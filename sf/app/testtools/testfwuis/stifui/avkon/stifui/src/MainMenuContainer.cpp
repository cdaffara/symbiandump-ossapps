/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains CMainMenuContainer class
* definition.
*
*/

// INCLUDE FILES
#include <aknlists.h>  // ListBox

#include <barsread.h> // for TResourceReader

#include <aknnotewrappers.h>

#include "MainMenuContainer.h"
#include "Stifui.hrh"
#include <Stifui.rsg>
#include "MenuListBox.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMainMenuContainer::ConstructL
// 
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CMainMenuContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

	iListBox = CMenuListBox::NewL(EMainMenuViewId);
	iListBox->SetContainerWindowL( *this );
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC( reader, R_MAIN_MENU_LISTBOX );
	iListBox->SetListBoxObserver( this );
	iListBox->ConstructFromResourceL( reader );
	CleanupStack::PopAndDestroy(); // resource stuffs. 
	iListBox->ActivateL(); // Sets control as ready to be drawn
	
	// retrieve previous position of focus for this view/continer and set focus to this value
	((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();
    }


// ---------------------------------------------------------
// CMainMenuContainer::~CMainMenuContainer
//
// Destructor
// ---------------------------------------------------------
//
CMainMenuContainer::~CMainMenuContainer()
    {
	if( iListBox )
		{
		delete iListBox;
		iListBox = NULL;
		}
	}

// ---------------------------------------------------------
// CMainMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMainMenuContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ---------------------------------------------------------
// CMainMenuContainer::CountComponentControls
//
// Returns number of controls indside this container.
// ---------------------------------------------------------
//
TInt CMainMenuContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------
// CMainMenuContainer::ComponentControl
// 
// Gets the specified component of a compound control.
// ---------------------------------------------------------
//
CCoeControl* CMainMenuContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
		case 0: 
			return iListBox;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CMainMenuContainer::Draw
// 
// Draw a control, called by window server.
// ---------------------------------------------------------
//
void CMainMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    }

// ---------------------------------------------------------
// CMainMenuContainer::HandleListBoxEventL
//
// Handles list box events.
// ---------------------------------------------------------
//
void CMainMenuContainer::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
	{

	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{	
        HandleSelectedListItemL( aListBox->CurrentItemIndex() );
		}

	}
	
// ----------------------------------------------------------------------------
// CMainMenuContainer::HandleSelectedListItemL
//
// Method HandleSelectedListItemL handles valix index.
// ----------------------------------------------------------------------------
//
void CMainMenuContainer::HandleSelectedListItemL( TInt aIndex )
	{	 
		TInt selection = aIndex;
        
        // Change active view.
	    switch ( selection )
        {
		    case 0:
				( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(EAppUIGoToTestCaseMenu);
				break;
			case 1:
				( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(EAppUIGoToModulesMenu);
				break;
			case 2: 
				( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(EAppUIGoToTestSetsMenu);
				break;
	        default:
				break;
		}
	}

// ---------------------------------------------------------
// CMainMenuContainer::OfferKeyEventL
// 
// Handles key events.
// ---------------------------------------------------------
//
TKeyResponse CMainMenuContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType != EEventKey)
	{
		return EKeyWasNotConsumed;
	}

	// Offers key events to list box
	if (iListBox)
	{
		return iListBox->OfferKeyEventL( aKeyEvent, aType );
	}
	else
	{
		return EKeyWasNotConsumed;
	}
}

// ----------------------------------------------------------------------------
// CMainMenuContainer:::GetActiveLine
//
// Get currently selected items index.
// ----------------------------------------------------------------------------
//
TInt CMainMenuContainer::GetActiveLine()
	{
	return iListBox->CurrentItemIndex();
	}

// ----------------------------------------------------------------------------
// CMainMenuContainer:::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CMainMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
