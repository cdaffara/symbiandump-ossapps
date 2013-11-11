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
* Description: This file contains CTestSetStartedCasesContainer 
* class definition.
*
*/

// INCLUDE FILES

#include <aknlists.h>  // ListBox
#include <AknIconArray.h>  // CAknIconArray
#include <badesca.h> // CDesCArray
#include <eikclbd.h> // CColumnListBoxData
#include <aknnotewrappers.h>

#include <Stifui.rsg>

//#include "UIStoreIf.h"         
//#include "UIStore.h"            
//#include "UIEngine.h"           
//#include "UIEngineContainer.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>

#include "TestSetStartedCasesContainer.h"
#include "ShowStartedCasesContainer.h"
#include "Stifui.hrh"
#include "AppUIAppUi.h"

#include "MenuListBox.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();
    
	iListBox = CMenuListBox::NewL(ETestSetStartedCasesViewId); /*CAknSingleStyleListBox();*/
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver );
    iListBox->ConstructL(this, EAknListBoxSelectionList);

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

    if ( iUIStore )
        {
        
        iListBoxModel = CStartedTestsListBoxModel::NewL( &iStartedTestsPtrs );
        iListBox->Model()->SetItemTextArray(iListBoxModel);
        ConstructListBoxModelL();
        iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
        iListBox->HandleItemAdditionL();

        }
    else
        {
        //User::Leave( syy?? )
        }
	iListBox->ActivateL();
	((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::ConstructListBoxModelL
// 
// Constructs list box model without any filtering.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::ConstructListBoxModelL()
    {
    TInt items(0);
    TInt index = ((CAppUIAppUi*)iCoeEnv->AppUi())->iStartedTestSet;
    
    // Check if there is started test sets and if there is,
    // show test set´s info.
    if ( index != -1 )
        {
        const RRefArray<CStartedTestCase>* startedTestCases;
        CStartedTestSet* startedTestSet =
     	    &iUIStore->StartedTestSetL( index );
        startedTestCases = &startedTestSet->TestCases();
        
        const TInt KStartedCaseCount = startedTestCases->Count();

        items = iStartedTestsPtrs.Count();
        iStartedTestsPtrs.Reset(); // Remove all pointers (does not delete objects)
        iListBox->HandleItemRemovalL();
        
        const TInt KSelectedMode = ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;
        
        // Loop through all started cases
        for( TInt i=0; i < KStartedCaseCount; i++ )
            {
            switch ( KSelectedMode )
                {
                case EShowAllStartedCases:
                    {
                    iStartedTestsPtrs.Append( startedTestCases->operator[](i) );
                    break;
                    }
                case EShowOngoingCases:
                    {
                    // Note: PAUSE IS ALSO RUNNIN STATUS
                    if ( startedTestCases->operator[](i).Status() & CUIStoreIf::EStatusRunning )
                        {
                        iStartedTestsPtrs.Append( startedTestCases->operator[](i) );
                        }
                    break;
                    }
                case EShowPassedCases:
                    {
                    if ( startedTestCases->operator[](i).Status() & CUIStoreIf::EStatusPassed )
                        {
                        iStartedTestsPtrs.Append( startedTestCases->operator[](i) );
                        }
                    break;
                    }
                case EShowPausedCases:
                    {
                    if ( startedTestCases->operator[](i).Status() & CUIStoreIf::EStatusRunning )
                        {
                        if( startedTestCases->operator[](i).UIEngineContainer().State() == CUIEngineContainer::EPaused )
                            {
                            iStartedTestsPtrs.Append( startedTestCases->operator[](i) );
                            }
                        }
                    break;
                    }
                case EShowFailedCases:
                    {
                    if ( startedTestCases->operator[](i).Status() & CUIStoreIf::EStatusFailed )
                        {
                        iStartedTestsPtrs.Append( startedTestCases->operator[](i) );
                        }
                    break;
                    }
                case EShowCrashedAbortedCases:
                    {
                    if ( startedTestCases->operator[](i).Status() & CUIStoreIf::EStatusAborted
                    		||
                    	 startedTestCases->operator[](i).Status() & CUIStoreIf::EStatusCrashed)
                        {
                        iStartedTestsPtrs.Append( startedTestCases->operator[](i) );
                        }
                    break;
                    }
                }
        }   

        if ( items < iStartedTestsPtrs.Count() )
            {
            iListBox->HandleItemAdditionL();
            }
        else
            {
            iListBox->HandleItemRemovalL();
            }
           
        }
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::~CTestSetStartedCasesContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetStartedCasesContainer::~CTestSetStartedCasesContainer()
    {
    delete iListBox;
    
    iStartedTestsPtrs.Close(); // does not delete objects whose pointers are contained in the array
    
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CTestSetStartedCasesContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CTestSetStartedCasesContainer::ComponentControl(TInt aIndex) const
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
// CTestSetStartedCasesContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CTestSetStartedCasesContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (aType != EEventKey)
    {
		return EKeyWasNotConsumed;
	}

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
// CTestSetStartedCasesContainer::SelectedTestCase
// 
// Returns reference to currently selected test case in view (listbox).
// ----------------------------------------------------------------------------
//
CStartedTestCase* CTestSetStartedCasesContainer::SelectedTestCase()
    {
    if ( iStartedTestsPtrs.Count() > 0 )
        {
        return &(iStartedTestsPtrs[ iListBox->CurrentItemIndex() ]);
        }
    else
        {
        return NULL;
        }
/*
    //if ( iListBox->ItemExists(0)  ) // Check that list box is not empty
    if ( ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount()  ) // Check that list box is not empty
        {
        return &(iStartedTestsPtrs[ iListBox->CurrentItemIndex() ]);
        }
    else
        {
        return NULL;
        }
*/
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::DrawListBox
// 
// Draws the list box.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::DrawListBox()
    {
    if ( iListBox )
        {
        iListBox->DrawNow();
        }

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::HandleItemAdditionL
// 
// Handles the addition of an item to the model.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::HandleItemAdditionL()
    {
    iListBox->HandleItemAdditionL();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::HandleItemRemovalL
// 
// Handles the removal of an item from the model.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::HandleItemRemovalL()
    {
    //TInt itemIndexBeforeRemoval = iListBox->CurrentItemIndex();

    iListBox->HandleItemRemovalL();

    // HandleItemRemovalL "loses selection" if current item is removed
    // -> we have to check it and set one item as current item to make it possible for
    // user to select one item from items left after remove
    if ( iListBox->CurrentItemIndex() == -1 ) // No item selected
        if ( ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount() > 0 ) // there are items
            SetCurrentItemIndex(0);

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::TestsInViewCount
// 
// Returns count of test cases in view.
// ----------------------------------------------------------------------------
//
TInt CTestSetStartedCasesContainer::TestsInViewCount()
    {
    return ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::CurrentTestCase
// 
// Returns pointer to currently selected test case.
// ----------------------------------------------------------------------------
//
CStartedTestCase* CTestSetStartedCasesContainer::CurrentTestCase()
    {
    return &iStartedTestsPtrs[ iListBox->CurrentItemIndex() ];
    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::CurrentItemIndex
// 
// Gets the index number of the current item in the view.
// ----------------------------------------------------------------------------
//
TInt CTestSetStartedCasesContainer::CurrentItemIndex()
    {
    return iListBox->CurrentItemIndex();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::SetCurrentItemIndex
// 
// Sets the current item.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::SetCurrentItemIndex(TInt aIndex)
    {
    TInt itemCount = ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount();
    if ( aIndex < itemCount )
        {
        iListBox->SetCurrentItemIndex(aIndex);
        }
    else
        {
        iListBox->SetCurrentItemIndex(itemCount);
        }

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::ResetListBox
// 
// Resets the selection indices, top and current item indices,
// the selection, and the horizontal scroll offset of this list box.
// This function does not redraw the list box.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::ResetListBox()
    {
    iListBox->Reset();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::RemoveSelectedExecutionsL
// 
// Removes items from list box.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::RemoveSelectedExecutionsL()
    {
    TInt indexOfRemovedExecution = 0;
    iStartedTestsPtrs.Remove(indexOfRemovedExecution);
    HandleItemRemovalL();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::RemoveAllExecutionsInViewL
// 
// Removes all started test cases from list box.
// ----------------------------------------------------------------------------
//
void CTestSetStartedCasesContainer::RemoveAllExecutionsInViewL()
    {
    iStartedTestsPtrs.Reset(); //Empties the array, does not delete the objects whose pointers are contained in the array
    HandleItemRemovalL();

    }

// ----------------------------------------------------------------------------
// CTestSetStartedCasesContainer::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//	
void CTestSetStartedCasesContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
