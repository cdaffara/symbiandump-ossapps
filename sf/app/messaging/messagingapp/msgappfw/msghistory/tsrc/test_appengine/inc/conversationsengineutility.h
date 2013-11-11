/*
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
 * Description: native mms message's utility class. Provides utility
 * functions for extracting data from SMIL and/or Attachment manager.
 */

#ifndef CONVERSATIONS_ENGINE_UTILITY_H
#define CONVERSATIONS_ENGINE_UTILITY_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <MSVSTD.H>    //for TMsvEntry
#include <MSVAPI.h>    //for CMsvSession
#include <UriUtils.h>  //Uri decoding and encoding
#include "convergedmessageattachment.h"

// FORWARD DECLARATION
class TMsvEntry;
class CMsvAttachment;
class MMsvAttachmentManager;
class QString;
class SmilXmlReader;

// CONSTANTS
/**
 * attribute identifying if an MMS object is attachment or not
 */
const TUid KUidAttachmentIndication = {0x10282CB0};

/**
 * Utility class for mms message parsing.
 */
class CNativeMmsUtility : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @param entry Msg entry in the msg-server
     * @param session Msg-server session
     * @return Instance of the utility class
     */
    static CNativeMmsUtility* NewL( const TMsvId& msgId,
                                    CMsvSession& session );

    /**
     * Two-phased constructor.
     * Leaves the object on the cleanup stack.
     * @param entry Msg entry in the msg-server
     * @param session Msg-server session
     * @return Instance of the utility class
     */
    static CNativeMmsUtility* NewLC( const TMsvId& msgId,
                                     CMsvSession& session );

    /**
     * Destructor.
     */
    virtual ~CNativeMmsUtility();

    /**
     * Fetch sequenced list of attachments
     * @param attachmentlist (to be populated)
     */
     void getAttachmentListL( ConvergedMessageAttachmentList& attachmentlist );

     /**
      * Fetch body text from the MMS
      * @param returnbuf to be populated by body text
      */
     void getBodyTextL(QString& returnbuf);

private:
    /**
     * C++ default constructor.
     */
    CNativeMmsUtility();

    /**
     * Constructor, second phase
     * @param entry Msg entry in the msg-server
     * @param session Msg-server session
     */
    void ConstructL( const TMsvId& msgId,
                     CMsvSession& session );

    /**
     * Fetch smil-file from the attachment manager
     * @return smil's filepath
     */
    const QString getSmilFileL();

     /**
      * Fetch body text of an MMS from its SMIL file
      * @param returnbuf to be populated with body text
      */
     void getBodyTextFromSmilL( QString& returnbuf );

    /**
     * Fetch the body text of an MMS from its attachment manager
     * @param returnbuf to be populated with body text
     */
    void getBodyTextFromAttManL( QString& returnbuf );

    /**
     * Fetch sequenced list of attachments from SMIL file
     * @param attachmentlist (to be populated)
     */
    void getAttachmentListFromSmilL( ConvergedMessageAttachmentList& attachmentlist );

    /**
     * Fetch those attachments which aren't listed in SMIL file from attachment manager
     * @param attachmentlist list of attachments
     */
    void getRemainingAttachmentsFromAttManL( ConvergedMessageAttachmentList& attachmentlist );

    /**
     * Fetch sequenced list of attachments from attachment manager
     * @param attachmentlist (to be populated)
     */
    void getAttachmentListFromAttManL( ConvergedMessageAttachmentList& attachmentlist );

    /**
     * Resolve content-location and/or content-id based url
     * to get fullpath of the attachment
     * @param aUrl CID based url of an object
     * @return full path of the attachment with given cid
     */
    const QString getByUrlL( const TDesC& url );

    /**
     * Find containing directory path
     * @param aFilePath full path of the file
     * @return containing directory
     */
    QString fileDir( const QString& filepath );

    /**
     * Determines if a given attachment is an MMS attachment
     * or an MMS inline object type
     * @param attachment
     * @return ETrue if MMS attachment
     *         EFalse if MMS object
     */
    TBool isAttachment( CMsvAttachment* attachment );

    /**
     * Reads the given text file and returns content in a buffer
     * @param filepath path of the text file
     * @param returnbuf (to be populated with) file content
     */
    void readFileIntoBuffer( const QString& filepath, QString& returnbuf );

    /**
     * Resolve the uri against content location
     * @param contentlocptr pointer to the Content location
     * @param targeturi URI to be resolved
     */
    TBool resolvedByContentLocL( const TPtrC& contentlocptr, const CUri16& targeturi );

    /**
     * Resolve the uri against content id
     * @param contentidptr pointer to the Content location
     * @param targeturi URI to be resolved
     */
    TBool resolvedByContentIdL( const TPtrC8& contentidptr, const CUri16& targeturi );

private: //Data
    /**
     * ismilfilepath SMIL file path
     */
    QString ismilfilepath;

    /**
     * ireader XML reader's instance
     */
    SmilXmlReader* ireader;

    /**
     * iattachmanager Attachment manager
     */
    MMsvAttachmentManager* iattachmanager;

    /**
     * imsventry Message entry in the server
     */
    CMsvEntry *imsventry;
    };


/**
 * Utility class for conversationengine.
 */
// CONSTANTS
_LIT(KUnixEpoch, "19700000:000000.000000");

#include <ccsdefs.h>

class ConversationsEngineUtility
    {
    
public:

    /**
     * Returns ConvergedMessage type.
     * @param value, TCsType
     */
    static int messageType(TCsType value);
     
    };

#endif //CONVERSATIONS_ENGINE_UTILITY_H
