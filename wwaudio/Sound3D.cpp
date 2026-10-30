/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : WWAudio                                                      *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/WWAudio/Sound3D.cpp                          $*
 *                                                                                             *
 *                       Author:: Patrick Smith                                                *
 *                                                                                             *
 *                     $Modtime:: 12/10/01 12:40p                                             $*
 *                                                                                             *
 *                    $Revision:: 20                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


#include "sound3d.h"
#include "soundbuffer.h"
#include "wwaudio.h"
#include "soundscene.h"
#include "utils.h"
#include "soundchunkids.h"
#include "persistfactory.h"
#include "chunkio.h"
#include "sound3dhandle.h"
#include "systimer.h"


//////////////////////////////////////////////////////////////////////////////////
//
//	Static factories
//
//////////////////////////////////////////////////////////////////////////////////
SimplePersistFactoryClass<Sound3DClass, CHUNKID_SOUND3D> _Sound3DPersistFactory;

bool Sound3DClass::_UnknownDopplerFlag;


enum
{
	CHUNKID_VARIABLES			= 0x11090955,
	CHUNKID_BASE_CLASS
};

enum
{
	VARID_AUTO_CALC_VEL		= 0x01,
	VARID_CURR_VEL,
	VARID_XXX1,
	VARID_XXX2,
	VARID_MAX_VOL_RADIUS,
	VARID_IS_STATIC,
};



////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Sound3DClass
//
////////////////////////////////////////////////////////////////////////////////////////////////
Sound3DClass::Sound3DClass (void)
	: m_bAutoCalcVel (true),
	  m_CurrentVelocity (0, 0, 0),
	  m_Unknown148 (0, 0, 0),
	  m_MaxVolRadius (0),
	  m_LastUpdate (0),
	  m_Unknown160 (0),
	  m_Unknown164 (0),
	  m_Unknown168 (0),
	  m_Unknown16C (0),
	  m_IsStatic (false),
	  m_IsTransformInitted (false)
{
	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Sound3DClass
//
////////////////////////////////////////////////////////////////////////////////////////////////
Sound3DClass::Sound3DClass (const Sound3DClass &src)
	: m_bAutoCalcVel (true),
	  m_CurrentVelocity (0, 0, 0),
	  m_Unknown148 (0, 0, 0),
	  m_MaxVolRadius (0),
	  m_LastUpdate (0),
	  m_Unknown160 (0),
	  m_Unknown164 (0),
	  m_Unknown168 (0),
	  m_Unknown16C (0),
	  m_IsStatic (false),
	  m_IsTransformInitted (false),
	  AudibleSoundClass (src)
{
	(*this) = src;
	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	~Sound3DClass
//
////////////////////////////////////////////////////////////////////////////////////////////////
Sound3DClass::~Sound3DClass (void)
{
 	Free_Miles_Handle ();
	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	operator=
//
////////////////////////////////////////////////////////////////////////////////////////////////
const Sound3DClass &
Sound3DClass::operator= (const Sound3DClass &src)
{
	m_bAutoCalcVel			= src.m_bAutoCalcVel;
	m_CurrentVelocity		= src.m_CurrentVelocity;
	m_MaxVolRadius			= src.m_MaxVolRadius;
	m_IsStatic				= src.m_IsStatic;
	m_LastUpdate			= src.m_LastUpdate;

	// Call the base class
	AudibleSoundClass::operator= (src);
	return (*this);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Play
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool
Sound3DClass::Play (bool alloc_handle)
{
	// Record our first 'tick' if we just started playing
	if (m_State != STATE_PLAYING) {
		m_LastUpdate = TIMEGETTIME ();
	}

	// Allow the base class to process this call
	return AudibleSoundClass::Play (m_IsCulled == false);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	On_Frame_Update
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool
Sound3DClass::On_Frame_Update (unsigned int milliseconds)
{
	MMSLockClass lock;

	Matrix3D prev_tm = m_PrevTransform;

	if (m_bDirty && (m_PhysWrapper != NULL)) {
		m_Scene->Update_Sound (m_PhysWrapper);
		m_bDirty = false;
	}

	Apply_Auto_Position ();

	if (m_IsTransformInitted == false) {
		prev_tm = m_Transform;
	}

	if (_UnknownDopplerFlag && m_bAutoCalcVel && Get_Class_ID () != CLASSID_LISTENER) {
		Vector3 last_pos = prev_tm.Get_Translation ();
		Vector3 curr_pos = m_Transform.Get_Translation ();
		if (last_pos != curr_pos) {
			Vector3 curr_vel;
			float secs_since_last_update = (TIMEGETTIME () - m_LastUpdate);
			if (secs_since_last_update > 0) {
				curr_vel = ((curr_pos - last_pos) / secs_since_last_update);
			} else {
				curr_vel.Set (0, 0, 0);
			}

			Set_Velocity (curr_vel, secs_since_last_update);
		}
	}

	m_LastUpdate = TIMEGETTIME ();
	m_PrevTransform = m_Transform;

	if (m_SoundHandle != NULL) {
		Update_Edge_Volume ();
	}

	Update_Miles_Transform ();
	return AudibleSoundClass::On_Frame_Update ();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Update_Edge_Volume
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Update_Edge_Volume (void)
{
	if (m_DropOffRadius == 0.0F || As_SoundPseudo3DClass () != NULL) {
		return ;
	}

	//
	// Find the difference in the sound position and its listener's position
	//
	Vector3 sound_pos	= m_ListenerTransform.Get_Translation () - m_Transform.Get_Translation ();
	float distance		= sound_pos.Quick_Length ();

	const float FALLOFF_RANGE = 0.85F;

	//
	//	Are we close to the edge of the dropoff radius?
	//
	float falloff_start = (m_DropOffRadius * FALLOFF_RANGE);
	if (distance >= falloff_start && distance <= m_DropOffRadius) {

		//
		//	Normalize our distance from the edge
		//
		float percent	= (distance - falloff_start) / (m_DropOffRadius - falloff_start);
		percent			= WWMath::Clamp (1.0F - percent, 0.0F, 1.0F);

		//
		//	Update the sound volume
		//
		Internal_Set_Volume (m_RealVolume * percent);
	} else if (distance < falloff_start) {
		Internal_Set_Volume (m_RealVolume);
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_Transform
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Set_Transform (const Matrix3D &transform)
{
	if (transform == m_Transform) {
		return ;
	}


	// Update our internal transform
	m_Transform			= transform;
	Set_Dirty ();

	if (m_IsTransformInitted == false) {
		m_PrevTransform = transform;
		m_IsTransformInitted = true;
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_Listener_Transform
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Set_Listener_Transform (const Matrix3D &tm)
{
	//
	//	If the transform has changed, then cache the new transform
	//
	if (m_ListenerTransform != tm) {

		//
		//	E&B: track the listener's velocity for the Doppler effect (see Set_Velocity)
		//
		if (_UnknownDopplerFlag && m_bAutoCalcVel) {
			m_Unknown148 = tm.Get_Translation () - m_ListenerTransform.Get_Translation ();

			unsigned int now = TIMEGETTIME ();
			if (m_Unknown160 == 0) {
				m_Unknown160 = now;
			}

			float elapsed_ms = (now - m_Unknown160);
			elapsed_ms = WWMath::Max (elapsed_ms, 1.0F);
			m_Unknown148 /= elapsed_ms;
			m_Unknown160 = now;
		}

		m_ListenerTransform = tm;
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Update_Miles_Transform
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Update_Miles_Transform (void)
{
	MMSLockClass lock;
	//
	// Do we have a valid miles handle?
	//
	if (m_SoundHandle != NULL) {

		//
		//	Build a matrix to transform coordinates from world-space to listener-space
		//
		Matrix3D world_to_listener_tm;
		m_ListenerTransform.Get_Orthogonal_Inverse (world_to_listener_tm);

		//
		//	Transform the object's TM into "listener-space"
		//
		Matrix3D listener_space_tm = world_to_listener_tm * m_Transform;

		//
		// Pass the sound's position onto miles
		//
		Vector3 position = listener_space_tm.Get_Translation ();
		::AIL_set_3D_position (m_SoundHandle->Get_H3DSAMPLE (), -position.Y, position.Z, position.X);

		//
		// Pass the sound's orientation (facing) onto miles
		//
		Vector3 facing	= listener_space_tm.Get_X_Vector ();
		Vector3 up		= listener_space_tm.Get_Z_Vector ();

		::AIL_set_3D_orientation (m_SoundHandle->Get_H3DSAMPLE (),
										  -facing.Y,
										  facing.Z,
										  facing.X,
										  -up.Y,
										  up.Z,
										  up.X);
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_Position
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Set_Position (const Vector3 &position)
{
	MMSLockClass lock;

	//
	// Pass the sound's position onto miles
	//
	if (m_Transform.Get_Translation () != position)  {
		// Update our internal transform
		//
		// SKB: 4/13/01 - Confirmed to be OK by Pat Smith.
		//  Took Set_Transform() outside of condition because even if sound is
		//  not playing I need to be able to change it's position.
		//  I had a problem that sounds would never be added to the scene because
		//  their positions stayed at 0,0,0 even after this Set_Postion() call.
		m_PrevTransform = m_Transform;
		m_Transform.Set_Translation (position);
		Set_Dirty ();

		if (m_IsTransformInitted == false) {
			m_PrevTransform = m_Transform;
			m_IsTransformInitted = true;
		}
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_Velocity
//
////////////////////////////////////////////////////////////////////////////////////////////////
//
//	E&B: Set_Velocity's Doppler constants (names not known)
//
const float DOPPLER_SMOOTHING	= 0.85F;
const float DOPPLER_SPEED		= 3.4F;
const float DOPPLER_MS_SCALE	= 1.0F / 33.3F;

void
Sound3DClass::Set_Velocity (const Vector3 &velocity, float elapsed_ms)
{
	MMSLockClass lock;

	Vector3 prev_vel = m_CurrentVelocity;
	m_CurrentVelocity = velocity;
	Set_Dirty ();

	//
	//	E&B: Doppler effect, as a pitch change
	//
	if (m_SoundHandle != NULL) {

		//
		//	The direction from the sound to the listener, and the sound's
		// velocity relative to the listener's
		//
		Vector3 dir = m_ListenerTransform.Get_Translation () - m_Transform.Get_Translation ();
		m_CurrentVelocity -= m_Unknown148;
		m_CurrentVelocity.Normalize ();
		dir.Normalize ();
		float dot = Vector3::Dot_Product (dir, m_CurrentVelocity);

		m_CurrentVelocity = velocity - m_Unknown148;
		m_CurrentVelocity *= dot;
		Vector3 vel = (m_CurrentVelocity * DOPPLER_SMOOTHING) + (prev_vel * (1.0F - DOPPLER_SMOOTHING));
		m_CurrentVelocity = vel;

		vel *= m_Unknown16C;
		float speed = vel.Length ();
		if (speed > DOPPLER_SPEED) {
			speed = DOPPLER_SPEED;
		}
		if (dot > 0.0F) {
			speed = DOPPLER_SPEED - speed;
		} else {
			speed = DOPPLER_SPEED + speed;
		}
		if (speed == 0.0F) {
			speed = 1.0F;
		}
		float pitch = DOPPLER_SPEED / speed;

		//
		//	Change the pitch gradually
		//
		float curr_pitch = Get_Pitch_Factor ();
		float step = elapsed_ms * 0.05F;
		step *= DOPPLER_MS_SCALE;
		if (pitch > curr_pitch + step) {
			pitch = curr_pitch + step;
		} else if (pitch < curr_pitch - step) {
			pitch = curr_pitch - step;
		}

		if (pitch < m_Unknown164) {
			pitch = m_Unknown164;
		} else if (pitch > m_Unknown168) {
			pitch = m_Unknown168;
		}
		Set_Pitch_Factor (pitch);
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_DropOff_Radius
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Set_DropOff_Radius (float radius)
{
	MMSLockClass lock;

	m_DropOffRadius = radius;
	Set_Dirty ();

	// Pass attenuation settings onto miles
	if (m_SoundHandle != NULL) {
		::AIL_set_3D_sample_distances (	m_SoundHandle->Get_H3DSAMPLE (),
													m_DropOffRadius,
													(m_MaxVolRadius > 1.0F) ? m_MaxVolRadius : 1.0F);
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_Max_Vol_Radius
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Set_Max_Vol_Radius (float radius)
{
	MMSLockClass lock;
	m_MaxVolRadius = radius;
	Set_Dirty ();

	// Pass attenuation settings onto miles
	if (m_SoundHandle != NULL) {
		::AIL_set_3D_sample_distances (	m_SoundHandle->Get_H3DSAMPLE (),
													m_DropOffRadius,
													(m_MaxVolRadius > 1.0F) ? m_MaxVolRadius : 1.0F);
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Initialize_Miles_Handle
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Initialize_Miles_Handle (void)
{
	MMSLockClass lock;

	// If this sound is already playing, then update its
	// playing position to make sure we really should
	// be playing it... (it will free the miles handle if not)
	if (m_State == STATE_PLAYING) {
		Update_Play_Position ();
	}

	// Do we have a valid sample handle from miles?
	if (m_SoundHandle != NULL) {

		//
		// Pass the actual sound data onto the sample
		//
		m_SoundHandle->Initialize (m_Buffer);

		//
		// Record the total length of the sample in milliseconds...
		//
		m_SoundHandle->Get_Sample_MS_Position ((S32 *)&m_Length, NULL);

		//
		// Pass our cached settings onto miles
		//
		float real_volume = Determine_Real_Volume ();
		m_SoundHandle->Set_Sample_Volume (int(real_volume * 127.0F));
		m_SoundHandle->Set_Sample_Pan (int(m_Pan * 127.0F));
		m_SoundHandle->Set_Sample_Loop_Count (m_LoopCount);

		//
		// Pass attenuation settings onto miles
		//
		::AIL_set_3D_sample_distances (	m_SoundHandle->Get_H3DSAMPLE (),
													m_DropOffRadius,
													(m_MaxVolRadius > 1.0F) ? m_MaxVolRadius : 1.0F);


		//
		//	Assign the 3D effects level accordingly (for reverb, etc)
		//
		::AIL_set_3D_sample_effects_level (m_SoundHandle->Get_H3DSAMPLE (),
				WWAudioClass::Get_Instance ()->Get_Effects_Level ());

		//
		//	Pass the sound's position and orientation onto Miles
		//
		Update_Miles_Transform ();

		//
		//	Apply the pitch factor (if necessary)
		//
		if (m_PitchFactor != 1.0F) {
			Set_Pitch_Factor (m_PitchFactor);
		}

		// If this sound is already playing (and just now got a handle)
		// then make sure we start it.
		if (m_State == STATE_PLAYING) {
			m_SoundHandle->Start_Sample ();

			// Update the loop count based on the number of loops left
			m_SoundHandle->Set_Sample_Loop_Count (m_LoopsLeft);
		}

		// Seek to the position of the sound where we last left off.
		// For example, this sound could have gotten bumped due to a low priority,
		// but is now back and ready to resume at the position it would have been
		// at if it was never bumped.
		Seek (m_CurrentPosition);

		// Associate this object instance with the handle
		m_SoundHandle->Set_Sample_User_Data (INFO_OBJECT_PTR, (S32)this);
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Allocate_Miles_Handle
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Allocate_Miles_Handle (void)
{
	//MMSLockClass lock;

	//
	// If we need to, get a play-handle from the audio system
	//
	if (m_SoundHandle == NULL) {
		Set_Miles_Handle ((MILES_HANDLE)WWAudioClass::Get_Instance ()->Get_3D_Sample (*this));
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Add_To_Scene
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Add_To_Scene (bool start_playing)
{
	SoundSceneClass *scene = WWAudioClass::Get_Instance ()->Get_Sound_Scene ();
	if ((scene != NULL) && (m_Scene == NULL)) {

		// Determine what culling system this sound belongs to
		if (m_IsStatic) {
			scene->Add_Static_Sound (this, start_playing);
		} else {
			scene->Add_Sound (this, start_playing);
		}
		m_Scene = scene;
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Remove_From_Scene
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Remove_From_Scene (void)
{
	if (m_Scene != NULL) {

		// Determine what culling system this sound belongs to
		if (m_IsStatic) {
			m_Scene->Remove_Static_Sound (this);
		} else {
			m_Scene->Remove_Sound (this);
		}

		m_Scene = NULL;
		m_PhysWrapper = NULL;
	}

	return ;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	On_Loop_End
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::On_Loop_End (void)
{
	// Allow the base class to process this message
	AudibleSoundClass::On_Loop_End ();
	return ;
}

/////////////////////////////////////////////////////////////////////////////////
//
//	Get_Factory
//
/////////////////////////////////////////////////////////////////////////////////
const PersistFactoryClass &
Sound3DClass::Get_Factory (void) const
{
	return _Sound3DPersistFactory;
}


//////////////////////////////////////////////////////////////////////////////////
//
//	Save
//
//////////////////////////////////////////////////////////////////////////////////
bool
Sound3DClass::Save (ChunkSaveClass &csave)
{
	csave.Begin_Chunk (CHUNKID_BASE_CLASS);
		AudibleSoundClass::Save (csave);
	csave.End_Chunk ();

	csave.Begin_Chunk (CHUNKID_VARIABLES);

		WRITE_MICRO_CHUNK (csave, VARID_AUTO_CALC_VEL, m_bAutoCalcVel);
		WRITE_MICRO_CHUNK (csave, VARID_CURR_VEL, m_CurrentVelocity);

		WRITE_MICRO_CHUNK (csave, VARID_MAX_VOL_RADIUS, m_MaxVolRadius);
		WRITE_MICRO_CHUNK (csave, VARID_IS_STATIC, m_IsStatic);

	csave.End_Chunk ();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////
//
//	Load
//
//////////////////////////////////////////////////////////////////////////////////
bool
Sound3DClass::Load (ChunkLoadClass &cload)
{
	while (cload.Open_Chunk ()) {
		switch (cload.Cur_Chunk_ID ()) {

			case CHUNKID_BASE_CLASS:
				AudibleSoundClass::Load (cload);
				break;

			case CHUNKID_VARIABLES:
			{
				//
				//	Read all the variables from their micro-chunks
				//
				while (cload.Open_Micro_Chunk ()) {
					switch (cload.Cur_Micro_Chunk_ID ()) {

						READ_MICRO_CHUNK (cload, VARID_AUTO_CALC_VEL, m_bAutoCalcVel);
						READ_MICRO_CHUNK (cload, VARID_CURR_VEL, m_CurrentVelocity);
						READ_MICRO_CHUNK (cload, VARID_MAX_VOL_RADIUS, m_MaxVolRadius);
						READ_MICRO_CHUNK (cload, VARID_IS_STATIC, m_IsStatic);
					}

					cload.Close_Micro_Chunk ();
				}
			}
			break;
		}

		cload.Close_Chunk ();
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Set_Miles_Handle
//
////////////////////////////////////////////////////////////////////////////////////////////////
void
Sound3DClass::Set_Miles_Handle (MILES_HANDLE handle)
{
	//
	// Start fresh
	//
	Free_Miles_Handle ();

	//
	//	Is our data valid?
	//
	if (handle != INVALID_MILES_HANDLE && m_Buffer != NULL) {

		//
		//	Configure the sound handle
		//
		m_SoundHandle = new Sound3DHandleClass;
		m_SoundHandle->Set_Miles_Handle (handle);

		//
		//	Use this new handle
		//
		Initialize_Miles_Handle ();
	}

	return ;
}