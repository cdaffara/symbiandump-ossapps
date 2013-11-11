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

#ifndef CNTEDITVIEWDETAILITEM_H
#define CNTEDITVIEWDETAILITEM_H

#include <QObject>
#include <hbwidget.h>
#include <qcontactdetail.h>
#include "cntstringmapper.h"
#include <cnteditviewitem.h>

#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContactDetail;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class HbIconItem;
class HbTextItem;
class QGraphicsGridLayout;
class HbFrameItem;
class HbGestureSceneFilter;
class HbGesture;


class CntEditViewDetailItem : public QObject, public CntEditViewItem
{
    Q_OBJECT

public:
    CntEditViewDetailItem( QContactDetail aDetail, QString aField, int aEditorView );
    CntEditViewDetailItem( QContactDetail aDetail, QStringList aField, int aEditorView );
    ~CntEditViewDetailItem();
    
    void addText( QString aText );
    void addIcon( HbIcon aIcon );
    
public:// From CntEditViewItem
    QVariant data(int role) const;
    void activated(CntEditViewItemCallback* interface);
    void longPressed(const QPointF &coords, CntEditViewItemCallback* interface);
    
private:
    QStringList mTextList;
    QVariantList mIconList;
    QContactDetail mDetail;
    QStringList mField;
    int mEditorViewId;
};
#endif // CNTEDITVIEWDETAILITEM_H

