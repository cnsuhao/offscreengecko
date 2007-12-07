#ifndef __COMPONENTMGR_H__
#define __COMPONENTMGR_H__

#include "baseobj_private.h"

#include GECKO_INCLUDE(xpcom,nscore.h)
#include GECKO_INCLUDE(xpcom,nsCOMPtr.h)
#include GECKO_INCLUDE(xpcom,nsIFactory.h)

#include "OffscreenGecko/componentmgr.h"

namespace OSGK
{
  namespace Impl
  {
    class ComponentMgr : public BaseObject<OSGK_ComponentMgr>
    {
      nsresult initResult;

      nsCOMPtr<nsIComponentManager> nsCompMgr;
      nsCOMPtr<nsIComponentRegistrar> nsCompReg;
    public:
      ComponentMgr ();

      nsresult RegisterXPCOMFactory (const nsCID& aClass, 
        const char* className, const char* contractID, nsIFactory* factory);
    };
  } // namespace Impl
} // namespace OSGK

#endif // __COMPONENTMGR_H__
