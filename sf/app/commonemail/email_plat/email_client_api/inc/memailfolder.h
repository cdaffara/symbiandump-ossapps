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
* Description: Email message API
*
*/

#ifndef _M_EMAILFOLDER
#define _M_EMAILFOLDER

#include <emailapidefs.h>
#include <emailsorting.h>

namespace EmailInterface {

class MEmailFolder;
class MMessageIterator;

typedef  RPointerArray<MEmailFolder> RFolderArray;

/**
* folder types
*/
enum TFolderType
    {
    EInbox,
    EOutbox,
    EDrafts,
    EDeleted,
    ESent,
    EOther
    };

/**
* Email folder interface 
* This is used for accessing folder information and included messages.
* Operations affecting folder itself are not provided.
*
* Use case: for accessing messages in specific folder use MEmailMessageQuery
* and set folder id with MEmailMessageQuery::SetFolderIdL()
*
* @since S60 v5.0
*/
class MEmailFolder : public MEmailInterface
{
public:
    /**
    * Returns folder id.
    * @return folder type
    */
    virtual TFolderId FolderId() const = 0;
    
    /**
    * Returns parent folder id.
    * @return parent folder id
    */
    virtual TFolderId ParentFolderId() const = 0;
    
    /**
    * Returns folder type.
    * @return folder type
    */
    virtual TFolderType FolderType() const = 0;    

    /**
    * Returns folder name.
    * @return folder name pointer descriptor
    */
    virtual TPtrC Name() const = 0;

    /**
    * Returns direct children of this folder, i.e. this is not recursive.
    * @return number of subfolders or an error code
    */    
    virtual TInt GetSubfoldersL( 
        RFolderArray& aSubfolders ) const = 0;

    /**
    * Returns message iterator for iterating messages in the folder
    * @param aSortCriteria sort criteria
    * @return message iterator
    */
    virtual EmailInterface::MMessageIterator* MessagesL( 
        const RSortCriteriaArray& aCriteria ) = 0;
    
    /**
    * Deletes messages in this folder
    * @param aMessageIds messages to delete. This method is no-op if
    * array is empty. It is expected that messages in the array are
    * located in same folder or KErrArgument exception is raised.
    */
    virtual void DeleteMessagesL( const REmailMessageIdArray& aMessageIds ) = 0;
};

} // namespace EmailInterface

#endif // _M_EMAILFOLDER
