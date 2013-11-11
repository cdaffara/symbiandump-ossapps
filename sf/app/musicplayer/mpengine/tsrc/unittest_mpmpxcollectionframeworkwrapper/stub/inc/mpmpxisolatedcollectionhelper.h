/*
* Copyright ( c ) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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

#ifndef CMPMPXISOLATEDCOLLECTIONHELPER_H
#define CMPMPXISOLATEDCOLLECTIONHELPER_H

#include <e32base.h>
class CMPXCollectionPath;
class CMPXMedia;

const TInt KIncrementalNullOffset = 0;

class MMpMpxIsolatedCollectionHelperObserver
{
public:

    virtual void HandleIsolatedOpenL( const CMPXMedia& aEntries, 
            TInt aError ) = 0;
    virtual void HandleIsolatedOpenRestorePathL( const CMPXCollectionPath& aPath,
            TInt aError ) = 0;
};

class CMpMpxIsolatedCollectionHelper : public CBase
{
public:
    enum MpOpenMode{
            DefaultMode = 0,
            RestorePathMode 
        };
    static CMpMpxIsolatedCollectionHelper* NewL( MMpMpxIsolatedCollectionHelperObserver* aObserver );
    static CMpMpxIsolatedCollectionHelper* NewLC( MMpMpxIsolatedCollectionHelperObserver* aObserver );
    virtual ~CMpMpxIsolatedCollectionHelper();
    void OpenCollectionL( CMPXCollectionPath& aPath, TInt aIndex = KIncrementalNullOffset, MpOpenMode aMode = DefaultMode );
    
private:
    CMpMpxIsolatedCollectionHelper( MMpMpxIsolatedCollectionHelperObserver* aObserver );

public:
    MMpMpxIsolatedCollectionHelperObserver* iObserver;
    TBool          iOpen;
    TInt           iOpenCount;
    TInt           iCountPath;
    MpOpenMode     iMode;

};

#endif // CMPMPXISOLATEDCOLLECTIONHELPER_H
    
//EOF
