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

#ifndef IRSTATIONDETAILSVIEW_H_
#define IRSTATIONDETAILSVIEW_H_

#include <hbscrollarea.h>
#include "irbaseview.h"
#include "irdocumentloader.h"

class HbLabel;
class HbListWidget;
class IRQPreset;
class IRQSongHistoryInfo;

class IRStationDetailsView : public IRBaseView
{
    Q_OBJECT
    
public:
    ~IRStationDetailsView();
    void setDetails();
    void setDetails(const IRQPreset * aPreset);
    
protected:
    IRStationDetailsView(IRApplication* aApplication, TIRViewId aViewId);
    
private slots:

    void handleOrientationChanged( Qt::Orientation aOrientation );
    
private:
    void createDetails();
    // Methods to set station information

private:
    HbScrollArea *iScrollArea;
    HbWidget* iContainer;
    // Station information
    HbLabel *iStationName;
    HbLabel *iDescription;
    HbLabel *iGenres;
    HbLabel *iLanguage;
    HbLabel *iCountry;
    HbLabel *iBitRate;
    HbLabel *iNowPlayingUrl;
    friend class IRViewManager;
};

#endif /* IRSTATIONDETAILSVIEW_H_ */
