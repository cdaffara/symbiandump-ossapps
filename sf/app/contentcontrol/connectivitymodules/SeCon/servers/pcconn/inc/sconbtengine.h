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
* Description:  CSconBtEngine declaration
*
*/



#ifndef SCONBTENGINE_H_
#define SCONBTENGINE_H_

#include <e32std.h>
#include <e32base.h>
#include <s32mem.h>
#include <BTEngDevMan.h>
#include <btengsettings.h>

const TInt KCenRepAddrBufSize = 2 * KBTDevAddrSize;

class CSconBtEngine : public CBase,
                      public MBTEngDevManObserver,
                      public MBTEngSettingsObserver
    {
public:
    static CSconBtEngine* NewL();
    ~CSconBtEngine();
    
    void ReadBTInfoL( RBufWriteStream& aStream );
    
    void SetBtAuthorizedL( const TDesC& aBtDevAddress, TBool aAuthorized );
    
    TInt SetBtPowerState( TBool aPower );
    
    TInt SetBtName( const TDesC& aBtName );
    
protected:
    // From MBTEngDevManObserver
    void HandleDevManComplete( TInt aErr );
    void HandleGetDevicesComplete( TInt aErr, 
           CBTDeviceArray* aDeviceArray );
    
    // From MBTEngSettingsObserver
    void PowerStateChanged( TBTPowerStateValue aState );
    void VisibilityModeChanged( TBTVisibilityMode aState );
    
private:
    CSconBtEngine();
    void ConstructL();
    
    void PrintDevicesL();
    
private:
    enum TState
        {
        EIdle,
        ERequestBtInfo,
        EChangeBtState,
        EChangeAuthorized
        };
    TState iState;
    TInt   iOperationError;
    CActiveSchedulerWait    iWait;
    RBufWriteStream*        iStream;
    CBTEngDevMan*           iBtEng;
    CBTDeviceArray*         iBtDevArray;
    CBTEngSettings*         iBtSettings;
    TBool                   iAuthorizedValue;
    TBuf<KCenRepAddrBufSize> iBtAddress;
    };

#endif /* SCONBTENGINE_H_ */
