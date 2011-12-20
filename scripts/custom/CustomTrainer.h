#define LIMITE_WARRIOR 137
#define LIMITE_SHAMAN 207
#define LIMITE_DRUID 235
#define LIMITE_WARLOCK 199
#define LIMITE_HUNTER 146
#define LIMITE_MAGE 198
#define LIMITE_PALADIN 181
#define LIMITE_PRIEST 199
#define LIMITE_ROGUE 136

#define LIMITE_COMP_ARME 14

uint32 comp_arme[] = 
{
196,197,198,
199,200,201,
202,227,264,
266,1180,2567,
5011,15590,
};


uint32 rogue_sort[] = 
{
53,408,674,703,921,1725,1757,1758,1759,1760,1766,1767,1768,1769,1776,1777,1784,1785,1786,1787,1804,1833,
1842,1856,1857,1860,1943,1966,2070,2094,2589,2590,2591,2835,2836,2837,2983,3127,3420,3421,
5171,5277,5763,5938,6760,6761,6762,6768,6770,6774,8621,8623,8629,8631,8632,8633,8637,8639,8640,8643,8647,
8649,8650,8676,8687,8691,8694,8696,8721,8724,8725,11197,11198,11267,11268,11269,11273,
11274,11275,11279,11280,11281,11285,11286,11289,11290,11293,11294,11297,11299,11300,11303,
11305,11341,11342,11343,11357,11358,11400,13220,13228,13229,13230,17347,17348,25300,
25302,25347,26669,26679,26786,26839,26861,26862,26863,26864,26865,26866,26867,26884,26889,
26892,26969,27282,27283,27441,27448,31016,31224,32645,32684,34411,34412,34413,38764,38768,
};

uint32 warrior_sort[] = 
{
71, 72, 100, 284, 285, 469, 674, 676, 694, 750, 772, 845, 871, 1160, 1161, 1464, 1608, 
1671, 1672, 1680, 1715, 1719, 2048, 2458, 2565, 2687, 3127, 3411, 5242, 5246, 5308, 6178, 
6190, 6192, 6343, 6546, 6547, 6548, 6552, 6554, 6572, 6574, 6673, 7369, 7372, 7373, 
7379, 7384, 7400, 7402, 7405, 7887, 8198, 8204, 8205, 8380, 8820, 11549, 11550, 11551, 
11554, 11555, 11556, 11564, 11565, 11566, 11567, 11572, 11573, 11574, 11578, 11580, 
11581, 11584, 11585, 11596, 11597, 11600, 11601, 11604, 11605, 11608, 11609, 12678, 
18499, 20230, 20559, 20560, 20569, 20616, 20617, 20658, 20660, 20661, 20662, 21551, 
21552, 21553, 23892, 23893, 23894, 23920, 23923, 23924, 23925, 25202, 25203, 25208, 
25212, 25225, 25231, 25234, 25236, 25241, 25242, 25248, 25251, 25258, 25264, 25266, 
25269, 25272, 25275, 25286, 25288, 25289, 29704, 29707, 30016, 30022, 30030, 30033, 
30330, 30335, 30356, 30357, 34428, 
};

uint32 shaman_sort[] = 
{
131,324,325,332,370,421,526,528,529,546,547,548,556,905,913,915,930,939,943,945,
959,1064,1535,2008,2062,2484,2645,2825,2860,2870,2894,3738,5675,5730,6041,6196,
6363,6364,6365,6375,6377,6390,6391,6392,6495,8004,8005,8008,8010,8012,8018,8019,8024,
8027,8030,8033,8038,8042,8044,8045,8046,8050,8052,8053,8056,8058,8075,8134,8143,8154,
8155,8160,8161,8166,8170,8177,8181,8184,8190,8227,8232,8235,8249,8498,8499,8512,
8737,8835,10391,10392,10395,10396,10399,10406,10407,10408,10412,10413,10414,10427,
10428,10431,10432,10437,10438,10442,10447,10448,10456,10462,10463,10466,10467,10468,
10472,10473,10478,10479,10486,10495,10496,10497,10526,10537,10538,10585,10586,10587,
10595,10600,10601,10605,10613,10614,10622,10623,10627,11314,11315,15107,15111,15112,
15207,15208,16314,16315,16316,16339,16341,16342,16355,16356,16362,16387,20608,20609,
20610,20776,20777,24398,25357,25359,25361,25391,25396,25420,25422,25423,25439,25442,
25448,25449,25454,25457,25464,25469,25472,25479,25485,25489,25500,25505,25508,25509,
25525,25528,25533,25546,25547,25552,25557,25560,25563,25567,25570,25574,25577,25585,
25587,25908,29228,32182,32593,32594,33736,36936,
};


uint32 druid_sort[] = 
{
99, 339, 467, 740, 768, 769, 770, 774, 778, 779, 780, 782, 783, 1058, 
1062, 1066, 1075, 1079, 1082, 1126, 1430, 1735, 1822, 1823, 1824, 1850, 
2090, 2091, 2637, 2782, 2893, 2908, 2912, 3029, 3627, 5177, 5178, 
5179, 5180, 5186, 5187, 5188, 5189, 5195, 5196, 5201, 5209, 5211, 
5215, 5217, 5221, 5225, 5229, 5232, 5234, 5487, 6756, 6778, 6780, 6783, 
6785, 6787, 6793, 6798, 6800, 6808, 6809, 8903, 8905, 8907, 8910, 
8914, 8918, 8921, 8924, 8925, 8926, 8927, 8928, 8929, 8936, 8938, 
8939, 8940, 8941, 8949, 8950, 8951, 8955, 8972, 8983, 8992, 8998, 9000, 
9005, 9490, 9492, 9493, 9634, 9745, 9747, 9749, 9750, 9752, 9754, 
9756, 9758, 9821, 9823, 9827, 9829, 9830, 9833, 9834, 9835, 9839, 
9840, 9841, 9845, 9846, 9849, 9850, 9852, 9853, 9856, 9857, 9858, 
9862, 9863, 9866, 9867, 9875, 9876, 9880, 9881, 9884, 9885, 9888, 
9889, 9892, 9894, 9896, 9898, 9901, 9904, 9907, 9908, 9910, 9912, 
9913, 16810, 16811, 16812, 16813, 16914, 17329, 17390, 17391, 17392, 
17401, 17402, 18657, 18658, 20484, 20719, 20739, 20742, 20747, 20748, 
21849, 21850, 22568, 22570, 22812, 22827, 22828, 22829, 22842, 22895, 
22896, 24248, 24974, 24975, 24976, 24977, 25297, 25298, 25299, 26978, 26979, 26980, 26981, 26982, 
26983, 26984, 26985, 26986, 26987, 26988, 26989, 26990, 26992, 26993, 26994, 26995, 26996, 26997, 
26998, 26999, 27000, 27001, 27002, 27003, 27004, 27005, 27006, 27008, 27009, 27011, 27012, 
27013, 29166, 31018, 31709, 33357, 33745, 33763, 33786, 33943, 
33982, 33983, 33986, 33987, 
};

uint32 warlock_sort[] = 
{
126,132,172,348,603,688,689,691,693,695,696,697,698,699,
702,704,705,706,707,709,710,712,755,980,1014,1086,1088,1094,
1098,1106,1108,1120,1454,1455,1456,1490,1714,1949,2362,
2941,3698,3699,3700,5138,5484,5500,5676,5697,5699,5740,
5782,5784,6201,6202,6205,6213,6215,6217,6219,6222,6223,
6226,6229,6353,6366,6789,7641,7646,7648,7651,7658,7659,8288,
8289,11659,11660,11661,11665,11667,11668,11671,11672,11675,11677,11678,
11683,11684,11687,11688,11689,11693,11694,11695,11699,11700,11703,
11704,11707,11708,11711,11712,11713,11717,11719,11721,
11722,11725,11726,11729,11730,11733,11734,11735,11739,11740,17727,
17728,17862,17919,17920,17921,17922,17923,17924,17925,17926,17928,
17937,17951,17952,17953,18647,18867,18868,18869,18870,18871,
18879,18880,18881,18930,18931,18932,18937,18938,20752,20755,
20756,20757,25307,25309,25311,27209,27210,27211,27212,27213,
27215,27216,27217,27218,27219,27220,27221,27222,27223,27224,
27226,27228,27229,27230,27238,27243,27250,27259,27260,27263,
27264,27265,27266,28172,28176,28189,28610,29722,29858,29893,
30404,30405,30413,30414,30459,30545,30546,30908,30909,30910,
30911,30912,32231,
};

uint32 hunter_sort[] = 
{
136,
674,
781,
883,
982,
1002,
1130,
1462,
1494,
1495,
1499,
1510,
1513,
1515,
1543,
1978,
2643,
2974,
3034,
3043,
3044,
3045,
3111,
3661,
3662,
5116,
5118,
5384,
6197,
8737,
6991,
13159,
13161,
13163,
13165,
13542,
13543,
13544,
13549,
13550,
13551,
13552,
13553,
13554,
13555,
13795,
13809,
13813,
14260,
14261,
14262,
14263,
14264,
14265,
14266,
14267,
14268,
14269,
14270,
14271,
14272,
14273,
14274,
14279,
14280,
14281,
14282,
14283,
14284,
14285,
14286,
14287,
14288,
14289,
14290,
14294,
14295,
14302,
14303,
14304,
14305,
14310,
14311,
14316,
14317,
14318,
14319,
14320,
14321,
14322,
14323,
14324,
14325,
14326,
14327,
15629,
15630,
15631,
15632,
19801,
19878,
19879,
19880,
19882,
19883,
19884,
19885,
20043,
20190,
20736,
20900,
20901,
20902,
20903,
20904,
20905,
20906,
20909,
20910,
24132,
24133,
25294,
25295,
25296,
27014,
27015,
27016,
27018,
27019,
27020,
27021,
27022,
27023,
27025,
27044,
27045,
27046,
27065,
27066,
27067,
27068,
34026,
34074,
34120,
34477,
34600,
36916,
};

uint32 mage_sort[] = 
{
10, 
66, 
116, 
118, 
120, 
122, 
130, 
143, 
145, 
205, 
475, 
543, 
587, 
597, 
604, 
759, 
837, 
865, 
990, 
1008, 
1449, 
1459, 
1460, 
1461, 
1463, 
1953, 
2120, 
2121, 
2136, 
2137, 
2138, 
2139, 
2948, 
3140, 
3552, 
3561, 
3562, 
3565, 
5143, 
5144, 
5145, 
5504, 
5505, 
5506, 
6117, 
6127, 
6129, 
6131, 
6141, 
6143, 
7300, 
7301, 
7302, 
7320, 
7322, 
8400, 
8401, 
8402, 
8406, 
8407, 
8408, 
8412, 
8413, 
8416, 
8417, 
8422, 
8423, 
8427, 
8437, 
8438, 
8439, 
8444, 
8445, 
8446, 
8450, 
8451, 
8455, 
8457, 
8458, 
8461, 
8462, 
8492, 
8494, 
8495, 
10053, 
10054, 
10059, 
10138, 
10139, 
10140, 
10144, 
10145, 
10148, 
10149, 
10150, 
10151, 
10156, 
10157, 
10159, 
10160, 
10161, 
10169, 
10170, 
10173, 
10174, 
10177, 
10179, 
10180, 
10181, 
10185, 
10186, 
10187, 
10191, 
10192, 
10193, 
10197, 
10199, 
10201, 
10202, 
10205, 
10206, 
10207, 
10211, 
10212, 
10215, 
10216, 
10219, 
10220, 
10223, 
10225, 
10230, 
11416, 
12051, 
12505, 
12522, 
12523, 
12524, 
12525, 
12526, 
12824, 
12825, 
12826, 
13018, 
13019, 
13020, 
13021, 
13031, 
13032, 
13033, 
18809, 
22782, 
22783, 
23028, 
25304, 
25306, 
25345, 
27070, 
27071, 
27072, 
27073, 
27074, 
27075, 
27078, 
27079, 
27080, 
27082, 
27085, 
27086, 
27087, 
27088, 
27101, 
27124, 
27125, 
27126, 
27128, 
27130, 
27131, 
27132, 
27133, 
27134, 
28609, 
28612, 
30449, 
30451, 
30455, 
30482, 
32796, 
33041, 
33042, 
33043, 
33405, 
33933, 
33938, 
33944, 
33946, 
37420, 
38699, 
43987, 
45438, 
};

uint32 paladin_sort[] = 
{
465,
498,
633,
639,
642,
643,
647,
750,
853,
879,
1020,
1022,
1026,
1032,
1038,
1042,
1044,
1152,
2800,
2812,
2878,
3127,
3472,
4987,
5573,
5588,
5589,
5599,
5614,
5615,
5627,
6940,
7294,
7328,
10278,
10290,
10291,
10292,
10293,
10298,
10299,
10300,
10301,
10308,
10310,
10312,
10313,
10314,
10318,
10322,
10324,
10326,
10328,
10329,
13819,
19740,
19742,
19746,
19750,
19752,
19834,
19835,
19836,
19837,
19838,
19850,
19852,
19853,
19854,
19876,
19888,
19891,
19895,
19896,
19897,
19898,
19899,
19900,
19939,
19940,
19941,
19942,
19943,
19977,
19978,
19979,
20116,
20162,
20164,
20165,
20166,
20271,
20287,
20288,
20289,
20290,
20291,
20292,
20293,
20305,
20306,
20307,
20308,
20347,
20348,
20349,
20356,
20357,
20729,
20772,
20773,
20912,
20913,
20914,
20915,
20918,
20919,
20920,
20922,
20923,
20924,
20927,
20928,
20929,
20930,
21082,
24239,
24274,
24275,
25290,
25291,
25292,
25780,
25782,
25890,
25894,
25895,
25898,
25899,
25916,
25918,
26573,
27135,
27136,
27137,
27138,
27139,
27140,
27141,
27142,
27143,
27144,
27145,
27147,
27148,
27149,
27150,
27151,
27152,
27153,
27154,
27155,
27158,
27160,
27166,
27168,
27169,
27170,
27173,
27174,
27179,
27180,
31785,
31789,
31801,
31884,
31895,
32223,
32699,
32700,
33072,
33776,
};

uint32 priest_sort[] = 
{
17,139,453,527,528,552,586,588,589,591,592,594,596,598,600,602,605,970,976,984,988,
992,996,1004,1006,1243,1244,1245,1706,2006,2010,2052,2053,2054,2055,2060,2061,2096,
2767,2791,3747,6060,6063,6064,6065,6066,6074,6075,6076,6077,6078,6346,7128,8092,8102,
8103,8104,8105,8106,8122,8124,8129,8131,8192,9472,9473,9474,9484,9485,9578,9579,9592,10874,
10875,10876,10880,10881,10888,10890,10892,10893,10894,10898,10899,10900,10901,10909,
10911,10912,10915,10916,10917,10927,10928,10929,10933,10934,10937,10938,10941,10942,
10945,10946,10947,10951,10952,10953,10955,10957,10958,10960,10961,10963,10964,10965,
14818,14819,14914,15261,15262,15263,15264,15265,15266,15267,15430,15431,17311,17312,
17313,17314,18807,19276,19277,19278,19279,19280,19281,19282,19283,19284,19285,20770,
21562,21564,25210,25213,25217,25218,25221,25222,25233,25235,25308,25312,25314,25315,
25316,25331,25363,25364,25367,25368,25372,25375,25379,25380,25384,25387,25389,25429,
25431,25433,25435,25467,25470,25477,25596,27681,27683,27799,27800,27801,27841,27870,
27871,28275,32375,32379,32546,32996,32999,33076,34433,34863,34864,34865,34866,34916,34917,
};