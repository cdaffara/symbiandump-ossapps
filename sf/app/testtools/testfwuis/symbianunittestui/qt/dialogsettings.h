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
* Description: Dialog class to set options when running tests.
*
*/

#ifndef DIALOGSETTINGS_H_
#define DIALOGSETTINGS_H_
#include <QDialog>
#include <QStringList>

class QCheckBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class Settings;

class DialogSettings: public QDialog
    {
    Q_OBJECT
    
public:
    DialogSettings(QWidget* parent = 0);
    virtual ~DialogSettings();
    
public:
    void restoreSettings();
    
private:
    void createTimeoutSettings();
    void setupTimeoutSettingsEventHandlers();
    void createOutputFormatSettings();
    void setupOutputFormatSettingsEventHandlers();
    void createMemSimSettings();
    void setupMemSimSettingsEventHandlers();
    void createButtons();
    void setupButtonsEventHandlers();
    void load();
    void setLayout();    
    QWidget* setTimeoutCtlsLayout();
    QWidget* setOutputCtlsLayout();
    QWidget* setButtonsLayout();
    QWidget* setMemAllocFailureSimCtrlLayout();
    void setBtnSaveEnabled();
    bool getTimeout(int *);
    
signals:
    void settingsSaved(const Settings*);
    
private slots:
    void setLineEditTimeoutEnabled(int chkState);
    void setComboxFormatEnabled(int chkState);
    void setMemAllocFailureSim(int chkState);
    void changeIndex(int);
    void changeText(const QString&);
    void saveSettings();
    
private:    
    QCheckBox*      chkBoxTimeout;
    QCheckBox*      chkBoxMemAllocFailureSim;
    QCheckBox*      chkBoxOutputFormat;
    QLabel*         labelTimeoutUnit;
    QLineEdit*      lineEditTimeout;
    QComboBox*      comboBoxOutputFormat;
    QPushButton*    btnSave;
    QPushButton*    btnCancel;
    
    // Populated data
    QStringList listFormats;
    
    // Data
    Settings* savedSettings;
    };

#endif /* DIALOGSETTINGS_H_ */
