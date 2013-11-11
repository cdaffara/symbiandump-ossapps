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
* Description:
*
*/

#include "MemSpyViewBase.h"

// System includes
#include <eikrted.h>  // for example label control
#include <txtrich.h>
#include <akntitle.h>
#include <akncontext.h>
#include <aknnavi.h>
#include <eiklbx.h>
#include <eiklbm.h>
#include <avkon.hrh>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>

// User includes
#include "MemSpyAppUi.h"
#include "MemSpyDocument.h"
#include "MemSpyContainer.h"
#include "MemSpyContainerObserver.h"



/*
CMemSpyViewBase::CMemSpyViewBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   iEngine( aEngine ), iObserver( aObserver )
    {
    }
*/
CMemSpyViewBase::CMemSpyViewBase( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   iMemSpySession( aSession ), iObserver( aObserver )
    {
    }


CMemSpyViewBase::~CMemSpyViewBase()
    {
    delete iListBox;
    }
 

void CMemSpyViewBase::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* /*aSelectionRune*/ )
    {
    iSettings = &static_cast< CMemSpyAppUi* >( iEikonEnv->EikAppUi() )->MemSpyDocument().Settings();
    //
    User::LeaveIfError( SetParent( &aContainer ) );
    SetContainerWindowL( aContainer );
    
    SetComponentsToInheritVisibility( ETrue );

    iListBox = ConstructListBoxL();
    if  ( iListBox )
        {
        iListBox->SetFocus( ETrue );
        }

    SetRect( aRect );
    ActivateL();
    }


void CMemSpyViewBase::SetTitleL( const TDesC& aText )
    {
    CEikStatusPane* statusPane = static_cast<CAknAppUi*> ( iEikonEnv->EikAppUi() )->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*> ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( aText );
    }


TPtrC CMemSpyViewBase::TitleL() const
    {
    TPtrC ret( KNullDesC );
    //
    CEikStatusPane* statusPane = static_cast<CAknAppUi*> ( iEikonEnv->EikAppUi() )->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*> ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    if  ( title->Text() )
        {
        ret.Set( *title->Text() );
        }
    //
    return ret;
    }


CMemSpyViewBase* CMemSpyViewBase::PrepareParentViewL()
    {
    return NULL;
    }


CMemSpyViewBase* CMemSpyViewBase::PrepareChildViewL()
    {
    return NULL;
    }
    
    
void CMemSpyViewBase::RefreshL()
    {
    if  ( iListBox )
        {
        iListBox->UpdateScrollBarsL();
        }
    
    DrawDeferred();
    }


TBool CMemSpyViewBase::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch( aCommand )
        {
	case EMemSpyCmdViewOutputToSink:
        OnCmdViewOutputToSinkL();
        break;
    default:
        handled = EFalse;
        break;
        }
    //
    return handled;
    }


void CMemSpyViewBase::DynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    }


TUint CMemSpyViewBase::MenuCascadeResourceId() const
    {
    return 0;
    }


TInt CMemSpyViewBase::MenuCascadeCommandId() const
    {
    return 0;
    }


void CMemSpyViewBase::OnCmdViewOutputToSinkL()
    {
	//cigasto so solve - outputs content of the listbox into the sink
	//to discuss format with Orbit
	/*
    if  ( iListBox )
        {
        // Prepare sink
        CMemSpyEngineOutputSink& sink = iEngine.Sink();
        HBufC* name = MemSpyEngineUtils::CleanupTextLC( TitleL() );
        sink.OutputSectionHeadingL( *name, TChar('-') );
        CleanupStack::PopAndDestroy( name );
        sink.OutputPrefixSetLC( _L("  ") ); // Slight insertion
        
        // Get text from underlying listbox model...
        MTextListBoxModel* model = static_cast< MTextListBoxModel* >( iListBox->Model() );
        const TInt count = model->NumberOfItems();

        // First pass to get max lengths
        TInt maxLengthCaption = 0;
        TInt maxLengthValue = 0;

        for( TInt j=0; j<count; j++ )
            {
            const TPtrC pItem( model->ItemText( j ) );
            const TInt length = pItem.Length();
    
            // Check if its split or not?
            const TInt splitPos = pItem.FindF( _L("\t\t") );
            if  ( splitPos > 0 )
                {
                maxLengthCaption = Max( maxLengthCaption, splitPos );
                maxLengthValue = Max( maxLengthValue, length - splitPos + 1 );
                }
            else
                {
                maxLengthCaption = Max( maxLengthCaption, length );
                }
            }

        // Second pass - real this time - to print the values
        HBufC* line = HBufC::NewLC( ( maxLengthCaption + maxLengthValue ) + 20 );
        TPtr pLine( line->Des() );
        //
        for( TInt i=0; i<count; i++ )
            {
            const TPtrC pItem( model->ItemText( i ) );
            const TInt length = pItem.Length();
            //
            TPtrC pCaption( KNullDesC );
            TPtrC pValue( KNullDesC );
            //
            const TInt splitPos = pItem.FindF( _L("\t\t") );
            if  ( splitPos > 0 )
                {
                pCaption.Set( pItem.Left( splitPos ) );
                pValue.Set( pItem.Mid( splitPos + 1 ) );
                }
            else
                {
                pCaption.Set( pItem );
                }

            // Remove initial tabs in caption
            HBufC* caption = MemSpyEngineUtils::CleanupTextLC( pCaption );
        
            // Create value item & replace any further tabs
            HBufC* value = MemSpyEngineUtils::CleanupTextLC( pValue );

            // Now format the final line, with padding.
            pLine.Justify( *caption, maxLengthCaption + 3, ELeft, TChar(' ') );
            pLine.Append( *value );
            CleanupStack::PopAndDestroy( 2, caption );

            // Sink output
            sink.OutputLineL( pLine );
            }

        // Remove prefix & tidy up
        CleanupStack::PopAndDestroy( line );
        sink.OutputBlankLineL();
        CleanupStack::PopAndDestroy(); // clear prefix
		}  
	*/     
    }


CEikListBox* CMemSpyViewBase::ConstructListBoxL()
    {
    delete iListBox;
    iListBox = NULL;
    CAknSettingStyleListBox* listbox = new(ELeave) CAknSettingStyleListBox();
    iListBox = listbox;
    //
    listbox->ConstructL( this, EAknListBoxSelectionList | EAknListBoxLoopScrolling );
    listbox->SetContainerWindowL( *this );
    listbox->CreateScrollBarFrameL( ETrue );
    SetListBoxModelL();
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listbox->SetListBoxObserver( this );
    listbox->SetObserver( this );
    listbox->SetComponentsToInheritVisibility( ETrue );
    //
    return listbox;
    }


void CMemSpyViewBase::HandleListBoxItemActionedL( TInt /*aIndex*/ )
    {
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewBase::HandleListBoxItemSelectedL( TInt /*aIndex*/ )
    {
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }


void CMemSpyViewBase::ReportEventL( MMemSpyViewObserver::TViewEventType aEvent, TAny* aContext )
    {
    iObserver.HandleMemSpyViewEventL( aEvent, ViewType(), *this, aContext );
    }


void CMemSpyViewBase::SetListBoxCurrentItemIndexL( TInt aIndex )
    {
    if  ( iListBox )
        {
        iListBox->SetCurrentItemIndex( aIndex );
        HandleListBoxItemSelectedL( aIndex );
        }
    }


CMemSpyContainer& CMemSpyViewBase::Container()
    {
    CMemSpyAppUi* appUi = static_cast< CMemSpyAppUi* >( iEikonEnv->EikAppUi() );
    return appUi->Container();
    }


CMemSpySettings& CMemSpyViewBase::Settings()
    {
    return *iSettings;
    }


const CMemSpySettings& CMemSpyViewBase::Settings() const
    {
    return *iSettings;
    }

    
void CMemSpyViewBase::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    //
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }


void CMemSpyViewBase::SizeChanged()
    {
    if  ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }


void CMemSpyViewBase::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if  ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    }


TInt CMemSpyViewBase::CountComponentControls() const
    {
    TInt count = 0;
    if  ( iListBox )
        {
        ++count;
        }
    //
    return count;
    }


CCoeControl* CMemSpyViewBase::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
    case 0:
        return iListBox;
    default:
        return NULL;
        }
    }


TKeyResponse CMemSpyViewBase::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
	{
    TKeyResponse resp = EKeyWasNotConsumed;
    if  ( iListBox )
        {
        resp = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
    //
    if ( resp == EKeyWasNotConsumed && aType == EEventKeyDown && aKeyEvent.iCode == EKeyBackspace )
        {
        // When backspace is pushed, go to the parent view
        CMemSpyAppUi* appUi = static_cast< CMemSpyAppUi* >( iEikonEnv->EikAppUi() );
        appUi->Container().NavigateToParentViewL();
        resp = EKeyWasConsumed;
        }
    //
	return resp;
	}


void CMemSpyViewBase::HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType )
    {
    if  ( aControl == iListBox )
        {
        if  ( aEventType == MCoeControlObserver::EEventStateChanged )
            {
            const TInt index = iListBox->CurrentItemIndex();
            HandleListBoxItemSelectedL( index );
            }
        }
    }


void CMemSpyViewBase::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    const TInt index = iListBox->CurrentItemIndex();
    //
    switch (aEventType)
        {
    case EEventItemActioned:
    case EEventEnterKeyPressed:
    case EEventItemDoubleClicked:
        HandleListBoxItemActionedL( index );
        break;

    default:
        break;
        }
    }




