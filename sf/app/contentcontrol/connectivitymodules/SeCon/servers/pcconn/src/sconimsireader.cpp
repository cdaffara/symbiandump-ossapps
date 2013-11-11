/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSconImsiReader implementation
*
*/


#include "sconimsireader.h"
#include "debug.h"

TInt CSconImsiReader::GetImsiL( TDes& aImsi )
    {
    TRACE_FUNC_ENTRY;
    CTelephony::TRegistrationStatus temp;
    CSconImsiReader* self = new (ELeave) CSconImsiReader(aImsi, temp);
    CleanupStack::PushL(self);
    self->ConstructL();
    
    self->RequestImsi();
    
    TInt err = self->iStatus.Int();
    CleanupStack::PopAndDestroy(self);
    TRACE_FUNC_RET(err);
    return err;
    }

TInt CSconImsiReader::GetNetworkStatusL( CTelephony::TRegistrationStatus& aRegistrationStatus )
    {
    TRACE_FUNC_ENTRY;
    TBuf<CTelephony::KIMSISize> temp;
    CSconImsiReader* self = new (ELeave) CSconImsiReader(temp, aRegistrationStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    
    self->RequestNetworkStatus();
    
    TInt err = self->iStatus.Int();
    CleanupStack::PopAndDestroy(self);
    TRACE_FUNC_RET(err);
    return err;
    }
 
void CSconImsiReader::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iTelephony = CTelephony::NewL();
    CActiveScheduler::Add(this);
    TRACE_FUNC_EXIT;
    }

void CSconImsiReader::RequestImsi()
    {
    iImsiReaderStatus = ERequestingImsi;
    iTelephony->GetSubscriberId(iStatus,iSubscriberIdV1Pckg);
    SetActive();
    CActiveScheduler::Start();
    }

void CSconImsiReader::RequestNetworkStatus()
    {
    iImsiReaderStatus = ERequestingNetworkStatus;
    iTelephony->GetNetworkRegistrationStatus(iStatus, iNetworkRegistrationV1Pckg);
    SetActive();
    CActiveScheduler::Start();
    }

CSconImsiReader::~CSconImsiReader()
    {
    TRACE_FUNC_ENTRY;
    delete iTelephony;
    TRACE_FUNC_EXIT;
    }
 
CSconImsiReader::CSconImsiReader(TDes& aImsi, CTelephony::TRegistrationStatus& aRegistrationStatus): CActive(EPriorityStandard),
    iImsi(aImsi), iNetworkStatus(aRegistrationStatus), iSubscriberIdV1Pckg(iSubscriberIdV1), iNetworkRegistrationV1Pckg(iNetworkRegistrationV1)
    {
    }
 
void CSconImsiReader::RunL()
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("iStatus: %d", iStatus.Int() );
    if ( iStatus == KErrNone )
        {
        if ( iImsiReaderStatus == ERequestingImsi )
            {
            iImsi = iSubscriberIdV1.iSubscriberId;
            LOGGER_WRITE_1("iImsi: %S", &iImsi);
            }
        else if ( iImsiReaderStatus == ERequestingNetworkStatus )
            {
            iNetworkStatus = iNetworkRegistrationV1.iRegStatus;
            LOGGER_WRITE_1( "iNetworkStatus: %d", (TInt)iNetworkStatus);
            }
        }
    CActiveScheduler::Stop();
    TRACE_FUNC_EXIT;
    }
 
void CSconImsiReader::DoCancel()
    {
    TRACE_FUNC_ENTRY;
    if ( iImsiReaderStatus == ERequestingImsi )
        {
        iTelephony->CancelAsync( CTelephony::EGetSubscriberIdCancel );
        }
    else if ( iImsiReaderStatus == ERequestingNetworkStatus )
        {
        iTelephony->CancelAsync( CTelephony::EGetNetworkRegistrationStatusCancel );
        }
    TRACE_FUNC_EXIT;
    }
