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
#include <algorithm>

// Include all podofo header files
#include <podofo/podofo.h>
//Include command line options parser
#include "getopt/getopt_pp.h"

const PoDoFo::PdfName NONE_COLOR("None");

void PrintHelpMessage()
{
    std::cout << std::endl;
    std::cout << "Application for disabling spot colors in PDF files."
              << " It hides all objects having specified spot color."
              << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:"
              << std::endl;
    std::cout << "  pdf-spot-disabler in.pdf [out.pdf] [-options]"
              << " [spotName1 spotName2 ... spotNameN]"
              << std::endl;
    std::cout << std::endl;
    std::cout << "Valid options are:"
              << std::endl;
    std::cout << "  -l, --list   lists spots of in.pdf that can be disabled."
              << std::endl;
    std::cout << "  -h, --help   prints this message"
              << std::endl;
    std::cout << std::endl;
    std::cout << "spotName is the name of the spot color being disabled. "; 
    std::cout << "It can be full (eg. \"Pantone 877 C\" or \"Pantone ";
    std::cout << "Process Magenta C\" or \"My Custom Spot\") ";
    std::cout << "or partial (eg. \"877\" or \"Magenta\" ). ";
    std::cout << "spotName is case independent (eg. \"Pantone 877 C\" ";
    std::cout << "equals \"pantone 877 c\" or \"PANTONE 877 C\")." << std::endl;
    std::cout << "If no spot names specified, disables all spots."
              << std::endl;
    std::cout << std::endl;
    std::cout << "Copyright (C) 2013 by Pavlo Oleshkevych"<< std::endl;
    std::cout << "Sources available at https://github.com/starkole/pdf-spot-disabler"
              << std::endl;
    std::cout << std::endl;
}

std::vector<PoDoFo::PdfReference> GetColorReferences( 
                                      const PoDoFo::PdfMemDocument & pdfDocument )
// Returns a vector with references to color decricption arrays in pdf document
{
    //Initilaise references vector being returned bu this function
    std::vector<PoDoFo::PdfReference> colorReferences;
    
    // Iterate over each page of pdf document
    for ( int pn = 0; pn < pdfDocument.GetPageCount(); ++pn ) 
    {
        PoDoFo::PdfPage* page = pdfDocument.GetPage(pn);
        // Get the Resource dictionary of the current page:
        PoDoFo::PdfObject* pageResources = (*page).GetResources();
        // If something wrong with this page - just continue with next
        if ( pageResources == NULL ) continue; 
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
                if ( (*it).second->IsReference() )
                {
                    PoDoFo::PdfReference ref = (*it).second->GetReference();
                    //Check refrence vector for dublicates
                    if ( std::count(colorReferences.begin(),
                                    colorReferences.end(),
                                    ref) == 0 )
                    {
                        // If current reference is unique, add it to the vector
                        colorReferences.push_back( ref );
                    }
                }
                ++it;
            } // ColorSpace subdictionary iterator
        } // ColorSpace subdictionary processing
    } // Current Page processing

    return colorReferences;
}

void ListAvailableSpots( const PoDoFo::PdfMemDocument & pdfDoc,
                         std::vector<PoDoFo::PdfReference> colorReferences )
{
    std::vector<PoDoFo::PdfReference>::iterator it = colorReferences.begin();
    while ( it != colorReferences.end() )
    {
        // Obtaining color array by reference
        if ( pdfDoc.GetObjects().GetObject(*it)->IsArray() )
            {
                /* Color array for separation colorspace has 4 entries: 
                * [ /Separation name alternateSpace tintTransform ]
                */
                PoDoFo::PdfVecObjects pdfDocObjects = pdfDoc.GetObjects();
                PoDoFo::PdfObject* colorArrayObject = pdfDocObjects.GetObject(*it);
                PoDoFo::PdfArray colorArray = colorArrayObject->GetArray();
                //Processing color array entries
                if ( colorArray.GetSize() > 1
                     && colorArray[0].IsName()
                     && colorArray[0].GetName().GetEscapedName() == "Separation"
                     && colorArray[1].IsName() )
                {
                    std::string spotName = colorArray[1].GetName().GetEscapedName();
                    while ( spotName.find("#20") != std::string::npos )
                        spotName.replace ( spotName.find("#20"), 3, " " );
                    std::cout << spotName << std::endl;
                }
            }
        ++it;
    } // Iterating through color references
}

int main( int argc, char* argv[] )
{
    // Initialise command line parser
    GetOpt::GetOpt_pp commandLine (argc, argv);
    // Print help message and exit if needed
    if ( argc < 2
         || commandLine >> GetOpt::OptionPresent('h', "help")
         || commandLine >> GetOpt::OptionPresent('?') )
    {
      PrintHelpMessage();
      return 0;
    }

    std::vector<std::string> programOptions;
    commandLine >> GetOpt::GlobalOption(programOptions);
    std::cout << "Global options: ";
    std::vector<std::string>::iterator i = programOptions.begin();
    while ( i != programOptions.end() )
    {
        std::cout << " " << (*i);
        ++i;
    }
    std::cout << std::endl;

    // Load pdf file
    PoDoFo::PdfMemDocument pdfDoc(argv[1]);
    PoDoFo::PdfVecObjects pdfDocObjects = pdfDoc.GetObjects();
    std::vector<PoDoFo::PdfReference> colorReferences;
    // TODO: Exit with error if loading fails

    colorReferences = GetColorReferences(pdfDoc);

    // List all spots from input file and exit if needed
    if ( commandLine >> GetOpt::OptionPresent('l', "list") )
    {
        ListAvailableSpots( pdfDoc, GetColorReferences(pdfDoc) );
        return 0;
    }


    std::vector<PoDoFo::PdfReference>::iterator it = colorReferences.begin();
    while ( it != colorReferences.end() )
    {
        // Obtaining color array by reference
        if ( pdfDoc.GetObjects().GetObject(*it)->IsArray() )
            {
                /* Color array for separation colorspace has 4 entries: 
                * [ /Separation name alternateSpace tintTransform ]
                * (see Pdf Reference, ch. 4.5.5)
                * If name entry would be replaced with special name /None, all objects
                * are using this colorspace become invisible.
                */
                
                PoDoFo::PdfObject* colorArrayObject = pdfDocObjects.GetObject(*it);
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
    } // Iterating through color references


    pdfDoc.Write(argv[2]);
    return 0;
}
