/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VT User input implementation
*
*/

#include "mvtprotocolhandler.h"
#include "cvtuserinput.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// CVTUserInputDtmf::ConstructL
// Constructor.
// -----------------------------------------------------------------------------
//
void CVTUserInputDtmf::ConstructL(TUint8 aTone)
    {
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputDtmf::ConstructL<")));	
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputDtmf::ConstructL>")));
    }

// -----------------------------------------------------------------------------
// CVTUserInputDtmf::CVTUserInputDtmf
// Constructor.
// -----------------------------------------------------------------------------
//
CVTUserInputDtmf* CVTUserInputDtmf::NewL(TUint8 aTone)
    {
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputDtmf::NewL<")));	
    CVTUserInputDtmf* self = new (ELeave)CVTUserInputDtmf();
    CleanupStack::PushL(self);
    self->ConstructL(aTone);
    CleanupStack::Pop();
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputDtmf::NewL>")));
    return self;
    }

// -----------------------------------------------------------------------------
// CVTUserInputDtmf::~CVTUserInputDtmf
// Destructor.
// -----------------------------------------------------------------------------
//
CVTUserInputDtmf::~CVTUserInputDtmf()
    {
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputDtmf::~CVTUserInputDtmf<")));	
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputDtmf::~CVTUserInputDtmf>")));
    }

// -----------------------------------------------------------------------------
// CVTUserInputDtmf::GetType
// Virtual function to return the user input type
// -----------------------------------------------------------------------------
//
TUserInputType CVTUserInputDtmf::GetType()
    {
    return EVtUiiDTMF;
    }

// -----------------------------------------------------------------------------
// CVTUserInputDtmf::ConstructL
// Constructor.
// -----------------------------------------------------------------------------
//
void CVTUserInputAlphanumeric::ConstructL(TUint8 aTone)
    {
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputAlphanumeric::ConstructL<")));	
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputAlphanumeric::ConstructL>")));
    }

// -----------------------------------------------------------------------------
// CVTUserInputAlphanumeric::CVTUserInputAlphanumeric
// Constructor.
// -----------------------------------------------------------------------------
//
CVTUserInputAlphanumeric* CVTUserInputAlphanumeric::NewL(TUint8 aTone)
    {
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputAlphanumeric::NewL<")));
    CVTUserInputAlphanumeric* self = new (ELeave)CVTUserInputAlphanumeric();
    CleanupStack::PushL(self);
    self->ConstructL(aTone);
    CleanupStack::Pop();
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputAlphanumeric::NewL>")));   
    return self;
    }

// -----------------------------------------------------------------------------
// CVTUserInputAlphanumeric::~CVTUserInputAlphanumeric
// Destructor.
// -----------------------------------------------------------------------------
//
CVTUserInputAlphanumeric::~CVTUserInputAlphanumeric()
    {
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputAlphanumeric::~CVTUserInputAlphanumeric<")));		
    __IF_DEBUG(Print(_L("VTProto: CVTUserInputAlphanumeric::~CVTUserInputAlphanumeric>")));		
    }

// -----------------------------------------------------------------------------
// CVTUserInputAlphanumeric::GetType
// Virtual function to return the user input type.
// -----------------------------------------------------------------------------
//
TUserInputType CVTUserInputAlphanumeric::GetType()
    {
    return EVtUiiAlphaNumeric;
    }
// end of file
