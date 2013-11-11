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

#include <hbdevicemessagebox.h>
#include <hbsplashscreen.h> 
#include <hbtranslator.h> 

#include "irhbapplication.h"
#include "irviewmanager.h"
#include "irapplication.h"
#include "irmemorycollector.h"
#include "irqlogger.h"
#include "irqsystemeventhandler.h"
#include "irqsettings.h"
#include "irservicedef.h"
#include "irviewdefinitions.h"
#include "irqsettings.h" 

bool isDiskSpaceLow(IRQSystemEventHandler* aEventHandler); 
void startSplashScreen();
TIRViewId getSplashViewID(); 

int main(int argc, char* argv[])
{   
    int  result = 0;
	  
    
    installLogDir();    
    INSTALL_MESSAGE_HANDLER;   
  
    IRHbApplication app(argc, argv, Hb::NoSplash);
    HbTranslator* translator = new HbTranslator();
    Q_ASSERT( NULL != translator );
    translator->loadCommon(); 
     
	  //we start the splash screen here for the translator will be 
	  //installed ok only after the HbApplication is constructed ready
    startSplashScreen();  
    
    //for the note should be translated too     
    IRQSystemEventHandler *systemEventHandler = new IRQSystemEventHandler();
    if( isDiskSpaceLow(systemEventHandler) )
    {         
        delete translator;
        delete systemEventHandler;
        return 0;
    }

    IRViewManager *mainWindow = new IRViewManager;    
    mainWindow->show();
    
    //here, we transfer the ownership of the event handler to the irapp
    IRApplication *irapp = new IRApplication(mainWindow, systemEventHandler);    
    IRMemoryCollector mc(irapp);
    
    app.initMattiAutomation();
    if(!mainWindow->isExiting())
    {
        result =  app.exec();                  
    }    
            
    delete translator; 
	  return result;
}

bool isDiskSpaceLow(IRQSystemEventHandler* aEventHandler)
{     
    bool ret = aEventHandler->isBelowCriticalLevel();
    if(ret)
    {
#ifdef SUBTITLE_STR_BY_LOCID
        HbDeviceMessageBox messageBox(hbTrId("txt_irad_info_insufficient_disk_space"),
                HbMessageBox::MessageTypeWarning);
#else
        HbDeviceMessageBox messageBox(hbTrId("Insufficient disk space"),
                HbMessageBox::MessageTypeWarning);        
#endif
        messageBox.setTimeout(HbPopup::NoTimeout);
        messageBox.exec();
    }
    return ret;
} 

void startSplashScreen()
{   
    TIRViewId viewId = getSplashViewID();    
    switch( viewId )
    {
    case EIRView_PlayingView:
         HbSplashScreen::setScreenId("nowplaying_screen");
         break;
    case EIRView_CategoryView:      
    case EIRView_FavoritesView:       
    case EIRView_MainView: 
    default: 
        HbSplashScreen::setScreenId("normal_screen");
        break;    
    } 
    
    HbSplashScreen::start();
}

TIRViewId getSplashViewID()
{ 
    TIRViewId startingViewID;
    IRQSettings *settings = IRQSettings::openInstance();
    settings->getStartingViewId(startingViewID);
    settings->closeInstance();
    return startingViewID;    
}

 
 
