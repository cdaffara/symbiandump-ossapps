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
*     The source file of the file manager s60 utilities
*/


#include "fms60utils.h"

void FmS60Utils::ConvertCharsToPwd(TDesC& aWord, TDes8& aConverted)
{
    aConverted.FillZ(aConverted.MaxLength());
    aConverted.Zero();
    
    if (aWord.Length())
        {
        aConverted.Copy( (TUint8*)(&aWord[0]), aWord.Size() );
        }
}
