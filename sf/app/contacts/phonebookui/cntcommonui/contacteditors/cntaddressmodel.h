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

#ifndef CNTDETAILADDRESSEDITORMODEL_H_
#define CNTDETAILADDRESSEDITORMODEL_H_

#include "cntdetaileditormodel.h"
#include <hbdataformmodelitem.h>
#include <qtcontacts.h>
#include "cntglobal.h"
#include <maptileservice.h>

QTM_BEGIN_NAMESPACE
class QContact;
class QContactAddress;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
class CntAddressModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntAddressModel( QContact* aContact );
    ~CntAddressModel();
    
    void saveContactDetails();
    QContactDetail detail() const;
    
private:
    void createAddressItems( HbDataFormModelItem* aGroup, QContactAddress* aAddress );
    bool saveAddressItems( HbDataFormModelItem* aGroup, QContactAddress* aAddress );
    bool isAddressEmpty( QContactAddress* aAddress ) const;
private:
    QContactAddress *mAddress;
    QContactAddress *mAddressHome;
    QContactAddress *mAddressWork;
    
protected:
    bool mIsLocationPickerEnabled;
    MapTileService* mMaptileInterface;
	
    friend class T_AddressEditorTest;
    };
#endif /* CNTDETAILADDRESSEDITORMODEL_H_ */
