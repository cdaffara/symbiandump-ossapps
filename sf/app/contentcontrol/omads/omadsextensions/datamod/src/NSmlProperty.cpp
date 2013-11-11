/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Property class
*
*/



// INCLUDES
#include <NSmlDataModBase.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlProperty* CNSmlProperty::NewL( CParserPropertyValue& aPropertyValue, const TDesC8& aName, CArrayPtr<CParserParam>* aArrayOfParams )
	{
	CNSmlProperty* self = new ( ELeave ) CNSmlProperty( aPropertyValue, aArrayOfParams );
	CleanupStack::PushL( self );
	ConstructSelfL( *self, aName );
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlProperty::Parameters
// -----------------------------------------------------------------------------
//
CArrayPtr<CParserParam>* CNSmlProperty::Parameters() const
	{
	return iArrayOfParams;
	}

// -----------------------------------------------------------------------------
// CNSmlProperty::ParamCount
// -----------------------------------------------------------------------------
//
TInt CNSmlProperty::ParamCount() const
	{
	if ( iArrayOfParams )
		{
		return iArrayOfParams->Count();
		}
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CNSmlProperty::Compare
// -----------------------------------------------------------------------------
//
TInt CNSmlProperty::Compare( const CNSmlProperty& aFirst, const CNSmlProperty& aSecond )
	{
	return aFirst.Name().Compare( aSecond.Name() );
	}
	
// -----------------------------------------------------------------------------
// CNSmlProperty::CNSmlProperty
// -----------------------------------------------------------------------------
//
CNSmlProperty::CNSmlProperty( CParserPropertyValue& aPropertyValue, CArrayPtr<CParserParam>* aArrayOfParams ) : CParserProperty(aPropertyValue, aArrayOfParams)
	{
	}
	
// End of file
