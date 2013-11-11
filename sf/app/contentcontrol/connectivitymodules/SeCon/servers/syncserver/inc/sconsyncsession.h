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
* Description:  CSconSyncSession header
*
*/


#ifndef __SCONSYNCSESSION_H__
#define __SCONSYNCSESSION_H__

#include <e32base.h>


class CSconSyncServer;
class CSconAsyncHandler;

class CSconSyncSession : public CSession2
    {
public:
    static CSconSyncSession* NewL();
    
    void CreateL();
    
private:
    CSconSyncSession();
    ~CSconSyncSession();
    void ConstructL();
    
    CSconSyncServer& Server();
    void ServiceL(const RMessage2& aMessage);
    
private:
    CSconAsyncHandler* iAsyncHandler;
    };

#endif // __SCONSYNCSESSION_H__
