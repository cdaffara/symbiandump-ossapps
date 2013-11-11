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
* Description: This module contains the 
* implementation of CUITestServerStarterAppView class member 
* functions.
*
*/

// INCLUDE FILES
#include <coemain.h>
#include <aknViewAppUi.h> 
#include "UITestServerStarterAppView.h"
#include "UITestServerStarterApplication.h"
#include "UITestServerStarterAppContainer.h"

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


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: None
     
     Return Values: Pointer to new CUITestServerStarterAppView object.

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppView* CUITestServerStarterAppView::NewL()
	{
	CUITestServerStarterAppView* self = CUITestServerStarterAppView::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: NewLC

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: None
     
     Return Values: Pointer to new CUITestServerStarterAppView object.

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppView* CUITestServerStarterAppView::NewLC()
	{
	CUITestServerStarterAppView* self = new ( ELeave ) CUITestServerStarterAppView;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: ConstructL

     Description: ConstructL is second phase of two-phased constructor.

     ConstructL is second phase of two-phased constructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppView::ConstructL()
	{
	BaseConstructL( 0 );
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: CUITestServerStarterAppView

     Description: C++ constructor.

     C++ constructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppView::CUITestServerStarterAppView()
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: CUITestServerStarterAppView

     Description: C++ destructor.

     C++ destructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppView::~CUITestServerStarterAppView()
	{
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: Id

     Description: Returns view Uid.

     Returns view Uid.

     Parameters: None
     
     Return Values: View Uid

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TUid CUITestServerStarterAppView::Id() const
	{
	return KMainViewId;	
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: HandleCommandL

     Description: Handle Commands.

     Handle Commands.

     Parameters: TInt aCommand: command id
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppView::HandleCommandL( TInt aCommand )
    {   
    AppUi()->HandleCommandL( aCommand );
    }

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: HandleClientRectChange

     Description: Handle size changes.

     Handle size changes.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: DoActivateL

     Description: From AknView, Activates view.

     From AknView, Activates view.

     Parameters: const TVwsViewId& aPrevViewId: previouse view id
     			 TUid aCustomMessageId: custom message id
   				 const TDesC8& aCustomMessage: custom message data
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if ( !iContainer )
        {
        // Create view container
        iContainer = CUITestServerStarterAppContainer::NewL( ClientRect() );
        iContainer->SetMopParent( this );
        // Add container to view stack
        AppUi()->AddToStackL( *this, iContainer );
        } 
   }

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppView

     Method: DoDeactivate

     Description: From AknView, Deactivates view.

     From AknView, Deactivates view.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppView::DoDeactivate()
    {
    if ( iContainer )
        {
        // Remove container from view stack
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    // Delete container since it's not longer needed.
    delete iContainer;
    iContainer = NULL;
    }

// End of File
