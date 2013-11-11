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

#ifndef MEMSPYVIEWKERNELCONTAINERS_H
#define MEMSPYVIEWKERNELCONTAINERS_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>
#include <BADESCA.H>
#include <aknwaitdialog.h>

// User includes
#include "MemSpyViewBase.h"

#include <memspy/driver/memspydriverenumerationsshared.h>
#include <memspy/api/memspyapikernelobject.h>

// Classes referenced
class CMemSpyEngineGenericKernelObjectContainer;

class RMemSpySession;

class CMemSpyViewKernelContainers : public CMemSpyViewBase
    {
public:
    CMemSpyViewKernelContainers( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewKernelContainers();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();    
    TPtrC TypeAsString( TMemSpyDriverContainerType aType );
    
    
public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_KERNEL_CONTAINERS; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdKernelContainers; }

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    TBool HandleCommandL( TInt aCommand );

private: // Command handlers
    void OnCmdOutputAllContainerContentsL();

private: // Internal methods

private: // Data members
    //CMemSpyEngineGenericKernelObjectContainer* iModel;
    RArray<CMemSpyApiKernelObject*> iKernelObjects; //array of raw items data
    
    CAknWaitDialog* iWaitDialog;
    };


#endif
