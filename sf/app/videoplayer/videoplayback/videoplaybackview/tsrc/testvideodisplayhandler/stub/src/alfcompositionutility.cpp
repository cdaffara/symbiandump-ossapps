/*
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
* Description:   Stub implementation for CAlfCompositionSource
*
*/

// Version : %version: 1 %

//  Include Files

#include "alfcompositionutility.h"


// -------------------------------------------------------------------------------------------------
// CAlfCompositionSource::NewL()
// -------------------------------------------------------------------------------------------------
//
CAlfCompositionSource* CAlfCompositionSource::NewL( RWindow& /*aClientWindow*/ )
{
    CAlfCompositionSource* self = new (ELeave) CAlfCompositionSource();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CAlfCompositionSource::~CAlfCompositionSource()
// -------------------------------------------------------------------------------------------------
//
CAlfCompositionSource::~CAlfCompositionSource()
{
}

// -------------------------------------------------------------------------------------------------
// CAlfCompositionSource::EnableAlpha()
// -------------------------------------------------------------------------------------------------
//
TInt CAlfCompositionSource::EnableAlpha(TBool /*aEnable*/)	
{
    return KErrNone;
}
// EOF
