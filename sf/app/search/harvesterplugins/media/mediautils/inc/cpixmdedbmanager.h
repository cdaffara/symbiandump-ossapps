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
* Description:  This class keeps a snap shot of the object id present in the MDS DB
*                Any change monitored by the mde harvester plugin, first checks in this
*                table and then updates the content in the lucene database
*
*/


#ifndef CPIX_MDE_DB_MANAGER_H_
#define CPIX_MDE_DB_MANAGER_H_

#include <e32def.h>
#include <e32base.h>
#include <d32dbms.h>

enum TCPixMdeDBManagerAPIReturnType
    {
    EOperationSuccess=0,
    EOperationFailure,
    EItemPresent,
    EItemNotPresent,
    EOperationUndefined
    };
typedef enum TCPixMdeDBManagerAPIReturnType TCPIXMDEDBAPIReturnType;

//Forward declaration
class CMDSEntity;

class CCPIXMDEDbManager : public CBase
    {
public:

    /**
     * Construction
     * @return instance of CCPIXMDEDbManager
     */
    static CCPIXMDEDbManager* NewL(const TUid& aPluginImplUid);
    /**
     * Construction
     * @return instance of CCPIXMDEDbManager
     */
    static CCPIXMDEDbManager* NewLC(const TUid& aPluginImplUid);
    /**
     * Destructor
     */    
    virtual ~CCPIXMDEDbManager();
    
    /**
     * Utility method Add an Object ID in the database
     * @param TUint& aObjectId
     * @param aMdsItem mds entity to be added
     * @return TCPixMdeDBManagerAPIReturnType
     */    
    TCPIXMDEDBAPIReturnType AddL(const TUint& aObjectId,CMDSEntity& aMdsItem);
    
    /**
     * Utility method Update an Object 
     * @param TUint& aObjectId
     * @param aMdsItem mds entity to be added
     * @return TCPixMdeDBManagerAPIReturnType
     */
    TCPIXMDEDBAPIReturnType UpdateL(const TUint& aObjId,CMDSEntity& aMdsItem);
    /*
     * Checks if item already exist in database
     * @param aObjectId object id of MDS Items
     * @return ETrue if exist else EFalse
     */
    TBool IsAlreadyExistL(const TUint& aObjectId);
    
    /*
     * Get the MDS item from Database
     * @param aKey object Id of item
     * @param aMdsItem reference of entity
     */
    void GetItemL(const TInt aKey, CMDSEntity& aMdsItem); 

    /**
     * Utility method remove an Object ID in the database
     * @param aObjectId Object ID
     * @return TCPixMdeDBManagerAPIReturnType
     */    
    TCPIXMDEDBAPIReturnType RemoveL(const TUint& aObjectId);
    
    /**
     * Utility method remove all items from database
     * @return TCPixMdeDBManagerAPIReturnType
     */    
    TCPIXMDEDBAPIReturnType ResetL();    
    
protected:
    /*
     * second phase constructor ConstructL
     */
    void ConstructL(const TUid& aPluginImplUid);
    /*
     * Default constructor
     */
    CCPIXMDEDbManager(); //Default constructor
    
    
private:
    void CreateMdeCPIXSyncDBL(const TDesC& aDatabaseFile); 
    
    /**
     * Creates a column set used to create a new database file.
     * @return column set.
     */
    CDbColSet* CreateColumnSetLC();
    void Rollback();
        
    
private:
    /**
     *  Session to File system
     */
    RFs  iFs;
    
    //Flag to check if database is opened or not
    TBool iOpened;
    /**
     * Interface to Notepad database     
     */ 
    RDbNamedDatabase iDatabase;    
    /**
     * handle to the table.owned
     */ 
    RDbTable iTable;
    };

#endif /* CPIXDOCFETCHER_H_ */
