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

#ifndef CNTDETAILPOPUP_H
#define CNTDETAILPOPUP_H

#include <QObject>
#include <hbselectiondialog.h>
#include <cntviewparams.h>

class QStandardItemModel;

typedef QList<int> CntViewIdList;

class CntDetailPopup : public HbSelectionDialog
{
    friend class TestCntDetailPopup;
    Q_OBJECT
    
public:
    static void selectDetail( CntViewIdList aList, QObject *receiver = 0, const char *member = 0 );

private:
    CntDetailPopup(QGraphicsItem *parent,  CntViewIdList aList );
    ~CntDetailPopup();

    void addListItem(QString aIcon, QString label, int aId);

private:
    QStandardItemModel  *mListModel;
    CntViewIdList        mViewIdList;
};

#endif // CNTDETAILPOPUP_H
