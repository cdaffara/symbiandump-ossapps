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
* Description: This file contains CTestCaseMenuContainer class
* definition.
*
*/

// INCLUDE FILES
#include "TestCaseMenuContainer.h"
#include "Stifui.hrh"
#include <Stifui.rsg>

#include <aknlists.h>  //ListBox
#include <barsread.h> // for TResourceReader

#include <aknnotewrappers.h>

#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
     
	iListBox = CMenuListBox::NewL(ETestCaseMenuViewId);
	iListBox->SetContainerWindowL( *this );
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC( reader, R_TESTCASE_MENU_LISTBOX );
	iListBox->SetListBoxObserver( this );
	iListBox->ConstructFromResourceL( reader );
	CleanupStack::PopAndDestroy(); // resource stuffs. 

    // Create Scroller control for ListBox and set its visibility
    // !!! Not needed yet because there are only two items in list box.
    // If items are added later, scroller may be taken to use
    //iListBox->CreateScrollBarFrameL(ETrue);
    //iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    iListBox->ActivateL();
    ((CMenuListBox*)iListBox)->SetPreviousFocus();
    
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CTestCaseMenuContainer::~CTestCaseMenuContainer()
    {
    if ( iListBox )
		{
		iListBox->Reset();
    	delete iListBox;
    	}	
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CTestCaseMenuContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CTestCaseMenuContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
		case 0: 
			return iListBox;
        default:
            return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }


// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::HandleListBoxEventL
// 
// Handles list box events.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuContainer::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
		    HandleSelectedListItemL( aListBox->CurrentItemIndex() );

		}	

	}
	
// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::HandleSelectedListItemL
// 
// Method HandleSelectedListItemL handles valid index.
// ----------------------------------------------------------------------------
//
void CTestCaseMenuContainer::HandleSelectedListItemL( TInt aIndex )
	{	 
		TInt selection = aIndex;

	    switch ( selection )
			{
		    case 0:
				//Vaihda StartCasesView aktiiviseksi
				( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdGoToStartCasesView);
				break;
			case 1:
				( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdGoToStartedCasesView);
				break;
	        default:
				break;
			}
	}	

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CTestCaseMenuContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType)
    {
	if (aType != EEventKey)
    	{
		return EKeyWasNotConsumed;
	    }

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
			{   /* Left ja right key eventit valitetaan AppUille joka hoitaa siirtymisen toisiin nakymiin */
			return EKeyWasNotConsumed;
            //break;
            }
        default:
			{
			//Muut Key eventit valitetaan listboxille, joka hoitaa ne
			if (iListBox)
				{
				return iListBox->OfferKeyEventL( aKeyEvent, aType );
				}
			}
        }     
    return EKeyWasNotConsumed;        
}

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::GetActiveLine
// 
// Get currently selected items index.
// ----------------------------------------------------------------------------
//
TInt CTestCaseMenuContainer::GetActiveLine()
	{
	return iListBox->CurrentItemIndex();
	}

// ----------------------------------------------------------------------------
// CTestCaseMenuContainer::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CTestCaseMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
