/// \file mothership.cpp
/// The Mothership
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\mothership.h"
#include "graphics\terrain.h"
#include "math3d\intersection.h"
#include "gamelogic\player.h"
#include "gamelogic/objectfactory.h"
#include "core/gamemain.h"
#include "graphics\light.h"

#include "utils\mmgr.h"

extern CTerrain *g_pCurrLevel;
extern CParticleManager g_ParticleManager;
extern CAudioManager g_AudioManager;
extern CPlayer	g_Player;
extern CObjectFactory   g_ObjectFactory;
extern CGameMain GameMain;
extern CQuadtree g_Quadtree;

extern float g_FrameTime;
extern float g_time;

extern CPositionalLight g_IonLight;

CMothership::CMothership()
{
   m_Type = OBJ_MOTHERSHIP;
	m_CurrOperation = INACTIVE;
   m_RadarType = RADAR_FRIENDLY;
	m_FrameCount = 0;
	m_ControlFrames = 0;
}

CMothership::~CMothership()
{
	if (m_ControlFrames)
	{
		delete [] m_ControlFrames;
		m_ControlFrames = 0;
	}
}

void CMothership::Setup()
{
   m_Type = OBJ_MOTHERSHIP;
	m_CurrOperation = INACTIVE;
   m_RadarType = RADAR_FRIENDLY;
	m_FrameCount = 0;
	m_ControlFrames = 0;  
   m_CurPosition.x = m_CurPosition.y = m_CurPosition.z = 0.0f;
   m_LastPosition.x = m_LastPosition.y = m_LastPosition.z = 0.0f;
	m_CurrOperation = INACTIVE;
   m_HitPoints = 500;
   m_pIonCannon = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_IONCANNON], 10); 
   m_pEngine = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_MOTHERSHIP], 10);
  
   CVector3f Pos(1000.0f, 1000.0f, 1000.0f);
   SetPosition(&Pos, 0);

   FindThrusterPositions();
   
   // Dirty hack
   //static bool doonce = true;
   //if(doonce)
   {
       // Setup thruster one
      m_pThruster1 = g_ParticleManager.ObtainSystem();
      if(m_pThruster1)
         g_ParticleManager.CreateSystem(m_pThruster1, m_Burner1, &m_Burner1, g_ParticleManager.MOTHERSHIP_THRUSTER, LOD_BOTH, 100.0f);

      // Setup thruster two
      m_pThruster2 = g_ParticleManager.ObtainSystem();
      if(m_pThruster2)
         g_ParticleManager.CreateSystem(m_pThruster2, m_Burner2, &m_Burner2, g_ParticleManager.MOTHERSHIP_THRUSTER, LOD_BOTH, 100.0f);
      //doonce = false;
   }
}

void CMothership::UpdateShootIon(void)
{
	CVector3f vec, pos, ppos;
	CObjFrame frame;
	float t;

   Color4f Ambient = {0.0f, 0.0f, 1.0f, 1.0f};
   Color4f Specular = {0.0f, 0.0f, 1.0f, 1.0f};
   Color4f Diffuse = {0.0f, 0.0f, 1.0f, 1.0f};
   
	// Do the right thing
	switch (m_IonStage)
	{
   case INIT:
      {
         GameMain.m_CurrentCommunication = GameMain.COM_INBOUND;
         m_Active = true;
         m_IonStage = ENTER;
         tempbool1 = true;
         tempbool2 = true;

         if(!(FSOUND_IsPlaying(m_pEngine->m_Channel)))
         {
            GetPosition(&m_CurPosition, 0);
            m_LastPosition = m_CurPosition;
   
            m_pEngine->Play3DAudio(100.0f, 10000.0f, 300);
         }
      }
      break;
	case ENTER:
      
		MoveForward(60.0f * g_FrameTime, 0);

		// Check if we reach target
		GetFrame(0)->RotateToLocalPoint(&vec, &m_Target);
		vec.y = 0.0f;
		if (vec.z < 1.0f)
		{
			m_Delay = 2.0f;
			m_IonStage = SHOOT;

         // Create the sound effect
         m_pIonCannon->Play3DAudio(100.0f, 10000.0f, 200);   
         CVector3f position;
         GetPosition(&position, 0);
         CVector3f velocity(0.0f, 0.0f, 0.0f);
         m_pIonCannon->UpdateSound(position, velocity);

         Diffuse.b = 0.0f;
         Specular.b = 0.0f;
         Ambient.b = 0.0f;
         g_IonLight.SetColors(&Ambient, &Diffuse, &Specular);
         g_IonLight.Enable();

         g_IonLight.SetPosition(&position);
		}
		break;
	case SHOOT:
		if (m_Delay > 0.0f)
      {
			m_Delay -= g_FrameTime;
         
         Diffuse.b = Ambient.b = Specular.b = 1.0f - (m_Delay * .5f);
         g_IonLight.SetColors(&Ambient, &Diffuse, &Specular);
         if(tempbool1)
         {
            GameMain.m_CurrentCommunication = GameMain.COM_IONFIRING;
            tempbool1 = false;
         }
      }
		else
		{
			CIonCannon * ion = g_ObjectFactory.CreateIonCannon();
			ion->SetUp(ION_CANNON, 1000, this);
			frame = *GetFrame(0);
			ion->SetFrame(&frame, 0);

			// Get a particle effect for the shot if one available
			m_IonBeam = g_ParticleManager.ObtainSystem();

			// If obtained a particle effect then we use it
			if(m_IonBeam != NULL)
			{
				// Create an effect for the shot
				g_ParticleManager.CreateSystem(m_IonBeam, *(ion->GetFrame(0)->GetPosition()),
				NULL, g_ParticleManager.ION_BEAM, LOD_BOTH);
			}  

			m_IonBeam->SetGravity(0.0f, -75.0f, 0.0f);
			m_IonBeam->m_ParticleLife = 2.0f; 

         Diffuse.b = Ambient.b = Specular.b = 1.0f;
         g_IonLight.SetColors(&Ambient, &Diffuse, &Specular);
			m_IonStage = WAIT;
		}
		break;
	case WAIT:
      {
         if(m_IonBeam)
         {
            
            /*
            Diffuse.b = Ambient.b = Specular.b = LightDelay;
            g_IonLight.SetColors(&Ambient, &Diffuse, &Specular);
            LightDelay -= g_FrameTime;
            */
            static int negspread = 0;
            int negspreadpersec = 5000;
            negspread = int(negspreadpersec * g_FrameTime);
            if(negspread >= 5000)
               negspread = 5000;
            m_IonBeam->SetSpread(1.0f, 0.0f, 0.0f, -(float)negspread, -20.0f, 0.0f, 0.0f);
         }
      }
		break;
	case LEAVE:
		if (m_Delay > 0.0f)
			m_Delay -= g_FrameTime;
		else
		{
         if(tempbool2)
         {
             GameMain.m_CurrentCommunication = GameMain.COM_LEAVE; 
             tempbool2 = false;
         }

			if (m_StartTime == -1.0f)
				m_StartTime = g_time;

			if (m_Speed < 150.0f)
				m_Speed += 50.0f * g_FrameTime;

			t = g_time - m_StartTime;
			if (t <= 3.0f)
			{
				CObjFrame frame = *GetFrame(0);
				CQuaternion a, b, q;
				QuatGetControlQuat(&a, &m_Q0, &m_Q1, &m_Q2);
				QuatGetControlQuat(&b, &m_Q1, &m_Q2, &m_Q3);
				QuatSquad(&q, &m_Q1, &m_Q2, &a, &b, t / 3.0f);
				frame.SetOrientation(&q);
				SetFrame(&frame, 0);
            
            Diffuse.b = Ambient.b = Specular.b = 1.0f - t * .33f;
            g_IonLight.SetColors(&Ambient, &Diffuse, &Specular);
			}
         else
            g_IonLight.Disable();

         
         
        
			MoveForward(m_Speed * g_FrameTime, 0);
			g_Player.GetPosition(&ppos, 0);
			GetPosition(&pos, 0);
			ppos.y = 0.0f;
			pos.y = 0.0f;
			if (DistanceSquare(&pos, &ppos) >= 360000.0f)
         {
				Die();
         
         }
		}
		break;
	};
}

void CMothership::InterpolateFrame(int index, float t)
{
	CObjFrame frame;

	ObjFrameInterpolate(&frame, m_ControlFrames, index, t);
	SetFrame(&frame, 0);
}

void CMothership::UpdateDropOff(void)
{
	CVector3f pos, ppos;

   if(!(FSOUND_IsPlaying(m_pEngine->m_Channel)))
   {
      GetPosition(&m_CurPosition, 0);
      m_LastPosition = m_CurPosition;
   
      m_pEngine->Play3DAudio(100.0f, 10000.0f, 300);
   }
   
	if (m_DropStart == -1.0f)
		m_DropStart = g_time;

	float t = g_time - m_DropStart;

	if (m_DropOffStage == ENTER)
	{
		if (t > m_SequenceLength)
		{
			m_DropOffStage = WAIT;
			return;
		}

		int segment = int(t / m_UpdatePeriod);
		t = (t - segment * m_UpdatePeriod) / m_UpdatePeriod;
		InterpolateFrame(segment + 1, t);
	}
	else if (m_DropOffStage == LEAVE)
	{
		if (m_Delay > 0.0f)
			m_Delay -= g_FrameTime;
		else
		{
			if (m_StartTime == -1.0f)
				m_StartTime = g_time;

			if (m_Speed < 150.0f)
				m_Speed += 50.0f * g_FrameTime;

			t = g_time - m_StartTime;
			if (t <= 3.0f)
			{
				CObjFrame frame = *GetFrame(0);
				CQuaternion a, b, q;
				QuatGetControlQuat(&a, &m_Q0, &m_Q1, &m_Q2);
				QuatGetControlQuat(&b, &m_Q1, &m_Q2, &m_Q3);
				QuatSquad(&q, &m_Q1, &m_Q2, &a, &b, t / 3.0f);
				frame.SetOrientation(&q);
				SetFrame(&frame, 0);
			}

			MoveForward(m_Speed * g_FrameTime, 0);
			g_Player.GetPosition(&ppos, 0);
			GetPosition(&pos, 0);
			ppos.y = 0.0f;
			pos.y = 0.0f;
			if (DistanceSquare(&pos, &ppos) >= 360000.0f)
				Die();
		}
	}
}

void CMothership::Update(void)
{
   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_pEngine->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;   

   float mag = Vec3fMagnitude(&Velocity);
   m_pEngine->AdjustVolume((unsigned int)abs(mag * 10));

	if (m_CurrOperation == INACTIVE)
		return;

	else if (m_CurrOperation == SHOOT_ION)
		UpdateShootIon();
	else if (m_CurrOperation == DROPOFF)
		UpdateDropOff();
   
   FindThrusterPositions();
   CGameObject::Update();
}

void CMothership::Collide(const CGameObject *obj)
{

}

void CMothership::Die(void)
{
	m_CurrOperation = INACTIVE;

   if(m_pThruster1)
      m_pThruster1->SetSystemAlive(false);
   if(m_pThruster2)
      m_pThruster2->SetSystemAlive(false);

   m_pEngine->StopAudio();
   RemoveFromQuadtree();
   
   delete [] m_ControlFrames;
   m_ControlFrames = 0;
}

void CMothership::Reset(void)
{
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the thrusters
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMothership::FindThrusterPositions()
{
   CVector3f Position;
   GetPosition(&Position, 0);

   CVector3f vector;
   CVector3f vector2;
   CVector3f right;
   CVector3f left;
   CVector3f up;
   m_RenderObj.GetFrame(0)->GetForward(&vector);
   m_RenderObj.GetFrame(0)->GetForward(&vector2);
   m_RenderObj.GetFrame(0)->GetRight(&right);
   left = right;
   m_RenderObj.GetFrame(0)->GetUp(&up);

   Vec3fScale(&vector, &vector, -63.0f);
   Vec3fScale(&right, &right, 10.0f);
   Vec3fScale(&left, &left, -10.0f);
   Vec3fScale(&up, &up, 5.00f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_Burner1, &vector, &right);
   Vec3fAdd(&m_Burner2, &vector, &left);
   Vec3fAdd(&m_Burner1, &m_Burner1, &up);
   Vec3fAdd(&m_Burner2, &m_Burner2, &up);
}

void CMothership::StartIonRun(const CVector3f *targetpos)
{
	if (m_CurrOperation != INACTIVE)
		return;

	m_CurrOperation = SHOOT_ION;
	m_StartTime = g_time;
	m_IonStage = INIT;
	m_Target = *targetpos;

	// Set initial position
	CObjFrame frame;
	CVector3f ppos;
	float angle;

	frame.SetPosition(targetpos);
	angle = (rand() % 360) * PI / 180.0f;
	frame.RotateLocalY(angle);
	frame.MoveUp(150.0f);
	frame.MoveForward(-800.0f);

	SetFrame(&frame, 0);
}

void CMothership::IonDone(void)
{
	if (m_CurrOperation == SHOOT_ION && m_IonStage == WAIT)
	{
		m_Delay = 5.0f;
		m_StartTime = -1.0f;
		m_Speed = 0.0f;
		m_IonStage = LEAVE;

		float angle;
		CObjFrame frame = *GetFrame(0);

		frame.GetOrientation(&m_Q1);
		m_Q0 = m_Q1;

		angle = (rand() % 180 - 90) * PI / 180.0f;
		frame.RotateLocalY(angle);

		frame.RotateLocalX(-PI / 6.0f);

		if (angle > 0.0f)
			frame.RotateLocalZ(-PI / 12.0f);
		else if (angle < 0.0f)
			frame.RotateLocalZ(PI / 12.0f);

		frame.GetOrientation(&m_Q2);
		m_Q3 = m_Q2;
	}
}

void CMothership::SetupDropOff(int framecount, CObjFrame *frames, float t)
{
	m_DropStart = -1.0f;
	m_FrameCount = framecount + 2;

	if (m_ControlFrames)
		delete [] m_ControlFrames;

	m_ControlFrames = new CObjFrame[m_FrameCount];
	for (int i = 1; i < m_FrameCount - 1; i++)
		m_ControlFrames[i] = frames[i - 1];

	m_ControlFrames[0] = m_ControlFrames[1];
	m_ControlFrames[m_FrameCount - 1] = m_ControlFrames[m_FrameCount - 2];
	m_SequenceLength = t;
	m_UpdatePeriod = t / (framecount - 1);

	SetFrame(&frames[0], 0);

	m_CurrOperation = DROPOFF;
	m_DropOffStage = ENTER;
}

void CMothership::DropOffDone(void)
{
	if (m_CurrOperation == DROPOFF)
	{
		m_Delay = 5.0f;
		m_StartTime = -1.0f;
		m_Speed = 0.0f;
		m_DropOffStage = LEAVE;

		CObjFrame frame = *GetFrame(0);

		frame.GetOrientation(&m_Q1);
		m_Q0 = m_Q1;

		frame.RotateLocalX(-PI / 6.0f);

		frame.GetOrientation(&m_Q2);
		m_Q3 = m_Q2;
	}
}
