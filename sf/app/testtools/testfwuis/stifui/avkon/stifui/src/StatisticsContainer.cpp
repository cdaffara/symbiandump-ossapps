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
* Description: This file contains CStatisticsContainer class 
* definition.
*
*/

// INCLUDE FILES
#include <aknlists.h>  // ListBox 
#include <AknIconArray.h> // CAknIconArray
#include <aknnotewrappers.h>

#include <eikclbd.h> // CColumnListBoxData

#include <Stifui.rsg>

//#include "UIStoreIf.h"         
//#include "UIStore.h"            
//#include "UIEngine.h"           
//#include "UIEngineContainer.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>

#include "StatisticsContainer.h"
#include "Stifui.hrh"
#include "AppUIAppUi.h"

#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStatisticsContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CStatisticsContainer::ConstructL( const TRect& aRect, 
                                        MEikListBoxObserver* aListBoxObserver )
    {

    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(
                            _L("StatisticsContainer: ConstructL"));

    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();

    //iTestCasesInView.Reset();
        //((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StatisticsContainer: iTCInV Reset"));
    
	iListBox = CMenuListBox::NewL(EStatisticsViewId);
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver /*this*/ ); // jos peritty MEikListBoxObserver:sta
    iListBox->ConstructL(this, EAknListBoxMarkableList /*EAknListBoxSelectionList*/);

    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StatisticsContainer: ListBox constructed"));

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

    if ( iUIStore )
        {
        //RRefArray<CStartedTestCase> allStarted;
        RRefArray<CStartedTestCase> allStarted;
        TInt ret = iUIStore->StartedTestCases( allStarted );
        if( KErrNone != ret )
            {
            allStarted.Reset();
            allStarted.Close();
            User::Leave( ret );
            }
        TInt count = allStarted.Count();
        // TInt notstarted( 0 );
        TInt ongoing( 0 );
        TInt passed( 0 );
        TInt failed( 0 );
        TInt other( 0 );
        //TInt aborted( 0 );
        // TInt paused( 0 );
        //TInt crashed( 0 );
        //TInt executed( 0 );
        // TInt state( 0 );

        for( TInt a = 0; a < count; a++ )
            {
            switch ( allStarted[a].Status() )
                {
                case CUIStoreIf::EStatusRunning:
                    {
                    ongoing++;
                    break;
                    }
                case CUIStoreIf::EStatusExecuted | CUIStoreIf::EStatusPassed:
                    {
                    passed++;
                    break;
                    }
                case CUIStoreIf::EStatusExecuted |CUIStoreIf::EStatusFailed:
                    {
                    failed++;
                    break;
                    }
                //case CUIStoreIf::EStatusAborted:
                //    {
                //    aborted++;
                //    break;
                //    }
                /*case ECrashed:
                    {
                    crashed++;
                    break;
                    }
                */
                /*case EExecuted:
                    {
                    executed++;
                    break;
                    }
                */
                default:
                    {
                    other++;
                    break;
                    }
                }
            }

        allStarted.Reset();
        allStarted.Close();

        iTestCaseArray = new (ELeave) CDesC16ArrayFlat(6);
        iTestCaseArray->Reset();

        HBufC* tmpHBuf = HBufC::NewL( 40 );
        TPtr buffer( tmpHBuf->Des() );

        buffer = ( _L("\t") );
        buffer.AppendNum( ongoing );
        buffer.Append( _L(" Running") );
        iTestCaseArray->AppendL( buffer );

        buffer = ( _L("\t") );
        buffer.AppendNum( passed );
        buffer.Append( _L(" Passed") );
        iTestCaseArray->AppendL( buffer );

        buffer = ( _L("\t") );
        buffer.AppendNum( failed );
        buffer.Append( _L(" Failed") );
        iTestCaseArray->AppendL( buffer );

        buffer = ( _L("\t") );
        buffer.AppendNum( other );
        buffer.Append( _L(" Crashed/Aborted") );
        iTestCaseArray->AppendL( buffer );

        delete tmpHBuf;

        iListBox->Model()->SetItemTextArray( iTestCaseArray );
             ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StatisticsContainer: ListBox model set"));
        }
    else
        {
        //User::Leave( syy?? )
        }

    // Creates graphic.
    //SetGraphicIconL( iListBox );
    //   ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StatisticsContainer: icons created"));

    iListBox->ActivateL();
    ((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();

    }

// ----------------------------------------------------------------------------
// CStatisticsContainer::~CStatisticsContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CStatisticsContainer::~CStatisticsContainer()
    {
    //iTestCasesInView.Close(); // does not delete objects whose pointers are contained in the array
    //delete iListBox;
    }

// ----------------------------------------------------------------------------
// CStatisticsContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CStatisticsContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CStatisticsContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CStatisticsContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CStatisticsContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//

CCoeControl* CStatisticsContainer::ComponentControl(TInt aIndex) const
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
// CStatisticsContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CStatisticsContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CStatisticsContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CStatisticsContainer::OfferKeyEventL(
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
// CStatisticsContainer::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CStatisticsContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
