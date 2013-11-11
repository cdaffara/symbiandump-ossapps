/*
 * Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
 *     An active object class which takes care of reading the possible new
 *     service centres from SIM and adds them to Sms Settings.
 *
 */

#ifndef __SIMSCNUMBERDETECTOR_H__
#define __SIMSCNUMBERDETECTOR_H__

//  INCLUDES
#include <msvapi.h>     // for MsvSession
#include <smsclnt.h>    // for CSmsClientMtm
#include <e32base.h>
#include <smutsimparam.h>   // for CSmsSimParamOperation

// CONSTANTS
const TInt KMaxNameLength = 60;
const TInt KSmumGranularity = 4;
const TInt KSmumRetryCount = 20;
const TInt KSmumRetryDelay = 5000000; // 5 secs
const TInt KSmumSMSPid = 50;
const TInt KTestStringLength = 100;
const TInt KDefDeliveryReport = 1;
const TInt KDefValidityPeriod = 1440;
const TInt KDefMessageConversion = 0;
const TInt KDefPreferredConnection = 3;
const TInt KDefReplyViaSameCentre = 0;

// CONSTANTS
_LIT( KSmsService,         "Sms Service" );
_LIT( KSmscDefaultName,    "New service centre");
_LIT( KSmscSimDefaultName, "New SIM service centre");
_LIT( KFmtSpec,            "%S(%02d)");

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CStartUpMonitor;
class CRepository;
class COutboxObserver;

/**
 * Observer to indicate when SIM operation has been completed.
 */
class MSimOperationObserver
{
public:
    virtual void CompleteOperation()=0;
};

/**
 *  CMsgSimOperation
 *  Inherited from CBase, MMsvSessionObserver
 */
class CMsgSimOperation : public CBase, public MMsvSessionObserver
{
public:
    
    /**
     * Constructor.
     */
    IMPORT_C static CMsgSimOperation* NewL(MSimOperationObserver& aObserver);

    /**
     * Destructor
     */
    ~CMsgSimOperation();

public:
    

    /**
     * Callback function for startup state monitor object
     */
    void HandleStartupReadyL();

private:
    
    /**
     * C++ constructor which initializes the active object with a 
     * default priority and adds it to the active scheduler.
     * @param TRequestStatus& aStatus, TInt aPriority
     * @return nothing
     */
    CMsgSimOperation(MSimOperationObserver& aObserver);

    /**
     * This function simulates issuing request, activates the object 
     * and starts to open Msv session
     * @param none
     * @return nothing
     */
    void StartL();

    /**
     * Complete the client's active object request status
     * @param TInt aValue
     * @return nothing
     */
    
    void CompleteClientRequest(TInt aValue);
    
    /**
     * Checks if the device has SIM when phone boots
     * @reutrn bool
     */  
    TBool IsSIMPresent();

    /**
     * Checks if the SIM has been changed in the next boot
     * @reutrn bool
     */
    TBool HasSIMChanged();

    /**
     * Checks if already SMSC setting is there
     * @reutrn bool
     */
    TBool HasNoSmscSettings();
    
    /**
     * Gets default name for SMSC, if SIM does not have one 
     */
    void FindNameForSCL(TDes& aName, TBool aNameIsForSCentreFromSIM,
                               CDesCArrayFlat* aSCNamesList);

    /**
     * Creates a SMS service, if not there
     */
    TMsvId CreateSmsServiceL();
    
    /**
     * Creates default settings for the SMS account
     */
    void CreateDefaultSettingsL(TMsvId aServiceId);

    /**
     * Reads default settings from the Cenrep
     */
    void ReadDefaultSettingsFromSharedDataL(CSmsSettings* aServiceSettings);
    
    /**
     * Gets the Service Id 
     */
    TMsvId ServiceIdL();
     
private:

    /**
     *
     */
    enum TSimOperationPanic
    {
        ESimOperationPanicRequestAlreadyActive = 0,
        ESimOperationPanicClientsRequestAlreadyCompleted
    };

    /**
     * Panic the thread
     * @param TSimOperationPanic aPanic
     * @return nothing
     */
    static void Panic(TSimOperationPanic aPanic);

private:

    void StartRunL(TInt aError);
    
    void DoStartRunL();

    /**
     * From MMsvSessionObserver 
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

private:
    // Data
    CSmsSimParamOperation* iSimOperation;
    CMsvSession* iMsvSession;
    CSmsClientMtm* iSmsClientMtm;
    CClientMtmRegistry* iClientRegistry;
    TInt iRetryCount;
    CStartUpMonitor* iStartupMonitor;
    MSimOperationObserver& iObserver;
    CRepository* iCenRepSession;
    COutboxObserver* iOutBoxObserver;

};

#endif  // not defined __SIMSCNUMBERDETECTOR_H__
// End of file
