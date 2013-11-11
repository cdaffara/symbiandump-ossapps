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


#ifndef PERFMON_APPUI_H
#define PERFMON_APPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include "perfmon_std.h"

// FORWARD DECLARATIONS
class CPerfMonModel;
class CAknNavigationControlContainer;
class CAknTabGroup;
class CAknNavigationDecorator;


// CLASS DECLARATIONS

class CPerfMonAppUi : public CAknViewAppUi
    {
    public: // // Constructors and destructor

        void ConstructL();

        ~CPerfMonAppUi();
        
    public: // New functions

    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
        void HandleForegroundEventL(TBool aForeground); 

        void HandleCommandL(TInt aCommand);

        void HandleResourceChangeL(TInt aType);

        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //Data
        CPerfMonModel*                  iModel;
        CAknNavigationControlContainer* iNaviPane;
        CAknTabGroup*                   iTabGroup;
        CAknNavigationDecorator*        iDecoratedTabGroup;
    };

#endif

// End of File
