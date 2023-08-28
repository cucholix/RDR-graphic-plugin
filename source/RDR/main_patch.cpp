#include "skyline/inlinehook/And64InlineHook.hpp"
#include "skyline/utils/cpputils.hpp"
#include "skyline/inlinehook/memcpy_controlled.hpp"

//C0 0F 00 36 [00 10 2E 1E] [fmov s0, #1.00000000]
constinit uint8_t mod1[8] = {0xC0, 0x0F, 0x00, 0x36, 0x00, 0x10, 0x2E, 0x1E};
							 
//C0 0F 00 36 [00 10 20 1E] [fmov s0, #2.00000000]
constinit uint8_t mod1_p[8] = {0xC0, 0x0F, 0x00, 0x36, 0x00, 0x10, 0x20, 0x1E};

//[80 36 40 BD] D6 7E 43 F9 [ldr s0, [x20, #0x34]]
constinit uint8_t mod2[8] = {0x80, 0x36, 0x40, 0xBD, 0xD6, 0x7E, 0x43, 0xF9};
							 
//00 10 3A 1E [fmov s0, #-0.25000000]
constinit uint8_t mod2_p[4] = {0x00, 0x10, 0x3A, 0x1E};

//[20 01 22 1E] 68 00 00 34 - [scvtf s0, w9]
constinit uint8_t mod3[8] = {0x20, 0x01, 0x22, 0x1E, 0x68, 0x00, 0x00, 0x34};
							 
//00 10 22 1E - [fmov s0, #4.00000000]
constinit uint8_t mod3_p[4] = {0x00, 0x10, 0x22, 0x1E};

//C9 03 01 90 [00 F6 03 0F] - [fmov v0.2s, #1.00000000]
constinit uint8_t mod4[8] = {0xC9, 0x03, 0x01, 0x90, 0x00, 0xF6, 0x03, 0x0F};
							 
//C9 03 01 90 [00 F4 00 0F] - [fmov v0.2s, #2.00000000]
constinit uint8_t mod4_p[8] = {0xC9, 0x03, 0x01, 0x90, 0x00, 0xF4, 0x00, 0x0F};

//20 E9 49 BD [69 66 86 52] [69 D6 A7 72] - [movz w9, #0x3333] [movk w9, #0x3eb3, lsl #16]
constinit uint8_t mod5[12] = {0x20, 0xE9, 0x49, 0xBD, 0x69, 0x66, 0x86, 0x52, 0x69, 0xD6, 0xA7, 0x72};
							 
//20 E9 49 BD [09 00 80 52] [09 D0 A7 72] - [movz w9, #0] [movk w9, #0x3e80, lsl #16]
constinit uint8_t mod5_p[12] = {0x20, 0xE9, 0x49, 0xBD, 0x09, 0x00, 0x80, 0x52, 0x09, 0xD0, 0xA7, 0x72};

//[89 64 A8 52] 49 5B 00 B9 - [movz w9, #0x4324, lsl #16]
constinit uint8_t mod6[8] = {0x89, 0x64, 0xA8, 0x52, 0x49, 0x5B, 0x00, 0xB9};
							 
//09 70 A8 52 - [movz w9, #0x4380, lsl #16]
constinit uint8_t mod6_p[4] = {0x09, 0x70, 0xA8, 0x52};

//[1B 64 A8 52] E6 13 80 3D - [movz w27, #0x4320, lsl #16]
constinit uint8_t mod7[8] = {0x1B, 0x64, 0xA8, 0x52, 0xE6, 0x13, 0x80, 0x3D};
							 
//DB 77 A8 52 - [movz w27, #0x43be, lsl #16]
constinit uint8_t mod7_p[4] = {0xDB, 0x77, 0xA8, 0x52};

//ED 7B 40 B9 EC 8B 40 B9 EB 9B 40 B9 EA B3 40 B9 
//E5 FF FF 17 F7 43 40 F9 A8 7D 01 13 9F 05 00 71 
//E8 47 00 B9 CB 36 00 54 68 C2 02 91 E9 03 51 39 
//00 F6 03 4F E8 3F 00 F9 E8 03 0C 2A 0A 10 2C 1E 
//E8 33 00 F9 E8 17 42 F9 0B 10 2E 1E 0C E4 00 2F 
//0D 10 3E 1E F4 03 1F AA F6 83 03 91 E8 4F 00 F9 
//E8 13 42 F9 E0 2F 80 3D E8 47 00 F9 E8 33 44 B9 
//E8 97 00 B9 E8 43 50 39 E9 23 0B 29 0A 00 00 14 
//F4 3B 40 F9 E9 22 5C F9 0A 00 8A 52 F6 83 03 91 
//28 69 2A B8 E8 33 40 F9 94 06 00 91 9F 02 08 EB 
//E0 32 00 54 E8 A3 05 91 08 11 14 8B 1D 01 40 F9 
//09 09 40 BD A8 33 40 79 C8 1C 00 34 [08 64 A8 52] - [movz w8, #0x4320, lsl #16]
constinit uint8_t mod8[192] = {0xED, 0x7B, 0x40, 0xB9, 0xEC, 0x8B, 0x40, 0xB9, 0xEB, 0x9B, 0x40, 0xB9, 0xEA, 0xB3, 0x40, 0xB9, 
                               0xE5, 0xFF, 0xFF, 0x17, 0xF7, 0x43, 0x40, 0xF9, 0xA8, 0x7D, 0x01, 0x13, 0x9F, 0x05, 0x00, 0x71, 
                               0xE8, 0x47, 0x00, 0xB9, 0xCB, 0x36, 0x00, 0x54, 0x68, 0xC2, 0x02, 0x91, 0xE9, 0x03, 0x51, 0x39, 
                               0x00, 0xF6, 0x03, 0x4F, 0xE8, 0x3F, 0x00, 0xF9, 0xE8, 0x03, 0x0C, 0x2A, 0x0A, 0x10, 0x2C, 0x1E, 
                               0xE8, 0x33, 0x00, 0xF9, 0xE8, 0x17, 0x42, 0xF9, 0x0B, 0x10, 0x2E, 0x1E, 0x0C, 0xE4, 0x00, 0x2F, 
                               0x0D, 0x10, 0x3E, 0x1E, 0xF4, 0x03, 0x1F, 0xAA, 0xF6, 0x83, 0x03, 0x91, 0xE8, 0x4F, 0x00, 0xF9, 
                               0xE8, 0x13, 0x42, 0xF9, 0xE0, 0x2F, 0x80, 0x3D, 0xE8, 0x47, 0x00, 0xF9, 0xE8, 0x33, 0x44, 0xB9, 
                               0xE8, 0x97, 0x00, 0xB9, 0xE8, 0x43, 0x50, 0x39, 0xE9, 0x23, 0x0B, 0x29, 0x0A, 0x00, 0x00, 0x14, 
                               0xF4, 0x3B, 0x40, 0xF9, 0xE9, 0x22, 0x5C, 0xF9, 0x0A, 0x00, 0x8A, 0x52, 0xF6, 0x83, 0x03, 0x91, 
                               0x28, 0x69, 0x2A, 0xB8, 0xE8, 0x33, 0x40, 0xF9, 0x94, 0x06, 0x00, 0x91, 0x9F, 0x02, 0x08, 0xEB, 
                               0xE0, 0x32, 0x00, 0x54, 0xE8, 0xA3, 0x05, 0x91, 0x08, 0x11, 0x14, 0x8B, 0x1D, 0x01, 0x40, 0xF9, 
                               0x09, 0x09, 0x40, 0xBD, 0xA8, 0x33, 0x40, 0x79, 0xC8, 0x1C, 0x00, 0x34, 0x08, 0x64, 0xA8, 0x52};
	
//ED 7B 40 B9 EC 8B 40 B9 EB 9B 40 B9 EA B3 40 B9 
//E5 FF FF 17 F7 43 40 F9 A8 7D 01 13 9F 05 00 71 
//E8 47 00 B9 CB 36 00 54 68 C2 02 91 E9 03 51 39 
//00 F6 03 4F E8 3F 00 F9 E8 03 0C 2A 0A 10 2C 1E 
//E8 33 00 F9 E8 17 42 F9 0B 10 2E 1E 0C E4 00 2F 
//0D 10 3E 1E F4 03 1F AA F6 83 03 91 E8 4F 00 F9 
//E8 13 42 F9 E0 2F 80 3D E8 47 00 F9 E8 33 44 B9 
//E8 97 00 B9 E8 43 50 39 E9 23 0B 29 0A 00 00 14 
//F4 3B 40 F9 E9 22 5C F9 0A 00 8A 52 F6 83 03 91 
//28 69 2A B8 E8 33 40 F9 94 06 00 91 9F 02 08 EB 
//E0 32 00 54 E8 A3 05 91 08 11 14 8B 1D 01 40 F9 
//09 09 40 BD A8 33 40 79 C8 1C 00 34 [08 84 A8 52] - [movz w8, #0x4420, lsl #16]
constinit uint8_t mod8_p[192] = {0xED, 0x7B, 0x40, 0xB9, 0xEC, 0x8B, 0x40, 0xB9, 0xEB, 0x9B, 0x40, 0xB9, 0xEA, 0xB3, 0x40, 0xB9, 
                                 0xE5, 0xFF, 0xFF, 0x17, 0xF7, 0x43, 0x40, 0xF9, 0xA8, 0x7D, 0x01, 0x13, 0x9F, 0x05, 0x00, 0x71, 
                                 0xE8, 0x47, 0x00, 0xB9, 0xCB, 0x36, 0x00, 0x54, 0x68, 0xC2, 0x02, 0x91, 0xE9, 0x03, 0x51, 0x39, 
                                 0x00, 0xF6, 0x03, 0x4F, 0xE8, 0x3F, 0x00, 0xF9, 0xE8, 0x03, 0x0C, 0x2A, 0x0A, 0x10, 0x2C, 0x1E, 
                                 0xE8, 0x33, 0x00, 0xF9, 0xE8, 0x17, 0x42, 0xF9, 0x0B, 0x10, 0x2E, 0x1E, 0x0C, 0xE4, 0x00, 0x2F, 
                                 0x0D, 0x10, 0x3E, 0x1E, 0xF4, 0x03, 0x1F, 0xAA, 0xF6, 0x83, 0x03, 0x91, 0xE8, 0x4F, 0x00, 0xF9, 
                                 0xE8, 0x13, 0x42, 0xF9, 0xE0, 0x2F, 0x80, 0x3D, 0xE8, 0x47, 0x00, 0xF9, 0xE8, 0x33, 0x44, 0xB9, 
                                 0xE8, 0x97, 0x00, 0xB9, 0xE8, 0x43, 0x50, 0x39, 0xE9, 0x23, 0x0B, 0x29, 0x0A, 0x00, 0x00, 0x14, 
                                 0xF4, 0x3B, 0x40, 0xF9, 0xE9, 0x22, 0x5C, 0xF9, 0x0A, 0x00, 0x8A, 0x52, 0xF6, 0x83, 0x03, 0x91, 
                                 0x28, 0x69, 0x2A, 0xB8, 0xE8, 0x33, 0x40, 0xF9, 0x94, 0x06, 0x00, 0x91, 0x9F, 0x02, 0x08, 0xEB, 
                                 0xE0, 0x32, 0x00, 0x54, 0xE8, 0xA3, 0x05, 0x91, 0x08, 0x11, 0x14, 0x8B, 0x1D, 0x01, 0x40, 0xF9, 
                                 0x09, 0x09, 0x40, 0xBD, 0xA8, 0x33, 0x40, 0x79, 0xC8, 0x1C, 0x00, 0x34, 0x08, 0x84, 0xA8, 0x52};

//20 00 40 BD 00 60 04 BD 20 04 40 BD 00 64 04 BD - [ldr s0, [x1]] - [str s0, [x0, #0x460]] - [ldr s0, [x1, #4]] - [str s0, [x0, #0x464]]
//20 08 40 BD 00 68 04 BD 20 0C 40 BD 00 6C 04 BD - [ldr s0, [x1, #8]] - [str s0, [x0, #0x468]] - [ldr s0, [x1, #0xc]] - [str s0, [x0, #0x46c]]
constinit uint8_t mod9[32] = {0x20, 0x00, 0x40, 0xBD, 0x00, 0x60, 0x04, 0xBD, 0x20, 0x04, 0x40, 0xBD, 0x00, 0x64, 0x04, 0xBD, 
                              0x20, 0x08, 0x40, 0xBD, 0x00, 0x68, 0x04, 0xBD, 0x20, 0x0C, 0x40, 0xBD, 0x00, 0x6C, 0x04, 0xBD};
							 
//E0 03 27 1E E0 03 27 1E E0 03 27 1E E0 03 27 1E - [fmov s0, wzr] - [fmov s0, wzr] - [fmov s0, wzr] - [fmov s0, wzr] 
//E0 03 27 1E E0 03 27 1E E0 03 27 1E E0 03 27 1E - [fmov s0, wzr] - [fmov s0, wzr] - [fmov s0, wzr] - [fmov s0, wzr]
constinit uint8_t mod9_p[32] = {0xE0, 0x03, 0x27, 0x1E, 0xE0, 0x03, 0x27, 0x1E, 0xE0, 0x03, 0x27, 0x1E, 0xE0, 0x03, 0x27, 0x1E, 
                                0xE0, 0x03, 0x27, 0x1E, 0xE0, 0x03, 0x27, 0x1E, 0xE0, 0x03, 0x27, 0x1E, 0xE0, 0x03, 0x27, 0x1E};

//62 8A 45 BD 62 39 22 1E - [ldr s2, [x19, #0x588]]
constinit uint8_t mod10[8] = {0x62, 0x8A, 0x45, 0xBD, 0x62, 0x39, 0x22, 0x1E};
							 
//E2 03 27 1E - [fmov s2, wzr]
constinit uint8_t mod10_p[4] = {0xE2, 0x03, 0x27, 0x1E};

//21 01 40 BD 20 C0 21 1E - [ldr s1, [x9]]
constinit uint8_t mod11[8] = {0x21, 0x01, 0x40, 0xBD, 0x20, 0xC0, 0x21, 0x1E};
							 
//01 10 2C 1E - [fmov s1, #0.50000000]
constinit uint8_t mod11_p[4] = {0x01, 0x10, 0x2C, 0x1E};

//49 8F A8 52 0A 59 A8 52 - [movz w9, #0x447a, lsl #16] - [movz w10, #0x42c8, lsl #16]
constinit uint8_t mod12[8] = {0x49, 0x8F, 0xA8, 0x52, 0x0A, 0x59, 0xA8, 0x52};
							 					 
//09 00 80 52 0A 00 80 52 - [movz w9, #0] - [movz w10, #0]
constinit uint8_t mod12_p[8] = {0x09, 0x00, 0x80, 0x52, 0x0A, 0x00, 0x80, 0x52};

//[08 F0 A7 52] BF 1E 08 B9 [movz w8, #0x3f80, lsl #16]
constinit uint8_t mod13[8] = {0x08, 0xF0, 0xA7, 0x52, 0xBF, 0x1E, 0x08, 0xB9};
							 					 
//08 08 A8 52 [movz w8, #0x4040, lsl #16]
constinit uint8_t mod13_p[4] = {0x08, 0x08, 0xA8, 0x52};

//FF 17 28 7C A8 52 00 01 - ?
constinit uint8_t mod14[8] = {0xFF, 0x17, 0x28, 0x7C, 0xA8, 0x52, 0x00, 0x01};
							 					 
//FF 17 68 88
constinit uint8_t mod14_p[4] = {0xFF, 0x17, 0x68, 0x88};

//[48 8F A8 52] 3F 00 00 71 - [movz w8, #0x447a, lsl #16]
constinit uint8_t mod15[8] = {0x48, 0x8F, 0xA8, 0x52, 0x3F, 0x00, 0x00, 0x71};
							 					 
//48 9F A8 52 - [movz w8, #0x44fa, lsl #16]
constinit uint8_t mod15_p[4] = {0x48, 0x9F, 0xA8, 0x52};

//[08 00 90 52] [68 97 A8 72] - [movz w8, #0x8000] [movk w8, #0x44bb, lsl #16]
constinit uint8_t mod16[8] = {0x08, 0x00, 0x90, 0x52, 0x68, 0x97, 0xA8, 0x72};
							 					 
//[08 00 88 52] [88 A3 A8 72] - [movz w8, #0x4000] [movk w8, #0x451c, lsl #16]
constinit uint8_t mod16_p[8] = {0x08, 0x00, 0x88, 0x52, 0x88, 0xA3, 0xA8, 0x72};

//[08 54 A8 52] 09 00 90 52 - [movz w8, #0x42a0, lsl #16]
constinit uint8_t mod17[8] = {0x08, 0x54, 0xA8, 0x52, 0x09, 0x00, 0x90, 0x52};
							 					 
//48 60 A8 52 - [movz w8, #0x4302, lsl #16]
constinit uint8_t mod17_p[4] = {0x48, 0x60, 0xA8, 0x52};

//08 54 A8 52 09 00 90 52 69 97 A8 72 - [movz w8, #0x42a0, lsl #16] - [movz w9, #0x8000] - [movk w9, #0x44bb, lsl #16]
constinit uint8_t mod18[12] = {0x08, 0x54, 0xA8, 0x52, 0x09, 0x00, 0x90, 0x52, 0x69, 0x97, 0xA8, 0x72};
							 					 
//48 60 A8 52 09 00 88 52 89 A3 A8 72 - [movz w8, #0x4302, lsl #16] - [movz w9, #0x4000] - [movk w9, #0x451c, lsl #16]
constinit uint8_t mod18_p[12] = {0x48, 0x60, 0xA8, 0x52, 0x09, 0x00, 0x88, 0x52, 0x89, 0xA3, 0xA8, 0x72};

//C7 FF FF 17 [28 7C A8 52] - [movz w8, #0x43e1, lsl #16]
constinit uint8_t mod19[8] = {0xC7, 0xFF, 0xFF, 0x17, 0x28, 0x7C, 0xA8, 0x52};
							 					 
//C7 FF FF 17 [68 88 A8 52] - [movz w8, #0x4443, lsl #16]
constinit uint8_t mod19_p[8] = {0xC7, 0xFF, 0xFF, 0x17, 0x68, 0x88, 0xA8, 0x52};

//[28 7C A8 52] FA 03 17 2A - [movz w8, #0x43e1, lsl #16]
constinit uint8_t mod20[8] = {0x28, 0x7C, 0xA8, 0x52, 0xFA, 0x03, 0x17, 0x2A};
							 					 
//68 88 A8 52 - [movz w8, #0x4443, lsl #16]
constinit uint8_t mod20_p[4] = {0x68, 0x88, 0xA8, 0x52};

//[C9 50 A8 52] 02 01 40 BD - [movz w9, #0x4286, lsl #16]
constinit uint8_t mod21[8] = {0xC9, 0x50, 0xA8, 0x52, 0x02, 0x01, 0x40, 0xBD};
							 					 
//89 5B A8 52 - [movz w9, #0x42dc, lsl #16]
constinit uint8_t mod21_p[4] = {0x89, 0x5B, 0xA8, 0x52};

//[88 51 A8 52] 00 01 27 1E 40 09 20 1E - ?
constinit uint8_t mod22[12] = {0x88, 0x51, 0xA8, 0x52, 0x00, 0x01, 0x27, 0x1E, 0x40, 0x09, 0x20, 0x1E};
							 					 
//08 5E A8 52 - ?
constinit uint8_t mod22_p[4] = {0x08, 0x5E, 0xA8, 0x52};

//A9 96 42 B9 [4A 8F A8 52] - [movz w10, #0x447a, lsl #16]
constinit uint8_t mod23[8] = {0xA9, 0x96, 0x42, 0xB9, 0x4A, 0x8F, 0xA8, 0x52};
							 					 
//A9 96 42 B9 [4A 9F A8 52] - [movz w10, #0x44fa, lsl #16]
constinit uint8_t mod23_p[8] = {0xA9, 0x96, 0x42, 0xB9, 0x4A, 0x9F, 0xA8, 0x52};

//[0A 59 A8 52] A4 4E 41 BD - [movz w10, #0x42c8, lsl #16]
constinit uint8_t mod24[8] = {0x0A, 0x59, 0xA8, 0x52, 0xA4, 0x4E, 0x41, 0xBD};
							 					 
//0A 79 A8 52 - [movz w10, #0x43c8, lsl #16]
constinit uint8_t mod24_p[4] = {0x0A, 0x79, 0xA8, 0x52};

//[4A 6F A8 52] 21 0C 23 1E [movz w10, #0x437a, lsl #16]
constinit uint8_t mod25[8] = {0x4A, 0x6F, 0xA8, 0x52, 0x21, 0x0C, 0x23, 0x1E};
							 					 
//4A 8F A8 52 - [movz w10, #0x447a, lsl #16]
constinit uint8_t mod25_p[4] = {0x4A, 0x8F, 0xA8, 0x52};

//[0A 59 A8 52] 84 08 24 1E - [movz w10, #0x42c8, lsl #16]
constinit uint8_t mod26[8] = {0x0A, 0x59, 0xA8, 0x52, 0x84, 0x08, 0x24, 0x1E};
							 					 
//0A 79 A8 52 - [movz w10, #0x43c8, lsl #16]
constinit uint8_t mod26_p[4] = {0x0A, 0x79, 0xA8, 0x52};

//[8A A3 A8 72] 09 10 B0 12 - [movk w10, #0x451c, lsl #16]
constinit uint8_t mod27[8] = {0x8A, 0xA3, 0xA8, 0x72, 0x09, 0x10, 0xB0, 0x12};
							 					 
//8A B3 A8 72 - [movk w10, #0x459c, lsl #16]
constinit uint8_t mod27_p[4] = {0x8A, 0xB3, 0xA8, 0x72};

//73 00 73 68 6F 63 6B 41 - [s.shockA]
constinit uint8_t mod28[8] = {0x73, 0x00, 0x73, 0x68, 0x6F, 0x63, 0x6B, 0x41};
							 					 
//6D 00 73 68 - [m.sh]
constinit uint8_t mod28_p[4] = {0x6D, 0x00, 0x73, 0x68};

//00 00 20 03 00 00 20 03
constinit uint8_t mod29[8] = {0x00, 0x00, 0x20, 0x03, 0x00, 0x00, 0x20, 0x03};
							 					 
//00 00 90 06 00 00 90 06
constinit uint8_t mod29_p[8] = {0x00, 0x00, 0x90, 0x06, 0x00, 0x00, 0x90, 0x06};

//9A 99 19 3E 00 00 A0 40 00 00 20 41
constinit uint8_t mod30[12] = {0x9A, 0x99, 0x19, 0x3E, 0x00, 0x00, 0xA0, 0x40, 0x00, 0x00, 0x20, 0x41};
							 					 
//00 00 80 3E 00 00 00 00 00 00 00 00
constinit uint8_t mod30_p[12] = {0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//0A D7 A3 3B 0A D7 23 3C 6F 12 83 3C B8 1E 85 3E
constinit uint8_t mod31[16] = {0x0A, 0xD7, 0xA3, 0x3B, 0x0A, 0xD7, 0x23, 0x3C, 0x6F, 0x12, 0x83, 0x3C, 0xB8, 0x1E, 0x85, 0x3E};
							 					 
//6F 12 03 3C 96 43 8B 3C EC 51 B8 3C 3D 0A D7 3E
constinit uint8_t mod31_p[16] = {0x6F, 0x12, 0x03, 0x3C, 0x96, 0x43, 0x8B, 0x3C, 0xEC, 0x51, 0xB8, 0x3C, 0x3D, 0x0A, 0xD7, 0x3E};

//40 00 00 80 3F 00 00 00 00 00 00 00
constinit uint8_t mod32[12] = {0x40, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
							 					 
//66 66 E6 3F
constinit uint8_t mod32_p[4] = {0x66, 0x66, 0xE6, 0x3F};


bool memcmp_f (const unsigned char *s1, const unsigned char *s2, size_t count) {
    while (count-- > 0)
        if (*s1++ != *s2++) 
            return false;
    return true;
}

size_t getCodeSize() {
        return (size_t)skyline::utils::getRegionAddress(skyline::utils::region::Rodata) - (size_t)skyline::utils::getRegionAddress(skyline::utils::region::Text);
}

/* You must provide full instructions in arrays of uint8_t.
It returns pointer to first found code in main 
or 0 if nothing have been found. */
uintptr_t findTextCode(const uint8_t* code, size_t size) {
    if (size % 4 != 0) return 0;
    uintptr_t addr = (uintptr_t)skyline::utils::getRegionAddress(skyline::utils::region::Text);
    size_t addr_size = getCodeSize();	
    uintptr_t addr_start = addr;
    while (addr != addr_start + addr_size) {
        bool result = memcmp_f((const unsigned char*)addr, code, size);
        if (result) return addr;
        addr += 4;
    }
    return 0;
}

void rdr_graphics_main() {	
    uintptr_t pointer = findTextCode(&mod1[0], sizeof(mod1));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod1_p[0], sizeof(mod1_p));
    }
	pointer = findTextCode(&mod2[0], sizeof(mod2));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod2_p[0], sizeof(mod2_p));
    }
	pointer = findTextCode(&mod3[0], sizeof(mod3));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod3_p[0], sizeof(mod3_p));
    }
	pointer = findTextCode(&mod4[0], sizeof(mod4));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod4_p[0], sizeof(mod4_p));
    }
	pointer = findTextCode(&mod5[0], sizeof(mod5));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod5_p[0], sizeof(mod5_p));
    }
	pointer = findTextCode(&mod6[0], sizeof(mod6));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod6_p[0], sizeof(mod6_p));
    }
	pointer = findTextCode(&mod7[0], sizeof(mod7));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod7_p[0], sizeof(mod7_p));
    }
	pointer = findTextCode(&mod8[0], sizeof(mod8));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod8_p[0], sizeof(mod8_p));
    }
	pointer = findTextCode(&mod9[0], sizeof(mod9));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod9_p[0], sizeof(mod9_p));
    }
	pointer = findTextCode(&mod10[0], sizeof(mod10));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod10_p[0], sizeof(mod10_p));
    }
	pointer = findTextCode(&mod11[0], sizeof(mod11));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod11_p[0], sizeof(mod11_p));
    }
	pointer = findTextCode(&mod12[0], sizeof(mod12));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod12_p[0], sizeof(mod12_p));
    }
	pointer = findTextCode(&mod13[0], sizeof(mod13));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod13_p[0], sizeof(mod13_p));
    }
	pointer = findTextCode(&mod14[0], sizeof(mod14));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod14_p[0], sizeof(mod14_p));
    }
	pointer = findTextCode(&mod15[0], sizeof(mod15));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod15_p[0], sizeof(mod15_p));
    }
	pointer = findTextCode(&mod16[0], sizeof(mod16));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod16_p[0], sizeof(mod16_p));
    }
	pointer = findTextCode(&mod17[0], sizeof(mod17));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod17_p[0], sizeof(mod17_p));
    }
	pointer = findTextCode(&mod18[0], sizeof(mod18));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod18_p[0], sizeof(mod18_p));
    }
	pointer = findTextCode(&mod19[0], sizeof(mod19));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod19_p[0], sizeof(mod19_p));
    }
	pointer = findTextCode(&mod20[0], sizeof(mod20));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod20_p[0], sizeof(mod20_p));
    }
	pointer = findTextCode(&mod21[0], sizeof(mod21));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod21_p[0], sizeof(mod21_p));
    }
	pointer = findTextCode(&mod22[0], sizeof(mod22));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod22_p[0], sizeof(mod22_p));
    }
	pointer = findTextCode(&mod23[0], sizeof(mod23));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod23_p[0], sizeof(mod23_p));
    }
	pointer = findTextCode(&mod24[0], sizeof(mod24));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod24_p[0], sizeof(mod24_p));
    }
	pointer = findTextCode(&mod25[0], sizeof(mod25));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod25_p[0], sizeof(mod25_p));
    }
	pointer = findTextCode(&mod26[0], sizeof(mod26));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod26_p[0], sizeof(mod26_p));
    }
	pointer = findTextCode(&mod27[0], sizeof(mod27));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod27_p[0], sizeof(mod27_p));
    }
	pointer = findTextCode(&mod28[0], sizeof(mod28));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod28_p[0], sizeof(mod28_p));
    }
	pointer = findTextCode(&mod29[0], sizeof(mod29));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod29_p[0], sizeof(mod29_p));
    }
	pointer = findTextCode(&mod30[0], sizeof(mod30));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod30_p[0], sizeof(mod30_p));
    }
	pointer = findTextCode(&mod31[0], sizeof(mod31));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod31_p[0], sizeof(mod31_p));
    }
	pointer = findTextCode(&mod32[0], sizeof(mod32));
    if (pointer) {
        sky_memcpy((void*)pointer, &mod32_p[0], sizeof(mod32_p));
    }
}
