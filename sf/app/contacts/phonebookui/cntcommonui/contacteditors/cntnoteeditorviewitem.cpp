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
#include "cntnoteeditorviewitem.h"
#include "cntdetailconst.h"
#include "cntdetailmodelitem.h"
#include <hbdataformmodel.h>
#include <hbabstractitemview.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include <qcontactnote.h>
#include <QGraphicsLinearLayout>

CntNoteEditorViewItem::CntNoteEditorViewItem( QGraphicsItem* aParent ) :
CntDetailViewItem(aParent),
mEdit(NULL),
mLayout(NULL)
    {
    }

CntNoteEditorViewItem::~CntNoteEditorViewItem()
    {
    }
    
HbAbstractViewItem* CntNoteEditorViewItem::createItem()
    {
    return new CntNoteEditorViewItem( *this );
    }

void CntNoteEditorViewItem::textChanged(const QString& aText)
    {
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactNote detail = item->detail();
    detail.setNote( aText );
    item->setDetail( detail );
    }

HbWidget* CntNoteEditorViewItem::createCustomWidget()
    {
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactNote detail = item->detail();

    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    HbWidget* widget = new HbWidget();
    
    mEdit = new HbLineEdit();
    mEdit->setInputMethodHints(Qt::ImhNoPredictiveText);
    
    mEdit->setMaxLength( CNT_NOTE_EDITOR_MAXLENGTH );
    mEdit->setMinRows( CNT_NOTE_EDITOR_MIN_ROWCOUNT );
    mEdit->setText( detail.note() );

    HbLabel* label = new HbLabel(hbTrId("txt_phob_formlabel_note"));
    mLayout->addItem(label);
    mLayout->addItem(mEdit);
    widget->setLayout( mLayout );
    
    connect( mEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)) );
    
    // Naming UI components for automation testability
    QString editorObjName = detail.definitionName() + " line edit %1";
    mEdit->setObjectName(editorObjName.arg(modelIndex().row()));
    return widget;
    }
// End of File
