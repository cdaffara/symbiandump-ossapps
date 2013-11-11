/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32err.h>
#include "rlocationtrail.h"

RLocationTrail::RLocationTrail()
    : mTrailConnected(false), mTrailStarted(false)
{
}

RLocationTrail::~RLocationTrail()
{
}

int RLocationTrail::StartLocationTrail(TTrailCaptureSetting /*aState*/)
{
    int err = KErrNone;
    
    if (mTrailConnected) {
        mTrailStarted = true;
    } else {
        err = KErrGeneral;
    }
    
    return err;
}

int RLocationTrail::StopLocationTrail()
{
    int err = KErrNone;
    if (!mTrailConnected || !mTrailStarted) {
        err = KErrGeneral;
    } else {
        mTrailStarted = false;
    }
    return err;
}



int RLocationTrail::Connect()
{
    mTrailConnected = true;
    return KErrNone;
}


void RLocationTrail::Close()
{
    mTrailConnected = false;
}
