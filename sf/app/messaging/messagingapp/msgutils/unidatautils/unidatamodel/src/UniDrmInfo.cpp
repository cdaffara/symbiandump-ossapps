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


// ========== INCLUDE FILES ================================

#include <e32def.h>     // for basic types
#include <eikenv.h>     // for CBase
#include <coemain.h>    // for CEikonEnv
#include <mtclbase.h>   // for CBaseMtm
#include <msvids.h>     // for KMsvTempIndexEntryId
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>

// Features
#include <featmgr.h>
#include <bldvariant.hrh>

// DRM
#ifndef DRMCOMMONIN_POISTO
#include <DRMCommon.h>
#endif

#include <DRMHelper.h>

#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>

#include "UniModelConst.h"
#include "UniMimeInfo.h"
#include "UniDrmInfo.h"
#include "UniObject.h"
#include "UniDataUtils.h"

//#include "UniEditorLogging.h"
#include "fileprotectionresolver.h"
#include "MsgMimeTypes.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================


// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniDrmInfo::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniDrmInfo* CUniDrmInfo::NewLC( RFs& aFs,
                                          CBaseMtm& aMtm,
                                          TMsvAttachmentId& aId,
                                          CMsgMediaInfo& aMediaInfo )
    {
    CUniDrmInfo* self =  new ( ELeave ) CUniDrmInfo( aFs, aMtm, aId, aMediaInfo );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CUniDrmInfo::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniDrmInfo* CUniDrmInfo::NewL( RFs& aFs,
                                         CBaseMtm& aMtm,
                                         TMsvAttachmentId& aId,
                                         CMsgMediaInfo& aMediaInfo )
    {
    CUniDrmInfo* self = NewLC( aFs, aMtm, aId, aMediaInfo );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CUniDrmInfo::CUniDrmInfo
//
// Constructor.
// ---------------------------------------------------------
//
CUniDrmInfo::CUniDrmInfo( RFs& aFs,
                          CBaseMtm& aMtm,
                          TMsvAttachmentId& aId,
                          CMsgMediaInfo& aMediaInfo ) :
    iFs( aFs ),
    iMtm( aMtm ),
    iAttachmentId( aId ),
    iMediaInfo( aMediaInfo ),
    iDRMConsumeStatus( EUniDRMNone ),
    iDRMConsumeCount( 0 ),
    iDRMRightsExist( ETrue ), //Rights ok by default
    iDRMExpiration( CDRMRights::EValidRights ),
    iDRMRestriction( CDRMRights::EFullRights ),
    iDRMConstraintType( CDRMRights::ENoConstraints )
    {
    }

// ---------------------------------------------------------
// CUniDrmInfo::CUniDrmInfo
//
// Destructor.
// ---------------------------------------------------------
//
CUniDrmInfo::~CUniDrmInfo()
    {
    ReleaseRights();
    delete iDRMHelper;

#ifdef DRMCOMMONIN_POISTO
    delete iRightsManager;
#else
    delete iDRMCommon;
#endif

    delete iSendContentUrl;
    }

// ---------------------------------------------------------
// CUniDrmInfo::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniDrmInfo::ConstructL()
    {
    if ( !iMediaInfo.DRMContentURI()->Length() )
        {
        User::Leave( KErrNotSupported );
        }

    // Get supported features from feature manager.
/*
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdDrmFull ) )
        {
        iSupportedFeatures |= EUniFeatureDrmFull;
        }
    FeatureManager::UnInitializeLib();
*/

#ifdef DRMCOMMONIN_POISTO

    // Seems pretty heavy. Should be done only once by e.g. Document
    // and a reference should be passed as an input parameter?
    CManager* manager = CManager::NewLC();

    RArray<TAgent> agents;
    manager->ListAgentsL( agents );
    CleanupClosePushL( agents );

    for ( TInt i = 0; !iRightsManager && i < agents.Count(); ++i )
        {
        if ( agents[ i ].Name().Compare( KOmaDrm2AgentName ) == 0 )
            {
            iRightsManager = manager->CreateRightsManagerL( agents[ i ] );
            }
        }

    CleanupStack::PopAndDestroy( 2, manager ); // agents, manager

    if ( !iRightsManager )
        {
        User::Leave( KErrNotFound );
        }

#else // DRMCOMMONIN_POISTO

    iDRMCommon = DRMCommon::NewL();

#endif // DRMCOMMONIN_POISTO

    iDRMHelper = CDRMHelper::NewL( *CEikonEnv::Static( ), iFs );
    DoGetRightsInfoL();

    // Check it here that image count is rights.
    // When image is displayed, use count is already decremented.
    SetIsDrmWithOnePreviewL();

    TInt drmMethods;
    CDRMHelper::TDRMHelperOMALevel omaLevel;
    iDRMHelper->SupportedDRMMethods2( drmMethods, omaLevel );
    iOma2 = ( omaLevel == CDRMHelper::EOMA_2_0 ? ETrue : EFalse );
    }

// ---------------------------------------------------------
// CUniDrmInfo::DoGetRightsInfoL
// ---------------------------------------------------------
//
void CUniDrmInfo::DoGetRightsInfoL( CDRMRights** /*aRightsObj*/ )
    {
/*
    if ( iSupportedFeatures & EUniFeatureDrmFull )
        {
        CDRMRights* rightsObj = NULL;
        TInt rightSpec = ConsumeIntent();

#ifdef DRMCOMMONIN_POISTO

        MAgentRightsBase* cafRights = NULL;
        RStreamablePtrArray<CRightsInfo> rights;

        // Copy to 16-bit buffer
        // TODO: Is this really the correct parameter to give to ListRightsL?!
        HBufC* temp = HBufC::NewLC( iMediaInfo.DRMContentURI()->Length() );
        TPtr ptr = temp->Des();
        ptr.Copy( *iMediaInfo.DRMContentURI() );

        iRightsManager->ListRightsL( rights, *temp );

        CleanupStack::PopAndDestroy( temp );
        if ( rights.Count() )
            {
            // The active RO is the first one in the list
            //CRightsInfo* rightsInfo = rights[0]->AllocL();
            CRightsInfo* rightsInfo = rights[ 0 ];
            cafRights = iRightsManager->GetRightsDataL( *rightsInfo );
            rightsObj = reinterpret_cast<CDRMRights*>( cafRights );
            rightsObj->GetRightsInfo(
                rightSpec,
                iDRMRestriction,
                iDRMExpiration,
                iDRMConstraintType );
            iDRMRightsExist = ETrue;
            }
        else
            {
            iDRMRightsExist = EFalse;
            }
        rights.ResetAndDestroy();

#else // DRMCOMMONIN_POISTO

        TInt restriction = iDRMCommon->GetActiveRights(
            *iMediaInfo.DRMContentURI(),
            rightSpec,
            rightsObj );

        if ( restriction == CDRMRights::ENoRights ||
            !rightsObj )
            {
            iDRMRightsExist = EFalse;
            }
        else
            {
            rightsObj->GetRightsInfo(
                rightSpec,
                iDRMRestriction,
                iDRMExpiration,
                iDRMConstraintType );
            iDRMRightsExist = ETrue;
            }

#endif // DRMCOMMONIN_POISTO

        if ( aRightsObj )
            {
            (*aRightsObj) = rightsObj;
            }
        else
            {
            delete rightsObj;
            }
        }
*/
    }

// ---------------------------------------------------------
// CUniDrmInfo::ConsumeRights
// ---------------------------------------------------------
//
EXPORT_C TInt CUniDrmInfo::ConsumeRights()
    {
//    UNILOGGER_ENTERFN( "CUniDrmInfo::ConsumeRights" );

    TInt err( KErrNone );
    if ( ( iMediaInfo.MediaType() == EMsgMediaAudio ||
           ( iMediaInfo.MediaType() != EMsgMediaAudio &&
             iDRMExpiration == CDRMRights::EValidRights ) ) &&
         iDRMConsumeStatus != EUniDRMConsumed )
        {
        if ( iDRMConsumeCount )
            {
            //UNILOGGER_WRITE( "Rights freezed" );

            // Already once consumed. Freeze is enough!
            FreezeRights();
            }
        else
            {
            if ( iDRMConsumeStatus == EUniDRMFrozen )
                {
                //UNILOGGER_WRITE( "Rights released" );

                ReleaseRights();
                }

            err = iDRMHelper->Consume2( iMediaInfo.DRMContentURI()->Des(),
                                        ConsumeIntent(),
                                        CDRMHelper::EStart );

            if ( !err )
                {
                //UNILOGGER_WRITE( "Rights consumed" );

                iDRMConsumeStatus = EUniDRMConsumed;
                iDRMConsumeCount++;
                }
            }
        }
    // else - never consume for preview

    //UNILOGGER_WRITEF( _L("Result: %d"), err );
    //UNILOGGER_LEAVEFN( "CUniDrmInfo::ConsumeRights" );

    return err;
    }

// ---------------------------------------------------------
// CUniDrmInfo::EvaluateRights
// ---------------------------------------------------------
//
EXPORT_C TInt CUniDrmInfo::EvaluateRights( TBool& aConsumed )
    {
    //UNILOGGER_ENTERFN( "CUniDrmInfo::EvaluateRights" );

    aConsumed = ( iDRMConsumeStatus == EUniDRMConsumed ||  iDRMConsumeCount );

    TInt err( KErrNone );

    CData* cData = NULL;
    TRAP_IGNORE( cData = CreateCDataL() );

    if ( cData )
        {
        err = cData->EvaluateIntent( ConsumeIntent() );
        delete cData;
        }

    //UNILOGGER_WRITEF( _L("Result: %d, aConsumed %d"), err, aConsumed );
    //UNILOGGER_LEAVEFN( "CUniDrmInfo::EvaluateRights" );

    return err;
    }

// ---------------------------------------------------------
// CUniDrmInfo::FreezeRights
// ---------------------------------------------------------
//
EXPORT_C TInt CUniDrmInfo::FreezeRights()
    {
    //UNILOGGER_ENTERFN( "CUniDrmInfo::FreezeRights" );

    TInt err( KErrNone );
    if ( ( iMediaInfo.MediaType() == EMsgMediaAudio ||
           ( iMediaInfo.MediaType() != EMsgMediaAudio &&
             iDRMExpiration == CDRMRights::EValidRights ) ) &&
         iDRMConsumeStatus == EUniDRMNone )
        {
        err = iDRMHelper->Consume2( iMediaInfo.DRMContentURI()->Des(),
                                    ContentAccess::EInstall,
                                    CDRMHelper::EStart );

        if ( !err )
            {
            //UNILOGGER_WRITE( "Rights frozen" );

            iDRMConsumeStatus = EUniDRMFrozen;
            }
        }

    //UNILOGGER_WRITEF( _L("Result: %d"), err );
    //UNILOGGER_LEAVEFN( "CUniDrmInfo::FreezeRights" );

    return err;
    }

// ---------------------------------------------------------
// CUniDrmInfo::ReleaseRights
// ---------------------------------------------------------
//
EXPORT_C TInt CUniDrmInfo::ReleaseRights()
    {
    //UNILOGGER_ENTERFN( "CUniDrmInfo::ReleaseRights" );

    TInt err( KErrNone );
    if ( iDRMConsumeStatus != EUniDRMNone )
        {
        TUint32 rightsSpec = ( iDRMConsumeStatus == EUniDRMFrozen )
            ? ContentAccess::EInstall
            : ConsumeIntent();

        err = iDRMHelper->Consume2( iMediaInfo.DRMContentURI()->Des(),
                                    rightsSpec,
                                    CDRMHelper::EFinish );

        //UNILOGGER_WRITE( "Rights released" );

        // else - never consumed for preview
        iDRMConsumeStatus = EUniDRMNone;
        }

    //UNILOGGER_WRITEF( _L("Result: %d"), err );
    //UNILOGGER_LEAVEFN( "CUniDrmInfo::ReleaseRights" );

    return err;
    }

// ---------------------------------------------------------
//  CUniDrmInfo::RightsValidL
// ---------------------------------------------------------
//
EXPORT_C TBool CUniDrmInfo::RightsValidL( TBool aUpdate )
    {
    //UNILOGGER_ENTERFN( "CUniDrmInfo::RightsValidL" );
    //UNILOGGER_WRITEF( _L("aUpdate: %d"), aUpdate );

    // By default rights are valid
    TBool retVal = ETrue;
    if ( aUpdate )
        {
        DoGetRightsInfoL();

        //UNILOGGER_WRITE( "Update done" );
        }

    if ( iMediaInfo.Protection() &
        ( EFileProtForwardLocked | EFileProtSuperDistributable ) )
        {
        if ( iDRMRightsExist )
            {
            if ( iDRMExpiration == CDRMRights::EExpiredRights ||
                 iDRMExpiration == CDRMRights::EFutureRights )
                {
                // Expired or future rights
                retVal = EFalse;
                }
            }
        else
            {
            // No rights
            retVal = EFalse;
            }
        }

    //UNILOGGER_WRITEF( _L("Result: %d"), retVal );
    //UNILOGGER_LEAVEFN( "CUniDrmInfo::RightsValidL" );

    return retVal;
    }

// ---------------------------------------------------------
// CUniDrmInfo::IsDrmWithOnePreview
// ---------------------------------------------------------
//
EXPORT_C TBool CUniDrmInfo::IsDrmWithOnePreview()
    {
    return iDrmOnePreview;
    }

// ---------------------------------------------------------
// CUniDrmInfo::SetIsDrmWithOnePreviewL
// ---------------------------------------------------------
//
void CUniDrmInfo::SetIsDrmWithOnePreviewL()
    {
/*
    iDrmOnePreview = EFalse;
    if ( !( iSupportedFeatures & EUniFeatureDrmFull ) )
        {
        return;
        }

    // Check only object list. Attachments are left for
    // dedicated attachment handler's responsibility.
    CDRMRights* rights = NULL;
    DoGetRightsInfoL(&rights);

    if ( rights &&
         iDRMRightsExist &&
         !( iDRMConstraintType ^ CDRMRights::ECountBased ) &&   // only ECountBased bit set
         iMediaInfo.Protection() & EFileProtForwardLocked )
        {
        // must get count still
        // no leaving functions in this function
        CDRMRightsConstraints* constraint = NULL;
        TMsgMediaType mediaType = iMediaInfo.MediaType();

        if ( mediaType == EMsgMediaImage ||
             mediaType == EMsgMediaSvg )
            {
            rights->GetDisplayRight( constraint );
            }
        else if ( mediaType == EMsgMediaAudio ||
                  mediaType == EMsgMediaVideo )
            {
            rights->GetPlayRight( constraint );
            }

        if ( constraint &&
             constraint->IsPreview() )
            {
            TUint32 origCount = 0;
            TUint32 nowCount = 0;
            constraint->GetCounters( nowCount,  origCount);

            if ( nowCount == 1 )
                {
                iDrmOnePreview = ETrue;
                }
            }

        delete constraint;
        }

    delete rights;
*/
    }


// ---------------------------------------------------------
// CUniDrmInfo::CreateCDataL
// ---------------------------------------------------------
//
CData* CUniDrmInfo::CreateCDataL()
    {
    CData* result = NULL;

    if ( iMediaInfo.DRMContentURI() )
        {
        // depends on iAttachmentId
        RFile attachmentFile = CUniDataUtils::GetAttachmentFileL( iMtm, iAttachmentId );
        CleanupClosePushL( attachmentFile );

        result = CData::NewL( attachmentFile,
                              KDefaultContentObject,
                              EPeek );

        CleanupStack::PopAndDestroy( &attachmentFile );
        }

    return result;
    }

// ---------------------------------------------------------
// CUniDrmInfo::HandleDrmErrorL
// ---------------------------------------------------------
//
EXPORT_C void CUniDrmInfo::HandleDrmErrorL( TInt aError )
    {
    if ( iDRMHelper )
        {
        RFile attachmentFile = CUniDataUtils::GetAttachmentFileL( iMtm, iAttachmentId );
        CleanupClosePushL( attachmentFile );

        iDRMHelper->HandleErrorL(   aError,
                                    attachmentFile );
        CleanupStack::PopAndDestroy( &attachmentFile );
        }
    }


// EOF
