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


#ifndef C_IRSESSIONDB_H
#define C_IRSESSIONDB_H

#include <d32dbms.h>

class CIRSessionLogger;

NONSHARABLE_CLASS( CIRSessionDb ) : public CBase
    {                                                
    //member functions
public:

    /**
    * Function : NewL()
    * Two phased construction
    * @return instance of CIRSessionDb
    */
    static CIRSessionDb* NewL();
    
    /**
    * Function : NewLC()
    * Two phased construction
    * @return instance of CIRSessionDb
    */    
    static CIRSessionDb* NewLC();
    
    /**
    * Function : ~CIRSessionDb()
    * default destructor
    */    
    ~CIRSessionDb();

    /**
    * Function : CreateDbConditional()
    * Creates both the dbms files conditionally( only if not yet created )
    * @param session database file
    * @return errcode
    */    
    TInt CreateDbConditional( TFileName &aSession );

    /**
    * Function : AddSessionStartL()
    * adds the session log entry into data base
    * @param session data instance
    */
    void AddSessionStartL( CIRSessionLogger& aSession );
    
    /**
    * CIRSessionDb::GetAllPresetL()
    * gets all the preset into an array 
    */
    void GetAllSessionL( CArrayPtrFlat<CIRSessionLogger>& aSessionDataList );
    
    /**
    * Function : DeleteAllSessionL
    * delete all the session from session log entry
    */
    void DeleteAllSessionL();

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
    * Function : CreateSessionTableL
    * creates sessionlogtable with two column one is sessionid and rest of session
    * log data
    * ---------------------------------------------------------------------------
    * SessionTable
    *---------------------------
    *| KID   |     KSessionLogCol |
    *---------------------------
    *|TInt32 | EDbColLongText8 |
    *---------------------------
    */    
    void CreateSessionTableL();
    
    /**
    * Function : CreateSessionIndexL
    * sets sessionid as the primary key
    */  
    void CreateSessionIndexL();
    
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
    void CreateDbL( TFileName& aSession );

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
    RDbNamedDatabase iSessionDb;   
    };

#endif  //C_IRSESSIONDB_H
