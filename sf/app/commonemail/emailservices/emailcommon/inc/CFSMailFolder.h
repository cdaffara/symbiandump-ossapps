/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email folder object
*
*/


#ifndef __FSMAILFOLDER_H
#define __FSMAILFOLDER_H

//<cmail>
#include "CFSMailFolderBase.h"
#include "MFSMailIterator.h"
//</cmail>

// forward declarations
class CFSMailIterator;

/**
 *  class for handling mailbox folder data
 *
 *  @lib FSFWCommonLib
 *  @since S60 S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailFolder ) : public CFSMailFolderBase
{
 public:
      
    /**
     * Two-phased constructor.
     *
     * @param aFolderId folder id in plugin containing folder
     */
     IMPORT_C static CFSMailFolder* NewL( TFSMailMsgId aFolderId );
    
    /**
     * Two-phased constructor.
     *
     * @param aFolderId folder id in plugin containing folder
     */
     IMPORT_C static CFSMailFolder* NewLC( TFSMailMsgId aFolderId );

    /**
     * Destructor.
     */  
     IMPORT_C ~CFSMailFolder();

    /**
     * List folder messages. Function returns iterator, which user can use
     * to scroll up/down message list.
     *
     * @param aDetails describes which details each CFSMailMessage contains
     * @param aSorting describes requested sort criteria.
     *        First item in array is primary sort criteria.
     *
     * @return email list iterator, ownership is transferred to user
     */
     IMPORT_C MFSMailIterator* ListMessagesL(
                                const TFSMailDetails aDetails,
                                const RArray<TFSMailSortCriteria>& aSorting);
    /**
     * removes given message from folder
     *
     * @param aMessageId id of message to be removed
     */
     IMPORT_C void RemoveMessageL( const TFSMailMsgId aMessageId );

     // <qmail>
     /**
      * removes given message from folder
      *
      * @param aMessageId id of message to be removed
      * @param aObserver Observer for the operation completion
      * 
      * @return id of the request, KErrFSMailPluginNotSupported if the protocol plugin this
      * instance is attached to does not support the async method.
      */
     IMPORT_C TInt RemoveMessageL( const TFSMailMsgId aMessageId,
                                   MFSMailRequestObserver& aObserver );
     // </qmail>
     
    /**
     * lists subfolders contained by this folder
     *
     * @param aSubFolderList list of subfolders
     */
     IMPORT_C void GetSubFoldersL(RPointerArray<CFSMailFolder>& aSubFolders);

    /**
     * method launches fetching of messages given by user from server
     *
     * @param aMessageIds ids of messages to be fetched
     * @param aDetails defines which details each message contains
     * @param aObserver request observer for conveying fetching progress
     *  events to user
     */
     IMPORT_C TInt FetchMessagesL(  const RArray<TFSMailMsgId>& aMessageIds,
                                    TFSMailDetails aDetails,
                                    MFSMailRequestObserver& aObserver );
    /**
     * method to check if this folder allows moving from given folder type
     *
     * @param aFolderType folder type
     */
     IMPORT_C TBool SupportsMoveFromL( TFSFolderType aFolderType );
     
public: // from  CExtendableEmail

     /** 
      * @see CExtendableEmail::ReleaseExtension
      */                                        
     IMPORT_C void ReleaseExtension( CEmailExtension* aExtension );
     
     /** 
      * @see CExtendableEmail::ExtensionL
      */
     IMPORT_C CEmailExtension* ExtensionL( const TUid& aInterfaceUid );
     
 protected:

    /**
     * C++ default constructor.
     */
     CFSMailFolder();

private:

    /**
     * Two-phased constructor
     */
    void ConstructL( TFSMailMsgId aFolderId );

private: // data

    /**
     * request handler for plugin requests
     */
     CFSMailRequestHandler*     iRequestHandler;    

};

#endif
