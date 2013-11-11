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
* Description: process monitor.
*/


#ifndef __HS_PROCESS_MONITOR__
#define __HS_PROCESS_MONITOR__

#include <e32base.h>

class t_hsapplicationlauncher;

/**
 * MHsProcessMonitorObserver
 * 
 * Monitor component observer.
 * 
 * @since S60 5.2
 */
class MHsProcessMonitorObserver
    {    
    public:
        /**
         * Called when monitored process is ended.
         * 
         * @param aExitType         Exit type
         * @param aExitReason       Exit reason
         * @param aExitCategory     Exit category
         */
        virtual void ProcessEnded( const TExitType& aExitType,
                                   const TInt aExitReason,
                                   const TExitCategoryName& aExitCategory ) = 0;

        /**
         * Called when error occured during monitoring.
         * 
         * @param aError System wide error code.
         */        
        virtual void ProcessMonitoringError( TInt aError ) = 0;											
    };

/**
 * HsProcessMonitor
 * 
 * Component for monitoring process states.
 * 
 * @since S60 5.2
 */  
class CHsProcessMonitor : public CActive
    {
    public:
        /**
         * Create new monitor
         * 
         * @param aProcessName	process id
         * @param aObserver monitor observer
         * @since S60 5.2     
         */    
        static CHsProcessMonitor* NewL( const TInt aProcessId,
                                        MHsProcessMonitorObserver& aObserver );   
        
        /**
         * Destructor
         * 
         * @since S60 5.2     
         */    
        ~CHsProcessMonitor();
    
        /**
         * Attach monitor
         * 
         * @return TInt     System wide error code.       
         * @since S60 5.2
         */        
        TInt AttachMonitor();
        
        /**
         * Start monitor
         * 
         * @return TInt     System wide error code.
         * @since S60 5.2
         */        
        TInt StartMonitor();        

    protected:
        /** 
         * C++ constructor
         *
         * @param aProcessName  process id
         * @param aObserver     watchdog observer.   
         * @since S60 5.2      
         */        
        CHsProcessMonitor( const TInt aProcessId,
                           MHsProcessMonitorObserver& aObserver );    

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

    private: // New methods
        /**
         * Close process handle if open
         * 
         * @since S60 5.2
         */        
        void SecureProcessHandleAsClosed();     

    private:	
        // process id
        TInt iProcessId;        
        
        // watchdog observer
        MHsProcessMonitorObserver& iObserver;
        
        // Process handle
        RProcess iProcess;
        
        // boolean to represent whether process handle is open
        TBool iProcessHandleOpen;        

        friend class t_hsapplicationlauncher;
    };

#endif // __HS_PROCESS_MONITOR__
