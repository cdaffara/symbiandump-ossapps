/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contain GSM specific implementation of CPEPhoneModeL::CreateL
*
*/


#include "cpephonemodelif.h"
#include "cpephonemodel.h"

// ---------------------------------------------------------------------------
// Returns the pointer of the CPEPhoneModel object.
// ---------------------------------------------------------------------------
//
EXPORT_C CPEPhoneModelIF* CPEPhoneModelIF::CreateL( MEngineMonitor& aEngineMonitor )
    {
    return CPEPhoneModel::NewL( aEngineMonitor );
    }

// ---------------------------------------------------------------------------
// Returns the pointer of the CPEPhoneModel object.
// ---------------------------------------------------------------------------
//
EXPORT_C CPEPhoneModelIF* CPEPhoneModelIF::CreateL( 
    MEngineMonitor& aEngineMonitor,
    CPEAudioFactory& aAudioFactory )
    {
    return CPEPhoneModel::NewL( aEngineMonitor, aAudioFactory );
    }

// End of File
