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
#include "cntdateeditorviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"

#include <qcontactdetail.h>
#include <qcontactbirthday.h>
#include <qcontactanniversary.h>

#include <hbabstractviewitem.h>
#include <hbabstractitemview.h>
#include <hbdataformmodel.h>
#include <hbwidget.h>
#include <hbdatetimepicker.h>
#include <hbpushbutton.h>
#include <hbdialog.h>
#include <hbdatetimepicker.h>
#include <hblabel.h>
#include <hbaction.h>

CntDateEditorViewItem::CntDateEditorViewItem( QGraphicsItem* aParent ) : 
    CntDetailViewItem( aParent ),
    mButton(NULL)
{
    mLocale = QLocale::system();
}

CntDateEditorViewItem::~CntDateEditorViewItem()
{
}

HbAbstractViewItem* CntDateEditorViewItem::createItem()
{
    return new CntDateEditorViewItem( *this );
}

void CntDateEditorViewItem::clicked()
{
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();

    if ( detail.definitionName() == QContactBirthday::DefinitionName )
    {
        QContactBirthday bd = detail;
        if (bd.date().isNull())
        {
            editDate( QDate::currentDate(), hbTrId("txt_phob_formlabel_birthday") );
        }
        else
        {
            editDate( bd.date(), hbTrId("txt_phob_formlabel_birthday") );
        }
    }

    if ( detail.definitionName() == QContactAnniversary::DefinitionName )
    {
        QContactAnniversary anniversary = detail;
        if (anniversary.originalDate().isNull())
        {
            editDate( QDate::currentDate(), hbTrId("txt_phob_formlabel_anniversary") );
        }
        else
        {
            editDate( anniversary.originalDate(), hbTrId("txt_phob_formlabel_anniversary") );
        }
    }
}

HbWidget* CntDateEditorViewItem::createCustomWidget()
{
    mButton = new HbPushButton();
    connect( mButton, SIGNAL(clicked(bool)), this, SLOT(clicked()) );

    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );

    QContactDetail detail = item->detail();
    QString text(hbTrId("txt_phob_formlabel_val_formlabel_val_no_date_set"));
    QString buttonObjName;

    if ( detail.definitionName() == QContactBirthday::DefinitionName )
    {
        buttonObjName = detail.definitionName() + " button";
        QContactBirthday birthday = detail;
        if ( !birthday.isEmpty() )
        {
            text = mLocale.toString( birthday.date() );
        }
    }

    if ( detail.definitionName() == QContactAnniversary::DefinitionName )
    {
        buttonObjName = detail.definitionName() + " button";
        QContactAnniversary anniversary = detail;
        if ( !anniversary.isEmpty() )
        {
            text = mLocale.toString( anniversary.originalDate() );
        }
    }
    mButton->setText( text );
    // Naming UI components for automation testability
    mButton->setObjectName(buttonObjName);
    return mButton;
}

void CntDateEditorViewItem::editDate( QDate aCurrent, QString aTitle )
{
    HbDialog *popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbPopup::NoTimeout);
    popup->setAttribute(Qt::WA_DeleteOnClose, true);

    HbDateTimePicker *picker = new HbDateTimePicker( popup );
    picker->setDisplayFormat( mLocale.dateFormat(QLocale::ShortFormat) );
    picker->setDateRange(CNT_DATEPICKER_FROM, CNT_DATEPICKER_TO );
    picker->setDate( aCurrent );

    HbLabel *headingText = new HbLabel( popup );
    headingText->setPlainText( aTitle );

    popup->setHeadingWidget(headingText);
    popup->setContentWidget(picker);
    popup->addAction(new HbAction(hbTrId("txt_common_button_ok"), popup));
    popup->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popup));
    
    popup->open(this, SLOT(handleEditDate(HbAction*)));
}

void CntDateEditorViewItem::changeDate( QDate aNewDate )
{
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();

    QString buttonText("No date set"); // loc missing
    if ( detail.definitionName() == QContactBirthday::DefinitionName )
    {
        QContactBirthday bd = detail;
        if ( aNewDate != bd.date() )
        {
            bd.setDate( aNewDate );
            item->setDetail( bd );
        }
        buttonText = mLocale.toString( aNewDate );
    }

    if ( detail.definitionName() == QContactAnniversary::DefinitionName )
    {
        QContactAnniversary anniversary = detail;
        if ( aNewDate != anniversary.originalDate() )
        {
            anniversary.setOriginalDate( aNewDate );
            item->setDetail( anniversary );
        }
        buttonText = mLocale.toString( aNewDate );
    }

    mButton->setText( buttonText );
}

void CntDateEditorViewItem::handleEditDate( HbAction *aAction )
{
    HbDialog *popup = static_cast<HbDialog*>(sender());
    
    if (popup && aAction == popup->actions().first())
    {
        QDate date = static_cast<HbDateTimePicker*>(popup->contentWidget())->date();
        changeDate(date);
    }
            
}

// End of File
