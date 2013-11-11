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
* Description:  Imsi reader
*
*/


#ifndef __SCONIMSIREADER_H__
#define __SCONIMSIREADER_H__

#include <e32base.h>
#include <Etel3rdParty.h>

enum TImsiReaderStatus
    {
    EIdle,
    ERequestingImsi,
    ERequestingNetworkStatus
    };
 
class CSconImsiReader : public CActive
    {
    public:
        
        static TInt GetImsiL( TDes& aImsi );
        
        static TInt GetNetworkStatusL( CTelephony::TRegistrationStatus& aRegistrationStatus );
 
    private:
        CSconImsiReader( TDes& aImsi, CTelephony::TRegistrationStatus& aRegistrationStatus );
        void ConstructL();
        ~CSconImsiReader();
        
        void RequestImsi();
        void RequestNetworkStatus();
        
    private: // From CActive
        
        void RunL();
        void DoCancel();
        
    private: // data
        TDes& iImsi;
        CTelephony::TRegistrationStatus& iNetworkStatus;
        CTelephony* iTelephony;
        CTelephony::TSubscriberIdV1 iSubscriberIdV1;
        CTelephony::TSubscriberIdV1Pckg iSubscriberIdV1Pckg;
        
        CTelephony::TNetworkRegistrationV1     iNetworkRegistrationV1;
        CTelephony::TNetworkRegistrationV1Pckg iNetworkRegistrationV1Pckg;
        TImsiReaderStatus iImsiReaderStatus;
    };

#endif // __SCONIMSIREADER_H__
