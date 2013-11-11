/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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
#include "cppersonalizationadvanceview.h"
#include <cpcategorysettingformitemdata.h>
#include <cpitemdatahelper.h>
/*#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <hblineedit.h>
#include <hbdataformmodelitem.h>
*/
#include "cpprofilesettingform.h"

#include <cppluginutility.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbstyleloader.h>

CpPersonalizationAdvanceView::CpPersonalizationAdvanceView(QGraphicsItem *parent /*=0*/):
    CpBaseSettingView(0,parent)
{
    // 1.init the dataform for advance settings here
    // 2.please modify the cpprofilesettingform.h/ cpp according to current ui spec
    // 3.NOTICE! you can get controlpanel's text map from the controlpanel ui wiki, use current text id for strings
    // 4.use cppersonalizationentryitemdata class to create ringtone, message tone...
    // 5.ignore cpmastervolumeslider class, please use default slideritem to create keyandscreen slider, new property added in slider, if any question, please contact me.
    // 6. don't forget the cfg file in data folder, you can try to add the profile activator plugin and ringtone plugin here
    HbStyleLoader::registerFilePath(":/resources/hbslider_color.css");
    mDataForm = new CpProfileSettingForm();
    setWidget( mDataForm );
    CpPluginUtility::addCpItemPrototype(mDataForm);
    initMenu();
}
CpPersonalizationAdvanceView::~CpPersonalizationAdvanceView()
{
}

void CpPersonalizationAdvanceView::initMenu()
{
    HbAction *editNameAction = new HbAction( this );
    editNameAction->setObjectName( "editNameAction" );
    editNameAction->setText( hbTrId( "txt_cp_list_edit_name" ) );
    
    menu()->addAction( editNameAction );
    connect( editNameAction, SIGNAL( triggered() ), mDataForm, SLOT( on_editNameAction_triggered() ));
}


