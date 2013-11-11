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
 * Description: Implementation of native mms message's utility.
 * Provides utility functions for extracting data from SMIL
 * and/or Attachment manager.
 */
//SYSTEM INCLUDES
#include <coemain.h>  //for CCoeEnv
#include <f32file.h>  //for FileSession RFs
#include <MSVSTORE.H>
#include <MMsvAttachmentManager.h>
#include <CMsvMimeHeaders.h>
#include <QString>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include "debugtraces.h"

// USER INCLUDES
#include "conversationsengineutility.h"
#include "smilxmlreader.h"
#include "smilliterals.h"
#include "msgmimetypes.h"
#include <xqconversions.h>

// CONSTANTS
#define KForwardSlash '/'
#define KBackwardSlash '\\'
_LIT( KContentIdString, "cid:*" );
// 8 bit angle bracket
_LIT8 ( KCidLeftAngle, "<");
// 8 bit angle bracket
_LIT8 ( KCidRightAngle, ">");

//---------------------------------------------------------------
// CNativeMmsUtility::CNativeMmsUtility
// @see header
//---------------------------------------------------------------
CNativeMmsUtility::CNativeMmsUtility()
{
}

//---------------------------------------------------------------
// CNativeMmsUtility::ConstructL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::ConstructL(const TMsvId& msgId, CMsvSession& session)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::ConstructL";
#endif

    //Get message entry
    imsventry = CMsvEntry::NewL(session, msgId, TMsvSelectionOrdering());
    CleanupStack::PushL(imsventry);

    //Read message store
    CMsvStore* store = imsventry->ReadStoreL();
    CleanupStack::PushL(store);

    //Get message store's attachment manager
    iattachmanager = &store->AttachmentManagerL();

    //get the smil file from attachment manager
    ismilfilepath = getSmilFileL();
    if (!ismilfilepath.isEmpty())
    {
        //get smilfile's handle to parse
        RFile smilfile;
        HBufC* path = XQConversions::qStringToS60Desc(ismilfilepath);

        RFs fsSession;
        User::LeaveIfError(fsSession.Connect());

        if (KErrNone == smilfile.Open(fsSession, *path, EFileRead))
        {
            CleanupClosePushL(smilfile);
            //create an instance of smil reader
            ireader = SmilXmlReader::NewL(smilfile);
            //close smilfile
            CleanupStack::PopAndDestroy();
        }
        else
        {
            //reset the smilfilepath
            ismilfilepath.clear();
        }

        smilfile.Close();
        //close file server session
        fsSession.Close();        
    }

    CleanupStack::Pop(2, imsventry);
#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::ConstructL";
#endif

}

//---------------------------------------------------------------
// CNativeMmsUtility::NewL
// @see header
//---------------------------------------------------------------
CNativeMmsUtility* CNativeMmsUtility::NewL(const TMsvId& msgId,
                                           CMsvSession& session)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::NewL";
#endif

    CNativeMmsUtility* self = CNativeMmsUtility::NewLC(msgId, session);
    CleanupStack::Pop(self);

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::NewL";
#endif

    return self;
}

//---------------------------------------------------------------
// CNativeMmsUtility::NewLC
// @see header
//---------------------------------------------------------------
CNativeMmsUtility* CNativeMmsUtility::NewLC(const TMsvId& msgId,
                                            CMsvSession& session)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::NewLC";
#endif

    CNativeMmsUtility* self = new (ELeave) CNativeMmsUtility();
    CleanupStack::PushL(self);

    self->ConstructL(msgId, session);

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::NewLC";
#endif

    return self;
}

//---------------------------------------------------------------
// CNativeMmsUtility::~CNativeMmsUtility
// @see header
//---------------------------------------------------------------
CNativeMmsUtility::~CNativeMmsUtility()
{
    if(ireader)
        {
        delete ireader;
        }   
}

//---------------------------------------------------------------
// CNativeMmsUtility::getAttachmentListL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getAttachmentListL(
                                           ConvergedMessageAttachmentList& attachmentlist)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getAttachmentListL";
#endif

    if (!ismilfilepath.isEmpty())
    {
        getAttachmentListFromSmilL(attachmentlist);
        getRemainingAttachmentsFromAttManL(attachmentlist);
    }
    else
    {
        getAttachmentListFromAttManL(attachmentlist);
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getAttachmentListL";
#endif

    return;
}

//---------------------------------------------------------------
// CNativeMmsUtility::getBodyTextL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getBodyTextL(QString& returnbuf)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getBodyTextL";
#endif

    if (!ismilfilepath.isEmpty())
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::getBodyTextL";
#endif

        return getBodyTextFromSmilL(returnbuf);
    }
    else
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::getBodyTextL";
#endif

        return getBodyTextFromAttManL(returnbuf);
    }
}

//---------------------------------------------------------------
// CNativeMmsUtility::getSmilFileL
// @see header
//---------------------------------------------------------------
const QString CNativeMmsUtility::getSmilFileL()
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getSmilFileL";
#endif

    TUint count = iattachmanager->AttachmentCount();
    for (int i = 0; i < count; i++)
    {
        CMsvAttachment *attachment = iattachmanager->GetAttachmentInfoL(i);
        if (attachment->MimeType().CompareF(KMsgMimeSmil) == 0)
        {
#ifdef _DEBUG_TRACES_
            qDebug() << " Exit CNativeMmsUtility::getSmilFileL";
#endif

            return XQConversions::s60DescToQString(attachment->FilePath());
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getSmilFileL";
#endif

    return QString();
}

//---------------------------------------------------------------
// CNativeMmsUtility::getBodyTextFromSmilL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getBodyTextFromSmilL(QString& returnbuf)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getBodyTextFromSmilL";
#endif

    TPtrC attrvalptr;

    //reset the reader to the root of the DOM tree
    ireader->reset();

    //begin traversal
    while (!ireader->atEndL())
    {
        CMDXMLNode* node = ireader->nextNodeL();

        if (node->NodeType() == CMDXMLNode::EElementNode
                && node->NodeName().CompareF(KTextTag) == 0)
        {
            CMDXMLElement* el = static_cast<CMDXMLElement*> (node);
            if (KErrNone == el->GetAttribute(KSrcAttr, attrvalptr))
            {
                QString filepath = getByUrlL(attrvalptr);
                if (filepath.isEmpty())
                {
                    //get smil's containing dir
                    //because media source is relative to it.
                    QString smildirpath = fileDir(ismilfilepath);
                    QString mfilename =
                            XQConversions::s60DescToQString(attrvalptr);
                    filepath = smildirpath.append(mfilename);
                }
                readFileIntoBuffer(filepath, returnbuf);
                break;
            }
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getBodyTextFromSmilL";
#endif

    return;
}

//---------------------------------------------------------------
// CNativeMmsUtility::getBodyTextFromAttManL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getBodyTextFromAttManL(QString& returnbuf)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getBodyTextFromAttManL";
#endif

    TUint count = iattachmanager->AttachmentCount();
    for (int i = 0; i < count; i++)
    {
        CMsvAttachment *attachment = iattachmanager->GetAttachmentInfoL(i);
        TPtrC8 mimetype = attachment->MimeType();

        if (!isAttachment(attachment) && mimetype.CompareF(KMsgMimeTextPlain)
                == 0)
        {
            QString filepath =
                    XQConversions::s60DescToQString(attachment->FilePath());
            readFileIntoBuffer(filepath, returnbuf);
            break;
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getBodyTextFromAttManL";
#endif

    return;
}

//---------------------------------------------------------------
// CNativeMmsUtility::getAttachmentListFromSmilL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getAttachmentListFromSmilL(
                                                   ConvergedMessageAttachmentList& attachmentlist)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getAttachmentListFromSmilL";
#endif

    //reset the reader to the root of the DOM tree
    ireader->reset();

    //begin traversal
    while (!ireader->atEndL())
    {
        CMDXMLNode* node = ireader->nextNodeL();

        if (node->NodeType() == CMDXMLNode::EElementNode
                && (node->NodeName().CompareF(KImageTag) == 0
                        || node->NodeName().CompareF(KAudioTag) == 0
                        || node->NodeName().CompareF(KVideoTag) == 0
                        || node->NodeName().CompareF(KTextTag) == 0))
        {
            CMDXMLElement* el = static_cast<CMDXMLElement*> (node);
            TPtrC attrvalptr;
            if (KErrNone == el->GetAttribute(KSrcAttr, attrvalptr))
            {
                //resolve content-id
                QString filepath = getByUrlL(attrvalptr);
                if (filepath.isEmpty())
                {
                    //get smil's containing dir
                    //because media source is relative to it.
                    QString smildirpath = fileDir(ismilfilepath);
                    QString mfilename =
                            XQConversions::s60DescToQString(attrvalptr);
                    filepath = smildirpath.append(mfilename);
                }
                // converting directory separators from native to
                // universal (QT) format
                filepath.replace(KBackwardSlash, KForwardSlash);
                ConvergedMessageAttachment
                        * att =
                                new ConvergedMessageAttachment(filepath,
                                                               ConvergedMessageAttachment::EInline);
                attachmentlist << att;
            }
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getAttachmentListFromSmilL";
#endif

}

//---------------------------------------------------------------
// CNativeMmsUtility::getRemainingAttachmentsFromAttManL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getRemainingAttachmentsFromAttManL(
                                                           ConvergedMessageAttachmentList& attachmentlist)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getRemainingAttachmentsFromAttManL";
#endif

    TUint count = iattachmanager->AttachmentCount();
    for (int i = 0; i < count; i++)
    {
        CMsvAttachment *attachment = iattachmanager->GetAttachmentInfoL(i);
        TPtrC8 mimetype = attachment->MimeType();
        QString filepath =
                XQConversions::s60DescToQString(attachment->FilePath());
        // converting directory separators from native to
        // universal (QT) format
        filepath.replace(KBackwardSlash, KForwardSlash);

        //create convergedmessage attachment to match with inline objects
        ConvergedMessageAttachment
                compareatt(filepath, ConvergedMessageAttachment::EInline);

        //before adding to the list check if already present in the list
        TBool found = EFalse;
        TUint listcount = attachmentlist.count();
        for (int j = 0; j < listcount && !found; j++)
        {
            if (compareatt == *attachmentlist.at(j))
            {
                found = ETrue;
            }
        }

        //no match found in the list
        if (!found)
        {
            //smil files aren't of either type
            if (mimetype.CompareF(KMsgMimeSmil) == 0)
            {
                ConvergedMessageAttachment
                        * targetatt =
                                new ConvergedMessageAttachment(filepath,
                                                               ConvergedMessageAttachment::ESmil);
                attachmentlist << targetatt;
            }
            else
            {
                ConvergedMessageAttachment
                        * targetatt =
                                new ConvergedMessageAttachment(filepath,
                                                               ConvergedMessageAttachment::EAttachment);
                attachmentlist << targetatt;
            }
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getRemainingAttachmentsFromAttManL";
#endif

    return;
}

//---------------------------------------------------------------
// CNativeMmsUtility::getAttachmentListFromAttManL
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::getAttachmentListFromAttManL(
                                                     ConvergedMessageAttachmentList& attachmentlist)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getAttachmentListFromAttManL";
#endif

    TUint count = iattachmanager->AttachmentCount();
    for (int i = 0; i < count; i++)
    {
        CMsvAttachment *attachment = iattachmanager->GetAttachmentInfoL(i);
        TPtrC8 mimetype = attachment->MimeType();
        QString filepath =
                XQConversions::s60DescToQString(attachment->FilePath());
        // converting directory separators from native to
        // universal (QT) format
        filepath.replace(KBackwardSlash, KForwardSlash);
        quint16 attachmenttype;
        if (isAttachment(attachment))
            attachmenttype = ConvergedMessageAttachment::EAttachment;
        else
            attachmenttype = ConvergedMessageAttachment::EUnknown;

        //smil files aren't of either type
        if (mimetype.CompareF(KMsgMimeSmil) == 0)
        {
            ConvergedMessageAttachment
                    * targetatt =
                            new ConvergedMessageAttachment(filepath,
                                                           ConvergedMessageAttachment::ESmil);
            attachmentlist << targetatt;
        }
        else
        {
            ConvergedMessageAttachment* targetatt =
                    new ConvergedMessageAttachment(filepath, attachmenttype);
            attachmentlist << targetatt;
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::getAttachmentListFromAttManL";
#endif

}

//---------------------------------------------------------------
// CNativeMmsUtility::getByUrlL
// @see header
//---------------------------------------------------------------
const QString CNativeMmsUtility::getByUrlL(const TDesC& url)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::getByUrlL";
#endif

    CMsvAttachment* targetattachment = NULL;
    TBool found = EFalse;

    //newlc puts it on cleanupstack
    HBufC8* url8bit = HBufC8::NewLC(url.Length());

    CUri16* decodedUri = NULL;
    TUriParser8 parser;

    //get the uri in display format
    if (url.MatchF(KContentIdString) == 0)
    {
        //Remove "cid:" from the beginning
        url8bit->Des().Copy(url.Right(url.Length()
                - KContentIdString().Length() + 1));
        parser.Parse(*url8bit);
    }
    else
    {
        url8bit->Des().Copy(url);
        parser.Parse(*url8bit);
    }
    decodedUri = UriUtils::ConvertToDisplayFormL(parser);
    CleanupStack::PushL(decodedUri);

    //run through the attachements to check for a match
    TUint count = iattachmanager->AttachmentCount();
    for (int i = 0; i < count && !found; i++)
    {
        CMsvAttachment *attachment = iattachmanager->GetAttachmentInfoL(i);
        CleanupStack::PushL(attachment);

        //restore mimeheaders from the attachment
        CMsvMimeHeaders* mimeheaders = CMsvMimeHeaders::NewL();
        CleanupStack::PushL(mimeheaders);
        mimeheaders->RestoreL(*attachment);

        //check for a match with content-loc and then content-id
        if (resolvedByContentLocL(mimeheaders->ContentLocation(), *decodedUri)
                || resolvedByContentIdL(mimeheaders->ContentId(), *decodedUri))
        {
            targetattachment = CMsvAttachment::NewL(*attachment);
            found = ETrue;
        }

        CleanupStack::PopAndDestroy(2, attachment); //mimeheaders, attachment
    }

    CleanupStack::PopAndDestroy(decodedUri);
    if (url8bit)
        CleanupStack::PopAndDestroy(url8bit);

    if (targetattachment)
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::getByUrlL";
#endif

        return XQConversions::s60DescToQString(targetattachment->FilePath());
    }
    else
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::getByUrlL";
#endif

        return QString();
    }
}

//---------------------------------------------------------------
// CNativeMmsUtility::fileDir
// @see header
//---------------------------------------------------------------
QString CNativeMmsUtility::fileDir(const QString& filepath)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::fileDir";
#endif

    if (filepath.lastIndexOf(KForwardSlash) != -1) //-1 means not found
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::fileDir";
#endif

        return filepath.left(filepath.lastIndexOf(KForwardSlash) + 1);
    }
    else
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::fileDir";
#endif

        return filepath.left(filepath.lastIndexOf(KBackwardSlash) + 1);
    }
}

//---------------------------------------------------------------
// CNativeMmsUtility::isAttachment
// @see header
//---------------------------------------------------------------
TBool CNativeMmsUtility::isAttachment(CMsvAttachment* attachment)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::isAttachment";
#endif

    TBool retval = EFalse;
    TInt err = attachment->GetIntAttribute(KUidAttachmentIndication, retval);
    if (err == KErrNone)
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::isAttachment";
#endif
        return retval;
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::isAttachment";
#endif

    return EFalse;
}

//---------------------------------------------------------------
// CNativeMmsUtility::readFileIntoBuffer
// @see header
//---------------------------------------------------------------
void CNativeMmsUtility::readFileIntoBuffer(const QString& filepath,
                                           QString& returnbuf)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::readFileIntoBuffer";
#endif

    QFile textfile(filepath);
    if (textfile.open(QIODevice::ReadOnly))
    {
        while (!textfile.atEnd())
        {
            QByteArray line = textfile.readLine();
            returnbuf.append(line);
        }
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::readFileIntoBuffer";
#endif

    return;
}

//---------------------------------------------------------------
// CNativeMmsUtility::resolvedByContentLocL
// @see header
//---------------------------------------------------------------
TBool CNativeMmsUtility::resolvedByContentLocL(const TPtrC& contentlocptr,
                                               const CUri16& targeturi)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::resolvedByContentLocL";
#endif

    TBool result = EFalse;
    TUriParser8 parser;
    CUri16* contLocUri = NULL;
    HBufC8* contentLoc = NULL;

    //get content-location uri
    if (contentlocptr.Length())
    {
        //Copy string to 8-bit descriptor
        contentLoc = HBufC8::NewLC(contentlocptr.Length());
        contentLoc->Des().Copy(contentlocptr);
        parser.Parse(*contentLoc);
        contLocUri = UriUtils::ConvertToDisplayFormL(parser);
        CleanupStack::PushL(contLocUri);
    }

    //compare content-loc against the target uri
    if ( (contentlocptr.Length())
            && (targeturi.Uri().UriDes().CompareF(contLocUri->Uri().UriDes())
                    == 0))
    {
#ifdef _DEBUG_TRACES_
        qDebug() << " Exit CNativeMmsUtility::resolvedByContentLocL";
#endif

        result = ETrue;
    }

    //do some cleanup
    if (contentLoc)
    {
        CleanupStack::PopAndDestroy(2, contentLoc); // contentLoc, contLocUri
    }

#ifdef _DEBUG_TRACES_
    qDebug() << " Exit CNativeMmsUtility::resolvedByContentLocL";
#endif

    return result;
}

//---------------------------------------------------------------
// CNativeMmsUtility::resolvedByContentIdL
// @see header
//---------------------------------------------------------------
TBool CNativeMmsUtility::resolvedByContentIdL(const TPtrC8& contentidptr,
                                              const CUri16& targeturi)
{
#ifdef _DEBUG_TRACES_
    qDebug() << " Enter CNativeMmsUtility::resolvedByContentIdL";
#endif

    TBool result = EFalse;
    TUriParser8 parser;
    CUri16* contIdUri = NULL;
    HBufC8* contentId = NULL;

    //get content-id uri
    if (contentidptr.Length())
    {
        //Copy string to 8-bit descriptor
        contentId = HBufC8::NewLC(contentidptr.Length());

        if (contentId->Find(KCidLeftAngle) == 0
                && contentId->Find(KCidRightAngle) == contentId->Length() - 1)
        {
            // When comparing against cid, remove "<" and ">"
            contentId->Des().Copy(contentidptr.Mid(1, contentidptr.Length() - 2));
        }
        else
        {
            contentId->Des().Copy(contentidptr);
        }

        parser.Parse(*contentId);
        contIdUri = UriUtils::ConvertToDisplayFormL(parser);
        CleanupStack::PushL(contIdUri);
    }

    //compare content-id against the target uri
    if ( (contentidptr.Length())
            && (targeturi.Uri().UriDes().CompareF(contIdUri->Uri().UriDes())
                    == 0))
    {
#ifdef _DEBUG_TRACES_
		qDebug() << " Exit CNativeMmsUtility::resolvedByContentIdL";
#endif

        result = ETrue;
    }

    //do some cleanup
    if (contentId)
    {
        CleanupStack::PopAndDestroy(2, contentId); // contentId, contIdUri
    }

#ifdef _DEBUG_TRACES_
		qDebug() << " Exit CNativeMmsUtility::resolvedByContentIdL";
#endif

    return result;
}

//Note: this code is kept separate for easy cleanup, as CNativeMmsUtility class 
//needs to be removed and unidata model to be used instead.

//Utility class for conersationengine
#include "convergedmessage.h"
//---------------------------------------------------------------
// ConversationsEngineUtility::messageType
// @see header
//---------------------------------------------------------------
int ConversationsEngineUtility::messageType(TCsType value)
    {
    int mMessageType;
    switch(value)
        {
        case ECsSMS:
            mMessageType = ConvergedMessage::Sms;
            break;
        case ECsMMS:
            mMessageType = ConvergedMessage::Mms;
            break;
        case ECsBioMsg:
            mMessageType = ConvergedMessage::BioMsg;
            break;
        case ECsBlueTooth:
            mMessageType = ConvergedMessage::BT;
            break;
        default:
            mMessageType = ConvergedMessage::Sms;
            break;        
        }
    return mMessageType;
    }


// End of file
