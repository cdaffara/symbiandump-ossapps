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
#ifndef IROPENWEBADDRESSVIEW_H_
#define IROPENWEBADDRESSVIEW_H_

#include "irbaseview.h"
#include "irqevent.h"

class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class IRQPreset;
class HbLineEdit;
class HbPushButton;

/**
 * This class is for users to input a station manually.
 */

class IROpenWebAddressView : public IRBaseView
{
Q_OBJECT

public:
    ~IROpenWebAddressView();

    // from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);

protected:
    IROpenWebAddressView(IRApplication* aApplication, TIRViewId aViewId);
   
private:
    void initMenu();
    void initDetails();
    void initUrlAndName();
    void initDataForm();
    bool initPreset(IRQPreset &aPreset);
    bool eventFilter(QObject *object, QEvent *event);
    void hideVkb();

private:
    // from HbWidgetBase
    void gestureEvent(QGestureEvent *aEvent);
    
private slots:
    // play this station
    void play();

    // get network controller event.
    void networkRequestNotified(IRQNetworkEvent aEvent);

    // used to check the url editor status
    void urlEditorTextChanged(const QString &aString);

    void handleOrientationChanged(Qt::Orientation aOrientation);
    void handleSettingAction();

private:
    // station information
    HbDataForm          *iForm;
    HbDataFormModel     *iModel;
    HbDataFormModelItem *iUrl;
    HbDataFormModelItem *iName;
    HbLineEdit          *iNameEditorPtr;
    HbLineEdit          *iUrlEditorPtr;
    
    HbPushButton        *iPlayButton;
    friend class         IRViewManager;
};

#endif /* IROPENWEBADDRESSVIEW_H_ */
