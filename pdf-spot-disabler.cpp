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

// Include all podofo header files
#include <podofo/podofo.h>
//Include command line options parser
#include "getopt/getopt_pp.h"


void PrintHelp()
{
    std::cout << "Application for disabling spot colors in PDF files." << std::endl
              << " =====here will be some brief descrition of what this app is doing ========" << std::endl
              << "======repo at git ==============" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  pdf-spot-disabler [-v] in.pdf [out.pdf] spotName1 [spotName2] [...] [spotNameN]" << std::endl << std::endl;
    std::cout << "  -v       lists spots of in.pdf that can be disabled." << std::endl; 
    std::cout << "  spotName is the name of the spot color being disabled." << std::endl; 
    std::cout << "           It can be full (eg. \"Pantone 877 C\" or \"Pantone" << std::endl;
    std::cout << "           Process Magenta C\" or \"My Custom Spot\")" << std::endl;
    std::cout << "           or partial (eg. \"877\" or \"Magenta\" )." << std::endl;
    std::cout << "           spotName is case independent (eg. \"Pantone 877 C\"" << std::endl;
    std::cout << "           equals \"pantone 877 c\" or \"PANTONE 877 C\")." << std::endl << std::endl;
}

const PoDoFo::PdfName NONE_COLOR("None");

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
    GetOpt::GetOpt_pp programOptions (argc, argv);
    if ( programOptions >> GetOpt::OptionPresent('h', "help") )
    {
      std::cout << "It Works!!!!!!!!!" << std::endl << std::endl;
    }
    // Load pdf file
    PoDoFo::PdfMemDocument pdfDoc(argv[1]);
    PoDoFo::PdfVecObjects pdfDocObjects = pdfDoc.GetObjects();
    std::vector<PoDoFo::PdfReference> colorReferences;
    // Setup output file for writing
    //PoDoFo::PdfOutputDevice pdfOutFile(argv[2]);
    // TODO: Exit with error if loading fails
    // Iterate over each page
    for ( int pn = 0; pn < pdfDoc.GetPageCount(); ++pn ) 
    {
        PoDoFo::PdfPage* page = pdfDoc.GetPage(pn);
        // Get the Resource dictionary of the current page:
        PoDoFo::PdfObject* pageResources = (*page).GetResources();
        if ( pageResources == NULL )
        {
            std::cout << "Error obtainig page\'s Resource dictionary!\n";
            return 1;
        } 
        // Get the ColorSpace subdictionary
        if( (*pageResources).IsDictionary()
            && (*pageResources).GetDictionary().HasKey("ColorSpace") 
            && (*pageResources).GetDictionary().GetKey("ColorSpace")->IsDictionary() )
        {
            PoDoFo::PdfDictionary colorSpace = (*pageResources).GetDictionary().
                                               GetKey("ColorSpace")->GetDictionary();
            /* The ColorSpace subdictionary contains entries
            * like Name : Reference. Name is something like
            * CS11, CS24 and Reference points to array
            * with actual values of color entry.
            */ 
            // Go through all entries of the ColorSpace subdictionary
            PoDoFo::TKeyMap::iterator it = colorSpace.GetKeys().begin();
            while( it != colorSpace.GetKeys().end() )
            {
                // Obtaining color array by reference
                if ( (*it).second->IsReference() 
                     && pdfDoc.GetObjects().
                        GetObject( (*it).second->GetReference() )->IsArray() )
                {
                    /* Color array for separation colorspace has 4 entries: 
                     * [ /Separation name alternateSpace tintTransform ]
                     * (see Pdf Reference, ch. 4.5.5)
                     * If name entry would be replaced with special name /None, all objects
                     * are using this colorspace become invisible.
                     */
                    colorReferences.push_back( (*it).second->GetReference() );
                    PoDoFo::PdfObject* colorArrayObject = pdfDocObjects.
                                                  GetObject( (*it).second->GetReference() );
                    PoDoFo::PdfArray colorArray = colorArrayObject->GetArray();
                    //Processing color array entries
                    if ( colorArray.GetSize() > 1
                         && colorArray[0].IsName()
                         && colorArray[0].GetName().GetEscapedName() == "Separation"
                         && colorArray[1].IsName() )
                    {
                        colorArray[1] = NONE_COLOR;
                        (*colorArrayObject) = PoDoFo::PdfObject (
                                    colorArrayObject->Reference(),
                                    colorArray );
                    }
                }
                ++it;
              } // ColorSpace subdictionary iterator
        } // ColorSpace subdictionary processing
    } // Current Page processing

    pdfDoc.Write(argv[2]);
    return 0;
}
