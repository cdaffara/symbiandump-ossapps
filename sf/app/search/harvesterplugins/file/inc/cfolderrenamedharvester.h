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


#ifndef CFOLDERRENAMEDHARVESTER_H
#define CFOLDERRENAMEDHARVESTER_H

//  INCLUDES 
#include <e32base.h>

class CFilePlugin;

class CFolderRenamedItem : public CBase
	{
public:
	static CFolderRenamedItem * NewL ( const TDesC &aOldName, const TDesC &aNewName );

	virtual ~CFolderRenamedItem();

	const TDesC& OldName();
	const TDesC& NewName();

private:
	CFolderRenamedItem();
	void ConstructL( const TDesC &aOldName, const TDesC &aNewName );

private:
	HBufC *iOldName;
	HBufC *iNewName;
	};

class CFolderRenamedHarvester : public CActive
    {
    public:

    	enum TFileHarvesterState
    		{
    		EFolderRenamedIdleState,
    		EFolderRenamedStartHarvest,
    		EFolderRenamedGetFileId
    		};

        /**
        * Construction
        * @return Harvester image plugin
        */
        static CFolderRenamedHarvester* NewL(CFilePlugin& aFilePlugin, RFs& aFs);

        /**
        * Destruction
        */
        virtual ~CFolderRenamedHarvester();

        /**
        * Start harvesting File items
        * @return ETrue, if ready.
        */
        TBool StartL( const TFileName& aOldDirectoryName, const TFileName& aNewDirectoryName );

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
    	CFolderRenamedHarvester( CFilePlugin& aFilePlugin, RFs& aFs );

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
        
        void HandleFolderRenameL();

    private:
        // File plugin for observing
        CFilePlugin& iFilePlugin; 

        // File harvester state.
        TFileHarvesterState iHarvestState;

        // RFs
        RFs& iFs;
        CDir* iDir;
        CDirScan* iDirscan;
        TInt iCurrentIndex;
        TInt iStepNumber;

        TFileName iOldFolderName;
        TFileName iNewFolderName;
        RPointerArray <CFolderRenamedItem> iRenamedFolders;
        //for unit testing.
        #ifdef HARVESTERPLUGINTESTER_FRIEND
            friend class CHarvesterPluginTester;
        #endif
    };

#endif // CFOLDERRENAMEDHARVESTER_H

// End of File
