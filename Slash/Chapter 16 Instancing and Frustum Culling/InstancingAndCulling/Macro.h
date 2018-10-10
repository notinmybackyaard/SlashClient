#pragma once

#define NO_COPY(CLASSNAME)							\
	private:										\
	CLASSNAME(const CLASSNAME&);					\
	CLASSNAME& operator = (const CLASSNAME&);		


#define DECLARE_SINGLETON(CLASSNAME)				\
	NO_COPY(CLASSNAME)								\
	private:										\
	static CLASSNAME*	m_pInstance;				\
	public:											\
	static CLASSNAME*	GetInstance( void );		\
	static unsigned long DestroyInstance( void );	


#define IMPLEMENT_SINGLETON(CLASSNAME)				\
	CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
	if(NULL == m_pInstance) {						\
	m_pInstance = new CLASSNAME;					\
	}												\
	return m_pInstance;								\
	}												\
	unsigned long CLASSNAME::DestroyInstance( void ) {		\
	unsigned long dwRefCnt = 0;						\
	if(NULL != m_pInstance)	{						\
		m_pInstance->Release();						\
	if(0 == dwRefCnt)								\
	m_pInstance = NULL;								\
	}												\
	return dwRefCnt;								\
	}												



#define MSG_BOX(_message)								\
	MessageBox(NULL, _message, L"System Message", MB_OK)	\

#define TAGMSG_BOX(_tag, _message)						\
	MessageBox(NULL, _message, _tag, MB_OK)