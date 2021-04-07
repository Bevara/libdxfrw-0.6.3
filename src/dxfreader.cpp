/******************************************************************************
**  libDXFrw - Library to read/write DXF files (ascii & binary)              **
**                                                                           **
**  Copyright (C) 2011-2015 José F. Soriano, rallazz@gmail.com               **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ios>
#include <string>
#include <sstream>
#include "dxfreader.h"
#include "drw_textcodec.h"
#include "drw_dbg.h"


bool dxfReader::readStream(const char* inBuf, int inBufLen) {

    // all that is happening is that the input stream is moved to inData
    //inData << inBuf;


    // we do not expect NULLS, but just in case, do this the hard way
    int i = 0;

    while (i < inBufLen)
    {
        if (*inBuf != '\0')
        {
            inData << *(inBuf+i);          
        }
        else
        {
            inData << ' ';            
        }
        i++;
    }
    return true;

}

bool dxfReader::readFile(std::ifstream* stream)  {

    //debug -- was using this for speed, but will later pass in the data in memory
    // so not worried at the moment
    /*constexpr auto read_size = std::size_t{ 4096 };*/
    //auto stream = std::ifstream{ path.data() };
    //stream.exceptions(std::ios_base::badbit);
    //auto out = std::string{};
   /* auto buf = std::string(read_size, '\0');
    while (stream->read(&buf[0], read_size)) {
        inData.append(buf, 0, stream->gcount());*/
        //out.append(buf, 0, stream->gcount());
    //}
    //out.append(buf, 0, stream->gcount());
    //inData.append(buf, 0, stream->gcount());

    inData << stream->rdbuf();


    return true;
}

bool dxfReader::peekRec(int* codeData) {
    int code;

    if (!peekCode(&code))
        return false;
    *codeData = code;
    DRW_DBG("peeked code "); DRW_DBG(code); DRW_DBG("\n");
    return true;
}

bool dxfReader::readRec(int *codeData) {
//    std::string text;
    int code;

    if (!readCode(&code))
        return false;
    *codeData = code;

    if (code < 10)
        readString();
    else if (code < 60)
        readDouble();
    else if (code < 80)
        readInt16();
    else if (code > 89 && code < 100) //TODO this is an int 32b
        readInt32();
    else if (code == 100 || code == 102 || code == 105)
        readString();
    else if (code > 109 && code < 150) //skip not used at the v2012
        readDouble();
    else if (code > 159 && code < 170) //skip not used at the v2012
        readInt64();
    else if (code < 180)
        readInt16();
    else if (code > 209 && code < 240) //skip not used at the v2012
        readDouble();
    else if (code > 269 && code < 290) //skip not used at the v2012
        readInt16();
    else if (code < 300) //TODO this is a boolean indicator, int in Binary?
        readBool();
    else if (code < 370)
        readString();
    else if (code < 390)
        readInt16();
    else if (code < 400)
        readString();
    else if (code < 410)
        readInt16();
    else if (code < 420)
        readString();
    else if (code < 430) //TODO this is an int 32b
        readInt32();
    else if (code < 440)
        readString();
    else if (code < 450) //TODO this is an int 32b
        readInt32();
    else if (code < 460) //TODO this is long??
        readInt32();
    else if (code < 470) //TODO this is a floating point double precision??
        readDouble();
    else if (code < 481)
        readString();
    else if (code > 998 && code < 1009) //skip not used at the v2012
        readString();
    else if (code < 1060) //TODO this is a floating point double precision??
        readDouble();
    else if (code < 1071)
        readInt16();
    else if (code == 1071) //TODO this is an int 32b
        readInt32();
    else if (skip)
        //skip safely this dxf entry ( ok for ascii dxf)
        readString();
    else
        //break in binary files because the conduct is unpredictable
        return false;

   // return (filestr->good());
    return inData.good();
}
int dxfReader::getHandleString(){
    int res;
#if defined(__APPLE__)
    int Succeeded = sscanf ( strData.c_str(), "%x", &res );
    if ( !Succeeded || Succeeded == EOF )
        res = 0;
#else
    std::istringstream Convert(strData);
    if ( !(Convert >> std::hex >>res) )
        res = 0;
#endif
    return res;
}

bool dxfReaderBinary::peekCode(int* code) {
    // placeholder

    return (filestr->good());
}

bool dxfReaderBinary::readCode(int *code) {
    unsigned short *int16p;
    char buffer[2];
    filestr->read(buffer,2);
    int16p = (unsigned short *) buffer;
//exist a 32bits int (code 90) with 2 bytes???
    if ((*code == 90) && (*int16p>2000)){
        DRW_DBG(*code); DRW_DBG(" de 16bits\n");
        filestr->seekg(-4, std::ios_base::cur);
        filestr->read(buffer,2);
        int16p = (unsigned short *) buffer;
    }
    *code = *int16p;
    DRW_DBG(*code); DRW_DBG("\n");

    return (filestr->good());
}

bool dxfReaderBinary::readString() {
    type = STRING;
    std::getline(*filestr, strData, '\0');
    DRW_DBG(strData); DRW_DBG("\n");
    return (filestr->good());
}

bool dxfReaderBinary::readString(std::string *text) {
    type = STRING;
    std::getline(*filestr, *text, '\0');
    DRW_DBG(*text); DRW_DBG("\n");
    return (filestr->good());
}

bool dxfReaderBinary::readInt16() {
    type = INT32;
    char buffer[2];
    filestr->read(buffer,2);
    intData = (int)((buffer[1] << 8) | buffer[0]);
    DRW_DBG(intData); DRW_DBG("\n");
    return (filestr->good());
}

bool dxfReaderBinary::readInt32() {
    type = INT32;
    unsigned int *int32p;
    char buffer[4];
    filestr->read(buffer,4);
    int32p = (unsigned int *) buffer;
    intData = *int32p;
    DRW_DBG(intData); DRW_DBG("\n");
    return (filestr->good());
}

bool dxfReaderBinary::readInt64() {
    type = INT64;
    unsigned long long int *int64p; //64 bits integer pointer
    char buffer[8];
    filestr->read(buffer,8);
    int64p = (unsigned long long int *) buffer;
    int64 = *int64p;
    DRW_DBG(int64); DRW_DBG(" int64\n");
    return (filestr->good());
}

bool dxfReaderBinary::readDouble() {
    type = DOUBLE;
    double *result;
    char buffer[8];
    filestr->read(buffer,8);
    result = (double *) buffer;
    doubleData = *result;
    DRW_DBG(doubleData); DRW_DBG("\n");
    return (filestr->good());
}

//saved as int or add a bool member??
bool dxfReaderBinary::readBool() {
    char buffer[1];
    filestr->read(buffer,1);
    intData = (int)(buffer[0]);
    DRW_DBG(intData); DRW_DBG("\n");
    return (filestr->good());
}

bool dxfReaderAscii::peekCode(int* code) {
    std::string text;
    // Get current position
    std::streampos p;
    //p = filestr->tellg();
    //    std::getline(*filestr, text);
    //    *code = atoi(text.c_str());
    //// Return to position before "Read line".
    //filestr->seekg(p);
    //return (filestr->good());
    p = inData.tellg();
    std::getline(inData, text);
    *code = atoi(text.c_str());
    inData.seekg(p);
    return (inData.good());
}

bool dxfReaderAscii::readCode(int *code) {
    std::string text;
    int num;

    std::getline(inData, text);
  
        *code = atoi(text.c_str());
        DRW_DBG("code value ");  DRW_DBG(*code); DRW_DBG("\n");
        return inData.good();

}


bool dxfReaderAscii::readString(std::string *text) {
    type = STRING;
    //std::getline(*filestr, *text);
    std::getline(inData, *text);
    if (!text->empty() && text->at(text->size()-1) == '\r')
        text->erase(text->size()-1);
   // return (filestr->good());
    return true;
}

bool dxfReaderAscii::readString() {
    type = STRING;
    //std::getline(*filestr, strData);
    std::getline(inData, strData);
    if (!strData.empty() && strData.at(strData.size()-1) == '\r')
        strData.erase(strData.size()-1);
    DRW_DBG(strData); DRW_DBG("\n");
    //return (filestr->good());
    return true;
}

bool dxfReaderAscii::readInt16() {
    type = INT32;
    std::string text;
    if (readString(&text)){
        intData = atoi(text.c_str());
        DRW_DBG(intData); DRW_DBG("\n");
        return true;
    } else
        return false;
}

bool dxfReaderAscii::readInt32() {
    type = INT32;
    return readInt16();
}

bool dxfReaderAscii::readInt64() {
    type = INT64;
    return readInt16();
}

bool dxfReaderAscii::readDouble() {
    type = DOUBLE;
    std::string text;
    if (readString(&text)){
#if defined(__APPLE__)
        int succeeded=sscanf( & (text[0]), "%lg", &doubleData);
        if(succeeded != 1) {
            DRW_DBG("dxfReaderAscii::readDouble(): reading double error: ");
            DRW_DBG(text);
            DRW_DBG('\n');
        }
#else
        std::istringstream sd(text);
        sd >> doubleData;
        DRW_DBG(doubleData); DRW_DBG('\n');
#endif
        return true;
    } else
        return false;
}

//saved as int or add a bool member??
bool dxfReaderAscii::readBool() {
    type = BOOL;
    std::string text;
    if (readString(&text)){
        intData = atoi(text.c_str());
        DRW_DBG(intData); DRW_DBG("\n");
        return true;
    } else
        return false;
}


bool dxfReaderStream::peekCode(int* code) {
    std::string text;
    // Get current position
    std::streampos p;
    //p = filestr->tellg();
    //    std::getline(*filestr, text);
    //    *code = atoi(text.c_str());
    //// Return to position before "Read line".
    //filestr->seekg(p);
    //return (filestr->good());
    p = inData.tellg();
    std::getline(inData, text);
    *code = atoi(text.c_str());
    inData.seekg(p);
    return (inData.good());
}

bool dxfReaderStream::readCode(int* code) {
    std::string text;
    int num;

    std::getline(inData, text);

    *code = atoi(text.c_str());
    DRW_DBG("code value ");  DRW_DBG(*code); DRW_DBG("\n");
    return inData.good();

}


bool dxfReaderStream::readString(std::string* text) {
    type = STRING;
    //std::getline(*filestr, *text);
    std::getline(inData, *text);
    if (!text->empty() && text->at(text->size() - 1) == '\r')
        text->erase(text->size() - 1);
    // return (filestr->good());
    return true;
}

bool dxfReaderStream::readString() {
    type = STRING;
    //std::getline(*filestr, strData);
    std::getline(inData, strData);
    if (!strData.empty() && strData.at(strData.size() - 1) == '\r')
        strData.erase(strData.size() - 1);
    DRW_DBG(strData); DRW_DBG("\n");
    //return (filestr->good());
    return true;
}

bool dxfReaderStream::readInt16() {
    type = INT32;
    std::string text;
    if (readString(&text)) {
        intData = atoi(text.c_str());
        DRW_DBG(intData); DRW_DBG("\n");
        return true;
    }
    else
        return false;
}

bool dxfReaderStream::readInt32() {
    type = INT32;
    return readInt16();
}

bool dxfReaderStream::readInt64() {
    type = INT64;
    return readInt16();
}

bool dxfReaderStream::readDouble() {
    type = DOUBLE;
    std::string text;
    if (readString(&text)) {
#if defined(__APPLE__)
        int succeeded = sscanf(&(text[0]), "%lg", &doubleData);
        if (succeeded != 1) {
            DRW_DBG("dxfReaderAscii::readDouble(): reading double error: ");
            DRW_DBG(text);
            DRW_DBG('\n');
        }
#else
        std::istringstream sd(text);
        sd >> doubleData;
        DRW_DBG(doubleData); DRW_DBG('\n');
#endif
        return true;
    }
    else
        return false;
}

//saved as int or add a bool member??
bool dxfReaderStream::readBool() {
    type = BOOL;
    std::string text;
    if (readString(&text)) {
        intData = atoi(text.c_str());
        DRW_DBG(intData); DRW_DBG("\n");
        return true;
    }
    else
        return false;
}

