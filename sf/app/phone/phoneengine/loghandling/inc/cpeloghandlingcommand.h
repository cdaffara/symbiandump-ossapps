/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPELogHandlingCommand object
*
*/


#ifndef CPELOGHANDLINGCOMMAND_H
#define CPELOGHANDLINGCOMMAND_H

//INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>
#include <logwrap.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CPELogHandling;
class CLogClient;
class CLogEvent;

// CLASS DECLARATION

/**
*  Sends Supplementary string directly to the serving network
*  
*
*  @lib callhandling.lib
*  @since Series60_2.7
*/
class CPELogHandlingCommand   
    : public CActive
    {
    public: //Constructors and descructor
        /**
        * C++ default constructor.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/        
        CPELogHandlingCommand( CPELogHandling& aOwner, CLogClient& aLogClient );
        
        /**
        * Destructor.
        */
        virtual ~CPELogHandlingCommand( );        

    public:    //New functions
        /**
        * This method offers phone's identity
        * @since Series60_2.7
        * @param aPhoneIdentity includes phone identity parameters.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        virtual TInt AddEvent( CLogEvent& aLogEvent );

        /**
        * This method asks phone identity parameters from the mmetel.
        * @since Series60_2.7
        * @param aMessage,The reply message to the phone application.
        * @return KErrNone or KErrInUse if CPELogHandlingCommand is active allready
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        virtual TInt ChangeEvent( const CLogEvent& aLogEvent );
        
    protected: // From CActive
        /**
        * Informs object that asyncronous request is ready.
        * @since Series60_4.0
        * @param None. 
        * @return None.
        */
        virtual void RunL();   
                      
        /**
        * Cancels asyncronous request(s).
        * @since Series60_4.0
        * @param None.    
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        virtual void DoCancel();
        
        
    private:
        CPELogHandling& iOwner;
    
        // reference to log client
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        CLogClient& iLogClient;  
        
        // reference to log database event.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogId iLogId;       
              
    };

#endif // CPELOGHANDLINGCOMMAND_H

// End of File
