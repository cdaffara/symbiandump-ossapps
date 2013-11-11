/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the class CSPCallArray
*
*/


#include <etelmm.h>

#include "cspcallarray.h"
#include "csplogger.h"
#include "cspcall.h"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// CSPCallArray::NewL.
// ---------------------------------------------------------------------------
//
CSPCallArray* CSPCallArray::NewL( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPCallArray::NewL()" );
    CSPCallArray* self = new ( ELeave ) CSPCallArray(  );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the array including remaining calls.
// ---------------------------------------------------------------------------
//
CSPCallArray::~CSPCallArray( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPCallArray::~CSPCallArray()" );
    TInt callCount = iCallArray.Count();
    CSPLOGSTRING2(CSPOBJECT, "CSPCallArray::~CSPCallArray() calls %d", callCount);
    
    for (TInt callIndex = 0; callIndex < callCount; callIndex++)
        {
        CSPCall* call = iCallArray[ callIndex ];
        delete call;
        }
        
    iCallArray.Reset();
    iCallArray.Close();
    }

// ---------------------------------------------------------------------------
// From MCSPCallInformation
// Iterates through call objects finding the call matching the given name.
// ---------------------------------------------------------------------------
//    
CSPCall* CSPCallArray::FindCall( const TName& aCallName )
    {
    CSPLOGSTRING(CSPINT, "CSPCallArray::FindCall()");
    
    TInt callCount = iCallArray.Count();
    for (TInt callIndex = 0; callIndex < callCount; callIndex++)
        {

        CSPCall* call = iCallArray[ callIndex ];        
        if ( call )
            {
            TName currentCallName;
            call->CallName( currentCallName );
            if ( currentCallName.Compare( aCallName ) == 0 )
                {
                CSPLOGSTRING(CSPINT, "CSPCallArray::FindCall() OK");
                return call;
                }
            }
        }

    CSPLOGSTRING(CSPINT, "CSPCallArray::FindCall() NOT FOUND");    
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSPCallArray::Add
// ---------------------------------------------------------------------------
//    
TInt CSPCallArray::Add(CSPCall* aCall ) 
    { 
    CSPLOGSTRING(CSPINT, "CSPCallArray::Add()");
    return iCallArray.Append( aCall ); 
    }

// ---------------------------------------------------------------------------
// CSPCallArray::Remove
// ---------------------------------------------------------------------------
//    
TInt CSPCallArray::Remove(CSPCall* aCall ) 
    {
    CSPLOGSTRING(CSPINT, "CSPCallArray::Remove()");
    TInt index = iCallArray.Find( aCall );
    if ( index != KErrNotFound )
        {
        iCallArray.Remove( index );
        return KErrNone;
        }
    CSPLOGSTRING(CSPINT, "CSPCallArray::Remove() NOT FOUND");
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CSPCallArray::GetCallCount
// ---------------------------------------------------------------------------
//    
TInt CSPCallArray::GetCallCount( )
    {
    return iCallArray.Count();
    }

// ---------------------------------------------------------------------------
// CSPCallArray::Get
// ---------------------------------------------------------------------------
//    
CSPCall* CSPCallArray::Get( TInt aIndex )
    {
    return iCallArray[aIndex];
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPCallArray::CSPCallArray()
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPCallArray::CSPCallArray()" );
    }

// End of File
