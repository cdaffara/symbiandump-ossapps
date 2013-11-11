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
*       CUniObject, Storage for single attachment in presentation.
*
*
*/


#ifndef __UNIOBJECT_H
#define __UNIOBJECT_H

// INCLUDES

#include <e32std.h>
#include <msvstd.h>     // for TMsvId
#include <cmsvattachment.h>

#include <DRMEvent.h>
#include <DRMEventObserver.h>

#include <MsgMedia.hrh>

#include <UniModelConst.h>

// CONSTANTS
const TUid KUidAttachmentIndication = {0x10282CB0};

// MACROS

// FORWARD DECLARATIONS
class CUniMimeInfo;
class CMsgMediaInfo;
class CUniDrmInfo;
class CBaseMtm;
class CUniDataUtils;
class CMsvStore;
class MMsvAttachmentManager;
class CUniObject;
class CDRMNotifier;
// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* MUniObjectSaveObserver - Callback API for object saving.
*
* @since 3.1
*/
class MUniObjectSaveObserver
    {
    public:
        virtual void ObjectSaveReady( TInt aError ) = 0;
    };

enum TUniObjectEvent
    {
	EUniObjectEventDrmRightsChanged = 0
    };

class MUniObjectObserver
    {
    public:
        virtual void HandleUniObjectEvent(  CUniObject&  aUniObject,
                                            TUniObjectEvent aUniEvent ) = 0;
    };

/**
* CUniObject - Representation of a single attachment of a message.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniObject : public CActive,
                   public MDRMEventObserver

    {

    public:  // New methods

        /**
        * Factory method that creates this object
        * from an attachment.
        *
        * @since    3.1
        * @param    aMtm    IN reference to client mtm
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @param    aManager        IN reference to attachment manager
        * @param    aAttachmentId   IN id of the attachment from
        *                           which to create the object
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniObject* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgMediaInfo* aMedia,
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment );

        /**
        * Factory method that creates this object
        * from an attachment.
        *
        * @since    3.1
        * @param    aMtm    IN reference to client mtm
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @param    aAttachmentId   IN id of the attachment from
        *                           which to create the object
        * @return   Pointer to instance
        */
        IMPORT_C static CUniObject* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgMediaInfo* aMedia,
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment );

        /**
        * Factory method that creates this object
        * from a file.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniObject* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgMediaInfo* aMedia );

        /**
        * Factory method that creates this object
        * from a file.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @return   Pointer to instance
        */
        IMPORT_C static CUniObject* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgMediaInfo* aMedia );

        /**
        * Destructor
        *
        * @since    3.1
        */
        virtual ~CUniObject();

        /**
        * Save
        * Save the object to message store.
        * If leave occurs, attachment is cleaned up from the
        * message store.
        *
        * @since    3.1
        * @param    aObserver   IN operation observer
        * @param    aSaveType   IN save type
        */
        virtual void Save(
            MUniObjectSaveObserver& aObserver,
            CMsvAttachment::TMsvAttachmentType aSaveType );

        /**
        * Remove from store
        * Removes attachment from message store.
        *
        * @since    3.1
        * @param    aMtm    IN reference to MMS client mtm
        */
        virtual void RemoveFromStoreL();

        /**
        * AttachmentId
        * Accessor.
        *
        * @since    3.1
        * @return   Attachment id of the object
        */
        inline TMsvAttachmentId AttachmentId() const;

        /**
        * MimeInfo
        * Accessor.
        *
        * @since    3.1
        * @return   Pointer to the object's mime header class
        */
        inline CUniMimeInfo* MimeInfo() const;

        /**
        * DrmInfo
        * Accessor.
        *
        * @since    3.1
        * @return   Pointer to the object's mime header class
        */
        inline CUniDrmInfo* DrmInfo() const;

        /**
        * MimeType
        * Returns the mime type of the object. Use this instead
        * of MimeInfo's or MediaInfo's mime type unless you are
        * absolutely sure what you are doing.
        *
        * @since    3.1
        * @return   Mime type of the object.
        */
        IMPORT_C TPtrC8 MimeType() const;

        /**
        * MediaInfo
        * Accessor.
        *
        * @since    3.1
        * @return   Pointer to the object's media info class
        */
        inline CMsgMediaInfo* MediaInfo() const;

        /**
        * MediaType
        * Returns the media type of the object. Use this instead
        * of MediaInfo's media type unless you are absolutely
        * sure what you are doing.
        *
        * @since    3.1
        * @return   Media type of the object.
        */
        IMPORT_C TMsgMediaType MediaType() const;

        /*
        * Size
        *
        * @since    3.1
        * @param    aWithoutHeaders IN if ETrue include MIME headers' sizes
        *                           if EFalse count only file sizes
        * @return   Size of the object in bytes
        */
        virtual TInt Size( TBool aWithoutHeaders = ETrue );

        /**
        * Corrupted
        * Accessor.
        *
        * @since    3.1
        * @return   ETrue, if object corrupted,
        *           EFalse otherwise
        */
        inline TBool Corrupted() const;

        /**
        * SetCorrupted
        * Mutator.
        *
        * @since    3.1
        * @param    aCorrupted  IN ETrue to mark object corrupted
        */
        inline void SetCorrupted( TBool aCorrupted );

        /**
        * StoreState
        * Accessor.
        *
        * @since    3.1
        * @return   Store state of the object:
        *           EStoreStateAttachment if object is stored into message store and "visible"
        *           EStoreStateTemporary if is not yet in message store
        *           EStoreStateHidden if object is stored into message store and "hidden"
        */
        //inline TMsgObjectStoreState StoreState() const;

        /**
        * SetReferenceCount
        * Mutator.
        *
        * @since    3.1
        * @param    aCount  IN number of times the object is referenced in
        *                   the message (normally in the SMIL).
        */
        inline void SetReferenceCount( TInt aCount );

        /**
        * ReferenceCount
        * Accessor.
        *
        * @since    3.1
        * @return   Number of times the object is referenced in
        *           the message (normally in the SMIL).
        */
        inline TInt ReferenceCount() const;

        /**
        * SetRegion
        * Mutator.
        *
        * @since    3.1
        * @param    region object is in SMIL
        */
        inline void SetRegion( TUniRegion aRegion );

        /**
        * Region
        * Accessor.
        *
        * @since    3.1
        * @return   region object is in SMIL
        */
        inline TUniRegion Region() const;

        /**
        * SetMediaTypeBySmilTag
        * Mutator.
        *
        * @since    3.1
        * @param    media type by SMIL media tag
        */
        inline void SetMediaTypeBySmilTag( TMsgMediaType aMediaType );

        /**
        * ReferenceCount
        * Accessor.
        *
        * @since    3.1
        * @return   media type by SMIL media tag
        */
        inline TMsgMediaType MediaTypeBySmilTag() const;

        /**
        * SetUniqueControlHandle
        * Mutator.
        *
        * @since    3.2
        * @param    UniqueControlHandle, -1 for "resetting".
        */
        inline void SetUniqueControlHandle( TInt aUniqueControlHandle );

        /**
        * UniqueControlHandle
        * Accessor.
        *
        * @since    3.2
        * @return   UniqueControlHandle, -1 if not set.
        */
        inline TInt UniqueControlHandle() const;

        /**
        * SetAttachment
        * Mutator.
        *
        * @since    3.2
        * @param    aObjectAttachment ETrue if object is attachment, otherwise EFalse
        */
        inline void SetAttachment( TBool aObjectAttachment );

        /**
        * Attachment
        * Accessor.
        *
        * @since    3.2
        * @return   ETrue if object is attachment, otherwise EFalse
        */
        inline TBool Attachment() const;

        /**
        * IsSaved
        * Accessor.
        *
        * @since    3.2
        * @return   Information whether this object has been saved into file system
        *           in Viewer.
        */
        inline TBool IsSaved( );

        /**
        * IsSaved
        * Mutator.
        *
        * @since    3.2
        * @param    aSaved ETrue attachment has been saved
        *                  EFalse attachment has not been saved
        */
        inline void SetSaved( TBool aSaved );

        /**
        * IsDrmRightsWhenLoaded. Function is applicable only, if DrmInfo exists.
        * Accessor.
        *
        * @since    5.0
        * @return   Checks whether object had DRM rights when this object was loaded
        *           into screen. If last DRM rights were consumed during loading,
        *           it returns ETrue.
        *
        */
        inline TBool IsDrmRightsWhenLoaded( ) const;

        /**
        * ResolveDrmRightsWhenLoadedL. Saves information whether this object had DRM
        *           rights when slide having the object was loaded. Uses
        *           CDrmInfo::EvaluateRights() to resolve status
        * Mutator.
        *
        * @since    5.0
        *
        */
        IMPORT_C void ResolveDrmRightsWhenLoadedL( );

        /**
        * SetDrmRightsWhenLoadedL. Saves information whether this object had DRM
        *            rights when slide having the object was loaded.
        * Mutator.
        *
        * @since    5.0
        * @param    aRights ETrue: objects had rights, which were already consumed or if not consumed,
        *                   have still rights left
        *                   EFalse: rights have not been consumed and they are not left
        *
        */
        IMPORT_C void SetDrmRightsWhenLoaded( TBool aRights );

        /**
        * IsDrmRightsWhenLoadedObjects. Function is applicable only, if DrmInfo exists.
        * Accessor.
        *
        * @since    5.0
        * @return   Checks whether object had DRM rights when this object was loaded
        *           into screen in the Objects view. If last DRM rights were consumed during loading,
        *           it returns ETrue.
        *
        */
        inline TBool IsDrmRightsWhenLoadedObjects( ) const;

        /**
        * SetDrmRightsWhenLoadedObjects. Saves information whether this object had DRM
        *            rights when slide having the object was loaded into screen in the
        *            Objects view.
        * Mutator.
        *
        * @since    5.0
        * @param    aRights ETrue: objects had rights, which were already consumed or if not consumed,
        *                   have still rights left
        *                   EFalse: rights have not been consumed and they are not left
        *
        */
        IMPORT_C void SetDrmRightsWhenLoadedObjects( TBool aRights );

        /**
        * Sets observer of UniObject events. Several observers are supported at the same time.
        * Mutator.
        *
        * @since    5.0
        * @param    aObserver observer
        *
        */
        IMPORT_C void SetObserverL(  MUniObjectObserver* aObserver );                       

        /**
        * Removes observer of UniObject events.
        * Accessor.
        *
        * @since    5.0
        *
        */
        IMPORT_C void RemoveObserver(   MUniObjectObserver*     aObserver );

    protected:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        TInt RunError( TInt aError );

    protected:

        /**
        * Constructor.
        *
        * @since    3.1
        */
        CUniObject(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgMediaInfo* aMedia );

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        */
        void ConstructL();

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        * @param    aManager        IN reference to attachment manager
        * @param    aAttachmentId   IN id of the attachment from
        *                           which to create the object
        */
        void ConstructFromAttachmentL(
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment );

        /**
        * CreateMimeInfoL
        * Create mime info for the object from the file information
        * and in-parameters.
        *
        * @since    3.1
        */
        void CreateMimeInfoL();

        /**
        * CreateAttachmentL
        * Creates an attachment to message store.
        * If leave occurs, attachment is cleaned up from the
        * message store.
        *
        * @since    3.1
        */
        void CreateAttachmentL( );

        /**
        * SetHiddenL
        * Sets attachment "hidden" or "visible"
        * "Hidden" means InPreparation flag is set on and
        * "visible" means InPreparation flag is set off
        * Not to be called for "temporary" object
        *
        * @since    3.1
        * @param    aHidden IN ETrue for "hidden", EFalse for "visible"
        */
        //void SetHiddenL( TBool aHidden );

        virtual void DoSaveL();

        void StartObjectSaveL();
        void FinalizeObjectSaveL();

        void CompleteSelf();

        /// From MDRMEventObserver
        virtual void HandleEventL( MDRMEvent* aEvent );


    protected: // data

        enum TMsgObjectSaveState
            {
            EObjectSavingIdle,
            EObjectCreatingAttachment
            };

        enum TMsgObjectFlags
            {
            EObjectCorrupted = 0x1,
            EObjectAttachment = 0x2,
            EObjectDrmRightsWhenLoaded = 0x4,
            EObjectDrmRightsWhenLoadedObjects = 0x8
            };

        RFs&                    iFs;
        CBaseMtm&               iMtm;
        CUniDataUtils&          iData;

		TInt                    iSupportedFeatures;

        TMsvAttachmentId        iAttachmentId;  // Attachment id of object. Can be id to Link file in store or to real atta.

        CUniMimeInfo*           iMimeInfo;      // Contains objects mime headers.
        CMsgMediaInfo*          iMediaInfo;     // Contains objects media information.
        CUniDrmInfo*            iDrmInfo;
        TInt                    iObjectFlags;
        TInt                    iReferenceCount;
        //TMsgObjectStoreState    iStoreState;
        TUniRegion              iRegion;
        TMsgMediaType           iMediaTypeBySmilTag;
        TInt                    iUniqueControlHandle;

        MUniObjectSaveObserver* iObserver;
        CMsvAttachment::TMsvAttachmentType iSaveType;

        MMsvAttachmentManager*  iManager;
        CMsvAttachment*         iAttachment;
        CMsvStore*              iEditStore;
        TInt                    iSaveState;

        TBool                   iSavedToFs;
        RFile                   iFileHandle;

        CDRMNotifier*           iDrmNotifier;
        CArrayPtrFlat<MUniObjectObserver>* iObservers;

    };

#include <UniObject.inl>

#endif // __UNIOBJECT_H
