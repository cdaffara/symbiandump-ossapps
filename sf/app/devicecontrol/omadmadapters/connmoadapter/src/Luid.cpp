/*
 * ==============================================================================
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * ==============================================================================
 */

#include "Luid.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "LuidTraces.h"
#endif


// ======== LOCAL FUNCTIONS =========

// ======== MEMBER FUNCTIONS ========

//------------------------------------------------------------------------------
// TLuid::TLuid()
//------------------------------------------------------------------------------
TLuid::TLuid() 
    {

    }

//------------------------------------------------------------------------------
// TLuid::TLuid()
//------------------------------------------------------------------------------
TLuid::TLuid( const TDesC8& aName, const TUint32 aCmId ) 
    {
    OstTraceExt2( TRACE_NORMAL, TLUID_TLUID, "ENTRY: TLuid::TLuid;aName=%s;aCmId=%u", aName, aCmId );
    
    iName = aName;
    iCmId = aCmId;
    }

//------------------------------------------------------------------------------
// TLuid::Equals()
//------------------------------------------------------------------------------
TBool TLuid::Equals( TUint32 aId ) const
    {
    OstTraceFunctionEntry1( TLUID_EQUALS_ENTRY, this );
    if ( aId == iCmId)
        {
        OstTraceFunctionExitExt( TLUID_EQUALS_EXIT, this, ETrue );
        return ETrue;
        }
    else
        {
        OstTraceFunctionExitExt( DUP1_TLUID_EQUALS_EXIT, this, EFalse );
        return EFalse;
        }
    }

//------------------------------------------------------------------------------
// TLuid::GetCmId()
//------------------------------------------------------------------------------
TUint32 TLuid::GetCmId()
    {
    OstTrace1( TRACE_NORMAL, TLUID_GETCMID, "TLuid::GetCmId;iCmId=%u", iCmId );
    
    return iCmId;
    }

// ======== GLOBAL FUNCTIONS ========  
