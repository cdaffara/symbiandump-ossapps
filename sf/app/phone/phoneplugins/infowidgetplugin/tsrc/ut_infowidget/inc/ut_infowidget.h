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

#ifndef UT_INFOWIDGET_H
#define UT_INFOWIDGET_H

#include <QtTest/QtTest>
#include <hblabel.h>
#include <mockservice.h>
#include <QColor>
#include <QGraphicsWidget>
#include "infowidgetengine.h"

class InfoWidget;

class UT_InfoWidget : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidget();
    ~UT_InfoWidget();
    
private slots:

    void init();
    void cleanup();
    
    //From QGraphicsItem.
    void t_boundingRect();
    void t_shape();
    void t_sizePolicy();
    
    void t_setSpnDisplay();
    void t_setMcnDisplay();
    void t_setSatDisplay();

    void t_paint(); 
    void t_updateInfoDisplay();
    void t_readPersistentPreferences();

    void t_onInitialize();
    void t_onUninitialize(); 
    void t_onShow(); 
    void t_onHide();
    void t_readModel();
    void t_handleModelError();
    void t_spnDisplaySettingChanged();
    void t_mcnDisplaySettingChanged();
    void t_satDisplaySettingChanged();
    void t_settingsEditingFinished();

    // private implementation tests
    void t_updateItemsVisibility();
    void t_layoutInfoDisplay(); 
    void t_layoutSettingsDialog(); 
    void t_initializeSettingsDialogItems();
    void t_startChanges();
    void t_endChanges();
    
    void t_sizeHint();
    
    void t_startMarquees();
    void t_stopMarquees();
    void t_marqueeNext(); 

    void t_setHomeZoneDisplay();
    void t_setActiveLineDisplay();
    void t_initializeCheckBoxStates();
    void t_settingsEditingCancelled();
    void t_settingsDialogClosed();
    void t_settingsValidationFailed();

    /*
     * Not work from commmand prompt.
     */
    void t_changeEvent();
    void t_gestureEvent(); 
    void t_timerEvent(); 

private:
    
    InfoWidget *m_infoWidget;
    InfoWidgetEngine::ModelData m_modelData;
    QColor m_qcolor;
    QGraphicsWidget m_qgraphicswidget;

};

#endif // UT_INFOWIDGET_H
