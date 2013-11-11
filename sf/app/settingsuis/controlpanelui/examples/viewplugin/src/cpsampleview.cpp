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
#include "cpsampleview.h"
#include <hbdataform.h>
#include <qstringlist>
#include <QDebug>
#include <hbdataformmodel.h>
#include <cpsettingformitemdata.h>
#include <hbmessagebox.h>

CpSampleView::CpSampleView(QGraphicsItem *parent) :
    CpBaseSettingView(0,parent),
    mGroupItem(0),
    mSliderItem(0),
    mCheckBoxItem(0)

{
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    if (form) {
        HbDataFormModel *model = new HbDataFormModel;
        
        form->setHeading(tr("View from sample plugin"));
        mGroupItem = new HbDataFormModelItem(HbDataFormModelItem::GroupItem, QString("Group"));
        
        model->appendDataFormItem(mGroupItem);
        
        mSliderItem = new CpSettingFormItemData(HbDataFormModelItem::SliderItem,
                QString("Sample Slider"));
        form->addConnection(mSliderItem,SIGNAL(valueChanged(int)),this,SLOT(sliderValueChanged(int)));
        mGroupItem->appendChild(mSliderItem);
        
        mCheckBoxItem = new CpSettingFormItemData(HbDataFormModelItem::CheckBoxItem,
                QString("Sample Check Box"));
        form->addConnection(mCheckBoxItem,SIGNAL(stateChanged (int)),this,SLOT(checkBoxStateChanged(int)));
        mGroupItem->appendChild(mCheckBoxItem);
        
        form->setModel(model);
    }
    

}
CpSampleView::~CpSampleView()
{
}

void CpSampleView::sliderValueChanged(int value)
{
    //TODO: store your changes
   // HbMessageBox::message(QString("slider value changed to:%1").arg(value));
}
void CpSampleView::checkBoxStateChanged(int state)
{
    //TODO: store your changes
    QString str = (state ? "checked" : "un-checked");
   // HbMessageBox::message(str);
}

