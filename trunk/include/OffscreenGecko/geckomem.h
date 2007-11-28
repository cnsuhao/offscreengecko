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

#ifndef __OFFSCREENGECKO_GECKOMEM_H__
#define __OFFSCREENGECKO_GECKOMEM_H__

/**\file
 * Gecko memory allocator access
 */

#include <memory.h>
#include "baseobj.h"

/**
 * Gecko memory allocator access. Using it is sometimes required when handing
 * memory back to Gecko (e.g. from custom class implementations).
 */
typedef OSGK_BaseObject OSGK_GeckoMem;

/**
 * Allocate some memory using Gecko's memory allocator.
 * \param mem Allocator object.
 * \param size Amount of memory to allocate.
 * \return Allocated block of memory or 0 if out of memory.
 */
OSGK_EXTERN_C OSGK_API void* osgk_geckomem_alloc (OSGK_GeckoMem* mem, size_t size);
/**
 * Free memory allocated using Gecko's memory allocator.
 * \param mem Allocator object.
 * \param p Block of memory to free.
 */
OSGK_EXTERN_C OSGK_API void osgk_geckomem_free (OSGK_GeckoMem* mem, void* p);
/**
 * Reallocate a block of memory allocated with Gecko's memory allocator with a
 * new size.
 * \param mem Allocator object.
 * \param p Block of memory to reallocate.
 * \param size New amount of memory.
 * \return Re-allocated block of memory or 0 if the reallocation failed.
 */
OSGK_EXTERN_C OSGK_API void* osgk_geckomem_realloc (OSGK_GeckoMem* mem, 
  void* p, size_t size);
/**
 * Allocate some memory using Gecko's memory allocator and copy some data into
 * into it.
 * \param mem Allocator object.
 * \param p Data to copy.
 * \param size Amount of memory to allocate and size of data to copy.
 * \return Allocated block of memory or 0 if out of memory.
 */
static OSGK_INLINE void* osgk_geckomem_clone (OSGK_GeckoMem* mem, const void* p, size_t size)
{
  void* newP = osgk_geckomem_alloc (mem, size);
  if (newP != 0)
    memcpy (newP, p, size);
  return newP;
}

#ifdef __cplusplus

namespace OSGK
{
  /**
   * Gecko memory allocator access. Using it is sometimes required when handing
   * memory back to Gecko (e.g. from custom class implementations).
   */
  struct GeckoMem : public CXXAPI::Wrap<OSGK_GeckoMem>
  {
    explicit GeckoMem (WrappedType* obj) : WrapperType (obj) {}
    GeckoMem& operator=(const GeckoMem& other)
    {
      WrapperType::operator=(other);
      return *this;
    }

    /**
     * Allocate some memory using Gecko's memory allocator.
     * \param size Amount of memory to allocate.
     * \return Allocated block of memory or 0 if out of memory.
     */
    void* Alloc (size_t size)
    {
      return osgk_geckomem_alloc (GetObject(), size);
    }
    /**
     * Free memory allocated using Gecko's memory allocator.
     * \param p Block of memory to free.
     */
    void Free (void* p)
    {
      return osgk_geckomem_free (GetObject(), p);
    }
    /**
     * Reallocate a block of memory allocated with Gecko's memory allocator with a
     * new size.
     * \param p Block of memory to reallocate.
     * \param size New amount of memory.
     * \return Re-allocated block of memory or 0 if the reallocation failed.
     */
    void* Realloc (void* p, size_t size)
    {
      return osgk_geckomem_realloc (GetObject(), p, size);
    }
    /**
     * Allocate some memory using Gecko's memory allocator and copy some data into
     * into it.
     * \param p Data to copy.
     * \param size Amount of memory to allocate and size of data to copy.
     * \return Allocated block of memory or 0 if out of memory.
     */
    void* Clone (const void* p, size_t size)
    {
      return osgk_geckomem_clone (GetObject(), p, size);
    }
  };
} // namespace OSGK
#endif

#endif // __OFFSCREENGECKO_GECKOMEM_H__
