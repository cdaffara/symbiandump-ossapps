/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CGI parser and generator
*
*/



// INCLUDE FILES
#include "nsmlcgiscriptparser.h"


// CONSTANTS
const TInt KNSmlGranularity = 4;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlCGIScript::CNSmlCGIScript
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlCGIScript::CNSmlCGIScript()
	{
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlCGIScript* CNSmlCGIScript::NewL()
	{
	CNSmlCGIScript* self = NewLC();
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlCGIScript* CNSmlCGIScript::NewLC()
	{
	CNSmlCGIScript* self = new ( ELeave ) CNSmlCGIScript();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlCGIScript::~CNSmlCGIScript
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlCGIScript::~CNSmlCGIScript()
	{
	delete iCGIScript;
	Clear();
	delete iScriptParts;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlCGIScript::ConstructL()
	{
	iSeparator = ENSmlCGIScriptLogicalOperatorAnd;
	iScriptParts = new (ELeave) CArrayPtrFlat<TNSmlCGIScriptPart>( KNSmlGranularity );
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::LogicalOperator
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlCGIScriptLogicalOperator CNSmlCGIScript::LogicalOperator() const
	{
	return iSeparator;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::SetLogicalOperator
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCGIScript::SetLogicalOperator( const TNSmlCGIScriptLogicalOperator aSeparator )
	{
	iSeparator = aSeparator;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::CGIScript
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CNSmlCGIScript::CGIScript() const
	{
	return iCGIScript;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::SetCGIScriptL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCGIScript::SetCGIScriptL( const TDesC& aCGIScript )
	{
		if ( iCGIScript )
		{
		delete iCGIScript;
		iCGIScript = NULL;
		}
	iCGIScript = HBufC::NewL( aCGIScript.Length() );
	*iCGIScript = aCGIScript;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::Count
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNSmlCGIScript::Count() const
	{
	return iScriptParts->Count();
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::Get
// -----------------------------------------------------------------------------
//
EXPORT_C const TNSmlCGIScriptPart* CNSmlCGIScript::Get( TInt aIndex ) const
	{
	return iScriptParts->At( aIndex );
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::AddL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCGIScript::AddL( TNSmlCGIScriptPart* aRecordPart )
	{
	iScriptParts->AppendL( aRecordPart );
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::Remove
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlCGIScriptPart* CNSmlCGIScript::Remove( TInt aIndex )
	{
	TNSmlCGIScriptPart* temp = iScriptParts->At( aIndex );
	iScriptParts->Delete( aIndex );
	return temp;
	}

// -----------------------------------------------------------------------------
// CNSmlCGIScript::Clear
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCGIScript::Clear()
    {
    for ( TInt i = 0; i < iScriptParts->Count(); i++ )
		{
		TNSmlCGIScriptPart* temp = iScriptParts->At( i );
		delete temp->iName;
        temp->iName = NULL;
		delete temp->iData;
        temp->iData = NULL;
		}
  	iScriptParts->ResetAndDestroy();
    }


//  End of File  
