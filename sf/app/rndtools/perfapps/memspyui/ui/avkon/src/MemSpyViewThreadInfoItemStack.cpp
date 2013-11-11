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

#include "MemSpyViewThreadInfoItemStack.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperstack.h>
#include <memspysession.h>

// User includes
#include "MemSpyContainerObserver.h"
#include "MemSpyViewThreads.h"
#include "MemSpyViewThreadInfoItemHeap.h"
#include "MemSpyAppUi.h"



CMemSpyViewThreadInfoItemStack::CMemSpyViewThreadInfoItemStack( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewThreadInfoItemGeneric( aSession, aObserver, aProcId, aId, EMemSpyThreadInfoItemTypeStack )
    {
    }


TBool CMemSpyViewThreadInfoItemStack::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
#ifndef __WINS__
	case EMemSpyCmdStackDataUser:
        OnCmdStackDataUserL();
		break;
	case EMemSpyCmdStackDataKernel:
        OnCmdStackDataKernelL();
		break;
#endif
	case EMemSpyCmdStackInfoThread:
        OnCmdStackInfoL();
		break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewThreadInfoItemStack::OnCmdStackInfoL()
    {
	iMemSpySession.OutputStackInfoL( ThreadId() );   
    }


void CMemSpyViewThreadInfoItemStack::OnCmdStackDataUserL()
    {
    iMemSpySession.OutputStackDataL( ThreadId(), EMemSpyDriverDomainUser );
    }


void CMemSpyViewThreadInfoItemStack::OnCmdStackDataKernelL()
    {
	iMemSpySession.OutputStackDataL( ThreadId(), EMemSpyDriverDomainKernel );
    }    


void CMemSpyViewThreadInfoItemStack::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == R_MEMSPY_MENUPANE )
        {
        //TODO aMenuPane->SetItemDimmed( EMemSpyCmdStack, Thread().IsDead() );
        }
    }
