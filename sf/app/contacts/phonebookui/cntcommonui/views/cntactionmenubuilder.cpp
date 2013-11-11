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
#include "cntactionmenubuilder.h"
#include <qcontact.h>
#include "cntstringmapper.h"
#include <hbaction.h>
#include "cntactionpopup.h"

CntActionMenuBuilder::CntActionMenuBuilder( QContactLocalId aMyCardId ) : 
QObject(),
iMyCardId( aMyCardId ),
mContact( 0 ),
mMap( 0 )
    {
    mMap = new CntStringMapper();
    }

CntActionMenuBuilder::~CntActionMenuBuilder()
{
    delete mMap;
    mMap = 0;
    
    delete mContact;
    mContact = 0;
}
    
HbMenu* CntActionMenuBuilder::buildActionMenu( QContact& aContact )
{
    HbMenu* menu = new HbMenu();
    
    // Regular contact, NOT MyCard
    if ( aContact.localId() != iMyCardId )
        {
        QList<QContactActionDescriptor> actionDescriptors = aContact.availableActions();
        QStringList actions;
        foreach ( QContactActionDescriptor d, aContact.availableActions() )
            {
            actions << d.actionName();
            }
        
        if ( actions.contains("call", Qt::CaseInsensitive) && isSupportedDetails("call", aContact))
            {
            createCallAction( *menu, aContact );
            }
        
        if ( actions.contains("message", Qt::CaseInsensitive) && isSupportedDetails("message", aContact) )
            {
            createMessageAction( *menu, aContact );
            }
        
        if ( actions.contains("email", Qt::CaseInsensitive) )
            {
            createEmailAction( *menu, aContact );
            }
        
        if ( menu->actions().size() > 0 )
            menu->addSeparator();
        }
    
    // If contact is NOT MyCard OR MyCard is not empty (detail count is more than 4)
    if ( aContact.localId() != iMyCardId || aContact.details().size() > 4 )
        {
        menu->addAction(hbTrId("txt_common_menu_open"), this, SLOT(emitOpenContact()) );
        menu->addAction(hbTrId("txt_common_menu_edit"), this, SLOT(emitEditContact()) );
        menu->addAction(hbTrId("txt_phob_menu_delete_contact"), this, SLOT(emitDeleteContact()));
        }
    return menu;
}

HbMenu* CntActionMenuBuilder::actionMenu( QContact& aContact, QContactLocalId myCardId)
{
    iMyCardId = myCardId;
    mContact = new QContact( aContact );
    return buildActionMenu( aContact );
}

void CntActionMenuBuilder::emitOpenContact()
{
    emit openContact( *mContact );
}

void CntActionMenuBuilder::emitEditContact()
{
    emit editContact( *mContact );
}

void CntActionMenuBuilder::emitDeleteContact()
{
    emit deleteContact( *mContact );
}

void CntActionMenuBuilder::emitCallContact()
{  
    QContactDetail detail = mContact->preferredDetail("call");
    if (!detail.isEmpty())
        {
        emit performContactAction( *mContact, detail, "call" );
        }
    else if (mContact->details<QContactPhoneNumber>().count() == 1 )
    {
        mContact->setPreferredDetail("call", mContact->details<QContactPhoneNumber>().first());
        emit performContactAction(*mContact, mContact->details<QContactPhoneNumber>().first(), "call");
    }
    else 
        {
        CntActionPopup *actionPopup = new CntActionPopup(mContact);
        actionPopup->showActionPopup("call");
        connect( actionPopup, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)), this, 
                SLOT(emitContactaction(QContact&,QContactDetail, QString)));
        }
}

void CntActionMenuBuilder::emitSmsContact()
{
    QContactDetail detail = mContact->preferredDetail("message");
    if (!detail.isEmpty())
    {
        emit performContactAction( *mContact, detail, "message" );
    }
    else if (mContact->details<QContactPhoneNumber>().count() == 1 )
    {
        mContact->setPreferredDetail("message", mContact->details<QContactPhoneNumber>().first());
        emit performContactAction(*mContact, mContact->details<QContactPhoneNumber>().first(), "message");
    }
    else 
    {
        CntActionPopup *actionPopup = new CntActionPopup(mContact);
        actionPopup->showActionPopup("message");
        connect( actionPopup, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)), this, 
                SLOT(emitContactaction(QContact&,QContactDetail, QString)));
    }
}

void CntActionMenuBuilder::emitMailContact()
{
    QContactDetail detail = mContact->preferredDetail("email");
    if (!detail.isEmpty())
        {
        emit performContactAction( *mContact,detail, "email" );
        }
    else if (mContact->details<QContactEmailAddress>().count() == 1 )
    {
        mContact->setPreferredDetail("email", mContact->details<QContactEmailAddress>().first());
        emit performContactAction(*mContact, mContact->details<QContactEmailAddress>().first(), "email");
    }
    else 
        {
        CntActionPopup *actionPopup = new CntActionPopup(mContact);
        actionPopup->showActionPopup("email");
        connect( actionPopup, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)), this, 
                SLOT(emitContactaction(QContact&,QContactDetail, QString)));
        }
}

void CntActionMenuBuilder::emitContactaction(QContact& aContact,QContactDetail contactDetail, QString aAction)
{
    emit performContactAction( aContact,contactDetail, aAction);
}

void CntActionMenuBuilder::createCallAction( HbMenu& aMenu, QContact& aContact )
{
    // Create call action
    QContactDetail detail = aContact.preferredDetail("call");
    QContactPhoneNumber number = detail.isEmpty() ? aContact.detail<QContactPhoneNumber>() : detail;
    QString context = number.contexts().isEmpty() ? QString() : number.contexts().first();
    QString subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();

    aMenu.addAction( mMap->getItemSpecificMenuLocString( subtype, context ), this, SLOT(emitCallContact()) );
}

void CntActionMenuBuilder::createEmailAction( HbMenu& aMenu, QContact& aContact )
{
    // Create email action
    QContactDetail detail = aContact.preferredDetail("email");
    QContactEmailAddress email = detail.isEmpty() ? aContact.detail<QContactEmailAddress>() : detail;
    QString context = email.contexts().isEmpty() ? QString() : email.contexts().first();
       
    aMenu.addAction( mMap->getItemSpecificMenuLocString( email.definitionName(), context), this, SLOT(emitMailContact()) );
}

void CntActionMenuBuilder::createMessageAction( HbMenu& aMenu, QContact& aContact )
{
    Q_UNUSED( aContact );
    
    aMenu.addAction(hbTrId("txt_phob_menu_send_message"), this, SLOT(emitSmsContact()));
}

bool CntActionMenuBuilder::isSupportedDetails( const QString &actionName, const QContact &contact )
{
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    if (actionDescriptors.isEmpty())
        {
        return false;
        }
    
    QContactAction* contactAction = QContactAction::action(actionDescriptors.first()); 
    QList<QContactDetail> details = contactAction->supportedDetails(contact);

    delete contactAction;
    
    for (int i = 0; i < details.count(); i++)
        {
        if (contact.details().contains(details[i]))
            {
            return true;
            }
        }
    
    return false;
}
    
// End of File
