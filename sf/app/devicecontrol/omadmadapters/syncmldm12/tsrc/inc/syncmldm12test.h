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
* Description:  Implementation of DM adapter test component
* 	This is part of omadmextensions/adapter test application.
*
*/






#ifndef TCTEST_H
#define TCTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <smldmadapter.h>
#include "dmatest.h"

const TUint KNSmlDMSettings12AdapterImplUid = 0x10282CE8;
	

const TUid KAdapterUid = 
		{
		KNSmlDMSettings12AdapterImplUid
		};
// Logging path
_LIT( KamtestLogPath, "\\logs\\testframework\\tctest\\" ); 
// Log file
_LIT( KamtestLogFile, "tctest.txt" ); 


// FORWARD DECLARATIONS
class Csyncmldm12Test;


// CLASS DECLARATION

/**
*  Csyncmldm12Test test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class Csyncmldm12Test : public Cdmatest
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static Csyncmldm12Test* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~Csyncmldm12Test();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                

    private:

        /**
        * C++ default constructor.
        */
        Csyncmldm12Test( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );
    
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        virtual void Delete();
        
        /**
        * Test methods are listed below. 
        */
        
        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
/*        virtual TInt ExampleL( CStifItemParser& aItem ) ;
        virtual TInt DeliverL( CStifItemParser& aItem ) ;
        virtual TInt DetailsL( CStifItemParser& aItem ) ;
		    virtual TInt InstallL( CStifItemParser& aItem ) ;
		    virtual TInt BareInstallL( CStifItemParser& aItem ) ;*/
		

    private:    // Data
        HBufC8 *GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName );


    };

#endif      // TCTEST_H
            
// End of File
