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
 * Description:   Fota server update and download functionality 
 *
 */

#include "deviceupdatemoniter.h"
#include "DeviceUpdateDebug.h"

// --------------------------------------------------------------------------
// CDeviceUpdateMoniter::NewL 
// --------------------------------------------------------------------------
//
CDeviceUpdateMoniter* CDeviceUpdateMoniter::NewL (MDeviceMoniterObserver* aObserver)
    {
    FLOG(_L("CDeviceUpdateMoniter::NewL >>"));    
    CDeviceUpdateMoniter* h=new (ELeave)CDeviceUpdateMoniter;
    h->iObserver = aObserver;
    h->ConstructL();
    FLOG(_L("CDeviceUpdateMoniter::NewL <<"));
    return h;
    }

void CDeviceUpdateMoniter::ConstructL()
    {
    FLOG(_L("CDeviceUpdateMoniter::ConstructL >>"));
    TInt err = RProperty::Define(TUid::Uid(KOmaDMAppUid), KFotaServerActive, RProperty::EInt);
    
    if(err == KErrAlreadyExists)
        {
        TInt value;
        RProperty::Get(TUid::Uid(KOmaDMAppUid), KFotaServerActive, value);
        if(value)
            iObserver->UpdateDMUI(EFalse);
        }
    err = iProperty.Attach(TUid::Uid(KOmaDMAppUid), KFotaServerActive);
    if(err == KErrNone)
        {
        iProperty.Subscribe(iStatus);
        if ( !IsActive() )
            {
            SetActive();
            }
        }
    FLOG(_L("CDeviceUpdateMoniter::ConstructL <<"));
    }


void CDeviceUpdateMoniter::StartMoniter()
    {
    FLOG(_L("CDeviceUpdateMoniter::StartMoniter() >>"));
    iProperty.Subscribe(iStatus);
    if ( !IsActive() )
        {
        SetActive();
        }
    FLOG(_L("CDeviceUpdateMoniter::StartMoniter <<"));
    }

// --------------------------------------------------------------------------
CDeviceUpdateMoniter::CDeviceUpdateMoniter()
            : CActive(EPriorityNormal)
    {
    FLOG(_L("CDeviceUpdateMoniter::CDeviceUpdateMoniter() >>"));
    CActiveScheduler::Add( this );
    FLOG(_L("CDeviceUpdateMoniter::CDeviceUpdateMoniter <<"));
    }

// --------------------------------------------------------------------------
CDeviceUpdateMoniter::~CDeviceUpdateMoniter()
    {
    FLOG(_L("CDeviceUpdateMoniter::~CDeviceUpdateMoniter >>"));
    //iNotifier.Close(); FOTA_10.1 changes
    //Cancel();
    FLOG(_L("CDeviceUpdateMoniter::~CDeviceUpdateMoniter <<"));
    }

// --------------------------------------------------------------------------
void CDeviceUpdateMoniter::RunL()
    {
    FLOG(_L("CDeviceUpdateMoniter::RunL >>"));
    TInt value;
    TUid uid = {0x102072C4};
    
    RProperty::Get(TUid::Uid(KOmaDMAppUid), KFotaServerActive, value);
    if(value)
        iObserver->UpdateDMUI(EFalse);
    else
        iObserver->UpdateDMUI(ETrue);
    StartMoniter();
    FLOG(_L("CDeviceUpdateMoniter::RunL <<"));
    }

// --------------------------------------------------------------------------
TInt CDeviceUpdateMoniter::RunError(TInt aError)
    {   
    FLOG(_L("CDeviceUpdateMoniter::RunL %d"),aError);
    return aError;
    }

TInt CDeviceUpdateMoniter::CancelRequest()
    {
    FLOG(_L("CDeviceUpdateMoniter::CancelRequest %d"));
    
    }

// --------------------------------------------------------------------------
void CDeviceUpdateMoniter::DoCancel()
    {
    FLOG(_L("CDeviceUpdateMoniter::DoCancel >>"));
    iProperty.Cancel();
    FLOG(_L("CDeviceUpdateMoniter::DoCancel <<"));
    }

