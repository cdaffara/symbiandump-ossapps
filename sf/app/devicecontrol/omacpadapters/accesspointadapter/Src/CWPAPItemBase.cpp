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
* Description:    Defines a base class for all the items in WPAPAdapter
*
*/





// INCLUDE FILES

#include "CWPAPItemBase.h"
#include "WPAPAdapter.pan"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPAPItemBase::~CWPAPItemBase()
	{
	}

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
CWPAPItemBase::CWPAPItemBase( const TDesC& aDefaultName )
                          : iDefaultName( aDefaultName )
	{
	}

// -----------------------------------------------------------------------------
// CWPAPItemBase::MarkReceived
// -----------------------------------------------------------------------------
//
void CWPAPItemBase::MarkReceived( TMarkedField aField )
	{
	iReceivedFieldsFlags |= aField;
	}

// -----------------------------------------------------------------------------
// CWPAPItemBase::IsReceived
// -----------------------------------------------------------------------------
//
TBool CWPAPItemBase::IsReceived( TMarkedField aField )
	{
	return ( iReceivedFieldsFlags & aField );
	}
	
//  End of File  
