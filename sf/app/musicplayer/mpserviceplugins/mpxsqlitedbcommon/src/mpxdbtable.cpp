/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all table classes.
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>

#include "mpxdbcommonstd.h"
#include "mpxdbcommonutil.h"
#include "mpxdbmanager.h"
#include "mpxdbtable.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXDbTable::CMPXDbTable(
    CMPXDbManager& aDbManager) :
    iDbManager(aDbManager)
    {
    MPX_FUNC("CMPXDbTable::CMPXDbTable");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::BaseConstructL()
    {
    MPX_FUNC("CMPXDbTable::BaseConstructL");
    iDbManager.RegisterTableL(*this);
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXDbTable::~CMPXDbTable()
    {
    MPX_FUNC("CMPXDbTable::~CMPXDbTable");
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::UpdateMediaL default implementation
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::UpdateMediaL(
    RSqlStatement& /* aRecord */,
    const TArray<TMPXAttribute>& /* aAttrs */,
    CMPXMedia& /* aMedia */)
    {
    // do nothing
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    TInt aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, aValue);
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    const TDesC& aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + aValue.Length());
    query->Des().Format(aQuery, &aValue);
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    TInt aValue1,
    TInt aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + 2 * KMCIntegerLen);
    query->Des().Format(aQuery, aValue1, aValue2);
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    const TDesC& aValue1,
    TInt aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + aValue1.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, &aValue1, aValue2);
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    const TDesC& aValue1,
    TInt aValue2,
    const TDesC& aValue3,
    TInt aValue4)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() +
                                aValue1.Length() +
                                aValue3.Length() +
                                2*KMCIntegerLen);
    query->Des().Format(aQuery, &aValue1, aValue2, &aValue3, aValue4);
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aQuery));
    CleanupClosePushL(recordset);

    TInt pPath(0);
    if (aMediaArray.Count())
        {
        CMPXMedia* pMedia = aMediaArray[0];
        if (pMedia->IsSupported(KMPXMediaGeneralValue))
            { // Query excuted by OpenL
            pPath = pMedia->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
            MPX_ASSERT(pPath);
            }
        }

    TInt err(KErrNone);
    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(recordset, aAttrs, *media);
        if (media->IsSupported(KMPXMediaGeneralId) && pPath)
            {
            ids.AppendL(media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
            }
        aMediaArray.AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        }

    // Append ids to the returned path
    if (pPath)
        {
        ((CMPXCollectionPath*)pPath)->AppendL(ids.Array());
        }
    CleanupStack::PopAndDestroy(&ids);
    CleanupStack::PopAndDestroy(&recordset);

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
	TInt aDrive,
	const TArray<TMPXAttribute>& aAttrs,
	CMPXMediaArray& aMediaArray,
	const TDesC& aQuery,
	TInt aPlaylistId)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, aPlaylistId);
    ExecuteMediaQueryOnDriveL(aDrive, aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryOnDriveL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryOnDriveL(
	TInt aDrive,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryOnAllDrivesL(aDrive,aQuery));
    CleanupClosePushL(recordset);

    TInt pPath(0);
    if (aMediaArray.Count())
        {
        CMPXMedia* pMedia = aMediaArray[0];
        if (pMedia->IsSupported(KMPXMediaGeneralValue))
            { // Query excuted by OpenL
            pPath = pMedia->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
            MPX_ASSERT(pPath);
            }
        }

    TInt err(KErrNone);
    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(recordset, aAttrs, *media);
        if (media->IsSupported(KMPXMediaGeneralId) && pPath)
            {
            ids.AppendL(media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
            }
        aMediaArray.AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        }

    // Append ids to the returned path
    if (pPath)
        {
        ((CMPXCollectionPath*)pPath)->AppendL(ids.Array());
        }
    CleanupStack::PopAndDestroy(&ids);
    CleanupStack::PopAndDestroy(&recordset);

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TBool aAsc,
    const TDesC& aQuery)
    {

    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aQuery));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(recordset, aAttrs, *media);
        if (aAsc != EFalse)
            {
            aMediaArray.AppendL(*media);
            }
        else
            {
            aMediaArray.InsertL(*media, 0);
            }
        CleanupStack::PopAndDestroy(media);
        }

    CleanupStack::PopAndDestroy(&recordset);


    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }


// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia,
    const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aQuery));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    UpdateMediaL(recordset, aAttrs, aMedia);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia,
    const TDesC& aQuery,
    TInt aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, aValue);
    ExecuteMediaQueryL(aAttrs, aMedia, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    const TDesC& aValue1,
    const TDesC& aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + aValue1.Length() + aValue2.Length());
    query->Des().Format(aQuery, &aValue1, &aValue2);
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteIntQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteIntQueryL(
    const TDesC& aQuery)
    {
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aQuery));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    TInt64 value( recordset.ColumnInt64( KMPXTableDefaultIndex ) );
    CleanupStack::PopAndDestroy(&recordset);

    return value < 0 ? 0: I64LOW( value );
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteIntQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteIntQueryL(
    const TDesC& aQuery,
    TUint32 aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteIntQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, aValue);
    TUint32 ret(ExecuteIntQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteIntQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteIntQueryL(
    const TDesC& aQuery,
    const TDesC& aValue1,
    TUint32 aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteIntQueryL");

    // replace single quotes
    // reserve double the value length for single quote duplicates
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value1 = HBufC::NewLC(aValue1.Length() * 2);
    TPtr value1Ptr(value1->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue1, value1Ptr);

    // construct the query
    HBufC* query = HBufC::NewLC(aQuery.Length() + value1Ptr.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, value1, aValue2);

    // execute the query
    TUint32 ret(ExecuteIntQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(value1);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteIntQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteIntQueryL(
    const TDesC& aQuery,
    const TDesC& aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteIntQueryL");

    // replace single quotes
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value = HBufC::NewLC(aValue.Length() * 2);
    TPtr valuePtr(value->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue, valuePtr);

    // construct the query
    HBufC* query = HBufC::NewLC(aQuery.Length() + valuePtr.Length());
    query->Des().Format(aQuery, value);

    // execute the query
    TUint32 ret(ExecuteIntQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(value);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteIntQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteIntQueryL(
    const TDesC& aQuery,
    TUint32 aValue1,
    TUint32 aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteIntQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + 2 * KMCIntegerLen);
    query->Des().Format(aQuery, aValue1, aValue2);
    TUint32 ret(ExecuteIntQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteIntQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteIntQueryL(TInt aDriveID,const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbTable::ExecuteIntQueryL");
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDriveID,aQuery));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    TUint32 value(recordset.ColumnInt64(KMPXTableDefaultIndex));
    CleanupStack::PopAndDestroy(&recordset);

    return value;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aQuery));
    CleanupClosePushL(recordset);

    TInt sum(0);
    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        sum += recordset.ColumnInt64(KMPXTableDefaultIndex);
        }

    CleanupStack::PopAndDestroy(&recordset);

    if (err != KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }

    return sum;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    TUint32 aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, aValue);
    TUint32 ret(ExecuteSumQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    TUint32 aValue1,
    TUint32 aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + 2 * KMCIntegerLen);
    query->Des().Format(aQuery, aValue1, aValue2);
    TUint32 ret(ExecuteSumQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    TUint32 aValue1,
    TUint32 aValue2,
    TUint32 aValue3)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + 3 * KMCIntegerLen);
    query->Des().Format(aQuery, aValue1, aValue2, aValue3);
    TUint32 ret(ExecuteSumQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    const TDesC& aValue1,
    TUint32 aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    // replace single quotes
    // reserve double the value length for single quote duplicates
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value1 = HBufC::NewLC(aValue1.Length() * 2);
    TPtr value1Ptr(value1->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue1, value1Ptr);

    // construct the query
    HBufC* query = HBufC::NewLC(aQuery.Length() + value1Ptr.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, value1, aValue2);

    // execute the query
    TUint32 ret(ExecuteSumQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(value1);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    const TDesC& aValue1,
    const TDesC& aValue2)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    // replace single quotes
    // reserve double the value length for single quote duplicates
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value1 = HBufC::NewLC(aValue1.Length() * 2);
    TPtr value1Ptr(value1->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue1, value1Ptr);

    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value2 = HBufC::NewLC(aValue2.Length() * 2);
    TPtr value2Ptr(value2->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue2, value2Ptr);

    // construct the query
    HBufC* query = HBufC::NewLC(aQuery.Length() + value1Ptr.Length() + value2Ptr.Length());
    query->Des().Format(aQuery, value1, value2);

    // execute the query
    TUint32 ret(ExecuteSumQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(value2);
    CleanupStack::PopAndDestroy(value1);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    const TDesC& aValue)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    // replace single quotes
    // reserve double the value length for single quote duplicates
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value = HBufC::NewLC(aValue.Length() * 2);
    TPtr valuePtr(value->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue, valuePtr);

    // construct the query
    HBufC* query = HBufC::NewLC(aQuery.Length() + valuePtr.Length());
    query->Des().Format(aQuery, value);

    // execute the query
    TUint32 ret(ExecuteSumQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(value);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumQueryL(
    const TDesC& aQuery,
    const TDesC& aValue1,
    TUint32 aValue2,
    const TDesC& aValue3,
    TUint32 aValue4)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumQueryL");

    // replace single quotes
    // reserve double the value length for single quote duplicates
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value1 = HBufC::NewLC(aValue1.Length() * 2);
    TPtr value1Ptr(value1->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue1, value1Ptr);

    // replace single quotes
    // reserve double the value length for single quote duplicates
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value3 = HBufC::NewLC(aValue3.Length() * 2);
    TPtr value3Ptr(value3->Des());
    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue3, value3Ptr);

    // construct the query
    HBufC* query = HBufC::NewLC( aQuery.Length() +
                                 value1Ptr.Length() +
                                 2 * KMCIntegerLen +
                                 value3Ptr.Length() );
    query->Des().Format(aQuery, value1, aValue2, value3, aValue4);

    // execute the query
    TUint32 ret(ExecuteSumQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(value3);
    CleanupStack::PopAndDestroy(value1);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteSumExQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbTable::ExecuteSumExQueryL(
    const TDesC& aQuery, 
    TUint32 aPlaylistId, 
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbTable::ExecuteSumExQueryL");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMCIntegerLen);
    query->Des().Format(aQuery, aPlaylistId);  

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryOnAllDrivesL(aDrive, *query));
    CleanupClosePushL(recordset);

    TInt sum(0);
    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        sum += recordset.ColumnInt64(KMPXTableDefaultIndex);
        }
    
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(query);

    if (err != KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }

    return sum;
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    const TDesC& aQuery,
    TInt aValue1,
    TInt aValue2,
    TUint aQueryId )
    {
    MPX_FUNC("CMPXDbTable::ExecuteMediaOffsetQueryL");

    RSqlStatement& recordset(iDbManager.ExecuteSelectQueryL(aQueryId,
                                                            aValue1,
                                                            aValue2,
                                                            aQuery));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(recordset, aAttrs, *media);

        aMediaArray.AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        }
    CleanupStack::Pop(&recordset);

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::ExecuteMediaQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbTable::ExecuteMediaQueryL( const TArray<TMPXAttribute>& aAttrs,
                                               CMPXMediaArray& aMediaArray,
                                               const TDesC& aQuery,
                                               const TDesC& aValue1,
                                               TInt aValue2,
                                               TBool aAsc,
                                               TUint aQueryId )
    {
    MPX_DEBUG1("CMPXDbTable::ExecuteMediaAscQueryL  <--");

    RSqlStatement& recordset(iDbManager.ExecuteSelectQueryL(aQueryId,
                                                            aValue1,
                                                            aValue2,
                                                            aQuery));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(recordset, aAttrs, *media);

        if( aAsc )
            {
            aMediaArray.AppendL(*media);
            }
        else
            {
            aMediaArray.InsertL(*media, 0);
            }
        CleanupStack::PopAndDestroy(media);
        }

    CleanupStack::Pop(&recordset);

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }

    MPX_DEBUG1("CMPXDbTable::ExecuteMediaAscQueryL  -->");
    }

// ----------------------------------------------------------------------------
// CMPXDbTable::DoCheckTable
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXDbTable::DoCheckTable(
    RSqlDatabase& aDatabase,
    const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbTable::DoCheckTable");

    RSqlStatement statement;
    TInt err(statement.Prepare(aDatabase, aQuery));
    statement.Close();

    return (err == KErrNone);
    }


// End of File
