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
 * Description:  Implementation of applicationmanagement components
 *
*/

#ifndef __SCOMOADAPTERDB_H
#define __SCOMOADAPTERDB_H

// INCLUDE FILES

#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>
#include <SyncMLDef.h>

// CONSTANTS

// null final result value
const TInt KAMNullResult = 0;

// Database name & location
const TInt KAMAdapterDbDrive = EDriveC;
const TUid KAMPolicyUID =
    {
    0x20021335
    };
const TInt KAMMaxPkgURLLength( 2048);
const TInt KAMMaxPkgNameLength( 80);
const TInt KAMMaxPkgVersionLength( 80);

_LIT( KDBMSSecureID, "secure[20021335]" );

_LIT( KAMAdapterDbName, "c:TARMAppMgmtAdapterDB.db" );

// Table and column names
_LIT( KTableAMMgmtObject, "AMMgmtObject" );
_LIT( KAMMgmtObjectId, "Id" );
_LIT( KAMMgmtObjectProfileId, "ProfileId" );
_LIT( KAMMgmtObjectServerId, "ServerId" );
_LIT( KAMMgmtObjectRetryCount, "RetryCount");

// SQL statements
_LIT( KCreateAMMgmtObjectTable, "CREATE TABLE AMMgmtObject ( Id \
COUNTER NOT NULL,  ProfileId INTEGER, ServerId CHAR(%d), RetryCount INTEGER" );

_LIT( KGetAMMgmtObject, "SELECT * FROM AMMgmtObject WHERE Id = %d" );

_LIT( KGetUnfinishedAMMgmtObjectByServerId, "SELECT * FROM AMMgmtObject \
WHERE Id = %d AND ServerId = '%S'" );

_LIT( KDeleteAMMgmtObject,"DELETE FROM AMMgmtObject WHERE Id = %d");

// Max. lengths of string fields in AMMgmtObject
const TInt KMaxMgmtUriLength = 60;
const TInt KMaxServerIdLength = 150;
const TInt KMaxCorrelatorLength = 150;

// Estimate of url size used in max row size (OOD checks)
const TInt KPkgURLLengthEstimate = 350;

// space needed for above integers in buffer
const TInt KAMMgmtObjectIntLength = 4;

// space needed for an integer in buffer
const TInt KAMMgmtObjectIntegerLength = 10;

// Size of empty AM database in bytes (+ additional treshold)
const TInt KAMAdapterEmptyDbSize = 350;

// Estimated maximum size of AMMgmtObject table row (+ additional treshold)
const TInt KMaxAMMgmtObjectRowSize = KAMMaxPkgNameLength
        + KAMMaxPkgVersionLength + KPkgURLLengthEstimate + KMaxMgmtUriLength
        + KMaxServerIdLength + KMaxCorrelatorLength + 115;

// TYPEDEFS
typedef TInt TDmAMObjectId;
typedef RArray<TDmAMObjectId> RDmAMObjectArray;

_LIT(NCol1, "Id");
_LIT(NCol2, "LUID");
_LIT(NCol3, "ProfileId");
_LIT(NCol4, "ServerId");
_LIT(NCol5, "Correlator");
_LIT(NCol6, "SourceURI");
_LIT(NCol7, "RetryCount");

class CAMDbItem : public CBase
    {
public:
    static CAMDbItem* NewL();
    ~CAMDbItem();

    TInt id;
    TUint32 iLUID;
    TInt iProfileId;

    HBufC8* iServerID;
    HBufC8* iCorrelator;
    HBufC8* iSourceURI;
    TInt iRetryCount;

    };

// CLASS DECLARATION

/**
 *  CSCOMOAdapterDb provides an interface for accessing AM database.
 *  
 *  @lib SCOMOAdapter.lib
 *  
 */
class CSCOMOAdapterDb : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @return A pointer to the newly created instance.
     */
    static CSCOMOAdapterDb* NewL();

    /**
     * Two-phased constructor. Pushes the pointer onto the CleanupStack.
     * @return A pointer to the newly created instance.
     */
    static CSCOMOAdapterDb* NewLC();

    /**
     * Destructor.
     */
    virtual ~CSCOMOAdapterDb();

    void CreateTableL(RDbDatabase& aDatabase);

    void CreateandOpenL(TParse& name);

    TBool DeleteFromDatabaseL(TUint32 aLuid);

    void SaveToDatabaseL(TInt& aIndex, TUint32 aLuid, TInt aProfileid,
            TInt aRetryCount, const TDesC8& aSourceURI,
            const TDesC8& aServerid, const TDesC8& aCorrelator);

    void ReadItemsL(RPointerArray<CAMDbItem>& aItemArray, TDesC& aSqlQuery);

    void ReadDbItemsL(RPointerArray<CAMDbItem>& aItemArray);

    void GetEntryForServerIDL(RPointerArray<CAMDbItem>& aItemArray, TDesC8& aValue);
    
    void UpdateRetryCountForLuidL(TInt aLuid, TInt aRetryCount);
    
    TInt GetRetryCountForLuidL(TInt aLuid);
    
    void BeginDatabaseTransaction();

    void CommitDatabaseTransaction();
    
    void PrepareLuidQuery(TInt aLuid, TDes& aSqlQuery);


private:

    void ConstructL();

    CSCOMOAdapterDb();

private:

    RDbNamedDatabase iDatabase;
    RFs iFsSession;
    RDbs iRdbSession;

    RDbTable iAMObjectTable;
    CDbColSet* iColSet;
    RDbView iView;

    };

#endif // __AMADAPTERDB_H   
// End of File
