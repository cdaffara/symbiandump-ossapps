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

#ifndef CPTHEMEPREVIEW_H
#define CPTHEMEPREVIEW_H

#include <hbview.h>
#include <QObject>
#include <hbicon.h>
#include "cpthemechanger.h"
#include "cpthemeinfo.h"


QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class HbAction;
class HbMainWindow;
class HbIconItem;

class CpThemePreview : public HbView
{
    Q_OBJECT

public:
    explicit CpThemePreview(const CpThemeInfo &theme, QGraphicsItem *parent = 0);
    ~CpThemePreview();
    void setThemeInfo(const CpThemeInfo& theme);
    const QString themeName() const;
    const HbIcon themeIcon() const;
	
signals:
    void applyTheme(const CpThemeInfo&);
    void aboutToClose();

private slots:
    void themeSelected();
    void previewOrientationChanged(Qt::Orientation orientation);
  
private:
    CpThemeInfo mTheme;
    HbAction* mSoftKeyBackAction;
    HbIconItem* mPreviewIcon;

};

#endif //CPTHEMEPREVIEW_H
