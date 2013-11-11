/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  copy of normally used "system" file for harvester client
*
*/


#ifndef __HARVESTER_CLIENT_H__
#define __HARVESTER_CLIENT_H__

#include <e32std.h>

class MHarvestObserver
	{
public:
    /**
     * Callback to inform when fast harvesting of a file is complete.
     * 
     * @param aURI    URI of the harvested file.
     * @param aError  Error code of the fast harvesting. <code>KErrNone</code> is
     *                expected if fast harvesting succeeded. Otherwise some
     *                system wide error code. 
     *
     */
	virtual void HarvestingComplete( TDesC& aURI, TInt aError ) = 0;
	};



/**
 * Harvester client session class which provides also means to:
 * - Pause/resume the Harvesting framework
 * - Fast harvest files and observe when they have been harvested
 * - Setup observers to observe the harvesting progress
 */
class RHarvesterClient
	{

	public:
		
        /**
         * Constructor
         */
        RHarvesterClient();

        /**
         * Public method to connect Harvester server.
         * 
         * @return Symbian OS error code.
         */
        TInt Connect();
         
        /**
         * Public method to close session to Harvester server.
         */
        void Close();
            
        /**
         * Public method to set observer for fast harvesting.
         * Only one observer is currently supported.
         *
         * @param aObserver  Pointer to the observer
         */     
        void SetObserver(MHarvestObserver* aObserver);

        /**
         * Public method to remove observer for fast harvesting.
         *
         * @param aObserver  Pointer to the observer
         */     
        void RemoveObserver(MHarvestObserver* aObserver);
        
        /**
         * Public method for doing fast harvesting. These files
         * are handled first before the ones that are in the harvesting 
         * queue inside the Harvesting framework.
         *
         * @param aURI          Uri of the file to be harvested
         * @param aAlbumIds     Array of album id's that the harvested file 
         *                      will be included in
         * @param aAddLocation  Should location information be added to item
         */ 
    	void HarvestFile(const TDesC& aURI, RArray<TUint32> &aAlbumIds, TBool aAddLocation);
        
        
	private: 

	    /**
	     * Observer of the class
	     */ 
	     MHarvestObserver* mObserver;

    	};

#endif // __HARVESTER_CLIENT_H__
