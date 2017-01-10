#include "Buildings.h"

#define DEGREES_TO_RADIANS        .017453292F

BuildingSize buildingSize (int x, int y, int z) {
  BuildingSize result;

  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}


/******************************************************************
 ********************* BUILDING ***********************************
 ******************************************************************/

Building::Building() {
    m_hasLight = false;
    m_numIndices = 0;
    m_numVertices = 0;
    m_vtxStride = sizeof(BaryVertexFormat2);
    m_hasSign = false;
    m_signTextureId = 0;
}

Building::~Building() {
}

void Building::CreateSimple(Material mat, GLvector3 loc, GLvector3 size, unsigned int tessFactor) {
    m_buildingType = BUILDING_SIMPLE;

    // Set material
    m_material = mat;

    // Randomly add hvac boxes
    if(COIN_FLIP) {
    // Add single block
        AddTexturedBlock(loc, size, tessFactor);
    } else {
        // Add hvac boxes
        AddBoxes(loc, size);
        // Add tallest block
        AddTexturedBlock(loc, glVector(size.x, size.y-1, size.z), tessFactor);
    }

    // Create bounding box
    m_AABB.CreateAABB(loc, size);
}

void Building::CreateBlocky(Material mat, Material lightMat, GLvector3 loc, GLvector3 size, unsigned int tessFactor) {
    m_buildingType = BUILDING_BLOCKY;
    m_material = mat;

    GLvector3 thisBlockSize;

    // Some buildings will have boxes on roof (hvac, etc)
    unsigned int xsize = RandomVal(2)+3;
    unsigned int zsize = RandomVal(2)+3;
    if(COIN_FLIP) {
        // Antennas?
        //if(COIN_FLIP) {
        if(size.y > 30) {
            // Add Antenna
            AddAntenna(lightMat, loc, glVector(0.1, size.y, 0.1));
            // Add tallest block
            thisBlockSize = glVector(xsize, ceil(size.y-2), zsize);
            AddTexturedBlock(loc, thisBlockSize, tessFactor);
        }
    } else {
        // Add hvac boxes
        AddBoxes(loc, glVector(xsize, size.y, zsize));
        // Add tallest block
        thisBlockSize = glVector(xsize, ceil(size.y-1), zsize);
        AddTexturedBlock(loc, thisBlockSize, tessFactor);
    }

    // Add X block
    thisBlockSize = glVector(10, (unsigned int) size.y * ((RandomVal(20)+51)/100.0), RandomVal(5)+5);
    AddTexturedBlock(loc, thisBlockSize, tessFactor);

    // Add Z block
    thisBlockSize = glVector(RandomVal(5)+5, (unsigned int) size.y * ((RandomVal(30)+20)/100.0), 10);
    AddTexturedBlock(loc, thisBlockSize, tessFactor);

    // Create bounding box
    m_AABB.CreateAABB(loc, size);

}

void Building::CreateTower(Material mat, GLvector3 loc, GLvector3 size, unsigned int tessFactor) {
    m_buildingType = BUILDING_TOWER;
    m_material = mat;

    GLvector3 thisBlockSize = size;
    float thisHeight;

    // Create sign texture
    m_hasSign = true;

    // Assume 20x20 grid
    // Random number of blocks (3-6)
    unsigned int numTiers = RandomVal(2) + 3;
    unsigned int maxHeight = size.y * 2;

    // Ensure suburb buildings do not have too many tiers
    if(size.y < 10.0)
        numTiers = 2;

    // Create blocks
    for(unsigned int i = 1; i <= numTiers; i++) {
        // Get narrower for every new tier (x,z), and y is half of remaining height
        thisHeight = maxHeight - (maxHeight * (1/(2.0*i)));
        // Divide height by 2 as block create talkes half block size
        AddTexturedBlock(loc, glVector(thisBlockSize.x, ceil((thisHeight/2)), thisBlockSize.z), tessFactor);
        // Divide by 2 as block create talkes half block size
        thisBlockSize = glVector((unsigned int) (thisBlockSize.x - 1),
                         (unsigned int)(thisHeight / 2),
                         (unsigned int)(thisBlockSize.z - 1));
    }

    // Ensure suburb buildings do not have too many tiers
    if(size.y < 30.0 && numTiers == 5)
        numTiers = 4;


    AddSign(loc, glVector(thisBlockSize.x +1, ceil(thisHeight/2), thisBlockSize.z +1));

    // Add spike to get to maxHeight
    AddSpike(glVector(loc.x, (unsigned int)thisHeight, loc.z),
             glVector(thisBlockSize.x, maxHeight, thisBlockSize.z));

    // Create bounding box
    m_AABB.CreateAABB(loc, size);
}

void Building::CreateModern(Material mat, GLvector3 loc, GLvector3 size, unsigned int tessFactor) {
    m_buildingType = BUILDING_MODERN;
    m_material = mat;
    std::vector<BaryVertexFormat2> vertices;
    BaryVertexFormat2 thisSection[4];
    int numVertices = 0;
    std::vector<GLuint>       indices;
    int numIndices = 0;

     int         windows;
     float       length;
     int         skip_interval;
     int         skip_counter;
     int         skip_delta;
     //int         i;
     bool        logo_done;

     logo_done = false;
     //How many 10-degree segments to build before the next skip.
     skip_interval = 1 + RandomVal (8);
     //When a skip happens, how many degrees should be skipped
     skip_delta = (1 + RandomVal (2)) * 30; //30 60 or 90

     GLvector center = glVector (loc.x, 0.0f, loc.z);
     float radius = 12.0f;
     windows = 0;
     skip_counter = 0;

     // Create the buliding
     for (int angle = 0; angle <= 350; angle+=0) {
         // Calculate first point
         thisSection[0].x = center.x - sinf ((float)angle * DEGREES_TO_RADIANS) * radius;
         thisSection[0].y = 0.0f;
         thisSection[0].z = center.z + cosf ((float)angle * DEGREES_TO_RADIANS) * radius;
         thisSection[0].u = (float)windows / 64.0f;
         thisSection[0].v = (size.y*2.0) / 64.0f;
         thisSection[0].bx = 1;
         thisSection[0].by = 0;
         thisSection[0].bz = 0;
         // Calculate fourth point
         thisSection[3].x = center.x - sinf ((float)angle * DEGREES_TO_RADIANS) * radius;
         thisSection[3].y = size.y * 2;
         thisSection[3].z = center.z + cosf ((float)angle * DEGREES_TO_RADIANS) * radius;
         thisSection[3].u = (float)windows / 64.0f;
         thisSection[3].v = 0.0f;
         thisSection[3].bx = 0;
         thisSection[3].by = 1;
         thisSection[3].bz = 0;

         // Do we skip some segments?
         if (skip_counter >= skip_interval && (angle + skip_delta < 360)) {
             angle += skip_delta;
             skip_counter = 0;
         } else {   // Just increment by 10
             angle += 10;
         }

         // Caulcate second point
         thisSection[1].x = center.x - sinf ((float)(angle) * DEGREES_TO_RADIANS) * radius;
         thisSection[1].y = 0.0f;
         thisSection[1].z = center.z + cosf ((float)(angle) * DEGREES_TO_RADIANS) * radius;
         thisSection[1].bx = 0;
         thisSection[1].by = 1;
         thisSection[1].bz = 0;
         // Caulcate third point
         thisSection[2].x = center.x - sinf ((float)(angle) * DEGREES_TO_RADIANS) * radius;
         thisSection[2].y = size.y * 2;
         thisSection[2].z = center.z + cosf ((float)(angle) * DEGREES_TO_RADIANS) * radius;
         thisSection[2].bx = 0;
         thisSection[2].by = 0;
         thisSection[2].bz = 1;

         //  How many windows added in this segment
         if (angle > 0 && skip_counter == 0) {
             length = sqrt(pow(thisSection[0].x - thisSection[1].x, 2.0f)  + pow(thisSection[0].z - thisSection[1].z, 2.0f));
             windows += (int)length;
             if (length > 10 && !logo_done) {
                 logo_done = true;
                 //start = glVector (thisSection[0].x, thisSection[0].z);
                 //end = glVector (thisSection[1].x, thisSection[1].z);
                 //d->CreateLogo (start, end, (float)_height, WorldLogoIndex (), RANDOM_COLOR);
             }
         } else if (skip_counter != 1)
             windows++;

         // Calculate tex coords
         thisSection[1].u = (float)windows / 64.0f;
         thisSection[1].v = (size.y*2.0) / 64.0f;
         thisSection[2].u = (float)windows / 64.0f;
         thisSection[2].v = 0.0f;

         // Add vertices to vertex buffer
         for(int i = 0; i <  4; i++) {
             vertices.push_back(thisSection[i]);
         }

         // Add indices to index buffer
         indices.push_back(0 + numVertices);
         indices.push_back(1 + numVertices);
         indices.push_back(2 + numVertices);
         indices.push_back(0 + numVertices);
         indices.push_back(2 + numVertices);
         indices.push_back(3 + numVertices);

         numVertices += 4;
         numIndices += 6;

         // Increment skip_counter
         skip_counter++;
     }

     // Add roof
     UCvector bary = {1, 0, 0};
     for(int i=0; i < numVertices; i+=4) {
         vertices.push_back(BaryVertexFormat2(vertices[i].x, size.y*2.0, vertices[i].z, 0.0f, 0.0f, bary.x, bary.y, bary.z));    // black texture
         if(bary.y == 1) {
             bary = ucVector(0, 0, 1);
         } else if(bary.z == 1) {
             bary = ucVector(0, 1, 0);
         } else if(bary.x == 1) {
             bary = ucVector(0, 1, 0);
         }
     }
     for(int i=0; i < (numVertices/4)-2; i++) {
         indices.push_back(0 + numVertices);
         indices.push_back(1 + i + numVertices);
         indices.push_back(2 + i + numVertices);
     }
     numIndices += ((numVertices/4)-2) * 3;
     numVertices += numVertices/4;

     // Add block vertices to building
     for(int i = 0; i <  numVertices; i++) {
         m_vertex.push_back(vertices[i]);
     }

     // Add block indices to building
     for(int i = 0; i <  numIndices; i++) {
         m_index.push_back(indices[i] + m_numVertices);
     }

     // Update # of verts & indices
     m_numVertices += numVertices;
     m_numIndices += numIndices;

    // Create bounding box
    m_AABB.CreateAABB(loc, size);
}

void Building::AddTexturedBlock(GLvector3 location, GLvector3 size, unsigned int tessFactor) {
    // location is (midX, botY, midZ) - i.e. middle of X/Z coord is middle of building, Y always 0.0 (on ground)
    // Tex u/v need to be calculated based on size of building to support window textures
    // 1 unit in size = 1 window. 64 windows to a texture. So each unit in size will be a texCoord of 1/64
    GLfloat texUVPerUnit = 1/64.0;
    float xSize = size.x * 2.0;
    float ySize = size.y * 2.0;
    float zSize = size.z * 2.0;

    float xIncr = xSize / tessFactor;
    float yIncr = ySize / tessFactor;
    float zIncr = zSize / tessFactor;

    float xStart = location.x - size.x;
    float yStart = location.y;
    float zStart = location.z - size.z;

    float x, y, z, u, v;
    float uStart = 0.0f;
    float vStart = 0.0f;
    float uIncr, vIncr;
    unsigned int curVIndex = 0;
    unsigned int curIIndex = 0;

    // Vertex Buffer (Interleaved)
    //BaryVertexFormat2 vertex[24 * tessFactor * tessFactor];
	//BaryVertexFormat2* vertex = new BaryVertexFormat2[24 * tessFactor * tessFactor];
	std::vector<BaryVertexFormat2> vertex;
    // Index Buffer
    //GLushort index[36 * tessFactor * tessFactor];
	//GLushort* index = new GLushort[36 * tessFactor * tessFactor];
	std::vector<GLushort> index;

    // *** Remember: tex u=0, v=0 => upper left of texture ***

    // Setup vertex and index VBOs
    // Need seperate vertices per face (for individual texture coordinates)
    // Front Face
    y = yStart;
    z = zStart;
    uStart = 0.0f;
    v = ySize/64.0f;
    uIncr = (float)  (xSize/64.0f) / tessFactor;
    vIncr = (float) -(ySize/64.0f) / tessFactor;
    for(unsigned int i = 0; i < tessFactor; i++) {
        x = xStart;
        u = uStart;
        for(unsigned int j = 0; j < tessFactor; j++) {
            //Vertices
            //vertex[curVIndex]     = BaryVertexFormat2(x,         y,         z, u,         v,              1, 0, 0);
            //vertex[curVIndex + 1] = BaryVertexFormat2(x + xIncr, y,         z, u + uIncr, v,              0, 1, 0);
            //vertex[curVIndex + 2] = BaryVertexFormat2(x + xIncr, y + yIncr, z, u + uIncr, v + vIncr,      0, 0, 1);
            //vertex[curVIndex + 3] = BaryVertexFormat2(x,         y + yIncr, z, u,         v + vIncr,      0, 1, 0);
			vertex.push_back(BaryVertexFormat2(x,         y,         z, u,         v,              1, 0, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y,         z, u + uIncr, v,              0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y + yIncr, z, u + uIncr, v + vIncr,      0, 0, 1));
            vertex.push_back(BaryVertexFormat2(x,         y + yIncr, z, u,         v + vIncr,      0, 1, 0));
            x += xIncr;
            u += uIncr;
            // Indices
            //index[curIIndex]   = curVIndex + 0;
            //index[curIIndex+1] = curVIndex + 1;
            //index[curIIndex+2] = curVIndex + 2;
            //index[curIIndex+3] = curVIndex + 0;
            //index[curIIndex+4] = curVIndex + 2;
            //index[curIIndex+5] = curVIndex + 3;
			index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 2);
            index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 2);
            index.push_back(curVIndex + 3);
            curVIndex += 4;
            curIIndex += 6;
        }
        y += yIncr;
        v += vIncr;
    }

    // Top Face
    // We know texel 0,0 will always be black, so just shade top all black (old u,v in comments)
    y = yStart + ySize;
    z = zStart;
    uIncr = (float)  (xSize/64.0f) / tessFactor;
    vIncr = (float) -(ySize/64.0f) / tessFactor;
    for(unsigned int i = 0; i < tessFactor; i++) {
        x = xStart;
        for(unsigned int j = 0; j < tessFactor; j++) {
            //Vertices
            //vertex[curVIndex]     = BaryVertexFormat2(x,         y, z,          0.0f, 0.0f,      1, 0, 0);
            //vertex[curVIndex + 1] = BaryVertexFormat2(x + xIncr, y, z,          0.0f, 0.0f,      0, 1, 0);
            //vertex[curVIndex + 2] = BaryVertexFormat2(x + xIncr, y, z + zIncr,  0.0f, 0.0f,      0, 0, 1);
            //vertex[curVIndex + 3] = BaryVertexFormat2(x,         y, z + zIncr,  0.0f, 0.0f,      0, 1, 0);
			vertex.push_back(BaryVertexFormat2(x,         y, z,          0.0f, 0.0f,      1, 0, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y, z,          0.0f, 0.0f,      0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y, z + zIncr,  0.0f, 0.0f,      0, 0, 1));
            vertex.push_back(BaryVertexFormat2(x,         y, z + zIncr,  0.0f, 0.0f,      0, 1, 0));
            x += xIncr;
            u += uIncr;
            // Indices
            //index[curIIndex]   = curVIndex + 0;
            //index[curIIndex+1] = curVIndex + 1;
            //index[curIIndex+2] = curVIndex + 2;
            //index[curIIndex+3] = curVIndex + 0;
            //index[curIIndex+4] = curVIndex + 2;
            //index[curIIndex+5] = curVIndex + 3;
			index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 2);
            index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 2);
            index.push_back(curVIndex + 3);
            curVIndex += 4;
            curIIndex += 6;
        }
        z += zIncr;
    }

    // Back Face
    y = yStart;
    z = zStart + zSize;
    v = ySize/64.0f;
    uStart = (zSize-1)/64.0f;
    uIncr = (float)  ((zSize-1+xSize)/64.0f) / tessFactor;
    vIncr = (float) -(ySize/64.0f) / tessFactor;
    for(unsigned int i = 0; i < tessFactor; i++) {
        x = xStart;
        u = uStart;
        for(unsigned int j = 0; j < tessFactor; j++) {
            //Vertices
            //vertex[curVIndex]     = BaryVertexFormat2(x,         y,         z, u,         v,             0, 1, 0);
            //vertex[curVIndex + 1] = BaryVertexFormat2(x + xIncr, y,         z, u + uIncr, v,             1, 0, 0);
            //vertex[curVIndex + 2] = BaryVertexFormat2(x + xIncr, y + yIncr, z, u + uIncr, v + vIncr,     0, 1, 0);
            //vertex[curVIndex + 3] = BaryVertexFormat2(x,         y + yIncr, z, u,         v + vIncr,     0, 0, 1);
			vertex.push_back(BaryVertexFormat2(x,         y,         z, u,         v,             0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y,         z, u + uIncr, v,             1, 0, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y + yIncr, z, u + uIncr, v + vIncr,     0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x,         y + yIncr, z, u,         v + vIncr,     0, 0, 1));
            x += xIncr;
            u += uIncr;
            // Indices
            //index[curIIndex]   = curVIndex + 1;
            //index[curIIndex+1] = curVIndex + 0;
            //index[curIIndex+2] = curVIndex + 3;
            //index[curIIndex+3] = curVIndex + 1;
            //index[curIIndex+4] = curVIndex + 3;
            //index[curIIndex+5] = curVIndex + 2;
			index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 3);
            index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 3);
            index.push_back(curVIndex + 2);
            curVIndex += 4;
            curIIndex += 6;
        }
        y += yIncr;
        v += vIncr;
    }
    // Bottom Face
    // We know texel 0,0 will always be black, so just shade bottom all black (old u,v in comments)
    y = yStart;
    z = zStart;
    uIncr = (float)  (xSize/64.0f) / tessFactor;
    vIncr = (float) -(ySize/64.0f) / tessFactor;
    for(unsigned int i = 0; i < tessFactor; i++) {
        x = xStart;
        for(unsigned int j = 0; j < tessFactor; j++) {
            //Vertices
            //vertex[curVIndex]     = BaryVertexFormat2(x,         y, z,          0.0f, 0.0f,      0, 1, 0);
            //vertex[curVIndex + 1] = BaryVertexFormat2(x + xIncr, y, z,          0.0f, 0.0f,      1, 0, 0);
            //vertex[curVIndex + 2] = BaryVertexFormat2(x + xIncr, y, z + zIncr,  0.0f, 0.0f,      0, 1, 0);
            //vertex[curVIndex + 3] = BaryVertexFormat2(x,         y, z + zIncr,  0.0f, 0.0f,      0, 0, 1);
			vertex.push_back(BaryVertexFormat2(x,         y, z,          0.0f, 0.0f,      0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y, z,          0.0f, 0.0f,      1, 0, 0));
            vertex.push_back(BaryVertexFormat2(x + xIncr, y, z + zIncr,  0.0f, 0.0f,      0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x,         y, z + zIncr,  0.0f, 0.0f,      0, 0, 1));
            x += xIncr;
            u += uIncr;
            // Indices
            //index[curIIndex]   = curVIndex + 1;
            //index[curIIndex+1] = curVIndex + 0;
            //index[curIIndex+2] = curVIndex + 3;
            //index[curIIndex+3] = curVIndex + 1;
            //index[curIIndex+4] = curVIndex + 3;
            //index[curIIndex+5] = curVIndex + 2;
			index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 3);
            index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 3);
            index.push_back(curVIndex + 2);
            curVIndex += 4;
            curIIndex += 6;
        }
        z += zIncr;
    }

    // Left Face
    // u,v coords are set to 'continue' on from front face & wrap (old u,v in comments)
    // the -1 allows windows to stay lit around corners
    y = yStart;
    x = xStart;
    v = ySize/64.0f;
    uStart = 0.0f;
    uIncr = (float)  (zSize/64.0f) / tessFactor;
    vIncr = (float) -(ySize/64.0f) / tessFactor;
    for(unsigned int i = 0; i < tessFactor; i++) {
        z = zStart;
        u = uStart;
        for(unsigned int j = 0; j < tessFactor; j++) {
            //Vertices
            //vertex[curVIndex]     = BaryVertexFormat2(x, y,          z,          u,         v,           1, 0, 0);
            //vertex[curVIndex + 1] = BaryVertexFormat2(x, y + yIncr,  z,          u,         v + vIncr,   0, 1, 0);
            //vertex[curVIndex + 2] = BaryVertexFormat2(x, y + yIncr,  z + zIncr,  u + uIncr, v + vIncr,   0, 0, 1);
            //vertex[curVIndex + 3] = BaryVertexFormat2(x, y,          z + zIncr,  u + uIncr, v,           0, 1, 0);
			vertex.push_back(BaryVertexFormat2(x, y,          z,          u,         v,           1, 0, 0));
            vertex.push_back(BaryVertexFormat2(x, y + yIncr,  z,          u,         v + vIncr,   0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x, y + yIncr,  z + zIncr,  u + uIncr, v + vIncr,   0, 0, 1));
            vertex.push_back(BaryVertexFormat2(x, y,          z + zIncr,  u + uIncr, v,           0, 1, 0));
            z += zIncr;
            u += uIncr;
            // Indices
           // index[curIIndex]   = curVIndex + 0;
            //index[curIIndex+1] = curVIndex + 1;
            //index[curIIndex+2] = curVIndex + 2;
            //index[curIIndex+3] = curVIndex + 0;
            //index[curIIndex+4] = curVIndex + 2;
            //index[curIIndex+5] = curVIndex + 3;
			index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 2);
            index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 2);
            index.push_back(curVIndex + 3);
            curVIndex += 4;
            curIIndex += 6;
        }
        y += yIncr;
        v += vIncr;
    }

    // Right Face
    // u,v coords are set to 'continue' on from front face & wrap (old u,v in comments)
    // the -1 allows windows to stay lit around corners
    y = yStart;
    x = xStart + xSize;
    v = ySize/64.0f;
    uStart = (xSize-1)/64.0f;
    uIncr = (float)  ((xSize-1+zSize)/64.0f) / tessFactor;
    vIncr = (float) -(ySize/64.0f) / tessFactor;
    for(unsigned int i = 0; i < tessFactor; i++) {
        z = zStart;
        u = uStart;
        for(unsigned int j = 0; j < tessFactor; j++) {
            //Vertices
            //vertex[curVIndex]     = BaryVertexFormat2(x, y,          z,          u,         v,           0, 1, 0);
            //vertex[curVIndex + 1] = BaryVertexFormat2(x, y + yIncr,  z,          u,         v + vIncr,   1, 0, 0);
            //vertex[curVIndex + 2] = BaryVertexFormat2(x, y + yIncr,  z + zIncr,  u + uIncr, v + vIncr,   0, 1, 0);
            //vertex[curVIndex + 3] = BaryVertexFormat2(x, y,          z + zIncr,  u + uIncr, v,           0, 0, 1);
			vertex.push_back(BaryVertexFormat2(x, y,          z,          u,         v,           0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x, y + yIncr,  z,          u,         v + vIncr,   1, 0, 0));
            vertex.push_back(BaryVertexFormat2(x, y + yIncr,  z + zIncr,  u + uIncr, v + vIncr,   0, 1, 0));
            vertex.push_back(BaryVertexFormat2(x, y,          z + zIncr,  u + uIncr, v,           0, 0, 1));
            z += zIncr;
            u += uIncr;
            // Indices
            //index[curIIndex]   = curVIndex + 1;
            //index[curIIndex+1] = curVIndex + 0;
            //index[curIIndex+2] = curVIndex + 3;
            //index[curIIndex+3] = curVIndex + 1;
            //index[curIIndex+4] = curVIndex + 3;
            //index[curIIndex+5] = curVIndex + 2;
			index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 0);
            index.push_back(curVIndex + 3);
            index.push_back(curVIndex + 1);
            index.push_back(curVIndex + 3);
            index.push_back(curVIndex + 2);
            curVIndex += 4;
            curIIndex += 6;
        }
        y += yIncr;
        v += vIncr;
    }

    //GLuint numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
	unsigned int numVertices = vertex.size();
    //GLuint numIndices = sizeof(index) / sizeof(GLushort);
	unsigned int numIndices = index.size();

    // Add block vertices to building
    for(unsigned int i = 0; i <  numVertices; i++) {
        m_vertex.push_back(vertex[i]);
    }

    // Add block indices to building
    for(unsigned int i = 0; i <  numIndices; i++) {
        m_index.push_back(index[i] + m_numVertices);
    }

    // Update # of verts & indices
    m_numVertices += numVertices;
    m_numIndices += numIndices;

	//delete [] vertex;
	//delete [] index;
}

void Building::AddUnTexturedBlock(GLvector3 location, GLvector3 size) {
    // location is (midX, botY, midZ) - i.e. middle of X/Z coord is middle of building, Y always 0.0 (on ground)

    // Tex u/v need to be calculated based on size of building to support window textures
    // 1 unit in size = 1 window. 64 windows to a texture. So each unit in size will be a texCoord of 1/64
    GLfloat texUVPerUnit = 1/64.0;
    int xSize = size.x * 2;
    int ySize = size.y * 2;
    int zSize = size.z * 2;

    // Vertex Buffer (Interleaved)
    BaryVertexFormat2 vertex[24];
    // Index Buffer
    GLushort index[36];

    // *** Remember: tex u=0, v=0 => upper left of texture ***
    // Setup vertex and index VBOs
    // Need seperate vertices per face (for individual texture coordinates)
    // Front Face
    vertex[0] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2 -2, location.z - size.z, 0.0f,  0.0f,     1, 0, 0);             // 0,0
    vertex[1] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2 -2, location.z - size.z, 0.0f,  0.0f,     0, 1, 0);      // 0,1
    vertex[2] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2, location.z - size.z, 0.0f,  0.0f,        0, 0, 1);   // 1,1
    vertex[3] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2, location.z - size.z, 0.0f,  0.0f,        0, 1, 0);             // 1,0
    // Top Face
    // We know texel 0,0 will always be black, so just shade top all black (old u,v in comments)
    vertex[4] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2, location.z - size.z, 0.0f,  0.0f,        1, 0, 0); // 0,0
    vertex[5] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2, location.z - size.z, 0.0f,  0.0f,        0, 1, 0); // 0,1
    vertex[6] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2, location.z + size.z, 0.0f,  0.0f,        0, 0, 1); // 1,1
    vertex[7] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2, location.z + size.z, 0.0f,  0.0f,        0, 1, 0); // 1,1
    // Back Face
    //***** center y coords should have 2 '+'s
    //****** Need to then update uv coords
    vertex[8] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2 -2, location.z + size.z, 0.0f,  0.0f,     0, 1, 0); // 0,0
    vertex[9] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2 -2, location.z + size.z, 0.0f,  0.0f,     1, 0, 0); // 0,1
    vertex[10] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2, location.z + size.z, 0.0f,  0.0f,       0, 1, 0); // 1,1
    vertex[11] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2, location.z + size.z, 0.0f,  0.0f,       0, 0, 1); // 1,0
    // Bottom Face
    // We know texel 0,0 will always be black, so just shade bottom all black (old u,v in comments)
    vertex[12] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2 -2, location.z - size.z, 0.0f,  0.0f,    0, 1, 0); // 1,0
    vertex[13] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2 -2, location.z - size.z, 0.0f,  0.0f,    0, 0, 1); // 1,1
    vertex[14] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2 -2, location.z + size.z, 0.0f,  0.0f,    0, 1, 0); // 0,1
    vertex[15] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2 -2, location.z + size.z, 0.0f,  0.0f,    1, 0, 0); // 0,0
    // Left Face
    // u,v coords are set to 'continue' on from front face & wrap (old u,v in comments)
    // the -1 allows windows to stay lit around corners
    vertex[16] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2 -2, location.z - size.z, 0.0f,  0.0f,    0, 1, 0); // 0,1
    vertex[17] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2, location.z - size.z, 0.0f,  0.0f,       0, 0, 1); // 1,1
    vertex[18] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2, location.z + size.z, 0.0f,  0.0f,       0, 1, 0); // 1,0
    vertex[19] = BaryVertexFormat2(location.x - size.x, location.y + size.y * 2 -2, location.z + size.z, 0.0f,  0.0f,    1, 0, 0); // 0,0
    // Right Face
    // u,v coords are set to 'continue' on from front face & wrap (old u,v in comments)
    // the -1 allows windows to stay lit around corners
    vertex[20] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2 -2, location.z - size.z, 0.0f,  0.0f,    1, 0, 0); // 0,0
    vertex[21] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2, location.z - size.z, 0.0f,  0.0f,       0, 1, 0); // 1,0
    vertex[22] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2, location.z + size.z, 0.0f,  0.0f,       0, 0, 1); // 1,1
    vertex[23] = BaryVertexFormat2(location.x + size.x, location.y + size.y * 2 -2, location.z + size.z, 0.0f,  0.0f,    0, 1, 0); // 0,1
    //Front
    index[0] = 0 + m_numVertices;   index[1] = 1 + m_numVertices;   index[2] = 2 + m_numVertices;
    index[3] = 0 + m_numVertices;   index[4] = 2 + m_numVertices;   index[5] = 3 + m_numVertices;
    // Top
    index[6] = 4 + m_numVertices;   index[7] = 5 + m_numVertices;   index[8] = 6 + m_numVertices;
    index[9] = 4 + m_numVertices;   index[10] = 6 + m_numVertices;  index[11] = 7 + m_numVertices;
    // Bottom
    index[12] = 15 + m_numVertices; index[13] = 14 + m_numVertices; index[14] = 13 + m_numVertices;
    index[15] = 15 + m_numVertices; index[16] = 13 + m_numVertices; index[17] = 12 + m_numVertices;
    // Back
    index[18] = 9 + m_numVertices;  index[19] = 8 + m_numVertices;  index[20] = 11 + m_numVertices;
    index[21] = 9 + m_numVertices;  index[22] = 11 + m_numVertices; index[23] = 10 + m_numVertices;
    // Left
    index[24] = 19 + m_numVertices; index[25] = 16 + m_numVertices; index[26] = 17 + m_numVertices;
    index[27] = 19 + m_numVertices; index[28] = 17 + m_numVertices; index[29] = 18 + m_numVertices;
    // Right
    index[30] = 20 + m_numVertices; index[31] = 23 + m_numVertices; index[32] = 22 + m_numVertices;
    index[33] = 20 + m_numVertices; index[34] = 22 + m_numVertices; index[35] = 21 + m_numVertices;

    GLuint numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    GLuint numIndices = sizeof(index) / sizeof(GLushort);

    // Add block vertices to building
    for(unsigned int i = 0; i <  numVertices; i++) {
        m_vertex.push_back(vertex[i]);
    }
    m_numVertices += numVertices;

    // Add block indices to building
    for(unsigned int i = 0; i <  numIndices; i++) {
        m_index.push_back(index[i]);
    }
    m_numIndices += numIndices;
}

void Building::AddAntenna(Material mat, GLvector3 location, GLvector3 size) {
    // location is (midX, botY, midZ) - i.e. middle of X/Z coord is middle of building, Y always 0.0 (on ground)
    // size is the size of the top of the roof block

    // Vertex Buffer (Interleaved)
    BaryVertexFormat2 vertex[8];
    // Index Buffer
    GLushort index[36];

    float xVac =  size.x;
    float zVac =  size.z;

    // *** Remember: tex u=0, v=0 => upper left of texture ***

    // Setup vertex and index VBOs
    // Need seperate vertices per face (for individual texture coordinates)
    // Light will use top 1.0
    m_hasLight = true;
    m_light.Create(mat, glVector(location.x, size.y * 2 - 0.5, location.z));

    // Front Face
    // Barys will be broken unless I add more vertices :(
    vertex[0] = BaryVertexFormat2(location.x - xVac, size.y * 2 - 1.0, location.z - zVac, 0.0f,  0.0f,   1, 0, 0);
    vertex[1] = BaryVertexFormat2(location.x + xVac, size.y * 2 - 1.0, location.z - zVac, 0.0f,  0.0f,   0, 0, 1);
    vertex[2] = BaryVertexFormat2(location.x + xVac, size.y * 2 - 1.0, location.z + zVac, 0.0f,  0.0f,   1, 0, 0);
    vertex[3] = BaryVertexFormat2(location.x - xVac, size.y * 2 - 1.0, location.z + zVac, 0.0f,  0.0f,   0, 1, 0);
    vertex[4] = BaryVertexFormat2(location.x - xVac, size.y * 2 - 4.0, location.z - zVac, 0.0f,  0.0f,   0, 0, 1);
    vertex[5] = BaryVertexFormat2(location.x + xVac, size.y * 2 - 4.0, location.z - zVac, 0.0f,  0.0f,   0, 1, 0);
    vertex[6] = BaryVertexFormat2(location.x + xVac, size.y * 2 - 4.0, location.z + zVac, 0.0f,  0.0f,   1, 0, 0);
    vertex[7] = BaryVertexFormat2(location.x - xVac, size.y * 2 - 4.0, location.z + zVac, 0.0f,  0.0f,   0, 1, 0);

    //Front
    index[0] = 0 + m_numVertices;   index[1] = 5 + m_numVertices;   index[2] = 1 + m_numVertices;
    index[3] = 0 + m_numVertices;   index[4] = 4 + m_numVertices;   index[5] = 5 + m_numVertices;
    // Bottom
    index[6] = 3 + m_numVertices;   index[7] = 1 + m_numVertices;   index[8] = 2 + m_numVertices;
    index[9] = 3 + m_numVertices;   index[10] = 0 + m_numVertices;  index[11] = 1 + m_numVertices;
    // Bottom
    index[12] = 4 + m_numVertices; index[13] = 6 + m_numVertices; index[14] = 5 + m_numVertices;
    index[15] = 4 + m_numVertices; index[16] = 7 + m_numVertices; index[17] = 6 + m_numVertices;
    // Back
    index[18] = 2 + m_numVertices;  index[19] = 7 + m_numVertices;  index[20] = 3 + m_numVertices;
    index[21] = 2 + m_numVertices;  index[22] = 6 + m_numVertices; index[23] = 7 + m_numVertices;
    // Left
    index[24] = 3 + m_numVertices; index[25] = 4 + m_numVertices; index[26] = 0 + m_numVertices;
    index[27] = 3 + m_numVertices; index[28] = 7 + m_numVertices; index[29] = 4 + m_numVertices;
    // Right
    index[30] = 1 + m_numVertices; index[31] = 6 + m_numVertices; index[32] = 2 + m_numVertices;
    index[33] = 1 + m_numVertices; index[34] = 5 + m_numVertices; index[35] = 6 + m_numVertices;

    GLuint numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    GLuint numIndices = sizeof(index) / sizeof(GLushort);

    // Add block vertices to building
    for(unsigned int i = 0; i <  numVertices; i++) {
        m_vertex.push_back(vertex[i]);
    }
    m_numVertices += numVertices;

    // Add block indices to building
    for(unsigned int i = 0; i <  numIndices; i++) {
        m_index.push_back(index[i]);
    }
    m_numIndices += numIndices;
}

void Building::AddBoxes(GLvector3 location, GLvector3 size) {
    // location is (midX, botY, midZ) - i.e. middle of X/Z coord is middle of building, Y always 0.0 (on ground)
    // size is the size of the top of the roof block

    // Vertex Buffer (Interleaved)
    BaryVertexFormat2 vertex[8];
    // Index Buffer
    GLushort index[36];

    GLuint xVac = RandomVal(size.x - 2) + 1;
    GLuint zVac = RandomVal(size.z - 2) + 1;

    // *** Remember: tex u=0, v=0 => upper left of texture ***

    // Setup vertex and index VBOs
    // Need seperate vertices per face (for individual texture coordinates)
    // Front Face
    // Barys will be broken unless I add more vetices :(
    vertex[0] = BaryVertexFormat2(location.x - xVac, size.y * 2,       location.z - zVac, 0.0f,  0.0f,   1, 0, 0);
    vertex[1] = BaryVertexFormat2(location.x + xVac, size.y * 2,       location.z - zVac, 0.0f,  0.0f,   0, 0, 1);
    vertex[2] = BaryVertexFormat2(location.x + xVac, size.y * 2,       location.z + zVac, 0.0f,  0.0f,   1, 0, 0);
    vertex[3] = BaryVertexFormat2(location.x - xVac, size.y * 2,       location.z + zVac, 0.0f,  0.0f,   0, 1, 0);
    vertex[4] = BaryVertexFormat2(location.x - xVac, size.y * 2 - 2.0, location.z - zVac, 0.0f,  0.0f,   0, 0, 1);
    vertex[5] = BaryVertexFormat2(location.x + xVac, size.y * 2 - 2.0, location.z - zVac, 0.0f,  0.0f,   0, 1, 0);
    vertex[6] = BaryVertexFormat2(location.x + xVac, size.y * 2 - 2.0, location.z + zVac, 0.0f,  0.0f,   1, 0, 0);
    vertex[7] = BaryVertexFormat2(location.x - xVac, size.y * 2 - 2.0, location.z + zVac, 0.0f,  0.0f,   0, 1, 0);

    //Front
    index[0] = 0 + m_numVertices;   index[1] = 5 + m_numVertices;   index[2] = 1 + m_numVertices;
    index[3] = 0 + m_numVertices;   index[4] = 4 + m_numVertices;   index[5] = 5 + m_numVertices;
    // Top
    index[6] = 3 + m_numVertices;   index[7] = 1 + m_numVertices;   index[8] = 2 + m_numVertices;
    index[9] = 3 + m_numVertices;   index[10] = 0 + m_numVertices;  index[11] = 1 + m_numVertices;
    // Bottom
    index[12] = 4 + m_numVertices; index[13] = 6 + m_numVertices; index[14] = 5 + m_numVertices;
    index[15] = 4 + m_numVertices; index[16] = 7 + m_numVertices; index[17] = 6 + m_numVertices;
    // Back
    index[18] = 2 + m_numVertices;  index[19] = 7 + m_numVertices;  index[20] = 3 + m_numVertices;
    index[21] = 2 + m_numVertices;  index[22] = 6 + m_numVertices; index[23] = 7 + m_numVertices;
    // Left
    index[24] = 3 + m_numVertices; index[25] = 4 + m_numVertices; index[26] = 0 + m_numVertices;
    index[27] = 3 + m_numVertices; index[28] = 7 + m_numVertices; index[29] = 4 + m_numVertices;
    // Right
    index[30] = 1 + m_numVertices; index[31] = 6 + m_numVertices; index[32] = 2 + m_numVertices;
    index[33] = 1 + m_numVertices; index[34] = 5 + m_numVertices; index[35] = 6 + m_numVertices;

    GLuint numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    GLuint numIndices = sizeof(index) / sizeof(GLushort);

    // Add block vertices to building
    for(unsigned int i = 0; i <  numVertices; i++) {
        m_vertex.push_back(vertex[i]);
    }
    m_numVertices += numVertices;

    // Add block indices to building
    for(unsigned int i = 0; i <  numIndices; i++) {
        m_index.push_back(index[i]);
    }
    m_numIndices += numIndices;
}

void Building::AddSpike(GLvector3 location, GLvector3 size) {
    // Vertex Buffer (Interleaved)
    BaryVertexFormat2 vertex[5];
    // Index Buffer
    GLushort index[12];

    // location is (midx,lowery,midz) of  spike
    // size is (1/2x, maxY, 1/2 z)

    int xSize = location.x * 2;
    int ySize = size.y - location.y;
    int zSize = location.z * 2;

    // Setup vertex and index VBOs
    // Spike is 4 triangles
    // Don't need seperate vertices per face (since no individual texture coordinates)
    vertex[0] = BaryVertexFormat2(location.x, size.y, location.z, 0.0f, 0.0f,                            1, 0, 0);           // Apex
    vertex[1] = BaryVertexFormat2(location.x - size.x, location.y, location.z - size.z, 0.0f,  0.0f,     0, 1, 0);
    vertex[2] = BaryVertexFormat2(location.x + size.x, location.y, location.z - size.z, 0.0f,  0.0f,     0, 0, 1);
    vertex[3] = BaryVertexFormat2(location.x + size.x, location.y, location.z + size.z, 0.0f,  0.0f,     0, 1, 0);
    vertex[4] = BaryVertexFormat2(location.x - size.x, location.y, location.z + size.z, 0.0f,  0.0f,     0, 0, 1);

    //Front
    index[0] = 0 + m_numVertices;   index[1] = 1 + m_numVertices;   index[2] = 2 + m_numVertices;
    // Right
    index[3] = 0 + m_numVertices;   index[4] = 2 + m_numVertices;   index[5] = 3 + m_numVertices;
    // Back
    index[6] = 0 + m_numVertices;   index[7] = 3 + m_numVertices;   index[8] = 4 + m_numVertices;
    // Left
    index[9] = 0 + m_numVertices;   index[10] = 4 + m_numVertices;  index[11] = 1 + m_numVertices;

    GLuint numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    GLuint numIndices = sizeof(index) / sizeof(GLushort);

    // Add block vertices to building
    for(unsigned int i = 0; i <  numVertices; i++) {
        m_vertex.push_back(vertex[i]);
    }
    m_numVertices += numVertices;

    // Add block indices to building
    for(unsigned int i = 0; i <  numIndices; i++) {
        m_index.push_back(index[i]);
    }
    m_numIndices += numIndices;
}

void Building::AddSign(GLvector3 location, GLvector3 size) {
    // location is (midX, botY, midZ) - i.e. middle of X/Z coord is middle of building, Y always 0.0 (on ground)
    // size is the size of the top of the roof block

    // Vertex Buffer (Interleaved)
    //BaryVertexFormat2 vertex[8];
    // Index Buffer
    //GLushort index[12];

    // *** Remember: tex u=0, v=0 => upper left of texture ***

    // Setup vertex and index VBOs
    // Need seperate vertices per face (for individual texture coordinates)
    // Front Face
    m_signVertex[0] = BaryVertexFormat2(location.x - size.x -0.1, size.y * 2,       location.z - size.z-0.1,     0.0f,  0.0f,    1, 0, 0);
    m_signVertex[1] = BaryVertexFormat2(location.x + size.x+0.1, size.y * 2,       location.z - size.z-0.1,      1.0f,  0.0f,    0, 1, 0);
    m_signVertex[2] = BaryVertexFormat2(location.x + size.x+0.1, size.y * 2 - 2.0, location.z - size.z-0.1,      1.0f,  1.0f,    0, 0, 1);
    m_signVertex[3] = BaryVertexFormat2(location.x - size.x-0.1, size.y * 2 - 2.0, location.z - size.z-0.1,      0.0f,  1.0f,    0, 1, 0);
    m_signVertex[4] = BaryVertexFormat2(location.x - size.x-0.1, size.y * 2,       location.z + size.z+0.1,      1.0f,  0.0f,    0, 1, 0);
    m_signVertex[5] = BaryVertexFormat2(location.x + size.x+0.1, size.y * 2,       location.z + size.z+0.1,      0.0f,  0.0f,    1, 0, 0);
    m_signVertex[6] = BaryVertexFormat2(location.x + size.x+0.1, size.y * 2 - 2.0, location.z + size.z+0.1,      0.0f,  1.0f,    0, 1, 0);
    m_signVertex[7] = BaryVertexFormat2(location.x - size.x-0.1, size.y * 2 - 2.0, location.z + size.z+0.1,      1.0f,  1.0f,    0, 0, 1);
    //Front
    m_signIndex[0] = 0;   m_signIndex[1] = 2;   m_signIndex[2] = 1;
    m_signIndex[3] = 0;   m_signIndex[4] = 3;   m_signIndex[5] = 2;
    // Back
    m_signIndex[6] = 5;   m_signIndex[7] = 7;   m_signIndex[8] = 4;
    m_signIndex[9] = 5;   m_signIndex[10] = 6;  m_signIndex[11] = 7;

    // Generate VBOs
    glGenBuffers(2, m_signVboIds);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_signVboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BaryVertexFormat2) * 8, m_signVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_signVboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 12, m_signIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
