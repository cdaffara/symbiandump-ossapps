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
* Description: This file contains CUIStoreHandler class declaration.
*
*/

#ifndef UISTOREHANDLER_H
#define UISTOREHANDLER_H

//  INCLUDES
//#include "UIStoreIf.h"  
//#include "UIStore.h"   
//#include "UIEngine.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>

#include "AppUIAppUi.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None
class CAppUIAppUi;

// DESCRIPTION
// CUIStoreHandler is a STIF Test Framework Series60 UI class.
// CUIStoreHandler's methods purpose is to offer handle to UI Engine.
// Because multible inheritance in not allowed in SOS we cannot 
// inheritance CUIStore class direct so we use this class.

class CUIStoreHandler 
            :public CUIStoreIf
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aAppUIAppUI Pointer to application ui.
        * @return Pointer to UIStoreHandler.
        */
        static CUIStoreHandler* NewL( CAppUIAppUi* aAppUIAppUI );

        /**
        * Destructor.
        */
        ~CUIStoreHandler();

    public:     // New functions

        /**
        * C++ default constructor.
        * @param aAppUIAppUI Pointer to application ui.
        */
        CUIStoreHandler( CAppUIAppUi* aAppUIAppUI );

        /**
        * Receives output update notify from started test case.
        * Checks if that test case is currently in output view then
        * sends notification to AppUI which handles notification onward.
        * @param aTestCase
        * @param aStatus
        */
        void Update( CStartedTestCase* aTestCase, TInt aStatus );

    public:     // Functions from base classes
	        virtual TInt PopupMsg( const TDesC&  aLine1, 
                               const TDesC&  aLine2, 
                               TInt  aTimeInSecs,
                               TKeyCode&  aKey,
                               TRequestStatus&  aStatus  );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     // Data

        /**
        * Returns the AppUI of the application
        * @return Pointer to the Application UI.
        */
        CAppUIAppUi* iAppUIAppUI;
        
        /**
        * Counter for executed test cases
        */
        TInt iExecutedTestCaseCount;

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // UISTOREHANDLER_H

// End of File
