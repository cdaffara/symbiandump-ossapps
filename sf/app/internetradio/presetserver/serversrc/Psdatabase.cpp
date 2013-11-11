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
* Description:  Implementation of the preset database
*
*/


#include <bautils.h>

#include "pscommon.h"
#include "psdebug.h"
#include "pspendingpreset.h"
#include "psdatabase.h"
#include "pspresetobserver.h"

// Uncomment the line below to get database print outs.
// Warning! This may cause a lot of debug messages, so use with caution.
// #define PS_DATABASE_PRINT_DEBUG

#ifdef PS_DATABASE_PRINT_DEBUG
    #define PS_PRINT_DATABASE  RPSDatabase::PrintDatabaseL()
#else
    #define PS_PRINT_DATABASE
#endif

const TInt KPSDatabaseVersion   = 1; // The latest version of the database. Must be kept up to date when adding new versions.

const TInt KPSDatabaseVersion1  = 1; // Database version 1 identifier.

// This array contains function pointers to all database update packages. One update corresponds to one function call, therefore
// when adding new database updates it is imperative that this array be updated as well.

const RPSDatabase::KPSDatabaseUpdateFunction RPSDatabase::iDatabaseUpdateFunctions[] =
    {
        &RPSDatabase::CreateTablesL
    };

// Table and columns literals.

_LIT( KPSPresetsTable,              "Presets" );
_LIT( KPSPresetsIdColumn,           "Id" );
_LIT( KPSPresetsNameColumn,         "Name" );
_LIT( KPSPresetsIndexColumn,        "Index" );
_LIT( KPSPresetsDataHandlerColumn,  "DataHandler" );
_LIT( KPSPresetsDataColumn,         "Data" );

_LIT( KPSVersionsTable,             "Versions" );
_LIT( KPSVersionsVersionColumn,     "Version" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
RPSDatabase::RPSDatabase( MPSPresetObserver& aObserver )
    : RDbNamedDatabase(), iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Opens the database.
// ---------------------------------------------------------------------------
//
void RPSDatabase::OpenL( RFs& aFs, const TDesC& aFileName )
    {
    Close(); // Ensure that the database is closed before trying to open it.

    BaflUtils::EnsurePathExistsL( aFs, aFileName );

    TInt err = RDbNamedDatabase::Create( aFs, aFileName );
    
    if ( err == KErrAlreadyExists )
        {
        // The database already existed, need to open it.
        User::LeaveIfError( RDbNamedDatabase::Open( aFs, aFileName ) );
        }
    else
        {
        User::LeaveIfError( err );
        }
        
    if ( IsDamaged() || !InTransaction() )
        {
        err = Recover();
        
        if ( !err )
            {
            err = Compact();
            }
        }

    if ( err )
        {
        Close();
        User::Leave( err );
        }

    // The database is now open and functional, check if the structure needs to be updated.
    TRAP( err, UpdateDatabaseL() )
    
    if ( err )
        {
        Destroy();
        User::Leave( err );
        }

    PS_PRINT_DATABASE;
    }

// ---------------------------------------------------------------------------
// Deletes all presets whose data handler is no longer available.
// ---------------------------------------------------------------------------
//
void RPSDatabase::DeleteOrphanedPresetsL( const RArray<TInt>& aDataHandlers )
    {
    PS_PRINT_DATABASE;

    _LIT( sql, "DELETE FROM %S" );
    _LIT( sqlWhere, " WHERE %S <> %d" );
    _LIT( sqlAnd, " AND %S <> %d" );

    TInt count = aDataHandlers.Count();

    // Constructs the query that selects all presets that have no registered data handlers.
    
    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsTable().Length() + count * KDefaultRealWidth +
                                  Min( 1, count ) * (sqlWhere().Length() + KPSPresetsDataHandlerColumn().Length()) +
                                  Max( 0, count - 1 ) * (sqlAnd().Length() + KPSPresetsDataHandlerColumn().Length()) );

    TPtr sqlPtr( sqlBuf->Des() );
    sqlPtr.Format( sql, &KPSPresetsTable );
    
    if ( count > 0 )
        {
        HBufC* tempBuf = HBufC::NewLC( sqlWhere().Length() + KPSPresetsDataHandlerColumn().Length() + KDefaultRealWidth );
        tempBuf->Des().Format( sqlWhere, &KPSPresetsDataHandlerColumn, aDataHandlers[0] );
        sqlPtr.Append( *tempBuf );
        CleanupStack::PopAndDestroy( tempBuf );

        for ( TInt i = 1; i < count; i++ )
            {
            tempBuf = HBufC::NewLC( sqlAnd().Length() + KPSPresetsDataHandlerColumn().Length() + KDefaultRealWidth );
            tempBuf->Des().Format( sqlAnd, &KPSPresetsDataHandlerColumn, aDataHandlers[i] );
            sqlPtr.Append( *tempBuf );
            CleanupStack::PopAndDestroy( tempBuf );
            }
        }

    TInt rowsDeleted = Execute ( sqlPtr );
    User::LeaveIfError(rowsDeleted);
    
    CleanupStack::PopAndDestroy( sqlBuf );

    if (rowsDeleted > 0)
    	{
        User::LeaveIfError( Compact() );
    	}

    PS_PRINT_DATABASE;
    }

// ---------------------------------------------------------------------------
// Gets all presets matching the supplied data handler.
// ---------------------------------------------------------------------------
//
void RPSDatabase::GetPresetListL( RArray<TInt>& aPresets, TUid aDataHandler )
    {
    HBufC* sqlBuf = NULL;

    if ( aDataHandler == KNullUid )
        {
        _LIT( sql, "SELECT %S FROM %S" );
        
        sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsIdColumn().Length() + KPSPresetsTable().Length() );
        
        sqlBuf->Des().Format( sql, &KPSPresetsIdColumn, &KPSPresetsTable );
        }
    else
        {
        _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );
        
        sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsIdColumn().Length() + KPSPresetsTable().Length() +
                                      KPSPresetsDataHandlerColumn().Length() + KDefaultRealWidth );
        
        sqlBuf->Des().Format( sql, &KPSPresetsIdColumn, &KPSPresetsTable, &KPSPresetsDataHandlerColumn, aDataHandler.iUid );
        }

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    CDbColSet* columns = view.ColSetL();
    TInt idColumn = columns->ColNo( KPSPresetsIdColumn );
    delete columns;

    for ( view.FirstL(); view.AtRow(); view.NextL() )
        {
        view.GetL();
        aPresets.AppendL( view.ColInt32( idColumn ) );
        }

    CleanupStack::PopAndDestroy( &view );                                  
    }

// ---------------------------------------------------------------------------
// Creates a preset.
// ---------------------------------------------------------------------------
//
TInt RPSDatabase::CreatePresetL( TInt aIndex, TUid aDataHandler )
    {
    PS_PRINT_DATABASE;

    _LIT( sql, "SELECT * FROM %S WHERE %S = %d AND %S = %d" );
    
    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsTable().Length() + KPSPresetsIndexColumn().Length() +
                                  KPSPresetsDataHandlerColumn().Length() + 2 * KDefaultRealWidth );
    
    sqlBuf->Des().Format( sql, &KPSPresetsTable, &KPSPresetsIndexColumn, aIndex, &KPSPresetsDataHandlerColumn, aDataHandler.iUid );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );
    User::LeaveIfError( view.EvaluateAll() );

    TInt id = KErrNotFound;
    
    if ( view.FirstL() )
        {
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        view.InsertL();
        
        CDbColSet* columns = view.ColSetL();
        TInt indexColumn = columns->ColNo( KPSPresetsIndexColumn );
        TInt dataHandlerColumn = columns->ColNo( KPSPresetsDataHandlerColumn );
        TInt idColumn = columns->ColNo( KPSPresetsIdColumn );
        delete columns;
        
        view.SetColL( indexColumn, aIndex );
        view.SetColL( dataHandlerColumn, aDataHandler.iUid );
        
        view.PutL();
        
        id = view.ColInt32( idColumn );
        
        iObserver.HandlePresetChangedL( id, aDataHandler, MPSPresetObserver::EPSCreated );
        }
        
    CleanupStack::PopAndDestroy( &view );

    PS_PRINT_DATABASE;

    return id;
    }

// ---------------------------------------------------------------------------
// Deletes a preset.
// ---------------------------------------------------------------------------
//
void RPSDatabase::DeletePresetL( TInt aId )
    {
    PS_PRINT_DATABASE;

    _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );

    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsTable().Length() + KPSPresetsDataHandlerColumn().Length() +
                                  KPSPresetsIdColumn().Length() + KDefaultRealWidth );
    sqlBuf->Des().Format( sql, &KPSPresetsDataHandlerColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    User::LeaveIfError( view.EvaluateAll() );

    if ( view.FirstL() )
        {
        view.GetL();
        CDbColSet* columns = view.ColSetL();
        TUid uid = TUid::Uid( view.ColInt32( columns->ColNo( KPSPresetsDataHandlerColumn ) ) );
        delete columns;
        view.DeleteL();
        iObserver.HandlePresetChangedL( aId, uid, MPSPresetObserver::EPSDeleted );
        }

    CleanupStack::PopAndDestroy( &view );

    PS_PRINT_DATABASE;
    }

// ---------------------------------------------------------------------------
// Commits a preset to the database.
// ---------------------------------------------------------------------------
//
void RPSDatabase::CommitPresetL( const CPSPendingPreset& aPreset )
    {
    PS_PRINT_DATABASE;

    _LIT( sql, "SELECT * FROM %S WHERE %S = %d" );
    
    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsTable().Length() + KPSPresetsIdColumn().Length() + KDefaultRealWidth );
    sqlBuf->Des().Format( sql, &KPSPresetsTable, &KPSPresetsIdColumn, aPreset.Id() );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    User::LeaveIfError( view.EvaluateAll() );
    
    if ( view.FirstL() )
        {
        CDbColSet* columns = view.ColSetL();
        CleanupStack::PushL( columns );

        view.UpdateL();

        RDbColWriteStream stream;

        stream.OpenLC( view, columns->ColNo( KPSPresetsNameColumn ) );
        stream.WriteL( aPreset.Name() );
        stream.CommitL();

        CleanupStack::PopAndDestroy( &stream );

        view.SetColL( columns->ColNo( KPSPresetsIndexColumn ), aPreset.Index() );
        
        stream.OpenLC( view, columns->ColNo( KPSPresetsDataColumn ) );
        stream.WriteL( aPreset.Data() );
        stream.CommitL();
        CleanupStack::PopAndDestroy( &stream );

        view.PutL();

        TUid uid = TUid::Uid( view.ColInt32( columns->ColNo( KPSPresetsDataHandlerColumn ) ) );

        CleanupStack::PopAndDestroy( columns );
        
        iObserver.HandlePresetChangedL( aPreset.Id(), uid, MPSPresetObserver::EPSModified );
        }

    CleanupStack::PopAndDestroy( &view );
    
    PS_PRINT_DATABASE;
    }

// ---------------------------------------------------------------------------
// Moves a preset to the supplied index.
// ---------------------------------------------------------------------------
//
void RPSDatabase::MovePresetL( TInt aId, TInt aDestinationIndex )
    {
    PS_PRINT_DATABASE;

    if ( aDestinationIndex < 0 )
        {
        User::Leave( KErrArgument );
        }

    // First, fetch the index and data handler of the preset that is about to be moved.

    _LIT( selectPreset, "SELECT %S, %S FROM %S WHERE %S = %d" );
    
    HBufC* selectPresetBuf = HBufC::NewLC( selectPreset().Length() + KPSPresetsIndexColumn().Length() + KPSPresetsDataHandlerColumn().Length() + 
                                           KPSPresetsTable().Length() + KPSPresetsIdColumn().Length() + KDefaultRealWidth );
    
    selectPresetBuf->Des().Format( selectPreset, &KPSPresetsIndexColumn, &KPSPresetsDataHandlerColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );
    
    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *selectPresetBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( selectPresetBuf );
    CleanupClosePushL( view );
    
    if ( !view.FirstL() )
        {
        User::Leave( KErrNotFound );
        }

    view.GetL();
    CDbColSet* columns = view.ColSetL();
    TInt sourceIndex = view.ColInt32( columns->ColNo( KPSPresetsIndexColumn ) );
    TUid dataHandler = TUid::Uid( view.ColInt32( columns->ColNo( KPSPresetsDataHandlerColumn ) ) );
    delete columns;
    CleanupStack::PopAndDestroy( &view );

    TInt minIndex = Min( sourceIndex, aDestinationIndex );
    TInt maxIndex = Max( sourceIndex, aDestinationIndex );
    
    TBool direction = ( aDestinationIndex > sourceIndex ); // EFalse means that the preset is to be moved up, ETrue means that it is to be moved down.

    _LIT( asc, "ASC" );
    _LIT( desc, "DESC" );

    TPtrC sortPtr( direction ? desc() : asc() );
    
    // Selects all preset in between the source and destination indices.
     _LIT( sql, "SELECT * FROM %S WHERE %S >= %d AND %S <= %d AND %S = %d ORDER BY %S %S" );
    
    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsIndexColumn().Length() * 3 + KPSPresetsTable().Length() +
                                  KPSPresetsDataHandlerColumn().Length() + sortPtr.Length() + KDefaultRealWidth * 3 );

    sqlBuf->Des().Format( sql, &KPSPresetsTable, &KPSPresetsIndexColumn, minIndex, &KPSPresetsIndexColumn,
                          maxIndex, &KPSPresetsDataHandlerColumn, dataHandler.iUid, &KPSPresetsIndexColumn, &sortPtr );

    User::LeaveIfError( view.Prepare( *this, *sqlBuf ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    User::LeaveIfError( view.EvaluateAll() );

    TInt lastIndex = aDestinationIndex;

    if ( view.LastL() ) // Handle the preset that is to be moved first.    
        {
        view.GetL();
        view.UpdateL();
        
        columns = view.ColSetL();
        CleanupStack::PushL( columns );
        
        view.SetColL( columns->ColNo( KPSPresetsIndexColumn ), aDestinationIndex );
        view.PutL();
        
        iObserver.HandlePresetChangedL( view.ColInt32( columns->ColNo( KPSPresetsIdColumn ) ),
                                        TUid::Uid( view.ColInt32( columns->ColNo( KPSPresetsDataHandlerColumn ) ) ),
                                        MPSPresetObserver::EPSModified );

        view.FirstL(); // Start iterating the presets from the beginning (the last element is always the preset that is to be moved).

        for ( TInt i = 0; i < view.CountL() - 1; i++ ) // The preset that was moved has already been handled.
            {
            view.GetL();
            view.UpdateL();
            
            TInt index = view.ColInt32( columns->ColNo( KPSPresetsIndexColumn ) );

            if ( lastIndex != index )
                {
                // If there was no preset with the index that the previous preset was moved to, there's no need to continue.
                /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
                break;
                /*lint -restore*/
                }
            else 
                {
                // Preset was moved on top of another preset, continue iterating through the presets.
                index += direction ? -1 : 1; // If moving up, all preset indices are incremented by one, otherwise they are decremented by one.
                lastIndex = index;
                view.SetColL( columns->ColNo( KPSPresetsIndexColumn ), index );
                view.PutL();

                iObserver.HandlePresetChangedL( view.ColInt32( columns->ColNo( KPSPresetsIdColumn ) ),
                                                TUid::Uid( view.ColInt32( columns->ColNo( KPSPresetsDataHandlerColumn ) ) ),
                                                MPSPresetObserver::EPSModified );
                }

            view.NextL();
            }
        
        CleanupStack::PopAndDestroy( columns );
        }

    CleanupStack::PopAndDestroy( &view );

    PS_PRINT_DATABASE;
    }

// ---------------------------------------------------------------------------
// Returns the name of a preset.
// ---------------------------------------------------------------------------
//
HBufC* RPSDatabase::PresetNameL( TInt aId )
    {
    _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );

    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsNameColumn().Length() + KPSPresetsTable().Length() +
                                  KPSPresetsIdColumn().Length() + KDefaultRealWidth );

    sqlBuf->Des().Format( sql, &KPSPresetsNameColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    TInt nameLength = PresetNameLengthL( aId );
    HBufC* name = HBufC::NewLC( nameLength );

    if ( view.FirstL() )
        {
    
        view.GetL();
        CDbColSet* columns = view.ColSetL();
        TInt nameColumn = columns->ColNo( KPSPresetsNameColumn );
        delete columns;

        RDbColReadStream stream;
        TPtr namePtr = name->Des();
        stream.OpenLC( view, nameColumn );
        stream.ReadL( namePtr, nameLength );
        CleanupStack::PopAndDestroy( &stream);
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::Pop( name );    
    CleanupStack::PopAndDestroy( &view );

    return name;
    }

// ---------------------------------------------------------------------------
// Returns the length of the name field for a preset.
// ---------------------------------------------------------------------------
//
TInt RPSDatabase::PresetNameLengthL( TInt aId )
    {
    return ColumnLengthL( KPSPresetsNameColumn, aId );
    }

// ---------------------------------------------------------------------------
// Returns the index of a preset.
// ---------------------------------------------------------------------------
//
TInt RPSDatabase::PresetIndexL( TInt aId )
    {
    _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );

    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsIndexColumn().Length() + KPSPresetsTable().Length() +
                                  KPSPresetsIdColumn().Length() + KDefaultRealWidth );

    sqlBuf->Des().Format( sql, &KPSPresetsIndexColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    TInt index = KErrNotFound;

    if ( view.FirstL() )
        {
        view.GetL();
        CDbColSet* columns = view.ColSetL();
        index = view.ColInt32( columns->ColNo( KPSPresetsIndexColumn ) );
        delete columns;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( &view );

    return index;
    }

// ---------------------------------------------------------------------------
// Returns the data handler of a preset.
// ---------------------------------------------------------------------------
//
TUid RPSDatabase::PresetDataHandlerL( TInt aId )
    {
    _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );

    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsDataHandlerColumn().Length() + KPSPresetsTable().Length() +
                                  KPSPresetsIdColumn().Length() + KDefaultRealWidth );

    sqlBuf->Des().Format( sql, &KPSPresetsDataHandlerColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    TUid dataHandler = KNullUid;

    if ( view.FirstL() )
        {
        view.GetL();
        CDbColSet* columns = view.ColSetL();
        dataHandler.iUid = view.ColInt32( columns->ColNo( KPSPresetsDataHandlerColumn ) );
        delete columns;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( &view );

    return dataHandler;
    }

// ---------------------------------------------------------------------------
// Returns the binary data of a preset.
// ---------------------------------------------------------------------------
//
HBufC8* RPSDatabase::PresetDataL( TInt aId )
    {
    _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );

    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + KPSPresetsDataColumn().Length() + KPSPresetsTable().Length() +
                                  KPSPresetsIdColumn().Length() + KDefaultRealWidth );

    sqlBuf->Des().Format( sql, &KPSPresetsDataColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    HBufC8* data = NULL;

    if ( view.FirstL() )
        {
        view.GetL();
        CDbColSet* columns = view.ColSetL();
        TInt columnNo = columns->ColNo( KPSPresetsDataColumn );
        delete columns;
        
        data = HBufC8::NewLC( view.ColLength( columnNo ) );
        TPtr8 ptr( data->Des() );
        
        RDbColReadStream stream;
        stream.OpenLC( view, columnNo );
        stream.ReadL( ptr, view.ColLength( columnNo ) );
        CleanupStack::PopAndDestroy( &stream );
        CleanupStack::Pop( data );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( &view );

    return data;
    }

// ---------------------------------------------------------------------------
// Returns the length of the binary data of a preset.
// ---------------------------------------------------------------------------
//
TInt RPSDatabase::PresetDataLengthL( TInt aId )
    {
    return ColumnLengthL( KPSPresetsDataColumn, aId );
    }

// ---------------------------------------------------------------------------
// Returns the current database version number.
// ---------------------------------------------------------------------------
//
TInt RPSDatabase::CurrentDatabaseVersionL()
    {
    TInt version = 0;

    RDbTable table;
    TInt err = table.Open( *this, KPSVersionsTable, RDbTable::EReadOnly );
    
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );

        CleanupClosePushL( table );

        if ( table.FirstL() )
            {
            table.GetL();

            CDbColSet* columns = table.ColSetL();
            version = table.ColInt32( columns->ColNo( KPSVersionsVersionColumn ) );
            delete columns;
            }

        CleanupStack::PopAndDestroy( &table );
        }

    return version;
    }

// ---------------------------------------------------------------------------
// Updates the database version number.
// ---------------------------------------------------------------------------
//
void RPSDatabase::UpdateDatabaseVersionL( TInt aVersion )
    {
    RDbTable table;
    User::LeaveIfError( table.Open( *this, KPSVersionsTable ) );
    CleanupClosePushL( table );
    
    CDbColSet* columns = table.ColSetL();
    CleanupStack::PushL( columns );

    table.Reset();
    table.FirstL() ? table.UpdateL() : table.InsertL(); // If a row exists open it for update, otherwise create a new one.
    table.SetColL( columns->ColNo( KPSVersionsVersionColumn ), aVersion );
    table.PutL();

    CleanupStack::PopAndDestroy( 2, &table );
    }

// ---------------------------------------------------------------------------
// Updates the whole database to the latest version.
// ---------------------------------------------------------------------------
//
void RPSDatabase::UpdateDatabaseL()
    {
    TInt currentVersion = CurrentDatabaseVersionL();

    if ( currentVersion < KPSDatabaseVersion )
        {
        for ( TInt i = currentVersion; i < KPSDatabaseVersion; i++ )
            {
            ((*this).*iDatabaseUpdateFunctions[i])(); // All of these function calls can leave.
            }
        }
    }

// ---------------------------------------------------------------------------
// Creates the tables.
// ---------------------------------------------------------------------------
//
void RPSDatabase::CreateTablesL()
    {
    // Creates the preset table.
    TDbCol presetsIdColumn( KPSPresetsIdColumn, EDbColInt32 );
    presetsIdColumn.iAttributes = TDbCol::EAutoIncrement;

    CDbColSet* columns = CDbColSet::NewLC();
    columns->AddL( presetsIdColumn );
    columns->AddL( TDbCol( KPSPresetsNameColumn, EDbColLongText ) );
    columns->AddL( TDbCol( KPSPresetsIndexColumn, EDbColInt32 ) );
    columns->AddL( TDbCol( KPSPresetsDataHandlerColumn, EDbColInt32 ) );
    columns->AddL( TDbCol( KPSPresetsDataColumn, EDbColLongText8 ) );

    User::LeaveIfError( CreateTable( KPSPresetsTable, *columns ) );
    
    CleanupStack::PopAndDestroy( columns );

    // This is a hack to circumvent problems with Publish & Subscribe, as it initializes all data properties to zero,
    // causing a notification to fire with the value of zero. This is the reason why most users of the API have defined
    // zero to be the uninitialized state of the property. The problem caused by this is that the DBMS API's auto increment
    // field starts from zero, and should there exist a preset with such an ID, it could not be properly conveyed to the
    // interested parties. Thus, we will make a dummy preset now that the preset table is created and remove it instantly,
    // thereby making the auto increment column to start from one instead of zero.
    
    _LIT( selectAll, "SELECT * FROM %S" );

    HBufC* sqlBuf = HBufC::NewLC( selectAll().Length() + KPSPresetsTable().Length() );
    sqlBuf->Des().Format( selectAll, &KPSPresetsTable );
    
    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );
    User::LeaveIfError( view.EvaluateAll() );

    view.FirstL();    
    view.InsertL();
    view.PutL();
    view.DeleteL();

    CleanupStack::PopAndDestroy( &view );

    // Creates the version table.
    columns = CDbColSet::NewLC();
    columns->AddL( TDbCol( KPSVersionsVersionColumn, EDbColInt32 ) );
    
    User::LeaveIfError( CreateTable( KPSVersionsTable, *columns ) );
    
    CleanupStack::PopAndDestroy( columns );
    
    UpdateDatabaseVersionL( KPSDatabaseVersion1 ); // Version 1 of the database successfully created.
    }

// ---------------------------------------------------------------------------
// Returns the length of a column.
// ---------------------------------------------------------------------------
//
TInt RPSDatabase::ColumnLengthL( const TDesC& aColumn, TInt aId )
    {
    _LIT( sql, "SELECT %S FROM %S WHERE %S = %d" );

    TInt length = KErrNotFound;

    HBufC* sqlBuf = HBufC::NewLC( sql().Length() + aColumn.Length() + KPSPresetsTable().Length() + KPSPresetsIdColumn().Length() + KDefaultRealWidth );
    sqlBuf->Des().Format( sql, &aColumn, &KPSPresetsTable, &KPSPresetsIdColumn, aId );

    RDbView view;
    User::LeaveIfError( view.Prepare( *this, *sqlBuf, RDbView::EReadOnly ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( view );

    if ( view.FirstL() )
        {
        view.GetL();
        CDbColSet* columns = view.ColSetL();
        length = view.ColLength( columns->ColNo( aColumn ) );
        delete columns;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( &view );

    return length;
    }
    
// ---------------------------------------------------------------------------
// Debug method that prints the contents of the database.
// ---------------------------------------------------------------------------
//
void RPSDatabase::PrintDatabaseL()
    {
    PSDEBUG( "RPSDatabase::PrintDatabaseL() - Versions:" );

    RDbTable table;
    TInt err = table.Open( *this, KPSVersionsTable, RDbTable::EReadOnly );
    
    if ( err == KErrNotFound )
        {
        PSDEBUG( "\tYarr, thar database be not in this world, mate!" );
        }
    else
        {
        User::LeaveIfError( err );

        CleanupClosePushL( table );

        if ( table.FirstL() )
            {
            CDbColSet* columns = table.ColSetL();
            TInt versionColumn = columns->ColNo( KPSVersionsVersionColumn );
            delete columns;
            
            do  
                {
                table.GetL();
                PSDEBUG2( "\t%d", table.ColInt32( versionColumn ) );
                } while ( table.NextL() );
            }
        else
            {
            PSDEBUG( "\tYarr, thar table be empty!" );
            }

        CleanupStack::PopAndDestroy( &table );
        }
    
    PSDEBUG( "RPSDatabase::PrintDatabaseL() - Presets:" );

    err = table.Open( *this, KPSPresetsTable, RDbTable::EReadOnly );
    
    if ( err == KErrNotFound )
        {
        PSDEBUG( "\tYarr, thar database be not in this world, mate!" );
        }
    else
        {
        User::LeaveIfError( err );
        
        CleanupClosePushL( table );

		if ( table.FirstL() )
		    {
            CDbColSet* columns = table.ColSetL();
            TInt idColumn = columns->ColNo( KPSPresetsIdColumn );
            TInt dataHandlerColumn = columns->ColNo( KPSPresetsDataHandlerColumn );
            TInt indexColumn = columns->ColNo( KPSPresetsIndexColumn );
            TInt nameColumn = columns->ColNo( KPSPresetsNameColumn );
            delete columns;
            
            do  
                {
                table.GetL();
                // stream is needed for long columns
                RDbColReadStream stream;
                TInt nameLength = PresetNameLengthL( table.ColInt32( idColumn ) );
                HBufC* name = HBufC::NewLC( nameLength );
                TPtr namePtr = name->Des();
                stream.OpenLC( table, nameColumn );
                stream.ReadL( namePtr, nameLength );

                CleanupStack::PopAndDestroy( &stream );
                
                PSDEBUG5( "\tId: %10u\tData Handler: 0x%x\tIndex: %10u\tName: %S", table.ColInt32( idColumn ), table.ColInt32( dataHandlerColumn ), table.ColInt32( indexColumn ), name );
                
                CleanupStack::PopAndDestroy( name );
                } while ( table.NextL() );
		    }
        else
            {
            PSDEBUG( "\tYarr, thar table be empty!" );
            }
    
        CleanupStack::PopAndDestroy( &table );
        }
    }

// ======== GLOBAL FUNCTIONS ========
