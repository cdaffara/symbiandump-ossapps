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
* Description:  Video telephony engine class
*
*/



#ifndef CVTENGMODEL_H
#define CVTENGMODEL_H

//  INCLUDES
#include <e32base.h>
//#include <MVtEngEngine.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MVtEngEventObserver;
class MVtEngCommandObserver;
class CVtEngEventManager;
class CVtEngHandlerContainer;
class MVtEngCommandHandler;
class CVtEngCommandHandler;
class MVtEngSessionInfo;
class MVtEngMedia;
class MVtEngAudio;
class CVtEngStateManager;
class CVtEngUtility;
class CVtEngExtensions;

// CLASS DECLARATION

/**
*  Model for video telephony engine.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngModel ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aEventObserver event observer
        * @param aCommandObserver command observer
        */
        IMPORT_C static CVtEngModel* NewL( 
            MVtEngEventObserver& aEventObserver,
            MVtEngCommandObserver& aCommandObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CVtEngModel();

    public: // New functions

        IMPORT_C MVtEngCommandHandler& CommandHandler() const;

        IMPORT_C MVtEngSessionInfo& Session() const;

        IMPORT_C MVtEngMedia& Media() const;

        IMPORT_C MVtEngAudio& Audio() const;

        /**
        * Returns extension or NULL if extension cannot be found.
        * @param aExtensionUid extension uid
        * @return pointer to extension
        */
        IMPORT_C TAny* Extension( const TUid& aExtensionUid );


        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */

    private:

        /**
        * C++ default constructor.
        */
        CVtEngModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MVtEngEventObserver& aEventObserver,
            MVtEngCommandObserver& aCommandObserver );    

    private:    // Data

        // Event manager acting as event mediator.
        CVtEngEventManager*     iEventManager;

        // Container for functional units.
        CVtEngHandlerContainer* iHandlers;

        // Handler for UI commands
        CVtEngCommandHandler*   iCommandHandler;

        // Manager for engine states
        CVtEngStateManager*     iStateManager;

        // Utility for TLS access
        CVtEngUtility*          iUtils;
        
        //  Container for engine extensions
        CVtEngExtensions* iExtensions;
       
    };

#endif      // CVTENGMODEL_H
            
// End of File
