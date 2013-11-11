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

#ifndef CPTHEMECONTROL_H
#define CPTHEMECONTROL_H

#include <QObject>
#include <QList>

#include "cpthemechanger.h"
#include <hbicon.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QString;
class QSortFilterProxyModel;
QT_END_NAMESPACE

class HbDialog;
class CpThemeListView;
class CpBaseSettingView;
class CpThemeListModel;


class CpThemeControl : public QObject
{
    Q_OBJECT

public:
    CpThemeControl();
    ~CpThemeControl();
    CpBaseSettingView* themeListView();
    QString currentThemeName() const;
    HbIcon currentThemeIcon() const;

signals:
    void themeUpdated(const QString& themeName, const HbIcon& icon);

private slots:
    void newThemeSelected(const QModelIndex& index);
    void applyTheme(const CpThemeInfo& theme);
    void themeListClosed();
    void themeChangeTimeout();
    void themeWaitTimeout();
    void themeChangeFinished();
    void getOviTheme();

private:
    void createThemeList();
    void triggerThemeListClose();
    void setActiveThemeIndex();

private:
    CpThemeListView* mThemeListView;
    CpThemeChanger* mThemeChanger;
    QAbstractItemModel* mListModel;
    bool mThemeChangeFinished;
    HbDialog* mWaitDialog;
};

#endif //CPTHEMECONTROL_H
