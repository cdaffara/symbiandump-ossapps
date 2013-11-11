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
* Description:  stub hbdeviceprofile for testing video collection ui uitils
* 
*/

#ifndef HBDEVICEPROFILE_H
#define HBDEVICEPROFILE_H

#include <QSize>



class  HbDeviceProfile 
{
public:
    
    /**
     * contructor
     */
    HbDeviceProfile(){mSize.setHeight(100); mSize.setWidth(100);}

    /**
     * destructor
     */
    ~HbDeviceProfile(){};

    /**
     * returns static object reference from this class
     */
    static HbDeviceProfile& current(){ static HbDeviceProfile _profile; return _profile; }
    
    /**
     * sets mSize
     */
    void setLogicalSize(QSize size){
        mSize.setHeight(size.height()); 
        mSize.setWidth(size.width());
        }
    
    /**
     * gets mSize
     */
    QSize logicalSize() const{return mSize;}
    
    QSize mSize;
           
};

#endif
