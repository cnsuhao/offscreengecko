#ifndef __DIRECTORYSERVICE_H__
#define __DIRECTORYSERVICE_H__

#include <vector>

#include GECKO_INCLUDE(xpcom,nsAutoPtr.h)
#include GECKO_INCLUDE(xpcom,nsIDirectoryService.h)
#include GECKO_INCLUDE(xpcom,nsISimpleEnumerator.h)

namespace OSGK
{
  namespace Impl
  {
    class DirectoryService : public nsIDirectoryServiceProvider2
    {
      typedef std::vector<std::string> StringVec;
      class DirEnum : public nsISimpleEnumerator
      {
        nsRefPtr<DirectoryService> parent;
        StringVec& paths;
        size_t pos;
      public:
        DirEnum (DirectoryService* parent,
          StringVec& paths) : parent (parent), paths (paths), pos (0) {}

        NS_DECL_ISUPPORTS
        NS_DECL_NSISIMPLEENUMERATOR
      };
      StringVec componentsPaths;
    public:
      NS_DECL_ISUPPORTS
      NS_DECL_NSIDIRECTORYSERVICEPROVIDER
      NS_DECL_NSIDIRECTORYSERVICEPROVIDER2

      void AddComponentsPath (const char* path);
    };
  } // namespace Impl
} // namespace OSGK

#endif // __DIRECTORYSERVICE_H__
