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
* Description:   Contains the clockserver information.
*
*/

#ifndef __CLOCKSERVER_COMMON_H__
#define __CLOCKSERVER_COMMON_H__

/**
* @var KClockServerMessageSlots
* @brief One async slot for notifier
*/
const TInt KClockServerMessageSlots( 1 );
const TUid KClockSrvUid = { 0x200159A2 };

_LIT( KNameClockServer, "ClockServer" );
_LIT( KNameClockServerStartSemaphore, "ClockServerStartSemaphore" );

#endif		// __CLOCKSERVER_COMMON_H__

// End of file