#include "defs_private.h"
#include "mozilla-config.h"

#include "componentmgr.h"

#include "OffscreenGecko/componentmgr.h"

#include GECKO_INCLUDE(xpcom,nsIComponentRegistrar.h)

namespace OSGK
{
  namespace Impl
  {
    ComponentMgr::ComponentMgr ()
    {
      initResult = NS_GetComponentManager (getter_AddRefs (nsCompMgr));
      if (NS_FAILED (initResult)) return;
      initResult = NS_GetComponentRegistrar (getter_AddRefs (nsCompReg));
    }

    nsresult ComponentMgr::RegisterXPCOMFactory (const nsCID& aClass, 
      const char* className, const char* contractID, nsIFactory* factory)
    {
      if (NS_FAILED (initResult)) return initResult;

      return nsCompReg->RegisterFactory (aClass, className, contractID, factory);
    }
  } // namespace Impl
} // namespace OSGK

int osgk_componentmgr_register_xpcom_factory (OSGK_ComponentMgr* compMgr, 
  const struct nsID* aClass, const char* className,
  const char* contractID, nsIFactory* factory,
  OSGK_GeckoResult* geckoResult)
{
  nsresult rc = static_cast<OSGK::Impl::ComponentMgr*> (compMgr)->
    RegisterXPCOMFactory (*aClass, className, contractID, factory);
  if (geckoResult) *geckoResult = rc;
  return NS_SUCCEEDED (rc);
}
