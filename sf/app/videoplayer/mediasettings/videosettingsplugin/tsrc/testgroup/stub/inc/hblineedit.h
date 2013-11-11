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
* Description: HbLineEdit stub
*
*/

#ifndef HBLINEEDIT_H
#define HBLINEEDIT_H

#include <qgraphicsitem.h>
#include <hbwidget.h>
#include <qstring.h>
#include <hbvalidator.h>

class HbLineEdit : public HbWidget
{
    Q_OBJECT

public:

    HbLineEdit(QGraphicsItem *parent = 0);
    HbLineEdit(const QString &text, QGraphicsItem *parent = 0);
    virtual ~HbLineEdit();

    void setValidator(HbValidator *validator);
    
    void setMaxLength(int max);

public:
    
    HbValidator* mValidator;
    
    int mMaxLength;
    
};

#endif // HBLINEEDIT_H
