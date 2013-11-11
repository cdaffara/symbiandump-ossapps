/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*    Phone resource resolver
*
*/


#ifndef __CPHONEMAINERRORMESSAGESHANDLER_H
#define __CPHONEMAINERRORMESSAGESHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <coemain.h>
#include "mphoneerrormessageshandler.h"
#include "mphoneerrormessageshandlerregister.h"


// CLASS DECLARATION

/**
*  CPhoneErrorMessagesHandlerRegister realizes the MPhoneErrorMessagesHandlerRegister interface, which
*  allows for any error messagesd handler to offer their services for the Main Error messages
*  Handler. Basically, the Main Error messages handler does not have any hard coded handling
*  functionality. It only serves as access point for all the individual error messages handler
*  scattered around the application ( via MPhoneErrorMessagesHandlerRegister interface ).
*/
class CPhoneMainErrorMessagesHandler : 
    public CCoeStatic,
    public MPhoneErrorMessagesHandler ,
    public MPhoneErrorMessagesHandlerRegister
    {
    public: // Constructors and destructor
        /**
        * Virtual Destructor
        * @param None
        * @return None
        */
        virtual ~CPhoneMainErrorMessagesHandler();
    
    public: // From MPhoneErrorMessagesHandler
        /**
        * ShowErrorSpecificNoteL of given error.
        * @param aErrorInfo: error code and callId
        * @return None.
        */
        void ShowErrorSpecificNoteL( const TPEErrorInfo& aErrorInfo );

    public: // From MPhoneErrorMessagesHandlerRegister
        /**
        * Register given handler for handling errors from given 
        * enumeration.
        * @param aHandler: Pointer to the handler to be registered.  
        * @return None.
        */
        void RegisterErrorMessagesHandler( MPhoneErrorMessagesHandler* aHandler );

    public: // New methods
     
        /**
        * First call initializes the singleton object. Subsequent calls return
        * instance.
        * @param None.
        * @return Pointer to the one and only instance of main error 
        *         handler -object.
        */
        IMPORT_C static  CPhoneMainErrorMessagesHandler* Instance();
        
    protected:  // Constructors and destructor
        /**
        * Protected constructor.
        * @param None
        * @return None
        */
        CPhoneMainErrorMessagesHandler();

    private:    // Constructors and destructor
        /**
        * NewL function for creating the main resolver.
        * @param None
        * @return CPhoneMainResourceResolver*
        */
        static CPhoneMainErrorMessagesHandler* NewL();
        
    private:    // Data      
        
        MPhoneErrorMessagesHandler* iHandler;

    };
#endif      // __CPHONERESOURCERESOLVER_H
            
// End of File
