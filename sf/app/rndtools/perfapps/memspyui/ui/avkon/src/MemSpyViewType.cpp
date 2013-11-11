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

#include "MemSpyViewType.h"




TBool MemSpyViewTypeUtils::IsOpenableItem( TMemSpyViewType aType )
    {
    TBool openable = EFalse;
    //
    switch( aType )
        {
    case EMemSpyViewTypeMainMenu:
    case EMemSpyViewTypeProcesses:
    case EMemSpyViewTypeServerList:
    case EMemSpyViewTypeThreads:
    case EMemSpyViewTypeThreadInfoItemList:
    case EMemSpyViewTypeThreadInfoItemServer:
    case EMemSpyViewTypeThreadInfoItemSession:
        openable = ETrue;
        break;
    default:
        break;
        }
    //
    return openable;
    }


TBool MemSpyViewTypeUtils::IsThreadInfoItem( TMemSpyViewType aType )
    {
    TBool infoItem = ETrue;
    //
    switch( aType )
        {
    case EMemSpyViewTypeMainMenu:
    case EMemSpyViewTypeRAMInfo:
    case EMemSpyViewTypeROMInfo:
    case EMemSpyViewTypeServerList:
    case EMemSpyViewTypeOpenFiles:
    case EMemSpyViewTypeProcesses:
    case EMemSpyViewTypeThreads:
    case EMemSpyViewTypeThreadInfoItemList:
        infoItem = EFalse;
        break;
    default:
        break;
        }
    //
    return infoItem;
    }


TBool MemSpyViewTypeUtils::IsExitableView( TMemSpyViewType aType )
    {
    TBool ret = EFalse;
    //
    switch( aType )
        {
    case EMemSpyViewTypeMainMenu:
        ret = ETrue;
        break;
    default:
        break;
        }
    //
    return ret;
    }


