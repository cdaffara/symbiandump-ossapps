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

#ifndef MEMSPYVIEWKERNELHEAP_H
#define MEMSPYVIEWKERNELHEAP_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"
#include <memspy/api/memspyapiheap.h>

// Classes referenced
class CMemSpyEngineGenericKernelObjectContainer;

class RMemSpySession;

class CMemSpyViewKernelHeap : public CMemSpyViewBase
    {
public:    
	CMemSpyViewKernelHeap( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );	
    ~CMemSpyViewKernelHeap();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_KERNEL_HEAP; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdKernelHeap; }

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    TBool HandleCommandL( TInt aCommand );    

private: // Command handlers
    void OnCmdDumpKernelHeapL();

private: // Internal methods
    //CDesCArrayFlat* FormatModel( RArray<CMemSpyApiHeap*> &aHeap );
    CDesCArrayFlat* FormatModelLC( CMemSpyApiHeap* aHeap );
    HBufC* FormatItemLC( const TDesC& aCaption, const TDesC& aValue );
    HBufC* FormatItemLC( const TDesC& aCaption, TInt aValue );
    HBufC* FormatItemLC( const TDesC& aCaption, TUint aValue );
    HBufC* FormatItemLC( const TDesC& aCaption, const TInt64& aValue );
    HBufC* FormatItemLC( const TDesC& aCaption, TAny* aValue );
    HBufC* FormatItemLC( const TDesC& aCaption, TUint* aValue );
    HBufC* FormatItemLC( const TDesC& aCaption, TUint8* aValue );
    HBufC* FormatPercentageItemLC( const TDesC& aCaption, TReal aOneHundredPercentValue, TReal aValue );

private: // Data members
    };


#endif
