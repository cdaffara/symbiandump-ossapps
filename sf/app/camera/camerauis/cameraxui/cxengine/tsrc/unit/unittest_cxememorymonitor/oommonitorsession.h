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
* Description: Fake class for testing
*
*/

#ifndef OOMMONITORSESSION_H
#define OOMMONITORSESSION_H

#include <e32base.h>

const TInt KOomMaxAllocationWithoutPermission = 1048576;
const TUid KOomMemoryMonitorStatusPropertyCategory = {0x10207218};
const TUint32 KOomMemoryMonitorStatusPropertyKey = 0;
enum TMemoryMonitorStatusPropertyValues
    {
    // Above Treshhold: Free Ram is above good treshhold point and memory monitor isn't freeing any memory
    EAboveTreshHold,

    // Freeing Memory: Memory monitor is in the middle of freeing memory
    EFreeingMemory,

    // Below Treshhold: Memory monitor has tried to free some RAM but total memory is still below treshhold and memory monitor has given up freeing more memory.
    EBelowTreshHold
    };
class ROomMonitorSession
    {
public:

    /**  Defines the application priorities of OOM monitor. */
    enum TOomPriority
        {
        /**
        * Application can be closed if needed.
        */
        EOomPriorityNormal = 0,

        /**
        * Application should not be closed if possible.
        */
        EOomPriorityHigh,

        /**
        * Application is busy and should not be closed.
        */
        EOomPriorityBusy
        };

public:
    /**
    * Connects a new session.
    * Sessions must be connected before any other APIs can be used.
    * When the client has finished using a session, Close() must be called.
    * @return KErrNone if successful, error code otherwise.
    */
    TInt Connect();

    /**
    * Request that the OOM monitor attempts to free some paged memory.
    * This function may take several seconds to execute, depending on
    * the memory state. It will not return until the attempt to recover
    * memory has completed.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @return KErrNone if the request memory is free. KErrNoMemory if that
    *         amount of memory could not be recovered. Other error codes may
    *         also be returned.
    */
    TInt RequestFreeMemory(TInt aBytesRequested);

    /**
    * Request that the OOM monitor attempts to free some paged memory.
    * This is an asynchronous version of the request for free paged memory.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @param aStatus will be completed when the attempt to recover memory
    *        has finished. This may take several seconds, depending on
    *        the memory state. On completion, aStatus will be set to
    *        KErrNone if the request memory is free. KErrNoMemory if that
    *        amount of memory could not be recovered. Other error codes may
    *        also be set.
    */
    void RequestFreeMemory(TInt aBytesRequested, TRequestStatus& aStatus);

    /**
    * Request that the OOM monitor attempts to free some memory for an optional allocation.
    * The passed in plugin ID is used to determine the priority for this allocation.
    * Lower priority OOM actions may be executed to free enough RAM for this allocation.
    * This function may take several seconds to execute, depending on
    * the memory state. It will not return until the attempt to recover
    * memory has completed.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @param aPluginId The ID of the plugin that may delete the allocation in event of low memory.
    * @return KErrNone if the request memory is free. KErrNoMemory if that
    *         amount of memory could not be recovered. Other error codes may
    *         also be returned.
    */
    TInt RequestOptionalRam(TInt aBytesRequested, TInt aMinimumBytesNeeded, TInt aPluginId, TInt& aBytesAvailable);

    /**
    * Request that the OOM monitor attempts to free some memory for an optional allocation.
    * The passed in plugin ID is used to determine the priority for this allocation.
    * Lower priority OOM actions may be executed to free enough RAM for this allocation.
    * This function may take several seconds to execute, depending on
    * the memory state. It will not return until the attempt to recover
    * memory has completed.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @param aPluginId The ID of the plugin that may delete the allocation in event of low memory.
    * @param aStatus The TRequestStatus (completes with the number of bytes freed (aStatus >= 0) or an error (aStatus <= 0))
    * @return None
    */
    void RequestOptionalRam(TInt aBytesRequested, TInt aMinimumBytesNeeded, TInt aPluginId, TRequestStatus& aStatus);

    /**
    * Cancels the asynchronous request for free memory.
    */
    void CancelRequestFreeMemory();

    /**
    * Notify the OOM monitor that this application is not responding
    * to the EEikCmdExit request to exit the application.
    * This function is used by the Avkon framework's app shutter.
    * @param aWgId the window group identifier of the app that is not exiting.
    */
    void ThisAppIsNotExiting(TInt aWgId);

    /**
    * Notify the OOM monitor that this application has the specified priority
    * @param aPriority the priority of the application
    */
    void SetOomPriority(TOomPriority aPriority);

    void Close();

    };

#endif // OOMMONITORSESSION_H
