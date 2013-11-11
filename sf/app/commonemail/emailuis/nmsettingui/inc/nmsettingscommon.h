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
* Description:
*
*/

#ifndef NMSETTINGSCOMMON_H
#define NMSETTINGSCOMMON_H

namespace NmSettings
{
    enum MailboxEventType {
        MailboxCreated = 0,
        MailboxDeleted
    };

    enum MailboxProperty {
        MailboxName = 0,
        MailboxAddress
    };

}

#endif // NMSETTINGSCOMMON_H

// End of file.
