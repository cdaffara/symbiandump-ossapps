/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video telephony logger stub
*
*/



// INCLUDE FILES
#include    "CVtLogger.h"
#include    <f32file.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtLogger::CVtLogger
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtLogger::CVtLogger()
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtLogger::ConstructL( )
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtLogger* CVtLogger::NewL( )
    {
    return NULL;
    }

// Destructor
CVtLogger::~CVtLogger()
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Prints to log.
// 
// -----------------------------------------------------------------------------
//
void CVtLogger::Print( TDesC& )
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Prints to log.
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print( 
    TInt , 
    TDesC&  )
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Prints to log.
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print( 
    TInt , 
    TDesC& , 
    TInt )
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Prints to log.
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print( 
    TInt , 
    TDesC& , 
    TInt ,
    TInt )
    {    
    }

CVtLogger* CVtLogger::Logger()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVtLogger::CheckLevels
// Check debug level.
// 
// -----------------------------------------------------------------------------
//
TBool CVtLogger::CheckLevels( TInt aLevels ) const
    {
    return EFalse;
    }

//  End of File  
