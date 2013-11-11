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
* Description: This file contains CAppUIAppUi class declaration.
*
*/

#ifndef APPUIAPPUI_H
#define APPUIAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <e32std.h>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <StifLogger.h>
#include "Stifui.hrh"

//#include "UIStoreIf.h"             
//#include "UIStore.h"               
//#include "UIEngine.h"              
//#include "SettingServerClient.h"   

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <SettingServerClient.h>

#include "UIStoreHandler.h"


// FORWARD DECLARATIONS
class CAppUIContainer;
class CUIStoreHandler;

// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT( KDefaultPathAndIni, "C:\\TestFramework\\TestFramework.ini" );

// CLASS DECLARATION


/**
 * Class contains static methods for calling different
 * types of message dialog boxes.
 */
class TMessageBoxUtil 
	{
	public:
	
	    /**
	     * Display message that executed funtion is not implemented yet.
	     */      
		static void ShowNotImplementedYetL();

	    /**
	     * Display error note.
	     * @param aMessage Error message to display.
	     */      
		static void ShowErrorNoteL( const TDesC& aMessage );
	};


/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CAppUIAppUi : public CAknViewAppUi
    {
    public:  // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CAppUIAppUi();

    public: // New functions

        /**
        * Returns pointer to UIStoreHandler object, 
        * which handles test cases and test modules.
        * @return UIStoreHandler to CData object
        */
        CUIStore* UIStoreHandler();

        /**
        * Shows outputs of the test case.
        * @param aTestCase Pointer to started test case.
        */
        void ShowTestCaseOutput(CStartedTestCase* aTestCase);

        /**
        * Receives output update notification from CData.
        * @param aTestCase Pointer to started test case.
        * @param aStatus Status.
        */
        void OutputUpdateL( CStartedTestCase* aTestCase, TInt aStatus );
        
		/**
        * Sets pointer to selected test case for viewing output.
        * @param aStartedCase Pointer to started test case.
        */
        void SetStartedTestCase( CStartedTestCase* aStartedCase );   
         
        /**
        * Returns pointer to started test case for viewing output.
        * @return Poiner to started test case.
        */
		CStartedTestCase* GetStartedTestCase( ); 
		
		/**
		 * Saves position of focus of any specific view
		 * @param aViewId ID of view under which the position will be stored
		 * @param aPosition is a number to be stored
		 */
		void SaveFocusPosition(TAppUIViewNumber aViewId, TInt aPosition);
		
		/**
		 * Used to retrieve previous focus position in a specific view
		 * @param aViewId - ID of view that the position should be retrieved for
		 * @return the previous focus position
		 */
		TInt GetFocusPosition(TAppUIViewNumber aViewId);
		

    public: // Functions from base classes

    private:
        /**
        * Initializes menu pane.
        * @param aResourceId Menu pane resource ID.
        * @param aMenuPane Menu pane pointer.
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //Data
        CUIStoreHandler*                iUIStoreHandler;
        CStartedTestCase*               iStartedTestCase;
        TInt                            iPreviousView; // For Back function
        
        RArray<TInt>					iPreviousFocusPosition;	
        // used to store positions in different views
        
        TBool							iPreviousPositionListValid; 
        // used to mark validity of iPreviousFocusPosition array
		
    public: //Data
        
        /**
        * Pointer to logger.
        */
        CStifLogger*                        iLogger;
        
        /**
        * Mode of view started cases.
        */
        TInt /*enum TShowStartedCasesMode*/ iShowStartedCasesMode;
        
        /**
        * Index of started test set.
        */
		TInt								iStartedTestSet;
		
	    /**
        * Handle to Setting server.
        */	
		RSettingServer                      iSettingServer;
		
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
                
        // For iExecutedTestCaseCount moving
        friend class CStartCasesView;
        friend class CStatisticsView;
        friend class CTestCaseOutputView;
        friend class CTestSetInsertMenuView;
        friend class CTestSetMenuView;

    };

#endif

// End of File
