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
#ifndef CPTHEMELISTVIEW_H
#define CPTHEMELISTVIEW_H

#include <QAbstractItemModel>

#include <hbview.h>
#include <hblistview.h>

#include <cpbasesettingview.h>


class CpThemeSelectionList;

class CpThemeListView : public CpBaseSettingView
{
    Q_OBJECT

public:
    explicit CpThemeListView(QGraphicsItem *parent = 0);
    ~CpThemeListView();
    void setWidget(QGraphicsWidget *widget);
    HbListView* themeList() const;
    void setModel(QAbstractItemModel* model);
    void closeView();
	
signals:
    void newThemeSelected(const QModelIndex& index);
    void oviClicked();

private:
    HbListView* mThemeList;
   
};

#endif //CPTHEMELISTVIEW_H

