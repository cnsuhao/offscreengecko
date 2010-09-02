/*

Copyright (c) 2008-2010, Frank Richter <frank.richter@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.
    * The name of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define usleep(x) Sleep(x)
#else
#include <unistd.h> // for usleep()
#endif

#include <OffscreenGecko/browser.h>

// TGA writing code, translated from tgawrite.cpp
static void tga_WriteByte (unsigned char c, FILE* out)
{ fwrite ((const char*)&c, 1, 1, out); }

static void tga_WriteWord (unsigned short s, FILE* out)
{ 
  // @@@ TODO: Support big endian machines
  fwrite ((const char*)&s, 2, 1, out); 
}
static void tga_WriteBGRAImage (int width, int height, const void* data, FILE* out)
{
  // TGA Header
  tga_WriteByte (0, out);  // ID length
  tga_WriteByte (0, out);  // Color map type
  tga_WriteByte (2, out);  // Image type - TrueColor
  tga_WriteWord (0, out);  // Color map stuff
  tga_WriteWord (0, out); 
  tga_WriteByte (0, out); 
  tga_WriteWord (0, out);  // X/Y offset
  tga_WriteWord (0, out); 
  tga_WriteWord (width, out); 
  tga_WriteWord (height, out); 
  tga_WriteByte (32, out); // Image depth
  tga_WriteByte (1 | 32, out); // 1 attribute; origin top left

  fwrite ((const char*)data, 1, width * height * 4, out);
}

int main (int argc, const char* const argv[])
{
  const char* uri;
  const char* outputFilename;
  // Rendering dimensions
  int renderW = 1024, renderH = 1024;
  int returnCode = 0;
  OSGK_GeckoResult embedInitResult;
  OSGK_Embedding* embedding;
  OSGK_Browser* browser;
  FILE* tgafile;
  const unsigned char* lockedData;

  if (argc != 3)
  {
    fputs ("expecting parameters: <URI to render> <output TGA file name>\n", stderr);
    return 1;
  }
  
  uri = argv[1];
  outputFilename = argv[2];
  
  // To use OSGK we first need an embedding object.
  embedding = osgk_embedding_create (&embedInitResult);
  if (!embedding)
  {
    // This means something went wrong
    fprintf (stderr, "Error initializing OSGK embedding (%.8x)\n", embedInitResult);
    return 1;
  }
  
  // The browser object is the main workhorse for web page rendering.
  browser = osgk_browser_create (embedding, renderW, renderH);
  if (!browser)
  {
    fputs ("Error creating OSGK browser\n", stderr);
    returnCode = 1;
    goto cleanupEmbedding;
  }
  
  // Instruct browser to navigate to the given URI
  osgk_browser_navigate (browser, uri);
  
  // Wait until the URI finished loading
  while (osgk_browser_query_load_state (browser) != loadFinished)
  {
    usleep (500);
  }
  
  // Get a size that covers all of the page without scrollbars
  if (!osgk_browser_get_preferred_dimensions (browser, &renderW, &renderH, renderW))
  {
    fputs ("Error when determining preferred image dimensions\n", stderr);
    returnCode = 1;
    goto cleanupBrowser;
  }
  
  // Set the browser size to the preferred size we just obtained
  osgk_browser_resize (browser, renderW, renderH);
  
  // Open output file
  tgafile = fopen (outputFilename, "wb");
  // Lock pixel data from browser object for retrieval
  lockedData = osgk_browser_lock_data (browser, 0);
  // Write output file
  tga_WriteBGRAImage (renderW, renderH, lockedData, tgafile);
  // Clean up file
  fclose (tgafile);
  // Unlock pixel data
  osgk_browser_unlock_data (browser, lockedData);
  
cleanupBrowser:
  // Clean up browser object
  osgk_release (browser);
cleanupEmbedding:
  // Clean up embedding object
  osgk_release (embedding);

  return returnCode;
}
