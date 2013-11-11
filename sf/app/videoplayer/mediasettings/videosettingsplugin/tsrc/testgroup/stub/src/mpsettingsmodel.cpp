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
* Description:   MPSettingsModel stub
* 
*/

// INCLUDE FILES
#include "stub/inc/mpsettingsmodel.h"
#include <qglobal.h>
#include <xqconversions.h>

TUid CMPSettingsModel::mImplUid(TUid::Null());
TInt CMPSettingsModel::mConfigMode(-1);
TInt CMPSettingsModel::mProxyMode(-1);
QString CMPSettingsModel::mHostName = QString();
TInt CMPSettingsModel::mProxyPort(-1);
TInt CMPSettingsModel::mMinUdpPort(-1);
TInt CMPSettingsModel::mMaxUdpPort(-1);
int CMPSettingsModel::mStoreSettingsCount(0);
int CMPSettingsModel::mDestructorCount(0);
int CMPSettingsModel::mApId(0);

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CMPSettingsModel* CMPSettingsModel::NewL(TUid aUid)
{
    mImplUid = aUid;
    return new CMPSettingsModel;
}

// ---------------------------------------------------------------------------
// CMPSettingsModel
// ---------------------------------------------------------------------------
//
CMPSettingsModel::CMPSettingsModel()
{
}

// ---------------------------------------------------------------------------
// ~CMPSettingsModel
// ---------------------------------------------------------------------------
//
CMPSettingsModel::~CMPSettingsModel()
{
    mDestructorCount++;
}

// ---------------------------------------------------------------------------
// LoadSettingsL
// ---------------------------------------------------------------------------
//
void CMPSettingsModel::LoadSettingsL(TInt aConfigVersion)
{
    mConfigMode = aConfigVersion;
}

// ---------------------------------------------------------------------------
// StoreSettingsL
// ---------------------------------------------------------------------------
//
void CMPSettingsModel::StoreSettingsL()
{
    mStoreSettingsCount++;
}
        
// ---------------------------------------------------------------------------
// SetProxyMode
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::SetProxyMode(TInt aMode)
{
    mProxyMode = aMode;
    return 0;
}
        
// ---------------------------------------------------------------------------
// GetProxyMode
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::GetProxyMode(TInt& aMode)
{
    aMode = mProxyMode;
    return 0;
}

// ---------------------------------------------------------------------------
// SetProxyHostNameL
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::SetProxyHostNameL(const TDesC& aHostName)
{
    mHostName = XQConversions::s60DescToQString(aHostName);
    return 0;
}
        
// ---------------------------------------------------------------------------
// GetProxyHostName
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::GetProxyHostName(TDes& aHostName)
{
    HBufC* name = XQConversions::qStringToS60Desc(mHostName);
    aHostName.Append(*name);
    delete name;
    return 0;
}

// ---------------------------------------------------------------------------
// SetProxyPort
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::SetProxyPort(TInt aPort)
{
    mProxyPort = aPort;
    return 0;
}
        
// ---------------------------------------------------------------------------
// GetProxyPort
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::GetProxyPort(TInt& aPort)
{
    aPort = mProxyPort;
    return 0;
}
        
// ---------------------------------------------------------------------------
// SetDefaultAp
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::SetDefaultAp(TUint32 aApId)
{
    mApId = aApId;
    return 0;
}

// ---------------------------------------------------------------------------
// GetDefaultAp
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::GetDefaultAp(TUint32& aApId)
{
    aApId = mApId;
    return 0;
}

// ---------------------------------------------------------------------------
// SetMinUDPPort
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::SetMinUDPPort(TInt aPort)
{
    mMinUdpPort = aPort;
    return 0;
}

// ---------------------------------------------------------------------------
// GetMinUDPPort
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::GetMinUDPPort(TInt& aPort)
{
    aPort = mMinUdpPort;
    return 0;
}

// ---------------------------------------------------------------------------
// SetMaxUDPPort
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::SetMaxUDPPort(TInt aPort)
{
    mMaxUdpPort = aPort;
    return 0;
}

// ---------------------------------------------------------------------------
// GetMaxUDPPort
// ---------------------------------------------------------------------------
//
TInt CMPSettingsModel::GetMaxUDPPort(TInt& aPort)
{
    aPort = mMaxUdpPort;
    return 0;
}
