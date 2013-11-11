/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Definition of AgendaEventViewerItem class
*
*/

#include <QtGui>
#include <QVariant>

// System Includes.
#include <hbstyleloader.h>
#include <hbstyle.h>
#include <hbtextitem.h>
#include <hbiconitem.h>

// User Includes.
#include "agendaeventvieweritem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "agendaeventvieweritemTraces.h"
#endif


/*!
 \class AgendaEventViewerItem

 This class creates all the primitives for AgendaEventViewer item.
 */

/*!
 Constructor.

 \param parent Pointer to QGraphicsItem.
 */
AgendaEventViewerItem::AgendaEventViewerItem(QGraphicsItem *parent) :
	HbWidget(parent), mPrimaryText(NULL), mSecondaryText(NULL),
	mPrimaryLeftIcon(NULL),mPrimaryRightIcon(NULL),
	mSecondaryLeftIcon(NULL),mSecondaryRightIcon(NULL)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERITEM_AGENDAEVENTVIEWERITEM_ENTRY );

	// Path for widgetml and css files.
	HbStyleLoader::registerFilePath(":/agendaeventvieweritem.css");
	HbStyleLoader::registerFilePath(":/agendaeventvieweritem.widgetml");
	HbStyleLoader::registerFilePath(":/agendaeventvieweritem_color.css");

	OstTraceFunctionExit0( AGENDAEVENTVIEWERITEM_AGENDAEVENTVIEWERITEM_EXIT );
}

/*!
 Destructor.
 */
AgendaEventViewerItem::~AgendaEventViewerItem()
{
	OstTraceFunctionEntry0( DUP1_AGENDAEVENTVIEWERITEM_AGENDAEVENTVIEWERITEM_ENTRY );
	HbStyleLoader::unregisterFilePath(":/agendaeventvieweritem.css");
	HbStyleLoader::unregisterFilePath(":/agendaeventvieweritem.widgetml");
	HbStyleLoader::unregisterFilePath(":/agendaeventvieweritem_color.css");

	OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEWERITEM_AGENDAEVENTVIEWERITEM_EXIT );
}

/*!
 Sets the data for EventViewer items
 To set textitem data use Qt::DisplayRole
 To set iconitem data use Qt::DecorationRole
 \param itemData string list of itemdata
 \param role qt role of text and icon item
 */
void AgendaEventViewerItem::setEventViewerItemData(const QStringList &itemData,
                                                   int role)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERITEM_SETEVENTVIEWERITEMDATA_ENTRY );
	if (!itemData.isEmpty()) {

		if (role == Qt::DisplayRole) {
			for (int index=0; index < itemData.count(); index++)
				setTextItemData(index, itemData.at(index));
			
		} else {
			if (role == Qt::DecorationRole) {
			for (int index=0; index < itemData.count(); index++)
				setIconItemData(index, itemData.at(index));
			
		}
	}
	repolish();
	OstTraceFunctionExit0( AGENDAEVENTVIEWERITEM_SETEVENTVIEWERITEMDATA_EXIT );
	}
}

/*!
 Sets the text data for EventViewer items
 
 \param index item index
 \param itenmData To set the given value to item
 */
void AgendaEventViewerItem::setTextItemData(int index, const QString &itemData)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERITEM_SETTEXTITEMDATA_ENTRY );
	switch(index) {
		case 0: {
			if (!itemData.isEmpty()) {
				if (!mPrimaryText) {
					mPrimaryText = new HbTextItem(this);
					HbStyle::setItemName(mPrimaryText, "primaryTextItem");
					mPrimaryText->setElideMode(Qt::ElideNone);
				}
				mPrimaryText->setText(itemData);
			} else {
				if (mPrimaryText) {
					delete mPrimaryText;
					mPrimaryText = NULL;
				}
			}
			break;
		}
		case 1: {
			if (!mSecondaryText) {
				mSecondaryText = new HbTextItem(this);
				HbStyle::setItemName(mSecondaryText, "secondaryTextItem");
				mSecondaryText->setTextWrapping(Hb::TextWordWrap);
			}
	
			if (!itemData.isEmpty()) {
	
				mSecondaryText->setText(itemData);
			} else {
				mSecondaryText->setText("");
			}
			break;
		}
	}
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERITEM_SETTEXTITEMDATA_EXIT );
}

/*!
 Sets the icon data for EventViewer items
 
 \param index item index
 \param itenmData To set the given value to item
 */

void AgendaEventViewerItem::setIconItemData(int index, const QString &itemData)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERITEM_SETICONITEMDATA_ENTRY );
	switch (index) {
		case 0: {
			if (!itemData.isEmpty()) {
				if (!mPrimaryLeftIcon) {
					mPrimaryLeftIcon = new HbIconItem(this);
				}
				HbStyle::setItemName(mPrimaryLeftIcon, "primaryLeftIconItem");
				mPrimaryLeftIcon->setIconName(itemData);

			} else {
				if (mPrimaryLeftIcon) {
					delete mPrimaryLeftIcon;
					mPrimaryLeftIcon = NULL;
				}

			}
			break;
		}
		case 1: {
			if (!itemData.isEmpty()) {
				if (!mPrimaryRightIcon) {
					mPrimaryRightIcon = new HbIconItem(this);
				}
				HbStyle::setItemName(mPrimaryRightIcon, "primaryRightIconItem");
				mPrimaryRightIcon->setVisible(true);
				mPrimaryRightIcon->setIconName(itemData);

			} else {
				if (mPrimaryRightIcon) {
					HbStyle::setItemName(mPrimaryRightIcon, "");
					mPrimaryRightIcon->setVisible(false);
				}

			}
			break;
		}
		case 2: {
			if (!itemData.isEmpty()) {
				if (!mSecondaryLeftIcon) {
					mSecondaryLeftIcon = new HbIconItem(this);
					HbStyle::setItemName(mSecondaryLeftIcon,
					                     "secondaryLeftIconItem");
				}
				mSecondaryLeftIcon->setIconName(itemData);

			} else {
				if (mSecondaryLeftIcon) {
					delete mSecondaryLeftIcon;
					mSecondaryLeftIcon = NULL;
				}

			}
			break;
		}
		case 3: {

			if (!itemData.isEmpty()) {
				if (!mSecondaryRightIcon) {
					mSecondaryRightIcon = new HbIconItem(this);
					HbStyle::setItemName(mSecondaryRightIcon,
					                     "secondaryRightIconItem");
				}
				mSecondaryRightIcon->setIconName(itemData);

			} else {
				if (mSecondaryRightIcon) {
					delete mSecondaryRightIcon;
					mSecondaryRightIcon = NULL;
				}

			}
			break;

		}

	}
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERITEM_SETICONITEMDATA_EXIT );
}

// End of file	--Don't remove this.

