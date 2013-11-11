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
#include <logsexport.h>

//forward declarations
class LogsDbConnector;

/**
 * LogsContact can be used to modify or create a contact in phonebook
 * 
 */
class LogsContact : public QObject 
{
    Q_OBJECT
    
public:
    enum RequestType {
        TypeLogsContactOpen,    //contact is in phonebook, can open it
        TypeLogsContactSave,    //contact isn't in phonebook, can save it
        TypeLogsContactOpenGroup //contact is a group in phonebook, can open it
    };

public: 
        
    //explicit LogsContact(LogsEvent& event, LogsDbConnector& dbConnector);
    explicit LogsContact(); // Stub constructor
    explicit LogsContact(const QString& number); // Stub constructor
    ~LogsContact();
    
    /**
     * Returns allowed request type
     */
    RequestType allowedRequestType();
    
public slots:

    /**
     * Launch phonebook view for modification of the contact. Will proceed 
     * only if allowedRequestType() is TypeLogsContactOpen, otherwise returns false.
     * Emits openCompeted() signal, when contact modifications are done.
     * @return true if opening was called successfully
     */
    bool open();
    
    /**
     * Launch phonebook view for creation of a new contact. 
     * Emits openCompleted() signal, when contact saving is done.
     * @return true if saving was called successfully
     */
    bool addNew();
    
    /**
     * Launch phonebook view for creation of a new contact. 
     * Emits openCompleted() signal, when contact saving is done.
     * @return true if saving was called successfully
     */
    bool updateExisting();
    
    void cancelServiceRequest();
    
signals:

    void openCompleted(bool modified);
    void saveCompleted(bool modified);
        
public: //stub data 
    
    static void setNextRequestType(LogsContact::RequestType type);
    static void reset();
    
    QString mNumber;
    static bool mServiceRequestCanceled;
};
                  
#endif // LOGSCONTACT_H
