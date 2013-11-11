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

#include "testpbkservices.h"
#include <cntservicescontact.h>
#include <hbmessagebox.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblabel.h>
#include <cntlistmodel.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <qtcontacts.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <QDebug>
#include <HbAction>

#include "cntdebug.h"


testPbkServices::testPbkServices(HbMainWindow *aParent) :
mRequest(NULL),
mMainWindow(aParent),
mPopup(NULL)
{
    setParent(aParent);
}

testPbkServices::~testPbkServices()
{
    delete mRequest;
    mRequest=0;
}

void testPbkServices::launchMultiFetch()
{
    CNT_ENTRY
    launchMultiFetch(
        true,           // aNewInterface = true
        KCntActionAll);  // aAction (all actions in this case)
    CNT_EXIT
}

void testPbkServices::launchMultiFetch_old()
{
    CNT_ENTRY
    launchMultiFetch(
        false,           // aNewInterface = false
        KCntActionAll);  // aAction (all actions in this case)
    CNT_EXIT
}

void testPbkServices::launchMultiFetch_sms()
{
    CNT_ENTRY
    launchMultiFetch(
        true,           // aNewInterface = false
        KCntActionSms);  // aAction
    CNT_EXIT
}

void testPbkServices::launchMultiFetch_sms_old()
{
    CNT_ENTRY
    launchMultiFetch(
        false,           // aNewInterface
        KCntActionSms);  // aAction
    CNT_EXIT
}

void testPbkServices::launchMultiFetch_email()
{
    CNT_ENTRY
    launchMultiFetch(
        true,              // aNewInterface
        KCntActionEmail);  // aAction
    CNT_EXIT
}

void testPbkServices::launchMultiFetch_email_old()
{
    CNT_ENTRY
    launchMultiFetch(
        false,             // aNewInterface
        KCntActionEmail);  // aAction
    CNT_EXIT
}

void testPbkServices::launchSingleFetch()
{
    CNT_ENTRY
    launchSingleFetch(
        true,           // aNewInterface
        KCntActionAll ); // aAction
    CNT_EXIT
}

void testPbkServices::launchSingleFetch_old()
{
    CNT_ENTRY
    launchSingleFetch(
        false,           // aNewInterface
        KCntActionAll ); // aAction
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_number()
{
    CNT_ENTRY
    launchEditCreateNew(
        true,                                // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567");                          // aFieldContent
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_number_old()
{
    CNT_ENTRY
    launchEditCreateNew(
        false,                               // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567");                          // aFieldContent
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_numberWithSubtype()
{
    CNT_ENTRY
    launchEditCreateNew(
        true,                                // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567",                           // aFieldContent
        QContactPhoneNumber::SubTypeLandline); //aDetailSubtype
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_numberWithSubtype_old()
{
    CNT_ENTRY
    launchEditCreateNew(
        false,                               // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567",                          // aFieldContent
        QContactPhoneNumber::SubTypeLandline); //aDetailSubtype
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_email()
{
    CNT_ENTRY
    launchEditCreateNew(
        true,                                 // aNewInterface
        QContactEmailAddress::DefinitionName, // aDetailType
        "email@mailprovider.com");            // aFieldContent
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_email_old()
{
    CNT_ENTRY
    launchEditCreateNew(
        false,                                // aNewInterface
        QContactEmailAddress::DefinitionName, // aDetailType
        "email@mailprovider.com");            // aFieldContent
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_onlineAccount()
{
    CNT_ENTRY
    launchEditCreateNew(
        true,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "account@provider.com");               // aFieldContent
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_onlineAccount_old()
{
    CNT_ENTRY
    launchEditCreateNew(
        false,                                 // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "account@provider.com");            // aFieldContent
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_onlineAccountWithSubtype()
{
    CNT_ENTRY
    launchEditCreateNew(
        true,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "provider:account@provider.com",                // aFieldContent
        QContactOnlineAccount::SubTypeImpp); // aDetailSubtype
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew_onlineAccountWithSubtype_old()
{
    CNT_ENTRY
    launchEditCreateNew(
        false,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "provider:account@provider.com",                // aFieldContent
        QContactOnlineAccount::SubTypeImpp); // aDetailSubtype
    CNT_EXIT
}

void testPbkServices::launchEditorVCard()
{
    CNT_ENTRY
    launchEditorVCard(
        true); // aNewInterface
    CNT_EXIT
}

void testPbkServices::launchEditorVCard_old()
{
    CNT_ENTRY
    launchEditorVCard(
        false); // aNewInterface
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_number()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        true,                                // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567");                          // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_number_old()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        false,                               // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567");                          // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_numberWithSubtype()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        true,                                // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567",                           // aDetailValue
        QContactPhoneNumber::SubTypeLandline);// aDetailSubtype 
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_numberWithSubtype_old()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        false,                               // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "1234567",                           // aDetailValue
        QContactPhoneNumber::SubTypeLandline);// aDetailSubtype    
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_email()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        true,                                // aNewInterface
        QContactEmailAddress::DefinitionName, // aDetailType
        "email@mailprovider.com");                          // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_email_old()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        false,                                // aNewInterface
        QContactEmailAddress::DefinitionName, // aDetailType
        "email@mailprovider.com");            // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_onlineAccount()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        true,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "account@provider.com");               // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_onlineAccount_old()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        false,                                 // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "account@provider.com");               // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_onlineAccountWithSubtype()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        true,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "provider:account@provider.com",       // aDetailValue
        QContactOnlineAccount::SubTypeImpp); // aDetailSubtype              
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting_onlineAccountWithSubtype_old()
{
    CNT_ENTRY
    launchEditUpdateExisting(
        false,                                 // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "provider:account@provider.com",       // aDetailValue
        QContactOnlineAccount::SubTypeImpp); // aDetailSubtype    
    CNT_EXIT
}

void testPbkServices::launchContactCard()
{
    CNT_ENTRY
    launchContactCard(
        true ); // aNewInterface
    CNT_EXIT
}

void testPbkServices::launchContactCard_old()
{
    CNT_ENTRY
    launchContactCard(
        false ); // aNewInterface
    CNT_EXIT
}

void testPbkServices::launchContactCardNumber()
{
    CNT_ENTRY
    launchContactCard(
        true,                                // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "777555669" );                       // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchContactCardNumber_old()
{
    CNT_ENTRY
    launchContactCard(
        false,                               // aNewInterface
        QContactPhoneNumber::DefinitionName, // aDetailType
        "777555669" );                       // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchContactCardEmail()
{
    CNT_ENTRY
    launchContactCard(
        true,                                 // aNewInterface
        QContactEmailAddress::DefinitionName, // aDetailType
        "aa.jj@aajj.com" );                   // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchContactCardEmail_old()
{
    CNT_ENTRY
    launchContactCard(
        false,                                // aNewInterface
        QContactEmailAddress::DefinitionName, // aDetailType
        "aa.jj@aajj.com" );                   // aDetailValue
    CNT_EXIT
    CNT_EXIT
}

void testPbkServices::launchContactCardOnlineAccount()
{
    CNT_ENTRY
    launchContactCard(
        true,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "account@provider.com" );              // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchContactCardOnlineAccount_old()
{
    CNT_ENTRY
    launchContactCard(
        true,                                  // aNewInterface
        QContactOnlineAccount::DefinitionName, // aDetailType
        "account@provider.com" );              // aDetailValue
    CNT_EXIT
}

void testPbkServices::launchGroupMemberView()
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    // save test contact
    QContactManager mgr("symbian");
    QContact contact;
    QContactName name;
    QContactPhoneNumber number;
    
    name.setFirstName("Test_Contact");
    number.setNumber("0202223344");
    contact.saveDetail(&name);
    contact.saveDetail(&number);
    mgr.saveContact(&contact);
    
    // save test group
    QString text("Work");
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
    QContactName groupName;
    groupName.setCustomLabel(text);
    groupContact.saveDetail(&groupName);
    mgr.saveContact(&groupContact);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(contact.id());
    // Save relationship
    mgr.saveRelationship(&relationship);
    
    // get contact id
    int id = groupContact.id().localId();
        
    QVariantList args; 
    XQApplicationManager appMng;
    QString interface("com.nokia.symbian.IContactsView");
    QString operation("openGroup(int)");
    // interface name is not needed
    mRequest = appMng.create( interface, operation, true); // embedded 
    if ( mRequest )
    {    
        CNT_LOG_ARGS("Request created.")
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
        
        args << id;
        
        mRequest->setArguments(args); 
        mRequest->send();
        CNT_LOG_ARGS("Request sent.")
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request")
    }
    CNT_EXIT
}

void testPbkServices::onEditCompleted(const QVariant& value)
{
    CNT_ENTRY
    mMainWindow->activateWindow();
    
    if (value.toInt() == 1)
    {
        HbMessageBox::information("Contact saved");
        CNT_LOG_ARGS("test: Contact saved")
    }
    else
    {
        HbMessageBox::information("Contact saving failed");
        CNT_LOG_ARGS("test: Contact saving failed")
    }
    CNT_EXIT
}


void testPbkServices::onRequestCompleted(const QVariant& value)
{
    CNT_ENTRY
    mMainWindow->activateWindow();

    CntServicesContactList retValue;
    retValue=qVariantValue<CntServicesContactList>(value);


    if (retValue.count() == 0)
    {
        HbMessageBox::information("test: Nothing returned");
        CNT_LOG_ARGS("No data returned.")
    }

    else
    {
        HbListWidget *listWidget = new HbListWidget();
        for(int i = 0; i < retValue.count(); i++ )
            {
            QString name      = retValue[i].mDisplayName;
            listWidget->addItem("name:" );
            listWidget->addItem(name);
            CNT_LOG_ARGS("Returned name: " << name)

            QString number    = retValue[i].mPhoneNumber;
            if (number!="")
                {
                listWidget->addItem("number:");
                listWidget->addItem(number);
                CNT_LOG_ARGS("Returned number: " << number)
                }

            QString emailAddress    = retValue[i].mEmailAddress;
            if (emailAddress!="")
                {
                listWidget->addItem("emailAddress:");
                listWidget->addItem(emailAddress);
                CNT_LOG_ARGS("Returned email: " << emailAddress)
                }


            QString contactId = QString("id: %1").arg(retValue[i].mContactId);
            listWidget->addItem( contactId );

            }
            mPopup = new HbDialog();
            mPopup->setAttribute(Qt::WA_DeleteOnClose, true);

            // Set dismiss policy that determines what tap events will cause the popup
            // to be dismissed
            mPopup->setDismissPolicy(HbDialog::NoDismiss);
            mPopup->setModal(true);

            // Set the label as heading widget
            mPopup->setHeadingWidget(new HbLabel(tr("Contact")));

            // Set a list widget as content widget in the popup
            mPopup->setContentWidget(listWidget);
            
            // Add an OK action to dismiss the popup
            HbAction *okAction = new HbAction(tr("Ok"), mPopup);
            mPopup->addAction(okAction);

            // Launch popup asyncronously
            mPopup->open();
    }
    CNT_EXIT
}

void testPbkServices::launchMultiFetch( bool aNewInterface, QString aAction )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    QVariantList args; 

    XQApplicationManager appMng;
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsFetch"); 
        QString operation("multiFetch(QString,QString)");
        // There are two kinds of create() methods in XQApplicationManager. The one with four arguments
        // takes the service name also. The one with three arguments (used below) does not take the service name.
        // The interface name is enough for finding the correct provider at run time.
        mRequest = appMng.create(interface, operation, true); // embedded
    }
    else
    {
        mRequest = appMng.create(
            "com.nokia.services.phonebookservices",// service
            "Fetch",                             // interface
            "fetch(QString,QString,QString)",  // operation
            true); // embedded 
    }

    if (mRequest)
        {
        CNT_LOG_ARGS("Created request.")
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
        
        args << "My own title"; // This is the title for the view
        args << aAction; 
        args << KCntFilterDisplayAll; 
        
        mRequest->setArguments(args); 
        mRequest->send();
        }
    else
        {
        CNT_LOG_ARGS("Failed to create request.")
        }
    CNT_EXIT
}

void testPbkServices::launchSingleFetch( bool aNewInterface, QString aAction )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    QVariantList args; 
    XQApplicationManager appMng;
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsFetch"); 
        QString operation("singleFetch(QString,QString)");
        // There are two kinds of create() methods in XQApplicationManager. The one with four arguments
        // takes the service name also. The one with three arguments (used below) does not take the service name.
        // The interface name is enough for finding the correct provider at run time.
        mRequest = appMng.create(interface, operation, true); // embedded
    }
    else
    {
        QString serviceName("com.nokia.services.phonebookservices");
        QString interface("Fetch");
        QString operation("Dofetch(QString,QString,QString,QString)");
        mRequest = appMng.create(serviceName, interface, operation, true); // embedded 
    }

    if ( mRequest )
    {
        CNT_LOG_ARGS("Created request.")        
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
        
        args << "Single-fetching";
        args << aAction; 
        args << KCntFilterDisplayAll;
        if ( !aNewInterface )
        {
            // Only the old interface takes this useless argument.
            args << KCntSingleSelectionMode;
        }
        
        mRequest->setArguments(args); 
        mRequest->send();
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request.")
    }
    CNT_EXIT
}

void testPbkServices::launchEditCreateNew( bool aNewInterface, QString aDetailType, QString aFieldContent, QString aDetailSubtype )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    QVariantList args; 
    XQApplicationManager appMng;
    QString operation("editCreateNew(QString,QString)");
    if ( !aDetailSubtype.isEmpty() )
    {
        operation = "editCreateNew(QString,QString,QString)";
    }
    
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsEdit"); 
        // service name is not needed
        mRequest = appMng.create( interface, operation, true); // embedded
    }
    else
    {
        QString interface("Fetch");
        QString serviceName("com.nokia.services.phonebookservices"); 
        mRequest = appMng.create(serviceName, interface, operation, true); // embedded 
    }

    if ( mRequest )
    {
        CNT_LOG_ARGS("Created request.")
        
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
        
        args << aDetailType;
        args << aFieldContent;
        if ( !aDetailSubtype.isEmpty() )
        {
            args << aDetailSubtype;
        }
        
        mRequest->setArguments(args); 
        mRequest->send();
        CNT_LOG_ARGS("Sent request.")
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request.")
    }
    CNT_EXIT
}

void testPbkServices::launchEditorVCard( bool aNewInterface )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    QVariantList args; 
    XQApplicationManager appMng;
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsEdit"); 
        QString operation("editCreateNewFromVCard(QString)");
        // service name is not needed
        mRequest = appMng.create( interface, operation, true); // embedded 
    }
    else
    {
        QString interface("Fetch");
        QString operation("editCreateNew(QString)");
        QString serviceName("com.nokia.services.phonebookservices");
        mRequest = appMng.create(serviceName, interface, operation, true); // embedded 
    }

    if ( mRequest )
    {
        CNT_LOG_ARGS("Created request.")
        
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
        
        args << "C:\\data\\Others\\testvcard.vcf";
        
        mRequest->setArguments(args); 
        mRequest->send();
        CNT_LOG_ARGS("Sent request.")
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request.")
    }
    CNT_EXIT
}

void testPbkServices::launchEditUpdateExisting( bool aNewInterface, QString aDetailType, QString aDetailValue, QString aDetailSubtype )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    QVariantList args; 
    XQApplicationManager appMng;
    QString operation("editUpdateExisting(QString,QString)");
    if ( !aDetailSubtype.isEmpty() )
    {
        operation = "editUpdateExisting(QString,QString,QString)";
    }
        
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsEdit");
        // service name is not needed
        mRequest = appMng.create(interface, operation, true); // embedded 
    }
    else
    {
        QString interface("Fetch");
        QString serviceName("com.nokia.services.phonebookservices"); 
        mRequest = appMng.create(serviceName, interface, operation, true); // embedded 
    }
    if ( mRequest )
    {
        CNT_LOG_ARGS("Created request.")
        
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
        
        args << aDetailType;
        args << aDetailValue;
        if ( !aDetailSubtype.isEmpty() )
        {
            args << aDetailSubtype;
        }
        
        mRequest->setArguments(args); 
        mRequest->send();
        CNT_LOG_ARGS("Sent request.")
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request.")
    }
    CNT_EXIT
}

void testPbkServices::launchContactCard( bool aNewInterface )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;

    // save test contact
    QContact contact;
    QContactName name;
    QContactPhoneNumber number;
    QContactManager mgr("symbian");
    
    name.setFirstName("Test launchContactCard");
    number.setNumber("0202221111");
    contact.saveDetail(&name);
    contact.saveDetail(&number);
    mgr.saveContact(&contact);
    
    // get contact id
    int id = contact.id().localId();
        
    QVariantList args; 
    XQApplicationManager appMng;
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsView");
        QString operation("openContactCard(int)");
        // interface name is not needed
        mRequest = appMng.create( interface, operation, true); // embedded 
    }
    else
    {
        QString interface("Fetch");
        QString operation("open(int)");
        QString serviceName("com.nokia.services.phonebookservices");
        mRequest = appMng.create(serviceName, interface, operation, true); // embedded 
    }
    if ( mRequest )
    {    
        CNT_LOG_ARGS("Request created.")
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
        
        args << id;
        
        mRequest->setArguments(args); 
        mRequest->send();
        CNT_LOG_ARGS("Request sent.")
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request")
    }
    CNT_EXIT
}

void testPbkServices::launchContactCard( bool aNewInterface, QString aDetailType, QString aDetailValue )
{
    CNT_ENTRY
    delete mRequest;
    mRequest=0;
        
    QVariantList args; 
    XQApplicationManager appMng;
    if ( aNewInterface )
    {
        QString interface("com.nokia.symbian.IContactsView");
        QString operation("openTemporaryContactCard(QString,QString)");
        // service name is not needed
        mRequest = appMng.create(interface, operation, true); // embedded 
    }
    else
    {
        QString interface("Fetch");
        QString operation("open(QString,QString)");
        QString serviceName("com.nokia.services.phonebookservices");
        mRequest = appMng.create(serviceName, interface, operation, true); // embedded 
    }
    if ( mRequest )
    {    
        CNT_LOG_ARGS("Request created.")
        // Result handlers 
        connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));

        args << aDetailType;
        args << aDetailValue;

        mRequest->setArguments(args); 
        mRequest->send();
    }
    else
    {
        CNT_LOG_ARGS("Failed to create request")
    }
    CNT_EXIT
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
