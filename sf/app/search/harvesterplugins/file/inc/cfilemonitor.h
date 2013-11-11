/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CFILEMONITOR_H
#define CFILEMONITOR_H

//  INCLUDES 

#include <e32base.h>

#include "cfileplugin.h"
#include "fastfindfileserverpluginclient.h"

// FORWARD DECLARATIONS
class CFolderRenamedHarvester;

class CFileMonitor : public CActive
  	{
	public:
		/**
		* NewL
		* @return
		*/
		static CFileMonitor* NewL(  CFilePlugin& aFilePlugin, RFs* aFsSession );

		/**
		* Destructor
		*/
		virtual ~CFileMonitor();
		
		/**
		* From 
		*/
		TBool StartMonitoring();
	
		/**
		* From 
		*/
		TInt Initialize();
    	
    	/**
		* From 
		*/
    	TInt Release();
    	
    	/**
		* From 
		*/
    	TInt OpenEngine();
    	
    	/**
		* From 
		*/
    	void AddNotificationPathsL( const TDriveNumber aDriveNumber );
    	
    	/**
		* From 
		*/
    	void RemoveNotificationPaths( const TDriveNumber aDriveNumber );
    	
    	/**
		* From 
		*/
    	TInt Enable();
    	
    	/**
		* From 
		*/
    	TInt Disable();
    
    protected: // CActivi
        /**
	     * From CActive.
	     */
        void DoCancel();

        /**
	     * From CActive.
         */
        void RunL();

        /**
	     * From CActive.
	     * @param aError Leave code from RunL()
	     * @return Error code, KErrNone if error was handled successfully.
         */
        TInt RunError( TInt aError );

    protected:
		/**
		* From ConstructL
		*/
		void ConstructL( CFilePlugin& aFilePlugin, RFs* aFsSession );
		
	private:
		/**
		* From Constructor
		*/
		CFileMonitor( CFilePlugin& aFilePlugin, RFs* aFsSession );

		/**
		* Reset status of package
		*/
		void ResetStatus();

	private:
	
		CFilePlugin&      iFilePlugin;
		RFastFindFSPEngine iEngine;
	    RFs* iFsSession;
	    TFastFindFSPStatusPckg iPckg;
	    // If a folder name is changed then files in that folder need to be indexed again as path changed.
	    CFolderRenamedHarvester* iFolderRenamedHarvester;
        #ifdef HARVESTERPLUGINTESTER_FRIEND
            friend class CHarvesterPluginTester;
        #endif
	};

#endif // CFILEMONITOR_H
