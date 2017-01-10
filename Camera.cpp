#include "Camera.h"
#include "Macro.h"

CCamera::CCamera() {
    // Reset / Clear all required values
    m_pPlayer = NULL;
    m_vecRight = glVector(1.0f, 0.0f, 0.0f);
    m_vecUp = glVector(0.0f, 1.0f, 0.0f);
    m_vecLook = glVector(0.0f, 0.0f, 1.0f);
    m_vecPos = glVector(0.0f, 0.0f, 0.0f);

    m_fFOV = 60.0f;
    m_fNearClip = 1.0f;
    m_fFarClip = 100.0f;
    m_Viewport.X = 0;
    m_Viewport.Y = 0;
    m_Viewport.Width = 640;
    m_Viewport.Height = 480;
    m_Viewport.MinZ = 0.0f;
    m_Viewport.MaxZ = 1.0f;

    // Set matrices to identity
    m_mtxView = glMatrixIdentity();
    m_mtxProj = glMatrixIdentity();
}

CCamera::CCamera(const CCamera* pCamera) {
    // Reset / Clear all required values
    m_pPlayer = NULL;
    m_vecRight = glVector(1.0f, 0.0f, 0.0f);
    m_vecUp = glVector(0.0f, 1.0f, 0.0f);
    m_vecLook = glVector(0.0f, 0.0f, 1.0f);
    m_vecPos = glVector(0.0f, 0.0f, 0.0f);

    m_fFOV = 60.0f;
    m_fNearClip = 1.0f;
    m_fFarClip = 100.0f;
    m_Viewport.X = 0;
    m_Viewport.Y = 0;
    m_Viewport.Width = 640;
    m_Viewport.Height = 480;
    m_Viewport.MinZ = 0.0f;
    m_Viewport.MaxZ = 1.0f;

    // Set matrices to identity
    m_mtxView = glMatrixIdentity();
    m_mtxProj = glMatrixIdentity();
}

CCamera::~CCamera() {
}

void CCamera::UpdateFrustumPlanes() {
    GLmatrix vMat = GetViewMatrix();
    GLmatrix pMat = GetProjMatrix();
    GLmatrix vpMat = glMatrixMultiply(vMat, pMat);

    // Left clip plane
    m_frustum[0].normal.x = -(vpMat.elements[0][3] + vpMat.elements[0][0]);
    m_frustum[0].normal.y = -(vpMat.elements[1][3] + vpMat.elements[1][0]);
    m_frustum[0].normal.z = -(vpMat.elements[2][3] + vpMat.elements[2][0]);
    m_frustum[0].distance = -(vpMat.elements[3][3] + vpMat.elements[3][0]);
    // Right clip plane
    m_frustum[1].normal.x = -(vpMat.elements[0][3] - vpMat.elements[0][0]);
    m_frustum[1].normal.y = -(vpMat.elements[1][3] - vpMat.elements[1][0]);
    m_frustum[1].normal.z = -(vpMat.elements[2][3] - vpMat.elements[2][0]);
    m_frustum[1].distance = -(vpMat.elements[3][3] - vpMat.elements[3][0]);
    // Top clip plane
    m_frustum[2].normal.x = -(vpMat.elements[0][3] - vpMat.elements[0][1]);
    m_frustum[2].normal.y = -(vpMat.elements[1][3] - vpMat.elements[1][1]);
    m_frustum[2].normal.z = -(vpMat.elements[2][3] - vpMat.elements[2][1]);
    m_frustum[2].distance = -(vpMat.elements[3][3] - vpMat.elements[3][1]);
    // Bottom clip plane
    m_frustum[3].normal.x = -(vpMat.elements[0][3] + vpMat.elements[0][1]);
    m_frustum[3].normal.y = -(vpMat.elements[1][3] + vpMat.elements[1][1]);
    m_frustum[3].normal.z = -(vpMat.elements[2][3] + vpMat.elements[2][1]);
    m_frustum[3].distance = -(vpMat.elements[3][3] + vpMat.elements[3][1]);
    // Near clip plane
    m_frustum[4].normal.x = -(vpMat.elements[0][2]);
    m_frustum[4].normal.y = -(vpMat.elements[1][2]);
    m_frustum[4].normal.z = -(vpMat.elements[2][2]);
    m_frustum[4].distance = -(vpMat.elements[3][2]);
    // Far clip plane
    m_frustum[5].normal.x = -(vpMat.elements[0][3] - vpMat.elements[0][2]);
    m_frustum[5].normal.y = -(vpMat.elements[1][3] - vpMat.elements[1][2]);
    m_frustum[5].normal.z = -(vpMat.elements[2][3] - vpMat.elements[2][2]);
    m_frustum[5].distance = -(vpMat.elements[3][3] - vpMat.elements[3][2]);

    // Normalize the planes
    for(int i=0; i < 6; i++) {
        // Get magnitude of vector
        float denom = 1.0f / glVectorLength(m_frustum[i].normal);
        m_frustum[i].normal.x *= denom;
        m_frustum[i].normal.y *= denom;
        m_frustum[i].normal.z *= denom;
        m_frustum[i].distance *= denom;
    }
}

bool CCamera::IsBoxInFrustum(GLbbox aabb) {
    GLvector bMin = aabb.min;
    GLvector bMax = aabb.max;
    GLvector nearPoint;

    for(int i=0; i<6; i++) {
        if(m_frustum[i].normal.x > 0.0f) {
            if(m_frustum[i].normal.y > 0.0f) {
                if(m_frustum[i].normal.z > 0.0f) {
                    nearPoint.x = bMin.x;
                    nearPoint.y = bMin.y;
                    nearPoint.z = bMin.z;
                } else {
                    nearPoint.x = bMin.x;
                    nearPoint.y = bMin.y;
                    nearPoint.z = bMax.z;
                }
            } else {
                if(m_frustum[i].normal.z > 0.0f) {
                    nearPoint.x = bMin.x;
                    nearPoint.y = bMax.y;
                    nearPoint.z = bMin.z;
                } else {
                    nearPoint.x = bMin.x;
                    nearPoint.y = bMax.y;
                    nearPoint.z = bMax.z;
                }
            }
        } else {   //   normal.x <= 0.0
            if(m_frustum[i].normal.y > 0.0f) {
                if(m_frustum[i].normal.z > 0.0f) {
                    nearPoint.x = bMax.x;
                    nearPoint.y = bMin.y;
                    nearPoint.z = bMin.z;
                } else {
                    nearPoint.x = bMax.x;
                    nearPoint.y = bMin.y;
                    nearPoint.z = bMax.z;
                }
            } else {    // normal.y <= 0.0
                if(m_frustum[i].normal.z > 0.0f) {
                    nearPoint.x = bMax.x;
                    nearPoint.y = bMax.y;
                    nearPoint.z = bMin.z;
                } else {
                    nearPoint.x = bMax.x;
                    nearPoint.y = bMax.y;
                    nearPoint.z = bMax.z;
                }
            }
        }

        if(glVectorDotProduct(m_frustum[i].normal, nearPoint) + m_frustum[i].distance > 0)
            return false;
    } // for

    return true;
}

void CCamera::AttachToPlayer(CPlayer *pPlayer) {
    m_pPlayer = pPlayer;
}

void CCamera::DetachFromPlayer() {
    m_pPlayer = NULL;
}

void CCamera::SetViewport(long Left, long Top, long Width, long Height, float NearClip, float FarClip) {
    // Set viewport sizes
    m_Viewport.X = Left;
    m_Viewport.Y = Top;
    m_Viewport.Width = Width;
    m_Viewport.Height = Height;
    m_Viewport.MinZ = 0.0f;
    m_Viewport.MaxZ = 1.0f;
    m_fNearClip = NearClip;
    m_fFarClip = FarClip;
    m_bProjDirty = true;
}

const GLmatrix& CCamera::GetProjMatrix() {
    // Only update matrix if something has changed
    if(m_bProjDirty) {
        float fAspect = (float) m_Viewport.Width / (float) m_Viewport.Height;

        // Set the perspective projection matrix
        //D3DMatrixPerspectiveFovLH(&m_mtxProj, D3DXToRadian(m_fFOV / 2.0), fAspect, m_fNearCLip, m_fFarClip);  // pOut, fovy, aspect, zn, zf
        // Following code emulates D3DMatrixPerspectiveFovLH - PH
        float fFOVRadians = m_fFOV * (PI / 180.0f);
        // cot(i) = 1 / tan(i)
        float yScale = 1 / (tan(fFOVRadians / 2));
        float xScale = yScale / fAspect;
        m_mtxProj.elements[0][0] = xScale;
        m_mtxProj.elements[0][1] = 0;
        m_mtxProj.elements[0][2] = 0;
        m_mtxProj.elements[0][3] = 0;

        m_mtxProj.elements[1][0] = 0;
        m_mtxProj.elements[1][1] = yScale;
        m_mtxProj.elements[1][2] = 0;
        m_mtxProj.elements[1][3] = 0;

        m_mtxProj.elements[2][0] = 0;
        m_mtxProj.elements[2][1] = 0;
        m_mtxProj.elements[2][2] = m_fFarClip/(m_fFarClip - m_fNearClip);
        m_mtxProj.elements[2][3] = 1.0f;

        m_mtxProj.elements[3][0] = 0;
        m_mtxProj.elements[3][1] = 0;
        m_mtxProj.elements[3][2] = -m_fNearClip*m_fFarClip/(m_fFarClip - m_fNearClip);
        m_mtxProj.elements[3][3] = 0;

        // Proj Matrix has been updated
        m_bProjDirty = false;
    }

    // Return the projection matrix
    return m_mtxProj;
}

const GLmatrix& CCamera::GetViewMatrix() {
    // Only update matrix if something has changed
    if(m_bViewDirty) {
        m_vecLook = glVectorNormalize(m_vecLook);
        m_vecRight = glVectorCrossProduct(m_vecUp, m_vecLook);
        m_vecRight = glVectorNormalize(m_vecRight);
        m_vecUp = glVectorCrossProduct(m_vecLook, m_vecRight);
        m_vecUp = glVectorNormalize(m_vecUp);

        // Set view matrix values
        m_mtxView.elements[0][0] = m_vecRight.x;
        m_mtxView.elements[0][1] = m_vecUp.x;
        m_mtxView.elements[0][2] = m_vecLook.x;
        m_mtxView.elements[0][3] = 0.0f;

        m_mtxView.elements[1][0] = m_vecRight.y;
        m_mtxView.elements[1][1] = m_vecUp.y;
        m_mtxView.elements[1][2] = m_vecLook.y;
        m_mtxView.elements[1][3] = 0.0f;

        m_mtxView.elements[2][0] = m_vecRight.z;
        m_mtxView.elements[2][1] = m_vecUp.z;
        m_mtxView.elements[2][2] = m_vecLook.z;
        m_mtxView.elements[2][3] = 0.0f;

        m_mtxView.elements[3][0] = - glVectorDotProduct(m_vecPos, m_vecRight);       
        m_mtxView.elements[3][1] = - glVectorDotProduct(m_vecPos, m_vecUp);
        m_mtxView.elements[3][2] = - glVectorDotProduct(m_vecPos, m_vecLook);
        m_mtxView.elements[3][3] = 1.0f;

        // View Matrix has been updated
        m_bViewDirty = false;
    }

    // Return the view matrix
    return m_mtxView;
}

void CCamera::SetVolumeInfo(const VOLUME_INFO &Volume) {
    m_Volume = Volume;
}

const VOLUME_INFO& CCamera::GetVolumeInfo() const {
    return m_Volume;
}

CCamfirstPerson::CCamfirstPerson( const CCamera* pCamera) {
    // Update the camera from the camera passed
    SetCameraDetails(pCamera);
}

void CCamfirstPerson::SetCameraDetails(const CCamera* pCamera) {
    // Validate parameters
    if(!pCamera) return ;

    // Reset/Clear all required values
    m_vecPos = pCamera->GetPosition();
    m_vecRight = pCamera->GetRight();
    m_vecLook = pCamera->GetLook();
    m_vecUp = pCamera->GetUp();
    m_fFOV = pCamera->GetFOV();
    m_fNearClip = pCamera->GetNearClip();
    m_fFarClip = pCamera->GetFarClip();
    m_Viewport = pCamera->GetViewPort();
    m_Volume = pCamera->GetVolumeInfo();

    // If we are switching buliding from a spacecraft style cam
    if(pCamera->GetCameraMode() == MODE_SPACECRAFT) {
        // Flatten out the vectors
        m_vecUp = glVector(0.0f, 1.0f, 0.0f);
        m_vecRight.y = 0.0f;
        m_vecLook.y = 0.0f;

        // Finally, normalize them
        m_vecRight = glVectorNormalize(m_vecRight);
        m_vecLook = glVectorNormalize(m_vecLook);
    }

    m_bViewDirty = true;
    m_bProjDirty = true;
}

void CCamfirstPerson::Rotate(float x, float y, float z) {
    GLmatrix mtxRotate = glMatrixIdentity();

    if(x != 0) {
        // Build rotation matrix
        mtxRotate = glMatrixRotate(mtxRotate, x, m_vecRight.x, m_vecRight.y, m_vecRight.z);
        m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);
        m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);
        m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);
    }
    if(y != 0) {
        mtxRotate = glMatrixRotate(mtxRotate, y, m_pPlayer->GetUp().x, m_pPlayer->GetUp().y, m_pPlayer->GetUp().z);
        m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);
        m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);
        m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);
    }
    if(z != 0){
        mtxRotate = glMatrixRotate(mtxRotate, z, m_pPlayer->GetLook().x, m_pPlayer->GetLook().y, m_pPlayer->GetLook().z);
        // Adjust camrea position
        m_vecPos -= m_pPlayer->GetPosition();
        m_vecPos = glMatrixTransformPoint(mtxRotate, m_vecPos);
        m_vecPos += m_pPlayer->GetPosition();

        // Update our vectors
        m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);      // PH Does this work???
        m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);
        m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);    // PH does this work???
    }

    // Set view matrix as dirty
    m_bViewDirty = true;
}

CCamSpaceCraft::CCamSpaceCraft( const CCamera* pCamera) {
    // Update the camera from the camera passed
    SetCameraDetails(pCamera);
}

void CCamSpaceCraft::SetCameraDetails(const CCamera* pCamera) {
    // Validate parameters
    if(!pCamera) return ;

    // Reset/Clear all required values
    m_vecPos = pCamera->GetPosition();
    m_vecRight = pCamera->GetRight();
    m_vecLook = pCamera->GetLook();
    m_vecUp = pCamera->GetUp();
    m_fFOV = pCamera->GetFOV();
    m_fNearClip = pCamera->GetNearClip();
    m_fFarClip = pCamera->GetFarClip();
    m_Viewport = pCamera->GetViewPort();
    m_Volume = pCamera->GetVolumeInfo();

    // Rebuild both matrices
    m_bViewDirty = true;
    m_bProjDirty = true;
}

void CCamSpaceCraft::Rotate(float x, float y, float z) {
    GLmatrix mtxRotate;

    if(x != 0) {
        // Build rotation matrix
        mtxRotate = glMatrixRotate(mtxRotate, x, m_pPlayer->GetRight().x, m_pPlayer->GetRight().y, m_pPlayer->GetRight().z);        // PH Does this work???
        m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);      // PH Does this work???
        m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);    // PH does this work???

        // Rotate about player
        m_vecPos -= m_pPlayer->GetPosition();
        m_vecPos = glMatrixTransformPoint(mtxRotate, m_vecPos);
        m_vecPos += m_pPlayer->GetPosition();
    }
    if(y != 0) {
        mtxRotate = glMatrixRotate(mtxRotate, y, m_pPlayer->GetUp().x, m_pPlayer->GetUp().y, m_pPlayer->GetUp().z);        // PH Does this work???
        m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);      // PH Does this work???
        m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);    // PH does this work???

        // Adjust position
        m_vecPos -= m_pPlayer->GetPosition();
        m_vecPos = glMatrixTransformPoint(mtxRotate, m_vecPos);
        m_vecPos += m_pPlayer->GetPosition();
    }
    if(z != 0){
        mtxRotate = glMatrixRotate(mtxRotate, z, m_pPlayer->GetLook().x, m_pPlayer->GetLook().y, m_pPlayer->GetLook().z);        // PH Does this work???
        m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);    // PH does this work???
        m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);    // PH does this work???

        // Adjust  position
        m_vecPos -= m_pPlayer->GetPosition();
        m_vecPos = glMatrixTransformPoint(mtxRotate, m_vecPos);
        m_vecPos += m_pPlayer->GetPosition();
    }

    // Set view matrix as dirty
    m_bViewDirty = true;
}

CCamthirdPerson::CCamthirdPerson( const CCamera* pCamera) {
    // Update the camera from the camera passed
    SetCameraDetails(pCamera);
}

void CCamthirdPerson::SetCameraDetails(const CCamera* pCamera) {
    // Validate parameters
    if(!pCamera) return ;

    // Reset/Clear all required values
    m_vecPos = pCamera->GetPosition();
    m_vecRight = pCamera->GetRight();
    m_vecLook = pCamera->GetLook();
    m_vecUp = pCamera->GetUp();
    m_fFOV = pCamera->GetFOV();
    m_fNearClip = pCamera->GetNearClip();
    m_fFarClip = pCamera->GetFarClip();
    m_Viewport = pCamera->GetViewPort();
    m_Volume = pCamera->GetVolumeInfo();

    // If we are switching buliding from a spacecraft style cam
    if(pCamera->GetCameraMode() == MODE_SPACECRAFT) {
        // Flatten out the vectors
        m_vecUp = glVector(0.0f, 1.0f, 0.0f);
        m_vecRight.y = 0.0f;
        m_vecLook.y = 0.0f;

        // Finally, normalize them
        m_vecRight = glVectorNormalize(m_vecRight);
        m_vecLook = glVectorNormalize(m_vecLook);
    }

    m_bViewDirty = true;
    m_bProjDirty = true;
}

void CCamthirdPerson::Update(float TimeScale, float Lag) {
    GLmatrix mtxRotate;
    GLvector3 vecOffset, vecPosition, vecDir;

    float fTimeScale = 1.0f, Length = 0.0f;
    if(Lag != 0.0f)
        fTimeScale = TimeScale * (1.0f / Lag);

    // Rotate out offset vector to its position behind the player
    mtxRotate = glMatrixIdentity();
    GLvector3 vecRight = m_pPlayer->GetRight();
    GLvector3 vecUp = m_pPlayer->GetUp();
    GLvector3 vecLook = m_pPlayer->GetLook();
    mtxRotate.elements[0][0] = vecRight.x;
    mtxRotate.elements[0][1] = vecRight.y;
    mtxRotate.elements[0][2] = vecRight.z;
    mtxRotate.elements[1][0] = vecUp.x;
    mtxRotate.elements[1][1] = vecUp.y;
    mtxRotate.elements[1][2] = vecUp.z;
    mtxRotate.elements[2][0] = vecLook.x;
    mtxRotate.elements[2][1] = vecLook.y;
    mtxRotate.elements[2][2] = vecLook.z;

    // Calculate our rotated offset vector
    vecOffset = glMatrixTransformPoint(mtxRotate, m_pPlayer->GetCamOffset());

    // vecOffset now contains info to calculate where our camrea position SHOULD be
   // vecPosition = m_pPlayer->GetPosition() + vecOffset;
    vecPosition = m_pPlayer->GetPosition();
    vecPosition += vecOffset;

    // Allow for lag
    vecDir = vecPosition - m_vecPos;
    Length = glVectorLength(vecDir);
    vecDir = glVectorNormalize(vecDir);

    // Move based on camre lag
    float Distance = Length * fTimeScale;
    if(Distance > Length)
        Distance = Length;

    // If we only have a short way to travel, move all the way
    if(Length < 0.01f)
        Distance = Length;

    // Update our camera
    if(Distance > 0) {
        m_vecPos += vecDir * Distance;

        // Ensure our camrea is looking at the axis origin
        SetLookAt(m_pPlayer->GetPosition());

        // Our view matrix parameters have been updated
        m_bViewDirty = true;
    }
}

void CCamthirdPerson::SetLookAt(const GLvector &vecLookAt) {
    //GLmatrix Matrix;

    // Generate a look at matrix
    //D3DMatrixLookAtLH(&Matrix, &m_vecPos, &vecLookAt, &m_pPlayer->GetUp());
    // This code emulates D3DMatrixLookAtLH - PH
    GLvector3 xaxis, yaxis, zaxis;
    zaxis = vecLookAt ;
    zaxis -= m_vecPos;
    zaxis = glVectorNormalize(zaxis);
    xaxis = glVectorCrossProduct( m_pPlayer->GetUp(), zaxis);
    xaxis = glVectorNormalize(xaxis);
    yaxis = glVectorCrossProduct(zaxis, xaxis);

    m_vecRight = xaxis;
    m_vecUp = yaxis;
    m_vecLook = zaxis;

    // Set view matrix as dirty
    m_bViewDirty = true;
}

void CCamera::SetLookAt(const GLvector3& vecLookAt) {
    GLmatrix Matrix;

    // Generate a look at matrix
    //D3DMatrixLookAtLH(&Matrix, &m_vecPos, &vecLookAt, &m_pPlayer->GetUp());   // pOut, pEye, pAt, pUp
    // This code emulates D3DMatrixLookAtLH - PH
    GLvector3 xaxis, yaxis, zaxis;
    zaxis = vecLookAt ;
    zaxis -= m_vecPos;
    zaxis = glVectorNormalize(zaxis);
    xaxis = glVectorCrossProduct( m_pPlayer->GetUp(), zaxis);
    xaxis = glVectorNormalize(xaxis);
    yaxis = glVectorCrossProduct(zaxis, xaxis);
    Matrix.elements[0][0] = xaxis.x;
    Matrix.elements[0][1] = xaxis.y;
    Matrix.elements[0][2] = xaxis.z;
    Matrix.elements[0][3] = - glVectorDotProduct(xaxis, m_vecPos);
    Matrix.elements[1][0] = yaxis.x;
    Matrix.elements[1][1] = yaxis.y;
    Matrix.elements[1][2] = yaxis.z;
    Matrix.elements[1][3] = - glVectorDotProduct(yaxis, m_vecPos);
    Matrix.elements[2][0] = zaxis.x;
    Matrix.elements[2][1] = zaxis.y;
    Matrix.elements[2][2] = zaxis.z;
    Matrix.elements[2][3] = - glVectorDotProduct(zaxis, m_vecPos);
    Matrix.elements[3][0] = 0;
    Matrix.elements[3][1] = 0;
    Matrix.elements[3][2] = 0;
    Matrix.elements[3][3] = 1;


    // Extract the vectors
    m_vecRight = glVector(Matrix.elements[0][0], Matrix.elements[0][1], Matrix.elements[0][2]);
    // Force up vector to remain 'upright'
    //m_vecUp    = glVector(Matrix.elements[1][0], Matrix.elements[1][1], Matrix.elements[1][2]);
    m_vecUp = glVector(0.0, 1.0, 0.0);
    m_vecLook  = glVector(Matrix.elements[2][0], Matrix.elements[2][1], Matrix.elements[2][2]);

    // Set view matrix as dirty
    m_bViewDirty = true;
}



CPlayer::CPlayer() {
    // Clear any required variables
    m_pCamera = NULL;
    //m_pthirdPersonObject = NULL;
    m_CameraMode = 0;

    // Initially no call-backs added to either array
    m_nUpdatePlayerCount = 0;
    m_nUpdateCameraCount = 0;

    // Players position & orientation (independent of camera)
    m_vecPos = glVector(0.0f, 0.0f, 0.0f);
    m_vecRight = glVector(1.0f, 0.0f, 0.0f);
    m_vecUp = glVector(0.0f, 1.0f, 0.0f);
    m_vecLook = glVector(0.0f, 0.0f, 1.0f);

    // Camera offset valudes (from the players origin)
    m_vecCamOffset = glVector(0.0f, 10.0f, 0.0f);
    m_fCameraLag = 0.0f;

    // The following force releated values are used in conjunction with 'Update' only
    m_vecVelocity = glVector(0.0f, 0.0f, 0.0f);
    m_vecGravity = glVector(0.0f, 0.0f, 0.0f);
    m_fMaxVelocityXZ = 125.0f;
    m_fMaxVelocityY = 125.0f;
    m_fFriction = 250.0f;

    // Set default bounding volume so it has no volume
    m_Volume.Min = glVector(0.0f, 0.0f, 0.0f);
    m_Volume.Max = glVector(0.0f, 0.0f, 0.0f);


}

CPlayer::~CPlayer() {
    // Release any allocated memory
    if(m_pCamera) delete m_pCamera;

    // Clear required values
    m_pCamera = NULL;
    //m_pthirdPersonObject = NULL;
}

bool CPlayer::SetCameraMode(unsigned long Mode) {
    CCamera* pNewCamera = NULL;

    // Check for a no-op
    if(m_pCamera && m_CameraMode == Mode) return true;

    // Which mode are we switching into
    switch(Mode) {
    case CCamera::MODE_FPS:
        if(!(pNewCamera = new CCamfirstPerson(m_pCamera))) return false;
        break;
    case CCamera::MODE_THIRDPERSON:
        if(!(pNewCamera = new CCamthirdPerson(m_pCamera))) return false;
        break;
    case CCamera::MODE_SPACECRAFT:
        if(!(pNewCamera = new CCamSpaceCraft(m_pCamera))) return false;
        break;
    }

    // Validate
    if(!pNewCamera) return false;

    // If our old camrea mode was SPACECRAFT, we need to sort out some things
    if(m_CameraMode == CCamera::MODE_SPACECRAFT) {
        // Flatten out the vectors
        m_vecUp = glVector(0.0f, 1.0f, 0.0f);
        m_vecRight.y = 0.0f;
        m_vecLook.y = 0.0f;

        // Now normalize them
        m_vecRight = glVectorNormalize(m_vecRight);
        m_vecLook = glVectorNormalize(m_vecLook);

        // Reset pitch/yaw/roll vaules
        m_fPitch = 0.0f;
        m_fRoll = 0.0f;
        // Degrees = (180/pi) * radians
        m_fYaw = (180/PI) * (acosf(glVectorDotProduct(glVector(0.0f, 0.0f, 1.0f), m_vecLook)));
        //m_fYaw = degrees(acosf(glVectorDotProduct(&glVector(0.0f, 0.0f, 1.0f), &m_vecLook)));
        if(m_vecLook.x < 0.0f) m_fYaw = -m_fYaw;
    } else if (m_pCamera && Mode == CCamera::MODE_SPACECRAFT) {
        m_vecRight = m_pCamera->GetUp();
        m_vecLook = m_pCamera->GetLook();
        m_vecUp = m_pCamera->GetUp();
    }
    // Store the new mode
    m_CameraMode = Mode;

    // Attach the new camera to 'this' player object
    pNewCamera->AttachToPlayer( this);

    // destroy the old camera and replace with our new one
    if(m_pCamera) delete m_pCamera;
    m_pCamera = pNewCamera;

    // Success!
    return true;
}

void CPlayer::SetCamOffset(const GLvector3 &Offset) {
    m_vecCamOffset = Offset;

    if(!m_pCamera) return;
    m_pCamera->SetPosition(m_vecPos + Offset);
}

void CPlayer::SetLookAt(const GLvector3& vecLookAt) {
    GLmatrix Matrix;

    // Generate a look at matrix
    //D3DMatrixLookAtLH(&Matrix, &m_vecPos, &vecLookAt, &m_pPlayer->GetUp());   // pOut, pEye, pAt, pUp
    // This code emulates D3DMatrixLookAtLH - PH
    // Need to update to use [0][0] as first element
    GLvector3 xaxis, yaxis, zaxis;
    zaxis = vecLookAt ;
    zaxis -= m_vecPos;
    zaxis = glVectorNormalize(zaxis);
    xaxis = glVectorCrossProduct(zaxis, GetUp());
    xaxis = glVectorNormalize(xaxis);
    yaxis = glVectorCrossProduct(xaxis, zaxis);
    Matrix.elements[0][0] = xaxis.x;
    Matrix.elements[0][1] = xaxis.y;
    Matrix.elements[0][2] = xaxis.z;
    Matrix.elements[0][3] = - glVectorDotProduct(xaxis, m_vecPos);
    Matrix.elements[1][0] = yaxis.x;
    Matrix.elements[1][1] = yaxis.y;
    Matrix.elements[1][2] = yaxis.z;
    Matrix.elements[1][3] = - glVectorDotProduct(yaxis, m_vecPos);
    Matrix.elements[2][0] = zaxis.x;
    Matrix.elements[2][1] = zaxis.y;
    Matrix.elements[2][2] = zaxis.z;
    Matrix.elements[2][3] = - glVectorDotProduct(zaxis, m_vecPos);
    Matrix.elements[3][0] = 0;
    Matrix.elements[3][1] = 0;
    Matrix.elements[3][2] = 0;
    Matrix.elements[3][3] = 1;


    // Extract the vectors
    m_vecRight = glVector(Matrix.elements[0][0], Matrix.elements[1][0], Matrix.elements[2][0]);
    // Force up vector to remain 'upright'
    //m_vecUp    = glVector(Matrix.elements[0][1], Matrix.elements[1][1], Matrix.elements[2][1]);
    m_vecUp = glVector(0.0, 1.0, 0.0);
    m_vecLook  = glVector(Matrix.elements[0][2], Matrix.elements[1][2], Matrix.elements[2][2]);

    // Set view matrix as dirty
    //m_pCamera->m_bViewDirty = true;
}

void CPlayer::Rotate(float x, float y, float z) {
    GLmatrix mtxRotate = glMatrixIdentity();

    // Validate requirements
    if(!m_pCamera) return;

    // retrieve camera mode
    CCamera::CAMERA_MODE Mode = m_pCamera->GetCameraMode();

    if(Mode == CCamera::MODE_FPS || Mode == CCamera::MODE_THIRDPERSON) {
        // update & clamp pitch / roll / yaw
        if(x) {     // PITCH
            // Make sure we dont overstep our pitch boundaries
            m_fPitch += x;
            if(m_fPitch > 89.0f) {
                x -= (m_fPitch - 89.0f);
                m_fPitch = 89.0f;
            }
            if(m_fPitch < -89.0f) {
                x -= (m_fPitch + 89.0f);
                m_fPitch = -89.0f;
            }
        }
        if(y) {     // YAW
            // Ensure yaw (in degrees) wraps around between 0 and 360
            m_fYaw += y;
            if(m_fYaw > 360.0f)
                m_fYaw -= 360.0f;
            if(m_fYaw < 0.0f)
                m_fYaw += 360.0f;
        }
        if(z) {     // ROLL
            // Make sure we don't overstep our roll boundaries
            m_fRoll += z;
            if(m_fRoll > 20.0f) {
                z -= (m_fRoll - 20.0f);
                m_fRoll = 20.0f;
            }
            if(m_fRoll < -20.0f) {
                z -= (m_fRoll + 20.0f);
                m_fRoll = -20.0f;
            }
        }
        // Allow camera to rotate prior to updating our axis
        m_pCamera->Rotate(x, y, z);
        // Now rotate our axis
        if(y) {
            // Build rotation matrix
            mtxRotate = glMatrixRotate(mtxRotate, y, m_vecUp.x, m_vecUp.y, m_vecUp.z);        // PH Does this work???
            m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);      // PH Does this work???
            m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);    // PH does this work???
        }
    }   // End if MODE_FIRSTPERSON or MODE_thirdPerson
    else if(Mode == CCamera::MODE_SPACECRAFT) {
        // Allow camrea to rotate prior to updating our axis
        m_pCamera->Rotate(x, y, z);

        if(x != 0) {
            mtxRotate = glMatrixRotate(mtxRotate, x, m_vecRight.x, m_vecRight.y, m_vecRight.z);        // PH Does this work???
            m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);      // PH Does this work???
            m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);    // PH does this work???
        }
        if(y != 0) {
            mtxRotate = glMatrixRotate(mtxRotate, y, m_vecUp.x, m_vecUp.y, m_vecUp.z);        // PH Does this work???
            m_vecLook = glMatrixTransformPoint(mtxRotate, m_vecLook);      // PH Does this work???
            m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);    // PH does this work???
        }
        if(z != 0) {
            mtxRotate = glMatrixRotate(mtxRotate, z, m_vecLook.x, m_vecLook.y, m_vecLook.z);        // PH Does this work???
            m_vecUp = glMatrixTransformPoint(mtxRotate, m_vecUp);      // PH Does this work???
            m_vecRight = glMatrixTransformPoint(mtxRotate, m_vecRight);    // PH does this work???
        }
    }   // End spaceCraft

    // Vector Regeneration
    m_vecLook = glVectorNormalize(m_vecLook);
    m_vecRight = glVectorCrossProduct(m_vecUp, m_vecLook);
    m_vecRight = glVectorNormalize(m_vecRight);
    m_vecUp = glVectorCrossProduct(m_vecLook, m_vecRight);
    m_vecUp = glVectorNormalize(m_vecUp);
    // PH - Force up to be up :)
    m_vecUp = glVector(0.0f, 1.0f, 0.0f);
}

void CPlayer::Move(unsigned long Direction, float Distance, bool Velocity) {
    GLvector3 vecShift = glVector(0.0, 0.0, 0.0);

    // Which direction are we moving
    if(Direction & DIR_FORWARD)
        vecShift += m_vecLook * Distance;
    if(Direction & DIR_BACKWARD)
        vecShift -= m_vecLook * Distance;
    if(Direction & DIR_RIGHT)
        vecShift += m_vecRight * Distance;
    if(Direction & DIR_LEFT)
        vecShift -= m_vecRight * Distance;
    if(Direction & DIR_UP)
        vecShift += m_vecUp * Distance;
    if(Direction & DIR_DOWN)
        vecShift -= m_vecUp * Distance;

    // update camera vectors
    if(Direction) Move(vecShift, Velocity);
}

void CPlayer::Move( GLvector3 vecShift, bool Velocity) {
    // Update velocity or actual position?
    if(Velocity) {
        m_vecVelocity += vecShift;
    } else {
        m_vecPos += vecShift;
        m_pCamera->Move(vecShift);
    }
}

void CPlayer::Update(float TimeScale) {
    // Add on our gravity vector
    m_vecVelocity += m_vecGravity * TimeScale;

    // Clamp the XZ velocity to our max velocity vector
    float Length = sqrtf(m_vecVelocity.x * m_vecVelocity.x + m_vecVelocity.z * m_vecVelocity.z);
    if(Length > m_fMaxVelocityXZ) {
        m_vecVelocity.x *= (m_fMaxVelocityXZ / Length);
        m_vecVelocity.z *= (m_fMaxVelocityXZ / Length);
    }

    // Clamp the Y velocity to our max velocity vector
    Length = sqrtf(m_vecVelocity.y * m_vecVelocity.y);
    if(Length > m_fMaxVelocityY) {
        m_vecVelocity.y *= (m_fMaxVelocityY / Length);
    }

    // Move our player (will also move camera if required)
    Move(m_vecVelocity * TimeScale, false);

    // Let our camera update if required
    m_pCamera->Update(TimeScale, m_fCameraLag);

    // Calculate the reverse of the velocity direction
    GLvector3 vecDec = m_vecVelocity * -1.0;

    // Normalize deceleration vector
    vecDec = glVectorNormalize(vecDec);

    // Retrieve the actual velocity length
    Length = glVectorLength(m_vecVelocity);

    // Calculate total decleration based on friction values
    float Dec = (m_fFriction * TimeScale);
    if(Dec > Length) Dec = Length;

    // Apply the friction force
    m_vecVelocity += vecDec * Dec;
}


