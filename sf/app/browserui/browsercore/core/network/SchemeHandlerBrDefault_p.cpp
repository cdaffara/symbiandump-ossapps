/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

// INCLUDE FILES
#include <QUrl>
#include <QDesktopServices>
#include <QStringList>

#if defined(Q_OS_SYMBIAN)
#include "WrtTelServiceBr.h"
#include <miutset.h>                // KUidMsgTypeSMTP
#include <sendui.h>                 // CSendUi
#include <cmessagedata.h>           // CMessageData
//For Rtsp Scheme
#include <aiwgenericparam.h> 
#include <DocumentHandler.h>
#include <apparc.h>

_LIT( KRtspFileName, "c:\\system\\temp\\RtspTemp.ram" );
#endif // defined(Q_OS_SYMBIAN)

#include "SchemeHandlerBr_p.h"
#include "SchemeHandlerBrDefault_p.h"

#define MAILTO_SCHEME "mailto"
#define TEL_SCHEME "tel"
#define RTSP_SCHEME "rtsp"
#define SIP_SCHEME "sip"
#define WTAI_SCHEME "wtai"
#define WTAI_MC "/mc;"
#define WTAI_AP "/ap;"

namespace WRT {

DefSchemeHandlerPrivate::DefSchemeHandlerPrivate()
#if defined(Q_OS_SYMBIAN)
    : m_telService(NULL)
#endif
{
}

DefSchemeHandlerPrivate::~DefSchemeHandlerPrivate()
{
#if defined(Q_OS_SYMBIAN) 
    delete m_telService;
#endif
}

/*!
 * HandleUrl
 * @return true/false
 */
SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleUrl(const QUrl &url, bool confirmDTMF)
{
#if defined(Q_OS_SYMBIAN) 
    if (!m_telService)
        m_telService = new WrtTelService();
#endif

    return SchemeHandlerPrivate::HandleUrl(url, confirmDTMF);
}

SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleScheme(const QUrl &url)
{
    QString scheme = url.scheme().toLower();
    SchemeHandler::SchemeHandlerError retVal = SchemeHandler::SchemeUnsupported;

    if (scheme == MAILTO_SCHEME) {
        // launch associated program
        retVal = HandleMailtoScheme(url);
    }
    else if (scheme == RTSP_SCHEME) {
        // launch associated program
        retVal = HandleRtspScheme(url);
    }

#if defined(Q_OS_SYMBIAN) 
    else if (scheme == TEL_SCHEME) {
        // launch associated program
        retVal = HandleTelScheme(url);
    }
    else if (scheme == SIP_SCHEME) {
        // launch associated program
        retVal = HandleSipScheme(url);
    }
    else if (scheme == WTAI_SCHEME) {
        QString path = url.path();
        
        if (path.startsWith(WTAI_MC)) {
            // launch associated program
            retVal = HandleWtaiMcScheme(url);
        }
    }
#endif

    return retVal;
}


#if !defined(Q_OS_SYMBIAN) 
/*
 * HandleMailtoScheme
 * @return  true/false
 */
SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleMailtoScheme(const QUrl &url)
{
    SchemeHandler::SchemeHandlerError retVal = SchemeHandler::NoError;
#ifndef QT_NO_DESKTOPSERVICES
    if (!QDesktopServices::openUrl(url)) {
        // Failed to handle scheme
        retVal = SchemeHandler::LaunchFailed;
    }
#else
    retVal = SchemeHandler::SchemeUnsupported;
#endif

    return (retVal);
}
#else // Q_OS_SYMBIAN
/*
 * HandleMailtoScheme
 * @return  true/false
 */

SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleMailtoScheme(const QUrl &url)
{
    TRAPD(err, HandleMailtoSchemeL(url));
    return err ? SchemeHandler::LaunchFailed : SchemeHandler::NoError;
}
void DefSchemeHandlerPrivate::HandleMailtoSchemeL(const QUrl &url)
{
    QString recipient = url.path();
    QString subject = url.queryItemValue("subject");
    QString body = url.queryItemValue("body");
    QString to = url.queryItemValue("to");
    QString cc = url.queryItemValue("cc");
    QString bcc = url.queryItemValue("bcc");

    // these fields might have comma separated addresses
    QStringList recipients = recipient.split(",");
    QStringList tos = to.split(",");
    QStringList ccs = cc.split(",");
    QStringList bccs = bcc.split(",");


    CSendUi* sendUi = CSendUi::NewLC();

    // Construct symbian sendUI data holder
    CMessageData* messageData = CMessageData::NewLC();

    // Subject
    TPtrC subj( qt_QString2TPtrC(subject) );
    messageData->SetSubjectL( &subj );

    // Body
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* bodyRichText = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( bodyRichText );

    TPtrC bodyPtr( qt_QString2TPtrC(body) );
    if( bodyPtr.Length() )
        bodyRichText->InsertL( 0, bodyPtr );
    else
        bodyRichText->InsertL( 0, KNullDesC );

    messageData->SetBodyTextL( bodyRichText );

    // To
    foreach(QString item, recipients)
        messageData->AppendToAddressL(qt_QString2TPtrC(item));

    foreach(QString item, tos)
        messageData->AppendToAddressL(qt_QString2TPtrC(item));

    // Cc
    foreach(QString item, ccs)
        messageData->AppendCcAddressL(qt_QString2TPtrC(item));

    // Bcc
    foreach(QString item, bccs)
        messageData->AppendBccAddressL(qt_QString2TPtrC(item));

    TRAP_IGNORE(sendUi->CreateAndSendMessageL( KUidMsgTypeSMTP, messageData ));
    CleanupStack::PopAndDestroy( 5 ); // bodyRichText, charFormat, paraFormat, messageData, sendUi
}

// Returned TPtrC is valid as long as the given parameter is valid and unmodified
TPtrC DefSchemeHandlerPrivate::qt_QString2TPtrC( const QString& string )
{
    return reinterpret_cast<const TUint16*>(string.utf16());
}
#endif // Q_OS_SYMBIAN

#if !defined(Q_OS_SYMBIAN) 
/*
 * HandleRtspScheme
 * @return  true/false
 */
SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleRtspScheme(const QUrl &url)
{
    SchemeHandler::SchemeHandlerError retVal = SchemeHandler::NoError;

#ifndef QT_NO_DESKTOPSERVICES
    if (!QDesktopServices::openUrl(url)) {
        // Failed to handle scheme
        retVal = SchemeHandler::LaunchFailed;
    }
#else
    retVal = SchemeHandler::SchemeUnsupported;
#endif

    return (retVal);
}
#else // Q_OS_SYMBIAN

SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleTelScheme(const QUrl &url)
{
    //QString path = url.path();
    
    // should probably delete this call to m_telService method
    //m_telService->MakeCall(path, confirmDTMF);
    QDesktopServices::openUrl(url);
    return SchemeHandler::NoError;
}

SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleSipScheme(const QUrl &url)
{
    // should probably delete this call to m_telService method
    //m_telService->MakeVOIPCall(path, confirmDTMF);
    QDesktopServices::openUrl(url);
    return SchemeHandler::NoError;
}

SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleWtaiMcScheme(const QUrl &url)
{
    QString path = url.path();
    QString number = path.mid(path.indexOf(';') + 1);
    
    // should probably delete this call to m_telService method
    //m_telService->MakeCall(number, confirmDTMF);
    number = "tel:" + number;
    QUrl dialUrl(number);
    QDesktopServices::openUrl(dialUrl);
    return SchemeHandler::NoError;
}

/*
 * HandleRtspSchemeL
 * @return  true/false
 */

SchemeHandler::SchemeHandlerError DefSchemeHandlerPrivate::HandleRtspScheme(const QUrl &url)
{
    TRAPD(err, HandleRtspSchemeL(url));
    return (err ? SchemeHandler::LaunchFailed : SchemeHandler::NoError);
}
void DefSchemeHandlerPrivate::HandleRtspSchemeL(const QUrl &url)
{
    //Launch the appropriate application in embedded mode 

    RFs rfs;
    RFile ramFile;

    HBufC* urlBuf = qt_QString2HBufC(url.toString());
    CleanupStack::PushL(urlBuf);
    // 8-bit buffer is required.
    HBufC8* urlBuf8 = HBufC8::NewLC( urlBuf->Size() );
    urlBuf8->Des().Copy( *urlBuf );
    
    // Open the file.
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    // Replace file if exists or Create file if not exist yet
    User::LeaveIfError( ramFile.Replace( rfs, KRtspFileName, EFileWrite | EFileShareAny ) );
    CleanupClosePushL(ramFile);
    // Write to file      
    User::LeaveIfError( ramFile.Write(*urlBuf8) );
    ramFile.Flush();

    CleanupStack::PopAndDestroy(/*ramFile*/);
    CleanupStack::PopAndDestroy(/*rfs*/);
    CleanupStack::PopAndDestroy( /*urlBuf8*/ );
    CleanupStack::PopAndDestroy( /*urlBuf*/ );
    
    RFile ramDocTempFile;
    CDocumentHandler* docHandler = CDocumentHandler::NewL();
    docHandler->OpenTempFileL( KRtspFileName, ramDocTempFile );

    CleanupClosePushL( ramDocTempFile );

    TDataType dataType( _L8("audio/x-pn-realaudio-plugin") );
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
    TAiwVariant filename( _L8("c:\\system\\temp\\RtspTemp.ram") );
    TAiwGenericParam param( EGenericParamFile, filename );
    
    paramList->AppendL( param );    

    // Allow save among Options

    TBool allowSave( ETrue );
    TAiwVariant allowSaveVariant( allowSave );
    TAiwGenericParam genericParamAllowSave
                     ( EGenericParamAllowSave, allowSaveVariant );

    paramList->AppendL( genericParamAllowSave );

    // launch RAM file via DocHandler

    docHandler->OpenFileEmbeddedL( ramDocTempFile, dataType, *paramList );

    CleanupStack::PopAndDestroy( paramList );
    CleanupStack::PopAndDestroy( &ramDocTempFile );
}

HBufC* DefSchemeHandlerPrivate::qt_QString2HBufC(const QString& string) {
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    return str.Alloc();
}
#endif // Q_OS_SYMBIAN


} // WRT
