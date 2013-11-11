/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer for Publish & Subscribe keys
*
*/

// INCLUDE FILES

#include "pspropertyobserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPSPropertyObserver::CPSPropertyObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPSPropertyObserver::CPSPropertyObserver(MPSPropertyChangeObserver& aObserver, const TUid& aCategory, const TUint aKey, const RProperty::TType aPropertyType)
    : CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iCategory( aCategory ),
    iKey( aKey ),
    iPropertyType( aPropertyType )
	{
	}

// -----------------------------------------------------------------------------
// CPSPropertyObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPSPropertyObserver::ConstructL()
	{
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
    RunL();
	}

// -----------------------------------------------------------------------------
// CPSPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPSPropertyObserver* CPSPropertyObserver::NewL(MPSPropertyChangeObserver& aObserver, const TUid& aCategory, const TUint aKey, const RProperty::TType aPropertyType)
	{
    CPSPropertyObserver* self = new( ELeave )CPSPropertyObserver( aObserver, aCategory, aKey, aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

// Destructor
CPSPropertyObserver::~CPSPropertyObserver()
	{
	Cancel();
    iProperty.Close();
	}

// -----------------------------------------------------------------------------
// CPSPropertyObserver::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPSPropertyObserver::RunL()
	{
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt err(KErrNone);
    
    switch (iPropertyType)
        {
        case RProperty::EInt:
	        {
	        err = iProperty.Get( iValueInt );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, iValueInt );
             	}
	        break;
	        }
	    case RProperty::EText:
	        {
	        err = iProperty.Get( iValueText );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, iValueText );
             	}
	        break;
	        }
	    default:
	        {
	        break;
	        }
        }
    
    if (err)
    	{
        iObserver.HandlePropertyChangeErrorL(iCategory, iKey, err);
     	}
	}

// -----------------------------------------------------------------------------
// CPSPropertyObserver::DoCancel
// Cancels an outstanding active request
// -----------------------------------------------------------------------------
//
void CPSPropertyObserver::DoCancel()
	{
    iProperty.Cancel();
	}

// -----------------------------------------------------------------------------
// CPSPropertyObserver::GetValue
// -----------------------------------------------------------------------------
//	
void CPSPropertyObserver::GetValue(TInt& aValue) const
	{
	aValue = iValueInt;
	}
	
// -----------------------------------------------------------------------------
// CPSPropertyObserver::GetValue
// -----------------------------------------------------------------------------
//	
void CPSPropertyObserver::GetValue(TPSTextProperty& aValue) const
	{
	aValue = iValueText;
	}
	
// End of File
