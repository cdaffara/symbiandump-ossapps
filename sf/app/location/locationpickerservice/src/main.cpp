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
* Description: main.cpp
*
*/

#include <HbApplication>
#include <HbTranslator>
#include "locationpickerappwindow.h"

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);   
    HbTranslator translator("lilpicker");    
    app.setApplicationName( hbTrId("txt_lint_title_select_location") );
    LocationPickerAppWindow mainWindow;
    mainWindow.viewport()->grabGesture(Qt::TapGesture);
    mainWindow.show();
    return app.exec();
}
