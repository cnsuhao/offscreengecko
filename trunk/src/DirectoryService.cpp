#include "defs_private.h"
#include "DirectoryService.h"
#include "pathutil.h"

#include GECKO_INCLUDE(xpcom,nsDirectoryServiceDefs.h)
#include GECKO_INCLUDE(xpcom,nsILocalFile.h)

namespace OSGK
{
  namespace Impl
  {
    NS_IMPL_ISUPPORTS1(DirectoryService::DirEnum,
      nsISimpleEnumerator)

    NS_IMETHODIMP DirectoryService::DirEnum::HasMoreElements (PRBool* _retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;
      *_retval = pos < paths.size();
      return NS_OK;
    }

    NS_IMETHODIMP DirectoryService::DirEnum::GetNext (nsISupports **_retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;

      nsCOMPtr<nsILocalFile> pathObj;
      nsresult res = GetLocalFile (paths[pos++].c_str(), pathObj);
      if (NS_FAILED (res)) return res;

      nsILocalFile* pathObjPtr;
      pathObj.forget (&pathObjPtr);
      *_retval = pathObjPtr;
      
      return NS_OK;
    }

    NS_IMPL_ISUPPORTS2(DirectoryService, 
      nsIDirectoryServiceProvider,
      nsIDirectoryServiceProvider2)

    NS_IMETHODIMP DirectoryService::GetFile (const char *prop, PRBool *persistent, 
                                             nsIFile **_retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;
      *_retval = 0;
      return NS_ERROR_NOT_AVAILABLE;
    }

    NS_IMETHODIMP DirectoryService::GetFiles (const char *prop, 
                                              nsISimpleEnumerator **_retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;
      if (strcmp (prop, NS_XPCOM_COMPONENT_DIR_LIST) == 0)
      {
        *_retval = new DirEnum (this, componentsPaths);
        NS_ADDREF (*_retval);
        return NS_OK;
      }

      *_retval = 0;
      return NS_ERROR_NOT_AVAILABLE;
    }

    void DirectoryService::AddComponentsPath (const char* path)
    {
      std::string pathStr (path);
      PathExpand (pathStr);
      componentsPaths.push_back (pathStr);
    }
  } // namespace Impl
} // namespace OSGK
