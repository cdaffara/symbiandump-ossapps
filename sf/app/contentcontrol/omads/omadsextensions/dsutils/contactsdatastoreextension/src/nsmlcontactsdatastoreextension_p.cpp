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
* Description:  Private implementation file for extended contacts adapter
*
*/

#include <s32strm.h>
#include <s32mem.h>
#include <e32base.h> 
#include <badesca.h>
#include <utf.h>
#include <e32hashtab.h>
#include <qcontactmanager.h>
#include <qcontact.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontactsynctarget.h>
#include <qcontacttimestamp.h>
#include <qcontactguid.h>
#include <qcontacttype.h>
#include <qversitwriter.h>
#include <qversitreader.h>
#include <qversitcontactexporter.h>
#include <qversitcontactimporter.h>
#include <xqconversions.h> 
#include <QBuffer>
#include <QMap>
#include <nsmlcontactsdatastoreextension.h>
#include "nsmlcontactsdatastoreextension_p.h"
#include "nsmldebug.h"


#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


/*!
  \class CNsmlContactsDataStoreExtensionPrivate
  \brief The CNsmlContactsDataStoreExtensionPrivate class provides interface
  to access the qtmobility Contacts APIs'

  \sa CNsmlContactsDataStoreExtensionPrivate
 */


/*!
 * Constructor
 */
CNsmlContactsDataStoreExtensionPrivate::CNsmlContactsDataStoreExtensionPrivate(const TDesC &manager )
{
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::CNsmlContactsDataStoreExtensionPrivate: BEGIN");
    
    // Get the QString manager
    mManagerName = QString::fromUtf16(manager.Ptr(),manager.Length());
    
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::Manager: %S"), &manager );
    
    mWriter = new QVersitWriter;
    mReader = new QVersitReader;
    mExporter = new QVersitContactExporter;
    mImporter = new QVersitContactImporter;
    
    // Instantiate the Contact Manager
    mContactManager = NULL;
    QStringList availableManagers = QContactManager::availableManagers();  
    
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::Manager Count: %d"), availableManagers.count());
    
    foreach (const QString manager, availableManagers)
    {
        if( mManagerName == manager )
        {
            _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::Got a Match");
            mContactManager = QContactManager::fromUri(QContactManager::buildUri(manager, QMap<QString, QString>()));
        }
    }
    if( mContactManager == NULL )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::Did Not Find a Match");
        mContactManager = new QContactManager( mManagerName );
    }
   
   _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::CNsmlContactsDataStoreExtensionPrivate: END");
}

/*!
 * Destructor
 */
CNsmlContactsDataStoreExtensionPrivate::~CNsmlContactsDataStoreExtensionPrivate()
{
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::~CNsmlContactsDataStoreExtensionPrivate: BEGIN");
   
    
    if( mContactManager )
        {
        delete mContactManager;
        mContactManager = NULL;
        }
    
    if( mWriter )
        {
        delete mWriter;
        mWriter = NULL;
        }
    if( mReader )
        {
        delete mReader;
        mReader = NULL;
        }
    if( mExporter )
        {
        delete mExporter;
        mExporter = NULL;
        }
    if( mImporter )
        {
        delete mImporter;
        mImporter = NULL;
        }
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::~CNsmlContactsDataStoreExtensionPrivate: END");
}

/*!
 * Fetch contact
 */
TInt CNsmlContactsDataStoreExtensionPrivate::ExportContactsL( const TUid &uid, CBufBase &contactbufbase )
{
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ExportContactsL: BEGIN: UID: %d"), uid);
   
    TInt error(KErrNone);
    QList<QContact> contacts;
    QBuffer contactsbuf;
    contactsbuf.open(QBuffer::ReadWrite);    
    mWriter->setDevice( &contactsbuf );  
    QContact contact = mContactManager->contact( uid.iUid );
    error = mContactManager->error();
    if( error != QContactManager::NoError )
        {
        DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ExportContactsL:Error %d"), error );
        return error; 
        }
	contacts.append( contact );    
    
    if( mExporter->exportContacts( contacts, QVersitDocument::VCard21Type ) )
        {
        QList<QVersitDocument> documents = mExporter->documents();        
        mWriter->startWriting( documents );
        bool status = mWriter->waitForFinished();  
        DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ExportContactsL:status %d"), status);  
        HBufC8* buf = XQConversions::qStringToS60Desc8( contactsbuf.data() );
        contactbufbase.InsertL( contactbufbase.Size(), *buf );
        delete buf;
        }
    else
        {
        error = KErrGeneral;
        DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ExportContactsL:Error in exporting %d"), error );
        }
    
    TPtr8 ptrbuf(contactbufbase.Ptr(0));
    DBG_DUMP( (void*)ptrbuf.Ptr(), ptrbuf.Length(),
           _S8("CNsmlContactsDataStoreExtensionPrivate::ExportContactsL: From DB:"));  
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ExportContactsL: END");
    
    return error;
}

/*!
 * Store contact
 */
CArrayFixFlat<TUid>* CNsmlContactsDataStoreExtensionPrivate::ImportContactsL( const TDesC8 &contactbufbase )
{
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ImportContactsL: BEGIN");    

    DBG_DUMP((void*)contactbufbase.Ptr(), contactbufbase.Length(),
        _S8("CNsmlContactsDataStoreExtensionPrivate::ImportContactsL: To DB :"));      
    
    TBool ret( ETrue );
    CArrayFixFlat<TUid>* contactItems = new(ELeave) CArrayFixFlat<TUid>(4);
    QBuffer contactsbuf;
    contactsbuf.open(QBuffer::ReadWrite);
    QByteArray bytearray((char*)contactbufbase.Ptr());
    
    contactsbuf.write(bytearray);
    contactsbuf.seek(0);
    
    mReader->setDevice(&contactsbuf);
    if (mReader->startReading() && mReader->waitForFinished()) 
        {
        DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::Buffer Count: %d"), mReader->results().count() );
        
        QList<QContact> contacts;
        if ( mImporter->importDocuments( mReader->results() ) )
            {
            contacts = mImporter->contacts();
            QMap<int, QContactManager::Error> errorMap;
            ret = mContactManager->saveContacts( &contacts, &errorMap );           
            DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::Import Status: %d"), ret );
            }
        if( ret )
            {
            foreach (QContact contact, contacts ) 
                {
                TUint contactid = contact.id().localId();
                
                DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::Contact ID: %d"), contactid );
                
                contactItems->AppendL( TUid::Uid( contactid ) );
                }
            }
            
        if( contactItems->Count() <= 0 )
            {
            delete contactItems;
            contactItems = NULL;
            }
        }
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ImportContactsL: END");
    
    return contactItems;
}

/*!
 * Reads a Contact Details
 */
TInt CNsmlContactsDataStoreExtensionPrivate::ReadContactL( const TUid &uid, CNsmlContactItem *contactitem )
    {
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ReadContactL: BEGIN ID: %d"), uid);
    
    TInt error(KErrNone);
    
    QContact contact = mContactManager->contact( uid.iUid );
    error = mContactManager->error();
    if( error != QContactManager::NoError )
        {
        DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ReadContactL:Error %d"), error );
        return error;
        }
        
    // Fetch the contact details
    QContactTimestamp timestamp = contact.detail( QContactTimestamp::DefinitionName );
    // Created Time
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::created: %d"), timestamp.created().toTime_t () );
    
    // Last Modified Time
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::modified: %d"), timestamp.lastModified().toTime_t () );
    contactitem->iLastModified = qTimeToS60TTime( timestamp.lastModified() );
       
    // Contact Type
    QContactType contacttype = contact.detail( QContactType::DefinitionName );
    contacttype.type();
    
    // GUID
    QContactGuid contactguid = contact.detail( QContactGuid::DefinitionName );
    contactitem->iGuid = XQConversions::qStringToS60Desc8( contactguid.guid() );
    
    // UID
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ReadContactL:LUID %d"), contact.localId() );
    contactitem->iUid = TUid::Uid(contact.localId());
    
    // SyncTarget
    QContactSyncTarget contactsynctarget = contact.detail( QContactSyncTarget::DefinitionName );
    contactsynctarget.syncTarget();
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ReadContactL: END");
    
    return error;
    }

/*!
 * Delete a contact
 */
TBool CNsmlContactsDataStoreExtensionPrivate::DeleteContactL( const TUid &uid )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::DeleteContactL: BEGIN");
    
    TBool ret = mContactManager->removeContact( uid.iUid );
    
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::DeleteContactL: END %d"), ret );
    
    return ret;
    }

/*!
 * Delete array of contacts
 */
TBool CNsmlContactsDataStoreExtensionPrivate::DeleteContactsL( CArrayFixFlat<TUid> *arrDelete )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::DeleteContactsL: BEGIN");
    
    QList<QContactLocalId> contacts;
    for( TInt count = 0; count < arrDelete->Count(); count++ )
        {
        contacts.append( arrDelete->At( count ).iUid );
        }
    
    QMap<int, QContactManager::Error> errorMap;
    TBool ret = mContactManager->removeContacts( contacts, &errorMap );
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::DeleteContactsL: END");
    
    return ret;
    }

/*!
 * Delete all contacts except selfcontact
 */
TBool CNsmlContactsDataStoreExtensionPrivate::DeleteAllContactsL()
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::DeleteAllContactsL: BEGIN");
    
    QContactLocalId selfcontactid = mContactManager->selfContactId();
    QList<QContactLocalId> contactIds = mContactManager->contactIds();
    int index = contactIds.indexOf( selfcontactid );   
    contactIds.removeAt( index );
    
    QMap<int, QContactManager::Error> errorMap;
    TBool ret = mContactManager->removeContacts( contactIds, &errorMap );
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::DeleteAllContactsL: END");
    
    return ret;
    }

/*!
 * List the available managers
 */
void CNsmlContactsDataStoreExtensionPrivate::ListStoresL( CDesCArray *cntstores )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ListStoresL: BEGIN");
    
    QStringList availableManagers = QContactManager::availableManagers();    
    foreach (const QString manager, availableManagers)
    {
        HBufC* buf = XQConversions::qStringToS60Desc( manager );
        cntstores->AppendL( buf->Des() );
        delete buf;
    }
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ListStoresL: END");
    }


/*!
 * Fetch Manager URI
 */
TInt64 CNsmlContactsDataStoreExtensionPrivate::MachineIdL()
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::MachineIdL: BEGIN");
   
    HBufC* buf = XQConversions::qStringToS60Desc( mContactManager->managerUri() );
    TInt64 machineid = DefaultHash::Des16( buf->Des() );
    delete buf;
    
    DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::MachineIdL: %ld"), machineid );
    
    return machineid;
    }

/*!
 * List all contacts
 */
void CNsmlContactsDataStoreExtensionPrivate::ListContactsL( CArrayFixFlat<TUid> *contactuidlist )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ListContactsL: BEGIN");
    
    QList<QContactLocalId> contactIds = mContactManager->contactIds();    
     
    foreach (QContactLocalId id, contactIds )
        {
        DBG_ARGS(_S("CNsmlContactsDataStoreExtensionPrivate::ListContactsL: %d"), id );
        contactuidlist->AppendL( TUid::Uid(id) );
        }    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::ListContactsL: END");
    }

/*!
 * Convert QTime to S60 TTime
 */
TTime CNsmlContactsDataStoreExtensionPrivate::qTimeToS60TTime(const QDateTime &datetime)
    {
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::qTimeToS60TTime: BEGIN");
    
    //Symbian month & day are zero based, so -1 from Qt's
    TDateTime dt(datetime.date().year(), 
    TMonth(datetime.date().month() - 1),
    datetime.date().day() - 1,
    datetime.time().hour(),
    datetime.time().minute(),
    datetime.time().second(),
    datetime.time().msec() * 1000);
    
    _DBG_FILE("CNsmlContactsDataStoreExtensionPrivate::qTimeToS60TTime: END");
    return TTime(dt);
    }

// End of File
