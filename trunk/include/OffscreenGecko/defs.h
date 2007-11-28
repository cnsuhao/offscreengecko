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

#ifndef __OFFSCREENGECKO_DEFS_H__
#define __OFFSCREENGECKO_DEFS_H__

/**\file
 * Definitions used throughout OffscreenGecko headers
 */

#ifdef __cplusplus
#  define OSGK_EXTERN_C		extern "C"
#  define OSGK_DEFAULT_ARG(X)	= X
#  define OSGK_CLASSTYPE_DEF	class
#  define OSGK_CLASSTYPE_REF
#else
#  define OSGK_EXTERN_C	extern
#  define OSGK_DEFAULT_ARG(X)
#  define OSGK_CLASSTYPE_DEF	struct
#  define OSGK_CLASSTYPE_REF	struct
#endif

#ifdef WIN32
#  ifdef BUILD_OSGK
#    define OSGK_API	_declspec(dllexport)
#  else
#    define OSGK_API	_declspec(dllimport)
#  endif
#else
#  ifdef __GNUC__
#    define OSGK_API	__attribute__((visibility("default")))
#  else
#    define OSGK_API
#  endif
#endif

#include <assert.h>
#define OSGK_ASSERT(x)	assert(x)

/// A Gecko result code
typedef unsigned int OSGK_GeckoResult;

#if defined(__cplusplus) || defined(__GNUC__)
#  define OSGK_INLINE	inline
#elif defined(_MSC_VER)
#  define OSGK_INLINE	__inline
#else
#  define OSGK_INLINE
#endif

#endif // __OFFSCREENGECKO_DEFS_H__
