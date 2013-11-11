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

#ifndef CNTNOTEEDITORVIEWITEM_H_
#define CNTNOTEEDITORVIEWITEM_H_

#include "cntdetailviewitem.h"
#include <hbabstractviewitem.h>
#include <qgraphicslinearlayout.h>
#include <hbwidget.h>
#include <hbtextitem.h>
#include <hblineedit.h>

class CntNoteEditorViewItem : public CntDetailViewItem
    {
    Q_OBJECT
    
public:
    CntNoteEditorViewItem( QGraphicsItem* aParent = 0 );
    ~CntNoteEditorViewItem();
    
public:
    HbAbstractViewItem* createItem();
    HbWidget* createCustomWidget();
    
public slots:
    void textChanged(const QString &text);
    
private:
    HbLineEdit* mEdit;
    QGraphicsLinearLayout* mLayout;
    
    friend class T_NoteEditorTest;
    };

#endif /* CNTNOTEEDITORVIEWITEM_H_ */
