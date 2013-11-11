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
* 
* Description:
*     The header file of the file manager s60 utilities
*/

#ifndef FMS60UTILS_H
#define FMS60UTILS_H

#include "fmcommon.h"
#include <e32des16.h> 

class FmS60Utils
{
public:
    static void ConvertCharsToPwd( TDesC& aWord, TDes8& aConverted );
};

#endif
