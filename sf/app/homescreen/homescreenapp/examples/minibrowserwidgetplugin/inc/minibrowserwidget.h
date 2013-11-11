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
* Description:  Example of home screen widget
*
*/

#ifndef MINIBROWSERWIDGET_H
#define MINIBROWSERWIDGET_H

#include <HbWidget>
class HbLineEdit;
class QGraphicsWebView;
class HbIconItem;
class HbPushButton;

class MiniBrowserWidget : public HbWidget
{
    Q_OBJECT
// Start of snippet 1
    Q_PROPERTY(bool isOnline READ isOnline WRITE setOnline)
// End of snippet 1
// Start of snippet 2
    Q_PROPERTY(QString url READ url WRITE setUrl)
// End of snippet 2
    
public:
    MiniBrowserWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~MiniBrowserWidget();
    
public:
    bool isOnline();
    void setOnline(bool online);
    QString url() const;
    void setUrl(const QString& url);
    
public slots:
    void onInitialize();
    void onShow();
    void onHide();

public slots:
    void pressGoButton();
    void pressStopButton();
    void loadFinished(bool ok);
    void loadProgress(int progress);
    void loadStarted();

// Start of snippet 3
signals:
    void setPreferences(const QStringList &names);
// End of snippet 3

private:
    Q_DISABLE_COPY(MiniBrowserWidget)
    
    HbPushButton *mGoButton;
    HbPushButton *mStopButton;
    HbLineEdit *mEditor;
    QGraphicsWebView *mWeb;
    HbIconItem *mSpinnerAnimationWidget;
    bool mOnline;    
    QString mUrl;
    
};

#endif // MINIBROWSERWIDGET_H
