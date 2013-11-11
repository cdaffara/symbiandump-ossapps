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
#include "cnturleditorviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"
#include "cntcommondetailviewitem.h"
#include <qstandarditemmodel.h>
#include <qcontacturl.h>
#include <hbdataformmodel.h>
#include <hbmainwindow.h>
#include <hbabstractitemview.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>

CntUrlEditorViewItem::CntUrlEditorViewItem( QGraphicsItem* aParent ) :
CntDetailViewItem( aParent ),
mItem(NULL)
{
}

CntUrlEditorViewItem::~CntUrlEditorViewItem()
{
}
    
HbAbstractViewItem* CntUrlEditorViewItem::createItem()
{
    return new CntUrlEditorViewItem(*this);
}

HbWidget* CntUrlEditorViewItem::createCustomWidget()
{
    mItem = new CntCommonDetailViewItem(this);
    mItem->editor()->setMaxLength( CNT_URL_EDITOR_MAXLENGTH );
    
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactUrl detail = item->detail();
    
    mItem->editor()->setInputMethodHints( Qt::ImhUrlCharactersOnly );
    
    constructSubTypeModel( detail.contexts() );
    
    if (detail.url().isEmpty())
    {
        mItem->editor()->setText( CNT_PREFILLED_URL );
    }
    else
    {
        mItem->editor()->setText( detail.url() );
    }
    
    connect( mItem->comboBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
    connect( mItem->editor(), SIGNAL(textChanged(QString)),this, SLOT(textChanged(QString)) );
    
    // Naming UI components for automation testability
    QString editorObjName = detail.definitionName() + " line edit %1";
    mItem->editor()->setObjectName(editorObjName.arg(modelIndex().row()));
    
    QString comboBoxObjName = detail.definitionName() + " combo box %1";
    mItem->comboBox()->setObjectName(comboBoxObjName.arg(modelIndex().row()));
    return mItem;
}
    
void CntUrlEditorViewItem::indexChanged( int aIndex )
{
    QString context = mItem->comboBox()->itemData( aIndex, DetailContext ).toString();
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QContactUrl url = item->detail();
    
    QStringList contextList;
    if ( !context.isEmpty() )
        contextList << context;
    
    url.setContexts( contextList );
    
    item->setDetail( url );
}

void CntUrlEditorViewItem::textChanged( QString aText )
{
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
        
    QContactUrl url = item->detail();
    url.setUrl( aText );
    item->setDetail( url );
}

void CntUrlEditorViewItem::constructSubTypeModel( QStringList aContext )
{
    QStandardItemModel* model = new QStandardItemModel();
            
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
    QString fieldAddress = QContactUrl::FieldUrl;
        
    QStandardItem *noContext = new QStandardItem;
    noContext->setText(hbTrId("txt_phob_formlabel_val_url"));
    noContext->setData(fieldAddress, DetailSubType);
    noContext->setData(CNT_URL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(noContext);
           
    QStandardItem *home = new QStandardItem;
    home->setText(hbTrId("txt_phob_formlabel_val_url_home"));
    home->setData(fieldAddress, DetailSubType);
    home->setData(contextHome, DetailContext);
    home->setData(CNT_URL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(home);
           
    QStandardItem *work = new QStandardItem;
    work->setText(hbTrId("txt_phob_formlabel_val_url_work"));
    work->setData(fieldAddress, DetailSubType);
    work->setData(contextWork, DetailContext);
    work->setData(CNT_URL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(work);
    
    mItem->comboBox()->setModel( model );
    
    // search the selected index to be set
    QString context = aContext.isEmpty() ? "" : aContext.first();
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
