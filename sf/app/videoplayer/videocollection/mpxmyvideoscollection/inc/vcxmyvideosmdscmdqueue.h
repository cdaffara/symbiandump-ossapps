/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Queues commands to MDS.
*/



#ifndef VCXMYVIDEOSMDSCMDQUEUE_H
#define VCXMYVIDEOSMDSCMDQUEUE_H

// INCLUDES
#include <vcxmyvideosdefs.h>
#include "vcxmyvideosalbum.h"
#include "vcxmyvideosmdsdb.h"

// FORWARD DECLARATIONS
class CMPXMedia;

// CONSTANTS

// CLASS DECLARATION

/**
* All MDS commands inherit this.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmd) : public CBase
    {    
    public:

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmd() {}
        
        /**
        * Command type. Used for casting the pointer to correct type.
        */
        CVcxMyVideosMdsDb::TRequestType iCmdType;
        
        /**
         * Pointer to client is stored here, used for performing response call.
         */
        MVcxMyVideosMdsAlbumsObserver* iClient;    
    };

/**
* Class which encapsulates GetVideoListL function parameters.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdGetVideoList) : public CVcxMyVideosMdsCmd
    {    
    public:

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdGetVideoList() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdGetVideoList() {}

    public:
        /**
        * Stored CreateVideoListL function parameters, see meanings from vcxmyvideosmdsdb.h file.
        */
        TVcxMyVideosSortingOrder iSortingOrder;
        TBool                    iAscending;
        TBool                    iFullDetails;
        CMPXMedia**              iVideoList;
    };

/**
* Class which stores GetAlbumsL function arguments.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdGetAlbums) : public CVcxMyVideosMdsCmd
    {    
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdGetAlbums() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdGetAlbums() {}

    public:
        /**
        * Stored GetAlbumsL function parameters, see meanings from vcxmyvideosmdsalbums.h file.
        */
        CMPXMedia* iAlbumList;
    };

/**
* Class which stores GetAlbumContentIdsL function arguments.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdGetAlbumContentIds) : public CVcxMyVideosMdsCmd
    {    
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdGetAlbumContentIds() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdGetAlbumContentIds() {}

    public:
        /**
        * Stored GetAlbumContentL function parameters, see meanings from vcxmyvideosmdsalbums.h file.
        */
        TUint32                         iAlbumId;        
        RArray<TVcxMyVideosAlbumVideo>* iAlbumContent; //not own
    };

/**
* Class which stores GetAlbumContentVideosL function arguments.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdGetAlbumContentVideos) : public CVcxMyVideosMdsCmd
    {    
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdGetAlbumContentVideos() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdGetAlbumContentVideos() {}

    public:
        /**
        * Stored GetAlbumContentL function parameters, see meanings from vcxmyvideosmdsalbums.h file.
        */
        TUint32    iAlbumId;        
        CMPXMedia* iAlbumContentVideos; //not own
    };

/**
* Class which stores AddVideosToAlbumL function arguments.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdAddVideosToAlbum) : public CVcxMyVideosMdsCmd
    {    
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdAddVideosToAlbum() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdAddVideosToAlbum() {}

    public:
        /**
        * Stored AddVideosToAlbumL function parameters, see meanings from vcxmyvideosdefs.h
        * KVcxCommandMyVideosAddToAlbum command descriptions.
        */
        CMPXMedia* iMpxCmd;
    };

/**
* Class which stores RemoveRelationsL function arguments.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdRemoveRelations) : public CVcxMyVideosMdsCmd
    {    
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdRemoveRelations() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdRemoveRelations() {}

    public:
        /**
        * Stored RemoveRelationsL function parameters, see meanings from vcxmyvideosmdsalbums.h file.
        */
        RArray<TUint32>* iRelationIds; //not own
        RArray<TUint32>* iResults;  //not own
    };

/**
* Class which stores RemoveAlbumsL function arguments.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdRemoveAlbums) : public CVcxMyVideosMdsCmd
    {    
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdRemoveAlbums() {}

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdRemoveAlbums() {}

    public:
        /**
        * Stored RemoveAlbumsL function parameters, see meanings from vcxmyvideosmdsalbums.h file.
        */
        CMPXMedia* iMpxCmd; //not own
    };

/**
* Queues MDS commands.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsCmdQueue) : public CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @return object constructed
        */
        static CVcxMyVideosMdsCmdQueue* NewL( CVcxMyVideosMdsDb& aMdsDb );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsCmdQueue();

    public: // new methods
        
        /**
        * Executes command if queue is empty, otherwise adds it to queue.
        * Ownership of aCmd moves to this object.
        *
        * @param aCmd  Command to execute.
        */
        void ExecuteCmdL( CVcxMyVideosMdsCmd* aCmd );
        
        /**
        * Called when command finishes. Executes next command from the queue.
        */
        void CmdFinished();
        
        /**
         * Removes commands from command queue.
         * 
         * @param aType Defines what kind of commands are removed from the command queue.
         */
        void Cancel( CVcxMyVideosMdsDb::TRequestType aType = CVcxMyVideosMdsDb::EAll );
                        
    private:

        /**
        * Constructor
        */
        CVcxMyVideosMdsCmdQueue( CVcxMyVideosMdsDb& aMdsDb );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Executes the command.
        *
        * @param aCmd  Command to execute, ownership does not move.
        */
        void DoExecuteCmdL( CVcxMyVideosMdsCmd* aCmd );
                    
    private:
        
        /**
        * Owner of this object.
        */
        CVcxMyVideosMdsDb& iMdsDb;
        
        /**
        * Command queue. Own.
        */
        RArray<CVcxMyVideosMdsCmd*> iQueue;
        
        /**
        * Set to ETrue when command is started and to EFalse
        * when command finishes.
        */
        TBool iCmdInProgress;
    };

#endif   // VCXMYVIDEOSMDSCMDQUEUE_H


