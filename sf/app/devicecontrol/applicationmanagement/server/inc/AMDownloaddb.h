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


#ifndef __AMDownloaddb_H
#define __AMDownloaddb_H

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
const TInt KAMDownloaddbDrive = EDriveC;
const TUid KAMPolicyUID =
    {
    0x200267FB
    };
const TInt KAMMaxPkgURLLength( 2048);
const TInt KAMMaxPkgNameLength( 80);
const TInt KAMMaxPkgVersionLength( 80);

_LIT( KDBMSSecureID, "secure[200267FB]" );

_LIT( KAMDownloaddbName, "c:TARMAppMgmtDownloadDB.db" );

// Table and column names
_LIT( KTableAMMgmtObject, "AMMgmtObject" );

_LIT(NCol1, "Id");
_LIT(NCol2, "LUID");
_LIT(NCol3, "Result");
_LIT(NCol4, "TargetURI");
_LIT(NCol5, "IAPId");

class CAMDbItem : public CBase
    {
public:
    static CAMDbItem* NewL();
    ~CAMDbItem();

    TInt id;
    TUint32 iLUID;
    TInt iResult;

    HBufC8* iTargetURI;

    TUint32 iapid;

    };

// CLASS DECLARATION

/**
 *  CAMDownloaddb provides an interface for accessing AM database.
 *  
 *  @lib AMadapter.lib
 *  
 */
class CAMDownloaddb : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @return A pointer to the newly created instance.
     */
    static CAMDownloaddb* NewL();

    /**
     * Two-phased constructor. Pushes the pointer onto the CleanupStack.
     * @return A pointer to the newly created instance.
     */
    static CAMDownloaddb* NewLC();

    /**
     * Destructor.
     */
    virtual ~CAMDownloaddb();

    void CreateTableL(RDbDatabase& aDatabase);

    void CreateandOpenL(TParse& name);

    TBool DeleteFromDatabaseL(TUint32 aLUID);

    void SaveToDatabaseL(TInt& aIndex, TUint32 aLuid, TInt aResult,
            const TDesC8& aTargetURI, TUint32 aIapid);

    void ReadItemsL(RPointerArray<CAMDbItem>& aItemArray, TDesC& aQueryBuffer);

    void ReadDbItemsL(RPointerArray<CAMDbItem>& aItemArray);

    void GetEntryForLUIDL(RPointerArray<CAMDbItem>& aItemArray, TDesC& aValue);

    /**
     * Second phase construction.
     */
    void ConstructL();

    /**
     * Constructor.
     */
    CAMDownloaddb();

private:

    RDbNamedDatabase iDatabase;
    RFs iFsSession;
    RDbs iRdbSession;

    RDbTable iAMObjectTable;
    CDbColSet* iColSet;
    RDbView iView;

    };

#endif // __AMDownloaddb_H   
// End of File
