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


#ifndef IPSSOSSMTPPLUGINTIMER_H
#define IPSSOSSMTPPLUGINTIMER_H


#include <e32base.h>    // CBase


/**
* class MIpsSosAOPluginTimerCallBack
* interface to timer call backs
*/
class MIpsSosAOPluginTimerCallBack
    {
public:
    
    virtual void TimerFiredL() = 0;
    
    };

/**
* class CIpsSosAOPluginTimer
*
* Ips sos ao plugin timer class, used by mbox logic class
* 
* @lib ipssosaoplugin.lib
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOPluginTimer) : 
//</Qmail>
    public CActive
    {
    
public:
    
        /**
        * Symbian OS NewL function
        * @return CIpsSosAOPluginTimer*, self pointer
        */
        static CIpsSosAOPluginTimer* NewL( 
            TInt aPriority,
            MIpsSosAOPluginTimerCallBack& aCallBack );


        /**
        * destructor
        */
        virtual ~CIpsSosAOPluginTimer();

private:
    
        /**
        * Constructor
        * @param aPriority active object priority
        */
        CIpsSosAOPluginTimer( 
            TInt aPriority,
            MIpsSosAOPluginTimerCallBack& aCallBack );

        /**
        * ConstructL
        * Symbian OS second phase constructor.
        */
		void ConstructL();

public:

        /**
        * Set timer active
        * @param const TTimeIntervalSeconds&, seconds to trick 
        */
		void After( const TTimeIntervalSeconds& aSeconds );


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
	
		RTimer                        iTimer;
		MIpsSosAOPluginTimerCallBack& iCallBack;

    };
    
#endif //IPSSOSSMTPPLUGINTIMER_H

//EOF
