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
* Description: Music Player MPX common definitions.
*
*/

#ifndef MPMPXCOMMONDEFS_H
#define MPMPXCOMMONDEFS_H

#include <QMetaType>

// Disk events from MPX Framework.
enum MpxDiskEvents {
    DiskFormatStarted,
    DiskFormatEnded,
    DiskRemoved,
    DiskInserted
};
// Register so type can be used in signal and slot connection
Q_DECLARE_METATYPE(MpxDiskEvents)

// USB events from MPX Framework.
enum MpxUsbEvents {
    UsbMassStorageStarted,
    UsbMassStorageEnded,
    UsbMtpStarted,
    UsbMtpEnded,
    UsbMtpNotActive
};
// Register so type can be used in signal and slot connection
Q_DECLARE_METATYPE(MpxUsbEvents)

#endif  // MPMPXCOMMONDEFS_H

