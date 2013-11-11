/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef BTONNOTIFY_H
#define BTONNOTIFY_H

#include <e32std.h>

class MBTOnNotify
    {
    public:
        virtual void BTOnCompleteL( TBool aResult ) = 0;
    };

#endif // BTONNOTIFY_H

//  End of File
