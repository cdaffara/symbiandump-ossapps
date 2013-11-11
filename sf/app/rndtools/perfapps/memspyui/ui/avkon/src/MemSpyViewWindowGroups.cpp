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

#include "MemSpyViewWindowGroups.h"

// System includes
#include <eikclbd.h>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthread.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"

// Literal constants
const TInt KMaxInfoLength = 128;


CMemSpyViewWindowGroups::CMemSpyViewWindowGroups( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


CMemSpyViewWindowGroups::~CMemSpyViewWindowGroups()
    {
    delete iWindowGroupList;
    }


void CMemSpyViewWindowGroups::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Window Groups" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


CEikListBox* CMemSpyViewWindowGroups::ConstructListBoxL()
    {
    delete iListBox;
    iListBox = NULL;
    CAknSingleNumberStyleListBox* listbox = new (ELeave) CAknSingleNumberStyleListBox();
    iListBox = listbox;
    listbox->ConstructL( this, EAknListBoxSelectionList | EAknListBoxLoopScrolling );
    listbox->SetContainerWindowL( *this );
    listbox->CreateScrollBarFrameL( ETrue );
    SetListBoxModelL();
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listbox->SetListBoxObserver( this );
    listbox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    listbox->SetObserver( this );
    return listbox;
    }

void CMemSpyViewWindowGroups::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewWindowGroups::ViewType() const
    {
    return EMemSpyViewTypeWindowGroups;
    }


CMemSpyViewBase* CMemSpyViewWindowGroups::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewWindowGroups::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    if ( iListBox && 
         iListBox->Model()->NumberOfItems() && 
         iListBox->CurrentItemIndex() > KErrNotFound )
        {
        DetailsL();
        }
    return child;
    }


void CMemSpyViewWindowGroups::SetListBoxModelL()
    {
    // Take ownership of new model
    MMemSpyEngineHelperWindowServer& windowServerManager = iEngine.HelperWindowServer();
    MMemSpyEngineWindowGroupList* windowGroupList = windowServerManager.WindowGroupListL();
    delete iWindowGroupList;
    iWindowGroupList = windowGroupList;
    
    // Set up list box
    CAknSingleNumberStyleListBox* listbox = static_cast< CAknSingleNumberStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( iWindowGroupList );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


TBool CMemSpyViewWindowGroups::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
        case EMemSpyCmdWindowGroupTerminate:
            {
            TRAPD( err, OnCmdEndL( aCommand ) );
            CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
            if ( err )
                {
                note->ExecuteLD( _L("Cannot terminate task") );
                }
            else
                {
                note->ExecuteLD( _L("Task terminated") );
                }
            break;
            }
        case EMemSpyCmdWindowGroupSwitchTo:
            {
            TRAPD( err, OnCmdSwitchToL() );
            if ( err )
                {
                CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
                note->ExecuteLD( _L("Cannot bring to foreground") );
                }
            break;
            }
        case EMemSpyCmdWindowGroupEnd:
            {
            TRAPD( err, OnCmdEndL( aCommand ) );
            CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
            if ( err )
                {
                note->ExecuteLD( _L("Cannot end task") );
                }
            else
                {
                note->ExecuteLD( _L("Task exited") );
                }
            break;
            }
        case EMemSpyCmdWindowGroupPanic:
            {
            TRAPD( err, OnCmdEndL( aCommand ) );
            CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
            if ( err )
                {
                note->ExecuteLD( _L("Cannot panic task") );
                }
            else
                {
                note->ExecuteLD( _L("Task panic'ed") );
                }
            break;
            }
        default:
            {
            handled = CMemSpyViewBase::HandleCommandL( aCommand );
            break;        
            }
        }
    //
    return handled;
    }


void CMemSpyViewWindowGroups::OnCmdSwitchToL()
    {
    MMemSpyEngineHelperWindowServer& windowServerManager = iEngine.HelperWindowServer();
    TInt id = iWindowGroupList->At( iListBox->CurrentItemIndex() ).iId;
    windowServerManager.SwitchToL( id );
    }


void CMemSpyViewWindowGroups::OnCmdEndL( TInt aCommand )
    {
	/*
    TBool doTerminate = ETrue;
    
    CMemSpyEngineObjectContainer& container = iEngine.Container();
    TThreadId id = iWindowGroupList->At( iListBox->CurrentItemIndex() ).iThreadId;
    
    // Try to find the thread in question...
    CMemSpyProcess* process = NULL;
    CMemSpyThread* thread = NULL; 
    User::LeaveIfError( container.ProcessAndThreadByThreadId( id, process, thread ) );

    if ( thread )
        {
        //
        if  ( thread->IsSystemPermanent() || thread->IsSystemCritical() )
            {
            CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
            doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
            }
        //
        if  ( doTerminate )
            {
            switch ( aCommand )
                {
                case EMemSpyCmdWindowGroupTerminate:
                    {
                    thread->TerminateL();
                    break;
                    }
                case EMemSpyCmdWindowGroupEnd:
                    {
                    thread->KillL();
                    break;
                    }
                case EMemSpyCmdWindowGroupPanic:
                    {
                    thread->PanicL();
                    break;
                    }
                default:
                    {
                    // Programming error
                    __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
                    }                
                }
            }                
        }
    RefreshL();
    */
    }


void CMemSpyViewWindowGroups::DetailsL()
    {
    HBufC* messageBuf = HBufC::NewLC( 4096 );
    TPtr messagePtr = messageBuf->Des();

    // Fetch data from helper
    TMemSpyEngineWindowGroupDetails selectedObject;
    MMemSpyEngineHelperWindowServer& windowServerManager = iEngine.HelperWindowServer();
    TInt id = iWindowGroupList->At( iListBox->CurrentItemIndex() ).iId;
    windowServerManager.GetWindowGroupDetailsL( id, selectedObject );
    
    // Append info to string buffer
    AppendFormatString( messagePtr, _L("WG ID: %d\n"), selectedObject.iId );
    AppendFormatString( messagePtr, _L("Client ThreadId: %Lu\n"), selectedObject.iThreadId.Id() );
    TFullName name;
    name.Copy( selectedObject.iFullName );
    AppendFormatString( messagePtr, _L("Thr: %S\n"), &name );
    name.Zero();
    AppendFormatString( messagePtr, _L("Priority: %d\n"), selectedObject.iPriority );
    AppendFormatString( messagePtr, _L("Handle: 0x%08X\n"), selectedObject.iWindowGroupHandle );
    name.Copy( selectedObject.iName );
    AppendFormatString( messagePtr, _L("Name: %S\n"), &name );
    name.Zero();
    AppendFormatString( messagePtr, _L("UID: 0x%08X\n"), selectedObject.iUID );
    AppendFormatString( messagePtr, _L("IsBusy: %d\n"), selectedObject.iIsBusy );
    AppendFormatString( messagePtr, _L("IsSystem: %d\n"), selectedObject.iIsSystem );
    AppendFormatString( messagePtr, _L("IsHidden: %d\n"), selectedObject.iIsHidden );
    name.Copy( selectedObject.iCaption );
    AppendFormatString( messagePtr, _L("Caption: %S\n"), &name );
    name.Zero();
    name.Copy( selectedObject.iDocName );
    AppendFormatString( messagePtr, _L("Docname: %S"), &name );
    name.Zero();

    // Display the buffer on a dialog
    CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog( CAknQueryDialog::ENoTone );
    dialog->PrepareLC( R_MEMSPY_KERNEL_OBJECT_DETAILS_DIALOG );
    TFileName headerText;
    headerText.Copy( selectedObject.iFullName );
    dialog->SetHeaderTextL( headerText );
    dialog->SetMessageTextL( messagePtr );
    dialog->RunLD();
   
    CleanupStack::PopAndDestroy( messageBuf );
    }


void CMemSpyViewWindowGroups::AppendFormatString( TPtr& aPtr, TRefByValue<const TDesC> aFmt, ... )
    {
    TBuf<KMaxInfoLength> infoString;
    VA_LIST list;
    VA_START ( list, aFmt );
    infoString.FormatList( aFmt, list );
    aPtr.Append( infoString );
    }


