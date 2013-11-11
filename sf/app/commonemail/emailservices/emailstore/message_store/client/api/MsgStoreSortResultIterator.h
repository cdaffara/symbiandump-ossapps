/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store sort result iterator.
*
*/



#ifndef __MSG_STORE_SORT_RESULT_ITERATOR_H__
#define __MSG_STORE_SORT_RESULT_ITERATOR_H__

//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStorePropertyContainer.h"
//</cmail>

class TMsgStoreIdAndFlag
	{
	public:
		inline TMsgStoreIdAndFlag(TMsgStoreId aId, TUint32 aFlag) : iId(aId), iFlag(aFlag) {}
		
		TMsgStoreId iId;
		TUint32     iFlag;
	};

class CMsgStoreSessionContext;

/** This class represents a result set of sorting opreation.

	The sorted result set is stored on the message store server, this class
	provides methods to access the results in "chunks", i.e., several rows
    at a time.
*/
class CMsgStoreSortResultIterator : public CBase
	{
	public:
	    
		// ---------------------------
		// INTERNAL USE (NOT EXPORTED)
		// ---------------------------
		static CMsgStoreSortResultIterator* NewL( CMsgStoreSessionContext& aContext, TMsgStoreId aSortSessionId );
        
        /**
         * Destructor
         */
        virtual ~CMsgStoreSortResultIterator();
        
        /** Retrieves the next N messages in the sorted result set.
         * 
         *  \param aCurrentMessageId (IN) retrieve the messages that are IMMEDIATELLY AFTER this message.
         *                     The current message will NOT be included in the return results. 
         *                     Use KMsgStoreSortResultTop to indicate retrieving from the top.
         * 
         *  \param aCount (IN) the number of messages to retrieve
         * 
         *  \param aProperties (OUT) the array of message properties been retrieved.  Each element in this
         *                     array represents a message. The message ID can be obtained by
         *                     CMsgStorePropertyContainer::Id().  The properties stored in each element
         *                     should match those properties keys added by CMsgStoreSortCriteria::AddResultPropertyL().
         *                     Note that after this method returns, aProperties.Count() is always <= aCount.
         *                     If there are less than aCount messages remain, than aProperties.Count() will < aCount.
         * 
         *  \retval ETrue if there are more messages AFTER the last element returned in this call.
         *          EFalse otherwise.
         * 
         *  : Include sample code:       
         *       
        */
        IMPORT_C TBool NextL( TMsgStoreId                                aCurrentMessageId, 
                              TUint                                      aCount, 
                              RPointerArray<CMsgStorePropertyContainer>& aProperties );
        
        /** Retrieves the next N messages from the row that starts with the specified prefix.
         * 
         *  This method can be used when sorting by string fields: sender or subject.
         *  When the UI displays a sorted list, the user can start typing and the UI
         *  needs to display the mathcing rows.  This method is designed for this purpose.
         * 
         *  \param aStartWith (IN) the leading char or stirng of the subject/sender.
         * 
         *  \param aCount (IN) the number of messages to retrieve, including the first mathing one.
         * 
         *  \param aProperties (OUT) the array of message properties been retrieved.  Each element in this
         *                     array represents a message. The message ID can be obtained by
         *                     CMsgStorePropertyContainer::Id().  The properties stored in each element
         *                     should match those properties keys added by CMsgStoreSortCriteria::AddResultPropertyL().
         *                     Note that after this method returns, aProperties.Count() is always <= aCount.
         *                     If there are less than aCount messages remain, than aProperties.Count() will < aCount.
         * 
         *  \retval ETrue if there are more messages AFTER the last element returned in this call.
         *          EFalse otherwise.
         * 
         *  : Include sample code:       
         *       
        */
        
        IMPORT_C TBool NextL( const TDesC&                               aStartWith, 
                              TUint                                      aCount, 
                              RPointerArray<CMsgStorePropertyContainer>& aProperties );
        
        /** Retrieves the next N messages in the sorted result set.
         * 
         *  \param aCurrentMessageId (IN) retrieve the messages that are IMMEDIATELLY BEFORE this message.
         *                     The current message will NOT be included in the return results.
         *                     Use KMsgStoreSortResultBottom to indicate retrieving from the bottom.
         * 
         *  \param aCount (IN) the number of messages to retrieve
         * 
         *  \param aProperties (OUT) the array of message properties been retrieved.  Each element in this
         *                     array represents a message. The message ID can be obtained by
         *                     CMsgStorePropertyContainer::Id().  The properties stored in each element
         *                     should match those properties keys added by CMsgStoreSortCriteria::AddResultPropertyL().
         *                     Note that after this method returns, aProperties.Count() is always <= aCount.
         *                     If there are less than aCount messages remain, than aProperties.Count() will < aCount.
         * 
         *  \retval ETrue if there are more messages BEFORE the last element returned in this call.
         *          EFalse otherwise.
         * 
         *  \note   The order of this message is
         *
         *  : Include sample code:       
        */
        IMPORT_C TBool PreviousL( TMsgStoreId                                aCurrentMessageId, 
                                  TUint                                      aCount, 
                                  RPointerArray<CMsgStorePropertyContainer>& aProperties );

        /** Retrieves the previous N messages from the row that starts with the specified prefix.
         * 
         *  This method can be used when sorting by string fields: sender or subject.
         *  When the UI displays a sorted list, the user can start typing and the UI
         *  needs to display the mathcing rows.  This method is designed for this purpose.
         * 
         *  \param aStartWith (IN) the leading char or stirng of the subject/sender.
         * 
         *  \param aCount (IN) the number of messages to retrieve, including the first matching one.
         * 
         *  \param aProperties (OUT) the array of message properties been retrieved.  Each element in this
         *                     array represents a message. The message ID can be obtained by
         *                     CMsgStorePropertyContainer::Id().  The properties stored in each element
         *                     should match those properties keys added by CMsgStoreSortCriteria::AddResultPropertyL().
         *                     Note that after this method returns, aProperties.Count() is always <= aCount.
         *                     If there are less than aCount messages remain, than aProperties.Count() will < aCount.
         * 
         *  \retval ETrue if there are more messages BEFORE the last element returned in this call.
         *          EFalse otherwise.
         * 
         *  \note   The order of this message is
         *
         *  : Include sample code:       
        */
        IMPORT_C TBool PreviousL( const TDesC&                               aStartWith,  
                                  TUint                                      aCount, 
                                  RPointerArray<CMsgStorePropertyContainer>& aProperties );
        
        /** Skip the current "group" and retrieves the next N messages in the next "group".
         * 
         *  \param aCurrentMessageId (IN) The message in the current "group" that we skip.
         *                     Use KMsgStoreSortResultTop to indicate to skip the first group.
         * 
         *  \param aCount (IN) the number of messages to retrieve
         * 
         *  \param aProperties (OUT) the array of message properties been retrieved.  Each element in this
         *                     array represents a message. The message ID can be obtained by
         *                     CMsgStorePropertyContainer::Id().  The properties stored in each element
         *                     should match those properties keys added by CMsgStoreSortCriteria::AddResultPropertyL().
         *                     Note that after this method returns, aProperties.Count() is always <= aCount.
         *                     If there are less than aCount messages remain, than aProperties.Count() will < aCount.
         * 
         *  \retval ETrue if there are more messages AFTER the last element returned in this call.
         *          EFalse otherwise.
         * 
         *  : Include sample code:       
         *       
        */
        IMPORT_C TBool SkipAndNextL( TMsgStoreId                                aCurrentMessageId, 
                                     TUint                                      aCount, 
                                     RPointerArray<CMsgStorePropertyContainer>& aProperties );
        
        /** Skip the current "group" and retrieves the previous N messages in the previous "group".
         * 
         *  \param aCurrentMessageId (IN) the message in the current group that we skip.
         *                     Use KMsgStoreSortResultBottom to indicate retrieving from the bottom.
         * 
         *  \param aCount (IN) the number of messages to retrieve
         * 
         *  \param aProperties (OUT) the array of message properties been retrieved.  Each element in this
         *                     array represents a message. The message ID can be obtained by
         *                     CMsgStorePropertyContainer::Id().  The properties stored in each element
         *                     should match those properties keys added by CMsgStoreSortCriteria::AddResultPropertyL().
         *                     Note that after this method returns, aProperties.Count() is always <= aCount.
         *                     If there are less than aCount messages remain, than aProperties.Count() will < aCount.
         * 
         *  \retval ETrue if there are more messages BEFORE the last element returned in this call.
         *          EFalse otherwise.
         * 
         *  \note   The order of this message is
         *
         *  : Include sample code:       
        */
        IMPORT_C TBool SkipAndPreviousL( TMsgStoreId                                aCurrentMessageId, 
                                         TUint                                      aCount, 
                                         RPointerArray<CMsgStorePropertyContainer>& aProperties );
        
        /** Get the total number of "groups" in this iteratorand.
         * 
        */
        IMPORT_C TInt GroupCountL( RArray<TUint>& aItemsInGroup );
        
        /** Retrieves ALL the IDs and Flags in the sorted order.
         *  NOTE: Customization work for Gimlet 2.0 
         *  The caller needs to Reset or Close the aIdsAndFlags array after this method returns.
         */
        IMPORT_C void IdsAndFlagsL( RArray<TMsgStoreIdAndFlag>& aIdsAndFlags );
        
        /** Find the index a message in the sorted order.
         *  NOTE: Customization work for Gimlet 2.0 
         */
        IMPORT_C TInt IndexOfL( TMsgStoreId aMessageId );
        
        /** Retrieve all message ids in the sorted order.
         *  NOTE: Customization work for Gimlet 2.0 
         */
        IMPORT_C void MessageIdsL( RArray<TMsgStoreId>& aMessageIds );
        
        /** Retrieve all message ids in the sorted order and 
         *  retrieve the number groups in this iterator.
         *  NOTE: Customization work for Gimlet 2.0 
         */
        IMPORT_C void IdsAndGroupCountL( RArray<TMsgStoreId>& aMessageIds, RArray<TUint>& aItemsInGroup );
        
    protected:
    
        CMsgStoreSortResultIterator( CMsgStoreSessionContext& aContext, TMsgStoreId aSortSessionId );
        void ConstructL();
		
	private:
    
	    CMsgStoreSessionContext& iContext;

        TMsgStoreId              iSortSessionId;
	
	}; // end class CMsgStoreSortResultIterator

#endif   //__MSG_STORE_SORT_RESULT_ITERATOR_H__
