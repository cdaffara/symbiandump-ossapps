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

#include "MemSpyViewKernelContainers.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>

#include <memspysession.h>
#include <memspy/api/memspyapiprocess.h>
#include <memspy/engine/memspyengineutils.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewKernel.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyViewKernelObjects.h"

// Literal constants



CMemSpyViewKernelContainers::CMemSpyViewKernelContainers( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    }


CMemSpyViewKernelContainers::~CMemSpyViewKernelContainers()
    {
    for (TInt i=0; i<iKernelObjects.Count(); i++)
        {
        delete iKernelObjects[i];
        }
    iKernelObjects.Close();
    
    if( iWaitDialog )    	
		delete iWaitDialog;    
    }


void CMemSpyViewKernelContainers::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
	iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), ETrue);
	iWaitDialog->PrepareLC( R_MEMSPY_WAIT_NOTE );	
	iWaitDialog->RunLD();
	
    _LIT( KTitle, "Kernel Objects" );
    SetTitleL( KTitle );
    //    
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    
    iWaitDialog->ProcessFinishedL();
    }


void CMemSpyViewKernelContainers::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewKernelContainers::ViewType() const
    {
    return EMemSpyViewTypeKernelContainers;
    }


CMemSpyViewBase* CMemSpyViewKernelContainers::PrepareParentViewL()
    {
    CMemSpyViewKernel* parent = new(ELeave) CMemSpyViewKernel( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewKernelContainers::PrepareChildViewL()
    {	
    CMemSpyViewBase* child = NULL;
    const TInt index = iListBox->CurrentItemIndex();    
    //child = new(ELeave) CMemSpyViewKernelObjects( iEngine, iObserver, iModel->At( index ).Type() );
    child = new(ELeave) CMemSpyViewKernelObjects( iMemSpySession, iObserver, iKernelObjects[index]->Type() );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;   
    }


void CMemSpyViewKernelContainers::SetListBoxModelL()
    {
    for (TInt i=0; i<iKernelObjects.Count(); i++)
        {
        delete iKernelObjects[i];
        }
	iMemSpySession.GetKernelObjects( iKernelObjects );
		
	
	CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( iKernelObjects.Count() ); //array for formated items
	
	for( TInt i=0 ; i<iKernelObjects.Count() ; i++ )
		{
		TInt count = iKernelObjects[i]->Count();
		TInt size = iKernelObjects[i]->Size();
		const TMemSpySizeText sizeText( MemSpyUiUtils::FormatSizeText( size, 0 ) );
								
		HBufC* tempName = HBufC::NewL( iKernelObjects[i]->Name().Length() + 32 );
		
		CleanupStack::PushL( tempName );
		TPtr tempNamePtr( tempName->Des() );
		tempNamePtr.Copy( iKernelObjects[i]->Name() );									
		
		_LIT(KNameFormat, "\t%S\t\t%d item");
		TPtr pName( tempName->Des() );
		//
		const TPtrC pType( MemSpyUiUtils::TypeAsString( iKernelObjects[i]->Type() ) );
		//
		pName.Format( KNameFormat, &pType, count );
		//
		if  ( count == 0 || count > 1 )
			{
			pName.Append( _L("s") );
			}

		pName.AppendFormat( _L(", %S"), &sizeText );
		
		model->AppendL( pName );
		
		CleanupStack::PopAndDestroy( tempName); //---
		}
	
	CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );	
	listbox->Model()->SetItemTextArray( model );
	listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    }


TBool CMemSpyViewKernelContainers::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdKernelContainersOutput:
        OnCmdOutputAllContainerContentsL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewKernelContainers::OnCmdOutputAllContainerContentsL()
    {
	iMemSpySession.OutputAllContainerContents();		
    }
