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
#ifndef CXERROR_H_
#define CXERROR_H_


#include <QMetaType>

/*
* All CameraX specific error codes
*/

namespace CxeError
{
    enum Id
        {
        //! No error, everything ok
        None                = 0,

        //! Out of Memory Situation
        OutOfMemory         = 1,

        //! Memory not accessible
        MemoryNotAccessible = 2,

        //! whenever leave occurs during intialization of adaptaion or anyother resources we need
        InitializationFailed= 3,

        //! for e.g. when setting key is not found
        NotFound            = 4,

        //! when trying to access a specific feature but not supported by adaptation
        NotSupported        = 5,

        //! Error When specific disk or drive is full
        DiskFull            = 6,

        //! DSP side error when encoder/decoder not available during video recording
        HwNotAvailable      = 7,

        //! Audio utility intilization error
        Died                = 8,

        //! Camera or anyother resources already in use
        InUse               = 9,

        //! when a specific resource is not ready to continue operation
        NotReady            = 10,

        //! any other general errors
        General             = 11
        };
}

Q_DECLARE_METATYPE(CxeError::Id)


#endif /*CXERROR_H_*/
