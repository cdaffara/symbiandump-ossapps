/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsmessagestoreutils  declaration
*
*/



#ifndef __MMSMESSAGESTOREUTILS_H
#define __MMSMESSAGESTOREUTILS_H

// INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include <msvapi.h>

// USERINCLUDE FILES
#include "mmsconst.h"

// CONSTANTS
// Folder for local mode
_LIT( KMmsDefaultServiceName, "MMSService" );

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* CMmsMessageStoreUtils is a helper class for CMmsSettings class.
* This class does not access CenRep, but instead it encapsulates communication
* with MessageStore.
* @lib mmsserversettings.lib
* @since 3.0
*/
class CMmsMessageStoreUtils : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsMessageStoreUtils* NewL( CMsvSession& aSession );
        
        /**
        * Destructor.
        */
        virtual ~CMmsMessageStoreUtils();

    public: // New functions
    
        /**
        * CreateServiceEntryL deletes all found old MMS service entries
        * directly under root, and then creates a new one.
        * @return entry id of the new service
        */
        TMsvId CreateServiceEntryL();

        /**
        * CreateFolderEntryL deletes all found old folder entries directly
        * under local service with 'aFolderName' name, and then creates a new
        * one.
        * @param name of folder entry
        * @return entry id of the new folder 
        */
        TMsvId CreateFolderEntryL( const TDesC& aFolderName );
 
        /**
        * CreateFolderEntryL creates a new folder entry under a given folder 
        * with 'aFolderName' name.
        * @param aParentFolder a folder, under which the new folder is created.
        * @param aFolderName name of folder entry
        * @param aFolderId entry id of the new folder or 
        * if the folder already exists, the folder id of the existing folder 
        * @return KErrNone if successful.
        * KErrAlreadyExists, if a folder with same name exists under aParentFoder
        */
        TInt CreateFolderEntryL( TMsvId aParentFolder, const TDesC& aFolderName, TMsvId& aFolderId );
        
        /**
        * Gets a folder entry id of the given aFolderName  
        *
        * @param aParentFolder a folder, under where the folder is searched
        * @param aFolderName name of folder entry
        * @return entry id of the folder 
        */
        TMsvId FolderEntryL( TMsvId aParentFolder, const TDesC& aFolderName );
                
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:
    
        /**
        * Cobstructor
        */
        void ConstructL( CMsvSession& aSession );

        /**
        * C++ default constructor.
        */
        CMmsMessageStoreUtils();

    public:     // Data

    protected:  // Data

    private:    // Data
        
        CMsvSession* iSession;
        CMsvEntry*   iParentEntry;
        CMsvEntry*   iMmsEntry;
    };

#endif // __MMSMESSAGESTOREUTILS_H   
            
// End of File
