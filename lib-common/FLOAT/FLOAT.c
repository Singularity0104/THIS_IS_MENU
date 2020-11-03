#include "../FLOAT.h"
#include "string.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	nemu_assert(0);
	long long tmp = (long long)a * (long long)b;
	tmp = tmp >> 16;
	FLOAT res = (FLOAT)tmp;
	return res;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */

	// nemu_assert(0);
	FLOAT tmp_a = *((int *)&a);
	FLOAT tmp_b = *((int *)&b);
	FLOAT a_h = tmp_a & 0xffff0000;
	FLOAT a_l = tmp_a & 0xffff;
	FLOAT res = ((a_h / tmp_b) << 16) + ((a_l << 16) / tmp_b);
	return res;
}

// FLOAT f2F(float a) {
// 	/* You should figure out how to convert `a' into FLOAT without
// 	 * introducing x87 floating point instructions. Else you can
// 	 * not run this code in NEMU before implementing x87 floating
// 	 * point instructions, which is contrary to our expectation.
// 	 *
// 	 * Hint: The bit representation of `a' is already on the
// 	 * stack. How do you retrieve it to another variable without
// 	 * performing arithmetic operations on it directly?
// 	 */
// 	// nemu_assert(0);
// 	FLOAT res = 0;
// 	FLOAT tmp = 0;
// 	memcpy(&tmp, &a, 4);
// 	int s = (tmp >> 31) & 1;
// 	int e = (tmp >> 23) & 0xff;
// 	int m = tmp & 0x7fffff;
// 	e = e - 127;
// 	m = m + (1 << 23);
// 	int offset = e - 7;
// 	if(offset > 0) {
// 		res = m << offset;
// 	}
// 	else {
// 		offset = -offset;
// 		res = m >> offset;
// 	}
// 	if(s == 1) {
// 		res = ~res + 1;
// 	}
// 	return res;
// }
FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */
	//   提示：“ a”的位表示形式已经在堆栈中。 如何在不直接对其执行算术运算的情况下将其检索到另一个变量？

	// nemu_assert(0);
	// float : 1 sign + 8 exp + 23 frac
	// 值：	规格化：					(-1)^s * 2^(exp - 127) * (1.frac) = (-1)^s * 2^(exp - 150) * (1frac)
	// 	   非规格化：	exp == 0		(-1)^s * 2^(1-127) * 0.frac = (-1)^s * 2^(1-150) * frac
	// 读取float a
	int bb[1];
	memcpy((void *)bb, (void *)&a, 4);
	int b = bb[0];
	int sign = b >> 31;
	int exp = (b >> 23) & 0xff;
	FLOAT frac = b & 0x7fffff;
	// 规格化和非规格化，全部指数化
	if (exp != 0) frac += 1 << 23;		//如果是规格化的，小数点前有1
	else exp = 1;						//如果非规格话，那么exp当1处理
	exp -= 150;
	// 得到FLOAT
	// frac << exp << 16		即		frac << (exp + 16)
	if(exp + 16 > 0) frac <<= exp + 16;
	else if(exp + 16 < 0) frac >>= -(exp + 16);
	// 返回，加上符号
	if(!sign) return frac;
	else return -frac;
}


FLOAT Fabs(FLOAT a) {
	// nemu_assert(0);
	FLOAT res = a;
	if(((a >> 31) & 1) == 1) {
		res = ~a + 1;
	}
	return res;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

