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
* Description:  The implementation for presentation elements.
*
*/


#ifndef C_IRNMSLOGDB_H
#define C_IRNMSLOGDB_H

#include <d32dbms.h>

class CIRNmsLogger;

NONSHARABLE_CLASS( CIRNmsLogDb ) : public CBase
    {                                                
    //member functions
public:

    /**
    * Function : NewL()
    * Two phased construction
    * @return instance of CIRNmsLogDb
    */
    static CIRNmsLogDb* NewL();
    
    /**
    * Function : NewLC()
    * Two phased construction
    * @return instance of CIRNmsLogDb
    */    
    static CIRNmsLogDb* NewLC();
    
    /**
    * Function : ~CIRNmsLogDb()
    * default destructor
    */    
    ~CIRNmsLogDb();

    /**
    * Function : CreateDbConditional()
    * Creates both the dbms files conditionally(only if not yet created)
    * @param session database file
    * @return errcode
    */    
    TInt CreateDbConditional( TFileName &aNmsLog );

    /**
    * Function : AddNmsLogStartL()
    * adds the NmsLog log entry into data base
    * @param NmsLog data instance
    */
    void AddNmsLogStartL( CIRNmsLogger& aNmsLog );
    
    /**
    * CIRNmsLogDb::GetAllNmsLogL()
    * gets all the preset into an array 
    */
    void GetAllNmsLogL( CArrayPtrFlat<CIRNmsLogger>& aNmsLogDataList );
    
    /**
    * Function : DeleteAllNmsLogL
    * delete all the NmsLog from NmsLog log entry
    */
    void DeleteAllNmsLogL();

private: 

    /**
    * Function : ConstructL()
    * two phase construction
    */   
    void ConstructL();
    
    /**
    * Function : CloseDb()
    * Closes the database 
    */
    void CloseDb();
    
    /**
    * Function : CreateNmsLogTableL
    * creates NmsLoglogtable with two column one is NmsLogid and rest of NmsLog
    * log data
    * ---------------------------------------------------------------------------
    * NmsLogTable
    *---------------------------
    *| KID   |     KNmsLogCol |
    *---------------------------
    *|TInt32 | EDbColLongText8 |
    *---------------------------
    */    
    void CreateNmsLogTableL();
    
    /**
    * Function : CreateNmsLogIndexL
    * sets NmsLogid as the primary key
    */  
    void CreateNmsLogIndexL();
    
    /**
    * Function : OpenDbL()
    * opening the data base 
    */
    void OpenDbL();
    
    /**
    * Function : CreateDbL()
    * creates both the dbms files 
    * database filename
    */    
    void CreateDbL( TFileName& aNmsLog );

    //data members                                        
private:

    /**
    * database file
    */
    TFileName iDbFile;
    
    /**
    * file session
    */
    RFs iFsSession;
    
    /**
    * data base handler
    */
    RDbNamedDatabase iNmsLogDb;
    
    };

#endif  //C_IRNMSLOGDB_H
