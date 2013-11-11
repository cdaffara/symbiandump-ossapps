/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MPERSONALITYNOTIFY_H
#define MPERSONALITYNOTIFY_H

#include <e32std.h>

class MPersonalityNotify
    {
    public:
        virtual void PersonalityQueryCompleteL() = 0;
        virtual void SetPreviousPersonalityCompleteL() = 0;

    };

#endif // MPERSONALITYNOTIFY_H

//  End of File
