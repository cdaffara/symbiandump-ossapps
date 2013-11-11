/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mdaaudioplayerutility stub for testing mpequalizerframeworkwrapper
*
*/


#include "mptrace.h"
#include "stub/inc/mdaaudioplayerutility.h"


/*!
 Stub function.
*/
CMdaAudioPlayerUtility::CMdaAudioPlayerUtility()
{
}

/*!
 Stub function.
*/
CMdaAudioPlayerUtility::~CMdaAudioPlayerUtility()
{
}

/*!
 Stub function.
*/
void CMdaAudioPlayerUtility::ConstructL()
{
}

/*!
 Stub function.
*/
CMdaAudioPlayerUtility* CMdaAudioPlayerUtility::NewL( MMdaAudioPlayerCallback& /*aCallback*/,
                                                      TInt /*aPriority*/,
                                                      TInt /*aPref*/ )
{
    CMdaAudioPlayerUtility* self = new( ELeave ) CMdaAudioPlayerUtility;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/*!
 Stub function.
*/
void CMdaAudioPlayerUtility::OpenFileL(const TDesC& /*aFileName*/)
{
}

/*!
 Stub function.
*/
void CMdaAudioPlayerUtility::Close()
{
}

//end of file
