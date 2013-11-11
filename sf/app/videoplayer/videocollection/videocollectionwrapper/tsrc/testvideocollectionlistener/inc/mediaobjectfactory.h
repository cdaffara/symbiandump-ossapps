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
* Description:  Helper class for creating mediaobjects for testing CVideoListDataModel class methods*
*/


#ifndef __TESTVIDEOLISTMEDIAFACTORY_H__
#define __TESTVIDEOLISTMEDIAFACTORY_H__

#include <e32cmn.h>
#include "mpxmedia.h"
class CMPXMediaArray;


class MediaObjectFactory 
{

public:
    /**
     * constructor.
     */
    MediaObjectFactory();
    
    /**
     * destructor
     */
    ~MediaObjectFactory();
    
    /**
     * returns pointer to new created media-array 
     */
    CMPXMediaArray* newMediaArray();
 
    /**
     * creates media-item
     * If using default value for id (-1)
     * no id will be saved to media
     * 
     * @id used as part of data values
     * @MediaDetailSelection
     * 
     * @return CMPXMedia 
     */
    CMPXMedia* newMedia(int id = -1, int id2 = 0);
    
    /**
     * appends media object to mediaarray
     * 
     * @whereToPut where to put
     * @param whatToPut what value to put
     * 
     */
    void putArrayContent(CMPXMediaArray *whereToPut, CMPXMedia *whatToPut);
    
    /**
     * puts wanted type value to wanted attribute
     * 
     * @whereToPut where to put
     * @attribute what attribute to pu
     * @param value what value to put
     * 
     */
    template<class T>
    void putTValue(CMPXMedia *whereToPut, const TMPXAttributeData& attribute, T& value)
    {
        if(!whereToPut)
        {
            return;
        }
        TRAP_IGNORE(whereToPut->SetTObjectValueL<T>(attribute, value));
    }
    
    /**
     * puts given ptr to ptr with given attribute
     * 
     * @whereToPut where to put
     * @attribute what attribute to pu
     * @param ptrToPut what value to put
     * 
     */
    template<class T>
    void putValuePtr(CMPXMedia *whereToPut, const TMPXAttributeData& attribute, T* ptrToPut)
    {
        if(!whereToPut)
        {
            return;
        }
        TRAP_IGNORE(whereToPut->SetCObjectValueL<T>( attribute, ptrToPut ));

    }
    
      
private:
    

};


#endif  // __TESTVIDEOLISTMEDIAFACTORY_H__

// End of file
    


