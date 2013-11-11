#include "coemain.h"
CCoeEnv *CCoeEnv::Static(  )
    {
    return 0;
    }

CCoeStatic *CCoeEnv::Static(TUid /*aUid*/)
    {
    return 0;
    }

CCoeStatic::CCoeStatic()
    {    
    }

CCoeStatic::~CCoeStatic()
    {    
    }

CCoeStatic::CCoeStatic(TUid /*aUid*/, TScope /*aScope*/)
    {    
    }

CCoeStatic::CCoeStatic(TUid /*aUid*/, TInt /*aDestructionPriority*/, TScope /*aScope*/ )
    {    
    }

void CCoeStatic::CCoeStatic_Reserved1(  )
    {
    }

void CCoeStatic::CCoeStatic_Reserved2(  )
    {
    }
    
#include <bautils.h>
TBool BaflUtils::FileExists(const RFs& /*aFs*/,const TDesC& /*aFileName*/)
{
    return ETrue;
}
