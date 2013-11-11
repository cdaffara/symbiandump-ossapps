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



#ifndef PROFILER_GUI_VALUESVIEW_H
#define PROFILER_GUI_VALUESVIEW_H

// INCLUDES
#include <aknview.h>

#include "profiler_gui_std.h"
#include "profiler_gui_model.h"


// CONSTANTS
// UID of view
const TUid KMainViewUID = {1};

// FORWARD DECLARATIONS
class CProfilerGuiMainContainer;
class CProfilerGuiModel;
class CProfilerEngineStatusChecker;
class CAknNavigationDecorator;


/**
*  CProfilerGuiMainView view class.
* 
*/
class CProfilerGuiMainView : public CAknView
    {
    public: // Constructors and destructor
        void ConstructL();
        ~CProfilerGuiMainView();

    public: // Functions from base classes
        TUid Id() const;
        void HandleCommandL(TInt aCommand);
        void HandleClientRectChange();
        
        /**
         * Method for updating the Profiler engine status pane
         * Called by Model class
         * 
         * @param aStatus new status of profiler engine  
         */
        void UpdateStatusPaneL( TInt aStatus ); 
        
    private: // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
    private: // From AknView
        void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
        void DoDeactivate();
        void HandleStatusPaneSizeChange();
        void SetupStatusPaneL();
        void CleanupStatusPaneL();
		HBufC* GetLabelTextLC(TInt aStatus);
    private: // Data
    	CAknNavigationDecorator*			iNaviDecorator;
        CProfilerGuiMainContainer*        	iContainer;
        CProfilerGuiModel*                  iModel;

    };

#endif

// End of File
