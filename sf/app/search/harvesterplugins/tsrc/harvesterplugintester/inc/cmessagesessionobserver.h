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

#ifndef CMESSAGESESSIONOBSERVER_H_
#define CMESSAGESESSIONOBSERVER_H_

#include <msvapi.h>

class CMessageSessionObserver : public CBase, public MMsvSessionObserver 
{
public:
    static CMessageSessionObserver* NewL();
    static CMessageSessionObserver* NewLC();
    virtual ~CMessageSessionObserver();
private:
    void ConstructL();
    CMessageSessionObserver();
    /**
     * callback from MMsvSessionObserver
     */
    virtual void HandleSessionEventL(TMsvSessionEvent aEvent, 
                                         TAny* aArg1, 
                                         TAny* aArg2, 
                                         TAny* aArg3);
};

class MessagingUtils
{
public:
    static TInt CreateMessageL(CMsvSession* aMsgSession, 
                               const TDesC& aMsgFrom,
                               const TDesC& aMsgTo,
                               const TDesC& aMsgBody
                               );
    static TInt RemoveEntryL(CMsvSession* aMsgSession,TMsvId aMsgId);
    
    static TInt CreateMmsMessageL(CMsvSession* aMsgSession, 
                                  const TDesC& aMsgFrom,
                                  const TDesC& aMsgTo,
                                  const TDesC& aMsgBody);
    
    static TInt RemoveMmsEntryL(CMsvSession* aMsgSession,TMsvId aMsgId);
    
    static TInt CreateEmailEntryL(CMsvSession* aMsgSession );
};

class CMessAsyncWaiter: public CActive
{
public:    
     static CMessAsyncWaiter* NewL();
     virtual ~CMessAsyncWaiter();
     void StartAndWait();
     TInt Result() const;
     void DoCancel();
     
private:
     CMessAsyncWaiter();
     void RunL();
     TInt iError;
};
 
#endif /* CMESSAGESESSIONOBSERVER_H_ */
