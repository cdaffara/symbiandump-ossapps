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
*     Main smpt plugin class
*
*/

#ifndef __CIPSSOSAOSMTPSENDWATCHER_H__
#define __CIPSSOSAOSMTPSENDWATCHER_H__


#include <e32base.h>
#include <e32property.h>

class CMsvSession;
class CIpsSosAOSmtpAgent;

/**
* CIpsSosAOSmtpSendWatcher
*
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOSmtpSendWatcher) : 
//</Qmail>
    public CActive
    {
    public:
    
        /**
        * Symbian OS NewL function
        * @return CIpsSosAOSmtpSendWatcher*, self pointer
        */
        static CIpsSosAOSmtpSendWatcher* NewL( 
            TInt aPriority,
            CMsvSession& aSession );


        /**
        * destructor
        */
        virtual ~CIpsSosAOSmtpSendWatcher();

    private:
    
        /**
        * Constructor
        * @param aPriority active object priority
        */
        CIpsSosAOSmtpSendWatcher( 
            TInt aPriority );

        /**
        * ConstructL
        * Symbian OS second phase constructor.
        */
		void ConstructL( CMsvSession& aSession );

    public:

        /**
        * At
        * @param const TTime&, time when to complete 
        */
		void StartWatch();


	protected:

        /**
        * DoCancel
        * From CActive
        */
		virtual void DoCancel();

        /**
        * RunL
        * From CActive
        */
		virtual void RunL();
		
		/**
		* Function to handle error during running the operation
		* @return KErrNone, when error has been handled
		*/
		virtual TInt RunError( TInt aError );

	protected: // Data
		//<Qmail>
	    CIpsSosAOSmtpAgent* iAgent;//owned
        //</Qmail>
        RProperty iProperty;
    };
    
#endif /* __CIPSSOSAOSMTPSENDWATCHER_H__ */ 

//EOF
