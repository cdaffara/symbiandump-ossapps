/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Bio control for Provisioning documents.
*
*/

#include <hbapplication.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <HbTextItem>
#include <HbFontSpec>
#include <HbMessageBox>
#include <QGraphicsLinearLayout>
#include <xqserviceprovider.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>
#include <e32base.h>
#include <flogger.h>
#include <CWPEngine.h>
#include <HbGroupBox>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbLabel>
#include <msvstd.h>
#include <smsclnt.h>
#include <mtclreg.h>
#include <smuthdr.h>
#include <QDateTime>
#include "CWPNameValue.h"
#include "CpQtSpView.h"
#include "CpQtSpMainWindow.h"
#include "ProvisioningDebug.h"
#include "ProvisioningUIDs.h"
#include "CWPBioControl.h"
const QString DATE_TIME_FORMAT("dd.MM.yy hh:mm");

// ----------------------------------------------------------------------------
// CpQtSpView::CpQtSpView
// Description: Construtor
// ----------------------------------------------------------------------------


CpQtSpView::CpQtSpView(CpQtSp* service,CpQtSpMainWindow* ptr, QGraphicsItem *parent)
    : HbView(parent), iServiceProvider(service), window(ptr)
{ 
       setTitle(hbTrId("txt_device_update_title_messaging"));
       iViewStatus = true;
       iOrientationFlag = false;
}

// ----------------------------------------------------------------------------
// CpQtSpView::~CpQtSpView
// Description: Destructor
// ----------------------------------------------------------------------------


CpQtSpView::~CpQtSpView()
{
}

// ----------------------------------------------------------------------------
// CpQtSpView::corruptMessageDialog
// Description: Construction of view for provisioning message
// ----------------------------------------------------------------------------

void CpQtSpView::corruptMessageDialog(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->primaryAction())
    {
    window->closeWindow();
    }   
}
// ----------------------------------------------------------------------------
// CpQtSpView::constructView
// Description: Construction of view for provisioning message
// ----------------------------------------------------------------------------

void CpQtSpView::constructView()
{
    iBio = iServiceProvider->returnBio();
    TInt err( KErrNone );
    TRAP(err, array = iBio->CollectItemsLC(); CleanupStack::Pop());
   
    if(err != KErrNone)
    	{
        array->ResetAndDestroy();
        HbMessageBox *box = new HbMessageBox(hbTrId("txt_device_update_info_unable_to_open_message_f"));
    	box->setAttribute(Qt::WA_DeleteOnClose);
    	box->setTimeout(HbPopup::NoTimeout);
    	box->open(this,SLOT(corruptMessageDialog(HbAction*)));        
    	}
    else
        {
        window->buildView();
        }
} 
// ----------------------------------------------------------------------------
// CpQtSpView::buildView
// Description: Construction of view for provisioning message
// ----------------------------------------------------------------------------

bool CpQtSpView::buildView(QGraphicsLinearLayout* layout)
{
    //Gather information about provisioning message
    TMsvId tmsvid = iServiceProvider->returnId();
    CMsvSession* session = iServiceProvider->returnSession();
    CMsvEntry* entry = session->GetEntryL(tmsvid);
    entry->SetEntryL(tmsvid);
    TMsvEntry index = entry->Entry();
    
    // Details about message sender    
    TBufC<KLENGTH> details = index.iDetails;
    QString fromStringName((QChar*)details.Ptr(),details.Length());
    QString fromString = "From: ";    
    QString from;
    from.append(fromString);
    from.append(fromStringName);
    

    
    //Construction of group box
    group = new HbGroupBox();
    connect(group,SIGNAL(toggled(bool)),this,SLOT(expand()));
    group->setHeading(fromStringName);            
    HbLabel* label = new HbLabel(from);
    group->setContentWidget(label);
    group->setCollapsed(true);
    orientationChanges();
    layout->addItem(group);
    iOrientationFlag = true;
    //Message recieved time
    _LIT(KUnixEpoch, "19700000:000000.000000");
    QDateTime retTimeStamp;
    TTime timeStamp = index.iDate;
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    timeStamp.DaysFrom(unixEpoch);     
    retTimeStamp.setTime_t(seconds.Int());
    QString qtime = retTimeStamp.toString(DATE_TIME_FORMAT);      
    HbTextItem* time = new HbTextItem(qtime,this);
    layout->addItem(time);
    
    //Blank Line    
    HbTextItem* blankLine = new HbTextItem("", this);    
    layout->addItem(blankLine);    
    
    //Details about save message
    QString cfgMsg = hbTrId("txt_device_update_dblist_configuration_message");
    QString saveMsg = hbTrId("txt_device_update_dblist_product_code_val_save_to");
    HbTextItem* mPrimaryItem1 = new HbTextItem(cfgMsg, this);
    HbTextItem* mSecondaryItem2 = new HbTextItem(saveMsg, this);
    
    //Provisioning setting details
    iPrimaryFont = new HbFontSpec(HbFontSpec::Primary);
    iSecondaryFont = new HbFontSpec(HbFontSpec::Secondary);
    
    QFont font = iPrimaryFont->font();
    mPrimaryItem1->setFont(font);
            
    font = iSecondaryFont->font();
    mSecondaryItem2->setFont(font);
    
    layout->addItem(mPrimaryItem1);
    layout->addItem(mSecondaryItem2);

    
    TBufC<KLENGTH> Name;
    TBufC<KLENGTH> Value;    
    HbTextItem* mPrimaryItem[100];
    HbTextItem* mSecondaryItem[KLENGTH];
    HbTextItem* mBlank[KLENGTH];
    
    QFont pfont = iPrimaryFont->font();
    QFont sfont = iSecondaryFont->font();      
    
    
	TBuf<KLENGTH> currentTitle(KNullDesC);
	
    for( TInt item = 0; item < array->Count(); item++ )
    {  	
    	CWPNameValue* pair = ((*array)[item]);
    	Name = pair->Name();
    	if(Name.Compare(currentTitle) !=0 )
    	{
    	QString qname((QChar*)Name.Ptr(),Name.Length());
    	mPrimaryItem[item] = new HbTextItem(qname);    	
    	mPrimaryItem[item]->setFont(pfont); 
    	mBlank[item] = new HbTextItem("", this);
    	mBlank[item]->setFont(pfont);    	
    	layout->addItem(mBlank[item]);
    	layout->addItem(mPrimaryItem[item]);
    	currentTitle.Copy(Name);
    	}
    	    	
    	Value = pair->Value();
    	QString qvalue((QChar*)Value.Ptr(),Value.Length());   
    	mSecondaryItem[item] = new HbTextItem(qvalue);
    	mSecondaryItem[item]->setFont(sfont);
    	layout->addItem(mSecondaryItem[item]);  
   }

    array->ResetAndDestroy();
    return true;
}

// ----------------------------------------------------------------------------
// CpQtSpView::saveProvisoningMessage
// Description: Saves the provisioning message
// ----------------------------------------------------------------------------


void CpQtSpView::saveProvisoningMessage()
{
    iBio->SaveSettingsL();
}

// ----------------------------------------------------------------------------
// CpQtSpView::expand
// Description: Alignement of groupbox according to orientation
// ----------------------------------------------------------------------------


void CpQtSpView::expand()
	{
    this->orientationChanges();	
	}

// ----------------------------------------------------------------------------
// CpQtSpView::orientationChanges
// Description: Group box orientation changes
// ----------------------------------------------------------------------------

void CpQtSpView::orientationChanges()
    {
    if(window->orientation() == Qt::Horizontal)
       {   
           if(!group->isCollapsed())
           {
             group->setMinimumSize(KLANDSCAPE,KEXPANDED);
             group->setMaximumSize(KLANDSCAPE,KEXPANDED);            
           }
           else
           {       
             group->setMinimumSize(KLANDSCAPE,KCOLLAPSED);
             group->setMaximumSize(KLANDSCAPE,KCOLLAPSED);               
           }
       }
       else
       {
           if(!group->isCollapsed())
           {
             group->setMinimumSize(KPORTRAIT,KEXPANDED);
             group->setMaximumSize(KPORTRAIT,KEXPANDED);         
           }
           else
           {       
             group->setMinimumSize(KPORTRAIT,KCOLLAPSED);
             group->setMaximumSize(KPORTRAIT,KCOLLAPSED);                
           }
       }
    }
    
// ----------------------------------------------------------------------------
// CpQtSpView::getOrientationFlag
// Description: Get the Orientation status
// ----------------------------------------------------------------------------
bool CpQtSpView::getOrientationFlag()
    {
    return iOrientationFlag;
    }
// ----------------------------------------------------------------------------
// CpQtSpView::setHorizontal
// Description: Landscape alignment
// ----------------------------------------------------------------------------


void CpQtSpView::setHorizontal()
	{
  	if(!group->isCollapsed())
			{
	  		group->setMinimumSize(KPORTRAIT,KEXPANDED);
	  		group->setMaximumSize(KPORTRAIT,KEXPANDED);			
		 	}
			else
			{		
				group->setMinimumSize(KPORTRAIT,KCOLLAPSED);
		   	group->setMaximumSize(KPORTRAIT,KCOLLAPSED);				
			}
	 
	}

// ----------------------------------------------------------------------------
// CpQtSpView::setVertical
// Description: Portrait alignment
// ----------------------------------------------------------------------------


void CpQtSpView::setVertical()
	{
  	if(!group->isCollapsed())
			{
	  		group->setMinimumSize(KLANDSCAPE,KEXPANDED);
	  		group->setMaximumSize(KLANDSCAPE,KEXPANDED);			
		 	}
			else
			{		
				group->setMinimumSize(KLANDSCAPE,KCOLLAPSED);
		   	group->setMaximumSize(KLANDSCAPE,KCOLLAPSED);				
			}
	}

// ----------------------------------------------------------------------------
// CpQtSpView::deleteMessage
// Description: Deletion of provisioning message
// ----------------------------------------------------------------------------


void CpQtSpView::deleteMessage()
{   
    TMsvId entryID = iServiceProvider->returnId();
    CMsvSession* session = iServiceProvider->returnSession();
    CMsvEntry* entry = session->GetEntryL(entryID);
    CleanupStack::PushL(entry);
    entry->DeleteL(entryID);
    CleanupStack::PopAndDestroy(entry);
    window->close();
}



// ----------------------------------------------------------------------------
// CpQtSpView::requestCompleted
// Description: Framework call
// ----------------------------------------------------------------------------

void CpQtSpView::requestCompleted(const QVariant& /*value*/)
{
}

// ----------------------------------------------------------------------------
// CpQtSp::CpQtSp
// Description: constructor
// ----------------------------------------------------------------------------


CpQtSp::CpQtSp(CpQtSpMainWindow *ptr, QObject* parent)
: XQServiceProvider("com.nokia.services.MDM.Provisioning",parent)
{  
    window = ptr;
    publishAll();
}

// ----------------------------------------------------------------------------
// CpQtSp::~CpQtSp
// Description: destructor
// ----------------------------------------------------------------------------

CpQtSp::~CpQtSp()
{
	delete iObserver;
	delete iSession;
	delete iBioCtrl;
}

// ----------------------------------------------------------------------------
// CpQtSp::complete
// Description: Framework call
// ----------------------------------------------------------------------------

void CpQtSp::complete()
{
   QString retvalue ="";
   completeRequest(1,retvalue);
}

// ----------------------------------------------------------------------------
// CpQtSp::ProcessMessage
// Description: Messaging application calls this function via QTHighway
// ----------------------------------------------------------------------------

QString CpQtSp::ProcessMessage(const QString& tmsvid)
{
    iId = tmsvid.toInt();	
    TMsvSelectionOrdering sort;    
    sort.SetShowInvisibleEntries(ETrue); 
    iObserver = new(ELeave) CObserver;
    iSession = CMsvSession::OpenSyncL(*iObserver); 
    iBioCtrl = CWPBioControl::NewL(iSession,iId,this); 
    iBioCtrl->RestoreMsgL();
    return "";
}

void CpQtSp::authenticationStatus()
{
    TInt ret = iBioCtrl->getAuth();    
    switch(ret)
    {
		  case KAUTENTICATIONCANCEL:
		  	{
     		window->close();
     		}
     		break;
     	
     	case KAUTENTICATIONFAILURE:
     		{     			
     		CMsvEntry* entry = iSession->GetEntryL(iId);
    		CleanupStack::PushL(entry);
    		entry->DeleteL(iId);
    		CleanupStack::PopAndDestroy(entry);
    		window->close();
    		}
    		break;

     	case KAUTENTICATIONSUCCESS:
     		{
     		iBioCtrl->updateAuthetication();
    		emit showView();
		    setCurrentRequestAsync();
		  	}
        break;
    }    
}

// ----------------------------------------------------------------------------
// CpQtSp::returnId
// Description: used by CpQtSpView
// ----------------------------------------------------------------------------

TMsvId CpQtSp::returnId()
{
	return iId;
}

// ----------------------------------------------------------------------------
// CpQtSp::returnSession
// Description: used by CpQtSpView
// ----------------------------------------------------------------------------

CMsvSession* CpQtSp::returnSession()
{
  return iSession;
}

// ----------------------------------------------------------------------------
// CpQtSp::returnBio
// Description: used by CpQtSpView
// ----------------------------------------------------------------------------

CWPBioControl* CpQtSp::returnBio()
	{
		return iBioCtrl;
	}
