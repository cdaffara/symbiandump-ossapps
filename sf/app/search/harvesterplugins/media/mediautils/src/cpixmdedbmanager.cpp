/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for creating and accessing the mde and Cpix Sync Table 
*
*/


#include "cpixmdedbmanager.h"
#include <harvesterserverlogger.h>
#include <s32strm.h> //to support RDbColReadStream::ReadL
#include <bautils.h>
#include "mdsitementity.h"

//database name
_LIT(KDriveC, "c:");
_LIT(KDBNAME, "_Sync.db");
_LIT(KMDECPIXSYNCTable,"MdeCpixSync");
_LIT(KObjectId, "ObjectId");
_LIT(KUri, "Uri");
_LIT(KDrive, "Drive");
_LIT(KMdeCpixSyncDbSqlInsert, "SELECT * FROM MdeCpixSync");
_LIT(KMdeCpixSyncDbSqlCheckExist, "SELECT * FROM MdeCpixSync WHERE ObjectId=%d");
//Not used. Can be deleted.
//_LIT(KMdeCpixSyncDbSqlDelete, "DELETE FROM MdeCpixSync WHERE ObjectId="); //39 chars
const TInt KMdeCpixSyncDbSqlDeleteSize(100); // %d costs 10 chars in max
_LIT(KUIDSTRINGHEX,"%x");

enum TNotepadColumnType
    {
    EKey = 1, // this must be 1 (DBMS/SQL restriction) mapped to ObjectID of MDS
    EUri,
    EDrive
    };

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CCPIXMDEDbManager* CCPIXMDEDbManager::NewL(const TUid& aPluginImplUid)
    {
    CCPIXMDEDbManager* self = CCPIXMDEDbManager::NewLC(aPluginImplUid);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXMDEDbManager::NewLC()
// -----------------------------------------------------------------------------
//
CCPIXMDEDbManager* CCPIXMDEDbManager::NewLC(const TUid& aPluginImplUid)
    {
    CCPIXMDEDbManager* self = new (ELeave) CCPIXMDEDbManager();
    CleanupStack::PushL(self);
    self->ConstructL(aPluginImplUid);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXMDEDbManager::~CCPIXMDEDbManager()
// -----------------------------------------------------------------------------
//
CCPIXMDEDbManager::~CCPIXMDEDbManager()
    {
    if(iOpened)
        {    
        iDatabase.Close();        
        }
    iFs.Close();
    }
    

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CCPixIndexerUtils()
// -----------------------------------------------------------------------------
//
CCPIXMDEDbManager::CCPIXMDEDbManager()
    {    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CCPIXMDEDbManager::ConstructL(const TUid& aPluginImplUid)
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::ConstructL : Enter");
    User::LeaveIfError(iFs.Connect());
    //Secure database needes policy file for every database and it is comparitively
    //lacks in performance from non secure database so we can use non secure
    //Create the database filename.
    HBufC* dataFile = HBufC::NewLC( KMaxFileName );    
    TBuf <10> aUidString;
    aUidString.Format(KUIDSTRINGHEX, aPluginImplUid);
    TFileName  privatePath;
    iFs.CreatePrivatePath(EDriveC);
    iFs.PrivatePath(privatePath);//data caged path of loading process
    dataFile->Des().Copy(KDriveC);
    dataFile->Des().Append(privatePath);//data caged path of Plugin        
    dataFile->Des().Append( aUidString );
    dataFile->Des().Append( KDBNAME );
    //Open the database
    TInt err(iDatabase.Open(iFs,*dataFile));
    switch (err)
         {
         case KErrNone:
             {
             iOpened = ETrue;            
             }
             break;
         case KErrNotFound:
             {
             //create the database and register for notifier
             TRAP( err , CreateMdeCPIXSyncDBL(*dataFile));
             if(err == KErrNone)
                 {
                 iOpened = ETrue;
                 }
             }
             break;
         default:            
             CPIXLOGSTRING("CNotesPlugin::ConstructL : Error in Database Open");
             break;
         }    
     CleanupStack::PopAndDestroy( dataFile );  
     CPIXLOGSTRING("END CCPIXMDEDbManager::ConstructL : Enter");
    }

void CCPIXMDEDbManager::CreateMdeCPIXSyncDBL(const TDesC& aDatabaseFile)
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::CreateMdeCPIXSyncDBL");
    iDatabase.Close();
    //Create the database filename.
    //create the database
    User::LeaveIfError(iDatabase.Create(iFs,aDatabaseFile));
    CDbColSet* columns = CreateColumnSetLC();//creates the columns and push to cleanupstack
    User::LeaveIfError( iDatabase.CreateTable( KMDECPIXSYNCTable , *columns ) );
    //clean up of variables (columns and dataFile)
    CleanupStack::PopAndDestroy( 1 ); //columns    
    CPIXLOGSTRING("END CCPixIndexerUtils::MountAllAvailableDriveL");
    }


// -----------------------------------------------------------------------------
// CNotesPlugin::CreateColSetLC
// -----------------------------------------------------------------------------
//
CDbColSet* CCPIXMDEDbManager::CreateColumnSetLC()
    {
    CPIXLOGSTRING("CCPIXMDEDbManager::CreateColSetLC: Entered");
    CDbColSet* columns = CDbColSet::NewLC();
    
    //Add Key column
    TDbCol col( KObjectId , EDbColInt32 );
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL( col );
    
    col.iName = KUri;
    col.iType = EDbColLongText;
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL(col);

    col.iName = KDrive;
    col.iType = EDbColInt32;
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL(col);
    
    return columns; // columns stays on CleanupStack
    }

// -----------------------------------------------------------------------------
// CCPIXMDEDbManager::AddL
// -----------------------------------------------------------------------------
//
TCPIXMDEDBAPIReturnType CCPIXMDEDbManager::AddL(const TUint& aObjId,CMDSEntity& aMdsItem)
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::AddL");
    TCPIXMDEDBAPIReturnType retVal = EOperationUndefined;
    // Database is not openend
    // return Not ready
    if(!iOpened)
     {
     User::Leave(KErrNotReady);
     }
    iDatabase.Begin();    
    RDbView dbView;
    CleanupClosePushL(dbView);
    TBuf<KMdeCpixSyncDbSqlDeleteSize> sql;
    sql.Copy(KMdeCpixSyncDbSqlInsert);
    TInt err =  dbView.Prepare(iDatabase, TDbQuery(sql));
    User::LeaveIfError(dbView.EvaluateAll());
    //Getthe pointer to the Content recieved as part of the Command
    if(KErrNone == err) 
     {       
     dbView.InsertL();
     dbView.SetColL(EKey,aObjId);
     dbView.SetColL(EDrive, aMdsItem.DriveNumber());
     dbView.SetColL(EUri,aMdsItem.Uri());
     //Inserts the row       
     dbView.PutL();
     CPIXLOGSTRING("CCPIXMDEDbManager::AddL PutL");          
     retVal = EOperationSuccess;         
     }
    CleanupStack::PopAndDestroy(&dbView); // dbView
    if(retVal == EOperationSuccess)
        {
        iDatabase.Commit();
        iDatabase.Compact();
        }
    else
        {
        iDatabase.Rollback();
        }
    User::LeaveIfError(err);
    CPIXLOGSTRING("END CCPIXMDEDbManager::AddL");
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCPIXMDEDbManager::UpdateL
// -----------------------------------------------------------------------------
//
TCPIXMDEDBAPIReturnType CCPIXMDEDbManager::UpdateL(const TUint& aObjId,CMDSEntity& aMdsItem)
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::UpdateL");
    TCPIXMDEDBAPIReturnType retVal = EOperationUndefined;
    // Database is not openend return Not ready
    if(!iOpened)
     {
     User::Leave(KErrNotReady);
     }
    iDatabase.Begin();
    RDbView dbView;
    CleanupClosePushL(dbView);
    TBuf<KMdeCpixSyncDbSqlDeleteSize> sql;
    sql.Format(KMdeCpixSyncDbSqlCheckExist,aObjId);
    User::LeaveIfError(dbView.Prepare(iDatabase, TDbQuery(sql)));
    User::LeaveIfError(dbView.EvaluateAll());
    (void)dbView.FirstL();
    if(dbView.AtRow())
        {
        dbView.UpdateL();
        dbView.SetColL(EKey,aObjId);
        dbView.SetColL(EDrive, aMdsItem.DriveNumber());
        dbView.SetColL(EUri,aMdsItem.Uri());
        //Update record 
        dbView.PutL();
        retVal = EOperationSuccess;
        }
    CleanupStack::PopAndDestroy(&dbView); // dbView
    if(retVal == EOperationSuccess)
        {
        iDatabase.Commit();
        iDatabase.Compact();
        }
    else
        {
        iDatabase.Rollback();
        }
    CPIXLOGSTRING("END CCPIXMDEDbManager::UpdateL");
    return retVal;
    }

// -----------------------------------------------------------------------------
// CNotesPlugin::RemoveL
// -----------------------------------------------------------------------------
//
TCPIXMDEDBAPIReturnType CCPIXMDEDbManager::RemoveL(const TUint& aObjId)
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::RemoveL");
    TCPIXMDEDBAPIReturnType retVal = EOperationUndefined;
    //If the database was not opened.
    // return Not ready
    if(!iOpened)
        {
        User::Leave(KErrNotReady);
        }
    TBuf<KMdeCpixSyncDbSqlDeleteSize> sql;
    sql.Format(KMdeCpixSyncDbSqlCheckExist,aObjId);
    iDatabase.Begin();
    //Prepare the view
    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError(dbView.Prepare(iDatabase, TDbQuery(sql)));
    User::LeaveIfError(dbView.EvaluateAll());
    //Get the note property from the database for the given key.
    TBool isAtRow(dbView.FirstL());
    if ( isAtRow )
        {
        dbView.DeleteL();
        retVal = EOperationSuccess;
        CPIXLOGSTRING("CCPIXMDEDbManager::RemoveL Successful!");
        }
    else
        {
        //If the specified key was not found.
        retVal= EOperationFailure;
        }
    CleanupStack::PopAndDestroy(&dbView); // dbView
    if(EOperationSuccess == retVal)
        {
        iDatabase.Commit();
        iDatabase.Compact();
        }
    else
        {
        iDatabase.Rollback();
        }
    CPIXLOGSTRING("END CCPIXMDEDbManager::RemoveL");
    return retVal; 
    }

TBool CCPIXMDEDbManager::IsAlreadyExistL(const TUint& aObjectId)
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::IsAlreadyExistL");
    TBool retVal(EFalse);
    //If the database was not opened.
    // return Not ready
    if(!iOpened)
        {
        User::Leave(KErrNotReady);
        }
    TBuf<KMdeCpixSyncDbSqlDeleteSize> sql;
    sql.Format(KMdeCpixSyncDbSqlCheckExist,aObjectId);
    //Prepare the view
    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError(dbView.Prepare(iDatabase, TDbQuery(sql), RDbView::EReadOnly ));
    User::LeaveIfError(dbView.EvaluateAll());
    //Get the note property from the database for the given key.
    TBool isAtRow(dbView.FirstL());
    if ( isAtRow )
        {
        retVal= ETrue; //Found 
        }
    CleanupStack::PopAndDestroy(&dbView); // dbView
    CPIXLOGSTRING("END CCPIXMDEDbManager::IsAlreadyExistL");
    return retVal; 
    }

void CCPIXMDEDbManager::GetItemL(const TInt aKey, CMDSEntity& aMdsItem)
    {
    CPIXLOGSTRING2("START CCPIXMDEDbManager::GetItemL aKey = %d",aKey);
    TInt err(KErrNone);
    //dataBase is not opened. return Not Ready
    if(!iOpened)
        {
        User::Leave(KErrNotReady);
        }
    TBuf<KMdeCpixSyncDbSqlDeleteSize> sql;
    sql.Format(KMdeCpixSyncDbSqlCheckExist, aKey);
    //Prepare the view
    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError(dbView.Prepare(iDatabase, TDbQuery(sql), RDbView::EReadOnly ));
    User::LeaveIfError(dbView.EvaluateAll());
    //Get the note property from the database for the given key.
    TBool isAtRow(dbView.FirstL());
    if ( isAtRow )
        {
        //HBufC* content = NULL;
        dbView.GetL();
        //Get the key and modified time
        TInt key = dbView.ColInt32(EKey);
        TInt drive = dbView.ColInt32(EDrive);
        RDbColReadStream blob;
        blob.OpenLC(dbView, EUri);
        //Get the length of the content.
        TInt colLength( dbView.ColLength(EUri));
        //Get the length of the content to be read as filename       
        TInt uriLength(0);
        if(colLength > KMaxFileName )
            {
            uriLength = KMaxFileName;            
            }
        else
            {
            uriLength = colLength;
            }       
        
        //Create the buffer to get the filename
        HBufC* content = HBufC::NewLC(uriLength);
        TPtr ptr = content->Des();            
        //read the data from the database for the given length
        blob.ReadL(ptr,uriLength);         
        //Set the properties of the note to the Note Entity.
        TDriveNumber drivenumber = TDriveNumber(drive);
        aMdsItem.Set(key,*content,drivenumber);
        CleanupStack::PopAndDestroy( content ); // content
        //CleanupStack::Pop(content); // content
        //delete content;
        CleanupStack::PopAndDestroy(&blob); // blob
        }
    else 
        {
        //return not found for an invlaid key.
        err = KErrNotFound;
        }
    CleanupStack::PopAndDestroy(&dbView); // dbView/
    CPIXLOGSTRING2("END CCPIXMDEDbManager::GetItemL err = %d",err);
    User::LeaveIfError(err);      
    }

// -----------------------------------------------------------------------------
// CNotesPlugin::ResetL
// -----------------------------------------------------------------------------
//
TCPIXMDEDBAPIReturnType CCPIXMDEDbManager::ResetL()
    {
    CPIXLOGSTRING("START CCPIXMDEDbManager::ResetL");
    TCPIXMDEDBAPIReturnType retVal = EOperationUndefined;
    TBuf<KMdeCpixSyncDbSqlDeleteSize> sql;
    sql.Copy(KMdeCpixSyncDbSqlInsert);
    iDatabase.Begin();
    //Prepare the view
    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError(dbView.Prepare(iDatabase, TDbQuery(sql)));
    User::LeaveIfError(dbView.EvaluateAll());
    CPIXLOGSTRING2("CCPIXMDEDbManager::ResetL dbView.CountL = %d",dbView.CountL());
    while(dbView.NextL())
        {
#ifdef _DEBUG
        TRAPD(err,dbView.DeleteL());
        CPIXLOGSTRING2("CCPIXMDEDbManager::ResetL dbView.DeleteL success err = %d",err);
#else
        TRAP_IGNORE( dbView.DeleteL() );
#endif
        }
    if(dbView.CountL() == 0)
        {
        CPIXLOGSTRING("CCPIXMDEDbManager::ResetL Success!"); 
        retVal= EOperationSuccess;
        }
    else
        {
        CPIXLOGSTRING("CCPIXMDEDbManager::ResetL Failure!");
        retVal= EOperationFailure;
        }
    CleanupStack::PopAndDestroy(1); // Myview
    if(retVal == EOperationSuccess)
        {
        iDatabase.Commit();
        iDatabase.Compact();
        }
    else
        {
        iDatabase.Rollback();
        }
    CPIXLOGSTRING("END CCPIXMDEDbManager::ResetL");
    return retVal; 
    }

// -----------------------------------------------------------------------------
// CMTPNotesDpDataController::Rollback
// -----------------------------------------------------------------------------
//
void CCPIXMDEDbManager::Rollback()
    {
    iDatabase.Rollback();
    if ( iDatabase.IsDamaged() )
        {
        iDatabase.Recover();
        }
    }

// End of file

