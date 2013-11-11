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
#ifndef LOGSCONTACT_H
#define LOGSCONTACT_H

#include <QObject>
#include <qcontact.h>
#include <logsexport.h>

//forward declarations
class XQAiwRequest;
class LogsDbConnector;
class LogsEvent;

QTM_USE_NAMESPACE

/**
 * LogsContact can be used to modify or create a contact in phonebook
 * 
 */
class LogsContact : public QObject 
{
    Q_OBJECT
    
public:
    enum RequestType {
        TypeLogsContactOpen,     //contact is in phonebook, can open it
        TypeLogsContactSave,     //contact isn't in phonebook, can save it
        TypeLogsContactOpenGroup //contact is a group in phonebook, can open it
    };

public: 
 		
    explicit LogsContact(LogsEvent& event, LogsDbConnector& dbConnector);
    explicit LogsContact(const QString& number, 
                         LogsDbConnector& dbConnector,
                         unsigned int contactId = 0);
    LOGSENGINE_EXPORT ~LogsContact();
 	
    /**
     * Returns allowed request type
     */
    LOGSENGINE_EXPORT RequestType allowedRequestType();
        
    bool isContactRequestAllowed(); 
    
public slots:

    /**
     * Launch phonebook view for modification of the contact. Will proceed 
     * only if allowedRequestType() is TypeLogsContactOpen, otherwise returns false.
     * Emits openCompleted() signal, when contact modifications are done.
     * @return true if opening was called successfully
     */
    LOGSENGINE_EXPORT bool open();
    
    /**
     * Launch phonebook view for creation of a new contact. 
     * Emits saveCompleted() signal, when contact saving is done.
     * @return true if adding was called successfully
     */
    LOGSENGINE_EXPORT bool addNew();
    
    /**
     * Launch phonebook view for updating of existing contact. 
     * Emits saveCompeted() signal, when contact saving is done.
     * @return true if updating was called successfully
     */
    LOGSENGINE_EXPORT bool updateExisting();   

    /**
     * Cancel outstanding phonebook request(open(), addNew(), updateExisting()) 
     */
    LOGSENGINE_EXPORT void cancelServiceRequest();
    
signals:

    /**
    * Indicates that contact modification completed
    * @param modified true if contact was modified, false - otherwise
    */
    void openCompleted(bool modified);
    
    /**
    * Indicates that contact saving completed
    * @param modified true if contact was saved, false - otherwise
    */
    void saveCompleted(bool modified);
    
    
private slots:

    void handleRequestCompleted(const QVariant& result);
    void handleError(int,const QString&);
    
private: 
    
    /**
     * Finds contact from phonebook
     * @return contact
     */
    QContact contact();

    /**
     * Checks whether contact is in phonebook
     * @return true if contact is in phonebook
     */
    bool isContactInPhonebook();
    
    bool save(const QString& operation);
    
    bool requestPhonebookService(const QString& interface, 
                                 const QString& operation,
                                 const QList<QVariant>& arguments);
    
    bool isContactGroup();

    
private: //data 
    
    LogsDbConnector& mDbConnector;
    QContact mContact;
    
    XQAiwRequest* mAiwRequest;
    RequestType mCurrentRequest;
    QString mNumber;
    unsigned int mContactId;
    bool mSaveAsOnlineAccount;
        
private:
    
    friend class UT_LogsContact;
    
};
                  
#endif // LOGSCONTACT_H
