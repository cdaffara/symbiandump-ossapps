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
 * Description:  
 *
 */

// INCLUDE FILES
#include "perfmon_powerlistener.h"

// Power monitoring not supported. Use stub implementation.
TBool CPerfMonPowerListener::iIsSupported = EFalse;

/*
 *
 * class CPerfMonPowerListener stub implementation
 * 
 */

CPerfMonPowerListener* CPerfMonPowerListener::NewL()
    {
    CPerfMonPowerListener* self = new ( ELeave ) CPerfMonPowerListener();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CPerfMonPowerListener::CPerfMonPowerListener() :
    iPowerListenerImpl(NULL)
    {
    }

void CPerfMonPowerListener::ConstructL()
    {
    }

CPerfMonPowerListener::~CPerfMonPowerListener()
    {
    DeActivate();
    }

TInt CPerfMonPowerListener::Activate()
    {
    return KErrNotSupported;
    }

void CPerfMonPowerListener::DeActivate()
    {
    }

TInt CPerfMonPowerListener::GetPower()
    {
    return 0;
    }

TInt CPerfMonPowerListener::GetMaxPower()
    {
    return 0;
    }

// End of File
