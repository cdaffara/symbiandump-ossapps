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

#include "unieditormmsplugin_p.h"

// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <msvids.h> 
#include <mtclreg.h>
#include <commdb.h> 
#include <mmsconst.h>
#include <mmsclient.h>
#include <mmsheaders.h>
#include <mmssettings.h>
#include <mmsmsventry.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh> 
#include <txtetext.h>
#include <txtrich.h>
#include <txtfmlyr.h>
#include <MuiuOperationWait.h>
#include <QDir>
#include <hbglobal.h>
#include <eikrted.h>
#include <MsgMediaInfo.h>
#include <MsgMediaResolver.h>

#include "UniSendingSettings.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "msgcontacthandler.h"
#include <xqconversions.h>
#include "debugtraces.h"

// Possible values for mms validity period in seconds
const TInt32 KUniMmsValidityPeriod1h = 3600;
const TInt32 KUniMmsValidityPeriod6h = 21600;
const TInt32 KUniMmsValidityPeriod24h = 86400;
const TInt32 KUniMmsValidityPeriod3Days = 259200;
const TInt32 KUniMmsValidityPeriodWeek = 604800;
const TInt32 KUniMmsValidityPeriodMax = 0;

const TInt  KMaxDetailsLength = 64; // Copy max this many chars to TMsvEntry::iDetails
_LIT( KAddressSeparator, ";" );

#define KSenduiMtmMmsUidValue 0x100058E1
const TUid KSenduiMtmMmsUid = {KSenduiMtmMmsUidValue};

#define LOC_FWD hbTrId("txt_messaging_formlabel_fwd")
#define LOC_RE hbTrId("txt_messaging_formlabel_re")

// -----------------------------------------------------------------------------
// Two-phased constructor.
// @see Header
// -----------------------------------------------------------------------------
//
CUniEditorMmsPluginPrivate* CUniEditorMmsPluginPrivate::NewL()
{
    CUniEditorMmsPluginPrivate* self = new ( ELeave ) CUniEditorMmsPluginPrivate(); 
    return self;
}

// -----------------------------------------------------------------------------
// Destructor
// @see Header
// -----------------------------------------------------------------------------
//
CUniEditorMmsPluginPrivate::~CUniEditorMmsPluginPrivate()
{
    //In case there is no settings at all leave occurs and resource not freed
    delete iMmsHeader;

    delete iEditor;
    delete iDom;
    delete iUniDataModel;
    ifsSession.Close();

    if(iMmsMtm)
    {
        delete iMmsMtm;
    }

    delete iMtmRegistry;
    delete iDataModelPluginLoader;

    if(iSession)
    {
        delete iSession;
    }
    delete iGenUtils;
}

// -----------------------------------------------------------------------------
// C++ default constructor
// @see Header
// -----------------------------------------------------------------------------
//
CUniEditorMmsPluginPrivate::CUniEditorMmsPluginPrivate( )
{
    TRAP_IGNORE(iSession = CMsvSession::OpenSyncL(*this));
    iGenUtils = new UniEditorGenUtils();
}

// -----------------------------------------------------------------------------
// convertFromL
// @see Header
// -----------------------------------------------------------------------------
//
ConvergedMessage* CUniEditorMmsPluginPrivate::convertFromL( TMsvId aId,
    UniEditorPluginInterface::EditorOperation aOperation )
{
    QDEBUG_WRITE("Enter convertFromL");

    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();

    TMsvEntry entry =MmsMtmL()->Entry().Entry();

    ConvergedMessage* msg = new ConvergedMessage;
    
    CleanupStack::PushL(msg);
    if( entry.Parent() == KMsvDraftEntryIdValue )
    {
       convertFromDraftsL(*msg);
    }
    else if( aOperation == UniEditorPluginInterface::Forward)
    {
        convertFromForwardHandlerL(*msg);          
    }
    else if(aOperation == UniEditorPluginInterface::Reply)
    {
        convertFromReplyHandlerL(msg);
    }
    else if(aOperation == UniEditorPluginInterface::ReplyAll)
    {
        convertFromReplyAllHandlerL(msg);
    }
    else if(aOperation == UniEditorPluginInterface::Default)
    {
        convertFromDefaultHandlerL(msg);
    }
    CleanupStack::Pop(msg);
    QDEBUG_WRITE("Exit convertFromL");
    return msg;
}

// -----------------------------------------------------------------------------
// convertFromDraftsL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::convertFromDraftsL(
    ConvergedMessage& aMessage )
{
    QDEBUG_WRITE("Enter convertFromDraftsL");
    
    TMsvEntry entry =MmsMtmL()->Entry().Entry();

    //Message id 
    ConvergedMessageId id(entry.Id());
    aMessage.setMessageId(id);

    // Set Message type
    aMessage.setMessageType(ConvergedMessage::Mms);

    //Populate recipients
    populateRecipientsL(aMessage);

    //populate convergedmessage with the subject
    aMessage.setSubject(XQConversions::s60DescToQString(
        MmsMtmL()->SubjectL()));

    // Priority
    TMsvPriority priority = entry.Priority();
    if( EMsvHighPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::High);
    }
    else if( EMsvLowPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::Low);
    }
    else if( EMsvMediumPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::Normal);
    }

    // Set direction and location
    aMessage.setDirection(ConvergedMessage::Outgoing);
    aMessage.setLocation (ConvergedMessage::Draft);

    //Populate body and attachments
    populateMessageBodyL(aMessage);

    QDEBUG_WRITE("Exit convertFromDraftsL");
}

// -----------------------------------------------------------------------------
// convertFromForwardHandlerL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::convertFromForwardHandlerL(
    ConvergedMessage &aMessage)
{
    QDEBUG_WRITE("Enter convertFromForwardHandlerL");
    iMessageForwarded = EFalse;
    TMsvEntry entry = MmsMtmL()->Entry().Entry();

    //populate convergedmessage with the subject prepended with FW:
    QString subject =
            XQConversions::s60DescToQString(MmsMtmL()->SubjectL());

    if (!(entry.Forwarded() || subject.startsWith(LOC_FWD,
            Qt::CaseInsensitive)))
        {
        subject.insert(0, LOC_FWD);
        iMessageForwarded = ETrue;
        }

    aMessage.setSubject(subject);

    // Priority
    TMsvPriority priority = entry.Priority();
    if( EMsvHighPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::High);
    }
    else if( EMsvLowPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::Low);
    }
    else if( EMsvMediumPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::Normal);
    }

    //Populate body and attachments
    //The region info inside slides is not maintained
    populateMessageBodyL(aMessage);
    
    QDEBUG_WRITE("Exit convertFromForwardHandlerL");
}

// -----------------------------------------------------------------------------
// populateMessageBodyL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::populateMessageBodyL(
    ConvergedMessage &aMessage)
{
    QDEBUG_WRITE("Enter populateMessageBodyL");
    
    if(!iDataModelPluginLoader)
    {
        iDataModelPluginLoader = new UniDataModelLoader;  
        iMmsDataPlugin = iDataModelPluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    }

    iMmsDataPlugin->restore(*MmsMtmL());

    int slideCount = iMmsDataPlugin->slideCount();

    int attachmentCount = iMmsDataPlugin->attachmentCount();

    ConvergedMessageAttachmentList attachmentList;

    if (slideCount > 0)
    {
        UniMessageInfoList slideContentList = 
            iMmsDataPlugin->slideContent(0);
        for (int i = 0; i < slideContentList.size(); ++i)
        {
            if( slideContentList.at(i)->mimetype().contains("text") )
            {
                QString textContent;
                QByteArray textArray;
                QFile file(slideContentList.at(i)->path());
                if (file.open(QIODevice::ReadOnly)) {
                    textArray = file.readAll();
                    char *data = new char[textArray.size()+1];
                    strcpy(data,textArray.data());
                    //This is needed since MMS text content 
                    //is stored in UTF8 format
                    textContent = textContent.fromUtf8(data,strlen(data));
                    aMessage.setBodyText(textContent);
                    file.close();
                    delete []data;
                }
                else {
                    return;
                }
            }
            else
            {
            ConvergedMessageAttachment* attachment =
                new ConvergedMessageAttachment(
                    slideContentList.at(i)->path(),
                    ConvergedMessageAttachment::EInline);
            attachmentList << attachment;
            }
        }
        
        foreach(UniMessageInfo* slide,slideContentList)
        {
            delete slide;
        }
    }

    
    if(attachmentCount > 0)
    {
        UniMessageInfoList modelAttachmentList = 
            iMmsDataPlugin->attachmentList();

        for (int i = 0; i < modelAttachmentList.count(); ++i)
        {
            ConvergedMessageAttachment* attachment =
                new ConvergedMessageAttachment(
                    modelAttachmentList.at(i)->path(),
                    ConvergedMessageAttachment::EAttachment);
            attachmentList << attachment;
        }
        
        foreach(UniMessageInfo* attachment,modelAttachmentList)
        {
            delete attachment;
        }
    }

    if(attachmentList.count() > 0)
    {
        aMessage.addAttachments(attachmentList);
    }       

    //Delete the pluginloader instance
    delete iDataModelPluginLoader;
    iDataModelPluginLoader = NULL;
    iMmsDataPlugin = NULL;
    
    QDEBUG_WRITE("Exit populateMessageBodyL");
}

// -----------------------------------------------------------------------------
// convertToL
// @see Header
// -----------------------------------------------------------------------------
//
TMsvId CUniEditorMmsPluginPrivate::convertToL(ConvergedMessage *aMessage)
{
    QDEBUG_WRITE("Enter ConvertToL");

    // create message in drafts.
    CMsvEntry* cEntry = MmsMtmL()->Session().GetEntryL(KMsvDraftEntryId);

    CleanupStack::PushL(cEntry);
    MmsMtmL()->SwitchCurrentEntryL(cEntry->EntryId());

    TMsvEntry entry;
    entry.iMtm = MmsMtmL()->Type();
    entry.iType = KUidMsvMessageEntry;
    entry.iServiceId = MmsMtmL()->DefaultServiceL();
    entry.iDate.UniversalTime();

    cEntry->CreateL(entry);

    TMsvId entryId = entry.Id();

    //Since entry is created if any of the below functions leave 
    //the created entry has to be deleted
    TInt error = KErrNone;
    TRAP(error,DoConvertToL(aMessage,entry));
    
    CleanupStack::PopAndDestroy(cEntry);

    if (error != KErrNone)
    {
        this->deleteDraftsEntryL(entryId);
        entryId = -1; //Set Invalid entry id
    }
    QDEBUG_WRITE_FORMAT("Exit ConvertToL the entryId= ",entryId);
    return entryId;
}

// -----------------------------------------------------------------------------
// DoConvertToL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::DoConvertToL(ConvergedMessage *aMessage,
    TMsvEntry& entry)
{
    TMsvId entryId = entry.Id();
 
     // switch to created entry
     MmsMtmL()->SwitchCurrentEntryL(entryId);

     //Add recipients
     addRecipientsL(aMessage);

     //There is no size check inside plugin as it assumes 
     //we get proper data from editor
     entry = MmsMtmL()->Entry().Entry();
     if (iMessageForwarded)
        {
        entry.SetForwarded(ETrue);
        iMessageForwarded = EFalse;
        }
     HBufC* sub = XQConversions::qStringToS60Desc(aMessage->subject());
     if( sub )
     {
         CleanupStack::PushL(sub);
         MmsMtmL()->SetSubjectL( *sub );
         CleanupStack::PopAndDestroy(sub);
     }

     //Add attachments
     // fetch attachment list and populate the smil model
     ConvergedMessageAttachmentList attachmentlist = aMessage->attachments();
     int attachmentcount = attachmentlist.count();
     if ((attachmentcount> 0) || !(aMessage->bodyText().isEmpty()))
     {
        QDEBUG_WRITE("Before calling iUniDataModel->restore");

        User::LeaveIfError(ifsSession.Connect());

        delete iUniDataModel;
        iUniDataModel = NULL;

        iUniDataModel = CUniDataModel::NewL(ifsSession, *MmsMtmL());
        iUniDataModel->RestoreL(*this, ETrue);

        QDEBUG_WRITE("Before calling iUniDataModel->addSlide");

        bool slideContentAdded = false;

        //Adding first slide
        iUniDataModel->SmilModel().AddSlideL(0);

        for (int i = 0; i < attachmentcount; i++)
        {
            if (attachmentlist[i]->attachmentType()
                    == ConvergedMessageAttachment::EInline)
            {
                slideContentAdded = true;
                addObjectL(0, attachmentlist[i]->filePath());

            }
            else if (attachmentlist[i]->attachmentType()
                    == ConvergedMessageAttachment::EAttachment)
            {
                addAttachmentL(attachmentlist[i]->filePath());
            }
        }

        if (! (aMessage->bodyText().isEmpty()))
        {
            slideContentAdded = true;
            addTextObjectL(0, aMessage->bodyText());
        }

        if (slideContentAdded)
        {
            iUniDataModel->ObjectList().SaveAll(*this, CMsvAttachment::EMsvFile);

            // Open store even if we didn't need it here.
            // -> We don't need to check later whether the store 
            //is open or not.    
            CMsvStore* editStore = MmsMtmL()->Entry().EditStoreL();

            //Ensure that the store is deleted if anything 
            //leaves before deletion otherwise the entry store will be locked
            //and entry cant be deleted
            CleanupStack::PushL(editStore);

            delete iDom;
            iDom = NULL;

            iDom = iUniDataModel->SmilModel().ComposeL();
            iUniDataModel->SmilList().CreateSmilAttachmentL(*this,
                                                            *editStore,
                                                            iDom);

             //Commit the store before setting the root
             editStore->CommitL();

             //delete the store before setMessageRoot is called
             CleanupStack::PopAndDestroy(editStore);

            //Set message root after composing SMIL but before 
            //calling SaveMessageL and after the store has been deleted
            MmsMtmL()->SetMessageRootL(iUniDataModel->SmilList().GetSmilAttachmentByIndex(0));
        }
        else
        {
            //Remove the slide   
            iUniDataModel->SmilModel().RemoveSlide(0);

             // Open store even if we didn't need it here.
             // -> We don't need to check later whether the store is open or not.    
             CMsvStore* editStore = MmsMtmL()->Entry().EditStoreL();
             
             //Ensure that the store is deleted if anything 
             //leaves before deletion otherwise the entry store will be locked
             //and entry cant be deleted
             CleanupStack::PushL(editStore);
             
             //Commit the store before setting the root
             editStore->CommitL();

             //delete the store before setMessageRoot is called
             CleanupStack::PopAndDestroy(editStore);
         }
     }

     //Set the priority before calling save
     ConvergedMessage::Priority priority = aMessage->priority();
     if (ConvergedMessage::High == priority)
     {
         MmsMtmL()->SetMessagePriority(EMmsPriorityHigh);
     }
     else if (ConvergedMessage::Low == priority)
     {
         MmsMtmL()->SetMessagePriority(EMmsPriorityLow);
     }
     else if (ConvergedMessage::Normal == priority)
     {
         MmsMtmL()->SetMessagePriority(EMmsPriorityNormal);
     }

     //Saving the changes
     MmsMtmL()->SaveMessageL();

     
     entry = MmsMtmL()->Entry().Entry();
     TBuf<KMaxDetailsLength> detailsBuf;
     MakeDetailsL( detailsBuf );
     entry.iDetails.Set( detailsBuf );

     MmsMtmL()->Entry().ChangeL(entry); // commit changes      

}

// -----------------------------------------------------------------------------
// sendL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::sendL( TMsvId aId )
{
    QDEBUG_WRITE("Enter sendL");

    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* oper = MmsMtmL()->SendL( wait->iStatus );
    CleanupStack::PushL( oper );
    wait->Start();
    CleanupStack::PopAndDestroy( oper );
    CleanupStack::PopAndDestroy( wait );

    QDEBUG_WRITE("Exit sendL");
}

// -----------------------------------------------------------------------------
// validateServiceL
// @see Header
// -----------------------------------------------------------------------------
//
TBool CUniEditorMmsPluginPrivate::validateServiceL( TBool /*aEmailOverSms*/ )
{
    QDEBUG_WRITE("Enter ValidateServiceL");

    //Check if the mms client mtm object is already created or not
    if( iMmsMtm )
    {
        // If mms client mtm object is already created restore the settings
        iMmsMtm->RestoreSettingsL();
    }

    TMsvId service = MmsMtmL()->DefaultServiceL();
    TBool valid( MmsMtmL()->ValidateService( service ) == KErrNone );

    QDEBUG_WRITE_FORMAT("Exit ValidateServiceL the return val= ",valid);
    return valid;
}

// -----------------------------------------------------------------------------
// isServiceValidL
// @see Header
// -----------------------------------------------------------------------------
//
TBool CUniEditorMmsPluginPrivate::isServiceValidL()
{
    return MmsMtmL()->ValidateService( MmsMtmL()->DefaultServiceL() ) == KErrNone;
}

// -----------------------------------------------------------------------------
// MmsMtmL
// @see Header
// -----------------------------------------------------------------------------
//
CMmsClientMtm* CUniEditorMmsPluginPrivate::MmsMtmL()
{
    if ( !iMmsMtm && iSession )
    {
        if ( !iMtmRegistry )
        {            
            iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
        }
        iMmsMtm = static_cast<CMmsClientMtm*>( iMtmRegistry->NewMtmL( 
            KSenduiMtmMmsUid ) );
    }
    return iMmsMtm;
}

// ---------------------------------------------------------
// MakeDetailsL
// @see Header
// ---------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::MakeDetailsL( TDes& aDetails )
{
    // This very same code can be found in CUniAddressHandler. 
    // They should be put in common location some day...
    const CMsvRecipientList& addresses = MmsMtmL()->AddresseeList();
    TInt addrCnt = addresses.Count();

    TPtrC stringToAdd;
    for ( TInt i = 0; i < addrCnt; i++)
    {
        // Only address is converted to western. 
        // There may numbers in contact name - they must not be converted 
        TPtrC alias = TMmsGenUtils::Alias( addresses[i] );
        HBufC* addressBuf = NULL;

        if ( alias.Length() != 0 )
        {
            stringToAdd.Set( alias );
        }
        else
        {
            TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );
            addressBuf = HBufC::NewLC( address.Length() );
            TPtr addressPtr = addressBuf->Des();
            addressPtr.Copy( address );
            stringToAdd.Set( addressPtr );

            // Internal data structures always holds the address data in western format.
            // UI is responsible of doing language specific conversions.    
            //MuiuTextUtils::ConvertDigitsTo( addressPtr, EDigitTypeWestern );
            iGenUtils->ConvertDigitsTo( addressPtr, EDigitTypeWestern );
        }

        if ( ( aDetails.Length() != 0 ) &&   // Not a first address
                ( aDetails.Length() + KAddressSeparator().Length() < KMaxDetailsLength ) )
        {
            // Add separator
            aDetails.Append( KAddressSeparator() );
        }

        if ( aDetails.Length() + stringToAdd.Length() < KMaxDetailsLength ) 
        {
            // whole string fits. Add it.
            aDetails.Append( stringToAdd );
            if ( addressBuf )
            {
                CleanupStack::PopAndDestroy( addressBuf );
            }
        }
        else
        {
            // Only part of the string fits
            TInt charsToAdd = KMaxDetailsLength - aDetails.Length();

            if ( charsToAdd <= 0 )
            {
                // Cannot add any more chars 
                break;
            }

            if ( charsToAdd >= stringToAdd.Length() )
            {
                // Guarantee that charsToAdd is not larger that stringToAdd lenght 
                charsToAdd = stringToAdd.Length();
            }

            aDetails.Append( stringToAdd.Left( charsToAdd ) );
            if ( addressBuf )
            {
                CleanupStack::PopAndDestroy( addressBuf );
            }
            break;
        }
    }
}

//---------------------------------------------------------------
// CUniEditorMmsPluginPrivate::addRecipientsL
// @see header
//---------------------------------------------------------------
void CUniEditorMmsPluginPrivate::addRecipientsL(ConvergedMessage *message)
{
    //add To feilds
    ConvergedMessageAddressList toAddressArray = message->toAddressList();
    addRecipientsL(toAddressArray, EMsvRecipientTo);

    //add cc feilds
    ConvergedMessageAddressList ccAddressArray = message->ccAddressList();
    addRecipientsL(ccAddressArray, EMsvRecipientCc);

    //add bcc feilds
    ConvergedMessageAddressList bccAddressArray = message->bccAddressList();
    addRecipientsL(bccAddressArray, EMsvRecipientBcc);
}

//---------------------------------------------------------------
// CUniEditorMmsPluginPrivate::addRecipientsL
// @see header
//---------------------------------------------------------------
void CUniEditorMmsPluginPrivate::addRecipientsL(
    const ConvergedMessageAddressList &array,
    TMsvRecipientType recpType)
{    

    for (int i = 0; i < array.count(); ++i)
    {
        if(array[i]->address().isEmpty())
        {
            continue;
        }
        // convert from QString to HBufC
        HBufC* addr = XQConversions::qStringToS60Desc(array[i]->address());

        CleanupStack::PushL(addr);

        if(TMmsGenUtils::Alias(*addr).Length() > 0)
        {    
            MmsMtmL()->AddAddresseeL(recpType, TMmsGenUtils::PureAddress(*addr),
                TMmsGenUtils::Alias(*addr));
        }
        else
        {
            HBufC* displayName = XQConversions::qStringToS60Desc(array[i]->alias());
            if(displayName)
            {
                CleanupStack::PushL(displayName);

                MmsMtmL()->AddAddresseeL(recpType, TMmsGenUtils::PureAddress(*addr),
                    *displayName);

                CleanupStack::PopAndDestroy(displayName);
            }
            else
            {
                MmsMtmL()->AddAddresseeL(recpType, TMmsGenUtils::PureAddress(*addr));    
            }
        }
        CleanupStack::PopAndDestroy(addr);
    }
}

// ----------------------------------------------------------------------------
// CUniEditorMmsPluginPrivate::populateRecipientsL
// @see header
// ----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::populateRecipientsL(
    ConvergedMessage &aMessage)
    {
    QDEBUG_WRITE("Enter populateRecipientsL");


    // get recipient list
    const CMsvRecipientList& addresses = MmsMtmL()->AddresseeList();
    TInt count = addresses.Count();

    // extract each address and populate into ConvergedMessageAddress
    for (TInt i = 0; i < count; ++i)
        {
        //Address
        QString address = XQConversions::s60DescToQString(
            TMmsGenUtils::PureAddress(addresses[i]));
        //Alias
        QString alias = XQConversions::s60DescToQString(
            TMmsGenUtils::Alias(addresses[i]));

        //add recipient to convergedMessage
        ConvergedMessageAddress messageAddress(address, alias);
        if (addresses.Type(i) == EMsvRecipientTo)
            {
            aMessage.addToRecipient(messageAddress);
            }
        else if (addresses.Type(i) == EMsvRecipientCc)
            {
            aMessage.addCcRecipient(messageAddress);
            }
        else if (addresses.Type(i) == EMsvRecipientBcc)
            {
            aMessage.addBccRecipient(messageAddress);
            }
        }

    QDEBUG_WRITE("Exit populateRecipientsL");

    }

// ----------------------------------------------------------------------------
// CUniEditorMmsPluginPrivate::deleteDraftsEntryL
// @see header
// ----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::deleteDraftsEntryL( TMsvId aId )
{
    if(iSession)
    {
        CMsvEntry* pEntry = iSession->GetEntryL(KMsvDraftEntryIdValue);
        CleanupStack::PushL(pEntry);
        pEntry->DeleteL( aId );
        CleanupStack::PopAndDestroy(pEntry);    
    }
}


// -----------------------------------------------------------------------------
// addObjectL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::addObjectL(int aSlideNum, const QString& aFilePath)
{

    HBufC* filePath = XQConversions::qStringToS60Desc(aFilePath);

    if (filePath)
    {
        CleanupStack::PushL(filePath);
        RFile file = iUniDataModel->MediaResolver().FileHandleL(*filePath);
        CleanupClosePushL(file);

        iInsertingMedia = iUniDataModel->MediaResolver().CreateMediaInfoL(file);

        iUniDataModel->MediaResolver().ParseInfoDetailsL(iInsertingMedia, file);
        iUniDataModel->SmilModel().AddObjectL(*this, aSlideNum, iInsertingMedia);

        //Since the ownership is transferred dont delete these
        iInsertingMedia = NULL;
        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PopAndDestroy(filePath);
    }

}


// -----------------------------------------------------------------------------
// addAttachmentL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::addAttachmentL(const QString& aFilePath)
{
    HBufC * filePath = XQConversions::qStringToS60Desc(aFilePath);
    if (filePath)
    {
        CleanupStack::PushL(filePath);

        RFile file = iUniDataModel->MediaResolver().FileHandleL(*filePath);
        CleanupClosePushL(file);

        iInsertingMedia = iUniDataModel->MediaResolver().CreateMediaInfoL(file);

        iUniDataModel->MediaResolver().ParseInfoDetailsL(iInsertingMedia, file);
        iUniDataModel->AddAttachmentL(*this,
                                      iInsertingMedia,
                                      CMsvAttachment::EMsvFile);

        //Since the ownership is transferred dont delete these
        iInsertingMedia = NULL;

        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PopAndDestroy(filePath);
    }

}


// -----------------------------------------------------------------------------
// addTextObjectL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::addTextObjectL(int aSlideNum, const QString& aBodyText)
{
    delete iEditor;
    iEditor = NULL;

    HBufC* textContent = XQConversions::qStringToS60Desc(aBodyText);
    if (textContent)
    {
        CleanupStack::PushL(textContent);

        iEditor = new CEikRichTextEditor;
        iEditor->ConstructL(NULL, 0, 0, CEikEdwin::ENoAutoSelection
                | CEikEdwin::EOwnsWindow, 0, 0);
        iEditor->RichText()->InsertL(0, *textContent);

        iUniDataModel->SmilModel().AddTextObjectL(aSlideNum, iEditor);
        CleanupStack::PopAndDestroy(textContent);
    }
}


// -----------------------------------------------------------------------------
// ObjectSaveReady
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::ObjectSaveReady(TInt /*aError*/)
{

}

// -----------------------------------------------------------------------------
// MediaInfoParsed
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::MediaInfoParsed()
{

}

// -----------------------------------------------------------------------------
// SmilComposeEvent
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::SmilComposeEvent(TInt /*aError*/)
{

}

// -----------------------------------------------------------------------------
// RestoreReady
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::RestoreReady(TInt /*aParseResult*/, TInt /*aError*/)
{
}

// -----------------------------------------------------------------------------
// HandleSessionEventL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                                  TAny* /*aArg1*/,
                                                  TAny* /*aArg2*/, TAny* /*aArg3*/)
{
// do nothing
}

// -----------------------------------------------------------------------------
// populateSenderL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::populateSenderL(
        ConvergedMessage& aMessage)
    {
    // get sender address    
    HBufC* fromAddress = (MmsMtmL()->Sender()).AllocLC();
    HBufC* pureAddr = TMmsGenUtils::PureAddress(*fromAddress).AllocLC();
    HBufC* aliasAddr = TMmsGenUtils::Alias(*fromAddress).AllocLC();

    if(pureAddr->Length() > 0)
        {
        ConvergedMessageAddress messageAddress(
                XQConversions::s60DescToQString(*pureAddr),
                XQConversions::s60DescToQString(*aliasAddr));
        aMessage.addToRecipient(messageAddress);
        }
    CleanupStack::PopAndDestroy(3, fromAddress );
    }

// -----------------------------------------------------------------------------
// convertFromReplyHandlerL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::convertFromReplyHandlerL(
        ConvergedMessage* aMessage)
    {
    // for received mms, populate sender address into To-field
    // for sent mms, populate recipients into To-field
    TMsvEntry entry = MmsMtmL()->Entry().Entry();
    if( entry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
        {
        populateSenderL(*aMessage);
        }
    else
        {
        populateRecipientsL(*aMessage);
        }
    
    // resolve contacts
    ConvergedMessageAddressList addrList = aMessage->toAddressList();
    int addrCount = addrList.count();
    for(int i=0; i<addrCount; i++)
        {
        ConvergedMessageAddress* addr = addrList.at(i);
        // resolve contact if alias is empty
        if(addr->alias().isEmpty())
            {
            QString alias;
            int count;
            if(-1 != MsgContactHandler::resolveContactDisplayName(
                            addr->address(), alias, count))
                {
                addr->setAlias(alias);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// convertFromReplyAllHandlerL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::convertFromReplyAllHandlerL(
        ConvergedMessage* aMessage)
    {
    ConvergedMessage* tempmsg = new ConvergedMessage();
    // populate all recipients (and sender for received mms)
    TMsvEntry entry = MmsMtmL()->Entry().Entry();
    if( entry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
        {
        populateSenderL(*tempmsg);
        }
    populateRecipientsL(*tempmsg);
    removeOwnNumberForReplyAll(tempmsg, aMessage);
    delete tempmsg;

    // populate the subject field
    QString subject = XQConversions::s60DescToQString(
            MmsMtmL()->SubjectL());
    if(!subject.startsWith(LOC_RE, Qt::CaseInsensitive))
        {
        subject.insert(0, LOC_RE);
        }
    aMessage->setSubject(subject);
    }

// -----------------------------------------------------------------------------
// convertFromDefaultHandlerL
// @see Header
// -----------------------------------------------------------------------------
void CUniEditorMmsPluginPrivate::convertFromDefaultHandlerL(ConvergedMessage* aMessage)
{
    QDEBUG_WRITE("Enter convertFromDefaultHandlerL");

    TMsvEntry entry = MmsMtmL()->Entry().Entry();
    
    if( entry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
        {
        // get sender address    
        HBufC* fromAddress = (MmsMtmL()->Sender()).AllocLC();
        HBufC* pureAddr = TMmsGenUtils::PureAddress(*fromAddress).AllocLC();
        HBufC* aliasAddr = TMmsGenUtils::Alias(*fromAddress).AllocLC();

        if(pureAddr->Length() > 0)
            {
            ConvergedMessageAddress messageAddress(
                    XQConversions::s60DescToQString(*pureAddr),
                    XQConversions::s60DescToQString(*aliasAddr));
            aMessage->addFromRecipient(messageAddress);
            }
        CleanupStack::PopAndDestroy(3, fromAddress );
        }
    else
        {
        populateRecipientsL(*aMessage);
        }

    //populate convergedmessage with the subject prepended with FW:
    QString subject = XQConversions::s60DescToQString(MmsMtmL()->SubjectL());
    aMessage->setSubject(subject);

    // Priority
    TMsvPriority priority = entry.Priority();
    if( EMsvHighPriority == priority )
    {
        aMessage->setPriority(ConvergedMessage::High);
    }
    else if( EMsvLowPriority == priority )
    {
        aMessage->setPriority(ConvergedMessage::Low);
    }
    else if( EMsvMediumPriority == priority )
    {
        aMessage->setPriority(ConvergedMessage::Normal);
    }

    //Populate body and attachments
    //The region info inside slides is not maintained
    populateMessageBodyL(*aMessage);
    
    QDEBUG_WRITE("Exit convertFromDefaultHandlerL");
}

// -----------------------------------------------------------------------------
// removeOwnNumberForReplyAll
// @see Header
// -----------------------------------------------------------------------------
void CUniEditorMmsPluginPrivate::removeOwnNumberForReplyAll(
        ConvergedMessage* sourcemsg, 
        ConvergedMessage* targetmsg)
{
    // allocate new memory for trimming the address list
    // why do we need it? Because ConvergedMessageAddressList is implicitely
    // shared, and Implicit sharing automatically detaches the object from
    // a shared block if the object is about to change and the reference count
    // is greater than one. This is called copy-on-write or value semantics.
    ConvergedMessageAddressList toAddrList = 
            copyAddrList(sourcemsg->toAddressList());
    ConvergedMessageAddressList ccAddrList = 
            copyAddrList(sourcemsg->ccAddressList());
    ConvergedMessageAddressList bccAddrList = 
            copyAddrList(sourcemsg->bccAddressList());

    // start self-address check/remove process. Stop if there's only one
    // address left in the address-list (to+cc+bcc)
    QStringList selfAddrs = MsgContactHandler::selfAddresses();
    int remainingAddr = 
            toAddrList.count() + ccAddrList.count() + bccAddrList.count();
    bool runOwnAddrCheck =(remainingAddr>1)?true:false;
    if(runOwnAddrCheck)
    {
        foreach(ConvergedMessageAddress *bccAddress,bccAddrList)
        {
            if(isSelfAddress(bccAddress->address(), selfAddrs))
            {
                bccAddrList.removeOne(bccAddress);
                --remainingAddr;
                if(remainingAddr == 1)
                {
                    runOwnAddrCheck = false;
                    break;
                }
            }
        }
    }

    if(runOwnAddrCheck)
    {
        foreach(ConvergedMessageAddress *ccAddress,ccAddrList)
        {
            if(isSelfAddress(ccAddress->address(), selfAddrs))
            {
                ccAddrList.removeOne(ccAddress);
                --remainingAddr;
                if(remainingAddr == 1)
                {
                    runOwnAddrCheck = false;
                    break;
                }
            }
        }
    }

    if(runOwnAddrCheck)
    {
        foreach(ConvergedMessageAddress *toAddress,toAddrList)
        {
            if(isSelfAddress(toAddress->address(), selfAddrs))
            {
                toAddrList.removeOne(toAddress);
                --remainingAddr;
                if(remainingAddr == 1)
                {
                    runOwnAddrCheck = false;
                    break;
                }
            }
        }
    }

    // Run contact-resolution on address list now. This is needed
    // because we by-pass server and directly feed data in UI
    resolveContacts(toAddrList);
    resolveContacts(ccAddrList);
    resolveContacts(bccAddrList);

    // restore addresses to aMessage
    targetmsg->addToRecipients(toAddrList);
    targetmsg->addCcRecipients(ccAddrList);
    targetmsg->addBccRecipients((bccAddrList));
}

// -----------------------------------------------------------------------------
// copyAddrList
// @see Header
// -----------------------------------------------------------------------------
ConvergedMessageAddressList CUniEditorMmsPluginPrivate::copyAddrList(
        ConvergedMessageAddressList addrList)
{
    ConvergedMessageAddressList copyAddrList;
    int count = addrList.count();
    for(int i=0; i<count; i++)
    {
        ConvergedMessageAddress* addr = new ConvergedMessageAddress;
        addr->setAddress(addrList.at(i)->address());
        addr->setAlias(addrList.at(i)->alias());
        copyAddrList << addr;
    }
    return copyAddrList;
}

// -----------------------------------------------------------------------------
// resolveContacts
// @see Header
// -----------------------------------------------------------------------------
void CUniEditorMmsPluginPrivate::resolveContacts(
        ConvergedMessageAddressList addrList)
{
    int count = addrList.count();
    for(int i=0; i<count; i++)
    {
        ConvergedMessageAddress* addr = addrList.at(i);
        // resolve contact if alias is empty
        if(addr->alias().isEmpty())
        {
            QString alias;
            int count;
            if(-1 != MsgContactHandler::resolveContactDisplayName(
                            addr->address(), alias, count))
            {
                addr->setAlias(alias);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// isSelfAddress
// @see Header
// -----------------------------------------------------------------------------
bool CUniEditorMmsPluginPrivate::isSelfAddress(
        QString address,
        QStringList selfAddrList)
{
    bool ifSelfAddr = false;
    
    TRAP_IGNORE(
    // check if the given address is an email-address
    HBufC* addr = XQConversions::qStringToS60Desc(address);
    CleanupStack::PushL(addr);

    if( iGenUtils->IsValidEmailAddress(*addr) )
    {
        ifSelfAddr = selfAddrList.contains(address, Qt::CaseInsensitive);
    }
    else // address is a phonenumber, use contact matching
    {
        foreach(QString selfaddress, selfAddrList)
        {
            bool ret = false;
            HBufC* selfAddr = XQConversions::qStringToS60Desc(selfaddress);
            CleanupStack::PushL(selfAddr);
            ret = iGenUtils->MatchPhoneNumberL(*addr,*selfAddr);
            CleanupStack::PopAndDestroy(selfAddr);
            if(ret)
            {
                ifSelfAddr = true;
                break;
            }
        }
    }
    CleanupStack::PopAndDestroy(addr);
    );
    return ifSelfAddr;
}

//  End of File
