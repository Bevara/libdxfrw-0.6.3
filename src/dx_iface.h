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

#ifndef DX_IFACE_H
#define DX_IFACE_H

#include "drw_interface.h"
#include "libdxfrw.h"
#include "dx_data.h"


class dx_iface : public DRW_Interface {
public:
    dx_iface(){dxfW = NULL;}
    ~dx_iface(){}
    bool streamImport(const char* inBuf, int inBufLen);
    bool streamExport();
    bool fileImport(const std::string& fileI, dx_data *fData);
    bool fileExport(const std::string& file, DRW::Version v, bool binary, dx_data *fData);
    void writeEntity(DRW_Entity* e, DRW_Block* bk);
    void writeEntitySVG(DRW_Entity* e, DRW_Block* bk, boundBox *entExtent);
    void calcEntityExtent(DRW_Entity* e, DRW_Block* bk, double* xMin, double* xMax, double* yMin, double* yMax);
    void compareExtents(double newMin, double newMax, double* currMin, double* currMax);

    

//reimplement virtual DRW_Interface functions

//reader part, stores all in class dx_data
    //header
    void addHeader(const DRW_Header* data){
        cData->headerC = *data;
    }

    //tables
    virtual void addBlockRecord(const DRW_Block_Record& data) {
        cData->blockRecords.push_back(data);
    }

    virtual void addLType(const DRW_LType& data){
        cData->lineTypes.push_back(data);
    }
    virtual void addLayer(const DRW_Layer& data){
        cData->layers.push_back(data);
    }
    virtual void addDimStyle(const DRW_Dimstyle& data){
        cData->dimStyles.push_back(data);
    }
    virtual void addVport(const DRW_Vport& data){
        cData->VPorts.push_back(data);
    }
    virtual void addTextStyle(const DRW_Textstyle& data){
        cData->textStyles.push_back(data);
    }
    virtual void addAppId(const DRW_AppId& data){
        cData->appIds.push_back(data);
    }

    //blocks
    virtual void addBlock(const DRW_Block& data){
        dx_ifaceBlock* bk = new dx_ifaceBlock(data);
        currentBlock = bk;
        cData->blocks.push_back(bk);
    }
    virtual void endBlock(){
        currentBlock = cData->mBlock;
    }

    virtual void setBlock(const int /*handle*/){}//unused

    //entities
    virtual void addPoint(const DRW_Point& data){
        currentBlock->ent.push_back(new DRW_Point(data));
    }
    virtual void addLine(const DRW_Line& data){
        currentBlock->ent.push_back(new DRW_Line(data));
    }
    virtual void addRay(const DRW_Ray& data){
        currentBlock->ent.push_back(new DRW_Ray(data));
    }
    virtual void addXline(const DRW_Xline& data){
        currentBlock->ent.push_back(new DRW_Xline(data));
    }
    virtual void addArc(const DRW_Arc& data){
        currentBlock->ent.push_back(new DRW_Arc(data));
    }
    virtual void addCircle(const DRW_Circle& data){
        currentBlock->ent.push_back(new DRW_Circle(data));
    }
    virtual void addEllipse(const DRW_Ellipse& data){
        currentBlock->ent.push_back(new DRW_Ellipse(data));
    }
    virtual void addLWPolyline(const DRW_LWPolyline& data){
        currentBlock->ent.push_back(new DRW_LWPolyline(data));
    }
    virtual void addPolyline(const DRW_Polyline& data){
        currentBlock->ent.push_back(new DRW_Polyline(data));
    }
    virtual void addSpline(const DRW_Spline* data){
        currentBlock->ent.push_back(new DRW_Spline(*data));
    }
    // ¿para que se usa?
    virtual void addKnot(const DRW_Entity& data){}

    virtual void addInsert(const DRW_Insert& data){
        currentBlock->ent.push_back(new DRW_Insert(data));
    }

    // To save on time when writing out, we'll add each block's inserts to a list as we go
    virtual void copyInsert(const DRW_Insert& data) {
        for (std::list<dx_ifaceBlock*>::iterator it = cData->blocks.begin(); it != cData->blocks.end(); ++it) {
            dx_ifaceBlock* bk = *it;
            // find block by matching the block name to the insert name
            if (bk->name.compare(data.name) == 0)
            { // found a matching block add the INSERT to the block's INSERT list
                bk->insList.push_back(data);
            }
        }
    }

    virtual void addTrace(const DRW_Trace& data){
        currentBlock->ent.push_back(new DRW_Trace(data));
    }
    virtual void add3dFace(const DRW_3Dface& data){
        currentBlock->ent.push_back(new DRW_3Dface(data));
    }
    virtual void addSolid(const DRW_Solid& data){
        currentBlock->ent.push_back(new DRW_Solid(data));
    }
    virtual void addMText(const DRW_MText& data){
        currentBlock->ent.push_back(new DRW_MText(data));
    }
    virtual void addText(const DRW_Text& data){
        currentBlock->ent.push_back(new DRW_Text(data));
    }
    virtual void addDimAlign(const DRW_DimAligned *data){
        currentBlock->ent.push_back(new DRW_DimAligned(*data));
    }
    virtual void addDimLinear(const DRW_DimLinear *data){
        currentBlock->ent.push_back(new DRW_DimLinear(*data));
    }
    virtual void addDimRadial(const DRW_DimRadial *data){
        currentBlock->ent.push_back(new DRW_DimRadial(*data));
    }
    virtual void addDimDiametric(const DRW_DimDiametric *data){
        currentBlock->ent.push_back(new DRW_DimDiametric(*data));
    }
    virtual void addDimAngular(const DRW_DimAngular *data){
        currentBlock->ent.push_back(new DRW_DimAngular(*data));
    }
    virtual void addDimAngular3P(const DRW_DimAngular3p *data){
        currentBlock->ent.push_back(new DRW_DimAngular3p(*data));
    }
    virtual void addDimOrdinate(const DRW_DimOrdinate *data){
        currentBlock->ent.push_back(new DRW_DimOrdinate(*data));
    }
    virtual void addLeader(const DRW_Leader *data){
        currentBlock->ent.push_back(new DRW_Leader(*data));
    }
    virtual void addHatch(const DRW_Hatch *data){
        currentBlock->ent.push_back(new DRW_Hatch(*data));
    }
    virtual void addViewport(const DRW_Viewport& data){
        currentBlock->ent.push_back(new DRW_Viewport(data));
    }
    virtual void addImage(const DRW_Image *data){
        dx_ifaceImg *img = new dx_ifaceImg(*data);
        currentBlock->ent.push_back(new dx_ifaceImg(*data));
        cData->images.push_back(img);
    }

    virtual void linkImage(const DRW_ImageDef *data){
        duint32 handle = data->handle;
        std::string path(data->name);
        for (std::list<dx_ifaceImg*>::iterator it=cData->images.begin(); it != cData->images.end(); ++it){
            if ((*it)->ref == handle){
                dx_ifaceImg *img = *it;
                img->path = path;
            }
        }
    }

//writer part, send all in class dx_data to writer
    virtual void addComment(const char* /*comment*/){}

    virtual void writeHeader(DRW_Header& data){
        //complete copy of header vars:
        data = cData->headerC;
        //or copy one by one:
//        for (std::map<std::string,DRW_Variant*>::iterator it=cData->headerC.vars.begin(); it != cData->headerC.vars.end(); ++it)
//            data.vars[it->first] = new DRW_Variant( *(it->second) );
    }

    virtual void writeBlocks(){
        //write each block
        for (std::list<dx_ifaceBlock*>::iterator it=cData->blocks.begin(); it != cData->blocks.end(); ++it){
            dx_ifaceBlock* bk = *it;
            dxfW->writeBlock(bk);
            //and write each entity in block
            for (std::list<DRW_Entity*>::const_iterator it=bk->ent.begin(); it!=bk->ent.end(); ++it)
                writeEntity(*it,bk);
        }
    }

    virtual void calcBlockExtents(double* xMin, double* xMax, double* yMin, double* yMax)
    {
        //step through  each block
        for (std::list<dx_ifaceBlock*>::iterator it = cData->blocks.begin(); it != cData->blocks.end(); ++it) {
            dx_ifaceBlock* bk = *it;
            //and step through  each entity in block
            for (std::list<DRW_Entity*>::const_iterator it = bk->ent.begin(); it != bk->ent.end(); ++it)
                calcEntityExtent(*it, bk, xMin, xMax, yMin, yMax);
        }

    }

    virtual void calcEntityExtents(double* xMin, double* xMax, double* yMin, double* yMax)
    {
        for (std::list<DRW_Entity*>::const_iterator it = cData->mBlock->ent.begin(); it != cData->mBlock->ent.end(); ++it)
            calcEntityExtent(*it, cData->mBlock, xMin, xMax, yMin, yMax);

    }

    virtual int findHeaderInt(UTF8STRING headerVal)
    {
        
        DRW_Header header;
        writeHeader(header);
        return header.headerInt(headerVal);

        //return -1;
    }

    virtual UTF8STRING findHeaderString(UTF8STRING headerEntry)
    {
        DRW_Header header;
        writeHeader(header);
        return header.headerString(headerEntry);
    }

    virtual UTF8STRING findLayerLineType(UTF8STRING layerName)
    {
        for (std::list<DRW_Layer>::iterator it = cData->layers.begin(); it != cData->layers.end(); ++it) {
            if (it->name == layerName)
                return it->lineType;
        }
        return "CONTINUOUS";
    }

    virtual int findLayerLineWidth(UTF8STRING layerName)
    {
        for (std::list<DRW_Layer>::iterator it = cData->layers.begin(); it != cData->layers.end(); ++it) {
            if (it->name == layerName)
                return it->lWeight;
        }

        return -1;
    }


    virtual int findLayerColor(UTF8STRING layerName)
    {
        for (std::list<DRW_Layer>::iterator it = cData->layers.begin(); it != cData->layers.end(); ++it) {
            if (it->name == layerName)
                return it->color;
        }

        return -1;
    }


    virtual bool findLayer(UTF8STRING layerName, DRW_Layer *lr)
    {
        for (std::list<DRW_Layer>::iterator it = cData->layers.begin(); it != cData->layers.end(); ++it) {
      //      *lr = *it;
      //      if (lr->name == layerName)
            if (it->name == layerName)
                return true;
        }

        return false;
    }


    bool updateDrawExtents(boundBox insExtFinal, boundBox entExtent, boundBox*  drawExtent)
    {
        // for each entity, modify its extents by the insertion values and adjust drawextent if needed

        float locXMin = 0, locXMax = 0, locYMin = 0, locYMax = 0;

        locXMin = entExtent.xMin * insExtFinal.xScale + insExtFinal.xMin;
        locXMax = entExtent.xMax * insExtFinal.xScale + insExtFinal.xMax;
        locYMin = entExtent.yMin * insExtFinal.yScale + insExtFinal.yMin;
        locYMax = entExtent.yMax * insExtFinal.yScale + insExtFinal.yMax;

        if (locXMin < locXMax) // could be flipped by scale value 
        {
            if (drawExtent->xMin > locXMin) drawExtent->xMin = locXMin;
            if (drawExtent->xMax < locXMax) drawExtent->xMax = locXMax;
        }
        else
        {
            if (drawExtent->xMin > locXMax) drawExtent->xMin = locXMax;
            if (drawExtent->xMax < locXMin) drawExtent->xMax = locXMin;
        }
        if (locYMin < locYMax) // could be flipped by scale value 
        {
            if (drawExtent->yMin > locYMin) drawExtent->yMin = locYMin;
            if (drawExtent->yMax < locYMax) drawExtent->yMax = locYMax;
        }
        else
        {
            if (drawExtent->yMin > locYMax) drawExtent->yMin = locYMax;
            if (drawExtent->yMax < locYMin) drawExtent->yMax = locYMin;
        }

        return true;
    }

    //check block  values to see whether a transform is needed; 
// return 1 on transform needed
    bool needTransform(DRW_Block* bk)
    {
        // for debuggging 
        //return 0;

        // need to transform if have a bk/entity angle, scale, or extrusion
        if ((bk->insPoint.x != 0) || (bk->insPoint.y != 0) || (bk->insPoint.z != 0))
        {
            return 1; // need to do transform

        }

        if ((bk->insScale.x != 1.0) || (bk->insScale.y != 1.0) || (bk->insScale.z != 1.0))
        {
            return 1;
        }

        if (bk->insAng != 0.0)
        {
            return 1; // need to do transform
        }

        // need to transform if have a bk/entity angle, scale, or extrusion
        if ((bk->insExtPt.x != 0) || (bk->insExtPt.y != 0) || (bk->insExtPt.z != 0))
        {
            return 1; // need to do transform

        }

        return 0;
    }

    // Handle general transforms -- outputting the SVG transform matrix
    // At the moment, the OCS-to-WCS is a re-do of the calculateaxis function.
    // This is the arbitrary axis algorithm from 
    // https://www.autodesk.com/techpubs/autocad/acadr14/dxf/object_coordinate_systems_40ocs41_al_u05_c.htm
    // and also in the  Advanced DXF Issues from the DXF Reference manual
    // we need it because INSERT is specified in OCS, and other entities may not have OCS=WCS
    // This is the full 3D algorithm; for most part we won't need it, since for 2D the extrusion will just flip the axis
    // passing in the insertion point and padding values
    bool  transformEntity(DRW_Block* bk, boundBox* insExtFinal)
    {

        // if transforming, then need to deal with
        // bk->basePoint
        // bk->insPoint 
        // bk->insScale.x = insScale.y = insScale.z = 1.0;
        // bk->insAng = 0.0;
        // insertion info in object space

        //DRW_Coord reScale;
        //reScale.x = 1.0;
        //reScale.y = 1.0;
        //reScale.z = 1.0;

        // OCS to WCS algorithm
        // assume WCS world Wy (0,1,0), and Wz (0,0,1)
        DRW_Coord Wy, Wz, Ax, Ay;
        Wy.x = 0.0;
        Wy.y = 1.0;
        Wy.z = 0.0;
        Wz.x = 0.0;
        Wz.y = 0.0;
        Wz.z = 1.0;

        Ax.x = 1.0;
        Ax.y = 0.0;
        Ax.z = 0.0;
        Ay.x = 0.0;
        Ay.y = 1.0;
        Ay.z = 0.0;

        if (bk->insExtPt.z != 0) // we could have reached here on a (0,0,0) insExtPt, so don't do anything
        {

            if ((fabs(bk->insExtPt.x) < 0.015625 && fabs(bk->insExtPt.y) < 0.015625)) // TODO simplify
            {
                // calc Ax = Wy X N
                Ax.x = Wy.y * bk->insExtPt.z - Wy.z * bk->insExtPt.y;
                Ax.y = Wy.z * bk->insExtPt.x - Wy.x * bk->insExtPt.z;
                Ax.z = Wy.x * bk->insExtPt.y - Wy.y * bk->insExtPt.x;


            }
            else
            {
                //calc Ax = Wz X N
                Ax.x = Wz.y * bk->insExtPt.z - Wz.z * bk->insExtPt.y;
                Ax.y = Wz.z * bk->insExtPt.x - Wz.x * bk->insExtPt.z;
                Ax.z = Wz.x * bk->insExtPt.y - Wz.y * bk->insExtPt.x;
            }

            // scale Ax
            double lenA = sqrt(pow(Ax.x, 2) + pow(Ax.y, 2) + pow(Ax.z, 2));
            Ax.x = Ax.x / lenA;
            Ax.y = Ax.y / lenA;
            Ax.z = Ax.z / lenA;

            // calc Ay=N X Ax
            Ay.x = bk->insExtPt.y * Ax.z - bk->insExtPt.z * Ax.y;
            Ay.y = bk->insExtPt.z * Ax.x - bk->insExtPt.x * Ax.z;
            Ay.z = bk->insExtPt.x * Ax.y - bk->insExtPt.y * Ax.x;

            // scale Ay
            lenA = sqrt(pow(Ay.x, 2) + pow(Ay.y, 2) + pow(Ay.z, 2));
            Ay.x = Ay.x / lenA;
            Ay.y = Ay.y / lenA;
            Ay.z = Ay.z / lenA;
        }

        // now, calculate the rotation, scale and transform in WCS
        double transX = 0, transY = 0, rot = 0, scaleX = 1, scaleY = 1, rotXPt = 0, rotYPt = 0;

        rot = bk->insAng * 360 / M_PIx2; //back into degrees


        // TODO: these cases need to be fully tested separately and potentially simplified
        if ((Ax.x >= 0) && (Ay.y >= 0))
        {
            scaleX = bk->insScale.x;
            scaleY = bk->insScale.y;
            transX = bk->insPoint.x * Ax.x  + bk->insPoint.y * Ay.x;
            transY = bk->insPoint.x * Ax.y + bk->insPoint.y * Ay.y;
            rotXPt = transX;
            rotYPt = transY;

            // compensate for scale given as negative
            transX = transX / scaleX;
            transY = transY / scaleY;
            //if (scaleX < 0)
            //{
            //    transX = -1 * transX;
            //}
            //if (scaleY < 0)
            //{
            ////scaleY = fabs(scaleY);
            //transY = -1 * transY;
            //}
            
        }
        // if were getting our scaling off of the EXT PT, handle differently than scale off the 
        // insScale
        if (Ax.x < 0)
        {
            rot = -1*rot;
            scaleX = bk->insScale.x * Ax.x;
            scaleY = bk->insScale.y * Ay.y;
            transX = bk->insPoint.x * Ax.x + bk->insPoint.y * Ay.x;
            transY = bk->insPoint.x * Ax.y + bk->insPoint.y * Ay.y;
            rotXPt = transX;
            rotYPt = transY;
            if (scaleX < 0)
            {
                transX = -1 * transX;
            }
            if (scaleY < 0)
            {
                //scaleY = fabs(scaleY);
                transY = -1 * transY;
            }
        }
        if (Ay.y < 0) // TODO: this is not tested
        {
            rot = -1 * rot;
            scaleX = bk->insScale.x * Ax.x;
            scaleY = bk->insScale.y * Ay.y;
            transX = bk->insPoint.x * Ax.x + bk->insPoint.y * Ay.x;
            transY = bk->insPoint.x * Ax.y + bk->insPoint.y * Ay.y;
            rotXPt = transX;
            rotYPt = transY;
            if (scaleX < 0)
            {
                transX = -1 * transX;
            }
            if (scaleY < 0)
            {
                //scaleY = fabs(scaleY);
                transY = -1 * transY;
            }

        }
      

      
#ifdef DUMP_TO_STDOUT 
        std::cout << "\t  <g \n"
            << "transform = \"rotate("
            << rot << "," << rotXPt << "," << rotYPt << ") scale(" << scaleX << "," << scaleY << ") "
            << "translate(" << transX << "," << transY << ") "
            << "\">\n";
#endif

        outBufStr << "\t  <g \n"
            << "transform = \"rotate("
            << rot << "," << rotXPt << "," << rotYPt << ") scale(" << scaleX << "," << scaleY << ") "
            << "translate(" << transX << "," << transY << ") "
            << "\">\n";

        insExtFinal->xMin = transX;
        insExtFinal->yMin = transY;
        insExtFinal->xScale = scaleX;
        insExtFinal->yScale = scaleY;

        return 1;  //we've output transform command

    }



    virtual void writeBlocksSVG(boundBox *drawExtent) {
        bool needXfrm = 0;
        DRW_Header header;
        writeHeader(header); // this really just sets the header
        //write each block
        for (std::list<dx_ifaceBlock*>::iterator it = cData->blocks.begin(); it != cData->blocks.end(); ++it) {
            dx_ifaceBlock* bk = *it;
            // for debugging
#ifdef DUMP_TO_STDOUT 
            std::cout << "<!--\n";
            std::cout << bk->name << "\n";
            std::cout << "-->\n";
            if (bk->name == "LAMP") // enable us to stop at a given block
            {
                std::cout << "<!--\n  handling block: ";
                std::cout << bk->name << "\n";
                std::cout << "-->\n";
            }
#endif
            // check if we have one or more INSERT for the block
            //TODO: possibly doublecheck against the BLOCK_RECORD list
            if (bk->insList.size() > 0)
            {
                // going to adjust the size of the entity by it's insertion values
                // when calculating the draw extent; can't pull directly from block
                // values due to special flipping cases
                boundBox insExtFinal; 
                insExtFinal.xMin = 0.0;
                insExtFinal.yMin = 0.0;
                insExtFinal.xMax = 0.0; // not using, since just return translation
                insExtFinal.yMax = 0.0; // not using, since just return translation
                insExtFinal.xScale = 1.0;
                insExtFinal.yScale = 1.0;

                // step through each insert
                for (std::list<DRW_Insert>::iterator it2 = bk->insList.begin(); it2 != bk->insList.end(); ++it2) {
                    //DRW_Insert currIns = it2;
                    // copy current insert data into block
                    bk->blkRecordHandle = it2->handle;
                    bk->insPoint.x = it2->insX;
                    bk->insPoint.y = it2->insY;
                    bk->insPoint.z = it2->insZ;
                    bk->insScale.x = it2->xscale;
                    bk->insScale.y = it2->yscale;
                    bk->insScale.z = it2->zscale;
                    bk->insExtPt.x = it2->extPoint.x;
                    bk->insExtPt.y = it2->extPoint.y;
                    bk->insExtPt.z = it2->extPoint.z;
                    bk->insAng = it2->angle;
                    bk->colSpacing = it2->colspace;
                    bk->rowSpacing = it2->rowspace;
                    bk->colCount = it2->colcount;
                    bk->rowCount = it2->rowcount;
                    bk->color = it2->color;

                    // proceed with current block
                    //check if need transform
                    needXfrm = needTransform((DRW_Block*)bk);
                    if (needXfrm)
                        transformEntity((DRW_Block*)bk, &insExtFinal);

                    //and write each entity in block
                    for (std::list<DRW_Entity*>::const_iterator it = bk->ent.begin(); it != bk->ent.end(); ++it)
                    {
                        boundBox entExtent;
                        entExtent.xMin = 0.0;
                        entExtent.yMin = 0.0;
                        entExtent.xMax = 0.0; 
                        entExtent.yMax = 0.0;
                        writeEntitySVG(*it, bk, &entExtent);
                        updateDrawExtents(insExtFinal, entExtent, drawExtent);
                    }

                    if (needXfrm)
                    {
#ifdef DUMP_TO_STDOUT 
                        std::cout << "</g> \n";
#endif
                        outBufStr << "</g> \n";
                    }
                }
            }
            else // proceed with lone block
            {
                // going to adjust the size of the entity by it's insertion values
                // when calculating the draw extent; can't pull directly from block
                // values due to special flipping cases
                boundBox insExtFinal;
                insExtFinal.xMin = 0.0;
                insExtFinal.yMin = 0.0;
                insExtFinal.xMax = 0.0; // not using, since just return translation
                insExtFinal.yMax = 0.0; // not using, since just return translation
                insExtFinal.xScale = 0.0;
                insExtFinal.yScale = 0.0;

                //check if need transform
                needXfrm = needTransform((DRW_Block*)bk);
                if (needXfrm)
                    transformEntity((DRW_Block*)bk, &insExtFinal);

                //and write each entity in block
                for (std::list<DRW_Entity*>::const_iterator it = bk->ent.begin(); it != bk->ent.end(); ++it)
                {
                    boundBox entExtent;
                    entExtent.xMin = 0.0;
                    entExtent.yMin = 0.0;
                    entExtent.xMax = 0.0;
                    entExtent.yMax = 0.0;
                    writeEntitySVG(*it, bk, &entExtent);
                    updateDrawExtents(insExtFinal, entExtent, drawExtent);
                }

                if (needXfrm)
                {
#ifdef DUMP_TO_STDOUT 
                    std::cout << "</g> \n";
#endif
                    outBufStr << "</g> \n";
                }
                    
            }
        }
    }

    //only send the name, needed by the reader to prepare handles of blocks & blockRecords
    virtual void writeBlockRecords(){
        for (std::list<dx_ifaceBlock*>::iterator it=cData->blocks.begin(); it != cData->blocks.end(); ++it)
            dxfW->writeBlockRecord((*it)->name);
    }
    //write entities of model space and first paper_space
    virtual void writeEntities(){
        for (std::list<DRW_Entity*>::const_iterator it=cData->mBlock->ent.begin(); it!=cData->mBlock->ent.end(); ++it)
            writeEntity(*it, cData->mBlock);
    }

    virtual void writeEntitiesSVG(DRW_Header header, boundBox* drawExtent) {
        for (std::list<DRW_Entity*>::const_iterator it = cData->mBlock->ent.begin(); it != cData->mBlock->ent.end(); ++it)
        {
            boundBox entExtent, insExtFinal;
            entExtent.xMin = 0.0;
            entExtent.yMin = 0.0;
            entExtent.xMax = 0.0;
            entExtent.yMax = 0.0;
            insExtFinal.xMin = 0.0;
            insExtFinal.yMin = 0.0;
            insExtFinal.xMax = 0.0;
            insExtFinal.yMax = 0.0;
            insExtFinal.xScale = 1.0;
            insExtFinal.yScale = 1.0;
            writeEntitySVG(*it, cData->mBlock, &entExtent);
            updateDrawExtents(insExtFinal, entExtent, drawExtent);

        }
    }

    virtual void writeLTypes(){
        for (std::list<DRW_LType>::iterator it=cData->lineTypes.begin(); it != cData->lineTypes.end(); ++it)
            dxfW->writeLineType(&(*it));
    }
    virtual void writeLayers(){
        for (std::list<DRW_Layer>::iterator it=cData->layers.begin(); it != cData->layers.end(); ++it)
            dxfW->writeLayer(&(*it));
    }
    virtual void writeTextstyles(){
        for (std::list<DRW_Textstyle>::iterator it=cData->textStyles.begin(); it != cData->textStyles.end(); ++it)
            dxfW->writeTextstyle(&(*it));
    }
    virtual void writeVports(){
        for (std::list<DRW_Vport>::iterator it=cData->VPorts.begin(); it != cData->VPorts.end(); ++it)
            dxfW->writeVport(&(*it));
    }
    virtual void writeDimstyles(){
        for (std::list<DRW_Dimstyle>::iterator it=cData->dimStyles.begin(); it != cData->dimStyles.end(); ++it)
            dxfW->writeDimstyle(&(*it));
    }
    virtual void writeAppId(){
        for (std::list<DRW_AppId>::iterator it=cData->appIds.begin(); it != cData->appIds.end(); ++it)
            dxfW->writeAppId(&(*it));
    }

    dxfRW* dxfW; //pointer to writer, needed to send data
    dx_data* cData; // class to store or read data
    dx_ifaceBlock* currentBlock;
    

};

#endif // DX_IFACE_H
