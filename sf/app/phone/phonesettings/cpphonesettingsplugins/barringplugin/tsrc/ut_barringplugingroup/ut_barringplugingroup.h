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

#ifndef UT_BARRINGPLUGINGROUP_H
#define UT_BARRINGPLUGINGROUP_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpBarringPluginGroup;
class PSetCallBarringWrapper;
class HbDataFormModel;

class UT_CpBarringPluginGroup : public QObject, MockService
{
    Q_OBJECT

public:    
    
    UT_CpBarringPluginGroup();
    ~UT_CpBarringPluginGroup();

private slots:

    void init();
    void cleanup();
    
    void t_memleak();
    void t_itemShownNotBarringItem();
    void t_itemShownBarringStatusRequestOngoing();
    void t_barringStatusRequestCompletedForUpdateInquiry();
    void t_barringStatusRequestCompletedWithAnError();
    void t_getBarringStatuses();
    void t_enableBarringRequestCompleted();
    void t_enableBarringRequestCompletedUnknownBarring();
    void t_enableBAOCRequestCompletedStatusUpdate();
    void t_enableBOICRequestCompletedStatusUpdate();
    void t_enableBOICexHCRequestCompletedStatusUpdate();
    void t_enableBAICRequestCompletedStatusUpdate();
    void t_enableBICRoamRequestCompletedStatusUpdate();
    void t_disableBarringRequestCompleted();
    void t_barringPasswordChangeRequestCompleted();
    void t_changeBarringStateRequested();
    void t_changeBarringPasswordRequested();

signals:
    void simulateCheckStateChange(int checkState);
    void simulateEditPasswordButtonClicked(bool checked);
    
private:
    
    CpBarringPluginGroup *m_barringpluginGroup;
    PSetCallBarringWrapper *m_barringWrapperMock;
    
    /** Now own. Data form model. */
    HbDataFormModel *m_dataFormModel;
};

#endif // UT_BARRINGPLUGINGROUP_H
