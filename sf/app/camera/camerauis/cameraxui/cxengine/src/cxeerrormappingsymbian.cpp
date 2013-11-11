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
* Description:
*
*/
#include "cxeerrormappingsymbian.h"
#include "cxeerror.h"
#include "e32err.h"

/*
* Mapping all symbian specific errors to cxerror id's
*/
CxeError::Id CxeErrorHandlingSymbian::map(int symbianerror)
{
    CxeError::Id cxerror = CxeError::General;
    
    switch( symbianerror ) {
        case KErrNone:
            cxerror = CxeError::None;
            break;
        case KErrNotFound:
            cxerror = CxeError::NotFound;
            break;
        case KErrNotReady:
            cxerror = CxeError::NotReady;
            break;
        case KErrNotSupported:
            cxerror = CxeError::NotSupported;
            break;
        case KErrInUse:
            cxerror = CxeError::InUse;
            break;
        case KErrDied:
            cxerror = CxeError::Died;
            break;
        case KErrHardwareNotAvailable:
            cxerror = CxeError::HwNotAvailable;
            break;
        case KErrGeneral:
            cxerror = CxeError::General;
            break;
        case KErrDiskFull:
            cxerror = CxeError::DiskFull;
            break;
        default:
            break;
    }

    return cxerror;
        
};
