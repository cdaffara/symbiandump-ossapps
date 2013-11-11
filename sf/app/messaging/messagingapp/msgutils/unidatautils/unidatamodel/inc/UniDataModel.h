/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniDataModel
*
*
*/


#ifndef __UNIDATAMODEL_H
#define __UNIDATAMODEL_H

//  INCLUDES
#include <gmxmlparser.h>
#include <gmxmldocument.h>
#include <msvapi.h>                     // CMsvEntry
#include <UniDataUtils.h>
#include <UniSmilModel.h>
#include <UniSmilList.h>
#include <UniObject.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <MsgMediaInfo.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CBaseMtm;
class CUniDataUtils;
class CUniObjectsModel;
class CUniObjectList;
class CUniObjectsInfo;
class CMsgMediaResolver;
class CMmsConformance;
class CMsvMimeHeaders;
class CMsvAttachment;

// CLASS DECLARATION

/**
* MUniDataModelObserver - Callback API for model operations.
*
* @since 3.1
*/
class MUniDataModelObserver
    {
    public:
        virtual void RestoreReady( TInt aParseResult, TInt aError ) = 0;
    };

/**
* CUniDataModel - A container class that owns instances of CMsgMediaResolver,
* CMmsConformance, CUniDataUtils, CUniObjectList, CUniSmilList and CUniSmilModel.
* It also handles the initialization of the lists & the SMIL model.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniDataModel : public CBase,
                      public MMDXMLParserObserver,
                      public MMediaInfoObserver
    {
    public:  // Constructors and destructor

        static IMPORT_C CUniDataModel* NewL( RFs& aFs, CBaseMtm& aMtm );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CUniDataModel();

    public: // New functions

        /**
        * RestoreL
        * Loads attachments from message store and initializes
        * the model.
        * @param aObserver          IN Callback observer
        * @param aRemoveCorrupted   IN If ETrue removes corrupted attachments
        *                              If EFalse leaves if corrupted attachments detected
        * @param aAllowSmil         IN If ETrue puts SMIL files to "SmilList"
        *                              If EFalse puts SMIL files to "AttachmentList"
        */ 
        IMPORT_C void RestoreL(
            MUniDataModelObserver& aObserver,
            TBool aRemoveCorrupted = ETrue,
            TBool aAllowSmil = ETrue );

        /**
        * AddObjectL
        * Adds an object into msg store and smil slide
        *
        * @since    3.1
        * @param    aObserver   IN Object save observer
        * @param    aMedia      IN media info - ownership transferred to MsgObject
        * @param    aSaveType   IN either "file" or "linked file"
        * @return   Pointer to the added object
        *           Ownership NOT transferred.
        */
        IMPORT_C CUniObject* AddAttachmentL(
            MUniObjectSaveObserver& aObserver,
            CMsgMediaInfo* aMedia,
            CMsvAttachment::TMsvAttachmentType aSaveType = CMsvAttachment::EMsvLinkedFile );

       /**
        * AddStoredAttachmentL
        * Adds an object that is already in msg store to smil slide
        *
        * @since    3.1
        * @param    aAttachmentId   IN file to be added
        * @param    aMedia      IN media info - ownership transferred to MsgObject
        * @return   Pointer to the added object
        *           Ownership NOT transferred.
        */
        IMPORT_C CUniObject* AddStoredAttachmentL(
            TMsvAttachmentId aAttachmentId,
            CMsgMediaInfo* aMedia );

       /**
        * FinalizeMediaParse
        * This function synchronizes asynchronous media info parsing if it is still ongoing
        * To be called from editor/viewer when the message is to be send/saved/forwarded
        * @since    3.2
        */
        IMPORT_C void FinalizeMediaParse();

    public:

       /**
        * MediaInfoParsed
        * Callback for MediaInfo active object
        * @since    3.2
        */
        void MediaInfoParsed();

       /**
        * ParseNextObject
        * Parses the next object in a to-be-parsed list
        * Actual parsing is dedicated to MediaInfo's asynchronous method
        * @since    3.2
        */
        void ParseNextObject();

        /**
        * ParseFileCompleteL
        * Call back function used to inform client of Parser when RunL function completes
        */
        void ParseFileCompleteL();

    public: // inlines

        /**
        * Returns number of corrupted attachments encountered at load time
        * i.e. entry is stil there but binary file is missing
        * Includes corrupted link, presentations etc.
        *
        * @return attachment count
        */
        inline TInt CorruptedCount() const;

        /**
        * Returns attachment count before any filtering for "presentation objects"
        * or corrupted attachments. SMIL attachments are not included in the count.
        *
        * @return attachment count
        */
        inline TInt OriginalAttachmentCount() const;

        /**
        * Get reference to file session
        *
        * @return reference to RFs
        */
        inline RFs& FsSession() const;

        /**
        * Get reference to base mtm
        *
        * @return reference to CBaseMtm
        */
        inline CBaseMtm& Mtm() const;

        /**
        * Returns object list owned by the model. Object list includes all
        * attachments that are referenced from the SMIL part (or if there is no
        * SMIL part attachments that are of supported media type).
        *
        * @return reference to the object list.
        */
        inline CUniObjectList& ObjectList() const;

        /**
        * Returns attachment list owned by the model. Attachment list includes all
        * attachments that are NOT referenced from the SMIL part.
        *
        * @return reference to the attachment list.
        */
        inline CUniObjectList& AttachmentList() const;

        /**
        * Returns SMIL list owned by the model. SMIL list includes all
        * attachments that are of type "application/smil".
        *
        * @return reference to the SMIL attachment list.
        */
        inline CUniSmilList& SmilList() const;

        /**
        * Returns data utils object used by the model.
        *
        * @return reference to the data utils object.
        */
        inline CUniDataUtils& DataUtils() const;

        /**
        * Returns SMIL type of the message.
        * @return Possible return values are:
        *         -ENoSmil
        *         -EMmsSmil
        *         -EOtherSmil
        *         -EMultipleSmil
        *         -ETemplateSmil
        */
        inline TUniSmilType SmilType();

        /**
        * Returns SMIL type of the message.
        * @param aSmilType:
        *         -ENoSmil
        *         -EMmsSmil
        *         -EOtherSmil
        *         -EMultipleSmil
        *         -ETemplateSmil
        */
        inline void SetSmilType( TUniSmilType aSmilType );

        /**
        * Get reference to SmilModel
        *
        * @return reference to SmilModel
        */
        inline CUniSmilModel& SmilModel() const;

        /**
        * Get reference to MediaResolver
        *
        * @return reference to MediaResolver
        */
        inline CMsgMediaResolver& MediaResolver() const;

        /**
        * Get reference to MmsConformance
        *
        * @return reference to MmsConformance
        */
        inline CMmsConformance& MmsConformance() const;

        /**
        * Return SMIL DOM
        *
        * @return Pointer to SMIL DOM
        */
        inline CMDXMLDocument* Dom();

        /**
        * Set SMIL DOM
        *
        * @param Pointer to SMIL DOM, takes ownership
        */
        inline void SetDom( CMDXMLDocument* aDom );

        /*
        * Accessor
        *
        * @return parse result from CUniSmilModel::ParseL()
        */
        inline TInt ParseResult() const;

    private:  // New functions

        /**
        * CUniDataModel
        * Constructor.
        */
        CUniDataModel( RFs& aFs, CBaseMtm& aMtm );

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New Functions


        /**
        * DoParseFileCompleteL
        */
        void DoParseFileCompleteL();

        /**
        * FinalizeRestoreL
        */
        void FinalizeRestoreL( TInt aError );

        /**
        * Handles corrupted attachments.
        *
        * @param aRemoveCorrupted   IN
        *   - If ETrue tries to recover by removing corrupted attachments.
        *
        *     Note: This means that SMIL parsing will fail with ESmil_Missing_Objects.
        *           and object order is not maintained. But better to do it this
        *           way so that message can be opened.
        *
        *   - If EFalse leaves with KErrCorrupt if corrupted attachments detected.
        *     Model not made caller should exit!
        */
        void FindAndHandleCorruptL( TBool aRemoveCorrupted );

        /**
        * Finds corrupted attachments from context message.
        */
        CMsvEntrySelection* FindCorruptLC();

        /**
        * FindAttachmentsL
        * Browses through SMIL and separates objects that are
        * referenced from attachments.
        */
        void FindAttachmentsL();

        /**
        * Creates an object from msg store attachment
        * @param aMimeType - mime type of new object.
        * @param aAttachmentId - ID to be used for searching
        *                        attachment from store
        */
        CUniObject* CreateObjectFromAttachmentLC(
            const TPtrC8& aMimeType,
            RFile& aFile,
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment );

        /**
        * Browses through (non-MMS) SMIL and searches reference counts for objects and separates
        * objects and attachments in different lists.
        */
        void FindDomObjectRefCountsL();

    protected:  // data

        RFs&                iFs;
        CBaseMtm&           iMtm;

        CMsgMediaResolver*  iMediaResolver;

        CUniDataUtils*      iDataUtils;
        CUniObjectList*     iObjectList;
        CUniObjectList*     iAttachmentList;
        CUniSmilList*       iSmilList;

        CUniSmilModel*      iSmilModel;
        TUniSmilType        iSmilType;
        TInt                iOrigAttachmentCount;
        TInt                iCorruptedCount; // Number of attas detected to be corrupted when loading

        CMDXMLParser*       iParser;
        CMDXMLDocument*     iDom;

        MUniDataModelObserver* iObserver;
        TInt                iParseResult;
        CActiveSchedulerWait     iObjWait;
        CUniObject*         iObjectToParse;
        CMmsConformance* iMmsConformance;
};

#include <UniDataModel.inl>

#endif   // __UNIDATAMODEL_H

// End of file
