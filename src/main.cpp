/******************************************************************************
**  dwg2dxf - Program to convert dwg/dxf to dxf(ascii & binary)              **
**                                                                           **
**  Copyright (C) 2015 José F. Soriano, rallazz@gmail.com                    **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "dx_iface.h"
#include "dx_data.h"

// We're going to use globals, rather than re-write too much 
// create a new read/write interface
dx_iface* dxfAccess;

// toggle between reading input stream or reading from file
// TODO: move over to config file
#define STREAM_READ 

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

void usage(){
    std::cout << "Usage: " << std::endl;
    std::cout << "   dwg2dxf <input> [-b] <-version> <output>" << std::endl << std::endl;
    std::cout << "   input      existing file to convert" << std::endl;
    std::cout << "   -b         optional, sets output as binary dxf" << std::endl;
    std::cout << "   -B         optional, batch mode reads a text file whit a list of full path input" << std::endl;
    std::cout << "               files and saves with the same name in the indicated folder as output" << std::endl;
    std::cout << "   -y -Y      optional, Warning! if output dxf exist overwrite without ask" << std::endl;
    std::cout << "   -version   version output of dxf file" << std::endl;
    std::cout << "   output     output file name" << std::endl << std::endl;
    std::cout << "     version can be:" << std::endl;
    std::cout << "        -R12   dxf release 12 version" << std::endl;
    std::cout << "        -v2000 dxf version 2000" << std::endl;
    std::cout << "        -v2004 dxf version 2004" << std::endl;
    std::cout << "        -v2007 dxf version 2007" << std::endl;
    std::cout << "        -v2010 dxf version 2010" << std::endl;
}

DRW::Version checkVersion(std::string param){
    if (param == "-R12")
        return DRW::AC1009;
    else if (param == "-v2000")
        return DRW::AC1015;
    else if (param == "-v2004")
        return DRW::AC1018;
    else if (param == "-v2007")
        return DRW::AC1021;
    else if (param == "-v2010")
        return DRW::AC1024;
    return DRW::UNKNOWNV;
}

bool convertFile(std::string inName, std::string outName, DRW::Version ver, bool binary, bool overwrite){

    

    bool badState = false;
    //verify if input file exist
    std::ifstream ifs;
    ifs.open (inName.c_str(), std::ifstream::in);
    badState = ifs.fail();
    ifs.close();
    if (badState) {
        std::cout << "Error can't open " << inName << std::endl;
        return false;
    }
    //verify if output file exist
    std::ifstream ofs;
    ofs.open (outName.c_str(), std::ifstream::in);
    badState = ofs.fail();
    ofs.close();
    if (!badState) {
        if (!overwrite){
            std::cout << "File " << outName << " already exist, overwrite Y/N ?" << std::endl;
            int c = getchar();
            if (c == 'Y' || c=='y')
                ;
            else {
                std::cout << "Cancelled.";
                return false;
            }
        }
    }
    //All ok proceed with conversion
    //class to store file read:
    dx_data fData;
    //First read a dwg or dxf file
    dx_iface *input = new dx_iface();
    badState = input->fileImport( inName, &fData );
    if (!badState) {
        std::cout << "Error reading file " << inName << std::endl;
        return false;
    }

    //And write a dxf file
    dx_iface *output = new dx_iface();
    badState = output->fileExport(outName, ver, binary, &fData);
    delete input;
    delete output;

    return badState;
}

int dxf_entry(char *inBuf, int inBufLen)
{

    //loads the input stream and triggers processing
    bool success = dxfAccess->streamImport(inBuf, inBufLen);
    if(success != true)//TODO: change to BEVARA error codes
        return false;
    //now convert to SVG
    success = dxfAccess->streamExport();
    return success; //TODO: change to BEVARA error codes

}



// We rely on you to call dxf_entry() first. 
char * getData()
{
    // decide whether want to use the following and ensure external mem handling
   return strdup(dxfAccess->outBufStr.str().c_str());

   // std::string tmp = dxfAccess->outBufStr.str();
   // return tmp.c_str();


}

// We rely on you to call dxf_entry() first but check the getData() call
int getSize()
{
   
    // dodging the copying used by length()
    dxfAccess->outBufStr.seekg(0, dxfAccess->outBufStr.end);
    int length = dxfAccess->outBufStr.tellg();
    dxfAccess->outBufStr.seekg(0, dxfAccess->outBufStr.beg);
    return (length);
}

void dxf_setup()
{
    dxfAccess = new dx_iface();
  
}

void dxf_shutdown()
{

    delete dxfAccess;
   
}

//int main() 
//{
//    bool badState = false;
//    bool binary = false;
//    bool overwrite = false;
//    bool batch = false;
//    
//    DRW::Version ver = DRW::UNKNOWNV;
//
// // toggle between reading a file and an input stream buffer
//#ifndef STREAM_READ  
//
//    // file read/write code
//    // you can use usage() to set the params
//     overwrite = true;
//     std::string fileName = "test.dxf";
//    bool ok = convertFile(fileName, "testdxfout.txt", ver, binary, overwrite);
//    if (ok)
//            return 0;
//    else
//            return 1;
//
//#else
//    // dxf_entry test code
//    FILE* fp;
//    char* dxfData;
//    int  retval,dxfSize;
//
//    fp = fopen("test.dxf", "rb");
//    if (!fp)
//    {
//        printf("Unable to open the file.\n");
//    }
//
//    dxf_setup();
//
//    // read in the data
//    fseek(fp, 0, SEEK_END);
//    dxfSize = (int)ftell(fp);
//    dxfData = (char *) malloc(dxfSize);
//    fseek(fp, 0, SEEK_SET);
//    dxfSize = (int)fread(dxfData, 1, dxfSize, fp);
//    fclose(fp);
//
//    // access the data
//    retval = dxf_entry(dxfData, dxfSize);
//
//    
//    char* locoutbuf;
//    int locoutsize;
//    locoutbuf = getData();
//    locoutsize = getSize();
//
//    // std out print
//    /*int i;
//    printf("outsize = %d", locoutsize);
//    for (i = 0; i < locoutsize; ++i)
//        printf("%c", *(locoutbuf + i));*/
//
//    // file print
//    fp = fopen("test_out.svg", "wb");
//    if (fp != NULL)
//    {
//        dxfSize = fwrite((const void*)locoutbuf, locoutsize, 1, fp);
//        if (dxfSize != locoutsize)
//            printf("error writing output file\n");
//        fclose(fp);
//    }
//    else 
//        printf("error opening output file\n");
//
//    dxf_shutdown();
//
//    free(locoutbuf);
//
//#endif   
// 



}

