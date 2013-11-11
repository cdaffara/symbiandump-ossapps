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
#include "cpsamplegroup.h"
#include <cpsettingformitemdata.h>
#include <cpitemdatahelper.h>
#include <hbmessagebox.h>

CpSampleGroup::CpSampleGroup(CpItemDataHelper &itemDataHelper) :
    CpSettingFormItemData(HbDataFormModelItem::GroupItem,QString("Sample Group")),
    mSliderItem(0), 
    mCheckBoxItem(0)
{
    mSliderItem = new CpSettingFormItemData(HbDataFormModelItem::SliderItem,
            QString("Sample Slider"));
    itemDataHelper.addConnection(mSliderItem,SIGNAL(valueChanged(int)),this,SLOT(sliderValueChanged(int)));
    this->appendChild(mSliderItem);
    
    mCheckBoxItem = new CpSettingFormItemData(HbDataFormModelItem::CheckBoxItem,
            QString("Sample Check Box"));
    itemDataHelper.addConnection(mCheckBoxItem,SIGNAL(stateChanged (int)),this,SLOT(checkBoxStateChanged(int)));
    this->appendChild(mCheckBoxItem);
    
}
CpSampleGroup::~CpSampleGroup()
{
    //TODO: release resource when necessary
}
void CpSampleGroup::sliderValueChanged(int value)
{
    //TODO: store your changes
   // HbMessageBox::message(QString("slider value changed to:%1").arg(value));
}
void CpSampleGroup::checkBoxStateChanged(int state)
{
    //TODO: store your changes
    QString str = (state ? "checked" : "un-checked");
   // HbMessageBox::message(str);
}
