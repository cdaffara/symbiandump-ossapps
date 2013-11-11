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
* Description:
*
*/
#include "irqnwkinfoobserver_p.h"
#include "irqnwkinfoobserver.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserverPrivate::IRQNwkInfoObserverPrivate(IRQNwkInfoObserver *aObserver) : CActive(EPriorityLow),
                                                 iTelephony(NULL),
                                                 iCurrentNetworkInfoV1Pckg(iCurrentNetworkInfo),
                                                 iHomeOperatorInfoPckg(iHomeOperatorInfo),
                                                 iFirstTime(true),
                                                 iCurrentNetworkType(true),
                                                 iHomeNetworkType(false),
                                                 q_ptr(aObserver)
{
}

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
bool IRQNwkInfoObserverPrivate::Construct()
{
    bool br = false;
    TInt leaveCode;
    
    // Get singleton instance
    TRAP(leaveCode, iTelephony = CTelephony::NewL());
    if(KErrNone == leaveCode)
    {
        CActiveScheduler::Add(this);
        br = true;
    }
   
    return br;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserverPrivate::~IRQNwkInfoObserverPrivate()
{
    Cancel();
    delete iTelephony;
    iTelephony = NULL;
}

// ---------------------------------------------------------------------------
// Function : IRQNwkInfoObserverPrivate::startNwkInfoMonitor()
// Function to start network monitoring
// ---------------------------------------------------------------------------
//

void IRQNwkInfoObserverPrivate::startNwkInfoMonitor()
{
    if(!IsActive() && iTelephony)
    {
        //get the current network info for the first time
        iCurrentNetworkType = true;
        iHomeNetworkType = false;
        iTelephony->GetCurrentNetworkInfo(iStatus, iCurrentNetworkInfoV1Pckg);      
        SetActive(); 
    }  
}

// ---------------------------------------------------------------------------
// Function : IRQNwkInfoObserverPrivate::issueRequest()
// Issue Subsequent network monitoring request
// ---------------------------------------------------------------------------
//

void IRQNwkInfoObserverPrivate::issueRequest()
{
    if(iCurrentNetworkType)
    {
        if(!IsActive() && iTelephony)
        {
            //notify if current operator changes
            iTelephony->NotifyChange(iStatus,
                    CTelephony::ECurrentNetworkInfoChange,
                    iCurrentNetworkInfoV1Pckg);
            SetActive();
        }      
    }
    
    if(iHomeNetworkType)
    {
        if(!IsActive() && iTelephony)
        {
            //notify if home operator changes
            iTelephony->NotifyChange(iStatus,
                      CTelephony::ENetworkRegistrationStatusChange,
                      iHomeOperatorInfoPckg);
            SetActive();
        }  
    }
}


// ---------------------------------------------------------------------------
// From CActive.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserverPrivate::DoCancel()
{
    if(IsActive() && iTelephony)
    {
        //cancel the out standing requests if any
        iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
        iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);  
        iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
        iTelephony->CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
    }
}

// ---------------------------------------------------------------------------
// From CActive.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserverPrivate::RunL()
{  
    if(KErrNone == iStatus.Int())
    {    
        if(iCurrentNetworkType)
        {
            //if the request was to find the current network changes
            //then the value is updated
          #ifdef __WINS__
            iCurrentNwkMCC = "0";
            iCurrentNwkMNC = "0";  
          #else
            iCurrentNwkMCC = QString::fromUtf16( iCurrentNetworkInfo.iCountryCode.Ptr(),
                                            iCurrentNetworkInfo.iCountryCode.Length());
            iCurrentNwkMNC = QString::fromUtf16( iCurrentNetworkInfo.iNetworkId.Ptr(),
                                            iCurrentNetworkInfo.iNetworkId.Length());
          #endif
            //updating mcc+mnc value
            if(q_ptr)
            {
                emit q_ptr->currentNwkChanged(iCurrentNwkMCC,iCurrentNwkMNC);
            }
            
            iHomeNetworkType = ETrue;
            iCurrentNetworkType = EFalse;
            if(iFirstTime && iTelephony)
            {
                //if first time we have check the current operator is home operator
                iTelephony->GetNetworkRegistrationStatus(iStatus,iHomeOperatorInfoPckg);      
                SetActive();
                iFirstTime = EFalse;
            }
            else
            {
                //else issue request is called for notifying home operator change
                issueRequest();  
            }      
        }
        else if(iHomeNetworkType)
        {
            //home operator (or the change in home operator is notified. 
          #ifdef __WINS__
            iHomeNetworkMCC = "0";
            iHomeNetworkMNC = "0";
            if(q_ptr)
            {            
                emit q_ptr->homeNwkChanged(iHomeNetworkMCC,iHomeNetworkMNC);
            }
          #else
            //if registration status shows that user is registered in home network
            //and not in roaming status, the current operator is updated
            if(CTelephony::ERegisteredOnHomeNetwork == iHomeOperatorInfo.iRegStatus)
            {
                iHomeNetworkMCC = iCurrentNwkMCC;
                iHomeNetworkMNC = iCurrentNwkMNC;
                if(q_ptr)
                {                    
                    emit q_ptr->homeNwkChanged(iHomeNetworkMCC,iHomeNetworkMNC);
                }
            }
          #endif
                //issue request to find the current network change
                iHomeNetworkType = EFalse;
                iCurrentNetworkType = ETrue;
                issueRequest();

        }    
        else
        {
            return;  
        }
    }
    //if any error network is updated to zero    
    else
    {
        iCurrentNwkMCC = "0";
        iCurrentNwkMNC = "0";  
        iHomeNetworkMCC = "0";
        iHomeNetworkMNC = "0";
        if(q_ptr)
        {
            emit q_ptr->currentNwkChanged(iCurrentNwkMCC,iCurrentNwkMNC);
            emit q_ptr->homeNwkChanged(iHomeNetworkMCC,iHomeNetworkMNC);
        }
    }
}
