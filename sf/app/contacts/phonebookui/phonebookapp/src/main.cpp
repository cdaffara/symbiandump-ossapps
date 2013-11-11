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

#include <hbtranslator.h>
#include "cntmainwindow.h"
#include "cntappservicehandler.h"
#include "cntglobal.h"
#include "cntdebug.h"
#include "cntapplication.h"

int main(int argc, char **argv)
{
#if defined (TRACES) || defined (TRACE2FILE)
    qInstallMsgHandler(MSG_HANDLER);
#endif

    CntApplication a( argc, argv );

    //For translation, loading and installing translator
    HbTranslator translator("contacts");
    translator.loadCommon();
    
    CntAppServiceHandler serviceHandler;
    CntMainWindow mainWindow;

    mainWindow.show();

    return a.exec();
}

