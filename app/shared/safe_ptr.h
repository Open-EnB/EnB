// Earth & Beyond client (reconstruction): app\shared\safe_ptr.h. Notes at the end of the file.
#ifndef SAFE_PTR_H
#define SAFE_PTR_H

#ifndef NULL
#define NULL 0
#endif

class SafePtrTargetClass;

/*
**	A pointer that unlinks itself when its target is destroyed: every pointer to a target sits in the
**	target's list (m_next, and m_prev pointing at the slot that points to it).
*/
class SafePtrBaseClass // NAME 1: file safe_ptr.cpp, assert 'm_prev' in 0x4F5ED0; the base of the pointer
{
public:
	SafePtrBaseClass(void);
	~SafePtrBaseClass(void);

	void Set(SafePtrTargetClass *target); // NAME 1: 0x4F5ED0 unlinks the old target, links the new one
	SafePtrTargetClass *Get(void) const { return(m_target); } // NAME 1

protected:
	SafePtrBaseClass *m_next; // NAME 1: beside the client's m_prev
	SafePtrBaseClass **m_prev;
	SafePtrTargetClass *m_target; // NAME 1
};

/*
**	The pointed-to object: its destructor (0x4F5E10, table 0xAEC890: the destructor only) clears every
**	pointer still linked to it.
*/
class SafePtrTargetClass // NAME 1: vtable 0xAEC890, the head of the pointers' list at +4
{
public:
	SafePtrTargetClass(void) : m_head(NULL) {}
	virtual ~SafePtrTargetClass(void);

private:
	friend class SafePtrBaseClass;
	SafePtrBaseClass *m_head; // NAME 1
};

#endif

/*
**	Reconstruction notes
**
**	Layout from 0x4F5ED0: the pointer is { m_next +0, m_prev +4, m_target +8 } (0xC bytes), the
**	target { vtable +0, m_head +4 }. No caller outside safe_ptr.cpp calls these out of line, and no
**	constructor in the client stores the table 0xAEC890: the class is unused or inlined everywhere.
*/
