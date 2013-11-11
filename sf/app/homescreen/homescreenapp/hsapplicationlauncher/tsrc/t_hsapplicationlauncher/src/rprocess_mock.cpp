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

#include "e32std.h"

TInt RProcess::Create(const TDesC& aFileName,const TDesC& aCommand,TOwnerType aType)
{
    return KErrNone;
}

TInt RProcess::Open(const TDesC &aName,TOwnerType aType)
{
    return KErrNone;
}

TInt RProcess::Open(TProcessId aId,TOwnerType aType)
{
    return KErrNone;
}
   
void RProcess::Resume()
{               
}

TExitType RProcess::ExitType() const
{
    return TExitType();       
}

TUidType RProcess::Type() const
{
    return TUidType();
}

TProcessId RProcess::Id() const
{
    return 0;
}

void RProcess::Rendezvous(TRequestStatus& aStatus) const
{
    aStatus = KRequestPending;
    return;
}

TInt RProcess::RendezvousCancel(TRequestStatus& aStatus) const
{
    return KErrNone;
}

void RProcess::Rendezvous(TInt aReason)
{
    
}

TVendorId RProcess::VendorId() const
{
    TVendorId vi(0);
    return vi;
}

void User::WaitForRequest(TRequestStatus& aStatus)
{
    aStatus = KErrNone;
    return;
}
