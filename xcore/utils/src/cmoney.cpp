#include "cmoney.h"
#include <memory.h>
#include "exceptions.h"

#define ERR_WRONGTYPE_S2N "CVariant：无法将字符数据赋值到数字变量"
#define ERR_WRONGTYPE_S2M "CVariant：无法将字符数据赋值到MONEY变量"
#define ERR_WRONGTYPE_N2S "CVariant：无法将数字数据赋值到字符变量"
#define ERR_WRONGTYPE_NULL "CVariant：无法将NULL赋值到变量"
#define ERR_WRONGTYPE_M2S "CVariant：无法将MONEY数据赋值到字符变量"
#define ERR_WRONGTYPE_M2N "CVariant：无法将MONEY数据赋值到数字变量"
#define ERR_WRONGTYPE_UNDEFINE "CVariant：类型未定义"
#define ERR_INVALID_MONEY_STR  "CMoney: Invalid string format"
#define ERR_INVALID_MONEY_SCALE "CMoney: 格式错误，SCALE只能为2或4"

#define MAX_MONEY_VALUE "9223372036854775807"
#define MIN_MONEY_VALUE "9223372036854775808" // 负数

void ThrowException(LPCSTR strErrMsg)
{
	throw TransException(strErrMsg);
}

const CMoney G_mMoneyList[19]{
	CMoney((int)0, 0x01),
	CMoney((int)0, 0x0A),
	CMoney((int)0, 0x64),
	CMoney((int)0, 0x3E8),
	CMoney((int)0, 0x2710),
	CMoney((int)0, 0x186A0),
	CMoney((int)0, 0xF4240),
	CMoney((int)0, 0x989680),
	CMoney((int)0, 0x5F5E100),
	CMoney((int)0, 0x3B9ACA00),
	CMoney(0x2, 0x540BE400),
	CMoney(0x17, 0x4876E800),
	CMoney(0xE8, (unsigned int)0xD4A51000),
	CMoney(0x918, 0x4E72A000),
	CMoney(0x5AF3, 0x107A4000),
	CMoney(0x38D7E, (unsigned int)0xA4C68000),
	CMoney(0x2386F2, 0x6FC10000),
	CMoney(0x1634578, 0x5D8A0000),
	CMoney(0xDE0B6B3, (unsigned int)0xA7640000),
};

const CMoney G_m19(0xDE0B6B3, 0xA7640000);
const CMoney G_m18(0x1634578, 0x5D8A0000);
const CMoney G_m17(0x2386F2, 0x6FC10000);
const CMoney G_m16(0x38D7E, 0xA4C68000);
const CMoney G_m15(0x5AF3, 0x107A4000);
const CMoney G_m14(0x918, 0x4E72A000);
const CMoney G_m13(0xE8, 0xD4A51000);
const CMoney G_m12(0x17, 0x4876E800);
const CMoney G_m11(0x2, 0x540BE400);
const CMoney G_m10(0, 0x3B9ACA00);
const CMoney G_m9(0, 0x5F5E100);
const CMoney G_m8(0, 0x989680);
const CMoney G_m7(0, 0xF4240);
const CMoney G_m6(0, 0x186A0);
const CMoney G_m5(0, 0x2710);
const CMoney G_m4(0, 0x3E8);
const CMoney G_m3(0, 0x64);
const CMoney G_m2(0, 0x0A);
const CMoney G_m1(0, 0x01);
const CMoney G_m0(0, 0x00);
const CMoney G_mZero = CMoney();

CMoney::CMoney()
{
	m_iHight = 0;
	m_uiLow = 0;
}

CMoney::CMoney(int iHight, unsigned int uiLow)
{
	m_iHight = iHight;
	m_uiLow = uiLow;
}

CMoney::CMoney(const CMoney &m)
{
	m_iHight = m.m_iHight;
	m_uiLow = m.m_uiLow;
}

CMoney::CMoney(int i)
{
	char pBuffer[30];

	memset(pBuffer, 0, sizeof(pBuffer));

	snprintf(pBuffer, sizeof(pBuffer), "%d", i);

	*this = from_string(pBuffer, 0);
}

CMoney::CMoney(double dbl)
{
	char pBuffer[30];

	memset(pBuffer, 0, sizeof(pBuffer));

	snprintf(pBuffer, sizeof(pBuffer), "%19.2f", dbl);

	*this = from_string(pBuffer, 0);
}

CMoney::CMoney(LPCSTR ptr)
{
	*this = from_string(ptr, 0);
}

const CMoney &CMoney::operator=(const CMoney &m)
{
	m_iHight = m.m_iHight;
	m_uiLow = m.m_uiLow;
	return *this;
}

double CMoney::to_double() const
{
	return atof(to_string());
}

String CMoney::to_string_no_dot(int iScale) const
{
	if (iScale != 2 && iScale != 4)
	{
		ThrowException(ERR_INVALID_MONEY_SCALE);
	}

	String s = to_string();

	if (iScale == 2)
	{
		s = s.left(strlen(s) - 5) + s.substr(strlen(s) - 4, 2);
	}
	else
	{
		s = s.left(strlen(s) - 5) + s.substr(strlen(s) - 4, 4);
	}

	return s;
}

String CMoney::to_string() const
{
	char pBuffer[40]{};
	char *p = pBuffer;

	CMoney m;

	if (m_iHight < 0)
	{
		m = G_mZero - *this;
		*p++ = '-';
	}
	else
	{
		m = *this;
	}

	bool bFlag = false;
	for (int i = 0; i < 19; i++)
	{
		int j = 0;

		while (m >= G_mMoneyList[18 - i])
		{
			m = m - G_mMoneyList[18 - i];
			j++;

			if (j > 10)
			{
				break;
			}
		}

		if (j || bFlag)
		{
			*p++ = j + '0';
		}

		if (j && !bFlag)
		{
			bFlag = true;
		}
	}

	*p = 0;
	String s;
	p = pBuffer;
	int iLen = strlen(pBuffer);

	if (m_iHight < 0)
	{
		s = "-";
		iLen--;
		p++;

		if (!iLen)
		{
			// 最大的负数
			s = s + MIN_MONEY_VALUE;
			s = s.left(strlen(s) - 4) + "." + s.right(4);
			return s;
		}
	}

	if (iLen <= 4)
	{
		s = s + "0." + String('0', 4 - iLen) + p;
	}
	else
	{
		s = s + p;
		s = s.left(strlen(s) - 4) + "." + s.right(4);
	}

	return s;
}

CMoney CMoney::from_string(LPCSTR ptr, int iScale)
{
	// 返回值
	CMoney m;

	// 只能是不带小数点的#####.##格式字串或#####.####字串，以及带小数点的字串
	if (iScale != 0 && iScale != 2 && iScale != 4)
	{
		String sErr;
		sErr = ERR_INVALID_MONEY_STR;
		sErr += ptr;
		ThrowException(sErr);
	}

	// 去除前面的空格
	while (*ptr == ' ')
	{
		ptr++;
	}

	// 去除后面的空格和空格后的无效串
	int iLen = 0;
	const char *p = ptr;

	while (*p)
	{
		if (*p != ' ')
		{
			iLen++;
			p++;
		}
		else
		{
			break;
		}
	}

	if (!iLen || !ptr)
	{
		// 空字串
		return m;
	}

	p = ptr;
	// 检查是否是合法数字串
	bool bDot = false;

	// 只有第一位可以是'+'、'-'
	if (!isdigit(*p) && (*p != '+') && (*p != '-') && (*p != '.'))
	{
		ThrowException(ERR_INVALID_MONEY_STR);
	}

	if (*p == '.')
	{
		bDot = true;
	}

	int i = -1;
	for (i = 0; i < iLen - 1; i++)
	{
		if ((!isdigit(*++p) && (*p != '.')) || (bDot && *p == '.'))
		{
			ThrowException(ERR_INVALID_MONEY_STR);
		}

		if (*p == '.')
		{
			bDot = true;
		}
	}

	if (iScale && bDot)
	{
		ThrowException(ERR_INVALID_MONEY_STR);
	}

	// 将字串考到Buffer并去除'.'，去除小数点四位以后的数
	p = ptr;
	String Buffer(iLen + 5);

	unsigned char *pBuffer = (unsigned char *)(Buffer.get().data());
	bool bNegative = false;

	if (*p == '-')
	{
		bNegative = true;
		p++;
		iLen--;
	}

	if (*p == '+')
	{
		p++;
		iLen--;
	}

	if (!iLen || iLen < iScale)
	{
		ThrowException(ERR_INVALID_MONEY_STR);
	}

	int iScaleFlag = 0;
	char c = 0;

	for (i = 0; i < iLen; i++)
	{
		if (*p != '.')
		{
			// 只取正数部分和小数部分的前四位
			if (!iScaleFlag || iScale < 4)
			{
				*pBuffer++ = *p;
			}

			if (iScaleFlag)
			{
				// 自带小数的串，且已到小数位
				iScale++;

				// 保留小数点后第5位，用于四舍五入
				if (iScale == 5)
				{
					c = *p;
				}
			}
		}
		else
		{
			iScaleFlag = 1;
		}

		p++;
	}

	// 对Scale < 4的字串结尾补"00";
	if (iScale < 4)
	{
		i = 4 - iScale;

		while (i--)
		{
			*pBuffer++ = '0';
		}
	}

	*pBuffer = 0;
	iLen = strlen(Buffer);

	if (iLen > 19 || (iLen == 19 && !bNegative && strcmp(Buffer, MAX_MONEY_VALUE) > 0) ||
		(iLen == 19 && bNegative && strcmp(Buffer, MIN_MONEY_VALUE) > 0))
	{
		ThrowException(ERR_INVALID_MONEY_STR);
	}

	// 将一个代表正整数的字串转为CMoney
	// 定位到最后一位数字
	pBuffer--;
	for (i = 0; i < iLen; i++)
	{
		int j = *pBuffer - '0';
		while (j--)
		{
			m = m + G_mMoneyList[i];
		}

		pBuffer--;
	}

	// 处理四舍五入，不能因加1导致溢出
	if (c >= '5' && ((!bNegative && strcmp(Buffer, MAX_MONEY_VALUE) < 0) || (bNegative && strcmp(Buffer, MIN_MONEY_VALUE) < 0)))
	{
		m = m + CMoney(0, 1);
	}

	if (bNegative)
	{
		m = G_mZero - m;
	}

	return m;
}

CMoney operator+(const CMoney &m1, const CMoney &m2)
{
	CMoney m;
	unsigned int uiSum = 0;
	unsigned int uiMask = 1;
	unsigned int ui1 = m1.m_uiLow;
	unsigned int ui2 = m2.m_uiLow;
	uiSum = 0;
	int iFlag = 0;

	int i = 0;
	for (i = 0; i < 32; i++)
	{
		int j = (ui1 & 1) + (ui2 & 1) + iFlag;
		if (j == 1 || j == 3)
		{
			uiSum |= uiMask;
		}

		if (j >= 2)
		{
			iFlag = 1;
		}
		else
		{
			iFlag = 0;
		}

		// printf("uiSum=%d,j=%d,mask=%d\n", uiSum, j, uiMask);
		uiMask <<= 1;
		ui1 >>= 1;
		ui2 >>= 1;
	}

	m.m_uiLow = uiSum;
	ui1 = (unsigned int)m1.m_iHight;
	ui2 = (unsigned int)m2.m_iHight;
	uiSum = 0;
	uiMask = 1;

	for (i = 0; i < 32; i++)
	{
		int j = (ui1 & 1) + (ui2 & 1) + iFlag;

		if (j == 1 || j == 3)
		{
			uiSum |= uiMask;
		}

		if (j >= 2)
		{
			iFlag = 1;
		}
		else
		{
			iFlag = 0;
		}

		// printf("uiSum=%d,j=%d,mask=%d\n", uiSum, j, uiMask);
		uiMask <<= 1;
		ui1 >>= 1;
		ui2 >>= 1;
	}

	m.m_iHight = uiSum;

	return m;
}

CMoney operator-(const CMoney &m1, const CMoney &m2)
{
	CMoney m3, m;
	m3.m_uiLow = ~m2.m_uiLow;
	m3.m_iHight = ~m2.m_iHight;

	CMoney one((int)0, 1);

	m = m1 + m3 + one;

	return m;
}