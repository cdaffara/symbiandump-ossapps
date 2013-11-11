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

#ifndef MEMSPYVIEWTYPE_H
#define MEMSPYVIEWTYPE_H

// System includes
#include <e32std.h>

// Enumerations
enum TMemSpyViewType
    {
    EMemSpyViewTypeNone = -1,
    EMemSpyViewTypeMainMenu = 0,
    EMemSpyViewTypeProcesses,
    EMemSpyViewTypeRAMInfo,
    EMemSpyViewTypeROMInfo,
    EMemSpyViewTypeServerList,
    EMemSpyViewTypeOpenFiles,
    EMemSpyViewTypeFBServBitmaps,
    EMemSpyViewTypeFBServBitmapInfo,
    EMemSpyViewTypeFBServBitmapViewer,
    EMemSpyViewTypeCodeSegmentList,
    EMemSpyViewTypeCodeSegmentDetails,
    EMemSpyViewTypeChunkList,
    EMemSpyViewTypeChunkDetails,
    EMemSpyViewTypeSystemConfig,
    EMemSpyViewTypeKernel,
    EMemSpyViewTypeKernelContainers,
    EMemSpyViewTypeKernelObjects,
    EMemSpyViewTypeKernelHeap,
    EMemSpyViewTypeDriveSummary,
    EMemSpyViewTypeDriveInfo,
    EMemSpyViewTypeECom,
    EMemSpyViewTypeEComCategory,
    EMemSpyViewTypeEComInterface,
    EMemSpyViewTypeEComImplementation,
    EMemSpyViewTypeThreads,
    EMemSpyViewTypeThreadInfoItemList,
    EMemSpyViewTypeThreadInfoItemHeap,
    EMemSpyViewTypeThreadInfoItemStack,
    EMemSpyViewTypeThreadInfoItemChunk,
    EMemSpyViewTypeThreadInfoItemCodeSeg,
    EMemSpyViewTypeThreadInfoItemServer,
    EMemSpyViewTypeThreadInfoItemServerDetails,
    EMemSpyViewTypeThreadInfoItemServerSessions,
    EMemSpyViewTypeThreadInfoItemSession,
    EMemSpyViewTypeThreadInfoItemSemaphore,
    EMemSpyViewTypeThreadInfoItemMutex,
    EMemSpyViewTypeThreadInfoItemTimer,
    EMemSpyViewTypeThreadInfoItemLDD,
    EMemSpyViewTypeThreadInfoItemPDD,
    EMemSpyViewTypeThreadInfoItemLogicalChannel,
    EMemSpyViewTypeThreadInfoItemChangeNotifier,
    EMemSpyViewTypeThreadInfoItemUndertaker,
    EMemSpyViewTypeThreadInfoItemMessageQueue,
    EMemSpyViewTypeThreadInfoItemConditionalVariable,
    EMemSpyViewTypeThreadInfoItemOpenFiles,
    EMemSpyViewTypeThreadInfoItemActiveObject,
    EMemSpyViewTypeThreadInfoItemActiveObjectDetails,
    EMemSpyViewTypeThreadInfoItemGeneralInfo,
    EMemSpyViewTypeThreadInfoItemOtherThreads,
    EMemSpyViewTypeThreadInfoItemOtherProcesses,
    EMemSpyViewTypeThreadInfoItemOwnedThreadHandles,
    EMemSpyViewTypeThreadInfoItemOwnedProcessHandles,
    EMemSpyViewTypeHeapTracking,
    EMemSpyViewTypeHeapTrackingSettings,
    EMemSpyViewTypeHeapTrackingResults,
    EMemSpyViewTypeHeapTrackingResultsCycleInfo,
    EMemSpyViewTypeHeapTrackingResultsCycleChangeDescriptor,
    EMemSpyViewTypeThreadInfoItemMemoryTracking,
    EMemSpyViewTypeThreadInfoItemMemoryTrackingAutoStartConfig,
    EMemSpyViewTypeThreadInfoItemMemoryTrackingStatistics,
    EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsCurrent,
    EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsHWM,
    EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsPeak,
    EMemSpyViewTypeWindowGroups
    };


class MemSpyViewTypeUtils
    {
public:
    static TBool IsOpenableItem( TMemSpyViewType aType );
    static TBool IsThreadInfoItem( TMemSpyViewType aType );
    static TBool IsExitableView( TMemSpyViewType aType );
    };


#endif
