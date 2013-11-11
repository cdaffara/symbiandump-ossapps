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

#ifndef FILEBROWSER_STD_H
#define FILEBROWSER_STD_H

#include <e32cmn.h>
#include <e32std.h>
#include <e32base.h>

LOCAL_C inline TBool IsQHD(const TSize& aSize) { return ((aSize.iWidth==640 && aSize.iHeight==360) || (aSize.iWidth==360 && aSize.iHeight==640)); }

#endif

// End of File

