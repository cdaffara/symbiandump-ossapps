/*
 * Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  This class provides custom list item for the
 * DataForm
 *
 */

#include <settingsdataformcustomitem.h>
#include <hbdataformmodelitem.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <qgraphicslinearlayout.h>
#include <hblabel.h>
#include <hbabstractitemview.h>
#include <hbdataformmodel.h>

SettingsDataFormCustomItem::SettingsDataFormCustomItem(QGraphicsItem *parent) :
HbDataFormViewItem(parent),mLineEdit(NULL)
{
}

SettingsDataFormCustomItem::~SettingsDataFormCustomItem()
{
}

HbAbstractViewItem* SettingsDataFormCustomItem::createItem()
{
    return new SettingsDataFormCustomItem(*this);
}

HbWidget* SettingsDataFormCustomItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType =
    static_cast<HbDataFormModelItem::DataItemType> 
    ( modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt() );    
    switch (itemType)
    {
      case HbDataFormModelItem::CustomItemBase :
            {            
            mLineEdit = new HbLineEdit();            
            HbEditorInterface editorInterface(mLineEdit);
            editorInterface.setMode(HbInputModeNumeric); 
            editorInterface.setInputConstraints(HbEditorConstraintFixedInputMode);            
            return mLineEdit;            
            }                
        default:
            return 0;
    }
}

 bool SettingsDataFormCustomItem::canSetModelIndex( const QModelIndex& aIndex ) const
 {
     int itemType = aIndex.data(HbDataFormModelItem::ItemTypeRole).toInt();
     return itemType == HbDataFormModelItem::CustomItemBase;
 }

 void SettingsDataFormCustomItem::restore()
  {  
     HbDataFormViewItem::restore();    
     HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
         modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
     if( itemType == HbDataFormModelItem::CustomItemBase  ) {
         QModelIndex itemIndex = modelIndex();
         HbDataFormModelItem *modelItem = static_cast<HbDataFormModelItem*>(
         static_cast<HbDataFormModel*>(itemView()->model())->itemFromIndex(itemIndex));
         QString port = modelItem->contentWidgetData("text").toString();         
         qDebug("port is %d",port.toInt());        
         mLineEdit->setText(port);        
     }     
 }
