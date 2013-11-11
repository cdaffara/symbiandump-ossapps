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

#ifndef LOGSSERVICETESTERWIDGET_H
#define LOGSSERVICETESTERWIDGET_H

#include <hbwidget.h>

class HbComboBox;
class HbCheckBox;
class HbLineEdit;
class LogsServiceTesterAppCloser;

class LogsServiceTesterWidget : public HbWidget
{
    Q_OBJECT

public:
    LogsServiceTesterWidget(QGraphicsItem *parent=0);
    ~LogsServiceTesterWidget();   

public slots:

    void startOldService();
    void startNewService();
    
    void dialerClosed();
    void dialerCloseError(int error);
    
private:
    
    void fetchData();
    
private:
    HbCheckBox* mCheckBox;
    HbComboBox* mComboBox;
    HbLineEdit* mEditor;
    
    int mViewIndex;
    bool mShowDialpad;
    QString mDialpadText;
    
    LogsServiceTesterAppCloser* mAppCloser;
};


#endif // LOGSSERVICETESTERWIDGET_H

