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
#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <QGraphicsLinearLayout>
#include <hbdataformmodel.h>
#include <HbGroupBox>

#include "irapplication.h"
#include "irsettingsview.h"
#include "irqsettings.h"
#include "irqenums.h"
#include <hbmenu.h>

IRSettingsView::IRSettingsView(IRApplication *aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId), iForm(NULL),iModel(NULL)
{
    setFlag(EViewFlag_UnStackable);
    setTitle(hbTrId("txt_irad_title_internet_radio"));
    initView();
}

IRSettingsView::~IRSettingsView()
{
}

void IRSettingsView::initView()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    HbGroupBox * titleText = new HbGroupBox();
#ifdef SUBTITLE_STR_BY_LOCID
    titleText->setHeading( hbTrId("txt_common_opt_settings") );
#else
    titleText->setHeading( hbTrId("Settings") );    
#endif
    layout->addItem(titleText);

    iForm = new HbDataForm();
    layout->addItem(iForm);

		
    iModel = new HbDataFormModel();
    
    //set the preferred quality
    setPreferredQuality();
    iForm->setModel(iModel);
    
    HbMenu *viewMenu = menu();
#ifdef SUBTITLE_STR_BY_LOCID
    viewMenu->addAction(hbTrId("txt_common_opt_exit"),iApplication, SIGNAL(quit()));
#else
    viewMenu->addAction(hbTrId("Exit"),iApplication, SIGNAL(quit()));    
#endif
}

void IRSettingsView::setPreferredQuality()
{
#ifdef SUBTITLE_STR_BY_LOCID
    HbDataFormModelItem *preferredQualityItem = new HbDataFormModelItem(
            HbDataFormModelItem::ToggleValueItem, hbTrId("txt_irad_setlabel_download_quality"));
#else
    HbDataFormModelItem *preferredQualityItem = new HbDataFormModelItem(
            HbDataFormModelItem::ToggleValueItem, hbTrId("Download quality"));    
#endif
    iModel->appendDataFormItem(preferredQualityItem);

    IRQPreferredQuality settingValue = iSettings->getPreferredQuality();
    if(EIRQStandardQuality == settingValue)
    {
#ifdef SUBTITLE_STR_BY_LOCID
        preferredQualityItem->setContentWidgetData("text", hbTrId("txt_irad_setlabel_download_quality_val_standard"));
        preferredQualityItem->setContentWidgetData("additionalText", hbTrId("txt_irad_setlabel_download_quality_val_high"));
#else
        preferredQualityItem->setContentWidgetData("text", hbTrId("Standard"));
        preferredQualityItem->setContentWidgetData("additionalText", hbTrId("High"));        
#endif
    }
    else if(EIRQHighQuality == settingValue)
    {
#ifdef SUBTITLE_STR_BY_LOCID
        preferredQualityItem->setContentWidgetData("text", hbTrId("txt_irad_setlabel_download_quality_val_high"));
        preferredQualityItem->setContentWidgetData("additionalText", hbTrId("txt_irad_setlabel_download_quality_val_standard"));
#else
        preferredQualityItem->setContentWidgetData("text", hbTrId("High"));
        preferredQualityItem->setContentWidgetData("additionalText", hbTrId("Standard"));        
#endif
    }
    
    connect(iModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), 
        this, SLOT(toggleChange(QModelIndex, QModelIndex)));
}

void IRSettingsView::toggleChange(QModelIndex aStartIn, QModelIndex aEndIn)
{
    Q_UNUSED(aEndIn);

    HbDataFormModelItem *item = iModel->itemFromIndex(aStartIn);
    
    if(item->type() == HbDataFormModelItem::ToggleValueItem) 
    {
#ifdef SUBTITLE_STR_BY_LOCID
        if(item->data(HbDataFormModelItem::LabelRole).toString() == hbTrId("txt_irad_setlabel_download_quality"))
#else
        if(item->data(HbDataFormModelItem::LabelRole).toString() == hbTrId("Download quality"))            
#endif
        {
            QVariant data = item->contentWidgetData("text");
            QString value = data.toString();
            
#ifdef SUBTITLE_STR_BY_LOCID
            if(value == (hbTrId("txt_irad_setlabel_download_quality_val_standard")))
#else
            if(value == (hbTrId("Standard")))                
#endif
            {    
                // User has made it Standard.
                iSettings->setPreferredQuality(EIRQStandardQuality);
            }
#ifdef SUBTITLE_STR_BY_LOCID
            else if(value == (hbTrId("txt_irad_setlabel_download_quality_val_high")))
#else
            else if(value == (hbTrId("High")))                
#endif
            {
                // User has made it High.
                iSettings->setPreferredQuality(EIRQHighQuality);
            }
        }
    }
}
