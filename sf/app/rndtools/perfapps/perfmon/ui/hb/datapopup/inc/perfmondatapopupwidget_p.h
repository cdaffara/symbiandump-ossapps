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
* Description: 
*
*/

#ifndef PERFMONDATAPOPUPWIDGET_P_H
#define PERFMONDATAPOPUPWIDGET_P_H

#include <HbWidget>

class PerfMonDataPopupWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(QStringList lines READ lines WRITE setLines)

public:
    explicit PerfMonDataPopupWidget(QGraphicsItem *parent = 0);

public:
    QStringList lines() const;
    void setLines(const QStringList &lines);

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);

private:
    QStringList mLines;

    QFont mFont;

};

#endif // PERFMONDATAPOPUPWIDGET_P_H
