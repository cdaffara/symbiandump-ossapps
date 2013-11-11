/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Harvester Utility stub for testing mpmpxframeworkwrapper
*
*/


#include <e32base.h>
#include "stub/inc/mpxharvesterutility.h"

/*!
 Stub function.
*/
MMPXHarvesterUtility::MMPXHarvesterUtility():iCheckSystemEvents(EFalse),
                                             iScan(EFalse),
                                             iLeave(EFalse)
{
}

/*!
 Stub function.
*/
MMPXHarvesterUtility::~MMPXHarvesterUtility()
{
}

/*!
 Stub function.
*/
void MMPXHarvesterUtility::ScanL()
{
    iScan = ETrue;
}

/*!
 Stub function.
*/
void MMPXHarvesterUtility::CancelScanL()
{
    if(iLeave){
        User::Leave(KErrNotFound);
    }else{
        iScan = EFalse;
    }
    
}

/*!
 Stub function.
*/
void MMPXHarvesterUtility::CheckForSystemEventsL()
{
    iCheckSystemEvents = ETrue;
}

/*!
 Stub function.
*/
void MMPXHarvesterUtility::Close()
{
    delete this;
}

/*!
 Stub function.
*/
MMPXHarvesterUtility* CMPXHarvesterFactory::NewL()
{
    return new MMPXHarvesterUtility();
}

