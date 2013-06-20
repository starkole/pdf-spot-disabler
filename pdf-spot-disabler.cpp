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

int main( int argc, char* argv[] )
{
    /*
     * Check if a filename was passed as commandline argument.
     * If more than 1 argument or no argument is passed,
     * a help message is displayed and the example application
     * will quit.
     */
    if( argc != 2 )
    {
        PrintHelp();
        return -1;
    }

    // Load a document into a PdfMemDocument
    PdfMemDocument pdf(argv[1]);
    // Iterate over each page
    for (int pn = 0; pn < pdf.GetPageCount(); ++pn) {
        PoDoFo::PdfPage* page = pdf.GetPage(pn);
        // Iterate over all the PDF commands on that page:
        PoDoFo::PdfContentsTokenizer tok(page);
        const char* token = NULL;
        PoDoFo::PdfVariant dvar;
        PoDoFo::EPdfContentsType dtype;
        while (tok.ReadNext(dtype, token, dvar)) {
           /* if (dtype == PoDoFo::ePdfContentsType_Keyword)
            {
            // process type, token & var
                //if (token == "l")
                {
                    std::cout << token;//<< "-";
                }
            }//*/
            if (dtype == PoDoFo::ePdfContentsType_Variant)
            {
            // process type, token & var
                if (dvar.IsName())
                {
                    std::cout << " "
                    << dvar.GetDataTypeString();
                }
            } //*/
        }
    } 
/*
The "process type, token & var" is where it gets a little more complex.
You are given raw PDF commands to process. Luckily, if you're not actually
rendering the page and all you want is the text, you can ignore most of them.
The commands you need to process are:
BT, ET, Td, TD, Ts, T, Tm, Tf, ", ', Tj and TJ
The BT and ET commands mark the beginning and end of a text stream,
so you want to ignore anything that's not between a BT/ET pair.
The PDF language is RPN based. A command stream consists of values which
are pushed onto a stack and commands which pop values off the stack and process them.
All the commands you need to process have, at most, one parameter.
That one parameter will be in the var object.
The ", ', Tj and TJ commands are the only ones which actually generate text.
", ' and Tj return a single string. Use var.IsString() and var.GetString() to process it.
TJ returns an array of strings. You can extract each one with:
if (var.isArray()) {
    PoDoFo::PdfArray& a = var.GetArray();
    for (size_t i = 0; i < a.GetSize(); ++i)
        if (a[i].IsString())
            // do something with a[i].GetString()
The other commands are used to determine when to introduce a line break.
" and ' also introduce line breaks. Your best bet is to download the PDF spec
from Adobe and look up the text processing section. It explains what each command does in more detail.
I found it very helpful to write a small program which takes a PDF file
and dumps out the command stream for each page.
    */

    std::cout << std::endl
              << "PDF file " 
              << argv[1]
              << " has "
              << pdf.GetPageCount()
              << " pages."
              << std::endl;
    
    return 0;
}
