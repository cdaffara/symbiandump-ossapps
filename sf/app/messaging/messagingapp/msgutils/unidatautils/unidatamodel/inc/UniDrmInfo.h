/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniDrmInfo, Storage for single attachment in presentation.
*
*
*/



#ifndef __UNIDRMINFO_H
#define __UNIDRMINFO_H

// INCLUDES

#include <e32std.h>
#include <cmsvattachment.h>

#include <caf/caf.h>
#include <DRMHelper.h>
#include <DRMRights.h>

#include <UniObject.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CBaseMtm;
class CMsgMediaInfo;
class CDRMHelper;
#ifndef DRMCOMMONIN_POISTO
class DRMCommon;
#endif

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniDrmInfo - DRM information of an attachment.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniDrmInfo : public CBase
    {

    public:  // New methods

        /**
        * Factory method that creates this object
        * from an attachment.
        *
        * @since    3.1
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniDrmInfo* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            TMsvAttachmentId& aId,
            CMsgMediaInfo& aMediaInfo );

        /**
        * Factory method that creates this object
        * from an attachment.
        *
        * @since    3.1
        * @return   Pointer to instance
        */
        IMPORT_C static CUniDrmInfo* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            TMsvAttachmentId& aId,
            CMsgMediaInfo& aMediaInfo );

        /**
        * Destructor
        *
        * @since    3.1
        */
        virtual ~CUniDrmInfo();

        /**
        * Freezes and locks the DRM rights of the object if available.
        * -> No counts will be reduced
        * @return   Error code from CDRMHelper::Consume2(). Some controls
        * may ignore the value but some don't
        * @since    3.1
        */
        IMPORT_C TInt FreezeRights();

        /**
        * Consumes and locks the DRM rights of the object if available.
        * -> One count will be reduced
        * @return   Error code from CDRMHelper::Consume2(). Some controls
        * may ignore the value but some don't
        * @since    3.1
        */
        IMPORT_C TInt ConsumeRights();

        /**
        * Evaluates DRM rights of the object if available.
        * Use count is not changed.
        * @param aConsumed
        *        return information whether DRM rights have already been consumed
        * @since    3.1
        */
        IMPORT_C TInt EvaluateRights( TBool& aConsumed );

        /**
        * Releases the DRM rights of the object.
        * @return   Error code from CDRMHelper::Consume2().Some controls
        * may ignore the value but some don't
        * @since    3.1
        */
        IMPORT_C TInt ReleaseRights();

        /**
        * Convenience function to check whether DRM rights for
        * the object are valid.
        * @since    3.1
        * @param    aUpdate         IN  if ETrue, updates rights info,
        *                               otherwise returns rights as they were
        *                               when the object was constructed.
        */
        IMPORT_C TBool RightsValidL( TBool aUpdate );

        /**
        * Check whether this is combined delivery cobject with one
        * play count left. This information is needed to show note
        * "Message contains an object that can only be viewed once"
        *  §qtn.drm.preview.msg§
        * @return ETrue
        *         EFalse
        */
        IMPORT_C TBool IsDrmWithOnePreview();

        /**
        * Handles DRM error ( taken place in an UI control).
        */
        IMPORT_C void HandleDrmErrorL( TInt aError );
        
    protected: 

        enum TUniDRMConsumeStatus
            {
            EUniDRMNone,
            EUniDRMFrozen,
            EUniDRMConsumed
            };

        /**
        * Constructor.
        *
        * @since    3.1
        */
        CUniDrmInfo(
            RFs& aFs,
            CBaseMtm& aMtm,
            TMsvAttachmentId& aId,
            CMsgMediaInfo& aMediaInfo );

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        */
        void ConstructL();

        /**
        * Gets rights info from a media object.
        * @param aRightsObj optionally returns CDRMRights of the object, if
        *        Ownership is transferred to caller.
        * @since    3.1
        */
        void DoGetRightsInfoL(CDRMRights** aRightsObj = NULL);

        inline ContentAccess::TIntent ConsumeIntent();

        /**
        * Check whether this is combined delivery cobject with one
        * play count left. This information is needed to show note
        * "Message contains an object that can only be viewed once"
        *  §qtn.drm.preview.msg§. Value is saved iDrmOnePreview.
        */
        void SetIsDrmWithOnePreviewL();

        /**
        * Creates internal CData object. It cannot be created during construction.
        * @return If this object is not DRM protected returns NULL.
        */
        CData* CreateCDataL();

    protected: // data

        RFs&                    iFs;
		TInt                    iSupportedFeatures;

        CBaseMtm&               iMtm;
        TMsvAttachmentId&       iAttachmentId;
        CMsgMediaInfo&          iMediaInfo;

#ifdef DRMCOMMONIN_POISTO
        ContentAccess::CRightsManager*  iRightsManager;
#else
        DRMCommon*              iDRMCommon;
#endif

        CDRMHelper*             iDRMHelper;
        TUniDRMConsumeStatus    iDRMConsumeStatus;
        TInt                    iDRMConsumeCount;
        TBool                   iDRMRightsExist;
        CDRMRights::TExpiration iDRMExpiration;
        CDRMRights::TRestriction    iDRMRestriction;
        TUint32                 iDRMConstraintType;

        // DRm2 follows:
        TBool                   iOma2;
        // Indicates preview type
        CDRMHelper::TDRMHelperPreviewType   iPreviewType;
        // content URI (unique id) of the embedded preview content
        TBool                   iHasInfoUri;
        HBufC*                  iSendContentUrl;
        TBool                   iCDataResolved;
        TPtrC8                  iUsedContentUri;
        TBool                   iDrmOnePreview;
    };

#include <UniDrmInfo.inl>

#endif // __UNIDRMINFO_H
