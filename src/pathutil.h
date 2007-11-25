#ifndef __PATHUTIL_H__
#define __PATHUTIL_H__

#include <direct.h>
#include <string>

#include GECKO_INCLUDE(xpcom,nscore.h)
#include GECKO_INCLUDE(xpcom,nsCOMPtr.h)

#if defined(XP_WIN) || defined(XP_OS2)
  #define FILE_PATH_SEPARATOR       '\\'
#else
  #define FILE_PATH_SEPARATOR       '/'
#endif

namespace OSGK
{
  namespace Impl
  {
    static inline bool IsPathSeparator (unsigned int ch)
    {
      return (ch == '/') || (ch == '\\');
    }

#ifdef _MSC_VER
#define getcwd  _getcwd
#define chdir   _chdir
#endif

    static void PathExpand (std::string& path)
    {
#ifdef _MSC_VER
      char* pathExpanded = _fullpath (0, path.c_str(), 0);
      path = pathExpanded;
      free (pathExpanded);
#else
      char* saveCwd = getcwd (0, MAX_PATH);
      chdir (path.c_str());
      char* newCwd = getcwd (0, MAX_PATH);
      chdir (saveCwd);
      path = newCwd;
      free (saveCwd);
      free (newCwd);
#endif
    }

    nsresult GetLocalFile (const char* str, nsCOMPtr<nsILocalFile>& localFile);
  } // namespace Impl
} // namespace OSGK

#endif // __PATHUTIL_H__