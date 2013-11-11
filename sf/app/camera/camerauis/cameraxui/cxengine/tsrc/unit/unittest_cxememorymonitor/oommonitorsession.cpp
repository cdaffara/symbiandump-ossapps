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
* Description: Fake ROomMonitorSession class implementation
*
*/

#include "oommonitorsession.h"

TInt ROomMonitorSession::Connect()
{
    return KErrNone;
}

TInt ROomMonitorSession::RequestFreeMemory(TInt /*aBytesRequested*/)
{
    return KErrNone;
}

void ROomMonitorSession::RequestFreeMemory(TInt /*aBytesRequested*/, TRequestStatus& /*aStatus*/)
{
    // do nothing
}

TInt ROomMonitorSession::RequestOptionalRam(TInt /*aBytesRequested*/, TInt /*aMinimumBytesNeeded*/, TInt /*aPluginId*/, TInt& /*aBytesAvailable*/)
{
    return KErrNone;
}

void ROomMonitorSession::RequestOptionalRam(TInt /*aBytesRequested*/, TInt /*aMinimumBytesNeeded*/, TInt /*aPluginId*/, TRequestStatus& /*aStatus*/)
{
    // do nothing
}

void ROomMonitorSession::CancelRequestFreeMemory()
{
    // do nothing
}

void ROomMonitorSession::ThisAppIsNotExiting(TInt /*aWgId*/)
{
    // do nothing
}

void ROomMonitorSession::SetOomPriority(TOomPriority /*aPriority*/)
{
    // do nothing
}

void ROomMonitorSession::Close()
{
    // do nothing
}

//End of file
