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

#ifndef CNTGROUPDELETEPOPUP_H
#define CNTGROUPDELETEPOPUP_H

#include <QObject>
#include <hbselectiondialog.h>
#include <qmobilityglobal.h>
#include <qcontactid.h>
#include "cntgroupdeletepopupmodel.h"

class HbListView;
class CntAbstractEngine;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupDeletePopup : public HbSelectionDialog
{
    Q_OBJECT
    friend class TestCntGroupDeletePopup;

public:
    CntGroupDeletePopup(CntAbstractEngine *aEngine, QGraphicsItem *parent = 0);
    ~CntGroupDeletePopup();

    void populateListOfGroup();
    QList<QContactLocalId> deleteGroup() const;

private:
    QContactManager          *mContactManager;
    CntGroupDeletePopupModel *mModel;  
};

#endif // CNTGROUPDELETEPOPUP_H
