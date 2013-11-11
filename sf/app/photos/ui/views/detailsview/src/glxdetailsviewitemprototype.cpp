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
 * Description:   ?Description
 *
 */

//Includes
#include <QDebug>
#include <hblabel.h>
#include <QGraphicsLinearLayout>

//User Includes
#include "glxviewids.h"
#include "glxmodelroles.h"  //Contains the declaration of the roles used to retrieve the data from model
#include "glxdetailstextedit.h"  //Contains the declaration of the custom widget glxdetailstextedit
#include "glxlocalisationstrings.h" //contains the localisation strings
#include "glxdetailsviewmodelwrapper.h" //Contains the declaration of the modelwrapper
#include "glxdetailsviewitemprototype.h" //contains the declaration of this file/class

#define CONTENTMARGINS  5
//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsViewItemProtoType
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsViewItemProtoType::GlxDetailsViewItemProtoType(QGraphicsItem* parent) 
    : HbListViewItem(parent),
      mImageNameEditor( NULL ),
      mDescriptionEditor( NULL ),
      mDateLabel( NULL ),
      mTimeLabel( NULL ),
      mSizeLabel( NULL ),
      mImgNameLabel( NULL )
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsViewItemProtoType
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsViewItemProtoType::~GlxDetailsViewItemProtoType()
{
    int widgetIndex = modelIndex().row();
    switch (widgetIndex) {

    case widgetNameIndex:
    {
        delete mImageNameEditor;
        delete mImgNameLabel;
        break;
    }

    case widgetDateIndex:
    {
        delete mDateLabel;
        mDateLabel = NULL;
        break;
    }

    case widgetTimeIndex:
    {
        delete mDateLabel;
        mDateLabel = NULL;
        break;
    }

    case widgetSizeIndex:
    {
        delete mSizeLabel;
        mSizeLabel = NULL;
        break;
    }

    case widgetDescriptionIndex:
    {
        delete mDescriptionEditor;
        mDescriptionEditor = NULL;
        break;
    }

    default:
        break;
    }

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//createItem
//--------------------------------------------------------------------------------------------------------------------------------------------
HbAbstractViewItem* GlxDetailsViewItemProtoType::createItem()
{
    return new GlxDetailsViewItemProtoType(*this);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateChildItems
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsViewItemProtoType::updateChildItems()
{

    switch (modelIndex().row()) {

    case widgetNameIndex:
    {
        int substate = modelIndex().data(GlxSubStateRole).toInt();
        if(substate == IMAGEVIEWER_DETAIL_S)
            {
            mImgNameLabel = new HbLabel(this);
            }
        else
            {
            //create the editor
            mImageNameEditor = new GlxDetailsTextEdit(this);        
            mImageNameEditor->setMaxRows(1);
            }
        //retrieve the name from the model
        QString nameString(GLX_DETAILS_NAME);
        QVariant nameData = modelIndex().data(GlxUriRole).toString();
        if (nameData.isValid()) {
            if (nameData.canConvert<QString> ()) {
                nameString.append(nameData.toString());
            }
        }
        if(substate == IMAGEVIEWER_DETAIL_S)
            {
            mImgNameLabel->setPlainText(nameString);
            mImgNameLabel->setMinimumHeight(size().height() + 50);
            }
        else
            {
            mImageNameEditor->setItemText(nameString);
            }

        //Get the sub state of the details view
        if (substate != IMAGEVIEWER_DETAIL_S) {
            connect(mImageNameEditor, SIGNAL(editorTapped()), this, SLOT(forwardSignalsToView()));
        }

        //Connect the Signals
        const QAbstractItemModel *model = (modelIndex().model());
        connect(model, SIGNAL(updateCustomWidgets()), this, SLOT(updateWidgetValues()));

        //Add the Layout
        QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        widgetLayout->setContentsMargins(CONTENTMARGINS,CONTENTMARGINS,CONTENTMARGINS,CONTENTMARGINS);
        if(substate == IMAGEVIEWER_DETAIL_S)
            {
            widgetLayout->addItem(mImgNameLabel);
            }
        else
            {
            widgetLayout->addItem(mImageNameEditor);
            }
        setLayout(widgetLayout);
        break;
    }

    case widgetDateIndex:
    {
        //retrieve the date from the model
        QString dateString;
        QVariant dateData = modelIndex().data(GlxDateRole).toString();    
        if (dateData.isValid()) {
            if (dateData.canConvert<QString> ()) {
                dateString = dateData.toString();
            }
        }
        
        //create the label and set the data
        mDateLabel = new HbLabel(this);
        mDateLabel->setPlainText(dateString);
        mDateLabel->setMinimumHeight(size().height() + 50);
        
        QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        widgetLayout->setContentsMargins(CONTENTMARGINS, CONTENTMARGINS, CONTENTMARGINS, CONTENTMARGINS);
        widgetLayout->addItem(mDateLabel);        
        setLayout(widgetLayout);
        break;

    }
    case widgetTimeIndex:
    {
        //retrieve the time from the model
        QString timeString;
        QVariant timeData = modelIndex().data(GlxTimeRole).toString();
        if (timeData.isValid()) {
            if (timeData.canConvert<QString> ()) {
                timeString = timeData.toString();
            }
        }

        //create the label and set the data
        mTimeLabel = new HbLabel(this);
        mTimeLabel->setPlainText(timeString);
        mTimeLabel->setMinimumHeight(size().height() + 50);
        
        //Add the Layout
        QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        widgetLayout->setContentsMargins(CONTENTMARGINS, CONTENTMARGINS, CONTENTMARGINS, CONTENTMARGINS);
        widgetLayout->addItem(mTimeLabel);
        setLayout(widgetLayout);
        break;
    }

    case widgetSizeIndex:
    {
        //retrieve the size from the model
        QString sizeString;
        QVariant sizeData = modelIndex().data(GlxSizeRole).toString();
        if (sizeData.isValid()) {
            if (sizeData.canConvert<QString> ()) {
                sizeString = sizeData.toString();
            }
        }

        //create the label and set the data
        mSizeLabel = new HbLabel(this);
        mSizeLabel->setPlainText(sizeString);
        mSizeLabel->setMinimumHeight(size().height() + 50);
                
        //Add the Layout
        QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);     
        widgetLayout->setContentsMargins(CONTENTMARGINS, CONTENTMARGINS, CONTENTMARGINS, CONTENTMARGINS);
        widgetLayout->addItem(mSizeLabel);
        setLayout(widgetLayout);
        break;
    }

    case widgetDescriptionIndex:
    {
        //create the editor
        mDescriptionEditor = new GlxDetailsTextEdit(this);              
        mDescriptionEditor->setMaxRows(5);
        
        //retrieve the size from the model
        QString descString(GLX_DETAILS_DESCRIPTION);
        QVariant descData = modelIndex().data(GlxDescRole).toString();
        if (descData.isValid()) {
            if (descData.canConvert<QString> ()) {
                descString.append(descData.toString());
            }
        }
        mDescriptionEditor->setItemText(descString);

        //Get the sub state of the details view
        int substate = modelIndex().data(GlxSubStateRole).toInt();
        if (substate != IMAGEVIEWER_DETAIL_S) {
            connect(mDescriptionEditor, SIGNAL(editorTapped()), this, SLOT(forwardSignalsToView()));
        }
        else {
            mDescriptionEditor->setTextItemReadOnly(TRUE);
        }

        //Connect the Signals
        const QAbstractItemModel *model = (modelIndex().model());
        connect(model, SIGNAL(updateCustomWidgets()), this, SLOT(updateWidgetValues()));
        
        //Add the Layout
        QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        widgetLayout->setContentsMargins(CONTENTMARGINS,CONTENTMARGINS,CONTENTMARGINS,CONTENTMARGINS);
        widgetLayout->addItem(mDescriptionEditor);
        setLayout(widgetLayout);
        break;
    }

    default:
        break;
    }

    HbListViewItem::updateChildItems();

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//forwardSignalsToView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsViewItemProtoType::forwardSignalsToView()
{
    int widgetIndex = modelIndex().row();
    emit static_cast<GlxDetailsViewItemProtoType*> (prototype())->signalFromCustomWidget(
        widgetIndex);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateWidgetValues
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsViewItemProtoType::updateWidgetValues()
{
    int widgetIndex = modelIndex().row();
    if (widgetIndex == GlxDetailsViewItemProtoType::widgetNameIndex) {

		int substate = modelIndex().data(GlxSubStateRole).toInt();
		QString nameString(GLX_DETAILS_NAME);
        QVariant nameData = modelIndex().data(GlxUriRole).toString();

        if (nameData.isValid()) {
            if (nameData.canConvert<QString> ()) {
                nameString.append(nameData.toString());
            }
        }
        if (substate == IMAGEVIEWER_DETAIL_S)
            {
            mImgNameLabel->setPlainText(nameString);
            }
        else
            {
            mImageNameEditor->setItemText(nameString);
            }
    }
    else {
        QString descString(GLX_DETAILS_DESCRIPTION);
        QVariant descData = modelIndex().data(GlxDescRole).toString();

        if (descData.isValid()) {
            if (descData.canConvert<QString> ()) {
                descString.append(descData.toString());
            }
        }
        mDescriptionEditor->setItemText(descString);
    }
}

