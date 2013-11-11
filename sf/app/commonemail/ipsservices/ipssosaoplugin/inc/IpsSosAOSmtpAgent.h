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
*     Always online plugin for freestyle smtp handling
*
*/

#ifndef __IPSSOSAOSMTPAGENT_H
#define __IPSSOSAOSMTPAGENT_H


#include <e32base.h>    // CBase
#include <msvapi.h>

class CIpsPlgSmtpOperation;
//<QMail>

//</QMail>

/**
* class CIpsSosAOSmtpAgent;
*
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOSmtpAgent) : 
//</Qmail>
    public CActive
    {
    public:
        /**
        * Two phased constructor
        * @return CIpsSosAOSmtpAgent*, self pointer
        */
        static CIpsSosAOSmtpAgent* NewL( 
                CMsvSession& aSession );
        
        /**
        * Two phased constructor
        * @return CIpsSosAOSmtpAgent*, self pointer
        */
        static CIpsSosAOSmtpAgent* NewLC( 
                CMsvSession& aSession );

        /**
        * Destructor
        */
        virtual ~CIpsSosAOSmtpAgent();
        
        
        virtual void DoCancel();
        
        virtual void RunL();
        //<Qmail>
        /**
         * CMsvSession event callback 
         */
        void HandleSessionEventL(    
            MMsvSessionObserver::TMsvSessionEvent aEvent,
            TAny* aArg1, 
            TAny* aArg2, 
            TAny* aArg3 );
            
        /**
         * Tries to send messages from outbox
         */
        void EmptyOutboxNowL( TMsvId aMessage );
		//</Qmail>
        
    private:
        /**
        * Constructor
        */
        CIpsSosAOSmtpAgent( 
                CMsvSession& aSession );

        /**
        * ConstructL
        * Second phase constructor
        */
        void ConstructL();    
		//<Qmail>
        /**
         * Creates needed internal data
         */
        void CreateInternalDataL( );
        
        /**
         * Releases internal resources
         */
        void ReleaseInternalData( );
		//</Qmail>
        
        
    private:


        //data
    private:

       CMsvSession& iSession;
		//<Qmail>
       CIpsPlgSmtpOperation* iOperation;//owned
	  

       //</QMail>
       TInt iError;
       
    };

#endif /* __IPSSOSAOSMTPAGENT_H*/
//EOF
