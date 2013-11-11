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
* Description: Email message iterator
*
*/

#ifndef CMESSAGEITERATOR_H
#define CMESSAGEITERATOR_H
                                                                             
#include <mmessageiterator.h>                                                                             
#include "emailapidefs.h"
#include "CFSMailCommon.h"

using namespace EmailInterface;

class MFSMailIterator;
class CPluginData;
class CFSMailMessage;
class CEmailMessage;
/**
 * Iterator for email messages
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( CMessageIterator ) : public CBase, public MMessageIterator
{
public:
    /**
    * Constructor
    * @param aIterator
    * @param aPluginData
    * @return new iterator
    */
    static CMessageIterator* NewL( MFSMailIterator* aIterator,
                                   CPluginData& aPluginData,
                                   TUint aCount );
    
    ~CMessageIterator();

private:
    CMessageIterator( MFSMailIterator* aIterator, CPluginData& aPluginData, TUint aCount );
    
    void ConstructL();
    
public: // from MEmailInterface              

    TEmailTypeId InterfaceId() const;

    void Release();
    
public: // from MMessageIterator

    /** @see MMessageIterator */
    MEmailMessage* NextL();
    
    /** @see MMessageIterator 
     (not implemented) */
    
    MEmailMessage* PreviousL();
    
    TUint Count() const;
        

private:   
    // Reads next chunk of messages from protocol plugin
    TBool ReadNextChunkL();
    TBool ReadPreviousChunkL();
    MEmailMessage* ReadFromChunkL();
    void CleanCache();    
    void AddToCacheL( CFSMailMessage* aFsMsg );    
private: // data
    
    /**
    * Iterator internal states. "Consuming" in state names mean getting and 
    * returning next message from the iterator.
    */
    enum TState {
        // No messages available in iterator, read them from protocl plugin.
        // NextL returns a message from chunk (or NULL if folder is empty)
        // This is initial state.
        EReadNextMessageChunk,
        EReadPreviousMessageChunk,
        
        // Message(s) are available (retrieved from plugin). No remaining 
        // messages excepted from the plugin. NextL returns a message from chunk.
        EConsumeFromChunk,
        
        // Message(s) are available in chunk and more in protocol plugin.
        EConsumeFromChunkWithMoreChunksToFollow,
        
        // Iterator is iterated throuh and NextL would return NULL.
        EIteratorConsumed
    };

    // plugin iterator, owned
    MFSMailIterator* iIterator;
    
    // reference to plugin data
    CPluginData&    iPluginData;

    CFSMailPlugin*  iPlugin;
    
    // message id used for reading messages from plugin
    TFSMailMsgId    iStartMsgId;
    
    // Internal state  
    TState           iState;
    
    // pointer array of messages read from protocol plugin (aka message chunk)
    RPointerArray<CFSMailMessage> iFsMessageArray;
    RPointerArray<CEmailMessage> iMessageArray;
    
    TUint iCount;
    
    TInt iCursor;
    
    TFSMailMsgId iFirstMsgId;
    
    TBool iHasMoreNextItems;
    TBool iHasMorePrevItems;
};

#endif // CMESSAGEITERATOR_H

// End of file
