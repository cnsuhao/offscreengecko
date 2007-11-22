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

#ifndef __OFFSCREENGECKO_EMBEDDING_H__
#define __OFFSCREENGECKO_EMBEDDING_H__

/**\file
 * Embedding object
 */

#include "baseobj.h"

typedef OSGK_BaseObject OSGK_Embedding;

/**
 * Create a new embedding object. An embedding is the 'mother' of all
 * OffscreenGecko objects.
 * \param geckoResult Returns the result code from Gecko. Can be 0.
 */
OSGK_EXTERN_C OSGK_API OSGK_Embedding* osgk_embedding_create (
  OSGK_GeckoResult* geckoResult OSGK_DEFAULT_ARG(0));

#ifdef __cplusplus

namespace OSGK
{
  /**
   * OffscreenGecko embedding object, the 'mother' of all OffscreenGecko
   * objects.
   * \sa \ref BasicsCXX
   */
  struct Embedding : public CXXAPI::Wrap<OSGK_Embedding>
  {
    /**
     * Create a new embedding object.
     */
    Embedding() : WrapperType (osgk_embedding_create()) { }
    /**
     * Create a new embedding object.
     * \param result Returns the result code from Gecko. Can be 0.
     */
    Embedding (OSGK_GeckoResult& result) : 
      WrapperType (osgk_embedding_create (&result)) { }

    explicit Embedding (WrappedType* obj) : WrapperType (obj) {}
    Embedding& operator=(const Embedding& other)
    {
      WrapperType::operator=(other);
      return *this;
    }
  };
  
} // namespace OSGK
#endif

#endif // __OFFSCREENGECKO_EMBEDDING_H__
