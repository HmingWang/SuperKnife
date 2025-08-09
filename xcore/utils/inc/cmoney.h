#pragma once 
#include "xstring.h"

class CMoney
{
#define LPCSTR const char *
    public:

        // 初始为0
        CMoney();

        // 用整数初始化
        CMoney(int l);

        // 用浮点数初始化
        CMoney(double dbl);

        // 用两个int 初始化
        CMoney(int iHight, unsigned int iLow);

        // 用CMoney数初始化
        CMoney(const CMoney& m);

        // 用字串初始化
        CMoney(LPCSTR p); 

        ~CMoney(){}
    	
    	// 转化为浮点数
    	double to_double()  const ;

    	// 转化为无小数点字串（输入参数为小数位个数）
    	String to_string_no_dot(int iScale = 2)  const ;

    	// 转化为带小数点的字串
    	String to_string()  const ;

        static CMoney from_string(LPCSTR ptr, int iScale=0);

    	const CMoney& operator=(const CMoney& m);

    	friend CMoney operator+(const CMoney& m1, const CMoney& m2);
    	friend CMoney operator-(const CMoney& m1, const CMoney& m2);
    	friend int operator>(const CMoney& m1, const CMoney& m2);
    	friend int operator<(const CMoney& m1, const CMoney& m2);
    	friend int operator==(const CMoney& m1, const CMoney& m2);
    	friend int operator>=(const CMoney& m1, const CMoney& m2);
    	friend int operator<=(const CMoney& m1, const CMoney& m2);
    	friend int operator!=(const CMoney& m1, const CMoney& m2);
    	friend int mod9(const CMoney& m);

    private:

    	int m_iHight;
    	unsigned int m_uiLow;
};

inline int mod9(const CMoney& ml)
{
	return (ml.m_iHight % 9 + ml.m_uiLow % 9) % 9;
}

inline int operator==(const CMoney& m1, const CMoney& m2) 
{
	return m1.m_iHight == m2.m_iHight && m1.m_uiLow == m2.m_uiLow;
}

inline int operator > (const CMoney& m1, const CMoney& m2) 
{
	if(m1.m_iHight != m2.m_iHight)
    {
		return m1.m_iHight > m2.m_iHight;
    }
	else
    {
		return m1.m_uiLow > m2.m_uiLow;
    }
}

inline int operator < (const CMoney& m1, const CMoney& m2) 
{
	if(m1.m_iHight != m2.m_iHight)
    {
		return m1.m_iHight < m2.m_iHight;
    }
	else
    {
		return m1.m_uiLow < m2.m_uiLow;
    }
}

inline int operator <= (const CMoney& m1, const CMoney& m2) 
{
	return (m1 < m2) || (m1 == m2);
}

inline int operator >= (const CMoney& m1, const CMoney& m2) 
{
	return (m1 > m2) || (m1 == m2);
}

inline int operator != (const CMoney& m1, const CMoney& m2) 
{
	return !(m1 == m2);
}