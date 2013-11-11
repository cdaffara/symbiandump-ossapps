/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Fota adapter's DB header file
*
*/





#ifndef __NSMLDMFOTAADAPTERDB_H
#define __NSMLDMFOTAADAPTERDB_H

// INCLUDE FILES

#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>
#include <SyncMLDef.h>
#include <fotaengine.h>
#include <fotaConst.h>

// CONSTANTS

// null final result value
const TInt KNSmlFotaNullResult = 0;

// Database name & location
const TInt KNSmlFotaAdapterDbDrive = EDriveC;
_LIT( KNSmlFotaAdapterDbName, "c:nsmldmfotaadapter.db" );

// Table and column names
_LIT( KNSmlTableFwMgmtObject,       "FwMgmtObject" );
_LIT( KNSmlFwMgmtObjectId,          "PkgId" );
_LIT( KNSmlFwMgmtObjectName,        "PkgName" );
_LIT( KNSmlFwMgmtObjectVersion,     "PkgVersion" );
_LIT( KNSmlFwMgmtObjectUrl,         "PkgURL" );
_LIT( KNSmlFwMgmtObjectMgmtUri,     "MgmtURI" );
_LIT( KNSmlFwMgmtObjectProfileId,   "ProfileId" );
_LIT( KNSmlFwMgmtObjectServerId,    "ServerId" );
_LIT( KNSmlFwMgmtObjectResult,      "FinalResult" );
_LIT( KNSmlFwMgmtObjectCorrelator,  "Correlator" );

// SQL statements
_LIT( KNSmlCreateFwMgmtObjectTable, "CREATE TABLE FwMgmtObject ( PkgId \
COUNTER NOT NULL, PkgName CHAR(%d), PkgVersion CHAR(%d), PkgURL LONG VARCHAR, \
MgmtURI CHAR(%d), ProfileId INTEGER, ServerId CHAR(%d), FinalResult INTEGER, \
Correlator CHAR(%d) )" );

_LIT( KNSmlGetFwMgmtObject, "SELECT * FROM FwMgmtObject WHERE PkgId = %d" );

_LIT( KNSmlGetUnfinishedFwMgmtObjects, "SELECT * FROM FwMgmtObject \
WHERE FinalResult = %d" );

_LIT( KNSmlGetUnfinishedFwMgmtObjectByServerId, "SELECT * FROM FwMgmtObject \
WHERE FinalResult = %d AND ServerId = '%S' AND NOT (MgmtURI = '')" );

_LIT( KNSmlDeleteFwMgmtObject,"DELETE FROM FwMgmtObject WHERE PkgId = %d");

// Max. lengths of string fields in FwMgmtObject
const TInt KNSmlMaxMgmtUriLength = 60;
const TInt KNSmlMaxServerIdLength = 150;
const TInt KNSmlMaxCorrelatorLength = 150;

// Estimate of url size used in max row size (OOD checks)
const TInt KNSmlPkgURLLengthEstimate = 350;

// space needed for above integers in buffer
const TInt KNSmlFwMgmtObjectIntLength = 4;

// space needed for an integer in buffer
const TInt KNSmlFwMgmtObjectIntegerLength = 10;

// Size of empty Fota database in bytes (+ additional treshold)
const TInt KNSmlFotaAdapterEmptyDbSize = 350;

// Estimated maximum size of FwMgmtObject table row (+ additional treshold)
const TInt KNSmlMaxFwMgmtObjectRowSize = KFotaMaxPkgNameLength + 
                                         KFotaMaxPkgVersionLength + 
                                         KNSmlPkgURLLengthEstimate + 
                                         KNSmlMaxMgmtUriLength + 
                                         KNSmlMaxServerIdLength + 
                                         KNSmlMaxCorrelatorLength + 115;

// TYPEDEFS
typedef TInt TNSmlDmFwObjectId;
typedef RArray<TNSmlDmFwObjectId> RNSmlDmFwObjectArray;



// CLASS DECLARATION

/**
*  CNSmlDmFotaAdapterDb provides an interface for accessing Fota database.
*  
*  @lib nsmldmfotaadapter.lib
*  
*/
class CNSmlDmFotaAdapterDb : public CBase
    {

public:
        
    /**
    * Two-phased constructor.
    * @return A pointer to the newly created instance.
    */
    static CNSmlDmFotaAdapterDb* NewL();    

    /**
    * Two-phased constructor. Pushes the pointer onto the CleanupStack.
    * @return A pointer to the newly created instance.
    */
    static CNSmlDmFotaAdapterDb* NewLC();

    /**
    * Destructor.
    */
    virtual ~CNSmlDmFotaAdapterDb();

    /**
    * Adds a new row (i.e. firmware object) to the FwMgmtObject table.
    * The values in each column are left NULL, except FinalResult to 
    * KNSmlFotaNullResult. 
    * Leaves with KErrDiskFull, if OOD check fails.
    * @return The id of the new fw object in Fota database.
    */
    TNSmlDmFwObjectId AddFwObjectL();
    
    /**
    * Deletes a firmware object from FwMgmtObject table.
    * @param aId The identifier of the object to be deleted.
    * @return KErrNone if the object was successfully deleted, KErrNotFound if 
    * the object was not found from database.
    */
    TInt DeleteFwObjectL( const TNSmlDmFwObjectId aId );
    
    /**
    * Finds all the firmware objects that have null final result and sets
    * the ids of those objetcs to aArray.
    * @param aArray that on return contains the ids.
    */
    void GetEmptyFinalResultsL( RNSmlDmFwObjectArray& aArray );
    
    /**
    * Finds all the firmware objects that have null final result, server id 
    * equals to given server id and mgmt uri is not empty. Sets the ids of 
    * those objetcs to aArray.
    * @param aArray Array that on return contains the ids.
    * @param aServerId The id of the server whose empty final result values 
    * are listed.
    */
    void GetEmptyFinalResultsL( RNSmlDmFwObjectArray& aArray, 
                                const TDesC8& aServerId );
    
    /**
    * Fetches the PkgName field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in PkgName field of the object, if object was found.
    * NULL otherwise.
    */
    HBufC8* PkgNameL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the PkgVersion field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in PkgVersion field of the object, if object was found.
    * NULL otherwise.
    */
    HBufC8* PkgVersionL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the PkgURL field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in PkgURL field of the object, if object was found.
    * NULL otherwise.
    */
    HBufC8* PkgUrlL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the MgmtURI field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in MgmtURI field of the object, if object was found.
    * NULL otherwise.
    */
    HBufC8* MgmtUriL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the ProfileId field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in ProfileId field of the object, if object was found.
    * KErrNotFound otherwise.
    */
    TSmlProfileId ProfileIdL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the ServerId field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in ServerId field of the object, if object was found.
    * NULL otherwise.
    */
    HBufC8* ServerIdL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the FinalResult field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in FinalResult field of the object, if object was 
    * found. KErrNotFound otherwise.
    */
    TInt FinalResultL( const TNSmlDmFwObjectId aId );
    
    /**
    * Fetches the Correlator field of a firmware object from database.
    * @param aId The identifier of the object, whose data should be fetched.
    * @return The value in Correlator field of the object, if object was 
    * found. NULL otherwise.
    */
    HBufC8* CorrelatorL( const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the value of PkgName field of a firmware object.
    * @param aName The new value for the field.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found. KErrOverflow, if given name is too large for the 
    * database.
    */
    TInt SetPkgNameL( const TDesC8& aName, const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the value of PkgVersion field of a firmware object.
    * @param aVersion The new value for the field.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found. KErrOverflow, if given version is too large 
    * for the database.
    */
    TInt SetPkgVersionL( const TDesC8& aVersion, const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the value of PkgURL field of a firmware object.
    * @param aUrl The new value for the field.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found. KErrOverflow, if given url is too large for the 
    * database.
    */
    TInt SetPkgUrlL( const TDesC8& aUrl, const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the value of MgmtURI field of a firmware object.
    * @param aUri The new value for the field.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found. KErrOverflow, if given uri is too large for the 
    * database.
    */
    TInt SetMgmtUriL( const TDesC8& aUri, const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the values of ProfileId and ServerId fields of a firmware object.
    * @param aProfile The new value for the ProfileId.
    * @param aServerId The new value for the ServerId.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found. KErrOverflow, if given server id is too large 
    * for the database.
    */
    TInt SetServerInfoL( const TSmlProfileId aProfile, 
                         const TDesC8& aServerId,
                         const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the value of FinalResult field of a firmware object.
    * @param aResult The new value for the field.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found.
    */
    TInt SetFinalResultL( const TInt aResult, const TNSmlDmFwObjectId aId );
    
    /**
    * Sets the value of Correlator field of a firmware object.
    * @param aCorrelator The new value for the field.
    * @param aId The identifier of the object, whose data should be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found. KErrOverflow, if given correlator is too large 
    * for the database.
    */
    TInt SetCorrelatorL( const TDesC8& aCorrelator, 
                         const TNSmlDmFwObjectId aId );

private:

    /**
    * Creates Fota database.
    * @param aFullName Name and path of database.
    */
    void CreateDatabaseL( const TDesC& aFullName );

    /**
    * Sets new value to a text field in a firmware object. This method is used 
    * by the public descriptor setters.
    * @param aColumn The name of the field to be updated.
    * @param aValue The new value for the field.
    * @param aObject The identifier of the object, whose data should be 
    * updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found.
    */
    TInt SetStrValueL( const TDesC& aColumn, 
                       const TDesC8& aValue, 
                       const TNSmlDmFwObjectId aObject );

    /**
    * Writes using Db streams the new value to a long text field in a 
    * firmware object. This method is used by the public descriptor 
    * setters (PkgURL).
    * @param aColumn The name of the field to be updated.
    * @param aValue The new value for the field.
    * @param aObject The identifier of the object, whose data should 
    * be updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if 
    * the object was not found.
    */
    TInt SetLongStrValueL( const TDesC& aColumn,
                           const TDesC8& aValue, 
                           const TNSmlDmFwObjectId aObject );

    /**
    * Sets new value to a integer field in a firmware object. This method 
    * is used by the public integer setters.
    * @param aColumn The name of the field to be updated.
    * @param aValue The new value for the field.
    * @param aObject The identifier of the object, whose data should be 
    * updated.
    * @return KErrNone, if the update was successful. KErrNotFound, if the 
    * object was not found.
    */
    TInt SetIntValueL( const TDesC& aColumn,
                       const TInt aValue, 
                       const TNSmlDmFwObjectId aObject );

    /**
    * Fetches the value in a text field of a firmware object. This method is 
    * used by the public descriptor getters.
    * @param aColumn The name of the field where the data should be fetched.
    * @param aObject The identifier of the object, whose data should be 
    * fetched.
    * @return The value in the field, if object was found. NULL otherwise.
    */
    HBufC8* StrValueL( const TDesC& aColumn, const TNSmlDmFwObjectId aObject );

    /**
    * Fetches the value in a long text field of a firmware object using Db 
    * streams. This method is used by the public descriptor getters (PkgURL).
    * @param aColumn The name of the field where the data should be fetched.
    * @param aObject The identifier of the object, whose data should be 
    * fetched.
    * @return The value in the field, if object was found. NULL otherwise.
    */
    HBufC8* LongStrValueL( const TDesC& aColumn, 
                           const TNSmlDmFwObjectId aObject );

    /**
    * Fetches the value in a integer field of a firmware object. This method 
    * is used by the public integer getters.
    * @param aColumn The name of the field where the data should be fetched.
    * @param aObject The identifier of the object, whose data should be 
    * fetched.
    * @return The value in the field, if object was found. KErrNotFound 
    * otherwise.
    */
    TInt IntValueL( const TDesC& aColumn, const TNSmlDmFwObjectId aObject );

    /**
    * Prepares iView with read-only access rights to a single firmware object.
    * @param aObject The identifier of the object, whose data is about 
    * to be read.
    */
    void FetchRowL( const TNSmlDmFwObjectId aObject );
    
    /**
    * Prepares iView with update access rights to a single firmware object. 
    * @param aObject The identifier of the object, whose data is about to 
    * be updated.
    * @return KErrNone, if there were no errors and iView was successfully
    * prepared. 
    * KErrNotFound, if the firmware object was not found.
    */
    TInt UpdateRowL( const TNSmlDmFwObjectId aObject );
    
    /**
    * Makes an SQL query string for getting a row from FwMgmtObject table.
    * @param aId The identifier of the object, which is used in the SQL query.
    * @return A string containing the formatted SQL query.
    */
    HBufC* FwMgmtObjectRowSqlLC( TNSmlDmFwObjectId aId ) const;

    /**
    * Closes the view and then prepares it with the given values.
    * @param aSql SQL statement defining the view, which is prepared by 
    * this method.
    * @param aAccess Permitted operations for the view.
    */
    void PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess );

    /**
    * Commits update and compacts the database.
    */
    void CommitAndCompact();

    /**
    * Converts a 8-bit descriptor to a 16-bit descriptor, which is allocated 
    * from heap and pushed onto the CleanupStack.
    * @param aDes Descriptor to be converted.
    * @return A pointer to the newly allocated copy.
    */
    HBufC* ConvertTo16BitLC( const TDesC8& aDes ) const;

    /**
    * Second phase construction.
    */
    void ConstructL();

    /**
    * Constructor.
    */
    CNSmlDmFotaAdapterDb();

private:

    RDbNamedDatabase iDatabase;
    RFs iFsSession;
    RDbs iRdbSession;

    RDbTable iFwObjectTable;
    CDbColSet* iColSet;
    RDbView iView;

    };


#endif // __NSMLDMFOTAADAPTERDB_H   
            
// End of File
