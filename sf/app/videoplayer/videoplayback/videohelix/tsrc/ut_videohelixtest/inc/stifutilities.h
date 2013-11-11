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
* Description:  mpx utilities class stub for STIF testing
*
*/

// Version : %version: 2 %

#ifndef STIFUTILITIES_H_
#define STIFUTILITIES_H_

#include "dlmgr_stub.h"
#include "mpxvideoplayerutility_stub.h"
#include "mpxvideoaccessoryobserver_stub.h"

class StifUtilities : public CBase
{
    public:
        static void CreateStifUtility();

    public:
        CMpxVideoPlayerUtility*     iPlayerUtility;
        CDlMgrTestClass*            iDlMgrTester;
        CMPXVideoAccessoryObserver* iAccObserver;
};

#endif /*STIFUTILITIES_H_*/
