/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  implementation of CPEParserMiscHandler class.
*
*/


// INCLUDES
#include "cpeparsermischandler.h"
#include "cpemessagehandler.h"
#include <mpecallhandling.h>

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEParserMiscHandler::CPEParserMiscHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEParserMiscHandler::CPEParserMiscHandler(
        CPEMessageHandler& aOwner,
        MPECallHandling& aCallHandling
        ) : iOwner( aOwner ),
            iCallHandling( aCallHandling )
    {
    }

// Destructor
CPEParserMiscHandler::~CPEParserMiscHandler()
    {    
    }

// -----------------------------------------------------------------------------
// CPEParserMiscHandler::ProcessUssdL
// Process USSD request
// -----------------------------------------------------------------------------
//
void CPEParserMiscHandler::ProcessUssdL(
        const TDesC& aString
        )
    {
    iOwner.HandleSendUssd( aString );
    }

// End of File
