/// \file gameobject.h
/// CGameObject class declaration and inline implementation
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/quadtree.h"
//#include "graphics/glinclude.h"
//#include "physics/physics.h"

enum // The list of all possible game object types
{
   // player
   OBJ_PLAYER,

   // enemies
   OBJ_MEDTANK,	  
	OBJ_HVYTANK, 
	OBJ_MSLETANK, 
	OBJ_KAMITANK, 
	OBJ_SPHERE, 
	OBJ_FIGHTER,
   OBJ_SCOUT,

   // buildings
   OBJ_ETURRET, 
	OBJ_FTURRET,
   OBJ_BUILDING,
   OBJ_BUILDING2,
   OBJ_BUILDING3,
   
   // doodads 
	OBJ_TREE,
   OBJ_TREE2,
   OBJ_TREE3,
	
   // powerups
   OBJ_SHDPWRUP,
   OBJ_MSLPACK,
   OBJ_TRKMSLPACK,
   OBJ_TRKMSLPWRUP,
   OBJ_MSLPWRUP,
   OBJ_PLASMAPWRUP,

   // rocks
   OBJ_ROCK1,
   OBJ_ROCK2, 
   OBJ_ROCK3,
   OBJ_ROCK4,
   OBJ_ROCK5, 
   OBJ_ROCK6,
   
   NUM_EDITOR_OBJECTS,

   // mothership
   OBJ_MOTHERSHIP,

   // Convoy
   OBJ_CONVOY,

   // shots
   OBJ_PLASMA,
	OBJ_MISSILE,
	OBJ_LASER,
   OBJ_IONCANNON, 
   OBJ_TRKMISSILE,
   OBJ_IONFLARE,

   OBJ_BOMB,
   OBJ_BOSS,
};

class CGameObject
{
   public:
   
      virtual void TakeDamage(float damage, CGameObject * guywhohitme) { }
   
      CGameObject(void)
		{  
			m_GuyNextToMe = 0;
			m_Next = 0;
			m_Prev = 0;
			m_CollisionAvoidanceListNext = 0;
			m_CollisionAvoidanceListPrev = 0;
			m_RigidBody = 0;
		}

		virtual ~CGameObject(void);
		void Unload();

      CGameObject(CGameObject& GameObject);

      enum RadarObjType{RADAR_PLAYER = 0, RADAR_ENEMY, RADAR_BUILDING, RADAR_FRIENDLY, RADAR_PROJECTILE, RADAR_ROCK};
      int m_RadarType;

      float m_MaxVelocity;
      float m_AccelerationTime;
      float m_TurnSpeedScale;

      //void Initialize(const unsigned int GeometryIndex, unsigned int* pMeshIndices,
      //                const unsigned int NumMeshes, const int TextureIndex, Sphere* pSphere);
      virtual void Initialize(unsigned int GeometryIndex, unsigned int NumParts,
                      CVector3f* pTranslations, int TextureIndex, const Sphere* pSphere,
                      AABB* pAABB, CQuadtree* pQuadtree);

      CQuadtreeRenderObj *GetRenderObj(void) { return &m_RenderObj; }
      void GetPosition(CVector3f* pPosition, unsigned int PartIndex);
      const Sphere *GetSphere(void) { return m_RenderObj.GetSphere(); }
      const CObjFrame *GetFrame(unsigned int PartIndex)
      {
         return m_RenderObj.GetFrame(PartIndex);
      }

      void SetFrame(const CObjFrame* pFrame, unsigned int PartIndex)
      {
         m_RenderObj.SetFrame(pFrame, PartIndex);
      }

      void RemoveFromQuadtree(void);
        
      void MoveRight(float Dist, unsigned int PartIndex);
      void MoveUp(float Dist, unsigned int PartIndex);
      void MoveForward(float Dist, unsigned int PartIndex);
      void RotateLocalX(float Angle, unsigned int PartIndex);
      void RotateLocalY(float Angle, unsigned int PartIndex);
	   void RotateLocalZ(float Angle, unsigned int PartIndex);
      
      /*
      void SetScaleZ(float Scale, unsigned int PartIndex)
      {
         m_OBB.z *= Scale;
         m_RenderObj.SetScaleZ(Scale, m_OBB.z, PartIndex);
      }*/ 

      virtual float GetAwakenPlayerRadius(void)
      {
         return 0;
      }

      virtual float GetAwakenEnemyRadius(void)
      {
         return 0;
      }

		void RBMoveForward(void);
		void RBMoveBackward(void);
		void RBMoveLeft(void);
		void RBMoveRight(void);
      void RBMoveUp(void);
		void RBTurnRight(float MouseX);
		virtual void GetVelocity(CVector3f *v);

		virtual void FollowTerrain(void);
		//virtual void FollowTerrain(float offset);

      virtual void Update(void);

		// Call this function when colliding with something, handle changes to this object 
		// due to the collision
		// eg. if a pair of objects objA and objB are colliding
		// (determined by collision module,) then the following calls are made:
		// objA->Collide(objB);
		// objB->Collide(objA);
		virtual void Collide(const CGameObject *obj) = 0;

		// Call this function when an object becomes inactive
		// Do appropriate cleanup such as remove from quat tree, destroy particle system, etc
		// but do not remove from obj list in game main
		// also don't return to factory here
		// that will be done in game main's update
		virtual void Die(void) = 0;

		// This function should reset a dead obj so that it can be used again
		virtual void Reset(void) = 0;

      void SetObjectPosition(float x, float y, float z);
      void SetPosition(const Point3f* pPosition, unsigned int PartIndex);

		int GetType(void) const
		{
			return m_Type;
		}

		bool IsActive(void)
		{
			return m_Active;
		}
		bool IsStaticObj(void)
		{
			return m_StaticObj;
		}

		CRigidBody *GetRigidBody(void)
		{
			return m_RigidBody;
		}

      inline OBB* GetOBB()
      {
         return &m_OBB;
      }
      
      virtual void Setup() { }
      virtual void Awaken() { }

   protected:
      CQuadtreeRenderObj m_RenderObj;
      OBB m_OBB;
		
		bool m_Active;		// Object active or not
		bool m_StaticObj;
		int  m_Type;		// Object type, use enums

		CRigidBody *m_RigidBody;	// Pointer to rigid body
											// If not a rigid body (eg. shots) then set it to 0
	public:
		CGameObject *m_Prev;			
		CGameObject *m_Next;	
      CGameObject *m_CollisionAvoidanceListNext;
      CGameObject *m_CollisionAvoidanceListPrev;
      CGameObject *m_GuyNextToMe;

      bool m_InMiniMap;
};





































