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
* Description:  Versit data stripper and merger for contacts.
*
*/



// INCLUDES
#include <NSmlDataModBase.h>
#include <vcard.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlVCardMod::CNSmlVCardMod
// C++ default constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlVCardMod::CNSmlVCardMod() : CNSmlDataModBase()
	{
	}

// -----------------------------------------------------------------------------
// CNSmlVCardMod::ChildCreateParserLC
// Creates correct type of parser for Contacts Data Modifications.
// -----------------------------------------------------------------------------
//
CVersitParser* CNSmlVCardMod::ChildCreateParserLC()
	{
	CVersitParser* p = CParserVCard::NewL();
	CleanupStack::PushL( p );
	return p;
	}

// End of file
