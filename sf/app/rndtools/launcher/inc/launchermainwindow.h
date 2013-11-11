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
* Description:
*
*/


#ifndef LauncherMainWindow_H_
#define LauncherMainWindow_H_

#include <hbmainwindow.h>
#include <hbview.h>

class HbApplication;
class ApplicationView;
class OutputView;
class EngineWrapper;
class CommonActions;

class LauncherMainWindow : public HbMainWindow
    {
public:
    
    LauncherMainWindow ( QWidget *parent = 0 );
    virtual ~LauncherMainWindow ();
    
    int init( HbApplication &app );
    
    void openApplicationView();
    void openOutputView();
    /**
     * Prints text into output
     */
    void printText(const QString &text);
    
private: //data
    EngineWrapper* mEngineWrapper;
    ApplicationView* mApplicationView;
    OutputView* mOutputView;
    CommonActions* mCommonActions;
    };

#endif /* LauncherMainWindow_H_ */
