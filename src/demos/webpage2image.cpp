/*

Copyright (c) 2008, Frank Richter <frank.richter@gmail.com>
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
#include <unistd.h> // for usleep()

#include <OffscreenGecko/browser.h>

#include <fstream>
#include "../common/tgawrite.h"

int main (int argc, const char* const argv[])
{
  if (argc != 3)
  {
    fputs ("expecting parameters: <URI to render> <output TGA file name>\n", stderr);
    return 1;
  }
  
  const char* uri = argv[1];
  const char* outputFilename = argv[2];
  // Rendering dimensions
  int renderW = 1024, renderH = 1024;
  
  // To use OSGK we first need an embedding object.
  OSGK_GeckoResult embedInitResult;
  OSGK::Embedding embedding (embedInitResult);
  if (!embedding.IsValid())
  {
    // This means something went wrong
    fprintf (stderr, "Error initializing OSGK embedding (%.8x)\n", embedInitResult);
    return 1;
  }
  
  // The browser object is the main workhorse for web page rendering.
  OSGK::Browser browser (embedding, renderW, renderH);
  if (!browser.IsValid())
  {
    fputs ("Error creating OSGK browser\n", stderr);
    return 1;
  }
  
  // Instruct browser to navigate to the given URI
  browser.Navigate (uri);
  
  // Wait until the URI finished loading
  while (browser.QueryLoadState() != loadFinished)
  {
    usleep (500);
  }
  
  // Get a size that covers all of the page without scrollbars
  if (!browser.GetPreferredDimensions (renderW, renderH, renderW))
  {
    fputs ("Error when determining preferred image dimensions\n", stderr);
    return 1;
  }
  
  // Set the browser size to the preferred size we just obtained
  browser.Resize (renderW, renderH);
  
  // Open output file
  std::ofstream tgastream;
  tgastream.open (outputFilename, std::ios::out | std::ios::binary);
  // Lock pixel data from browser object for retrieval
  const unsigned char* lockedData = browser.LockData();
  // Write output file
  OSGK::TGAWriter::WriteBGRAImage (renderW, renderH, lockedData, tgastream);
  // Clean up file
  tgastream.close ();
  // Unlock pixel data
  browser.UnlockData (lockedData);
  
  return 0;
}
