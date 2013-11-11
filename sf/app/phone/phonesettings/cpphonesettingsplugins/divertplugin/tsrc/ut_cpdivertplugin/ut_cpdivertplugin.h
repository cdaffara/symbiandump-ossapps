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

#ifndef UT_CPTELEPHONYPLUGIN_H
#define UT_CPTELEPHONYPLUGIN_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CpDivertPlugin;
class CpDivertPluginGroup;
class CpItemDataHelper;
class HbDialog;
class HbDataFormModel;

class UT_CpDivertPlugin : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CpDivertPlugin();
    ~UT_CpDivertPlugin();
    
private slots:

    void init();
    void cleanup();
    
    void t_createSettingFormItemData();

    void t_changeDivertingStateRequested();
    
    void t_itemShown();
    
    void t_handleDivertingChanged();
    void t_handleDivertingStatus();
    void t_handleDivertingError();

    void t_memleak();
    void t_popUpTimerQuery();
    
private:
    
    enum actionType {
        selectAction,
        selectItem,
        insertText
    };
    
    class dialogAction {
    public:
        dialogAction( const QString& d, actionType t, const QString& i )
        {
            dialog = d;
            type = t;
            item = i;
        }
        
        QString dialog;
        actionType type;
        QString item;
    };
    
private:
    void appendAction(const QString& dialog, actionType actionType, const QString& action);
    void executeAction( const dialogAction &action );
    HbDialog *visibleDialog( const QString &heading );
    void doAndVerifyAction( const QString& dialog, const QString& action );
    void selectItemFromListWidget( const QString& dialog, const QString& item );
    void timerEvent( QTimerEvent* event );
    void waitForQueueEmpty();
    
signals:
    void queueEmpty();
    
private:

    CpDivertPlugin *m_divertplugin;
    CpDivertPluginGroup *m_divertpluginGroup;
    CpItemDataHelper *m_helper;
    HbDataFormModel *m_dataForm;

    QQueue<dialogAction*> actionQueue;
    
};

#endif // UT_CPTELEPHONYPLUGIN_H
