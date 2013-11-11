/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains entry point for the test object.
*
*/

// INCLUDES
#include <e32base.h>

#include "caleninterimutils2test.h"

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    
    TBuf< 1024 > titleString;
    aTestModuleIf.GetTestCaseTitleL( titleString );
    
    TChar separatorChar = ':';
    TPtrC titlePtr;
    titlePtr.Set( titleString.Ptr(), titleString.Length() );
    
    TInt colonPos = titlePtr.Locate( separatorChar );
    TPtrC16 stringLeft = titlePtr.Left( colonPos );
    
    if( 0 == stringLeft.Compare( _L( "InterimUtils" ) ) )
        {
        return ( CScriptBase* ) CCalenInterimUtils2Test::NewL( aTestModuleIf );
        }
    
    return NULL;

    }

// End of file
