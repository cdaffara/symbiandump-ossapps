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


#ifndef PERFMON_GRAPHSVIEW_H
#define PERFMON_GRAPHSVIEW_H

// INCLUDES
#include <aknview.h>

#include "perfmon_std.h"



// CONSTANTS
// UID of view
const TUid KGraphsViewUID = {2};


// FORWARD DECLARATIONS
class CPerfMonGraphsContainer;
class CPerfMonModel;


// CLASS DECLARATION

/**
*  CPerfMonGraphsView view class.
* 
*/
class CPerfMonGraphsView : public CAknView
    {
    public: // Constructors and destructor
        void ConstructL();
        ~CPerfMonGraphsView();

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
        CPerfMonGraphsContainer*        iContainer;
        CPerfMonModel*                  iModel;
    };

#endif

// End of File
