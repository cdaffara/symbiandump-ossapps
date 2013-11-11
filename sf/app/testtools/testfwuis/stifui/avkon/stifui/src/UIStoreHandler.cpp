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
* Description: This module contains implementation of 
* CUIStoreHandler class member functions.
*
*/

// INCLUDE FILES
#include "UIStoreHandler.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None
class CUIStore;

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================
// ----------------------------------------------------------------------------
// CUIStoreHandler::CUIStoreHandler
// 
// Default constructor.
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CUIStoreHandler::CUIStoreHandler( CAppUIAppUi* aAppUIAppUI ) :
    iAppUIAppUI( aAppUIAppUI ),
    iExecutedTestCaseCount( 0 )
    {

    }

// ----------------------------------------------------------------------------
// CUIStoreHandler::ConstructL
// 
// Symbian OS second phase constructor.
// Symbian OS default constructor can leave.
// ----------------------------------------------------------------------------
//
void CUIStoreHandler::ConstructL()
    {
    CUIStoreIf::ConstructL();

    }

// ----------------------------------------------------------------------------
// CUIStoreHandler::NewL
// 
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUIStoreHandler* CUIStoreHandler::NewL( CAppUIAppUi* aAppUIAppUI )
    {
    // Create CUIStoreHandler object uistorehandler
    CUIStoreHandler* uistorehandler = new (ELeave) CUIStoreHandler( aAppUIAppUI );

    CleanupStack::PushL( uistorehandler );
    uistorehandler->ConstructL();
    CleanupStack::Pop( uistorehandler );

    return uistorehandler;

    }

// ----------------------------------------------------------------------------
// CUIStoreHandler::~CUIStoreHandler
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CUIStoreHandler::~CUIStoreHandler()
    {

    }

// ----------------------------------------------------------------------------
// CUIStoreHandler::Update
// 
// Receives output update notify from started test case.
// Checks if that test case is currently in output view then
// sends notification to AppUI which handles notification onward.
// ----------------------------------------------------------------------------
//
void CUIStoreHandler::Update( CStartedTestCase* aTestCase, TInt aStatus )
    {
	_LIT( KErrorMsg, "Error during output update" );
    TRAPD(err,iAppUIAppUI->OutputUpdateL( aTestCase, aStatus ));
	if ( err != KErrNone )
		{
		RDebug::Print( KErrorMsg );
		}

        //iAppUi->OutputUpdate();
    //if( aStatus == EPrintUpdate )
    //    {
        //iAppUIAppUI->OutputUpdate( aTestCase );
    //    }
    //else
    //    {
        //iAppUIAppUI->TestCaseStateChanged();
    //    }

    }

TInt CUIStoreHandler::PopupMsg( const TDesC& /*aLine1*/, 
                             const TDesC& /*aLine2*/, 
                             TInt /*aTimeInSecs*/,
                             TKeyCode& /*aKey*/,
                             TRequestStatus& aStatus )
    {
    
    TRequestStatus *status = &aStatus;
    User::RequestComplete(status, 0);

    return 0;
    
    }
// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
