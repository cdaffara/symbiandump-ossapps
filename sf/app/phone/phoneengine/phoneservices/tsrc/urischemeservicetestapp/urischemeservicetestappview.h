/*!
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
* Description:
*
*/

#ifndef URISCHEMESERVICETESTAPPVIEW_H
#define URISCHEMESERVICETESTAPPVIEW_H

#include <QTime>
#include <QMap>
#include <HbView>

class HbMainWindow;
class HbLineEdit;
class HbCheckBox;
class HbLabel;

class UriSchemeServiceTestAppView : public HbView
{
    Q_OBJECT

public:
    UriSchemeServiceTestAppView(HbMainWindow& mainWindow);
    ~UriSchemeServiceTestAppView();

private slots:
    void makeCall(bool checked = false);
    void clearInputField(bool checked = false);
    void inputSpecialCharacter(bool checked);
    void setupTelUriConfiguration(int state);
    void setupCtiUriConfiguration(int state);

private:
    HbMainWindow& m_mainWindow;
    HbLineEdit *m_lineEdit;
    HbCheckBox *m_telUriCheckBox;
    HbCheckBox *m_ctiUriCheckBox;
    HbLabel *m_uriLabel;
    
    QTime m_multitapTimer;
    int m_multitapIndex;
    int m_multitapCount;
    QMap<int, QChar> m_extraChar;
    QList<int> m_multitapCharacters;
};

#endif // URISCHEMESERVICETESTAPPVIEW_H
