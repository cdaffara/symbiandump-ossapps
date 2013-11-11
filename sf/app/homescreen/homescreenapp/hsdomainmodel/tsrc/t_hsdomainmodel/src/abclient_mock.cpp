// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Mocked implementation of CActiveBackupClient class.
//
//


#include <QtGlobal>
#ifdef Q_OS_SYMBIAN

#include "abclient.h"

namespace conn
    {

    CActiveBackupClient::CActiveBackupClient() : iClientSession(NULL), iABCallbackHandler(NULL)
    {
    }


    EXPORT_C CActiveBackupClient* CActiveBackupClient::NewL()
    {
        CActiveBackupClient* self = new (ELeave) CActiveBackupClient();
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);
        return self;
    }

    EXPORT_C CActiveBackupClient* CActiveBackupClient::NewL(MActiveBackupDataClient* aClient)
    {
        CActiveBackupClient* self = new (ELeave) CActiveBackupClient();
        CleanupStack::PushL(self);
        self->ConstructL(aClient);
        CleanupStack::Pop(self);
        return self;
    }

    void CActiveBackupClient::ConstructL()
    {
    }

    void CActiveBackupClient::ConstructL(MActiveBackupDataClient* aClient)
    {
        ConstructL();
        Q_UNUSED(aClient);
    }

    EXPORT_C CActiveBackupClient::~CActiveBackupClient()
    {
    }

    EXPORT_C void CActiveBackupClient::BURModeInfoL(TDriveList& aDriveList, TBURPartType& aBackupType, TBackupIncType& aIncBackupType)
    {
        Q_UNUSED(aDriveList);
        Q_UNUSED(aBackupType);
        Q_UNUSED(aIncBackupType);
    }

    EXPORT_C TBool CActiveBackupClient::DoesPartialBURAffectMeL()
    {
        return EFalse;
    }

    EXPORT_C void CActiveBackupClient::ConfirmReadyForBURL(TInt aErrorCode)
    {
        Q_UNUSED(aErrorCode);
    }

} // end of conn namespace

#endif
