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
* Description:  mpx collection playlist class stub for unit testing
*
*/

// Version : %version:  1 %

#ifndef TESTUTILITIES_H_
#define TESTUTILITIES_H_

#include "mpxcollectionutilityimp_stub.h"
#include "mpxcollectionutilityimp_stub.h"
#include "mpxplaybackutilityimp_stub.h"

class TestUtilities : public CBase
{
    public:

        void DeleteCollectionUtility();
        void DeletePlaybackUtility();

        void CloseUtility();

        CMPXCollectionUtility* iCollectionUtility;
        CMPXPlaybackUtility*   iPlaybackUtility;
};
#endif /*TESTUTILITIES_H_*/
