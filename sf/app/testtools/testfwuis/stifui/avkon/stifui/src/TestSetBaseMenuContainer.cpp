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
* Description: This file contains CTestSetBaseMenuContainer class 
* definition.
*
*/

// INCLUDE FILES
#include "TestSetBaseMenuContainer.h"
#include "Stifui.hrh"

#include <Stifui.rsg>

#include <aknlists.h>  //ListBox
#include <barsread.h> // for TResourceReader

#include <aknnotewrappers.h>

#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    CreateWindowL();
     
	iListBox = CMenuListBox::NewL(ETestSetMenuViewId);
	iListBox->SetContainerWindowL( *this );
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC( reader, R_TESTSETBASE_MENU_LISTBOX );
	iListBox->SetListBoxObserver( aListBoxObserver );
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

// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::~CTestSetBaseMenuContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetBaseMenuContainer::~CTestSetBaseMenuContainer()
    {
    if ( iListBox )
		{
		iListBox->Reset();
    	delete iListBox;
    	}	
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CTestSetBaseMenuContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CTestSetBaseMenuContainer::ComponentControl(TInt aIndex) const
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
// CTestSetBaseMenuContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::OfferKeyEventL
// 
// Handles key events..
// ----------------------------------------------------------------------------
//
TKeyResponse CTestSetBaseMenuContainer::OfferKeyEventL(
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
// CTestSetBaseMenuContainer::GetActiveLine
// 
// Get currently selected items index.
// ----------------------------------------------------------------------------
//
TInt CTestSetBaseMenuContainer::GetActiveLine()
	{
	return iListBox->CurrentItemIndex();
	}

// ----------------------------------------------------------------------------
// CTestSetBaseMenuContainer::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CTestSetBaseMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
