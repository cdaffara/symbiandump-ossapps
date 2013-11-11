/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Structure to hold MMS encapsulation 1.2 version
*                result array for M-MBox delete confirmation
*
*/




// INCLUDE FILES
#include    "mmsdeleteresultarray.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsDeleteResultArray::CMmsDeleteResultArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsDeleteResultArray::CMmsDeleteResultArray()
    {
    // everything is initailized to 0 by CBase
    }

    
// Destructor
CMmsDeleteResultArray::~CMmsDeleteResultArray()
    {
    delete iContentLocation;
    delete iResponseText;
    }


// -----------------------------------------------------------------------------
// CMmsDeleteResultArray::Compare
// -----------------------------------------------------------------------------
//
TInt CMmsDeleteResultArray::Compare(
    const CMmsDeleteResultArray& aFirst,
    const CMmsDeleteResultArray& aSecond )
    {
    if ( aFirst.iOrder == aSecond.iOrder )
        {
        return 0;
        }
    if ( aFirst.iOrder > aSecond.iOrder )
        {
        return 1;
        }
    else
        {
        return -1;
        }
    }

// -----------------------------------------------------------------------------
// CMmsDeleteResultArray::ResponseText
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CMmsDeleteResultArray::ResponseText() const
    {
    return iResponseText ? TPtrC( *iResponseText ) : TPtrC(); 
    }

// -----------------------------------------------------------------------------
// CMmsDeleteResultArray::SetResponseStatusL
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsDeleteResultArray::SetResponseTextL( const TDesC& aResponseText )
    {
    HBufC* newAttrib = aResponseText.AllocL();
    delete iResponseText;
    iResponseText = newAttrib;
    }

// -----------------------------------------------------------------------------
// CMmsDeleteResultArray::ContentLocation
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsDeleteResultArray::ContentLocation() const
    {
    return iContentLocation ? TPtrC8( *iContentLocation ) : TPtrC8(); 
    }

// -----------------------------------------------------------------------------
// CMmsDeleteResultArray::SetContentLocationL
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsDeleteResultArray::SetContentLocationL( const TDesC8& aContentLocation )
    {
    HBufC8* newAttrib = aContentLocation.AllocL();
    delete iContentLocation;
    iContentLocation = newAttrib;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
