/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
 *
*/



#ifndef __T_CGLXCOMMANDREQUEST_H__
#define __T_CGLXCOMMANDREQUEST_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxcommandrequest.h"

//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CGlxCommandRequest )
	: public CEUnitTestSuiteClass,
	  public MGlxDataSourceUpdateObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxCommandRequest* NewL();
        static T_CGlxCommandRequest* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxCommandRequest();

        
    public: // From MGlxDataSourceUpdateObserver 
	    void HandleMessage(CMPXMessage& aMessage);
	
    private:    // Constructors and destructors

        T_CGlxCommandRequest();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CGlxCommandRequest_CommandL();
        

    private:    // Data
		
        CGlxCommandRequest* iCGlxCommandRequest;
        EUNIT_DECLARE_TEST_TABLE; 
		CMPXCommand* iCommand;
    };

#endif      //  __T_CGLXCOMMANDREQUEST_H__

// End of file
