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
#include "cnteditviewitembuilder.h"
#include "cnteditviewlistmodel.h"
#include "cnteditviewdetailitem.h"
#include "cntviewparams.h"
#include "cntstringmapper.h"
#include "cntdetailorderinghelper.h"
#include <hbnumbergrouping.h>
#include <QDir>

CntEditViewItemBuilder::CntEditViewItemBuilder() :
mMap( new CntStringMapper() )
{    
}

CntEditViewItemBuilder::~CntEditViewItemBuilder()
{
    delete mMap;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::phoneNumberItems(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    QList<QContactPhoneNumber> numbers = CntDetailOrderingHelper::getOrderedSupportedPhoneNumbers(aContact);
    // template for editor launcher
    if ( numbers.isEmpty() ) 
    {
        QContactPhoneNumber number;
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem( 
                number,
                QContactPhoneNumber::FieldNumber,
                phoneNumberEditorView );
        detailItem->addText( hbTrId("txt_phob_list_add_phone_number") );
        detailItem->addIcon( HbIcon(mMap->getContactEditorIconString(QContactPhoneNumber::DefinitionName, "")) );
        list.append( detailItem );
    }
    // existing phonenumber(s)
    else  
    {
        foreach ( QContactPhoneNumber number, numbers )
        {
            QString context = number.contexts().isEmpty() ? "" : number.contexts().first();
            QString subtype = number.subTypes().first();

            CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                    number,
                    QContactPhoneNumber::FieldNumber,
                    phoneNumberEditorView);

            detailItem->addIcon( mMap->getContactEditorIconString(subtype, context) );
            detailItem->addText( mMap->getContactEditorLocString(subtype, context) );
            /*
             * Internationalization support, activate the following code 
             * when support available from Orbit
             */
            //detailItem->addText( HbNumberGrouping::formatPhoneNumber(number.number())); 
            detailItem->addText( number.number());
            list.append( detailItem );
        }
    }
    
    QList<QContactOnlineAccount> urls = CntDetailOrderingHelper::getOrderedSupportedOnlineAccounts(aContact);
    // template for editor launcher
    if ( !urls.isEmpty() ) 
    {
        foreach ( QContactOnlineAccount number, urls )
        {
            if ( !number.subTypes().isEmpty() )
            {
                QString context = number.contexts().isEmpty() ? "" : number.contexts().first();
                QString subtype = number.subTypes().first();
                        
                CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                        number,
                        QContactOnlineAccount::FieldAccountUri,
                        phoneNumberEditorView);
                detailItem->addIcon( mMap->getContactEditorIconString(subtype, context) );
                
                if ( subtype == QContactOnlineAccount::SubTypeSip )
                {
                    detailItem->addText( hbTrId("txt_phob_formlabel_sip") );
                    detailItem->addText( number.accountUri() );
                    list.append( detailItem );
                }
                else if ( subtype == QContactOnlineAccount::SubTypeSipVoip )
                {
                    if ( context == QContactOnlineAccount::ContextHome )
                        detailItem->addText( hbTrId("txt_phob_formlabel_internet_telephone_home") );
                    else if ( context == QContactOnlineAccount::ContextWork )
                        detailItem->addText( hbTrId("txt_phob_formlabel_internet_telephone_work") );
                    else
                        detailItem->addText( hbTrId("txt_phob_formlabel_internet_telephone") );
                    
                    detailItem->addText( number.accountUri() );
                    list.append( detailItem );
                }
                else
                {
                    /* Other subtypes of QContactOnlineAccount are not supported by UI */
                    delete detailItem;
                }
            }
        }
    }
        
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::emailAddressItems(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    QList<QContactEmailAddress> emails = CntDetailOrderingHelper::getOrderedEmailAccounts(aContact);
    // template for editor launcher
    if ( emails.isEmpty() ) 
    {
        QContactEmailAddress email;
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                email,
                QContactEmailAddress::FieldEmailAddress,
                emailEditorView);
        detailItem->addText( hbTrId("txt_phob_list_add_email_address") );
        detailItem->addIcon( HbIcon(mMap->getContactEditorIconString(QContactEmailAddress::DefinitionName, "")) );
        list.append( detailItem );
    }
    // existing email(s)
    else
    {
        foreach ( QContactEmailAddress address, emails )
        {
            QString context = address.contexts().isEmpty() ? "" : address.contexts().first();
                
            CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                    address,
                    QContactEmailAddress::FieldEmailAddress,
                    emailEditorView);
            detailItem->addIcon( mMap->getContactEditorIconString(QContactEmailAddress::DefinitionName, context) );
            
            if ( context == QContactEmailAddress::ContextHome )
                detailItem->addText(hbTrId("txt_phob_formlabel_email_home"));
            else if ( context == QContactEmailAddress::ContextWork )
                detailItem->addText(hbTrId("txt_phob_formlabel_email_work"));
            else
                detailItem->addText( hbTrId("txt_phob_formlabel_email") );
            
            detailItem->addText( address.emailAddress() );
            list.append( detailItem );
        }
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::addressItems(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    QList<QContactAddress> addresses = aContact.details<QContactAddress>();
    
    if ( addresses.isEmpty() )
    {
        QStringList fieldList;
        fieldList << QContactAddress::FieldStreet << QContactAddress::FieldPostcode;
        fieldList << QContactAddress::FieldLocality << QContactAddress::FieldRegion << QContactAddress::FieldCountry;
              
        QContactAddress addr;
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                addr,
                fieldList,
                addressEditorView);
        detailItem->addText( hbTrId("txt_phob_list_add_address") );
        detailItem->addIcon( HbIcon(mMap->getContactEditorIconString(QContactAddress::DefinitionName, "")) );
        list.append( detailItem );
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::urlItems(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    QList<QContactUrl> urls = CntDetailOrderingHelper::getOrderedUrls(aContact);
    // template for editor launcher
    if ( urls.isEmpty() ) 
    {
        QContactUrl url;
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                url,
                QContactUrl::FieldUrl,
                urlEditorView );
        detailItem->addText( hbTrId("txt_phob_list_add_url_address") );
        detailItem->addIcon( HbIcon(mMap->getContactEditorIconString(QContactUrl::DefinitionName, "")) );
        list.append( detailItem );
    }
    // existing url(s)
    else  
    {
        foreach ( QContactUrl url, urls )
        {
            QString context = url.contexts().isEmpty() ? "" : url.contexts().first();
            
            CntEditViewDetailItem* detailItem = new CntEditViewDetailItem( 
                    url, 
                    QContactUrl::FieldUrl,
                    urlEditorView );
            detailItem->addIcon( mMap->getContactEditorIconString(QContactUrl::DefinitionName, context) );
            
            if ( context == QContactUrl::ContextHome )
                detailItem->addText( hbTrId("txt_phob_dblist_url_home") );
            else if ( context == QContactUrl::ContextWork )
                detailItem->addText( hbTrId("txt_phob_dblist_url_work") );
            else
                detailItem->addText( hbTrId("txt_phob_dblist_url") );
            
            detailItem->addText( url.url() );
            list.append( detailItem );
        }
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::addressDetails(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    // Address 
    foreach( QContactAddress addr, CntDetailOrderingHelper::getOrderedAddresses(aContact) )
    {
        QStringList fieldList;
        fieldList << QContactAddress::FieldStreet << QContactAddress::FieldPostcode;
        fieldList << QContactAddress::FieldLocality << QContactAddress::FieldRegion << QContactAddress::FieldCountry;
        
        QStringList valueList;
        foreach ( QString str, fieldList )
        {
            QString value = addr.value( str );
            if ( !value.isEmpty() )
            {
                valueList << addr.value( str );
            }
        }
            
        QString address = valueList.join(", ").trimmed();
        if ( !address.isEmpty() )
        {
            QString context = addr.contexts().isEmpty() ? "" : addr.contexts().first();
            CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                    addr,
                    fieldList,
                    addressEditorView);
            
            if ( context == QContactAddress::ContextHome )
                detailItem->addText(hbTrId("txt_phob_formlabel_address_home"));
            else if ( context == QContactAddress::ContextWork )
                detailItem->addText(hbTrId("txt_phob_formlabel_address_work"));
            else
                detailItem->addText(hbTrId("txt_phob_formlabel_address"));
            
            detailItem->addText( address );
            list.append( detailItem );
        }
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::companyDetails(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    // Company
    QContactOrganization org = aContact.detail<QContactOrganization>();
    QStringList detailList;
    if ( !org.title().isEmpty())
        detailList << org.title();
    if ( !org.name().isEmpty())
        detailList << org.name();
    if ( !org.department().isEmpty())
        detailList << org.department().join(", ").trimmed();

    QString company = detailList.join(", ").trimmed(); 
    if ( !company.isEmpty() )
    {
        QStringList fields;
        fields << QContactOrganization::FieldTitle;
        fields << QContactOrganization::FieldName;
        fields << QContactOrganization::FieldDepartment;
        
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                org,
                fields,
                companyEditorView);
        detailItem->addText( hbTrId("txt_phob_formlabel_company_details") );
        detailItem->addText( company );
        list.append( detailItem );
    }
       
    // Assistant
    QString assistant = org.assistantName();
    if ( !assistant.isEmpty() )
    {
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                org,
                QContactOrganization::FieldAssistantName,
                companyEditorView);
        detailItem->addText( hbTrId("txt_phob_formlabel_assistant") );
        detailItem->addText( assistant );
        list.append( detailItem );
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::dateDetails(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    // Birthday
    QContactBirthday bd = aContact.detail<QContactBirthday>();
    if ( bd.date().isValid() )
    {
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem( 
                bd,
                QContactBirthday::FieldBirthday,
                dateEditorView);
        detailItem->addText( hbTrId("txt_phob_formlabel_birthday") );
        detailItem->addText(QLocale::system().toString( bd.date() ));
        list.append( detailItem );
    }
        
    // Anniversary
    QContactAnniversary anniversary = aContact.detail<QContactAnniversary>();
    if ( anniversary.originalDate().isValid() )
        {
            CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                    anniversary, 
                    QContactAnniversary::FieldOriginalDate,
                    dateEditorView);
            detailItem->addText( hbTrId("txt_phob_formlabel_anniversary") );
            detailItem->addText( QLocale::system().toString( anniversary.originalDate() ));
            list.append( detailItem );
        }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::noteDetails(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    // Note
    foreach ( QContactNote note, aContact.details<QContactNote>()) 
    {
        QString text = note.note();
        if ( !text.isEmpty() )
        {
            CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                    note,
                    QContactNote::FieldNote,
                    noteEditorView);
            detailItem->addText( hbTrId("txt_phob_formlabel_note") );
            detailItem->addText( text );
            list.append( detailItem );
        }
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::familyDetails(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    // Spouse
    QContactFamily family = aContact.detail<QContactFamily>();
    QString spouse = family.spouse();
    if ( !spouse.isEmpty() )
    {
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                family,
                QContactFamily::FieldSpouse,
                familyDetailEditorView);
        detailItem->addText( hbTrId("txt_phob_formlabel_spouse") );
        detailItem->addText( spouse );
        list.append( detailItem );
    }
        
    // Children
    QString children = family.children().join(", ");
    if ( !children.isEmpty() )
    {
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                family,
                QContactFamily::FieldChildren,
                familyDetailEditorView);
        detailItem->addText( hbTrId("txt_phob_formlabel_children") );
        detailItem->addText( children );
        list.append( detailItem );
    }
    return list;
}

QList<CntEditViewItem*> CntEditViewItemBuilder::ringtoneDetails(QContact& aContact)
{
    QList<CntEditViewItem*> list;
    // Ring Tone
    QContactRingtone ringtone = aContact.detail<QContactRingtone>();
    QUrl ringtoneUrl = ringtone.audioRingtoneUrl();
    if ( !ringtoneUrl.isEmpty() )
    {
        CntEditViewDetailItem* detailItem = new CntEditViewDetailItem(
                ringtone,
                QContactRingtone::FieldAudioRingtoneUrl,
                ringToneFetcherView);
        detailItem->addText( hbTrId("txt_phob_formlabel_ringing_tone") );
        QFileInfo ringtoneFileInfo(ringtoneUrl.toString());
        QString ringtoneFileName = ringtoneFileInfo.fileName();
        detailItem->addText( ringtoneFileName);        
        list.append( detailItem );
    }
        
    return list;
}

bool CntEditViewItemBuilder::isTemplateItem( const QString& aDefinition )
{
    QStringList templateItems;
    templateItems << QContactPhoneNumber::DefinitionName;
    templateItems << QContactEmailAddress::DefinitionName;
    templateItems << QContactAddress::DefinitionName;
    templateItems << QContactUrl::DefinitionName;
    
    return templateItems.contains( aDefinition );
}

void CntEditViewItemBuilder::removeDetail( QContact& aContact, QContactDetail& aDetail, QStringList aFields )
{
    if ( aDetail.definitionName() == QContactOrganization::DefinitionName )
    {   
        QContactOrganization org = aContact.detail<QContactOrganization>();
        if ( aFields.contains(QContactOrganization::FieldAssistantName) )
        {
            org.setAssistantName(QString()); // empty
        }
        else
        {
            org.setTitle( QString() );
            org.setName( QString() );
            org.setDepartment( QStringList() );
        }
        aContact.saveDetail(&org);
        
        if ( org.assistantName().isEmpty() && 
             org.title().isEmpty() && 
             org.name().isEmpty() && 
             org.department().isEmpty() )
        {
            aContact.removeDetail(&org);
        }
    }

    else if ( aDetail.definitionName() == QContactFamily::DefinitionName )
    {
        QContactFamily family = aContact.detail<QContactFamily>();
        if ( aFields.contains(QContactFamily::FieldSpouse) )
        {
            family.setSpouse( QString() );
        }
        else
        {
            family.setChildren( QStringList() );
        }
        
        aContact.saveDetail( &family );
        
        if ( family.spouse().isEmpty() && family.children().isEmpty() )
        {
            aContact.removeDetail( &family );
        }
    }
    else
    {
        aContact.removeDetail( &aDetail );
    } 
}
// End of File
