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
* Description:  Harvester message plugin
*
*/


#ifndef CMESSAGEHARVESTER_H
#define CMESSAGEHARVESTER_H

//  INCLUDES 

#include <e32base.h>
#include <msvapi.h>
//#include <mdeitem.h>

#include "cmessageplugin.h"


class CMessageHarvester : public CActive
    {
    public:
        /**
        * Construction
        * @return Harvester image plugin
        */
        static CMessageHarvester* NewL( CMessagePlugin& aMessagePlugin, CMsvSession& aSession );

        /**
        * Destruction
        */
        virtual ~CMessageHarvester();
                
        /**
        * Start harvesting message items
        */
        void StartHarvestingL();
        
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
        CMessageHarvester( CMessagePlugin& aMessagePlugin, CMsvSession& aSession );

        /**
        * Get next messaging folder 
        */
        void GetNextFolderL();
        
        /**
        * List subfolder from current folder
        * @param aFolderEntry Folder entry
        */        
        void ListSubFoldersL(  const CMsvEntry& aFolderEntry );
        
        /**
        * List subfolder from current folder
        * @param aFolderId a folder identifier
        */ 
        void AddFolderWithSubFoldersL( const TMsvId& aFolderId );
        
        /**
        * Get message folders under the messaging   
        */ 
        void GetMessageFoldersL();
        
        /**
        * Get message identifier
        */ 
        void GetMessageIdL();

        /**
        * Handles next request
        */
        void HandleNextRequest();      

    private:
    	enum TState
    		{
    		EStateIdle, // Stopped
    		EStateNextFolder, // Getting the next folder 
    		EStateNextMessage, // Getting the next message
    		};
    	
    private:
        // Whether Harvesting is ready   
        TBool iHarvestCompleted;
        // Message plugin for observing
        CMessagePlugin& iMessagePlugin; 
        // Session Message server session, own
        CMsvSession& iMsvSession;
        // Internal state machine state
        TState iState;
        // Array of folders to be searched  
        RArray<TMsvId> iToBeSearchedFolders;
        // UID of current folder searched
        TMsvId iCurrentFolder;
        // Current folder message items 
        CMsvEntrySelection* iChildren;
        
#ifdef HARVESTERPLUGINTESTER_FRIEND
    friend class CHarvesterPluginTester;
#endif
    };

#endif // CMESSAGEHARVESTER_H

// End of File
