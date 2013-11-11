/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "cpprofilenameeditdialog.h"

#include <hblineedit.h>
#include <hblabel.h>
#include <hbaction.h>

#include <QGraphicsLinearLayout>
#include <QScopedPointer>

CpProfileNameEditDialog::CpProfileNameEditDialog(QGraphicsItem *parent)
                                                 : HbDialog(parent)
{
    init();
}

CpProfileNameEditDialog::~CpProfileNameEditDialog()
{
}

void CpProfileNameEditDialog::init()
{
    setDismissPolicy(HbPopup::NoDismiss);
    setHeadingWidget(new HbLabel(hbTrId("txt_cp_title_edit_name"), this));
    
    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation(Qt::Vertical);
    HbWidget *contentWidget = new HbWidget(this);
    HbLabel *label = new HbLabel(hbTrId("txt_cp_heading_profile_name"));
    vLayout->addItem(label);
    mLineEdit = new HbLineEdit(this);
    mLineEdit->setFontSpec(HbFontSpec(HbFontSpec::Primary));    
    vLayout->addItem(mLineEdit);
    contentWidget->setLayout(vLayout);
    setContentWidget(contentWidget);    
    connect(mLineEdit, SIGNAL(contentsChanged()), this, SLOT(checkPrimaryAction()));
    
    setTimeout(NoTimeout);    
}

void CpProfileNameEditDialog::setLineEditText(const QString &text)
{
    mLineEdit->setText(text);
}

QString CpProfileNameEditDialog::lineEditText()
{
    return mLineEdit->text().trimmed();
}

void CpProfileNameEditDialog::setLineEditMaxLength(int length)
{
    mLineEdit->setMaxLength(length);
}

void CpProfileNameEditDialog::checkPrimaryAction()
{
    //the first button(number 0) is Ok Button. 
    HbAction *const primaryAction = qobject_cast<HbAction *>(actions().at(0));
    if (primaryAction) {        
        if (!mLineEdit->text().trimmed().isEmpty()) {
            primaryAction->setEnabled(true);            
        } else {        
            primaryAction->setEnabled(false);        
        }        
    }
}
