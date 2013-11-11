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
* Description: 
*
*/

#include "CObjectInterface.h"
#include "CCPixNPPluginEcom.h"
#include <utf.h>


// ============================ MEMBER FUNCTIONS ===============================
CObjectInterface::CObjectInterface()
	{
	}

// -----------------------------------------------------------------------------

CObjectInterface::~CObjectInterface()
	{
    delete [] iPropertyIdentifiers;
    delete [] iMethodIdentifiers; 
	}

void CObjectInterface::Deallocate () 
    {
    delete this; 
    }

// -----------------------------------------------------------------------------

void CObjectInterface::SetIdentifiersL( const NPUTF8** aPropertyNames, TInt aPropertyCount, const NPUTF8** aMethodNames, TInt aMethodCount )
	{
	iPropertyCount = aPropertyCount;
	if (aPropertyNames)
	    {
	    iPropertyIdentifiers = new (ELeave) NPIdentifier[iPropertyCount];
	    NPN_GetStringIdentifiers( aPropertyNames, iPropertyCount, iPropertyIdentifiers );
	    }
	iMethodCount = aMethodCount;
	if (aMethodNames)
	    {
	    iMethodIdentifiers = new (ELeave) NPIdentifier[iMethodCount];
	    NPN_GetStringIdentifiers( aMethodNames, iMethodCount, iMethodIdentifiers );
	    }
	}

// -----------------------------------------------------------------------------

bool CObjectInterface::HasProperty( NPIdentifier name )
    {
    for ( TInt i= 0; i < iPropertyCount; ++i )
        {
        if ( name == iPropertyIdentifiers[i] ) return true;
        }
    return false;
    }

// -----------------------------------------------------------------------------

bool CObjectInterface::HasMethod( NPIdentifier name )
    {
    for ( TInt i= 0; i < iMethodCount; ++i )
        {
        if ( name == iMethodIdentifiers[i] ) return true;
        }
    return false;
    }

