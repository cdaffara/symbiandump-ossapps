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

#ifndef MEMSPYVIEWTHREADINFOITEMCHUNK_H
#define MEMSPYVIEWTHREADINFOITEMCHUNK_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewThreadInfoItemGeneric.h"

class RMemSpySession;

class CMemSpyViewThreadInfoItemChunk : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemChunk( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId,  TThreadId aId, TMemSpyThreadInfoItemType aType );

private: // From CMemSpyViewBase
    CMemSpyViewBase* PrepareChildViewL();
    };


#endif
