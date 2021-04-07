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
#include <algorithm>
#include "dx_iface.h"
#include "libdwgr.h"
#include "libdxfrw.h"


bool dx_iface::streamImport(const char* inBuf, int inBufLen)
    {

    cData = new dx_data;
    currentBlock = cData->mBlock;

     dxfRW* dxf = new dxfRW("placeholder");
    bool success = dxf->processStream(this, false, inBuf, inBufLen);
    delete dxf;
    return success;
    }

bool dx_iface::streamExport()
{
    // DRW::Version v; // placeholder -- may be added
    dxfW = new dxfRW("placeholder");
    bool success = dxfW->writeSVG(this, DRW::UNKNOWNV);
    delete dxfW;
    return success;

}

bool dx_iface::fileImport(const std::string& fileI, dx_data *fData){
    unsigned int found = fileI.find_last_of(".");
    std::string fileExt = fileI.substr(found+1);
    std::transform(fileExt.begin(), fileExt.end(),fileExt.begin(), ::toupper);
    cData = fData;
    currentBlock = cData->mBlock;

    if (fileExt == "DXF"){
        //loads dxf
        dxfRW* dxf = new dxfRW(fileI.c_str());
        bool success = dxf->read(this, false);
        delete dxf;
        return success;
    } else if (fileExt == "DWG"){
        //loads dwg
        dwgR* dwg = new dwgR(fileI.c_str());
        bool success = dwg->read(this, false);
        delete dwg;
        return success;
    }
    std::cout << "file extension can be dxf or dwg" << std::endl;
    return false;
}

bool dx_iface::fileExport(const std::string& file, DRW::Version v, bool binary, dx_data *fData){
    cData = fData;
    dxfW = new dxfRW(file.c_str());
    bool success = dxfW->write(this, v, binary);
    success = dxfW->writeSVG(this, v);
    delete dxfW;
    return success;

}

// keep the two new inputs, just in case of later consolidation of functionality
void dx_iface::compareExtents(double newMin, double newMax, double* currMin, double* currMax)
{

    if (newMin < *currMin) *currMin = newMin;
    if (newMax > *currMax) *currMax = newMax;


}

void dx_iface::calcEntityExtent(DRW_Entity* e, DRW_Block* bk, double* xMin, double* xMax, double* yMin, double* yMax)
{
    int vertexnum;
    double tmpX = 0, tmpY = 0; 
    // for each we find the max and min and compare against the current extents
    // TODO: add in insertion scale into calculations
    switch (e->eType) {
    case DRW::POINT:
        tmpX = static_cast<DRW_Point*>(e)->basePoint.x + bk->basePoint.x + bk->insPoint.x;
        compareExtents(tmpX, tmpX, xMin, xMax);
        tmpY = static_cast<DRW_Point*>(e)->basePoint.y + bk->basePoint.y + bk->insPoint.y;
        compareExtents(tmpY, tmpY, yMin, yMax);
        break;
    case DRW::LINE:
        tmpX = static_cast<DRW_Line*>(e)->basePoint.x + bk->basePoint.x + bk->insPoint.x;
        compareExtents(tmpX, tmpX, xMin, xMax);
        tmpX = static_cast<DRW_Line*>(e)->secPoint.x + bk->basePoint.x + bk->insPoint.x;
        compareExtents(tmpX, tmpX, xMin, xMax);
        tmpY = static_cast<DRW_Line*>(e)->basePoint.y + bk->basePoint.y + bk->insPoint.y;
        compareExtents(tmpY, tmpY, yMin, yMax);
        tmpY = static_cast<DRW_Line*>(e)->secPoint.y + bk->basePoint.y + bk->insPoint.y;
        compareExtents(tmpY, tmpY, yMin, yMax);
        break;
    case DRW::CIRCLE:
        tmpX = static_cast<DRW_Circle*>(e)->basePoint.x + bk->basePoint.x + bk->insPoint.x;
        compareExtents(tmpX, tmpX, xMin, xMax);
        tmpY = static_cast<DRW_Circle*>(e)->basePoint.y + bk->basePoint.y + bk->insPoint.y;
        compareExtents(tmpY, tmpY, yMin, yMax);
        break;
    case DRW::ARC:
        // TODO: consider corner case of arc sweep out of draw space
        tmpX = static_cast<DRW_Circle*>(e)->basePoint.x + bk->basePoint.x + bk->insPoint.x;
        compareExtents(tmpX, tmpX, xMin, xMax);
        tmpY = static_cast<DRW_Circle*>(e)->basePoint.y + bk->basePoint.y + bk->insPoint.y;
        compareExtents(tmpY, tmpY, yMin, yMax);
        break;
    case DRW::SOLID:
        
        break;
    case DRW::ELLIPSE:
        
        break;
    case DRW::LWPOLYLINE:
        DRW_Vertex2D* v;
        vertexnum = static_cast<DRW_LWPolyline*>(e)->vertlist.size();
        for (int i = 0; i < vertexnum; i++) {
            v = static_cast<DRW_LWPolyline*>(e)->vertlist.at(i);
            tmpX = v->x + bk->insPoint.x + bk->basePoint.x;
            compareExtents(tmpX, tmpX, xMin, xMax);
            tmpY = v->y + bk->insPoint.y + bk->basePoint.y;
            compareExtents(tmpY, tmpY, yMin, yMax);
        }
        break;
    case DRW::POLYLINE:
        DRW_Vertex* vPL;
        vertexnum = static_cast<DRW_Polyline*>(e)->vertlist.size();
        for (int i = 0; i < vertexnum; i++) {
            vPL = static_cast<DRW_Polyline*>(e)->vertlist.at(i);
            tmpX = vPL->basePoint.x + bk->insPoint.x + bk->basePoint.x;
            compareExtents(tmpX, tmpX, xMin, xMax);
            tmpY = vPL->basePoint.y + bk->insPoint.y + bk->basePoint.y;
            compareExtents(tmpY, tmpY, yMin, yMax);
        }
        break;
    case DRW::SPLINE:
 
        break;
        //    case RS2::EntitySplinePoints:
        //        writeSplinePoints(static_cast<DRW_Point*>(e));
        //        break;
        //    case RS2::EntityVertex:
        //        break;

    case DRW::MTEXT:
        //static_cast<DRW_MText*>(e)
        break;
    case DRW::TEXT:
        //static_cast<DRW_Text*>(e)
        break;
    case DRW::DIMLINEAR:
    case DRW::DIMALIGNED:
    case DRW::DIMANGULAR:
    case DRW::DIMANGULAR3P:
    case DRW::DIMRADIAL:
    case DRW::DIMDIAMETRIC:
    case DRW::DIMORDINATE:
        //static_cast<DRW_Dimension*>(e)
        break;
    case DRW::LEADER:
        //static_cast<DRW_Leader*>(e)
        break;
    case DRW::HATCH:
        //static_cast<DRW_Hatch*>(e)
        break;
    case DRW::IMAGE:
        //static_cast<DRW_Image*>(e)
        break;
    default:
        break;
    }
}

void dx_iface::writeEntitySVG(DRW_Entity* e, DRW_Block* bk, boundBox* entExtent) {
    switch (e->eType) {
    case DRW::POINT:
        dxfW->writePointSVG(static_cast<DRW_Point*>(e),bk, entExtent);
        break;
    case DRW::LINE:
        dxfW->writeLineSVG(static_cast<DRW_Line*>(e), bk, entExtent);
        break;
    case DRW::CIRCLE:
        dxfW->writeCircleSVG(static_cast<DRW_Circle*>(e), bk, entExtent);
        break;
    case DRW::ARC:
        dxfW->writeArcSVG(static_cast<DRW_Arc*>(e), bk, entExtent);
        break;
    case DRW::SOLID:
        dxfW->writeSolidSVG(static_cast<DRW_Solid*>(e), bk, entExtent);
        break;
    case DRW::ELLIPSE:
        dxfW->writeEllipseSVG(static_cast<DRW_Ellipse*>(e), bk, entExtent);
        break;
    case DRW::LWPOLYLINE:
        dxfW->writeLWPolylineSVG(static_cast<DRW_LWPolyline*>(e), bk, entExtent);
        break;
    case DRW::POLYLINE:
        dxfW->writePolylineSVG(static_cast<DRW_Polyline*>(e), bk, entExtent);
        break;
    case DRW::SPLINE:
        dxfW->writeSplineSVG(static_cast<DRW_Spline*>(e), bk, entExtent);
        break;
        //    case RS2::EntitySplinePoints:
        //        writeSplinePoints(static_cast<DRW_Point*>(e));
        //        break;
        //    case RS2::EntityVertex:
        //        break;
    /*case DRW::INSERT: // INSERT info copied to the block
        dxfW->writeInsert(static_cast<DRW_Insert*>(e),offW,offH);
        break;*/
    case DRW::MTEXT:
        dxfW->writeMTextSVG(static_cast<DRW_MText*>(e), bk, entExtent);
        break;
    case DRW::TEXT:
        dxfW->writeTextSVG(static_cast<DRW_Text*>(e), bk, entExtent);
        break;
    case DRW::DIMLINEAR:
    case DRW::DIMALIGNED:
    case DRW::DIMANGULAR:
    case DRW::DIMANGULAR3P:
    case DRW::DIMRADIAL:
    case DRW::DIMDIAMETRIC:
    case DRW::DIMORDINATE:
        dxfW->writeDimensionSVG(static_cast<DRW_Dimension*>(e), bk, entExtent);
        break;
    case DRW::LEADER:
        dxfW->writeLeaderSVG(static_cast<DRW_Leader*>(e), bk, entExtent);
        break;
    case DRW::HATCH:
        dxfW->writeHatchSVG(static_cast<DRW_Hatch*>(e), bk, entExtent);
        break;
    case DRW::IMAGE:
        dxfW->writeImageSVG(static_cast<DRW_Image*>(e), static_cast<dx_ifaceImg*>(e)->path, bk, entExtent);
        break;
    default:
        break;
    }
}


void dx_iface::writeEntity(DRW_Entity* e, DRW_Block* bk){

    //std::cout << "Writing Entity \n";
    //std::cout << "\t Handle= " << std::hex << e->handle << "  Parent Handle= " << e->parentHandle << "\n";
    //std::cout << "\t Owner Block Handle= " << std::hex << bk->handle << "\n";


    switch (e->eType) {
    case DRW::POINT:
        dxfW->writePoint(static_cast<DRW_Point*>(e));
        break;
    case DRW::LINE:
        //std::cout << "\t Entity Type= LINE \n";
        //std::cout << "\t \t line data (start pt / end pt)" << static_cast<DRW_Line*>(e)->basePoint.x << "  " << static_cast<DRW_Line*>(e)->basePoint.y << "  " << static_cast<DRW_Line*>(e)->basePoint.z << "    ";
        //std::cout << "\t \t \t " << static_cast<DRW_Line*>(e)->secPoint.x << "  " << static_cast<DRW_Line*>(e)->secPoint.y << "  " << static_cast<DRW_Line*>(e)->secPoint.z  << "\n";
        //std::cout << "\t \t line weight" << static_cast<DRW_Line*>(e)->thickness  << "\n";
        dxfW->writeLine(static_cast<DRW_Line*>(e));
        break;
    case DRW::CIRCLE:
        dxfW->writeCircle(static_cast<DRW_Circle*>(e));
        break;
    case DRW::ARC:
        dxfW->writeArc(static_cast<DRW_Arc*>(e));
        break;
    case DRW::SOLID:
        dxfW->writeSolid(static_cast<DRW_Solid*>(e));
        break;
    case DRW::ELLIPSE:
        dxfW->writeEllipse(static_cast<DRW_Ellipse*>(e));
        break;
    case DRW::LWPOLYLINE:
        dxfW->writeLWPolyline(static_cast<DRW_LWPolyline*>(e));
        break;
    case DRW::POLYLINE:
        dxfW->writePolyline(static_cast<DRW_Polyline*>(e));
        break;
    case DRW::SPLINE:
        dxfW->writeSpline(static_cast<DRW_Spline*>(e));
        break;
//    case RS2::EntitySplinePoints:
//        writeSplinePoints(static_cast<DRW_Point*>(e));
//        break;
//    case RS2::EntityVertex:
//        break;
    case DRW::INSERT:
        dxfW->writeInsert(static_cast<DRW_Insert*>(e));
        break;
    case DRW::MTEXT:
        dxfW->writeMText(static_cast<DRW_MText*>(e));
        break;
    case DRW::TEXT:
        dxfW->writeText(static_cast<DRW_Text*>(e));
        break;
    case DRW::DIMLINEAR:
    case DRW::DIMALIGNED:
    case DRW::DIMANGULAR:
    case DRW::DIMANGULAR3P:
    case DRW::DIMRADIAL:
    case DRW::DIMDIAMETRIC:
    case DRW::DIMORDINATE:
        dxfW->writeDimension(static_cast<DRW_Dimension*>(e));
        break;
    case DRW::LEADER:
        dxfW->writeLeader(static_cast<DRW_Leader*>(e));
        break;
    case DRW::HATCH:
        dxfW->writeHatch(static_cast<DRW_Hatch*>(e));
        break;
    case DRW::IMAGE:
        dxfW->writeImage(static_cast<DRW_Image*>(e), static_cast<dx_ifaceImg*>(e)->path);
        break;
    default:
        break;
    }
}
