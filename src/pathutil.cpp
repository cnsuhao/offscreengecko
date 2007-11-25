#include "defs_private.h"
#include "pathutil.h"

#include GECKO_INCLUDE(xpcom,nsStringAPI.h)

namespace OSGK
{
  namespace Impl
  {
    nsresult GetLocalFile (const char* str, nsCOMPtr<nsILocalFile>& localFile)
    {
      NS_ConvertASCIItoUTF16 nsPath (str);
      return NS_NewLocalFile (nsPath, false, getter_AddRefs(localFile));
    }
  } // namespace Impl
} // namespace OSGK

