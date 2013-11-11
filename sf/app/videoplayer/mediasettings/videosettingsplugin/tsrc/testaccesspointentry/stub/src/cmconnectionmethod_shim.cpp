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
* Description:   CmConnectionMethodShim stub
* 
*/

// INCLUDE FILES
#include "cmconnectionmethod_shim.h"
#include <qlist.h>
#include <qvariant.h>

// ---------------------------------------------------------------------------
// CmConnectionMethodShim
// ---------------------------------------------------------------------------
//
CmConnectionMethodShim::CmConnectionMethodShim(uint id, uint iapId, QString name) 
{
    mAttributes[CMManagerShim::CmId] = id;
    mAttributes[CMManagerShim::CmIapId] = iapId;
    mAttributes[CMManagerShim::CmName] = name;
}

// ---------------------------------------------------------------------------
// CmConnectionMethodShim
// ---------------------------------------------------------------------------
//
CmConnectionMethodShim::~CmConnectionMethodShim()
{
}

// ---------------------------------------------------------------------------
// getIntAttribute
// ---------------------------------------------------------------------------
//
uint CmConnectionMethodShim::getIntAttribute(
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    return mAttributes[attribute].toUInt();
}

// ---------------------------------------------------------------------------
// getStringAttribute
// ---------------------------------------------------------------------------
//
QString CmConnectionMethodShim::getStringAttribute(
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    return mAttributes[attribute].toString();
}
