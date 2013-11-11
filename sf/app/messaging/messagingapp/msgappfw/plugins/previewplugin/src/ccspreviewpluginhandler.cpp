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
 * Description:  CS Preview Plugin Handler, This class creates and updates sqlite based db
 *                with the message-preview data.
 *
 */
// USER INCLUDES
#include "ccspreviewpluginhandler.h"
#include "UniObject.h"
#include "msgbiouids.h"
// SYSTEM INCLUDES
#include <bioscmds.h>
#include <biocmtm.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <msvids.h>
#include <e32const.h>
#include <SendUiConsts.h>
#include <utf.h>
#include <centralrepository.h>
#include <MmsConformance.h>
#include <mmsconst.h>
#include <msgmediainfo.h>
#include <MsgMediaResolver.h>
#include <fileprotectionresolver.h>
#include <MmsEngineInternalCRKeys.h>
#include <imageconversion.h>
//CONSTANTS
//DB-file
_LIT(KDbFileName, "c:[2002A542]conversations.db");
//Encoding
_LIT(KEncodingStmnt,"PRAGMA encoding=\"UTF-8\"");
//Size
_LIT(KCacheSizeStmnt,"PRAGMA default_cache_size = 1024");
// Create table query statement
_LIT(KSqlCreateStmt, "CREATE TABLE IF NOT EXISTS conversation_messages ( message_id  INTEGER PRIMARY KEY, msg_processingstate INTEGER DEFAULT 0, subject TEXT(100), body_text TEXT(160), preview_path TEXT, msg_property INTEGER, preview_icon BLOB DEFAULT NULL ) " );
//Create an empty record for the given message id
_LIT(KSqlBasicInsertStmt, "INSERT OR REPLACE INTO conversation_messages ( message_id ) VALUES( :message_id )");
//Insert without bitmap query
_LIT(KSqlInsertStmt, "INSERT OR REPLACE INTO conversation_messages ( message_id, msg_processingstate, subject, body_text, preview_path, msg_property ) VALUES( :message_id, :msg_processingstate, :subject, :body_text, :preview_path,  :msg_property )");
//update processing-state flag of a message
_LIT(KSqlUpdateProcessingStateStmt, "UPDATE conversation_messages SET msg_processingstate=:msg_processingstate WHERE message_id=:message_id " );
//update with bitmap query
_LIT(KSqlUpdateBitmapStmt, "UPDATE conversation_messages SET preview_icon=:preview_icon WHERE message_id=:message_id " );
// query to see if msg is under process at the moment
_LIT(KSelectProcessingStateStmt, " SELECT message_id, msg_processingstate FROM conversation_messages WHERE message_id=:message_id ");
// Remove record from conversation_messages table.
_LIT(KRemoveMsgStmnt,"DELETE FROM conversation_messages WHERE message_id=:message_id");
//Insert vcard meta-daa query
_LIT(KSqlInsertBioMsgStmt, "INSERT OR REPLACE INTO conversation_messages ( message_id, msg_processingstate, preview_path ) VALUES( :message_id, :msg_processingstate, :preview_path )");

const TInt KDefaultMaxSize = 300 * 1024;
//Preview thumbnail size
const TInt KWidth = 24 * 6.7;
const TInt KHeight = 24 * 6.7;

// NOTE:- DRAFTS ENTRIES ARE NOT HANDLED IN THE PLUGIN

// ============================== MEMBER FUNCTIONS ============================
// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsPreviewPluginHandler* CCsPreviewPluginHandler::NewL(
    CCsPreviewPlugin *aMsgObserver)
{
    PRINT ( _L("Enter CCsMsgHandler::NewL") );

    CCsPreviewPluginHandler* self = new (ELeave) CCsPreviewPluginHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aMsgObserver);
    CleanupStack::Pop(self);

    PRINT ( _L("End CCsPreviewPluginHandler::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::~CCsPreviewPluginHandler
// Destructor
// ----------------------------------------------------------------------------
//
CCsPreviewPluginHandler::~CCsPreviewPluginHandler()
{
    PRINT ( _L("Enter CCsPreviewPluginHandler::~CCsPreviewPluginHandler") );

    iSqlDb.Close();
    iThumbnailRequestArray.Close();
    ifsSession.Close();

    if (iMmsMtm)
    {
        delete iMmsMtm;
        iMmsMtm = NULL;
    }

    if (iBioClientMtm)
        {
        delete iBioClientMtm;
        iBioClientMtm = NULL;
        }

    if (iMtmRegistry)
    {
        delete iMtmRegistry;
        iMtmRegistry = NULL;
    }

    if (iSession)
    {
        delete iSession;
        iSession = NULL;
    }

    if (iThumbnailManager)
    {
        delete iThumbnailManager;
        iThumbnailManager = NULL;
    }

    PRINT ( _L("End CCsPreviewPluginHandler::~CCsPreviewPluginHandler") );
}

// ----------------------------------------------------------------------------
// CCsMsgHandler::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::ConstructL(CCsPreviewPlugin *aMsgObserver)
{
    PRINT ( _L("Enter CCsPreviewPluginHandler::ConstructL") );

    iMsgObserver = aMsgObserver;

    //file session connect
    User::LeaveIfError(ifsSession.Connect());

    //create msv session
    iSession = CMsvSession::OpenSyncL(*this);

    //create mtm registry
    iMtmRegistry = CClientMtmRegistry::NewL(*iSession);

    //create mms client mtm
    iMmsMtm = static_cast<CMmsClientMtm*> (iMtmRegistry-> NewMtmL(
            KSenduiMtmMmsUid));

    // create biomsg client mtm
    iBioClientMtm = static_cast<CBIOClientMtm*> (iMtmRegistry->NewMtmL(
            KSenduiMtmBioUid));

    //create thumbnail manager
    iThumbnailManager = CThumbnailManager::NewL(*this);

    // open DB
    TInt error = iSqlDb.Open(KDbFileName);

    PRINT1 ( _L("End CCsPreviewPluginHandler::ConstructL open DB file error=%d"), error );

    // if not found, create DB
    if (error == KErrNotFound)
    {
        //create sqlite-DB
        TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
        RSqlSecurityPolicy securityPolicy;
        securityPolicy.Create(defaultPolicy);

        // TODO, setting UID security policy
        //TSecurityPolicy readPolicy(ECapabilityReadUserData);  
        //securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, readPolicy);

        iSqlDb.Create(KDbFileName, securityPolicy);

        //Create the table inside DB
        iSqlDb.Exec(KSqlCreateStmt);
        iSqlDb.Exec(KEncodingStmnt);
        iSqlDb.Exec(KCacheSizeStmnt);
    }
    else
    {
        User::LeaveIfError(error);
    }

    //get the max size of mms from the repository
    TRAP_IGNORE(
            CRepository* repository = CRepository::NewL(KCRUidMmsEngine);
            CleanupStack::PushL(repository);

            //Fetch and set max mms composition size
            TInt maxSize = KDefaultMaxSize;
            repository->Get( KMmsEngineMaximumSendSize, maxSize );
            iMaxMmsSize = maxSize;

            //Fetch and set creation mode
            TInt creationMode = EMmsCreationModeRestricted;
            repository->Get(KMmsEngineCreationMode, creationMode);
            iCreationMode = creationMode;

            CleanupStack::PopAndDestroy(repository);
    );
    PRINT ( _L("End CCsPreviewPluginHandler::ConstructL") );
}

// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::CCsPreviewPluginHandler
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsPreviewPluginHandler::CCsPreviewPluginHandler()
{
}

// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleSessionEventL
// Implemented for MMsvSessionObserver
// ----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleSessionEventL(TMsvSessionEvent aEvent,
    TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
{
    PRINT1 ( _L("Enter CCsPreviewPluginHandler::HandleSessionEventL aEvent=%d"),aEvent );

    CMsvEntrySelection* selection = NULL;
    TMsvId parent;

    //args
    if (aArg1 == NULL || aArg2 == NULL)
    {
        PRINT ( _L("Enter CCsPreviewPluginHandler::HandleSessionEventL arguments invalid"));
        return;
    }

    //start, processing the event
    selection = (CMsvEntrySelection*) aArg1;
    parent = *(TMsvId*) aArg2;

    //Drafts not handled
    if (KMsvDraftEntryIdValue == parent)
    {
        return;
    }

    switch (aEvent)
    {
        case EMsvEntriesChanged:
        case EMsvEntriesMoved:
        {
            HandleEventL(selection);
        }
        break;

        case EMsvEntriesDeleted:
        {
            for (TInt i = 0; i < selection->Count(); i++)
            {
                RSqlStatement sqlDeleteStmt;
                CleanupClosePushL(sqlDeleteStmt);
                sqlDeleteStmt.PrepareL(iSqlDb, KRemoveMsgStmnt);

                TInt messageIdIndex = sqlDeleteStmt.ParameterIndex(_L(
                    ":message_id"));
                User::LeaveIfError(sqlDeleteStmt.BindInt(messageIdIndex, selection->At(i)));

                User::LeaveIfError(sqlDeleteStmt.Exec());
                CleanupStack::PopAndDestroy(&sqlDeleteStmt);
            }
        }
        break;
    }

    PRINT ( _L("Exit CCsPreviewPluginHandler::HandleSessionEventL") );
}

// ---------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleEvent
// Handle events
// ---------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleEventL(CMsvEntrySelection* aSelection)
{
    PRINT ( _L("Enter CCsPreviewPluginHandler::HandleEvent start.") );

    TMsvEntry entry;
    TMsvId service;
    TInt error = KErrNone;

    for (TInt i = 0; i < aSelection->Count(); i++)
    {
        error = iSession->GetEntry(aSelection->At(i), service, entry);

        if ((KErrNone == error) && !entry.InPreparation() && entry.Visible())
        {
            PRINT ( _L("Enter CCsPreviewPluginHandler::HandleEvent for loop started.") );
            if ((KSenduiMtmMmsUidValue == entry.iMtm.iUid))
                {
                HandleMMSEntryL(entry);
                }
            else if ((KSenduiMtmSmsUidValue == entry.iMtm.iUid) || (KSenduiMtmBioUidValue == entry.iMtm.iUid))
                {
                if ((KMsgBioUidVCard.iUid == entry.iBioType))
                    {                 
                    HandleVCardEntryL(entry);
                    }
                }
            }
        }//end for loop

    PRINT ( _L("Exit CCsPreviewPluginHandler::HandleEvent end.") );
    }
// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleMMSEntryL()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleMMSEntryL(const TMsvEntry& aEntry)
    {
    PRINT ( _L("Enter CCsPreviewPluginHandler::HandleMMSEntry start.") );

    TInt msgId = aEntry.Id();

    // check if the msg is already under processing Or processed
    if (EPreviewMsgNotProcessed != msgProcessingState(msgId))
        {
        // skip processing this event for the given message
        return;
        }
    
    // start processing message, set flag
    setMsgProcessingState(msgId, EPreviewMsgProcessing);


    TRAPD( err,
            {        
    // update db with message preview data
    RSqlStatement sqlInsertStmt;
    CleanupClosePushL(sqlInsertStmt);
    sqlInsertStmt.PrepareL(iSqlDb, KSqlInsertStmt);
    
    // parse message
    iMmsMtm->SwitchCurrentEntryL(msgId);
    iMmsMtm->LoadMessageL();

    CUniDataModel* uniDataModel = CUniDataModel::NewL(ifsSession, *iMmsMtm);
    CleanupStack::PushL(uniDataModel);
    uniDataModel->RestoreL(*this, ETrue);
    
    //msg property
    TInt msgProperty = 0;
    if (uniDataModel->AttachmentList().Count() > 0)
        {
        msgProperty |= EPreviewAttachment;
        }

    //check for msg forward
    //Validate if the mms msg can be forwarded or not
    if (ValidateMsgForForward(uniDataModel))
        {
        msgProperty |= EPreviewForward;
        }

    TPtrC videoPath;
    TPtrC imagePath;

    // preview parsing
    TInt slideCount = uniDataModel->SmilModel().SlideCount();
    TBool isBodyTextSet = EFalse;
    TBool isImageSet = EFalse;
    TBool isAudioSet = EFalse;
    TBool isVideoSet = EFalse;

    for (int i = 0; i < slideCount; i++)
        {
        int slideobjcount = uniDataModel->SmilModel().SlideObjectCount(i);
        for (int j = 0; j < slideobjcount; j++)
            {
            CUniObject *obj = uniDataModel->SmilModel(). GetObjectByIndex(i,
                    j);
            CMsgMediaInfo *mediaInfo = obj->MediaInfo();

            TPtrC8 mimetype = obj->MimeType();
            TMsvAttachmentId attachId = obj->AttachmentId();

            //bodytext
            if (!isBodyTextSet
                    && (mimetype.Find(_L8("text")) != KErrNotFound))
                {
                //bind bodytext into statement
                BindBodyText(sqlInsertStmt, attachId);
                isBodyTextSet = ETrue;
                }

            //image parsing
            if (!isVideoSet && !isImageSet && (mimetype.Find(_L8("image"))
                    != KErrNotFound))
                {
                //get thumbnail for this image
                isImageSet = ETrue;
                imagePath.Set(mediaInfo->FullFilePath());
                msgProperty |= EPreviewImage;

                if (EFileProtNoProtection != mediaInfo->Protection())
                    {
                    msgProperty |= EPreviewProtectedImage;
                    }
                if (mediaInfo->Corrupt())
                    {
                    msgProperty |= EPreviewCorruptedImage;
                    }

                if (!(EPreviewProtectedImage & msgProperty)
                        && !(EPreviewCorruptedImage & msgProperty))
                    {
                    //Generate thumbnail for non protected,
                    //non corrupted image.
                    GetThumbNailL(attachId, mimetype, msgId);
                    }
                }

            //audio content
            if (!isVideoSet && !isAudioSet && (mimetype.Find(_L8("audio"))
                    != KErrNotFound))
                {
                isAudioSet = ETrue;
                msgProperty |= EPreviewAudio;
                if (EFileProtNoProtection != mediaInfo->Protection())
                    {
                    msgProperty |= EPreviewProtectedAudio;
                    }
                if (mediaInfo->Corrupt())
                    {
                    msgProperty |= EPreviewCorruptedAudio;
                    }
                }

            //video content
            if (!(isImageSet || isAudioSet) && !isVideoSet && (mimetype.Find(
                    _L8("video")) != KErrNotFound))
                {
                videoPath.Set(mediaInfo->FullFilePath());
                isVideoSet = ETrue;
                msgProperty |= EPreviewVideo;
                if (EFileProtNoProtection != mediaInfo->Protection())
                    {
                    msgProperty |= EPreviewProtectedVideo;
                    }
                if (mediaInfo->Corrupt())
                    {
                    msgProperty |= EPreviewCorruptedVideo;
                    }
                }
            }
        }

    //set preview path
    TInt previewPathIndex = sqlInsertStmt.ParameterIndex(_L(
            ":preview_path"));
    if (isVideoSet)
        {
        User::LeaveIfError(
                sqlInsertStmt.BindText(previewPathIndex, videoPath));
        }
    else if (isImageSet)
        {
        User::LeaveIfError(
                sqlInsertStmt.BindText(previewPathIndex, imagePath));
        }

    //msg_id
    TInt msgIdIndex = sqlInsertStmt.ParameterIndex(_L(":message_id"));
    User::LeaveIfError(sqlInsertStmt.BindInt(msgIdIndex, msgId));

    //subjext
    TInt subjectIndex = sqlInsertStmt.ParameterIndex(_L(":subject"));
    User::LeaveIfError(sqlInsertStmt.BindText(subjectIndex,
            iMmsMtm->SubjectL()));

    //msg_property
    TInt msgPropertyIndex = sqlInsertStmt.ParameterIndex(_L(
            ":msg_property"));
    User::LeaveIfError(sqlInsertStmt.BindInt(msgPropertyIndex, msgProperty));

    //msg_processingstate
    TInt msgProcessingStateIndex = sqlInsertStmt.ParameterIndex(
            _L(":msg_processingstate"));
    User::LeaveIfError(sqlInsertStmt.BindInt(msgProcessingStateIndex,
            EPreviewMsgProcessed));

    //execute sql stament
    User::LeaveIfError(sqlInsertStmt.Exec());

    //cleanup
    CleanupStack::PopAndDestroy(2, &sqlInsertStmt);
            }); //TRAP
    
    if( err != KErrNone )
        {
        setMsgProcessingState(msgId, EPreviewMsgNotProcessed);
        }

    PRINT ( _L("Enter CCsPreviewPluginHandler::HandleMMSEntry end.") );
    }

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleVCardEntryL()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleVCardEntryL(const TMsvEntry& aEntry)
    {
    PRINT ( _L("Enter CCsPreviewPluginHandler::HandleBioMsgEntry start.") );

    TMsvId msgId = aEntry.Id();   
    
    // check if the msg is already under processing Or processed
    TInt msgProcessState = EPreviewMsgNotProcessed;
    msgProcessState = msgProcessingState(msgId);
    if (EPreviewMsgProcessed == msgProcessState)
        {
        return;
        }
    //get attachments
    CMsvEntry* cMsvEntry = CMsvEntry::NewL(iBioClientMtm->Session(), msgId,
               TMsvSelectionOrdering());
    
    CleanupStack::PushL(cMsvEntry);
    CMsvStore* store = cMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();

    TInt attachmentCount = attachMan.AttachmentCount();
    if (attachmentCount)
        {
        // get attachment file path
        RFile file = attachMan.GetAttachmentFileL(0);
        CleanupClosePushL(file);
        TFileName fullName;
        User::LeaveIfError(file.FullName(fullName));

        // update db with meta-data
        RSqlStatement sqlInsertStmt;
        CleanupClosePushL(sqlInsertStmt);
        sqlInsertStmt.PrepareL(iSqlDb, KSqlInsertBioMsgStmt);
       
        //msg_id
        TInt msgIdIndex = sqlInsertStmt.ParameterIndex(_L(":message_id"));
        User::LeaveIfError(sqlInsertStmt.BindInt(msgIdIndex, msgId));

        //set attachment path
        TInt previewPathIndex = sqlInsertStmt.ParameterIndex(_L(
                ":preview_path"));
        User::LeaveIfError(sqlInsertStmt.BindText(previewPathIndex, fullName));

        //msg_processingstate
        TInt msgProcessingStateIndex = sqlInsertStmt.ParameterIndex(
                _L(":msg_processingstate"));
        User::LeaveIfError(sqlInsertStmt.BindInt(msgProcessingStateIndex,
                EPreviewMsgProcessed));

        //execute sql stament
        User::LeaveIfError(sqlInsertStmt.Exec());

        //cleanup
        CleanupStack::PopAndDestroy(&sqlInsertStmt);
        CleanupStack::PopAndDestroy(&file);
        }

    CleanupStack::PopAndDestroy(2, cMsvEntry);//cMsvEntry,store

    PRINT ( _L("Enter CCsPreviewPluginHandler::HandleBioMsgEntry End.") );
    }

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::RestoreReady()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::RestoreReady(TInt /*aParseResult*/, TInt /*aError*/)
{

}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::ThumbnailReady()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::ThumbnailReady(TInt aError,
    MThumbnailData& aThumbnail, TThumbnailRequestId aId)
{
    // This function must not leave.
    if (!aError)
    {
        PRINT ( _L("CCsPreviewPluginHandler::ThumbnailReady received.") );
        TInt err;
        TRAP(err, HandleThumbnailReadyL(aThumbnail, aId));
        PRINT1 ( _L("CCsPreviewPluginHandler::ThumbnailReady handling error= %d."), err );
    }
    else
    {
        // An error occurred while getting the thumbnail.
        PRINT1 ( _L("End CCsPreviewPluginHandler::ThumbnailReady error= %d."), aError );
    }
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::ThumbnailPreviewReady()
// callback
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::ThumbnailPreviewReady(
    MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/)
{

}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleThumbnailReadyL()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleThumbnailReadyL(MThumbnailData& aThumbnail,
    TThumbnailRequestId aId)
{
    //match response to request
    ThumbnailRequestData tempObj;
    tempObj.iRequestId = aId;

    TInt index = iThumbnailRequestArray.Find(tempObj,
        CCsPreviewPluginHandler::CompareByRequestId);
    if (index < 0)
    {
        PRINT ( _L("End CCsPreviewPluginHandler::HandleThumbnailReady request match not found.") );
        return;
    }

    // get msg-id corresponding to the request-id
    TInt msgId = iThumbnailRequestArray[index].iMsgId;
    //remove the request from requestarray
    iThumbnailRequestArray.Remove(index);

    // get bitmap
    CFbsBitmap* bitmap = aThumbnail.Bitmap();

    // sql-statment to set preview-icon
    RSqlStatement sqlInsertStmt;
    CleanupClosePushL(sqlInsertStmt);
    sqlInsertStmt.PrepareL(iSqlDb, KSqlUpdateBitmapStmt);

    TInt msgIdIndex = sqlInsertStmt.ParameterIndex(_L(":message_id"));
    TInt previewIconIndex = sqlInsertStmt.ParameterIndex(_L(":preview_icon"));

    User::LeaveIfError(sqlInsertStmt.BindInt(msgIdIndex, msgId));

    RSqlParamWriteStream previewIconStream;
    CleanupClosePushL(previewIconStream);

    //bind data
    User::LeaveIfError(previewIconStream.BindBinary(sqlInsertStmt, previewIconIndex));
    bitmap->ExternalizeL(previewIconStream);
    previewIconStream.CommitL();

    //execute the statent
    User::LeaveIfError(sqlInsertStmt.Exec());

    CleanupStack::PopAndDestroy(2,&sqlInsertStmt);//sqlInsertStmt,previewIconStream
}

TBool CCsPreviewPluginHandler::ValidateMsgForForward(CUniDataModel* aUniDataModel)
{
    TBool retValue = ETrue;

    //1. Check the slide count more than 1
    if (aUniDataModel->SmilModel().SlideCount() > 1)
    {
        retValue = EFalse;
        return retValue;
    }

    //2. message sixe check
    //Fetch and set max mms composition size
    if (iMmsMtm->MessageSize() > iMaxMmsSize)
    {
        retValue = EFalse;
        return retValue;
    }

    //3. If there is restricted content then return false
    RArray<TMsvAttachmentId>* pathList = GetSlideAttachmentIds(
            0, 
            aUniDataModel);
    
    CleanupStack::PushL(pathList);

    for (int i = 0; i < pathList->Count(); i++)
    {
        TMsvAttachmentId aId = (*pathList)[i];
        CMsvStore * store = iMmsMtm->Entry().ReadStoreL();
        CleanupStack::PushL(store);
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
        RFile fileHandle = attachMan.GetAttachmentFileL(aId);
        //close the store
        CleanupStack::PopAndDestroy(store);

        if (CheckModeForInsertL(fileHandle) != EInsertSuccess)
        {
            retValue = EFalse;
            break;
        }
    }

    if (retValue == EFalse)
    {
        CleanupStack::PopAndDestroy(pathList);
        return retValue;
    }

    CleanupStack::Pop(pathList);
    delete pathList;
    pathList = NULL;

    //4. check the same case for all attachments
    pathList = GetAttachmentIdList(aUniDataModel);
    CleanupStack::PushL(pathList);

    for (int i = 0; i < pathList->Count(); i++)
    {
        TMsvAttachmentId aId = (*pathList)[i];
        CMsvStore * store = iMmsMtm->Entry().ReadStoreL();
        CleanupStack::PushL(store);
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
        RFile fileHandle = attachMan.GetAttachmentFileL(aId);
        //close the store
        CleanupStack::PopAndDestroy(store);
        
        if (CheckModeForInsertL(fileHandle) != EInsertSuccess)
        {
            retValue = EFalse;
            break;
        }
    }

    CleanupStack::PopAndDestroy(pathList);
    return retValue;
}

RArray<TMsvAttachmentId>*
CCsPreviewPluginHandler::GetSlideAttachmentIds(TInt aSlideNum,
                                        CUniDataModel* aUniDataModel)
{
    TInt slideObjectCount =
            aUniDataModel->SmilModel().SlideObjectCount(aSlideNum);

    RArray<TMsvAttachmentId> *attachmentIdList = new (ELeave) RArray<
            TMsvAttachmentId> ();
    for (TInt i = 0; i < slideObjectCount; i++)
    {
        CUniObject *obj =
                aUniDataModel->SmilModel().GetObjectByIndex(aSlideNum, i);
        attachmentIdList->Append(obj->AttachmentId());
    }
    return attachmentIdList;
}

RArray<TMsvAttachmentId>*
CCsPreviewPluginHandler::GetAttachmentIdList(CUniDataModel* aUniDataModel)
{
    TInt attcount = aUniDataModel->AttachmentList().Count();
    RArray<TMsvAttachmentId> *attachmentIdList = new (ELeave) RArray<
            TMsvAttachmentId> ();

    for (TInt i = 0; i < attcount; i++)
    {
        CUniObject *obj = aUniDataModel->AttachmentList().GetByIndex(i);

        attachmentIdList->AppendL(obj->AttachmentId());
    }
    return attachmentIdList;
}

TInt CCsPreviewPluginHandler::CheckModeForInsertL(RFile aFileHandle)
{
    CleanupClosePushL(aFileHandle);

    CMmsConformance* mmsConformance = CMmsConformance::NewL();
    mmsConformance->CheckCharacterSet(EFalse);

    CleanupStack::PushL(mmsConformance);

    CMsgMediaResolver* mediaResolver = CMsgMediaResolver::NewL();
    mediaResolver->SetCharacterSetRecognition(EFalse);

    CleanupStack::PushL(mediaResolver);

    CMsgMediaInfo* info = mediaResolver->CreateMediaInfoL(aFileHandle);
    mediaResolver->ParseInfoDetailsL(info, aFileHandle);

    TMmsConformance conformance = mmsConformance->MediaConformance(*info);
    iConfStatus = conformance.iConfStatus;

    CleanupStack::PopAndDestroy(3);

    // In "free" mode user can insert images that are larger by dimensions than allowed by conformance
    if (iCreationMode != EMmsCreationModeRestricted)
    {
        TInt i = EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded
                | EMmsConfNokTooBig;
        TInt j = ~ (EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded
                | EMmsConfNokTooBig);

        // If user answers yes to Guided mode confirmation query he/she moves to free mode
        if ( (iConfStatus & i) && ! (iConfStatus & j))
        {
            if (iCreationMode == EMmsCreationModeFree || info->Protection()
                    & EFileProtSuperDistributable)
            {
                // SuperDistribution not checked here
                // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                iConfStatus &= ~EMmsConfNokFreeModeOnly;
                iConfStatus &= ~EMmsConfNokScalingNeeded;
            }
            else
            {
                delete info;
                //query not accepted. Stop insertion.
                return EInsertQueryAbort;
            }
        }
    }
    else if (iConfStatus & EMmsConfNokDRM || iConfStatus
            & EMmsConfNokNotEnoughInfo || iConfStatus & EMmsConfNokNotSupported
            || iConfStatus & EMmsConfNokFreeModeOnly || iConfStatus
            & EMmsConfNokCorrupt)
    {
        delete info;
        return EInsertNotSupported;
    }

    delete info;
    return EInsertSuccess;
}

//-----------------------------------------------------------------------------
// CCsPreviewPluginHandler::CompareByRequestId
// Compare to conversation entry object based on Entry Ids
//----------------------------------------------------------------------------
TBool CCsPreviewPluginHandler::CompareByRequestId(
    const ThumbnailRequestData& aFirst, const ThumbnailRequestData& aSecond)
{
    if (aFirst.iRequestId == aSecond.iRequestId)
        return ETrue;

    return EFalse;
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::BindBodyText()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::BindBodyText(RSqlStatement& sqlStmt,
    TMsvAttachmentId attachmentId)
{
    //get file handle from attachmnet manager.
    CMsvStore * store = iMmsMtm->Entry().ReadStoreL();
    CleanupStack::PushL(store);
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    RFile file = attachMan.GetAttachmentFileL(attachmentId);
    CleanupClosePushL(file);

    //read file contents to buffer
    TInt length = 0;
    file.Size(length);
    HBufC8* bodyText = HBufC8::NewLC(length);
    TPtr8 textBuffer = bodyText->Des();
    file.Read(textBuffer);

    // convert from HBufC8 to HBufC16
    HBufC16 *text16 = HBufC16::NewLC(textBuffer.Length());
    TPtr16 textPtr16 = text16->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(textPtr16, textBuffer);

    //set bodytext in the sql statement
    TInt bodyTextIndex = sqlStmt.ParameterIndex(_L(":body_text"));
    sqlStmt.BindText(bodyTextIndex, textPtr16);

    CleanupStack::PopAndDestroy(4, store); //store,file, bodyText, text16
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::GetThumbNailL()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::GetThumbNailL(TMsvAttachmentId attachmentId,
    TDesC8& mimeType, TMsvId msgId)
{

    // Create Thumbnail object source representing a path to a file
    HBufC* mimeInfo = HBufC::NewLC(mimeType.Length());
    mimeInfo->Des().Copy(mimeType);

    CMsvStore * store = iMmsMtm->Entry().ReadStoreL();
    CleanupStack::PushL(store);

    //get file handle from attachment manager.
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    RFile file = attachMan.GetAttachmentFileL(attachmentId);
    CleanupClosePushL(file);

    //Find if the image is portrait image (or) landscape image
    CImageDecoder *decoder =  CImageDecoder::FileNewL(file,mimeType,ContentAccess::EUnknown);
    CleanupStack::PushL(decoder);
    TSize size(0,0);
       
    TFrameInfo info = decoder->FrameInfo();
    
    size = info.iOverallSizeInPixels;
    
    CleanupStack::PopAndDestroy(decoder);
   
    TReal32 newLength = 0;
    
    if(size.iWidth >= size.iHeight)
    {
        //TODO replace with hb-param-graphic-size-image-portrait * value of un in pixcels
        if(size.iWidth < KWidth)
        {
            iThumbnailManager->SetThumbnailSizeL(size);    
        }
        else
        {
            newLength = (KWidth * size.iHeight) /size.iWidth;
            iThumbnailManager->SetThumbnailSizeL(TSize(KWidth, newLength));
        }
    }
    else
    {        
        //TODO replace with hb-param-graphic-size-image-portrait * value of un in pixcels
        if(size.iHeight < KHeight)
        {
            iThumbnailManager->SetThumbnailSizeL(size);    
        }
        else
        {
            newLength = (KHeight * size.iWidth) / size.iHeight;
            iThumbnailManager->SetThumbnailSizeL(TSize(newLength, KHeight));    
        }
    }

    //Scale the image
    iThumbnailManager->SetFlagsL(CThumbnailManager::ECropToAspectRatio);

    //optimize for performace
    iThumbnailManager->SetQualityPreferenceL(
        CThumbnailManager::EOptimizeForQuality);

    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
        (RFile64&) file, mimeInfo->Des());

    // Issue the request for thumbnail creation
    ThumbnailRequestData reqObject;
    reqObject.iMsgId = msgId;
    reqObject.iRequestId = iThumbnailManager->GetThumbnailL(*source);
    iThumbnailRequestArray.Append(reqObject);

    CleanupStack::PopAndDestroy(4, mimeInfo);//mimeInfo,store,file,source
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::msgProcessingState
// 
// -----------------------------------------------------------------------------
//
TInt CCsPreviewPluginHandler::msgProcessingState(TMsvId aMsgId)
{
    TInt retState = EPreviewMsgNotProcessed;

    // sql-statement to check if msg's under processing flag is set or not
    RSqlStatement sqlSelectStmt;
    CleanupClosePushL(sqlSelectStmt);
    sqlSelectStmt.PrepareL(iSqlDb,KSelectProcessingStateStmt);

    TInt msgIdIndex = sqlSelectStmt.ParameterIndex(_L(":message_id"));
    User::LeaveIfError(sqlSelectStmt.BindInt(msgIdIndex, aMsgId));

    // read the flag
    TInt msgProcessingStateIndex = sqlSelectStmt.ColumnIndex(_L("msg_processingstate"));
    if (sqlSelectStmt.Next() == KSqlAtRow)
    {
         retState = static_cast<TInt>(sqlSelectStmt.ColumnInt(msgProcessingStateIndex));
    }
    else
    {
        // this is first event for this msgid, hence record doesn't exist
        // create an empty record, so that we can set & use flags
        RSqlStatement sqlBasicInsertStmt;
        CleanupClosePushL(sqlBasicInsertStmt);
        sqlBasicInsertStmt.PrepareL(iSqlDb, KSqlBasicInsertStmt);
        TInt index_msgid = sqlBasicInsertStmt.ParameterIndex(_L(":message_id"));
        User::LeaveIfError(sqlBasicInsertStmt.BindInt(index_msgid, aMsgId));
        User::LeaveIfError(sqlBasicInsertStmt.Exec());
        CleanupStack::PopAndDestroy(&sqlBasicInsertStmt);
    }
    // cleanup
    CleanupStack::PopAndDestroy(&sqlSelectStmt);
    return retState;
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::setMsgProcessingState
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::setMsgProcessingState(TMsvId aMsgId, TInt aState)
{
    // sql-statment to set/reset msg's under processing flag
    RSqlStatement sqlUpdateStmt;
    CleanupClosePushL(sqlUpdateStmt);
    sqlUpdateStmt.PrepareL(iSqlDb, KSqlUpdateProcessingStateStmt);

    TInt msgIdIndex = sqlUpdateStmt.ParameterIndex(_L(":message_id"));
    User::LeaveIfError(sqlUpdateStmt.BindInt(msgIdIndex, aMsgId));

    // bind data
    TInt msgProcessingStateIndex = sqlUpdateStmt.ParameterIndex(_L(":msg_processingstate"));
    User::LeaveIfError(sqlUpdateStmt.BindInt(msgProcessingStateIndex, aState));

    // execute the statement
    User::LeaveIfError(sqlUpdateStmt.Exec());
    // cleanup
    CleanupStack::PopAndDestroy(&sqlUpdateStmt);
}

// End of file

