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
* Description:  CSconSyncServer header
*
*/


#ifndef __SCONSYNCSERVER_H__
#define __SCONSYNCSERVER_H__

#include <e32base.h>

class CSconSyncServer : public CPolicyServer
{
public:
    static CSconSyncServer* NewL();
    static CSconSyncServer* NewLC();      
    virtual ~CSconSyncServer();
    
    void AddSession();
    void RemoveSession();

private:
    CSconSyncServer();
    void ConstructL();

    CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
    TInt RunError(TInt aError);

private:
    TInt iSessionCount;

};

#endif // __SCONSYNCSERVER_H__