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


#ifndef TESTSIMNUMDETECTOR_STUB_H_
#define TESTSIMNUMDETECTOR_STUB_H_

//  INCLUDES
#include <msvapi.h>     // for MsvSession

// FORWARD DECLARATIONS
class CMsvSession;

/**
 *  CTestMsgSimOperation
 *  Inherited from CBase, MMsvSessionObserver
 */
class CTestMsgSimOperation : public MMsvSessionObserver
{
public:
    
    /**
     * Constructor.
     */
    IMPORT_C static CTestMsgSimOperation* NewL();

    /**
     * Destructor
     */
    ~CTestMsgSimOperation();

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
	CTestMsgSimOperation();
    
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();
    
public:
    //data
    CMsvSession* iMsvSession;

};

#endif /* TESTSIMNUMDETECTOR_STUB_H_ */
