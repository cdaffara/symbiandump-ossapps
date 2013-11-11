/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Preset database used by the preset server
*
*/


#ifndef R_PSDATABASE_H
#define R_PSDATABASE_H

#include <d32dbms.h>

class CPSPendingPreset;
class MPSPresetObserver;

/**
 * Preset database used by the preset server.
 *
 * Handles all database related functionality related to saving and retrieving presets.
 */
NONSHARABLE_CLASS( RPSDatabase ) : public RDbNamedDatabase
    {

public:

    /**
     * Constructor.
     *
     * @param   aObserver   The observer to notify when the database is changed.
     */
    RPSDatabase( MPSPresetObserver& aObserver );

    /**
     * Opens an existing database or creates a new one if it doesn't exist already.
     *
     * The directory supplied is created if it doesn't exist.
     *
     * @param   aFs         File server session.
     * @param   aFileName   Full path and file name to the database.
     */
    void OpenL( RFs& aFs, const TDesC& aFileName );

    /**
     * Deletes presets from the database that have no data handler installed for them.
     *
     * @param   aDataHandlers       Currently installed data handlers.
     */
    void DeleteOrphanedPresetsL( const RArray<TInt>& aDataHandlers );

    /**
     * Gets a list of presets matching the supplied data handler.
     *
     * If <code>KNullUid</code> is supplied as the data handler, all presets from the database are retrieved.
     *
     * @param   aPresets        On return, the presets matching the data handler supplied.
     * @param   aDataHandler    Data handler for the presets, or <code>KNullUid</code> if all presets are to be retrieved.
     */
    void GetPresetListL( RArray<TInt>& aPresets, TUid aDataHandler );

    /**
     * Inserts a new preset into the database.
     *
     * The preset with the supplied data handler and index must not exist in the database,
     * or the call will leave with <code>KErrAlreadyExists</code>.
     *
     * @param   aIndex          Index of the preset.
     * @param   aDataHandler    Data handler of the preset.
     * @return  The ID of the created preset.
     */
    TInt CreatePresetL( TInt aIndex, TUid aDataHandler );

    /**
     * Deletes a preset from the database.
     *
     * @param   aId     ID of the preset to delete.
     */
    void DeletePresetL( TInt aId );
    
    /**
     * Commits a pending preset to the database.
     *
     * @param   aPreset     The preset to commit.
     */
    void CommitPresetL( const CPSPendingPreset& aPreset );

    /**
     * Moves a preset in the database.
     *
     * @param   aId                 ID of the preset to move.
     * @param   aDestinationIndex   Index to move the preset to.
     */
    void MovePresetL( TInt aId, TInt aDestinationIndex );

    /**
     * Returns the name of a preset.
     *
     * @param   aId     ID of the preset.
     * @return  Name of the preset, ownership is transferred to the caller.
     */
    HBufC* PresetNameL( TInt aId );

    /**
     * Returns the length of a preset's name.
     *
     * @param   aId     ID of the preset.
     * @return  Length of the preset's name.
     */
    TInt PresetNameLengthL( TInt aId );

    /**
     * Returns the index of a preset.
     *
     * @param   aId     ID of the preset.
     * @return  Index of the preset.
     */
    TInt PresetIndexL( TInt aId );

    /**
     * Returns the data handler of a preset.
     *
     * @param   aId     ID of the preset.
     * @return  Data handler of the preset.
     */
    TUid PresetDataHandlerL( TInt aId );

    /**
     * Returns the data of a preset.
     *
     * @param   aId     ID of the preset.
     * @return  Data of the preset, ownership is transferred to the caller.
     */
    HBufC8* PresetDataL( TInt aId );    

    /**
     * Returns the length of a preset's data.
     *
     * @param   aId     ID of the preset.
     * @return  Length of the preset's data.
     */
    TInt PresetDataLengthL( TInt aId );

private:

    /**
     * Returns the length of a column in the database.
     *
     * @param   aColumn     The column to return the length of.
     * @param   aId         ID of the preset.
     * @return  Length of the column.
     */
    TInt ColumnLengthL( const TDesC& aColumn, TInt aId );

    /**
     * Returns the current database version.
     *
     * @return  The current database version.
     */
    TInt CurrentDatabaseVersionL();

    /**
     * Updates the database version.
     *
     * @param   aVersion    Dtabase version to update to.
     */
    void UpdateDatabaseVersionL( TInt aVersion );

    /**
     * Runs all update functions on the database.
     */
    void UpdateDatabaseL();

    /**
     * Database version 1 update function.
     *
     * Creates the tables in the database as they were defined.
     */
    void CreateTablesL();

    /**
     * Prints database information to debug output.
     */
    void PrintDatabaseL();

private:

    /** The observer that is notified upon changes in the database. */
    MPSPresetObserver& iObserver;

    /** Database update function pointer. */
    typedef void (RPSDatabase::*KPSDatabaseUpdateFunction)();
    
    /** Supported database update functions. */
    static const KPSDatabaseUpdateFunction iDatabaseUpdateFunctions[];

    };

#endif // R_PSDATABASE_H
