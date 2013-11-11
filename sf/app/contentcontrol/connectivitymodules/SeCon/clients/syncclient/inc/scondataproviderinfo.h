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
* Description:  CSconDataproviderInfo header
*
*/


#ifndef SCONDATAPROVIDERINFO_H_
#define SCONDATAPROVIDERINFO_H_

#include <e32base.h>
#include <badesca.h>

class RWriteStream;
class RReadStream;

/**
 * Contains information about DataSync dataprovider.
 */
class CSconDataproviderInfo : public CBase
{
public:
    IMPORT_C static CSconDataproviderInfo* NewL();
    IMPORT_C ~CSconDataproviderInfo();
    
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    IMPORT_C TUid        ImplementationUid() const;
    IMPORT_C void        SetImplementationUid( const TUid aUid );
    IMPORT_C TDesC&      DisplayNameL();
    IMPORT_C void        SetDisplayNameL( const TDesC& aName );
    IMPORT_C TDesC&      DefaultStoreL();
    IMPORT_C void        SetDefaultStoreL( const TDesC& aStore );
    IMPORT_C CDesCArray* ListStoresL() const;
    IMPORT_C void        SetStoresL( const CDesCArray& aStores );
private:
	CSconDataproviderInfo();
	void ConstructL();
	
private:
    TInt        iUid;
    HBufC*      iName;
    HBufC*      iDefaultStore;
    CDesCArrayFlat* iStores;
};

typedef RPointerArray<CSconDataproviderInfo> RSconDataProviderInfoArray;

#endif /*SCONDATAPROVIDERINFO_H_*/
