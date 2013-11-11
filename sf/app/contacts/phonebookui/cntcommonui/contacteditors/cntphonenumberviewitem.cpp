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
#include "cntdetailconst.h"
#include "cntphonenumberviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntcommondetailviewitem.h"
#include "cntdetaileditormodel.h"

#include <qdebug.h>
#include <qcontactphonenumber.h>
#include <qcontactonlineaccount.h>
#include <qcontactdetail.h>
#include <qstandarditemmodel.h>

#include <hbmainwindow.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <hbabstractviewitem.h>
#include <hbabstractitemview.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbinputstandardfilters.h>

CntPhoneNumberViewItem::CntPhoneNumberViewItem( QGraphicsItem* aParent ) : 
CntDetailViewItem( aParent ),
    mItem(NULL)
    {
    }

CntPhoneNumberViewItem::~CntPhoneNumberViewItem()
    {
    }

HbAbstractViewItem* CntPhoneNumberViewItem::createItem()
    {
    return new CntPhoneNumberViewItem( *this );
    }

void CntPhoneNumberViewItem::indexChanged( int aIndex )
    {
    QString subType = mItem->comboBox()->itemData( aIndex, DetailSubType ).toString();
    QString context = mItem->comboBox()->itemData( aIndex, DetailContext ).toString();
    
    // check that if current QContactDetail contains the changed subtype
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QStringList contextList;
    if ( !context.isEmpty() )
        contextList << context;
    
    // check if we switched to QContactOnlineAccount subtype
    if ( isOnlineAccount(subType) )
        {
        qDebug() << "QContactOnlineAccount(" << subType << ")";
        constructOnlineAccount( item, subType, contextList );
        }
    else
        {
        qDebug() << "QContactPhoneNumber(" << subType << ")";
        constructPhoneNumber( item, subType, contextList );
        }
    }

void CntPhoneNumberViewItem::textChanged( QString aText )
    {
    qDebug() << "textChanged( " << aText << " )";
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
        QContactPhoneNumber number = detail;
        number.setNumber( aText );
        item->setDetail( number );
        }
    
    if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
        QContactOnlineAccount account = detail;
        account.setAccountUri( aText );
        item->setDetail( account );
        }
    }

HbWidget* CntPhoneNumberViewItem::createCustomWidget()
    {
    mItem = new CntCommonDetailViewItem(this);
    
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    // select correct index
    QContactDetail detail = item->detail();
    QStringList context;
    QString subType;
    QString value;
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
        mItem->editor()->setMaxLength( CNT_PHONENUMBER_EDITOR_MAXLENGTH );
        QContactPhoneNumber phone = detail;
        subType = phone.subTypes().isEmpty() ? "" : phone.subTypes().first();
        if ( !phone.contexts().isEmpty() )
            context << phone.contexts().first();
        value = phone.number();
        }
    
    if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
        mItem->editor()->setMaxLength( CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH );
        QContactOnlineAccount account = detail;
        subType = account.subTypes().isEmpty() ? "" : account.subTypes().first();
        if (!account.contexts().isEmpty())
            context << account.contexts().first();
        value = account.accountUri();
        }
    
    mItem->editor()->setText( value );
    constructSubtypeModel( subType, context );
    
    connect( mItem->comboBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
    connect( mItem->editor(), SIGNAL(textChanged(QString)),this, SLOT(textChanged(QString)) );
    
    // Naming UI components for automation testability
    QString editorObjName = detail.definitionName() + " line edit %1";
    mItem->editor()->setObjectName(editorObjName.arg(modelIndex().row()));
    
    QString comboBoxObjName = detail.definitionName() + " combo box %1";
    mItem->comboBox()->setObjectName(comboBoxObjName.arg(modelIndex().row()));
    return mItem;
    }

void CntPhoneNumberViewItem::constructPhoneNumber( CntDetailModelItem* aItem, QString aSubType, QStringList aContext )
    {
    mItem->editor()->setInputMethodHints( Qt::ImhDialableCharactersOnly );
    mItem->editor()->setMaxLength( CNT_PHONENUMBER_EDITOR_MAXLENGTH );
            
    QContactDetail detail = aItem->detail();
    
    // check if the detail type needs to changed
    if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
        // also we need to remove the old online account and replace it
        // with phonenumber
        CntDetailEditorModel* model = static_cast<CntDetailEditorModel*>( itemView()->model() );
        QContact* contact = model->contact();
        contact->removeDetail( &detail );
                
        QContactPhoneNumber number;
        number.setSubTypes( aSubType );
        number.setContexts( aContext );
        number.setNumber( mItem->editor()->text() );
        aItem->setDetail( number );
        }
    else
        {
        QContactPhoneNumber number = detail;
        number.setSubTypes( aSubType );
        number.setContexts( aContext );
        aItem->setDetail( number );
        }
    }

void CntPhoneNumberViewItem::constructOnlineAccount( CntDetailModelItem* aItem, QString aSubType, QStringList aContext )
    {
    mItem->editor()->setMaxLength( CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH );
    mItem->editor()->setInputMethodHints( Qt::ImhUrlCharactersOnly );
    
    QContactDetail detail = aItem->detail();
    // check if the detail type needs to changed
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
        // also we need to remove the old phonenumber and replace it
        // with online account
        CntDetailEditorModel* model = static_cast<CntDetailEditorModel*>( itemView()->model() );
        QContact* contact = model->contact();
        contact->removeDetail( &detail );
        
        QContactOnlineAccount account;
        account.setSubTypes( aSubType );
        account.setContexts( aContext );
        account.setAccountUri( mItem->editor()->text() );
        aItem->setDetail( account );
        }
    else
        {
        QContactOnlineAccount account = detail;
        account.setSubTypes( aSubType );
        account.setContexts( aContext );
        account.setAccountUri( mItem->editor()->text() );
        aItem->setDetail( account );
        }
    }

void CntPhoneNumberViewItem::constructSubtypeModel( QString aSubType, QStringList aContext )
    {
    QStandardItemModel* model = new QStandardItemModel();
        
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
        
    QString subTypeMobile = QContactPhoneNumber::SubTypeMobile;
    QString subTypeLandline = QContactPhoneNumber::SubTypeLandline;
    QString subTypeFax = QContactPhoneNumber::SubTypeFax;
    QString subTypePager = QContactPhoneNumber::SubTypePager;
    QString subTypeCarPhone = QContactPhoneNumber::SubTypeCar;
    QString subTypeDtmf = QContactPhoneNumber::SubTypeDtmfMenu;
    QString subTypeAssistant = QContactPhoneNumber::SubTypeAssistant;
        
    QString subTypeInternet = QContactOnlineAccount::SubTypeSipVoip;
    QString subTypeSIP = QContactOnlineAccount::SubTypeSip;
    QString subTypeVideoShare = QContactOnlineAccount::SubTypeVideoShare;
    
    QStandardItem *mobile = new QStandardItem;
    mobile->setText(hbTrId("txt_phob_formlabel_val_mobile"));
    mobile->setData(subTypeMobile, DetailSubType);
    mobile->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(mobile);
       
    QStandardItem *mobilehome = new QStandardItem;
    mobilehome->setText(hbTrId("txt_phob_formlabel_val_mobile_home"));
    mobilehome->setData(subTypeMobile, DetailSubType);
    mobilehome->setData(contextHome, DetailContext);
    mobilehome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(mobilehome);
       
    QStandardItem *mobilework = new QStandardItem;
    mobilework->setText(hbTrId("txt_phob_formlabel_val_mobile_work"));
    mobilework->setData(subTypeMobile, DetailSubType);
    mobilework->setData(contextWork, DetailContext);
    mobilework->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(mobilework);

    QStandardItem *land = new QStandardItem;
    land->setText(hbTrId("txt_phob_formlabel_val_phone"));
    land->setData(subTypeLandline, DetailSubType);
    land->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(land);
       
    QStandardItem *landhome = new QStandardItem;
    landhome->setText(hbTrId("txt_phob_formlabel_val_phone_home"));
    landhome->setData(subTypeLandline, DetailSubType);
    landhome->setData(contextHome, DetailContext);
    landhome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(landhome);
       
    QStandardItem *landwork = new QStandardItem;
    landwork->setText(hbTrId("txt_phob_formlabel_val_phone_work"));
    landwork->setData(subTypeLandline, DetailSubType);
    landwork->setData(contextWork, DetailContext);
    landwork->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(landwork);
    
    QStandardItem *fax = new QStandardItem;
    fax->setText(hbTrId("txt_phob_formlabel_val_fax"));
    fax->setData(subTypeFax, DetailSubType);
    fax->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(fax);
    
    QStandardItem *faxhome = new QStandardItem;
    faxhome->setText(hbTrId("txt_phob_formlabel_val_fax_home"));
    faxhome->setData(subTypeFax, DetailSubType);
    faxhome->setData(contextHome, DetailContext);
    faxhome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(faxhome);
    
    QStandardItem *faxwork = new QStandardItem;
    faxwork->setText(hbTrId("txt_phob_formlabel_val_fax_work"));
    faxwork->setData(subTypeFax, DetailSubType);
    faxwork->setData(contextWork, DetailContext);
    faxwork->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(faxwork);
        
    QStandardItem *pager = new QStandardItem;
    pager->setText(hbTrId("txt_phob_formlabel_val_pager"));
    pager->setData(subTypePager, DetailSubType);
    pager->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(pager);
    
    QStandardItem *assistant = new QStandardItem;
    assistant->setText(hbTrId("txt_phob_formlabel_val_assistant"));
    assistant->setData(subTypeAssistant, DetailSubType);
    assistant->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(assistant);
        
    QStandardItem *carPhone = new QStandardItem;
    carPhone->setText(hbTrId("txt_phob_formlabel_val_car"));
    carPhone->setData(subTypeCarPhone, DetailSubType);
    carPhone->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(carPhone);
       
    QStandardItem *internet = new QStandardItem;
    internet->setText(hbTrId("txt_phob_formlabel_val_internet_telephone"));
    internet->setData(subTypeInternet, DetailSubType);
    internet->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(internet);
      
    QStandardItem *internethome = new QStandardItem;
    internethome->setText(hbTrId("txt_phob_formlabel_val_internet_telephone_home"));
    internethome->setData(subTypeInternet, DetailSubType);
    internethome->setData(contextHome, DetailContext);
    internethome->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(internethome);

    QStandardItem *internetwork = new QStandardItem;
    internetwork->setText(hbTrId("txt_phob_formlabel_val_internet_telephone_work"));
    internetwork->setData(subTypeInternet, DetailSubType);
    internetwork->setData(contextWork, DetailContext);
    internetwork->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(internetwork);
          
    QStandardItem *sip = new QStandardItem;
    sip->setText(hbTrId("txt_phob_formlabel_sip"));
    sip->setData(subTypeSIP, DetailSubType);
    sip->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(sip);
         
    mItem->comboBox()->setModel( model );
    
    // search the selected index to be set
    QString context = aContext.isEmpty() ? "" : aContext.first();
    for ( int i(0); i < model->rowCount(); i++ )
        {
        if ( model->item(i)->data( DetailSubType ).toString() == aSubType &&
             model->item(i)->data( DetailContext ).toString() == context )
            {
            mItem->comboBox()->setCurrentIndex( i );
            if ( isOnlineAccount( aSubType ) )
            {
                mItem->editor()->setMaxLength( CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH );
                mItem->editor()->setInputMethodHints( Qt::ImhUrlCharactersOnly );
            }
            else
            {
                mItem->editor()->setInputMethodHints( Qt::ImhDialableCharactersOnly );
                mItem->editor()->setMaxLength( CNT_PHONENUMBER_EDITOR_MAXLENGTH );
            }
                
            break;
            }
        }
    }

bool CntPhoneNumberViewItem::isOnlineAccount( QString aSubtype )
{
    return (aSubtype == QContactOnlineAccount::SubTypeSip || 
            aSubtype == QContactOnlineAccount::SubTypeSipVoip ||
            aSubtype == QContactOnlineAccount::SubTypeVideoShare );
}
// End of File
