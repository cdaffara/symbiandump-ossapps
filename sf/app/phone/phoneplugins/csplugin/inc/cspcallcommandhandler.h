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
* Description:  Declarations for class CSPCallCommandHandler
*
*/


#ifndef CSPCALLCOMMANDHANDLER_H
#define CSPCALLCOMMANDHANDLER_H

#include <e32base.h>

#include "mcspcallinformation.h"

class CSPCall;
class MCCPCallCommandHandling;

/**
* Handles call adding from calls not done by the plugin.
*
*  @lib csplugin.dll
*/
enum TCSPPendingCallCommand {
            ECSPNone,
            ECSPAnswerCall,
            ECSPDialCall
        };
        
class CSPCallCommandHandler: public CBase
    {
    
    public: //Constructors and descructor    
    
        /**
        * Two-phased constructing for the array.
        */
        static CSPCallCommandHandler* NewL();

        /**
        * C++ default destructor
        */
        virtual ~CSPCallCommandHandler( );

        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateActiveHangup( MCCPCallCommandHandling& aCall );
        
        /**
        * Indicates hangup command complete.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateHangupComplete( MCCPCallCommandHandling& aCall );
        
        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateDialRequest( MCCPCallCommandHandling& aCall );
        
        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateAnswerRequest( MCCPCallCommandHandling& aCall );

    
    private:
        /**
        * C++ default constructor
        */
        CSPCallCommandHandler();
                                          
    private: // data
        
        /**
        * Active hangup array.
        */
        RPointerArray<MCCPCallCommandHandling> iActiveHangupArray;
        
        /**
        * Pending command.
        */
        TCSPPendingCallCommand iPendingCommand;
        
        /**
        * Pending call command object.
        * Not own
        */
        MCCPCallCommandHandling* iPendingCall;
        
    };

#endif // CSPCALLCOMMANDHANDLER_H