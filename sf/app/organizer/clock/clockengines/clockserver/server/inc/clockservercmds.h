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
* Description:   Contains that clock server service command ids.
*
*/

#ifndef __CLOCKSERVER_CMDS_H__
#define __CLOCKSERVER_CMDS_H__

// Constants
/**
* @enum TClockServerMessage
* @brief The ClockServer service commands. Used by client to identify the service required.
*/
enum TClockServerMessage
    {
    EClkSrvActivateProtocol,
    EClkSrvIsProtocolActive,
    EClkSrvDeactivateProtocol,
    EClkSrvGetProtocolInfo,
    EClkSrvGetCurrentMcc,
    EClkSrvGetCurrentTimeZoneId,
    EClkSrvIsAutoTimeUpdadeOn,
    EClkSrvNotifyOnChange,
    EClkSrvCancelNotifyOnChange
    };
    
#endif		// __CLOCKSERVER_CMDS_H__

// End of file
