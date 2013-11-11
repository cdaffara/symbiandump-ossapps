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
* Description:  Container store utils implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreUtils.h"
#include "ContainerStoreDeleteHandler.h"

#include <sysutil.h>
#include <d32dbms.h>  // database
#include <bautils.h>

//#include <s32crypt.h>

// =============
// LOCAL CLASSES
// =============

// =============================================================================
// CLASS: CCompactionHandler
// 
// This class performs database compaction operations incrementally using the
// RDbIncremental class.  Each active object invocation performs one step of the
// compaction.  
//
// The Symbian documentation explains that certain operations cannot be performed
// on the database while an incremental operation is in progress.  The Suspend
// and Resume functions are provided to suspend incremental operations until the
// database operation is completed.
//
// The wonderful Symbian documentation doesn't give a list of the functions that
// do not work during incremental operations.  In order to determine this, the
// code was temporarily modified to always have an incremental operation in
// progress, unless Suspend and Resume has been called.  The unit tester were
// executed, and each operation that had a problem was then wrapped with
// Suspend/Resume, and the code was then returned to its original state.
// =============================================================================
class CCompactionHandler : public CActive
    {
    public:

        // ==============
        // PUBLIC METHODS
        // ==============
            
        static CCompactionHandler* NewL( RDbDatabase& aDatabase,
                                         TInt         aPriority );  
                                               
        virtual ~CCompactionHandler();
    
        void Compact();
        
        void Suspend();
        
        void SuspendLC();
        
        void Resume( TBool aPopCleanupItem = ETrue );
        
        void FinishCompactionL();
        
    private:
    
        // ===============
        // PRIVATE METHODS
        // ===============
            
        CCompactionHandler( RDbDatabase& aDatabase,
                            TInt         aPriority );
        
        void ConstructL();
        
        void Reschedule();
    
		// inherited from CActive
		virtual void RunL();
		virtual void DoCancel();
	
	    RTimer         iDelayTimer;
	    RDbDatabase&   iDatabase;
	    RDbIncremental iIncremental;
	    TInt           iStep;
	    
	    TBool          iCompactionNeeded;
	    TInt           iSuspendCount;
	    
	    __LOG_DECLARATION
    
    }; // end class CCompactionHandler

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CContainerStoreUtils* CContainerStoreUtils::NewL(  TDriveNumber    aDriveNumber, 
                                                   TInt            aCompactionPriority, 
                                                   const TDesC&    aDbFilename,
                                                   CDeleteHandler& aDeleteHandler )
    {
    CContainerStoreUtils* self = new(ELeave) CContainerStoreUtils( aDriveNumber, 
                                                                   aCompactionPriority, 
                                                                   aDbFilename,
                                                                   aDeleteHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    } // end NewL
    
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreUtils::CContainerStoreUtils( TDriveNumber    aDriveNumber, 
                                            TInt            aCompactionPriority, 
                                            const TDesC&    aDbFilename,
                                            CDeleteHandler& aDeleteHandler ) :
    iDriveNumber( aDriveNumber ),
    iCompactionPriority( aCompactionPriority ),
    iDbFilename( aDbFilename ),
    iDeleteHandler( aDeleteHandler )
    {
#ifdef _DEBUG		
    iLowDiskSpaceLatency = -1;
#endif    
    } // end constructor

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CContainerStoreUtils::ConstructL()
    {
    __LOG_CONSTRUCT( "msg", "CContainerStoreUtils" )
    
    User::LeaveIfError( iFs.Connect() );
    
    iFs.CreatePrivatePath( iDriveNumber );
    iFs.SetSessionToPrivate( iDriveNumber ); 
    
    iFs.PrivatePath( iPrivatePath );
    const TUint bufSize = 3;
    TBuf<bufSize> driveLetter;
    _LIT( KFormatString, "%C:" ); 
    driveLetter.Format( KFormatString, (iDriveNumber - EDriveA + 'A' ) );
    iPrivatePath.Insert( 0, driveLetter );
    
	} // end ConstructL
    
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreUtils::~CContainerStoreUtils()
    {
    delete iCompactionHandler;
    
    iTables.Close();
    
    iDatabase.Close();
    
    delete iFileStore;
    
    iFs.Close(); 
    
    __LOG_DESTRUCT
    } // end destructor

// ==========================================================================
// FUNCTION: LeaveIfFalseL
// ==========================================================================
void CContainerStoreUtils::LeaveIfFalse( TBool aCondition, TInt aLeaveCode )
{
	if ( !aCondition )
	{
		User::Leave( aLeaveCode );
	}
}


// ==========================================================================
// FUNCTION: FileSystem
// ==========================================================================
RFs& CContainerStoreUtils::FileSystem()
    {
    return iFs;
    } // end FileSystem
    
// ==========================================================================
// FUNCTION: CreateDatabaseL
// ==========================================================================
void CContainerStoreUtils::CreateDatabaseL()
	{
	__LOG_ENTER( "CreateDatabaseL" );

    // Create the file store.
	iFileStore = CPermanentFileStore::ReplaceL( iFs, iDbFilename, EFileRead|EFileWrite|EFileWriteDirectIO );
	
	iFileStore->SetTypeL( iFileStore->Layout() );	
	
	// Create a database within the file store.
	TStreamId id = iDatabase.CreateL( iFileStore );
	
	// Set the database as the file store root object.
	iFileStore->SetRootL( id );
	
	// Commit the database.
	iFileStore->CommitL();

    iCompactionHandler = CCompactionHandler::NewL( iDatabase, iCompactionPriority );
        
	__LOG_EXIT
	} // end CreateDatabaseL
	
// ==========================================================================
// FUNCTION: OpenDatabaseL
// ==========================================================================
void CContainerStoreUtils::OpenDatabaseL()
	{
	__LOG_ENTER( "OpenDatabaseL" )

	// construct a file store object
	iFileStore = CPermanentFileStore::OpenL( iFs, iDbFilename, EFileRead|EFileWrite|EFileWriteDirectIO );
	
	// open database from the root of the store
	iDatabase.OpenL( iFileStore, iFileStore->Root() );	
	
	if( iDatabase.IsDamaged() )
		{
		__LOG_WRITE_ERROR( "Recovering database" );
		User::LeaveIfError( iDatabase.Recover() );
		} // end if
	
    iCompactionHandler = CCompactionHandler::NewL( iDatabase, iCompactionPriority );
	iCompactionHandler->Compact();
	
 	__LOG_EXIT
	} // end OpenDatabaseL

// ==========================================================================
// FUNCTION: CloseDatabaseL
// ==========================================================================
void CContainerStoreUtils::CloseDatabaseL()
	{
	__LOG_ENTER( "CloseDatabaseL" )
	
    // Finish compaction, if necessary.
    if( iCompactionHandler )
        {        
        iCompactionHandler->FinishCompactionL();
        } // end if
    
	CloseDatabase();
	
	__LOG_EXIT
	} // end CloseDatabaseL

// ==========================================================================
// FUNCTION: CloseDatabase
// ==========================================================================
void CContainerStoreUtils::CloseDatabase()
    {
    __LOG_ENTER( "CloseDatabase" )
    
    delete iCompactionHandler;
    iCompactionHandler = NULL;
    
    iDatabase.Close();
    
    delete iFileStore;
    iFileStore = NULL;
    
    __LOG_EXIT
    } // end CloseDatabase

// ==========================================================================
// FUNCTION: PopulateViewL
// ==========================================================================
void CContainerStoreUtils::PopulateViewL( RDbView& aView, const TDesC& aSqlQuery, RDbRowSet::TAccess aAccess )
    {
    __LOG_ENTER( "PopulateViewL" )
    
    User::LeaveIfError( aView.Prepare( iDatabase, TDbQuery(aSqlQuery), aAccess ) );
    
    // Evaluate the view (i.e. populate it based on the SQL query).
	User::LeaveIfError( aView.EvaluateAll() );
	
	__LOG_EXIT
    } // end PopulateViewL
    
// ==========================================================================
// FUNCTION: Execute
// ==========================================================================
void CContainerStoreUtils::Execute( const TDesC &aSql, TDbTextComparison aComparison )
	{
    __LOG_ENTER( "Execute" )
	
    iDatabase.Execute( aSql, aComparison );
    
	__LOG_EXIT
	}


// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreUtils::CreateTableL( const TDesC& aTableName, CDbColSet& aColSet )
    {
    User::LeaveIfError( iDatabase.CreateTable( aTableName, aColSet ) );
    } // end CreateTableL

// ==========================================================================
// FUNCTION: CreateIndexL
// ==========================================================================
void CContainerStoreUtils::CreateIndexL( const TDesC& aName, const TDesC& aTable, const CDbKey& aKey )
    {
    User::LeaveIfError( iDatabase.CreateIndex( aName, aTable, aKey ) );    
    } // end CreateIndexL

// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreUtils::OpenTableL( RDbTable& aTable, const TDesC& aTableName )
    {
    User::LeaveIfError( aTable.Open( iDatabase, aTableName ) );
    
    iTables.AppendL( &aTable );
    
    } // end OpenTableL

// ==========================================================================
// FUNCTION: CloseTable
// ==========================================================================
void CContainerStoreUtils::CloseTable( RDbTable& aTable )
    {
    aTable.Close();
    
    TInt index = iTables.Find( &aTable );
    if( index != KErrNotFound )
        {        
        iTables.Remove( index );
        } // end if
    
    } // end CloseTable
    
// ==========================================================================
// FUNCTION: ReadLongColumnL
// ==========================================================================
void CContainerStoreUtils::ReadLongColumnL( RDbRowSet& aRowSet, 
                                            TUint      aColNum, 
                                            RBuf8&     aBuffer,
                                            TUint      aBufferPadSpace )
    {
    __LOG_ENTER_SUPPRESS( "ReadLongColumnL" )
    
    iCompactionHandler->SuspendLC();
    
	RDbColReadStream readStream;
	
	TUint colLength = aRowSet.ColLength( aColNum );
	
	TUint desiredLength = colLength + aBufferPadSpace;
	
	if( aBuffer.MaxLength() < desiredLength )
		{
		__LOG_WRITE_INFO( "growing buffer" )
		aBuffer.ReAllocL( desiredLength );		
		} // end if
	
	aBuffer.SetLength( 0 );
	readStream.OpenLC( aRowSet, aColNum );
	readStream.ReadL( aBuffer, colLength );
	
	CleanupStack::PopAndDestroy( &readStream ); 
	
	iCompactionHandler->Resume();
	
    } // end ReadLongColumnL

// ==========================================================================
// FUNCTION: WriteLongColumnL
// ==========================================================================
void CContainerStoreUtils::WriteLongColumnL( RDbRowSet&    aRowSet,
                               	             TUint         aColNum, 
                                      	     const TDesC8& aBuffer )
    { 
    __LOG_ENTER( "WriteLongColumnL" )
    
    if ( aBuffer.Length() > 0 )
        {
    	RDbColWriteStream writeStream;
    	
    	LeaveIfLowDiskSpaceL( aBuffer.Length() );
    	
    	writeStream.OpenLC( aRowSet, aColNum );
    	writeStream.WriteL( aBuffer );
    	writeStream.CommitL();
    
    	CleanupStack::PopAndDestroy( &writeStream ); 
        }
    else
        {
        aRowSet.SetColNullL( aColNum );
        }
	__LOG_EXIT
    } // end WriteLongColumnL

// ==========================================================================
// FUNCTION: LeaveIfLowDiskSpaceL
// ==========================================================================
void CContainerStoreUtils::LeaveIfLowDiskSpaceL( TUint aBytesToWrite )
	{
	__LOG_ENTER_SUPPRESS( "LeaveIfLowDiskSpaceL" )
	
	// This safety margin is to account for some level of uncertainty in aBytesToWrite due to overhead
	// in the database, file system, encryption, etc.
	const TUint KSafetyMargin = 8*1024;

	__LOG_WRITE8_FORMAT1_INFO( "LeaveIfLowDiskSpace(%i)", aBytesToWrite )

	TBool belowCritical = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aBytesToWrite+KSafetyMargin, iDriveNumber );

#ifdef _DEBUG
    TBool belowCriticalOverride = EFalse;    
    if( iLowDiskSpaceLatency > -1 )
        {
        belowCriticalOverride = (iLowDiskSpaceLatency == 0);        
        belowCritical = belowCritical || belowCriticalOverride;
        iLowDiskSpaceLatency--;
        } // end if
#endif

    if( belowCritical )    
        {
        // Force the deletes and database compaction to complete in the foreground.  This may take
        // a while, but may free up enough space to fix this condition.

        // : It was found that the following call will fail if a database row update is in progress (calls
        //       such as FirstL assert in those cases).  The cleanup mechanism would need to be refined in order
        //       to avoid that situation.
        // iDeleteHandler.FinishDeletes();
        
        iCompactionHandler->FinishCompactionL();        

	    belowCritical = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aBytesToWrite+KSafetyMargin, iDriveNumber );

#ifdef _DEBUG
        belowCritical = belowCritical || belowCriticalOverride;
#endif

        } // end if

	if( belowCritical )
		{
		__LOG_WRITE_ERROR( "below critical disk space!" );	
		User::Leave( KErrNoMemory );
		} // end if
	
	} // end LeaveIfLowDiskSpaceL

// ==========================================================================
// FUNCTION: PrivatePath
// ==========================================================================
const TDesC& CContainerStoreUtils::PrivatePath() const
    {
    return iPrivatePath;
    } // end PrivatePath
    
// ==========================================================================
// FUNCTION: BeginDatabaseTransactionLC
// ==========================================================================
void CContainerStoreUtils::BeginDatabaseTransactionLC()
	{
	__LOG_ENTER( "BeginDatabaseTransactionL" )
	
	// A database transaction cannot be started while an incremental operation is in progress, so
	// suspend it now.
	iCompactionHandler->Suspend();
	
	TUint result = iDatabase.Begin();
	
	if( result != KErrNone )
		{
		__LOG_WRITE8_FORMAT1_ERROR( "failed, err=%i", result );
		User::Leave( result );
		} // end if
		
    // Push an item on the cleanup stack taht will rollback the database transaction
    // if something fails during the transaction.		
	CleanupStack::PushL(TCleanupItem(&RollbackDatabaseTransactionL, this) );

    __LOG_EXIT
	} // end BeginDatabaseTransactionL

// ==========================================================================
// FUNCTION: CommitDatabaseTransactionL
// ==========================================================================
void CContainerStoreUtils::CommitDatabaseTransactionL()
	{
	__LOG_ENTER( "CommitDatabaseTransactionL" )
	
	User::LeaveIfError( iDatabase.Commit() );

    // Pop the rollback item.	
	CleanupStack::Pop( this );

	iCompactionHandler->Resume( EFalse );
	
	iCompactionHandler->Compact();
	
    __LOG_EXIT
	} // end CommitDatabaseTransactionL
	
// ==========================================================================
// FUNCTION: RollbackDatabaseTransactionL
// ==========================================================================
void CContainerStoreUtils::RollbackDatabaseTransactionL()
	{
	__LOG_ENTER( "RollbackDatabaseTransaction" )

	iDatabase.Rollback();
	
	// Rollbacks can damage the database indexes.  This will fix them.
	User::LeaveIfError( iDatabase.Recover() );

    // All rowsets must be reset after a Rollback, otherwise row functions will leave with
    // KErrNotReady.
    for( TInt index = 0; index < iTables.Count(); index++ )
        {
        iTables[index]->Reset();
        } // end for
			
	iCompactionHandler->Resume( EFalse );
	iCompactionHandler->Compact();
	
	__LOG_EXIT
	} // end RollbackDatabaseTransactionL

// ==========================================================================
// FUNCTION: RollbackDatabaseTransactionL
// ==========================================================================
void CContainerStoreUtils::RollbackDatabaseTransactionL( TAny* aObject )
	{
	__LOG_STATIC_ENTER( "msg", "RollbackDatabaseTransactionL" )	
	
	CContainerStoreUtils* utils = reinterpret_cast<CContainerStoreUtils*>(aObject);	
	utils->RollbackDatabaseTransactionL();
	
	__LOG_STATIC_EXIT
	} // end RollbackDatabaseTransaction
	
// ==========================================================================
// FUNCTION: SuspendCompactionLC
// ==========================================================================
void CContainerStoreUtils::SuspendCompactionLC()
	{
	iCompactionHandler->SuspendLC();
	}

// ==========================================================================
// FUNCTION: ResumeCompaction
// ==========================================================================
void CContainerStoreUtils::ResumeCompaction()
	{
	iCompactionHandler->Resume();	
	}

// ==========================================================================
// FUNCTION: FindFirstEncryptedOrUnencryptedL
// ==========================================================================
TBool CContainerStoreUtils::FindFirstEncryptedOrUnencryptedL( RDbTable&    aTable, 
                                                             const TDesC& aColName, 
                                                             TBool        aFindEncrypted, 
                                                             TDbBookmark& aBookmark )
    {
    TBool found = EFalse;
    
    if ( !aTable.IsEmptyL() )
        {
        const TUint querrySize=60;
        TBuf<querrySize> queryString;
        
        _LIT( KEquals, "=" );
        
        queryString.Copy( aColName );
        queryString.Append( KEquals );
        queryString.AppendNum( aFindEncrypted );
        
        if ( aTable.FirstL() )
            {
            found = aTable.FindL( RDbRowSet::EForwards, queryString ) != KErrNotFound ;
            
            if( found )
                {
                aBookmark = aTable.Bookmark( );
                } // end if
            }
        }    
    
    return found;
    }

// ------------------    
// CCompactionHandler
// ------------------    
    
// Used to make sure that compaction occurs at most once every 10 seconds.  This protects lower priority clients
// from being shut out during many sequential operations (initial sync, for example).
const TUint KInitialCompactionDelay = 10000000;
    
CCompactionHandler* CCompactionHandler::NewL( RDbDatabase& aDatabase,
                                              TInt         aPriority )
    {
    CCompactionHandler* self = new(ELeave) CCompactionHandler( aDatabase, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
CCompactionHandler::CCompactionHandler( RDbDatabase& aDatabase,
                                        TInt         aPriority ) :
    CActive( aPriority ),                                        
    iDatabase( aDatabase )                                        
    {
    __LOG_CONSTRUCT( "msg", "CCompactionHandler" )

   	CActiveScheduler::Add(this);
    }

void CCompactionHandler::ConstructL()
    {
    User::LeaveIfError( iDelayTimer.CreateLocal() );
    }

CCompactionHandler::~CCompactionHandler()
    {
    Cancel();
        
    iIncremental.Close();
    
    iDelayTimer.Close();
            
    __LOG_DESTRUCT
    }

void CCompactionHandler::Compact()
    {
    __LOG_ENTER_SUPPRESS( "Compact" )

     iCompactionNeeded = ETrue;
    
    if( (iSuspendCount == 0) && !IsActive() )
        {
        __LOG_WRITE_INFO( "starting compaction" );
        iStep = 0;
        
        iDelayTimer.After( iStatus, KInitialCompactionDelay );
        SetActive();          
        } // end if
    }

static void ResumeCompaction( TAny* aObject )
	{
	__LOG_STATIC_ENTER( "msg", "ResumeCompactionL" )	
	
	CCompactionHandler* handler = reinterpret_cast<CCompactionHandler*>(aObject);	
	handler->Resume( EFalse );  // do not pop the cleanup item
	
	__LOG_STATIC_EXIT
	} // end RollbackDatabaseTransaction
	
void CCompactionHandler::Suspend()
    {
    __LOG_ENTER_SUPPRESS( "Suspend" )
    
    if( iSuspendCount == 0 )
        {        
        __LOG_WRITE_DEBUG3( "suspending compaction" )

        Cancel();    
        iIncremental.Close();
    
        } // end if
    
    iSuspendCount++;    
    }         

void CCompactionHandler::SuspendLC()
    {
    Suspend();
    
    // Push an item on the cleanup stack taht will resume compaction if something leaves.
	CleanupStack::PushL(TCleanupItem(&ResumeCompaction, this) );
    }         

void CCompactionHandler::Resume( TBool aPopCleanupItem )
    {
    __LOG_ENTER_SUPPRESS( "Resume" )
    
    iSuspendCount--;
    
    if( iCompactionNeeded && (iSuspendCount == 0) )
        {
        __LOG_WRITE_INFO( "resuming compaction" );
        Compact();
        } // end if

	if( aPopCleanupItem )
		{		
		// Pop the resume item.	
		CleanupStack::Pop( this );
		} // end if
    }    
    
void CCompactionHandler::FinishCompactionL()
    {
    __LOG_ENTER( "FinishCompactionL" )
    
    SuspendLC();
    
    // Call the synchronous compaction.
    iDatabase.Compact();
    iCompactionNeeded = EFalse;
    
    Resume();    
    
    __LOG_EXIT
    }
        
void CCompactionHandler::Reschedule()
    {
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );      
    }

void CCompactionHandler::RunL()
    {
    __LOG_ENTER_SUPPRESS( "RunL" )
    
    if( iStep == 0 )
        {        
        iIncremental.Compact( iDatabase, iStep );
        }
    else
        {
        iIncremental.Next( iStep );
        } // end if

    if( iStep == 0 )
        {
        __LOG_WRITE_INFO( "Compaction completed" )

        iCompactionNeeded = EFalse;
        iIncremental.Close();
        }
    else
        {            
        Reschedule();
        } // end if
    }

void CCompactionHandler::DoCancel()
    {
    // The timer is only active on the first step.
    if( iStep == 0 )
        {
        iDelayTimer.Cancel();        
        } // end if
    } // end DoCancel
	    
// FUNCTIONS TO SUPPORT AUTOMATED UNIT TESTING
    
#ifdef _DEBUG		
void CContainerStoreUtils::SimulateLowDiskSpace( TInt aLatency )
    {
    iLowDiskSpaceLatency = aLatency;
    }    
#endif
    
