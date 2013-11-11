/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Message Preview Handler
 *
 */

#ifndef _C_CS_PREVIEW_PLUGIN_HANDLER_H_
#define _C_CS_PREVIEW_PLUGIN_HANDLER_H_

//SYSTEM INCLUDES
#include <msvapi.h>
#include <f32file.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnaildata.h>
#include <sqldb.h>

//USER INCLUDES
#include <ccsdefs.h>
#include "ccspreviewplugin.h"
#include "UniDataModel.h"

// FORWARD DECLARATION
class CCsPlugin;
class CMsvSession;
class MCsPluginEventObserver;
class MMsvSessionObserver;
class CCsConversationEntry;
class CCsPreviewPlugin;
class CClientMtmRegistry;
class CMmsClientMtm;
class CBIOClientMtm;
class CUniDataModel;
class MMsvAttachmentManager;

enum MmsConformanceCheckErrors
{
    EInsertSuccess = 0, EInsertQueryAbort, EInsertNotSupported
};

/*
 * Thumbnail Request Data
 */
struct ThumbnailRequestData
    {
    /*
     * Request Id
     */
    TThumbnailRequestId iRequestId;

    /*
     * Message Id
     */
    TMsvId iMsgId;
    };

/**
 *  This class interacts with MsvServer to fetch message data
 */
class CCsPreviewPluginHandler : public CBase,
        public MMsvSessionObserver,
        public MUniDataModelObserver,
        public MThumbnailManagerObserver
    {

public:

    /**
     * Two phase construction
     */
    static CCsPreviewPluginHandler* NewL(CCsPreviewPlugin *aMsgObserver);

    /**
     * Destructor
     */
    virtual ~CCsPreviewPluginHandler();

public:
    //MMsvSessionObserver
    /**
     * Handles the MsvServer updates. Implemented for MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
            TAny* aArg2, TAny* aArg3);

public:
    //MUniDataModelObserver
    /**
     */
    void RestoreReady(TInt aParseResult, TInt aError);

public:
    // Callbacks from MThumbnailManagerObserver for getting thumbnails
    void ThumbnailPreviewReady(MThumbnailData& aThumbnail,
            TThumbnailRequestId aId);

    void ThumbnailReady(TInt aError, MThumbnailData& aThumbnail,
            TThumbnailRequestId aId);

public:

    /*
     * Compare by request Id.
     */
    static TBool CompareByRequestId(const ThumbnailRequestData& aFirst,
            const ThumbnailRequestData& aSecond);

private:

    /**
     * Constructor   
     */
    CCsPreviewPluginHandler();

    /**
     * 2nd phase construtor
     */
    void ConstructL(CCsPreviewPlugin *aMsgPlugin);

    /**
     * Process each entry
     *@param aSelection, CMsvEntrySelections
     */
    void HandleEventL(CMsvEntrySelection* aSelection);
    
    /**
     * Process MMS entry
     *@param aEntry, TMsvEntry
     */
    void HandleMMSEntryL(const TMsvEntry& aEntry);

    /**
     * Process VCard enry
     *@param aEntry, TMsvEntry
     */
    void HandleVCardEntryL(const TMsvEntry& aEntry);

    /**
     * Bind bodytext to sql statement.
     *@param sqlStmt, RSqlStatement
     *@param attachmentId, TMsvAttachmentId
     */
    void BindBodyText(RSqlStatement& sqlStmt, TMsvAttachmentId attachmentId);

    /**
     * Get Thumbnail data from attachment
     *@param attachmentId, TMsvAttachmentId
     *@param mimeType, 
     *@param msgId, message id
     */
    void GetThumbNailL(TMsvAttachmentId attachmentId, TDesC8& mimeType,
            TMsvId msgId);

    /*
     * Process Thumbnail data
     * @param aThumbnail MThumbnailData
     * @param aId TThumbnailRequestId
     */
    void HandleThumbnailReadyL(MThumbnailData& aThumbnail,
            TThumbnailRequestId aId);

    /**
     * Validates if message can be forwarded
     * The validation checks include slide count check,MMS size check and
     * media conformance checks and this fucntion should be called only for mms
     * @param aUniDataModel, instance of unidatamodel
     * @return true if message can be forwarded
     *         false if message cant be forwarded
     */
    TBool ValidateMsgForForward(CUniDataModel* aUniDataModel);

    /**
     * This shall return the particular slide attachments ids
     * @param aSlideNum slide count
     * @param aUniDataModel instance of unidatamodel
     * @return array of attachment id's
     */
    RArray<TMsvAttachmentId>* GetSlideAttachmentIds(
            TInt aSlideNum,
            CUniDataModel* aUniDataModel);

    /**
     * This shall return list of attachment id's
     * @param aUniDataModel instance of unidatamodel
     * @return array of attachment id's
     */
    RArray<TMsvAttachmentId>* GetAttachmentIdList(
            CUniDataModel* aUniDataModel);

    /**
     * This shall check the insert mode for the attachment file
     * @param aFileHandle instance of file handle
     * @return enum MmsConformanceCheckErrors
     */
    TInt CheckModeForInsertL(RFile aFileHandle);
    
    /**
     * Finds the message's processing state
     * @param aMsgId, message id
     * @return, TInt from TCsPreviewMsgProcessingState enum
     */
    TInt msgProcessingState(TMsvId aMsgId);
    
    /**
     * Sets message under processing to true
     * @param aMsgId, message id
     * @param aState, TInt from TCsPreviewMsgProcessingState enum
     */
    void setMsgProcessingState(TMsvId aMsgId, TInt aState);

private:
    //Data

    /**
     * MsvServer session object
     * Own
     */
    CMsvSession* iSession;

    /**
     * iMsgObserver
     * The Observer interface
     * Not Own.
     */
    CCsPreviewPlugin *iMsgObserver;

    /**
     * Mtm registry object
     * Own
     */
    CClientMtmRegistry* iMtmRegistry;

    /**
     * Mms client mtm object
     * Own.
     */
    CMmsClientMtm* iMmsMtm;
    
    /**
     * BioMsg client mtm object
     * Own.
     */
    CBIOClientMtm* iBioClientMtm;

    /*
     * File session
     */
    RFs ifsSession;

    /*
     * Thumbnail request array.
     * Own
     */
    RArray<ThumbnailRequestData> iThumbnailRequestArray;

    /*
     * Thumbnail manager.
     * Own
     */
    CThumbnailManager* iThumbnailManager;

    /*
     * Sqlite DB Handle
     */
    RSqlDatabase iSqlDb;

    /**
     * Max MMS Size
    */
    TInt iMaxMmsSize;

    /*
     * Mms creation mode
     */
    TInt iCreationMode;

    /**
     * Conformance status
     */
    TUint32 iConfStatus;

};

#endif // _C_CS_PREVIEW_PLUGIN_HANDLER_H_
