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
* Description:  Message store multipart message support.
*
*/



#ifndef __MSG_STORE_MESSAGE_PART_H__
#define __MSG_STORE_MESSAGE_PART_H__

//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStorePropertyContainerWithContent.h"
//</cmail>

class CMsgStoreMessage;

/** This class represents a part of a message */
class CMsgStoreMessagePart : public CMsgStorePropertyContainerWithContent
	{
	public:

        /** check if this part contains any child parts */
		//IMPORT_C TBool IsMultiPart();

        /** Adds a child part to this part 

            /param aProperties the headers of the newly added part
            /param aContentFilenams optional parameter that contains the name of the content/attachment file.
            
            /note This method leaves with KErrNotSupported if it is NOT multi-part, i.e. this part
            already has content.
         */
		IMPORT_C CMsgStoreMessagePart* AddChildPartL( const CMsgStorePropertyContainer& aProperties,
                                                      const TDesC& aContentFilename = KNullDesC      );
//cmail
		
        /** Adds a child part to this part and set the content to the same content stored in the input file handle

            /param aProperties the headers of the newly added part
            /param aFile file handle that contains the content.
//cmail
                   NOTE 1: RFs.ShareProtected() must have been called before calling this method.
                   NOTE 2: Ownersip of the file handle is transferred to this method.
//~cmail
            
            /note This method leaves with KErrNotSupported if it is NOT multi-part, i.e. this part
            already has content.
         */
		IMPORT_C CMsgStoreMessagePart* AddChildPartL( const CMsgStorePropertyContainer& aProperties,
                                                      RFile& aFile );

//~cmail
		
        /** Adds an embedded message to this part 
         * 
            /param aProperties the headers of the newly added part
            /note This method leaves with KErrNotSupported if it is NOT multi-part, i.e. this part
            already has content.
         */
		IMPORT_C CMsgStoreMessage* AddChildMessageL( CMsgStorePropertyContainer& aProperties );

		/** Copies an attachment from one message to another message.
			
			This function copies an an attachment, including the properties and content, from one
			message to another message.  This function will leave with KErrNotFound if the specified
			message is not found in the source folder or if the destination message is not found in
			the destination folder.
	
            KMsgStoreInvalidId may be passed for the destination folder ID if there is no desire to 
            check the source folder.
            
			ECapabilityWriteUserData is required to use this function.		
			
			\param aNewParentId the destination parent, either a message or a part	
			
			\retval Returns the ID of the new message.
	
		    \note Only works if the store is in an authenticated state, otherwise this function leaves
		          with KErrNotReady.
		*/		
		IMPORT_C TMsgStoreId CopyChildPartL( TMsgStoreId aPartId,
		                                      TMsgStoreId aNewParentId );
		
        /** Removes a child part from this part 
            \param aRecursive if true, recursively search into the tree 
         */
		IMPORT_C void RemoveChildPartL( TMsgStoreId aPartId, TBool aRecursive = EFalse );

        /** gets a child part fo this part 
            \param aRecursive if true, recursively search into the tree 
         */
		IMPORT_C CMsgStoreMessagePart* ChildPartL( TMsgStoreId aPartId, TBool aRecursive = EFalse  );

        /** list all the child part to this part 
            /note This method leaves with KErrNotSupported if it is NOT multi-part.
        */
		IMPORT_C void ChildPartsL( RPointerArray<CMsgStoreMessagePart>& aParts );
		
        /** flag that indicates whether this part is actually a message.
            /note This method can be used to check if a child part is actually 
                  an embedded message, if it is, than it is safe to cast
                  this part to CMsgStoreMessage.  This can avoid run-time
                  type checking.
        */
		IMPORT_C TBool IsEmbeddedMessageL();

		// ---------------------------
		// INTERNAL USE (NOT EXPORTED)
		// ---------------------------
		
		static CMsgStoreMessagePart* NewL( CMsgStoreSessionContext& aSessionContext, 
									       TMsgStoreId              aId, 
									       TMsgStoreId              aParentId, 
									       TMsgStoreId              aMailBoxId,
									       const TDesC8&            aPropertiesBuf );

	protected:
	
		CMsgStoreMessagePart( CMsgStoreSessionContext& aSessionContext, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aMailBoxId ); 
	
		void ConstructL( const TDesC8& aPropertiesBuf );
		
	}; // end CMsgStoreMessagePart
		
#endif // __MESSAGE_STORE_CLIENT_H__
