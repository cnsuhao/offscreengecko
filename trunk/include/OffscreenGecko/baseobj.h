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

#ifndef __OFFSCREENGECKO_BASEOBJ_H__
#define __OFFSCREENGECKO_BASEOBJ_H__

/**\file
 * Base object
 */

#include "defs.h"

/**\mainpage
 * This is the OffscreenGecko API documentation.
 *
 * Before you hack away make sure to take a look at the 
 * \ref Basics "object basics".
 *
 * If you're programming in C, you may be interested in the 
 * <a href="globals_func.html">list of functions</a>.
 *
 * If you're programmin in C++ instead, you might be more interested in
 * <a href="annotated.html">the class list</a>.
 */

/**\page Basics OffscreenGecko object basics
 * The OffscreenGecko API is centered around objects. Whatever you do with it,
 * an object is involved: Initializing the library is equivalent to creating
 * an object. A "browser" to display is an object. And so on.
 *
 * All objects are <i>reference counted</i> for lifetime management. That
 * means if a reference/pointer to an object is stored anywhere for a somewhat
 * longer time, a reference must be "added". Likewise, is the referenced/pointed
 * to object is not needed any more, the reference must be "released". That way
 * the object is cleaned up automatically when it's not used at all any more.
 * One important point here is that reference adding and releasing must be
 * balanced, ie each reference added must be released at some point.
 *
 * There is one exception where a reference <i>must not</i> be added: when
 * storing an object returned from a "create" funtion. Object creation
 * implicitly adds a reference and the creation function "gives" that reference
 * back to the caller. Thus adding a reference would actually cause an excess
 * reference, released by noone, meaning that the object can't be properly
 * cleaned up.
 *
 * (These are just very very terse descriptions of "object orientedness" and
 * "reference counting". If these concepts are new to you, it's *strongly*
 * recommended you get some literature on the subjects.)
 *
 * \section BasicsC Dealing with OffscreenGecko objects (C)
 * In the C interface objects are represented by opaque pointers. To add a
 * reference, use the osgk_addref() function. To release a references, use the
 * osgk_release() function.
 *
 * \section BasicsCXX Dealing with OffscreenGecko objects (C++)
 * The OffscreenGecko C++ interface wraps all OffscreenGecko objects into C++
 * classes; so instead of calling a function and passing in the opaque object
 * pointer you can directly a method on the object. Furthermore, reference
 * counting is handled automatically when an object is created, destroyed,
 * copied etc.
 *
 * However, to use these classes efficiently, an implementation detail must be
 * noted: the classes merely wrap the opaque pointers from the C interface.
 * This has several implications:
 * - A copy is not a true copy - the "copy" is merely an additional reference
 *   to the same object as the original.
 * - Creating objects on the heap is somewhat wasteful, it will just allocate
 *   the storage for the pointer and add another layer of indirection.
 * In that sense, the wrapper classes have a behaviour that more resembles Java
 * references to classes than "true" C++ inlined classes.
 *
 * An instance of the wrapped object is created immediately when the wrapper
 * class is constructed. Delayed creation is still possible, though: for that,
 * the wrapper must be initialized with a <i>null reference</i>. Later, a new
 * instance of the wrapper for the desired object can be assigned. Example:
 * \code
 * // Initializes as a "null reference"
 * OSGK::Browser myBrowser (OSGK::Browser::Null());
 *
 * // ... do other stuff ...
 *
 * // Now actually create an object
 * myBrowser = OSGK::Browser (...);
 * \endcode
 */

struct OSGK_BaseObject_s
{
#ifdef OSGK_BUILD
  ///\internal Object reference count
  int refCount;
#else
# ifndef __cplusplus
  int reserved;
# endif
#endif
};
/**
 * Base OffscreenGecko object. All other types derive from this.
 */
typedef struct OSGK_BaseObject_s OSGK_BaseObject;

/*#ifdef __cplusplus
# ifdef BUILD_OSGK
#   define OSGK_DERIVEDTYPE(T)     class T;
# else
#   define OSGK_DERIVEDTYPE(T)            \
      class T : public OSGK_BaseObject {  \
      }
# endif
#else*/
# define OSGK_DERIVEDTYPE(T)            \
    typedef struct T ## _s {            \
      OSGK_BaseObject baseobj;          \
    } T
//#endif

/**
 * Acquire a reference to an OffscreenGecko object. Returns new reference 
 * count.
 */
OSGK_EXTERN_C OSGK_API int osgk_addref (OSGK_BaseObject* obj);

/// Release a reference to an OffscreenGecko object.
OSGK_EXTERN_C OSGK_API int osgk_release (OSGK_BaseObject* obj);

static OSGK_INLINE int osgk_addref_real (OSGK_BaseObject* obj)
{
  return osgk_addref (obj);
}

static OSGK_INLINE int osgk_release_real (OSGK_BaseObject* obj)
{
  return osgk_release (obj);
}

#define osgk_addref(obj)    osgk_addref_real (&((obj)->baseobj))
#define osgk_release(obj)   osgk_release_real (&((obj)->baseobj))

#ifdef __cplusplus
namespace OSGK
{
  namespace CXXAPI
  {
    /** Wrapper class for all OffscreenGecko objects; automatically
     *   handles reference counting.
     */
    template<typename T>
    class Wrap
    {
    public:
      typedef T WrappedType;
      typedef Wrap WrapperType;
    protected:
      /// Wrapped object
      T* obj;
    public:
      static WrappedType* Null() { return 0; }
    
      /// Construct with taking ownership of a given object.
      explicit Wrap (T* obj) : obj (obj) {}
      /// Construct by adding another reference to an object.
      Wrap (const Wrap& other) : obj (other.obj)
      {
	if (obj != 0) osgk_addref (obj);
      }
      /// Release reference to an object.
      ~Wrap()
      {
	if (obj != 0) osgk_release (obj);
      }
      /// Copy the object from another reference.
      Wrap& operator=(const Wrap& other)
      {
	if (other.obj != 0) osgk_addref (other.obj);
	if (obj != 0) osgk_release (obj);
	obj = other.obj;
	return *this;
      }
      
      /// Check if the reference pointed to is valid.
      bool IsValid() const { return obj != 0; }
      /// Release the reference.
      void Invalidate() { *this = Wrap (Null()); }
      
      /**
       * Directly set the wrapped object.
       * \warning Does not increase the reference count of the new object
       *   or decrease the reference count of the old! Use an assignment
       *   operator for that.
       */
      void SetObject (T* obj) { this->obj = obj; }
      /// Directly get the wrapped object.
      T* GetObject() const { return obj; }
    };
  } // namespace CXXAPI
} // namespace OSGK
#endif
  
  
#endif // __OFFSCREENGECKO_BASEOBJ_H__
