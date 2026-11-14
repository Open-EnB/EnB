// Earth & Beyond client (reconstruction): app\shared\safe_ptr.cpp. Notes at the end of the file.
#include "safe_ptr.h"
#include "g_assert.h"



SafePtrTargetClass::~SafePtrTargetClass(void)
{
	while (m_head != NULL) {
		m_head->Set(NULL);
	}
}


SafePtrBaseClass::SafePtrBaseClass(void) : m_next(NULL), m_prev(NULL), m_target(NULL)
{
}


SafePtrBaseClass::~SafePtrBaseClass(void)
{
	Set(NULL);
}


void SafePtrBaseClass::Set(SafePtrTargetClass *target)
{
	if (m_target != NULL) {


		G_ASSERT(m_prev);
		*m_prev = m_next;
		if (m_next != NULL) {
			m_next->m_prev = m_prev;
		}
		m_next = NULL;
		m_prev = NULL;
		m_target = NULL;
	}

	if (target != NULL) {
		m_target = target;
		m_next = target->m_head;
		m_prev = &target->m_head;
		if (m_next != NULL) {
			m_next->m_prev = &m_next;
		}
		target->m_head = this;
	}
}


/*
**	Reconstruction notes
**
**	Client 0x4F5E10-0x4F5F3C: the object opens after the previous one's guarded <string> initialiser
**	(0x4F5DE0) and ends before shipcust_desc.cpp's PermStringInitClass static (0x4F5F60), which opens
**	the next object. 0x4F5E10 the target's destructor, 0x4F5E40 its scalar deleting destructor,
**	0x4F5E90 / 0x4F5EB0 the pointer's constructor and destructor, 0x4F5ED0 Set (assert 'm_prev',
**	line 31). The Vector3 / Vector2 constructors 0x4F6160 / 0x4F6190 after it are shipcust_desc.cpp's
**	(its static initialiser 0x4F5FB0 calls them).
*/
