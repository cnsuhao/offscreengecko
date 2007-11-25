/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is OffscreenGecko code.
 *
 * The Initial Developer of the Original Code is
 * Frank Richter <frank.richter@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __EMBEDDING_H__
#define __EMBEDDING_H__

#include <stdarg.h>
#include <vector>

#include GECKO_INCLUDE(xpcom,nsStringAPI.h)
#include GECKO_INCLUDE(xulapp,nsXULAppAPI.h)

#include "OffscreenGecko/embedding.h"

#include "baseobj_private.h"
#include "DirectoryService.h"
#include "OffscreenComponents.h"

namespace OSGK
{
  namespace Impl
  {
    class EmbeddingOptions : public BaseObject
    {
      nsRefPtr<DirectoryService> directoryService;
    public:
      std::vector<std::string> searchPaths;

      void AddSearchPath (const char* path);
      void AddComponentsPath (const char* path);

      nsIDirectoryServiceProvider* GetDirectoryService ()
      { return directoryService; }
    };

    class Embedding : public BaseObject
    {
      int xpcom_init_level;
      OffscreenComponents components;

      XRE_InitEmbeddingType XRE_InitEmbedding;
      XRE_TermEmbeddingType XRE_TermEmbedding;
    public:
      Embedding (EmbeddingOptions* opt, OSGK_GeckoResult& result);
      ~Embedding();

      void DebugPrint (const wchar_t* format, ...);
      void DebugPrintV (const wchar_t* format, va_list args);
    };
  } // namespace Impl
} // namespace OSGK

#endif // __EMBEDDING_H__
