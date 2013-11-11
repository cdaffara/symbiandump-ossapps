
/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: stub collection for testing VideoCollectionListener
* 
*/

#ifndef __CSTUBVIDECOLLECTION_H__
#define __CSTUBVIDECOLLECTION_H__

#include <mpxcollectionobserver.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>


class StubCollection
{
public:
    
    /**
     * contructor
     */
    StubCollection(MMPXCollectionObserver &observer);
    
    /**
     * destructor
     */
    virtual ~StubCollection();
    
    /**
     * calls MMPXCollectionMediaObserver::HandleCollectionMediaL 
     */
    void callHandleCollectionMediaLFunc(const CMPXMedia& aMedia, TInt aError);
    
    /**
     * calls MMPXCollectionMediaObserver::HandleOpenL 
     */
    void callHandleOpenLFunc(const CMPXMedia& aEntries, TInt aIndex, TBool aComplete, TInt aError);
    
    /**
     * calls MMPXCollectionMediaObserver::HandleOpenL 
     */
    void callHandleOpenLFunc(const CMPXCollectionPlaylist& aPlaylist, TInt aError);
    
    /**
     * calls MMPXCollectionMediaObserver::HandleCommandComplete 
     */
    void callHandleCommandComplete(CMPXCommand* aCommandResult, TInt aError);
    
    /**
     * calls MMPXCollectionMediaObserver::HandleCollectionMessage 
     */
    void callHandleCollectionMessage(CMPXMessage* aMessage, TInt aError);

private:

    /**
     * observer
     */
    MMPXCollectionObserver &mObserver;
    
};

#endif
