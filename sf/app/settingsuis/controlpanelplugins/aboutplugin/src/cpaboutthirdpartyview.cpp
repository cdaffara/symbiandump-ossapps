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
#include "cpaboutthirdpartyview.h"
#include "cpaboututils.h"
#include <HbScrollArea>
#include <HbGroupBox>
#include <QGraphicsLinearLayout>
#include <QDesktopServices>
#include <QUrl>
/*!
  \class CpAboutThirdPartyView
*/

/*!
    Constructor
*/
CpAboutThirdPartyView::CpAboutThirdPartyView(QGraphicsItem *parent) 
: HbView(parent)    
{      
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0, 0, 0, 0);
    
    //view titile  
    HbGroupBox *label = new HbGroupBox();
    label->setHeading(hbTrId("txt_cp_subhead_3rd_party_notices"));
    layout->addItem(label);
    //the first text edit contains the icon.
    HbTextEdit *firstEdit = new HbTextEdit();//CpAboutUtils::createTextEdit();
    QImage image("c:\\data\\qgn_graf_mup2_visualizer_image.svg"); 
    QTextCursor cursor(firstEdit->textCursor());
    cursor.insertImage(image);    
    QString str = CpAboutUtils::contentString("txt_cp_3rd_party_notices_",1 );
    str.insert(0, doubleHtmlLineBreak);    
    cursor.insertHtml(str);
    firstEdit->setTextCursor(cursor);
    layout->addItem(firstEdit);    
    //2-14 logic string's text edit.     
    for (int i = 2; i<=14; i++ ) {
        QString content(CpAboutUtils::findAndReplaceWithLink(\
                CpAboutUtils::contentString("txt_cp_3rd_party_notices_", i)));        
        HbTextEdit *edit = CpAboutUtils::createTextEdit();        
        edit->setHtml(content);        
        connect(edit, SIGNAL(anchorTapped(QString)), this, SLOT(handleLinkClicked(QString)));
        layout->addItem(edit);
    }
     
    HbScrollArea* scrollArea = new HbScrollArea();    
    QGraphicsWidget* contentWidget = new QGraphicsWidget();    
    contentWidget->setLayout(layout);   
    scrollArea->setContentWidget(contentWidget);    
    scrollArea->setScrollDirections(Qt::Vertical);
    setWidget(scrollArea);
}

/*!
    Destructor
*/
CpAboutThirdPartyView::~CpAboutThirdPartyView()
{
}

/*!
    Responds to link clicked signal.   
*/
void CpAboutThirdPartyView::handleLinkClicked(const QString &anchor)
{    
    if (!anchor.isEmpty() && 
            !QDesktopServices::openUrl(QUrl(anchor))) {
        return;
    }
}

