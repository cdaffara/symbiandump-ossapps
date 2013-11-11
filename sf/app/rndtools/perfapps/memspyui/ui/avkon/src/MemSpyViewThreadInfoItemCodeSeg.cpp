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

#include "MemSpyViewThreadInfoItemCodeSeg.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>

// User includes
#include "MemSpyContainerObserver.h"
#include "MemSpyViewCodeSegList.h"



CMemSpyViewThreadInfoItemCodeSeg::CMemSpyViewThreadInfoItemCodeSeg( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewThreadInfoItemGeneric( aSession, aObserver, aProcId, aId, EMemSpyThreadInfoItemTypeCodeSeg )
    {
    }


TBool CMemSpyViewThreadInfoItemCodeSeg::HandleCommandL( TInt aCommand )
    {
    TBool handled = CMemSpyViewBase::HandleCommandL( aCommand );
    return handled;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemCodeSeg::PrepareChildViewL()
    {	
    CMemSpyViewBase* child = NULL;
    /* TODO
    // Get the code segment list
    CMemSpyThreadInfoCodeSeg* codeSegInfoItem = static_cast< CMemSpyThreadInfoCodeSeg* >( iInfoItem );
    CMemSpyEngineCodeSegList* list = &codeSegInfoItem->List();
    
    // Get the current code segment that corresponds to our currently focused list item.
    const TInt index = iListBox->CurrentItemIndex();
    if  ( index >= 0 && index < list->Count() )
        {
        CMemSpyEngineCodeSegEntry& entry = list->At( index );
        child = new(ELeave) CMemSpyViewCodeSegDetails( iEngine, iObserver, *list, entry );
        
        // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
        // This object is about to die in any case.
        codeSegInfoItem->NullifyList();

        CleanupStack::PushL( child );
        child->ConstructL( Rect(), *Parent() );
        CleanupStack::Pop( child );
        }
    */
    return child;    
    }


