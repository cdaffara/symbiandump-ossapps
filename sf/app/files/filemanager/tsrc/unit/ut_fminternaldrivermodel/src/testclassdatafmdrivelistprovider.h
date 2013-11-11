/*
 * testDataClassFmDriveListProvider.h
 *
 *  Created on: Jul 13, 2010
 *  
 */

#ifndef TESTCLASSDATAFMDRIVELISTPROVIDER_H
#define TESTCLASSDATAFMDRIVELISTPROVIDER_H
#include "fmdrivemodel.h"

class TestDataClassFmDriveListProvider: public FmDriveListProvider
{
public:
    TestDataClassFmDriveListProvider();
    ~TestDataClassFmDriveListProvider();
    virtual void getDriveList( QStringList &driveList );
    
};

#endif //TESTCLASSDATAFMDRIVELISTPROVIDER_H

TestDataClassFmDriveListProvider::TestDataClassFmDriveListProvider()
{
    
}

TestDataClassFmDriveListProvider::~TestDataClassFmDriveListProvider()
{
    
}

void TestDataClassFmDriveListProvider::getDriveList(QStringList &driveList)
{
    driveList<<"drive1:"
             <<"drive2:"
             <<"drive3:";             
}
