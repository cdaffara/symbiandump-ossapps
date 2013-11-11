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
* Description:  cone utilities stub for unit testing
*
*/

// Version : %version: 1 %

#include <f32file.h>
#include <coeutils.h>

TBool ConeUtils::FileExists(const TDesC& aFileName)
{
    TBool retVal = EFalse;

    RFs fs;
    RFile file;

    TInt error = fs.Connect();

    if ( error == KErrNone )
    {
        fs.ShareProtected();

        error = file.Open( fs, aFileName, EFileRead );

        if ( error == KErrNone )
        {
            retVal = ETrue;

            file.Close();
        }

        fs.Close();
    }

    return retVal;
}
