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

#ifndef CNTSERVICES_H
#define CNTSERVICES_H

#include <cntviewparams.h>
#include <qtcontacts.h>

#include "cntabstractserviceprovider.h"

class CntAbstractEngine;
class CntAbstractViewManager;
class XQServiceProvider;

QTM_USE_NAMESPACE

class CntServices : public QObject, public CntAbstractServiceProvider
{
    Q_OBJECT
    
public:
    CntServices();
    ~CntServices();

public:

    //void setViewManager( CntAbstractViewManager& aViewManager );
    void setEngine( CntAbstractEngine& aEngine );
    
    /**
     * Opens contact selection view where multiple contacts can be selected.
     * @param title The view title
     * @param action Filter definition for filtering contacts based on supported action. Can be left empty.
     * @param aServiceProvider The service provider
     */
    void singleFetch(const QString &title,
                const QString &action,
                CntAbstractServiceProvider& aServiceProvider  );

    /**
     * Opens contact selection view where multiple contacts can be selected.
     * @param title The view title
     * @param action Filter definition for filtering contacts based on supported action. Can be left empty.
     * @param aServiceProvider The service provider
     */
    void multiFetch(const QString &title,
                const QString &action,
                CntAbstractServiceProvider& aServiceProvider  );

    /**
     * Launch editor for creating a new contact with a given detail.
     * @param definitionName The field type
     * @param value The value for the field.
     * @param aServiceProvider The service provider
     * @param defaultForOnlineAccountIsImpp If no subtype is given, default
     *          for QContactOnlineAccount is QContactOnlineAccount::SubTypeImpp. If false, then
     *          the default is QContactOnlineAccount::SubTypeSipVoip.
     */
    void editCreateNew(const QString &definitionName, const QString &value,
        CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp = true );

    /**
     * Launch editor for creating a new contact with a given detail and subtype.
     * If the given subtype string is empty, then QContactPhoneNumber::SubTypeMobile is used.
     * @param definitionName The field type
     * @param value The value for the field.
     * @param subType The subtype for the field. (for eg. QContactPhoneNumber::SubTypeLandline)
     * @param aServiceProvider The service provider
     * @param defaultForOnlineAccountIsImpp If no subtype is given, default
     *          for QContactOnlineAccount is QContactOnlineAccount::SubTypeImpp. If false, then
     *          the default is QContactOnlineAccount::SubTypeSipVoip.
     */
    void editCreateNew(const QString &definitionName, const QString &value, const QString& subType,
        CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp = true );

    /**
     * Launch editor for creating a new contact based on vCard file indicated in arg.
     * @param vCardFile The vCard file name and path.
     * @param aServiceProvider The service provider
     */
    void editCreateNewFromVCard(const QString &vCardFile,
        CntAbstractServiceProvider& aServiceProvider );

    /**
     * User can first choose a contact, and then an editor is opened.
     * A new detail is provided by caller and it will be prefilled in the editor.
     * @param definitionName The field type
     * @param value The value for the field.
     * @param aServiceProvider The service provider
     */
    void editUpdateExisting(const QString &definitionName, const QString &value,
        CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp = true );

    /**
     * User can first choose a contact, and then an editor is opened.
     * A new detail is provided by caller and it will be prefilled in the editor.
     * @param definitionName The field type
     * @param value The value for the field.
     * @param subType The subtype for the field. (for eg. QContactPhoneNumber::SubTypeLandline)
     * @param aServiceProvider The service provider
     * @param defaultForOnlineAccountIsImpp If no subtype is given, default
     *          for QContactOnlineAccount is QContactOnlineAccount::SubTypeImpp. If false, then
     *          the default is QContactOnlineAccount::SubTypeSipVoip.
     */
    void editUpdateExisting(const QString &definitionName, const QString &value, const QString& subType,
        CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp = true );

    /**
     * Open the editor for an existing contact.'
     * @param contactId
     * @param aServiceProvider The service provider
     */
    void editExisting(int contactId,
        CntAbstractServiceProvider& aServiceProvider);

    /**
     * Opens Contact Card.
     * @param contactId The contact ID
     * @param aServiceProvider The service provider
     */
    void launchContactCard(int contactId,
        CntAbstractServiceProvider& aServiceProvider );
    
    /**
     * Opens Group member view.
     * @param contactId The contact ID
     * @param aServiceProvider The service provider
     */
    void launchGroupMemberView(int contactId,
        CntAbstractServiceProvider& aServiceProvider );

    /**
     * Opens Contact Card for non-saved contact.
     * @param definitionName Field name.
     * @param value The field value.
     * @param aServiceProvider The service provider
     */
    void launchTemporaryContactCard(const QString &definitionName, const QString &value,
        CntAbstractServiceProvider& aServiceProvider );
        
public slots:
    /**
     * Quit the application
     */
    void quitApp();
    /**
     * Terminate a service
     */
    void terminateService();
    
private:
    void removeNotSupportedFields(QContact& contact);
    void removeNotSupportedDetails(QContact& contact);
    void fillOnlineAccount( QContactOnlineAccount& account, const QString& value, const QString& subType, bool defaultForOnlineAccountIsImpp);
    QContactManager* contactManager();
    
    /**
     * Checks if we already have the given contact global uid in database. If so,
     * updates the local id and managerUri accordingly.
     * 
     * If given contact has already a local id, method does nothing.
     */
    void updateLocalId( QContact& aContact );
    
private: // from CntAbstractServiceProvider
    void CompleteServiceAndCloseApp(const QVariant& retValue);
    void overrideReturnValue(const QVariant& retValue);
    bool allowSubViewsExit();
private:
    CntAbstractViewManager* mViewManager; // not owned
    CntAbstractEngine* mEngine;
    QStringList m_definitionNames;
    CntAbstractServiceProvider* mCurrentProvider; // not owned
};

#endif /* CNTSERVICES_H */
