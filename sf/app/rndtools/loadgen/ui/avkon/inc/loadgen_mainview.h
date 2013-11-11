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


#ifndef LOADGEN_VALUESVIEW_H
#define LOADGEN_VALUESVIEW_H

// INCLUDES
#include <aknview.h>

#include "loadgen_std.h"



// CONSTANTS
// UID of view
const TUid KMainViewUID = {1};


// FORWARD DECLARATIONS
class CLoadGenMainContainer;
class CLoadGenModel;


// CLASS DECLARATION

/**
*  CLoadGenMainView view class.
* 
*/
class CLoadGenMainView : public CAknView
    {
    public: // Constructors and destructor
        void ConstructL();
        ~CLoadGenMainView();

    public: // Functions from base classes
        TUid Id() const;
        void HandleCommandL(TInt aCommand);
        void HandleClientRectChange();

    private: // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    private: // From AknView
        void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
        void DoDeactivate();

    private: // Data
        CLoadGenMainContainer*        iContainer;
        CLoadGenModel*                  iModel;
    };

#endif

// End of File
