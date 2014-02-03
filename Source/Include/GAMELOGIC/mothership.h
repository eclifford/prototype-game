/// \file mothership.h
/// The mothership
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/friendly.h"
#include "sound/audio.h"
#include "graphics/particlemanager.h"


class CMothership: public CFriendly
{
	private:
		enum {INACTIVE = 0, SHOOT_ION, DROPOFF};
		enum {ENTER = 0, SHOOT, WAIT, LEAVE, INIT};

		int		m_CurrOperation;
		float		m_StartTime;

		int			m_IonStage;
		CVector3f	m_Target;
		float			m_Delay;
		float			m_Speed;
		CQuaternion m_Q0, m_Q1, m_Q2, m_Q3;

      bool tempbool1;
      bool tempbool2;

		int			m_DropOffStage;
		int			m_FrameCount;
		CObjFrame	*m_ControlFrames;
		float			m_SequenceLength;
		float			m_DropStart;
		float			m_UpdatePeriod;

		void UpdateShootIon(void);
		void UpdateDropOff(void);
		void InterpolateFrame(int index, float t);
	public:
      CMothership(CMothership& Mothership) : CFriendly(Mothership)
      {

      }

		CMothership();

		~CMothership();

      CAudio * m_pEngine;
      CAudio * m_pIonCannon;

      CParticleSystem * m_pThruster1;
      CParticleSystem * m_pThruster2;
      CParticleSystem * m_IonBeam;

      CVector3f m_Burner1;
      CVector3f m_Burner2;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void TakeDamage(float damage, CGameObject * guywhohitme) {}
      void FindThrusterPositions();

		void StartIonRun(const CVector3f *targetpos);
		void IonDone(void);

		void SetupDropOff(int framecount, CObjFrame *frames, float t);
		void DropOffDone(void);
		float GetDropStart(void) const
		{
			return m_DropStart;
		}
		bool InOperation(void) const
		{
			return !m_CurrOperation == INACTIVE;
		}
};