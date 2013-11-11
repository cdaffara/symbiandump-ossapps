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
* Description: mpxcollectionopenutility stub for testing mpmpxframeworkwrapper
*
*/



#include <e32debug.h>
#include <Qt>

#include "stub/inc/mpxcollectionopenutility.h"


/*!
 Stub function.
*/
CMPXCollectionOpenUtility::CMPXCollectionOpenUtility():iDelay(0),
                                                       iChunkSize(0),
                                                       iStop(EFalse),
                                                       iStart(EFalse),
                                                       iDirection(EFetchDown)
{
    
}

/*!
 Stub function.
*/   
CMPXCollectionOpenUtility* CMPXCollectionOpenUtility::NewL(MMPXCollectionObserver* aObs, TUid aMode)
{
    Q_UNUSED(aObs);
    Q_UNUSED(aMode);
    CMPXCollectionOpenUtility* collectionOpenUtility = new CMPXCollectionOpenUtility();
    return collectionOpenUtility;
}

/*!
 Stub function.
*/
CMPXCollectionOpenUtility::~CMPXCollectionOpenUtility()
{
    
}

/*!
 Stub function.
*/
void CMPXCollectionOpenUtility::Stop()
{
    iStop=ETrue;
}

/*!
 Stub function.
*/
void CMPXCollectionOpenUtility::SetDelay( TInt aDelay )
{
    iDelay = aDelay;
}

/*!
 Stub function.
*/
void CMPXCollectionOpenUtility::StartL( TArray<TMPXAttribute> aAttrs,
                              TInt aChunkSize,
                              TInt aOffset,
                              TDirection aDirection,
                              TMPXAttribute aKeyAttribute)
{
    Q_UNUSED(aAttrs);
    Q_UNUSED(aOffset);
    Q_UNUSED(aKeyAttribute);
    iStart = ETrue;
    iChunkSize = aChunkSize;
    iDirection = aDirection;
}

//end of file
