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
#include "../../symbian_version.hrh"

#include <centralrepository.h>
#include <hwrm/hwrmpowerdomaincrkeys.h>

TBool CPerfMonPowerListener::iIsSupported = ETrue;

/*
 *
 * class CPerfMonPowerListener implementation
 * 
 */

CPerfMonPowerListener* CPerfMonPowerListener::NewL()
    {
    CPerfMonPowerListener* self = new (ELeave) CPerfMonPowerListener();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CPerfMonPowerListener::CPerfMonPowerListener() :
    iPowerListenerImpl(NULL)
    {
    }

// --------------------------------------------------------------------------------------------

void CPerfMonPowerListener::ConstructL()
    {
    TRAPD(err, iPowerListenerImpl = CPerfMonPowerListenerImpl::NewL());
    if (err != KErrNone)
        {
        iPowerListenerImpl = NULL;

        // Could not initialize power monitoring client. Feature support not available.
        iIsSupported = EFalse;
        }
    }

// --------------------------------------------------------------------------------------------

CPerfMonPowerListener::~CPerfMonPowerListener()
    {
    DeActivate();

    if (iPowerListenerImpl)
        {
        delete iPowerListenerImpl;
        iPowerListenerImpl = 0;
        }
    }

// --------------------------------------------------------------------------------------------

TInt CPerfMonPowerListener::Activate()
    { 
    if (iPowerListenerImpl)
        {
        return iPowerListenerImpl->Activate();
        }
    else
        {
        return KErrNotSupported;
        }
    }

// ---------------------------------------------------------------------------

void CPerfMonPowerListener::DeActivate()
    {
    if (iPowerListenerImpl)
        {
        iPowerListenerImpl->DeActivate();
        }
    }

// ---------------------------------------------------------------------------

TInt CPerfMonPowerListener::GetPower()
    {
    if (iPowerListenerImpl)
        {
        return iPowerListenerImpl->GetPower();
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------

TInt CPerfMonPowerListener::GetMaxPower()
    {
    if (iPowerListenerImpl)
        {
        return iPowerListenerImpl->GetMaxPower();
        }
    else
        {
        return 0;
        }
    }

/*
 *
 * class CPerfMonPowerListenerImpl implementation
 * 
 */

CPerfMonPowerListenerImpl* CPerfMonPowerListenerImpl::NewL()
    {
    CPerfMonPowerListenerImpl* self = new (ELeave) CPerfMonPowerListenerImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CPerfMonPowerListenerImpl::CPerfMonPowerListenerImpl() :
    iHWRMPower(NULL),
    iLastPowerAvg(0),
    iMaxPower(0),
    iOriginalMaxReportingPeriod(0)
    {
    }

// --------------------------------------------------------------------------------------------

void CPerfMonPowerListenerImpl::ConstructL()
    {
    iHWRMPower = CHWRMPower::NewL();

    // Callbacks to this object
    User::LeaveIfError(iHWRMPower->SetPowerReportObserver(this));

    iOriginalMaxReportingPeriod = GetReportingPeriodL();
    }

// --------------------------------------------------------------------------------------------

CPerfMonPowerListenerImpl::~CPerfMonPowerListenerImpl()
    {
    DeActivate();

    if (iHWRMPower)
        {
        delete iHWRMPower;
        iHWRMPower = 0;
        }
    }

// --------------------------------------------------------------------------------------------

TInt CPerfMonPowerListenerImpl::Activate()
    {
    // Set infinite reporting period
    TRAPD(err, SetReportingPeriodL(KHWRMReportingDuration));
    if (err != KErrNone)
        {
        return err;
        }

    TRequestStatus status(KRequestPending);

    // Start the power consumption notification
    iHWRMPower->StartAveragePowerReporting(status, KHWRMSampleIntervalMultiple);
    User::WaitForRequest(status);

    // Check if monitoring was succesfully started or already ongoing.
    if ((status.Int() != KErrNone) && (status.Int() != KErrAlreadyExists))
        {
        return status.Int();
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------

void CPerfMonPowerListenerImpl::DeActivate()
    {
    if (iHWRMPower)
        {
        TRAP_IGNORE(iHWRMPower->StopAveragePowerReportingL());
        }

    // Restore original value to max sampling duration
    TRAP_IGNORE(SetReportingPeriodL(iOriginalMaxReportingPeriod));

    iPowerBuffer.Reset();
    iLastPowerAvg = 0;
    }

// ---------------------------------------------------------------------------

TInt CPerfMonPowerListenerImpl::GetPower()
    {
    TInt avgPower = 0;
    TInt newValueCount = iPowerBuffer.Count();

    if (newValueCount > 0)
        {
        // Read all new values from buffer and calculate average from them.
        for (int i = 0; i < newValueCount; i++)
            {
            avgPower += iPowerBuffer[i];
            }
        avgPower = avgPower / newValueCount;

        iPowerBuffer.Reset();
        iLastPowerAvg = avgPower;
        }
    // Use previous value if no new data has been reported since previous call.
    // This may happen since perfmon sampling and power reporting are not synchronized.
    else
        {
        avgPower = iLastPowerAvg;
        }

    return avgPower;
    }

// ---------------------------------------------------------------------------

void CPerfMonPowerListenerImpl::PowerMeasurement(TInt aErrCode, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement)
    {
    if (aErrCode == KErrNone)
        {
        // Calculate power value
        TInt value = aMeasurement.iAverageVoltage * aMeasurement.iAverageCurrent;

        // If charger is connected, reported values may be negative.
        if (value < 0)
            {
            value = 0;
            }

        // Store new value to buffer to wait for reading
        iPowerBuffer.Append(value);

        if ( value > iMaxPower )
            {
            iMaxPower = value;
            }
        }
    // Ignore any reports containing error information
    }

// ---------------------------------------------------------------------------

TInt CPerfMonPowerListenerImpl::GetReportingPeriodL()
    {
    CRepository* cenRep = CRepository::NewL(KCRUidPowerSettings);

    CleanupStack::PushL(cenRep);
    TInt reportingPeriod(0);
    User::LeaveIfError(cenRep->Get(KPowerMaxReportingPeriod, reportingPeriod));
    CleanupStack::PopAndDestroy();

    return reportingPeriod;
    }

// ---------------------------------------------------------------------------

void CPerfMonPowerListenerImpl::SetReportingPeriodL(TInt aDuration)
    {
    CRepository* cenRep = CRepository::NewL(KCRUidPowerSettings);

    CleanupStack::PushL(cenRep);
    User::LeaveIfError(cenRep->Set(KPowerMaxReportingPeriod, aDuration));
    CleanupStack::PopAndDestroy();
    }

// End of File
