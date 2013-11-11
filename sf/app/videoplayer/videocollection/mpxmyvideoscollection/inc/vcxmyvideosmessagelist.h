/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Groups messages and sends them to clients.*
*/




#ifndef VCXMYVIDEOSMESSAGELIST_H
#define VCXMYVIDEOSMESSAGELIST_H

// INCLUDES

#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>

// FORWARD DECLARATIONS
class CVcxMyVideosCollectionPlugin;

// CONSTANTS

// CLASS DECLARATION

/**
* Groups messages and sends them to clients.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMessageList) :
    public CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        *
        * @param aCollection Collection, owner of this object.  
        * @return            Object constructed
        */
        static CVcxMyVideosMessageList* NewL ( CVcxMyVideosCollectionPlugin& aCollection );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMessageList();
 
    private:
    
        /**
        * Constructor
        */
        CVcxMyVideosMessageList( CVcxMyVideosCollectionPlugin& aCollection );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();
        
        /**
        * Creates iMessageList if not created already.
        */
        void CreateMessageListL();

        /**
        * Checks if this event is already on the message list. Used for
        * optimizing duplicate events.
        *
        * @param aId         MPX Item ID.
        * @param aChange     Change type.
        * @param aExtraInfo  Event extra info, see TVcxMyVideosEventInfo enum
        *                    from vcxmyvideosdefs.h for values.
        * @return            ETrue if the event is already on the list,
        *                    EFalse otherwise.
        */
        TBool AlreadyInMessageListL( const TMPXItemId& aId,
                TMPXChangeEventType aChange, TInt32 aExtraInfo );    
    public:
    
        /**
        * Creates collection change event and appends it to iMessageList.
        *
        * @param aId        Context that was changed
        * @param aChange    Change type, see values from mpxcollectionmessagedefs.h.
        * @param aExtraInfo Extra information about the event, this value is written to
        *                   KVcxMediaMyVideosInt32Value attribute. If aExtraInfo
        *                   is 0, nothing is written. See TVcxMyVideosEventInfo enum
        *                   from vcxmyvideosdefs.h for values.
        * @param aVideo     Video object which is in cache already.
        */
        void AddEventL(const TMPXItemId& aId, TMPXChangeEventType aChange,
                TInt32 aExtraInfo = 0, CMPXMedia* aVideo = NULL );

        /**
        * Creates My Videos custom event and appends it to iMessageList.
        *
        * @param aEvent Event ID, see values from vcxmyvideosdefs.h.
        */
        void AddEventL( TInt aEvent );

        /**
        * Adds message to iMessageList.
        *
        * @param aMessage  Message to add, may contain media array as a parameter.
        */
        void AddL( CMPXMessage* aMessage );
                
        /**
        * Sends iMessageList to clients.
        */
        void SendL();

    private:
            
        /**
        * Messages are collected here before sending.
        */
        CMPXMessage* iMessageList;        

        /**
        * Array item of iMessageList, stored as member to speed things up.
        * Owned by iMessageList.
        */
        CMPXMessageArray* iMessageArray;

        /**
        * Count of messages in iMessageList, main object plus items in array.
        */
        TInt iMessageCount;
        
        /**
        * My Videos collection plugin, owner of this object.
        */
        CVcxMyVideosCollectionPlugin& iCollection;
    };

#endif   // VCXMYVIDEOSMESSAGELIST_H


