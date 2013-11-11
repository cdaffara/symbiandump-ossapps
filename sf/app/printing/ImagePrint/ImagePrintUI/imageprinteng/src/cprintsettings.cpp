/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#include "cprintsettings.h"
#include "cbasecapability.h"

// CONSTRUCTION
CPrintSettings::CPrintSettings()
    {    
    }

// Default destructor
CPrintSettings::~CPrintSettings()
    {
    iCapabilities.ResetAndDestroy();
    }

// Clone this instance and return it
CPrintSettings* CPrintSettings::CloneL()
    {
    CPrintSettings* clone = new ( ELeave ) CPrintSettings;
    CleanupStack::PushL( clone );
    
    TInt i( 0 );
    TInt num( iCapabilities.Count() );
    for ( i = 0; i < num; i++ )
        {
        User::LeaveIfError( 
            clone->iCapabilities.Append( iCapabilities[i]->CloneL() ) );
        }

    CleanupStack::Pop();	// clone

    return clone;
    }

//  End of File
