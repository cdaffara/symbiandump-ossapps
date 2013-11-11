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
#ifndef TESTDEFS_H_
#define TESTDEFS_H_

#include <CEunitAllocTestCaseDecorator.h>

//Is eunit alloc decorator enabled or not.
class TTestUtils
    {
public:
    static TBool AllocTestEnabled()
        {
        CEUnitTestCaseDecorator* decorator 
            = CEUnitTestCaseDecorator::ActiveTestCaseDecorator( 
            KEUnitAllocTestCaseDecoratorName );
        return ( decorator ? ETrue : EFalse );
        }
    };

//id for test controller
const TInt KTestControllerUid = 0xFFFF;

//call id
TInt const KVoipCallId = 157;

//service id
TUint32 const KServiceId = 3;

//remote party name match found
_LIT( KRemotePartyNameMatchFound, "Match" );

//no remote party name found
_LIT( KRemotePartyNameMatchNotFound, "No Match" );

#endif /*TESTDEFS_H_*/
