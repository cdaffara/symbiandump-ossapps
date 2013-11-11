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
*     Zhiqiang Yang <zhiqiang.yang@nokia.com>
*     Steven Yao <steven.yao@nokia.com>
*     Yong Zhang <yong.5.zhang@nokia.com>
* 
* Description:
*     The main application file for file manager
*
*/


#include "fmmainwindow.h"

#include <hbapplication.h>
#include <hbstyleloader.h>
#include <hbtranslator.h>
#include <QTranslator>
#include <QLocale>

int main( int argc, char *argv[] )
{
    FM_LOG( "main start" );
    HbApplication app( argc, argv );  
    
    HbTranslator translator("filemanager");
    translator.loadCommon();    
    app.setApplicationName( hbTrId("txt_fmgr_title_file_manager") );
    
    HbStyleLoader::registerFilePath(":/resources/hblistviewitem.css");    
    HbStyleLoader::registerFilePath(":/resources/hblistviewitem.widgetml");                                                
    
    FM_LOG( "main_createMainwindow start" );
    FmMainWindow mw;
    FM_LOG( "main_createMainwindow end" );

    mw.show(); 
    FM_LOG( "main end" );
    return app.exec();
}
