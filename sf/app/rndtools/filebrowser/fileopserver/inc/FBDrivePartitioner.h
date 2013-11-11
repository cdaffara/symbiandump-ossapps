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


#ifndef __FBDRIVEPARTITIONER_H__
#define __FBDRIVEPARTITIONER_H__

#include <e32base.h>
#include <f32file.h>

class FBDrivePartioner
    {
public:
    static TInt FindLocalDriveNumber(RFs& aFs, TInt aDriveNumber, TInt& aLocalDriveNum);
    static TInt EraseMBR(RFs& aFs, TInt aDriveNumber);
    static TInt CreatePartitions(RFs& aFs, TInt aDriveNumber, TInt aPartCount);
    static TInt FormatPartitions(RFs& aFs, TInt aDriveNumber, TInt aPartCount);    
    };


#endif
