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
* Description:   Message store property container with content.
*
*/



#ifndef __MSG_STORE_PROPERTY_CONTAINER_WITH_CONTENT_H__
#define __MSG_STORE_PROPERTY_CONTAINER_WITH_CONTENT_H__

#include <f32file.h>
//<cmail>
#include "MsgStoreWritablePropertyContainer.h"
//</cmail>


/** This class adds the capability to store content to a property container.

	This class adds the capability to store content with a set of name/value pairs.  The content can be
	accessed via descriptor buffers or file handles.  File handles are used in order to more easily
	transfer large data within a data-caged environment.
*/
class CMsgStorePropertyContainerWithContent : public CMsgStoreWritablePropertyContainer
	{
	public:

		/** Returns the type of the current content. */	
		IMPORT_C const TDesC& ContentTypeL();

		/** Sets the type of the current content. */	
		IMPORT_C void SetContentTypeL(const TDesC& aType);

		/** Returns the length of the current content. */	
		IMPORT_C TUint ContentLengthL();
	
		/** Fetches the content into a buffer.
		
		    This function reads a portion or all of the content into a contiguous RAM buffer provided
		    by the caller. 	The content will be will be truncated if the amount in local storage is
		    larger than the given buffer.
		    
			ECapabilityReadUserData is required to use this function.						
		*/
		IMPORT_C void FetchContentToBufferL( TDes8& aBuffer, TUint aStartOffset = 0 );

		/** Fetches the content into the designated file. 
		
		    This function copies the entire content buffer into the given file.  The given file is opened
		    in the caller's process, so any file that is accessible by the caller can be used in this call
		    (including private files).
		    
			ECapabilityReadUserData is required to use this function.						
		*/
		IMPORT_C void FetchContentToFileL( const TDesC& aFilename );
		
		/** Wipes the contents of the content from local storage. 
		
			ECapabilityWriteUserData is required to use this function.						
		*/
		IMPORT_C void RemoveContentL();
	
		/** Appends the given buffer to the content in the message store.
		
			ECapabilityWriteUserData is required to use this function.						
		*/
		IMPORT_C void AppendToContentL( const TDesC8& aBuffer );
	
		/** Replaces the current content with the given buffer.
		
			ECapabilityWriteUserData is required to use this function.						
		*/
		IMPORT_C void ReplaceContentL( const TDesC8& aBuffer );
	
		/** Replaces the current content with the contents of the given file.  
		
			ECapabilityWriteUserData is required to use this function.  The given file is opened
		    in the caller's process, so any file that is accessible by the caller can be used in this call
		    (including private files).						
		*/
		IMPORT_C void ReplaceContentWithFileL( const TDesC& aFilename );
        
        /** Opens the original content file and transfer the file handle to the provided RFile handle.  
        
            This method is intended to be used when the message store is NOT encrypted.
            It leaves with KErrNotSupported if the content is encrypted.            
            The file will be opened with EFileRead and EFileShareAny
            ECapabilityReadUserData is required to use this function.   
        */
        IMPORT_C void OpenOriginalContentFileL( RFile& aFile );
        
        /** Prepends the given buffer to the content in the message store.
        
            ECapabilityWriteUserData is required to use this function.                      
        */
        IMPORT_C void PrependToContentL( const TDesC8& aBuffer );

	protected:

	 	CMsgStorePropertyContainerWithContent( CMsgStoreSessionContext& aSessionContext, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aMailBoxId );

	 	TBool iHasContent;
	
	}; // end class CMsgStorePropertyContainerWithContent

#endif   //__MSG_STORE_WRITABLE_PROPERTY_CONTAINER_H__
