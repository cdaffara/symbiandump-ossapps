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

#include "cppersonalizationcustomviewitem.h"
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbabstractitemview.h>
#include <QMetaProperty>
#include <hbpushbutton.h>

CpPersonalizationCustomViewItem::CpPersonalizationCustomViewItem(QGraphicsItem *parent )
																: HbDataFormViewItem(parent),
																  mWidget(0)
{
}
CpPersonalizationCustomViewItem::~CpPersonalizationCustomViewItem()
{
}
HbAbstractViewItem* CpPersonalizationCustomViewItem::createItem()
{
	return new CpPersonalizationCustomViewItem(*this);
}
bool CpPersonalizationCustomViewItem::canSetModelIndex(const QModelIndex &index) const
{
    CpVolumeCustomItemType type = static_cast<CpVolumeCustomItemType>(index.data(HbDataFormModelItem::ItemTypeRole).toInt());
	/*if (type == MasterVolumeSliderItem) {
		return true;
	}
	else*/ if (type == SilenceIndicatorItem) {
        return true;
	} 
	else {
		return false;
	}
}

HbWidget *CpPersonalizationCustomViewItem::createCustomWidget()
{
    CpVolumeCustomItemType type = static_cast<CpVolumeCustomItemType>
                (modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
	/*if (type == MasterVolumeSliderItem) {
		CpMasterVolumeSlider *masterVolumeSlider = new CpMasterVolumeSlider();
		connect(masterVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(store()));
		mWidgetMap.insert(type,masterVolumeSlider);
		return masterVolumeSlider;
	} else*/ if (type == SilenceIndicatorItem ) {
        HbPushButton *slienceIndicator = new HbPushButton();
        slienceIndicator->setCheckable(true);
        connect(slienceIndicator, SIGNAL(toggled(bool)),this,SLOT(store()));
        mWidget = slienceIndicator;
        return slienceIndicator;
	}
	else {
		return 0;
	}
}

void CpPersonalizationCustomViewItem::load()
{
	//HbDataFormViewItem::load();
	
    CpVolumeCustomItemType itemType = static_cast<CpVolumeCustomItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

    if(itemType == SilenceIndicatorItem) {
        
        QModelIndex itemIndex = modelIndex();
        HbDataFormModel *model = static_cast<HbDataFormModel*>(itemView()->model());;
        HbDataFormModelItem *modelItem = static_cast<HbDataFormModelItem*>(
            model->itemFromIndex(itemIndex));
        
       
        if (mWidget != 0) {
            const QMetaObject *metaObj = mWidget->metaObject();
            int count = metaObj->propertyCount();
            for (int i = 0; i < count; i++) {
                QMetaProperty metaProperty = metaObj->property(i);
                if (metaProperty.isValid() && metaProperty.isWritable()) {
                    metaProperty.write(mWidget,modelItem->contentWidgetData(metaProperty.name()));
                }
            }
        }
    }	
}

void CpPersonalizationCustomViewItem::restore()
{
    HbDataFormViewItem::restore();
    
    CpVolumeCustomItemType itemType = static_cast<CpVolumeCustomItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

    if(itemType == SilenceIndicatorItem) {
        
        QModelIndex itemIndex = modelIndex();
        HbDataFormModel *model = static_cast<HbDataFormModel*>(itemView()->model());;
        HbDataFormModelItem *modelItem = static_cast<HbDataFormModelItem*>(
            model->itemFromIndex(itemIndex));
        
       
        if (mWidget != 0) {
            const QMetaObject *metaObj = mWidget->metaObject();
            int count = metaObj->propertyCount();
            for (int i = 0; i < count; i++) {
                QMetaProperty metaProperty = metaObj->property(i);
                if (metaProperty.isValid() && metaProperty.isWritable()) {
                    metaProperty.write(mWidget,modelItem->contentWidgetData(metaProperty.name()));
                }
            }
        }
    }   
}


void CpPersonalizationCustomViewItem::store()
{
	//HbDataFormViewItem::store();

    CpVolumeCustomItemType itemType = static_cast<CpVolumeCustomItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

    if(itemType == SilenceIndicatorItem) {

        QModelIndex itemIndex = modelIndex();
        HbDataFormModel *model = static_cast<HbDataFormModel*>(itemView()->model());
        HbDataFormModelItem *modelItem = static_cast<HbDataFormModelItem*>(
            model->itemFromIndex(itemIndex));
        
        
        /*if (CpMasterVolumeSlider *slider = qobject_cast<CpMasterVolumeSlider *>(widget)) {
            modelItem->setContentWidgetData("value",slider->value());
        }*/
        if (HbPushButton *silenceIndicator = qobject_cast<HbPushButton *>(mWidget)) {
            modelItem->setContentWidgetData("checked",silenceIndicator->isChecked());
        }                
    }
}

/*void CpPersonalizationCustomViewItem::onValueChanged(int value)
{
	HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
									modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

	if(itemType == MasterVolumeSliderItem) {

		QModelIndex itemIndex = modelIndex();
		HbDataFormModel *model = static_cast<HbDataFormModel*>(itemView()->model());;
		HbDataFormModelItem *modelItem = static_cast<HbDataFormModelItem*>(
			model->itemFromIndex(itemIndex));
		modelItem->setContentWidgetData("value",value);
	}
}*/
