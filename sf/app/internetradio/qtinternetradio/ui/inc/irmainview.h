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
#ifndef IRMAINVIEW_H
#define IRMAINVIEW_H

#include "irabstractlistviewbase.h"
#include "irqevent.h"

class IRMainModel;

class IRMainView : public IrAbstractListViewBase
{
	Q_OBJECT
public:
	~IRMainView();
	    
protected:
    //only friend class IRViewManager can create this view
    IRMainView(IRApplication *aApplication, TIRViewId aViewId);
    
private slots:
	/*
	 * Description : slot function for active network event
	 * Parameters  : aEvent, see the definition of IRQNetworkEvent
	 */
	void networkRequestNotified(IRQNetworkEvent aEvent);
	
private:
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
    //from base class IrAbstractListViewBase
    void handleItemSelected();
 
    void itemAboutToBeSelected(bool& aNeedNetwork);
 
    void setCheckedAction();
 
    //from base class IRBaseView
    void updateView();

private:
    IRMainModel *iMainModel;

    enum TUserSelection
    {
        EBrowseByCountry,
        EBrowseByLanguage,
        ERecentlyPlayedStations,
        ESongsPlayed,
        EPlayList
    };
    
    friend class IRViewManager;
};

#endif // IRMAINVIEW_H

