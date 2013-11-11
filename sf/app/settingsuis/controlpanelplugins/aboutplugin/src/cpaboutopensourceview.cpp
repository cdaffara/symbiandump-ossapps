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
#include "cpaboutopensourceview.h"
#include "cpaboututils.h"
#include <HbScrollArea>
#include <HbGroupBox>
#include <QGraphicsLinearLayout>
/*!
  \class CpAboutOpenSourceView
*/

/*!
    Constructor
*/
CpAboutOpenSourceView::CpAboutOpenSourceView(QGraphicsItem *parent) 
: HbView(parent)    
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0, 0, 0, 0);
    //view title        
    HbGroupBox *label = new HbGroupBox();
    label->setHeading(hbTrId("txt_cp_subhead_open_source_software_notices"));
    layout->addItem(label);
    for (int i=1; i <=2; ++i) {
        HbTextEdit *edit = CpAboutUtils::createTextEdit();        
        QString str(CpAboutUtils::contentString(hbTrId("txt_cp_open_source_software_notices_"), i));        
        edit->setHtml(CpAboutUtils::preprocessText(str));
        layout->addItem(edit);
    }    
    HbScrollArea* scrollArea = new HbScrollArea();    
    QGraphicsWidget* content = new QGraphicsWidget();    
    content->setLayout(layout);   
    scrollArea->setContentWidget(content);    
    scrollArea->setScrollDirections(Qt::Vertical);
    setWidget(scrollArea);    
}

/*!
    Destructor
*/
CpAboutOpenSourceView::~CpAboutOpenSourceView()
{
}




