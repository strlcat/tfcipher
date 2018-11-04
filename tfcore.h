#ifndef _THREEFISH_CIPHER_CORE_HEADER
#define _THREEFISH_CIPHER_CORE_HEADER

#ifndef _THREEFISH_CIPHER_DEFINITIONS_HEADER
#error Threefish definitions header is required! Include tfdef.h first.
#endif

#define ROL(x, s, max) ((x << s) | (x >> (-s & (max-1))))
#define ROR(x, s, max) ((x >> s) | (x << (-s & (max-1))))

#define KE_MIX(x, y, k1, k2, sl)				\
	do {							\
		x += k1;					\
		y += x;						\
		y += k2;					\
		x = ROL(x, sl, TF_UNIT_BITS);			\
		x ^= y;						\
	} while (0)

#define BE_MIX(x, y, sl)					\
	do {							\
		x += y;						\
		y = ROL(y, sl, TF_UNIT_BITS);			\
		y ^= x;						\
	} while (0)

#define KD_MIX(x, y, k1, k2, sr)				\
	do {							\
		x ^= y;						\
		x = ROR(x, sr, TF_UNIT_BITS);			\
		y -= x;						\
		y -= k2;					\
		x -= k1;					\
	} while (0)

#define BD_MIX(x, y, sr)					\
	do {							\
		y ^= x;						\
		y = ROR(y, sr, TF_UNIT_BITS);			\
		x -= y;						\
	} while (0)

#define THREEFISH_CONST 0x1bd11bdaa9fc1a22ULL

#if defined(TF_256BITS)
enum tf_rotations {
	TFS_KS01 = 14, TFS_KS02 = 16, TFS_KS03 = 25, TFS_KS04 = 33,
	TFS_BS01 = 52, TFS_BS02 = 57, TFS_BS03 = 23, TFS_BS04 = 40,
	TFS_BS05 =  5, TFS_BS06 = 37, TFS_BS07 = 46, TFS_BS08 = 12,
	TFS_BS09 = 58, TFS_BS10 = 22, TFS_BS11 = 32, TFS_BS12 = 32,
};
#elif defined(TF_512BITS)
enum tf_rotations {
	TFS_KS01 = 46, TFS_KS02 = 36, TFS_KS03 = 19, TFS_KS04 = 37,
	TFS_KS05 = 39, TFS_KS06 = 30, TFS_KS07 = 34, TFS_KS08 = 24,
	TFS_BS01 = 33, TFS_BS02 = 27, TFS_BS03 = 14, TFS_BS04 = 42,
	TFS_BS05 = 17, TFS_BS06 = 49, TFS_BS07 = 36, TFS_BS08 = 39,
	TFS_BS09 = 44, TFS_BS10 =  9, TFS_BS11 = 54, TFS_BS12 = 56,
	TFS_BS13 = 13, TFS_BS14 = 50, TFS_BS15 = 10, TFS_BS16 = 17,
	TFS_BS17 = 25, TFS_BS18 = 29, TFS_BS19 = 39, TFS_BS20 = 43,
	TFS_BS21 =  8, TFS_BS22 = 35, TFS_BS23 = 56, TFS_BS24 = 22,
};
#elif defined(TF_1024BITS)
enum tf_rotations {
	TFS_KS01 = 24, TFS_KS02 = 13, TFS_KS03 =  8, TFS_KS04 = 47,
	TFS_KS05 =  8, TFS_KS06 = 17, TFS_KS07 = 22, TFS_KS08 = 37,
	TFS_KS09 = 41, TFS_KS10 =  9, TFS_KS11 = 37, TFS_KS12 = 31,
	TFS_KS13 = 12, TFS_KS14 = 47, TFS_KS15 = 44, TFS_KS16 = 30,
	TFS_BS01 = 38, TFS_BS02 = 19, TFS_BS03 = 10, TFS_BS04 = 55,
	TFS_BS05 = 49, TFS_BS06 = 18, TFS_BS07 = 23, TFS_BS08 = 52,
	TFS_BS09 = 33, TFS_BS10 =  4, TFS_BS11 = 51, TFS_BS12 = 13,
	TFS_BS13 = 34, TFS_BS14 = 41, TFS_BS15 = 59, TFS_BS16 = 17,
	TFS_BS17 =  5, TFS_BS18 = 20, TFS_BS19 = 48, TFS_BS20 = 41,
	TFS_BS21 = 47, TFS_BS22 = 28, TFS_BS23 = 16, TFS_BS24 = 25,
	TFS_BS25 = 16, TFS_BS26 = 34, TFS_BS27 = 56, TFS_BS28 = 51,
	TFS_BS29 =  4, TFS_BS30 = 53, TFS_BS31 = 42, TFS_BS32 = 41,
	TFS_BS33 = 31, TFS_BS34 = 44, TFS_BS35 = 47, TFS_BS36 = 46,
	TFS_BS37 = 19, TFS_BS38 = 42, TFS_BS39 = 44, TFS_BS40 = 25,
	TFS_BS41 =  9, TFS_BS42 = 48, TFS_BS43 = 35, TFS_BS44 = 52,
	TFS_BS45 = 23, TFS_BS46 = 31, TFS_BS47 = 37, TFS_BS48 = 20,
};
#else
#error No cipher was defined! Aborting build.
#endif

#endif
