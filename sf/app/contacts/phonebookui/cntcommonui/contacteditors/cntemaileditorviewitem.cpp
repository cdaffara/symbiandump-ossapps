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
#include "cntemaileditorviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"
#include "cntcommondetailviewitem.h"

#include <qcontactdetail.h>
#include <qcontactemailaddress.h>
#include <qgraphicslinearlayout.h>
#include <hbwidget.h>
#include <hbabstractitemview.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <hbmainwindow.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <QStandardItemModel>

CntEmailEditorViewItem::CntEmailEditorViewItem( QGraphicsItem* aParent ) :
CntDetailViewItem( aParent ),
mItem(NULL)
    {
    }

CntEmailEditorViewItem::~CntEmailEditorViewItem()
    {
    
    }
    
HbAbstractViewItem* CntEmailEditorViewItem::createItem()
    {
    return new CntEmailEditorViewItem(*this);
    }

HbWidget* CntEmailEditorViewItem::createCustomWidget()
    {
    mItem = new CntCommonDetailViewItem(this);
    mItem->editor()->setMaxLength( CNT_EMAIL_EDITOR_MAXLENGTH );
    mItem->editor()->setInputMethodHints(Qt::ImhPreferLowercase);
    
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();
    
    mItem->editor()->setInputMethodHints( Qt::ImhEmailCharactersOnly );
    
    constructSubTypeModel( detail.contexts() );
    
    QContactEmailAddress address = detail;
    QString d = address.emailAddress();
    mItem->editor()->setText( address.emailAddress() );
    
    connect( mItem->comboBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
    connect( mItem->editor(), SIGNAL(textChanged(QString)),this, SLOT(textChanged(QString)) );
    
    // Naming UI components for automation testability
    QString editorObjName = detail.definitionName() + " line edit %1";
    mItem->editor()->setObjectName(editorObjName.arg(modelIndex().row()));
    
    QString comboBoxObjName = detail.definitionName() + " combo box %1";
    mItem->comboBox()->setObjectName(comboBoxObjName.arg(modelIndex().row()));
    return mItem;
    }
    
void CntEmailEditorViewItem::indexChanged( int aIndex )
    {
    QString context = mItem->comboBox()->itemData( aIndex, DetailContext ).toString();
        
    // check that if current QContactDetail contains the changed subtype
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QStringList contextList;
    if ( !context.isEmpty() )
        contextList << context;
    
    QContactEmailAddress address = item->detail();
    address.setContexts( contextList );
    item->setDetail( address );
    }

void CntEmailEditorViewItem::textChanged( QString aText )
    {
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QContactEmailAddress address = item->detail();
    address.setEmailAddress( aText );
    item->setDetail( address );
    }

void CntEmailEditorViewItem::constructSubTypeModel( QStringList aContext )
    {
    QStandardItemModel* model = new QStandardItemModel();
            
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
    QString fieldAddress = QContactEmailAddress::FieldEmailAddress;
        
    QStandardItem *noContext = new QStandardItem;
    noContext->setText(hbTrId("txt_phob_formlabel_val_email"));
    noContext->setData(fieldAddress, DetailSubType);
    noContext->setData(CNT_EMAIL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(noContext);
           
    QStandardItem *home = new QStandardItem;
    home->setText(hbTrId("txt_phob_formlabel_val_email_home"));
    home->setData(fieldAddress, DetailSubType);
    home->setData(contextHome, DetailContext);
    home->setData(CNT_EMAIL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(home);
           
    QStandardItem *work = new QStandardItem;
    work->setText(hbTrId("txt_phob_formlabel_val_email_work"));
    work->setData(fieldAddress, DetailSubType);
    work->setData(contextWork, DetailContext);
    work->setData(CNT_EMAIL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(work);
    
    mItem->comboBox()->setModel( model );
    
    QString context = aContext.isEmpty() ? "" : aContext.first();
    // search the selected index to be set
    for ( int i(0); i < model->rowCount(); i++ )
        {
        if ( model->item(i)->data( DetailContext ).toString() == context )
            {
            mItem->comboBox()->setCurrentIndex( i );
            break;
            }
        }
    }

// End of File
