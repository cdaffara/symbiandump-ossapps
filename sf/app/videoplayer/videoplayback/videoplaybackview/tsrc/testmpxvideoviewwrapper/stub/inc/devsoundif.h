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
* Description:  Implementation of CDevSoundIf
*
*/

// Version : %version:  1 %



#ifndef __DEVSOUNDIF_H
#define __DEVSOUNDIF_H

#include <e32base.h>

class CDevSoundIf : public CBase
{
    public:

        static CDevSoundIf* NewL();
        virtual ~CDevSoundIf();

    public:
        TInt GetNumberOfVolumeSteps();

    private:
        CDevSoundIf();
        void ConstructL();

    public:
        TInt iVolumeSteps;
};

#endif      // __DEVSOUNDIF_H

// End of File
