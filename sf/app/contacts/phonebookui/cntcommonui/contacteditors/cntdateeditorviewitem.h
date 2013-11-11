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
#ifndef CNTDATEEDITORVIEWITEM_H_
#define CNTDATEEDITORVIEWITEM_H_

#include "cntdetailviewitem.h"

#include <QDate>
#include <QLocale>
#include <hbpushbutton.h>

class HbAction;

class CntDateEditorViewItem : public CntDetailViewItem
    {
    Q_OBJECT
    
public:
    CntDateEditorViewItem( QGraphicsItem* aParent = 0 );
    ~CntDateEditorViewItem();
    
public slots:
    void clicked(); // push button clicked
    
public:
    HbAbstractViewItem* createItem();
    HbWidget* createCustomWidget();
    
private:
    void editDate( QDate aCurrent, QString aTitle );
    void changeDate( QDate aNewDate );
    
private slots:
    void handleEditDate( HbAction *aAction );
    
    
private:
    HbPushButton* mButton;
    QLocale mLocale;
    
    friend class T_DateEditorTest;
    };
#endif /* CNTDATEEDITORVIEWITEM_H_ */
