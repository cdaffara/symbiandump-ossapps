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
 * Description: Menu view.
 *
 */

#include <HbMainWindow>
#include <HbView>
#include <HbInstance>
#include "hsmainwindow.h"
#include "hsmenuview.h"
#include "hsmenumodewrapper.h"
#include "hsscene.h"
#include "hspage.h"
#include "hspagevisual.h"
#include "hswallpaper.h"
/*!
 Constructor
 */



/*
 * A word about HsMainWindow class.
 * This is not an utility class. It was designed to isolate HbMainWindow connected code, so
 * it can be substitued in MT for application library. Unfortunately it seems to be the best
 * way to cope with HbMainWindow problem in MT.  
 */


HsMainWindow::HsMainWindow(HsMenuModeWrapper* wrapper):mMenuMode(wrapper) 
{
}

/*!
 Destructor
 */
HsMainWindow::~HsMainWindow()
{
}

/*!
 Add the \menuView to first instance of HbMainWindow registered in HbInstance.
 \param view View to be added to HbMainWindow.
*/
void HsMainWindow::setCurrentView(HbView *view)
{
    HSMENUTEST_FUNC_ENTRY("HsMainWindow::setCurrentView");
    HbMainWindow *const hbW(
        HbInstance::instance()->allMainWindows().value(0));
    
    if (mMenuMode && mMenuMode->getHsMenuMode() == Hs::AddHsMenuMode) {
        view->setTitle(hbTrId("txt_applib_title_select_item"));
    } else {
        view->setTitle(hbTrId("txt_applib_title_applications"));
    }
        
    if (!hbW->views().contains(view)) {
        hbW->addView(view);
    }
    
    // For some reason all visible items will pe rendered even they are outside of the viewport
    // Hide inactive pages and wallpapers to make screen shot faster
    HsScene *scene(HsScene::instance()); 	
    QList<HsPage *> pages(scene->pages());
    foreach (HsPage *p, pages) {
        if (p != scene->activePage()) {
        		p->wallpaper()->hide();
            p->visual()->hide();
        } 
    }
    bool animate  = !hbW->isObscured();       
    // source view will be rendered to cache
    hbW->setCurrentView(view, animate, Hb::ViewSwitchCachedFullScreen);
   	// restore previous
   	foreach (HsPage *p, pages) {
        if (p != scene->activePage()) {
        		p->wallpaper()->show();
            p->visual()->show();
        } 
    }
    HSMENUTEST_FUNC_EXIT("HsMainWindow::setCurrentView");
}
