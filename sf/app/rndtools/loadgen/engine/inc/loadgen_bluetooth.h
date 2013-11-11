/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef LOADGEN_BLUETOOTH_H
#define LOADGEN_BLUETOOTH_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <centralrepository.h>
#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"
#include <es_sock.h>
#include <bt_sock.h>

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

class CBluetooth : public CLoadBase
    {
public:
    static CBluetooth* NewL(TBluetoothAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CBluetooth();
        
private: // Constructors
    CBluetooth(TBluetoothAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TBluetoothAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TBluetoothAttributes& aAttributes) { iAttributes = aAttributes; }
    
public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);
    
private:  // New static methods
    static void GenerateLoad(TBluetoothAttributes& aAttributes);    
    void InitializeBluetoothL();
    TBool SetBTPowerState(TBool aState);
    
private: // Data
           
    TBluetoothAttributes    iAttributes;    
    RThread                 iThread; 
    };
 


 
class CBTManager : public CActive//, public MBTPowerManagerObserver
    {
    public:
        
        static CBTManager* NewL(TBluetoothAttributes& aAttrs);
        virtual ~CBTManager();
 
    private:
        
        CBTManager( TBluetoothAttributes& aAttrs);
        void ConstructL();
        TInt StartBTDeviceDiscovery();
 
    private:
            
        static TInt PeriodicTimerCallBack(TAny* aAny);        
    public:
                                                        
        /**
         * From CActive:
         */
        void RunL();
        void DoCancel();       

    private:
        
        CPeriodic*              iPeriodicTimer;
        TBluetoothAttributes&   iAttributes;  
        CRepository*            iBTPowerStateCRepo;
        RSocketServ             iSocketServerHnd;
        RHostResolver           iHostResolver;        
        TNameEntry              iBTNameEntry;        
    };


#endif

