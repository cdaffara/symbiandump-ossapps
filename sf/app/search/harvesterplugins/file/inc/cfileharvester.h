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


#ifndef CFILEHARVESTER_H
#define CFILEHARVESTER_H

//  INCLUDES 

#include <e32base.h>

#include "cfileplugin.h"

class CFilePlugin;

class CFileHarvester : public CActive
    {
    public:
    
    	enum TFileHarvesterState
    		{
    		EHarvesterIdleState,
    		EHarvesterStartHarvest,
    		EHarvesterGetFileId    		    		
    		};
        /**
        * Construction
        * @return Harvester image plugin
        */
        static CFileHarvester* NewL(CFilePlugin& aFilePlugin, RFs& aFs);

        /**
        * Destruction
        */
        virtual ~CFileHarvester();
                
        /**
        * Start harvesting File items
        * @param aDriveNumber drive to start harvesting
        */
        void StartL( const TDriveNumber aDriveNumber );

        /**
        * Add ignore paths
        * @param aDriveNumber drive to add the ignore paths
        */
        void AddIgnorePathsL( const TDriveNumber aDriveNumber );

        /**
        * Remove ignore paths
        * @param aDriveNumber drive to remove the paths from
        */
        void RemoveIgnorePaths( const TDriveNumber aDriveNumber );
        /**
        *  On Resume plugin, continue with file harvesting       
        */
        void ResumeRequest();

    private:  //From CActive

	    /**
	     * From CACtive.
	     */
        void DoCancel();

        /**
	     * From CACtive.
         */
        void RunL();

        /**
	     * From CACtive.
	     * @param aError Leave code from RunL()
	     * @return Error code, KErrNone if error was handled successfully.
         */
        TInt RunError( TInt aError );
    
    private:

        /**
        * Private constructor
        */    
        CFileHarvester( CFilePlugin& aFilePlugin, RFs& aFs );
        
        /**
        * 2nd phase construction
        */    
        void ConstructL();
        
        /**
        * Get next messaging folder 
        */
        void GetNextFolderL();
        
        /**
        * Get File identifier
        */ 
        void GetFileIdL();
        
        /**
        * Checks if harvesting is ready or not.
        * @return ETrue, if ready.
        */
        void SetNextRequest( TFileHarvesterState aState );

        TInt AddIgnorePathL( const TFileName& aIgnorePath );

        TInt RemoveIgnorePath( const TFileName& aIgnorePath );

        TBool CheckPath( const TFileName& aFileName );

    private:
        // File plugin for observing
        CFilePlugin& iFilePlugin; 
        
        // File harvester state.
        TFileHarvesterState iHarvestState;
        
        // RFs
        RFs& iFs;

        // The drive being harvested now
        TDriveNumber iCurrentHarvestDrive;

        CDir* iDir;
        CDirScan* iDirscan;
        TInt iCurrentIndex;
        TInt iStepNumber;
        RPointerArray<TFileName> iIgnorePaths;
        
        #ifdef HARVESTERPLUGINTESTER_FRIEND
            friend class CHarvesterPluginTester;
        #endif
    };

#endif // CFILEHARVESTER_H

// End of File
