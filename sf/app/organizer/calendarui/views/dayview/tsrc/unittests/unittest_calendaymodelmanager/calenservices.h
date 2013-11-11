/*
 * calenservices.h
 *
 *  Created on: Jun 21, 2010
 *      Author: zabinpio
 */

#ifndef CALENSERVICES_H_
#define CALENSERVICES_H_

#include "calencontext.h"

/*!
 Mocked class MCalenServices
 */
class MCalenServices
{
public:
    MCalenServices() {
        
    }
    
    MCalenContext& Context(){
			return mContext;
    	}
    
    ~MCalenServices() {
        
    }
    
    MCalenContext mContext;
};
#endif /* CALENSERVICES_H_ */
