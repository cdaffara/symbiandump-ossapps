/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CSettingsData stores the settings for ConnTest
*
*/

#ifndef SETTINGDATA_H
#define SETTINGDATA_H

// INCLUDES
#include <e32base.h>
#include "conntest.hrh"

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
* Class for encapsulating setting data.
*/
class CSettingData : public CBase
{
public:
    static CSettingData* NewL();
    ~CSettingData();
    
private:
    CSettingData();
    void ConstructL();
    
public:
    TBuf<KMaxServerNameLength> iServerName;
    TInt iPort;
    TInt iProtocol;
    TInt iPacketSize;
    TInt iPackets;
    TInt iDelay;
    TInt iRoaming;
    TBuf<KMaxServerNameLength> iHttpPage;
};


#endif

// End of File
