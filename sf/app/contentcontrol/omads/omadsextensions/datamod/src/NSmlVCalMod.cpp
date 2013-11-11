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
* Description:  Versit data stripper and merger for calendar.
*
*/



// INCLUDES
#include <NSmlDataModBase.h>
#include <vcal.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlVCalMod::CNSmlVCalMod
// C++ default constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlVCalMod::CNSmlVCalMod() : CNSmlDataModBase()
	{
	}

// -----------------------------------------------------------------------------
// CNSmlVCalMod::ChildCreateParserLC
// Creates correct type of parser for Calendar Data Modifications.
// -----------------------------------------------------------------------------
//
CVersitParser* CNSmlVCalMod::ChildCreateParserLC()
	{
	CVersitParser* p = CParserVCal::NewL();
	CleanupStack::PushL( p );
	return p;
	}

// End of file
