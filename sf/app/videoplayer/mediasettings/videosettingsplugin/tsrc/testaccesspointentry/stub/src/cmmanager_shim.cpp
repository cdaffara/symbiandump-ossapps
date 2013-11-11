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
* Description:   CmManagerShim stub
* 
*/

// INCLUDE FILES
#include "cmmanager_shim.h"
#include "cmconnectionmethod_shim.h"
#include <qlist.h>

int CmManagerShim::mDestructorCount = 0;
int CmManagerShim::mConnectionMethodThrowsAtId = -1;

// ---------------------------------------------------------------------------
// CmManagerShim
// ---------------------------------------------------------------------------
//
CmManagerShim::CmManagerShim() 
{
}

// ---------------------------------------------------------------------------
// CmManagerShim
// ---------------------------------------------------------------------------
//
CmManagerShim::~CmManagerShim()
{
    mDestructorCount++;
    clear();
}

// ---------------------------------------------------------------------------
// connectionMethod
// ---------------------------------------------------------------------------
//
CmConnectionMethodShim* CmManagerShim::connectionMethod(uint connectionMethodId) const
{
    if(mConnectionMethodThrowsAtId == connectionMethodId)
    {
        throw std::exception();
    }
    
    return mConnectionMethods.value(connectionMethodId, 0);
}

// ---------------------------------------------------------------------------
// connectionMethod
// ---------------------------------------------------------------------------
//
void CmManagerShim::connectionMethod(
    QList<uint> &cmArray,
    bool legacyOnly) const
{
    Q_UNUSED(legacyOnly);
    cmArray.append(mConnectionMethods.keys());
}

// ---------------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------------
//
void CmManagerShim::clear()
{
    foreach(uint key, mConnectionMethods.keys())
    {
        delete mConnectionMethods[key];
        mConnectionMethods[key] = 0;
    }
    
    mConnectionMethods.clear();
}
