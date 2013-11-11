/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// User include files
#include "directprintprintsettings.h"
#include "directprintbasecapability.h"

// CONSTRUCTION
CDirectPrintPrintSettings::CDirectPrintPrintSettings()
    {
    }

// Default destructor
CDirectPrintPrintSettings::~CDirectPrintPrintSettings()
    {
    iCapabilities.ResetAndDestroy();
    }

// Clone this instance and return it
CDirectPrintPrintSettings* CDirectPrintPrintSettings::CloneL()
    {
    CDirectPrintPrintSettings* clone = new ( ELeave ) CDirectPrintPrintSettings;
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
