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
* Description: This fiole contains CreatedTestSetMenuContainer class
* definition.
*
*/

// INCLUDE FILES
#include <aknlists.h>   // ListBox
#include <AknIconArray.h> // CAknIconArray
#include <aknnotewrappers.h>

#include <eikclbd.h> // CColumnListBoxData

#include "CreatedTestSetMenuContainer.h"
#include "StartCasesView.h"
#include <Stifui.rsg>
#include "Stifui.hrh"
#include "AppUIAppUi.h"

#include "Container.h"
#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CreatedTestSetMenuContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("CCreatedTestSetMenuContainer: ConstructL"));

	iParentView = (CCreatedTestSetMenuView*)aListBoxObserver;
    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();

    iTestCasesInView.Reset();
    
	iListBox = CMenuListBox::NewL(ECreatedTestSetMenuViewId);
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver /*this*/ ); // 
    iListBox->ConstructL(this, EAknListBoxMarkableList /*EAknListBoxSelectionList*/);

    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("CCreatedTestSetMenuContainer: ListBox constructed"));

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    if ( iUIStore )
        {
        CDesCArray* items = static_cast<CDesCArray*> (iListBox->Model()->ItemTextArray());
        
        //Create list of available Test Set
        RRefArray<TDesC>    allSet;      
        RRefArray<CTestInfo> allCases;
        TInt ret = iUIStore->GetTestSetsList( allSet );
        if( KErrNone != ret )
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("iUIStore->GetTestSetsList() fails"));
            allSet.Reset();
            allSet.Close();
            User::Leave( ret );
            }

        const TInt KTestCaseCount = allSet.Count();
        for (TInt i=0; i <KTestCaseCount; i++)
            {
            _LIT(KItemFromat, "0\t%S\t\t");
     
           RBuf tmp;
           tmp.Create(allSet[i].Length() + 6);
           tmp.Format( KItemFromat, &allSet[i] );
           items->AppendL( tmp );
           tmp.Close();
           }
       ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("CCreatedTestSetMenuContainer: ListBox model"));         
       iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
       iListBox->HandleItemAdditionL();
 
        allSet.Reset();
        allSet.Close();
        }
    else
        {
        User::Leave( KErrGeneral );
        }

    // Creates graphic.
    SetGraphicIconL( iListBox );
       ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("CCreatedTestSetMenuContainer: icons created"));

    iListBox->ActivateL();
    ((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
//CreatedTestSetMenuContainer::~CStartCasesContainer
// 
// Destructor
// ----------------------------------------------------------------------------
//
CCreatedTestSetMenuContainer::~CCreatedTestSetMenuContainer()
    {
    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuContainer::SizeChanged()
    {
    
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
//CreatedTestSetMenuContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CCreatedTestSetMenuContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CCreatedTestSetMenuContainer::ComponentControl(TInt aIndex) const
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
// CreatedTestSetMenuContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    }

// ----------------------------------------------------------------------------
// CreatedTestSetMenuContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CCreatedTestSetMenuContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType )
    {
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
// CreatedTestSetMenuContainer:::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CCreatedTestSetMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
