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
* Description:  CSconOviAccountHandler header file
*
*/


#ifndef _SCONOVIACCOUNTHANDLER_H_
#define _SCONOVIACCOUNTHANDLER_H_

#include <e32base.h>
#include <s32strm.h>

_LIT(KSconOviAccountHandlerDllName, "sconoviaccounthandler.dll");
const TUid KSconOviAccountHandlerDllUid={0x10009D8D};

class CWSOviAccountManager;

/**
 * CSconOviAccountHandler provides Ovi account information.
 * Dynamically loadable dll.
 */
class CSconOviAccountHandler : public CBase
{
    public:
    static CSconOviAccountHandler* NewL();
    ~CSconOviAccountHandler();
    
    /**
     * Reads account information from device
     * @param aAccountInfoStream stream where to export account details.
     * @leave KErrNotFound if account information does not exist.
     */
    virtual void GetOviAccountDetailsL( RWriteStream& aAccountInfoStream );
    
    private:
    void ConstructL();
    
    private: // Data
    CWSOviAccountManager* iOviAccountManager;
    
};

IMPORT_C CSconOviAccountHandler* CreateCSconOviAccountHandlerL();
typedef CSconOviAccountHandler* (*TSConCreateCSconOviAccountHandlerFunc) (); 

#endif // _SCONOVIACCOUNTHANDLER_H_
