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
* Description: isolated collection helper stub for testing mpmpxframeworkwrapper.
*
*/


#include <mpxcollectionpath.h>
#include <QtGlobal>
#include "stub/inc/mpmpxisolatedcollectionhelper.h"




/*!
 \internal
 */
CMpMpxIsolatedCollectionHelper* CMpMpxIsolatedCollectionHelper::NewL( 
        MMpMpxIsolatedCollectionHelperObserver* aObserver )
    {
    CMpMpxIsolatedCollectionHelper* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

/*!
 \internal
 */
CMpMpxIsolatedCollectionHelper* CMpMpxIsolatedCollectionHelper::NewLC( 
        MMpMpxIsolatedCollectionHelperObserver* aObserver )
    {
    CMpMpxIsolatedCollectionHelper* self =
            new ( ELeave ) CMpMpxIsolatedCollectionHelper( aObserver );
    CleanupStack::PushL( self );
    return self;
    }

/*!
 \internal
 */
CMpMpxIsolatedCollectionHelper::~CMpMpxIsolatedCollectionHelper()
    {
    }
    

/*!
 \internal
 */
void CMpMpxIsolatedCollectionHelper::OpenCollectionL( CMPXCollectionPath& aPath, TInt aIndex, MpOpenMode aMode )
    {
    Q_UNUSED( aIndex );
    iMode = aMode;
    iCountPath = aPath.Count();
    iOpen = ETrue;
    iOpenCount++;
    }

/*!
 \internal
 */
CMpMpxIsolatedCollectionHelper::CMpMpxIsolatedCollectionHelper( MMpMpxIsolatedCollectionHelperObserver* aObserver )
    : iObserver( aObserver ),
      iOpen(EFalse),
      iOpenCount(0),
      iCountPath(0),
      iMode( DefaultMode )
    {
    }

    
//EOF
