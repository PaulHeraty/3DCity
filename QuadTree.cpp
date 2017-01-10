#include "QuadTree.h"

std::vector< std::vector<GLuint> > QuadTree::m_currentIndexCount;

QuadTree::QuadTree() {
    m_numSlIndices = 0;
    m_numSlVertices = 0;
    m_numstreetlights = 0;

    std::vector<GLuint> i;
    m_indexCount.push_back(i);
    m_primCount.push_back(i);
    //m_currentIndexCount.push_back(i);

    for(int i=0; i<MAX_MATERIAL_TYPES; i++) {
        m_indexCount[0].push_back(0);
        m_primCount[0].push_back(0);
        //m_currentIndexCount[0].push_back(0);
    }
}

QuadTree::~QuadTree() {
}

void QuadTree::AddBuilding(Material mat, Material lightMat, GLvector3 loc, GLvector3 size, unsigned short tessFactor,
                           BuildingType buildingType) {
    Building thisBuilding;

    switch(buildingType) {
    case(BUILDING_SIMPLE):
        //thisBuilding.CreateSimple(vertexShader, fragmentShader, loc, size, tessFactor, texType);
        thisBuilding.CreateSimple(mat, loc, size, tessFactor);
        break;
    case(BUILDING_BLOCKY):
        //thisBuilding.CreateBlocky(vertexShader, fragmentShader, loc, size, tessFactor, texType);
        thisBuilding.CreateBlocky(mat, lightMat, loc, size, tessFactor);
        break;
    case(BUILDING_MODERN):
        //thisBuilding.CreateModern(vertexShader, fragmentShader, loc, size, tessFactor, texType);
        thisBuilding.CreateModern(mat, loc, size, tessFactor);
        break;
    case(BUILDING_TOWER):
        //thisBuilding.CreateTower(vertexShader, fragmentShader, loc, size, tessFactor, texType);
        thisBuilding.CreateTower(mat, loc, size, tessFactor);
        break;
    }

    // Add building to City list
    m_buildings.push_back(thisBuilding);
    m_numBuildings++;
}

void QuadTree::Create(unsigned int lowerx, unsigned int lowerz, unsigned short tessFactor, GLint m_vtxStride,
                      MaterialBin *materials, int quadTreeSize) {
    GLvector buildingMid, buildingSize;
    unsigned int bHeight;
    BuildingType bType;
    Material mat, lightMat;
    unsigned int highest = 0;

    // Create Buildings
    //printf("Creating quadtree buildings...\n");

    // Suburbs are outer 2 quadTree boxes
    bool burbs = false;
    if ((lowerx < 256 || lowerx > 640) || (lowerz < 256 || lowerz > 640))
        burbs = true;

    int qtSize = 1024 / quadTreeSize;

    // 4x4 plots in the 128 quadTree (each plot is 32x32)
    for(int i = 0; i < qtSize; i+=32) {
        for(int j = 0; j < qtSize; j+=32) {
            // Offset to building center will be 20 (half buliding size(12) + road width(6) + 2xfootpath width(2))
            buildingMid = glVector(lowerx+i+20.0f, 0.0f, lowerz+j+20.0f);
            // size will always be 12*12 and height will be taller in center for skyscrapers
            bHeight = RandomVal(5) + 5;
            bType = BuildingType(RandomVal(4));
            if(!burbs) {
                // In middle, make buliding heights in 25-35 range
                bHeight = RandomVal(10) + 25;
                // Occasionally add in a skyscraper
                if(RandomVal(100) >= 90) {
                    bHeight = RandomVal(10) + 45;
                }
                bType = BuildingType(RandomVal(3)+1);
            }
            buildingSize = glVector(12.0f, bHeight, 12.0f);
            mat = materials[RandomVal(10) + BUILDING0_MAT].m_material;

            // Keep track of highest builing for quadTree AABB
            if(bHeight > highest)
                highest = bHeight;

            // Add the building
            lightMat = materials[LIGHT_MAT].m_material;
            AddBuilding(mat, lightMat, buildingMid, buildingSize, tessFactor, bType );

        }
    }


    // Now Create AABB for quadTree
    // Pass in center point plus size (size is /2)
    m_AABB.CreateAABB(glVector(lowerx+(qtSize/2.0f), 0.0f, lowerz+(qtSize/2.0f)), glVector((qtSize/2.0f), highest, (qtSize/2.0f)), true);

    // Add Building Signs
    // Step 1. Create a single texture map with 8 signs
    // NB : SIGN0_MAT is 13th in the materialType list, and there are 10 of them
    m_signMaterial = materials[RandomVal(10)+SIGN0_MAT].m_material;
    // Step 2. Find buildings with m_hasSign = true
    unsigned int signNumber = 0;
    unsigned int vertOffset = 0;
    BaryVertexFormat2 thisVert;
    GLushort thisIndex;
    // Loop for number of buildings in each quadTree node
    int numQtBuildings = (qtSize / 32) * (qtSize / 32);
    for(int i = 0; i < numQtBuildings; i++) {
        if(m_buildings[i].m_hasSign) {
            // Step 3. Copy sign vertex info
            //   8 vertices (2 signs)
            for(int j=0; j < 8; j++) {
                thisVert = m_buildings[i].m_signVertex[j];
                // Update v coords
                if(thisVert.v == 0) {
                    thisVert.v = signNumber / 8.0f;
                    if(thisVert.v > 0.875)
                        thisVert.v = 0.875;
                }
                else {
                    thisVert.v = (signNumber+1) / 8.0f;
                    if(thisVert.v > 1.0)
                        thisVert.v = 1.0;
                }
                m_signVertex.push_back(thisVert);
                m_numSignVertices++;
            }
            signNumber++;
            // Setp 4. Copy sign index info
            // Copy indices to appropriate index buffer
            //    12 indices (2 signs)
            for(int j=0; j < 12; j++) {
                thisIndex = m_buildings[i].m_signIndex[j];
                m_signIndex.push_back(thisIndex + vertOffset);
                m_numSignIndices++;
            }
            // Updata vertex offset
            vertOffset = m_numSignVertices;
        }
    }

    // Create Streetlights in X
    Streetlight newSl;
    // For now, assign random texture to quadTree node
    mat = materials[STREETLIGHT_MAT].m_material;
    for(int i = 0; i < qtSize; i+=32) {
        for(int j = 0; j < qtSize; j+=32) {
            newSl.Create(mat, glVector(lowerx+i+8.0f,2.0f,lowerz+j), glVector(lowerx+i+32.0f,2.0f,lowerz+j+1.0f));
            m_streetlights.push_back(newSl);
            m_numstreetlights++;
            newSl.Create(mat, glVector(lowerx+i+8.0f,2.0f,lowerz+j+7.0f), glVector(lowerx+i+32.0f,2.0f,lowerz+j+8.0f));
            m_streetlights.push_back(newSl);
            m_numstreetlights++;
            //m_cityVertices += 8;
            //m_cityIndices += 12;
        }
    }

    // Create Streetlights in Z
    for(int i = 0; i < qtSize; i+=32) {
        for(int j = 0; j < qtSize; j+=32) {
            newSl.Create(mat, glVector(lowerx+j,2.0f,lowerz+i+8.0f), glVector(lowerx+j+1.0f,2.0f,lowerz+i+32.0f));
            m_streetlights.push_back(newSl);
            m_numstreetlights++;
            newSl.Create(mat, glVector(lowerx+j+7.0f,2.0f,lowerz+i+8.0f), glVector(lowerx+j+8.0f,2.0f,lowerz+i+32.0f));
            m_streetlights.push_back(newSl);
            m_numstreetlights++;
            //m_cityVertices += 8;
            //m_cityIndices += 12;
        }
    }

    // Create roads in X
    mat = materials[ROAD_MAT].m_material;
    RoadRoute road;
    for(int i = 0; i < qtSize; i+=32) {
        for(int j = 0; j < qtSize; j+=32) {
            road.Create(mat, glVector(lowerx+i+0.0f,0.03f,lowerz+j+1.0f), glVector(lowerx+i+32.0f,0.03f,lowerz+j+7.0f));
            m_roads.push_back(road);
            m_numroads++;
            //m_cityVertices += 8;
            //m_cityIndices += 12;
        }
    }

    // Create roads in Z
    for(int i = 0; i < qtSize; i+=32) {
        for(int j = 0; j < qtSize; j+=32) {
            road.Create(mat, glVector(lowerx+j+1.0f,0.05f,lowerz+i+0.0f), glVector(lowerx+j+7.0f,0.05f,lowerz+i+32.0f));
            m_roads.push_back(road);
            m_numroads++;
            //m_cityVertices += 8;
            //m_cityIndices += 12;
        }
    }

    // Populate street light vertex/index buffers
    vertOffset = 0;
    for(unsigned int i=0; i < m_numstreetlights; i++) {
        // Copy vertices into quadTree vertex buffer
        for(int j=0; j<m_streetlights[i].numVertices; j++) {
            m_slVertex.push_back(m_streetlights[i].vertex[j]);
            m_numSlVertices++;
        }
        // Copy indices to appropriate index buffer
        for(int j=0; j < m_streetlights[i].numIndices; j++) {
            thisIndex = m_streetlights[i].index[j];
            m_slIndex.push_back(m_streetlights[i].index[j] + vertOffset);
            m_numSlIndices++;
        }

        // Updata vertex offset
        vertOffset = m_numSlVertices;
    }

    // Populate roads vertex/index buffers
    vertOffset = 0;
    for(unsigned int i=0; i < m_numroads; i++) {
        // Copy vertices into quadTree vertex buffer
        for(int j=0; j<m_roads[i].m_numVertices; j++) {
            m_rVertex.push_back(m_roads[i].m_vertex[j]);
            m_numRVertices++;
        }
        // Copy indices to appropriate index buffer
        for(int j=0; j < m_roads[i].m_numIndices; j++) {
            thisIndex = m_roads[i].m_index[j];
            m_rIndex.push_back(m_roads[i].m_index[j] + vertOffset);
            m_numRIndices++;
        }

        // Updata vertex offset
        vertOffset = m_numRVertices;
    }
}

void QuadTree::Draw(CCamera *camera) {
    // Draw will be done in BaseTree

    // Draw AABB?
    if(g_drawAABB) {
        m_AABB.Draw(camera);
        for(unsigned int i=0; i < m_numBuildings; i++)
            m_buildings[i].m_AABB.Draw(camera);
    }
}

RenderBatch::RenderBatch() {
    m_indexCount = 0;
    m_primCount = 0;
}

RenderBatch::~RenderBatch() {
    m_indexCount = 0;
    m_primCount = 0;
}


MaterialBin::MaterialBin() {
    // Create the first index buffer
    //std::vector<GLushort> ib;
    std::vector<GLuint> ib;
    m_index.push_back(ib);
    GLuint v = 0;
    m_numIndices.push_back(v);
    m_numRenderBatches.push_back(v);

    // Create first RenderBatch
    std::vector<RenderBatch> i;
    m_renderBatches.push_back(i);

}

MaterialBin::~MaterialBin() {
}

void MaterialBin::Create(materialType t) {
    m_material.m_matType = t;

    switch(t) {
    case(BUILDING0_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING0);
        break;
    case(BUILDING1_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING1);
        break;
    case(BUILDING2_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING2);
        break;
    case(BUILDING3_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING3);
        break;
    case(BUILDING4_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING4);
        break;
    case(BUILDING5_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING5);
        break;
    case(BUILDING6_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING6);
        break;
    case(BUILDING7_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING7);
        break;
    case(BUILDING8_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING8);
        break;
    case(BUILDING9_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_BUILDING9);
        break;
    case(SIMPLE_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/basic.vp", "shaders/basic.fp", SIMPLE_TEXTURE);
        break;
    case(BLACK_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/basic.vp", "shaders/basic.fp", BLACK_TEXTURE);
        break;
    case(ROAD_MAT):
        m_material.CreateProgram(ROUTE_SHADER, "shaders/roadroute.vp", "shaders/roadroute.fp", TEXTURE_FILE, "images/road.tga", 144);
        break;
        /*
    case(SKYBOX_LEFT_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/skybox.vp", "shaders/skybox.fp", TEXTURE_FILE);
        break;
    case(SKYBOX_RIGHT_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/skybox.vp", "shaders/skybox.fp", TEXTURE_FILE);
        break;
    case(SKYBOX_TOP_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/skybox.vp", "shaders/skybox.fp", TEXTURE_FILE);
        break;
    case(SKYBOX_FRONT_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/skybox.vp", "shaders/skybox.fp", TEXTURE_FILE);
        break;
    case(SKYBOX_BACK_MAT):
        m_material.CreateProgram(BASIC_SHADER, "shaders/skybox.vp", "shaders/skybox.fp", TEXTURE_FILE);
        break;
        */
    case(SIGN0_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN0);
        break;
    case(SIGN1_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN1);
        break;
    case(SIGN2_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN2);
        break;
    case(SIGN3_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN3);
        break;
    case(SIGN4_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN4);
        break;
    case(SIGN5_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN5);
        break;
    case(SIGN6_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN6);
        break;
    case(SIGN7_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN7);
        break;
    case(SIGN8_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN8);
        break;
    case(SIGN9_MAT):
        m_material.CreateProgram(BARY_SHADER, "shaders/bary.vp", "shaders/bary.fp", TEXTURE_SIGN9);
        break;
    case(LIGHT_MAT):
        m_material.CreateProgram(LIGHT_SHADER, "shaders/light.vp", "shaders/light.fp", TEXTURE_LIGHT);
        m_material.m_state.m_alpha = true;
        break;
    case(CAR_MAT):
        m_material.CreateProgram(CAR_SHADER, "shaders/car.vp", "shaders/car.fp", TEXTURE_CAR);
        m_material.m_state.m_alpha = true;
        break;
    case(CARS_MAT):
        m_material.CreateProgram(CARS_SHADER, "shaders/cars.vp", "shaders/cars.fp", TEXTURE_CARS);
        m_material.m_state.m_alpha = true;
        break;
    case(STREETLIGHT_MAT):
        m_material.CreateProgram(STREETLIGHT_SHADER, "shaders/streetlight.vp", "shaders/streetlight.fp", TEXTURE_STREETLIGHT);
        m_material.m_state.m_alpha = true;
        break;
    case(DEPTH_COMPLEXITY_MAT):
        m_material.CreateProgram(DEPTH_COMPLEXITY_SHADER, "shaders/depthComplexity.vp", "shaders/depthComplexity.fp", TEXTURE_NONE);
        break;
    default:
        printf("*ERROR* Unsupported material type\n");
        break;
    }
}

void MaterialBin::UpdateRenderBatch(GLuint indexCount, GLuint primCount, int vboNum) {
    RenderBatch rb;
    rb.m_indexCount = indexCount;
    rb.m_primCount = primCount;
    m_renderBatches[vboNum].push_back(rb);
    m_numRenderBatches[vboNum]++;
}

void MaterialBin::MergeRenderBatches(int vboNum) {
    unsigned int currentIndex;
    for(unsigned int i=0; i<m_numRenderBatches[vboNum]-1; ) {
        currentIndex = m_renderBatches[vboNum][i].m_primCount * 3 + m_renderBatches[vboNum][i].m_indexCount;
        if(m_renderBatches[vboNum][i+1].m_indexCount == currentIndex) {
            m_renderBatches[vboNum][i].m_primCount += m_renderBatches[vboNum][i+1].m_primCount;
            m_renderBatches[vboNum].erase(m_renderBatches[vboNum].begin() + i + 1);
            m_numRenderBatches[vboNum]--;
            //m_renderBatches[vboNum].resize(m_numRenderBatches[vboNum]);
        } else {
            i++;
        }
    }
}

void MaterialBin::DrawMaterial(CCamera *camera, int vboNum) {
    if(m_material.m_state.m_visible) {
        // Reset offset for each object
        GLuint offset = 0;

        // Use the program object
        glUseProgram(m_material.m_shader.m_programObject);

        // Load the vertex position attribute
        glVertexAttribPointer ( m_material.m_shader.m_positionLoc, 3, GL_FLOAT,
                GL_FALSE, sizeof(BaryVertexFormat2), (const void*)offset );
        offset += sizeof(GLfloat) * 3;

        // Load the texture coordinate attribute
        glVertexAttribPointer ( m_material.m_shader.m_texCoordLoc, 2, GL_FLOAT,
                GL_FALSE, sizeof(BaryVertexFormat2), (const void *)offset );
        offset += sizeof(GLfloat) * 2;

        // Load the bary coordinate attribute
        glVertexAttribPointer ( m_material.m_shader.m_baryLoc, 3, GL_BYTE,
                GL_FALSE, sizeof(BaryVertexFormat2), (const void *)offset );

        // Set the mvpMatrix uniform
        GLmatrix vMatrix = camera->GetViewMatrix();
        GLmatrix pMatrix = camera->GetProjMatrix();
        GLmatrix mvpMatrix = glMatrixMultiply(vMatrix, pMatrix);
        glUniformMatrix4fv(m_material.m_shader.m_mvpMatrixLoc, 1, GL_FALSE,  &mvpMatrix.elements[0][0]);

        glEnableVertexAttribArray(m_material.m_shader.m_positionLoc);
        glEnableVertexAttribArray(m_material.m_shader.m_texCoordLoc);
        glEnableVertexAttribArray(m_material.m_shader.m_baryLoc);

        // Time to draw
        // User VBOs Index Data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVboId[vboNum]);

        // Bind the texture
        glActiveTexture (GL_TEXTURE0);
        // glEnable(GL_TEXTURE_2D); DEPRECATED
        glBindTexture(GL_TEXTURE_2D, m_material.m_texture.m_textureId);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        // Set the sampler texture unit to 0
        glUniform1i (m_material.m_shader.m_samplerLoc, 0);

        // Set wireframe mode in shader
        glUniform1i(m_material.m_shader.m_wireframeLoc, g_wireframe);

        // Set the highlight to 0
        glUniform1i ( m_material.m_shader.m_highlightLoc, 0 );

        if(g_wireframe==2) {
            // Blend particles
            //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glDisable(GL_CULL_FACE);
        }

        if(m_material.m_state.m_alpha) {
            // Blend particles
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }

        // Now loop through the number of consecutive render batches
        for(unsigned int i=0; i < m_numRenderBatches[vboNum]; i++) {
            // User VBOs Index Data
            // Submit draw
            // Each render buffer has m_primCount*3 indices, and an offset into the index VBO of m_indexCount
            glDrawElements(GL_TRIANGLES, m_renderBatches[vboNum][i].m_primCount*3, GL_UNSIGNED_INT,
                            (void*)(m_renderBatches[vboNum][i].m_indexCount * sizeof(GLuint)));
            g_numBuildingDraws++;
        }

        if(g_wireframe==2) {
            // Disable Blend
            //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            glDisable(GL_BLEND );
            glEnable(GL_CULL_FACE);
        }

        if(m_material.m_state.m_alpha) {
            glDisable ( GL_BLEND );
        }

        // Clear render batches
        m_numRenderBatches[vboNum] = 0;
        m_renderBatches[vboNum].clear();
    } // if m_visible
}

BaseTree::BaseTree() {
    m_vtxStride = sizeof(BaryVertexFormat2);
    // Create the first vertex buffer
    std::vector<BaryVertexFormat2> vb;
    m_vertex.push_back(vb);
    m_numVBOs = 0;
    m_totalVertices = 0;
    GLint v = 0;
    m_numVertices.push_back(v);
    m_quadTreeSize = 1;
    m_numCars = 2000;
}

BaseTree::~BaseTree() {
}

void BaseTree::AddNewQuadTreeVBOIndex() {
    // Create 8x8 QuadTree (buildings and streetlights)
    for(unsigned int i=0; i < m_quadTreeSize; i++) {
        for(unsigned int j=0; j < m_quadTreeSize; j++) {
            // Create new quadTree vars
            std::vector<GLuint> t;
            m_quadTree[i][j].m_indexCount.push_back(t);
            m_quadTree[i][j].m_primCount.push_back(t);
            m_quadTree[i][j].m_currentIndexCount.push_back(t);
            for(int k=0; k<MAX_MATERIAL_TYPES; k++) {
                m_quadTree[i][j].m_indexCount[m_numVBOs].push_back(0);
                m_quadTree[i][j].m_primCount[m_numVBOs].push_back(0);
                m_quadTree[i][j].m_currentIndexCount[m_numVBOs].push_back(0);
            }
        }
    }
}

void BaseTree::AddVertIndBuffersToMain(materialType mType, std::vector<BaryVertexFormat2> verts, unsigned int numVertices,
                                       std::vector<GLushort> inds, unsigned int numIndices, unsigned int &offset) {
    // Copy vertices into BaseTree vertex buffers.
    //Max of MAX_VBO_VERTICES per buffer
    if((m_numVertices[m_numVBOs] + numVertices) > MAX_VBO_VERTICES) {
        // Need to create new vertex buffer
        std::vector<BaryVertexFormat2> vb;
        m_vertex.push_back(vb);
        GLuint v = 0;
        m_numVertices.push_back(v);
        m_numVBOs++;
        // create new material index buffers
        for(int l=0; l<MAX_MATERIAL_TYPES; l++) {
            //std::vector<GLushort> ib;
            std::vector<GLuint> ib;
            m_materialBin[l].m_index.push_back(ib);
            m_materialBin[l].m_numIndices.push_back(v);
            m_materialBin[l].m_numRenderBatches.push_back(v);
            // Add new RenderBatch
            std::vector<RenderBatch> rb;
            m_materialBin[l].m_renderBatches.push_back(rb);
        }
        // Create new quadTree vars
        AddNewQuadTreeVBOIndex();

        offset = 0;
    }

    // Add vertices to appropriate vertex buffer
    for(unsigned int l=0; l < numVertices; l++) {
        m_vertex[m_numVBOs].push_back(verts[l]);
        m_numVertices[m_numVBOs]++;
        m_totalVertices++;
    }

    // Copy indices to appropriate index buffer
    for(unsigned int l=0; l < numIndices; l++) {
        m_materialBin[mType].m_index[m_numVBOs].push_back(inds[l] + offset);
        m_materialBin[mType].m_numIndices[m_numVBOs]++;
    }

    // Update the index offset variable
    offset = m_numVertices[m_numVBOs];
}

void BaseTree::Create(unsigned short tessFactor, unsigned short quadTreeSize, unsigned int xSize, unsigned int ySize) {
    unsigned int vertOffset = 0;
    unsigned int signVertOffset = 0;
    unsigned int slVertOffset = 0;
    unsigned int rVertOffset = 0;
    //textureType tType;
    materialType mType;

    // Initialize the static variables in quadTree
    std::vector<GLuint> i;
    m_quadTree[0][0].m_currentIndexCount.push_back(i);
    for(int i=0; i<MAX_MATERIAL_TYPES; i++) {
        m_quadTree[0][0].m_currentIndexCount[0].push_back(0);
    }

    // Create the MaterialBins
    for(int i=0; i<MAX_MATERIAL_TYPES; i++) {
    //for(int i=0; i<10; i++) {
        m_materialBin[i].Create((materialType)i);
    }

    // Lets make roads 6 units wide + 2 units for footpaths
    // Building sites are 24x24
    // Map is 1024x1024
    // So divide map into a grid of 24+6+2=32 (total of 32 repetitions)
    printf("Creating City - please wait...\n");

    m_quadTreeSize = quadTreeSize;
    if(quadTreeSize != 1 && quadTreeSize != 2 &&
            quadTreeSize != 4 && quadTreeSize != 8 &&
            quadTreeSize != 16 && quadTreeSize != 32) {
        printf("*ERROR* Invalid quadTree_size value. Must be either 1,2,4,8,16 or 32. Setting to 1.");
        m_quadTreeSize = 1;
    }

    // Create 8x8 QuadTree (buildings and streetlights)
    for(unsigned int i=0; i < m_quadTreeSize; i++) {
        for(unsigned int j=0; j < m_quadTreeSize; j++) {
            // Create the quadTree
            // Each quad hold 4 building sites (32 units per side)
            m_quadTree[i][j].Create(i*(xSize/m_quadTreeSize), j*(ySize/m_quadTreeSize), tessFactor, m_vtxStride, m_materialBin, m_quadTreeSize);

            // Set the quadTree indexCount variables to the current Index counts per material
            for(int k=0; k<MAX_MATERIAL_TYPES; k++) {
                m_quadTree[i][j].m_indexCount[m_numVBOs][k] = m_quadTree[i][j].m_currentIndexCount[m_numVBOs][k];
            }

            // *** BUILDINGS ***
            // At this stage, quadTree.m_buildings are all populated
            // Now process the quadTree nodes, copy building vertices in m_vertex and update quadTree index vars
            for(unsigned int k=0; k < m_quadTree[i][j].m_numBuildings; k++) {

                // Depending on the texture type, we add indices to a appropriate MaterialBin array
                mType = m_quadTree[i][j].m_buildings[k].m_material.m_matType;

                AddVertIndBuffersToMain(mType, m_quadTree[i][j].m_buildings[k].m_vertex, m_quadTree[i][j].m_buildings[k].m_numVertices,
                                        m_quadTree[i][j].m_buildings[k].m_index, m_quadTree[i][j].m_buildings[k].m_numIndices, vertOffset);

                // Increment the primitive count for this material type
                m_quadTree[i][j].m_primCount[m_numVBOs][mType] += m_quadTree[i][j].m_buildings[k].m_numIndices / 3;
                m_quadTree[i][j].m_currentIndexCount[m_numVBOs][mType] += m_quadTree[i][j].m_buildings[k].m_numIndices;
            }

            // *** SIGNS ***
            // Copy the quadTree sign texture
            mType = m_quadTree[i][j].m_signMaterial.m_matType;

            AddVertIndBuffersToMain(mType, m_quadTree[i][j].m_signVertex, m_quadTree[i][j].m_numSignVertices,
                                    m_quadTree[i][j].m_signIndex, m_quadTree[i][j].m_numSignIndices, vertOffset);

            // Increment the primitive count for this material type
            m_quadTree[i][j].m_primCount[m_numVBOs][mType] += m_quadTree[i][j].m_numSignIndices / 3;
            m_quadTree[i][j].m_currentIndexCount[m_numVBOs][mType] += m_quadTree[i][j].m_numSignIndices;

            // *** STREETLIGHTS ***
            // Copy sign vertices/indices
            mType = m_materialBin[STREETLIGHT_MAT].m_material.m_matType;

            AddVertIndBuffersToMain(mType, m_quadTree[i][j].m_slVertex, m_quadTree[i][j].m_numSlVertices,
                                    m_quadTree[i][j].m_slIndex, m_quadTree[i][j].m_numSlIndices, vertOffset);

            // Increment the primitive count for this material type
            m_quadTree[i][j].m_primCount[m_numVBOs][mType] += m_quadTree[i][j].m_numSlIndices / 3;
            m_quadTree[i][j].m_currentIndexCount[m_numVBOs][mType] += m_quadTree[i][j].m_numSlIndices;

            // *** ROADS ***
            // Copy sign vertices/indices
            mType = m_materialBin[ROAD_MAT].m_material.m_matType;

            AddVertIndBuffersToMain(mType, m_quadTree[i][j].m_rVertex, m_quadTree[i][j].m_numRVertices,
                                    m_quadTree[i][j].m_rIndex, m_quadTree[i][j].m_numRIndices, vertOffset);

            // Increment the primitive count for this material type
            m_quadTree[i][j].m_primCount[m_numVBOs][mType] += m_quadTree[i][j].m_numRIndices / 3;
            m_quadTree[i][j].m_currentIndexCount[m_numVBOs][mType] += m_quadTree[i][j].m_numRIndices;
        }
    }

    // Create ground plane
    GroundPlane *ground = new GroundPlane("shaders/bary.vp", "shaders/bary.fp");
    ground->Create(xSize, ySize, BLACK_TEXTURE);
    m_ground.push_back(ground);
    //m_cityVertices += ground->numVertices;
    //m_cityIndices +=  ground->numIndices;

    // Create skybox
    SkyBox *skybox = new SkyBox ("shaders/skybox.vp", "shaders/skybox.fp");
    skybox->Create(xSize, ySize, "skybox/nightsky", 512);
    m_skybox.push_back(skybox);
    //m_cityVertices += skybox->m_numVertices;
    //m_cityIndices +=  skybox->m_numIndices;

    // Create road routes
    Material mat = m_materialBin[ROAD_MAT].m_material;
    RoadRoute *route1 = new RoadRoute;
    //route1->Create(mat, glVector(257.0f,0.1f,263.0f), glVector(263.0f,0.1f,801.0f));        // PosZ
    route1->Create(mat, glVector(225.0f,0.1f,231.0f), glVector(231.0f,0.1f,801.0f));        // PosZ
    m_routes.push_back(route1);
    //m_cityVertices += route1->m_numVertices;
    //m_cityIndices +=  route1->m_numIndices;
    RoadRoute *route2 = new RoadRoute;
    //route2->Create(mat, glVector(801.0f,0.1f,263.0f), glVector(807.0f,0.1f,801.0f));        //
    route2->Create(mat, glVector(801.0f,0.1f,231.0f), glVector(807.0f,0.1f,801.0f));        //
    m_routes.push_back(route2);
    //m_cityVertices += route2->m_numVertices;
    //m_cityIndices +=  route2->m_numIndices;
    RoadRoute *route3 = new RoadRoute;
    //route3->Create(mat, glVector(257.0f,0.1f,257.0f), glVector(807.0f,0.1f,263.0f));
    route3->Create(mat, glVector(225.0f,0.1f,225.0f), glVector(807.0f,0.1f,231.0f));
    m_routes.push_back(route3);
    //m_cityVertices += route3->m_numVertices;
    //m_cityIndices +=  route3->m_numIndices;
    RoadRoute *route4 = new RoadRoute;
    //route4->Create(mat, glVector(257.0f,0.1f,801.0f), glVector(807.0f,0.1f,807.0f));
    route4->Create(mat, glVector(225.0f,0.1f,801.0f), glVector(807.0f,0.1f,807.0f));
    m_routes.push_back(route4);
    //m_cityVertices += route4->m_numVertices;
    //m_cityIndices +=  route4->m_numIndices;

    // Create car
    m_car.Create(m_materialBin[CAR_MAT].m_material);

    // Create cockpit
    //m_cockpit.Create(m_materialBin[COCKPIT_MAT].m_material);
    m_cockpit.Create(1920, 1080);

    // Create vertex buff objects
    CreateVBOs();

    // Add in cars
    m_cars.Init(m_materialBin[CARS_MAT].m_material);

    for(int p=0; p <m_numCars; p++) {
        //Cars newCar;
        CarDirection direction = CarDirection(RandomVal(4));
        unsigned int road = RandomVal(32);
        float offset[3];
        if(direction == 0) {
            // Pos X
            offset[0] = RandomVal(256) * 4.0;
            offset[1] = 0.2f;
            offset[2] = (road*32) + 1.5f;
            //pos = glVector(RandomVal(512) * 2.0, 0.2f, (road*32) + 1.5f);
        } else if(direction == 1) {
            // Neg X
            offset[0] = RandomVal(256) * 4.0;
            offset[1] = 0.2f;
            offset[2] = (road*32) + 4.5f;
            //pos = glVector(RandomVal(512) * 2.0, 0.2f, (road*32) + 4.5f);
        } else if (direction == 2) {
            // Pos Z
            offset[0] = (road*32) + 4.5f;
            offset[1] = 0.2f;
            offset[2] = RandomVal(256) * 4.0;
            //pos = glVector((road*32) + 4.5f, 0.2f, RandomVal(512) * 2.0);
        } else {
            // Neg Z
            offset[0] = (road*32) + 1.5f;
            offset[1] = 0.2f;
            offset[2] = RandomVal(256) * 4.0;
            //pos = glVector((road*32) + 1.5f, 0.2f, RandomVal(512) * 2.0);
        }
        //newCar.Create(m_materialBin[CARS_MAT].m_material, offset, direction);
        //m_cars.push_back(newCar);
        m_cars.AddCar(offset, direction);
    }
    m_cars.FinishInit();
}

void BaseTree::CreateVBOs() {
    // BUILDINGS
    // Create building VBO objects
    for(unsigned int i=0; i<=m_numVBOs; i++) {
        GLuint vbo;
        m_vertexVboId.push_back(vbo);

        // Generate Vertex VBOs
        glGenBuffers(1, &m_vertexVboId[i]);

        // Vertex Array
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexVboId[i]);
        glBufferData(GL_ARRAY_BUFFER, m_vtxStride * m_numVertices[i], &m_vertex[i][0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Index Array Buffers
    for(unsigned int i=0; i<=m_numVBOs; i++) {
        for(int j=0; j<MAX_MATERIAL_TYPES; j++) {
            if(m_materialBin[j].m_numIndices[i] > 0) {
                GLuint ibo;
                m_materialBin[j].m_indexVboId.push_back(ibo);
                glGenBuffers(1, &m_materialBin[j].m_indexVboId[i]);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_materialBin[j].m_indexVboId[i]);
                //(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_materialBin[j].m_numIndices[i], &m_materialBin[j].m_index[i][0], GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_materialBin[j].m_numIndices[i], &m_materialBin[j].m_index[i][0], GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }
    }
}

void BaseTree::Add2RenderBatch(QuadTree *qt) {
    // Foreach Vertex VBO
    for(unsigned int i=0; i<=m_numVBOs; i++) {
        // foreach material, update LeafTree renderbatch bins
        for(int j=0; j<MAX_MATERIAL_TYPES; j++) {
            if(qt->m_primCount[i][j] > 0)
                m_materialBin[j].UpdateRenderBatch(qt->m_indexCount[i][j], qt->m_primCount[i][j], i);
        }
    }
}

void BaseTree::DrawDepthComplexity(CCamera *camera, int depth) {
    static GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f};
    static GLushort index[6] = {0, 2, 1, 0, 3, 2};
    static GLuint vtxStride = 3 * sizeof(GLfloat);

    float           color[4];

    // Store depth value to alpha value (range 0.0 to 1.0)
    color[3] = depth / 255.0f;

    // Group depths (max of 10 colors in heatmap)
    unsigned int groupSize = 1;
    unsigned int dGroup = depth / groupSize;

    // Assign the color for this depth
    if(dGroup == 0) {        // BLACK
        color[0] = 0.0f;
        color[1] = 0.0f ;
        color[2] = 0.0f ;
    } else if(dGroup == 1) {
        color[0] = 0.0f;
        color[1] = 0.0f ;
        color[2] = 0.5f ;
    } else if(dGroup == 2) { // BLUE
        color[0] = 0.0f;
        color[1] = 0.0f ;
        color[2] = 1.0f ;
    } else if(dGroup == 3) {
        color[0] = 0.0f;
        color[1] = 0.5f ;
        color[2] = 1.0f ;
    } else if(dGroup == 4) { // CYAN
        color[0] = 0.0f;
        color[1] = 1.0f ;
        color[2] = 1.0f ;
    } else if(dGroup == 5) {
        color[0] = 0.0f;
        color[1] = 1.0f ;
        color[2] = 0.5f ;
    } else if(dGroup == 6) { // GREEN
        color[0] = 0.0f;
        color[1] = 1.0f ;
        color[2] = 0.0f ;
    } else if(dGroup == 7) {
        color[0] = 0.5f;
        color[1] = 1.0f ;
        color[2] = 0.0f ;
    } else if(dGroup == 8) { // YELLOW
        color[0] = 1.0f;
        color[1] = 1.0f ;
        color[2] = 0.0f ;
    } else if(dGroup == 9) {
        color[0] = 1.0f;
        color[1] = 0.5f ;
        color[2] = 0.0f ;
    } else if(dGroup == 10) { // RED
        color[0] = 1.0f;
        color[1] = 0.0f ;
        color[2] = 0.0f ;
    } else {    // depth > 10*groupSize => WHITE
        color[0] = 1.0f;
        color[1] = 1.0f ;
        color[2] = 1.0f ;
    }


    // Setup drawing state
    glUseProgram(m_materialBin[DEPTH_COMPLEXITY_MAT].m_material.m_shader.m_programObject);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(m_materialBin[DEPTH_COMPLEXITY_MAT].m_material.m_shader.m_positionLoc );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vtxStride, verts);


    glEnableVertexAttribArray(m_materialBin[DEPTH_COMPLEXITY_MAT].m_material.m_shader.m_positionLoc );
    glUniform4fv(m_materialBin[DEPTH_COMPLEXITY_MAT].m_material.m_shader.m_colorLoc, 1, &color[0] );

    // Set stencil comparision function. This will only fill in pixels on screen that equal this depth value
    glStencilFunc(GL_EQUAL, depth, 0xff); // Stencil buffer is 8bit, so mask = 0xff
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glEnable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    // Don't write to depth buffer
    glDepthMask(GL_FALSE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, index);

    // Turn back on depth test, and disable stencil test
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
}

void BaseTree::Draw(CCamera *camera) {
    // Setup Rendering options
    glViewport(0, 0, camera->GetViewPort().Width, camera->GetViewPort().Height);
    glScissor(0, 0, camera->GetViewPort().Width, camera->GetViewPort().Height);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0,0.0,0.0,1.00000000);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glClearDepthf(1.00000000);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // No need to clear color buffer, as all pixels will be overwritten due to skybox
    // Seems we do need to clear color buffer - strange things happen with blending otherwise (e.g. streetlights)
    if(g_depthComplexity) {
        glClearStencil(0);
        // No need to clear color buffer, as all pixels will be overwritten due to skybox
        glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    } else {
        // Clear color buffer for wireframe mode (looks better!)
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    }

    // depth complexity mode?
    if(g_depthComplexity) {
        glEnable(GL_STENCIL_TEST);
        // Ensure stencil test always passes
        glStencilFunc(GL_ALWAYS, 0, 0);
        // Increment the stencil buffer for every pixel. Keep existing value if depth test fails
        // Incr even if depth test fails, as frag shader will have been excercised at that stage
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        // No need to actually write the colors to framebuffer for DC mode
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    } else {
        glDisable(GL_STENCIL_TEST);       
    }

    // Streetlights only visible in HOME_MODE
    if(g_appMode == HOME_MODE) {
        m_materialBin[STREETLIGHT_MAT].m_material.m_state.m_visible = true;
    } else {
        m_materialBin[STREETLIGHT_MAT].m_material.m_state.m_visible = false;
    }

    // Check to see which quadTree are visible
    for(unsigned int i=0; i < m_quadTreeSize; i++) {
        for(unsigned int j=0; j < m_quadTreeSize; j++) {
            // AABB FRUSTRUM CULLING
            // Only draw if AABB is in view frustum
            if(camera->IsBoxInFrustum(m_quadTree[i][j].m_AABB.m_AABBinWS)) {
                // Update LeafTree Renderbatchs
                Add2RenderBatch(&m_quadTree[i][j]);

                // Draw AABBs
                m_quadTree[i][j].Draw(camera);
            }
        }
    }

    // Merge render batches
    for(unsigned int i=0; i<=m_numVBOs; i++) {
        for(int j=0; j<MAX_MATERIAL_TYPES; j++) {
            if(m_materialBin[j].m_numRenderBatches[i] > 0) {
                m_materialBin[j].MergeRenderBatches(i);
            }
        }
    }


    // Time to draw buildings
    // Foreach Vertex VBO
    for(unsigned int i=0; i<=m_numVBOs; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexVboId[i]);
        // Foreach Material
        for(int j=0; j<MAX_MATERIAL_TYPES; j++) {
            // Foreach RenderBatch
            if(m_materialBin[j].m_numRenderBatches[i] > 0) {
                // Draw each material
                m_materialBin[j].DrawMaterial(camera, i);
            }
        }
    }    

    // Draw Ground Plane
    //m_ground[0]->Draw(camera);

    // Draw cars
    if(m_numCars > 0) {
        m_cars.Draw(camera);
    }

    m_ground[0]->Draw(camera);


    // Draw skybox
    m_skybox[0]->Draw(camera);

    // Draw Route
    if(g_appMode == NAV_MODE) {
        for(unsigned int i = 0; i < m_routes.size(); i++) {
            m_routes[i]->Draw(camera);
        }
        // Draw camera object
        //m_car.Draw(camera);
        m_cockpit.Draw(camera);
    }

    // Roof lights won;t be visible in Nav mode
    if(g_appMode == HOME_MODE) {
        // Draw Billboards (Lights)
        // These are an exception to Material batch drawing, because each billboard needs an individual lookAt matrix
        //    sent to the shader (for translation to the correct spot), so these need to be drawn individually
        for(unsigned int i=0; i < m_quadTreeSize; i++) {
            for(unsigned int j=0; j<m_quadTreeSize; j++) {
                for(unsigned int k=0; k < m_quadTree[i][j].m_numBuildings; k++) {
                    if(m_quadTree[i][j].m_buildings[k].m_hasLight) {
                        m_quadTree[i][j].m_buildings[k].m_light.Draw(camera);
                        g_numLightDraws++;
                    }
                }
            }
        }
    }

    // Depth complexity mode?
    if(g_depthComplexity) {
        // Clear the back buffer to black
        glClearColor(0.0,0.0,0.0,0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        // Turn back on writing of color to FB
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

        // Test each level of depth complexity. Stencil buffer is 8bit, so only loop for 255 levels of depth
        for(int i=0; i<=MAX_OVERDRAW_DRAWN+1; i++) {
            DrawDepthComplexity(camera, i);
        }

        // Report overdraw metrics to console (enabled by 'b')
        if(g_reportDC) {
            // Compute the amount of overdraw
            // Can't read from the stencil buffer in ES2.0 (you can in OpenGL), so we
            // need to read the overdraw values from the framebufffer
            // since we *can* read from the frame buffer
            // Remember, we stored the overdraw values in the alpha value of stencil buffer
            unsigned int w = camera->GetViewPort().Width;
            unsigned int h = camera->GetViewPort().Height;
            GLubyte* fb = new GLubyte[w*h];
            // Fill stencilBuffer with alpha data from the stencil buffer
            glReadPixels(0, 0, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, fb);
            unsigned int num_pixels = w * h;
            unsigned int pixels_overdrawn = 0;
            unsigned int max_overdraw = 0;

            for(unsigned int i=0; i<w*h; i++) {
                GLubyte curr_pixel_od = fb[i];
                pixels_overdrawn += curr_pixel_od;
                if(curr_pixel_od > max_overdraw)
                    max_overdraw = curr_pixel_od;
            }
            float overdraw = pixels_overdrawn / (num_pixels*1.0f);
            printf("Overdraw = %f, max overdraw = %d\n", overdraw, max_overdraw);

            delete [] fb;
        } // g_reportDC
    }
}

void BaseTree::Update(CCamera *camera, float deltaTime) {
    // Update billboards (lights)
    for(unsigned int i=0; i < m_quadTreeSize; i++) {
        for(unsigned int j=0; j<m_quadTreeSize; j++) {
            for(unsigned int k=0; k < m_quadTree[i][j].m_numBuildings; k++) {
                if(m_quadTree[i][j].m_buildings[k].m_hasLight) {
                    m_quadTree[i][j].m_buildings[k].m_light.Update(camera, deltaTime);
                }

            }
        }
    }

    // Update cars
    m_cars.Update(camera, deltaTime);
}

