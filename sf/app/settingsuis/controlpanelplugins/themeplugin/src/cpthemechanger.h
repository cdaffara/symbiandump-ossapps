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

#ifndef CPTHEMECHANGER_H
#define CPTHEMECHANGER_H

#include <QStringList>

#include <hbicon.h>
#include <hbglobal.h>

class CpThemeChangerPrivate;
class CpThemeInfo;

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

class CpThemeChanger : public QObject
{
    Q_OBJECT
    
public:
     explicit CpThemeChanger(QObject* parent=0);
    ~CpThemeChanger();
    
    const CpThemeInfo* currentTheme() const;
    bool changeTheme(const CpThemeInfo& newtheme);
    
signals:
    void themeChangeFinished();
    
private slots:
    void changeFinished();
	
private:
    void setCurrentTheme();
    
    CpThemeInfo* mCurrentTheme;
};


#endif /* CPTHEMECHANGER_H */
