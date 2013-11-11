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
* Description:  ConML parser/generator
*
*/


// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "sconxmlelement.h"
#include "sconconmldtd.h"

// -----------------------------------------------------------------------------
// TElementParams
// -----------------------------------------------------------------------------
TXMLElementParams::TXMLElementParams( 
    TAny* aCallbacks, 
    CXMLStack<CXMLElement>* aCmdStack, 
    CXMLStack<CXMLElement>* aCleanupStack ) :
	iCallbacks(aCallbacks), 
	iCmdStack(aCmdStack), 
	iCleanupStack(aCleanupStack)
	{
	}

// -----------------------------------------------------------------------------
// CXMLElement
// -----------------------------------------------------------------------------
CXMLElement* CXMLElement::BeginElementL( 
    TUint8 /*aTag*/, const TXMLElementParams& /*Params*/ )
	{
	return 0;
	}

// -----------------------------------------------------------------------------
CXMLElement::TAction CXMLElement::EndElementL( TAny* /*aCallbacks*/, 
    TUint8 /*aTag*/ )
	{
	return EPop;
	}


// -----------------------------------------------------------------------------
TBool CXMLElement::NeedsCleanup() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
void CXMLElement::SetDataL( const TDesC8& /*aData*/ )
	{
	}

// -----------------------------------------------------------------------------
TPtrC8 CXMLElement::Data() const
	{
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
TBool CXMLElement::Validate()
	{
	return ETrue;
	}
