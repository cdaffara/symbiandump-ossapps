/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It defines constants shared between this subsystem and 
*                clients of the subsystem.
*
*/



#ifndef PHONEANIMDLLSTD_H
#define PHONEANIMDLLSTD_H

// DATA TYPES

/** 
* Animation services. 
*
* EAnimClearBlackList - blacklist clearing.
*/
enum TPhoneAnimType
    {
    EAnimClearBlackList
    };

/** 
* Operations for blacklist clearing.
*
* ECancelBlackList - clear blacklist upon next keypress.
*/
enum TPhoneAnimOpcode
    {
    ECancelBlackList,
    EClearBlackListNow
    };

#endif // PHONEANIMDLLSTD_H

// End Of File
