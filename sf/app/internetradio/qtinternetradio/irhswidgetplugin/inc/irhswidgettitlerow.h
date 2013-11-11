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
* Description:  Internet Radio home screen widget
*
*/

#ifndef IRHSWIDGETTITLEROW_H_
#define IRHSWIDGETTITLEROW_H_

// System includes
#include <HbWidget>

//FORWARD DECLARATIONS:
class HbLabel;
class QEvent;

class IrHsWidgetTitleRow : public HbWidget
{
    Q_OBJECT
  
public:
    explicit IrHsWidgetTitleRow(QGraphicsItem *aParent = 0, Qt::WindowFlags aFlags = 0);
    virtual ~IrHsWidgetTitleRow();
    
    void setDefaultTitle();
    void setStationName(const QString &aStationName);
  
    void loadStationLogo();
    void setDefaultLogo();
    
signals:
    void titleRowClicked();
    
protected:
    // from HbWidgetBase
    void gestureEvent(QGestureEvent *aEvent);
	
    // from HbWidget
    void changeEvent(QEvent *event);

private:
    void loadUi();
    
private:
    HbLabel *mStationLogo;
    HbLabel *mStationName;
};

#endif // IRHSWIDGETTITLEROW_H_
