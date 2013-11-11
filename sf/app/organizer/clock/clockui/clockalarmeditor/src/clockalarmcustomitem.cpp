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
* Implementation for the ClockAlarmCustomItem class.
*
*/

// System includes.
#include <HbPushButton>
#include <HbDataFormModelItem>
#include <HbAbstractItemView>
#include <HbDataFormModel>

// User includes.
#include "clockalarmcustomitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockalarmcustomitemTraces.h"
#endif


/*!
	\class ClockAlarmCustomItem

	This class is responsible for creating customitem for Alarm Time field in
	Alarmeditor.
 */

/*!
	Default Constructor.

	\param parent The parent.
 */
ClockAlarmCustomItem::ClockAlarmCustomItem(QGraphicsItem *parent)
:HbDataFormViewItem(parent)
{
	OstTraceFunctionEntry0( CLOCKALARMCUSTOMITEM_CLOCKALARMCUSTOMITEM_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( CLOCKALARMCUSTOMITEM_CLOCKALARMCUSTOMITEM_EXIT );
}

/*!
	Destructor.
 */
ClockAlarmCustomItem::~ClockAlarmCustomItem()
{
	OstTraceFunctionEntry0( DUP1_CLOCKALARMCUSTOMITEM_CLOCKALARMCUSTOMITEM_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( DUP1_CLOCKALARMCUSTOMITEM_CLOCKALARMCUSTOMITEM_EXIT );
}

/*!
	Creates customitem.

	\sa
 */
HbAbstractViewItem* ClockAlarmCustomItem::createItem()
{
	OstTraceFunctionEntry0( CLOCKALARMCUSTOMITEM_CREATEITEM_ENTRY );
	return new ClockAlarmCustomItem(*this);
}

/*!
	
	\sa
 */
bool ClockAlarmCustomItem::canSetModelIndex(const QModelIndex &index) const
{
	OstTraceFunctionEntry0( CLOCKALARMCUSTOMITEM_CANSETMODELINDEX_ENTRY );
	HbDataFormModelItem::DataItemType itemType = 
			static_cast<HbDataFormModelItem::DataItemType>(
			index.data(HbDataFormModelItem::ItemTypeRole).toInt());

	if(itemType == (HbDataFormModelItem::CustomItemBase)) {
		OstTraceFunctionExit0( CLOCKALARMCUSTOMITEM_CANSETMODELINDEX_EXIT );
		return true;
	} else {
		OstTraceFunctionExit0( DUP1_CLOCKALARMCUSTOMITEM_CANSETMODELINDEX_EXIT );
		return false;
	}

}

void ClockAlarmCustomItem::restore()
{
	OstTraceFunctionEntry0( CLOCKALARMCUSTOMITEM_RESTORE_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
	if(itemType == (HbDataFormModelItem::CustomItemBase)) {

		HbDataFormModel* model =
				static_cast<HbDataFormModel*>(
				static_cast<HbAbstractViewItem*>(this)->itemView()->model());
		HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex()); 
		mCustomButton->setProperty("text", modlItem->contentWidgetData("text"));
		mCustomButton->setProperty(
				"objectName", modlItem->contentWidgetData("objectName"));
	}

	OstTraceFunctionExit0( CLOCKALARMCUSTOMITEM_RESTORE_EXIT );
}

/*!
	Creates the custom widget

	\sa
 */
HbWidget* ClockAlarmCustomItem::createCustomWidget()
{
	OstTraceFunctionEntry0( CLOCKALARMCUSTOMITEM_CREATECUSTOMWIDGET_ENTRY );
	mCustomButton = new HbPushButton(this);
	OstTraceFunctionExit0( CLOCKALARMCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
	return mCustomButton;
}

// End of file	--Don't remove this.
