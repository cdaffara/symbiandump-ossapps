/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class declaration for monitoring current network info change
*
*/


#ifndef IRQNWKINFOOBSERVER_P_H_
#define IRQNWKINFOOBSERVER_P_H_


#include <e32base.h> // CActive
#include <etel3rdparty.h> // CTelephony
#include <QString>

class IRQNwkInfoObserver;

class IRQNwkInfoObserverPrivate : public CActive
{
public:
 
    /*!
     * Constructor
     */
    explicit IRQNwkInfoObserverPrivate(IRQNwkInfoObserver *aObserver);
    
    /**
     * Second-phase constructor.
     */
    bool Construct();
    
    /*!
     * Destructor.
     */
    ~IRQNwkInfoObserverPrivate();
    
    /*!
     * starts network info monitoring
     */
    void startNwkInfoMonitor();    

private:


private:
  
    /*!
     * Issue Subsequent network monitoring request
     */
    void issueRequest();

    /*!
     * From CActive.
     */
    void DoCancel();
    void RunL();

private:  

    CTelephony* iTelephony;
    
    //Current Network
    CTelephony::TNetworkInfoV1 iCurrentNetworkInfo;
    CTelephony::TNetworkInfoV1Pckg iCurrentNetworkInfoV1Pckg;
  
    //Home Operator
    CTelephony::TNetworkRegistrationV1 iHomeOperatorInfo;
    CTelephony::TNetworkRegistrationV1Pckg iHomeOperatorInfoPckg;  
      
    QString iCurrentNwkMCC;
    QString iCurrentNwkMNC;
    
    QString iHomeNetworkMCC;
    QString iHomeNetworkMNC;    

    bool iFirstTime;
    bool iCurrentNetworkType;
    bool iHomeNetworkType;
  
    IRQNwkInfoObserver *q_ptr;
};

#endif // IRQNWKINFOOBSERVER_P_H_
