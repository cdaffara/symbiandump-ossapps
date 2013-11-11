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

#ifndef MEMSPYUIUTILS_H
#define MEMSPYUIUTILS_H

// System includes
#include <e32std.h>
#include <e32const.h>

// Engine includes
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>

typedef TBuf<20> TMemSpySizeText;

_LIT( KTab, "\t" );

//ThreadInfoItemList view types
_LIT( KTypeUnknown, "Unknown Type" );
_LIT( KGeneral, "General" );
_LIT( KHeap, "Heap" );
_LIT( KStack, "Stack" );
_LIT( KChunks, "Chunks" );
_LIT( KCodeSegs, "Code Segments" );
_LIT( KOpenFiles, "Open Files" );
_LIT( KActiveObjects, "Active Objects" );
_LIT( KThreadHandlers, "Handlers to other Threads" );
_LIT( KProcessHandlers, "Handlers to other Processes" );
_LIT( KServers, "Servers running in Thread" );
_LIT( KConnections, "Client <> Server Connections" );
_LIT( KSemaphores, "Semaphores" );
_LIT( KThreadReferences, "References in Thread" );
_LIT( KProcessReferences, "References in Process" );
_LIT( KMutexes, "Mutexes" );
_LIT( KTimers, "Timers" );
_LIT( KDD, "Logical DD Channels" );
_LIT( KChangeNotif, "Change Notifiers" );
_LIT( KUndertakers, "Undertakers" );
_LIT( KLogicalDrivers, "Logical Device Drivers" );
_LIT( KPhysicalDrivers, "Physical Device Drivers" );	

//Kernel Object types
_LIT( KTypeThread, "Thread" );
_LIT( KTypeProcess, "Process" );
_LIT( KTypeChunk, "Chunk" );
_LIT( KTypeLibrary, "Library" );
_LIT( KTypeSemaphore, "Semaphore" );
_LIT( KTypeMutex, "Mutex" );
_LIT( KTypeTimer, "Timer" );
_LIT( KTypeServer, "Server" );
_LIT( KTypeSession, "Session" );
_LIT( KTypeLogicalDevice, "Logical Device" );
_LIT( KTypePhysicalDevice, "Physical Device" );
_LIT( KTypeLogicalChannel, "Logical Channel" );
_LIT( KTypeChangeNotifier, "Change Notifier" );
_LIT( KTypeUndertaker, "Undertaker" );
_LIT( KTypeMsgQueue, "Msg. Queue" );
_LIT( KTypePropertyRef, "Property Ref." );
_LIT( KTypeCondVar, "Conditional Var." );


class MemSpyUiUtils : public MemSpyEngineUtils
    {
public:
    static void Format( TDes& aBuf, TInt aResourceId, ...);
    static void GetErrorText( TDes& aBuf, TInt aError );
    static HBufC* FormatItemLC( const TDesC& aCaption );
    static TDesC& ThreadInfoItemNameByType( TMemSpyThreadInfoItemType aType );
    static void AppendPriority( TDes& aDes, TProcessPriority aPriority );
    static void AppendExitInfo( TDes& aDes, TExitType aType, TInt aExitReason, const TDesC& aExitCategory  );
    static void AppendExitType( TDes& aDes, TExitType aType );
    
    //for Kernel Objects
    static TPtrC TypeAsString( TMemSpyDriverContainerType aType );
    static TMemSpySizeText FormatSizeText( const TInt64& aValue, TInt aDecimalPlaces, TBool aExtraRounding = EFalse );
    };




#endif
