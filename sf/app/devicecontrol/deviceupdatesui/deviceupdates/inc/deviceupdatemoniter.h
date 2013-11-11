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

#ifndef DEVICEUPDATEMONITER_H
#define DEVICEUPDATEMONITER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <centralrepository.h>
#include "deviceupdatemoniterobserver.h"
#include <e32cmn.h>
// CLASS DECLARATION

/**
 *  CClassName
 * 
 */
#include <e32property.h>

#define  KOmaDMAppUid   0x101F6DE5

//static const TUid KOmaDMAppUid = { 0x101F6DE5 };

const TUint32 KFotaServerActive = 0x0000008;


//#include <hb/hbcore/hbsymbiandevicedialog.h>    // MDeviceMoniterObserver

class  CDeviceUpdateMoniter  
        :
        public CActive
        //public MDeviceMoniterObserver
        //: public QObject, public MHbDeviceDialogObserver
    {
public: // new functions
    static CDeviceUpdateMoniter* NewL (MDeviceMoniterObserver* aObserver);
    
    CDeviceUpdateMoniter();
    
    ~CDeviceUpdateMoniter();
    
    void StartMoniter();
    
    void ConstructL();
    
public:
    /**
     * Handle user action
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    virtual void RunL();


     /**
     * Handle leaving Runl
     *
     * @since   S60   v3.1
     * @param   aError  error code
     * @return  Error code
     */
    virtual TInt RunError(TInt aError);


     /**
     * Handle cancel of active object
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void DoCancel();
    
    
    TInt CancelRequest();

    
private:

    //MDeviceMoniterObserver* iDownload;
    MDeviceMoniterObserver * iObserver;
    
    RProperty iProperty;
    
    };

#endif // DEVICEUPDATEMONITER_H
