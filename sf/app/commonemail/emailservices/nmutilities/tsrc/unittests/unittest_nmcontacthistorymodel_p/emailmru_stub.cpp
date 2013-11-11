#include <emailmru.h>

extern bool constructorCalled;
extern bool destructorCalled;
extern bool entryCountCalled;
extern bool getEntryCalled;
extern bool nameOrderCalled;
extern bool fillMruWithFalseValues;
extern bool returnFalseFromGetEntry;

EmailMRU::EmailMRU()
{
    constructorCalled = true;
}

EmailMRU::~EmailMRU()
{
    destructorCalled = true;
}

quint32 EmailMRU::entryCount() const
{
    entryCountCalled  = true;

    if (!fillMruWithFalseValues)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

bool EmailMRU::getEntry(qint32 entryIndex, QString &displayName, QString &address)
{
    getEntryCalled  = true;
    bool success = false;

    if (!fillMruWithFalseValues)
    {
        switch (entryIndex)
        {
            case 1:
                displayName = "Camel Case";
                address = "camel.case@plusplus.com";
                success = true;
            break;
            case 2:
                displayName = "Agent Reference";
                address = "byvalue@plusplus.com";
                success = true;
            break;
            default:
                displayName = "Default Name";
                address = "default@default.com";
                success = false;
            break;
        }
    }
    else
    {
        switch (entryIndex)
        {
            case 1:
                // Without display name
                displayName = "";
                address = "camel.case@plusplus.com";
                success = true;
            break;
            case 2:
                // Email Matching purpose
                displayName = "Agent Plus";
                address = "plus@plusplus.com";
                success = true;
            break;
            case 3:
                // Match in other
                displayName = "Agent Smith";
                address = "noreply@mib.com";
                success = true;
            break;
            default:
                displayName = "Default Name";
                address = "default@default.com";
                success = false;
            break;
        }
    }

    if (returnFalseFromGetEntry) 
    {
        success = false;
    }

    return success;
}

int EmailMRU::nameOrder()
{
    nameOrderCalled  = true;
    return 2;
}
