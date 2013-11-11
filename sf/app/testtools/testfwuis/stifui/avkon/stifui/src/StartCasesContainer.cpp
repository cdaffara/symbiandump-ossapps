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
* Description: This file contains CStartCasesContainer class 
* definition
*
*/

// INCLUDE FILES
#include <aknlists.h>   // ListBox
#include <AknIconArray.h> // CAknIconArray
#include <aknnotewrappers.h>

#include <eikclbd.h> // CColumnListBoxData

#include "StartCasesContainer.h"
#include "StartCasesView.h"
#include <Stifui.rsg>
#include "Stifui.hrh"
#include "AppUIAppUi.h"

#include "Container.h"
#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStartCasesContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CStartCasesContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ConstructL"));

	iParentView = (CStartCasesView*)aListBoxObserver;
    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();

    iTestCasesInView.Reset();
        //((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: iTCInV Reset"));
    
	iListBox = CMenuListBox::NewL(EStartCaseMenuViewId);
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver /*this*/ ); // jos peritty MEikListBoxObserver:sta
    iListBox->ConstructL(this, EAknListBoxMarkableList /*EAknListBoxSelectionList*/);

    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ListBox constructed"));

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    if ( iUIStore )
        {
        RRefArray<CTestInfo> allCases;
        TInt ret = iUIStore->TestCases( allCases );
        if( KErrNone != ret )
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("iUIStore->TestCases() fails"));
            allCases.Reset();
            allCases.Close();
            User::Leave( ret );
            }

        const TInt KTestCaseCount = allCases.Count();
        for (TInt i=0; i < KTestCaseCount; i++)
            {
            ret = iTestCasesInView.Append( &allCases[i] );
            if( ret != KErrNone )
            	{
            	((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("Test case append fails with: %d"), ret );
    			User::Leave( ret );
	            }
            }

        iListBoxModel = CTestCaseListBoxModel::NewL(&iTestCasesInView);
             ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ListBox model"));

        iListBox->Model()->SetItemTextArray(iListBoxModel);
             ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ListBox model set"));

        allCases.Reset();
        allCases.Close();
        }
    else
        {
        User::Leave( KErrGeneral );
        }

    // Creates graphic.
    SetGraphicIconL( iListBox );
       ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: icons created"));

    iListBox->ActivateL();
    ((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CStartCasesContainer::~CStartCasesContainer
// 
// Destructor
// ----------------------------------------------------------------------------
//
CStartCasesContainer::~CStartCasesContainer()
    {
    }

// ----------------------------------------------------------------------------
// CStartCasesContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CStartCasesContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CStartCasesContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CStartCasesContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CStartCasesContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CStartCasesContainer::ComponentControl(TInt aIndex) const
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
// CStartCasesContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CStartCasesContainer::Draw(const TRect& /*aRect*/) const
    {
    }

// ----------------------------------------------------------------------------
// CStartCasesContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CStartCasesContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType )
    {
	if (aType != EEventKey)
	    {
		return EKeyWasNotConsumed;
	    }

	if (iListBox)
		{
		//if multiple items selected
		if ( iListBox->SelectionIndexes()->Count() > 0 )
			{
			TUint mask = 0x40488;
			
			//if event is enter key,
			//don´t send it to listbox
			if ( aKeyEvent.iScanCode == 0xa7 
				&& ( aKeyEvent.iModifiers & mask ) == 0 )
				{
				iParentView->HandleListBoxEventL( iListBox, MEikListBoxObserver::EEventEnterKeyPressed );
				return EKeyWasConsumed;	
				}
			}
			
		return iListBox->OfferKeyEventL( aKeyEvent, aType );
		}
	else
		{
		return EKeyWasNotConsumed;
		}
    }

// ----------------------------------------------------------------------------
// CStartCasesContainer:::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CStartCasesContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
