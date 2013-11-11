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
* Description:  Private header file for extended Contacts API
*
*/


#ifndef NSMLCONTACTSDATASTOREEXTENSIONPRIVATE_H_
#define NSMLCONTACTSDATASTOREEXTENSIONPRIVATE_H_

#include <QObject>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QVersitWriter;
class QVersitReader;
class QVersitContactExporter;
class QVersitContactImporter;
class QContactManager;
QTM_END_NAMESPACE
class QBuffer;
class QDateTime;
class QTime;
class CNsmlContactsStoreUtil;
class CNsmlContactItem;
class RDesReadStream;

QTM_USE_NAMESPACE
class CNsmlContactsDataStoreExtensionPrivate: public CBase
{
public:
    /**
    * C++ constructor.
    */
    CNsmlContactsDataStoreExtensionPrivate( const TDesC &manager );
    
    /**
    * Destructor.
    */
    ~CNsmlContactsDataStoreExtensionPrivate();
    
public: // Methods
    /**
    * Fetch the Contact Item from the DB 
    * @param uid UID of the desired item to be fetched
    * @param contactbufbase on return will have the fetched Contact details
    */
    TInt ExportContactsL( const TUid &uid, CBufBase &contactbufbase );
    
    /**
    * Store the Contact Item to the DB
    * @param contactbufbase Contact item to be saved to the DB
    * @return A pointer to list of newly saved Contact UIDs'
    */
    CArrayFixFlat<TUid>* ImportContactsL( const TDesC8 &contactbufbase );
    
    /**
    * Fetch Contact details
    * @param uid UID of the desired Contact item
    * @param contactitem on return will have pointer class containing contact details
    */
    TInt ReadContactL( const TUid &uid, CNsmlContactItem *contactitem );
    
    /**
    * Delete a Contact
    * @param uid UID of the desired Contact item
    * @return status of the operation
    */
    TBool DeleteContactL( const TUid &uid );
    
    /**
    * Delete List of Contacts
    * @param arrDelete List of Contact UIDs' to be deleted
    * @return status of the operation
    */
    TBool DeleteContactsL( CArrayFixFlat<TUid> *arrDelete );
    
    /**
    * Delete All Contacts
    * @return status of the operation
    */
    TBool DeleteAllContactsL();
    
    /**
    * List Supported Stores
    * @param contactuidlist on return will have list of stores
    */
    void ListStoresL( CDesCArray *contactuidlist );
    
    /**
    * Unique ID for a Store
    * @return unique id represented as integer  
    */
    TInt64 MachineIdL();
    
    /**
    * Fetch all available Contacts from the DB
    * @param contactuidlist on return will have list of all Contact UIDs'
    */
    void ListContactsL( CArrayFixFlat<TUid> *contactuidlist );
private:
    /**
    * Convert the given QDateTime to S60 TTime
    * @param datetime variable to be converted
    */
    TTime qTimeToS60TTime(const QDateTime &datetime);
    
   
private: // Data
    QString mManagerName;
	QContactManager *mContactManager;
 	QVersitWriter *mWriter;
 	QVersitReader *mReader;
	QVersitContactExporter *mExporter;
	QVersitContactImporter *mImporter;
};

#endif /* NSMLCONTACTSDATASTOREEXTENSIONPRIVATE_H_*/
