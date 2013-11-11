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
 * Description:
 *
 */

#ifndef __UNIEDITORMMSPLUGINPRIVATE_H
#define __UNIEDITORMMSPLUGINPRIVATE_H

#include <msvapi.h>
#include <cmsvrecipientlist.h>
#include "unieditorplugininterface.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "UniDataModel.h"
#include "UniEditorGenUtils.h"

// DATA TYPES

// FORWARD DECLARATIONS

class CClientMtmRegistry;
class CMmsClientMtm;
class CMmsHeaders;
class CMmsSettings;
class CCommsDatabase;
class TUniSendingSettings;
class UniDataModelPluginInterface;
class UniDataModelLoader;
class CUniDataModel;
class CMsgMediaInfo;
class CMDXMLDocument;
class MMediaInfoObserver;

// CLASS DECLARATION

class CUniEditorMmsPluginPrivate : public CBase,
        public MUniDataModelObserver,
        public MUniObjectSaveObserver,
        public MUniSmilComposeObserver,
        public MMediaInfoObserver,
		public MMsvSessionObserver
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUniEditorMmsPluginPrivate* NewL();

    /**
     * Destructor.
     */
    virtual ~CUniEditorMmsPluginPrivate();

public: 

    /**
     * Converts message from message store into ConvergedMessage 
     * based on the operation
     * @param TMsvId id
     * @param aOperation operation type
     * @return ConvergedMessage object is returned in successfull cases and 
     *         NULL is returned in failure cases. The ownership of the object
     *         is transferred to the caller. 
     */
    ConvergedMessage* convertFromL( TMsvId aId,
        UniEditorPluginInterface::EditorOperation aOperation );

    /**
     * delete entry from drafts folder
     * @param aId TMsvId of the message entry to be deleted
     */
    void deleteDraftsEntryL( TMsvId aId );

    /**
     * Converts ConvergedMessage message into a message in store
     * @param ConvergedMessage obj
     * @return TMsvId id
     */             
    TMsvId convertToL( ConvergedMessage *aMessage );
    
    /**
     * Implementation for convertToL which converts ConvergedMessage message 
     * into a message in store
     * @param ConvergedMessage obj
     * @param TMsvEntry
     */             
    void DoConvertToL(ConvergedMessage *aMessage,TMsvEntry& entry);

    /**
     * Sends the message
     * @param TMsvId id
     */                 
    void sendL( TMsvId aId );

    /**
     * Validates the service for send
     * @param aEmailOverSms
     * @return true if the service is valid for send
     */                    
    TBool validateServiceL( TBool aEmailOverSms = EFalse );

    /**
     * isServiceValidL
     * @return TBool
     */              
    TBool isServiceValidL();

private: // Constructors

    /**
     * C++ default constructor.
     */
    CUniEditorMmsPluginPrivate();

    /**
     * Returns pointer to iMmsMtm member variable  
     */
    CMmsClientMtm* MmsMtmL();

    /**
     * Generates details for TMsvEntry object
     */
    void MakeDetailsL( TDes& aDetails );

    /**
     * Populates address feilds in message from ConvergedMessage
     */
    void addRecipientsL(ConvergedMessage *message);

    /**
     * Populates address feilds in message from ConvergedMessage
     */
    void addRecipientsL(const ConvergedMessageAddressList &array, 
        TMsvRecipientType recpType);

    /**
     * Populates converged message from the drafts entry
     */
    void convertFromDraftsL(ConvergedMessage& aMessage );

    /**
     * Populates address feilds into converged message
     */
    void populateRecipientsL(
        ConvergedMessage &aMessage);

    /**
     * Populates body and attachment list into converged message
     */
    void populateMessageBodyL(ConvergedMessage &aMessage);
    
    /**
     * Populates converged message for forward case
     */
    void convertFromForwardHandlerL(ConvergedMessage& aMessage);

    /**
     * Add object into smil 
     * @param slide number
     * @param filepath
     */
    void addObjectL(int aSlideNum, const QString& aFilePath);

    /**
     * Add attachment 
     * @param file path
     */
    void addAttachmentL(const QString& aFilePath);

    /**
     * Add text object into smil 
     * @param slide number
     * @param text
     */
    void addTextObjectL(int aSlideNum, const QString& aBodyText);

    /**
     * @see MUniObjectSaveObserver
     */
    void ObjectSaveReady(TInt aError);

    /**
     * @see MUniSmilComposeObserver
     */
    void SmilComposeEvent(TInt aError);

    /**
     * @see MMediaInfoObserver
     */
    void MediaInfoParsed();

    /**
     * @see MUniDataModelObserver
     */
    void RestoreReady(TInt aParseResult, TInt aError);
    
    /**
     * @see MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
    
    /**
     * Populate sender address (for reply/replyAll case)
     */
    void populateSenderL(ConvergedMessage& aMessage);
    
    /**
     * Populates converged message for reply case
     */
    void convertFromReplyHandlerL(ConvergedMessage* aMessage);
    
    /**
     * Populates converged message for replyAll case
     */
    void convertFromReplyAllHandlerL(ConvergedMessage* aMessage);
    
    /**
     * Populates converged message for default case
     */
    void convertFromDefaultHandlerL(ConvergedMessage* aMessage);
    
    /**
     * Handles own-number identification/deletion for composing reply-all
     * to a message
     * @param sourcemsg, message on which own-number logic is applied
     * @param targetmsg, message to which trimmed address list is committed
     */
    void removeOwnNumberForReplyAll(
            ConvergedMessage* sourcemsg, 
            ConvergedMessage* targetmsg);
    
    /**
     * Helper method to make a copy of address list
     * @param addrList, list of addresses to make a copy of
     * @return ConvergedMessageAddressList, copy of list of addresses
     */
    ConvergedMessageAddressList copyAddrList(
            ConvergedMessageAddressList addrList);

    /**
     * Helper method to resolve contacts in an addresslist
     * @param addrList, list of addresses whose contacts need to be resolved
     */
    void resolveContacts(ConvergedMessageAddressList addrList);
    
    /**
     * Helper method to check if a given address is self-address or not
     * @param address, address to be verified
     * @param selfAddrList, list of self-addresses
     * @return bool, true if address is a self-address
     */
    bool isSelfAddress(QString address,QStringList selfAddrList);


private:
    // Data
    CMsvSession* iSession;
    CClientMtmRegistry* iMtmRegistry;
    CMmsClientMtm* iMmsMtm;
    CMmsHeaders* iMmsHeader;

    // Flag for forwarded messages
    TBool iMessageForwarded;
    
    // owned
    UniDataModelLoader* iDataModelPluginLoader;        
    //Not owned
    UniDataModelPluginInterface* iMmsDataPlugin;
    // owned
    CUniDataModel* iUniDataModel;
    RFs ifsSession;
    
    // not owned
    CMsgMediaInfo* iInsertingMedia;
    // owned
    CEikRichTextEditor* iEditor;
    // owned
    CMDXMLDocument* iDom;
    // owned
    UniEditorGenUtils* iGenUtils;
};

#endif   // __UNIEDITORMMSPLUGINPRIVATE_H

// End of File
