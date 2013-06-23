/***************************************************************************
 *   Copyright (C) 2013 by Pavlo Oleshkevych                               *
 *   oleshkevych@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Include the standard headers for cout to write some output to the console.
#include <iostream>
#include <string>

// Now include all podofo header files
#include <podofo/podofo.h>

// All podofo classes are member of the PoDoFo namespace.
using namespace PoDoFo;

void PrintHelp()
{
    std::cout << "Application for disabling spot colors in PDF files." << std::endl
              << " =====here will be some brief descrition of what this app is doing ========" << std::endl
              << "======repo at git ==============" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  pdf-spot-disabler in.pdf [out.pdf] spotName1 [spotName2] [...] [spotNameN]" << std::endl << std::endl;
    std::cout << "  spotName is the name of the spot color being disabled." << std::endl; 
    std::cout << "           It can be full (eg. \"Pantone 877 C\" or \"Pantone" << std::endl;
    std::cout << "           Process Magenta C\" or \"My Custom Spot\")" << std::endl;
    std::cout << "           or partial (eg. \"877\" or \"Magenta\" )." << std::endl;
    std::cout << "           spotName is case independent (eg. \"Pantone 877 C\"" << std::endl;
    std::cout << "           equals \"pantone 877 c\" or \"PANTONE 877 C\")." << std::endl << std::endl;
}

const PoDoFo::PdfName noneColor("None");

int main( int argc, char* argv[] )
{
    /*
     * Check if a filename was passed as commandline argument.
     * If more than 1 argument or no argument is passed,
     * a help message is displayed and the application
     * will quit.
     */
    if( argc < 3 )
    {
        PrintHelp();
        return -1;
    }

    // Load a document into a PdfMemDocument
    PdfMemDocument pdf(argv[1]);
    // Iterate over each page
    for (int pn = 0; pn < pdf.GetPageCount(); ++pn) 
    {
        PoDoFo::PdfPage* page = pdf.GetPage(pn);

        // Iterate over all the PDF commands on that page:
        PoDoFo::PdfObject* res = (*page).GetResources();
        if (res == NULL) std::cout << "\n\nEpic Fail!\n\n";
        if( (*res).IsDictionary() && (*res).GetDictionary().HasKey( "ColorSpace" ) )
        {
          if ( (*res).GetDictionary().GetKey( "ColorSpace" )->IsDictionary() )
          {
            PoDoFo::PdfDictionary cs = (*res).GetDictionary().GetKey( "ColorSpace" )->GetDictionary();
            // Go through all keys
            PoDoFo::TKeyMap::iterator it = cs.GetKeys().begin();
            while( it != cs.GetKeys().end() )
              {
                if ( (*it).second->IsReference() )
                {
                  if (pdf.GetObjects().GetObject( (*it).second->GetReference() )->IsArray())
                  {
                    PoDoFo::PdfArray colorArray = pdf.GetObjects().GetObject( (*it).second->GetReference() )->GetArray();
                    if ( colorArray.GetSize() > 1
                         && colorArray[0].IsName()
                         && colorArray[0].GetName().GetEscapedName() == "Separation" )
                    {
                      if ( (colorArray[1]).IsName() )
                          std::cout << colorArray[1].GetName().GetEscapedName() << std::endl;
                          colorArray[1] = noneColor;
                          std::cout << colorArray[1].GetName().GetEscapedName() << std::endl;
                    }
                  }
                }
                ++it;
              }
          }
        }
    } 

    pdf.Write(argv[2]);
    return 0;
}
