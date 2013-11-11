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
* Description: This file contains CTestCaseListBoxModel class
* definition.
*
*/

// INCLUDE FILES
#include <aknlists.h>  // ListBox
#include <AknIconArray.h> // CAknIconArray
#include <aknnotewrappers.h>

#include <eikclbd.h> //  CColumnListBoxData

#include "Container.h"
#include <Stifui.rsg>
#include "Stifui.hrh"
#include "AppUIAppUi.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestCaseListBoxModel::NewL
// 
// Two-phased constructor.
// ----------------------------------------------------------------------------
//

CTestCaseListBoxModel* CTestCaseListBoxModel::NewL(RPointerArray<CTestInfo>* aTestCasesInView)
    {
    CTestCaseListBoxModel* self = new ( ELeave ) CTestCaseListBoxModel();
    CleanupStack::PushL( self );
    self->iBuffer = HBufC::NewL( 130 );
    self->iTestCasesInView = aTestCasesInView;
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CTestCaseListBoxModel::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//

void CTestCaseListBoxModel::ConstructL()
    {
    //iBuffer = HBufC::NewLC( KMaxInfoName + KMaxName + 2 );
    }

// ----------------------------------------------------------------------------
// CTestCaseListBoxModel::~CTestCaseListBoxModel
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestCaseListBoxModel::~CTestCaseListBoxModel()
    {
    delete iBuffer;
    }

// ----------------------------------------------------------------------------
// CTestCaseListBoxModel::MdcaCount
// 
// Returns the number of descriptor elements in the array.
// ----------------------------------------------------------------------------
//

TInt CTestCaseListBoxModel::MdcaCount() const
    {
    return iTestCasesInView->Count();
    }

// ----------------------------------------------------------------------------
// CTestCaseListBoxModel::MdcaPoint
// 
// Indexes into a descriptor array.
// ----------------------------------------------------------------------------
//
TPtrC CTestCaseListBoxModel::MdcaPoint(TInt aIndex) const
    {
    CTestInfo* testCasePtr = (*iTestCasesInView)[aIndex];
    TPtr buffer( iBuffer->Des() );
    buffer.Zero();
    buffer.Append(_L("\t"));
    buffer.Append(testCasePtr->TestCaseTitle() );
    return *iBuffer;
    }

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CContainer::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CContainer::~CContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CContainer::~CContainer()
    {
    iTestCasesInView.Close(); // does not delete objects whose pointers are contained in the array
    delete iListBox;
    }


// ----------------------------------------------------------------------------
// CContainer::SetGraphicIconL
// 
// Sets graphic icon using listbox as CEikColumnListBox.
// ----------------------------------------------------------------------------
//
void CContainer::SetGraphicIconL( CEikColumnListBox* aListBox )
	{
	    if ( aListBox )
	        {
	        // Creates gul icon.
	        CAknIconArray* iconArray = new(ELeave) CAknIconArray(1);
	        CleanupStack::PushL( iconArray );
	        
	        GraphicIconL( iconArray ); // Appends graphic data.
	        
	        // Sets graphics as ListBox icon.
	        aListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
	        
	        CleanupStack::Pop();
	        }
	    }

// ----------------------------------------------------------------------------
// CContainer::CurrentItemIndex
// 
// Returns current item index in list box.
// ----------------------------------------------------------------------------
//
TInt CContainer::CurrentItemIndex()
    {
    return iListBox->CurrentItemIndex();
    }

// ----------------------------------------------------------------------------
// CContainer::SetCurrentItemIndex
// 
// Sets current item index in list box.
// ----------------------------------------------------------------------------
//
void CContainer::SetCurrentItemIndex(TInt aCurrentTestCase)
    {
    iListBox->SetCurrentItemIndexAndDraw(aCurrentTestCase);
    }

// ----------------------------------------------------------------------------
// CContainer::GraphicIconL
// 
// Appends graphics data.
// ----------------------------------------------------------------------------
//
void CContainer::GraphicIconL( CArrayPtr<CGulIcon>* aIcons )
    {
    if ( aIcons )
            {
            CFbsBitmap* markBitmap = NULL;
            CFbsBitmap* markBitmapMask = NULL;
    	
            TRgb defaultColor;
            defaultColor = CEikonEnv::Static()->Color(EColorControlText);
    	
            AknsUtils::CreateColorIconL(AknsUtils::SkinInstance(),
            		KAknsIIDQgnIndiMarkedAdd,
            		KAknsIIDQsnIconColors,
            		EAknsCIQsnIconColorsCG13,
            		markBitmap,
            		markBitmapMask,
            		AknIconUtils::AvkonIconFileName(),
            		EMbmAvkonQgn_indi_marked_add,
            		EMbmAvkonQgn_indi_marked_add_mask,
            		defaultColor );
            
            CGulIcon* markIcon = CGulIcon::NewL(markBitmap,markBitmapMask);
            aIcons->AppendL(markIcon); 
            }
        }


// ----------------------------------------------------------------------------
// CContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CContainer::ComponentControl(TInt aIndex) const
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
// CContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CContainer::SelectedTestCases
// 
// Returns pointers to selected test cases.
// ----------------------------------------------------------------------------
//
void CContainer::SelectedTestCases
                            (RPointerArray<CTestInfo>& aSelectedTestCases)
    {
    TInt selectedItemIdx = iListBox->CurrentItemIndex();
    if ( ( selectedItemIdx >= 0 ) && ( iTestCasesInView.Count() > selectedItemIdx ) )
        {
        aSelectedTestCases.Append( iTestCasesInView[ selectedItemIdx ] );    
        }
    }

// ----------------------------------------------------------------------------
// CContainer::SelectedTestCaseInfo
// 
// Returns pointers to currently selected (in list box) test cases info.
// ----------------------------------------------------------------------------
//
CTestInfo* CContainer::SelectedTestCaseInfo()
    {
    TInt selectedItemIdx = iListBox->CurrentItemIndex();
    if ( ( selectedItemIdx >= 0 ) && ( iTestCasesInView.Count() > selectedItemIdx ) )
        {
        return iTestCasesInView[ selectedItemIdx ];
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CContainer::TestCaseInfo
// 
// Returns pointers to selected test cases info.
// ----------------------------------------------------------------------------
//
CTestInfo* CContainer::TestCaseInfo( TInt aIndex )
    {
    if ( ( aIndex >=0 ) && ( iTestCasesInView.Count() > aIndex ) )
        {
        return iTestCasesInView[ aIndex ];
        }
    return NULL;
    }
    
// ----------------------------------------------------------------------------
// CContainer::HandleMarkCommandL
// 
// Handles mark commands.
// ----------------------------------------------------------------------------
//
void CContainer::HandleMarkCommandL( TInt aCommand )
	{
	if (iListBox)
		{
		AknSelectionService::HandleMarkableListProcessCommandL( aCommand, iListBox );		
		}
	}

// ----------------------------------------------------------------------------
// CContainer::ProcessCommandL
// 
// Processes user commands.
// ----------------------------------------------------------------------------
//
void CContainer::ProcessCommandL( TInt  aCommand )
	{
	AknSelectionService::HandleMarkableListProcessCommandL( aCommand, iListBox );
    }
    
// ----------------------------------------------------------------------------
// CContainer::SelectionListProcessCommandL
// 
// Processes user commands.
// ----------------------------------------------------------------------------
//     
void CContainer::SelectionListProcessCommandL( TInt  aCommand )
	{
	AknSelectionService::HandleSelectionListProcessCommandL( aCommand, iListBox );
    }
    
// ----------------------------------------------------------------------------
// CContainer::FilterCasesByModuleL
// 
// Show only testcases which are defined is specified module.
// ----------------------------------------------------------------------------
//
void CContainer::FilterCasesByModuleL( TName aModuleName )
    {
    iTestCasesInView.Reset(); // Clear testCasesInView pointer array
   
    RRefArray<CTestInfo> allCases;
    TInt ret = iUIStore->TestCases( allCases );
    if( KErrNone != ret )
        {
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("FilterCasesByModuleL(): iUIStore->TestCases() fails"));
        allCases.Reset();
        allCases.Close();
        User::Leave( ret );
        }
    const TInt KTestCaseCount = allCases.Count();
    for( TInt i=0; i < KTestCaseCount; i++ )
        {
        if ( allCases[i].ModuleName() == aModuleName )
            {
            iTestCasesInView.Append( &( allCases[i] ) );
            }
        }
    allCases.Reset();
    allCases.Close();

    iListBox->Reset();
    iListBox->DrawNow();
    }

// ----------------------------------------------------------------------------
// CContainer::FilterCasesByTCFileNameL
// 
// Show only testcases which are defined is specified test case file.
// ----------------------------------------------------------------------------
//
void CContainer::FilterCasesByTCFileNameL( 
                                                TFileName aTestCaseFileName )
    {

    iTestCasesInView.Reset(); // Clear testCasesInView pointer array
  
    RRefArray<CTestInfo> allCases;
    TInt ret = iUIStore->TestCases( allCases );
    if( KErrNone != ret )
        {
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("FilterCasesByModuleL(): iUIStore->TestCases() fails"));
        allCases.Reset();
        allCases.Close();
        User::Leave( ret );
        }
    const TInt KTestCaseCount = allCases.Count();
    for( TInt i=0; i < KTestCaseCount; i++ )
        {
        if ( allCases[i].TestCaseFile() == aTestCaseFileName )
            {
            iTestCasesInView.Append( &( allCases[i] ) );
            }
        }

    allCases.Reset();
    allCases.Close();

    iListBox->Reset();
    iListBox->DrawNow();
    }

// ----------------------------------------------------------------------------
// CContainer::NoFilteringL
// 
// Remove possible filtering of test cases -> show all test cases.
// ----------------------------------------------------------------------------
//
void CContainer::NoFilteringL()
    {
    iTestCasesInView.Reset(); // Clear testCasesInView pointer array

    RRefArray<CTestInfo> allCases;
    TInt ret = iUIStore->TestCases( allCases );
    if( KErrNone != ret )
        {
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("FilterCasesByModuleL(): iUIStore->TestCases() fails"));
        allCases.Reset();
        allCases.Close();
        User::Leave( ret );
        }
    const TInt KTestCaseCount = allCases.Count();

    // Add all cases to iTestCasesInView pointer array
    for( TInt i=0; i < KTestCaseCount; i++ )
        {
        iTestCasesInView.Append( &( allCases[i] ) );
        }

    allCases.Reset();
    allCases.Close();

    iListBox->Reset();
    iListBox->DrawNow();
    }

// End of File
