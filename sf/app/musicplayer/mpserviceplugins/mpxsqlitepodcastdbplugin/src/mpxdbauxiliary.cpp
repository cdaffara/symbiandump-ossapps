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
* Description:  Responsible for interaction with the Auxiliary table.
*
*/


// INCLUDE FILES
#include <sqldb.h>
#include <mpxlog.h>

#include "mpxdbcommonutil.h"
#include "mpxdbmanager.h"

#include "mpxpodcastdbpluginqueries.h"
#include "mpxpodcastcollectiondbdef.h"
#include "mpxdbutil.h"
#include "mpxdbauxiliary.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAuxiliary* CMPXDbAuxiliary::NewL(
    CMPXDbManager& aDbManager)
    {
    MPX_FUNC("CMPXDbAuxiliary::NewL");

    CMPXDbAuxiliary* self = CMPXDbAuxiliary::NewLC(aDbManager);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAuxiliary* CMPXDbAuxiliary::NewLC(
    CMPXDbManager& aDbManager)
    {
    MPX_FUNC("CMPXDbAuxiliary::NewLC");

    CMPXDbAuxiliary* self = new (ELeave) CMPXDbAuxiliary(aDbManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbAuxiliary::~CMPXDbAuxiliary()
    {
    MPX_FUNC("CMPXDbAuxiliary::~CMPXDbAuxiliary");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbAuxiliary::CMPXDbAuxiliary(
    CMPXDbManager& aDbManager) :
    CMPXDbTable(aDbManager)
    {
    MPX_FUNC("CMPXDbAuxiliary::CMPXDbAuxiliary");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::ConstructL()
    {
    MPX_FUNC("CMPXDbAuxiliary::ConstructL");
    BaseConstructL();
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::SetLastRefreshedTimeL
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::SetLastRefreshedTimeL(
    TTime aTime)
    {
    MPX_FUNC("CMPXDbAuxiliary::SetLastRefreshedTimeL");

    // update all databases
    HBufC* time = MPXDbCommonUtil::TTimeToDesLC(aTime);
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryAuxiliarySetTime, time);
    CleanupStack::PopAndDestroy(time);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::LastRefreshedTimeL
// ----------------------------------------------------------------------------
//
TTime CMPXDbAuxiliary::LastRefreshedTimeL()
    {
    MPX_FUNC("CMPXDbAuxiliary::LastRefreshedTimeL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryAuxiliaryGetTime));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrCorrupt);
        }

    // read the time string and convert it to TTime
    TTime time(MPXDbCommonUtil::DesToTTimeL(
    	MPXDbCommonUtil::GetColumnTextL(recordset, KMPXTableDefaultIndex)));
    CleanupStack::PopAndDestroy(&recordset);

    return time;
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::SetDBCorruptedL
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::SetDBCorruptedL(
    TBool aCorrupt)
    {
    MPX_FUNC("CMPXDbAuxiliary::SetDBCorruptedL");

    // update all databases
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryAuxiliarySetCorrupt,
        aCorrupt);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::DBCorruptedL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAuxiliary::DBCorruptedL()
    {
    MPX_FUNC("CMPXDbAuxiliary::DBCorruptedL");
    return (ExecuteSumQueryL(KQueryAuxiliaryGetCorrupt) > 0);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::SetSaveDeletedRecordCountL
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::SetSaveDeletedRecordCountL(TInt aDrive,
    TUint32 aValue)
    {
    MPX_FUNC("CMPXDbAuxiliary::SetSaveDeletedRecordCountL");

    // update all databases
    iDbManager.ExecuteQueryL(aDrive, KQueryAuxiliarySetCount, aValue);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::SaveDeletedRecordCountL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAuxiliary::SaveDeletedRecordCountL()
    {
    MPX_FUNC("CMPXDbAuxiliary::SaveDeletedRecordCountL");
    return ExecuteSumQueryL(KQueryAuxiliaryGetCount);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::SaveDeletedRecordCountL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAuxiliary::SaveDeletedRecordCountL(TInt aDrive)
    {
    MPX_FUNC("CMPXDbAuxiliary::SaveDeletedRecordCountL ");
    return ExecuteIntQueryL(aDrive, KQueryAuxiliaryGetCount);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::IsRefreshedL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAuxiliary::IsRefreshedL()
    {
    MPX_FUNC("CMPXDbAuxiliary::IsRefreshedL");

    TBool refreshed(ETrue);

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryAuxiliaryGetTime));
    CleanupClosePushL(recordset);

    TInt count(0);
    while (recordset.Next() == KSqlAtRow)
        {
        count++;

        // read the time string and convert it to TTime
        if (Time::NullTTime() == MPXDbCommonUtil::DesToTTimeL(
        	MPXDbCommonUtil::GetColumnTextL(recordset, KMPXTableDefaultIndex)))
            {
            refreshed = EFalse;
            break;
            }
        }

    CleanupStack::PopAndDestroy(&recordset);
    return refreshed && (count > 0);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::IdL
// ----------------------------------------------------------------------------
//
TInt CMPXDbAuxiliary::IdL( TInt aDrive )
    {
    MPX_DEBUG1("CMPXDbAuxiliary::IdL <--");
    TInt id(0);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDrive, KQueryAuxiliaryGetId));
    CleanupClosePushL( recordset );

    while(recordset.Next() == KSqlAtRow )
        {
        id = recordset.ColumnInt(KMPXTableDefaultIndex);
        }

    CleanupStack::PopAndDestroy( &recordset );
    MPX_DEBUG1("CMPXDbAuxiliary::IdL -->");
    return id;
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::SetIdL
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::SetIdL( TInt aDrive, TInt aId )
    {
    MPX_DEBUG1("CMPXDbAuxiliary::SetIdL <--");
    iDbManager.ExecuteQueryL(aDrive,KQueryAuxiliarySetId, aId);
    MPX_DEBUG1("CMPXDbAuxiliary::SetIdL -->");
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool aCorruptTable)
    {
    MPX_FUNC("CMPXDbAuxiliary::CreateTableL");

    // create the table
    User::LeaveIfError(aDatabase.Exec(KAuxiliaryCreateTable));

    // insert the default record
    // use the same length as '%u' is longer than '0' or '1'
    HBufC* query = KQueryAuxiliaryInsert().AllocLC();
    query->Des().Format(KQueryAuxiliaryInsert, aCorruptTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::DropTableL
// ----------------------------------------------------------------------------
//
void CMPXDbAuxiliary::DropTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbAuxiliary::DropTableL");
    User::LeaveIfError(aDatabase.Exec(KAuxiliaryDropTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbAuxiliary::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAuxiliary::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbAuxiliary::CheckTableL");
    return DoCheckTable(aDatabase, KAuxiliaryCheckTable);
    }

// End of File
