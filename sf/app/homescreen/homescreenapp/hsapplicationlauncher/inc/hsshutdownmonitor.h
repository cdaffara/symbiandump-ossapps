/*
* Copyright (c) 1020 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: shutdown monitor.
*/


#ifndef __HS_SHUTDOWN_MONITOR__
#define __HS_SHUTDOWN_MONITOR__

#include <e32base.h>
#include <ssm/ssmstateawaresession.h>

class t_hsapplicationlauncher;

/**
 * MHsShutdownMonitorObserver
 * 
 * Monitor component observer.
 * 
 * @since S60 5.2
 */
class MHsShutdownMonitorObserver
    {    
    public:
        /**
         * Called when shutdown occurs 
         */
        virtual void SystemShutdownEvent() = 0;
    };

/**
 * HsShutdownMonitor
 * 
 * Component for monitoring shutdown
 * 
 * @since S60 5.2
 */  
class CHsShutdownMonitor : public CActive
    {
    public:
        /**
         * Create new monitor
         * 
         * @param aObserver monitor observer
         * @since S60 5.2     
         */    
        static CHsShutdownMonitor* NewL(
                MHsShutdownMonitorObserver& aObserver );   
        
        /**
         * Destructor
         * 
         * @since S60 5.2     
         */    
        ~CHsShutdownMonitor();
    
        /**
         * Start monitor
         * 
         * @since S60 5.2
         */        
        void StartMonitor();        

    protected:
        /** 
         * C++ constructor
         *
         * @param aObserver     monitor observer.   
         * @since S60 5.2      
         */        
        CHsShutdownMonitor( MHsShutdownMonitorObserver& aObserver );    

        /**
         * Symbian 2nd phase constructor
         * 
         * @since S60 5.2
         */        
        void ConstructL();

    private: // From CActive
        /**
         * From CActive
         */        
        void RunL();

        /**
         * From CActive
         * 
         * @since S60 5.2
         */        
        void DoCancel(); 

        /**
         * From CActive
         * 
         * @since S60 5.2
         */        
        TInt RunError( TInt aError );

    private:	
        // watchdog observer
        MHsShutdownMonitorObserver& iObserver;

        // State aware session.
        RSsmStateAwareSession iSAS;

        friend class t_hsapplicationlauncher;
    };

#endif // __HS_PROCESS_MONITOR__
