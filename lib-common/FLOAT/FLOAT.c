#include "../FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	// nemu_assert(0);
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
	int tmp_a = a;
	int tmp_b = b;
	int res = (tmp_a / tmp_b) << 16;
	tmp_a %= tmp_b;
	int i;
	for(i = 1; i <= 16; i++) {
		tmp_a <<= 1;
		res += ((tmp_a / tmp_b) << (16 - i));
		tmp_a %= tmp_b;
	}
	return res;
}

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
	// nemu_assert(0);
	FLOAT *ptr = (int *)&a;
	FLOAT tmp = *ptr;
	// int s = (tmp >> 31) & 1;
	int e = (tmp >> 23) & 0xff;
	FLOAT m = tmp & 0x7fffff;
	e = e - 134;
	m = m + (1 << 23);
	if(e > 0) {
		m <<= e;
	}
	else if(e < 0) {
		m >>= (-e);
	}
	if((tmp >> 31) & 1) {
		return -m;
	}
	else {
		return m;
	}
}



FLOAT Fabs(FLOAT a) {
	// nemu_assert(0);
	FLOAT res = a;
	if(a < 0) {
		return -res;
	}
	else {
		return res;
	}
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

