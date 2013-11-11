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

#ifndef MEMSPYVIEWKERNELOBJECTS_H
#define MEMSPYVIEWKERNELOBJECTS_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>
#include <AknTabObserver.h> 

// User includes
#include "MemSpyViewBase.h"
#include <memspy/driver/memspydriverenumerationsshared.h>
#include <memspy/api/memspyapikernelobjectitem.h>

// Classes referenced
class CMemSpyEngineGenericKernelObjectContainer;
class CMemSpyEngineGenericKernelObjectList;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CAknTabGroup;

class RMemSpySession;

class CMemSpyViewKernelObjects : public CMemSpyViewBase, public MAknTabObserver
    {
public:
    //CMemSpyViewKernelObjects( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, TMemSpyDriverContainerType aObjectType );
	CMemSpyViewKernelObjects( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TMemSpyDriverContainerType aObjectType );
    ~CMemSpyViewKernelObjects();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    CEikListBox* ConstructListBoxL();
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_KERNEL_OBJECTS; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdKernelObjects; }
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    TBool HandleCommandL( TInt aCommand );

private: // From CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private: // From MAknTabObserver
    void TabChangedL( TInt aIndex );     
    
private: // Command handlers
    void OnCmdTerminateL();
    void OnCmdSwitchToL();
    void OnCmdEndL();
    void OnCmdPanicL();

private: // Internal methods
    void CreateTabsL();
    void DetailsL();
    void AppendFormatString( TPtr& aPtr, TRefByValue<const TDesC> aFmt, ... );

private: // Data members
    //CMemSpyEngineGenericKernelObjectContainer* iModel;
    CDesCArrayFlat* iItems;
    TMemSpyDriverContainerType iObjectType;
    CMemSpyEngineGenericKernelObjectList* iObjectList;
    CAknNavigationDecorator* iNavDecorator;
    CAknNavigationControlContainer* iNaviPane;
    CAknNavigationControlContainer* iNavContainer;
    CAknTabGroup* iTabs;
    TInt iCurrItemIndex;
    CDesCArrayFlat* iModel;
    RArray<CMemSpyApiKernelObjectItem*> iKernelObjectItems;
    
    CAknWaitDialog* iWaitDialog;
    };

#endif
