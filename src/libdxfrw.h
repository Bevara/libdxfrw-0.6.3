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

#ifndef LIBDXFRW_H
#define LIBDXFRW_H

#include <iostream>
#include <sstream>

#include <string>
#include "drw_entities.h"
#include "drw_objects.h"
#include "drw_header.h"
#include "drw_interface.h"

//struct boundBox {
//    float xMin, xMax, yMin, yMax;
//    float xScale, yScale;
//};
//


// Used for old-school printf debugging
//#define DUMP_TO_STDOUT 


class dxfReader;
class dxfWriter;

class dxfRW {
public:
    dxfRW(const char* name);
    ~dxfRW();
    void setDebug(DRW::DBG_LEVEL lvl);
    /// reads the file specified in constructor
    /*!
     * An interface must be provided. It is used by the class to signal various
     * components being added.
     * @param interface_ the interface to use
     * @param ext should the extrusion be applied to convert in 2D?
     * @return true for success
     */
    bool read(DRW_Interface *interface_, bool ext); // used when reading/processing a file
    bool processStream(DRW_Interface* interface_, bool ext, const char* inBuf, int inBufLen); // used when processing an input stream

    void setBinary(bool b) {binFile = b;}

    // Bevara: added for SVG writing
    bool writeSVG(DRW_Interface* interface_, DRW::Version ver);
   /* bool dxfRW::updateDrawExtents(boundBox insExtFinal, boundBox entExtent, boundBox* drawExtent);
    bool dxfRW::transformEntity(DRW_Block* bk);
    bool dxfRW::needTransform(DRW_Block* bk);
    bool dxfRW::checkMemAndRealloc();
    bool dxfRW::writeToBuffer(std::stringstream& bufString);*/


    bool write(DRW_Interface *interface_, DRW::Version ver, bool bin);
    bool writeLineType(DRW_LType *ent);
    bool writeLayer(DRW_Layer *ent);
    bool writeDimstyle(DRW_Dimstyle *ent);
    bool writeTextstyle(DRW_Textstyle *ent);
    bool writeVport(DRW_Vport *ent);
    bool writeAppId(DRW_AppId *ent);
    bool writePoint(DRW_Point *ent);
    bool writeLine(DRW_Line *ent);
    bool writeRay(DRW_Ray *ent);
    bool writeXline(DRW_Xline *ent);
    bool writeCircle(DRW_Circle *ent);
    bool writeArc(DRW_Arc *ent);
    bool writeEllipse(DRW_Ellipse *ent);
    bool writeTrace(DRW_Trace *ent);
    bool writeSolid(DRW_Solid *ent);
    bool write3dface(DRW_3Dface *ent);
    bool writeLWPolyline(DRW_LWPolyline *ent);
    bool writePolyline(DRW_Polyline *ent);
    bool writeSpline(DRW_Spline *ent);
    bool writeBlockRecord(std::string name);
    bool writeBlock(DRW_Block *ent);
    bool writeInsert(DRW_Insert *ent);
    bool writeMText(DRW_MText *ent);
    bool writeText(DRW_Text *ent);
    bool writeHatch(DRW_Hatch *ent);
    bool writeViewport(DRW_Viewport *ent);
    DRW_ImageDef *writeImage(DRW_Image *ent, std::string name);
    bool writeLeader(DRW_Leader *ent);
    bool writeDimension(DRW_Dimension *ent);
    void setEllipseParts(int parts){elParts = parts;} /*!< set parts munber when convert ellipse to polyline */

    // Bevara: added for SVG writing
    void findColor(int* color, DRW_Block* bk,  UTF8STRING layerName);
    void findLineInfo(double* lineWeightmm, UTF8STRING* lineType, DRW_Block* bk, UTF8STRING layerName, double entThickness);
    bool writePointSVG(DRW_Point* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeLineSVG(DRW_Line* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeRaySVG(DRW_Ray* ent, DRW_Block* bk, boundBox* entExtent);
    //bool writeXlineSVG(DRW_Xline* ent, DRW_Block* bk);
    bool writeCircleSVG(DRW_Circle* ent, DRW_Block* bk, boundBox* entExtent);
    void convert_arc_angle(double* X, double* Y, double cx, double cy, double radius, double angle);
    void transformArc(DRW_Block* bk, DRW_Arc* ent, double* startX, double* startY, double* endX, double* endY, boundBox* entExtent);
    bool writeArcSVG(DRW_Arc* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeEllipseSVG(DRW_Ellipse* ent, DRW_Block* bk, boundBox* entExtent);
    //bool writeTraceSVG(DRW_Trace* ent, DRW_Block* bk,  double offW, double offH);
    bool writeSolidSVG(DRW_Solid* ent, DRW_Block* bk, boundBox* entExtent);
    bool write3dfaceSVG(DRW_3Dface* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeLWPolylineSVG(DRW_LWPolyline* ent, DRW_Block* bk, boundBox* entExtent);
    bool writePolylineSVG(DRW_Polyline* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeSplineSVG(DRW_Spline* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeMTextSVG(DRW_MText* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeTextSVG(DRW_Text* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeHatchSVG(DRW_Hatch* ent, DRW_Block* bk, boundBox* entExtent);
    DRW_ImageDef* writeImageSVG(DRW_Image* ent, std::string name, DRW_Block* bk, boundBox* entExtent);
    bool writeLeaderSVG(DRW_Leader* ent, DRW_Block* bk, boundBox* entExtent);
    bool writeDimensionSVG(DRW_Dimension* ent, DRW_Block* bk, boundBox* entExtent);
    const char* dxfColors[257] = {
    "ffffff",  // color 0 BYBLOCK; default to white -- Bevara changing to blue since display on white background
     "ff0000", // color 1	Red
     "ffff00", // color 2	Yellow
     "00ff00", // color 3	Green
     "00ffff", // color 4   Cyan
     "0000ff", // color 5	Blue
     "ff00ff", // color 6   Magenta
     "ffffff", // color 7	White
     "989898", // color 8	Grey
     "c0c0c0", // color 9
     "ff0000", // color 10
     "ff8080", // color 11
     "a60000", // color 12
     "a65353", // color 13
     "800000", // color 14
     "804040", // color 15
     "4c0000", // color 16
     "4c2626", // color 17
     "260000", // color 18
     "261313", // color 19
     "ff4000", // color 20
     "ff9f80", // color 21
     "a62900", // color 22
     "a66853", // color 23
     "802000", // color 24
     "805040", // color 25
     "4c1300", // color 26
     "4c3026", // color 27
     "260a00", // color 28
     "261813", // color 29
     "ff8000", // color 30
     "ffbf80", // color 31
     "a65300", // color 32
     "a67c53", // color 33
     "804000", // color 34
     "806040", // color 35
     "4c2600", // color 36
     "4c3926", // color 37
     "261300", // color 38
     "261d13", // color 39
     "ffbf00", // color 40
     "ffdf80", // color 41
     "a67c00", // color 42
     "a69153", // color 43
     "806000", // color 44
     "807040", // color 45
     "4c3900", // color 46
     "4c4326", // color 47
     "261d00", // color 48
     "262113", // color 49
     "ffff00", // color 50
     "ffff80", // color 51
     "a6a600", // color 52
     "a6a653", // color 53
     "808000", // color 54
     "808040", // color 55
     "4c4c00", // color 56
     "4c4c26", // color 57
     "262600", // color 58
     "262613", // color 59
     "bfff00", // color 60
     "dfff80", // color 61
     "7ca600", // color 62
     "91a653", // color 63
     "608000", // color 64
     "708040", // color 65
     "394c00", // color 66
     "434c26", // color 67
     "1d2600", // color 68
     "212613", // color 69
     "80ff00", // color 70
     "bfff80", // color 71
     "53a600", // color 72
     "7ca653", // color 73
     "408000", // color 74
     "608040", // color 75
     "264c00", // color 76
     "394c26", // color 77
     "132600", // color 78
     "1d2613", // color 79
     "40ff00", // color 80
     "9fff80", // color 81
     "29a600", // color 82
     "68a653", // color 83
     "208000", // color 84
     "508040", // color 85
     "134c00", // color 86
     "304c26", // color 87
     "0a2600", // color 88
     "182613", // color 89
     "00ff00", // color 90
     "80ff80", // color 91
     "00a600", // color 92
     "53a653", // color 93
     "008000", // color 94
     "408040", // color 95
     "004c00", // color 96
     "264c26", // color 97
     "002600", // color 98
     "132613", // color 99
     "00ff40", // color 100
     "80ff9f", // color 101
     "00a629", // color 102
     "53a668", // color 103
     "008020", // color 104
     "408050", // color 105
     "004c13", // color 106
     "264c30", // color 107
     "00260a", // color 108
     "132618", // color 109
     "00ff80", // color 110
     "80ffbf", // color 111
     "00a653", // color 112
     "53a67c", // color 113
     "008040", // color 114
     "408060", // color 115
     "004c26", // color 116
     "264c39", // color 117
     "002613", // color 118
     "13261d", // color 119
     "00ffbf", // color 120
     "80ffdf", // color 121
     "00a67c", // color 122
     "53a691", // color 123
     "008060", // color 124
     "408070", // color 125
     "004c39", // color 126
     "264c43", // color 127
     "00261d", // color 128
     "132621", // color 129
     "00ffff", // color 130
     "80ffff", // color 131
     "00a6a6", // color 132
     "53a6a6", // color 133
     "008080", // color 134
     "408080", // color 135
     "004c4c", // color 136
     "264c4c", // color 137
     "002626", // color 138
     "132626", // color 139
     "00bfff", // color 140
     "80dfff", // color 141
     "007ca6", // color 142
     "5391a6", // color 143
     "006080", // color 144
     "407080", // color 145
     "00394c", // color 146
     "26434c", // color 147
     "001d26", // color 148
     "132126", // color 149
     "0080ff", // color 150
     "80bfff", // color 151
     "0053a6", // color 152
     "537ca6", // color 153
     "004080", // color 154
     "406080", // color 155
     "00264c", // color 156
     "26394c", // color 157
     "001326", // color 158
     "131d26", // color 159
     "0040ff", // color 160
     "809fff", // color 161
     "0029a6", // color 162
     "5368a6", // color 163
     "002080", // color 164
     "405080", // color 165
     "00134c", // color 166
     "26304c", // color 167
     "000a26", // color 168
     "131826", // color 169
     "0000ff", // color 170
     "8080ff", // color 171
     "0000a6", // color 172
     "5353a6", // color 173
     "000080", // color 174
     "404080", // color 175
     "00004c", // color 176
     "26264c", // color 177
     "000026", // color 178
     "131326", // color 179
     "4000ff", // color 180
     "9f80ff", // color 181
     "2900a6", // color 182
     "6853a6", // color 183
     "200080", // color 184
     "504080", // color 185
     "13004c", // color 186
     "30264c", // color 187
     "0a0026", // color 188
     "181326", // color 189
     "8000ff", // color 190
     "bf80ff", // color 191
     "5300a6", // color 192
     "7c53a6", // color 193
     "400080", // color 194
     "604080", // color 195
     "26004c", // color 196
     "39264c", // color 197
     "130026", // color 198
     "1d1326", // color 199
     "bf00ff", // color 200
     "df80ff", // color 201
     "7c00a6", // color 202
     "9153a6", // color 203
     "600080", // color 204
     "704080", // color 205
     "39004c", // color 206
     "43264c", // color 207
     "1d0026", // color 208
     "211326", // color 209
     "ff00ff", // color 210
     "ff80ff", // color 211
     "a600a6", // color 212
     "a653a6", // color 213
     "800080", // color 214
     "804080", // color 215
     "4c004c", // color 216
     "4c264c", // color 217
     "260026", // color 218
     "261326", // color 219
     "ff00bf", // color 220
     "ff80df", // color 221
     "a6007c", // color 222
     "a65391", // color 223
     "800060", // color 224
     "804070", // color 225
     "4c0039", // color 226
     "4c2643", // color 227
     "26001d", // color 228
     "261321", // color 229
     "ff0080", // color 230
     "ff80bf", // color 231
     "a60053", // color 232
     "a6537c", // color 233
     "800040", // color 234
     "804060", // color 235
     "4c0026", // color 236
     "4c2639", // color 237
     "260013", // color 238
     "26131d", // color 239
     "ff0040", // color 240
     "ff809f", // color 241
     "a60029", // color 242
     "a65368", // color 243
     "800020", // color 244
     "804050", // color 245
     "4c0013", // color 246
     "4c2630", // color 247
     "26000a", // color 248
     "261318", // color 249
     "545454", // color 250   dark gray
     "767676", // color 251
     "989898", // color 252
     "bbbbbb", // color 253
     "dddddd", // color 254
     "000000", // color 255
     "ffffff" // color 256 bylayer
    };

private:
    /// used by read() to parse the content of the file
    bool processDxf();
    bool processHeader();
    bool processTables();
    bool processBlocks();
    bool processBlock();
    bool processEntities(bool isblock);
    bool processObjects();

    bool processBlockRecord();
    bool processLType();
    bool processLayer();
    bool processDimStyle();
    bool processTextStyle();
    bool processVports();
    bool processAppId();

    bool processPoint();
    bool processLine();
    bool processRay();
    bool processXline();
    bool processCircle();
    bool processArc();
    bool processEllipse();
    bool processTrace();
    bool processSolid();
    bool processInsert();
    bool processLWPolyline();
    bool processPolyline();
    bool processVertex(DRW_Polyline* pl);
    bool processText();
    bool processMText();
    bool processHatch();
    bool processSpline();
    bool process3dface();
    bool processViewport();
    bool processImage();
    bool processImageDef();
    bool processDimension();
    bool processLeader();

//    bool writeHeader();
    bool writeEntity(DRW_Entity *ent);
    bool writeTables();
    bool writeBlocks();
    bool writeObjects();
    bool writeExtData(const std::vector<DRW_Variant*> &ed);
    /*use version from dwgutil.h*/
    std::string toHexStr(int n);//RLZ removeme


    // Bevara:: buffer used for SVG output
    std::stringstream outBufStr;


private:
    DRW::Version version;
    std::string fileName;
    std::string codePage;
    bool binFile;
    dxfReader *reader;
    dxfWriter *writer;
    DRW_Interface *iface;
    DRW_Header header;
//    int section;
    std::string nextentity;
    int entCount;
    bool wlayer0;
    bool dimstyleStd;
    bool applyExt;
    bool writingBlock;
    int elParts;  /*!< parts munber when convert ellipse to polyline */
    std::map<std::string,int> blockMap;
    std::vector<DRW_ImageDef*> imageDef;  /*!< imageDef list */

    int currHandle;

};

#endif // LIBDXFRW_H
