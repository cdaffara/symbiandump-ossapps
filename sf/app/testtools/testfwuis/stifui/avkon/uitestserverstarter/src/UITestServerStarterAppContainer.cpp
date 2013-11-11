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
* Description: This module contains the implementation of 
* CUITestServerStarterAppContainer class member functions.
*
*/

// INCLUDE FILES
#include "UITestServerStarterAppContainer.h"
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknDef.h>
#include <AknUtils.h> 

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

     Class: CUITestServerStarterAppContainer

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: const TRect& aRect: container rectangle.
     
     Return Values: Pointer to new CUITestServerStarterAppContainer object.

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppContainer* CUITestServerStarterAppContainer::NewL( const TRect& aRect )
	{
	CUITestServerStarterAppContainer* self = NewLC( aRect );
	CleanupStack::Pop( self );
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: NewLC

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: const TRect& aRect: container rect
     
     Return Values: Pointer to new CUITestServerStarterAppContainer object.

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppContainer* CUITestServerStarterAppContainer::NewLC( const TRect& aRect )
	{
	CUITestServerStarterAppContainer* self = 
		new(ELeave)CUITestServerStarterAppContainer;
	CleanupStack::PushL( self );
	self->ConstructL( aRect );
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: CUITestServerStarterAppContainer

     Description: C++ constructor.

     C++ constructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppContainer::CUITestServerStarterAppContainer()
	{	
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: CUITestServerStarterAppContainer

     Description: C++ destructor.

     C++ destructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppContainer::~CUITestServerStarterAppContainer()
	{
	delete iBgContext;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: ConstructL

     Description: ConstructL is second phase of two-phased constructor.

     ConstructL is second phase of two-phased constructor.

     Parameters: const TRect& aRect: container rectangle.
     
     Return Values: None

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppContainer::ConstructL( const TRect& aRect )
	{
	CreateWindowL();
	
	iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain, aRect, ETrue );	
	
	SetRect( aRect );
	ActivateL();
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: Draw

     Description: Draws the control.

     Draws the control.

     Parameters: const TRect& aRect: rectangle which should be redrawn.
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppContainer::Draw( const TRect& aRect ) const
	{
	// Get the standard graphics context 
	CWindowGc& gc = SystemGc();

	// Redraw the background using the default skin
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	MAknsControlContext* controlContext = AknsDrawUtils::ControlContext( this );
	AknsDrawUtils::Background( skin, controlContext, this, gc, aRect );	
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: SizeChanged

     Description: Responds to changes to the size and position of the contents of this control

     Responds to changes to the size and position of the 
     contents of this control

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppContainer::SizeChanged()
	{
	if ( iBgContext )
		{
		iBgContext->SetRect( Rect() );
		if ( &Window() )
			{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
			}
		}
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppContainer

     Method: MopSupplyObject

     Description: This function is used to allow controls to ask their owners for access to other objects that they own

     This function is used to allow controls to ask their owners 
	 for access to other objects that they own
	 
     Parameters: TTypeUid aId: requested object type id.
     
     Return Values: Pointer to requested object.

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TTypeUid::Ptr CUITestServerStarterAppContainer::MopSupplyObject( TTypeUid aId )
	{
	if ( iBgContext )
		{
		return MAknsControlContext::SupplyMopObject( aId, iBgContext );
		}
	return CCoeControl::MopSupplyObject( aId );
	}

// End of File
