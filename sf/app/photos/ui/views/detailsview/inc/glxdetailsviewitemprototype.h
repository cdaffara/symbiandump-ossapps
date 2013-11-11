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

#ifndef GLXDETAILSVIEWITEMPROTOTYPE_H
#define GLXDETAILSVIEWITEMPROTOTYPE_H

//Includes
#include <hblistviewitem.h>

//Orbit/Qt forward declartion
class HbLabel;

//User Defined forward declartion
class GlxDetailsTextEdit;

/**
 * Class Description
 * This is Custom widget class to create custom widgets in the list widget of the details view 
 */
class GlxDetailsViewItemProtoType : public HbListViewItem
    {
Q_OBJECT

public:
    /*
     * The Enum here corresponds to the the custom widget's indexes occuring in the List Widget
     */
    enum WidgetIndex
        {
        widgetNameIndex = 0,
        widgetDateIndex,
        widgetTimeIndex,
        widgetSizeIndex,
        widgetDescriptionIndex
        };

    /*
     * Constructor
     * @param - parent of this widget
     */
    GlxDetailsViewItemProtoType(QGraphicsItem* parent = 0);

    /*
     * Destructor
     */
    virtual ~GlxDetailsViewItemProtoType();

    /**
     * createItem()
     * From HbAbstractViewItem
     * Creates a new item.     
     */
    HbAbstractViewItem* createItem();

    /**
     * updateChildItems()
     * From HbAbstractViewItem
     * Creates the custom widget corresponding to the widget index retrived through the model    
     */
    void updateChildItems();

private slots:

    /**
     * forwardSignalsToView()
     * This slot is called when the rename or description has to be changed
     */
    void forwardSignalsToView();

    /**
     * updateWidgetValues()
     * This slot is called when there is a change in the model data 
     */
    void updateWidgetValues();

signals:

    /**
     * signalFromCustomWidget : This signal is emitted to the details view, to update or launch dialogs 
     * like description field editing or renaming of the image file name  
     * @param - row , specifies the widget index in the list widget
     */
    void signalFromCustomWidget(int row);

private:
    //Image Name Editor:To display the Image Name
    GlxDetailsTextEdit *mImageNameEditor;
    
    //Description Editor:To display the Description/comments of the image 
    GlxDetailsTextEdit *mDescriptionEditor;
        
    //Displays the Date of the image 
    HbLabel *mDateLabel;
    
    //Displays the time of the image 
    HbLabel *mTimeLabel;
    
    //Displays the size of the image 
    HbLabel *mSizeLabel;

	//Displays the name of the image in imageviewer
    HbLabel *mImgNameLabel;
    };

#endif // GLXDETAILSVIEWITEMPROTOTYPE_H
