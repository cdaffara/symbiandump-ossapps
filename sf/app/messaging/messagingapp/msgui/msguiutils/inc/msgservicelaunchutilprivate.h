/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef MSGSERVICELAUNCHUTILPRIVATE_H_
#define MSGSERVICELAUNCHUTILPRIVATE_H_


#include <msvapi.h>


class CMsgServiceLaunchUtilPrivate: 
public CBase, public MMsvSessionObserver
    {    
public:
    static CMsgServiceLaunchUtilPrivate* NewL();
    ~CMsgServiceLaunchUtilPrivate();
    HBufC* GetMessagePath(TInt aMessageId, TInt aError);
    HBufC8* GetMimeType();

private: // From MMsvSessionObserver
    void HandleSessionEventL(TMsvSessionEvent aEvent, 
            TAny* aArg1, TAny* aArg2, TAny* aArg3);
    
private:
    void GetMessagePathL(TPtr aMsgPath, const TInt aMessageId);
    void StoreMessageMimeTypeL(TPtr aMsgPath);
       
private:
    CMsgServiceLaunchUtilPrivate();
    void ConstructL();
    
private:
    CMsvSession* iMsvSession;
    HBufC8*      iMimeType;
    };

#endif /* MSGSERVICELAUNCHUTILPRIVATE_H_ */

