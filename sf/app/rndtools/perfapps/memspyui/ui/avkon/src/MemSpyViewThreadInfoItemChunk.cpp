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

#include "MemSpyViewThreadInfoItemChunk.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperchunk.h>

// User includes
#include "MemSpyContainerObserver.h"
#include "MemSpyViewChunkList.h"



CMemSpyViewThreadInfoItemChunk::CMemSpyViewThreadInfoItemChunk( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewThreadInfoItemGeneric( aSession, aObserver, aProcId, aId, EMemSpyThreadInfoItemTypeChunk )
    {
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemChunk::PrepareChildViewL()
    {	
    CMemSpyViewBase* child = NULL;
    /* TODO
    // Get the code segment list
    CMemSpyThreadInfoChunk* chunkInfoItem = static_cast< CMemSpyThreadInfoChunk* >( iInfoItem );
    CMemSpyEngineChunkList* list = &chunkInfoItem->List();
    
    // Get the current code segment that corresponds to our currently focused list item.
    const TInt index = iListBox->CurrentItemIndex();
    if  ( index >= 0 && index < list->Count() )
        {
        CMemSpyEngineChunkEntry& entry = list->At( index );
        child = new(ELeave) CMemSpyViewChunkDetails( iEngine, iObserver, *list, entry );
        
        // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
        // This object is about to die in any case.
        chunkInfoItem->NullifyList();

        CleanupStack::PushL( child );
        child->ConstructL( Rect(), *Parent() );
        CleanupStack::Pop( child );
        }
    */
    return child;    
    }






