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

#ifndef TESTPBKSERVICES_H
#define TESTPBKSERVICES_H

#include <QObject>
#include <QVariant>

#include <qmobilityglobal.h> //defines Q_SFW_EXPORT
#include <xqappmgr.h>

class HbMainWindow;
class HbAction;
class HbDialog;

/**
 * Launches phonebookservices highway services, for functional testing.
 */
class testPbkServices : public QObject
{
    Q_OBJECT

public slots:
    // FETCH
    void launchMultiFetch();
    void launchMultiFetch_old();
    void launchMultiFetch_sms();
    void launchMultiFetch_sms_old();
    void launchMultiFetch_email();
    void launchMultiFetch_email_old();
    void launchSingleFetch();
    void launchSingleFetch_old();

    // EDIT - create new
    void launchEditCreateNew_number();
    void launchEditCreateNew_number_old();
    void launchEditCreateNew_numberWithSubtype();
    void launchEditCreateNew_numberWithSubtype_old();    
    void launchEditCreateNew_email();
    void launchEditCreateNew_email_old();
    void launchEditCreateNew_onlineAccount();
    void launchEditCreateNew_onlineAccount_old();
    void launchEditCreateNew_onlineAccountWithSubtype();
    void launchEditCreateNew_onlineAccountWithSubtype_old();
    void launchEditorVCard();
    void launchEditorVCard_old();

    // EDIT - update existing
    void launchEditUpdateExisting_number();
    void launchEditUpdateExisting_number_old();
    void launchEditUpdateExisting_numberWithSubtype();
    void launchEditUpdateExisting_numberWithSubtype_old();    
    void launchEditUpdateExisting_email();
    void launchEditUpdateExisting_email_old();
    void launchEditUpdateExisting_onlineAccount();
    void launchEditUpdateExisting_onlineAccount_old();
    void launchEditUpdateExisting_onlineAccountWithSubtype();
    void launchEditUpdateExisting_onlineAccountWithSubtype_old();

    // CONTACT CARD
    void launchContactCard();
    void launchContactCard_old();
    void launchContactCardNumber();
    void launchContactCardNumber_old();
    void launchContactCardEmail();
    void launchContactCardEmail_old();
    void launchContactCardOnlineAccount();
    void launchContactCardOnlineAccount_old();
    
    // GROUP MEMBER VIEW
    void launchGroupMemberView();

    void onEditCompleted(const QVariant& value);
    void onRequestCompleted(const QVariant& value);

public:
    testPbkServices(HbMainWindow *aParent);
    ~testPbkServices();

private:
    void launchMultiFetch( bool aNewInterface, QString aAction );
    void launchSingleFetch( bool aNewInterface, QString aAction );
    void launchEditCreateNew( bool aNewInterface, QString aDetailType, QString aFieldContent, QString aDetailSubtype = QString() );
    void launchEditorVCard( bool aNewInterface );
    void launchEditUpdateExisting( bool aNewInterface, QString aDetailType, QString aDetailValue, QString aDetailSubtype = QString());
    void launchContactCard( bool aNewInterface );
    void launchContactCard( bool aNewInterface, QString aDetailType, QString aDetailValue );

private:

    XQAiwRequest         *mRequest; 
    HbMainWindow         *mMainWindow;
    HbDialog             *mPopup;

};

#endif
