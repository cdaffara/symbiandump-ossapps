/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Utility class for fetching email.
*
*/

#include "qtemailfetcher.h"
#include <csearchdocument.h>
#include <nmapiemailaddress.h>
#include <nmapimessagebody.h>
#include <nmapimailbox.h>
#include <QTimer>
#include <QDebug>
#include <qdatetime.h>

//Symbian specific details; picked up from cemailplugin.cpp. 
//Refactor it to cpixmaindefs.h
_LIT(KMsgBaseAppClassGeneric, "root msg email");

_LIT(KMsgSubject, "Subject");
_LIT(KMsgSender, "Sender");
_LIT(KMsgRecipients, "Recipients");
_LIT(KMsgBody, "Body");
_LIT(KMailBoxId, "MailBoxId");
_LIT(KFolderId, "FolderId");
_LIT(KHasAttachment, "HasAttachment");
_LIT(KMailBoxName, "MailBoxName");
//_LIT(KFolderName, "FolderName");
_LIT(KMimeTypeField, CPIX_MIMETYPE_FIELD);
_LIT(KMimeTypeMsg, "Messages");
_LIT(KAttachment, "Attachment");
_LIT(KSentTime, "SentTime");

//------------------------------------------------------------------------------
QEmailFetcher::QEmailFetcher( MEmailItemObserver& aObserver )
    :iEmailObserver( aObserver ), 
     iEmailEventNotifier( NULL ),
     iEmailService( NULL ),
     iMailBoxListings( NULL ), 
     iMailFolderList( NULL ),
     iEnvelopeListing( NULL ),
     iCurrentMailboxIndex( 0 ), 
     iCurrentFolderIndex( 0 )
    {
    qDebug() << "QEmailFetcher::QEmailFetcher";
    }

//------------------------------------------------------------------------------
QEmailFetcher::~QEmailFetcher()
    {
    if ( iEmailEventNotifier )
       iEmailEventNotifier->cancel();
    iMailBoxes.clear();
    iFolders.clear();
    delete iEmailEventNotifier;
    delete iEmailService;
    delete iEnvelopeListing;
    delete iMailFolderList;
    delete iMailBoxListings;    
    }

//------------------------------------------------------------------------------
QEmailFetcher* QEmailFetcher::newInstance( MEmailItemObserver& aObserver ){
    QEmailFetcher* emailFetcher = NULL;
    qDebug() << "QEmailFetcher::newInstance :START";
    //Leak free init.
    try{
        emailFetcher = new QEmailFetcher( aObserver );
        emailFetcher->iEmailService = new NmApiEmailService( emailFetcher );
        emailFetcher->iEmailEventNotifier =  new NmApiEventNotifier( emailFetcher );
        emailFetcher->iMailBoxListings = new NmApiMailboxListing( emailFetcher );
        emailFetcher->initialize( ); //Do the rest of the init.
    }catch(...){ //cleanup.
    qDebug() << "QEmailFetcher::newInstance ( Catch Block)";
        delete emailFetcher; 
        delete emailFetcher->iEmailService;
        delete emailFetcher->iEmailEventNotifier;
        delete emailFetcher->iMailBoxListings;
        emailFetcher->iEmailService = NULL;
        emailFetcher->iEmailEventNotifier = NULL;
        emailFetcher->iMailBoxListings = NULL;
        throw; //rethrow the exception to caller.
    }    
    qDebug() << "QEmailFetcher::newInstance :END";
    return emailFetcher; //returns only if not null.
}

//------------------------------------------------------------------------------
void QEmailFetcher::initialize( ){    
    qDebug() << "QEmailFetcher::initialize :START";
    connect( iEmailService, SIGNAL(initialized(bool)), 
             this, SLOT(emailServiceIntialized(bool)) );
    iEmailService->initialise();
    //Monitor for Message changes
    connect( iEmailEventNotifier, 
             SIGNAL(messageEvent(EmailClientApi::NmApiMessageEvent, quint64, quint64, QList<quint64>)),
             this, 
             SLOT(handleMessageEvent(EmailClientApi::NmApiMessageEvent, quint64, quint64, QList<quint64>)) );
    //Start the monitoring
    iEmailEventNotifier->start();
    qDebug() << "QEmailFetcher::Started monitoring for Email message event";
    qDebug() << "QEmailFetcher::initialize :END";
}

//------------------------------------------------------------------------------
void QEmailFetcher::emailServiceIntialized(bool aAllOk){
    qDebug() << "QEmailFetcher::emailServiceIntialized :START  aAllOk = " << aAllOk;
    if( aAllOk ){
        connect( iMailBoxListings, SIGNAL(mailboxesListed(qint32)), this, SLOT(handleMailboxesListed(qint32)) );
    }
    qDebug() << "QEmailFetcher::emailServiceIntialized :END";
}

//------------------------------------------------------------------------------
void QEmailFetcher::StartHarvesting(){
    qDebug() << "QEmailFetcher::StartHarvesting :START";
    bool ret = iMailBoxListings->start();
    qDebug() << "QEmailFetcher::StartHarvesting :END return = " << ret;
}

//------------------------------------------------------------------------------
namespace {

// Taken from qt/src/corelib/kernel/qcore_symbian_p.cpp, as recomended in
// http://qt.nokia.com/files/pdf/whitepaper-using-qt-and-symbian-c-together, page 34.
// URL last accessed on April 6th, 2010.

// Returned TPtrC is valid as long as the given parameter is valid and unmodified
static inline TPtrC qt_QString2TPtrC( const QString& string )
{
    return TPtrC16(static_cast<const TUint16*>(string.utf16()), string.length());
}

//------------------------------------------------------------------------------
CSearchDocument* getPartialSearchDocument( quint64 aEnvelopeId ) {
    qDebug() << "getPartialSearchDocument :START";
    CSearchDocument* doc = 0;
    QT_TRAP_THROWING(
        //Use qt_Qstring2TPtrC since we are working with <b>const</b> EmailMessageEnvelope.
        doc = CSearchDocument::NewL( qt_QString2TPtrC( QString().setNum( aEnvelopeId ) ), 
                                     KMsgBaseAppClassGeneric );        
        );
    qDebug() << "getPartialSearchDocument :END";
    return doc;
    }
} //anonymous namespace
//------------------------------------------------------------------------------

CSearchDocument* QEmailFetcher::getSearchDocumentL( const NmApiMessageEnvelope& aEnvelope ,quint64 aMailboxId, quint64 aFolderId ){
    QList<NmApiEmailAddress> toList;
    qDebug() << "QEmailFetcher::getSearchDocumentL :START";
    //We need ALL the recipients in a SINGLE field.
    
    //Need to cast away const-ness since the get method is unfortunately not const.
    const_cast<NmApiMessageEnvelope&>(aEnvelope).getToRecipients( toList );
    QString recipients = "";
    for( int i=0; i<toList.length(); i++ )
        {
        qDebug() << "QEmailFetcher::To receipient displayname :"<< toList.at( i ).displayName() << "Address : "<<toList.at( i ).address();
        recipients += toList.at( i ).displayName() + " ";
        recipients += toList.at( i ).address() + " ";
        }
    // Get CC receipents
    const_cast<NmApiMessageEnvelope&>(aEnvelope).getCcRecipients( toList );
    for( int i=0; i<toList.length(); i++ )
        {
        qDebug() << "QEmailFetcher::CC receipient displayname :"<< toList.at( i ).displayName() << "Address : "<<toList.at( i ).address();
        recipients += toList.at( i ).displayName() + " ";
        recipients += toList.at( i ).address() + " ";
        }
    
    NmApiMessageBody body;
    //Cast away const-ness since the get method is unfortunately not const.
    //Returns void. Cannot check for success/failure.
    const_cast<NmApiMessageEnvelope&>(aEnvelope).getPlainTextBody( body ); 
    QString msgBody = body.content();
    qDebug() << "QEmailFetcher::Body of mail using paintextAPI:"<< aEnvelope.plainText() ;
    CSearchDocument* doc = 0;
    QT_TRAP_THROWING(
    //Use qt_Qstring2TPtrC since we are working with <b>const</b> EmailMessageEnvelope.
    doc = CSearchDocument::NewL( qt_QString2TPtrC( QString().setNum( aEnvelope.id() ) ), 
                                 KMsgBaseAppClassGeneric );
    //Add the sender details
    doc->AddFieldL( KMsgSender, qt_QString2TPtrC( const_cast<NmApiMessageEnvelope&>(aEnvelope).sender() ), 
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
    //Add the Mimetype
    doc->AddFieldL( KMimeTypeField, KMimeTypeMsg, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized );
    //Add the Subject field
    doc->AddFieldL( KMsgSubject, qt_QString2TPtrC( aEnvelope.subject() ), 
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized );
    //Add the recipients list (Includes To and CC fields)
    doc->AddFieldL( KMsgRecipients, qt_QString2TPtrC( recipients ), 
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText );
    //Add the email body
    doc->AddFieldL( KMsgBody, qt_QString2TPtrC( msgBody ), 
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized );
    //Add the mailboxid
    doc->AddFieldL( KMailBoxId, qt_QString2TPtrC( QString().setNum( aMailboxId ) ), 
                    CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized | CDocumentField::EAggregateNo );
    //Add the folder Id
    doc->AddFieldL( KFolderId, qt_QString2TPtrC( QString().setNum( aFolderId ) ), 
                    CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized | CDocumentField::EAggregateNo );
    //Add the attachment field. This field will be added only if there is any attachment.Details of the attachment
    //are not added due to lack of email application support.
    if ( aEnvelope.hasAttachments() )
       doc->AddFieldL( KHasAttachment, KAttachment, CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);
    //Add mailbox name
    NmApiMailbox aMailBox;
    iEmailService->getMailbox( aMailboxId, aMailBox );
    doc->AddFieldL( KMailBoxName, qt_QString2TPtrC(aMailBox.name()) , 
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText );
    
    // Sent date time  KSentTime
    QDateTime time = aEnvelope.sentTime();    
    doc->AddFieldL( KSentTime, qt_QString2TPtrC(time.toString ()) , 
                        CDocumentField::EStoreYes | CDocumentField::EIndexTokenized );
    //TODO : Foldername is not harvested as there is no API exposed from email application to get the foldername based on folderID.
    //This feature will be implemented as soon as we get supporting API's from Email team
    
    //Adding subject and body to the excerpt. 
    QString excerpt ;
    excerpt = aEnvelope.subject();
	excerpt += " " ;
    excerpt += msgBody ;
    doc->AddExcerptL( qt_QString2TPtrC(excerpt) );
    );
//#ifdef USE_HIGHLIGHTER
        doc->AddHLDisplayFieldL(qt_QString2TPtrC( const_cast<NmApiMessageEnvelope&>(aEnvelope).sender() ));
//#endif
    qDebug() << "QEmailFetcher::getSearchDocumentL :END";
    return doc;
}


//------------------------------------------------------------------------------
void QEmailFetcher::NotifyHarvestingComplete(){
    qDebug() << "QEmailFetcher::NotifyHarvestingComplete :START";
    iCurrentMailboxIndex = iCurrentFolderIndex = 0;
    //Free the iMailBoxes and iFolders
    iMailBoxes.clear();
    iFolders.clear();
    QT_TRAP_THROWING( iEmailObserver.HarvestingCompleted() );
    qDebug() << "QEmailFetcher::NotifyHarvestingComplete :END";
    return;
}

//------------------------------------------------------------------------------
void QEmailFetcher::handleMailboxesListed(qint32 aCount){
    qDebug() << "QEmailFetcher::handleMailboxesListed :START";
    iCurrentMailboxIndex = 0;
    if( aCount == NmApiMailboxListing::MailboxListingFailed ) {
        NotifyHarvestingComplete();
        qDebug() << "QEmailFetcher::handleMailboxesListed :MailboxListingFailed";
        return;
    }
    if( aCount>0 && iMailBoxListings->getMailboxes( iMailBoxes ) ){
        //Already set to NULL in constructor, so safe to call delete first time.
        qDebug() << "QEmailFetcher::handleMailboxesListed :processNextMailbox";
        processNextMailbox();
    }else{
        qDebug() << "QEmailFetcher::handleMailboxesListed :Harvesting Completed";
        NotifyHarvestingComplete();
        return;
    }
}

//------------------------------------------------------------------------------
//Options to make async (like other plugins' Asynchronizer):
//1. Use http://doc.trolltech.com/4.6/qtimer.html and connect timeout() signal to something?
//Downside: 
//Have to save the state of the function and resume. Achievable via static members. 
//Remeber to reset counters.
//2. Use timer; unlike above, have handleMailboxesListed() simply trigger a  
//Timer controlled function.
//3. Use QThread::currentThread()->yieldCurrentThread();
//Downside: Not tested.
//4. As recommended by the email API documentation, use SingleShotTimer:
//QTimer::singleShot(nsecs,nmFolderListing,SLOT(start());
//
//Recommendation: Use option 4.

void QEmailFetcher::processNextMailbox(){
    qDebug() << "QEmailFetcher::processNextMailbox :START";
    //No more mailboxes, notify completion.
    if( iCurrentMailboxIndex >= iMailBoxes.count() ) {
        NotifyHarvestingComplete();
        qDebug() << "QEmailFetcher::processNextMailbox :END (harvesting completed)";
        return;
    }
    //More mailboxes available.
    delete iMailFolderList; iMailFolderList = NULL;
    iMailFolderList = new NmApiFolderListing( this, iMailBoxes.at( iCurrentMailboxIndex++ ).id() );
    qDebug() << "\n QEmailFetcher::processing "<< iMailBoxes.at( iCurrentMailboxIndex-1 ).name()<< "Mailbox";
    connect( iMailFolderList, SIGNAL(foldersListed( qint32 )), this, SLOT(handleMailFoldersListed( qint32)) );
    iMailFolderList->start();
   
//    const int waitForSeconds = 30; //TODO Move this constant out of here if needed elsewhere
//    QTimer::singleShot( waitForSeconds, iMailFolderList, SLOT( start()) );
    qDebug() << "QEmailFetcher::processNextMailbox :END (goto next mailbox)";
}

//------------------------------------------------------------------------------
void QEmailFetcher::handleMailFoldersListed(int aCount){
    qDebug() << "QEmailFetcher::handleMailFoldersListed :START";
    iCurrentFolderIndex = 0;    
    if( aCount == NmApiFolderListing::FolderListingFailed ){
        qDebug() << "QEmailFetcher::handleMailFoldersListed :FolderListingFailed";
        processNextMailbox();
        return;//Don't proceed futher.
    }
    if( aCount && iMailFolderList->getFolders( iFolders ) ){ 
       qDebug() << "QEmailFetcher::handleMailFoldersListed :processNextFolder";
        processNextFolder();
    }else{
        qDebug() << "QEmailFetcher::handleMailFoldersListed :processNextMailbox";
        processNextMailbox();
        return;
    }
}

//------------------------------------------------------------------------------
void QEmailFetcher::processNextFolder(){
    qDebug() << "QEmailFetcher::processNextFolder :START";
    //No more folders in current mailbox.
    if( iCurrentFolderIndex >= iFolders.count() ) {
        qDebug() << "QEmailFetcher::processNextFolder :processNextMailbox";
        processNextMailbox();
        return;//Don't proceed futher.
    }
    //More folders to process.
    //Already set to NULL in constructor, so safe to call delete first time.
    delete iEnvelopeListing; iEnvelopeListing = NULL; 
    iEnvelopeListing= new NmApiEnvelopeListing( 
            this, 
            iFolders.at( iCurrentFolderIndex++ ).id(),
            iMailBoxes.at( iCurrentMailboxIndex-1 ).id() ); //we have already incremented iMailboxIndex.
    qDebug() << "\n QEmailFetcher::collecting mails from the "<< iFolders.at( iCurrentFolderIndex++ ).name() << "Folder \n";
    connect(iEnvelopeListing, SIGNAL(envelopesListed(qint32)),this,SLOT(processMessages(qint32)));
    iEnvelopeListing->start();
    qDebug() << "QEmailFetcher::processNextFolder :processNextFolder";
}

//------------------------------------------------------------------------------
void QEmailFetcher::processMessages(qint32 aCount){
    qDebug() << "QEmailFetcher::processMessages :START";
    if( aCount == NmApiEnvelopeListing::EnvelopeListingFailed ) {
        qDebug() << "QEmailFetcher::processMessages :EnvelopeListingFailed";
        processNextFolder();
        return;//Don't proceed futher.
    }
    QList<NmApiMessageEnvelope> envelopes;
    if ( aCount > 0 && iEnvelopeListing->getEnvelopes(envelopes) ) {
        for( int i=0; i<envelopes.count(); i++ ) {
            const NmApiMessageEnvelope &envelope = envelopes.at( i );
            //Create document and call back observer.
            QT_TRAP_THROWING( iEmailObserver.HandleDocumentL( getSearchDocumentL( envelope, iFolders.at( iCurrentFolderIndex -1 ).id(), 
                                                                                 iMailBoxes.at( iCurrentMailboxIndex-1 ).id() ), ECPixAddAction ) );
        }
    }
    qDebug() << "QEmailFetcher::processMessages :END";
}

//------------------------------------------------------------------------------
void QEmailFetcher::handleMessageEvent( EmailClientApi::NmApiMessageEvent aEvent, quint64 aMailboxId, quint64 aFolderId, QList<quint64> aMessageList){
    NmApiMessageEnvelope envelope;
    qDebug() << "QEmailFetcher::handleMessageEvent :START";
    qDebug() << "\n QEmailFetcher::aEvent="<< aEvent<<" MailboxId =" << aMailboxId << " FolderId = "<< aFolderId;
    const int messageCount = aMessageList.count();
    qDebug() << "\n QEmailFetcher::message count="<< messageCount;
    if( messageCount>0 ){
    if( aEvent == MessageCreated || aEvent == MessageChanged ){
        qDebug() << "QEmailFetcher::handleMessageEvent :MessageCreated || MessageChanged";
        for( int i=0; i<messageCount; i++ ){
            qDebug() << "\n MessageId"<< aMessageList.at( i );
            if( iEmailService->getEnvelope( aMailboxId, aFolderId, aMessageList.at( i ), envelope ) ){
               qDebug() << "QEmailFetcher::handleMessageEvent :HandleDocumentL";
                QT_TRAP_THROWING( 
                   iEmailObserver.HandleDocumentL( getSearchDocumentL( envelope, aMailboxId, aFolderId ), 
                           //Doing this simply avoids *duplicate* code for update action.
                           aEvent == MessageCreated ? ECPixAddAction : ECPixUpdateAction ) );
            }
        }
    }
    else if( aEvent == MessageDeleted ) {
        qDebug() << "QEmailFetcher::handleMessageEvent :MessageDeleted";
        for( int i=0; i<messageCount; i++ ){
                qDebug() << "\n MessageId"<< aMessageList.at( i );
                qDebug() << "QEmailFetcher::handleMessageEvent :MessageDeleted : HandleDocumentL";
                QT_TRAP_THROWING( 
                iEmailObserver.HandleDocumentL( getPartialSearchDocument( aMessageList.at( i ) ), ECPixRemoveAction ) );
            }
        }
    }
}
