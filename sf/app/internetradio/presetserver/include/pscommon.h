/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common client-server header file for the Preset Server
*
*/


#ifndef PSCOMMON_H
#define PSCOMMON_H

#include <e32std.h>

#include "pspresetobserver.h"

const TInt KPSVersionMajor = 1;     /**< Major version number of the server. */
const TInt KPSVersionMinor = 0;     /**< Minor version number of the server. */
const TInt KPSVersionBuild = 0;     /**< Build version number of the server. */

_LIT( KPSServerExe, "psserver.exe" );           /**< Name of the preset server executable. */
_LIT( KPSServerName, "PresetServer" );          /**< Name of the preset server process. */

/**
 * Notification package struct.
 *
 * Used when a preset change notification is requested or completed.
 */
typedef struct
    {
    /** Identifier of the preset the observer is interested in, or <code>KErrNotFound</code> if all changes should be notified. */
    TInt iId;
    /** Data handler of the preset (UID of the ECom plugin that handles this kind of presets). */
    TUid iDataHandler;
    /** Reason for the change. */
    MPSPresetObserver::TPSReason iReason;
    /** Unique identifier for the notification. */
    TUint64 iNotificationId;
    /** Universal time for this notification's origin. This is not used in the client-server communication and can be omitted. */
    TTime iNotificationTime;
    } TPSNotifyDataPackage;

/**
 * Client-server operation codes.
 */
enum TPSOpCode
    {

    /**
     * Creates a new preset.
     *
     * Parameters:
     *  [IN]    Int0    Index of the preset to create.
     *  [IN]    Int1    Data handler of the preset to create.
     *  [OUT]   Int2    Id of the preset that was created.
     */
    EPSOpCreatePreset,         

    /**
     * Deletes a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset to delete.
     */
    EPSOpDeletePreset,

    /**
     * Starts an explicit read/write transaction with a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     */
    EPSOpStartTransaction,

    /**
     * Commits the changes made to a preset during a transaction.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     */
    EPSOpCommitTransaction,

    /**
     * Reverts all changes made to a preset during a transaction.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     */
    EPSOpRollbackTransaction,

    /**
     * Moves a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     *  [IN]    Int1    Destination index of the preset.
     */
    EPSOpMovePreset,

    /**
     * Notifies on preset changes.
     *
     * Parameters:
     *  [IN]    Int0    Pointer to the notifier as an integer.
     *  [IN]    Int1    ID of the preset to observe, or <code>KErrNotFound</code>
                        if all changes should result in a notification.
     *  [IN]    Ptr2    Packed data of type TPSNotifyDataPackage.
     */
    EPSOpNotify,

    /**
     * Cancels a pending notification of a preset change.
     *
     * Parameters:
     *  [IN]    Int0    Pointer to the notifier as an integer.
     */
    EPSOpCancelNotify,

    /**
     * Sets the name of a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     *  [IN]    Ptr1    Name of the preset.
     */
    EPSOpSetPresetName,

    /**
     * Sets the index of a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     *  [IN]    Int1    Index to set.
     */
    EPSOpSetPresetIndex,

    /**
     * Sets the data of a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     *  [IN]    Ptr1    Data to set.
     */
    EPSOpSetPresetData,

    /**
     * Gets the name of a preset.
     * 
     * Parameters:
     * [IN]     Int0    The id of the preset.
     * [OUT]    Ptr1    The name of the preset.
     */
    EPSOpGetPresetName,

    /**
     * Gets the length of a preset's name.
     * 
     * Parameters:
     * [IN]     Int0    Id of the preset.
     * [OUT]    Int1    Length of the preset's name.
     */
    EPSOpGetPresetNameLength,

    /**
     * Gets the index of a preset.
     *
     * Parameters:
     * [IN]     Int0    Id of the preset.
     * [OUT]    Int1    Index of the preset.
     */
    EPSOpGetPresetIndex,

    /**
     * Gets the data handler of a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     *  [OUT]   Int1    Data handler of the preset.
     */
    EPSOpGetPresetDataHandler,

    /**
     * Gets the data of a preset.
     *
     * Parameters:
     *  [IN]    Int0    Id of the preset.
     *  [OUT]   Ptr1    Data of the preset.
     */
    EPSOpGetPresetData,

    /**
     * Gets the length of a preset's data.
     * 
     * Parameters:
     * [IN]     Int0    Id of the preset.
     * [OUT]    Int1    Length of the preset's data.
     */
    EPSOpGetPresetDataLength,

    /**
     * Gets an array of preset ids.
     *
     * Parameters:
     *  [IN]    Int0    Data handler of the presets.
     *  [OUT]   Ptr1    Preset ids matching the data handler.
     */
    EPSOpGetPresets,

    /**
     * Gets the length of the preset array.
     * 
     * Parameters:
     *  [IN]    Int0    Data handler of the presets.
     *  [OUT]   Int1    Length of the array.
     */
    EPSOpGetPresetsLength

    };

#endif // PSCOMMON_H
