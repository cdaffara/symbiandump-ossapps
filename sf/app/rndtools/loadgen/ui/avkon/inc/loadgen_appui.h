/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOADGEN_APPUI_H
#define LOADGEN_APPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include "loadgen_std.h"

// FORWARD DECLARATIONS
class CLoadGenModel;
class CAknNavigationControlContainer;
class CAknTabGroup;
class CAknNavigationDecorator;


// CLASS DECLARATIONS

class CLoadGenAppUi : public CAknViewAppUi
    {
public: // constructors and destructor
    void ConstructL();
    ~CLoadGenAppUi();
    
private: // From MEikMenuObserver
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

private:
    void HandleCommandL(TInt aCommand);

    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

private: //Data
    CLoadGenModel*                  iModel;
    CAknNavigationControlContainer* iNaviPane;
    CAknTabGroup*                   iTabGroup;
    CAknNavigationDecorator*        iDecoratedTabGroup;
    };

#endif

// End of File
