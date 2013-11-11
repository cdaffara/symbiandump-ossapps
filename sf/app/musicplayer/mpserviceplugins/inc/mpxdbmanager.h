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
* Description:  This class is responsible for managing all of music collection
*                databases.
*
*/


#ifndef MPXDBMANAGER_H
#define MPXDBMANAGER_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <sqldb.h>
#include <badesca.h>

// FORWARD DECLARATIONS

class MMPXTable;
class RSqlStatement;

// CONSTANTS

const TInt KDbManagerAllDrives = 0;

// CLASS DECLARATION

/**
* Generic class responsible for managing databases on multiple drives.
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbManager :
    public CBase
    {
    protected: //   Constructors

        /**
        * C++ default constructor
        * @param aFs file server session
        */
        IMPORT_C CMPXDbManager(RFs& aFs);

        /**
        * The second phase constructor to safely construct things
        * that can leave
        * @param aDatabaseFile database filename
        */
        IMPORT_C void ConstructL(const TFileName& aDatabaseFile);

    public:

        /**
        * Destructor
        */
        IMPORT_C virtual ~CMPXDbManager();

    public: // New methods

        /**
        * Checks if all databases have been initialized.
        * @return ETrue if initialized, EFalse otherwise
        */
        IMPORT_C TBool IsInitialized();

        /**
        * Begins a transaction on all databases.
        */
        IMPORT_C void BeginL();

        /**
        * Commits a transaction on all databases.
        */
        IMPORT_C void CommitL();

        /**
        * Rolls back a transaction on all databases.
        */
        IMPORT_C void RollbackL();

        /**
        * Checks if the database is currently in a transaction
        *
        * @return ETrue if database is currently in a transaction, EFalse otherwise
        */
        IMPORT_C TBool InTransaction();

        /**
        * Tries to create and open the databases on all specified drives.
        * If a drive is not valid (like an MC not plugged in) it will be skipped.
        * @param aDrives array of drives to create and open databases on.
        */
        IMPORT_C void InitDatabasesL(RArray<TInt> aDrives);

        /**
        * Opens a specified database.
        * @param aDrive identifies the drive index of the database to open
        */
        IMPORT_C void OpenDatabaseL(TInt aDrive);

        /**
        * Closes a specified database.
        * @param aDrive identifies the drive index of the database to close
        */
        IMPORT_C void CloseDatabaseL(TInt aDrive);

        /**
        * Closes all open databases.
        */
        IMPORT_C void CloseAllDatabases();

        /**
        * Opens all open databases.
        */
        IMPORT_C void OpenAllDatabasesL();

        /**
        * Checks if the database on a specified drive is open.
        * @param aDrive identifies the drive index of the database to check
        * @return ETrue if the database is open, EFalse otherwise
        */
        IMPORT_C TBool IsOpen(TInt aDrive) const;

        /**
        * Returns the number of currently open databases.
        * @return Number of databases
        */
        IMPORT_C TInt DatabaseCount() const;

        /**
        * Returns the drive corresponding to a give index.
        * @param aIndex identifies the index in the list of drives the database uses
        * @return Drive index the database uses.
        */
        IMPORT_C TInt DbDrive(TInt aIndex) const;

        /**
        * Recreate a specified database.
        * @param aDrive identifies the drive index ro recreate the database
        */
        IMPORT_C void RecreateDatabaseL(TInt aDrive);

        /**
        * Recreate all databases.
        */
        IMPORT_C void RecreateAllDatabasesL();

        /**
        * Return current DB version
        * @return the version of db structure
        */
        IMPORT_C TVersion Version() const;

        /**
        * Return current DB version
        * @return the version of db structure
        */
        IMPORT_C void RegisterTableL(MMPXTable& aTable);

        /**
        * Executes a select query with variable number of parameters on all
        * available drives.
        * @param aFmt query format string
        * @return resulted result set
        */
        IMPORT_C RSqlStatement ExecuteSelectQueryL(TRefByValue<const TDesC> aFmt, ...);

        /**
        * Executes a select query with variable number of parameters
        * against a specified drive
        * @param aDrive to execute query on
        * @param aFmt query format string
        * @return resulted result set
        */
        IMPORT_C RSqlStatement ExecuteSelectQueryL(TInt aDrive, TRefByValue<const TDesC> aFmt,
            ...);

        /**
        * Executes a select query with variable number of parameters
        * on all available drives. Two values will be bound to the statement
        * @param aStatementId unique id of a statement to bind to
        *                     creates a new statement if the id is not found
        * @param aFirstValue TInt Value to bind as first argument
        * @param aSecondValue TInt Value to bind as second argument
        * @param aFmt query format string
        * @return resulted result set. The ownership is not passed so the caller should
        *         not destroy this as it may be reused in future calls.
        */
        IMPORT_C RSqlStatement& ExecuteSelectQueryL( TUint aStatementId,
                                                     TInt aFirstValue, TInt aSecondValue,
                                                     TRefByValue<const TDesC> aFmt, ...);

        /**
        * Executes a select query with variable number of parameters
        * on all available drives. Two values will be bound to the statement
        * @param aStatementId unique id of a statement to bind to
        *                     creates a new statement if the id is not found
        * @param aFirstValue TDesC Value to bind as first argument
        * @param aSecondValue TInt Value to bind as second argument
        * @param aFmt query format string
        * @return resulted result set. The ownership is not passed so the caller should
        *         not destroy this as it may be reused in future calls.
        */
        IMPORT_C RSqlStatement& ExecuteSelectQueryL( TUint aStatementId,
                                                     const TDesC& aFirstValue,
                                                     TInt aSecondValue,
                                                     TRefByValue<const TDesC> aFmt, ...);
        /**
        * Executes a query that does not return a record set
        * @param aDrive to execute query on
        * @param aFmt query format string
        */
        IMPORT_C void ExecuteQueryL(TInt aDrive, TRefByValue<const TDesC> aFmt, ...);

        /**
        * Prints the contents of all the databases to the log file
        * @param aDrive to execute query on
        */
        IMPORT_C void PrintDatabaseL();

        /**
        * Returns the file server session.
        * @return file server session
        */
        IMPORT_C RFs& Fs();

        /**
        * Checks if the space on the specified drive(s) is above the critical level
        * @param aDrive drive ID or KDbManagerAllDrives
        * @leave KErrDiskFull if the space on the specified drive(s) is below the
        *        critical level
        */
        IMPORT_C void CheckDiskSpaceL(TInt aDrive);

        /**
        * Completely recreate all databases.
        */
        IMPORT_C void RegenerateAllDatabasesL();
        
        /**
         * Checks if the spefified drive is a remove drive
         */      
        IMPORT_C TBool IsRemoteDrive(TDriveNumber aDrive);

    protected:

        /**
        * Creates a database on a specified drive.
        * @param aDrive identifies the database
        */
        IMPORT_C void CreateDatabaseL(TInt aDrive);

        /**
        * Remove the database
        * @param aDrive identifies the database
        */
        IMPORT_C void RemoveDatabaseL(TInt aDrive);

        /**
        * Create the all tables
        * @param aDatabase the database to add tables to
        */
        IMPORT_C void CreateTablesL(RSqlDatabase& aDatabase);

        /**
        * Drop all tables
        * @param aDatabase the database to drop tables from
        */
        IMPORT_C void DropTablesL(RSqlDatabase& aDatabase);
        
    public:
    	
        /**
        * Executes a select query with variable number of parameters
        * against a specified drive
        * @param aDrive to execute query on
        * @param aFmt query format string
        * @return resulted result set
        */
        IMPORT_C RSqlStatement ExecuteSelectQueryOnAllDrivesL(TInt aDrive, TRefByValue<const TDesC> aFmt,
            ...);

        /**
        * Copy all databases from RAM disk back to normal drive, E, F,...
        * 
        * @return none
        */
        IMPORT_C void CopyDBsFromRamL(); 

        /**
        * Copy all databases to RAM disk back from normal drive, E, F,...
        * 
        * @return none
        */
        IMPORT_C void CopyDBsToRamL( TBool aIsMTPInUse = EFalse);
        

        /**
        * Check if RAM disk is enough to operatte. If not, DBs will be copied back to drives.
        *
        * @return TInt index to the database handler
        */
        IMPORT_C void EnsureRamSpaceL() ;

        /**
        *  Move DBs from RAMDisk to disks
        */
        //IMPORT_C void BackupDBsL();

    private:

        /**
        * Find available RAMDISK
        * @return error code
        */
        TInt GetRAMDiskPath();

        /**
        * Check if RAM disk is available to copy.
        *
        * @return ETrue if there is enough space, EFalse otherwise
        */
        TBool IsRamDiskSpaceAvailable();
        
        /**
        * To block a diskspace so that it can gurantee for a write back from RAM disk
        *
        * @return ETrue if the dummy file is created successfully, EFalse otherwise
        */
        TBool BlockDiskSpace( TInt aIndex, TBool aIsMTPInUse = EFalse );
        
        /**
        * To copy db from regular drive to RAM
        *
        * @return ETrue if succeed 
        * 
        * No-operation if fails
        */
        TBool DoCopyDBToRam( TInt aIndex, TBool aIsMTPInUse );

        /**
        * To copy db back regular drive from RAM
        *
        */
        void DoCopyDBFromRam( TInt aIndex );
    
        /**
        * To replace dummy file with new content
        * Writes over previous dummy file without freeing disk space
        */
        void ReplaceFileL( const TDesC& aSrcName, const TDesC& aDstName );
        	
        /**
        * To calculate necessary file size of the dummy file
        *
        * @return TInt64 estimated file size
        */
        TInt64 CalculateInitalDummyDBSize( const TVolumeInfo& aVol, TInt aOrigDbSize, TBool aIsMTPInUse = EFalse);
        
        /**
        * Get database index by giving drive index
        *
        * @return TInt index to the database handler
        */
        TInt GetDatabaseIndex(TInt aDrive);

        /**
         * Sum up the total size in bytes of the databases.
         * 
         * @param aSize - On return, the total size of the databases.
         * @return TInt System error.
         */
        TInt GetTotalDatabasesSize(TInt& aSize);

        /**
         * Sum up the total size in bytes of the databases on the RAM drive.
         * 
         * @param aSize - On return, the total size of the databases on the RAM drive.
         * @return TInt System error.
         */
        TInt GetTotalRamDatabasesSizeL(TInt& aSize);

        /**
        * Remove dummy file
        *
        * @return TInt index to the database handler
        */
        void RemoveDummyFile( TInt index );

        /**
        * Check if disksapce is enough to operatte. If not, it leaves with KErrDiskFull
        *
        */
        void EnsureDiskSpaceL(TInt aDrive) ;
        
        /** 
         * Begin transaction, leaves on error
         */
        void DoBeginL();
        
        /** 
         * Commit transaction, leaves on error
         */
        void DoCommitL();

        /**
        * Create full path and filename on a specified drive unit.
        * @param aDrive identifies the drive unit
        */
        HBufC* CreateFullFilenameL(TDriveUnit aDrive);

    protected:  // Types

        typedef struct
            {
            TInt iDrive;
            TBool iOpen;
            HBufC* iAliasname;
#ifdef __RAMDISK_PERF_ENABLE 
            TBool iUseRAMdb;
            HBufC* iOrigFullFilePath;
            HBufC* iTargetFullFilePath;
            TFileName iDummyFilePath;
#endif //__RAMDISK_PERF_ENABLE 
            } DatabaseHandle;

    protected:  // Data

        HBufC* iDbFile;
        RArray<MMPXTable*> iTables;
        RSqlDatabase iDatabase;
        RArray<DatabaseHandle> iDatabaseHandles;

    private:

        /**
        * Create the all tables
        * @param aDatabase the database to add tables to
        * @param aCorrupt specifies whether the database is corrupted or not
        */
        void CreateTablesL(RSqlDatabase& aDatabase, TBool aCorrupt);

        /**
        * Opens root database at C-drive.
        */
        void OpenRootDatabaseL();

        /**
        * Creates a specified database.
        * @param aDrive identifies the drive unit of the database to create
        */
        void CreateDatabaseL(TDriveUnit aDrive);

        /**
        * Attached a specified database.
        * @param aIndex  Index to iDatabaseHandles
        */
        void AttachDatabaseL(TInt aIndex);

        /**
        * Detach a specified database.
        * @param aIndex  Index to iDatabaseHandles
        */
        void DetachDatabaseL(TInt aIndex);

        /**
        * Open database
        * @param aIndex Index to iDatabaseHandles
        */
        void OpenDatabaseAtIndexL( TInt aIndex );
        
        /**
        * Close database
        * @param aIndex Index to iDatabaseHandles
        */
        void CloseDatabaseAtIndexL(TInt aIndex);

        /**
        * Create filename on a specified drive unit.
        * @param aDrive identifies the drive unit
        */
        HBufC* CreateFilenameL(TDriveUnit aDrive);


        /**
        * Replaces specially formatted query symbols with a specified alias.
        * @param aAlias identifies the alias name to use in query
        */
        void ReplaceDriveAlias(TDes& aQuery, const TDesC& aAlias);

        /**
        * Replaces specially formatted query symbols with a specified alias.
        * @param aAlias identifies the alias name to use in query
        * @param aToKen replace by aAlias
        */
        void ReplaceDriveAlias(TDes& aQuery, const TDesC& aAlias, const TDesC& aToKen);

        /**
        * Removes specially formatted query symbols from the query
        * @param aQuery identifies the query to remove specially formatted symbols
        */
        void RemoveDriveAlias(TDes& aQuery);
        
        /**
        * Removes specially formatted query symbols from the query
        * @param aQuery identifies the query to remove specially formatted symbols
        * @param aToKen identifies need remove string
        */
        void RemoveDriveAlias(TDes& aQuery, const TDesC& aToKen);

        /**
        * Attempts to recreate database by dropping and creating tables
        * used inside RecreateDatabaseL
        * @param aFilename database filename
        */
        void DoRecreateDatabaseL(HBufC * aFilename);

        /**
         * Executes SQL statement against the database.
         *
         * @param aDatabase database handle
         * @param aStatement sql statement buffer
         * @return KErrNone if successfull, error code otherwise.
         *
         */
        TInt ExecuteSqlStatement(RSqlDatabase& aDatabase, const TDesC& aStatement);

        /**
        * Formats the query.
        * @param aFmt query format
        * @param aList variable parameter list
        * @return formatted query, the ownership is transferred.
        */
        HBufC* FormatQueryLC(TRefByValue<const TDesC> aFmt, VA_LIST aList);

        /**
        * Executes an SQL query on all drives
        * @param aQuery query string
        * @return prepared SQL statement
        */
        RSqlStatement ExecuteSelectQueryOnAllDrivesL(TPtr aQuery);
        
        /**
        * Executes an SQL query on all drives
        * @param aDrive drive to execute the query on
        * @param aQuery query string
        * @return prepared SQL statement
        */
        RSqlStatement ExecuteSelectQueryOnAllDrivesL(TInt aDrive,TPtr aQuery);

        /**
        * Executes an SQL query on a specified drive
        * @param aDrive drive to execute the query on
        * @param aQuery query string
        * @return prepared SQL statement
        */
        RSqlStatement ExecuteSelectQueryOnDriveL(TInt aDrive, TPtr aQuery);

        /**
        * Prepares an sql query given the statment
        * @param aStatementId unique identifier of the statement
        * @param aFmt query format string
        * @param aList variable parameter list
        * @return reference to the SQL statement
        */
        RSqlStatement& PrepareQueryL( TUint aStatementId,
                                      TRefByValue<const TDesC> aFmt,
                                      VA_LIST aList );

        /**
        * Resets all prepared queries
        */
        void ResetPreparedQueries();

        /**
        * Create Database
        */
        void DoCreateDatabaseL( TDriveUnit aDrive );
        
#ifdef _DEBUG

        /**
        * Returns the number of columns from a specified SQL statement
        * @param aStatement identifies the SQL query to check
        */
        TInt GetColumnCountL(RSqlStatement& aStatement);

        /**
        * Prints the table of results from a specified SQL query to the debug log
        * @param aStatement identifies the SQL query made
        */
        void PrintTableValuesL(RSqlStatement& aStatement);

        /**
        * Finds all the tables on the main or attached drives
        * @param aAlias identifies the alias name to use if a attached drive KNullDesC if main drive
        * @param aTableName identifies the array of table names on the main or attached drive
        */
        void FindAllTablesL(const TDesC& aAlias, RArray<HBufC*>& aTableName);

        /**
        * Prints the table on the main or attached drives
        * @param aAlias identifies the alias name to use if a attached drive KNullDesC if main drive
        * @param aTableName identifies the table name on the main or attached drive
        */
        void PrintTableL(const TDesC& aAlias, const TDesC& aTableName);

#endif
    private:

        /*
        * Structure to hold the state of a sql statement
        */
        NONSHARABLE_STRUCT( TSqlStatementState )
            {
            TBool iPrepared;
            TUint iId;
            };

    private:

        TBool iInitialized;
        TInt iTransactionCount;
        RFs& iFs;

        RArray<TSqlStatementState> iPreparedStatements;
        RPointerArray<RSqlStatement> iStatements;

        // Defined for RAM disk performance
        TBool                   iRAMDiskPerfEnabled;  // flag to indicate RAM disk feature is enabled from cenrep.
        TUint64                 iMaximumAllowedRAMDiskSpaceToCopy; // maximum number of megabytes allow to do RAM disk operation.
        TFileName               iRAMFolder;
        TChar                   iRAMDrive;
        TBool                   iRAMInUse;
        TInt64                  iEstimatedDBSizes;
    };

#endif  // MPXDBMANAGER_H
