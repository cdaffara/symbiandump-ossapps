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
#ifndef CNTHISTORYMODELCONSTS_H
#define CNTHISTORYMODELCONSTS_H

enum Attributes
    {
    CntIncoming = 1,
    CntUnseen = 2,
    CntAttachment = 4
    };

enum ItemType
    {
    CntCallLog = 32,
    CntMessage = 64
    };

enum Icon
    {
    CntMissedCall = 128,
    CntReceivedCall = 256,
    CntDialledCall = 512
    };

enum CustomRoles
    {
    CntFlagsRole = Qt::UserRole + 1,
    CntPhoneNumberRole,
    CntConverstaionIdRole
    };

// Constants
#define MISSED_CALL_ICON "qtg_small_missed_call"
#define DAILED_CALL_ICON "qtg_small_sent"
#define RECEIVED_CALL_ICON "qtg_small_received"
#define MESSAGE_ICON "qtg_small_message"

#endif
