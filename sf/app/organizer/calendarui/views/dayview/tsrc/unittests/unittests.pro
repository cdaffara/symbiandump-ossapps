#
#  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
#  All rights reserved.
#  This component and the accompanying materials are made available
#  under the terms of "Eclipse Public License v1.0"
#  which accompanies this distribution, and is available
#  at the URL "http://www.eclipse.org/legal/epl-v10.html".
#  
#  Initial Contributors:
#  Nokia Corporation - initial contribution.
#  
#  Contributors: 
#  
#  Description: calendayview project file for all unittests
#

TEMPLATE = subdirs

SUBDIRS =  unittest_calendaycontentwidget \
           unittest_calendaystatusstrip \
           unittest_calendayutils \
           unittest_calendayitem \
           unittest_calendayhourelement \
           unittest_calendayhourscrollarea \
           unittest_calendayeventspane \
           unittest_calendayinfo \
           unittest_calendaycontainer \
           unittest_calendaymodel \
	   unittest_calendayitemview \
	   unittest_calendaymodelmanager \
	   unittest_calendayview \
	   unittest_calendaycontentscrollarea
           
CONFIG += symbian_test

# End of file	--Don't remove this.