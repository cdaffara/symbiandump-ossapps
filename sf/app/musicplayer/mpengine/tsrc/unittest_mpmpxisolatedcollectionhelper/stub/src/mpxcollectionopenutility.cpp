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
* Description: mpxcollectionopenutility stub for testing CMpMpxIsolatedCollectionHelper
*
*/



#include <e32debug.h>
#include <Qt>
#include <mpxcollectionpath.h>

#include "stub/inc/mpxcollectionopenutility.h"


/*!
 Stub function.
*/
CMPXCollectionOpenUtility::CMPXCollectionOpenUtility():iDelay(0),
                                                       iChunkSize(0),
                                                       iStop(EFalse),
                                                       iStart(EFalse),
                                                       iDirection(EFetchDown),
                                                       iMode(TUid::Null())
{
    
}

/*!
 Stub function.
*/   
CMPXCollectionOpenUtility* CMPXCollectionOpenUtility::NewL(MMPXCollectionObserver* aObs, TUid aMode)
{
    Q_UNUSED(aObs);
    CMPXCollectionOpenUtility* collectionOpenUtility = new CMPXCollectionOpenUtility();
    collectionOpenUtility->iMode = aMode;
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
CMPXCollectionPath* CMPXCollectionOpenUtility::PathL()
{
    return CMPXCollectionPath::NewL();
}
/*!
 Stub function.
*/
void CMPXCollectionOpenUtility::StartL( const CMPXCollectionPath& aPath,
                              TArray<TMPXAttribute> aAttrs,
                              TInt aChunkSize,
                              TInt aOffset,
                              TDirection aDirection,
                              TMPXAttribute aKeyAttribute)
{
    Q_UNUSED(aAttrs);
    Q_UNUSED(aOffset);
    Q_UNUSED(aKeyAttribute);
    iPath = &aPath;
    iStart = ETrue;
    iChunkSize = aChunkSize;
    iDirection = aDirection;
}

//end of file
