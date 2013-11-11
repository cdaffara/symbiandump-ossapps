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
* Description:  Container for factory library handles.
*
*/


// INCLUDE FILES
#include "cphonelibrarycontainer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneLibraryContainer::~CPhoneLibraryContainer()
// Destructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneLibraryContainer::~CPhoneLibraryContainer()
    {
    TInt i( 0 );
    TInt count( iLibraries.Count() ); 

    // Close the libraries
    for( ; i < count; i++ )
        {
        iLibraries[i].Close();
        }

    // Free allocated memory from array
    iLibraries.Close();
    }

// ---------------------------------------------------------
// CPhoneLibraryContainer::CPhoneLibraryContainer()
// Default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPhoneLibraryContainer::CPhoneLibraryContainer()
    {
    }

// ---------------------------------------------------------
// CPhoneLibraryContainer::NewL()
// Two-phased constructor.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneLibraryContainer* CPhoneLibraryContainer::NewL()
    {
    CPhoneLibraryContainer* self = new( ELeave ) CPhoneLibraryContainer;

    return self;
    }

// ---------------------------------------------------------
// CPhoneLibraryContainer::AddLibraryL()
// Adds library to container.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPhoneLibraryContainer::AddLibraryL( 
    const RLibrary& aLibraryHandle )
    {
    User::LeaveIfError( iLibraries.Append( aLibraryHandle ) );
    }

//  End of File  
