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

#include <QObject>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsLinearLayout>

#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbicon.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbiconitem.h>
#include <hbmainwindow.h>
#include <HbParameterLengthLimiter>
#include <HbDataForm>

#include "cpthemepreview.h"
#include "cpthemeinfo.h"

/*!
    \class CpThemePreview
    \brief CpThemePreview shows a preview of a selected theme with a heading displaying the name of the theme as well as
           a toolbar with Select and Cancel buttons.  This view is used for the user to either select the theme and apply
           the theme change or press Cancel and go back to theme list view.
*/


/*!
    constructor.
*/
CpThemePreview::CpThemePreview(const CpThemeInfo& theme, QGraphicsItem *parent) :
     HbView(parent), 
     mTheme(theme),
     mSoftKeyBackAction(0),
     mPreviewIcon(0)
{
    QGraphicsLinearLayout* containerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    QGraphicsLinearLayout* bottomLayout = new QGraphicsLinearLayout(Qt::Vertical);
        
    //Preview icon margins.
    qreal leftMargin = 0.0;
    qreal rightMargin = 0.0;
    qreal topMargin = 0.0;
    qreal bottomMargin = 0.0;
        
    style()->parameter(QString("hb-param-margin-gene-left"), leftMargin);
    style()->parameter("hb-param-margin-gene-right", rightMargin);
    style()->parameter("hb-param-margin-gene-top", topMargin);
    style()->parameter("hb-param-margin-gene-bottom", bottomMargin);

    containerLayout->setContentsMargins(0,0,0,0);
    bottomLayout->setContentsMargins(leftMargin, topMargin, rightMargin, bottomMargin);
       
    //Using an empty dataform as heading because the heading should
    //look like an HbDataForm headiing.
    HbDataForm* heading = new HbDataForm(this);
    QString themeHeading = HbParameterLengthLimiter("txt_cp_title_preview_1").arg(mTheme.name());   
    heading->setHeading(themeHeading);
       
    containerLayout->addItem(heading);
    //Fixed vertical policy so that the heading doesn't expand.
    heading->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed,QSizePolicy::DefaultType);
    
    if(mainWindow()->orientation() == Qt::Horizontal) {
        mPreviewIcon = new HbIconItem(mTheme.landscapePreviewIcon(), this);
    }
    else {
        mPreviewIcon = new HbIconItem(mTheme.portraitPreviewIcon(), this);
        //set to ignore aspect ratio so the layout would rezise the icon correctly.
          
    }
    mPreviewIcon->setAspectRatioMode(Qt::IgnoreAspectRatio);
  
    // set an object name for preview icon to make it testable for automation testing
    mPreviewIcon->setObjectName(QString("themePreviewIcon"));

    
    bottomLayout->addItem(mPreviewIcon);
    containerLayout->addItem(bottomLayout);

    setLayout(containerLayout);
        
    //Create the toolbar and "Select" and "Cancel" actions.
    HbToolBar* mToolBar = new HbToolBar(this);

    HbAction* selectAction = new HbAction(hbTrId("txt_common_button_select"));
    
    //Add Action to the toolbar and show toolbar
    mToolBar->addAction( selectAction );

    HbAction* cancelAction = new HbAction(hbTrId("txt_common_button_cancel"));
    mToolBar->addAction( cancelAction );

    QObject::connect( selectAction, SIGNAL(triggered()), 
                      this, SLOT(themeSelected()));

    QObject::connect( cancelAction, SIGNAL(triggered()), 
                      this, SIGNAL(aboutToClose()));
    QObject::connect( mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                      this, SLOT(previewOrientationChanged(Qt::Orientation)));
       
        
    setToolBar(mToolBar);
    //Setup the Back button action and set softkey. Back button 
    //takes the user to the theme list view.
    mSoftKeyBackAction = new HbAction(Hb::BackNaviAction, this);
    QObject::connect(mSoftKeyBackAction, SIGNAL(triggered()), 
            this, SIGNAL(aboutToClose()) );

    setNavigationAction(mSoftKeyBackAction);
    
}

/*!
    destructor.
*/
CpThemePreview::~CpThemePreview()
{
}

/*!
  sets the theme to \a theme.
*/
void CpThemePreview::setThemeInfo(const CpThemeInfo& theme)
{
    mTheme = theme;
}

/*!
    returns the themeName.
*/
const QString CpThemePreview::themeName() const
{
    return mTheme.name();
}

/*!
    returns the repersentative themeIcon of the current theme.
*/
const HbIcon CpThemePreview::themeIcon() const
{
    return mTheme.icon();
}

/*!
    Slot to handle when the user selects a theme.  
*/
void CpThemePreview::themeSelected()
{
    emit applyTheme(mTheme);
}

/*! 
 *  Slot to handle landscape/portrait orientation change to use the
 *  right graphics.
 */
void CpThemePreview::previewOrientationChanged(Qt::Orientation orientation)
{
   
    QGraphicsLinearLayout* containerLayout = dynamic_cast<QGraphicsLinearLayout*>(layout());
    QGraphicsLinearLayout* previewLayout = 0;
    if(containerLayout) {
        //get the layout that preview icon belongs to.
        previewLayout = dynamic_cast<QGraphicsLinearLayout*>(containerLayout->itemAt(1));
    }
   
    if(previewLayout && mPreviewIcon && mPreviewIcon == dynamic_cast<HbIconItem*>(previewLayout->itemAt(0)) ) {
        //Remove preview icon.
        previewLayout->removeAt(0);
        
        if(orientation == Qt::Horizontal) {
            mPreviewIcon->setIcon(mTheme.landscapePreviewIcon());
                    
        }
        else {
            mPreviewIcon->setIcon(mTheme.portraitPreviewIcon());
        }
        
        previewLayout->addItem(mPreviewIcon);
    }
    
}


    
