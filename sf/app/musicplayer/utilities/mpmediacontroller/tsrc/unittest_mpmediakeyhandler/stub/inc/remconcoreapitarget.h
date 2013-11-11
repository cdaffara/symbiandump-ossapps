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
* Description: remconcoreapitarget stub for testing mpmediakeyhandler
*
*/


#ifndef REMCONCOREAPITARGET_H
#define REMCONCOREAPITARGET_H


#include <remconcoreapi.h>  // TRemConCoreApiOperationId 


class CRemConInterfaceSelector;
class MRemConCoreApiTargetObserver;

class CRemConCoreApiTarget : public CActive
    {

public:
    
    static int getCount();
    static void setNewLLeave();
    static void NewLLeaveIfDesiredL();
    static int getSendResponseCount();
    
public:

    static CRemConCoreApiTarget* NewL(CRemConInterfaceSelector& aInterfaceSelector, MRemConCoreApiTargetObserver& aObserver);
    virtual ~CRemConCoreApiTarget();

    void PausePlayFunctionResponse(TRequestStatus& aStatus, TInt aError);
    void PlayResponse(TRequestStatus& aStatus, TInt aError);
    void StopResponse(TRequestStatus& aStatus, TInt aError);
    void PauseResponse(TRequestStatus& aStatus, TInt aError);
    void RewindResponse(TRequestStatus& aStatus, TInt aError);
    void FastForwardResponse(TRequestStatus& aStatus, TInt aError);
    void BackwardResponse(TRequestStatus& aStatus, TInt aError);
    void ForwardResponse(TRequestStatus& aStatus, TInt aError);
    void VolumeUpResponse(TRequestStatus& aStatus, TInt aError);
    void VolumeDownResponse(TRequestStatus& aStatus, TInt aError);
    void SendResponse(TRequestStatus& aStatus, TRemConCoreApiOperationId aOperationId, TInt aError);

protected:

    // from CActive
    virtual void DoCancel();
    virtual void RunL();
    
private:
    
    CRemConCoreApiTarget();
    
private:

    TRequestStatus*             iClientStatus;
    
    };

#endif      // REMCONCOREAPITARGET_H

// End of File
