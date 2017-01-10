#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"

#include "glTypes.h"

class CPlayer;
class CCamera;

typedef struct _VOLUME_INFO {
    GLvector3 Min;
    GLvector3 Max;
} VOLUME_INFO;

typedef struct _CALLBACK_FUNC {
    void* pFunction;     // function pointer
    void* pContext;     // Context to pass to the function
} CALLBACK_FUNC;

typedef struct _VIEWPORT {
    long X;
    long Y;
    long Width;
    long Height;
    float MinZ;
    float MaxZ;
} VIEWPORT;

struct PLANE {
    GLvector normal;
    float    distance;
};

class CCamera {
public:
    // Enumerator
    enum CAMERA_MODE {
        MODE_FPS                = 1,
        MODE_THIRDPERSON        = 2,
        MODE_SPACECRAFT         = 3,
        MODE_FORCE_32BIT        = 0x7FFFFFFF
    };

    // Constructors & Destructors for this class
    CCamera( const CCamera* pCamera);
    CCamera();
    virtual ~CCamera();

    // Public functions
    void SetFOV(float FOV) {m_fFOV = FOV; m_bProjDirty = true;}
    void SetViewport(long Left, long Top, long Width, long Height, float NearClip, float FarClip);

    const GLmatrix& GetProjMatrix();
    float GetFOV() const {return m_fFOV;}
    float GetNearClip() const {return m_fNearClip;}
    float GetFarClip() const {return m_fFarClip;}
    const VIEWPORT& GetViewPort() const {return m_Viewport;}
    CPlayer* GetPlayer() const {return m_pPlayer;}
    const GLvector3& GetPosition() const {return m_vecPos;}
    const GLvector3& GetLook() const {return m_vecLook;}
    const GLvector3& GetUp() const {return m_vecUp;}
    const GLvector3& GetRight() const {return m_vecRight;}
    const GLmatrix& GetViewMatrix() ;
    void SetVolumeInfo(const VOLUME_INFO& Volume);
    const VOLUME_INFO& GetVolumeInfo() const;
    void SetLookAt(const GLvector& vecLookAt);

    // public virtual functions
    virtual void AttachToPlayer(CPlayer* pPlayer);
    virtual void DetachFromPlayer();
    virtual void SetPosition(const GLvector3& Position) {m_vecPos = Position; m_bViewDirty = true;}
    virtual void Move(const GLvector3& vecShift) {m_vecPos = m_vecPos + vecShift; m_bViewDirty = true;}
    virtual void Rotate(float x, float y, float z) {}
    virtual void Update(float TimeScale, float Lag) {}
    virtual void SetCameraDetails(const CCamera* pCamera) {}
    virtual CAMERA_MODE GetCameraMode() const = 0;

    void UpdateFrustumPlanes();
    bool IsBoxInFrustum(GLbbox aabb);
protected:
    CPlayer* m_pPlayer; // The player object we are attached to
    VOLUME_INFO m_Volume;   // Stores info about cameras collision volume
    GLmatrix m_mtxView; // Cached view matrix
    GLmatrix m_mtxProj;    // Cached projection matrix
    bool m_bViewDirty;  // View matrix dirty
    bool m_bProjDirty;  // Proj matrix dirty?

    // Perspective projection parameters
    float m_fFOV;   // FOV angle
    float m_fNearClip;  // Near cip plane distance
    float m_fFarClip;   // Far clip plane distance
    VIEWPORT m_Viewport;    // Viewport details into which we are rendering

    // Cameras current position & orientation
    GLvector3 m_vecPos; // Camera position
    GLvector3 m_vecUp;  // Camera up vector
    GLvector3 m_vecLook; // Camera look vector
    GLvector3 m_vecRight; // Camera Right vector

    // Frustum Planes
    PLANE m_frustum[6];
};

class CCamfirstPerson : public CCamera {
public:
    // Constructors
    CCamfirstPerson( const CCamera* pCamera);
    CCamfirstPerson();

    // Public base class overrides
    CAMERA_MODE GetCameraMode() const { return MODE_FPS;}
    void Rotate(float x, float y, float z);
    void SetCameraDetails(const CCamera* pCamera);
};

class CCamSpaceCraft : public CCamera {
public:
    // Constructors
    CCamSpaceCraft(const CCamera* pCamera);
    CCamSpaceCraft();

    // Public functions
    CAMERA_MODE GetCameraMode() const {return MODE_SPACECRAFT;}
    void Rotate(float x, float y, float z);
    void SetCameraDetails(const CCamera* pCamera);
};

class CCamthirdPerson : public CCamera {
public:
    // Constructors
    CCamthirdPerson(const CCamera* pCamera);
    CCamthirdPerson();

    // Public functions
    CAMERA_MODE GetCameraMode() const {return MODE_THIRDPERSON;}
    void Move(const GLvector3& vecShift) {};
    void Rotate(float x, float y, float z) {};
    void Update(float TimeScale, float Lag);
    void SetCameraDetails(const CCamera* pCamera);
    void SetLookAt(const GLvector& vecLookAt);
};

class CPlayer {
public:
    // Enumerator
    enum DIRECTION {
        DIR_FORWARD = 1,
        DIR_BACKWARD = 2,
        DIR_LEFT = 4,
        DIR_RIGHT = 8,
        DIR_UP = 16,
        DIR_DOWN = 32,

        DIR_FORCE_32BIT = 0x7FFFFFFF
    };

    // Constructor & Destructor
    CPlayer();
    virtual ~CPlayer();

    // Public Functions
    bool SetCameraMode(unsigned long Mode);
    void Update(float TimeScale);

    //void SetthirdPersonObject( CObject* pObject) {m_pthirdPersonObject = pObject;}
    void setFriction(float Friction) {m_fFriction = Friction;}
    void SetGravity(const GLvector3& Gravity) {m_vecGravity = Gravity;}
    void SetMaxVelocityXZ(float MaxVelocity) {m_fMaxVelocityXZ = MaxVelocity;}
    void SetMaxVelocityY(float MaxVelocity) {m_fMaxVelocityY = MaxVelocity;}
    void SetVelocity(const GLvector3& Velocity) {m_vecVelocity = Velocity;}
    void SetCamLag(float CamLag) {m_fCameraLag = CamLag;}
    void SetCamOffset(const GLvector3& Offset);
    void SetVolumeInfo(const VOLUME_INFO& Volume);
    const VOLUME_INFO& GetVolumeInfo() const;

    CCamera* GetCamera() const {return m_pCamera;}
    const GLvector3& GetVelocity() const {return m_vecVelocity;}
    const GLvector3& GetCamOffset() const {return m_vecCamOffset;}
    const GLvector3& GetPosition() const {return m_vecPos;}
    const GLvector3& GetLook() const {return m_vecLook;}
    const GLvector3& GetUp() const {return m_vecUp;}
    const GLvector3& GetRight() const {return m_vecRight;}
    float GetYaw() const {return m_fYaw;}
    float GetPitch() const {return m_fPitch;}
    float GetRoll() const {return m_fRoll;}
    void SetPosition( GLvector3& Position) { Move(Position - m_vecPos, false); }
    void Move(unsigned long Direction, float Distance, bool Velocity = false);
    void Move(GLvector3 vecShift, bool Velocity = false );
    void Rotate(float x, float y, float z);
    void SetLookAt(const GLvector& vecLookAt);
private:
    // Private variables
    CCamera* m_pCamera;     // Current camera object
    VOLUME_INFO m_Volume;   // Stores info about players collision volume
    unsigned long m_CameraMode; // Stored camera mode

    // Players position and orientation values
    GLvector3 m_vecPos;        // Player Position
    GLvector3 m_vecUp;         // Up vector
    GLvector3 m_vecRight;       // Right Vector
    GLvector3 m_vecLook;       // Look vector
    GLvector3 m_vecCamOffset;  // Camera offset
    float m_fPitch;             // Player pitch
    float m_fRoll;              // Player roll
    float m_fYaw;               // Player yaw

    // Force / Player Update vars
    GLvector3 m_vecVelocity;       // Movement velocity vector
    GLvector3 m_vecGravity;        // Gravity vector
    float m_fMaxVelocityXZ;         // Max camera velocity in XZ plane
    float m_fMaxVelocityY;          // Max camera velocity in Y axis
    float m_fFriction;              // Amount of friction causing camera to slow
    float m_fCameraLag;             // Amount of camera loag in seconds (0 to disable)

    // Stored collision callbacks
    CALLBACK_FUNC m_pUpdatePlayer[255]; // Array of 'UpdatePlayer' callbacks
    CALLBACK_FUNC m_pUpdateCamera[255]; // Array of 'UpdateCamera' callbacks
    unsigned short m_nUpdatePlayerCount; // Number of 'UpdatePlayer' callbacks stored
    unsigned short m_nUpdateCameraCount; // Number of 'UpdateCamera' callbacks stored
};


#endif // CAMERA_H
