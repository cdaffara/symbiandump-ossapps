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

#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <hbwidget.h>
#include <QList>
#include <QFlags>
#include "infowidgetlayoutmanager.h"
#include "infowidgetengine.h"
#include "infowidgetpreferences.h"

// Forward declarations 
class QGraphicsSceneMouseEvent;
class QGraphicsLinearLayout;
class QTranslator; 
class QGestureEvent; 
class HbDialog;
class HbIconItem; 
class HbMarqueeItem;
class HbLabel; 
class HbCheckBox; 
class HbPushButton;
class HbFrameItem;
class HbAction; 
class InfoWidgetEngine; 
class InfoWidgetPreferences;


// Class declaration
class InfoWidget : public HbWidget
{
    Q_OBJECT

public: 

    // State for animation
    enum AnimationState {
        AnimationIdle, 
        AnimationStarting,
        AnimationOngoing
    } m_animationState; 
    
public:    
    InfoWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~InfoWidget();

public: // From QGraphicsItem.
    QRectF boundingRect() const;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint) const;  
    QSizePolicy sizePolicy () const;  

public: // Property definitions
    Q_PROPERTY(QString homeZoneDisplay READ homeZoneDisplay WRITE setHomeZoneDisplay)
    Q_PROPERTY(QString mcnDisplay READ mcnDisplay WRITE setMcnDisplay)
    Q_PROPERTY(QString activeLineDisplay READ activeLineDisplay WRITE setActiveLineDisplay)
    Q_PROPERTY(QString satDisplay READ satDisplay WRITE setSatDisplay)
    Q_PROPERTY(QString spnDisplay READ spnDisplay WRITE setSpnDisplay)
    QString homeZoneDisplay();
    void setHomeZoneDisplay(QString value);
    QString mcnDisplay();
    void setMcnDisplay(QString value);
    QString activeLineDisplay();
    void setActiveLineDisplay(QString value);
    QString satDisplay();
    void setSatDisplay(QString value);
    QString spnDisplay();
    void setSpnDisplay(QString value);
    
protected:
    void gestureEvent(QGestureEvent *event); 
    void changeEvent(QEvent *event);
    void timerEvent(QTimerEvent *event);
    
    void updateInfoDisplay();
    bool readPersistentPreferences();
    void initializeCheckBoxStates(); 
     
private: 
    void updateItemsVisibility();
    void layoutInfoDisplay(); 
    void updateInfoDisplayItem(
            InfoWidgetLayoutManager::LayoutItemRole itemRole, 
            QString text); 
    void layoutSettingsDialog();
    void initializeSettingsDialogItems();
    void startChanges(); 
    void endChanges(); 

public slots: 
    // Slots for Homescreen FW callbacks
    void onInitialize();
    void onUninitialize();
    void onShow();
    void onHide();

    // Info widget specific slots 
    void readModel(); 
    void handleModelError(int operation,int errorCode);  
    void spnDisplaySettingChanged(int state);
    void mcnDisplaySettingChanged(int state);
    void satDisplaySettingChanged(int state);

    void settingsEditingFinished();
    void settingsEditingCancelled();
    void settingsValidationFailed(); 
    void settingsDialogClosed(HbAction* action); 
    
    bool startMarquees();
    void stopMarquees(); 
    void marqueeNext(); 
    
signals: 
    void setPreferences(const QStringList &names);

private:
    Q_DISABLE_COPY(InfoWidget)
    
    // Guarded objects, owned
    QScopedPointer<InfoWidgetEngine> m_engine;
    QScopedPointer<InfoWidgetPreferences> m_preferences;
    QScopedPointer<InfoWidgetLayoutManager> m_layoutManager;

    // Widget main layout, not owned 
    QGraphicsLinearLayout *m_layout;
    
    // Currently animating item(s), not owned 
    HbMarqueeItem *m_animatingItem; 
    QList<HbMarqueeItem *> m_animatingItems; 
    
    // Background frame drawer, not owned
    HbFrameItem *m_backgroundFrameItem;
    
    // Id of active timer 
    int m_timerId; 
    
    // Flags 
    bool m_layoutChanging;
    bool m_initialized;

};


#endif // INFOWIDGET_H

