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
 * Description: QT C++ based Class.
 * 
 */

#include "dlgsetselector.h"
#include <QtGui>

DlgSetSelector::DlgSetSelector(QList<QString> list, QWidget* parent):
        QDialog(parent), selectName(""), setList(list)
    {
    SetupUI();
    }

void DlgSetSelector::SetupUI()
    {
     QGridLayout *mainLayout = new QGridLayout(this);
     this->setLayout(mainLayout);
     this->setContextMenuPolicy(Qt::NoContextMenu);
     
     rdoNewSet = new QRadioButton(this);
     rdoNewSet->setChecked(true);
     rdoNewSet->setText(tr("Create a new Set"));
     QObject::connect(rdoNewSet, SIGNAL(clicked(bool)), this, 
             SLOT(on_radio1Selection_Changed(bool)));
     rdoOldSet = new QRadioButton(this);
     rdoOldSet->setChecked(false);
     rdoOldSet->setText(tr("Select a exist set:"));
     QObject::connect(rdoOldSet, SIGNAL(clicked(bool)), this, 
             SLOT(on_radio2Selection_Changed(bool)));
     
     lstSet = new QListWidget(this);
     for(int i=0;i<setList.count();i++)
         {
         lstSet->addItem(setList[i]);
         }
     lstSet->setEnabled(false);
     
     QWidget *toolWidget = new QWidget(this);
     toolWidget->setContextMenuPolicy(Qt::NoContextMenu);
     QGridLayout *toolLayout = new QGridLayout(this);
     toolWidget->setLayout(toolLayout);
     btnOk = new QPushButton(tr("Ok"), toolWidget);
     btnOk->setContextMenuPolicy(Qt::NoContextMenu);
     QObject::connect(btnOk, SIGNAL(clicked()), this,
             SLOT(on_btnOk_clicked()));
     btnCancel = new QPushButton(tr("Cancel"), toolWidget);
     btnCancel->setContextMenuPolicy(Qt::NoContextMenu);
     QObject::connect(btnCancel, SIGNAL(clicked()), this,
             SLOT(on_btnCancel_clicked()));
     toolLayout->addWidget(btnOk, 0, 0);
     toolLayout->addWidget(btnCancel, 0, 1);
         

     mainLayout->addWidget(rdoNewSet, 0, 0);
     mainLayout->addWidget(rdoOldSet, 1, 0);
     mainLayout->addWidget(lstSet, 2, 0);
            
     mainLayout->addWidget(toolWidget, 3, 0);
     this->showMaximized();
     
    }
void DlgSetSelector::on_radio1Selection_Changed(bool /* checked */)
    {
    lstSet->setEnabled(false);
    }


void DlgSetSelector::on_radio2Selection_Changed(bool checked)
    {
    lstSet->setEnabled(true);
    if(checked)
        {
        if(setList.count() == 0)
            {
            rdoNewSet->setChecked(true);
            }
        else
            {
            lstSet->setCurrentRow(0);
            }
        }
    
    }

void DlgSetSelector::on_btnOk_clicked()
    {
    if(rdoOldSet->isChecked())
        {
        selectName = lstSet->selectedItems()[0]->text();
        }
    else
        {
        selectName = "";
        }
    this->accept();
    }

void DlgSetSelector::on_btnCancel_clicked()
    {
    this->reject();
    }

// End of File
