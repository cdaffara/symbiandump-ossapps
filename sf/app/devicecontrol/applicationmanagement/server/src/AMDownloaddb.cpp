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

#include "AMDownloaddb.h"
#include "debug.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
//  CAMDownloaddb::NewL()
//  Creates a new instance of CAMDownloaddb object.
// ---------------------------------------------------------------------------
// 
CAMDownloaddb* CAMDownloaddb::NewL()
    {
    RDEBUG("CAMDownloaddb::NewL(): begin");

    CAMDownloaddb* self = CAMDownloaddb::NewLC();
    CleanupStack::Pop(self);

    RDEBUG("CAMDownloaddb::NewL(): end");

    return self;
    }

// ---------------------------------------------------------------------------
//  CAMDownloaddb::NewLC()
//  Creates a new instance of CAMDownloaddb object. 
//  Pushes and leaves new instance onto CleanupStack.
// ---------------------------------------------------------------------------
// 
CAMDownloaddb* CAMDownloaddb::NewLC()
    {
    RDEBUG("CAMDownloaddb::NewLC(): begin");

    CAMDownloaddb* self = new( ELeave ) CAMDownloaddb();
    CleanupStack::PushL(self);
    self->ConstructL();

    RDEBUG("CAMDownloaddb::NewLC(): end");

    return self;
    }

// ---------------------------------------------------------------------------
//  CAMDownloaddb::ConstructL()
//  Second phase constructor.
// ---------------------------------------------------------------------------
// 
void CAMDownloaddb::ConstructL()
    {
    RDEBUG("CAMDownloaddb::ConstructL(): begin");

    TParse name;
    TInt err;

    User::LeaveIfError(iFsSession.Connect());
    User::LeaveIfError(iRdbSession.Connect() );

    iFsSession.SetSessionToPrivate(EDriveC);

#ifdef SYMBIAN_SECURE_DBMS
    name.Set(KAMDownloaddbName, NULL, NULL);
    err = iDatabase.Open(iRdbSession, name.FullName(), KDBMSSecureID);
#else
    name.Set( KAMDownloaddbName, KNonSecureDbFullName, NULL );
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
            // something seriously wrong with the db, delete it and try 
            // to create new
            iRdbSession.DeleteDatabase(name.FullName(), KAMPolicyUID);
            CreateandOpenL(name);
            }

        }

    RDEBUG("CAMDownloaddb::ConstructL(): end");
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::CreateandOpenL
// -----------------------------------------------------------------------------
//    
void CAMDownloaddb::CreateandOpenL(TParse& name)
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
void CAMDownloaddb::CreateTableL(RDbDatabase& aDatabase)
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
    columns->AddL(TDbCol(NCol5, EDbColInt32));

    // Create a table
    TRAPD(err, aDatabase.CreateTable(KTableAMMgmtObject, *columns) )
    ;

    if (err!=KErrNone)
        User::Leave(err);

    // cleanup the column set
    CleanupStack::PopAndDestroy(columns);

    }

// ---------------------------------------------------------------------------
//  CAMDownloaddb::CAMDownloaddb()
//  Constructor.
// ---------------------------------------------------------------------------
// 
CAMDownloaddb::CAMDownloaddb()
    {
    RDEBUG("CAMDownloaddb::CAMDownloaddb(): begin");
    RDEBUG("CAMDownloaddb::CAMDownloaddb(): end");
    }

// ---------------------------------------------------------------------------
//  CAMDownloaddb::~CAMDownloaddb()
//  Destructor.
// ---------------------------------------------------------------------------
// 
CAMDownloaddb::~CAMDownloaddb()
    {
    RDEBUG("CAMDownloaddb::~CAMDownloaddb(): begin");

    iView.Close();

    delete iColSet;
    iAMObjectTable.Close();

    iDatabase.Close();
    iFsSession.Close();
    iRdbSession.Close();

    RDEBUG("CAMDownloaddb::~CAMDownloaddb(): ends");
    }

CAMDbItem::~CAMDbItem()
    {
    delete iTargetURI;

    }

void CAMDownloaddb::GetEntryForLUIDL(RPointerArray<CAMDbItem>& aItemArray,
        TDesC& aValue)
    {
    RDEBUG( "CAMDownloaddb::GetEntryForLUIDL: Step1" );
    aItemArray.Reset();// first reset the array

    _LIT(KEqualToString,"=%d");
    _LIT(KQuery, "SELECT * FROM %S WHERE %S");

    RDEBUG( "CAMDownloaddb::GetEntryForLUIDL: Step2" );

    TBuf<100> bufsql;
    bufsql.Append(KQuery);
    bufsql.Append(KEqualToString);
    TLex lex(aValue);

    RDEBUG( "CAMDownloaddb::GetEntryForLUIDL: Step3" );

    TInt value;
    lex.Val(value);

    TBuf<100> bufsql1;
    bufsql1.Format(bufsql, &KTableAMMgmtObject, &NCol2, value);

    RDEBUG( "CAMDownloaddb::GetEntryForLUIDL: Step4" );

    ReadItemsL(aItemArray, bufsql1);

    RDEBUG( "CAMDownloaddb::GetEntryForLUIDL: Step5" );

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::ReadDbItemsL
// -----------------------------------------------------------------------------
//   
void CAMDownloaddb::ReadDbItemsL(RPointerArray<CAMDbItem>& aItemArray)
    {

    aItemArray.Reset();// first reset the array

    TFileName sqlQuery;

    // just get all columns & rows of same originator
    sqlQuery.Copy(_L("SELECT * FROM "));
    sqlQuery.Append(KTableAMMgmtObject);

    ReadItemsL(aItemArray, sqlQuery);

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::ReadItemsL
// -----------------------------------------------------------------------------
// 
void CAMDownloaddb::ReadItemsL(RPointerArray<CAMDbItem>& aItemArray,
        TDesC& asqlQuery)
    {

    RDEBUG( "CAMDownloaddb::ReadItemsL: Step1" );

    RDbView view;
    view.Prepare(iDatabase, TDbQuery(asqlQuery));
    CleanupClosePushL(view);
    view.EvaluateAll();
    view.FirstL();

    while (view.AtRow())
        {
        view.GetL();

        CAMDbItem *dbitem = new(ELeave) CAMDbItem;
        CleanupStack::PushL(dbitem);
        dbitem->id = view.ColInt(1);
        dbitem->iLUID = view.ColInt(2);
        dbitem->iResult = view.ColInt(3);

        dbitem->iTargetURI= (view.ColDes8(4)).AllocL();

        dbitem->iapid = view.ColInt(5);
        aItemArray.AppendL(dbitem);
        CleanupStack::Pop(dbitem);
        view.NextL();
        }

    CleanupStack::PopAndDestroy(); // view

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::SaveToDatabaseL
// -----------------------------------------------------------------------------
//   
void CAMDownloaddb::SaveToDatabaseL(TInt& aIndex, TUint32 aLuid,
        TInt aResult, const TDesC8& aTargetURI, TUint32 aIapid)
    {

    iDatabase.Begin();

    TFileName sqlQuery;
    sqlQuery.Copy(_L("SELECT * FROM "));
    sqlQuery.Append(KTableAMMgmtObject);

    RDbView view;
    view.Prepare(iDatabase, TDbQuery(sqlQuery));
    CleanupClosePushL(view);

    view.InsertL();

    view.SetColL(2, aLuid);
    view.SetColL(3, aResult);
    view.SetColL(4, aTargetURI);
    view.SetColL(5, aIapid);

    view.PutL();

    aIndex = view.ColInt(1);// autoincrement gives us unique index.

    CleanupStack::PopAndDestroy(1); // view
    iDatabase.Commit();

    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::DeleteFromDatabaseL
// -----------------------------------------------------------------------------
//
TBool CAMDownloaddb::DeleteFromDatabaseL(TUint32 aLUID)
    {

    TBool rowsdeleted = EFalse;
    TFileName sqlQuery;
    sqlQuery.Copy(_L("SELECT * FROM "));
    sqlQuery.Append(KTableAMMgmtObject);
    sqlQuery.Append(_L(" WHERE "));
    sqlQuery.Append(NCol2);
    sqlQuery.Append(_L(" = "));
    sqlQuery.AppendNum(aLUID);

    iDatabase.Begin();

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

        iDatabase.Commit();
        // compacts the databse, by physically removig deleted data.
        iDatabase.Compact();
        rowsdeleted = ETrue;
        }
    CleanupStack::PopAndDestroy(1); // view


    return rowsdeleted;
    }

