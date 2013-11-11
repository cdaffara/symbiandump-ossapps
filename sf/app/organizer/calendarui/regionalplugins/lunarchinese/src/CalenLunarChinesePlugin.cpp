/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Lunar Plugin 
 *
*/


#include <QString>
#include <QStringList>
#include <HbAction>
#include <HbView>
#include <HbMenu>
#include <HbMessageBox>
#include <HbLabel>
#include <HbTranslator>

#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

#include "calendarui_debug.h" 
#include "CalenLunarChinesePlugin.h"
#include "CalenLunarPanic.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenLunarLocalizer.h"
#include "CalenLunarInfoProvider.h"
#include "CalendarPrivateCRKeys.h"
#include "calenRegionalInfoData.h"
#include "hb_calencommands.hrh"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CalenLunarChinesePluginTraces.h"
#endif


//CONSTANTS
static const QString fieldSeparator("\n");
static const QString headerSeparator("\n  ");

const TInt KFieldCount = 5;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CCalenLunarChinesePlugin
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin::CCalenLunarChinesePlugin(MCalenServices* aServices)
	:iLocalizer(NULL),
	 iLocInfo(NULL),
	 iServices(aServices),
	 iInfoBarText(NULL)
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_CCALENLUNARCHINESEPLUGIN_ENTRY );
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_CCALENLUNARCHINESEPLUGIN_EXIT );
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CreateChinesePluginL
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin* CCalenLunarChinesePlugin::NewL( 
													MCalenServices* aServices )
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_NEWL_ENTRY );
	CCalenLunarChinesePlugin* self = new( ELeave ) 
										CCalenLunarChinesePlugin( aServices);
	CleanupStack::PushL( self );
	self->ConstructL();
    CleanupStack::Pop(self);
	TRACE_EXIT_POINT;
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_NEWL_EXIT );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::~CCalenLunarChinesePlugin
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin::~CCalenLunarChinesePlugin()
	{
	OstTraceFunctionEntry0( DUP1_CCALENLUNARCHINESEPLUGIN_CCALENLUNARCHINESEPLUGIN_ENTRY );
	//Deregister services
	if ( iServices )
        {
        iServices->CancelNotifications(this);
        iServices->Release();
        }
    if(iInfoBarText)
 	    {
 	    delete iInfoBarText;
 	    iInfoBarText = NULL;
 	    }
    if(iRegionalInfo)
        {
         delete iRegionalInfo;
         iRegionalInfo = NULL;
        }
	delete iInfoProvider;
	iInfoProvider = NULL;
	delete iLocalizer;
	iLocalizer = NULL;
	delete iLocInfo;
	iLocInfo = NULL;
	
	// Remove the translator for plugin
	if (iTranslator) {
		delete iTranslator;
		iTranslator = 0;
	}
	OstTraceFunctionExit0( DUP1_CCALENLUNARCHINESEPLUGIN_CCALENLUNARCHINESEPLUGIN_EXIT );
	}
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ContructL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::ConstructL()
	{
    OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_CONSTRUCTL_ENTRY );
    
    RArray<TCalenNotification> notifications;
    notifications.Append(ECalenNotifyContextChanged);
    notifications.Append(ECalenNotifyCloseDialogs);
    
    iServices->RegisterForNotificationsL( this, notifications );
    
    notifications.Close();
    
    
	iServices->GetCommandRange( iStart, iEnd );
	
	// Install the translator before the CCalenLunarLocalizer is constructed
	iTranslator = new HbTranslator("calenregional");
	
	iLocalizer = CCalenLunarLocalizer::NewL();
	
	iInfoProvider = CCalenLunarInfoProvider::NewL( 
											CEikonEnv::Static()->FsSession() );
    //Qt class having a slot ,calls when user clicked show lunar data option
	iRegionalInfo = new CalenRegionalInfo(*this);
	
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_CONSTRUCTL_EXIT );
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::FormatExtraRowStringL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::FormatExtraRowStringL()

    {
    OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_FORMATEXTRAROWSTRINGL_ENTRY );
    if ( iLocInfo )
        {
        iExtraRowText.Set( iLocalizer->GetExtraRowTextL( *iLocInfo) );
        }
    else 
        {
        iExtraRowText.Set( KNullDesC );
        }
    OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_FORMATEXTRAROWSTRINGL_EXIT );
    }
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::InfobarL
// -----------------------------------------------------------------------------
//	
HbWidget* CCalenLunarChinesePlugin::InfobarL( )
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_INFOBARL_ENTRY );
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_INFOBARL_EXIT );
	return NULL;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Hitchcock views
// ----------------------------------------------------------------------------
//
QString* CCalenLunarChinesePlugin::InfobarTextL( )
    {
    OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_INFOBARTEXTL_ENTRY );
    if(iInfoBarText)
        {
        delete iInfoBarText;
        iInfoBarText = NULL;
        }
    //Update the local information based on current context
    //from framework.
    UpdateLocalizerInfoL();  
    FormatExtraRowStringL();
    iInfoBarText = iExtraRowText.AllocLC();
    CleanupStack::Pop();
    
    return  (new QString((QChar*)iInfoBarText->Des().Ptr(),iInfoBarText->Length()));
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called to add menuitem 
// ----------------------------------------------------------------------------
//

void CCalenLunarChinesePlugin::CustomiseMenu(HbMenu* aHbMenu)
    {
    OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_CUSTOMISEMENU_ENTRY );
    HbAction* lunarAction = new HbAction(hbTrId("txt_calendar_opt_show_lunar_data"));
    QList<QAction*> actionList = aHbMenu->actions();     
    TInt count = actionList.count() - 1;  //To show the option "show lunar data"
                                          //as a second last action item  for all 
                                          //Menu
    if(count >= 0)
    aHbMenu->insertAction(actionList[count], lunarAction);
    //calls a slot whenever user clicked "show lunar data" option
    QObject::connect(lunarAction,SIGNAL(triggered()), iRegionalInfo,SLOT(showRegionalDetails()));
    OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_CUSTOMISEMENU_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CCalenLunarChinesePlugin::HandleCommandL( const TCalenCommand&  aCommand )
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_HANDLECOMMANDL_ENTRY );
	const TInt commandId = aCommand.Command();
	if(ECalenRegionalPluginTapEvent == commandId)
		{
		ShowDetailsL();	
		}
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_HANDLECOMMANDL_EXIT );
	return EFalse;	
	}

// ----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CalenCommandHandlerExtensionL
//
// ----------------------------------------------------------------------------
//
TAny* CCalenLunarChinesePlugin::CalenCommandHandlerExtensionL( 
														TUid /*aExtensionUid*/ )
    {
    OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_CALENCOMMANDHANDLEREXTENSIONL_ENTRY );
    OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_CALENCOMMANDHANDLEREXTENSIONL_EXIT );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CommandHandlerL
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenLunarChinesePlugin::CommandHandlerL( TInt aCommand )
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_COMMANDHANDLERL_ENTRY );
	MCalenCommandHandler*  commandHandler = NULL;	
		
	if(ECalenRegionalPluginTapEvent == aCommand)
		{
		commandHandler = this;	
		}
   
    OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_COMMANDHANDLERL_EXIT );
    return commandHandler;
	}
 	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::UpdateInfoLabelL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::UpdateLocalizerInfoL()
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_UPDATELOCALIZERINFOL_ENTRY );
	QDateTime focusDateTime= iServices->Context().focusDateAndTime();
	TDateTime tempDateTime(
					focusDateTime.date().year(),
					static_cast<TMonth>(focusDateTime.date().month() - 1),
					focusDateTime.date().day() - 1, focusDateTime.time().hour(),
					focusDateTime.time().minute(), 0, 0);
	TTime focusTime(tempDateTime);
    TCalenLunarInfo lunarInfo = iInfoProvider->GetLunarInfoL( focusTime );
    delete iLocInfo;
	iLocInfo = NULL;
	if(iLocalizer)
		{
		iLocInfo = iLocalizer->LocalizeL( lunarInfo );	
		}
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_UPDATELOCALIZERINFOL_EXIT );
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::HandleNotification
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::HandleNotification( 
										const TCalenNotification aNotification )
	{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_HANDLENOTIFICATION_ENTRY );
	if (aNotification == ECalenNotifyContextChanged) {
		TRAP_IGNORE(UpdateLocalizerInfoL());
	} else if(aNotification == ECalenNotifyCloseDialogs) {
		if(mPopup){
			//close the popup
			mPopup->done(0);
		}
	}
	OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_HANDLENOTIFICATION_EXIT );
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ShowDetailsL
// Message popup from menu
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::ShowDetailsL( )
{
	OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_SHOWDETAILSL_ENTRY );
	QString msgText;
	if(iLocInfo) {
		QStringList headerIds;
		headerIds.append(hbTrId("txt_calendar_info_festival"));
		headerIds.append(hbTrId("txt_calendar_solar_term"));
		headerIds.append(hbTrId("txt_calendar_lunar_date"));
		headerIds.append(hbTrId("txt_calendar_animal_year"));
		headerIds.append(hbTrId("txt_calendar_gregorian_date"));

		QStringList fieldInfo;
		QString data;

		data = QString::fromUtf16(iLocInfo->Festival().Ptr(),iLocInfo->Festival().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->SolarTerm().Ptr(),iLocInfo->SolarTerm().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->FullLunarDate().Ptr(),iLocInfo->FullLunarDate().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->AnimalYear().Ptr(),iLocInfo->AnimalYear().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->GregorianDate().Ptr(),iLocInfo->GregorianDate().Length());
		fieldInfo.append(data);
		// Append the data in the proper order
		for (TInt i=0; i < KFieldCount; i++) {
			if(!fieldInfo.at(i).isEmpty()) {
				msgText.append(headerIds[i]);
				msgText.append(headerSeparator);
				msgText.append(fieldInfo[i]);
				if ( i < KFieldCount - 1 ) { 
					// not last item
					msgText.append(fieldSeparator);
				}
			}
		}
	}
	ExecuteMessageDialogL(msgText);
OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_SHOWDETAILSL_EXIT );
}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ExecuteMessageDialogL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::ExecuteMessageDialogL(QString aMsgText)
	{
    OstTraceFunctionEntry0( CCALENLUNARCHINESEPLUGIN_EXECUTEMESSAGEDIALOGL_ENTRY );

    // Instantiate a popup
    mPopup = new HbMessageBox();
    mPopup->setDismissPolicy(HbDialog::NoDismiss);
    mPopup->setTimeout(HbDialog::NoTimeout);
    mPopup->setIconVisible(false);
    mPopup->setAttribute(Qt::WA_DeleteOnClose, true);

    mPopup->setHeadingWidget(new HbLabel(hbTrId(
            "txt_calendar_title_lunar_calendar")));
    mPopup->setText(aMsgText);

    // Remove the default actions
    QList<QAction*> list = mPopup->actions();
    for (int i = 0; i < list.count(); i++)
        {
        mPopup->removeAction(list[i]);
        }
    // Sets the primary action
    mPopup->addAction(new HbAction(hbTrId(
            "txt_common_button_close_singledialog"), mPopup));

    // Launch popup
    mPopup->open();

    OstTraceFunctionExit0( CCALENLUNARCHINESEPLUGIN_EXECUTEMESSAGEDIALOGL_EXIT );
    }
//EOF


