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
 * Description: omacppinquerydialog class declaration
 *
 */

#ifndef OMACPPINQUERYDIALOG_H_
#define OMACPPINQUERYDIALOG_H_

#include <hbpushbutton.h>
#include <hblabel.h>
#include <hblineedit.h>
#include <QGraphicsWidget>
#include <hbdocumentloader.h>
#include <hbinputeditorinterface.h>
#include "devicemanagementnotifierwidget_p.h"

class omacppinquerydialog : public devicemanagementnotifierwidget
    {

Q_OBJECT

public:
    omacppinquerydialog(const QVariantMap &parameters);

public:
    void createcppinquery(const QVariantMap &parameters);
    HbDialog *deviceDialogWidget() const;

public slots:

    void pintextChanged();
    void okSelected();
    void cancelSelected();
    
private:
    HbLineEdit *mlineedit;
    HbAction *mactionok;
    };
    


#endif
