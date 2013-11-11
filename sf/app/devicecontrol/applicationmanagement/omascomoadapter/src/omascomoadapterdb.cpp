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


// INCLUDE FILES
#include <sysutil.h>
#include <bautils.h>

#include "omascomoadapterdb.h"
#include "debug.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
//  CSCOMOAdapterDb::NewL()
//  Creates a new instance of CSCOMOAdapterDb object.
// ---------------------------------------------------------------------------
// 
CSCOMOAdapterDb* CSCOMOAdapterDb::NewL()
    {
    RDEBUG("CSCOMOAdapterDb::NewL(): begin");

    CSCOMOAdapterDb* self = CSCOMOAdapterDb::NewLC();
    CleanupStack::Pop(self);

    RDEBUG("CSCOMOAdapterDb::NewL(): end");

    return self;
    }

// ---------------------------------------------------------------------------
//  CSCOMOAdapterDb::NewLC()
//  Creates a new instance of CSCOMOAdapterDb object. 
//  Pushes and leaves new instance onto CleanupStack.
// ---------------------------------------------------------------------------
// 
CSCOMOAdapterDb* CSCOMOAdapterDb::NewLC()
    {
    RDEBUG("CSCOMOAdapterDb::NewLC(): begin");

    CSCOMOAdapterDb* self = new( ELeave ) CSCOMOAdapterDb();
    CleanupStack::PushL(self);
    self->ConstructL();

    RDEBUG("CSCOMOAdapterDb::NewLC(): end");

    return self;
    }

// ---------------------------------------------------------------------------
//  CSCOMOAdapterDb::ConstructL()
//  Second phase constructor.
// ---------------------------------------------------------------------------
// 
void CSCOMOAdapterDb::ConstructL()
    {
    RDEBUG("CSCOMOAdapterDb::ConstructL(): begin");

    TParse name;
    TInt err;

    User::LeaveIfError(iFsSession.Connect());
    User::LeaveIfError(iRdbSession.Connect() );

    iFsSession.SetSessionToPrivate(EDriveC);

#ifdef SYMBIAN_SECURE_DBMS
    name.Set(KAMAdapterDbName, NULL, NULL);
    err = iDatabase.Open(iRdbSession, name.FullName(), KDBMSSecureID);
#else
    name.Set( KAMAdapterDbName, KNonSecureDbFullName, NULL );
    err = iDatabase.Open(iRdbSession, DBFileName);
#endif

    if (err == KErrNotFound)
        {
        CreateandOpenL(name);
        }
    else
        {

        if ( (err == KErrEof ) || (err == KErrCorrupt ) || (err
                == KErrArgument ))
            {

            iRdbSession.DeleteDatabase(name.FullName(), KAMPolicyUID);
            CreateandOpenL(name);
            }

        }

    RDEBUG("CSCOMOAdapterDb::ConstructL(): end");
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::CreateandOpenL
// -----------------------------------------------------------------------------
//    
void CSCOMOAdapterDb::CreateandOpenL(TParse& name)
    {

    TInt err;

#ifdef SYMBIAN_SECURE_DBMS
    iDatabase.Create(iRdbSession, name.FullName(), KDBMSSecureID);
#else
    if( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, KEmptyDbSizeEstimate ) )
        {
        User::Leave( KErrDiskFull );
        }
    iDatabase.Create(iFsSession, name.FullName());
#endif	

    CreateTableL(iDatabase);
    iDatabase.Close();
#ifdef SYMBIAN_SECURE_DBMS

    err = iDatabase.Open(iRdbSession, name.FullName(), KDBMSSecureID);

#else
    err = iDatabase.Open(iFsSession, DBFileName);
#endif

    //Debug
    if (err != KErrNone)
        {

        User::LeaveIfError(err);
        }

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::CreateTableL
// -----------------------------------------------------------------------------
//  
void CSCOMOAdapterDb::CreateTableL(RDbDatabase& aDatabase)
    {

    // Create a table definition
    CDbColSet* columns=CDbColSet::NewLC();

    // Add Columns
    TDbCol id(NCol1, EDbColInt32);

    // automatic indexing for items,it is our key field.
    id.iAttributes=id.EAutoIncrement;
    columns->AddL(id);
    columns->AddL(TDbCol(NCol2, EDbColInt32));
    columns->AddL(TDbCol(NCol3, EDbColInt32));
    columns->AddL(TDbCol(NCol4, EDbColText8, 255));
    columns->AddL(TDbCol(NCol5, EDbColText8, 255));
    columns->AddL(TDbCol(NCol6, EDbColText8, 255));
    columns->AddL(TDbCol(NCol7, EDbColInt32));

    //possibility of Panic 

    // Create a table
    TRAPD(err, aDatabase.CreateTable(KTableAMMgmtObject, *columns) );

    if(err!=KErrNone)
    User::Leave(err);

    // cleanup the column set
    CleanupStack::PopAndDestroy(columns);

    }

// ---------------------------------------------------------------------------
//  CSCOMOAdapterDb::CSCOMOAdapterDb()
//  Constructor.
// ---------------------------------------------------------------------------
// 
CSCOMOAdapterDb::CSCOMOAdapterDb()
    {
    RDEBUG("CSCOMOAdapterDb::CSCOMOAdapterDb(): begin");
    RDEBUG("CSCOMOAdapterDb::CSCOMOAdapterDb(): end");
    }

// ---------------------------------------------------------------------------
//  CSCOMOAdapterDb::~CSCOMOAdapterDb()
//  Destructor.
// ---------------------------------------------------------------------------
// 
CSCOMOAdapterDb::~CSCOMOAdapterDb()
    {
    RDEBUG("CSCOMOAdapterDb::~CSCOMOAdapterDb(): begin");

    iView.Close();

    delete iColSet;
    iAMObjectTable.Close();

    iDatabase.Close();
    iFsSession.Close();
    iRdbSession.Close();

    RDEBUG("CSCOMOAdapterDb::~CSCOMOAdapterDb(): ends");
    }

CAMDbItem::~CAMDbItem()
    {
    delete iServerID;
    delete iCorrelator;
    delete iSourceURI;
    }

void CSCOMOAdapterDb::GetEntryForServerIDL(RPointerArray<CAMDbItem>& aItemArray,
        TDesC8& aValue)
    {

    aItemArray.Reset();// first reset the array

    _LIT(KEqualToString,"='%S'");
    _LIT(KQuery, "SELECT * FROM %S WHERE %S");

    TBuf<256> serverid;
    serverid.Copy(aValue);

    TBuf<100> bufsql;
    bufsql.Append(KQuery);
    bufsql.Append(KEqualToString);
    TBuf<100> bufsql1;
    bufsql1.Format(bufsql, &KTableAMMgmtObject, &NCol4, &serverid);

    ReadItemsL(aItemArray, bufsql1);

    }



// -----------------------------------------------------------------------------
// CWPInternetAPDB::ReadItemsL
// -----------------------------------------------------------------------------
// 
void CSCOMOAdapterDb::ReadItemsL(RPointerArray<CAMDbItem>& aItemArray,
        TDesC& aSqlQuery)
    {

    RDbView view;
    view.Prepare(iDatabase, TDbQuery(aSqlQuery));
    CleanupClosePushL(view);
    view.EvaluateAll();
    view.FirstL();

    while (view.AtRow())
        {
        view.GetL();

        CAMDbItem *dbitem = new(ELeave) CAMDbItem;
        dbitem->id = view.ColInt(1);
        dbitem->iLUID = view.ColInt(2);
        dbitem->iProfileId = view.ColInt(3);

        dbitem->iServerID = (view.ColDes8(4)).AllocL();
        dbitem->iCorrelator = (view.ColDes8(5)).AllocL();
        dbitem->iSourceURI = (view.ColDes8(6)).AllocL();

        dbitem->iRetryCount = view.ColInt(7);

        aItemArray.AppendL(dbitem);
        view.NextL();
        }

    CleanupStack::PopAndDestroy(); // view

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::SaveToDatabaseL
// -----------------------------------------------------------------------------
//   
void CSCOMOAdapterDb::SaveToDatabaseL(TInt& aIndex, TUint32 aLuid,
        TInt aProfileid, TInt aRetryCount, const TDesC8& aSourceURI,
        const TDesC8& aServerid, const TDesC8& aCorrelator)
    {

    BeginDatabaseTransaction();

    TFileName sqlQuery;
    sqlQuery.Copy(_L("SELECT * FROM "));
    sqlQuery.Append(KTableAMMgmtObject);

    RDbView view;
    view.Prepare(iDatabase, TDbQuery(sqlQuery));
    CleanupClosePushL(view);

    view.InsertL();

    view.SetColL(2, aLuid);
    view.SetColL(3, aProfileid);
    view.SetColL(4, aServerid);
    view.SetColL(5, aCorrelator);
    view.SetColL(6, aSourceURI);
    view.SetColL(7, aRetryCount);

    view.PutL();

    aIndex = view.ColInt(1);// autoincrement gives us unique index.

    CleanupStack::PopAndDestroy(1); // view
    
    CommitDatabaseTransaction();

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::DeleteFromDatabaseL
// -----------------------------------------------------------------------------
//
TBool CSCOMOAdapterDb::DeleteFromDatabaseL(TUint32 aLuid)
    {

    TBool rowsdeleted = EFalse;
    TFileName sqlQuery;
	
	PrepareLuidQuery(aLuid, sqlQuery);
	

    BeginDatabaseTransaction();

    RDbView view;
    // query buffer with index finds only the selected item row.
    view.Prepare(iDatabase, TDbQuery(sqlQuery));
    CleanupClosePushL(view);

    view.EvaluateAll();
    view.FirstL();

    if (!view.IsEmptyL())
        {
        // we have autoincrement in index so it should be unique
        // but just to make sure, we use 'while', instead of 'if'
        while (view.AtRow())
            {
            view.GetL();
            view.DeleteL();
            view.NextL();
            }

        
        CommitDatabaseTransaction();
        // compacts the databse, by physically removig deleted data.
        iDatabase.Compact();
        rowsdeleted = ETrue;
        }
    CleanupStack::PopAndDestroy(1); // view


    return rowsdeleted;
    }

void CSCOMOAdapterDb::UpdateRetryCountForLuidL(TInt aLuid, TInt aRetryCount)
{
	TFileName sqlQuery;
	
	PrepareLuidQuery(aLuid, sqlQuery);
	
	BeginDatabaseTransaction();
	
	RDbView view;
	view.Prepare(iDatabase, TDbQuery(sqlQuery));
	CleanupClosePushL(view);
	
	view.EvaluateAll();
	view.FirstL();
	
	if(view.AtRow())            
	{			
		view.UpdateL();
		view.SetColL(7, aRetryCount);		
		view.PutL();
	}
			
	CleanupStack::PopAndDestroy(1); // view
	
    CommitDatabaseTransaction();
}

TInt CSCOMOAdapterDb::GetRetryCountForLuidL(TInt aLuid)
{
    TInt retrycount = KErrNone;
	TFileName sqlQuery;
    
    PrepareLuidQuery(aLuid, sqlQuery);
    
    BeginDatabaseTransaction();
    
    RDbView view;
    view.Prepare(iDatabase, TDbQuery(sqlQuery));
    CleanupClosePushL(view);
    view.EvaluateAll();
    view.FirstL();

    if (!view.IsEmptyL())
    {
    
    if(view.AtRow())            
	{
    view.GetL();
	}
	
    retrycount = view.ColInt(7);
	
    }

	CleanupStack::PopAndDestroy(1); // view
	
	CommitDatabaseTransaction();

	return retrycount;    
    
}

void CSCOMOAdapterDb::PrepareLuidQuery(TInt aLuid, TDes& aSqlQuery)
{
	aSqlQuery.Copy(_L("SELECT * FROM "));
    aSqlQuery.Append(KTableAMMgmtObject);
    aSqlQuery.Append(_L(" WHERE "));
    aSqlQuery.Append(NCol2);
    aSqlQuery.Append(_L(" = "));
    aSqlQuery.AppendNum(aLuid);
}

void CSCOMOAdapterDb::BeginDatabaseTransaction()
{
	iDatabase.Begin();
}

void CSCOMOAdapterDb::CommitDatabaseTransaction()
{
	iDatabase.Commit();
}
