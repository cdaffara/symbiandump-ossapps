/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message handler
*
*/


// -----------------------------------------------------------------------------
// Gets basic service group value.
// -----------------------------------------------------------------------------
//
inline RMobilePhone::TMobileService CPEMessageHandler::BasicServiceGroup()
    { 
    return iBasicServiceGroup;
    }

// -----------------------------------------------------------------------------
// Handles swap message from the phone application 
// -----------------------------------------------------------------------------
//
inline TInt CPEMessageHandler::HandleSwapCalls() 
    { 
    return iCallHandling.SwapCalls(); 
    }

// -----------------------------------------------------------------------------
// Handles transfer message from phone application.
// -----------------------------------------------------------------------------
//
inline TInt CPEMessageHandler::HandleTransferCalls() 
    {
    return iCallHandling.TransferCalls();
    }

// -----------------------------------------------------------------------------
// Sets basic service group for future using.
// -----------------------------------------------------------------------------
//
inline void CPEMessageHandler::SetBasicServiceGroup( 
        const RMobilePhone::TMobileService& aServiceGroup )
    { 
    iBasicServiceGroup = aServiceGroup;
    }

// -----------------------------------------------------------------------------
// Sets gsm parser error code for future using.
// -----------------------------------------------------------------------------
//
inline void CPEMessageHandler::SetGsmParserErrorCode( 
        TInt aGsmParserErrorCode)
    {
    iGsmParserErrorCode = aGsmParserErrorCode;
    }
