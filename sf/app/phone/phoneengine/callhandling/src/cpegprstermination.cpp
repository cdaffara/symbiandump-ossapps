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
* Description:  GPRS termination active object implementation.
*
*/


// INCLUDE FILES
#include "cpegprstermination.h"
#include <talogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEGprsTermination::CPEGprsTermination
// -----------------------------------------------------------------------------
//
CPEGprsTermination::CPEGprsTermination()
    : CActive( EPriorityStandard )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPEGprsTermination::CPEGprsTermination" );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEGprsTermination::CPEGprsTermination
// -----------------------------------------------------------------------------
//
CPEGprsTermination::~CPEGprsTermination()
    {
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPEGprsTermination::~CPEGprsTermination" );
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPEGprsTermination::StartL
// -----------------------------------------------------------------------------
//
void CPEGprsTermination::StartL()
    {
    TEFLOGSTRING( KTAINT, "cpecall: CPEGprsTermination::StartL" );
    }

// -----------------------------------------------------------------------------
// CPEGprsTermination::IsTerminating
// -----------------------------------------------------------------------------
//
TBool CPEGprsTermination::IsTerminating() const
    {
    TEFLOGSTRING( KTAINT, "cpecall: CPEGprsTermination::IsTerminating" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPEGprsTermination::RunL
// -----------------------------------------------------------------------------
//
void CPEGprsTermination::RunL()
    {
    TEFLOGSTRING2( KTAREQEND, "cpecall: CPEGprsTermination::RunL status: %d", iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CPEGprsTermination::DoCancel
// -----------------------------------------------------------------------------
//
void CPEGprsTermination::DoCancel()
    {
    TEFLOGSTRING( KTAREQEND, "cpecall: CPEGprsTermination::DoCancel" );
    }

//  End of File  
