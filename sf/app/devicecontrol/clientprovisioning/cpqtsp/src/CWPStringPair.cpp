/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Sortable pair of strings.
*
*/


// INCLUDE FILES
#include "CWPStringPair.h"

// ============================ MEMBER FUNCTIONS ===============================

// C++ default constructor.
CWPStringPair::CWPStringPair()
	{
	}

// Destructor
CWPStringPair::~CWPStringPair()
	{
	delete iName;
	delete iValue;
	}

// ---------------------------------------------------------
// CWPStringPair::SetL
// ---------------------------------------------------------
//
void CWPStringPair::SetL( const TDesC& aName, const TDesC& aValue )
	{
	HBufC* name = aName.AllocLC();
	HBufC* value = aValue.AllocL();
	CleanupStack::Pop(); // name

	delete iName;
	iName = name;
	iNamePtr.Set( *iName );

	delete iValue;
	iValue = value;
	iValuePtr.Set( *iValue );
	}

//  End of File
