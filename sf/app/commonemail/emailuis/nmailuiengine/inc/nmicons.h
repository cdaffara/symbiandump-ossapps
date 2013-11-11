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

#ifndef NMICONS_H_
#define NMICONS_H_

#include "nmuienginedef.h"

class HbIcon;

class NMUIENGINE_EXPORT NmIcons {
 public:
    enum Icon
    {
        NmIconDefaultMailbox = 0,
        NmIconPlusSign,
        NmIconMinusSign,
        NmIconAttachment,
        NmIconPriorityHigh,
        NmIconPriorityLow,
        NmIconAttach,
        NmIconSend,
        NmIconContacts,
        NmIconNewEmail,
        NmIconDelete,
        NmIconForward,
        NmIconReply,
        NmIconReplyAll,
        NmIconSynching,
        NmIconOnline,
        NmIconDisconnected,
        NmIconOffline,
        NmLastItem
    };

    static HbIcon &getIcon(NmIcons::Icon icon);
    static void freeIcons();
};

#endif /* NMICONS_H_ */
