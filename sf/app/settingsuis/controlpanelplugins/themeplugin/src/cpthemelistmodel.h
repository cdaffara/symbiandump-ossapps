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

#ifndef CP_THEME_LIST_MODEL_P_H
#define CP_THEME_LIST_MODEL_P_H

#include <QAbstractListModel>
#include <QModelIndex>
#include <QObject>
#include "cpthemeutil.h"

class QFileSystemWatcher;

class CpThemeListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    
    enum ThemeListUserRole {
        ItemTypeRole = Qt::UserRole,
        ItemDataRole
    };
    
    explicit CpThemeListModel(QObject *parent = 0);
    virtual ~CpThemeListModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int indexOf(const CpThemeInfo& theme) const;
    
public slots:
    void themeListChanged();

private:
    QList<CpThemeInfo> mTopThemeList;
    QList<CpThemeInfo> mThemeList;
    QList<CpThemeInfo> mBottomThemeList;
    QFileSystemWatcher *mFileWatcher;
};

#endif //CP_THEME_LIST_MODEL_P_H
