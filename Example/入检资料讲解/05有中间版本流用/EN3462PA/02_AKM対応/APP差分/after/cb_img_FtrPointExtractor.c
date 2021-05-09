/**
 *	@addtogroup Calib�R���|�[�l���g
 */
/*@{*/

/******************************************************************************/
/**
 * @file		cb_img_FtrPointExtractor.c
 * @brief		�����_���o
 * @author		K.Kato
 * @date		2013.01.10	K.Kato			�V�K�쐬
 * @date		2016.06.24	M.Ando			�G���u�����ɂ��P�����Ή�
 * @date		2017.10.04	A.Honda			���ۂ̌��m�\�͌���(#2-��2)
 * @note		None
 */
/******************************************************************************/

/********************************************************************
 * �C���N���[�h�t�@�C��
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifndef _WINDOWS
	#include <mathf.h>
#endif /* _WINDOWS */

#include "ipxdef.h"
#include "ipxsys.h"
#include "ipxprot.h"

/* ���ʃw�b�_ */
#include "CL_TYPE.H"
#ifndef _WINDOWS
	#include "CL_Common_ex.h"
	#include "CL_uITRON_ex.h"
	#include "CL_Define_ex.H"
	#include "CL_AppSection2_ex.h"
#else
	#include "CL_uITRON_pc.h"
#endif /* _WINDOWS */

#include "cb_main_ex.h"

#include "cb_img_FtrPointExtractor_ex.h"
#include "cb_CameraGeometry.h"
#include "cb_img_Param.h"
#include "cb_DEF_CarPrm.h"
#include "cb_selfCalib.h"
#include "cb_share.h"
#include "cb_recog.h"

/* �������Ԍv���pinclude */
#ifndef _WINDOWS
#include "RG_ModeConfig.h"
#include "RG_Dbg_TimeMeasurement.h"
#endif /* _WINDOWS */

/* �f�o�b�O�pinclude */
#ifdef CB_FPE_CSW_DEBUG_ON
	#include "cb_DBG_ColorPalette.h"
	#include "IPC_Common.h"
	#include "ipext.h"
#endif /* CB_FPE_CSW_DEBUG_ON */

#ifdef _WINDOWS
#pragma warning( disable : 4057 )
#pragma warning( disable : 4996 )
#endif /* _WINDOWS */

/********************************************************************
 * extern
 ********************************************************************/

//extern const double_t CB_GEO_PI;

/********************************************************************
 * �����}�N����`
 ********************************************************************/

#ifdef SOFTVP
	#define	implib_IP_IntegralImage	implib_IntegralImage	/* ��FLIB����API���ԈႢ�ɑ΂���b��΍� */
#endif /* SOFTVP */

/* �R���p�C���X�C�b�` */
//#define	CB_FPE_CSW_ENABLE_TEST_MODE
//#define	CB_FPE_CSW_ENABLE_SRCH_ADD_PATTERN_FAST
//#define	CB_FPE_CSW_DEMO
#define	CB_FPE_CSW_CIRCLE_SRCH_RGN		/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 */
#define	CB_FPE_CSW_JUDGE_BR_SIMILARITY	/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 */
#define CB_FPE_SEARCH_REGION_NARROW	/* ���E�T���͈͏k���΍� <CHG> 2015.01.21 */

#define	CB_GEO_PI	( (float_t)CB_CG_PI )	/*!< ,�~����,value=3.141593,-,[-], */

#define	CB_FPE_CHK_OUTSIDE_RGN( nX, nY, nSxRgn, nSyRgn, nExRgn, nEyRgn )	(    ( ( ( nSxRgn ) <= ( nX ) ) && ( ( nX ) <= ( nExRgn ) ) ) \
																			  && ( ( ( nSyRgn ) <= ( nY ) ) && ( ( nY ) <= ( nEyRgn ) ) ) )	/*!< ,�̈�O�`�F�b�N�}�N����`,-,[-], */
#define	CB_FPE_CONVERT_m2mm_double( nValue )	( ( nValue ) * 1000.0 )	/*!< ,mm����m�ւ̕ϊ�(double�p),-,[-], */
#define	CB_FPE_SWAP( a, b )	{	( a ) = ( a ) ^ ( b );		\
								( b ) = ( a ) ^ ( b );		\
								( a ) = ( a ) ^ ( b );		\
							}	/*!< ,SWAP�p�}�N����`,-,[-], */
#define	CB_FPE_CONVERT_DEG2RAD( dDeg )	( ( (dDeg) * CB_GEO_PI ) / 180.0 )	/*!< ,Degree to Radian,-,[-], */
#define	CB_FPE_CONVERT_RAD2DEG_F( dRad )	( ( (dRad) * 180.0F ) / CB_GEO_PI )	/*!< ,Radian to Degree,-,[-], */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY
#define	CB_FPE_COMP( a, b )		( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )		/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */
#define CB_FPE_NELEMS( a ) ( sizeof( ( a ) ) / sizeof( ( a )[0] ) )					/* �z��̗v�f���擾 �Q�l����`��CMN_NELEMS <ADD> 2013.11.25 */

/* --- ���g�p�ϐ����[�j���O��� --- */
#define	CALIB_UNUSED_VARIABLE(value)		(void)((value))		/*!< ,���g�p�ϐ���Warning���p,-,[-], */
#define	CALIB_UNUSED_VARIABLE_P(value)		(void*)((value))	/*!< ,���g�p�|�C���^�ϐ���Warning���p,-,[-], */

#define CB_FPE_FLT_MIN	( 1.0e-5F )		/*!< ,�{�ŏ��l�����������l�́u0�v�Ƃ݂Ȃ�,value=1.0e-5F,[-], */
#define CB_FPE_DBL_MIN	( 1.0e-14L )	/*!< ,�{�ŏ��l�����������l�́u0�v�Ƃ݂Ȃ�,value=1.0e-14L,[-], */

#define	CB_FPE_MAXNUM_SRCH_RGN_PNT			( 4L )	/*!< ,�T���̈���\������ő�_��,value=4,[-], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CHKBOARD		( 10L )	/*!< ,�s���^�[�Q�b�g�p�T���̈�X�����̃}�[�W��,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CHKBOARD		( 10L )	/*!< ,�s���^�[�Q�b�g�p�T���̈�Y�����̃}�[�W��,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CIRCLE_LINE	( 10L )	/*!< ,���ۃ^�[�Q�b�g�p�T���̈�X�����̃}�[�W��,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CIRCLE_LINE	( 10L )	/*!< ,���ۃ^�[�Q�b�g�p�T���̈�Y�����̃}�[�W��,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_ADDCHKBOARD		( 10L )	/*!< ,���E�s���^�[�Q�b�g�p�T���̈�X�����̃}�[�W��,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_ADDCHKBOARD		( 10L )	/*!< ,���E�s���^�[�Q�b�g�p�T���̈�Y�����̃}�[�W��,value=10,[pixel], */

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN
#define	CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN	( 3L )	/*!< ,���ےT���̈�p�}�[�W��,value=3,[pixel], *//* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */

#define	CB_FPE_GRID_INTERVAL_WLD_X			( 1.0 )		/*!< ,�O���b�h���̊Ԋu(X����),value=1.0,[-], */
#define	CB_FPE_GRID_INTERVAL_WLD_Y			( 1.0 )		/*!< ,�O���b�h���̊Ԋu(Y����),value=1.0,[-], */
#define	CB_FPE_GRID_HIGH_ORDER_LEFT_WLD		( -5.0 )	/*!< ,�O���b�h�����[�ʒu(World),value=-5.0,[m], */
#define	CB_FPE_GRID_HIGH_ORDER_RIGHT_WLD	( +5.0 )	/*!< ,�O���b�h���E�[�ʒu(World),value=+5.0,[m], */
#define	CB_FPE_GRID_HIGH_ORDER_NEAR_WLD		( +0.0 )	/*!< ,�O���b�h�����[�ʒu(World),value=-5.0,[m], */
#define	CB_FPE_GRID_HIGH_ORDER_FAR_WLD		( +8.0 )	/*!< ,�O���b�h���E�[�ʒu(World),value=+5.0,[m], */

#define	CB_FPE_PRM_ZOOMOUT_XMAG		( 2L )		/*!< ,�k���{��,value=2,[-], */
#define	CB_FPE_PRM_ZOOMOUT_YMAG		( 2L )		/*!< ,�k���{��,value=2,[-], */
#define	CB_FPE_IMG_XSIZE_ZOOMOUTx2	( 360L )	/*!< ,�k�����̉摜X�����T�C�Y,value=360,[pixel], */
#define	CB_FPE_IMG_YSIZE_ZOOMOUTx2	( 240L )	/*!< ,�k�����̉摜Y�����T�C�Y,value=240,[pixel], */

#define	CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE	( 10L/*6L/*10L*/ )	/*!< ,Haar-like���o��(�s���G�b�W)�A�u���b�N��X�����T�C�Y,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE	(  5L/*4L/*5L*/ )	/*!< ,Haar-like���o��(�s���G�b�W)�A�u���b�N��Y�����T�C�Y,value=5,[pixel], */
#define	CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_XSIZE	( 10L )	/*!< ,Haar-like���o��(�c�G�b�W)�A�u���b�N��X�����T�C�Y,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE	(  5L )	/*!< ,Haar-like���o��(�c�G�b�W)�A�u���b�N��Y�����T�C�Y,value=5,[pixel], */
#define	CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE	(  5L )	/*!< ,Haar-like���o��(���G�b�W)�A�u���b�N��X�����T�C�Y,value=5,[pixel], */
#define	CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_YSIZE	( 10L )	/*!< ,Haar-like���o��(���G�b�W)�A�u���b�N��Y�����T�C�Y,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE	( 10L )	/*!< ,Haar-like���o��(���E�s���G�b�W)�A�u���b�N��X�����T�C�Y,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE	( 10L )	/*!< ,Haar-like���o��(���E�s���G�b�W)�A�u���b�N��Y�����T�C�Y,value=10,[pixel], */
#define CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL	( 6L )	/*!< ,Haar-like���̈挟�o��(���E�s���G�b�W)�A�u���b�N��X�����T�C�Y,value=6,[pixel], */
#define CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL	( 6L )	/*!< ,Haar-like���̈挟�o��(���E�s���G�b�W)�A�u���b�N��Y�����T�C�Y,value=6,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE	( 290L )	/*!< ,Haar-like���o��(���E�s���G�b�W)�A��̈�̖ʐ�,value=290,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL	( 110L )	/*!< ,Haar-like���o��(���E�s���G�b�W)�A���̈�̖ʐ�,value=110,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL	( 102L )	/*!< ,Haar-like�����o��(���E�s���G�b�W)�A��̈�̖ʐ�,value=102,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL	( 42L )	/*!< ,Haar-like�����o��(���E�s���G�b�W)�A���̈�̖ʐ�,value=42,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FRONT	( 320L )	/*!< ,Haar-like���o��(���E�s���G�b�W)�A��̈�̖ʐ�,value=320,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FRONT	( 80L )	/*!< ,Haar-like���o��(���E�s���G�b�W)�A���̈�̖ʐ�,value=80,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FRONT	( 112L )	/*!< ,Haar-like�����o��(���E�s���G�b�W)�A��̈�̖ʐ�,value=112,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FRONT	( 30L )	/*!< ,Haar-like�����o��(���E�s���G�b�W)�A���̈�̖ʐ�,value=30,[pixel], */

#ifdef CALIB_PARAM_P32S_EUR_UK
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FR		( 318L )	/*!< ,Haar-like���o��(�t�����g�J�����E���E�s���G�b�W)�A��̈�̖ʐ�,value=290,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FR		(  82L )	/*!< ,Haar-like���o��(�t�����g�J�����E���E�s���G�b�W)�A���̈�̖ʐ�,value=110,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FR	( 114L )	/*!< ,Haar-like�����o��(�t�����g�J�����E���E�s���G�b�W)�A��̈�̖ʐ�,value=102,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FR	(  30L )	/*!< ,Haar-like�����o��(�t�����g�J�����E���E�s���G�b�W)�A���̈�̖ʐ�,value=42,[pixel], */
#endif /* CALIB_PARAM_P32S_EUR_UK */

#define	CB_FPE_PRM_COEFF_SMOOTH_FLT	\
						1L, 2L, 1L, \
						2L, 4L, 1L, \
						1L, 2L, 1L			/*!< ,�������t�B���^�̌W��,-,[-], */
#define	CB_FPE_PRM_SCALE_SMOOTH_FLT	( 4L )	/*!< ,�������t�B���^�̃V�t�g�_�E����,value=4,[-], */

static slong	CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN = 0L;			/*!< ,�����x�ʒu���ߗp�̒T���̈�(X����),3��value��255,[pixel], */
static slong	CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN= 0L;			/*!< ,�����x�ʒu���ߗp�̒T���̈�(Y����),3��value��255,[pixel], */
static slong	CB_FPE_PRM_HALF_WIDTH_PRECISE_SRCH_RGN = 0L;	/*!< ,�����x�ʒu���ߗp�̒T���̈�(1/2�AX����),1��value��127,[pixel], */
static slong	CB_FPE_PRM_HALF_HEIGHT_PRECISE_SRCH_RGN = 0L;	/*!< ,�����x�ʒu���ߗp�̒T���̈�(1/2�AY����),1��value��127,[pixel], */
static slong	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_X = 0L;		/*!< ,�T���̈�ݒ莞�̃x�[�X�_�܂ł̋���(X),0��value��255,[pixel], */
static slong	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_Y = 0L;		/*!< ,�T���̈�ݒ莞�̃x�[�X�_�܂ł̋���(Y),0��value��255,[pixel], */

static slong	CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN = 0L;	/*!< ,���E�s�������x�ʒu���߁A�G�b�W�_�T����,value=CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN/2L,[-], */

#define	CB_FPE_PRM_SIZE_LINE_FLT				(  5 )						/*!< ,���C���t�B���^�T�C�Y,value=5,[pixel], */
#define	CB_FPE_PRM_COEFF_LINE_FLT				{ -1L, -1L, 0L, +1L, +1L }	/*!< ,���C���t�B���^�̌W��,?<=value<=?,[-], */
#define	CB_FPE_PRM_PERIPHERAL_NO_USE			(  2L )						/*!< ,�������̈�,value=2,[pixel], */

#define	CB_FPE_PRM_PROJECTION_DATA_NUM			( 20L )									/*!< ,���e�T�C�Y,value=20,[pixel], */
#define	CB_FPE_PRM_PROJECTION_DATA_NUM_HALF		( CB_FPE_PRM_PROJECTION_DATA_NUM / 2L )	/*!< ,���e�T�C�Y(1/2),value=10,[pixel], */
#define	CB_FPE_PRM_PROJECTION_CENTER_INTERVAL	( 5L )									/*!< ,���ړ_����̋���,value=5,[pixel], */
#define	CB_FPE_PRM_PROJECTION_WIDTH				( 5L )									/*!< ,���e��,value=5,[pixel], */

#define	CB_FPE_PRM_RRF_LENGTH_REACH					( 10L )						/*!< ,���[�`�̒����ő�l,value=10,[-], */
#define	CB_FPE_PRM_RRF_DIFF_INTENSITY				( 45L/*30L/*60L/*80L*/ )	/*!< ,���ړ_�ƃ��[�`��̋P�x�l�̍�,value=80,[-], */
#define	CB_FPE_PRM_RRF_DIFF_INTENSITY_RETRY_DIFF	( -8L )						/*!< ,���ړ_�ƃ��[�`��̋P�x�l�̍�,value=80,[-], */
#define	CB_FPE_PRM_RRF_START_REACH					(  2L )						/*!< ,���[�`�J�n����,value=2,[-], */
#define	CB_FPE_PRM_RRG_THR_REACH_FOR_MAG			(  5L )						/*!< ,�Ԉ������s���Ƃ��̒��ړ_�̃��[�`��,value=5,[-], */

#define	CB_FPE_PRM_THR_CNT_REACH			( 6L )		/*!< ,�~��̃��[�`���Ɣ��肷�邵�����l,value=7,[-], */
#define	CB_FPE_PRM_THR_MIN_LABEL_AREA		( 7L )		/*!< ,���x���ŏ��������l,value=16,[-], */
#define	CB_FPE_PRM_THR_MAX_LABEL_AREA		( 10000L )	/*!< ,���x���ő債�����l,value=10000,[-], */
#define	CB_FPE_PRM_ASPECT_RATIO				( 0.25F )	/*!< ,���x���̃A�X�y�N�g��,value=0.3,[-], */
#define	CB_FPE_PRM_LENGTH_RATIO				( 0.6F )	/*!< ,������,value=0.6,[-], */
#define	CB_FPE_PRM_THR_AREA_RATIO			( 0.45F )	/*!< ,�ʐϔ�,value=0.45,[-], */
#define	CB_FPE_PRM_THR_SIZE_LABEL			( 3L )		/*!< ,���x���T�C�Y�ɑ΂���臒l,value=3,[-], */
#define	CB_FPE_PRM_THR_DISTANCE_RATIO		( 0.5F )	/*!< ,������ɑ΂���臒l,value=0.5,[-], */

#define	CB_FPE_PRM_MERGE_DISTANCE_CAND_CROSS_PNT	( 15 * 15 )	/*!< ,�\���_�̃}�[�W����,value=15*15,[-], */
#define	CB_FPE_PRM_XSIZE_HALF_SCAN_RGN				( 5L )		/*!< ,T���_�T�����̃X�L�����T�C�Y(X),value=5,[pixel], */
#define	CB_FPE_PRM_XSIZE_REFINE_HALF_SCAN_RGN		( 2L )		/*!< ,T���_�T�����̃X�L�����T�C�Y(�i����)(X),value=2,[pixel], */
#define	CB_FPE_PRM_YSIZE_HALF_SCAN_RGN				( 5L )		/*!< ,T���_�T�����̃X�L�����T�C�Y(X),value=5,[pixel], */
#define	CB_FPE_PRM_YSIZE_REFINE_HALF_SCAN_RGN		( 2L )		/*!< ,T���_�T�����̃X�L�����T�C�Y(�i����)(Y),value=2,[pixel], */
#define	CB_FPE_PRM_DIFF_HAAR_LIKE					( 4000L )	/*!< ,�O��_�ƍ���_�ɂ�����Haar-like�����ʂ̍�,value=4000,[-], */
#define	CB_FPE_PRM_COEFF_THR_HAAR_LIKE				( 0.80F )	/*!< ,�O��_�ƍ���_�ɂ�����Haar-like�����ʂ̍��`�F�b�N���̍���Haar-like�����ʂɑ΂���W��,value=0.85,[-], */

#define	CB_FPE_PRM_XSIZE_HALF_SCAN_RGN_ADD_CHK			( 2L )			/*!< ,���E�s��T���_�T�����̃X�L�����T�C�Y(X),value=2,[pixel], */
#define	CB_FPE_PRM_COEFF_THR_EDGE						( 0.60F )		/*!< ,�O��_�ƍ���_�ɂ�����G�b�W���x�����ʂ̍��`�F�b�N����,����G�b�W���x�����ʂɑ΂���W��,value=0.60,[-], */
#define	CB_FPE_PRM_THR_EDGE_FTR_ADD_CHK_T_PNT_UL		( 50L )			/*!< ,���E�s���^�[�Q�b�g��(�㉺)��̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂���G�b�W���x�������l,value=100,[-], */
#define	CB_FPE_PRM_DISTANCE_SRCH_RGN_ADD_CHK_T_PNT_UL	( 3L/*2L*/ )			/*!< ,���E�s���^�[�Q�b�g��(�㉺)��̓����_���o���A�T���̈�܂ł̋���,value=3(�K��),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN					( 10L/*3L/*5L/*6L*/ )		/*!< ,���E�s�������x�ʒu���߁A�G�b�W�_�T����,value=3(�K��),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL				( 5L/*3L/*5L/*6L*/ )		/*!< ,���E�s�������x�ʒu���߁A�G�b�W�_�T����,value=3(�K��),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_SRCH_BASE_PNT	( 5L/*6L*/ )		/*!< ,���E�s�������x�ʒu���߁A�T���J�n�_�̊�_����̋���,value=3(�K��),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_THR_EDGE_FTR_T_PNT		( 10L/*50L*/ )		/*!< ,���E�s�������x�ʒu���߁A�����_���Ƃ���G�b�W���x�������l,value=10(�K��),[-], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_THR_RANGE_THETA			( 10L/*5L*/ )		/*!< ,���E�s�������x�ʒu���߁A�����_���Ƃ���G�b�W�p�x���e�͈�,0<value<90,[2deg], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_THR_MAX_RETRY			( 3L )		/*!< ,���E�s�������x�ʒu���߁A�G�b�W�_�T�����g���C�񐔏��,value=2(�K��),[-], */
#define	CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE			( 0.60F/*0.75F*/ )	/*!< ,���E�s�������x�ʒu���߁A�G�b�W���x�����ʂɑ΂���W��,value=0.80(�K��),[-], */

#define	CB_FPE_PRM_THR_LINE_LABEL		( 100L )	/*!< ,���E�ۂ̐������Ƃ��Ĉ������x���ʐς������l,value=100,[-], */
#define	CB_FPE_PRM_THR_CIRCLE_LABEL		( 100L )	/*!< ,���E�ۂ̊ە����Ƃ��Ĉ������x���ʐς������l,value=100,[-], */
#define	CB_FPE_PRM_SCAN_HEIGHT_LINE_UL	(   2L )	/*!< ,���E�ۂ̐������ɂ������[�E���[�̃X�L��������,value=2,[pixel], */
#define	CB_FPE_PRM_THR_DIFF_EST_X		( 5.0f )	/*!< ,����ʒu(X)�Ƃ̍��ɑ΂��邵�����l,value=5.0,[pixel], */
#define	CB_FPE_PRM_COEFF_LINE_LENGTH	( 2.0f )	/*!< ,���E�ۂ̃y�A�Ƃ݂Ȃ��Ƃ��̐��̒����ɑ΂���W��,value=2.0,[-], */

#define	CB_FPE_PRM_RRF_DIFF_INTENSITY_PRECISE	( 80L )	/*!< ,�~�̓����_�������x���o����Ƃ��̋P�x�̍��ɑ΂��邵�����l,value=80,[-], */

#define	CB_FPE_PRM_THR_HAAR_FTR_CROSS_PNT		( 3200L )	/*!< ,�s���^�[�Q�b�g�����̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l,value=6400,[-], */
#define	CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT	( 1000L )	/*!< ,���E�s���^�[�Q�b�g�����̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l,value=6400,[-], */
#define	CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT_FRRR	( 400L )	/*!< ,�t�����g�J�����E���A�J�����̍��E�s���^�[�Q�b�g�����̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l */

#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_UL		( 1600L )	/*!< ,�s���^�[�Q�b�g��(�㉺)��̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l,value=3200,[-], */
#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_LR		( 1600L )	/*!< ,�s���^�[�Q�b�g��(���E)��̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l,value=3200,[-], */
#define	CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_UL	( 2L )		/*!< ,�s���^�[�Q�b�g��(�㉺)��̓����_���o���A�T���̈�܂ł̋���,value=2,[pixel], */
#define	CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_LR	( 2L )		/*!< ,�s���^�[�Q�b�g��(���E)��̓����_���o���A�T���̈�܂ł̋���,value=2,[pixel], */
#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_RHO_LIMIT		( 170L )	/*!< ,�s���^�[�Q�b�g��(�㉺)��̓����_���o���ɁA�����_�T�������I������ɗp����G�b�W���x�̂������l */
#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_THETA_LIMIT	( 12L )	/*!< ,�s���^�[�Q�b�g��(�㉺)��̓����_���o���ɁA�����_�T�������I������ɗp����G�b�W�p�x�̂������l */

#define	CB_FPE_PRM_THR_EDGE_RHO				( 128L )	/*!< ,�����x�ʒu���ߎ��̃G�b�W���o�̂��߂̃G�b�W���x�ɑ΂��邵�����l,value=128,[-], */
#define	CB_FPE_PRM_THR_EDGE_CNT_DOWN		(   4L )	/*!< ,�G�b�W���x���������Ȃ����Ɣ��肷��Ƃ��̃J�E���^�ɑ΂��邵�����l,value=4,[-], */

#define	CB_FPE_ERR_TBL_MAXNUM	( 256L )	/*!< ,���e�덷�̑g�ݍ��킹�e�[�u���̃T�C�Y,value=256,[-], */

#define	CB_FPE_CIRCLE_CAND_POINT_MAXNUM	( 20L )	/*!< ,�������_�̌��ʒu�̓_���ő�l,value=20,[-], */

#define	CB_FPE_MAX_SLOPE				( 4096.0F )		/*!< ,�����ɂȂ����Ƃ��̌X�����ݒ�l,value=4096.0F,[-], */
#define CB_FPE_DBG_BR_RESULT_MAXNUM		( 10L )			/*!< ,�P�x���f�o�b�O�ŕ`�悷��ő��␔,value=10L,[-],  */

#define	CB_FPE_CNT_RETRY_CIRCLE_LINE_MAXNUM		( 3L )		/*!< ,�ۗ̈�������_���o�����̃��g���C��(�ő�),value=3,[-], */
#define	CB_FPE_CNT_RETRY_LABELING_MAXNUM		( 3L )		/*!< ,���x�����O���̃��g���C��(�ő�),value=3,[-], */
#define	CB_FPE_CNT_RETRY_LABELING_THR_AREA_DIFF	( 2L )		/*!< ,���g���C���̊�������l����ϓ���������l,value=2,[-], */
#define	CB_FPE_CODE_LABEL_OVERFLOW				( 0x7FFFL )	/*!< ,���x��Overflow���̃R�[�h,value=0x7FFF,[-], */

#define CB_FPE_PRM_HISTTBL_MAXNUM		( 256L )		/*!< ,�P�x���q�X�g�O�����̃e�[�u���T�C�Y,value=256L,[-],  */
#define CB_FPE_PRM_HIST_BIN_WIDTH		( 4L )			/*!< ,�P�x���q�X�g�O�����̃r����,value=4L,[-],  */
#define CB_FPE_PRM_HIST_BIN_NUM			( CB_FPE_PRM_HISTTBL_MAXNUM / CB_FPE_PRM_HIST_BIN_WIDTH )	/*!< ,�P�x���q�X�g�O�����̃r����,value=64L,[-],  */

#define	CB_FPE_PROC_MAXSIZE_X		( 256L )	/*!< ,EdgeCode����X�����ő�T�C�Y,value=256,[pixel], */
#define	CB_FPE_PROC_MAXSIZE_Y		( 256L )	/*!< ,EdgeCode����Y�����ő�T�C�Y,value=256,[pixel], */
#define	CB_FPE_EDGE_CODE_TBL_MAXNUM	( CB_FPE_PROC_MAXSIZE_X * CB_FPE_PROC_MAXSIZE_Y )	/*!< ,EdgeCodeTable�̍ő吔,value=65536,[-], */

#define	CB_FPE_PRM_START_EDGE_ANGLE_FR_RR					( 148L )	/*!< ,�p�x�R�[�h�ݒ�J�n�ʒu�̃G�b�W�p�x(Fr/Rr),value=148,[-], */
#define	CB_FPE_PRM_START_EDGE_ANGLE_SL_SR					( 158L )	/*!< ,�p�x�R�[�h�ݒ�J�n�ʒu�̃G�b�W�p�x(SL/SR),value=158,[-], */
#define	CB_FPE_PRM_RANGE_EDGE_ANGLE_FR_RR					( 65L )		/*!< ,�p�x�͈�(Fr/Rr),value=65,[-], */
#define	CB_FPE_PRM_RANGE_EDGE_ANGLE_SL_SR					( 45L )		/*!< ,�p�x�͈�(SL/SR),value=45,[-], */
#define	CB_FPE_PRM_EDGE_CODE_SHIFT_DOWN						( 3L )		/*!< ,�G�b�W�R�[�h���̃_�E���V�t�g��,value=3,[-], */
#define	CB_FPE_PRM_EDGE_CODE_THR_MIN						( 10L )		/*!< ,�G�b�W�R�[�h���̂������l(�ŏ�)value=10,[-], */
#define	CB_FPE_PRM_EDGE_CODE_THR_MAX						( 255L )	/*!< ,�G�b�W�R�[�h���̂������l(�ő�)value=255,[-], */
#define	CB_FPE_PRM_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK		( 6L/*3L*/ )		/*!< ,���E�s����̏㉺T���T�����̒T���̈�(����),value=6,[pixel], */
#define	CB_FPE_PRM_REFINE_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK	( 3L )		/*!< ,���E�s����̏㉺T���T�����̒T���̈�(����),value=3,[pixel], */
#define	CB_FPE_PRM_THR_EDGE_RHO_SRCH_CAND_T_PNT_UL_ADD_CHK	( 20L/*10L*/ )		/*!< ,���E�s����̏㉺T���T�����̃G�b�W���x�ɑ΂��邵�����l,value=10,[-], */

/* ���E�s���p�^�[��Haar-like�����ʕ�������p�萔 */
#define CB_IMG_CHECKBOARD_SIGN_PLUS		(  +1L )		/*!< , ���E�s���p�^�[��Haar-like�����ʕ����i�v���X�j  ,value= 1,[-], */
#define CB_IMG_CHECKBOARD_SIGN_MINUS	(  -1L )		/*!< , ���E�s���p�^�[��Haar-like�����ʕ����i�}�C�i�X�j,value=-1,[-], */
#define CB_IMG_CHECKBOARD_SIGN_NONE		(   0L )		/*!< , ���E�s���p�^�[��Haar-like�����ʕ����i�����Ȃ��i�_�~�[�j�j,value= 0,[-], */
#define CB_IMG_CHECKBOARD_SIGN_CHANGE	(  -1L )		/*!< , ���E�s���p�^�[��Haar-like�����ʕ����i�������]�p�萔�j,value=-1,[-], */


/* ���E�s���p�^�[�������\���_�̍����x�ʒu���ߗp�̒T���_�̍ő�T�C�Y	*
 * (��+��) or (��+�E)�ŒT������̂ŁA								*
 * CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM(20) ��2�{��ݒ�		*/
#define CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS	( 40L )

#define CB_FPE_PRM_HAAR_LABEL_MAX_NUM				( 5L )			/*!< ,�}�[�J���ݎ��̏\���_���x�����̍ő吔, value=5,[-], */
#define	CB_FPE_PRM_THR_HAAR_MIN_LABEL_AREA			( 5L )			/*!< ,�}�[�J���ݎ��̏\���_���x���ŏ��ʐς������l,value=5,[-], */
#define	CB_FPE_PRM_THR_HAAR_MAX_LABEL_AREA			( 10000L )		/*!< ,�}�[�J���ݎ��̏\���_���x���ő�ʐς������l,value=10000,[-], */
#define CB_FPE_PRM_SRCH_WIDTH_CHECK_HAAR_UL			( 3L )			/*!< ,�}�[�J���ݎ��̏\���__���E�T��(����)���̏㉺��, value=3,[pixel], */
#define CB_FPE_PRM_COEFF_THR_HAAR_CROSS_FTR			( 0.65F )		/*!< ,�}�[�J���ݎ��̏\���__Haar-Like������臒l�Z�o�p�̌W��, value=0.65,[-], */

#define CB_FPE_MAX_ARREY_EGDE_MEDIAN						( 3L )			/*!< ,�G�b�W�����l�����߂�z��T�C�Y, value=3,[pixel], */

#define CB_FPE_PRM_ERR_DPITCHDEG	( 0.0 )		/*!< ,�s�b�`�p�덷, value=2.0,[-], */
#define CB_FPE_PRM_ERR_DROLLDEG		( 0.0 )		/*!< ,���[���p�덷, value=2.0,[-], */
#define CB_FPE_PRM_ERR_DYAWDEG		( 0.0 )		/*!< ,���[�p�덷, value=2.0,[-], */
#define CB_FPE_PRM_ERR_DSHIFTX		( 0.00 )	/*!< ,�J�����ʒuX���W�덷, value=0.05,[-], */
#define CB_FPE_PRM_ERR_DSHIFTY		( 0.00 )	/*!< ,�J�����ʒuY���W�덷, value=0.05,[-], */
#define CB_FPE_PRM_ERR_DSHIFTZ		( 0.00 )	/*!< ,�J�����ʒuZ���W�덷, value=0.05,[-], */
#define CB_FPE_PRM_ERR_DPOSHORI 	( 30.0 )	/*!< ,���������ʒu�덷, value=20.0,[-], */
#define CB_FPE_PRM_ERR_DPOSVERT 	( 30.0 )	/*!< ,���������ʒu�덷, value=20.0,[-], */

#define CB_FPE_PRM_INTIMG_FLG_CENTER 	( 0U )	/*!< ,�����}�[�J�[�̃C���e�O�����C���[�W�쐬�t���O�@�@�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�, value=0,[-], */
#define CB_FPE_PRM_INTIMG_FLG_SIDE	 	( 1U )	/*!< ,�T�C�h�}�[�J�[�̃C���e�O�����C���[�W�쐬�t���O�@�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�, value=1,[-], */

#define CB_FPE_PRM_TOLERANCE_DIST	 	( 7.0 )	/*!< ,�s�N�Z�����x���W�l�ƃT�u�s�N�Z�����x���W�l�̒����������e�l�@�s�N�Z�����x���W�l���O�Ή�, value=7.0,[-], */

/********************************************************************
 * �����񋓌^�@��`
 ********************************************************************/

/**
 * @brief	�摜�^�C�v
 */
enum enum_CB_FPE_IMG_TYPE
{
	CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL = 0L,								/*!< ,���ˉ摜(���͉摜�Ɠ��T�C�Y),value=0,[-], */
	CB_FPE_IMG_Y_WORK01_NORMAL,												/*!< ,���[�N�摜1(���͉摜�Ɠ��T�C�Y),value=1,[-], */
	CB_FPE_IMG_Y_WORK02_NORMAL,												/*!< ,���[�N�摜2(���͉摜�Ɠ��T�C�Y),value=2,[-], */
	CB_FPE_IMG_Y_WORK03_NORMAL,												/*!< ,���[�N�摜3(���͉摜�Ɠ��T�C�Y),value=3,[-], */
	CB_FPE_IMG_Y_SIZE_NORMAL_MAXNUM,										/*!< ,���͉摜�Ɠ��T�C�Y����Y��ʂ̐�,value=4,[-], */

	CB_FPE_IMG_ZOOMOUT2x2 = CB_FPE_IMG_Y_SIZE_NORMAL_MAXNUM,				/*!< ,���͉摜1/2�T�C�Y,value=4,[-], */
	CB_FPE_IMG_EDGE_RHO,													/*!< ,�G�b�W���x�摜,value=5,[-], */
	CB_FPE_IMG_EDGE_ANGLE,													/*!< ,�G�b�W�p�x�摜,value=6,[-], */
	CB_FPE_IMG_EDGE_AC,														/*!< ,�G�b�W�p�x�R�[�h�摜,value=7,[-], */
	CB_FPE_IMG_HAAR_NORMALIZE,												/*!< ,Haar-Like������(�s���G�b�W)���K���摜,value=8,[-], */
	CB_FPE_IMG_HAAR_BINARIZE,												/*!< ,Haar-Like������(�s���G�b�W)��l���摜,value=9,[-], */
	CB_FPE_IMG_HAAR_LABELING,												/*!< ,Haar-Like������(�s���G�b�W)���x���摜,value=10,[-], */
	CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM,									/*!< ,���͉摜�Ɠ��T�C�Y����Y��ʂ̐�,value=11,[-], */
	
	CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2 = CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM,	/*!< ,IntegralImage(���͉摜�T�C�Y�c��1/2),value=11,[-], */
	CB_FPE_HAAR_LIKE_FTR_CHK_EDGE,											/*!< ,Haar-like������(�s���G�b�W),value=12,[-], */
	CB_FPE_HAAR_LIKE_FTR_VERT_EDGE,											/*!< ,Haar-like������(�c�G�b�W),value=13,[-], */
	CB_FPE_HAAR_LIKE_FTR_HORI_EDGE,											/*!< ,Haar-like������(���G�b�W),value=14,[-], */
	CB_FPE_IMG_Y32_SIZE_ZOOMOUT2x2_MAXNUM,									/*!< ,���͉摜�T�C�Y�c��1/2,value=15,[-], */

	CB_FPE_IMG_NORMAL = CB_FPE_IMG_Y32_SIZE_ZOOMOUT2x2_MAXNUM,				/*!< ,���͉摜�Ɠ����T�C�Y,value=15,[-], */
	CB_FPE_INTEGRAL_IMG_NORMAL,												/*!< ,IntegralImage(���͉摜�Ɠ����T�C�Y),value=16,[-], */
	CB_FPE_IMG_EDGE_RHO_NORMAL,												/*!< ,�G�b�W���x�摜(���͉摜�Ɠ����T�C�Y),value=17,[-], */
	CB_FPE_IMG_EDGE_ANGLE_NORMAL,											/*!< ,�G�b�W�p�x�摜(���͉摜�Ɠ����T�C�Y),value=18,[-], */
	CB_FPE_IMG_EDGE_AC_NORMAL,												/*!< ,�G�b�W�p�x�R�[�h�摜(���͉摜�Ɠ����T�C�Y),value=19,[-], */
	CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL,									/*!< ,Haar-like������(�s���G�b�W)(���͉摜�Ɠ����T�C�Y),value=20,[-], */
	CB_FPE_IMG_SIDEMARKER_SIZE_CHG_MAXNUM,									/*!< ,�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή��p�̉摜�^�C�v�ő�l,value=21,[-], */

	CB_FPE_IMG_MAXNUM = CB_FPE_IMG_SIDEMARKER_SIZE_CHG_MAXNUM				/*!< ,�摜�ő吔,value=21,[-], */
};

/**
 * @brief	Haar-like�����ʂ̃^�C�v
 */
enum enum_CB_FPE_HAAR_LIKE_TYPE
{
	CB_FPE_HAAR_LIKE_CHK_EDGE = 0,	/*!< ,�s���G�b�W��Haar-like������,value=0,[-], */
	CB_FPE_HAAR_LIKE_VERT_EDGE,		/*!< ,�c�G�b�W��Haar-like������,value=1,[-], */
	CB_FPE_HAAR_LIKE_HORI_EDGE,		/*!< ,���G�b�W��Haar-like������,value=2,[-], */
	
	CB_FPE_HAAR_LIKE_MAXNUM			/*!< ,Haar-like�^�C�v�̍ő吔,value=2,[-], */
};

/**
 * @brief	�X�L��������(Y����)
 */
enum enum_CB_FPE_SCAN_DIR_Y
{
	CB_FPE_SCAN_DIR_Y_PLUS = 0,	/*!< ,Y�����̃v���X���ɃX�L����,value=0,[-], */
	CB_FPE_SCAN_DIR_Y_MINUS,	/*!< ,Y�����̃}�C�i�X���ɃX�L����,value=1,[-], */

	CB_FPE_SCAN_DIR_Y_MAXNUM	/*!< ,�X�L��������(Y����)�̍ő吔,value=2,[-], */
};

/**
 * @brief	�X�L��������(X����)
 */
enum enum_CB_FPE_SCAN_DIR_X
{
	CB_FPE_SCAN_DIR_X_PLUS = 0,	/*!< ,X�����̃v���X���ɃX�L����,value=0,[-], */
	CB_FPE_SCAN_DIR_X_MINUS,	/*!< ,X�����̃}�C�i�X���ɃX�L����,value=1,[-], */

	CB_FPE_SCAN_DIR_X_MAXNUM	/*!< ,�X�L��������(X����)�̍ő吔,value=,2,[-], */
};

/**
 * @brief	Line�t�B���^�̃��[�h
 */
enum enum_CB_FPE_LINE_FLT_MODE
{
	CB_FPE_LINE_FLT_HORI_EDGE = 0,	/*!< ,���G�b�W�p��Line�t�B���^,value=0,[-], */
	CB_FPE_LINE_FLT_VERT_EDGE,		/*!< ,�c�G�b�W�p��Line�t�B���^,value=1,[-], */

	CB_PFE_LINE_FLT_MODE_MAXNUM		/*!< ,Line�t�B���^�̃��[�h��,value=2,[-], */
};

/**
 * @brief	�����̌X���^�C�v
 */
enum enum_CB_FPE_LINE_SLOPE_TYPE
{
	CB_FPE_LINE_SLOPE_DIAG_TYPE = 0,	/*!< ,�΂�,value=0,[-], */
	CB_FPE_LINE_SLOPE_HORI_TYPE,		/*!< ,����,value=1,[-], */
	CB_FPE_LINE_SLOPE_VERT_TYPE,		/*!< ,����,value=2,[-], */
	
	CB_FPE_LINE_SLOPE_TYPE_MAXNUM		/*!< ,�����̌X���^�C�v��,value=3,[-], */
};

/**
 * @brief	���e�̈��ID
 */
enum enum_CB_FPE_PROJECT_AREA_ID
{
	CB_FPE_PROJECT_UP_AREA = 0,	/*!< ,�㑤�̗̈�ID,value=0,[-], */
	CB_FPE_PROJECT_DOWN_AREA,	/*!< ,�����̗̈�ID,value=1,[-], */
	CB_FPE_PROJECT_LEFT_AREA,	/*!< ,�����̗̈�ID,value=2,[-], */
	CB_FPE_PROJECT_RIGHT_AREA,	/*!< ,�E���̗̈�ID,value=3,[-], */

	CB_FPE_PORJECT_AREA_MAXNUM	/*!< ,���e�̈��ID�ő吔,value=4,[-], */
};

/**
 * @brief	��f�l��X,Y��������ւ����[�h
 */
enum enum_CB_FPE_SHUFFLE_MODE
{
	CB_FPE_NO_SHUFFLE = 0,			/*!< ,����ւ��Ȃ�,value=0,[-], */
	CB_FPE_SHUFFLE,					/*!< ,����ւ�����,value=1,[-], */

	CB_FPE_SHUFFLE_MODE_MAXNUM		/*!< ,����ւ����[�h�̍ő吔,value=2,[-], */
};

/**
 * @brief	�s���^�[�Q�b�g��̃G�b�W�ʒu
 */
enum enum_CB_FPE_EDGE_POS_PATTERN
{
	CB_FPE_UPPER_EDGE_POS_PATTERN = 0,	/*!< ,�㑤�̃G�b�W,value=0,[-], */
	CB_FPE_LOWER_EDGE_POS_PATTERN,		/*!< ,�����̃G�b�W,value=1,[-], */
	CB_FPE_LEFT_EDGE_POS_PATTERN,		/*!< ,�����̃G�b�W,value=2,[-], */
	CB_FPE_RIGHT_EDGE_POS_PATTERN,		/*!< ,�E���̃G�b�W,value=3,[-], */

	CB_FPE_EDGE_POS_PATTERN_MAXNUM		/*!< ,�s���^�[�Q�b�g��̃G�b�W�ʒu�̍ő吔,value=4,[-], */
};

/**
 * @brief	�����_�ʒuIndex(�s���^�[�Q�b�g)
 */
enum enum_CB_FPE_CHK_FTR_PNT_INDEX
{
	CB_FPE_CHK_FTR_PNT_1 = 0L,	/*!< ,�����_1,value=0,[-], */
	CB_FPE_CHK_FTR_PNT_2,		/*!< ,�����_2,value=1,[-], */
	CB_FPE_CHK_FTR_PNT_3,		/*!< ,�����_3,value=2,[-], */
	CB_FPE_CHK_FTR_PNT_4,		/*!< ,�����_4,value=3,[-], */
	CB_FPE_CHK_FTR_PNT_5,		/*!< ,�����_5,value=4,[-], */
	CB_FPE_CHK_FTR_PNT_6,		/*!< ,�����_6,value=5,[-], */
	CB_FPE_CHK_FTR_PNT_7,		/*!< ,�����_7,value=6,[-], */
	CB_FPE_CHK_FTR_PNT_8,		/*!< ,�����_8,value=7,[-], */
	CB_FPE_CHK_FTR_PNT_9,		/*!< ,�����_9,value=8,[-], */

	CB_FPE_CHK_FTR_PNT_MAXNUM	/*!< ,�����_�̍ő吔,value=9,[-], */
};

/**
 * @brief	�����_�ʒuIndex(�s���^�[�Q�b�g(Side Camera))
 */
enum enum_CB_FPE_CHK_FTR_PNT_INDEX_SIDE_CAM
{
	CB_FPE_CHK_FTR_PNT_SC_INVALID = -1L,
	CB_FPE_CHK_FTR_PNT_SC_1 = 0L,	/*!< ,�����_1,value=0,[-], */
	CB_FPE_CHK_FTR_PNT_SC_2,		/*!< ,�����_2,value=1,[-], */
	CB_FPE_CHK_FTR_PNT_SC_3,		/*!< ,�����_3,value=2,[-], */
	CB_FPE_CHK_FTR_PNT_SC_4,		/*!< ,�����_4,value=3,[-], */
	CB_FPE_CHK_FTR_PNT_SC_5,		/*!< ,�����_5,value=4,[-], */
	CB_FPE_CHK_FTR_PNT_SC_8,		/*!< ,�����_8,value=5,[-], */
	CB_FPE_CHK_FTR_PNT_SC_9,		/*!< ,�����_9,value=6,[-], */

	CB_FPE_CHK_FTR_PNT_SC_MAXNUM	/*!< ,�����_�̍ő吔,value=7,[-], */
};

/**
 * @brief	�p�x�R�[�h�ϊ��e�[�u��INDEX
 */
enum enum_CB_FPE_EDGE_AC_TBL_INDEX
{
	CB_FPE_EDGE_AC_TBL_FR_RR = 0L,	/*!< ,�p�x�R�[�h�ϊ��e�[�u��(Fr/Rr�p),value=0,[-], */
	CB_FPE_EDGE_AC_TBL_SL_SR,		/*!< ,�p�x�R�[�h�ϊ��e�[�u��(SL/SR�p),value=1,[-], */

	CB_FPE_EDGE_AC_TBL_MAXNUM		/*!< ,�p�x�R�[�h�ϊ��e�[�u���̍ő吔,value=2,[-], */
};

/**
 * @brief	�p�x�R�[�h
 */
enum enum_CB_FPE_EDGE_AC
{
	CB_FPE_EDGE_AC_RIGHT = 0L,	/*!< ,�E��(0degree����),value=0,[-], */
	CB_FPE_EDGE_AC_LOWER,		/*!< ,����(90degree����),value=1,[-], */
	CB_FPE_EDGE_AC_LEFT,		/*!< ,����(180degree����),value=2,[-], */
	CB_FPE_EDGE_AC_UPPER,		/*!< ,�㑤(270degree����),value=3,[-], */

	CB_FPE_EDGE_AC_MAXNUM		/*!< ,�p�x�R�[�h�̐�,value=4,[-], */
};

/**
 * @brief	�������Ԍv���ʒu
 */
enum enum_CALIB_TIME_TYPE
{
	/* 0-9 */
	E_CALIB_TIME_IMG_TOTAL				= 0,	/*!< ,�S��,value=0,[-], */
	E_CALIB_TIME_CHECKERBOARD_TOTAL,			/*!< ,�s�������_���o,value=1,[-], */
	E_CALIB_TIME_CIRCLE_LINE_TOTAL,				/*!< ,���ۓ����_���o,value=2,[-], */
	E_CALIB_TIME_STORE_RESULT,					/*!< ,���ʊi�[,value=3,[-], */
	E_CALIB_TIME_INTEGRAL_IMG,					/*!< ,Integral Image,value=4,[-], */
	E_CALIB_TIME_HAAR_LIKE_CHK_EDGE,			/*!< ,Haar-like������(�s���G�b�W)�Z�o,value=5,[-], */
	E_CALIB_TIME_HAAR_LIKE_VERT_EDGE,			/*!< ,Haar-like������(�c�G�b�W)�Z�o,value=6,[-], */
	E_CALIB_TIME_HAAR_LIKE_HORI_EDGE,			/*!< ,Haar-like������(���G�b�W)�Z�o,value=7,[-], */
	E_CALIB_TIME_SRCH_CROSS_PNT,				/*!< ,�s���G�b�W��̓����_�T��,value=8,[-], */
	E_CALIB_TIME_SRCH_T_PNT_RI,					/*!< ,�E���G�b�W��̂̓����_�T��,value=9,[-], */
	/* 10-19 */
	E_CALIB_TIME_SRCH_T_PNT_LE,					/*!< ,�����G�b�W��̂̓����_�T��,value=10,[-], */
	E_CALIB_TIME_SRCH_T_PNT_LO,					/*!< ,�����G�b�W��̂̓����_�T��,value=11,[-], */
	E_CALIB_TIME_SRCH_T_PNT_UP,					/*!< ,�㑤�G�b�W��̂̓����_�T��,value=12,[-], */
	E_CALIB_TIME_PREC_POS_CHK,					/*!< ,�����x�ʒu����(�s�������_),value=13,[-], */
	E_CALIB_TIME_4PNT_CROSS,					/*!< ,�����x�ʒu����(�s���G�b�W��̓����_),value=14,[-], */
	E_CALIB_TIME_EST_PNT_EDGE_PTN,				/*!< ,�����x�ʒu����(�s���G�b�W��̓����_�ȊO),value=15,[-], */
	E_CALIB_TIME_SMOOTH,						/*!< ,������,value=16,[-], */
	E_CALIB_TIME_RRF,							/*!< ,Radial Reach Feature,value=17,[-], */
	E_CALIB_TIME_CIRC_RGN,						/*!< ,���ۓ����_���o,value=18,[-], */
	E_CALIB_TIME_CIRC_RGN_FR,					/*!< ,���ۓ����_���o(Fr�p),value=19,[-], */
	/* 20-29 */
	E_CALIB_TIME_SET_SRCH_RGN,					/*!< ,�T���̈�ݒ�,value=20,[-], */
	
	E_CALIB_TIME_MAXNUM							/*!< ,�������Ԍv���ʒu�̍ő吔,value=21,[-], */
};

/********************************************************************
 * �����\���́@��`
 ********************************************************************/

/**
 * @brief	�摜�������A�N�Z�X���
 */
typedef struct tagCB_FPE_IMG_ACCESS_INFO
{
	slong			nXSize;				/*!< ,�摜������X�����T�C�Y,1<=value<?,[pixel], */
	slong			nYSize;				/*!< ,�摜������Y�����T�C�Y,1<=value<?,[pixel], */
	schar			*pnAddrY8;			/*!< ,8bitY�摜�������̐擪�A�h���X,-,[-], */
	sshort			*pnAddrY16;			/*!< ,16bitY�摜�������̐擪�A�h���X,-,[-], */
	slong			*pnAddrY32;			/*!< ,32bitY�摜�������̐擪�A�h���X,-,[-], */
	CB_RECT_RGN		*ptRgn;				/*!< ,�����̈�\���̂ւ̃|�C���^,-,[-], */
} CB_FPE_IMG_ACCESS_INFO;

/**
 * @brief	Vector
 */
typedef struct tagCB_FPE_VECTOR_2D
{
	slong	nX;	/*!< ,X����,?<=value<=?,[-], */
	slong	nY;	/*!< ,Y����,?<=value<=?,[-], */
} CB_FPE_VECTOR_2D;

/**
 * @brief	Vector(float)
 */
typedef struct tagCB_FPE_VECTOR_2D_F
{
	float_t	fX;	/*!< ,X����,?<=value<=?,[-], */
	float_t	fY;	/*!< ,Y����,?<=value<=?,[-], */
} CB_FPE_VECTOR_2D_F;

/**
 * @brief	�����W��
 */
typedef struct tagCB_FPE_LINE_COEFF
{
	double_t							dSlope;		/*!< ,�X��,?<=value<=?,[-], */
	double_t							dIntercept;	/*!< ,�ؕ�,?<=value<=?,[-], */
	enum enum_CB_FPE_LINE_SLOPE_TYPE	nTypeSlope;	/*!< ,�����̌X���^�C�v,?,[-], */
} CB_FPE_LINE_COEFF;

/**
 * @brief	�����W��(float)
 */
typedef struct tagCB_FPE_LINE_COEFF_F
{
	float_t								fSlope;		/*!< ,�X��,?<=value<=?,[-], */
	float_t								fIntercept;	/*!< ,�ؕ�,?<=value<=?,[-], */
	enum enum_CB_FPE_LINE_SLOPE_TYPE	nTypeSlope;	/*!< ,�����̌X���^�C�v,?,[-], */
} CB_FPE_LINE_COEFF_F;

/**
 * @brief	�J�����p�����[�^���
 */
typedef struct tagCB_FPE_CAM_PRM
{
	slong				nCamID;			/*!< ,�J����ID,0<value<=?,[-], */
	CB_CG_EXTRINSIC_PRM	tExtrinsicPrm;	/*!< ,�O���p�����[�^,-,[-], */
	CB_CG_INTRINSIC_PRM	tIntrinsicPrm;	/*!< ,�����p�����[�^,-,[-], */
	CB_CG_DIST_PRM		tDistPrm;		/*!< ,�c�݃p�����[�^,-,[-], */
} CB_FPE_CAM_PRM;

/**
 * @brief	�J�����p�����[�^�A�덷�e�[�u��
 */
typedef struct tagCB_FPE_CAM_ERR_TBL
{
	double_t	dErrPitch;			/*!< ,Pitch�̌덷,?<=value<=?,[deg], */
	double_t	dErrRoll;			/*!< ,Roll�̌덷,?<=value<=?,[deg], */
	double_t	dErrYaw;			/*!< ,Yaw�̌덷,?<=value<=?,[deg], */
	double_t	dErrOffsetWldX;		/*!< ,���t���ʒu(X)�̌덷,?<=value<=?,[m], */
	double_t	dErrOffsetWldY;		/*!< ,���t���ʒu(Y)�̌덷,?<=value<=?,[m], */
	double_t	dErrOffsetWldZ;		/*!< ,���t���ʒu(Z)�̌덷,?<=value<=?,[m], */
	double_t	dErrCntPntHori;		/*!< ,���������ʒu�̌덷,?<=value<=?,[pixel], */
	double_t	dErrCntPntVert;		/*!< ,���������ʒu�̌덷,?<=value<=?,[pixel], */
} CB_FPE_CAM_ERR_TBL;

/**
 * @brief	�J�����p�����[�^(�덷)���
 */
typedef struct tagCB_FPE_CAM_ERR_INFO
{
	CB_FPE_CAM_ERR_TBL	tErrTbl[CB_FPE_ERR_TBL_MAXNUM];	/*!< ,���e�덷�e�[�u��,-,[-], */
} CB_FPE_CAM_ERR_INFO;


/**
 * @brief	�����p�^�[�����\����
 */
typedef struct tagCB_FPE_IMG_PATTERN_INFO
{
	enum enum_CB_IMG_CHKBOARD_PLACEMENT		nFlagPlacement;		/*!< ,�p�^�[���̌���,-,[-], */
	enum enum_CB_IMG_PATTERN_TYPE			nTypePattern;		/*!< ,�p�^�[����Type,-,[-], */
	enum enum_CB_IMG_ALGORITHM				nTypeAlgorithm;		/*!< ,�p�^�[�����o�A���S���Y��,-,[-], */
	uchar	nDoubleType;
	uchar									nlayoutType;		/*!< ,���C�A�E�g���,-,[-], */											
	uchar									nCenterOffsetType;	/*!< ,�I�t�Z�b�g�^�C�v,-,[-], */											
} CB_FPE_IMG_PATTERN_INFO;

/**
 * @brief	�e�p�^�[���ɑ΂���T���̈�
 */
typedef struct tagCB_FPE_SRCH_RGN_INFO_EACH_PATTERN
{
	slong				nNumPnt;								/*!< ,�T���̈���\������_�̐�,1<=value<?,[-], */
	CB_CG_PNT_WORLD		tPntWld[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,�T���̈���\������_���W,-,[-], */
	CB_IMG_POINT		tPntImg[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,�T���̈���\������_���W,-,[-], */
	CB_RECT_RGN			tRgnImg;								/*!< ,�T���̈�,-,[-], */
	CB_RECT_RGN			tRgnImgZoomOut;							/*!< ,�T���̈�(�k����),-,[-], */
	CB_RECT_RGN			tRgnImgHLsrc;							/*!< ,�T���̈�(Haar-like�l��),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLsrc;					/*!< ,�T���̈�(�k�����̃\�[�X),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLChkEdge;				/*!< ,�T���̈�(�k�����AHaar-like(�s��)�l��),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLVertEdge;				/*!< ,�T���̈�(�k�����AHaar-like(�c�G�b�W)�l��),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLHoriEdge;				/*!< ,�T���̈�(�k�����AHaar-like(���G�b�W)�l��),-,[-], */
	ulong				nNumCenterPnt;							/*!< ,�i�[���W��(�J�����p�����[�^����p),-,[-], */
	CB_CG_PNT_WORLD		tCenterPntWld[CB_FPE_CENTER_PNT_MAXNUM];	/*!< ,�i�[���W(�J�����p�����[�^����p),-,[-], */
	CB_FPE_IMG_PATTERN_INFO		tPtnInfo;						/*!< ,�����p�^�[�����,-,[-], */
} CB_FPE_SRCH_RGN_INFO_EACH_PATTERN;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

/**
 * @brief	���ۂɑ΂���T���̈�
 */
typedef struct tagCB_FPE_CIRCLE_SRCH_RGN_INFO
{
	CB_IMG_POINT		tCircleSrchRgnTL;						/*!< ,���ےT���̈�(����),-,[-], */
	CB_IMG_POINT		tCircleSrchRgnBR;						/*!< ,���ےT���̈�(�E��),-,[-], */

#ifdef CB_FPE_CSW_DEBUG_ON
	CB_IMG_POINT		tCircleErrPntTbl[CB_FPE_ERR_TBL_MAXNUM];/*!< ,���ے��S���W(�J�����덷�l��),-,[-], */
#endif /* CB_FPE_CSW_DEBUG_ON */

} CB_FPE_CIRCLE_SRCH_RGN_INFO;

#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

/**
 * @brief	���E�s���ɑ΂���T���̈�
 */
typedef struct tagCB_FPE_ADD_CHK_SRCH_RGN_INFO
{
	CB_RECT_RGN		tAddChkSrchRgn;						/*!< ,�T���̈�,-,[-], */
#ifdef CB_FPE_CSW_DEBUG_ON
	CB_IMG_POINT		tAddChkErrPntTbl[CB_FPE_ERR_TBL_MAXNUM];/*!< ,���S���W(�J�����덷�l��),-,[-], */
#endif /* CB_FPE_CSW_DEBUG_ON */

} CB_FPE_ADD_CHK_SRCH_RGN_INFO;

/**
 * @brief	�ǉ��p�^�[���ɑ΂���T���̈�
 */
typedef struct tagCB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN
{
	slong				nNumPnt;								/*!< ,�T���̈���\������_�̐�,1<=value<?,[-], */
	CB_CG_PNT_WORLD		tPntWld[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,�T���̈���\������_���W(World),-,[-], */
	CB_IMG_POINT		tPntImg[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,�T���̈���\������_���W(Image),-,[-], */
	CB_RECT_RGN			tRgnImg;								/*!< ,�T���̈�,-,[-], */
	CB_RECT_RGN			tRgnImgZoomOut;							/*!< ,�T���̈�(�k����),-,[-], */
	CB_RECT_RGN			tRgnImgNormalSize;						/*!< ,�T���̈�@�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή��itRgnImg�Ɠ���̒l��ݒ肷��j,-,[-], */
	CB_RECT_RGN			tRgnImgRRF;								/*!< ,RRF�p�T���̈�,-,[-], */
	CB_RECT_RGN			tRgnImgRRFZoomOut;						/*!< ,RRF�p�T���̈�(�k����),-,[-], */

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN													/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
	CB_CG_PNT_WORLD					tPntWldCircle[CB_ADD_PATTERN_PNT_POS_MAXNUM];	/*!< ,���ے��S���W(���E���W�n),-,[-], */
	CB_FPE_CIRCLE_SRCH_RGN_INFO		tPntImgCircle[CB_ADD_PATTERN_PNT_POS_MAXNUM];	/*!< ,���ۂɂ�����T���̈�,-,[-], */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */												/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

	CB_RECT_RGN			tRgnImgHLsrc;												/*!< ,���E�s���T���̈�(Haar-like�l��),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLsrc;										/*!< ,���E�s���T���̈�(�k�����̃\�[�X),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLChkEdge;									/*!< ,���E�s���T���̈�(�k�����AHaar-like(�s��)�l��),-,[-], */
	CB_RECT_RGN			tRgnImgNormalHLsrc;											/*!< ,���E�s���T���̈�@�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή��itRgnImgHLsrc�Ɠ���̒l��ݒ肷��j,-,[-], */
	CB_CG_PNT_WORLD					tPntWldSideChkCenter;							/*!< ,���E�s�����S���W(���E���W�n),-,[-], */
	CB_FPE_ADD_CHK_SRCH_RGN_INFO	tPntImgSideChkCenter;							/*!< ,���E�s���㉺�_�T���̈�,-,[-], */
	CB_FPE_ADD_CHK_SRCH_RGN_INFO	tPntImgSideChkCenterZoomOut;					/*!< ,���E�s���㉺�_�T���̈�,-,[-], */
	CB_FPE_ADD_CHK_SRCH_RGN_INFO	tPntImgSideChkCenterNormalSize;					/*!< ,���E�s���㉺�_�T���̈�@�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή��itPntImgSideChkCenter�Ɠ���̒l��ݒ肷��j,-,[-], */
	CB_FPE_IMG_PATTERN_INFO		tPtnInfo;											/*!< ,�����p�^�[�����,-,[-], */

} CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN;

/*
 * @brief	�e�J�����ɂ�����T���̈�
 */
typedef struct tagCB_FPE_SRCH_RGN_INFO_EACH_CAM
{
	slong									nFlagValidInfo4Pattern;								/*!< ,�s���p�^�[�����̗L���t���O,0<=value<=1,[-], */
	slong									nFlagValidInfo4AddPattern;							/*!< ,���Ɗۃp�^�[�����̗L���t���O,0<=value<=1,[-], */
	CB_FPE_SRCH_RGN_INFO_EACH_PATTERN		tSrchRgnInfo4Pattern[CB_PATTERN_POS_MAXNUM];		/*!< ,�e�p�^�[���ɂ�����T���̈�,-,[-], */
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_MAXNUM];	/*!< ,�ǉ��p�^�[���ɂ�����T���̈�,-,[-], */
} CB_FPE_SRCH_RGN_INFO_EACH_CAM;

/**
 * @brief	Haar-Like���
 */
typedef struct tagCB_FPE_HAAR_LIKE_FTR
{
	slong	nXSize;		/*!< ,X�����T�C�Y,1<=value<?,[pixel], */
	slong	nYSize;		/*!< ,Y�����T�C�Y,1<=value<?,[pixel], */
	slong*	pnFtrValue;	/*!< ,�����ʂւ̃|�C���^,-,[-], */
} CB_FPE_HAAR_LIKE_FTR;

/**
 * @brief	�P�x�l�v�Z���(�t�����g�J�����E���A�J�����g�p���̍��E�}�[�J�ɑ΂��Ďg�p)
 */
typedef struct tagCB_FPE_BRIGHT_CALC_INFO
{
	slong*		pnFtrValue[2];		/*!< ,�P�x�v�Z�ɗp����|�C���^, */
	slong		nXSize;				/*!< ,X�����T�C�Y,1<=value<?,[pixel], */
	slong		nHaarXSize;			/*!< ,Haar-like��`X�����T�C�Y,1<=value<?,[pixel], */
	slong		CalcBrightness[2];	/*!< ,�ݒ�̈���̋P�x�l���v, */
} CB_FPE_BRIGHT_CALC_INFO;

/**
 * @brief	�T�����ʓ_���
 */
typedef struct tagCB_FPE_SRCH_RSLT_PNT_INFO
{
	slong				nNumCrossPnt;									/*!< ,�\���_��,0<=value<?,[-], */
	slong				nNumTPntLower;									/*!< ,�����s���_��,0<=value<?,[-], */
	slong				nNumTPntUpper;									/*!< ,�㑤�s���_��,0<=value<?,[-], */
	slong				nNumTPntLeft;									/*!< ,�����s���_��,0<=value<?,[-], */
	slong				nNumTPntRight;									/*!< ,�E���s���_��,0<=value<?,[-], */
	CB_IMG_POINT		tCrossPnt[CB_FPE_CROSS_PNT_MAXNUM];				/*!< ,�\���_���W,-,[-], */
	CB_IMG_POINT		tTPntLower[CB_FPE_T_PNT_LOWER_MAXNUM];			/*!< ,�����s���_���W,-,[-], */
	CB_IMG_POINT		tTPntUpper[CB_FPE_T_PNT_UPPER_MAXNUM];			/*!< ,�㑤�s���_���W,-,[-], */
	CB_IMG_POINT		tTPntLeft[CB_FPE_T_PNT_LEFT_MAXNUM];			/*!< ,�����s���_���W,-,[-], */
	CB_IMG_POINT		tTPntRight[CB_FPE_T_PNT_RIGHT_MAXNUM];			/*!< ,�E���s���_���W,-,[-], */
	CB_IMG_POINT		tCrossPntHalf[CB_FPE_CROSS_PNT_MAXNUM];			/*!< ,�\���_���W(1/2�T�C�Y),-,[-], */
	CB_IMG_POINT		tTPntLowerHalf[CB_FPE_T_PNT_LOWER_MAXNUM];		/*!< ,�����s���_���W(1/2�T�C�Y),-,[-], */
	CB_IMG_POINT		tTPntUpperHalf[CB_FPE_T_PNT_UPPER_MAXNUM];		/*!< ,�㑤�s���_���W(1/2�T�C�Y),-,[-], */
	CB_IMG_POINT		tTPntLeftHalf[CB_FPE_T_PNT_LEFT_MAXNUM];		/*!< ,�����s���_���W(1/2�T�C�Y),-,[-], */
	CB_IMG_POINT		tTPntRightHalf[CB_FPE_T_PNT_RIGHT_MAXNUM];		/*!< ,�E���s���_���W(1/2�T�C�Y),-,[-], */
	t_cb_img_CenterPos	tCrossPntPrecise[CB_FPE_CROSS_PNT_MAXNUM];		/*!< ,�\���_���W(�T�u�s�N�Z��),-,[-], */
	t_cb_img_CenterPos	tTPntLowerPrecise[CB_FPE_T_PNT_LOWER_MAXNUM];	/*!< ,�����s���_���W(�T�u�s�N�Z��),-,[-], */
	t_cb_img_CenterPos	tTPntUpperPrecise[CB_FPE_T_PNT_UPPER_MAXNUM];	/*!< ,�㑤�s���_���W(�T�u�s�N�Z��),-,[-], */
	t_cb_img_CenterPos	tTPntLeftPrecise[CB_FPE_T_PNT_LEFT_MAXNUM];		/*!< ,�����s���_���W(�T�u�s�N�Z��),-,[-], */
	t_cb_img_CenterPos	tTPntRightPrecise[CB_FPE_T_PNT_RIGHT_MAXNUM];	/*!< ,�E���s���_���W(�T�u�s�N�Z��),-,[-], */
} CB_FPE_SRCH_RSLT_PNT_INFO;

/**
 * @brief	�T�����ʏ��(�ǉ��p�^�[��)
 */
typedef struct tagCB_FPE_SRCH_RSLT_ADD_PNT_INFO
{
	slong				nNumCenterPnt;										/*!< ,�~�̒��_�̐�,0<=value<,[-], */
	CB_IMG_POINT		tCenterPnt[CB_ADD_PATTERN_PNT_POS_MAXNUM];			/*!< ,�~�̒��_,-,[-], */
	t_cb_img_CenterPos	tCenterPntPrecise[CB_ADD_PATTERN_PNT_POS_MAXNUM];	/*!< ,�~�̒��_(�T�u�s�N�Z��),-,[-], */
} CB_FPE_SRCH_RSLT_ADD_PNT_INFO;

/**
 * @brief	�T�����ʏ��
 */
typedef struct tagCB_FPE_SRCH_RSLT_INFO
{
	CB_FPE_SRCH_RSLT_PNT_INFO			tSrchRsltPntInfo[CB_PATTERN_POS_MAXNUM];			/*!< ,�T�����ʓ_���,-,[-], */
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO		tSrchRsltAddPntInfo[CB_ADD_PATTERN_POS_MAXNUM];		/*!< ,�T�����ʓ_(�ǉ��p�^�[���F�F����),-,[-], */
	CB_FPE_SRCH_RSLT_PNT_INFO			tSrchRsltAddChkPntInfo[CB_ADD_PATTERN_POS_MAXNUM];	/*!< ,�T�����ʓ_(�ǉ��p�^�[���F�F�s��),-,[-], */
} CB_FPE_SRCH_RSLT_INFO;

/**
 * @brief	���_���
 */
typedef struct tagCB_FPE_CAND_PNT_INFO
{
	slong			nFtrValue;	/*!< ,������,?<=value<=?,[-], */
	CB_IMG_POINT	tPnt;		/*!< ,���_���W,-,[-], */
} CB_FPE_CAND_PNT_INFO;

/**
 * @brief	���~�̈���
 */
typedef struct tagCB_FPE_CAND_CIRCLE_INFO
{
	slong	nIdxUpperLabel;	/*!< ,�㑤�ۂ��������x��Index,1<=value<=255,[-], */
	slong	nIdxLowerLabel;	/*!< ,�����ۂ��������x��Index,1<=value<=255,[-], */
	float_t	fDistance;		/*!< ,����,0.0<=value,[-], */
} CB_FPE_CAND_CIRCLE_INFO;

/**
 * @brief	���ƊۂɊւ���g�ݍ��킹���
 */
typedef struct
{
	uchar	nIdxUpperCircle;	/*!< ,�㑤�ۂ�����Index,?<=value<=?,[-], */
	uchar	nIdxLowerCircle;	/*!< ,�����ۂ�����Index,?<=value<=?,[-], */
	uchar	nIdxLine;			/*!< ,��������Index,?<=value<=?,[-], */
} CB_FPE_CIRCLE_LINE_COMB;

/**
 * @brief	���p�^�[���ɂ���������
 */
typedef struct
{
	uchar				nIdxLine;			/*!< ,��������Index,?<=value<=?,[-], */
	float_t				fLength;			/*!< ,����,0.0<=value,[-], */
	CB_IMG_POINT_F		tUpperCenterPnt;	/*!< ,�㑤�ۂ̒��S�ʒu,-,[-], */
	CB_IMG_POINT_F		tLowerCenterPnt;	/*!< ,�����ۂ̒��S�ʒu,-,[-], */
	CB_IMG_POINT_F		tMiddleCenterPnt;	/*!< ,�����̒��S�ʒu,-,[-], */
	CB_FPE_LINE_COEFF_F	tUpperLineCoeff;	/*!< ,�㑤�����W��,-,[-], */
	CB_FPE_LINE_COEFF_F	tLowerLineCoeff;	/*!< ,���������W��,-,[-], */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY		/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
	slong				nWidthUpper;		/*!< ,�����x����(��[),?<=value<=?,[-], */
	slong				nWidthLower;		/*!< ,�����x����(���[),?<=value<=?,[-], */
	slong				nWidthMiddle;		/*!< ,�����x����(����),?<=value<=?,[-], */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */	/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
} CB_FPE_LINE_PART_INFO;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY		/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
/**
 * @brief	�P�x����̍��W���
 */
typedef struct tagCB_FPE_CBR_POINT_INFO
{
	CB_IMG_POINT_F		tLabelUpperPnt;		/*!< ,���x����[���S���W,-,[-], */
	CB_IMG_POINT_F		tLabelLowerPnt;		/*!< ,���x�����[���S���W,-,[-], */
	slong				nSrchWidth;			/*!< ,�P�x����T���̈敝,-,[-], */
	slong				nFlagVertical;		/*!< ,�P�x����T�����C�������t���O,0<=value<=1,[-], */
	float_t				fSrchSlope;			/*!< ,�P�x����T�����C���X��,-,[-], */
} CB_FPE_CBR_POINT_INFO;

/**
 * @brief	�P�x����̋P�x���
 */
typedef struct tagCB_FPE_CBR_BR_INFO
{
	float_t				fBrTblUpper[ CB_FPE_PRM_HIST_BIN_NUM ];	/*!< ,�T���_�P�x�l(��[),-,[-], */
	float_t				fBrTblLower[ CB_FPE_PRM_HIST_BIN_NUM ];	/*!< ,�T���_�P�x�l(���[),-,[-], */
	float_t				fAvgUpper;			/*!< ,�T���̈敽�ϋP�x�l(��[),-,[-], */
	float_t				fAvgLower;			/*!< ,�T���̈敽�ϋP�x�l(���[),-,[-], */
} CB_FPE_CBR_BR_INFO;

/**
 * @brief	�P�x����̋P�x���
 */
typedef struct tagCB_FPE_CBR_INPUT
{
	CB_FPE_CBR_POINT_INFO	tBrPointTbl[CB_ADD_PATTERN_POS_MAXNUM];
	slong					nImgSrcXSize;
	slong					nImgRRFXSize;
	float_t					fAvgBlackArea;		/*!< ,���̈敽�ϋP�x�l(��[�� + �� + ���[��),-,[-], */
} CB_FPE_CBR_INPUT;

#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */	/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

/*
 * @brief	������
 */
typedef struct tagCB_FPE_CTRL
{
	CB_FPE_CAM_PRM					tCamPrm[E_CB_SELFCALIB_CAMPOS_MAX];				/*!< ,�J�����p�����[�^,-,[-], */
	CB_FPE_CAM_ERR_INFO				tCamErrInfo[E_CB_SELFCALIB_CAMPOS_MAX];			/*!< ,�J�����p�����[�^�덷,-,[-], */
	CB_FPE_SRCH_RGN_INFO_EACH_CAM	tSrchRgnInfo4Cam[E_CB_SELFCALIB_CAMPOS_MAX];	/*!< ,�e�J�����ɂ�����T���̈�,-,[-], */
} CB_FPE_CTRL;

/**
 * @brief	�o�͏��
 */
typedef struct tagCB_FPE_RESULT
{
	CB_FPE_SRCH_RSLT_INFO	tSrchRsltInfo[E_CB_SELFCALIB_CAMPOS_MAX];	/*!< ,�T�����ʏ��,-,[-], */
} CB_FPE_RESULT;

/**
 * @brief	�p�����[�^
 */
typedef struct tagCB_FPE_PRM
{
	slong	nXmagZoomOut;	/*!< ,�k����(X����),1<=value<?,[-], */
	slong	nYmagZoomOut;	/*!< ,�k����(Y����),1<=value<?,[-], */
	/* ===== Haar-like ===== */
	slong	nXSizeHaarChkEdgeBlock;	/*!< ,�s��Haar-like��1�u���b�N�ɂ�����X�����T�C�Y,1<=value<?,[pixel], */
	slong	nYSizeHaarChkEdgeBlock;	/*!< ,�s��Haar-like��1�u���b�N�ɂ�����Y�����T�C�Y,1<=value<?,[pixel], */
	slong	nXSizeHaarVertEdgeBlock;	/*!< ,����Haar-like��1�u���b�N�ɂ�����X�����T�C�Y,1<=value<?,[pixel], */
	slong	nYSizeHaarVertEdgeBlock;	/*!< ,����Haar-like��1�u���b�N�ɂ�����Y�����T�C�Y,1<=value<?,[pixel], */
	slong	nXSizeHaarHoriEdgeBlock;	/*!< ,����Haar-like��1�u���b�N�ɂ�����X�����T�C�Y,1<=value<?,[pixel], */
	slong	nYSizeHaarHoriEdgeBlock;	/*!< ,����Haar-like��1�u���b�N�ɂ�����Y�����T�C�Y,1<=value<?,[pixel], */
	/* ===== Radial Reach ===== */
	slong	nLengthReach;			/*!< ,���[�`�̍ő咷��,1<=value<?,[pixel], */
	slong	nDiffIntensity;			/*!< ,�P�x�l�̍�,0<=value<?,[-], */
	slong	nDiffIntensity4Retry;	/*!< ,�P�x�l�̍�(���g���C��),0<=value<?,[-], */
	/* ===== �~�̒��_���o ===== */
	slong	nCntReach;				/*!< ,��������𖞂��������[�`�̐�,0<=value<?,[-], */
	float_t	fAspectRatio;			/*!< ,�}�`�̃A�X�y�N�g��,0.0<=value<=1.0,[-], */
	float_t	fLengthRatio;			/*!< ,�����̔�,0.0<=value<=1.0,[-], */
	/* ===== �s���^�[�Q�b�g���̓����_���o ===== */
	slong	nXSizePeri;				/*!< ,���ӗ̈��X�����T�C�Y,0<=value<?,[pixel], */
	slong	nYSizePeri;				/*!< ,���ӗ̈��Y�����T�C�Y,0<=value<?,[pixel], */
	slong	nXSizeScanRgn;			/*!< ,�X�L�����̈��X�����T�C�Y,1<value<?,[pixel], */
} CB_FPE_PRM;

/**
 * @brief	�������
 */
typedef struct tagCB_FPE_INFO
{
	IMPLIB_IMGID			nImg[CB_FPE_IMG_MAXNUM];			/*!< ,���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-], */
	CB_FPE_HAAR_LIKE_FTR	tHaarLike[CB_FPE_HAAR_LIKE_MAXNUM];	/*!< ,Haar-like������,-,[-], */
	CB_FPE_CTRL				tCtrl;								/*!< ,������,-,[-], */
	CB_FPE_RESULT			tRslt;								/*!< ,���ʏ��,-,[-], */
} CB_FPE_INFO;

/**
 * @brief	���[�N���
 */
typedef struct tagCB_FPE_WORK_INFO
{
	slong					nNumBasePnt4Upper;											/*!< ,��/����T���_�T�����̊�_��,-,[-], */
	CB_IMG_POINT			tBasePnt4Upper[3];											/*!< ,��/����T���_�T�����̊�_,-,[-], */
	slong					nNumBasePnt4Lower;											/*!< ,��/����T���_�T�����̊�_��,-,[-], */
	CB_IMG_POINT			tBasePnt4Lower[3];											/*!< ,��/����T���_�T�����̊�_,-,[-], */
	slong					nTblLabelMinX[256];											/*!< ,���x���̍ŏ�X���W,0<=value<?,[pixel], */
	slong					nTblLabelMaxX[256];											/*!< ,���x���̍ő�X���W,0<=value<?,[pixel], */
	slong					nTblLabelMinY[256];											/*!< ,���x���̍ŏ�Y���W,0<=value<?,[pixel], */
	slong					nTblLabelMaxY[256];											/*!< ,���x���̍ő�Y���W,0<=value<?,[pixel], */
	slong					nTblAreaLabel[256];											/*!< ,���x���̖ʐ�,0<=value<?,[pixel^2], */
	IMPLIB_IPLOGravityTbl	tTblGravity[256];											/*!< ,���x�����Əd�S���W,-,[-], */
	slong					nTblMedian[CB_FPE_PRM_PROJECTION_DATA_NUM];					/*!< ,Median,0<=value<?,[-], */
	slong					nTblProjGO[CB_FPE_PRM_PROJECTION_DATA_NUM];					/*!< ,���e�l,0<=value<?,[-], */
	slong					nTblMedian4EdgeOfPtn[CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];	/*!< ,Median,0<=value<=255,[-], */
	IMPLIB_IPGOFeatureTbl	tGOFtrTbl;													/*!< ,�Z�W�����ʃe�[�u��,-,[-], */

	uchar					nPixelDataUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];	/*!< ,�����x�ʒu���ߗp�̉�f�l�z��(�c����),?,[-], */
	uchar					nPixelDataLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];	/*!< ,�����x�ʒu���ߗp�̉�f�l�z��(������),?,[-], */
	sshort					nEdgeRhoUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];		/*!< ,�����x�ʒu���ߗp�̃G�b�W���x�z��(�c����),?,[-], */
	sshort					nEdgeRhoLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];		/*!< ,�����x�ʒu���ߗp�̃G�b�W���x�z��(������),?,[-], */
	CB_IMG_POINT			tEdgePntUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];											/*!< ,�����x�ʒu���ߗp�̃G�b�W�_�z��(�c����),-,[-], */
	CB_IMG_POINT			tEdgePntLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];											/*!< ,�����x�ʒu���ߗp�̃G�b�W�_�z��(������),-,[-], */
	t_cb_img_CenterPos		tEdgePrecPntUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];										/*!< ,�����x�ʒu���ߗp�̃G�b�W�_�z��(�c����),-,[-], */
	t_cb_img_CenterPos		tEdgePrecPntLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];										/*!< ,�����x�ʒu���ߗp�̃G�b�W�_�z��(������),-,[-], */
	CB_FPE_IMG_ACCESS_INFO	tPixelDataUL,																			/*!< ,�P�x�l�i�[�摜�������A�N�Z�X���(�c����),-,[-], */
							tPixelDataLR;																			/*!< ,�P�x�l�i�[�摜�������A�N�Z�X���(������),-,[-], */
	CB_FPE_IMG_ACCESS_INFO	tEdgeRhoUL,																				/*!< ,�G�b�W���x�i�[�摜�������A�N�Z�X���(�c����),-,[-], */
							tEdgeRhoLR;																				/*!< ,�G�b�W���x�i�[�摜�������A�N�Z�X���(������),-,[-], */

	CB_IMG_POINT_F	tCircleCandPnt[CB_FPE_CIRCLE_CAND_POINT_MAXNUM];	/*!< ,���ۓ����_���ʒu�i�[�e�[�u��,-,[-], */
	slong			nIdxLabelCandPnt[CB_FPE_CIRCLE_CAND_POINT_MAXNUM];	/*!< ,���ی��ʒu���x��Index�e�[�u��,1<=value<=255,[-], */

	CB_FPE_CAND_CIRCLE_INFO	tCircleCandInfo[CB_FPE_CIRCLE_CAND_POINT_MAXNUM];	/*!< ,���ی��ʒu���e�[�u��,-,[-], */

	CB_FPE_LINE_PART_INFO	tLinePartInfo[256L];	/*!< ,���ɊY�����郉�x��No.�i�[�p�e�[�u��,1<=value<=255,[-], */
	CB_FPE_CIRCLE_LINE_COMB	tCircleLineComb[256L];	/*!< ,���ۑg�ݍ��킹�e�[�u��,-,[-], */

	IMPLIB_IPErrorTbl	tErrTbl;	/*!< ,IMP�G���[���Ǘ��e�[�u��,-,[-], */
} CB_FPE_WORK_INFO;


/**
 * @brief	�f�o�b�O���(�O���b�h���)
 */
typedef struct tagCB_FPE_DBG_GRID_INFO
{
	CB_CG_PNT_WORLD	tPntWld[CB_FPE_GRID_NUM_WLD_Y][CB_FPE_GRID_NUM_WLD_X];	/*!< ,�O���b�h���`��p�̃|�C���g(World),-,[-], */
	CB_IMG_POINT	tPntImg[CB_FPE_GRID_NUM_WLD_Y][CB_FPE_GRID_NUM_WLD_X];	/*!< ,�O���b�h���`��p�̃|�C���g(Image),-,[-], */
} CB_FPE_DBG_GRID_INFO;

/**
 * @brief	�f�o�b�O���
 */
typedef struct tagCB_FPE_DBG_INFO
{
#ifdef CB_FPE_CSW_DEBUG_ON
	CB_COLOR_PALETTE_YUV	tColorYUV;	/*!< ,YUV�F,-,[-], */
#endif /* CB_FPE_CSW_DEBUG_ON */
	CB_FPE_DBG_GRID_INFO	tGridInfo[E_CB_SELFCALIB_CAMPOS_MAX];	/*!< ,�O���b�h���,-,[-], */
	CB_IMG_POINT	tPntCandidate[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM][2];		/*!< ,�p�^�[�����̏㉺�[���W(3�����ڂ�0�F��,1�F��),-,[-], */
	float_t			tScore[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM];					/*!< ,�p�^�[�����̋P�x����X�R�A,-,[-], */
	float_t			tSimilarity[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM];			/*!< ,�p�^�[�����̔��̈�ގ��x,-,[-], */
	float_t			tContrast[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM];				/*!< ,�p�^�[�����̔��E���̈�ԃR���g���X�g,-,[-], */
	slong			nNum[CB_ADD_PATTERN_POS_MAXNUM];												/*!< ,�p�^�[����␔,-,[-], */
} CB_FPE_DBG_INFO;


/********************************************************************
 * �����O���[�o���ϐ��@�錾
 ********************************************************************/

/* ���� */
static CB_FPE_INFO	m_FPE_tInfo;	/*!< ,�����f�[�^,-,[-], */
static CB_FPE_PRM	m_FPE_tPrm;		/*!< ,�p�����[�^���,-,[-], */

/* ���[�N */
static CB_FPE_WORK_INFO	m_FPE_tWorkInfo;	/*!< ,���[�N�f�[�^,-,[-], */
static slong	m_FPE_nDirX4PreciseSrchRgn[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { 
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Upper */
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Lower */
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Left  */
																				CB_IMG_CHECKBOARD_SIGN_MINUS  /* Right */
																				};	/*!< ,�T���̈��ݒ肷�����(X),?,[-], */
static slong	m_FPE_nDirY4PreciseSrchRgn[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = {
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Upper */
																				CB_IMG_CHECKBOARD_SIGN_MINUS, /* Lower */
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Left  */
																				CB_IMG_CHECKBOARD_SIGN_MINUS  /* Right */
																				};	/*!< ,�T���̈��ݒ肷�����(Y),?,[-], */
static slong	m_FPE_nFlagInit = CB_FALSE;	/*!< ,�������ς݃t���O,value=0,[-], */
static float_t	m_FPE_fSimilarityMAX;	/*!< ,�ގ��x��MAX�l,value=0.0<value<?,[-], */


/* ���E�s���p�^�[���p */
static slong	m_FPE_nDirX4PreciseSrchRgnAddChk[E_CB_SELFCALIB_CAMPOS_MAX][4] = {
																					/* Upper, Lower, Left , Right */
																					{    -1L,   +1L,    0L,    0L },	/* FRONT */
																					{    -1L,   +1L,    0L,    0L },	/* LEFT */ 
																					{    -1L,   +1L,    0L,    0L },	/* RIGHT */
																					{    +1L,   -1L,    0L,    0L }		/* REAR */
																				 };	/*!< ,�T���̈��ݒ肷�����(X),?,[-], */
static slong	m_FPE_nDirY4PreciseSrchRgnAddChk[E_CB_SELFCALIB_CAMPOS_MAX][4] = {
																					/* Upper, Lower, Left , Right */
																					{    +1L,   -1L,    0L,    0L },	/* FRONT */
																					{    +1L,   -1L,    0L,    0L },	/* LEFT */ 
																					{    +1L,   -1L,    0L,    0L },	/* RIGHT */
																					{    +1L,   -1L,    0L,    0L }		/* REAR */
																				 };	/*!< ,�T���̈��ݒ肷�����(X),?,[-], */


static slong	m_FPE_nDirX4PreciseSrchRgnAddCenter[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { 
																						CB_IMG_CHECKBOARD_SIGN_MINUS,  /* Upper */
																						CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Lower */
																						CB_IMG_CHECKBOARD_SIGN_MINUS,  /* Left  */
																						CB_IMG_CHECKBOARD_SIGN_PLUS  /* Right */
																						};	/*!< ,�T���̈��ݒ肷�����(X),?,[-], */
static slong	m_FPE_nDirY4PreciseSrchRgnAddCenter[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = {
																						CB_IMG_CHECKBOARD_SIGN_MINUS,  /* Upper */
																						CB_IMG_CHECKBOARD_SIGN_PLUS, /* Lower */
																						CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Left  */
																						CB_IMG_CHECKBOARD_SIGN_MINUS  /* Right */
																						};	/*!< ,�T���̈��ݒ肷�����(Y),?,[-], */


/* �f�o�b�O */
static CB_FPE_DBG_INFO	m_FPE_tDbgInfo;	/*!< ,�f�o�b�O���,-,[-], */

static IMPLIB_CNVLUT	m_FPE_nCnvLUT_EdgeAC[CB_FPE_EDGE_AC_TBL_MAXNUM][256];	/*!< ,�G�b�W�p�x�R�[�h�����pLUT,0<=value<=255,[-], */
static slong	m_FPE_nprm_EdgeCodeCoeff_H[9] = {	-1L, -2L, -1L, 
													 0L,  0L,  0L, 
													 1L,  2L,  1L };	/*!< ,�G�b�W�R�[�h���̌W��(�����G�b�W�p),-,[-], */
static slong	m_FPE_nprm_EdgeCodeCoeff_V[9] = {	-1L,  0L,  1L, 
													-2L,  0L,  2L, 
													-1L,  0L,  1L };	/*!< ,�G�b�W�R�[�h���̌W��(�����G�b�W�p),-,[-], */
static IMPLIB_IPEdgePoint	m_FPE_tEdgeCodeTbl[CB_FPE_EDGE_CODE_TBL_MAXNUM];	/*!< ,�G�b�W�R�[�h�e�[�u��,-,[-], */

/********************************************************
 * �G�b�W�R�[�h�p�x�ϊ��֌W�𑼃v���W�F�N�g����������� *
 ********************************************************/

/* �G�b�W�R�[�h�p�x�ϊ��e�[�u��(12bit����7bit�ւ̕ϊ��Ŏg�p) T.Arano 03/04/04 */
static uchar CB_atanTbl[4096] = {/* atanTbl[]  */
127U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U, 90U,
  0U, 45U, 63U, 72U, 76U, 79U, 81U, 82U, 83U, 84U, 84U, 85U, 85U, 86U, 86U, 86U, 86U, 87U, 87U, 87U, 87U, 87U, 87U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U, 89U,
  0U, 27U, 45U, 56U, 63U, 68U, 72U, 74U, 76U, 77U, 79U, 80U, 81U, 81U, 82U, 82U, 83U, 83U, 84U, 84U, 84U, 85U, 85U, 85U, 85U, 85U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U, 88U,
  0U, 18U, 34U, 45U, 53U, 59U, 63U, 67U, 69U, 72U, 73U, 75U, 76U, 77U, 78U, 79U, 79U, 80U, 81U, 81U, 81U, 82U, 82U, 83U, 83U, 83U, 83U, 84U, 84U, 84U, 84U, 84U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U, 87U,
  0U, 14U, 27U, 37U, 45U, 51U, 56U, 60U, 63U, 66U, 68U, 70U, 72U, 73U, 74U, 75U, 76U, 77U, 77U, 78U, 79U, 79U, 80U, 80U, 81U, 81U, 81U, 82U, 82U, 82U, 82U, 83U, 83U, 83U, 83U, 83U, 84U, 84U, 84U, 84U, 84U, 84U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U, 86U,
  0U, 11U, 22U, 31U, 39U, 45U, 50U, 54U, 58U, 61U, 63U, 66U, 67U, 69U, 70U, 72U, 73U, 74U, 74U, 75U, 76U, 77U, 77U, 78U, 78U, 79U, 79U, 80U, 80U, 80U, 81U, 81U, 81U, 81U, 82U, 82U, 82U, 82U, 83U, 83U, 83U, 83U, 83U, 83U, 84U, 84U, 84U, 84U, 84U, 84U, 84U, 84U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U, 85U,
  0U,  9U, 18U, 27U, 34U, 40U, 45U, 49U, 53U, 56U, 59U, 61U, 63U, 65U, 67U, 68U, 69U, 71U, 72U, 72U, 73U, 74U, 75U, 75U, 76U, 77U, 77U, 77U, 78U, 78U, 79U, 79U, 79U, 80U, 80U, 80U, 81U, 81U, 81U, 81U, 81U, 82U, 82U, 82U, 82U, 82U, 83U, 83U, 83U, 83U, 83U, 83U, 83U, 84U, 84U, 84U, 84U, 84U, 84U, 84U, 84U, 84U, 84U, 85U,
  0U,  8U, 16U, 23U, 30U, 36U, 41U, 45U, 49U, 52U, 55U, 58U, 60U, 62U, 63U, 65U, 66U, 68U, 69U, 70U, 71U, 72U, 72U, 73U, 74U, 74U, 75U, 75U, 76U, 76U, 77U, 77U, 78U, 78U, 78U, 79U, 79U, 79U, 80U, 80U, 80U, 80U, 81U, 81U, 81U, 81U, 81U, 82U, 82U, 82U, 82U, 82U, 82U, 82U, 83U, 83U, 83U, 83U, 83U, 83U, 83U, 83U, 84U, 84U,
  0U,  7U, 14U, 21U, 27U, 32U, 37U, 41U, 45U, 48U, 51U, 54U, 56U, 58U, 60U, 62U, 63U, 65U, 66U, 67U, 68U, 69U, 70U, 71U, 72U, 72U, 73U, 73U, 74U, 75U, 75U, 76U, 76U, 76U, 77U, 77U, 77U, 78U, 78U, 78U, 79U, 79U, 79U, 79U, 80U, 80U, 80U, 80U, 81U, 81U, 81U, 81U, 81U, 81U, 82U, 82U, 82U, 82U, 82U, 82U, 82U, 83U, 83U, 83U,
  0U,  6U, 13U, 18U, 24U, 29U, 34U, 38U, 42U, 45U, 48U, 51U, 53U, 55U, 57U, 59U, 61U, 62U, 63U, 65U, 66U, 67U, 68U, 69U, 69U, 70U, 71U, 72U, 72U, 73U, 73U, 74U, 74U, 75U, 75U, 76U, 76U, 76U, 77U, 77U, 77U, 78U, 78U, 78U, 78U, 79U, 79U, 79U, 79U, 80U, 80U, 80U, 80U, 80U, 81U, 81U, 81U, 81U, 81U, 81U, 81U, 82U, 82U, 82U,
  0U,  6U, 11U, 17U, 22U, 27U, 31U, 35U, 39U, 42U, 45U, 48U, 50U, 52U, 54U, 56U, 58U, 60U, 61U, 62U, 63U, 65U, 66U, 67U, 67U, 68U, 69U, 70U, 70U, 71U, 72U, 72U, 73U, 73U, 74U, 74U, 74U, 75U, 75U, 76U, 76U, 76U, 77U, 77U, 77U, 77U, 78U, 78U, 78U, 78U, 79U, 79U, 79U, 79U, 80U, 80U, 80U, 80U, 80U, 80U, 81U, 81U, 81U, 81U,
  0U,  5U, 10U, 15U, 20U, 24U, 29U, 32U, 36U, 39U, 42U, 45U, 47U, 50U, 52U, 54U, 55U, 57U, 59U, 60U, 61U, 62U, 63U, 64U, 65U, 66U, 67U, 68U, 69U, 69U, 70U, 70U, 71U, 72U, 72U, 73U, 73U, 73U, 74U, 74U, 75U, 75U, 75U, 76U, 76U, 76U, 77U, 77U, 77U, 77U, 78U, 78U, 78U, 78U, 78U, 79U, 79U, 79U, 79U, 79U, 80U, 80U, 80U, 80U,
  0U,  5U,  9U, 14U, 18U, 23U, 27U, 30U, 34U, 37U, 40U, 43U, 45U, 47U, 49U, 51U, 53U, 55U, 56U, 58U, 59U, 60U, 61U, 62U, 63U, 64U, 65U, 66U, 67U, 68U, 68U, 69U, 69U, 70U, 71U, 71U, 72U, 72U, 72U, 73U, 73U, 74U, 74U, 74U, 75U, 75U, 75U, 76U, 76U, 76U, 77U, 77U, 77U, 77U, 77U, 78U, 78U, 78U, 78U, 79U, 79U, 79U, 79U, 79U,
  0U,  4U,  9U, 13U, 17U, 21U, 25U, 28U, 32U, 35U, 38U, 40U, 43U, 45U, 47U, 49U, 51U, 53U, 54U, 56U, 57U, 58U, 59U, 61U, 62U, 63U, 63U, 64U, 65U, 66U, 67U, 67U, 68U, 68U, 69U, 70U, 70U, 71U, 71U, 72U, 72U, 72U, 73U, 73U, 74U, 74U, 74U, 75U, 75U, 75U, 75U, 76U, 76U, 76U, 76U, 77U, 77U, 77U, 77U, 78U, 78U, 78U, 78U, 78U,
  0U,  4U,  8U, 12U, 16U, 20U, 23U, 27U, 30U, 33U, 36U, 38U, 41U, 43U, 45U, 47U, 49U, 51U, 52U, 54U, 55U, 56U, 58U, 59U, 60U, 61U, 62U, 63U, 63U, 64U, 65U, 66U, 66U, 67U, 68U, 68U, 69U, 69U, 70U, 70U, 71U, 71U, 72U, 72U, 72U, 73U, 73U, 73U, 74U, 74U, 74U, 75U, 75U, 75U, 75U, 76U, 76U, 76U, 76U, 77U, 77U, 77U, 77U, 77U,
  0U,  4U,  8U, 11U, 15U, 18U, 22U, 25U, 28U, 31U, 34U, 36U, 39U, 41U, 43U, 45U, 47U, 49U, 50U, 52U, 53U, 54U, 56U, 57U, 58U, 59U, 60U, 61U, 62U, 63U, 63U, 64U, 65U, 66U, 66U, 67U, 67U, 68U, 68U, 69U, 69U, 70U, 70U, 71U, 71U, 72U, 72U, 72U, 73U, 73U, 73U, 74U, 74U, 74U, 74U, 75U, 75U, 75U, 75U, 76U, 76U, 76U, 76U, 77U,
  0U,  4U,  7U, 11U, 14U, 17U, 21U, 24U, 27U, 29U, 32U, 35U, 37U, 39U, 41U, 43U, 45U, 47U, 48U, 50U, 51U, 53U, 54U, 55U, 56U, 57U, 58U, 59U, 60U, 61U, 62U, 63U, 63U, 64U, 65U, 65U, 66U, 67U, 67U, 68U, 68U, 69U, 69U, 70U, 70U, 70U, 71U, 71U, 72U, 72U, 72U, 73U, 73U, 73U, 73U, 74U, 74U, 74U, 75U, 75U, 75U, 75U, 76U, 76U,
  0U,  3U,  7U, 10U, 13U, 16U, 19U, 22U, 25U, 28U, 30U, 33U, 35U, 37U, 39U, 41U, 43U, 45U, 47U, 48U, 50U, 51U, 52U, 54U, 55U, 56U, 57U, 58U, 59U, 60U, 60U, 61U, 62U, 63U, 63U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 68U, 68U, 69U, 69U, 70U, 70U, 70U, 71U, 71U, 72U, 72U, 72U, 73U, 73U, 73U, 73U, 74U, 74U, 74U, 74U, 75U, 75U,
  0U,  3U,  6U,  9U, 13U, 16U, 18U, 21U, 24U, 27U, 29U, 31U, 34U, 36U, 38U, 40U, 42U, 43U, 45U, 47U, 48U, 49U, 51U, 52U, 53U, 54U, 55U, 56U, 57U, 58U, 59U, 60U, 61U, 61U, 62U, 63U, 63U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 68U, 68U, 69U, 69U, 69U, 70U, 70U, 71U, 71U, 71U, 72U, 72U, 72U, 72U, 73U, 73U, 73U, 74U, 74U, 74U,
  0U,  3U,  6U,  9U, 12U, 15U, 18U, 20U, 23U, 25U, 28U, 30U, 32U, 34U, 36U, 38U, 40U, 42U, 43U, 45U, 46U, 48U, 49U, 50U, 52U, 53U, 54U, 55U, 56U, 57U, 58U, 58U, 59U, 60U, 61U, 62U, 62U, 63U, 63U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 68U, 68U, 68U, 69U, 69U, 70U, 70U, 70U, 71U, 71U, 71U, 72U, 72U, 72U, 72U, 73U, 73U, 73U,
  0U,  3U,  6U,  9U, 11U, 14U, 17U, 19U, 22U, 24U, 27U, 29U, 31U, 33U, 35U, 37U, 39U, 40U, 42U, 44U, 45U, 46U, 48U, 49U, 50U, 51U, 52U, 53U, 54U, 55U, 56U, 57U, 58U, 59U, 60U, 60U, 61U, 62U, 62U, 63U, 63U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 67U, 68U, 68U, 69U, 69U, 69U, 70U, 70U, 70U, 71U, 71U, 71U, 72U, 72U, 72U, 72U,
  0U,  3U,  5U,  8U, 11U, 13U, 16U, 18U, 21U, 23U, 25U, 28U, 30U, 32U, 34U, 36U, 37U, 39U, 41U, 42U, 44U, 45U, 46U, 48U, 49U, 50U, 51U, 52U, 53U, 54U, 55U, 56U, 57U, 58U, 58U, 59U, 60U, 60U, 61U, 62U, 62U, 63U, 63U, 64U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 68U, 68U, 68U, 69U, 69U, 69U, 70U, 70U, 70U, 71U, 71U, 71U, 72U,
  0U,  3U,  5U,  8U, 10U, 13U, 15U, 18U, 20U, 22U, 24U, 27U, 29U, 31U, 32U, 34U, 36U, 38U, 39U, 41U, 42U, 44U, 45U, 46U, 47U, 49U, 50U, 51U, 52U, 53U, 54U, 55U, 55U, 56U, 57U, 58U, 59U, 59U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 64U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 67U, 68U, 68U, 69U, 69U, 69U, 70U, 70U, 70U, 70U, 71U,
  0U,  2U,  5U,  7U, 10U, 12U, 15U, 17U, 19U, 21U, 23U, 26U, 28U, 29U, 31U, 33U, 35U, 36U, 38U, 40U, 41U, 42U, 44U, 45U, 46U, 47U, 49U, 50U, 51U, 52U, 53U, 53U, 54U, 55U, 56U, 57U, 57U, 58U, 59U, 59U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 64U, 64U, 65U, 65U, 66U, 66U, 67U, 67U, 67U, 68U, 68U, 68U, 69U, 69U, 69U, 70U, 70U,
  0U,  2U,  5U,  7U,  9U, 12U, 14U, 16U, 18U, 21U, 23U, 25U, 27U, 28U, 30U, 32U, 34U, 35U, 37U, 38U, 40U, 41U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 51U, 52U, 53U, 54U, 55U, 56U, 56U, 57U, 58U, 58U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 64U, 64U, 65U, 65U, 66U, 66U, 66U, 67U, 67U, 68U, 68U, 68U, 69U, 69U, 69U,
  0U,  2U,  5U,  7U,  9U, 11U, 13U, 16U, 18U, 20U, 22U, 24U, 26U, 27U, 29U, 31U, 33U, 34U, 36U, 37U, 39U, 40U, 41U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 51U, 52U, 53U, 54U, 54U, 55U, 56U, 57U, 57U, 58U, 59U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 64U, 64U, 65U, 65U, 66U, 66U, 66U, 67U, 67U, 67U, 68U, 68U, 68U,
  0U,  2U,  4U,  7U,  9U, 11U, 13U, 15U, 17U, 19U, 21U, 23U, 25U, 27U, 28U, 30U, 32U, 33U, 35U, 36U, 38U, 39U, 40U, 41U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 51U, 52U, 53U, 53U, 54U, 55U, 56U, 56U, 57U, 58U, 58U, 59U, 59U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 63U, 64U, 64U, 65U, 65U, 65U, 66U, 66U, 67U, 67U, 67U, 68U,
  0U,  2U,  4U,  6U,  8U, 10U, 13U, 15U, 17U, 18U, 20U, 22U, 24U, 26U, 27U, 29U, 31U, 32U, 34U, 35U, 37U, 38U, 39U, 40U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 51U, 52U, 52U, 53U, 54U, 55U, 55U, 56U, 57U, 57U, 58U, 58U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 63U, 64U, 64U, 65U, 65U, 65U, 66U, 66U, 66U, 67U,
  0U,  2U,  4U,  6U,  8U, 10U, 12U, 14U, 16U, 18U, 20U, 21U, 23U, 25U, 27U, 28U, 30U, 31U, 33U, 34U, 36U, 37U, 38U, 39U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 51U, 51U, 52U, 53U, 54U, 54U, 55U, 56U, 56U, 57U, 58U, 58U, 59U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 63U, 64U, 64U, 65U, 65U, 65U, 66U, 66U,
  0U,  2U,  4U,  6U,  8U, 10U, 12U, 14U, 15U, 17U, 19U, 21U, 22U, 24U, 26U, 27U, 29U, 30U, 32U, 33U, 35U, 36U, 37U, 38U, 40U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 50U, 51U, 52U, 53U, 53U, 54U, 55U, 55U, 56U, 57U, 57U, 58U, 58U, 59U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 63U, 64U, 64U, 65U, 65U, 65U,
  0U,  2U,  4U,  6U,  8U,  9U, 11U, 13U, 15U, 17U, 18U, 20U, 22U, 23U, 25U, 27U, 28U, 30U, 31U, 32U, 34U, 35U, 36U, 37U, 39U, 40U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 49U, 50U, 51U, 52U, 52U, 53U, 54U, 54U, 55U, 56U, 56U, 57U, 57U, 58U, 59U, 59U, 60U, 60U, 60U, 61U, 61U, 62U, 62U, 63U, 63U, 63U, 64U, 64U, 65U,
  0U,  2U,  4U,  6U,  7U,  9U, 11U, 13U, 14U, 16U, 18U, 20U, 21U, 23U, 24U, 26U, 27U, 29U, 30U, 32U, 33U, 34U, 35U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 48U, 49U, 50U, 51U, 52U, 52U, 53U, 54U, 54U, 55U, 55U, 56U, 57U, 57U, 58U, 58U, 59U, 59U, 60U, 60U, 61U, 61U, 61U, 62U, 62U, 63U, 63U, 63U, 64U,
  0U,  2U,  4U,  5U,  7U,  9U, 11U, 12U, 14U, 16U, 17U, 19U, 21U, 22U, 24U, 25U, 27U, 28U, 29U, 31U, 32U, 33U, 35U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 48U, 49U, 50U, 51U, 51U, 52U, 53U, 53U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 58U, 58U, 59U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 62U, 63U, 63U,
  0U,  2U,  3U,  5U,  7U,  9U, 10U, 12U, 14U, 15U, 17U, 18U, 20U, 22U, 23U, 24U, 26U, 27U, 29U, 30U, 31U, 32U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 47U, 48U, 49U, 50U, 50U, 51U, 52U, 52U, 53U, 54U, 54U, 55U, 55U, 56U, 57U, 57U, 58U, 58U, 59U, 59U, 59U, 60U, 60U, 61U, 61U, 62U, 62U, 62U,
  0U,  2U,  3U,  5U,  7U,  8U, 10U, 12U, 13U, 15U, 16U, 18U, 19U, 21U, 22U, 24U, 25U, 27U, 28U, 29U, 30U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 44U, 45U, 46U, 47U, 47U, 48U, 49U, 50U, 50U, 51U, 52U, 52U, 53U, 54U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 58U, 58U, 59U, 59U, 60U, 60U, 60U, 61U, 61U, 62U,
  0U,  2U,  3U,  5U,  7U,  8U, 10U, 11U, 13U, 14U, 16U, 17U, 19U, 20U, 22U, 23U, 25U, 26U, 27U, 28U, 30U, 31U, 32U, 33U, 34U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 42U, 43U, 44U, 45U, 46U, 47U, 47U, 48U, 49U, 50U, 50U, 51U, 51U, 52U, 53U, 53U, 54U, 54U, 55U, 56U, 56U, 57U, 57U, 58U, 58U, 58U, 59U, 59U, 60U, 60U, 61U, 61U,
  0U,  2U,  3U,  5U,  6U,  8U,  9U, 11U, 13U, 14U, 16U, 17U, 18U, 20U, 21U, 23U, 24U, 25U, 27U, 28U, 29U, 30U, 31U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 43U, 44U, 45U, 46U, 47U, 47U, 48U, 49U, 49U, 50U, 51U, 51U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 58U, 58U, 59U, 59U, 59U, 60U, 60U,
  0U,  2U,  3U,  5U,  6U,  8U,  9U, 11U, 12U, 14U, 15U, 17U, 18U, 19U, 21U, 22U, 23U, 25U, 26U, 27U, 28U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 43U, 44U, 45U, 46U, 47U, 47U, 48U, 49U, 49U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 57U, 58U, 58U, 59U, 59U, 60U,
  0U,  2U,  3U,  5U,  6U,  7U,  9U, 10U, 12U, 13U, 15U, 16U, 18U, 19U, 20U, 22U, 23U, 24U, 25U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 43U, 44U, 45U, 46U, 46U, 47U, 48U, 49U, 49U, 50U, 50U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 58U, 58U, 58U, 59U,
  0U,  1U,  3U,  4U,  6U,  7U,  9U, 10U, 12U, 13U, 14U, 16U, 17U, 18U, 20U, 21U, 22U, 24U, 25U, 26U, 27U, 28U, 29U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 38U, 39U, 40U, 41U, 42U, 43U, 43U, 44U, 45U, 46U, 46U, 47U, 48U, 48U, 49U, 50U, 50U, 51U, 51U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 57U, 58U, 58U,
  0U,  1U,  3U,  4U,  6U,  7U,  9U, 10U, 11U, 13U, 14U, 15U, 17U, 18U, 19U, 21U, 22U, 23U, 24U, 25U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 40U, 41U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 48U, 48U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 56U, 56U, 57U, 57U, 58U,
  0U,  1U,  3U,  4U,  6U,  7U,  8U, 10U, 11U, 12U, 14U, 15U, 16U, 18U, 19U, 20U, 21U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U, 40U, 41U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 48U, 48U, 49U, 49U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 56U, 56U, 57U, 57U,
  0U,  1U,  3U,  4U,  5U,  7U,  8U,  9U, 11U, 12U, 13U, 15U, 16U, 17U, 18U, 20U, 21U, 22U, 23U, 24U, 25U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 36U, 37U, 38U, 39U, 40U, 41U, 41U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 48U, 48U, 49U, 49U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 55U, 56U, 56U,
  0U,  1U,  3U,  4U,  5U,  7U,  8U,  9U, 11U, 12U, 13U, 14U, 16U, 17U, 18U, 19U, 20U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 38U, 39U, 40U, 41U, 41U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 55U, 55U, 56U,
  0U,  1U,  3U,  4U,  5U,  6U,  8U,  9U, 10U, 12U, 13U, 14U, 15U, 16U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 39U, 40U, 41U, 42U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 55U, 55U,
  0U,  1U,  3U,  4U,  5U,  6U,  8U,  9U, 10U, 11U, 13U, 14U, 15U, 16U, 17U, 18U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 35U, 36U, 37U, 38U, 39U, 39U, 40U, 41U, 42U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 54U, 54U, 54U,
  0U,  1U,  2U,  4U,  5U,  6U,  7U,  9U, 10U, 11U, 12U, 13U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 25U, 26U, 27U, 28U, 29U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 36U, 37U, 38U, 39U, 40U, 40U, 41U, 42U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U, 53U, 53U, 54U,
  0U,  1U,  2U,  4U,  5U,  6U,  7U,  8U, 10U, 11U, 12U, 13U, 14U, 15U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 33U, 34U, 35U, 36U, 37U, 37U, 38U, 39U, 40U, 40U, 41U, 42U, 42U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U, 53U,
  0U,  1U,  2U,  4U,  5U,  6U,  7U,  8U,  9U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 35U, 36U, 37U, 38U, 38U, 39U, 40U, 41U, 41U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U, 53U,
  0U,  1U,  2U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 31U, 32U, 33U, 34U, 35U, 36U, 36U, 37U, 38U, 39U, 39U, 40U, 41U, 41U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U, 52U,
  0U,  1U,  2U,  3U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 33U, 34U, 35U, 36U, 37U, 37U, 38U, 39U, 39U, 40U, 41U, 41U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 51U, 51U, 52U,
  0U,  1U,  2U,  3U,  4U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 30U, 31U, 32U, 33U, 34U, 34U, 35U, 36U, 37U, 37U, 38U, 39U, 39U, 40U, 41U, 41U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 51U, 51U,
  0U,  1U,  2U,  3U,  4U,  5U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 30U, 31U, 32U, 32U, 33U, 34U, 35U, 35U, 36U, 37U, 38U, 38U, 39U, 40U, 40U, 41U, 41U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 50U, 50U, 50U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 30U, 31U, 32U, 33U, 33U, 34U, 35U, 36U, 36U, 37U, 38U, 38U, 39U, 40U, 40U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 49U, 49U, 49U, 50U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 12U, 13U, 14U, 15U, 16U, 17U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 30U, 31U, 31U, 32U, 33U, 34U, 34U, 35U, 36U, 37U, 37U, 38U, 39U, 39U, 40U, 40U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 48U, 48U, 48U, 49U, 49U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 24U, 25U, 26U, 27U, 28U, 29U, 29U, 30U, 31U, 32U, 32U, 33U, 34U, 35U, 35U, 36U, 37U, 37U, 38U, 39U, 39U, 40U, 41U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 47U, 47U, 47U, 48U, 48U, 49U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 30U, 31U, 31U, 32U, 33U, 33U, 34U, 35U, 36U, 36U, 37U, 38U, 38U, 39U, 39U, 40U, 41U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 46U, 46U, 46U, 47U, 47U, 48U, 48U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 25U, 26U, 27U, 28U, 29U, 29U, 30U, 31U, 32U, 32U, 33U, 34U, 34U, 35U, 36U, 36U, 37U, 38U, 38U, 39U, 40U, 40U, 41U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 45U, 46U, 46U, 47U, 47U, 48U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 22U, 23U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 30U, 30U, 31U, 32U, 33U, 33U, 34U, 35U, 35U, 36U, 37U, 37U, 38U, 38U, 39U, 40U, 40U, 41U, 41U, 42U, 42U, 43U, 43U, 44U, 45U, 45U, 45U, 46U, 46U, 47U, 47U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 20U, 21U, 22U, 23U, 24U, 25U, 25U, 26U, 27U, 28U, 28U, 29U, 30U, 31U, 31U, 32U, 33U, 33U, 34U, 35U, 35U, 36U, 37U, 37U, 38U, 39U, 39U, 40U, 40U, 41U, 41U, 42U, 42U, 43U, 44U, 44U, 45U, 45U, 45U, 46U, 46U, 47U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 18U, 19U, 20U, 21U, 22U, 23U, 23U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 30U, 30U, 31U, 32U, 32U, 33U, 34U, 34U, 35U, 36U, 36U, 37U, 37U, 38U, 39U, 39U, 40U, 40U, 41U, 41U, 42U, 43U, 43U, 44U, 44U, 45U, 45U, 45U, 46U, 46U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 16U, 17U, 18U, 19U, 20U, 21U, 21U, 22U, 23U, 24U, 25U, 25U, 26U, 27U, 28U, 28U, 29U, 30U, 31U, 31U, 32U, 33U, 33U, 34U, 35U, 35U, 36U, 36U, 37U, 38U, 38U, 39U, 39U, 40U, 40U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 45U, 45U, 46U,
  0U,  1U,  2U,  3U,  4U,  5U,  6U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 20U, 21U, 22U, 23U, 24U, 24U, 25U, 26U, 27U, 27U, 28U, 29U, 29U, 30U, 31U, 32U, 32U, 33U, 33U, 34U, 35U, 35U, 36U, 37U, 37U, 38U, 38U, 39U, 39U, 40U, 41U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 45U, 45U,
  0U,  1U,  2U,  3U,  4U,  5U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 13U, 14U, 15U, 16U, 17U, 18U, 18U, 19U, 20U, 21U, 22U, 22U, 23U, 24U, 25U, 25U, 26U, 27U, 28U, 28U, 29U, 30U, 30U, 31U, 32U, 32U, 33U, 34U, 34U, 35U, 36U, 36U, 37U, 37U, 38U, 38U, 39U, 40U, 40U, 41U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 45U
};
	
static ushort CB_thetaTbl[512] = {
/*  360-270 */
	  1U,  1U,  2U,  2U,  3U,  3U,  4U,  4U,  5U,  5U,  6U,  6U,  7U,  7U,  8U,  8U,  9U,  9U, 10U, 10U,
	 11U, 11U, 12U, 12U, 13U, 13U, 14U, 14U, 15U, 15U, 16U, 16U, 17U, 17U, 18U, 18U, 19U, 19U, 20U, 20U,
	 21U, 21U, 22U, 22U, 23U, 23U, 24U, 24U, 25U, 25U, 26U, 26U, 27U, 27U, 28U, 28U, 29U, 29U, 30U, 30U,
	 31U, 31U, 32U, 32U, 33U, 33U, 34U, 34U, 35U, 35U, 36U, 36U, 37U, 37U, 38U, 38U, 39U, 39U, 40U, 40U,
	 41U, 41U, 42U, 42U, 43U, 43U, 44U, 44U, 45U, 45U, 46U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,
	181U,180U,180U,179U,179U,178U,178U,177U,177U,176U,176U,175U,175U,174U,174U,173U,173U,172U,172U,171U,
	171U,170U,170U,169U,169U,168U,168U,167U,167U,166U,166U,165U,165U,164U,164U,163U,163U,162U,162U,161U,
	161U,160U,160U,159U,159U,158U,158U,157U,157U,156U,156U,155U,155U,154U,154U,153U,153U,152U,152U,151U,
	151U,150U,150U,149U,149U,148U,148U,147U,147U,146U,146U,145U,145U,144U,144U,143U,143U,142U,142U,141U,
	141U,140U,140U,139U,139U,138U,138U,137U,137U,136U,136U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,
	 91U, 90U, 90U, 89U, 89U, 88U, 88U, 87U, 87U, 86U, 86U, 85U, 85U, 84U, 84U, 83U, 83U, 82U, 82U, 81U,
	 81U, 80U, 80U, 79U, 79U, 78U, 78U, 77U, 77U, 76U, 76U, 75U, 75U, 74U, 74U, 73U, 73U, 72U, 72U, 71U,
	 71U, 70U, 70U, 69U, 69U, 68U, 68U, 67U, 67U, 66U, 66U, 65U, 65U, 64U, 64U, 63U, 63U, 62U, 62U, 61U,
	 61U, 60U, 60U, 59U, 59U, 58U, 58U, 57U, 57U, 56U, 56U, 55U, 55U, 54U, 54U, 53U, 53U, 52U, 52U, 51U,
	 51U, 50U, 50U, 49U, 49U, 48U, 48U, 47U, 47U, 46U, 46U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,
	 91U, 91U, 92U, 92U, 93U, 93U, 94U, 94U, 95U, 95U, 96U, 96U, 97U, 97U, 98U, 98U, 99U, 99U,100U,100U,
	101U,101U,102U,102U,103U,103U,104U,104U,105U,105U,106U,106U,107U,107U,108U,108U,109U,109U,110U,110U,
	111U,111U,112U,112U,113U,113U,114U,114U,115U,115U,116U,116U,117U,117U,118U,118U,119U,119U,120U,120U,
	121U,121U,122U,122U,123U,123U,124U,124U,125U,125U,126U,126U,127U,127U,128U,128U,129U,129U,130U,130U,
	131U,131U,132U,132U,133U,133U,134U,134U,135U,135U,136U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,255U,
	255U,255U,255U,255U,255U,255U,255U,255U
};
/********************************************************
 * �G�b�W�R�[�h�p�x�ϊ��֌W�𑼃v���W�F�N�g����������� *
 ********************************************************/

/**
 * @brief	�s��Haar-like�����ʕ�������p�e�[�u��
 */
/* �e�[�u���̃��[�������L�Ɏ��� */
/* (1)CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER�i�������j���I�����ꂽ�ꍇ�́A�s���̌`�ɂ�����炸�ACB_IMG_ALGORITHM_DEFAULT�Ɠ��l�̓���Ƃ��� */
/* (2)CB_IMG_PATTERN_TYPE_CHKBOARD2x2 �̏ꍇ�ɁACB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT�^CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT���I������Ă��ACB_IMG_ALGORITHM_DEFAULT�Ɠ��l�̓���Ƃ���*/
/* (3)CB_IMG_PATTERN_TYPE_CHKBOARD2x3 �̏ꍇ�ɁACB_IMG_ALGORITHM_DEFAULT�^CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER���I�����ꂽ�ꍇ�A�X�}�[�i�E�L�����g���H��̔���i�ԗ���Fr���Ŕ���j���f�t�H���g�Ƃ���*/
/* (4)CB_IMG_PATTERN_TYPE_CHKBOARD2x4 �̏ꍇ�ɁACB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT�^CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT���I������Ă��ACB_IMG_ALGORITHM_DEFAULT�Ɠ��l�̓���Ƃ���*/
static slong SignHaarLikeFeatureTable[ CB_IMG_PATTERN_TYPE_MAXNUM ][ CB_IMG_CHKBOARD_PLACEMENT_MAXNUM ][ CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM ] = {
	{	/* CB_IMG_PATTERN_TYPE_NONE */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_CHECKBOARD_SIGN_NONE,	CB_IMG_CHECKBOARD_SIGN_NONE, 			CB_IMG_CHECKBOARD_SIGN_NONE, 				CB_IMG_CHECKBOARD_SIGN_NONE },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_CHECKBOARD_SIGN_NONE,	CB_IMG_CHECKBOARD_SIGN_NONE, 			CB_IMG_CHECKBOARD_SIGN_NONE, 				CB_IMG_CHECKBOARD_SIGN_NONE },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD2x2 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{ CB_IMG_CHECKBOARD_SIGN_MINUS,		CB_IMG_CHECKBOARD_SIGN_MINUS, 			CB_IMG_CHECKBOARD_SIGN_MINUS,				CB_IMG_CHECKBOARD_SIGN_MINUS },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{ CB_IMG_CHECKBOARD_SIGN_PLUS,		CB_IMG_CHECKBOARD_SIGN_PLUS, 			CB_IMG_CHECKBOARD_SIGN_PLUS,				CB_IMG_CHECKBOARD_SIGN_PLUS },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD2x3 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{ CB_IMG_CHECKBOARD_SIGN_PLUS,		CB_IMG_CHECKBOARD_SIGN_MINUS, 			CB_IMG_CHECKBOARD_SIGN_PLUS, 				CB_IMG_CHECKBOARD_SIGN_PLUS },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{ CB_IMG_CHECKBOARD_SIGN_PLUS,		CB_IMG_CHECKBOARD_SIGN_PLUS, 			CB_IMG_CHECKBOARD_SIGN_MINUS, 				CB_IMG_CHECKBOARD_SIGN_PLUS },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD2x4 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{ CB_IMG_CHECKBOARD_SIGN_PLUS,		CB_IMG_CHECKBOARD_SIGN_PLUS, 			CB_IMG_CHECKBOARD_SIGN_PLUS, 				CB_IMG_CHECKBOARD_SIGN_PLUS },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{ CB_IMG_CHECKBOARD_SIGN_MINUS,		CB_IMG_CHECKBOARD_SIGN_MINUS, 			CB_IMG_CHECKBOARD_SIGN_MINUS, 				CB_IMG_CHECKBOARD_SIGN_MINUS },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD1x4 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{ CB_IMG_CHECKBOARD_SIGN_PLUS,		CB_IMG_CHECKBOARD_SIGN_PLUS, 			CB_IMG_CHECKBOARD_SIGN_PLUS, 				CB_IMG_CHECKBOARD_SIGN_PLUS },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{ CB_IMG_CHECKBOARD_SIGN_MINUS,		CB_IMG_CHECKBOARD_SIGN_MINUS, 			CB_IMG_CHECKBOARD_SIGN_MINUS, 				CB_IMG_CHECKBOARD_SIGN_MINUS },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},															
	{	/* CB_IMG_PATTERN_TYPE_CIRCLE_LINE */ /* CB_IMG_PATTERN_TYPE_CIRCLE_LINE�̏ꍇ�͖����ɂ��Ă��� */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{ CB_IMG_CHECKBOARD_SIGN_NONE,		CB_IMG_CHECKBOARD_SIGN_NONE, 			CB_IMG_CHECKBOARD_SIGN_NONE, 				CB_IMG_CHECKBOARD_SIGN_NONE },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{ CB_IMG_CHECKBOARD_SIGN_NONE,		CB_IMG_CHECKBOARD_SIGN_NONE, 			CB_IMG_CHECKBOARD_SIGN_NONE, 				CB_IMG_CHECKBOARD_SIGN_NONE },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	}
};

/**
 * @brief	�����_�ʒu�^�C�v��`�e�[�u��
 */
static enum enum_CB_IMG_ALGORITHM m_FPE_nFtrPntPosTypeTbl4Chkboard[ CB_IMG_PATTERN_TYPE_MAXNUM ][ CB_IMG_CHKBOARD_PLACEMENT_MAXNUM ][ CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM ] = 
{
	{	/* CB_IMG_PATTERN_TYPE_NONE */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD2x2 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD2x3 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD2x4 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CHKBOARD1x4 */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,   CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	},
	{	/* CB_IMG_PATTERN_TYPE_CIRCLE_LINE */ /* CB_IMG_PATTERN_TYPE_CIRCLE_LINE�̏ꍇ�͖����ɂ��Ă��� */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	}
};
/* �d�ݕt��Haar-like�����ʌv�Z���̃u���b�N�T�C�Y */																																												
#define		CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MAX		(   10L )				/*!< ,�C���s���p�^�[���̈�ʐώZ�o���̃u���b�N�T�C�Y�ő�l, value=10, [pixel], */																							
#define		CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MIN		(    6L )				/*!< ,�C���s���p�^�[���̈�ʐώZ�o���̃u���b�N�T�C�Y�ŏ��l, value=6, [pixel], */																							
																																												
/**																																												
 * @brief			�}�[�J�[�̌X�����																																									
 */																																												
enum enum_CB_FPE_SLOPE_TYPE																																												
{																																												
	CB_FPE_SLOPE_TYPE_VERT = 0,									/*!< ,��������,value=0,[-], */																																		
	CB_FPE_SLOPE_TYPE_HORI,									/*!< ,��������,value=1,[-], */																																		
	CB_FPE_SLOPE_TYPE_MAX									/*!< ,�ő�l,value=2,[-], */																																		
};																																												

/*Y���W�ʒu�ݒ�e�[�u��*/																												
slong		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_MAX][CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MAX];																										
slong		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_MAX][CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MAX];																										


/********************************************************************
 * �����֐��@�v���g�^�C�v�錾
 ********************************************************************/

static void cb_img_FtrPointExtractor_Reinit( void );
static slong cb_FPE_AllocImg( IMPLIB_IMGID* pnImg );
static slong cb_FPE_SetCameraParam( const CB_IMG_CAMERA_PRM* const ptCamPrmSrc, CB_FPE_CAM_PRM* ptCamPrmDst );
static slong cb_FPE_DeleteCameraParam( CB_FPE_CAM_PRM* ptCamPrm );
static slong cb_FPE_SetCameraErrParam( const CB_IMG_CAMERA_PRM* const ptCamPrmSrc, CB_FPE_CAM_ERR_INFO* ptCamErrInfo );
static slong cb_FPE_SetSearchRegionWorld( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo );
static slong cb_FPE_SetSearchRegionWorld4AddPattern( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo );
static slong cb_FPE_SetSearchRegionWorld4AddChkboard( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, slong nCamDirection );
static slong cb_FPE_SetSearchRegionImg( CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, const CB_FPE_CAM_PRM* const ptCamPrm, const CB_FPE_CAM_ERR_INFO* const ptCamErrInfo );
static slong cb_FPE_SetSearchRegionImg4AddPattern( CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, const CB_FPE_CAM_PRM* const ptCamPrm, const CB_FPE_CAM_ERR_INFO* const ptCamErrInfo );
static slong cb_FPE_SetSearchRegionImg4AddChkboard( CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, const CB_FPE_CAM_PRM* const ptCamPrm, const CB_FPE_CAM_ERR_INFO* const ptCamErrInfo );
static slong cb_FPE_SetSearchRegion( CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* ptSrchRgnInfo );
static slong cb_FPE_SetSearchRegion4AddPattern( CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo );
static slong cb_FPE_SetSearchRegion4AddChkboard( CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo );
static ulong cb_FPE_CheckRegion( slong nSxRgn, slong nSyRgn, slong nExRgn, slong nEyRgn, slong nXSizeImg, slong nYSizeImg );
static slong cb_FPE_ResetRegion( CB_RECT_RGN *ptSetRgn, slong nXSizeImg, slong nYSizeImg );
static slong cb_FPE_CheckExecCondition( slong nFlagInit, const CB_FPE_SRCH_RGN_INFO_EACH_CAM* const ptSrchRgnInfo );
static slong cb_FPE_ClearResult( CB_IMG_RESULT* const ptResult );
static slong cb_FPE_ExtractFtrPoint4Checkerboard( const IMPLIB_IMGID nImgSrc, CB_FPE_INFO *ptFPEInfo, slong nCamDirection, slong nCalibType );
static slong cb_FPE_ExtractFtrPoint4AddChk( const IMPLIB_IMGID nImgSrc, CB_FPE_INFO *ptFPEInfo, slong nCamDirection, slong nCalibType );
static slong cb_FPE_WeightedHaarLikeChkEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo );
#ifdef CALIB_PARAM_P32S_EUR_UK
static slong cb_FPE_WeightedHaarLikeChkEdge_FR_LEFT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo );
static slong cb_FPE_WeightedHaarLikeChkEdge_RR_LEFT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo );
static slong cb_FPE_WeightedHaarLikeChkEdge_FR_RIGHT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo );
static slong cb_FPE_WeightedHaarLikeChkEdge_RR_RIGHT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo );
#else	/* CALIB_PARAM_P32S_EUR_UK */
static slong cb_FPE_WeightedHaarLikeChkEdge_FRRR_LEFT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo ,slong nCamDirection );
static slong cb_FPE_WeightedHaarLikeChkEdge_FRRR_RIGHT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo, slong nCamDirection );
#endif /* CALIB_PARAM_P32S_EUR_UK */
static slong cb_FPE_CalcLeftBrightness( CB_FPE_BRIGHT_CALC_INFO *ptCalcInfo, const slong *pYPos );
static slong cb_FPE_CalcRightBrightness( CB_FPE_BRIGHT_CALC_INFO *ptCalcInfo, const slong *pYPos );
static slong cb_FPE_CalcMedian( slong* nMedianValue, const slong* const nValArray, const slong nNumValue);
static slong cb_FPE_GetSearchCoeffDir( enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos, enum enum_CB_ADD_PATTERN_POS nAddPatternPos, slong* nCoeffDirX, slong* nCoeffDirY);
static slong cb_FPE_GetSignHaarLikeFeature( const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, slong nCamDirection, slong* nSignHaarLikeFeature);
static slong cb_FPE_GetTypeAlgorithm( const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, enum enum_CB_IMG_ALGORITHM* pnTypeAlgorithm );
static slong cb_FPE_ExtractEdgePnt4AddChk(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
										    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
										    enum enum_CB_FPE_LINE_FLT_MODE nFltMode, slong nDirX, slong nDirY);
static slong cb_FPE_EstimateFtrPoint4AddChk( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, const CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* const ptSrchRgnInfo,
												CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltAddPntInfo, slong nCamDirection );
static slong cb_FPE_EstimateFtrPoint4AddCrossPnt( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo,
													CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltAddPntInfo, slong nCamDirection );
static slong cb_FPE_ExtractEdgePnt4AddCross(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
												CB_IMG_POINT tCrossPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
												enum enum_CB_FPE_LINE_FLT_MODE nFltMode, slong nDirX, slong nDirY, slong nSrchAngleCode );
static slong cb_FPE_CalcIntersectionPoint(	const t_cb_img_CenterPos* const ptEdgePrecPntUL, const t_cb_img_CenterPos* const ptEdgePrecPntLR, slong nNumEdgePrecPntUL, slong nNumEdgePrecPntLR,
												t_cb_img_CenterPos* ptRsltPntArray );
static slong cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
														    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
										   					slong* pnThrThetaMin, slong* pnThrThetaMax, slong* pnThrRho, slong nDirX, slong nDirY, slong nSrchAngleCode, slong nprm_HalfScanRgn);
static slong cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
														    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
															slong* pnThrThetaMin, slong* pnThrThetaMax, slong* pnThrRho, slong nDirY, slong nSrchAngleCode, slong nprm_HalfScanRgn);
static slong cb_FPE_ExtractEdgePnt4AddChk_Search_Hori(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,
										    			CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt, slong nDirY, slong nThrThetaMin, slong nThrThetaMax, slong nThrRho, slong nDirectNum, slong nprm_HalfScanRgn);
static slong cb_FPE_ExtractEdgePnt4AddChk_Search_Vert(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,
										    	CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt, slong nThrThetaMin, slong nThrThetaMax, slong nThrRho, slong nDirectNum, slong nprm_HalfScanRgn);
static slong cb_FPE_SetEdgeACTable( IMPLIB_CNVLUT* pnTbl4FrRr, IMPLIB_CNVLUT* pnTbl4LR );
static slong cb_FPE_ExtractEdgeCodeInfo( IMPLIB_IMGID nImgSrc, IMPLIB_IMGID nImgEdgeRho, IMPLIB_IMGID nImgEdgeAngle, IMPLIB_IMGID nImgEdgeAC, const CB_RECT_RGN* const ptSrchRgn, slong nCamDirection );
static slong cb_FPE_SearchCandTPntUL4AddChk_byEdgeCode( const IMPLIB_IMGID* const pnImgTbl, const CB_IMG_POINT* const ptCrossPnt, slong nNumCrossPnt, 
												CB_IMG_POINT* ptDstPnt, slong* pnNumDstPnt, const CB_RECT_RGN* const ptSrchRgn, enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos, enum enum_CB_ADD_PATTERN_POS nAddPatternPos, slong nCamDirection,  const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo );
static slong cb_FPE_ExtractFtrPoint4CircleLine( const IMPLIB_IMGID nImgSrc, CB_FPE_INFO *ptFPEInfo, slong nCamDirection, slong nCalibType );
static slong cb_FPE_ClearSearchResultPointInfo( slong nLoopIdx, CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo );
static slong cb_FPE_ClearSearchResultAddPointInfo( CB_FPE_SRCH_RSLT_ADD_PNT_INFO* ptSrchRsltPntInfo );
static slong cb_FPE_IntegralImage( const IMPLIB_IMGID* const pnImgWk, const CB_RECT_RGN* const ptSrchRgn, const uchar nIntgralImgFlg );
static slong cb_FPE_Smoothing( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo );
static slong cb_FPE_RadialReachFeature( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo );
static slong cb_FPE_ExtractCircleRegion( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RSLT_ADD_PNT_INFO *ptSrchRsltInfo, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo, slong nCamDirection );
static slong cb_FPE_ExtractCircleRegion4FrontCAM( const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RSLT_ADD_PNT_INFO *ptSrchRsltInfo, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo );
static slong cb_FPE_HaarLikeChkEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_RECT_RGN* const ptSrchRgn );
static slong cb_FPE_HaarLikeVertEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_RECT_RGN* const ptSrchRgn );
static slong cb_FPE_HaarLikeHoriEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_RECT_RGN* const ptSrchRgn );
static slong cb_FPE_SearchCandCrossPnt( const IMPLIB_IMGID* const pnImgWk, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, CB_IMG_POINT* ptCrossPnt, slong* pnNumCrossPnt, const CB_RECT_RGN* const ptSrchRgn, slong nMaxNumPnt, slong nCamDirection, slong nThrHaarFtrCrossPnt, const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, uchar nDoubleType);
static slong cb_FPE_SearchCandTPntUL( const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_IMG_POINT* const ptCrossPnt, slong nNumCrossPnt, 
										CB_IMG_POINT* ptDstPnt, slong* pnNumDstPnt, const CB_RECT_RGN* const ptSrchRgn, enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos );
static slong cb_FPE_SearchCandTPntLR( const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_IMG_POINT* const ptCrossPnt, slong nNumCrossPnt, 
										CB_IMG_POINT* ptDstPnt, slong* pnNumDstPnt, const CB_RECT_RGN* const ptSrchRgn, enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos );
static slong cb_FPE_ZoomIn4Coordinates( CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo );
static slong cb_FPE_PrecisionPositioning4Checkerboard( const IMPLIB_IMGID nImgSrc, CB_FPE_SRCH_RSLT_PNT_INFO *ptSrchRsltPntInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo, slong nCamDirection );
static slong cb_FPE_EstimateFtrPoint4EdgeOfPattern( const IMPLIB_IMGID nImgSrc, CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo, slong nCamDirection );
static slong cb_FPE_methodFourPointCross( IMPLIB_IMGID nImgSrc, slong nCrossX, slong nCrossY, t_cb_img_CenterPos *ptPrecisePnt );
static slong cb_FPE_MedianLineFilter( const slong* const pnSrc, slong* pnDst, slong nNumData );
static slong cb_FPE_Sort( slong* pnValue, slong nNumValue );
static slong cb_FPE_StorePixelValue( const CB_FPE_IMG_ACCESS_INFO* const ptImgAccessInfo, const CB_FPE_IMG_ACCESS_INFO* const ptStore, enum enum_CB_FPE_SCAN_DIR_X nScanDirX, enum enum_CB_FPE_SCAN_DIR_Y nScanDirY, enum enum_CB_FPE_SHUFFLE_MODE nShuffleMode );
static slong cb_FPE_LineEdgeFLT( const CB_FPE_IMG_ACCESS_INFO* const ptImgSrc, const CB_FPE_IMG_ACCESS_INFO* const ptEdgeDst );
static slong cb_FPE_ExtractEdgePoint( const CB_FPE_IMG_ACCESS_INFO* const ptEdgeSrc, const CB_FPE_IMG_ACCESS_INFO* ptPixelInfo, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt );
static slong cb_FPE_ExtractEdgePrecisePoint( const CB_FPE_IMG_ACCESS_INFO* const ptImgSrc, const CB_IMG_POINT* const ptEdgePnt, slong nNumEdgePnt, t_cb_img_CenterPos* ptEdgePrecPnt, slong* pnNumEdgePrecPnt, enum enum_CB_FPE_LINE_FLT_MODE nFltMode, enum enum_CB_FPE_SCAN_DIR_X nScanDirX, enum enum_CB_FPE_SCAN_DIR_Y nScanDirY );
static slong cb_FPE_ExtractEdgePrecisePoint4AddChk( const CB_FPE_IMG_ACCESS_INFO* const ptImgSrc, const CB_IMG_POINT* const ptEdgePnt, slong nNumEdgePnt, t_cb_img_CenterPos* ptEdgePrecPnt, slong* pnNumEdgePrecPnt, enum enum_CB_FPE_LINE_FLT_MODE nFltMode, enum enum_CB_FPE_SCAN_DIR_X nScanDirX, enum enum_CB_FPE_SCAN_DIR_Y nScanDirY );
static slong cb_FPE_LinearRegression4VertLine( const t_cb_img_CenterPos* const ptPoint, slong nNumPoint, double_t* pdSlope, double_t* pdIntercept );
static slong cb_FPE_LinearRegression4HoriLine( const t_cb_img_CenterPos* const ptPoint, slong nNumPoint, double_t* pdSlope, double_t* pdIntercept );
static slong cb_FPE_CalcIntersection( const CB_FPE_LINE_COEFF* const ptLineCoeff1, const CB_FPE_LINE_COEFF* const ptLineCoeff2, t_cb_img_CenterPos* ptIntersectPnt );
static slong cb_FPE_CalcLineCoeff_F( const float_t* const pfCoord1, const float_t* const pfCoord2, float_t* pfSlope, float_t* pfIntercept );
static slong cb_FPE_StoreResultData( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo, t_cb_img_CenterPosEx* const ptCenterPos, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgn, slong nCamDirection );
static slong cb_FPE_StoreResultCenterData( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo, CB_SELF_CALIB* ptselfCalibInfo, const CB_FPE_SRCH_RGN_INFO_EACH_CAM* const ptSrchRgn, slong nCamDirection );
static slong cb_FPE_CheckFtrPointPos( t_cb_img_CenterPosEx* const ptCenterPos, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgn, slong nCamDirection );
static slong cb_FPE_SetSearchRegionAddPattern( const CB_IMG_PATTERN_INFO* const ptPatternInfoAdd, const CB_IMG_CAMERA_PRM* const ptCamPrmAdd, const CB_IMG_INFO* const ptInfoAdd );
static slong cb_FPE_InvalidSetting(CB_SELF_CALIB *ptselfCalibInfo, CB_RECOG_CENT_PAT_LAYOUT_INFO *ptCCentPatLayoutInfo,  const ulong ulCentEssNum,  const ulong ulCentOtherNum);
static slong cb_FPE_CheckCenterEssCount(CB_RECOG_CENT_PAT_LAYOUT_INFO *ptCCentPatLayoutInfo, const ulong ulCentEssNum, const ulong ulCentOtherNum);
static slong cb_FPE_SettingForEstimate(CB_SELF_CALIB *ptselfCalibInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo);
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																													/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
static slong cb_FPE_CalcBrightnessSimilarity( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_INPUT* ptBrInput, float_t* pfSimilarity, float_t* pfContrast );
static slong cb_FPE_CalcBrightnessHistogram( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_BR_INFO* ptBrInfoTbl, CB_FPE_CBR_INPUT* ptBrInput );
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																												/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

#ifdef CB_FPE_CSW_ENABLE_TEST_MODE

static slong cb_FPE_TEST_GenerateTestData( const IMPLIB_IMGID nImgID, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, uchar nCamDirection );

#endif /* CB_FPE_CSW_ENABLE_TEST_MODE */

/* �f�o�b�O�֘A */
static slong cb_FPE_DBG_Init( void );
static slong cb_FPE_DBG_SetGridInfo( CB_FPE_DBG_GRID_INFO* ptGridInfo, const CB_FPE_CAM_PRM* const ptCamPrm, slong nCamDirection );

#ifdef CB_FPE_CSW_DEBUG_ON
static slong cb_FPE_DBG_SaveHaarLikeInfo( CB_FPE_HAAR_LIKE_FTR* ptHaarLike, schar *pFilePath );
static slong cb_FPE_DBG_SavePixelValue( IMPLIB_IMGID nImgID, schar *pFilePath );
#endif /* CB_FPE_CSW_DEBUG_ON */

static void cb_img_FtrPointExtractor_ParamInit( void );

static slong cb_FPE_CalcDistCheck( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo );

/********************************************************************
 * �O���֐��@��`
 ********************************************************************/

/******************************************************************************/
/**
 * @brief		�����_���o����(������)
 *
 * @param		�Ȃ�
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.10	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_img_FtrPointExtractor_Init( void )
{
	slong					nRet = CB_IMG_NG;

	if ( CB_TRUE == m_FPE_nFlagInit )
	{
		/* ���d�������̂���NG */
		return ( CB_IMG_NG_MULTIPLE_INIT );
	}

	/* 0�N���A */
	memset( &m_FPE_tInfo, 0x00, sizeof( CB_FPE_INFO ) );
	memset( &m_FPE_tWorkInfo, 0x00, sizeof( CB_FPE_WORK_INFO ) );
	memset( &m_FPE_tPrm, 0x00, sizeof( CB_FPE_PRM ) );

	/* �O���p�����[�^�l�̎擾 */
	nRet = cb_img_ParamInit();
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* �O���p�����[�^�̏����� */
	cb_img_FtrPointExtractor_ParamInit();
	
	/* �����x�ʒu���߂ƒT���̈���L���t���O�̏����� */
	cb_img_FtrPointExtractor_Reinit();

	/* �������m�� */
	nRet = cb_FPE_AllocImg( &(m_FPE_tInfo.nImg[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_ALLOC_IMG );
	}

	/* �J�����􉽃��W���[���̏����� */
	nRet = cb_CG_Init();
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG_CG_INIT );
	}

	/* �e��t���O�ݒ� */
	/* �������t���O */
	m_FPE_nFlagInit = CB_TRUE;

	/* �q�X�g�O�����ގ��x��MAX�l */
	m_FPE_fSimilarityMAX = sqrtf( 2.0f );

	/* �p�x�R�[�h�ϊ��e�[�u���̐ݒ� */
	nRet = cb_FPE_SetEdgeACTable( &( m_FPE_nCnvLUT_EdgeAC[0][0] ), &( m_FPE_nCnvLUT_EdgeAC[1][0] ) );
	if ( CB_IMG_OK != nRet ) 
	{
		return ( CB_IMG_NG );
	}

	/* [DEBUG] */
	nRet = cb_FPE_DBG_Init();
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �������Ԋi�[�p������ */
#ifdef TIME_MEASUREMENT_CALIB
	memset( &(RG_Tim_CALIB[0L]), 0x00, ( 30L * sizeof( RG_Time_Measure ) ) );
#endif /* TIME_MEASUREMENT_CALIB */
	
	return ( CB_IMG_OK );
}



/******************************************************************************/
/**
 * @brief			�O���p�����[�^�̎擾�Ə������֐��i�O�������j
 *
 * @param[in,out]	�Ȃ�			:,,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.12.20	F.Sano	�V�K�쐬
 *
 */
/******************************************************************************/
slong cb_img_FtrPointExtractor_MMParam_Init( void )
{
	slong	nRet = CB_IMG_NG;

	/* �O���p�����[�^�l�̎擾 */
	nRet = cb_img_ParamInit();
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* �O���p�����[�^�̏����� */
	cb_img_FtrPointExtractor_ParamInit();
	
	/* �����x�ʒu���߂ƒT���̈���L���t���O�̍ď����� */
	cb_img_FtrPointExtractor_Reinit();

	return ( CB_IMG_NG_MM_OBJECT_READ );
}


/******************************************************************************/
/**
 * @brief			�O���p�����[�^�l�擾�֐�
 *
 * @param[in,out]	�Ȃ�			:,,-,[-],
 *
 * @retval			�Ȃ�			:,����I��,value=0,[-],
 *
 * @date			2013.12.06	F.Sano	�V�K�쐬
 *
 */
/******************************************************************************/
static void cb_img_FtrPointExtractor_ParamInit( void )
{

	/* ���LOBJ����擾�����p�����[�^�l���Z�b�g */
	CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_Width();
	CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_Height();
	CB_FPE_PRM_HALF_WIDTH_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_HalfWidth();
	CB_FPE_PRM_HALF_HEIGHT_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_HalfHeight();
	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_X = (slong)cb_img_GetParam_PreciseSrchRgn_DistanceBasePnt_X();
	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_Y = (slong)cb_img_GetParam_PreciseSrchRgn_DistanceBasePnt_Y();

	/* �擾�����p�����[�^�l�𗘗p����l���Z�b�g */
	/* ���E�s�������x�ʒu���߁A�G�b�W�_�T���� */
//	CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN = CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN / 2L;
	CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN = 2L;

	return;
}


/******************************************************************************/
/**
 * @brief		�����_���o����(�ď�����)
 *
 * @param		�Ȃ�
 *
 * @retval		�Ȃ�
 *
 * @date		2013.12.12	F.Sano			�V�K�쐬
 *
 * @note		�����������̂����A�u�����x�ʒu���߁v�ƁA�u�T���̈���L���t���O�v�̍ď��������s��
 *				�{�ϐ��̏������͎ԗ��p�����[�^���X�V���ꂽ�^�C�~���O�ŏ��������������߁A
 *				�N�����ɂ̂ݏ�������������_���o����(������)�Ƃ͕ʓr�쐬�B
 */
/******************************************************************************/

static void cb_img_FtrPointExtractor_Reinit( void )
{
	slong					ni = 0L;
	CB_FPE_IMG_ACCESS_INFO	*ptImgAccessInfo = NULL;
	/* �p�����[�^ */
	slong	nprm_WidthSrchRgn = 0L, 
			nprm_HeightSrchRgn = 0L;

	/* �p�����[�^�擾 */
	nprm_WidthSrchRgn = CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN;
	nprm_HeightSrchRgn = CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN;

	/* �����x�ʒu���߂̍Đݒ� */
	ptImgAccessInfo = &(m_FPE_tWorkInfo.tPixelDataUL);
	ptImgAccessInfo->nXSize = nprm_WidthSrchRgn;
	ptImgAccessInfo->nYSize = nprm_HeightSrchRgn;
	ptImgAccessInfo->pnAddrY8 = (schar*)&(m_FPE_tWorkInfo.nPixelDataUL[0][0]);
	ptImgAccessInfo->ptRgn = NULL;

	ptImgAccessInfo = &(m_FPE_tWorkInfo.tPixelDataLR);
	ptImgAccessInfo->nXSize = nprm_WidthSrchRgn;
	ptImgAccessInfo->nYSize = nprm_HeightSrchRgn;
	ptImgAccessInfo->pnAddrY8 = (schar*)&(m_FPE_tWorkInfo.nPixelDataLR[0][0]);
	ptImgAccessInfo->ptRgn = NULL;

	ptImgAccessInfo = &(m_FPE_tWorkInfo.tEdgeRhoUL);
	ptImgAccessInfo->nXSize = nprm_WidthSrchRgn;
	ptImgAccessInfo->nYSize = nprm_HeightSrchRgn;
	ptImgAccessInfo->pnAddrY16 = &(m_FPE_tWorkInfo.nEdgeRhoUL[0][0]);
	ptImgAccessInfo->ptRgn = NULL;

	ptImgAccessInfo = &(m_FPE_tWorkInfo.tEdgeRhoLR);
	ptImgAccessInfo->nXSize = nprm_WidthSrchRgn;
	ptImgAccessInfo->nYSize = nprm_HeightSrchRgn;
	ptImgAccessInfo->pnAddrY16 = &(m_FPE_tWorkInfo.nEdgeRhoLR[0][0]);
	ptImgAccessInfo->ptRgn = NULL;

	/* �T���̈���L���t���O */
	for ( ni = 0L; ni < E_CB_SELFCALIB_CAMPOS_MAX; ni++ )
	{
		( (m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam )[ni].nFlagValidInfo4Pattern		= CB_FALSE;
		( (m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam )[ni].nFlagValidInfo4AddPattern	= CB_FALSE;
	}

	return;
}

/******************************************************************************/
/**
 * @brief		�T���̈�̐ݒ�
 *
 * @param[in]	ptPatternInfo	:,�����p�^�[�����\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCamPrm		:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptInfo			:,�����_���o���͏��\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.20	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_img_SetSearchRegion( const CB_IMG_PATTERN_INFO* const ptPatternInfo, const CB_IMG_CAMERA_PRM* const ptCamPrm, const CB_IMG_INFO* const ptInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	nCamDirection = E_CB_SELFCALIB_CAMPOS_MAX;

	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �J����������� */
	nCamDirection = ptInfo->nCamDirection;

	/* �J���������͈̔̓`�F�b�N */
	if ( ( nCamDirection < E_CB_SELFCALIB_CAMPOS_FRONT ) || ( E_CB_SELFCALIB_CAMPOS_MAX <= nCamDirection   ) )
	{
		return ( CB_IMG_NG_INVALID_CAMDIRECTION );
	}

	/* �T���̈���L���t���O */
	/* cb_img_SetSearchRegion()�̊J�n�����Ŗ�����Ԃɂ��A����I�����_�ŗL����Ԃɂ��� */
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4Pattern	= CB_FALSE;
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4AddPattern	= CB_FALSE;

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SET_SRCH_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �J�����p�����[�^�ݒ� */
	nRet = cb_FPE_SetCameraParam( ptCamPrm, &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		if ( 0L < m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].nCamID )
		{
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		}
		return ( CB_IMG_NG_SET_CAM_PRM );
	}

	/* �J�����p�����[�^�A�덷�ʐݒ� */
	nRet = cb_FPE_SetCameraErrParam( ptCamPrm, &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_CAM_ERR_PRM );
	}

	/* �s���̒T���̈�ݒ� */
	/* �T���̈�(World)�ݒ� */
	nRet = cb_FPE_SetSearchRegionWorld( ptPatternInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_RGN_WLD_CHK_PATTERN );
	}

	/* �T���̈�(Image)�ݒ� */
	nRet = cb_FPE_SetSearchRegionImg( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_CNV_W2I_RGN_CHK_PATTERN );
	}

	/* �T���̈�ݒ� */
	nRet = cb_FPE_SetSearchRegion( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[0]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_RGN_CHK_PATTERN );
	}

	/* �ǉ��p�^�[���̒T���̈�ݒ� */ /* �����I�ɒǉ��p�^�[���͍��E�ʂɔ��肵�č쐬���� */
	/* �ǉ��p�^�[���̔��� */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfo->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfo->nTypeRightPattern ) )
	{
		/* ���F�Ȃ��@�@�E�F�Ȃ��@�@�F*/
		/* NO OPERATION */
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfo->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfo->nTypeRightPattern ) )
	{
		/* ���F���ہ@�@�E�F���ہ@�@�F*/

		/* �T���̈�(World)�ݒ�(�ǉ��p�^�[��) */
		nRet = cb_FPE_SetSearchRegionWorld4AddPattern( ptPatternInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_LINE_CIRC_PATTERN );
		}

		/* �T���̈�(Image)�ݒ�(�ǉ��p�^�[��) */
		nRet = cb_FPE_SetSearchRegionImg4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_LINE_CIRC_PATTERN );
		}

		/* �T���̈�ݒ�(�ǉ��p�^�[��) */
		nRet = cb_FPE_SetSearchRegion4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_LINE_CIRC_PATTERN );
		}
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfo->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfo->nTypeRightPattern ) )
	{
		/* ���F�s��(2x2) �E�F�s��(2x2) �F*/
		/* �T���̈�(World)�ݒ�(�ǉ��p�^�[��) */
		nRet = cb_FPE_SetSearchRegionWorld4AddChkboard( ptPatternInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), nCamDirection );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_ADD_CHK_PATTERN );
		}

		/* �T���̈�(Image)�ݒ�(�ǉ��p�^�[��) */
		nRet = cb_FPE_SetSearchRegionImg4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_ADD_CHK_PATTERN );
		}

		/* �T���̈�ݒ�(�ǉ��p�^�[��) */
		nRet = cb_FPE_SetSearchRegion4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_ADD_CHK_PATTERN );
		}
	}
	else
	{
		/* ���E�̃p�^�[�����قȂ�ꍇ */
		/* ����ł͂��肦�Ȃ��p�^�[���Ȃ̂ŁA����̓G���[�Ƃ���B�����I�ɂ͑Ή�����B  */
		return ( CB_IMG_NG_COMBI_ADD_PATTERN );
	}

	/* [DEBUG] */
	/* �O���b�h���̐ݒ� */
	nRet = cb_FPE_DBG_SetGridInfo( &(m_FPE_tDbgInfo.tGridInfo[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG );
	}

	/* �J�����p�����[�^�̍폜 */
	nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_DEL_CAM_PRM );
	}

	/* �T���̈���L���t���O */
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4Pattern	= CB_TRUE;
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4AddPattern	= CB_TRUE;

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SET_SRCH_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�T���̈�̐ݒ�(���E�^�[�Q�b�g)
 *
 * @param[in]	ptPatternInfoAdd	:,�����p�^�[�����\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCamPrmAdd			:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptInfoAdd			:,�����_���o���͏��\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.09.02	S.Morita		�V�K�쐬
 * @date		2014.12.01	S.Morita		�R�����g�ǋL
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionAddPattern( const CB_IMG_PATTERN_INFO* const ptPatternInfoAdd, const CB_IMG_CAMERA_PRM* const ptCamPrmAdd, const CB_IMG_INFO* const ptInfoAdd )
{
	slong							nRet = CB_IMG_NG;
	slong							nCamDirection = E_CB_SELFCALIB_CAMPOS_MAX;
	E_CB_LOGIC_KIND					EstimateLogicKind = E_CB_LOGIC_NO_ESTIMATION;
	CB_SELF_CALIB					selfCalibInfo;
	uchar							camPos = E_CB_SELFCALIB_CAMPOS_MAX;
	uchar							mapType = E_CB_MAPDATA_TYPE_MAX;
	CB_IMG_CAMERA_PRM				camPrmAdd;
	CB_EST_PARAM_COORD				estimateCoordinates;
	CB_CREATEMAP_CAMPARA			camParamReal;
	CB_CREATEMAP_CAMPARA			camParamVirtual;
	CB_CREATEMAP_CAMPARA			camEstParamReal;
	CB_CREATEMAP_CAMPARA			camEstParamVirtual;
	CB_IMG_CAMERA_PRM_ERR_INFO		*ptErrInfoSrcAdd = NULL;
	slong							ret_estimate_1st = E_CB_SELFCALIB_NG;	/* 1��ڂ̐��茋�� */
	slong							ret_check_1st = E_CB_SELFCALIB_NG;		/* 1��ڂ̐���l�͈̔̓`�F�b�N���� */	
	CB_RECOG_CENT_PAT_LAYOUT_INFO	CCentPatLayoutInfo;						/* �����p�^�[���z�u��� */

	/* �����_���o�\�ő吔 */
	ulong							ulCentEssNumMax = 0UL;					/* �����K�{�_�̒��o�\�ő吔 */
	ulong							ulCentOtherNumMax = 0UL;				/* �������̑��_�̒��o�\�ő吔 */

	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfoAdd ) || ( NULL == ptCamPrmAdd ) || ( NULL == ptInfoAdd ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �J�����p�����[�^���R�s�[ */
	camPrmAdd = *ptCamPrmAdd;

	/* �p�����[�^�擾 */
	/* �J�����p�����[�^�̊e�틖�e�덷��� */
	ptErrInfoSrcAdd = &( camPrmAdd.tErrInfo );

	/* �O���p�����[�^���菈���Ɏg�p����\���̂������� */
	memset(&estimateCoordinates, 0L, sizeof(CB_EST_PARAM_COORD));

	/* �J����������� */
	nCamDirection = ptInfoAdd->nCamDirection;

	/* �J���������͈̔̓`�F�b�N */
	if ( ( nCamDirection < E_CB_SELFCALIB_CAMPOS_FRONT ) || ( E_CB_SELFCALIB_CAMPOS_MAX <= nCamDirection   ) )
	{
		return ( CB_IMG_NG_INVALID_CAMDIRECTION );
	}

	/* �Z���t�L�����u���\���̂Ƀf�[�^���i�[  (�O���p�����[�^����̂��߂̏���)*/
	switch ( nCamDirection )
	{
		case E_CB_SELFCALIB_CAMPOS_FRONT:
			/* �t�����g�J���� */
			camPos = E_CB_SELFCALIB_CAMPOS_FRONT;
			mapType = E_CB_MAPDATA_TYPE_TOP_FRONT;
			break;
		case E_CB_SELFCALIB_CAMPOS_LEFT:
			/* �����J���� */
			camPos = E_CB_SELFCALIB_CAMPOS_LEFT;
			mapType = E_CB_MAPDATA_TYPE_TOP_LEFT;
			break;
		case E_CB_SELFCALIB_CAMPOS_RIGHT:
			/* �E���J���� */
			camPos = E_CB_SELFCALIB_CAMPOS_RIGHT;
			mapType = E_CB_MAPDATA_TYPE_TOP_RIGHT;
			break;
		case E_CB_SELFCALIB_CAMPOS_REAR:
			/* ���A�J���� */
			camPos = E_CB_SELFCALIB_CAMPOS_REAR;
			mapType = E_CB_MAPDATA_TYPE_TOP_REAR;
			break;
		default:
			/* default�̏ꍇ�̓J���������͈̔̓`�F�b�N�G���[�ł��邽�߁A�����Ȃ� */
			break;
	}

	/* �O���p�����[�^����̂��߁A�Z���t�L�����u�\���̂ɒl��ݒ� */
	nRet = cb_FPE_SettingForEstimate(&selfCalibInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[CB_PATTERN_POS_CENTER]));
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �����K�{�_�E�������̑��_�̎擾�������ݒ肷�� */
	/* �uselfCalibInfo.centerTargetPatternType�v��"uchar"�^�ł����A���ۂɂ�"CALIB_CHECKBOARD_PAT_TYPE"�^�̒l�������Ă��܂� */
	nRet = cb_recog_getCentPointNumMax(&ulCentEssNumMax, &ulCentOtherNumMax, selfCalibInfo.centerTargetPatternType);
	if(E_CB_RECOG_OK != nRet)
	{
		return ( CB_IMG_NG );
	}

	/* �����p�^�[���z�u��񏉊��� */
	memset(&CCentPatLayoutInfo, 0L, sizeof(CB_RECOG_CENT_PAT_LAYOUT_INFO));

	/* �����p�^�[���z�u����CB_SELF_CALIB����擾 */
	nRet = cb_selfCalib_getCentPatLayoutInfoFromCbSelfCalib(&(CCentPatLayoutInfo), &selfCalibInfo, camPos, (ulCentEssNumMax + ulCentOtherNumMax));
	if(E_CB_RECOG_OK != nRet)
	{
		return ( CB_IMG_NG );
	}

	/* �����K�{�_���T�_�擾�o���Ă��邩�𔻒� */
	nRet = cb_FPE_CheckCenterEssCount(&CCentPatLayoutInfo, ulCentEssNumMax, ulCentOtherNumMax);
	if( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* ���L�I�u�W�F�N�g������J�����p�����[�^�����擾 (�O���p�����[�^����̂��߂̏���) */
	nRet = cb_getShare_RealCamParam( camPos, &camParamReal );
	if( E_CB_SHARE_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* ���L�I�u�W�F�N�g���牼�z�J�����p�����[�^�����擾 (�O���p�����[�^����̂��߂̏���) */
	nRet = cb_getShare_VirtualCamParam( mapType, &camParamVirtual );
	if( E_CB_SHARE_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* �����s�������_�f�[�^�̊i�[ (�O���p�����[�^����̂��߂̏���) */ 
	nRet = cb_FPE_StoreResultCenterData( &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection]), &selfCalibInfo, &( ((m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam)[nCamDirection] ), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* ���E�^�[�Q�b�g�͎g�p���Ȃ� */
	selfCalibInfo.sideTargetNumDef = 0UL;

	/* �O���J�����p�����[�^����ɗp��������i�[����  (�O���p�����[�^����̂��߂̏���) */
	nRet = cb_selfCalib_getEstimateLogicKind(&EstimateLogicKind, &estimateCoordinates, &selfCalibInfo, camPos);
	if( E_CB_SELFCALIB_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �O���J�����p�����[�^���� */
	ret_estimate_1st = 	cb_selfCalib_estimateCamParam_ExtOnly(&estimateCoordinates, &camParamReal, &camParamVirtual, &camEstParamReal, &camEstParamVirtual);
	if( E_CB_SELFCALIB_OK == ret_estimate_1st )
	{
		/* �p�����[�^����͈̓`�F�b�N */
		ret_check_1st = cb_checkEstimatResult(&camEstParamReal, &camEstParamVirtual, &camParamReal, &camParamVirtual);
	}

	if((ret_estimate_1st != E_CB_SELFCALIB_OK) || (ret_check_1st != E_CB_SELFCALIB_OK))
	{
		/* �������̑��_�𖳌��l�ɐݒ肷�� */
		nRet = cb_FPE_InvalidSetting(&selfCalibInfo, &CCentPatLayoutInfo, ulCentEssNumMax, ulCentOtherNumMax);
		if( CB_IMG_OK != nRet )
		{
			/* �Đ��肪����I�����Ȃ������ꍇ�́A���������Őݒ肳�ꂽ�T���̈��p���� */
			return ( CB_IMG_NG );
		}

		/* �������̑��_�𖳌��l�ɐݒ肵�Ă��邽�߁A�����K�{�_�T�_���i�[����� */
		nRet = cb_selfCalib_getEstimateLogicKind(&EstimateLogicKind, &estimateCoordinates, &selfCalibInfo, camPos);
		if( E_CB_SELFCALIB_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* �O���J�����p�����[�^���� */
		nRet = 	cb_selfCalib_estimateCamParam_ExtOnly(&estimateCoordinates, &camParamReal, &camParamVirtual, &camEstParamReal, &camEstParamVirtual);
		if( E_CB_SELFCALIB_OK != nRet )
		{
			/* �Đ��肪����I�����Ȃ������ꍇ�́A���������Őݒ肳�ꂽ�T���̈��p���� */
			return ( CB_IMG_NG );
		}

		/* �p�����[�^����͈̓`�F�b�N */
		nRet = cb_checkEstimatResult(&camEstParamReal, &camEstParamVirtual, &camParamReal, &camParamVirtual);
		if ( E_CB_SELFCALIB_OK != nRet)
		{
			/* �Đ��茋�ʂ��͈͊O�ł���ꍇ�́A���������Őݒ肳�ꂽ�T���̈��p���� */
			return ( CB_IMG_NG );
		}
	}

	/* �J�����p�����[�^�ݒ� */
	nRet = cb_FPE_SetCameraParam( &camPrmAdd, &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		if ( 0L < m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].nCamID )
		{
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		}
		return ( CB_IMG_NG_SET_CAM_PRM );
	}

	//* �O���J�����p�����[�^����A�܂��͍Đ��肪����I�������ꍇ�́A���e�덷���X�V���� */
	ptErrInfoSrcAdd->dPitchDeg = CB_FPE_PRM_ERR_DPITCHDEG;	/* �s�b�`�p�덷 */
	ptErrInfoSrcAdd->dRollDeg = CB_FPE_PRM_ERR_DROLLDEG;	/* ���[���p�덷 */
	ptErrInfoSrcAdd->dYawDeg = CB_FPE_PRM_ERR_DYAWDEG;		/* ���[�p�덷 */
	ptErrInfoSrcAdd->dShiftX = CB_FPE_PRM_ERR_DSHIFTX;		/* �J�����ʒuX���W�덷 */
	ptErrInfoSrcAdd->dShiftY = CB_FPE_PRM_ERR_DSHIFTY;		/* �J�����ʒuY���W�덷 */
	ptErrInfoSrcAdd->dShiftZ = CB_FPE_PRM_ERR_DSHIFTZ;		/* �J�����ʒuX���W�덷 */
	ptErrInfoSrcAdd->dPosHori = CB_FPE_PRM_ERR_DPOSHORI;	/* ���������ʒu�덷 */
	ptErrInfoSrcAdd->dPosVert = CB_FPE_PRM_ERR_DPOSVERT;	/* ���������ʒu�덷 */

	/* �J�����p�����[�^�\���̂ɐ��茋�ʂ��i�[���� */
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dShiftX = camEstParamReal.px;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dShiftY = camEstParamReal.py;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dShiftZ = camEstParamReal.pz;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dPitchDeg = camEstParamReal.alpha;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dRollDeg = camEstParamReal.gamma;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dYawDeg = camEstParamReal.beta;

	/* �J�����p�����[�^�A�덷�ʐݒ� */
	nRet = cb_FPE_SetCameraErrParam( &camPrmAdd, &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_CAM_ERR_PRM );
	}

	/* ���E�^�[�Q�b�g�̒T���̈�ݒ� */ /* �����I�ɍ��E�^�[�Q�b�g�͍��E�ʂɔ��肵�č쐬���� */
	/* ���E�^�[�Q�b�g�̔��� */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfoAdd->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfoAdd->nTypeRightPattern ) )
	{
		/* ���F�Ȃ��@�@�E�F�Ȃ��@�@�F*/
		/* NO OPERATION */
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfoAdd->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfoAdd->nTypeRightPattern ) )
	{
		/* ���F���ہ@�@�E�F���ہ@�@�F*/

		/* �T���̈�(World)�ݒ�(���E�^�[�Q�b�g) */
		nRet = cb_FPE_SetSearchRegionWorld4AddPattern( ptPatternInfoAdd, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_LINE_CIRC_PATTERN );
		}

		/* �T���̈�(Image)�ݒ�(���E�^�[�Q�b�g) */
		nRet = cb_FPE_SetSearchRegionImg4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_LINE_CIRC_PATTERN );
		}

		/* �T���̈�ݒ�(���E�^�[�Q�b�g) */
		nRet = cb_FPE_SetSearchRegion4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_LINE_CIRC_PATTERN );
		}
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfoAdd->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfoAdd->nTypeRightPattern ) )
	{
		/* ���F�s��(2x2) �E�F�s��(2x2) �F*/
		/* �T���̈�(World)�ݒ�(���E�^�[�Q�b�g) */
		nRet = cb_FPE_SetSearchRegionWorld4AddChkboard( ptPatternInfoAdd, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), nCamDirection );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_ADD_CHK_PATTERN );
		}

		/* �T���̈�(Image)�ݒ�(���E�^�[�Q�b�g) */
		nRet = cb_FPE_SetSearchRegionImg4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_ADD_CHK_PATTERN );
		}

		/* �T���̈�ݒ�(���E�^�[�Q�b�g) */
		nRet = cb_FPE_SetSearchRegion4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* �J�����p�����[�^�̍폜 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_ADD_CHK_PATTERN );
		}
	}
	else
	{
		/* ���E�^�[�Q�b�g�̌`�󂪈قȂ�ꍇ */

		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_COMBI_ADD_PATTERN );
	}

	/* [DEBUG] */
	/* �O���b�h���̐ݒ� */
	nRet = cb_FPE_DBG_SetGridInfo( &(m_FPE_tDbgInfo.tGridInfo[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		/* �J�����p�����[�^�̍폜 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG );
	}

	/* �J�����p�����[�^�̍폜 */
	nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_DEL_CAM_PRM );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����s�������_�f�[�^�̊i�[
 *
 * @param[in]	ptSrchRsltInfo		:,�T�����ʏ��\���̂ւ̃|�C���^,-,[-],
 * @param[out]	ptselfCalibInfo		:,�����_���o���ʂ��i�[����Z���t�L�����u���ʏ��\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgn			:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.09.03	S.Morita		�V�K�쐬
 *
 * @note		����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����
 *
 */
/******************************************************************************/
static slong cb_FPE_StoreResultCenterData( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo, CB_SELF_CALIB* ptselfCalibInfo, const CB_FPE_SRCH_RGN_INFO_EACH_CAM* const ptSrchRgn, slong nCamDirection )
{
	slong							nRet = CB_IMG_OK;
	CB_FPE_SRCH_RSLT_PNT_INFO		*ptSrchRsltPnt = NULL;
	enum enum_CB_IMG_ALGORITHM		nTypeAlgorithm = CB_IMG_ALGORITHM_DEFAULT;
	
	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRsltInfo ) || ( NULL == ptselfCalibInfo ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �����s���̓����_���W���i�[���Ă���\���̂Ƀ|�C���^��ݒ� */
	ptSrchRsltPnt = &(ptSrchRsltInfo->tSrchRsltPntInfo[0]);
	
	/* �t�����g�J�����A�܂��̓��A�J�����̏ꍇ */
	if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
	{
		/* 9�_�̓����_���W��ptselfCalibInfo�Ɋi�[���� */
		( ptselfCalibInfo->centerTarget )[0].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].w;
		( ptselfCalibInfo->centerTarget )[0].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].h;
		( ptselfCalibInfo->centerTarget )[1].px = ( ptSrchRsltPnt->tCrossPntPrecise )[1].w;
		( ptselfCalibInfo->centerTarget )[1].py = ( ptSrchRsltPnt->tCrossPntPrecise )[1].h;
		( ptselfCalibInfo->centerTarget )[2].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].w;
		( ptselfCalibInfo->centerTarget )[2].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].h;
		( ptselfCalibInfo->centerTarget )[3].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].w;
		( ptselfCalibInfo->centerTarget )[3].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].h;
		( ptselfCalibInfo->centerTarget )[4].px = ( ptSrchRsltPnt->tCrossPntPrecise )[0].w;
		( ptselfCalibInfo->centerTarget )[4].py = ( ptSrchRsltPnt->tCrossPntPrecise )[0].h;
		( ptselfCalibInfo->centerTarget )[5].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].w;
		( ptselfCalibInfo->centerTarget )[5].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].h;
		( ptselfCalibInfo->centerTarget )[6].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[2].w;
		( ptselfCalibInfo->centerTarget )[6].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[2].h;
		( ptselfCalibInfo->centerTarget )[7].px = ( ptSrchRsltPnt->tCrossPntPrecise )[2].w;
		( ptselfCalibInfo->centerTarget )[7].py = ( ptSrchRsltPnt->tCrossPntPrecise )[2].h;
		( ptselfCalibInfo->centerTarget )[8].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[2].w;
		( ptselfCalibInfo->centerTarget )[8].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[2].h;
	}
	/* �����J�����A�܂��͉E���J�����̏ꍇ */
	else if ( ( E_CB_SELFCALIB_CAMPOS_LEFT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_RIGHT == nCamDirection ) )
	{
		/* �����s����"2x3�̎s��"�ȊO�̌`��ł���ꍇ */
		if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ptSrchRgn->tSrchRgnInfo4Pattern[0].tPtnInfo.nTypePattern )
		{
			/* 7�_�̓����_���W��ptselfCalibInfo�Ɋi�[���� */
			( ptselfCalibInfo->centerTarget )[0].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].w;
			( ptselfCalibInfo->centerTarget )[0].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].h;
			( ptselfCalibInfo->centerTarget )[1].px = ( ptSrchRsltPnt->tCrossPntPrecise )[0].w;
			( ptselfCalibInfo->centerTarget )[1].py = ( ptSrchRsltPnt->tCrossPntPrecise )[0].h;
			( ptselfCalibInfo->centerTarget )[2].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].w;
			( ptselfCalibInfo->centerTarget )[2].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].h;
			( ptselfCalibInfo->centerTarget )[3].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].w;
			( ptselfCalibInfo->centerTarget )[3].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].h;
			( ptselfCalibInfo->centerTarget )[4].px = ( ptSrchRsltPnt->tTPntLeftPrecise )[0].w;
			( ptselfCalibInfo->centerTarget )[4].py = ( ptSrchRsltPnt->tTPntLeftPrecise )[0].h;
			( ptselfCalibInfo->centerTarget )[5].px = ( ptSrchRsltPnt->tTPntRightPrecise )[0].w;
			( ptselfCalibInfo->centerTarget )[5].py = ( ptSrchRsltPnt->tTPntRightPrecise )[0].h;
			( ptselfCalibInfo->centerTarget )[6].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].w;
			( ptselfCalibInfo->centerTarget )[6].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].h;
		}
		else
		{
			/* �A���S���Y���^�C�v�̔��� */
			nRet = cb_FPE_GetTypeAlgorithm( &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ), &nTypeAlgorithm );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
			
			/* �A���S���Y���^�C�v��"�����̓����_���o�A���S���Y��"�ł���ꍇ */
			if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
			{
				/* �s���p�^�[���̌�����"���̌���(�s���͗l�̍ō����̐F����)"�ł���ꍇ */
				if ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8�_�̓����_���W��ptselfCalibInfo�Ɋi�[���� */
					( ptselfCalibInfo->centerTarget )[0].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[0].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[1].px = ( ptSrchRsltPnt->tCrossPntPrecise )[0].w;	
					( ptselfCalibInfo->centerTarget )[1].py = ( ptSrchRsltPnt->tCrossPntPrecise )[0].h;	
					( ptselfCalibInfo->centerTarget )[2].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[2].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[3].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[3].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[4].px = ( ptSrchRsltPnt->tTPntLeftPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[4].py = ( ptSrchRsltPnt->tTPntLeftPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[5].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[2].w;
					( ptselfCalibInfo->centerTarget )[5].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[2].h;
					( ptselfCalibInfo->centerTarget )[6].px = ( ptSrchRsltPnt->tCrossPntPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[6].py = ( ptSrchRsltPnt->tCrossPntPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[7].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[7].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].h;
				}
				/* �s���p�^�[���̌�����"���̌���(�s���͗l�̍ō����̐F����)"�ł���ꍇ */
				else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8�_�̓����_���W��ptselfCalibInfo�Ɋi�[���� */
					( ptselfCalibInfo->centerTarget )[0].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[0].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[1].px = ( ptSrchRsltPnt->tCrossPntPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[1].py = ( ptSrchRsltPnt->tCrossPntPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[2].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[2].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[3].px = ( ptSrchRsltPnt->tTPntLeftPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[3].py = ( ptSrchRsltPnt->tTPntLeftPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[4].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[4].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[5].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[5].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[6].px = ( ptSrchRsltPnt->tCrossPntPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[6].py = ( ptSrchRsltPnt->tCrossPntPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[7].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[2].w;
					( ptselfCalibInfo->centerTarget )[7].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[2].h;
				}
				/* �s���p�^�[���̌�����"���̌���"�ł�"���̌���"�ł��Ȃ��ꍇ */
				else
				{
					return ( CB_IMG_NG );
				}
			}
			/* �A���S���Y���^�C�v��"�E���̓����_���o�A���S���Y��"�ł���ꍇ */
			else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
			{
				/* �s���p�^�[���̌�����"���̌���(�s���͗l�̍ō����̐F����)"�ł���ꍇ */
				if ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8�_�̓����_���W��ptselfCalibInfo�Ɋi�[���� */
					( ptselfCalibInfo->centerTarget )[0].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[0].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[1].px = ( ptSrchRsltPnt->tCrossPntPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[1].py = ( ptSrchRsltPnt->tCrossPntPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[2].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[2].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[3].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[3].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[4].px = ( ptSrchRsltPnt->tCrossPntPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[4].py = ( ptSrchRsltPnt->tCrossPntPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[5].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[5].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[6].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[2].w;
					( ptselfCalibInfo->centerTarget )[6].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[2].h;
					( ptselfCalibInfo->centerTarget )[7].px = ( ptSrchRsltPnt->tTPntRightPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[7].py = ( ptSrchRsltPnt->tTPntRightPrecise )[0].h;
				}
				/* �s���p�^�[���̌�����"���̌���(�s���͗l�̍ō����̐F����)"�ł���ꍇ */
				else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8�_�̓����_���W��ptselfCalibInfo�Ɋi�[���� */
					( ptselfCalibInfo->centerTarget )[0].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[0].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[1].px = ( ptSrchRsltPnt->tCrossPntPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[1].py = ( ptSrchRsltPnt->tCrossPntPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[2].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].w;
					( ptselfCalibInfo->centerTarget )[2].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[1].h;
					( ptselfCalibInfo->centerTarget )[3].px = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[3].py = ( ptSrchRsltPnt->tTPntUpperPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[4].px = ( ptSrchRsltPnt->tCrossPntPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[4].py = ( ptSrchRsltPnt->tCrossPntPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[5].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[5].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[6].px = ( ptSrchRsltPnt->tTPntRightPrecise )[0].w;
					( ptselfCalibInfo->centerTarget )[6].py = ( ptSrchRsltPnt->tTPntRightPrecise )[0].h;
					( ptselfCalibInfo->centerTarget )[7].px = ( ptSrchRsltPnt->tTPntLowerPrecise )[2].w;
					( ptselfCalibInfo->centerTarget )[7].py = ( ptSrchRsltPnt->tTPntLowerPrecise )[2].h;
				}
				/* �s���p�^�[���̌�����"���̌���"�ł�"���̌���"�ł��Ȃ��ꍇ */
				else
				{
					return ( CB_IMG_NG );
				}
			}
			/* �A���S���Y���^�C�v��"�����̓����_���o�A���S���Y��"�ł�"�E���̓����_���o�A���S���Y��"�ł��Ȃ��ꍇ */
			else
			{
				return ( CB_IMG_NG );
			}
		}
	}
	/* �t�����g�J�����E���A�J�����E�����J�����E�E���J�����̂ǂ�ł������ꍇ */
	else
	{
		/* �������Ȃ� */
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�������̑��_�̖����l�ݒ�
 *
 * @param[in,out]	ptselfCalibInfo			:,�Z���t�L�����u���\����,-,[-],
 * @param[in]		ptCCentPatLayoutInfo	:,���������p�^�[���z�u���,-,[-],
 * @param[in]		ulCentEssNum			:,�����K�{�_�̌�,-,[-],
 * @param[in]		ulCentOtherNum			:,�������̑��_�̌�,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG			:,�ُ�I��,value=-1,[-],
 *
 * @date		2014.11.28	S.Morita		�V�K�쐬
 * @date		2014.12.01	S.Morita		�����̏C��
 *
 * @note		
 *
 */
/******************************************************************************/
static slong cb_FPE_InvalidSetting(CB_SELF_CALIB *ptselfCalibInfo, CB_RECOG_CENT_PAT_LAYOUT_INFO *ptCCentPatLayoutInfo,  const ulong ulCentEssNum,  const ulong ulCentOtherNum)
{
	ulong	targetCount;							/* ���[�v�p */
	slong	nRet = E_CB_RECOG_NG;
	BOOL	bIsCentEssPoint = FALSE;				/* �����K�{�_���ۂ� */
	ulong	ulCenterOhterNgNum = 0UL;				/* �������̑��_�ł���A�����l�ł�������_�̌� */

	/* �����`�F�b�N */
	if ( ( NULL == ptselfCalibInfo ) || ( NULL == ptCCentPatLayoutInfo ) )
	{
		return ( CB_IMG_NG );
	}

	for(targetCount = 0UL; targetCount < (ulCentEssNum+ulCentOtherNum); targetCount++)
	{
		/* �����K�{�_���ۂ����擾 */
		nRet = cb_recog_isCentEssPoint(&bIsCentEssPoint, ptCCentPatLayoutInfo, targetCount);
		if(E_CB_RECOG_OK != nRet)
		{
			return ( CB_IMG_NG );
		}
		else if(FALSE == bIsCentEssPoint)
		{	/* ���̑��_ */
			if(TRUE == cb_recog_isValidPickupResult(ptselfCalibInfo->centerTarget[targetCount].px, ptselfCalibInfo->centerTarget[targetCount].py))
			{
				/* �������̑��_�𖳌��l�ɐݒ肷�� */
				ptselfCalibInfo->centerTarget[targetCount].px = -1.0;
				ptselfCalibInfo->centerTarget[targetCount].py = -1.0;
			}
			else
			{
				ulCenterOhterNgNum++;
			}
		}
		else{	/* �K�{ */
			/* �������Ȃ� */
		}
	}

	/* �P�x�ڂ̐�����s�����ۂɒ������̑��_���S�Ė����l�ł������ꍇ�́A�����_���ς��Ȃ��̂ŁA�Đ���͍s�킸�ɏI���B */
	if( ulCenterOhterNgNum == ulCentOtherNum )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����K�{�_�̎擾������
 *
 * @param[in]		ptCCentPatLayoutInfo	:,���������p�^�[���z�u���,-,[-],
 * @param[in]		ulCentEssNum			:,�����K�{�_�̌�,-,[-],
 * @param[in]		ulCentOtherNum			:,�������̑��_�̌�,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG			:,�ُ�I��,value=-1,[-],
 *
 * @date		2014.11.28	S.Morita		�V�K�쐬
 * @date		2014.12.01	S.Morita		�����̏C����
 *
 * @note		
 *
 */
/******************************************************************************/
static slong cb_FPE_CheckCenterEssCount(CB_RECOG_CENT_PAT_LAYOUT_INFO *ptCCentPatLayoutInfo, const ulong ulCentEssNum, const ulong ulCentOtherNum)
{
	ulong			targetCount;
	slong			nRet = E_CB_RECOG_NG;
	ulong			centerEsscount = 0UL;					/* �����K�{�_�̌� */
	BOOL			bIsCentEssPoint = FALSE;				/* �����K�{�_���ۂ� */

	/* �����`�F�b�N */
	if( NULL == ptCCentPatLayoutInfo )
	{
		return ( CB_IMG_NG );
	}

	for(targetCount = 0UL; targetCount < (ulCentEssNum+ulCentOtherNum); targetCount++)
	{
		/* �����K�{�_���ۂ����擾 */
		nRet = cb_recog_isCentEssPoint(&bIsCentEssPoint, ptCCentPatLayoutInfo, targetCount);
		if(E_CB_RECOG_OK != nRet)
		{
			return ( CB_IMG_NG );
		}
		else if(TRUE == bIsCentEssPoint)
		{
			centerEsscount++;
		}
		else
		{
			/* NO OPERATION */
		}
	}

	/* �����K�{�_�T�_���擾�ł��Ă��Ȃ��ꍇ�͏������I������ */
	if( ulCentEssNum != centerEsscount )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�O���p�����[�^����̂��߁A�Z���t�L�����u�\���̂ɒl��ݒ肷��
 *
 * @param[in,out]	ptselfCalibInfo			:,�Z���t�L�����u���\����,-,[-],
 * @param[in]		ptSrchRgnInfo			:,�T���̈���\����,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG			:,�ُ�I��,value=-1,[-],
 *
 * @date		2014.11.28	S.Morita		�V�K�쐬
 * @date		2014.12.01	S.Morita		�����̏C����
 *
 * @note		
 *
 */
/******************************************************************************/
static slong cb_FPE_SettingForEstimate(CB_SELF_CALIB *ptselfCalibInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo)
{
	ulong	i = 0UL;

	/* �����`�F�b�N */
	if( (NULL == ptselfCalibInfo) || (NULL == ptSrchRgnInfo) )
	{
		return ( CB_IMG_NG );
	}

	/* �s���^�[�Q�b�g���S���W���擾���� */
	ptselfCalibInfo->centerTargetNumDef = ptSrchRgnInfo->nNumCenterPnt;
	for( i = 0UL; i < (ptselfCalibInfo->centerTargetNumDef); i++ )
	{
		ptselfCalibInfo->centerTargetDef[i].px = ptSrchRgnInfo->tCenterPntWld[i].dX;
		ptselfCalibInfo->centerTargetDef[i].py = ptSrchRgnInfo->tCenterPntWld[i].dY;
		ptselfCalibInfo->centerTargetDef[i].pz = ptSrchRgnInfo->tCenterPntWld[i].dZ;
	}

	/* ����(�i�q)�p�^�[���^�C�v */
	ptselfCalibInfo->centerTargetPatternType = ptSrchRgnInfo->tPtnInfo.nTypePattern;			/* "enum_CB_IMG_PATTERN_TYPE"�^���L���X�g���Ă��܂��B0�`5�܂ł������Ȃ��̂Ŗ��Ȃ� */

	/* �I�t�Z�b�g�^�C�v(����/�E��/���񂹓�) */
	ptselfCalibInfo->centerTargetPointOffset = ptSrchRgnInfo->tPtnInfo.nCenterOffsetType;

	/* ����/���� */
	ptselfCalibInfo->centerTargetPlacement = ptSrchRgnInfo->tPtnInfo.nFlagPlacement;			/* "enum_CB_IMG_CHKBOARD_PLACEMENT"�^���L���X�g���Ă��܂��B0�`2�܂ł������Ȃ��̂Ŗ��Ȃ� */

	/* ���C�A�E�g��� */
	ptselfCalibInfo->layoutType = ptSrchRgnInfo->tPtnInfo.nlayoutType;

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief		�����_���o����(�{��)
 *
 * @param[in]	nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptResult		:,�����_���o�o�͏��\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptInfo			:,�����_���o���͏��\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptPatternInfo	:,�����p�^�[�����\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCamPrm		:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.10	K.Kato			�V�K�쐬
 * @date		2013.11.15	F.Sano			�ǉ��p�^�[���̔��ʏ����ǉ�
 * @date		2015.09.14	S.Morita		�s�N�Z�����x�E�T�u�s�N�Z�����x���W�l�Ԃ̒���������p���������ǉ�
 */
/******************************************************************************/
slong cb_img_getCenterPosEx( const IMPLIB_IMGID nImgSrc, CB_IMG_RESULT* const ptResult, const CB_IMG_INFO* const ptInfo, const CB_IMG_PATTERN_INFO* const ptPatternInfo, const CB_IMG_CAMERA_PRM* const ptCamPrm )
{
	slong		nRet = CB_IMG_NG;
	slong		nSxSrc0 = 0L, 
				nSySrc0 = 0L, 
				nExSrc0 = 0L, 
				nEySrc0 = 0L;
	slong		nSxSrc1 = 0L, 
				nSySrc1 = 0L, 
				nExSrc1 = 0L, 
				nEySrc1 = 0L;
	slong		nSxDst = 0L, 
				nSyDst = 0L, 
				nExDst = 0L, 
				nEyDst = 0L;
	slong		nSxSys = 0L, 
				nSySys = 0L, 
				nExSys = 0L, 
				nEySys = 0L;
	slong		nCamDirection = 0L, 
				nCalibType = 0L;
	slong		nFlagExec = CB_FALSE;
	CB_RECT_RGN	*ptSrchRgn = NULL, 
				*ptSrchRgnZoomOut = NULL;
	/* �p�����[�^ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	enum enum_CB_IMG_PATTERN_TYPE		nPtnType_Left = CB_IMG_PATTERN_TYPE_NONE ,
										nPtnType_Right = CB_IMG_PATTERN_TYPE_NONE;

	/* �����`�F�b�N */
	if ( ( NULL == ptResult ) || ( NULL == ptInfo ) || ( NULL == ptPatternInfo ) || ( NULL == ptCamPrm ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_IMG_TOTAL] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �p�����[�^ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* �J�������� */ 
	nCamDirection = ptInfo->nCamDirection;
	/* �J���������͈̔̓`�F�b�N */
	if ( ( nCamDirection < E_CB_SELFCALIB_CAMPOS_FRONT ) || ( E_CB_SELFCALIB_CAMPOS_MAX <= nCamDirection ) )
	{
		return ( CB_IMG_NG_INVALID_CAMDIRECTION );
	}

	/* �L�����u��� */
	nCalibType = ptInfo->nCalibType;

	/* ���s�����`�F�b�N */
	nFlagExec = cb_FPE_CheckExecCondition( m_FPE_nFlagInit, &( ( (m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam )[nCamDirection] ) );
	if ( CB_TRUE != nFlagExec )
	{
		return ( CB_IMG_NG_CHK_EXEC_CONDITION );
	}

	/* ���ʃN���A */
	nRet = cb_FPE_ClearResult( ptResult );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_CLEAR_RESULT );
	}

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

#ifdef CB_FPE_CSW_ENABLE_TEST_MODE
	cb_FPE_TEST_GenerateTestData( nImgSrc, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), nCamDirection );
#endif /* CB_FPE_CSW_ENABLE_TEST_MODE */

#ifdef CB_FPE_CSW_DEBUG_ON
#if 0
	{
		slong	ndbg_sxSys, ndbg_sySys, ndbg_exSys, ndbg_eySys;

		nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &ndbg_sxSys, &ndbg_sySys, &ndbg_exSys, &ndbg_eySys );
		nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0L, 0L, 719L, 479L );
//		nRet = implib_SaveBMPFile( nImgSrc, "C:/work/work_kato/SELF_CB_IMG_SRC.bmp", IMPLIB_BW_BITMAP );
		nRet = cb_FPE_DBG_SavePixelValue( nImgSrc, "C:/work/work_kato/SELF_CB_IMG_SRC.csv" );
		nRet = implib_SetWindow( IMPLIB_SYS_WIN, ndbg_sxSys, ndbg_sySys, ndbg_exSys, ndbg_eySys );
	}
#endif
#endif /* CB_FPE_CSW_DEBUG_ON */

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_CHECKERBOARD_TOTAL] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �k���摜 */
	ptSrchRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[0].tRgnImgHLsrc);
	ptSrchRgnZoomOut = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[0].tRgnImgZoomOutHLsrc);
#if 0
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, ptSrchRgnZoomOut->nSX, ptSrchRgnZoomOut->nSY, ptSrchRgnZoomOut->nEX, ptSrchRgnZoomOut->nEY );
#else
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, 0L, 0L, 719L, 479L );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, 0L, 0L, 359L, 239L );
#endif
	nRet = implib_IP_ZoomOutExt( nImgSrc, m_FPE_tInfo.nImg[CB_FPE_IMG_ZOOMOUT2x2], nprm_xMagZoomOut, nprm_yMagZoomOut );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMG_ZOOM_OUT );
	}

	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, 0L, 0L, 719L, 479L );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, 0L, 0L, 719L, 479L );
	nRet = implib_IP_ZoomOutExt( nImgSrc, m_FPE_tInfo.nImg[CB_FPE_IMG_NORMAL], 1L, 1L );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMG_ZOOM_OUT );
	}

#ifdef CB_FPE_CSW_DEBUG_ON
#if 0
	{
		slong	ndbg_sxSys, ndbg_sySys, ndbg_exSys, ndbg_eySys;

		nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &ndbg_sxSys, &ndbg_sySys, &ndbg_exSys, &ndbg_eySys );
		nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0L, 0L, 359L, 239L );
		nRet = implib_SaveBMPFile( m_FPE_tInfo.nImg[CB_FPE_IMG_ZOOMOUT2x2], "C:/work/work_dbg/SELF_CB_IMG_SRC_ZOOMOUT.bmp", IMPLIB_BW_BITMAP );
		nRet = cb_FPE_DBG_SavePixelValue( m_FPE_tInfo.nImg[CB_FPE_IMG_ZOOMOUT2x2], "C:/work/work_dbg/SELF_CB_IMG_SRC_ZOOMOUT.csv" );
		nRet = implib_SetWindow( IMPLIB_SYS_WIN, ndbg_sxSys, ndbg_sySys, ndbg_exSys, ndbg_eySys );
	}
#endif
#endif /* CB_FPE_CSW_DEBUG_ON */

	/* Checkerboard�ɂ���������_���o */
	nRet = cb_FPE_ExtractFtrPoint4Checkerboard( nImgSrc, &m_FPE_tInfo, nCamDirection, nCalibType );
	if ( CB_IMG_OK != nRet )
	{
		/* �����֐��̃G���[�R�[�h�����̂܂ܕԂ�����!(�G���[��Ԃ��ڍׂɐ؂蕪���邽��) */
		return ( nRet );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_CHECKERBOARD_TOTAL] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_CIRCLE_LINE_TOTAL] );
#endif /* TIME_MEASUREMENT_CALIB */

#ifdef CB_FPE_SEARCH_REGION_NARROW
	/* �T���͈͂̐ݒ�(���E�^�[�Q�b�g) */
	/* ����I�����Ȃ��ꍇ�͊��ɐݒ肳��Ă���T���͈͂�p���邽�߁A�߂�l�̔���͍s��Ȃ� */
	nRet = cb_FPE_SetSearchRegionAddPattern(ptPatternInfo, ptCamPrm, ptInfo);
#endif /* CB_FPE_SEARCH_REGION_NARROW */

	/* �p�^�[����Type���擾 */
	nPtnType_Left = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT].tPtnInfo.nTypePattern;
	nPtnType_Right = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT].tPtnInfo.nTypePattern;

	/* �ǉ��p�^�[���̒T���̈�ݒ� */ /* �����I�ɒǉ��p�^�[���͍��E�ʂɔ��肵�č쐬���� */
	/* �ǉ��p�^�[���̔��� */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Right ) )
	{
		/* ���F�Ȃ��@�@�E�F�Ȃ��@�@�F*/
		/* NO OPERATION */
		nRet = CB_IMG_OK;
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Right ) )
	{
		/* ���F���ہ@�@�E�F���ہ@�@�F*/
		/* �~�ƒ�������\�������p�^�[���ɂ���������_���o */
		nRet = cb_FPE_ExtractFtrPoint4CircleLine( nImgSrc, &m_FPE_tInfo, nCamDirection, nCalibType );

	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Right ) )
	{
		/* ���F�s��(2x2) �E�F�s��(2x2) �F*/
		/* ���E�s���p�^�[�������_���o */
		nRet = cb_FPE_ExtractFtrPoint4AddChk( nImgSrc, &m_FPE_tInfo, nCamDirection, nCalibType );
		if ( CB_IMG_OK != nRet )
		{
			/* �����֐��̃G���[�R�[�h�����̂܂ܕԂ�����!(�G���[��Ԃ��ڍׂɐ؂蕪���邽��) */
			return ( nRet );
		}

		/* ���E�}�[�J�[�̃s�N�Z�����x���W�l�ƃT�u�s�N�Z�����x���W�l�̒���������p�������� */
		nRet = cb_FPE_CalcDistCheck( &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection]) );

	}
	else
	{
		/* ���E�̃p�^�[�����قȂ�ꍇ */
		/* �{���ł͂��肦�Ȃ��p�^�[���Ȃ̂ŁA���ꂪ���������ꍇ�̓G���[��Ԃ�  */
		return( CB_IMG_NG_COMBI_ADD_PATTERN );
	}
	
	if ( CB_IMG_OK != nRet )
	{
		/* �����֐��̃G���[�R�[�h�����̂܂ܕԂ�����!(�G���[��Ԃ��ڍׂɐ؂蕪���邽��) */
		return ( nRet );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_CIRCLE_LINE_TOTAL] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_STORE_RESULT] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �o�̓f�[�^�i�[ */
	nRet = cb_FPE_StoreResultData( &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection]), &(ptResult->tFtrPoint), &( ((m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam)[nCamDirection] ), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_STORE_RESULT_DATA );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_STORE_RESULT] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_IMG_TOTAL] );
#endif /* TIME_MEASUREMENT_CALIB */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�T���̈�(�s���ɑ΂���)���擾
 *
 * @param[out]	ptSrchRgn			:,�̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.17	K.Kato			�V�K�쐬
 *
 * @note		�f�o�b�O�p�̎擾�֐��̂��߁A�{�֐����ł̈���nCamDirection�͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
slong cb_img_getSearchRgnInfo4Chkboard( CB_RECT_RGN* ptSrchRgn, slong nCamDirection )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6707
{
	slong	ni = 0L;
	slong	nPatternPosMaxNum = 0L;
	CB_FPE_SRCH_RGN_INFO_EACH_CAM	*ptSrchRgnInfo = NULL;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�^�[���� */
	nPatternPosMaxNum = CB_PATTERN_POS_MAXNUM;

	/* �擪�|�C���^ */
	ptSrchRgnInfo = &( m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection] );

	for ( ni = 0L; ni < nPatternPosMaxNum; ni++ )
	{
		/* ���W�l�Z�b�g */
		ptSrchRgn[ni] = ptSrchRgnInfo->tSrchRgnInfo4Pattern[ni].tRgnImg;
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�T���̈�(���E�ۂɑ΂���)���擾
 *
 * @param[out]	ptSrchRgn			:,�̈�\���̂ւ̃|�C���^(CB_ADD_PATTERN_POS_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.17	K.Kato			�V�K�쐬
 *
 * @note		�f�o�b�O�p�̎擾�֐��̂��߁A�{�֐����ł̈���nCamDirection�͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
slong cb_img_getSearchRgnInfo4LineCircle( CB_RECT_RGN* ptSrchRgn, slong nCamDirection )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6708
{
	slong	ni = 0L;
	slong	nPatternPosMaxNum = 0L;
	CB_FPE_SRCH_RGN_INFO_EACH_CAM	*ptSrchRgnInfo = NULL;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�^�[���� */
	nPatternPosMaxNum = CB_ADD_PATTERN_POS_MAXNUM;

	/* �擪�|�C���^ */
	ptSrchRgnInfo = &( m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection] );

	for ( ni = 0L; ni < nPatternPosMaxNum; ni++ )
	{
		/* ���W�l�Z�b�g */
		ptSrchRgn[ni] = ptSrchRgnInfo->tSrchRgnInfoAddPattern[ni].tRgnImg;
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����_�ʒu(�s���ɑ΂���)�擾
 *
 * @param[out]	ptCrossPnt			:,�\���_�ʒu�\���̂ւ̃|�C���^(CB_FPE_CROSS_PNT_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumCrossPnt		:,�\���_�ʒu�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[out]	ptTPntUpper			:,�s���ʒu(�㑤)�\���̂ւ̃|�C���^(CB_FPE_T_PNT_UPPER_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumTPntUpper		:,�s���ʒu(�㑤)�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[out]	ptTPntLower			:,�s���ʒu(����)�\���̂ւ̃|�C���^(CB_FPE_T_PNT_LOWER_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumTPntLower		:,�s���ʒu(����)�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[out]	ptTPntLeft			:,�s���ʒu(����)�\���̂ւ̃|�C���^(CB_FPE_T_PNT_LEFT_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumTPntLeft		:,�s���ʒu(����)�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[out]	ptTPntRight			:,�s���ʒu(�E��)�\���̂ւ̃|�C���^(CB_FPE_T_PNT_RIGHT_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumTPntRight		:,�s���ʒu(�E��)�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.17	K.Kato			�V�K�쐬
 *
 * @note		�f�o�b�O�p�̎擾�֐��̂��߁A�{�֐����ł̈���nCamDirection�͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
slong cb_img_getFtrPntPos4Chkboard( t_cb_img_CenterPos* ptCrossPnt, slong* pnNumCrossPnt, 						// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6709
								   t_cb_img_CenterPos* ptTPntUpper, slong* pnNumTPntUpper, 
								   t_cb_img_CenterPos* ptTPntLower, slong* pnNumTPntLower, 
								   t_cb_img_CenterPos* ptTPntLeft, slong* pnNumTPntLeft, 
								   t_cb_img_CenterPos* ptTPntRight, slong* pnNumTPntRight, slong nCamDirection )
{
	slong	ni = 0L;
	slong	nNumPnt = 0L;
	CB_FPE_SRCH_RSLT_PNT_INFO	*ptSrchRsltPntInfo = NULL;

	/* �����`�F�b�N */
	if (   ( NULL == ptCrossPnt  ) || ( NULL == pnNumCrossPnt  )
		|| ( NULL == ptTPntUpper ) || ( NULL == pnNumTPntUpper )
		|| ( NULL == ptTPntLower ) || ( NULL == pnNumTPntLower )
		|| ( NULL == ptTPntLeft  ) || ( NULL == pnNumTPntLeft  )
		|| ( NULL == ptTPntRight ) || ( NULL == pnNumTPntRight ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	ptSrchRsltPntInfo = &( m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltPntInfo[CB_PATTERN_POS_CENTER] );

	/* �\���_ */
	nNumPnt = ptSrchRsltPntInfo->nNumCrossPnt;
	*pnNumCrossPnt = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptCrossPnt[ni] = (ptSrchRsltPntInfo->tCrossPntPrecise)[ni];
	}

	/* �s���_(�㑤) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntUpper;
	*pnNumTPntUpper = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntUpper[ni] = (ptSrchRsltPntInfo->tTPntUpperPrecise)[ni];
	}

	/* �s���_(����) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntLower;
	*pnNumTPntLower = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntLower[ni] = (ptSrchRsltPntInfo->tTPntLowerPrecise)[ni];
	}

	/* �s���_(����) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntLeft;
	*pnNumTPntLeft = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntLeft[ni] = (ptSrchRsltPntInfo->tTPntLeftPrecise)[ni];
	}

	/* �s���_(�E��) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntRight;
	*pnNumTPntRight = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntRight[ni] = (ptSrchRsltPntInfo->tTPntRightPrecise)[ni];
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����_�ʒu(���E�ۂɑ΂���)�擾
 *
 * @param[out]	ptCenterPntL		:,�ۂ̒��_�ʒu(���p�^�[��)�\���̂ւ̃|�C���^(CB_ADD_PATTERN_PNT_POS_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumCenterPntL		:,�ۂ̒��_�ʒu(���p�^�[��)�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[out]	ptCenterPntR		:,�ۂ̒��_�ʒu(�E�p�^�[��)�\���̂ւ̃|�C���^(CB_ADD_PATTERN_PNT_POS_MAXNUM���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumCenterPntR		:,�ۂ̒��_�ʒu(�E�p�^�[��)�̐����i�[����̈�ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.17	K.Kato			�V�K�쐬
 *
 * @note		�f�o�b�O�p�̎擾�֐��̂��߁A�{�֐����ł̈���nCamDirection�͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
slong cb_img_getFtrPntPos4LineCircle( t_cb_img_CenterPos* ptCenterPntL, slong* pnNumCenterPntL, 						// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6710
									 t_cb_img_CenterPos* ptCenterPntR, slong* pnNumCenterPntR, slong nCamDirection )
{
	slong	ni = 0L;
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO	*ptSrchRsltPntInfo = NULL;

	/* �����`�F�b�N */
	if (   ( NULL == ptCenterPntL ) || ( NULL == pnNumCenterPntL )
		|| ( NULL == ptCenterPntR ) || ( NULL == pnNumCenterPntR ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �����̃p�^�[�� */
	/* �擪�|�C���^ */
	ptSrchRsltPntInfo = &( m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltAddPntInfo[CB_ADD_PATTERN_POS_LEFT] );
	*pnNumCenterPntL = ptSrchRsltPntInfo->nNumCenterPnt;
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumCenterPnt; ni++ )
	{
		ptCenterPntL[ni] = (ptSrchRsltPntInfo->tCenterPntPrecise)[ni];
	}

	/* �E���̃p�^�[�� */
	/* �擪�|�C���^ */
	ptSrchRsltPntInfo = &( m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltAddPntInfo[CB_ADD_PATTERN_POS_RIGHT] );
	*pnNumCenterPntR = ptSrchRsltPntInfo->nNumCenterPnt;
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumCenterPnt; ni++ )
	{
		ptCenterPntR[ni] = (ptSrchRsltPntInfo->tCenterPntPrecise)[ni];
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�O���b�h����̒��_�擾
 *
 * @param[out]	ptPeak				:,���_�\���̂ւ̃|�C���^(CB_FPE_GRID_NUM_WLD_Y*CB_FPE_GRID_NUM_WLD_X���̃e�[�u�����m�ۂ���Ă��邱��),-,[-],
 * @param[out]	pnNumGridVert		:,�O���b�h���̐�(��������)���i�[����̈�ւ̃|�C���^,-,[-],
 * @param[out]	pnNumGridHori		:,�O���b�h���̐�(��������)���i�[����̈�ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.17	K.Kato			�V�K�쐬
 *
 * @note		�f�o�b�O�p�̎擾�֐��̂��߁A�{�֐����ł̈���nCamDirection�͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
slong cb_img_getGridLinePeak( CB_IMG_POINT* ptPeak, slong* pnNumGridVert, slong* pnNumGridHori, slong nCamDirection )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6711
{
	slong	ni = 0L, 
			nj = 0L;
	slong	nOffset = 0L;
	slong	nNumGridVert = 0L, 
			nNumGridHori = 0L;
	CB_FPE_DBG_GRID_INFO	*ptGridInfo = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == ptPeak ) || ( NULL == pnNumGridVert ) || ( NULL == pnNumGridHori ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �O���b�h���L���`�F�b�N */
	if ( ( CB_TRUE != m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4Pattern ) || ( CB_TRUE != m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4AddPattern ) )
	{
		return ( CB_IMG_NG );
	}

	/* �擪�|�C���^ */
	ptGridInfo = &( m_FPE_tDbgInfo.tGridInfo[nCamDirection] );

	/* �O���b�h���̖{�� */
	nNumGridVert = CB_FPE_GRID_NUM_WLD_X;
	nNumGridHori = CB_FPE_GRID_NUM_WLD_Y;
	*pnNumGridVert = nNumGridVert;
	*pnNumGridHori = nNumGridHori;

	/* �O���b�h����̓_���i�[ */
	for ( ni = 0L; ni < nNumGridHori; ni++ )
	{
		for ( nj = 0L; nj < nNumGridVert; nj++ )
		{
			/* �I�t�Z�b�g */
			nOffset = ( ni * nNumGridVert ) + nj;

			/* �i�[ */
			*( ptPeak + nOffset ) = ( ptGridInfo->tPntImg )[ni][nj];
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�F�����s��Ԏ擾
 *
 * @param[out]	pnState				:,���s��Ԃ��i�[����̈�ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.17	K.Kato			�V�K�쐬
 *
 * @note		�f�o�b�O�p�̎擾�֐��̂��߁A�{�֐����ł̈���nCamDirection�͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
slong cb_img_getExecState( slong* pnState, slong nCamDirection )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6712
{
	/* �����`�F�b�N */
	if ( NULL == pnState )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ���g�p�ϐ���� ���[�j���O�΍� */
	CALIB_UNUSED_VARIABLE( nCamDirection );	/* �ϐ��g�p���ɂ͍폜���Ă������� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O1.3  R-53, ID-6713
	
	*pnState = CB_IMG_OK;

	return ( CB_IMG_OK );
}


/********************************************************************
 * �����֐��@��`
 ********************************************************************/

/******************************************************************************/
/**
 * @brief		�摜�������m��
 *
 * @param[out]	pnImg			:,���ID�ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.14	K.Kato			�V�K�쐬
 * @date		2015.08.26	S.Morita		�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�
 */
/******************************************************************************/
static slong cb_FPE_AllocImg( IMPLIB_IMGID* pnImg )
{
	slong	nRet = CB_IMG_NG;
	sint	ni = 0;

	/* �����`�F�b�N */
	if ( NULL == pnImg )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 0; ni < CB_FPE_IMG_Y_SIZE_NORMAL_MAXNUM; ni++ )
	{
		pnImg[ni] = implib_AllocImg( (IMPLIB_ImageFrameSize)CB_IMG_TYPE1_SIZE_720H_480W );
		nRet = implib_IP_ClearImg( pnImg[ni] );
	}
	for ( ni = CB_FPE_IMG_Y_SIZE_NORMAL_MAXNUM; ni < CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM; ni++ )
	{
		pnImg[ni] = implib_AllocImg( (IMPLIB_ImageFrameSize)CB_IMG_TYPE2_SIZE_360H_240V );
		nRet = implib_IP_ClearImg( pnImg[ni] );
	}
//	for ( ni = CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM; ni < CB_FPE_IMG_Y32_SIZE_NORMAL_MAXNUM; ni++ )
//	{
//		pnImg[ni] = implib_AllocImg32( (IMPLIB_ImageFrameSize)CB_IMG_TYPE1_SIZE_720H_480W );
//		nRet = implib_IP_ClearImg( pnImg[ni] );
//	}
	for ( ni = CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM; ni < CB_FPE_IMG_Y32_SIZE_ZOOMOUT2x2_MAXNUM; ni++ )
	{
		pnImg[ni] = implib_AllocImg32( (IMPLIB_ImageFrameSize)CB_IMG_TYPE2_SIZE_360H_240V );
		nRet = implib_IP_ClearImg( pnImg[ni] );
	}
	for ( ni = CB_FPE_IMG_Y32_SIZE_ZOOMOUT2x2_MAXNUM; ni < CB_FPE_IMG_SIDEMARKER_SIZE_CHG_MAXNUM; ni++ )
	{

		if((CB_FPE_INTEGRAL_IMG_NORMAL == ni) || (CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL == ni))
		{
			pnImg[ni] = implib_AllocImg32( (IMPLIB_ImageFrameSize)CB_IMG_TYPE1_SIZE_720H_480W );
		}
		else
		{
			pnImg[ni] = implib_AllocImg( (IMPLIB_ImageFrameSize)CB_IMG_TYPE1_SIZE_720H_480W );
		}

		nRet = implib_IP_ClearImg( pnImg[ni] );
	}

	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�J�����p�����[�^�̐ݒ�
 *
 * @param[in]	ptCamPrmSrc		:,�J�����p�����[�^(�\�[�X)�ւ̃|�C���^,-,[-],
 * @param[out]	ptCamPrmDst		:,�J�����p�����[�^(�f�X�e�B�l�[�V����)�ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.08	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetCameraParam( const CB_IMG_CAMERA_PRM* const ptCamPrmSrc, CB_FPE_CAM_PRM* ptCamPrmDst )
{
	slong						nCamID = 0L;
	CB_CG_EXTRINSIC_PRM			*ptExtrinsicPrmDst = NULL;
	CB_CG_INTRINSIC_PRM			*ptIntrinsicPrmDst = NULL;
	CB_CG_DIST_PRM				*ptDistPrmDst = NULL;
	CB_IMG_CAMERA_EXTRINSIC_PRM	*ptExtrinsicPrmSrc = NULL;
	CB_IMG_CAMERA_INTRINSIC_PRM	*ptIntrinsicPrmSrc = NULL;
	
	/* �����`�F�b�N */
	if ( ( NULL == ptCamPrmSrc ) || ( NULL == ptCamPrmDst ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	/* �O���p�����[�^ */
	ptExtrinsicPrmSrc = &( ptCamPrmSrc->tExtrinsicPrm );
	ptExtrinsicPrmDst = &( ptCamPrmDst->tExtrinsicPrm );
	ptExtrinsicPrmDst->dPitchDeg	= ptExtrinsicPrmSrc->dPitchDeg;
	ptExtrinsicPrmDst->dRollDeg		= ptExtrinsicPrmSrc->dRollDeg;
	ptExtrinsicPrmDst->dYawDeg		= ptExtrinsicPrmSrc->dYawDeg;
	ptExtrinsicPrmDst->dShiftX		= ptExtrinsicPrmSrc->dShiftX;
	ptExtrinsicPrmDst->dShiftY		= ptExtrinsicPrmSrc->dShiftY;
	ptExtrinsicPrmDst->dShiftZ		= ptExtrinsicPrmSrc->dShiftZ;
	/* �����p�����[�^ */
	ptIntrinsicPrmSrc = &( ptCamPrmSrc->tIntrinsicPrm );
	ptIntrinsicPrmDst = &( ptCamPrmDst->tIntrinsicPrm );
	ptIntrinsicPrmDst->nFlgMirrorImg	= (slong)ptIntrinsicPrmSrc->nFlagMirror;
	ptIntrinsicPrmDst->nXSizeImg		= ptIntrinsicPrmSrc->nXSizeImg;
	ptIntrinsicPrmDst->nYSizeImg		= ptIntrinsicPrmSrc->nYSizeImg;
	ptIntrinsicPrmDst->dUnitLenW		= ptIntrinsicPrmSrc->dXSizePixel;
	ptIntrinsicPrmDst->dUnitLenH		= ptIntrinsicPrmSrc->dYSizePixel;
	ptIntrinsicPrmDst->dCentPntW		= ptIntrinsicPrmSrc->dPosHori;
	ptIntrinsicPrmDst->dCentPntH		= ptIntrinsicPrmSrc->dPosVert;
	ptIntrinsicPrmDst->dFocalLen		= ptIntrinsicPrmSrc->dFocalLength;
	/* �c�݃p�����[�^ */
	ptDistPrmDst = &( ptCamPrmDst->tDistPrm );
	ptDistPrmDst->nHeightTblMaxNum		= ptIntrinsicPrmSrc->nNumImgHeightTbl;
	ptDistPrmDst->dMaxAngleDeg			= ptIntrinsicPrmSrc->dMaxAngle;
	ptDistPrmDst->pdHeightTbl			= ptIntrinsicPrmSrc->pdImgHeightTbl;
	ptDistPrmDst->nFlgDistortion		= (slong)ptIntrinsicPrmSrc->nFlagDistortion;

	/* �p�����[�^�o�^ */

	/* �擪�|�C���^ */
	ptExtrinsicPrmDst = &( ptCamPrmDst->tExtrinsicPrm );
	ptIntrinsicPrmDst = &( ptCamPrmDst->tIntrinsicPrm );
	ptDistPrmDst = &( ptCamPrmDst->tDistPrm );
	/* �o�^ */
	nCamID = cb_CG_SetCameraPrm( ptExtrinsicPrmDst, ptIntrinsicPrmDst, ptDistPrmDst );
	if ( 0L >= nCamID )
	{
		return ( CB_IMG_NG );
	}
	/* ID */
	ptCamPrmDst->nCamID = nCamID;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�J�����p�����[�^�̍폜
 *
 * @param[in]	ptCamPrm		:,�J�����p�����[�^(�\�[�X)�ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.06.25		K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_DeleteCameraParam( CB_FPE_CAM_PRM* ptCamPrm )
{
	slong						nRet = CB_IMG_NG;
	
	/* �����`�F�b�N */
	if ( NULL == ptCamPrm )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�폜 */
	nRet = cb_CG_DeleteCameraPrm( ptCamPrm->nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �����l�ݒ� */
	ptCamPrm->nCamID = -1L;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�J�����p�����[�^(���e�덷)�̐ݒ�
 *
 * @param[in]	ptCamPrmSrc			:,�J�����p�����[�^(�\�[�X)�ւ̃|�C���^,-,[-],
 * @param[out]	ptCamErrInfo		:,���e�덷���\����(�f�X�e�B�l�[�V����)�ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.04.15	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetCameraErrParam( const CB_IMG_CAMERA_PRM* const ptCamPrmSrc, CB_FPE_CAM_ERR_INFO* ptCamErrInfo )
{
	slong						ni = 0L;
	slong						nType = 0L;
	slong						nTblMaxNum = 0L;
	slong						nCnt = 0L, 
								nCntMaxNum = 0L;
	double_t					dCoeff = 0.0;
	double_t					dErrPitch = 0.0, 
								dErrRoll = 0.0, 
								dErrYaw = 0.0, 
								dErrOffsetWldX = 0.0, 
								dErrOffsetWldY = 0.0, 
								dErrOffsetWldZ = 0.0, 
								dErrCntPntHori = 0.0, 
								dErrCntPntVert = 0.0;
	CB_IMG_CAMERA_PRM_ERR_INFO	*ptErrInfo = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == ptCamPrmSrc ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	ptErrInfo = &( ptCamPrmSrc->tErrInfo );
	dErrPitch		= ptErrInfo->dPitchDeg;
	dErrRoll		= ptErrInfo->dRollDeg;
	dErrYaw			= ptErrInfo->dYawDeg;
	dErrOffsetWldX	= ptErrInfo->dShiftX;
	dErrOffsetWldY	= ptErrInfo->dShiftY;
	dErrOffsetWldZ	= ptErrInfo->dShiftZ;
	dErrCntPntHori	= ptErrInfo->dPosHori;
	dErrCntPntVert	= ptErrInfo->dPosVert;

	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;
	nType = 0L;

	/* pitch */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrPitch = dCoeff * dErrPitch;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* roll */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrRoll = dCoeff * dErrRoll;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* yaw */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrYaw = dCoeff * dErrYaw;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* Offset(X) */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrOffsetWldX = dCoeff * dErrOffsetWldX;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* Offset(Y) */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrOffsetWldY = dCoeff * dErrOffsetWldY;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* Offset(Z) */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrOffsetWldZ = dCoeff * dErrOffsetWldZ;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* �����ʒu(����) */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrCntPntHori = dCoeff * dErrCntPntHori;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	/* �����ʒu(����) */
	nType++;
	nCnt = 0L;
	nCntMaxNum = nTblMaxNum / ( (slong)powf( 2.0f, (float_t)nType ) );
	dCoeff = -1.0; 
	for ( ni = 0L; ni < nTblMaxNum; ni++ )
	{
		(ptCamErrInfo->tErrTbl)[ni].dErrCntPntVert = dCoeff * dErrCntPntVert;

		nCnt++;
		if ( nCntMaxNum <= nCnt )
		{
			nCnt = 0L;
			dCoeff = dCoeff * ( -1.0 );
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�T���̈�ݒ�(World)
 *
 * @param[in]	ptPatternInfo		:,�����p�^�[�����\���̂ւ̃|�C���^,-,[-],
 * @param[out]	ptSrchRgnInfo		:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.07	K.Kato			�V�K�쐬
 * @date		2014.12.01	S.Morita		�L�����u���p�����[�^���菈���̂��ߏC��(�L���X�g�C��)
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionWorld( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo )
{
	ulong								nj = 0UL;
	ulong								nk = 0UL;
	CB_FPE_SRCH_RGN_INFO_EACH_PATTERN	*ptSrchRgnInfo4Ptn = NULL;
	CB_IMG_PATTERN_CORNER_INFO			*ptCornerInfo = NULL;
	CB_IMG_PATTERN_FTR_PNT_INFO			*ptFtrPntInfo = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �R�[�i�[�|�C���g�̐����`�F�b�N */
	if ( 4UL != ptPatternInfo->tCornerInfo.nNumCenter )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}

	/* �擪�|�C���^ */
	ptCornerInfo = &( ptPatternInfo->tCornerInfo );
	ptFtrPntInfo = &( ptPatternInfo->tFtrPntInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfo4Pattern[CB_PATTERN_POS_CENTER]);
	/* �_���ƍ��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->nNumPnt = (slong)ptCornerInfo->nNumCenter;
	for ( nj = 0UL; nj < ptCornerInfo->nNumCenter; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWld[nj].dX = ( ptCornerInfo->tCenter )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dY = ( ptCornerInfo->tCenter )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dZ = ( ptCornerInfo->tCenter )[nj].dZ;
	}	/* for ( nj ) */

	/* �J�����p�����[�^����p�ɍ��W���i�[���� */
	ptSrchRgnInfo4Ptn->nNumCenterPnt = ptFtrPntInfo->nNumCenter;
	for ( nk = 0UL; nk < ptFtrPntInfo->nNumCenter; nk++ )
	{
		ptSrchRgnInfo4Ptn->tCenterPntWld[nk].dX = ( ptFtrPntInfo->tCenter )[nk].dX;
		ptSrchRgnInfo4Ptn->tCenterPntWld[nk].dY = ( ptFtrPntInfo->tCenter )[nk].dY;
		ptSrchRgnInfo4Ptn->tCenterPntWld[nk].dZ = ( ptFtrPntInfo->tCenter )[nk].dZ;
	}	/* for ( nk ) */

	/* �����p�^�[�����(�p�^�[���̌���) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagCenterPlacement;
	/* �����p�^�[�����(�p�^�[����Type) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeCenterPattern;
	/* �����p�^�[�����(�p�^�[�����o�A���S���Y��) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeCenterAlgorithm;
	/* �����p�^�[�����(�p�^�[�����ݏ�ԃt���O) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nDoubleType = ptPatternInfo->nDoubleType;

	/* �����p�^�[�����(���C�A�E�g���) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nlayoutType = ptPatternInfo->nlayoutType;
	/* �����p�^�[�����(�I�t�Z�b�g�^�C�v) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nCenterOffsetType = ptPatternInfo->nCenterOffsetType;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�ǉ��p�^�[���ɑ΂���T���̈�ݒ�(World)
 *
 * @param[in]	ptPatternInfo		:,�����p�^�[�����\���̂ւ̃|�C���^,-,[-],
 * @param[out]	ptSrchRgnInfo		:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.03	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionWorld4AddPattern( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo )
{
	ulong									nj = 0UL;
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo4Ptn = NULL;
	CB_IMG_PATTERN_CORNER_INFO				*ptCornerInfo = NULL;
#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN
	CB_IMG_PATTERN_FTR_PNT_INFO				*ptFtrPntInfo = NULL;				/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */

	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �R�[�i�[�|�C���g�̐����`�F�b�N */
	if (   ( 4UL != ptPatternInfo->tCornerInfo.nNumLeft )
		|| ( 4UL != ptPatternInfo->tCornerInfo.nNumRight ) )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN												/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
	/* ���ۃ}�[�J�[�̓����_�����`�F�b�N */
	if (   ( 2UL != ptPatternInfo->tFtrPntInfo.nNumLeft )
		|| ( 2UL != ptPatternInfo->tFtrPntInfo.nNumRight ) )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */											/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

	/* �擪�|�C���^ */
	ptCornerInfo = &( ptPatternInfo->tCornerInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);
	/* �_���ƍ��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->nNumPnt = (slong)ptCornerInfo->nNumLeft;
	for ( nj = 0UL; nj < ptCornerInfo->nNumLeft; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWld[nj].dX = ( ptCornerInfo->tLeft )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dY = ( ptCornerInfo->tLeft )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dZ = ( ptCornerInfo->tLeft )[nj].dZ;
	}	/* for ( nj ) */

	/* �擪�|�C���^ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);
	/* �_���ƍ��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->nNumPnt = (slong)ptCornerInfo->nNumRight;
	for ( nj = 0UL; nj < ptCornerInfo->nNumRight; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWld[nj].dX = ( ptCornerInfo->tRight )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dY = ( ptCornerInfo->tRight )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dZ = ( ptCornerInfo->tRight )[nj].dZ;
	}	/* for ( nj ) */
	
#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN												/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
	/* �擪�|�C���^ */
	ptFtrPntInfo = &( ptPatternInfo->tFtrPntInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);
	/* ���W�l�ݒ� */
	for ( nj = 0UL; nj < ptFtrPntInfo->nNumLeft; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dX = ( ptFtrPntInfo->tLeft )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dY = ( ptFtrPntInfo->tLeft )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dZ = ( ptFtrPntInfo->tLeft )[nj].dZ;
	}	/* for ( nj ) */

	/* �擪�|�C���^ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);
	/* ���W�l�ݒ� */
	for ( nj = 0UL; nj < ptFtrPntInfo->nNumRight; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dX = ( ptFtrPntInfo->tRight )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dY = ( ptFtrPntInfo->tRight )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dZ = ( ptFtrPntInfo->tRight )[nj].dZ;
	}	/* for ( nj ) */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */											/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

	/* �擪�|�C���^ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);
	/* �����p�^�[�����(�p�^�[���̌���) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagLeftPlacement;
	/* �����p�^�[�����(�p�^�[����Type) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeLeftPattern;
	/* �����p�^�[�����(�p�^�[�����o�A���S���Y��) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeLeftAlgorithm;

	/* �擪�|�C���^ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);
	/* �����p�^�[�����(�p�^�[���̌���) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagRightPlacement;
	/* �����p�^�[�����(�p�^�[����Type) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeRightPattern;
	/* �����p�^�[�����(�p�^�[�����o�A���S���Y��) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeRightAlgorithm;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�ǉ��p�^�[��(���E�s��)�ɑ΂���T���̈�ݒ�(World)
 *
 * @param[in]	ptPatternInfo		:,�����p�^�[�����\���̂ւ̃|�C���^,-,[-],
 * @param[out]	ptSrchRgnInfo		:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.11.12	F.Sano			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionWorld4AddChkboard( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, slong nCamDirection )
{
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo4Ptn = NULL;
	CB_IMG_PATTERN_ADD_INFO					*ptAddInfo = NULL;			/* �ǉ������W�擾�p */

	slong nSignHaarLikeFeature = 0L;
	slong nRet = 0L;


	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ���E�s���̒ǉ�����_�����`�F�b�N */
	if (   ( CB_IMG_ADD_PNT_MAXNUM_LEFT  != ptPatternInfo->tAddInfo.nNumLeft )
		|| ( CB_IMG_ADD_PNT_MAXNUM_RIGHT != ptPatternInfo->tAddInfo.nNumRight ) )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}

	/* �擪�|�C���^ */
	ptAddInfo = &( ptPatternInfo->tAddInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);

	/* ���E�s���i���j�\���_�̍��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_CC].dX;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_CC].dY;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_CC].dZ;

	/* ���E�s��(��)�̓_���ƍ��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->nNumPnt = 4L;


	/* �d�ݕt��Haar-like�����ʕ����̎擾 */
	nRet = cb_FPE_GetSignHaarLikeFeature( &( ptSrchRgnInfo->tSrchRgnInfoAddPattern[0].tPtnInfo ), nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		return ( nRet );
	}

	/* �摜��ŁA���������̏ꍇ */
	if( nSignHaarLikeFeature == CB_IMG_CHECKBOARD_SIGN_MINUS )
	{
		/* �s������̃R�[�i�[�_(�ǉ����̏㑤�|��) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UL].dZ;
		/* �\���_������T���_(�ǉ����̉����|����) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* �\���_�㑤��T���_(�ǉ����̏㑤�|����) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* �s���E���̃R�[�i�[�_(�ǉ����̉����|�E) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LR].dZ;
	}
	/* �摜��ŁA���������̏ꍇ */
	else
	{
		/* �s�������̃R�[�i�[�_(�ǉ����̉����|��) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LL].dZ;
		/* �\���_������T���_(�ǉ����̉����|����) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* �\���_�㑤��T���_(�ǉ����̏㑤�|����) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* �s���E��̃R�[�i�[�_(�ǉ����̏㑤�|�E) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UR].dZ;
	}


	/* �����p�^�[�����(�p�^�[���̌���) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagLeftPlacement;
	/* �����p�^�[�����(�p�^�[����Type) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeLeftPattern;
	/* �����p�^�[�����(�p�^�[�����o�A���S���Y��) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeLeftAlgorithm;

	/* ���E�s���i�E�j�\���_�̍��W�l�ݒ� */
	/* �擪�|�C���^ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);

	/* ���E�s���i�E�j�\���_�̍��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_CC].dX;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_CC].dY;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_CC].dZ;
	
	/* ���E�s��(�E)�̓_���ƍ��W�l�ݒ� */
	ptSrchRgnInfo4Ptn->nNumPnt = 4L;

	/* �摜��ŁA���������̏ꍇ */
	if( nSignHaarLikeFeature == CB_IMG_CHECKBOARD_SIGN_MINUS )
	{
		/* �s������̃R�[�i�[�_(�ǉ����̏㑤�|��) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UL].dZ;
		/* �\���_������T���_(�ǉ����̉����|����) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* �\���_�㑤��T���_(�ǉ����̏㑤�|����) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* �s���E���̃R�[�i�[�_(�ǉ����̉����|�E) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LR].dZ;
	}
	/* �摜��ŁA���������̏ꍇ */
	else
	{
		/* �s�������̃R�[�i�[�_(�ǉ����̉����|��) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LL].dZ;
		/* �\���_������T���_(�ǉ����̉����|����) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* �\���_�㑤��T���_(�ǉ����̏㑤�|����) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* �s���E��̃R�[�i�[�_(�ǉ����̏㑤�|�E) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UR].dZ;
	}
	/* �����p�^�[�����(�p�^�[���̌���) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagRightPlacement;
	/* �����p�^�[�����(�p�^�[����Type) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeRightPattern;
	/* �����p�^�[�����(�p�^�[�����o�A���S���Y��) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeRightAlgorithm;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�T���̈�ݒ�(Image)
 *
 * @param[in,out]	ptSrchRgnInfo		:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptCamPrm			:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptCamErrInfo		:,���e�덷���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.02.07	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionImg( CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, const CB_FPE_CAM_PRM* const ptCamPrm, const CB_FPE_CAM_ERR_INFO* const ptCamErrInfo )
{
	slong								nRet = CB_IMG_NG;
	slong								nj = 0L, 
										nk = 0L, 
										nl = 0L;
	slong								nNumPtn = 0L;
	slong								nXmin = 0L, 
										nXmax = 0L, 
										nYmin = 0L, 
										nYmax = 0L;
	slong								nCamID = 0L;
	slong								nTblMaxNum = 0L;
	CB_CG_PNT_IMG						tTempSrchPntImg;
	CB_IMG_POINT						tSrchPntImg;
	CB_CG_EXTRINSIC_PRM					tExtrinsicPrm;
	CB_CG_INTRINSIC_PRM					tIntrinsicPrm;
	CB_CG_DIST_PRM						tDistPrm;
	CB_FPE_SRCH_RGN_INFO_EACH_PATTERN	*ptSrchRgnInfo4Ptn = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRgnInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nNumPtn = CB_PATTERN_POS_MAXNUM;
	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;

	/* �Ƃ肠�����p�����[�^�ݒ�(�X�V�֐������s���邽��) */
	nRet = cb_CG_SetCameraPrm( &(ptCamPrm->tExtrinsicPrm), &(ptCamPrm->tIntrinsicPrm), &(ptCamPrm->tDistPrm) );
	if ( 0L >= nRet )
	{
		return ( CB_IMG_NG );
	}
	nCamID = nRet;

	/* ������ */
	for ( nj = 0L; nj < nNumPtn; nj++ )	/* �s���p�^�[���� */
	{
		/* �擪�|�C���^ */
		ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfo4Pattern[nj]);
		ptSrchRgnInfo4Ptn->tPntImg[0L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[0L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nY = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nY = -1L;
	}

	for ( nl = 0L; nl < nTblMaxNum; nl++ )	/* ���e�덷�̃p�^�[�� */
	{
		/* �p�����[�^�R�s�[ */
		tExtrinsicPrm = ptCamPrm->tExtrinsicPrm;
		tIntrinsicPrm = ptCamPrm->tIntrinsicPrm;
		tDistPrm = ptCamPrm->tDistPrm;
		/* �덷���l�������� */
		tExtrinsicPrm.dPitchDeg = ( ptCamPrm->tExtrinsicPrm ).dPitchDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrPitch;
		tExtrinsicPrm.dRollDeg = ( ptCamPrm->tExtrinsicPrm ).dRollDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrRoll;
		tExtrinsicPrm.dYawDeg = ( ptCamPrm->tExtrinsicPrm ).dYawDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrYaw;
		tExtrinsicPrm.dShiftX = ( ptCamPrm->tExtrinsicPrm ).dShiftX + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldX;
		tExtrinsicPrm.dShiftY = ( ptCamPrm->tExtrinsicPrm ).dShiftY + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldY;
		tExtrinsicPrm.dShiftZ = ( ptCamPrm->tExtrinsicPrm ).dShiftZ + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldZ;
		tIntrinsicPrm.dCentPntW = ( ptCamPrm->tIntrinsicPrm ).dCentPntW + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntHori;
		tIntrinsicPrm.dCentPntH = ( ptCamPrm->tIntrinsicPrm ).dCentPntH + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntVert;
		/* �p�����[�^�X�V */
		nRet = cb_CG_UpdateCameraPrm( nCamID, &tExtrinsicPrm, &tIntrinsicPrm, &tDistPrm );
		if ( CB_CG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* �p�^�[���̃��[�v */
		for ( nj = 0L; nj < nNumPtn; nj++ )
		{
			/* �擪�|�C���^ */
			ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfo4Pattern[nj]);

			/* �����l */
			nXmin = ptSrchRgnInfo4Ptn->tPntImg[0L].nX;
			nYmin = ptSrchRgnInfo4Ptn->tPntImg[0L].nY;
			nXmax = ptSrchRgnInfo4Ptn->tPntImg[3L].nX;
			nYmax = ptSrchRgnInfo4Ptn->tPntImg[3L].nY;

			for ( nk = 0L; nk < ptSrchRgnInfo->tSrchRgnInfo4Pattern[nj].nNumPnt; nk++ )
			{
				/* World to Image */
				nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWld[nk]), &tTempSrchPntImg );
				if ( CB_CG_OK != nRet )
				{
					return ( CB_IMG_NG );
				}

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6714
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6715

				/* �ŏ��l�E�ő�l��r */
				if ( tSrchPntImg.nX < nXmin )
				{
					nXmin = tSrchPntImg.nX;
				}
				if ( tSrchPntImg.nY < nYmin )
				{
					nYmin = tSrchPntImg.nY;
				}
				if ( nXmax < tSrchPntImg.nX )
				{
					nXmax = tSrchPntImg.nX;
				}
				if ( nYmax < tSrchPntImg.nY )
				{
					nYmax = tSrchPntImg.nY;
				}
			}	/* for ( nk ) */

			/* �O�ڋ�`��T���̈�Ƃ��Đݒ� */
			ptSrchRgnInfo4Ptn->tPntImg[0L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[0L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nY = nYmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nY = nYmax;
		}	/* for ( nj ) */
	}	/* for ( nl ) */

	/* �p�����[�^�폜 */
	nRet = cb_CG_DeleteCameraPrm( nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�ǉ��p�^�[���ɑ΂���T���̈�ݒ�(Image)
 *
 * @param[in,out]	ptSrchRgnInfo		:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptCamPrm			:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptCamErrInfo		:,���e�덷���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.04	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionImg4AddPattern( CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, const CB_FPE_CAM_PRM* const ptCamPrm, const CB_FPE_CAM_ERR_INFO* const ptCamErrInfo )
{
	slong									nRet = CB_IMG_NG;
	slong									nj = 0L, 
											nk = 0L, 
											nl = 0L;
	slong									nNumPtn = 0L;
	slong									nXmin = 0L, 
											nXmax = 0L, 
											nYmin = 0L, 
											nYmax = 0L;
	slong									nCamID = 0L;
	slong									nTblMaxNum = 0L;
	CB_CG_PNT_IMG							tTempSrchPntImg;
	CB_IMG_POINT							tSrchPntImg;
	CB_CG_EXTRINSIC_PRM						tExtrinsicPrm;
	CB_CG_INTRINSIC_PRM						tIntrinsicPrm;
	CB_CG_DIST_PRM							tDistPrm;
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo4Ptn = NULL;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN
	slong									ni = 0L;							/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */

	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRgnInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nNumPtn = CB_ADD_PATTERN_POS_MAXNUM;
	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;

	/* �Ƃ肠�����p�����[�^�ݒ�(�X�V�֐������s���邽��) */
	nRet = cb_CG_SetCameraPrm( &(ptCamPrm->tExtrinsicPrm), &(ptCamPrm->tIntrinsicPrm), &(ptCamPrm->tDistPrm) );
	if ( 0L >= nRet )
	{
		return ( CB_IMG_NG );
	}
	nCamID = nRet;

	/* ������ */
	for ( nj = 0L; nj < nNumPtn; nj++ )
	{
		/* �擪�|�C���^ */
		ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);
		ptSrchRgnInfo4Ptn->tPntImg[0L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[0L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nY = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nY = -1L;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN					/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
		/* �㑤�̊� */
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nY = -1L;
		/* �����̊� */
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nY = -1L;

#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */				/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
	}

	for ( nl = 0L; nl < nTblMaxNum; nl++ )
	{
		/* �p�����[�^�R�s�[ */
		tExtrinsicPrm = ptCamPrm->tExtrinsicPrm;
		tIntrinsicPrm = ptCamPrm->tIntrinsicPrm;
		tDistPrm = ptCamPrm->tDistPrm;
		/* �덷���l�������� */
		tExtrinsicPrm.dPitchDeg = ( ptCamPrm->tExtrinsicPrm ).dPitchDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrPitch;
		tExtrinsicPrm.dRollDeg = ( ptCamPrm->tExtrinsicPrm ).dRollDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrRoll;
		tExtrinsicPrm.dYawDeg = ( ptCamPrm->tExtrinsicPrm ).dYawDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrYaw;
		tExtrinsicPrm.dShiftX = ( ptCamPrm->tExtrinsicPrm ).dShiftX + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldX;
		tExtrinsicPrm.dShiftY = ( ptCamPrm->tExtrinsicPrm ).dShiftY + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldY;
		tExtrinsicPrm.dShiftZ = ( ptCamPrm->tExtrinsicPrm ).dShiftZ + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldZ;
		tIntrinsicPrm.dCentPntW = ( ptCamPrm->tIntrinsicPrm ).dCentPntW + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntHori;
		tIntrinsicPrm.dCentPntH = ( ptCamPrm->tIntrinsicPrm ).dCentPntH + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntVert;
		/* �p�����[�^�X�V */
		nRet = cb_CG_UpdateCameraPrm( nCamID, &tExtrinsicPrm, &tIntrinsicPrm, &tDistPrm );
		if ( CB_CG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* �p�^�[���̃��[�v */
		for ( nj = 0L; nj < nNumPtn; nj++ )
		{
			/* �擪�|�C���^ */
			ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);

			/* �����l */
			nXmin = ptSrchRgnInfo4Ptn->tPntImg[0L].nX;
			nYmin = ptSrchRgnInfo4Ptn->tPntImg[0L].nY;
			nXmax = ptSrchRgnInfo4Ptn->tPntImg[3L].nX;
			nYmax = ptSrchRgnInfo4Ptn->tPntImg[3L].nY;

			for ( nk = 0L; nk < ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj].nNumPnt; nk++ )
			{
				/* World to Image */
				nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWld[nk]), &tTempSrchPntImg );
				if ( CB_CG_OK != nRet )
				{
					return ( CB_IMG_NG );
				}

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6716
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6717

				/* �ŏ��l�E�ő�l��r */
				if ( tSrchPntImg.nX < nXmin )
				{
					nXmin = tSrchPntImg.nX;
				}
				if ( tSrchPntImg.nY < nYmin )
				{
					nYmin = tSrchPntImg.nY;
				}
				if ( nXmax < tSrchPntImg.nX )
				{
					nXmax = tSrchPntImg.nX;
				}
				if ( nYmax < tSrchPntImg.nY )
				{
					nYmax = tSrchPntImg.nY;
				}
			}	/* for ( nk ) */

			/* �O�ڋ�`��T���̈�Ƃ��Đݒ� */
			ptSrchRgnInfo4Ptn->tPntImg[0L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[0L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nY = nYmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nY = nYmax;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN					/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
			for( ni = 0L; ni < CB_ADD_PATTERN_PNT_POS_MAXNUM; ni++ )
			{
				/* World to Image */
				nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWldCircle[ni]), &tTempSrchPntImg );
				if ( CB_CG_OK != nRet )
				{
					return ( CB_IMG_NG );
				}

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6718
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6719

				/* ���ےT���̈�Ƃ��čŏ��l(�n�_)��ݒ� */
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nX > tSrchPntImg.nX )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nX = tSrchPntImg.nX;
				}
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nY > tSrchPntImg.nY )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nY = tSrchPntImg.nY;
				}
				/* ���ےT���̈�Ƃ��čő�l(�I�_)��ݒ� */
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nX < tSrchPntImg.nX )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nX = tSrchPntImg.nX;
				}
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nY < tSrchPntImg.nY )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nY = tSrchPntImg.nY;
				}

#ifdef CB_FPE_CSW_DEBUG_ON
				/* �f�o�b�O�`��p */
				ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleErrPntTbl[nl].nX = tSrchPntImg.nX;
				ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleErrPntTbl[nl].nY = tSrchPntImg.nY;
#endif /* CB_FPE_CSW_DEBUG_ON */
			}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */				/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
		}	/* for ( nj ) */
	}	/* for ( nl ) */

	/* �p�����[�^�폜 */
	nRet = cb_CG_DeleteCameraPrm( nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�ǉ��p�^�[��(���E�s��)�ɑ΂���T���̈�ݒ�(Image)
 *
 * @param[in,out]	ptSrchRgnInfo		:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptCamPrm			:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptCamErrInfo		:,���e�덷���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.12	F.Sano			�V�K�쐬
 * @date			2015.08.26	S.Morita		�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionImg4AddChkboard( CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, const CB_FPE_CAM_PRM* const ptCamPrm, const CB_FPE_CAM_ERR_INFO* const ptCamErrInfo )
{
	slong									nRet = CB_IMG_NG;
	slong									nj = 0L, 
											nk = 0L, 
											nl = 0L;
	slong									nNumPtn = 0L;
	slong									nXmin = 0L, 
											nXmax = 0L, 
											nYmin = 0L, 
											nYmax = 0L;
	slong									nCamID = 0L;
	slong									nTblMaxNum = 0L;
	CB_CG_PNT_IMG							tTempSrchPntImg;
	CB_IMG_POINT							tSrchPntImg;
	CB_CG_EXTRINSIC_PRM						tExtrinsicPrm;
	CB_CG_INTRINSIC_PRM						tIntrinsicPrm;
	CB_CG_DIST_PRM							tDistPrm;
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo4Ptn = NULL;


	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRgnInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nNumPtn = CB_ADD_PATTERN_POS_MAXNUM;
	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;

	/* �Ƃ肠�����p�����[�^�ݒ�(�X�V�֐������s���邽��) */
	nRet = cb_CG_SetCameraPrm( &(ptCamPrm->tExtrinsicPrm), &(ptCamPrm->tIntrinsicPrm), &(ptCamPrm->tDistPrm) );
	if ( 0L >= nRet )
	{
		return ( CB_IMG_NG );
	}
	nCamID = nRet;

	/* ������ */
	for ( nj = 0L; nj < nNumPtn; nj++ )
	{
		/* �擪�|�C���^ */
		ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);
		ptSrchRgnInfo4Ptn->tPntImg[0L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[0L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nY = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nY = -1L;

		/* �\���_ */
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEX = -1L;
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEY = -1L;

	}

	for ( nl = 0L; nl < nTblMaxNum; nl++ )
	{
		/* �p�����[�^�R�s�[ */
		tExtrinsicPrm = ptCamPrm->tExtrinsicPrm;
		tIntrinsicPrm = ptCamPrm->tIntrinsicPrm;
		tDistPrm = ptCamPrm->tDistPrm;
		/* �덷���l�������� */
		tExtrinsicPrm.dPitchDeg = ( ptCamPrm->tExtrinsicPrm ).dPitchDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrPitch;
		tExtrinsicPrm.dRollDeg = ( ptCamPrm->tExtrinsicPrm ).dRollDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrRoll;
		tExtrinsicPrm.dYawDeg = ( ptCamPrm->tExtrinsicPrm ).dYawDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrYaw;
		tExtrinsicPrm.dShiftX = ( ptCamPrm->tExtrinsicPrm ).dShiftX + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldX;
		tExtrinsicPrm.dShiftY = ( ptCamPrm->tExtrinsicPrm ).dShiftY + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldY;
		tExtrinsicPrm.dShiftZ = ( ptCamPrm->tExtrinsicPrm ).dShiftZ + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldZ;
		tIntrinsicPrm.dCentPntW = ( ptCamPrm->tIntrinsicPrm ).dCentPntW + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntHori;
		tIntrinsicPrm.dCentPntH = ( ptCamPrm->tIntrinsicPrm ).dCentPntH + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntVert;
		/* �p�����[�^�X�V */
		nRet = cb_CG_UpdateCameraPrm( nCamID, &tExtrinsicPrm, &tIntrinsicPrm, &tDistPrm );
		if ( CB_CG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* �p�^�[���̃��[�v */
		for ( nj = 0L; nj < nNumPtn; nj++ )
		{
			/* �擪�|�C���^ */
			ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);

			/* �����l */
			nXmin = ptSrchRgnInfo4Ptn->tPntImg[0L].nX;
			nYmin = ptSrchRgnInfo4Ptn->tPntImg[0L].nY;
			nXmax = ptSrchRgnInfo4Ptn->tPntImg[3L].nX;
			nYmax = ptSrchRgnInfo4Ptn->tPntImg[3L].nY;

			for ( nk = 0L; nk < ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj].nNumPnt; nk++ )
			{
				/* World to Image */
				nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWld[nk]), &tTempSrchPntImg );
				if ( CB_CG_OK != nRet )
				{
					return ( CB_IMG_NG );
				}

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6716
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6717

				/* �ŏ��l�E�ő�l��r */
				if ( tSrchPntImg.nX < nXmin )
				{
					nXmin = tSrchPntImg.nX;
				}
				if ( tSrchPntImg.nY < nYmin )
				{
					nYmin = tSrchPntImg.nY;
				}
				if ( nXmax < tSrchPntImg.nX )
				{
					nXmax = tSrchPntImg.nX;
				}
				if ( nYmax < tSrchPntImg.nY )
				{
					nYmax = tSrchPntImg.nY;
				}
			}	/* for ( nk ) */

			/* �O�ڋ�`��T���̈�Ƃ��Đݒ� */
			ptSrchRgnInfo4Ptn->tPntImg[0L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[0L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nY = nYmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nY = nYmax;

			/* ���S�_ */
			/* World to Image */
			nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWldSideChkCenter), &tTempSrchPntImg );
			if ( CB_CG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}

			tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6718
			tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6719

			/* ���E�s�����S�_�T���̈�Ƃ��čŏ��l(�n�_)��ݒ� */
			if( ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSX > tSrchPntImg.nX )
			{
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSX = tSrchPntImg.nX;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterZoomOut.tAddChkSrchRgn.nSX = tSrchPntImg.nX / 2L;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterNormalSize.tAddChkSrchRgn.nSX = tSrchPntImg.nX;
			}
			if( ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSY > tSrchPntImg.nY )
			{
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSY = tSrchPntImg.nY;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterZoomOut.tAddChkSrchRgn.nSY = tSrchPntImg.nY / 2L;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterNormalSize.tAddChkSrchRgn.nSY = tSrchPntImg.nY;
			}
			/* ���E�s�����S�_�T���̈�Ƃ��čő�l(�I�_)��ݒ� */
			if( ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEX < tSrchPntImg.nX )
			{
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEX = tSrchPntImg.nX;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterZoomOut.tAddChkSrchRgn.nEX = tSrchPntImg.nX / 2L;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterNormalSize.tAddChkSrchRgn.nEX = tSrchPntImg.nX;
			}
			if( ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEY < tSrchPntImg.nY )
			{
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEY = tSrchPntImg.nY;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterZoomOut.tAddChkSrchRgn.nEY = tSrchPntImg.nY / 2L;
				ptSrchRgnInfo4Ptn->tPntImgSideChkCenterNormalSize.tAddChkSrchRgn.nEY = tSrchPntImg.nY;
			}

#ifdef CB_FPE_CSW_DEBUG_ON
			/* �f�o�b�O�`��p */
			ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkErrPntTbl[nl].nX = tSrchPntImg.nX;
			ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkErrPntTbl[nl].nY = tSrchPntImg.nY;
#endif /* CB_FPE_CSW_DEBUG_ON */
		}	/* for ( nj ) */
	}	/* for ( nl ) */

	/* �p�����[�^�폜 */
	nRet = cb_CG_DeleteCameraPrm( nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			�T���̈�ݒ�
 *
 * @param[in,out]	ptSrchRgnInfo		:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.02.16	K.Kato			�V�K�쐬
 * @date			2017.10.12	A.Honda			�T���͈͂̊ۂߍ��ݏ����ǉ�
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegion( CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* ptSrchRgnInfo )
{
	slong			nXSize = 0L, 
					nYSize = 0L;
	slong			nXSizeZoomOut = 0L, 
					nYSizeZoomOut = 0L;
	slong			nXSizeImg = 0L, 
					nYSizeImg = 0L, 
					nHalfXSizeImg = 0L, 
					nHalfYSizeImg = 0L;
	ulong			nFlagChkRgn = CB_TRUE;
	slong			nRet = CB_IMG_NG;
	CB_RECT_RGN		*ptSrchRgn = NULL, 
					*ptSrchRgnZoomOut = NULL;
	CB_RECT_RGN		*ptSrchRgnHLsrc = NULL, 
					*ptSrchRgnZoomOutHLChkEdge = NULL, 
					*ptSrchRgnZoomOutHLVertEdge = NULL, 
					*ptSrchRgnZoomOutHLHoriEdge = NULL, 
					*ptSrchRgnZoomOutHLsrc = NULL;
	CB_IMG_POINT	*ptSrchRgnSrc = NULL;
	/* �p�����[�^ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	slong	nprm_xOffsetHL = 0L, 
			nprm_yOffsetHL = 0L;
	slong	nprm_xsizeBlockHLChkEdge = 0L, 
			nprm_ysizeBlockHLChkEdge = 0L;
	slong	nprm_xsizeBlockHLVertEdge = 0L, 
			nprm_ysizeBlockHLVertEdge = 0L;
	slong	nprm_xsizeBlockHLHoriEdge = 0L, 
			nprm_ysizeBlockHLHoriEdge = 0L;
	slong	nprm_marginX4SrchRgn = 0L, 
			nprm_marginY4SrchRgn = 0L;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_xOffsetHL = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_yOffsetHL = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_xsizeBlockHLChkEdge = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockHLChkEdge = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeBlockHLVertEdge = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockHLVertEdge = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE;
	nprm_xsizeBlockHLHoriEdge = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockHLHoriEdge = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_YSIZE;
	nprm_marginX4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CHKBOARD;
	nprm_marginY4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CHKBOARD;

	/* �摜�T�C�Y */
	nXSizeImg = CB_GET_IMG_XSIZE;
	nYSizeImg = CB_GET_IMG_YSIZE;
	nHalfXSizeImg = CB_GET_IMG_XSIZE_HALF;
	nHalfYSizeImg = CB_GET_IMG_YSIZE_HALF;

	/* �擪�|�C���^ */
	ptSrchRgnSrc				= &(ptSrchRgnInfo->tPntImg[0L]);
	ptSrchRgn					= &(ptSrchRgnInfo->tRgnImg);
	ptSrchRgnZoomOut			= &(ptSrchRgnInfo->tRgnImgZoomOut);
	ptSrchRgnHLsrc				= &(ptSrchRgnInfo->tRgnImgHLsrc);
	ptSrchRgnZoomOutHLChkEdge	= &(ptSrchRgnInfo->tRgnImgZoomOutHLChkEdge);
	ptSrchRgnZoomOutHLVertEdge	= &(ptSrchRgnInfo->tRgnImgZoomOutHLVertEdge);
	ptSrchRgnZoomOutHLHoriEdge	= &(ptSrchRgnInfo->tRgnImgZoomOutHLHoriEdge);
	ptSrchRgnZoomOutHLsrc		= &(ptSrchRgnInfo->tRgnImgZoomOutHLsrc);

	/* �ʏ� */
	ptSrchRgn = &(ptSrchRgnInfo->tRgnImg);
	ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
	ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
	ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
	ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* �k����T�C�Y */
	nXSizeZoomOut = ( ( ptSrchRgn->nEX - ptSrchRgn->nSX ) + 1L ) / nprm_xMagZoomOut;
	nYSizeZoomOut = ( ( ptSrchRgn->nEY - ptSrchRgn->nSY ) + 1L ) / nprm_yMagZoomOut;

	/* �k���� */
	ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
	ptSrchRgnZoomOut->nEX = ( ptSrchRgnZoomOut->nSX + nXSizeZoomOut ) - 1L;
	ptSrchRgnZoomOut->nEY = ( ptSrchRgnZoomOut->nSY + nYSizeZoomOut ) - 1L;
	/* �ʏ�łŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	/* Haar-like�l�� */
	/* �s��(�k����) */
	ptSrchRgnZoomOutHLChkEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLChkEdge;
	ptSrchRgnZoomOutHLChkEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLChkEdge;
	ptSrchRgnZoomOutHLChkEdge->nEX = ptSrchRgnZoomOut->nEX + nprm_xsizeBlockHLChkEdge;
	ptSrchRgnZoomOutHLChkEdge->nEY = ptSrchRgnZoomOut->nEY + nprm_ysizeBlockHLChkEdge;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLChkEdge->nSX, ptSrchRgnZoomOutHLChkEdge->nSY, ptSrchRgnZoomOutHLChkEdge->nEX, ptSrchRgnZoomOutHLChkEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLChkEdge, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* �c�G�b�W(�k����) */
	ptSrchRgnZoomOutHLVertEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLVertEdge;
	ptSrchRgnZoomOutHLVertEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLVertEdge;
	ptSrchRgnZoomOutHLVertEdge->nEX = ptSrchRgnZoomOut->nEX + nprm_xsizeBlockHLVertEdge;
	ptSrchRgnZoomOutHLVertEdge->nEY = ptSrchRgnZoomOut->nEY;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLVertEdge->nSX, ptSrchRgnZoomOutHLVertEdge->nSY, ptSrchRgnZoomOutHLVertEdge->nEX, ptSrchRgnZoomOutHLVertEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLVertEdge, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* ���G�b�W(�k����) */
	ptSrchRgnZoomOutHLHoriEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLHoriEdge;
	ptSrchRgnZoomOutHLHoriEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLHoriEdge;
	ptSrchRgnZoomOutHLHoriEdge->nEX = ptSrchRgnZoomOut->nEX;
	ptSrchRgnZoomOutHLHoriEdge->nEY = ptSrchRgnZoomOut->nEY + nprm_ysizeBlockHLHoriEdge;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLHoriEdge->nSX, ptSrchRgnZoomOutHLHoriEdge->nSY, ptSrchRgnZoomOutHLHoriEdge->nEX, ptSrchRgnZoomOutHLHoriEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLHoriEdge, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* �S�̂��܂ޗ̈�(�k����) */
	ptSrchRgnZoomOutHLsrc->nSX = ( ptSrchRgnZoomOutHLChkEdge->nSX < ptSrchRgnZoomOutHLVertEdge->nSX ) ? ptSrchRgnZoomOutHLChkEdge->nSX : ptSrchRgnZoomOutHLVertEdge->nSX;
	ptSrchRgnZoomOutHLsrc->nSX = ( ptSrchRgnZoomOutHLHoriEdge->nSX < ptSrchRgnZoomOutHLsrc->nSX ) ? ptSrchRgnZoomOutHLHoriEdge->nSX : ptSrchRgnZoomOutHLsrc->nSX;
	ptSrchRgnZoomOutHLsrc->nSY = ( ptSrchRgnZoomOutHLChkEdge->nSY < ptSrchRgnZoomOutHLVertEdge->nSY ) ? ptSrchRgnZoomOutHLChkEdge->nSY : ptSrchRgnZoomOutHLVertEdge->nSY;
	ptSrchRgnZoomOutHLsrc->nSY = ( ptSrchRgnZoomOutHLHoriEdge->nSY < ptSrchRgnZoomOutHLsrc->nSY ) ? ptSrchRgnZoomOutHLHoriEdge->nSY : ptSrchRgnZoomOutHLsrc->nSY;
	ptSrchRgnZoomOutHLsrc->nEX = ( ptSrchRgnZoomOutHLChkEdge->nEX > ptSrchRgnZoomOutHLVertEdge->nEX ) ? ptSrchRgnZoomOutHLChkEdge->nEX : ptSrchRgnZoomOutHLVertEdge->nEX;
	ptSrchRgnZoomOutHLsrc->nEX = ( ptSrchRgnZoomOutHLHoriEdge->nEX > ptSrchRgnZoomOutHLsrc->nEX ) ? ptSrchRgnZoomOutHLHoriEdge->nEX : ptSrchRgnZoomOutHLsrc->nEX;
	ptSrchRgnZoomOutHLsrc->nEY = ( ptSrchRgnZoomOutHLChkEdge->nEY > ptSrchRgnZoomOutHLVertEdge->nEY ) ? ptSrchRgnZoomOutHLChkEdge->nEY : ptSrchRgnZoomOutHLVertEdge->nEY;
	ptSrchRgnZoomOutHLsrc->nEY = ( ptSrchRgnZoomOutHLHoriEdge->nEY > ptSrchRgnZoomOutHLsrc->nEY ) ? ptSrchRgnZoomOutHLHoriEdge->nEY : ptSrchRgnZoomOutHLsrc->nEY;
	/* Haar-like(�l��)�A���G�b�W�A�c�G�b�W�ŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	/* �S�̂��܂ޗ̈�(�ʏ�) */
	nXSize = ( ( ptSrchRgnZoomOutHLsrc->nEX - ptSrchRgnZoomOutHLsrc->nSX ) + 1L ) * nprm_xMagZoomOut;
	nYSize = ( ( ptSrchRgnZoomOutHLsrc->nEY - ptSrchRgnZoomOutHLsrc->nSY ) + 1L ) * nprm_yMagZoomOut;
	ptSrchRgnHLsrc->nSX = ptSrchRgnZoomOutHLsrc->nSX * nprm_xMagZoomOut;
	ptSrchRgnHLsrc->nSY = ptSrchRgnZoomOutHLsrc->nSY * nprm_yMagZoomOut;
	ptSrchRgnHLsrc->nEX = ( ptSrchRgnHLsrc->nSX + nXSize ) - 1L;
	ptSrchRgnHLsrc->nEY = ( ptSrchRgnHLsrc->nSY + nYSize ) - 1L;
	/* Haar-like(�l��)�A���G�b�W�A�c�G�b�W�ŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�ǉ��p�^�[���ɑ΂���T���̈�ݒ�
 *
 * @param[in,out]	ptSrchRgnInfo		:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.04	K.Kato			�V�K�쐬
 * @date			2017.10.12	A.Honda			�T���͈͂̊ۂߍ��ݏ����ǉ�
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegion4AddPattern( CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )
{
	slong			nXSizeImg = 0L, 
					nYSizeImg = 0L, 
					nHalfXSizeImg = 0L, 
					nHalfYSizeImg = 0L;
	ulong			nFlagChkRgn = CB_TRUE;
	slong			nRet = CB_IMG_NG;
	CB_RECT_RGN		*ptSrchRgn = NULL, 
					*ptSrchRgnRRF = NULL;
	CB_RECT_RGN		*ptSrchRgnZoomOut = NULL, 
					*ptSrchRgnRRFZoomOut = NULL;
	CB_IMG_POINT	*ptSrchRgnSrc = NULL;
	/* �p�����[�^ */
	slong	nprm_lengthReach = 10L;
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	slong	nprm_marginX4SrchRgn = 0L, 
			nprm_marginY4SrchRgn = 0L;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_lengthReach = CB_FPE_PRM_RRF_LENGTH_REACH;
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_marginX4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CIRCLE_LINE;
	nprm_marginY4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CIRCLE_LINE;

	/* �摜�T�C�Y */
	nXSizeImg = CB_GET_IMG_XSIZE;
	nYSizeImg = CB_GET_IMG_YSIZE;
	nHalfXSizeImg = CB_GET_IMG_XSIZE_HALF;
	nHalfYSizeImg = CB_GET_IMG_YSIZE_HALF;

	/* Left */
	/* �擪�|�C���^ */
	ptSrchRgnSrc = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tPntImg[0L]);

	/* �ʏ� */
	ptSrchRgn = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImg);
	ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
	ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
	ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
	ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* �ʏ�(�k��) */
	ptSrchRgnZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImgZoomOut);
	ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
	ptSrchRgnZoomOut->nEX = ptSrchRgn->nEX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nEY = ptSrchRgn->nEY / nprm_yMagZoomOut;
	/* �ʏ�łŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	/* RRF�p */
	ptSrchRgnRRF = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImgRRF);
	ptSrchRgnRRF->nSX = ptSrchRgn->nSX - nprm_lengthReach;
	ptSrchRgnRRF->nSY = ptSrchRgn->nSY - nprm_lengthReach;
	ptSrchRgnRRF->nEX = ptSrchRgn->nEX + nprm_lengthReach;
	ptSrchRgnRRF->nEY = ptSrchRgn->nEY + nprm_lengthReach;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnRRF->nSX, ptSrchRgnRRF->nSY, ptSrchRgnRRF->nEX, ptSrchRgnRRF->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgnRRF, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* RRF�p(�k��) */
	ptSrchRgnRRFZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImgRRFZoomOut);
	ptSrchRgnRRFZoomOut->nSX = ptSrchRgnRRF->nSX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nSY = ptSrchRgnRRF->nSY / nprm_yMagZoomOut;
	ptSrchRgnRRFZoomOut->nEX = ptSrchRgnRRF->nEX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nEY = ptSrchRgnRRF->nEY / nprm_yMagZoomOut;
	/* RRF(�ʏ��)�ŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	/* Right */
	/* �擪�|�C���^ */
	ptSrchRgnSrc = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tPntImg[0L]);

	/* �ʏ� */
	ptSrchRgn = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImg);
	ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
	ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
	ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
	ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* �ʏ�(�k��) */
	ptSrchRgnZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImgZoomOut);
	ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
	ptSrchRgnZoomOut->nEX = ptSrchRgn->nEX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nEY = ptSrchRgn->nEY / nprm_yMagZoomOut;
	/* �ʏ�łŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	/* RRF�p */
	ptSrchRgnRRF = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImgRRF);
	ptSrchRgnRRF->nSX = ptSrchRgn->nSX - nprm_lengthReach;
	ptSrchRgnRRF->nSY = ptSrchRgn->nSY - nprm_lengthReach;
	ptSrchRgnRRF->nEX = ptSrchRgn->nEX + nprm_lengthReach;
	ptSrchRgnRRF->nEY = ptSrchRgn->nEY + nprm_lengthReach;
	/* �̈�`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnRRF->nSX, ptSrchRgnRRF->nSY, ptSrchRgnRRF->nEX, ptSrchRgnRRF->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* �`�F�b�NNG���A�̈�Đݒ� */
		nRet = cb_FPE_ResetRegion( ptSrchRgnRRF, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* RRF�p(�k��) */
	ptSrchRgnRRFZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImgRRFZoomOut);
	ptSrchRgnRRFZoomOut->nSX = ptSrchRgnRRF->nSX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nSY = ptSrchRgnRRF->nSY / nprm_yMagZoomOut;
	ptSrchRgnRRFZoomOut->nEX = ptSrchRgnRRF->nEX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nEY = ptSrchRgnRRF->nEY / nprm_yMagZoomOut;
	/* RRF(�ʏ��)�ŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�ǉ��p�^�[��(���E�s��)�ɑ΂���T���̈�ݒ�
 *
 * @param[in,out]	ptSrchRgnInfo		:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.12	F.Sanoo			�V�K�쐬
 * @date			2015.08.26	S.Morita		�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�
 * @date			2017.10.12	A.Honda			�T���͈͂̊ۂߍ��ݏ����ǉ�
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegion4AddChkboard( CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )
{
	slong			ni = 0L;
	slong			nXSize = 0L, 
					nYSize = 0L;
	slong			nXSizeZoomOut = 0L, 
					nYSizeZoomOut = 0L;
	slong			nXSizeImg = 0L, 
					nYSizeImg = 0L, 
					nHalfXSizeImg = 0L, 
					nHalfYSizeImg = 0L;
	ulong			nFlagChkRgn = CB_TRUE;
	slong			nRet = CB_IMG_NG;
	CB_RECT_RGN		*ptSrchRgn = NULL, 
					*ptSrchRgnZoomOut = NULL;
	CB_RECT_RGN		*ptSrchRgnNormalSize = NULL; 
	CB_RECT_RGN		*ptSrchRgnHLsrc = NULL, 
					*ptSrchRgnZoomOutHLChkEdge = NULL, 
					*ptSrchRgnZoomOutHLsrc = NULL;
	CB_RECT_RGN		*ptSrchRgnNormalHLsrc = NULL;
	CB_IMG_POINT	*ptSrchRgnSrc = NULL;
	/* �p�����[�^ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	slong	nprm_xsizeBlockHLChkEdge = 0L, 
			nprm_ysizeBlockHLChkEdge = 0L;
	slong	nprm_marginX4SrchRgn = 0L, 
			nprm_marginY4SrchRgn = 0L;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_xsizeBlockHLChkEdge = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockHLChkEdge = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_marginX4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_ADDCHKBOARD;
	nprm_marginY4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_ADDCHKBOARD;

	/* �摜�T�C�Y */
	nXSizeImg = CB_GET_IMG_XSIZE;
	nYSizeImg = CB_GET_IMG_YSIZE;
	nHalfXSizeImg = CB_GET_IMG_XSIZE_HALF;
	nHalfYSizeImg = CB_GET_IMG_YSIZE_HALF;

	/* Left��Right�̓��[�v�ŉ� */
	for( ni = 0L ; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ ) {

		/* �擪�|�C���^ */
		ptSrchRgnSrc				= &(ptSrchRgnInfo[ni].tPntImg[0L]);
		ptSrchRgn					= &(ptSrchRgnInfo[ni].tRgnImg);
		ptSrchRgnZoomOut			= &(ptSrchRgnInfo[ni].tRgnImgZoomOut);
		ptSrchRgnNormalSize			= &(ptSrchRgnInfo[ni].tRgnImgNormalSize);
		ptSrchRgnHLsrc				= &(ptSrchRgnInfo[ni].tRgnImgHLsrc);
		ptSrchRgnNormalHLsrc		= &(ptSrchRgnInfo[ni].tRgnImgNormalHLsrc);
		ptSrchRgnZoomOutHLChkEdge	= &(ptSrchRgnInfo[ni].tRgnImgZoomOutHLChkEdge);
		ptSrchRgnZoomOutHLsrc		= &(ptSrchRgnInfo[ni].tRgnImgZoomOutHLsrc);

		/* �ʏ� */
		ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
		ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
		ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
		ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
		/* �̈�`�F�b�N */
		nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
		if ( CB_TRUE != nFlagChkRgn )
		{
			/* �`�F�b�NNG���A�̈�Đݒ� */
			nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
		}

		/* �T�C�h�}�[�J�[�����_���o�p�T���͈� */
		/* ptSrchRgn �Ɠ���̒l��ݒ肷�� */
		ptSrchRgnNormalSize->nSX = ptSrchRgn->nSX;
		ptSrchRgnNormalSize->nSY = ptSrchRgn->nSY;
		ptSrchRgnNormalSize->nEX = ptSrchRgn->nEX;
		ptSrchRgnNormalSize->nEY = ptSrchRgn->nEY;

		/* �k����T�C�Y */
		nXSizeZoomOut = ( ( ptSrchRgn->nEX - ptSrchRgn->nSX ) + 1L ) / nprm_xMagZoomOut;
		nYSizeZoomOut = ( ( ptSrchRgn->nEY - ptSrchRgn->nSY ) + 1L ) / nprm_yMagZoomOut;

		/* �k���� */
		ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
		ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
		ptSrchRgnZoomOut->nEX = ( ptSrchRgnZoomOut->nSX + nXSizeZoomOut ) - 1L;
		ptSrchRgnZoomOut->nEY = ( ptSrchRgnZoomOut->nSY + nYSizeZoomOut ) - 1L;
		/* �ʏ�łŗ̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

		/* Haar-like�l�� */
		/* �s��(�k����) */
		ptSrchRgnZoomOutHLChkEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLChkEdge;
		ptSrchRgnZoomOutHLChkEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLChkEdge;
		ptSrchRgnZoomOutHLChkEdge->nEX = ptSrchRgnZoomOut->nEX + nprm_xsizeBlockHLChkEdge;
		ptSrchRgnZoomOutHLChkEdge->nEY = ptSrchRgnZoomOut->nEY + nprm_ysizeBlockHLChkEdge;
		/* �̈�`�F�b�N */
		nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLChkEdge->nSX, ptSrchRgnZoomOutHLChkEdge->nSY, ptSrchRgnZoomOutHLChkEdge->nEX, ptSrchRgnZoomOutHLChkEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_TRUE != nFlagChkRgn )
		{
			/* �`�F�b�NNG���A�̈�Đݒ� */
			nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLChkEdge, nHalfXSizeImg, nHalfYSizeImg );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
		}

		/* �S�̂��܂ޗ̈�(�k����) */
		ptSrchRgnZoomOutHLsrc->nSX = ptSrchRgnZoomOutHLChkEdge->nSX;
		ptSrchRgnZoomOutHLsrc->nSY = ptSrchRgnZoomOutHLChkEdge->nSY;
		ptSrchRgnZoomOutHLsrc->nEX = ptSrchRgnZoomOutHLChkEdge->nEX;
		ptSrchRgnZoomOutHLsrc->nEY = ptSrchRgnZoomOutHLChkEdge->nEY;
		/* Haar-like���l�������̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

		/* �S�̂��܂ޗ̈�(�ʏ�) */
		nXSize = ( ( ptSrchRgnZoomOutHLsrc->nEX - ptSrchRgnZoomOutHLsrc->nSX ) + 1L ) * nprm_xMagZoomOut;
		nYSize = ( ( ptSrchRgnZoomOutHLsrc->nEY - ptSrchRgnZoomOutHLsrc->nSY ) + 1L ) * nprm_yMagZoomOut;
		ptSrchRgnHLsrc->nSX = ptSrchRgnZoomOutHLsrc->nSX * nprm_xMagZoomOut;
		ptSrchRgnHLsrc->nSY = ptSrchRgnZoomOutHLsrc->nSY * nprm_yMagZoomOut;
		ptSrchRgnHLsrc->nEX = ( ptSrchRgnHLsrc->nSX + nXSize ) - 1L;
		ptSrchRgnHLsrc->nEY = ( ptSrchRgnHLsrc->nSY + nYSize ) - 1L;
		/* Haar-like���l�������̈�`�F�b�N�^�ۂߍ��݂��s���Ă��邽�߁A�̈�`�F�b�N�͕s�v */

		/* �T�C�h�}�[�J�[�����_���o�p�T���͈� */
		/* ptSrchRgnHLsrc �Ɠ���̒l��ݒ肷�� */
		ptSrchRgnNormalHLsrc->nSX = ptSrchRgnHLsrc->nSX;
		ptSrchRgnNormalHLsrc->nSY = ptSrchRgnHLsrc->nSY;
		ptSrchRgnNormalHLsrc->nEX = ptSrchRgnHLsrc->nEX;
		ptSrchRgnNormalHLsrc->nEY = ptSrchRgnHLsrc->nEY;

	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�̈�`�F�b�N
 *
 * @param[in]	nSxRgn				:,�n�_x���W,?<=value<=?,[pixel],
 * @param[in]	nSyRgn				:,�n�_y���W,?<=value<=?,[pixel],
 * @param[in]	nExRgn				:,�I�_x���W,?<=value<=?,[pixel],
 * @param[in]	nEyRgn				:,�I�_y���W,?<=value<=?,[pixel],
 * @param[in]	nXSizeImg			:,�摜��������X�����T�C�Y,0<value<?,[pixel],
 * @param[in]	nYSizeImg			:,�摜��������Y�����T�C�Y,0<value<?,[pixel],
 *
 * @retval		CB_TRUE				:,TRUE,value=1,[-],
 * @retval		CB_FALSE			:,FALSE,value=0,[-],
 *
 * @date		2013.08.20	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static ulong cb_FPE_CheckRegion( slong nSxRgn, slong nSyRgn, slong nExRgn, slong nEyRgn, slong nXSizeImg, slong nYSizeImg )
{
	ulong	nFlagChk = CB_TRUE;

	/* �n�_�`�F�b�N */
	if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nSxRgn, nSyRgn, 0L, 0L, ( nXSizeImg - 1L ), ( nYSizeImg - 1L ) ) )
	{
		nFlagChk = CB_FALSE;
	}
	/* �I�_�`�F�b�N */
	if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nExRgn, nEyRgn, 0L, 0L, ( nXSizeImg - 1L ), ( nYSizeImg - 1L ) ) )
	{
		nFlagChk = CB_FALSE;
	}
	/* �n�I�_�`�F�b�N */
	if ( nExRgn <= nSxRgn )
	{
		nFlagChk = CB_FALSE;
	}
	if ( nEyRgn <= nSyRgn )
	{
		nFlagChk = CB_FALSE;
	}

	return ( nFlagChk );
}

/******************************************************************************/
/**
 * @brief		�̈�Đݒ�
 *
 * @param[in,out]	ptSetRgn		:,�Đݒ�̈�|�C���^,-,[-],
 * @param[in]		nXSizeImg		:,�摜��������X�����T�C�Y,0<value<?,[pixel],
 * @param[in]		nYSizeImg		:,�摜��������Y�����T�C�Y,0<value<?,[pixel],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2017.10.12	A.Honda			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ResetRegion( CB_RECT_RGN *ptSetRgn, slong nXSizeImg, slong nYSizeImg )
{
	slong	minSX = 0L;
	slong	minSY = 0L;
	slong	maxEX = nXSizeImg - 1L;
	slong	maxEY = nYSizeImg - 1L;
	ulong	nFlagChkRgn = CB_TRUE;

	/* �����`�F�b�N */
	if ( NULL == ptSetRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}
	/* �n�_�`�F�b�N */
	if( minSX > ptSetRgn->nSX )
	{
		ptSetRgn->nSX = minSX;
	}
	if( minSY > ptSetRgn->nSY )
	{
		ptSetRgn->nSY = minSY;
	}
	/* �I�_�`�F�b�N */
	if( maxEX < ptSetRgn->nEX )
	{
		ptSetRgn->nEX = maxEX;
	}
	if( maxEY < ptSetRgn->nEY )
	{
		ptSetRgn->nEY = maxEY;
	}
	/* �Đݒ�͈̓`�F�b�N */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSetRgn->nSX, ptSetRgn->nSY, ptSetRgn->nEX, ptSetRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		return ( CB_IMG_NG );
	}
	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����_���o���s�����`�F�b�N
 *
 * @param[in]	nFlagInit			:,�������t���O,0<=value<=1,[-],
 * @param[in]	ptSrchRgnInfo		:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_TRUE				:,TRUE,value=1,[-],
 * @retval		CB_FALSE			:,FALSE,value=0,[-],
 * @retval		CB_IMG_NG			:,�ُ�I��,value=-1,[-],
 *
 * @date		2013.06.03	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CheckExecCondition( slong nFlagInit, const CB_FPE_SRCH_RGN_INFO_EACH_CAM* const ptSrchRgnInfo )
{
	slong	nFlagExec = CB_FALSE;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nFlagExec = CB_FALSE;
	/* ��������ԂɊւ���`�F�b�N */
	if ( CB_TRUE != nFlagInit )
	{
		return ( nFlagExec );
	}
	/* �T���̈�ݒ���(�s���p�^�[��)�Ɋւ���`�F�b�N */
	if ( CB_TRUE != ptSrchRgnInfo->nFlagValidInfo4Pattern )
	{
		return ( nFlagExec );
	}
	/* �T���̈�ݒ���(���ۃp�^�[��)�Ɋւ���`�F�b�N */
	if ( CB_TRUE != ptSrchRgnInfo->nFlagValidInfo4AddPattern )
	{
		return ( nFlagExec );
	}

	/* ���s�\��� */
	nFlagExec = CB_TRUE;

	return ( nFlagExec );
}

/******************************************************************************/
/**
 * @brief			���o���ʂ̃N���A
 *
 * @param[out]		ptResult		:,�����_���o�o�͏��\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.08.04	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ClearResult( CB_IMG_RESULT* const ptResult )
{
	slong					ni = 0L;
	t_cb_img_CenterPosEx	*ptCenterPosEx = NULL;
	t_cb_img_CenterPos		*ptCenterPos = NULL;

	/* �����`�F�b�N */
	if ( NULL == ptResult )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	ptCenterPosEx = &( ptResult->tFtrPoint );
	/* �����_�� */
	ptCenterPosEx->CenterNum	= 0UL;
	ptCenterPosEx->LeftNum		= 0UL;
	ptCenterPosEx->RightNum		= 0UL;
	/* ���W */
	ptCenterPos = &( (ptCenterPosEx->Center)[0] );
	for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
	{
		ptCenterPos[ni].w = -1.0;
		ptCenterPos[ni].h = -1.0;
		ptCenterPos[ni].nFlagPrecise = CB_FALSE;
	}
	ptCenterPos = &( (ptCenterPosEx->Left)[0] );
	for ( ni = 0L; ni < CB_FPE_LEFT_PNT_MAXNUM; ni++ )
	{
		ptCenterPos[ni].w = -1.0;
		ptCenterPos[ni].h = -1.0;
		ptCenterPos[ni].nFlagPrecise = CB_FALSE;
	}
	ptCenterPos = &( (ptCenterPosEx->Right)[0] );
	for ( ni = 0L; ni < CB_FPE_RIGHT_PNT_MAXNUM; ni++ )
	{
		ptCenterPos[ni].w = -1.0;
		ptCenterPos[ni].h = -1.0;
		ptCenterPos[ni].nFlagPrecise = CB_FALSE;
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			Checkerboard�ɂ���������_���o
 *
 * @param[in]		nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptFPEInfo		:,�������\���̂ւ̃|�C���^,-,[-],
 * @param[out]		nCamDirection	:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]		nCalibType		:,�L�����u���(�H��E�̎�),CB_CALIB_REAR_KIND_FACTORY<=value<=CB_CALIB_REAR_KIND_RUNTIME,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.08	K.Kato			�V�K�쐬
 * @date			2016.06.24	M.Ando			�G���u�����ɂ��P�����Ή�
 *
 * @note			����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_ExtractFtrPoint4Checkerboard( const IMPLIB_IMGID nImgSrc, CB_FPE_INFO *ptFPEInfo, slong nCamDirection, slong nCalibType )
{
	slong								nRet = CB_IMG_NG;
	slong								ni = 0L, 
										nj = 0L;
	slong								nSxSrc0 = 0L, 
										nSySrc0 = 0L, 
										nExSrc0 = 0L, 
										nEySrc0 = 0L;
	slong								nSxSrc1 = 0L, 
										nSySrc1 = 0L, 
										nExSrc1 = 0L, 
										nEySrc1 = 0L;
	slong								nSxDst = 0L, 
										nSyDst = 0L, 
										nExDst = 0L, 
										nEyDst = 0L;
	enum enum_CB_IMG_ALGORITHM			nTypeAlgorithm;
	slong*								pnNumPntStore[2];
	CB_FPE_SRCH_RGN_INFO_EACH_PATTERN	*ptSrchRgnInfo = NULL;
	CB_FPE_SRCH_RSLT_INFO				*ptSrchRsltInfo = NULL;
	CB_FPE_HAAR_LIKE_FTR				*ptHaarLike = NULL;
	CB_IMG_POINT*						ptPntStore[2];
	/* �p�����[�^ */
	slong	nprm_nMaxNumPnt = 0L;
	slong	nprm_thrHaarFtrCrossPnt = 0L;	/* �s���^�[�Q�b�g�����̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l�i�[�p */

	/* �����`�F�b�N */
	if ( NULL == ptFPEInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_FTR_CROSS_PNT;

	/* ���g�p�ϐ���� ���[�j���O�΍� */
	CALIB_UNUSED_VARIABLE( nCalibType );	/* �ϐ��g�p���ɂ͍폜���Ă������� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O1.3  R-53, ID-6724

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �擪�|�C���^ */
	ptSrchRgnInfo = &(ptFPEInfo->tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[0]);
	ptSrchRsltInfo = &(ptFPEInfo->tRslt.tSrchRsltInfo[nCamDirection]);

	/* �p�����[�^�擾 */
	if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ( ptSrchRgnInfo->tPtnInfo ).nTypePattern )
	{
		nprm_nMaxNumPnt = 1L;
	}
	else if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 == ( ptSrchRgnInfo->tPtnInfo ).nTypePattern )
	{
		nprm_nMaxNumPnt = 2L;
	}
	else if ( (CB_IMG_PATTERN_TYPE_CHKBOARD2x4 == ( ptSrchRgnInfo->tPtnInfo ).nTypePattern) || (CB_IMG_PATTERN_TYPE_CHKBOARD1x4 == ( ptSrchRgnInfo->tPtnInfo ).nTypePattern) )
	{
		nprm_nMaxNumPnt = 3L;
	}
	else
	{
		/* �G���[���N���A */
		return ( CB_IMG_NG );
	}

	/* �N���A */
	nRet = cb_FPE_ClearSearchResultPointInfo( CB_FPE_NELEMS(ptSrchRsltInfo->tSrchRsltPntInfo), &(ptSrchRsltInfo->tSrchRsltPntInfo[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_INTEGRAL_IMG] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Integral Image�쐬 */
	nRet = cb_FPE_IntegralImage( &(ptFPEInfo->nImg[0]), &( ptSrchRgnInfo->tRgnImgZoomOutHLsrc), CB_FPE_PRM_INTIMG_FLG_CENTER );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_INTEGRAL_IMG );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_INTEGRAL_IMG] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like�����ʗp�̉摜���������蓖��(�摜�������A�N�Z�X�J�n) */
	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		return ( CB_IMG_NG_IMPLIB );
	}
	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		return ( CB_IMG_NG_IMPLIB );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_CHK_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like(�s��) */
	nRet = cb_FPE_HaarLikeChkEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &(ptSrchRgnInfo->tRgnImgZoomOutHLChkEdge) );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_HAAR_LIKE_FTR_CHK_EDGE );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_CHK_EDGE] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_VERT_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like(�c�G�b�W) */
	nRet = cb_FPE_HaarLikeVertEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), &(ptSrchRgnInfo->tRgnImgZoomOutHLVertEdge) );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_HAAR_LIKE_FTR_VERT_EDGE );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_VERT_EDGE] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_HORI_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like(���G�b�W) */
	nRet = cb_FPE_HaarLikeHoriEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), &(ptSrchRgnInfo->tRgnImgZoomOutHLHoriEdge) );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_HAAR_LIKE_FTR_HORI_EDGE );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_HORI_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* [DEBUG] */
#ifdef CB_FPE_CSW_DEBUG_ON
#if 0
	cb_FPE_DBG_SaveHaarLikeInfo( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), "C:/work/work_kato/SELF_CB_DBG_HAAR_LIKE_CHK_EDGE.csv" );
	cb_FPE_DBG_SaveHaarLikeInfo( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), "C:/work/work_kato/SELF_CB_DBG_HAAR_LIKE_VERT_EDGE.csv" );
	cb_FPE_DBG_SaveHaarLikeInfo( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), "C:/work/work_kato/SELF_CB_DBG_HAAR_LIKE_HORI_EDGE.csv" );
#endif
#endif /* CB_FPE_CSW_DEBUG_ON */

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_CROSS_PNT] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �\���_�T�� */
	nRet = cb_FPE_SearchCandCrossPnt( &(ptFPEInfo->nImg[0]), &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt), 
									&(ptSrchRgnInfo->tRgnImgZoomOut), nprm_nMaxNumPnt, nCamDirection, nprm_thrHaarFtrCrossPnt, &(ptSrchRgnInfo->tPtnInfo), ptSrchRgnInfo->tPtnInfo.nDoubleType );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_SRCH_CAND_CROSS_PNT );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_CROSS_PNT] );
#endif /* TIME_MEASUREMENT_CALIB */

	m_FPE_tWorkInfo.nNumBasePnt4Upper = 0L;
	m_FPE_tWorkInfo.nNumBasePnt4Lower = 0L;

	/* �J�����ʒu��Left or Right�̏ꍇ�A�E��/����T���_�T�� */
	if ( ( E_CB_SELFCALIB_CAMPOS_LEFT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_RIGHT == nCamDirection ) )
	{
	
		if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != (ptSrchRgnInfo->tPtnInfo).nTypePattern )
		{

#ifdef TIME_MEASUREMENT_CALIB
			RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_RI] );
#endif /* TIME_MEASUREMENT_CALIB */

			/* �E���s���_�T�� */
			nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt, 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntRightHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntRight), 
											&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_RIGHT_EDGE_POS_PATTERN );
			if ( CB_IMG_OK != nRet )
			{
				/* Open�����摜��������Close���� */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG_SRCH_CAND_T_PNT_RIGHT );
			}
		
#ifdef TIME_MEASUREMENT_CALIB
			RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_RI] );
			RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LE] );
#endif /* TIME_MEASUREMENT_CALIB */

			/* �����s���_�T�� */
			nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt, 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLeftHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLeft), 
											&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_LEFT_EDGE_POS_PATTERN );
			if ( CB_IMG_OK != nRet )
			{
				/* Open�����摜��������Close���� */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG_SRCH_CAND_T_PNT_LEFT );
			}
		
#ifdef TIME_MEASUREMENT_CALIB
		RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LE] );
#endif /* TIME_MEASUREMENT_CALIB */

			/* �x�[�X�_�ǉ� */
			/* UPPER */
			m_FPE_tWorkInfo.tBasePnt4Upper[m_FPE_tWorkInfo.nNumBasePnt4Upper] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tTPntLeftHalf[0];
			m_FPE_tWorkInfo.nNumBasePnt4Upper++;
			/* �x�[�X�_�ݒ� */
			for ( ni = 0L; ni < ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt; ni++ )
			{
				m_FPE_tWorkInfo.tBasePnt4Upper[m_FPE_tWorkInfo.nNumBasePnt4Upper + ni] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tCrossPntHalf[ni];
			}
			m_FPE_tWorkInfo.nNumBasePnt4Upper += ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt;
			/* LOWER */
			/* �x�[�X�_�ݒ� */
			for ( ni = 0L; ni < ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt; ni++ )
			{
				m_FPE_tWorkInfo.tBasePnt4Lower[m_FPE_tWorkInfo.nNumBasePnt4Lower + ni] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tCrossPntHalf[ni];
			}
			m_FPE_tWorkInfo.nNumBasePnt4Lower += ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt;
			m_FPE_tWorkInfo.tBasePnt4Lower[m_FPE_tWorkInfo.nNumBasePnt4Lower] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tTPntRightHalf[0];
			m_FPE_tWorkInfo.nNumBasePnt4Lower++;

		}
		else
		{
			/* �A���S���Y���^�C�v */
			nRet = cb_FPE_GetTypeAlgorithm( &( ptSrchRgnInfo->tPtnInfo ), &nTypeAlgorithm );
			if ( CB_IMG_OK != nRet )
			{
				/* Open�����摜��������Close���� */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG );
			}
			
			if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
			{
				/* �E���s���_�T�� */
				nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[1]), 1L, 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntRightHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntRight), 
												&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_RIGHT_EDGE_POS_PATTERN );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
					return ( CB_IMG_NG_SRCH_CAND_T_PNT_RIGHT );
				}

				/* ���ʗ̈�̃A�h���X���擾 */
				ptPntStore[0]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0] );
				pnNumPntStore[0]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt );
				ptPntStore[1]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntRightHalf[0] );
				pnNumPntStore[1]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntRight );
			}
			else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
			{
				/* �����s���_�T�� */
				nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), 1L, 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLeftHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLeft), 
												&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_LEFT_EDGE_POS_PATTERN );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
					return ( CB_IMG_NG_SRCH_CAND_T_PNT_LEFT );
				}

				/* ���ʗ̈�̃A�h���X���擾 */
				ptPntStore[0]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLeftHalf[0] );
				pnNumPntStore[0]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLeft );
				ptPntStore[1]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0] );
				pnNumPntStore[1]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt );
			}
			else
			{
				/* Open�����摜��������Close���� */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG );
			}

			/* �x�[�X�_�ǉ� */
			if ( CB_IMG_CHKBOARD_POS_PLACEMENT == (ptSrchRgnInfo->tPtnInfo).nFlagPlacement )
			{
				/* UPPER */
				m_FPE_tWorkInfo.nNumBasePnt4Upper = 0L;
				for ( ni = 0L; ni < 2L; ni++ )
				{
					for ( nj = 0L; nj < *(pnNumPntStore[ni]); nj++ )
					{
						m_FPE_tWorkInfo.tBasePnt4Upper[ m_FPE_tWorkInfo.nNumBasePnt4Upper ] = (ptPntStore[ni])[nj];
						m_FPE_tWorkInfo.nNumBasePnt4Upper++;
					}
				}	/* for ( ni ) */
				/* LOWER */
				m_FPE_tWorkInfo.nNumBasePnt4Lower = 0L;
				for ( nj = 0L; nj < ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt; nj++ )
				{
					m_FPE_tWorkInfo.tBasePnt4Lower[ m_FPE_tWorkInfo.nNumBasePnt4Lower ] = ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[nj];
					m_FPE_tWorkInfo.nNumBasePnt4Lower++;
				}	/* for ( nj ) */
			}
			else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == (ptSrchRgnInfo->tPtnInfo).nFlagPlacement )
			{
				/* UPPER */
				m_FPE_tWorkInfo.nNumBasePnt4Upper = 0L;
				for ( nj = 0L; nj < ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt; nj++ )
				{
					m_FPE_tWorkInfo.tBasePnt4Upper[ m_FPE_tWorkInfo.nNumBasePnt4Upper ] = ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[nj];
					m_FPE_tWorkInfo.nNumBasePnt4Upper++;
				}	/* for ( nj ) */
				/* LOWER */
				m_FPE_tWorkInfo.nNumBasePnt4Lower = 0L;
				for ( ni = 0L; ni < 2L; ni++ )
				{
					for ( nj = 0L; nj < *(pnNumPntStore[ni]); nj++ )
					{
						m_FPE_tWorkInfo.tBasePnt4Lower[ m_FPE_tWorkInfo.nNumBasePnt4Lower ] = (ptPntStore[ni])[nj];
						m_FPE_tWorkInfo.nNumBasePnt4Lower++;
					}
				}	/* for ( ni ) */
			}
			else
			{
				/* Open�����摜��������Close���� */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG );
			}
		}
	}
	else
	{
		/* �x�[�X�_�ݒ� */
		for ( ni = 0L; ni < ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt; ni++ )
		{
			m_FPE_tWorkInfo.tBasePnt4Upper[ni] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tCrossPntHalf[ni];
			m_FPE_tWorkInfo.tBasePnt4Lower[ni] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tCrossPntHalf[ni];
		}
		m_FPE_tWorkInfo.nNumBasePnt4Upper = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt;
		m_FPE_tWorkInfo.nNumBasePnt4Lower = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt;
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LO] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* ����T���_�T�� */
	nRet = cb_FPE_SearchCandTPntUL( &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), 
									&(m_FPE_tWorkInfo.tBasePnt4Lower[0]), m_FPE_tWorkInfo.nNumBasePnt4Lower, 
									&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLowerHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLower), 
									&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_LOWER_EDGE_POS_PATTERN );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_SRCH_CAND_T_PNT_LOWER );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LO] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_UP] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �㑤�s���_�T�� */
	nRet = cb_FPE_SearchCandTPntUL( &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), 
									&(m_FPE_tWorkInfo.tBasePnt4Upper[0]), m_FPE_tWorkInfo.nNumBasePnt4Upper, 
									&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntUpperHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntUpper), 
									&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_UPPER_EDGE_POS_PATTERN );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_SRCH_CAND_T_PNT_UPPER );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_UP] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like�����ʗp�̉摜���������蓖�ĉ���(�摜�������A�N�Z�X�I��) */
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �g��(���W�l) */
	nRet = cb_FPE_ZoomIn4Coordinates( &(ptSrchRsltInfo->tSrchRsltPntInfo[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_ZOOM_IN_CAND_COORD );
	}


#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_PREC_POS_CHK] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �����x�ʒu���� */
	nRet = cb_FPE_PrecisionPositioning4Checkerboard( nImgSrc, &(ptSrchRsltInfo->tSrchRsltPntInfo[0]), ptSrchRgnInfo, nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		/* �����֐��̃G���[�R�[�h�����̂܂ܕԂ�����!(�G���[��Ԃ��ڍׂɐ؂蕪���邽��) */
		return ( nRet );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_PREC_POS_CHK] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�~�������p�^�[���ɂ���������_���o
 *
 * @param[in]		nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptFPEInfo		:,�������\���̂ւ̃|�C���^,-,[-],
 * @param[out]		nCamDirection	:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]		nCalibType		:,�L�����u���(�H��E�̎�),CB_CALIB_REAR_KIND_FACTORY<=value<=CB_CALIB_REAR_KIND_RUNTIME,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.10	K.Kato			�V�K�쐬
 *
 * @note			����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_ExtractFtrPoint4CircleLine( const IMPLIB_IMGID nImgSrc, CB_FPE_INFO *ptFPEInfo, slong nCamDirection, slong nCalibType )
{
	slong									nRet = CB_IMG_NG;
	slong									nSxSrc0 = 0L, 
											nSySrc0 = 0L, 
											nExSrc0 = 0L, 
											nEySrc0 = 0L;
	slong									nSxSrc1 = 0L, 
											nSySrc1 = 0L, 
											nExSrc1 = 0L, 
											nEySrc1 = 0L;
	slong									nSxDst = 0L, 
											nSyDst = 0L, 
											nExDst = 0L, 
											nEyDst = 0L;
	slong									nCntRetry = 0L;
	slong									nFlagProcOK = CB_FALSE;
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo = NULL;
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO			*ptSrchRsltPntInfo = NULL;
	/* �p�����[�^ */
	slong	nprm_cntRetryCircleLineMaxNum = 0L;
	slong	nprm_retryDiff4RRFdiffIntensity = 0L;

#ifndef	_WINDOWS
	ER	ercd = E_OK;
#endif	// _WINDOWS

	/* �����`�F�b�N */
	if ( NULL == ptFPEInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ���g�p�ϐ���� ���[�j���O�΍� */
	CALIB_UNUSED_VARIABLE( nCalibType );	/* �ϐ��g�p���ɂ͍폜���Ă������� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O1.3  R-53, ID-6725

	/* �p�����[�^�擾 */
	nprm_cntRetryCircleLineMaxNum = CB_FPE_CNT_RETRY_CIRCLE_LINE_MAXNUM;
	nprm_retryDiff4RRFdiffIntensity = CB_FPE_PRM_RRF_DIFF_INTENSITY_RETRY_DIFF;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �擪�|�C���^ */
	ptSrchRgnInfo = &(ptFPEInfo->tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]);
	ptSrchRsltPntInfo = &(ptFPEInfo->tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltAddPntInfo[0]);

	/* �N���A */
	nRet = cb_FPE_ClearSearchResultAddPointInfo( ptSrchRsltPntInfo );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SMOOTH] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* ������ */
#if 0
	nRet = cb_FPE_Smoothing( nImgSrc, &(ptFPEInfo->nImg[0]), ptSrchRgnInfo );
	if ( CB_IMG_OK != nRet ) 
	{
		return ( CB_IMG_NG_SMOOTHING );
	}
#endif

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SMOOTH] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_RRF] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �p�����[�^�ݒ� */
	m_FPE_tPrm.nDiffIntensity4Retry = CB_FPE_PRM_RRF_DIFF_INTENSITY;
	/* �t���O�ݒ� */
	nFlagProcOK = CB_FALSE;
	for ( nCntRetry = 0L; nCntRetry < nprm_cntRetryCircleLineMaxNum; nCntRetry++ )
	{
		/* Radial Reach���� */
		nRet = cb_FPE_RadialReachFeature( nImgSrc, &(ptFPEInfo->nImg[0]), ptSrchRgnInfo );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG_RRF );
		}
	
#ifdef TIME_MEASUREMENT_CALIB
		RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_RRF] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* [DEBUG] */
#ifdef CB_FPE_CSW_DEBUG_ON
//		cb_FPE_DBG_SavePixelValue( ptFPEInfo->nImg[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], "C:/work/work_kato/SELF_CB_DBG_RRF_IMG.csv" );
#endif /* CB_FPE_CSW_DEBUG_ON */

#ifdef TIME_MEASUREMENT_CALIB
		RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_CIRC_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */

		/* �~�̈�̒��o */
//		if ( E_CB_SELFCALIB_CAMPOS_FRONT != nCamDirection )
//		{
#ifdef TIME_MEASUREMENT_CALIB
			RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_CIRC_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */

			nRet = cb_FPE_ExtractCircleRegion( nImgSrc, &(ptFPEInfo->nImg[0]), ptSrchRsltPntInfo, ptSrchRgnInfo, nCamDirection );
		
#ifdef TIME_MEASUREMENT_CALIB
			RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_CIRC_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */
//		}
//		else
//		{
//#ifdef TIME_MEASUREMENT_CALIB
//			RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_CIRC_RGN_FR] );
//#endif /* TIME_MEASUREMENT_CALIB */
//			nRet = cb_FPE_ExtractCircleRegion4FrontCAM( &(ptFPEInfo->nImg[0]), ptSrchRsltPntInfo, ptSrchRgnInfo );
//#ifdef TIME_MEASUREMENT_CALIB
//			RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_CIRC_RGN_FR] );
//#endif /* TIME_MEASUREMENT_CALIB */
//		}

		if ( CB_IMG_OK == nRet )
		{
			nFlagProcOK = CB_TRUE;
			break;
		}
		else
		{
			/* �����_�����݂��Ȃ��Ƃ�����ԈȊO�́A������RETURN���� */
			if ( CB_IMG_NG_NO_CIRCLE_FTR_PNT != nRet )
			{
				return ( CB_IMG_NG_EXTRACT_CIRC_RGN );
			}
		}

		/* �p�����[�^�ݒ�(���g���C��) */
		m_FPE_tPrm.nDiffIntensity4Retry = m_FPE_tPrm.nDiffIntensity4Retry + nprm_retryDiff4RRFdiffIntensity;

#ifndef _WINDOWS
		ercd = cl_dly_tsk( 30UL );		/* ��������30ms�̃f�B���C��}�� */
		if( E_OK != ercd )
		{
			/* Not Operation */	
		}
#endif	//_WINDOWS

	}	/* for ( nCntRetry ) */

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			Checkerboard�p�����_���o���̃N���A
 *
 * @param[in]		nLoopIdx			:,����������\���̂̔z��,-,[-],
 * @param[out]		ptSrchRsltPntInfo	:,�T�����ʏ��\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.10	K.Kato			�V�K�쐬
 * @date			2013.11.22	F.Sano			����nLoopIdx��ǉ�(����������z��T�C�Y���ςɕύX)
 */
/******************************************************************************/
static slong cb_FPE_ClearSearchResultPointInfo( slong nLoopIdx, CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo )
{
	slong			ni = 0L, 
					nj = 0L;
	CB_IMG_POINT	*ptImgPnt = NULL;
	t_cb_img_CenterPos	*ptImgPntPrecise = NULL;

	/* �����`�F�b�N */
	if( NULL == ptSrchRsltPntInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 0L; ni < nLoopIdx; ni++ )
	{
		ptSrchRsltPntInfo[ni].nNumCrossPnt = 0L;
		ptSrchRsltPntInfo[ni].nNumTPntLeft = 0L;
		ptSrchRsltPntInfo[ni].nNumTPntRight = 0L;
		ptSrchRsltPntInfo[ni].nNumTPntUpper = 0L;
		ptSrchRsltPntInfo[ni].nNumTPntLower = 0L;

		ptImgPnt = &(ptSrchRsltPntInfo[ni].tCrossPnt[0]);
		ptImgPntPrecise = &(ptSrchRsltPntInfo[ni].tCrossPntPrecise[0]);
		for ( nj = 0L; nj < CB_FPE_CROSS_PNT_MAXNUM; nj++ )
		{
			ptImgPnt[nj].nX = -1;
			ptImgPnt[nj].nY = -1;
			ptImgPntPrecise[nj].w = -1.0;
			ptImgPntPrecise[nj].h = -1.0;
		}
		ptImgPnt = &(ptSrchRsltPntInfo[ni].tTPntLeft[0]);
		ptImgPntPrecise = &(ptSrchRsltPntInfo[ni].tTPntLeftPrecise[0]);
		for ( nj = 0L; nj < CB_FPE_T_PNT_LEFT_MAXNUM; nj++ )
		{
			ptImgPnt[nj].nX = -1;
			ptImgPnt[nj].nY = -1;
			ptImgPntPrecise[nj].w = -1.0;
			ptImgPntPrecise[nj].h = -1.0;
		}
		ptImgPnt = &(ptSrchRsltPntInfo[ni].tTPntRight[0]);
		ptImgPntPrecise = &(ptSrchRsltPntInfo[ni].tTPntRightPrecise[0]);
		for ( nj = 0L; nj < CB_FPE_T_PNT_RIGHT_MAXNUM; nj++ )
		{
			ptImgPnt[nj].nX = -1;
			ptImgPnt[nj].nY = -1;
			ptImgPntPrecise[nj].w = -1.0;
			ptImgPntPrecise[nj].h = -1.0;
		}
		ptImgPnt = &(ptSrchRsltPntInfo[ni].tTPntUpper[0]);
		ptImgPntPrecise = &(ptSrchRsltPntInfo[ni].tTPntUpperPrecise[0]);
		for ( nj = 0L; nj < CB_FPE_T_PNT_UPPER_MAXNUM; nj++ )
		{
			ptImgPnt[nj].nX = -1;
			ptImgPnt[nj].nY = -1;
			ptImgPntPrecise[nj].w = -1.0;
			ptImgPntPrecise[nj].h = -1.0;
		}
		ptImgPnt = &(ptSrchRsltPntInfo[ni].tTPntLower[0]);
		ptImgPntPrecise = &(ptSrchRsltPntInfo[ni].tTPntLowerPrecise[0]);
		for ( nj = 0L; nj < CB_FPE_T_PNT_LOWER_MAXNUM; nj++ )
		{
			ptImgPnt[nj].nX = -1;
			ptImgPnt[nj].nY = -1;
			ptImgPntPrecise[nj].w = -1.0;
			ptImgPntPrecise[nj].h = -1.0;
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�~�������p�^�[���p�����_���o���̃N���A
 *
 * @param[out]		ptSrchRsltPntInfo	:,�T�����ʏ��\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.10	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ClearSearchResultAddPointInfo( CB_FPE_SRCH_RSLT_ADD_PNT_INFO* ptSrchRsltPntInfo )
{
	slong			ni = 0L, 
					nj = 0L;
	CB_IMG_POINT	*ptImgPnt = NULL;
	t_cb_img_CenterPos	*ptCenterPntPrecise = NULL;

	/* �����`�F�b�N */
	if( NULL == ptSrchRsltPntInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		ptSrchRsltPntInfo[ni].nNumCenterPnt = 0L;

		ptImgPnt = &(ptSrchRsltPntInfo[ni].tCenterPnt[0]);
		ptCenterPntPrecise = &(ptSrchRsltPntInfo[ni].tCenterPntPrecise[0]);
		for ( nj = 0L; nj < CB_ADD_PATTERN_PNT_POS_MAXNUM; nj++ )
		{
			ptImgPnt[nj].nX = -1;
			ptImgPnt[nj].nY = -1;
			ptCenterPntPrecise[nj].w = -1.0;
			ptCenterPntPrecise[nj].h = -1.0;
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Integral Image�쐬
 *
 * @param[in,out]	pnImgWk			:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[in]		ptSrchRgnInfo	:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.16	K.Kato			�V�K�쐬
 * @date		2015.08.26	S.Morita		�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�
 */
/******************************************************************************/
static slong cb_FPE_IntegralImage( const IMPLIB_IMGID* const pnImgWk, const CB_RECT_RGN* const ptSrchRgn, const uchar nIntgralImgFlg )
{
	slong		nRet = CB_IMG_NG;
	slong		nSxSrc0 = 0L, 
				nSySrc0 = 0L, 
				nExSrc0 = 0L, 
				nEySrc0 = 0L;
	slong		nSxDst = 0L, 
				nSyDst = 0L, 
				nExDst = 0L, 
				nEyDst = 0L;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}


	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY );

	if( CB_FPE_PRM_INTIMG_FLG_CENTER == nIntgralImgFlg )
	{
		/* Integral Image(�k��) */
		nRet = implib_IP_IntegralImage( pnImgWk[CB_FPE_IMG_ZOOMOUT2x2], pnImgWk[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2] );
	}
	else if( CB_FPE_PRM_INTIMG_FLG_SIDE == nIntgralImgFlg )
	{
		/* Integral Image(���͉摜�Ɠ����T�C�Y) */
		nRet = implib_IP_IntegralImage( pnImgWk[CB_FPE_IMG_NORMAL], pnImgWk[CB_FPE_INTEGRAL_IMG_NORMAL] );
	}
	else
	{
		nRet = CB_IMG_NG;
	}

	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		������
 *
 * @param[in]	nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	pnImgWk			:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^(�ǉ��p�^�[��),-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.11	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_Smoothing( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6727
{
	slong		nRet = CB_IMG_NG;
	slong		ni = 0L;
	slong		nSxSrc0 = 0L, 
				nSySrc0 = 0L, 
				nExSrc0 = 0L, 
				nEySrc0 = 0L;
	slong		nSxDst = 0L, 
				nSyDst = 0L, 
				nExDst = 0L, 
				nEyDst = 0L;
	CB_RECT_RGN	*ptRgn = NULL;
	/* �p�����[�^ */
	slong	nprm_coeffSmoothFLT[9] = { CB_FPE_PRM_COEFF_SMOOTH_FLT };
	slong	nprm_scaleSmoothFLT = CB_FPE_PRM_SCALE_SMOOTH_FLT;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* �擪�|�C���^ */
		ptRgn = &(ptSrchRgnInfo[ni].tRgnImgRRF);

		/* ������ */
		nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ( ptRgn->nSX - 1L ), ( ptRgn->nSY - 1L ), ( ptRgn->nEX + 1L ), ( ptRgn->nEY + 1L ) );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ( ptRgn->nSX - 1L ), ( ptRgn->nSY - 1L ), ( ptRgn->nEX + 1L ), ( ptRgn->nEY + 1L ) );
		nRet = implib_IP_SmoothFLT( nImgSrc, pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], nprm_scaleSmoothFLT, &(nprm_coeffSmoothFLT[0]) );
		if ( 0L > nRet )
		{
			/* �G���[���N���A */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		���˃��[�`����
 *
 * @param[in]	nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	pnImgWk			:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^(�ǉ��p�^�[��),-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.28	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_RadialReachFeature( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )
{
	slong				nRet = CB_IMG_NG;
	slong				ni = 0L, 
						nj = 0L, 
						nk = 0L, 
						nm = 0L;
	slong				nXSizeImg = 0L, 
						nYSizeImg = 0L;
//	slong				nXSizeImgSmooth = 0L, 
//						nYSizeImgSmooth = 0L;
	slong				nXSizeImgDst = 0L, 
						nYSizeImgDst = 0L;
	slong				nAveValue = 0L;
	slong				nFlgReachOK[8] = { 0L }, 
						nCntReachOK = 0L;
	slong				nCntReach = 0L;
	slong				nLenReach[8] = { 0L };
	slong				nXSizeSrchRgn = 0L, 
						nYSizeSrchRgn = 0L;
	slong				nOffset = 0L;
	CB_FPE_VECTOR_2D	tVecArray[8] = {
											{ +0L, -1L },	/* [0]�� */
											{ +1L, -1L },	/* [1]�E�� */
											{ +1L, +0L },	/* [2]�E */
											{ +1L, +1L },	/* [3]�E�� */
											{ +0L, +1L },	/* [4]�� */
											{ -1L, +1L },	/* [5]���� */
											{ -1L, +0L },	/* [6]�� */
											{ -1L, -1L },	/* [7]���� */
										};
	uchar				*pnAddrPixelArray[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	uchar				*pnTopAddrPixel = NULL, 
						*pnAddrPixel = NULL;
//	uchar				*pnTopAddrPixelSmooth = NULL, 
//						*pnAddrPixelSmooth = NULL;
	uchar				*pnTopAddrPixelDst = NULL, 
						*pnAddrPixelDst = NULL;
	CB_RECT_RGN			*ptSrchRgn = NULL;
	/* �p�����[�^ */
	slong	nprm_lengthReach = CB_FPE_PRM_RRF_LENGTH_REACH;
	slong	nprm_diffIntensity = m_FPE_tPrm.nDiffIntensity4Retry;
	slong	nprm_startReach = CB_FPE_PRM_RRF_START_REACH;
	slong	nprm_thrReach4Mag = CB_FPE_PRM_RRG_THR_REACH_FOR_MAG;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgSrc, &nXSizeImg, &nYSizeImg, (void**)&pnTopAddrPixel );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
#if 0
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], &nXSizeImgSmooth, &nYSizeImgSmooth, (void**)&pnTopAddrPixelSmooth );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();	/* implib_CloseImgDirect()���s�̂��߂ɁA�G���[���N���A */
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
#endif
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], &nXSizeImgDst, &nYSizeImgDst, (void**)&pnTopAddrPixelDst );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();	/* implib_CloseImgDirect()���s�̂��߂ɁA�G���[���N���A */
//		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}

	for ( nm = 0L; nm < CB_ADD_PATTERN_POS_MAXNUM; nm++ )	/* �p�^�[���̐��Ń��[�v */
	{
		ptSrchRgn = &(ptSrchRgnInfo[nm].tRgnImgRRF);
		nXSizeSrchRgn = ( ptSrchRgn->nEX - ptSrchRgn->nSX ) + 1L;
		nYSizeSrchRgn = ( ptSrchRgn->nEY - ptSrchRgn->nSY ) + 1L;

		for ( ni = nprm_lengthReach; ni < ( nYSizeSrchRgn - nprm_lengthReach ); ni++ )	/* Y���� */
		{
			/* �A�N�Z�X�J�n�A�h���X */
//			pnAddrPixelSmooth = pnTopAddrPixelSmooth + ( ( ptSrchRgn->nSY + ni ) * nXSizeImgSmooth ) + ( ptSrchRgn->nSX + nprm_lengthReach );
			pnAddrPixel = pnTopAddrPixel + ( ( ptSrchRgn->nSY + ni ) * nXSizeImg ) + ( ptSrchRgn->nSX + nprm_lengthReach );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6732
			pnAddrPixelDst = pnTopAddrPixelDst + ( ( ptSrchRgn->nSY + ni ) * nXSizeImgDst ) + ( ptSrchRgn->nSX + nprm_lengthReach );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6733

#if 0
			for ( nj = nprm_lengthReach; nj < ( nXSizeSrchRgn - nprm_lengthReach ); nj++ )	/* X���� */
#else
			nOffset = 1L;
			for ( nj = nprm_lengthReach; nj < ( nXSizeSrchRgn - nprm_lengthReach ); nj+=nOffset )	/* X���� */
#endif
			{
				/* ���S�t�߂̕��� */
				nAveValue = (slong)(*pnAddrPixel);

				/* ������ */
				nCntReachOK = 0L;
				memset( &(nFlgReachOK[0]), 0x00, ( 8UL * sizeof( slong ) ) );
				memset( &(nLenReach[0]), 0x00, ( 8UL * sizeof( slong ) ) );
				
				for ( nCntReach = nprm_startReach; nCntReach <= nprm_lengthReach; nCntReach++ )
				{
					for ( nk = 0L; nk < 8L; nk++ )
					{
						/* �Q�Ɛ�X�V */
						pnAddrPixelArray[nk] = pnAddrPixel + ( ( nCntReach * tVecArray[nk].nY ) * nXSizeImg ) + ( nCntReach * tVecArray[nk].nX );

						/* �P�x�l�̍����`�F�b�N */
						if ( ( CB_FALSE == nFlgReachOK[nk] ) && ( ( nAveValue + nprm_diffIntensity ) < (slong)(*(pnAddrPixelArray[nk])) ) )
						{
							nFlgReachOK[nk] = CB_TRUE;
							nLenReach[nk] = nCntReach;
							nCntReachOK++;
						}
					}
				}

				/* 2013.06.11 K.Kato add */
				if ( 0L == nm )	/* �� */
				{
					if (   ( CB_TRUE != nFlgReachOK[2] ) || ( CB_TRUE != nFlgReachOK[6] )
						|| ( CB_TRUE != nFlgReachOK[7] ) || ( CB_TRUE != nFlgReachOK[3] ) )
					{
						nCntReachOK = 0L;
					}
				}
				else
				{
					if (   ( CB_TRUE != nFlgReachOK[2] ) || ( CB_TRUE != nFlgReachOK[6] )
						|| ( CB_TRUE != nFlgReachOK[1] ) || ( CB_TRUE != nFlgReachOK[5] ) )
					{
						nCntReachOK = 0L;
					}
				}

				/* ���˃J�E���^���i�[ */
				*pnAddrPixelDst = (uchar)nCntReachOK;

				/* X�����ɃV�t�g */
//				pnAddrPixelSmooth++;
				pnAddrPixel++;
				pnAddrPixelDst++;
#if 1
				nOffset = 1L;
				if ( nprm_thrReach4Mag > nCntReachOK )
				{
					*pnAddrPixelDst = (uchar)nCntReachOK;

					nOffset++;
//					pnAddrPixelSmooth++;
					pnAddrPixel++;		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6734
					pnAddrPixelDst++;
				}
#endif
			}
		}
	}
	
	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nImgSrc );
//	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );

	/* ��ʃf�[�^�^�C�v��ύX */
	nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], IMPLIB_UNSIGN8_DATA );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�~�̈�̒��_���o
 *
 * @param[in]	nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	pnImgWk			:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[out]	ptSrchRsltInfo	:,�T�����ʏ��\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection	:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.01	K.Kato			�V�K�쐬
 * @date		2017.10.04	A.Honda			���ۂ̌��m�\�͌���(#2-��2)
 *
 * @note		����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_ExtractCircleRegion( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RSLT_ADD_PNT_INFO *ptSrchRsltInfo, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo, slong nCamDirection )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj, nk, nl;
	slong	nIdxUpper = 0L, 
			nIdxLower = 0L;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nNumLabel = 0L;
	slong	nCntCandPntUpper, nCntCandPntLower;
	slong	nDistanceCandPntUpper, nDistanceCandPntLower;
	slong	nXSizeImg = 0L, 
			nYSizeImg = 0L;
	slong	nScanY = 0L,
			nScanX = 0L, 
			nScanWidth = 0L;
	slong	nSumX = 0L, 
			nSumY = 0L;
	slong	nCntCoord = 0L;
	slong	nCntCandLine = 0L;
	slong	nIdxCandLine = 0L;
	slong	nCntCircleLineComb = 0L;
	slong	nIdxCircleLineComb = 0L;
	slong	nAttnX = 0L, 
			nAttnY = 0L;
	slong	nSumValue = 0L;
	slong	nRepValue = 0L;
	slong	nIdxCircle[2] = { 0L, 0L };
	slong	nCntRetry = 0L;
	slong	nFlagProcOK = CB_FALSE;
	float_t	fCoordUpper[2L], 
			fCoordLower[2L],
			fCoordMiddle[2L];
	float_t		fEstUpperX = 0.0f, 
				fEstLowerX = 0.0f;
	float_t	fDistanceUpper, fDistanceLower;
	float_t	fLengthCirclePart = 0.0f;
	float_t	fMinDiffDistance = 0.0f, 
			fMinFarDistance = 0.0f;
#ifndef CB_FPE_CSW_JUDGE_BR_SIMILARITY
	float_t	fFarDistance = 0.0f, 
			fDiffX = 0.0f,
			fDiffY = 0.0f, 
			fDiffDistance = 0.0f;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */
	double_t	dCoeffAve = 0.0;
	CB_FPE_LINE_COEFF_F	tUpperLineCoeff4Line = {0.0f, 0.0f, CB_FPE_LINE_SLOPE_DIAG_TYPE};
	CB_FPE_LINE_COEFF_F	tLowerLineCoeff4Line = {0.0f, 0.0f, CB_FPE_LINE_SLOPE_DIAG_TYPE};
	CB_FPE_IMG_ACCESS_INFO	tImgSrcAccessInfo;
	uchar	*pnTopAddrPixel = NULL, 
			*pnWkAddrPixel = NULL;
	slong	*pnTblLabelMinX, *pnTblLabelMaxX, *pnTblLabelMinY, *pnTblLabelMaxY;
	slong	*pnTblAreaLabel;
	CB_RECT_RGN	*ptRectRgn;
	CB_IMG_POINT	*ptCandPoint;
	IMPLIB_IPLOGravityTbl	*ptLOGravTbl;
	CB_FPE_LINE_PART_INFO	*ptLinePartInfo = NULL;
	CB_FPE_CIRCLE_LINE_COMB		*ptCircleLineComb = NULL;
	float_t	fUpperLineLength;			/* ��p�ߎ���������(d_1) */
	float_t	fLowerLineLength;			/* ���p�ߎ���������(d_2) */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY													/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
	CB_FPE_CBR_INPUT			tBrInput;	
	CB_FPE_CBR_POINT_INFO		*ptBrPointTbl = NULL;                                   /* �P�x����p���W���                 */
	CB_FPE_IMG_ACCESS_INFO		tImgRRFAccessInfo;
	slong						nBrTbl[256]              = { 0L };                      /*                                    */
	slong						nIdTbl[256]              = { 0L };                      /* ��                                 */
	slong						nWidthUpper              = 0L;                          /*                                    */
	slong						nWidthLower              = 0L;                          /*                                    */
	slong						nWidthMiddle             = 0L;                          /*                                    */
	slong						nIdxLinePart             = 0L;                          /* ��                                 */
	slong						nSrchUpperMinX           = 0L;                          /* QAC�Ή� �㑤�ےT���ŏ�X            */
	slong						nSrchLowerMinX           = 0L;                          /* QAC�Ή� �����ےT���ŏ�X            */
	slong						nSrchUpperMaxX           = 0L;                          /* QAC�Ή� �㑤�ےT���ő�X            */
	slong						nSrchLowerMaxX           = 0L;                          /* QAC�Ή� �����ےT���ő�X            */
	slong						nSrchMinX                = 0L;                          /* QAC�Ή� �����̈�O�`�F�b�N�p�ϐ�   */
	slong						nSrchMaxX                = 0L;                          /* QAC�Ή� �����̈�O�`�F�b�N�p�ϐ�   */
	uchar						nIdxUpperCircle;                                        /* �ی��C���f�b�N�X(��[)           */
	uchar						nIdxLowerCircle;                                        /* �ی��C���f�b�N�X(���[)           */
	uchar						nIdxLine;                                               /* �����C���f�b�N�X                 */
	float_t						fSimilarity              = 0.0F;                        /*                                    */
	float_t						fContrast                = 0.0F;                        /*                                    */
	float_t						fSlopeUpper,fSlopeLower  = 0.0F;                        /*                                    */
	float_t 					fMaxScore                = 0.0F;                        /*                                    */
	float_t						fShiftXUpper             = 0.0F;                        /* ��                                 */
	float_t						fShiftXLower             = 0.0F;                        /* ��                                 */
	float_t						fSumBlackArea            = 0.0F;                        /* ��                                 */
	float_t						fScoreTbl[256]           = { 0.0F };                    /* ��                                 */		// MISRA-C����̈�E [EntryAVM_QAC#3] R1.2.1  R-31, ID-6735
	float_t						fSumDistance             = 0.0F;                        /* ��                                 */
	float_t						fDiffXUpper, fDiffYUpper, fDiffXLower, fDiffYLower;     /*                                    */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */												/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

	/* �p�����[�^ */
	slong	nprm_cntReach;
	slong	nprm_thrminLabelArea, nprm_thrmaxLabelArea;
	slong	nprm_thrLineLabel, nprm_thrCircleLabel;
	slong	nprm_scanHeightLineUL;
	slong	nprm_diffIntensityPrecise;
	slong	nprm_cntRetryLabelingThrAreaDiff;
	slong	nprm_cntRetryLabelingMaxNum;
	float_t	/*fprm_aspectRatioVert, fprm_aspectRatioHori,*/ fprm_aspectRatio, fprm_aspectRatioInv;
	float_t	fprm_thrDistanceRatio, fprm_thrDistanceRatioInv;
	float_t	fprm_lengthRatio, fprm_lengthRatioInv;
	float_t	fprm_thrDiffEstX;
	float_t	fprm_coeffLineLength;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRsltInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ���g�p�ϐ���� ���[�j���O�΍� */
	CALIB_UNUSED_VARIABLE( nCamDirection );	/* �ϐ��g�p���ɂ͍폜���Ă������� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O1.3  R-53, ID-6736

	/* �p�����[�^�擾 */
	nprm_cntReach = CB_FPE_PRM_THR_CNT_REACH;
	nprm_thrminLabelArea = CB_FPE_PRM_THR_MIN_LABEL_AREA;
	nprm_thrmaxLabelArea = CB_FPE_PRM_THR_MAX_LABEL_AREA;
	nprm_thrLineLabel = CB_FPE_PRM_THR_LINE_LABEL;
	nprm_thrCircleLabel = CB_FPE_PRM_THR_CIRCLE_LABEL;
	nprm_scanHeightLineUL = CB_FPE_PRM_SCAN_HEIGHT_LINE_UL;
	nprm_diffIntensityPrecise = CB_FPE_PRM_RRF_DIFF_INTENSITY_PRECISE;
	nprm_cntRetryLabelingThrAreaDiff = CB_FPE_CNT_RETRY_LABELING_THR_AREA_DIFF;
	nprm_cntRetryLabelingMaxNum = CB_FPE_CNT_RETRY_LABELING_MAXNUM;
//	fprm_aspectRatioVert = 0.4f;
//	fprm_aspectRatioHori = 0.3f;
	fprm_aspectRatio = CB_FPE_PRM_ASPECT_RATIO;
	fprm_aspectRatioInv = 1.0f / fprm_aspectRatio;
	fprm_lengthRatio = CB_FPE_PRM_LENGTH_RATIO;
	fprm_lengthRatioInv = 1.0f / fprm_lengthRatio;
	fprm_thrDistanceRatio = CB_FPE_PRM_THR_DISTANCE_RATIO;
	fprm_thrDistanceRatioInv = 1.0f / fprm_thrDistanceRatio;
	fprm_thrDiffEstX = CB_FPE_PRM_THR_DIFF_EST_X;
	fprm_coeffLineLength = CB_FPE_PRM_COEFF_LINE_LENGTH;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �擪�|�C���^ */
	pnTblLabelMinX = &(m_FPE_tWorkInfo.nTblLabelMinX[0]);
	pnTblLabelMaxX = &(m_FPE_tWorkInfo.nTblLabelMaxX[0]);
	pnTblLabelMinY = &(m_FPE_tWorkInfo.nTblLabelMinY[0]);
	pnTblLabelMaxY = &(m_FPE_tWorkInfo.nTblLabelMaxY[0]);
	pnTblAreaLabel = &(m_FPE_tWorkInfo.nTblAreaLabel[0]);
	ptLOGravTbl = &(m_FPE_tWorkInfo.tTblGravity[0]);
	ptLinePartInfo = &(m_FPE_tWorkInfo.tLinePartInfo[0L]);
	ptCircleLineComb = &(m_FPE_tWorkInfo.tCircleLineComb[0L]);
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																	/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
	ptBrPointTbl = &(tBrInput.tBrPointTbl[0]);
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																	/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
	/* 0�N���A */
	memset( &tBrInput, 0x00, sizeof( CB_FPE_CBR_INPUT ) );
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgSrcAccessInfo.nXSize ), &( tImgSrcAccessInfo.nYSize ), (void**)&( tImgSrcAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* �����̈� */
		ptRectRgn = &(ptSrchRgnInfo[ni].tRgnImg);
		/* ���_�i�[�� */
		ptCandPoint = &(ptSrchRsltInfo[ni].tCenterPnt[0]);
		/* �N���A */
		nCntCandPntUpper = 0L;
		nCntCandPntLower = 0L;
		memset( pnTblLabelMinX, 0x00, ( 256UL * sizeof( slong ) ) );
		memset( pnTblLabelMaxX, 0x00, ( 256UL * sizeof( slong ) ) );
		memset( pnTblAreaLabel, 0x00, ( 256UL * sizeof( slong ) ) );
		nDistanceCandPntUpper = ( 720 * 720 ) + ( 480 * 480 );
		nDistanceCandPntLower = ( 720 * 720 ) + ( 480 * 480 );

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY		/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
		m_FPE_tDbgInfo.nNum[ni] = 0L;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */	/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

		/* �������l��������2�l�� */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_Binarize( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], nprm_cntReach );

		/* �m�C�Y����(�����x���t���ł�overflow��h�~���邽�ߒǉ� 2013.09.30 K.Kato) */
//		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
//		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = IP_PickNoise4( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );

		/* [DEBUG] */
#ifdef CB_FPE_CSW_DEBUG_ON
//		cb_FPE_DBG_SavePixelValue( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], "C:/work/work_kato/SELF_CB_DBG_RRF_IMG_withPickNoise.csv" );
#endif /* CB_FPE_CSW_DEBUG_ON */

		/* Labeling */
		/* Window�ݒ� */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );

		/* �p�����[�^�ݒ� */
		nprm_thrminLabelArea = CB_FPE_PRM_THR_MIN_LABEL_AREA;
		nFlagProcOK = CB_FALSE;
		for ( nCntRetry = 0L; nCntRetry < nprm_cntRetryLabelingMaxNum; nCntRetry++ )
		{	
			nNumLabel = implib_IP_Label8withAreaFLT( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], nprm_thrminLabelArea, nprm_thrmaxLabelArea, IMPLIB_LABEL_OBJ );
			if ( CB_FPE_CODE_LABEL_OVERFLOW == nNumLabel )
			{
				nRet = implib_ClearIPError();
				/* ���g���C���̂������l�ݒ� */
				nprm_thrminLabelArea += nprm_cntRetryLabelingThrAreaDiff;
			}
			else if ( 0L > nNumLabel )
			{
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nImgSrc );
				return ( CB_IMG_NG_IMPLIB );
			}
			else
			{
				nFlagProcOK = CB_TRUE;
				break;
			}
		}	/* for ( nCntRetry ) */
		/* ���x�����O���s�̏ꍇ�A�����_�ʒu��������Ȃ������ꍇ�̐U�镑���ɂȂ�悤�ɂ��� */
		if ( CB_TRUE != nFlagProcOK )
		{
			ptSrchRsltInfo[ni].tCenterPntPrecise[0].w = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[0].h = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1].w = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1].h = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[0].nFlagPrecise = CB_FALSE;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1].nFlagPrecise = CB_FALSE;
			ptSrchRsltInfo[ni].nNumCenterPnt = 0L;
			continue;
		}

		/* Label���Ƃɍŏ�/�ő�x,y���W */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_ExtractLORegionX( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinX, pnTblLabelMaxX );
		nRet = implib_IP_ExtractLORegionY( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinY, pnTblLabelMaxY );

		/* Label���Ɩʐ� */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
//		nRet = implib_IP_ExtractLOArea( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblAreaLabel );
		nRet = implib_IP_ExtractLOGravity( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], ptLOGravTbl );

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																				/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
		/* Label���ƂɋP�x�l�݌v */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_SRC1_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_ProjectLabelGO( nImgSrc, pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], (int32_t*)(nBrTbl) );
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																			/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

		/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		/*
		==============================
			�����̒��o
		==============================
		*/

		/* �摜�������A�N�Z�X�J�n */
		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], &nXSizeImg, &nYSizeImg, (void**)&pnTopAddrPixel );
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		nCntCandLine = 0L;
		for ( nj = 1L; nj <= nNumLabel; nj++ )
		{
			if ( nprm_thrLineLabel <= ptLOGravTbl[nj].AREA )	/* �����Ƃ��Ĉ������x���̖ʐσ`�F�b�N */
			{
				/* �㑤�X�L���� */
				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				for ( nk = 0L; nk < nprm_scanHeightLineUL; nk++ )
				{
					/* �X�L��������Y���W�ݒ� */
					nScanY = ( ptRectRgn->nSY + pnTblLabelMinY[nj] ) + nk;
					/* X�����ɃX�L���� */
					nScanWidth = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
					pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6737
					for ( nl = 0L; nl < nScanWidth; nl++ )
					{
						if ( nj == (slong)(*pnWkAddrPixel) )
						{
							nSumX = nSumX + ( ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) + nl );
							nSumY = nSumY + nScanY;
							/* �T���v���� */
							nCntCoord++;
						}
						pnWkAddrPixel++;
					}	/* for ( nl ) */
				}	/* for ( nk ) */
				/* ��\���W */
				if ( 0L < nCntCoord )	/* 0���`�F�b�N�ǉ� */
				{
					fCoordUpper[0L] = (float_t)nSumX / (float_t)nCntCoord;
					fCoordUpper[1L] = (float_t)nSumY / (float_t)nCntCoord;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY														/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
					if ( 0L < nprm_scanHeightLineUL )	/* 0���`�F�b�N�ǉ� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6738	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6739
					{
						nWidthUpper = (slong)( nCntCoord / nprm_scanHeightLineUL );
					}
					else
					{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6740
						/* Open�����摜��������Close���� */
						nRet = implib_CloseImgDirect( nImgSrc );
						nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
						/* 0�����̏��� */
						return ( CB_IMG_NG_DIV0 );
					}
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */													/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
				}
				else
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					/* 0�����̏��� */
					return ( CB_IMG_NG_DIV0 );
				}


				/* �����X�L���� */
				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				for ( nk = 0L; nk < nprm_scanHeightLineUL; nk++ )
				{
					/* �X�L��������Y���W�ݒ� */
					nScanY = ( ptRectRgn->nSY + pnTblLabelMaxY[nj] ) - nk;
					/* X�����ɃX�L���� */
					nScanWidth = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
					pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) );
					for ( nl = 0L; nl < nScanWidth; nl++ )
					{
						if ( nj == (slong)(*pnWkAddrPixel) )
						{
							nSumX = nSumX + ( ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) + nl );
							nSumY = nSumY + nScanY;
							/* �T���v���� */
							nCntCoord++;
						}
						pnWkAddrPixel++;
					}	/* for ( nl ) */
				}	/* for ( nk ) */
				/* ��\���W */
				if ( 0L < nCntCoord )	/* 0���`�F�b�N�ǉ� */
				{
					fCoordLower[0L] = (float_t)nSumX / (float_t)nCntCoord;
					fCoordLower[1L] = (float_t)nSumY / (float_t)nCntCoord;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY														/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
					if ( 0L < nprm_scanHeightLineUL )	/* 0���`�F�b�N�ǉ� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6741	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6742
					{
						nWidthLower = (slong)( nCntCoord / nprm_scanHeightLineUL );
					}
					else
					{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6743
						/* Open�����摜��������Close���� */
						nRet = implib_CloseImgDirect( nImgSrc );
						nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
						/* 0�����̏��� */
						return ( CB_IMG_NG_DIV0 );
					}
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */													/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
				}
				else
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					/* 0�����̏��� */
					return ( CB_IMG_NG_DIV0 );
				}

				/* �����X�L���� */
				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				/* �X�L��������Y���W�ݒ� */
				nScanY = ( ptRectRgn->nSY + pnTblLabelMinY[nj] ) + ( (pnTblLabelMaxY[nj] - pnTblLabelMinY[nj]) / 2L) + 1L;
				/* X�����ɃX�L���� */
				nScanWidth = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
				pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6737
				for ( nl = 0L; nl < nScanWidth; nl++ )
				{
					if ( nj == (slong)(*pnWkAddrPixel) )
					{
						nSumX = nSumX + ( ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) + nl );
						nSumY = nSumY + nScanY;
						/* �T���v���� */
						nCntCoord++;
					}
					pnWkAddrPixel++;
				}	/* for ( nl ) */
				/* ��\���W */
				if ( 0L < nCntCoord )	/* 0���`�F�b�N�ǉ� */
				{
					fCoordMiddle[0L] = (float_t)nSumX / (float_t)nCntCoord;
					fCoordMiddle[1L] = (float_t)nSumY / (float_t)nCntCoord;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY														/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
					nWidthMiddle = nCntCoord;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */													/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
				}
				else
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					/* 0�����̏��� */
					return ( CB_IMG_NG_DIV0 );
				}

				/* ��p�ߎ�����(�X���E�ؕ�) */
				nRet = cb_FPE_CalcLineCoeff_F( &(fCoordUpper[0L]), &(fCoordMiddle[0L]), &( tUpperLineCoeff4Line.fSlope ), &( tUpperLineCoeff4Line.fIntercept ) );
				if ( CB_IMG_OK != nRet )
				{
					nRet = implib_ClearIPError();
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					return ( CB_IMG_NG );
				}

				/* ���ߎ�����(�X���E�ؕ�) */
				nRet = cb_FPE_CalcLineCoeff_F( &(fCoordMiddle[0L]), &(fCoordLower[0L]), &( tLowerLineCoeff4Line.fSlope ), &( tLowerLineCoeff4Line.fIntercept ) );
				if ( CB_IMG_OK != nRet )
				{
					nRet = implib_ClearIPError();
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					return ( CB_IMG_NG );
				}

				if ( CB_ADD_PATTERN_POS_LEFT == ni )
				{
					//if (1)// 0.0f > tLineCoeff4Line.fSlope )//�ύX
					//{
						ptLinePartInfo[nCntCandLine].nIdxLine = (uchar)nj;
						fUpperLineLength = sqrtf(   ( ( fCoordUpper[0L] - fCoordMiddle[0L] ) * ( fCoordUpper[0L] - fCoordMiddle[0L] ) )
											      + ( ( fCoordUpper[1L] - fCoordMiddle[1L] ) * ( fCoordUpper[1L] - fCoordMiddle[1L] ) ) );
						fLowerLineLength = sqrtf(   ( ( fCoordMiddle[0L] - fCoordLower[0L] ) * ( fCoordMiddle[0L] - fCoordLower[0L] ) )
											      + ( ( fCoordMiddle[1L] - fCoordLower[1L] ) * ( fCoordMiddle[1L] - fCoordLower[1L] ) ) );
						ptLinePartInfo[nCntCandLine].fLength = fUpperLineLength + fLowerLineLength;
						ptLinePartInfo[nCntCandLine].tUpperLineCoeff = tUpperLineCoeff4Line;
						ptLinePartInfo[nCntCandLine].tLowerLineCoeff = tLowerLineCoeff4Line;
						ptLinePartInfo[nCntCandLine].tUpperCenterPnt.fX = fCoordUpper[0];
						ptLinePartInfo[nCntCandLine].tUpperCenterPnt.fY = fCoordUpper[1];
						ptLinePartInfo[nCntCandLine].tLowerCenterPnt.fX = fCoordLower[0];
						ptLinePartInfo[nCntCandLine].tLowerCenterPnt.fY = fCoordLower[1];
						ptLinePartInfo[nCntCandLine].tMiddleCenterPnt.fX = fCoordMiddle[0];
						ptLinePartInfo[nCntCandLine].tMiddleCenterPnt.fY = fCoordMiddle[1];

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY												/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
						ptLinePartInfo[nCntCandLine].nWidthUpper = nWidthUpper;
						ptLinePartInfo[nCntCandLine].nWidthLower = nWidthLower;
						ptLinePartInfo[nCntCandLine].nWidthMiddle = nWidthMiddle;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */											/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

						nCntCandLine++;
					//}
				}
				else
				{
					//if (1)// 0.0f < tLineCoeff4Line.fSlope )//�ύX
					//{
						ptLinePartInfo[nCntCandLine].nIdxLine = (uchar)nj;
						fUpperLineLength = sqrtf(   ( ( fCoordUpper[0L] - fCoordMiddle[0L] ) * ( fCoordUpper[0L] - fCoordMiddle[0L] ) )
											      + ( ( fCoordUpper[1L] - fCoordMiddle[1L] ) * ( fCoordUpper[1L] - fCoordMiddle[1L] ) ) );
						fLowerLineLength = sqrtf(   ( ( fCoordMiddle[0L] - fCoordLower[0L] ) * ( fCoordMiddle[0L] - fCoordLower[0L] ) )
											      + ( ( fCoordMiddle[1L] - fCoordLower[1L] ) * ( fCoordMiddle[1L] - fCoordLower[1L] ) ) );
						ptLinePartInfo[nCntCandLine].fLength = fUpperLineLength + fLowerLineLength;
						ptLinePartInfo[nCntCandLine].tUpperLineCoeff = tUpperLineCoeff4Line;
						ptLinePartInfo[nCntCandLine].tLowerLineCoeff = tLowerLineCoeff4Line;
						ptLinePartInfo[nCntCandLine].tUpperCenterPnt.fX = fCoordUpper[0];
						ptLinePartInfo[nCntCandLine].tUpperCenterPnt.fY = fCoordUpper[1];
						ptLinePartInfo[nCntCandLine].tLowerCenterPnt.fX = fCoordLower[0];
						ptLinePartInfo[nCntCandLine].tLowerCenterPnt.fY = fCoordLower[1];
						ptLinePartInfo[nCntCandLine].tMiddleCenterPnt.fX = fCoordMiddle[0];
						ptLinePartInfo[nCntCandLine].tMiddleCenterPnt.fY = fCoordMiddle[1];

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY												/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
						ptLinePartInfo[nCntCandLine].nWidthUpper = nWidthUpper;
						ptLinePartInfo[nCntCandLine].nWidthLower = nWidthLower;
						ptLinePartInfo[nCntCandLine].nWidthMiddle = nWidthMiddle;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */											/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

						nCntCandLine++;
					//}
				}
			}
		}

		/* �摜�������A�N�Z�X�I�� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		/*
		==============================
			�ی��̒��o
		==============================
		*/
		nCntCircleLineComb = 0L;
		for ( nj = 0L; nj < nCntCandLine; nj++ )
		{
			/* Index */
			nIdxCandLine = (slong)( ptLinePartInfo[nj].nIdxLine );

			for ( nIdxUpper = 1L; nIdxUpper <= nNumLabel; nIdxUpper++ )	/* �㑤�̊� */
			{
				if ( nIdxCandLine == nIdxUpper )	/* �������x���łȂ��� */
				{
					continue;
				}
				if ( nprm_thrCircleLabel < ptLOGravTbl[nIdxUpper].AREA )	/* �傫�����̂͑Ώۂɂ��Ȃ� */
				{
					continue;
				}
				if ( pnTblLabelMinY[nIdxCandLine] <= pnTblLabelMaxY[nIdxUpper] )	/* ���̏㑤�[�_��艺���ɂ��郉�x���͖��� */
				{
					continue;
				}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
				/* ���ےT���̈�O�ɂ��郉�x���͖��� */
				if (   ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nX - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxUpper].X ) ) )
					|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nY - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxUpper].Y ) ) )
					|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nX + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxUpper].X ) ) )
					|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nY + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxUpper].Y ) ) ) )
				{
					continue;
				}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

				for ( nIdxLower = 1L; nIdxLower <= nNumLabel; nIdxLower++ )	/* �����̊� */
				{
					if ( ( nIdxCandLine == nIdxLower ) || ( nIdxUpper == nIdxLower ) )	/* �������x���łȂ��� */
					{
						continue;
					}
					if ( nprm_thrCircleLabel < ptLOGravTbl[nIdxLower].AREA )	/* �傫�����̂͑Ώۂɂ��Ȃ� */
					{
						continue;
					}
					if ( pnTblLabelMaxY[nIdxCandLine] >= pnTblLabelMinY[nIdxLower] )	/* ���̉����[�_���㑤�ɂ��郉�x���͖��� */
					{
						continue;
					}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
					/* ���ےT���̈�O�ɂ��郉�x���͖��� */
					if (   ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nX - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxLower].X ) ) )
						|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nY - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxLower].Y ) ) )
						|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nX + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxLower].X ) ) )
						|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nY + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxLower].Y ) ) ) )
					{
						continue;
					}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */

					if ( CB_ADD_PATTERN_POS_LEFT == ni )
					{
						if ( ptLOGravTbl[nIdxUpper].X <= ptLOGravTbl[nIdxLower].X )	/* X�����̈ʒu�֌W */
						{
							//continue;//�ύX
						}
					}
					else
					{
						if ( ptLOGravTbl[nIdxLower].X <= ptLOGravTbl[nIdxUpper].X )	/* X�����̈ʒu�֌W */
						{
							//continue;//�ύX
						}
					}

					/* X���W�𐄒� */
					fEstUpperX = ( ptLinePartInfo[nj].tUpperLineCoeff.fSlope * ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxUpper].Y ) ) + ptLinePartInfo[nj].tUpperLineCoeff.fIntercept;
					fEstLowerX = ( ptLinePartInfo[nj].tLowerLineCoeff.fSlope * ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxLower].Y ) ) + ptLinePartInfo[nj].tLowerLineCoeff.fIntercept;

					if (   ( fprm_thrDiffEstX < fabsf( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxUpper].X ) - fEstUpperX ) )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.8.1  R-33, ID-6744
						|| ( fprm_thrDiffEstX < fabsf( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLower].X ) - fEstLowerX ) ) )
					{
						continue;
					}

					/* �����̔�r */
					fLengthCirclePart = sqrtf( ( ( ptLOGravTbl[nIdxUpper].X - ptLOGravTbl[nIdxLower].X ) * ( ptLOGravTbl[nIdxUpper].X - ptLOGravTbl[nIdxLower].X ) )
												+ ( ( ptLOGravTbl[nIdxUpper].Y - ptLOGravTbl[nIdxLower].Y ) * ( ptLOGravTbl[nIdxUpper].Y - ptLOGravTbl[nIdxLower].Y ) ) );
					if ( ( fprm_coeffLineLength * ptLinePartInfo[nj].fLength ) < fLengthCirclePart )
					{
						continue;
					}

					if ( nCntCircleLineComb < ( 256L - 1L ) )
					{
						ptCircleLineComb[nCntCircleLineComb].nIdxUpperCircle = (uchar)nIdxUpper;
						ptCircleLineComb[nCntCircleLineComb].nIdxLowerCircle = (uchar)nIdxLower;
						ptCircleLineComb[nCntCircleLineComb].nIdxLine = (uchar)nj;
						nCntCircleLineComb++;
					}

				}	/* for ( nIdxLower ) */
			}	/* for ( nIdxUpper ) */

		}	/* for ( nj ) */

		/* �i���� */
		nIdxCircleLineComb = -1L;
		fMinDiffDistance = sqrtf( ( 720.0f * 720.0f ) + ( 480.0f * 480.0f ) );
		fMinFarDistance = sqrtf( ( 720.0f * 720.0f ) + ( 480.0f * 480.0f ) );
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY											/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
		memset( &nIdTbl, 0xFF, ( 256UL * sizeof( slong ) ) );
		memset( &fScoreTbl, 0x00, (  256UL * sizeof( float_t ) ) );
		fSumDistance = sqrtf( ( 720.0f * 720.0f ) + ( 480.0f * 480.0f ) );

		/* RRF�摜�I�[�v�� */
		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], &( tImgRRFAccessInfo.nXSize ), &( tImgRRFAccessInfo.nYSize ), (void**)&( tImgRRFAccessInfo.pnAddrY8 ) );
		if ( 0L > nRet )
		{
			/* �G���[���N���A */
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		/* �\�[�X�摜�ERRF�摜x���W�����i�[ */
		tBrInput.nImgSrcXSize = tImgSrcAccessInfo.nXSize;
		tBrInput.nImgRRFXSize = tImgRRFAccessInfo.nXSize;


#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */										/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

		for ( nj = 0L; nj < nCntCircleLineComb; nj++ )
		{
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY											/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
			/*
			==============================
				�P�x����
			==============================
			*/
			/* 0�N���A */
			fSimilarity	= 0.0F;
			fContrast	= 0.0F;

			/* �C���f�b�N�X�ݒ� */
			nIdxUpperCircle = ptCircleLineComb[nj].nIdxUpperCircle;     /* ��[�ی��C���f�b�N�X */
			nIdxLowerCircle = ptCircleLineComb[nj].nIdxLowerCircle;     /* ���[�ی��C���f�b�N�X */
			nIdxLine = ptCircleLineComb[nj].nIdxLine;                   /* �����C���f�b�N�X     */

			/* �����l�Z�o(��[) */
			fDiffXUpper = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxUpperCircle].X ) - ptLinePartInfo[nIdxLine].tUpperCenterPnt.fX;
			fDiffYUpper = ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxUpperCircle].Y ) - ptLinePartInfo[nIdxLine].tUpperCenterPnt.fY;
			/* �����l�Z�o(���[) */
			fDiffXLower = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLowerCircle].X ) - ptLinePartInfo[nIdxLine].tLowerCenterPnt.fX;
			fDiffYLower = ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxLowerCircle].Y ) - ptLinePartInfo[nIdxLine].tLowerCenterPnt.fY;

			/*--------------------------------*/
			/* �X���ݒ�                       */
			/*--------------------------------*/

			/* �X���ݒ�(��[) */
			if( CB_FPE_FLT_MIN > fabsf(fDiffXUpper) )
			{
				fSlopeUpper = (float_t) CB_FPE_MAX_SLOPE;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nFlagVertical = 1L;    /* ������Ԑݒ� */
			}
			else
			{
				fSlopeUpper = fDiffYUpper / fDiffXUpper;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nFlagVertical = 0L;    /* �񐂒���Ԑݒ� */
			}

			/* �X���ݒ�(���[) */
			if( CB_FPE_FLT_MIN > fabsf(fDiffXLower) )
			{
				fSlopeLower = (float_t) CB_FPE_MAX_SLOPE;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nFlagVertical = 1L;    /* ������Ԑݒ� */
			}
			else
			{
				fSlopeLower = fDiffYLower / fDiffXLower;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nFlagVertical = 0L;    /* �񐂒���Ԑݒ� */
			}

			/*--------------------------------*/
			/* �P�x����p���W���ݒ�         */
			/*--------------------------------*/

			fShiftXUpper = ( (float_t)pnTblLabelMaxY[nIdxUpperCircle] - ptLOGravTbl[nIdxUpperCircle].Y ) / fSlopeUpper;

			/* ���x��(�ی��)��[���S���W�ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelUpperPnt.fX = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxUpperCircle].X ) + fShiftXUpper;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelUpperPnt.fY = (float_t)( ptRectRgn->nSY + pnTblLabelMaxY[nIdxUpperCircle] );

			/* ���x��(�ی��)���[���S���W�ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelLowerPnt.fX = ptLinePartInfo[nIdxLine].tUpperCenterPnt.fX;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelLowerPnt.fY = ptLinePartInfo[nIdxLine].tUpperCenterPnt.fY;

			/* �P�x����T���̈敝�ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nSrchWidth = ptLinePartInfo[nIdxLine].nWidthUpper;

			/* �P�x����T�����C���X���ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].fSrchSlope = fSlopeUpper;


			fShiftXLower = ( ( ptLOGravTbl[nIdxLowerCircle].Y ) - (float_t)pnTblLabelMinY[nIdxLowerCircle] ) / fSlopeLower;

			/* ���x��(�ی��)��[���S���W�ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelUpperPnt.fX = ptLinePartInfo[nIdxLine].tLowerCenterPnt.fX;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelUpperPnt.fY = ptLinePartInfo[nIdxLine].tLowerCenterPnt.fY;

			/* ���x��(�����)���[���S���W�ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelLowerPnt.fX = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLowerCircle].X ) - fShiftXLower;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelLowerPnt.fY = (float_t)( ptRectRgn->nSY + pnTblLabelMinY[nIdxLowerCircle] );

			/* �P�x����T���̈敝�ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nSrchWidth = ptLinePartInfo[nIdxLine].nWidthLower;

			/* �P�x����T�����C���X���ݒ� */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].fSrchSlope = fSlopeLower;


			/* -s 20130827[QAC�w�E]�����̈�O�A�N�Z�X */
			/* �T���̈悪�����͈͊O�ɂȂ���ی��́A��₩�珜�O���� */

			/* �㉺�̒T���̈�̍ŏ�X�E�ő�X���v�Z */
			nSrchUpperMinX = (slong)( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelUpperPnt.fX + 0.5F ) - ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nSrchWidth / 2L );		// MISRA-C����̈�E [EntryAVM_QAC#4] R2.4.1  , ID-6794
			nSrchLowerMinX = (slong)( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelLowerPnt.fX + 0.5F ) - ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nSrchWidth / 2L );		// MISRA-C����̈�E [EntryAVM_QAC#4] R2.4.1  , ID-6795
			nSrchUpperMaxX = nSrchUpperMinX + ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nSrchWidth - 1L );
			nSrchLowerMaxX = nSrchLowerMinX + ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nSrchWidth - 1L );

			/* �T���̈�ŏ�X���W���v�Z */
			if( nSrchUpperMinX < nSrchLowerMinX )
			{
				nSrchMinX = nSrchUpperMinX;
			}
			else
			{
				nSrchMinX = nSrchLowerMinX;
			}

			/* �T���̈�ő�X���W���v�Z */
			if( nSrchUpperMaxX > nSrchLowerMinX )
			{
				nSrchMaxX = nSrchUpperMaxX;
			}
			else
			{
				nSrchMaxX = nSrchLowerMaxX;
			}

			/* �T�����ɏ����̈�O�ɏo��ꍇ�́A�������Ƃ��ď������΂� */
			if(		( nSrchMinX < ptRectRgn->nSX )
				||	( nSrchMaxX > ptRectRgn->nEX ) )
			{

#ifdef CB_FPE_CSW_DEBUG_ON
				if( nj <= CB_FPE_DBG_BR_RESULT_MAXNUM )
				{
					/* �㑤����ۈʒu�ɖ����Ȓl��o�^(DBG�`�掞�ɖ������Ƃ��ĕ`�悵�Ȃ��悤�ɂ���)*/
					m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX = -1L;
					/* �p�^�[����␔ */
					m_FPE_tDbgInfo.nNum[ni] ++;
				}
				else
				{
					/* Not Operation */
				}
#endif /* CB_FPE_CSW_DEBUG_ON */

				continue;
			}

			/* -e 20130827[QAC�w�E]�����̈�O�A�N�Z�X */


			/*--------------------------------*/
			/* ���̈敽�ϋP�x�l�ݒ�           */
			/*--------------------------------*/

			/* ����⃉�x���C���f�b�N�X�ݒ� */
			nIdxLinePart = (slong)( ptLinePartInfo[nIdxLine].nIdxLine );
			/* ���̈�(��[�� + �� + ���[��)�̍��v�ʐώZ�o */
			fSumBlackArea = (float_t)( ptLOGravTbl[nIdxLinePart].AREA + ptLOGravTbl[nIdxUpperCircle].AREA + ptLOGravTbl[nIdxLowerCircle].AREA );

			/* ���̈敽�ϋP�x�l�Z�o */
			if( CB_FPE_FLT_MIN > fSumBlackArea )	/* 0���`�F�b�N */
			{
				/* �����ɗ���̂͂��蓾�Ȃ��p�^�[�� */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nImgSrc );
				nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );

				return ( CB_IMG_NG_CALC_BR_SIMILARITY );
			}
			else
			{
				tBrInput.fAvgBlackArea = (float_t)( nBrTbl[nIdxLinePart] + nBrTbl[nIdxUpperCircle] + nBrTbl[nIdxLowerCircle] ) / fSumBlackArea;
			}

			/*--------------------------------*/
			/* �ގ��x�v�Z                     */
			/*--------------------------------*/

			/* �P�x�̗ގ��x�v�Z */
			nRet = cb_FPE_CalcBrightnessSimilarity( tImgSrcAccessInfo.pnAddrY8, tImgRRFAccessInfo.pnAddrY8, &tBrInput, &fSimilarity, &fContrast );		// MISRA-C����̈�E [EntryAVM_QAC#3] R1.1.1  R-30, ID-6745	// MISRA-C����̈�E [EntryAVM_QAC#3] R1.1.1  R-30, ID-6746

			if ( 0L > nRet )
			{
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nImgSrc );
				nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );
				return ( CB_IMG_NG_CALC_BR_SIMILARITY );
			}

			/*--------------------------------*/
			/* ��������                       */
			/*--------------------------------*/

			/* ���x���ԋ����Z�o(��[) */
			fDistanceUpper = sqrtf( ( fDiffXUpper * fDiffXUpper ) + ( fDiffYUpper * fDiffYUpper ) );
			/* ���x���ԋ����Z�o(���[) */
			fDistanceLower = sqrtf( ( fDiffXLower * fDiffXLower ) + ( fDiffYLower * fDiffYLower ) );

			/* ��₪�������Ă��Ȃ���� */
			if ( 0L > nIdxCircleLineComb )
			{
				/* �C���f�b�N�X�ݒ� */
				nIdxCircleLineComb = nj;    /* ���ۃy�A���C���f�b�N�X   */
				nIdTbl[nIdxLine] = nj;      /* �����x���C���f�b�N�X       */

				/* ���x���ԋ���(�㉺�[���v�l) */
				fSumDistance = fDistanceUpper + fDistanceLower;

				/* �ގ��x�X�R�A */
				fScoreTbl[nIdxLine] = fSimilarity * ( fContrast / 255.0F );
			}
			/* ��₪���łɌ������Ă����� */
			else
			{
				/***
				* �ȉ��̏����̎��A�C���f�b�N�X���X�V����
				*  �@�����������������y�A�ł͂Ȃ��ꍇ
				*  �A�����������������y�A ���� ���x���ԋ���(�㉺�[���v�l)���ŒZ�̏ꍇ
				****/
				if (   ( nIdxLine != ptCircleLineComb[nIdxCircleLineComb].nIdxLine )
					|| ( fSumDistance > ( fDistanceUpper + fDistanceLower ) ) )
				{
					/* �C���f�b�N�X�ݒ� */
					nIdxCircleLineComb = nj;    /* ���ۃy�A���C���f�b�N�X   */
					nIdTbl[nIdxLine] = nj;      /* �����x���C���f�b�N�X       */

					/* ���x���ԋ���(�㉺�[���v�l) */
					fSumDistance = fDistanceUpper + fDistanceLower;

					/* �ގ��x�X�R�A */
					fScoreTbl[nIdxLine] = fSimilarity * ( fContrast / 255.0F );
				}
				else
				{
					/* Not Operation */
				}
			}

#ifdef CB_FPE_CSW_DEBUG_ON
			if( nj < CB_FPE_DBG_BR_RESULT_MAXNUM )
			{
				/* �㑤����ۈʒu */
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX = (slong)( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ nIdxUpperCircle ].X + 0.5F );
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY = (slong)( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ nIdxUpperCircle ].Y + 0.5F );
				/* ��������ۈʒu */
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX = (slong)( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ nIdxLowerCircle ].X + 0.5F );
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY = (slong)( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ nIdxLowerCircle ].Y + 0.5F );
				/* �ގ��x�E�R���g���X�g */
				m_FPE_tDbgInfo.tSimilarity[ni][nj] = fSimilarity;
				m_FPE_tDbgInfo.tContrast[ni][nj] = fContrast / 255.0F;
				/* �p�^�[����␔ */
				m_FPE_tDbgInfo.nNum[ni] ++;
				/* �P�x����X�R�A */
				/* ( ���K�����ݑ��� ) * ( �R���g���X�g(���K��) ) */
				m_FPE_tDbgInfo.tScore[ni][nj] = fScoreTbl[nIdxLine];

			}
			else
			{
				/* Not Operation */
			}
#endif /* CB_FPE_CSW_DEBUG_ON */
		} /* nj */

		/*--------------------------------*/
		/* �ގ��x����                     */
		/*--------------------------------*/

		/* ������ */
		fMaxScore = 0.0F;

		for( nj = 0L; nj < nCntCandLine; nj++ )
		{
			if( fMaxScore < fScoreTbl[nj] )
			{
				fMaxScore = fScoreTbl[nj];          /* �X�R�A���������̂ɍX�V */
				nIdxCircleLineComb = nIdTbl[nj];    /* ���̎��̐��ۃy�A�C���f�b�N�X��ݒ� */
			}
			else
			{
				/* Not Operation */
			}
		}


		/* �r���T�����ʂ��i�[ */
		/* Index */
		nIdxCircle[0] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxUpperCircle );
		nIdxCircle[1] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxLowerCircle );

		/* �㑤����ۈʒu */
		ptSrchRsltInfo[ni].tCenterPnt[0].nX = ptRectRgn->nSX + (slong)( ptLOGravTbl[ nIdxCircle[0] ].X + 0.5f );
		ptSrchRsltInfo[ni].tCenterPnt[0].nY = ptRectRgn->nSY + (slong)( ptLOGravTbl[ nIdxCircle[0] ].Y + 0.5f );
		/* ��������ۈʒu */
		ptSrchRsltInfo[ni].tCenterPnt[1].nX = ptRectRgn->nSX + (slong)( ptLOGravTbl[ nIdxCircle[1] ].X + 0.5f );
		ptSrchRsltInfo[ni].tCenterPnt[1].nY = ptRectRgn->nSY + (slong)( ptLOGravTbl[ nIdxCircle[1] ].Y + 0.5f );


		/* RRF�摜�������A�N�Z�X�I�� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

#else		/* ��������(����) */
			fDiffX = ( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxUpperCircle ].X ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tUpperCenterPnt.fX );
			fDiffY = ( ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxUpperCircle ].Y ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tUpperCenterPnt.fY );
			fDistanceUpper = sqrtf( ( fDiffX * fDiffX ) + ( fDiffY * fDiffY ) );
			fDiffX = ( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxLowerCircle ].X ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tLowerCenterPnt.fX );
			fDiffY = ( ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxLowerCircle ].Y ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tLowerCenterPnt.fY );
			fDistanceLower = sqrtf( ( fDiffX * fDiffX ) + ( fDiffY * fDiffY ) );
			fDiffDistance = fabsf( fDistanceUpper - fDistanceLower );
			fFarDistance = ( fDistanceUpper > fDistanceLower ) ? fDistanceUpper : fDistanceLower;

			if ( 0L > nIdxCircleLineComb )	/* ��₪�������Ă��Ȃ���� */
			{
				if ( fMinDiffDistance > fDiffDistance )
				{
					nIdxCircleLineComb = nj;
					fMinDiffDistance = fDiffDistance;
					fMinFarDistance = fFarDistance;
				}
			}
			else	/* ��₪���łɌ������Ă����� */
			{
				if ( ptCircleLineComb[nj].nIdxLine != ptCircleLineComb[nIdxCircleLineComb].nIdxLine )	/* �����������������y�A�ł͂Ȃ��ꍇ */
				{
					if ( fMinDiffDistance > fDiffDistance )
					{
						nIdxCircleLineComb = nj;
						fMinDiffDistance = fDiffDistance;
						fMinFarDistance = fFarDistance;
					}
				}
				else		/* �����������������y�A�̏ꍇ */
				{
					if ( fMinFarDistance > fFarDistance )
					{
						nIdxCircleLineComb = nj;
						fMinDiffDistance = fDiffDistance;
						fMinFarDistance = fFarDistance;
					}
				}
			}
		} /* nj */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */										/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */

		/*
		==============================
			�����x�ʒu����
		==============================
		*/
		if ( 0L > nIdxCircleLineComb ) 
		{
			ptSrchRsltInfo[ni].tCenterPntPrecise[0L].w = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[0L].h = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1L].w = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1L].h = -1.0;
			ptSrchRsltInfo[ni].tCenterPntPrecise[0L].nFlagPrecise = CB_FALSE;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1L].nFlagPrecise = CB_FALSE;
			ptSrchRsltInfo[ni].nNumCenterPnt = 0L;
		}
		else
		{
			/* �摜�������A�N�Z�X���ݒ� */
			pnTopAddrPixel = (uchar*)( tImgSrcAccessInfo.pnAddrY8 );
			nXSizeImg = tImgSrcAccessInfo.nXSize;
			nYSizeImg = tImgSrcAccessInfo.nYSize;		// MISRA-C����̈�E [EntryAVM_QAC#3] R1.1.1  R-30, ID-6747

			/* Index */
			nIdxCircle[0] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxUpperCircle );
			nIdxCircle[1] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxLowerCircle );

			for ( nj = 0L; nj < CB_ADD_PATTERN_PNT_POS_MAXNUM; nj++ )
			{
				/* ���ڈʒu */
				nAttnX = ptRectRgn->nSX + (slong)( ptLOGravTbl[ nIdxCircle[nj] ].X + 0.5f );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6748
				nAttnY = ptRectRgn->nSY + (slong)( ptLOGravTbl[ nIdxCircle[nj] ].Y + 0.5f );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6749

				/* ���ڃA�h���X */
				pnWkAddrPixel = pnTopAddrPixel + ( nAttnY * nXSizeImg ) + nAttnX;		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6750

				/* ��\��f�l */
				nSumValue = (slong)(   ( *( pnWkAddrPixel - ( nXSizeImg + 1L ) ) )
									 + ( *( pnWkAddrPixel - ( nXSizeImg      ) ) )
									 + ( *( pnWkAddrPixel - ( nXSizeImg - 1L ) ) )
									 + ( *( pnWkAddrPixel - (           + 1L ) ) )
									 + ( *( pnWkAddrPixel                      ) )
									 + ( *( pnWkAddrPixel + (           + 1L ) ) )
									 + ( *( pnWkAddrPixel + ( nXSizeImg - 1L ) ) )
									 + ( *( pnWkAddrPixel + ( nXSizeImg      ) ) )
									 + ( *( pnWkAddrPixel + ( nXSizeImg + 1L ) ) ) );	// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6802
				nRepValue = (slong)( ( (float_t)nSumValue / 9.0f ) + 0.5f );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6752

				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				for ( nk = ( pnTblLabelMinY[ nIdxCircle[nj] ] - 1L ); nk <= ( pnTblLabelMaxY[ nIdxCircle[nj] ] + 1L ); nk++ )
				{
					/* �X�L��������Y���W�ݒ� */
					nScanY = ( ptRectRgn->nSY + nk );

					for ( nl = ( pnTblLabelMinX[ nIdxCircle[nj] ] - 1L ); nl <= ( pnTblLabelMaxX[ nIdxCircle[nj] ] + 1L ); nl++ )
					{
						/* �X�L��������X���W�ݒ� */
						nScanX = ( ptRectRgn->nSX + nl );
						/* �Q�ƃA�h���X */
						pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + nScanX );

						if ( ( nRepValue + nprm_diffIntensityPrecise ) >= (slong)(*pnWkAddrPixel) )
						{
							nSumX = nSumX + nScanX;
							nSumY = nSumY + nScanY;
							/* �T���v���� */
							nCntCoord++;
						}
					}	/* for ( nl ) */
				}	/* for ( nk ) */

				if ( 0L < nCntCoord )	/* ��\��f�l�Ƃ̗ގ���f�l�����݂��邩 */
				{
					dCoeffAve = 1.0 / (double_t)nCntCoord;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].w = (double_t)nSumX * dCoeffAve;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].h = (double_t)nSumY * dCoeffAve;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].nFlagPrecise = CB_TRUE;
				}
				else
				{
					/* ��\��f�l�Ƃ̗ގ���f�l�����݂��Ȃ� -> 0���蔭�� */
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].w = (double_t)ptSrchRsltInfo[ni].tCenterPnt[nj].nX;//-1.0;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].h = (double_t)ptSrchRsltInfo[ni].tCenterPnt[nj].nY;//-1.0;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].nFlagPrecise = CB_FALSE;
				}
			}	/* for ( nj ) */

			/* �_�� */
			ptSrchRsltInfo[ni].nNumCenterPnt = CB_ADD_PATTERN_PNT_POS_MAXNUM;
		}

	}	/* for ( ni ) */

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	
	/* ���o�ł������`�F�b�N */
	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		if ( 0L == ptSrchRsltInfo[ni].nNumCenterPnt )
		{
			return ( CB_IMG_NG_NO_CIRCLE_FTR_PNT );
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�~�̈�̒��_���o(�t�����g�J�����p�A�^�񒆂̐����Z���ꍇ)
 *
 * @param[in]	pnImgWk			:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[out]	ptSrchRsltInfo	:,�T�����ʏ��\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.04	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ExtractCircleRegion4FrontCAM( const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RSLT_ADD_PNT_INFO *ptSrchRsltInfo, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6753
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj, nk, nl;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nNumLabel;
	slong	nXSizeLabel, nYSizeLabel;
	slong	nCntCandPoint;
	slong	nCntCandPntUpper, nCntCandPntLower;
	slong	nFlagSearch;
	slong	nFlgAspectChk, nFlgAreaChk;
	slong	nAreaBoundRect;
	float_t	fAspectRatio;
	float_t	fLengthRatio;
	float_t	fDistanceCand1, fDistanceCand2, fDistanceRatio;
	float_t	fCosCand1, fCosCand2, fThetaCand1, fThetaCand2;
	slong	nIdxLabelUpperPnt, nIdxLabelLowerPnt;
	CB_FPE_VECTOR_2D_F	tVector[2];
	CB_IMG_POINT_F	tCandPoint[2];
	slong	*pnTblLabelMinX, *pnTblLabelMaxX, *pnTblLabelMinY, *pnTblLabelMaxY;
	slong	*pnIdxLabelCandPnt;
	CB_RECT_RGN	*ptRectRgn;
	CB_IMG_POINT	*ptCenterPnt;
	CB_IMG_POINT_F	*ptCandPoint;
	IMPLIB_IPLOGravityTbl	*ptLOGravTbl;
	/* �p�����[�^ */
	slong	nprm_cntReach;
	slong	nprm_thrminLabelArea, nprm_thrmaxLabelArea;
	slong	nprm_thrSizeLabel;
	float_t	/*fprm_aspectRatioVert, fprm_aspectRatioHori, */fprm_aspectRatio, fprm_aspectRatioInv;
	float_t	fprm_lengthRatio, fprm_lengthRatioInv;
	float_t	fprm_thrAreaRatio;
	float_t	fprm_thrDistanceRatio, fprm_thrDistanceRatioInv;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRsltInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	nprm_cntReach = CB_FPE_PRM_THR_CNT_REACH;
	nprm_thrminLabelArea = CB_FPE_PRM_THR_MIN_LABEL_AREA;
	nprm_thrmaxLabelArea = CB_FPE_PRM_THR_MAX_LABEL_AREA;
	nprm_thrSizeLabel = CB_FPE_PRM_THR_SIZE_LABEL;
//	fprm_aspectRatioVert = 0.4f;
//	fprm_aspectRatioHori = 0.3f;
	fprm_aspectRatio = CB_FPE_PRM_ASPECT_RATIO;
	fprm_aspectRatioInv = 1.0f / fprm_aspectRatio;
	fprm_lengthRatio = CB_FPE_PRM_LENGTH_RATIO;
	fprm_lengthRatioInv = 1.0f / fprm_lengthRatio;
	fprm_thrAreaRatio = CB_FPE_PRM_THR_AREA_RATIO;
	fprm_thrDistanceRatio = CB_FPE_PRM_THR_DISTANCE_RATIO;
	fprm_thrDistanceRatioInv = 1.0f / fprm_thrDistanceRatio;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �擪�|�C���^ */
	pnTblLabelMinX = &(m_FPE_tWorkInfo.nTblLabelMinX[0]);
	pnTblLabelMaxX = &(m_FPE_tWorkInfo.nTblLabelMaxX[0]);
	pnTblLabelMinY = &(m_FPE_tWorkInfo.nTblLabelMinY[0]);
	pnTblLabelMaxY = &(m_FPE_tWorkInfo.nTblLabelMaxY[0]);
	ptLOGravTbl = &(m_FPE_tWorkInfo.tTblGravity[0]);
	pnIdxLabelCandPnt = &( m_FPE_tWorkInfo.nIdxLabelCandPnt[0] );
	ptCandPoint = &( m_FPE_tWorkInfo.tCircleCandPnt[0] );

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* �����̈� */
		ptRectRgn = &(ptSrchRgnInfo[ni].tRgnImg);
		/* ���_�i�[�� */
		ptCenterPnt = &(ptSrchRsltInfo[ni].tCenterPnt[0]);
		/* �N���A */
		nCntCandPoint = 0L;
		nCntCandPntUpper = 0L;
		nCntCandPntLower = 0L;
		memset( pnTblLabelMinX, 0x00, ( 256UL * sizeof( slong ) ) );
		memset( pnTblLabelMaxX, 0x00, ( 256UL * sizeof( slong ) ) );

		/* �������l��������2�l�� */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_Binarize( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], nprm_cntReach );

		/* Labeling */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nNumLabel = implib_IP_Label8withAreaFLT( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], nprm_thrminLabelArea, nprm_thrmaxLabelArea, IMPLIB_LABEL_OBJ );

		/* Label���Ƃɍŏ�/�ő�x,y���W */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_ExtractLORegionX( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinX, pnTblLabelMaxX );
		nRet = implib_IP_ExtractLORegionY( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinY, pnTblLabelMaxY );

		/* �d�S */
		nRet = implib_IP_ExtractLOGravity( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], ptLOGravTbl );

		/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
		if ( 0L > nRet )
		{
			/* �G���[���N���A */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}

		if ( CB_FPE_CIRCLE_CAND_POINT_MAXNUM < nNumLabel )
		{
			return ( CB_IMG_NG );
		}

		/* �c������g���āA�i���� */
		for ( nj = 1L; nj <= nNumLabel; nj++ )
		{
			nXSizeLabel = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
			nYSizeLabel = ( pnTblLabelMaxY[nj] - pnTblLabelMinY[nj] ) + 1L;

			if( ( 0L == nXSizeLabel ) || ( 0L == nYSizeLabel ) )
			{
					/* 0�����̏��� *//* �����ɗ���̂͂��蓾�Ȃ����߁A�G���[*/
					return ( CB_IMG_NG_DIV0 );
			}

			/* �c���� */
			nFlgAspectChk = CB_FALSE;
			fAspectRatio = (float_t)nYSizeLabel / (float_t)nXSizeLabel;
			if (   ( ( fprm_aspectRatio <= fAspectRatio ) && ( fAspectRatio <= fprm_aspectRatioInv ) ) 
				&& ( ( nprm_thrSizeLabel <= nXSizeLabel ) && ( nprm_thrSizeLabel <= nYSizeLabel ) ) )
			{
				nFlgAspectChk = CB_TRUE;
			}
			/* �ʐϔ� */
			nAreaBoundRect = nXSizeLabel * nYSizeLabel;
			nFlgAreaChk = CB_FALSE;
			if ( fprm_thrAreaRatio <= ( (float_t)( ptLOGravTbl[nj].AREA ) / (float_t)nAreaBoundRect ) )
			{
				nFlgAreaChk = CB_TRUE;
			}

			/* �c���䂩��i���� */
			if ( ( CB_TRUE == nFlgAspectChk ) && ( CB_TRUE == nFlgAreaChk ) )
			{
				/* ���_ */
				ptCandPoint[nCntCandPoint].fX = (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nj].X;
				ptCandPoint[nCntCandPoint].fY = (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nj].Y;

				pnIdxLabelCandPnt[nCntCandPoint] = nj;
				nCntCandPoint++;
			}
		}	/* for ( nj = 1; nj <= nNumLabel; nj++ ) */

		tCandPoint[0].fX = -1.0F;
		tCandPoint[0].fY = -1.0F;
		tCandPoint[1].fX = -1.0F;
		tCandPoint[1].fY = -1.0F;

		nIdxLabelUpperPnt = -1;
		nIdxLabelLowerPnt = -1;

		nFlagSearch = CB_FALSE;
		for ( nj = 0L; nj < nCntCandPoint; nj++ )	/* ���ړ_ */
		{
			for ( nk = 0L; nk < nCntCandPoint; nk++ )	/* �Ή��_1 */
			{
				if ( nj == nk )	/* �����f�[�^���Q�Ƃ��Ȃ� */
				{
					continue;
				}

				/* ���� */
				tVector[0].fX = ( ptCandPoint[nk].fX - ptCandPoint[nj].fX );
				tVector[0].fY = ( ptCandPoint[nk].fY - ptCandPoint[nj].fY );
				fDistanceCand1 = sqrtf( ( tVector[0].fX * tVector[0].fX ) + ( tVector[0].fY * tVector[0].fY ) );

				if ( CB_FPE_FLT_MIN > fDistanceCand1 )	/* 0���`�F�b�N�ǉ� */
				{
					/* �����ɗ���̂͂��蓾�Ȃ��p�^�[�� */
					continue;
				}

				for ( nl = 0L; nl < nCntCandPoint; nl++ )	/* �Ή��_2 */
				{
					if ( ( nj == nl ) || ( nk == nl ) )	/* �����f�[�^���Q�Ƃ��Ȃ� */
					{
						continue;
					}

					/* ���� */
					tVector[1].fX = ( ptCandPoint[nl].fX - ptCandPoint[nj].fX );
					tVector[1].fY = ( ptCandPoint[nl].fY - ptCandPoint[nj].fY );
					fDistanceCand2 = sqrtf( ( tVector[1].fX * tVector[1].fX ) + ( tVector[1].fY * tVector[1].fY ) );

					if ( CB_FPE_FLT_MIN > fDistanceCand2 )	/* 0���`�F�b�N�ǉ� */
					{
						/* �����ɗ���̂͂��蓾�Ȃ��p�^�[�� */
						continue;
					}

					fDistanceRatio = fDistanceCand2 / fDistanceCand1;
					if ( ( fprm_thrDistanceRatio > fDistanceRatio ) || ( fDistanceRatio > fprm_thrDistanceRatioInv ) )
					{
						continue;
					}
				
					/* tVector[0]�AtVector[1]��X�����܂���Y�����̂��Âꂩ��0 */ /* 0���`�F�b�N */
					if (   ( CB_FPE_FLT_MIN >= fabsf( tVector[0].fX ) ) || ( CB_FPE_FLT_MIN >= fabsf( tVector[0].fY ) )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.8.1  R-33, ID-6758
						|| ( CB_FPE_FLT_MIN >= fabsf( tVector[1].fX ) ) || ( CB_FPE_FLT_MIN >= fabsf( tVector[1].fY ) ) )
					{
						continue;
					}
					
					/* X������������ */
					if (   ( ( tVector[0].fX < CB_FPE_FLT_MIN ) && ( tVector[1].fX < CB_FPE_FLT_MIN ) )
						|| ( ( tVector[0].fX > CB_FPE_FLT_MIN ) && ( tVector[1].fX > CB_FPE_FLT_MIN ) ) )
					{
						continue;
					}
					/* Y������������ */
					if (   ( ( tVector[0].fY < CB_FPE_FLT_MIN ) && ( tVector[1].fY < CB_FPE_FLT_MIN ) )
						|| ( ( tVector[0].fY > CB_FPE_FLT_MIN ) && ( tVector[1].fY > CB_FPE_FLT_MIN ) ) )
					{
						continue;
					}

					if ( CB_ADD_PATTERN_POS_LEFT == ni )
					{
						if (   ( ( tVector[0].fX < CB_FPE_FLT_MIN ) && ( tVector[0].fY < CB_FPE_FLT_MIN ) )
							|| ( ( tVector[0].fX > CB_FPE_FLT_MIN ) && ( tVector[0].fY > CB_FPE_FLT_MIN ) ) )
						{
							continue;
						}
					}
					else
					{
						if (   ( ( tVector[0].fX < CB_FPE_FLT_MIN ) && ( tVector[0].fY > CB_FPE_FLT_MIN ) )
							|| ( ( tVector[0].fX > CB_FPE_FLT_MIN ) && ( tVector[0].fY < CB_FPE_FLT_MIN ) ) )
						{
							continue;
						}
					}

					/* X�����̔� */
					fLengthRatio = (float_t)fabsf( tVector[1].fX ) / (float_t)fabsf( tVector[0].fX );

					/* X�����̔���������l���� */
					if ( ( fLengthRatio < fprm_lengthRatio ) || ( fprm_lengthRatioInv < fLengthRatio ) )
					{
						continue;
					}

					/* Y�����̔� */
					fLengthRatio = (float_t)fabsf( tVector[1].fY ) / (float_t)fabsf( tVector[0].fY );

					/* Y�����̔���������l���� */
					if ( ( fLengthRatio < fprm_lengthRatio ) || ( fprm_lengthRatioInv < fLengthRatio ) )
					{
						continue;
					}

					/* �p�x�ɂ���r */
					fCosCand1 = fabsf( tVector[0].fX ) / fDistanceCand1;
					fThetaCand1 = CB_FPE_CONVERT_RAD2DEG_F( acosf( fCosCand1 ) );
					fCosCand2 = fabsf( tVector[1].fX ) / fDistanceCand2;
					fThetaCand2 = CB_FPE_CONVERT_RAD2DEG_F( acosf( fCosCand2 ) );
					if ( 10.0F < fabsf( fThetaCand1 - fThetaCand2 ) )
					{
						continue;
					}

					/* �T������ */
					nFlagSearch = CB_TRUE;
					if ( ptCandPoint[nk].fY < ptCandPoint[nl].fY )
					{
						tCandPoint[0] = ptCandPoint[nk];
						tCandPoint[1] = ptCandPoint[nl];
						nIdxLabelUpperPnt = pnIdxLabelCandPnt[nk];
						nIdxLabelLowerPnt = pnIdxLabelCandPnt[nl];
						nCntCandPntUpper = 1L;
						nCntCandPntLower = 1L;
					}
					else
					{
						tCandPoint[0] = ptCandPoint[nl];
						tCandPoint[1] = ptCandPoint[nk];
						nIdxLabelUpperPnt = pnIdxLabelCandPnt[nl];
						nIdxLabelLowerPnt = pnIdxLabelCandPnt[nk];
						nCntCandPntUpper = 1L;
						nCntCandPntLower = 1L;
					}

					break;
				}

				if ( CB_TRUE == nFlagSearch )	/* �T�������Ȃ甲���� */
				{
					break;
				}
			}

			if ( CB_TRUE == nFlagSearch )	/* �T�������Ȃ甲���� */
			{
				break;
			}
		}

		/* �T�����ʂ��i�[ */
//		ptCandPoint[0] = tCandPoint[0];
//		ptCandPoint[1] = tCandPoint[1];
		ptCenterPnt[0].nX = (slong)( tCandPoint[0].fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6759
		ptCenterPnt[0].nY = (slong)( tCandPoint[0].fY + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6760
		ptCenterPnt[1].nX = (slong)( tCandPoint[1].fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6761
		ptCenterPnt[1].nY = (slong)( tCandPoint[1].fY + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6762
		
		/* ��␔�i�[ */
		ptSrchRsltInfo[ni].nNumCenterPnt = nCntCandPntUpper + nCntCandPntLower;

		/* �����x�� */
		if ( 0L < nIdxLabelUpperPnt )
		{
			ptSrchRsltInfo[ni].tCenterPntPrecise[0].w = (double_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLabelUpperPnt].X;
			ptSrchRsltInfo[ni].tCenterPntPrecise[0].h = (double_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxLabelUpperPnt].Y;
		}
		if ( 0L < nIdxLabelLowerPnt )
		{
			ptSrchRsltInfo[ni].tCenterPntPrecise[1].w = (double_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLabelLowerPnt].X;
			ptSrchRsltInfo[ni].tCenterPntPrecise[1].h = (double_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxLabelLowerPnt].Y;
		}
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Haar-like�����ʂ̌v�Z(�s���^�C�v)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgn		:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.17	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_HaarLikeChkEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_RECT_RGN* const ptSrchRgn )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �擪�|�C���^ */
		/* ����̃u���b�N */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6767
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6812
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6814
		/* �E��̃u���b�N */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6772
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6773
		/* �����̃u���b�N */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̃u���b�N */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6774
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6775
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6776
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6777
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �e�u���b�N���̋P�x�l���v */
			nSumBlockLU = ( ( (*pnWkSumRL_BlockLU) - (*pnWkSumRU_BlockLU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU);
			nSumBlockRU = ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockRU) ) + (*pnWkSumLU_BlockRU);
			nSumBlockLL = ( ( (*pnWkSumRL_BlockLL) - (*pnWkSumRU_BlockLL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL);
			nSumBlockRL = ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockRL) ) + (*pnWkSumLU_BlockRL);

			/* ������ */
			*pnWkFtr = ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL );

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;
			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Haar-like�����ʂ̌v�Z(�c�G�b�W�^�C�v)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgn		:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.06	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_HaarLikeVertEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_RECT_RGN* const ptSrchRgn )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �擪�|�C���^ */
		/* ���̃u���b�N */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6790
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6823
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6825
		/* �E�̃u���b�N */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6795
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6796

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �e�u���b�N���̋P�x�l���v */
			nSumBlockLU = ( ( (*pnWkSumRL_BlockLU) - (*pnWkSumRU_BlockLU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU);
			nSumBlockRU = ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockRU) ) + (*pnWkSumLU_BlockRU);

			/* ������ */
			*pnWkFtr = nSumBlockLU - nSumBlockRU;

			/* �|�C���^��i�߂� */
			/* �� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Haar-like�����ʂ̌v�Z(���G�b�W�^�C�v)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgn		:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.09	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_HaarLikeHoriEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_RECT_RGN* const ptSrchRgn )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockU, nSumBlockL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockU, *pnWkSumRU_BlockU, *pnWkSumLL_BlockU, *pnWkSumRL_BlockU;
	slong	*pnWkSumLU_BlockL, *pnWkSumRU_BlockL, *pnWkSumLL_BlockL, *pnWkSumRL_BlockL;
	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_YSIZE;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �擪�|�C���^ */
		/* ��̃u���b�N */
		pnWkSumRL_BlockU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6805
		pnWkSumLL_BlockU = pnWkSumRL_BlockU - nprm_xsizeBlockW;									// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6830
		pnWkSumRU_BlockU = pnWkSumRL_BlockU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockU = pnWkSumRU_BlockU - nprm_xsizeBlockW;									// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6832
		/* ���̃u���b�N */
		pnWkSumRL_BlockL = pnWkSumRL_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLL_BlockL = pnWkSumLL_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );				// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6810
		pnWkSumRU_BlockL = pnWkSumRU_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockL = pnWkSumLU_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );				// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6811

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �e�u���b�N���̋P�x�l���v */
			nSumBlockU = ( ( (*pnWkSumRL_BlockU) - (*pnWkSumRU_BlockU) ) - (*pnWkSumLL_BlockU) ) + (*pnWkSumLU_BlockU);
			nSumBlockL = ( ( (*pnWkSumRL_BlockL) - (*pnWkSumRU_BlockL) ) - (*pnWkSumLL_BlockL) ) + (*pnWkSumLU_BlockL);

			/* ������ */
			*pnWkFtr = nSumBlockU - nSumBlockL;

			/* �|�C���^��i�߂� */
			/* �� */
			pnWkSumRL_BlockU++;
			pnWkSumLL_BlockU++;
			pnWkSumRU_BlockU++;
			pnWkSumLU_BlockU++;
			/* �E */
			pnWkSumRL_BlockL++;
			pnWkSumLL_BlockL++;
			pnWkSumRU_BlockL++;
			pnWkSumLU_BlockL++;
			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�\���_(���)�̒T��
 *
 * @param[in]	ptHaarLike			:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCrossPnt			:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[out]	pnNumCrossPnt		:,�摜���W(�\���ʒu)�\���̂̐��ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgn			:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nMaxNumPnt			:,�T���Ώۂ̍ő�_��,1<=value<=3,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]	nThrHaarFtrCrossPnt	:,�s���^�[�Q�b�g�����̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l,-,[-]
 * @param[in]	ptPatternInfo		:,�s���^�[�Q�b�g�̓����p�^�[�����,-,[-]
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.17	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SearchCandCrossPnt( const IMPLIB_IMGID* const pnImgWk, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, CB_IMG_POINT* ptCrossPnt, slong* pnNumCrossPnt, const CB_RECT_RGN* const ptSrchRgn,
										slong nMaxNumPnt, slong nCamDirection, slong nThrHaarFtrCrossPnt, const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, uchar nDoubleType)
{
	slong	nRet = CB_IMG_NG;
	slong	ni = 0L, 
			nj = 0L;
	slong	nSxSrchRgn = 0L, 
			nSySrchRgn = 0L, 
			nExSrchRgn = 0L, 
			nEySrchRgn = 0L;
	slong	nXSizeSrchRgn = 0L, 
			nYSizeSrchRgn = 0L;
	slong	nNumPnt = 0L;
	slong	nScanSX, nScanEX, nScanSY, nScanEY;
	CB_FPE_CAND_PNT_INFO	tPntAry[3];	/* �{�֐����ɏ����ݒ肪���邽�߁A�����ł͏��������Ȃ� */
	slong	*pnWkFtrAry[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	slong	nSignHaarLikeFeature = 0L;
	slong	nSignHaarLikeFeature2 = 0L;
	enum enum_CB_IMG_ALGORITHM				nTypeAlgorithm;
	/* �p�����[�^ */
	slong	nprm_xSizePeri = 0L, 
			nprm_ySizePeri = 0L;
	slong	nprm_distancePnt = 0L;
//	slong	nprm_thrHaarFtrCrossPnt = 0L;
	slong	nprm_HaarLabelMinArea = 0L;
	slong	nprm_HaarLabelMaxArea = 0L;
	slong	nprm_HaarLabelMaxID = 0L;
	slong	nprm_SrchWidthUL = 0L;
	float_t	fprm_CoeffHaarThr = 0.0F;
	float_t	fprm_CoeffNormHaarFtr = 0.0F;


	schar	*pnAddrImg;
	schar	*pnWkNormAry = NULL;
	schar	*pnWkLabelAry = NULL;
	slong	nSuccessCnt = 0L;
	slong	*pnTblLabelMinX, *pnTblLabelMaxX, *pnTblLabelMinY, *pnTblLabelMaxY;
	slong	nXSizeLabelSrchRgn = 0L,
			nYSizeLabelSrchRgn = 0L;
	slong	nXSizeImg, nYSizeImg;
	slong	nCnt = 0L;
	slong	nNumLabel = 0L;
	slong	nThrHaarFtr = 0L,
			nThrHaarFtr_Min = 0L,
			nThrHaarFtr_Max = 0L;

	float_t	fMaxHaarLikeFeature = 0.0F;
	CB_FPE_CAND_PNT_INFO	tHaarLabelAry[CB_FPE_PRM_HAAR_LABEL_MAX_NUM + 1L];	/* �{�֐����ɏ����ݒ肪���邽�߁A�����ł͏��������Ȃ� */
	CB_FPE_CAND_PNT_INFO	tTempAry;


	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk ) || ( NULL == ptHaarLike ) || ( NULL == ptCrossPnt ) || ( NULL == pnNumCrossPnt ) || ( NULL == ptSrchRgn ) || ( NULL == ptPatternInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_xSizePeri = 1L;
	nprm_ySizePeri = 1L;
	nprm_distancePnt = CB_FPE_PRM_MERGE_DISTANCE_CAND_CROSS_PNT;
//	nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_FTR_CROSS_PNT;
	nprm_HaarLabelMinArea = CB_FPE_PRM_THR_HAAR_MIN_LABEL_AREA;
	nprm_HaarLabelMaxArea = CB_FPE_PRM_THR_HAAR_MAX_LABEL_AREA;
	nprm_HaarLabelMaxID = CB_FPE_PRM_HAAR_LABEL_MAX_NUM + 1L;
	nprm_SrchWidthUL = CB_FPE_PRM_SRCH_WIDTH_CHECK_HAAR_UL;

	fprm_CoeffHaarThr = CB_FPE_PRM_COEFF_THR_HAAR_CROSS_FTR;
	/* �s��Haar-Like�̍ő�lor�ŏ��l�́A[ 255 * ( 1�u���b�N�̖ʐ� ) * 2 ] �Ȃ̂�	*/
	/* ���̒l��-128�`127(�����t��8bit)�͈̔͂ɐ��K�����邽�߁A						*/
	/* [ ( 1�u���b�N�̖ʐ� ) * 2 * 2 ] �ŏ��Z����B									*/
	fprm_CoeffNormHaarFtr = (float_t)( CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE * CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE * 2L * 2L );

	/* �ُ�l�`�F�b�N( 0���������́AHaar�̌W�������̂Ƃ� )	*/
	/* �ʏ�͋N���肦�Ȃ��p�^�[���Ȃ̂ŁANG��Ԃ�			*/
	if ( CB_FPE_DBL_MIN <= fprm_CoeffNormHaarFtr )
	{
		/* No Operation */
	}
	else
	{
		/* 0�����̏��� */
		/* ���蓾�Ȃ��p�^�[���Ȃ̂�NG��Ԃ� */
		return ( CB_IMG_NG );
	}


	/* �擪�|�C���^ */
	pnTblLabelMinX = &(m_FPE_tWorkInfo.nTblLabelMinX[0]);
	pnTblLabelMaxX = &(m_FPE_tWorkInfo.nTblLabelMaxX[0]);
	pnTblLabelMinY = &(m_FPE_tWorkInfo.nTblLabelMinY[0]);
	pnTblLabelMaxY = &(m_FPE_tWorkInfo.nTblLabelMaxY[0]);

	/* �T���̈� */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	for( ni = 0L ; ni < (slong)( sizeof( tPntAry ) / sizeof( CB_FPE_CAND_PNT_INFO ) ) ; ni++) {
		tPntAry[ni].nFtrValue = 0L;
		tPntAry[ni].tPnt.nX = -1L;
		tPntAry[ni].tPnt.nY = -1L;
	}

	for( ni = 0L ; ni < (slong)( sizeof( tHaarLabelAry ) / sizeof( CB_FPE_CAND_PNT_INFO ) ) ; ni++) {
		tHaarLabelAry[ni].nFtrValue = 0L;
		tHaarLabelAry[ni].tPnt.nX = -1L;
		tHaarLabelAry[ni].tPnt.nY = -1L;
	}

	tTempAry.nFtrValue = 0L;
	tTempAry.tPnt.nX = -1L;
	tTempAry.tPnt.nY = -1L;


	/* �d�ݕt��Haar-like�����ʕ����̎擾 */
	nRet = cb_FPE_GetSignHaarLikeFeature( ptPatternInfo, nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		return ( nRet );
	}


	if( nDoubleType == 0U )
	{
		/********************************************
		 *�@�L�����u�}�[�J�����݂���Ă��Ȃ��ꍇ	*
		 *�@�ˊ������������s						*
		 ********************************************/
		/* �s���̐^�񒆂̓_��T�� */
		for ( ni = nprm_ySizePeri; ni < ( nYSizeSrchRgn - nprm_ySizePeri ); ni++ )
		{
			/* �擪�|�C���^ */
			pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ( ( nSySrchRgn + ni ) * ptHaarLike->nXSize ) + ( nSxSrchRgn + nprm_xSizePeri ) );

			for ( nj = nprm_xSizePeri; nj < ( nXSizeSrchRgn - nprm_xSizePeri ); nj++ )
			{
#if 0
				if ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection )
				{
	//				if ( (*(pnWkFtrAry[0])) > tPntAry[0].nFtrValue )
					if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature ) > tPntAry[0].nFtrValue )
					{
						tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
						tPntAry[0].tPnt.nX = nSxSrchRgn + nj;
						tPntAry[0].tPnt.nY = nSySrchRgn + ni;
					}

				}
				else
				{
					if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature ) < tPntAry[0].nFtrValue )
					{
						tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
						tPntAry[0].tPnt.nX = nSxSrchRgn + nj;
						tPntAry[0].tPnt.nY = nSySrchRgn + ni;
					}
				}
#endif
				if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature ) > tPntAry[0].nFtrValue )
				{
					tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
					tPntAry[0].tPnt.nX = nSxSrchRgn + nj;
					tPntAry[0].tPnt.nY = nSySrchRgn + ni;
				}

				(pnWkFtrAry[0])++;
			}
		}

		/* �M���x�`�F�b�N */
		if ( labs( tPntAry[0].nFtrValue ) < nThrHaarFtrCrossPnt )
		{
			tPntAry[0].tPnt.nX = -1L;
			tPntAry[0].tPnt.nY = -1L;
		}
	}
	else
	{
		/********************************************
		 *�@�L�����u�}�[�J�����݂���Ă���ꍇ		*
		 *�@�ːV�K����(�����_�Ή�)�����s			*
		 ********************************************/

		/*----------------------------------*/
		/* �@������							*/
		/*----------------------------------*/

		/* �摜��������^�C�v��SIGN8_DATA�ɕύX */
		nRet = implib_SetIPDataType( IMPLIB_SIGN8_DATA );
		/* ��ʃf�[�^�^�C�v�������I�ɐݒ� */
		nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], IMPLIB_SIGN8_DATA );
		nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], IMPLIB_SIGN8_DATA );

		/* �}�X�N�p2�l�摜���I�[�v�� */
		nRet = implib_IP_Const( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], 0L );
		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], &( nXSizeImg ), &( nYSizeImg ), (void**)&( pnAddrImg ) );
		if ( 0L > nRet )
		{
			/* �G���[���N���A */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}

		/*----------------------------------*/
		/* �A�\�����̈�̃��x�����O		*/
		/*----------------------------------*/

		/* �s���̐^�񒆂̓_��T�� */
		/* ���s���āAHaar-Like�����ʂ̐��K�����s���Awork�摜�Ɋi�[ */
		for ( ni = nprm_ySizePeri; ni < ( nYSizeSrchRgn - nprm_ySizePeri ); ni++ )
		{
			/* �擪�|�C���^ */
			pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ( ( nSySrchRgn + ni ) * ptHaarLike->nXSize ) + ( nSxSrchRgn + nprm_xSizePeri ) );
			pnWkNormAry = pnAddrImg + ( ( ( nSySrchRgn + ni ) * nXSizeImg ) + ( nSxSrchRgn + nprm_xSizePeri ) );

			for ( nj = nprm_xSizePeri; nj < ( nXSizeSrchRgn - nprm_xSizePeri ); nj++ )
			{
				if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature ) > tPntAry[0].nFtrValue )
				{
					fMaxHaarLikeFeature = (float_t)( *(pnWkFtrAry[0]) );
					tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
					tPntAry[0].tPnt.nX = nSxSrchRgn + nj;
					tPntAry[0].tPnt.nY = nSySrchRgn + ni;
				}
				/* Define����AHaar-Like�����ʂ�-127�`127�͈̔͂ɕ����� */
				*(pnWkNormAry) = (schar)( (float_t)( *(pnWkFtrAry[0L]) ) / fprm_CoeffNormHaarFtr );

				(pnWkFtrAry[0])++;
				(pnWkNormAry)++;
			}
		}

		/* �摜��Close */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE] );

		/* �����̈���̍ō�or�Œ�Haar-Like�����ʂ���A2�l�摜�쐬�p��臒l���v�Z */
		nThrHaarFtr = (slong)( ( fMaxHaarLikeFeature / fprm_CoeffNormHaarFtr ) * fprm_CoeffHaarThr );

		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );

		/* Haar�̕����ɉ����āA臒l��ݒ� */
		if( nSignHaarLikeFeature == CB_IMG_CHECKBOARD_SIGN_PLUS )
		{
			nThrHaarFtr_Min = nThrHaarFtr;
			nThrHaarFtr_Max = 127L;
		}
		else if( nSignHaarLikeFeature == CB_IMG_CHECKBOARD_SIGN_MINUS )
		{
			nThrHaarFtr_Min = -128L;
			nThrHaarFtr_Max = nThrHaarFtr;
		}
		else
		{
			/* �{���͂��肦�Ȃ��p�X */
			return ( CB_IMG_NG );
		}

		/* ���K������Haar-Like�摜(�����t��8bit)���A�Z�o����臒l��2�l�摜������ */
		nRet = IP_BinarizeExt( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE],  pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], nThrHaarFtr_Min, nThrHaarFtr_Max, 0 );
		if ( 0L > nRet )
		{
			/* �G���[���N���A */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}


		/* �摜��������^�C�v��UNSIGN8_DATA�ɕύX */
		nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
		nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], IMPLIB_UNSIGN8_DATA );

		/* �Q�l�摜�����x�����O(�ʐϏ��Ƀ\�[�g) */
		nNumLabel = implib_IP_Label8withAreaFLTSort( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], pnImgWk[CB_FPE_IMG_HAAR_LABELING], nprm_HaarLabelMinArea, nprm_HaarLabelMaxArea, IMPLIB_LABEL_OBJ, IMPLIB_LABEL_SORT_MAX );
		if ( 0L > nNumLabel )
		{
			/* �G���[���N���A */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}

		/*----------------------------------*/
		/* �B�e���x���̑�\�_�Z�o			*/
		/*----------------------------------*/

		/* ���x�����ɁA�O�ڋ�`X,Y���W���擾 */
		nRet = implib_IP_ExtractLORegionX( pnImgWk[CB_FPE_IMG_HAAR_LABELING], pnTblLabelMinX, pnTblLabelMaxX );
		nRet = implib_IP_ExtractLORegionY( pnImgWk[CB_FPE_IMG_HAAR_LABELING], pnTblLabelMinY, pnTblLabelMaxY );

		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_HAAR_LABELING], &( nXSizeImg ), &( nYSizeImg ), (void**)&( pnAddrImg ) );


		/* ���x�����Ń��[�v */
		for( nCnt = 1L; nCnt < nprm_HaarLabelMaxID ; nCnt++ )
		{
			nXSizeLabelSrchRgn = ( pnTblLabelMaxX[nCnt] - pnTblLabelMinX[nCnt] ) + 1L;
			nYSizeLabelSrchRgn = ( pnTblLabelMaxY[nCnt] - pnTblLabelMinY[nCnt] ) + 1L;

			/* ���Y���x���ŁAHaar-Like�����ʂ̍ł�����or�Ⴂ�_��T�� */
			for ( ni = 0L; ni < nYSizeLabelSrchRgn; ni++ )
			{
				/* Haar-Like������/���x���摜�̐擪�|�C���^ */
				pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ( ( nSySrchRgn + pnTblLabelMinY[nCnt] + ni ) * ptHaarLike->nXSize ) + ( nSxSrchRgn + pnTblLabelMinX[nCnt] + nprm_xSizePeri ) );
				pnWkLabelAry = pnAddrImg + ( ( ( nSySrchRgn + pnTblLabelMinY[nCnt] + ni ) * ptHaarLike->nXSize ) + ( nSxSrchRgn + pnTblLabelMinX[nCnt] + nprm_xSizePeri ) );

				for ( nj = nprm_xSizePeri; nj < ( nXSizeLabelSrchRgn - nprm_xSizePeri ); nj++ )
				{
					if(		( *(pnWkLabelAry) != 0L )
						&&	( *(pnWkLabelAry) < nprm_HaarLabelMaxID )
						&&	( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature ) > tHaarLabelAry[nCnt].nFtrValue ) )
					{
						tHaarLabelAry[nCnt].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
						tHaarLabelAry[nCnt].tPnt.nX = ( nSxSrchRgn + pnTblLabelMinX[nCnt] + nj );
						tHaarLabelAry[nCnt].tPnt.nY = ( nSySrchRgn + pnTblLabelMinY[nCnt] + ni );
					}
					(pnWkFtrAry[0])++;
					(pnWkLabelAry)++;
				}
			}
		}

		/* �摜��Close */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_HAAR_LABELING] );

		/* HaarLike�����ʂ��������ɔz����o�u���\�[�g */
		for( ni = 1L; ni < ( nprm_HaarLabelMaxID - 1L ) ; ni++ )
		{
			for ( nj = ( nprm_HaarLabelMaxID - 1L ) ; nj > ni; nj-- )
			{
				if ( tHaarLabelAry[nj - 1L].nFtrValue < tHaarLabelAry[nj].nFtrValue )
				{
					tTempAry = tHaarLabelAry[nj];
					tHaarLabelAry[nj] = tHaarLabelAry[nj - 1L];
					tHaarLabelAry[nj - 1L] = tTempAry;
				}
			}
		}


		/*--------------------------------------------------*/
		/* �C��\�_�ɑ΂��āA���E��T�����錟�菈��			*/
		/*--------------------------------------------------*/

		/* ���x�����Ń��[�v */
		for( nCnt = 1L; nCnt < ( nNumLabel+ 1L ) ; nCnt++ )
		{
			/* ���x�����̌��_���A��l�Ƃ��Ċi�[ */
			tPntAry[0].nFtrValue = tHaarLabelAry[nCnt].nFtrValue;
			tPntAry[0].tPnt.nX = tHaarLabelAry[nCnt].tPnt.nX;
			tPntAry[0].tPnt.nY = tHaarLabelAry[nCnt].tPnt.nY;

			/* �M���x�`�F�b�N */
			if ( labs( tPntAry[0].nFtrValue ) < nThrHaarFtrCrossPnt )
			{
				tPntAry[0].tPnt.nX = -1L;
				tPntAry[0].tPnt.nY = -1L;
			}
			else
			{
				nSuccessCnt++;
			}

			if ( 1L < nMaxNumPnt )
			{
				if ( 3L == nMaxNumPnt )
				{

					/* ���� */
					tPntAry[1].nFtrValue = 0L;
					tPntAry[1].tPnt.nX = -1L;
					tPntAry[1].tPnt.nY = -1L;

					nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

					if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
					{
						for ( ni = ( tPntAry[0].tPnt.nY - nprm_SrchWidthUL ); ni <= ( tPntAry[0].tPnt.nY + nprm_SrchWidthUL ); ni++ )
						{
							/* �擪�|�C���^ */
							pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ni * ptHaarLike->nXSize ) + nSxSrchRgn;

							for ( nj = nSxSrchRgn; nj < tPntAry[0].tPnt.nX; nj++ )
							{

								if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature2) > tPntAry[1].nFtrValue )
								{
									tPntAry[1].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
									tPntAry[1].tPnt.nX = nj;
									tPntAry[1].tPnt.nY = ni;
								}

								(pnWkFtrAry[0])++;
							}
						}

						/* �M���x�`�F�b�N */
						if ( labs( tPntAry[1].nFtrValue ) < labs( (slong)(fMaxHaarLikeFeature * fprm_CoeffHaarThr) ) )
						{
							tPntAry[1].nFtrValue = 0L;
							tPntAry[1].tPnt.nX = -1L;
							tPntAry[1].tPnt.nY = -1L;
						}
						else
						{
							nSuccessCnt++;
						}
					}	/* if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) ) */


					/* �E�� */
					tPntAry[2].nFtrValue = 0L;
					tPntAry[2].tPnt.nX = -1L;
					tPntAry[2].tPnt.nY = -1L;

					if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
					{
						for ( ni = ( tPntAry[0].tPnt.nY - nprm_SrchWidthUL ); ni <= ( tPntAry[0].tPnt.nY + nprm_SrchWidthUL ); ni++ )
						{
							/* �擪�|�C���^ */
							pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ni * ptHaarLike->nXSize ) + ( tPntAry[0].tPnt.nX + 1L );

							for ( nj = ( tPntAry[0].tPnt.nX + 1L ); nj <= nExSrchRgn; nj++ )
							{

								if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature2) > tPntAry[2].nFtrValue )
								{
									tPntAry[2].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
									tPntAry[2].tPnt.nX = nj;
									tPntAry[2].tPnt.nY = ni;
								}

								(pnWkFtrAry[0])++;
							}
							
						}


						/* �M���x�`�F�b�N */
						if ( labs( tPntAry[2].nFtrValue ) < labs( (slong)(fMaxHaarLikeFeature * fprm_CoeffHaarThr) ) )
						{
							tPntAry[2].nFtrValue = 0L;
							tPntAry[2].tPnt.nX = -1L;
							tPntAry[2].tPnt.nY = -1L;
						}
						else
						{
							nSuccessCnt++;
						}
					}	/* if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) ) */

					/* �����_���ő吔�T���ł�����(���T�������Ǝv����)�ꍇ�́ABreak���ď����𔲂��� */
					if( nSuccessCnt == nMaxNumPnt )
					{
						break;
					}
					else
					{
						/* �N���A���Ď��̃��x���� */
						/* ���W/�p�����[�^ */
						tPntAry[0].nFtrValue = 0L;
						tPntAry[0].tPnt.nX = -1L;
						tPntAry[0].tPnt.nY = -1L;
						tPntAry[1].nFtrValue = 0L;
						tPntAry[1].tPnt.nX = -1L;
						tPntAry[1].tPnt.nY = -1L;
						tPntAry[2].nFtrValue = 0L;
						tPntAry[2].tPnt.nX = -1L;
						tPntAry[2].tPnt.nY = -1L;

						/* �J�E���^ */
						nNumPnt = 0L;
						nSuccessCnt = 0L;
					}

				}
				else if ( 2L == nMaxNumPnt )
				{
					if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
					{
						/* �A���S���Y���^�C�v */
						nRet = cb_FPE_GetTypeAlgorithm( ptPatternInfo, &nTypeAlgorithm );
						if ( CB_IMG_OK != nRet )
						{
							return ( nRet );
						}
						
						if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
						{
							nScanSX = tPntAry[0].tPnt.nX + 1L;
							nScanEX = nExSrchRgn;
						}
						else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
						{
							nScanSX = nSxSrchRgn;
							nScanEX = tPntAry[0].tPnt.nX - 1L;
						}
						else
						{
							return ( CB_IMG_NG );
						}
						nScanSY = ( tPntAry[0].tPnt.nY - nprm_SrchWidthUL );
						nScanEY = ( tPntAry[0].tPnt.nY + nprm_SrchWidthUL );

						/* �����ݒ� */
						nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

						/* �����l */
						tPntAry[1].nFtrValue = 0L;
						tPntAry[1].tPnt.nX = -1L;
						tPntAry[1].tPnt.nY = -1L;

						for ( ni = nScanSY; ni <= nScanEY; ni++ )
						{
							/* �擪�|�C���^ */
							pnWkFtrAry[0] = ptHaarLike->pnFtrValue + ( ( ni * ptHaarLike->nXSize ) + nScanSX );

							for ( nj = nScanSX; nj <= nScanEX; nj++ )
							{
								if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2 ) > tPntAry[1].nFtrValue )
								{
									tPntAry[1].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
									tPntAry[1].tPnt.nX = nj;
									tPntAry[1].tPnt.nY = ni;
								}

								/* ���̃A�N�Z�X��� */
								(pnWkFtrAry[0])++;
							}	/* for ( nj ) */
						}	/* for ( ni ) */

						/* �M���x�`�F�b�N */
						if ( labs( tPntAry[1].nFtrValue ) < labs( (slong)(fMaxHaarLikeFeature * fprm_CoeffHaarThr) ) )
						{
							tPntAry[1].nFtrValue = 0L;
							tPntAry[1].tPnt.nX = -1L;
							tPntAry[1].tPnt.nX = -1L;
						}
						else
						{
							nSuccessCnt++;
						}
					}	/* if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) ) */

					/* �����_���ő吔�T���ł�����(���T�������Ǝv����)�ꍇ�́ABreak���ď����𔲂��� */
					if( nSuccessCnt == nMaxNumPnt )
					{
						break;
					}
					else
					{
						/* �N���A���Ď��̃��x���� */
						/* ���W/�p�����[�^ */
						tPntAry[0].nFtrValue = 0L;
						tPntAry[0].tPnt.nX = -1L;
						tPntAry[0].tPnt.nY = -1L;
						tPntAry[1].nFtrValue = 0L;
						tPntAry[1].tPnt.nX = -1L;
						tPntAry[1].tPnt.nY = -1L;
						tPntAry[2].nFtrValue = 0L;
						tPntAry[2].tPnt.nX = -1L;
						tPntAry[2].tPnt.nY = -1L;

						/* �J�E���^ */
						nNumPnt = 0L;
						nSuccessCnt = 0L;
					}

				}
				else
				{
					return ( CB_IMG_NG );
				}/* if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ptPatternInfo->nTypePattern ) */
			}
		}

#ifdef CB_FPE_CSW_DEBUG_ON
#if 0
		{
			/* �f�o�b�O�p_Work�摜���o�� */
			slong	nSxSys, nSySys, nExSys, nEySys;

			nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );
			
			nRet =implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, 359, 239 );
			nRet = cb_FPE_DBG_SavePixelValue( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], (char_t*)"C:/SelfCalib_Dbg_ImgHaarNORM.csv" );
			nRet = cb_FPE_DBG_SavePixelValue( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], (char_t*)"C:/SelfCalib_Dbg_ImgHaarBIN.csv" );
			nRet = cb_FPE_DBG_SavePixelValue( pnImgWk[CB_FPE_IMG_HAAR_LABELING], (char_t*)"C:/SelfCalib_Dbg_ImgHaarLABEL.csv" );

			nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );
		}
#endif
#endif /* CB_FPE_CSW_DEBUG_ON */

	}



	/* �_�����C���N�������g */
	nNumPnt++;

	if ( 1L < nMaxNumPnt )
	{
		if ( 3L == nMaxNumPnt )
		{

			/* Index:0 -> Index:1�ɓ���ւ� */
			tPntAry[1] = tPntAry[0];

			/* ���� */
			tPntAry[0].nFtrValue = 0L;
			tPntAry[0].tPnt.nX = -1L;
			tPntAry[0].tPnt.nY = -1L;

			nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

			if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) )
			{
				for ( ni = tPntAry[1].tPnt.nY - 10L; ni <= ( tPntAry[1].tPnt.nY + 10L ); ni++ )
				{
					/* �擪�|�C���^ */
					pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ni * ptHaarLike->nXSize ) + nSxSrchRgn;

					for ( nj = nSxSrchRgn; nj < tPntAry[1].tPnt.nX; nj++ )
					{
#if 0
						if ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection )
						{
							if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature) < tPntAry[0].nFtrValue )
							{
								tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
								tPntAry[0].tPnt.nX = nj;
								tPntAry[0].tPnt.nY = ni;
							}

						}
						else
						{
							if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature) > tPntAry[0].nFtrValue )
							{
								tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
								tPntAry[0].tPnt.nX = nj;
								tPntAry[0].tPnt.nY = ni;
							}
						}
#endif
						if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature2) > tPntAry[0].nFtrValue )
						{
							tPntAry[0].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
							tPntAry[0].tPnt.nX = nj;
							tPntAry[0].tPnt.nY = ni;
						}

						(pnWkFtrAry[0])++;
					}
				}

				/* �M���x�`�F�b�N */
				if ( labs( tPntAry[0].nFtrValue ) < nThrHaarFtrCrossPnt )
				{
					tPntAry[0].nFtrValue = 0L;
					tPntAry[0].tPnt.nX = -1L;
					tPntAry[0].tPnt.nY = -1L;
				}
			}	/* if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) ) */

			/* �_���C���N�������g */
			nNumPnt++;


			/* �E�� */
			tPntAry[2].nFtrValue = 0L;
			tPntAry[2].tPnt.nX = -1L;
			tPntAry[2].tPnt.nY = -1L;

			if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) )
			{
				for ( ni = tPntAry[1].tPnt.nY - 10L; ni <= ( tPntAry[1].tPnt.nY + 10L ); ni++ )
				{
					/* �擪�|�C���^ */
					pnWkFtrAry[0L] = ptHaarLike->pnFtrValue + ( ni * ptHaarLike->nXSize ) + ( tPntAry[1].tPnt.nX + 1L );

					for ( nj = ( tPntAry[1].tPnt.nX + 1L ); nj <= nExSrchRgn; nj++ )
					{
#if 0
						if ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection )
						{
							if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature) < tPntAry[2].nFtrValue )
							{
								tPntAry[2].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
								tPntAry[2].tPnt.nX = nj;
								tPntAry[2].tPnt.nY = ni;
							}

						}
						else
						{
							if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature) > tPntAry[2].nFtrValue )
							{
								tPntAry[2].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature;
								tPntAry[2].tPnt.nX = nj;
								tPntAry[2].tPnt.nY = ni;
							}
						}
#endif
						if ( ((*(pnWkFtrAry[0])) * nSignHaarLikeFeature2) > tPntAry[2].nFtrValue )
						{
							tPntAry[2].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
							tPntAry[2].tPnt.nX = nj;
							tPntAry[2].tPnt.nY = ni;
						}

						(pnWkFtrAry[0])++;
					}
					
				}


				/* �M���x�`�F�b�N */
				if ( labs( tPntAry[2].nFtrValue ) < nThrHaarFtrCrossPnt )
				{
					tPntAry[2].nFtrValue = 0L;
					tPntAry[2].tPnt.nX = -1L;
					tPntAry[2].tPnt.nY = -1L;
				}
			}	/* if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) ) */

			/* �_�����C���N�������g */
			nNumPnt++;

		}
		else if ( 2L == nMaxNumPnt )
		{
			if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
			{
				/* �A���S���Y���^�C�v */
				nRet = cb_FPE_GetTypeAlgorithm( ptPatternInfo, &nTypeAlgorithm );
				if ( CB_IMG_OK != nRet )
				{
					return ( nRet );
				}
				
				if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
				{
					nScanSX = tPntAry[0].tPnt.nX + 1L;
					nScanEX = nExSrchRgn;
				}
				else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
				{
					nScanSX = nSxSrchRgn;
					nScanEX = tPntAry[0].tPnt.nX - 1L;
				}
				else
				{
					return ( CB_IMG_NG );
				}
				nScanSY = tPntAry[0].tPnt.nY - 10L;
				nScanEY = tPntAry[0].tPnt.nY + 10L;

				/* �����ݒ� */
				nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

				/* �����l */
				tPntAry[1].nFtrValue = 0L;
				tPntAry[1].tPnt.nX = -1L;
				tPntAry[1].tPnt.nY = -1L;

				for ( ni = nScanSY; ni <= nScanEY; ni++ )
				{
					/* �擪�|�C���^ */
					pnWkFtrAry[0] = ptHaarLike->pnFtrValue + ( ( ni * ptHaarLike->nXSize ) + nScanSX );

					for ( nj = nScanSX; nj <= nScanEX; nj++ )
					{
						if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2 ) > tPntAry[1].nFtrValue )
						{
							tPntAry[1].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
							tPntAry[1].tPnt.nX = nj;
							tPntAry[1].tPnt.nY = ni;
						}

						/* ���̃A�N�Z�X��� */
						(pnWkFtrAry[0])++;
					}	/* for ( nj ) */
				}	/* for ( ni ) */

				/* �M���x�`�F�b�N */
				if ( labs( tPntAry[1].nFtrValue ) < nThrHaarFtrCrossPnt )
				{
					tPntAry[1].nFtrValue = 0L;
					tPntAry[1].tPnt.nX = -1L;
					tPntAry[1].tPnt.nX = -1L;
				}


				if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )	/* �E���̏ꍇ�̂݁A�z��̓���ւ� */
				{
					/* ����ւ� */
					tPntAry[2] = tPntAry[0];
					tPntAry[0] = tPntAry[1];
					tPntAry[1] = tPntAry[2];
				}
				/*
				�����̏ꍇ�́A�z��̓���ւ��͕s�v�̂��߁A�R�����g�A�E�g
				else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
				{

				}
				else
				{
				}
				*/
			}	/* if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) ) */

			/* �_�����C���N�������g */
			nNumPnt++;
		}
		else
		{
			return ( CB_IMG_NG );
		}/* if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ptPatternInfo->nTypePattern ) */
	}

	/* ���ʊi�[ */
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptCrossPnt[ni] = tPntAry[ni].tPnt;
	}
	*pnNumCrossPnt = nNumPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		��/�����s���_(���)�̒T��
 *
 * @param[in]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCrossPnt		:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nNumCrossPnt	:,�摜���W(�\���ʒu)�\���̂̐�,-,[-],
 * @param[out]	ptDstPnt		:,�摜���W(�s���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[out]	pnNumDstPnt		:,�摜���W(�s���ʒu)�\���̂̐�,-,[-],
 * @param[in]	ptSrchRgn		:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nEdgePos		:,�T������,CB_FPE_UPPER_EDGE_POS_PATTERN<=value<=CB_FPE_LOWER_EDGE_POS_PATTERN,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.12	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SearchCandTPntUL( const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_IMG_POINT* const ptCrossPnt, slong nNumCrossPnt, 
										CB_IMG_POINT* ptDstPnt, slong* pnNumDstPnt, const CB_RECT_RGN* const ptSrchRgn, enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos )
{
	slong	ni, nj, nk;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeScanRgn, nYSizeScanRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nXattn, nYattn;
	slong	nFlgSrchComp;
	slong	nNumDstPnt;
	slong	nCoeffDirY;
	float_t	fThrFtrValue;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo, tPrevMaxPntInfo;
	slong	*pnWkFtr;
	/* �p�����[�^ */
	slong	nprm_ysizeBlockW;
	slong	nprm_xsizeHalfScanRgn;
	slong	nprm_diffHaarLike;
	slong	nprm_thrHaarFtrTPnt;
	slong	nprm_distanceSrchRgn;
	float_t	fprm_coeffThrHaarLike;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptCrossPnt ) || ( NULL == ptDstPnt ) || ( NULL == pnNumDstPnt ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE;
//	nprm_xsizeHalfScanRgn = CB_FPE_PRM_XSIZE_HALF_SCAN_RGN;
	nprm_diffHaarLike = CB_FPE_PRM_DIFF_HAAR_LIKE;
	fprm_coeffThrHaarLike = CB_FPE_PRM_COEFF_THR_HAAR_LIKE;
	nprm_thrHaarFtrTPnt = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_UL;
	nprm_distanceSrchRgn = CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_UL;

	/* �T���̈�ݒ� */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	/* ������ */
	nNumDstPnt = 0L;

	/* �T�������̌W�� */
	if ( CB_FPE_LOWER_EDGE_POS_PATTERN == nEdgePos )
	{
		nCoeffDirY = (+1);
	}
	else if ( CB_FPE_UPPER_EDGE_POS_PATTERN == nEdgePos )
	{
		nCoeffDirY = (-1);
	}
	else
	{
		return ( CB_IMG_NG );
	}

	/* �T�� */
	for ( ni = 0L; ni < nNumCrossPnt; ni++ )
	{
		ptDstPnt[ni].nX = -1L;
		ptDstPnt[ni].nY = -1L;

		if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) )
		{
			/* �p�����[�^�ݒ� */
			nprm_xsizeHalfScanRgn = CB_FPE_PRM_XSIZE_HALF_SCAN_RGN;

			nXattn = ptCrossPnt[ni].nX;
//			nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * nprm_ysizeBlockW );
			nXSizeScanRgn = ( 2L * nprm_xsizeHalfScanRgn ) + 1L;
			if ( nCoeffDirY < 0L )
			{
				nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * nprm_distanceSrchRgn );
				nYSizeScanRgn = ( nYattn - ( nSySrchRgn + nprm_ysizeBlockW ) ) + 1L;
			}
			else if ( 0L < nCoeffDirY )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6816	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6817
			{
				nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * ( nprm_ysizeBlockW + nprm_distanceSrchRgn ) );
				nYSizeScanRgn = ( nEySrchRgn - nYattn ) + 1L;
			}
			else
			{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6818
				return ( CB_IMG_NG );
			}

			tPrevMaxPntInfo.nFtrValue = -1L;	/* �����l */
			tPrevMaxPntInfo.tPnt.nX = -1L;	/* �����l */
			tPrevMaxPntInfo.tPnt.nY = -1L;	/* �����l */
			tMaxPntInfo.nFtrValue = 0L;		/* �����l */
			tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
			tMaxPntInfo.tPnt.nY = -1L;		/* �����l */
			nFlgSrchComp = CB_FALSE;

			/* Y�����ɒT����i�߂� */
			fThrFtrValue = CB_FPE_FLT_MIN;
			for ( nj = 0L; nj < nYSizeScanRgn; nj++ )
			{
				/* �T���̈���`�F�b�N */
				if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nXattn, nYattn, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn ) )
				{
					if ( 0L >= tPrevMaxPntInfo.nFtrValue )
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
					}
					break;
				}

				pnWkFtr = ptHaarLike->pnFtrValue + ( nYattn * ptHaarLike->nXSize ) + ( nXattn - nprm_xsizeHalfScanRgn );

				/* Haar-like�����ʂ̐�Βl���傫���_��T�� */
				tMaxPntInfo.nFtrValue = 0L;
				for ( nk = -nprm_xsizeHalfScanRgn; nk <= nprm_xsizeHalfScanRgn; nk++ )
				{
					if ( tMaxPntInfo.nFtrValue < labs( *pnWkFtr ) )
					{
						tMaxPntInfo.nFtrValue = labs( *pnWkFtr );
						tMaxPntInfo.tPnt.nX = nXattn + nk;
						tMaxPntInfo.tPnt.nY = nYattn;
					}

					/* X������1pixel�V�t�g������ */
					pnWkFtr++;
				}

				/* ���ړ_�X�V */
				if ( -1 < tPrevMaxPntInfo.nFtrValue )	/* ����T���ȊO */
				{
	//				if ( tMaxPntInfo.nFtrValue < ( tPrevMaxPntInfo.nFtrValue - nprm_diffHaarLike ) )	/* �O��̒T���ʒu��Haar-like�����ʂ���}���ɓ����ʂ������� */
	//				if ( labs( tMaxPntInfo.nFtrValue - tPrevMaxPntInfo.nFtrValue ) > nprm_diffHaarLike )	/* �O��̒T���ʒu��Haar-like�����ʂ���}���ɓ����ʂ������� */
	//				if ( (float_t)( tMaxPntInfo.nFtrValue ) < ( (float_t)( tPrevMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike ) )	/* �O��̒T���ʒu��Haar-like�����ʂ���}���ɓ����ʂ������� */
					if ( (float_t)( tMaxPntInfo.nFtrValue ) < fThrFtrValue )
					{
						nFlgSrchComp = CB_TRUE;	/* �T������ */
						break;
					}
					else
					{
						/* �T�����s(Y������1�V�t�g�������ʒu�𒍖ڗ̈�Ƃ���) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;
					}
				}
				else	/* ����T�� */
				{
					if ( 0L < tMaxPntInfo.nFtrValue )
					{
						/* �T�����s(Y������1�V�t�g�������ʒu�𒍖ڗ̈�Ƃ���) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;

						/* ����T������Haar-like�����ʂ���A�������l��ݒ� */
						fThrFtrValue = (float_t)( tMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike;
					}
					else
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
						break;
					}
				}

				/* 2��ڈȍ~�̒T���ł́A�T���̈�����߂� */
				/* �p�����[�^�ݒ� */
				nprm_xsizeHalfScanRgn = CB_FPE_PRM_XSIZE_REFINE_HALF_SCAN_RGN;
			}	/* for ( nj ) */
		
			/* �M���x�`�F�b�N */
			if ( labs( tPrevMaxPntInfo.nFtrValue ) < nprm_thrHaarFtrTPnt )
			{
				ptDstPnt[ni].nX = -1L;
				ptDstPnt[ni].nY = -1L;
			}
			else
			{
				/* ���_�i�[ */
				if ( nCoeffDirY < 0L )
				{
					ptDstPnt[ni].nX = tPrevMaxPntInfo.tPnt.nX;
					ptDstPnt[ni].nY = tPrevMaxPntInfo.tPnt.nY - nprm_ysizeBlockW;
				}
				else if ( 0L < nCoeffDirY )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6819	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6820
				{
					ptDstPnt[ni] = tPrevMaxPntInfo.tPnt;
				}
				else
				{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6821
					return ( CB_IMG_NG );
				}
			}
		}	/* if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) ) */

		/* �_�����C���N�������g */
		nNumDstPnt++;
	}	/* for ( ni ) */

	*pnNumDstPnt= nNumDstPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�E/�����s���_(���)�̒T��
 *
 * @param[in]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCrossPnt		:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nNumCrossPnt	:,�摜���W(�\���ʒu)�\���̂̐�,-,[-],
 * @param[out]	ptDstPnt		:,�摜���W(�s���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[out]	pnNumDstPnt		:,�摜���W(�s���ʒu)�\���̂̐�,-,[-],
 * @param[in]	ptSrchRgn		:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nEdgePos		:,�T������,CB_FPE_LEFT_EDGE_POS_PATTERN<=value<=CB_FPE_RIGHT_EDGE_POS_PATTERN,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.18	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SearchCandTPntLR( const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_IMG_POINT* const ptCrossPnt, slong nNumCrossPnt, 
										CB_IMG_POINT* ptDstPnt, slong* pnNumDstPnt, const CB_RECT_RGN* const ptSrchRgn, enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos )
{
	slong	ni, nj, nk;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeScanRgn, nYSizeScanRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nXattn, nYattn;
	slong	nFlgSrchComp;
	slong	nNumPnt;
	slong	nCoeffDirX;
	float_t	fThrFtrValue;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo, tPrevMaxPntInfo;
	slong	*pnWkFtr;
	/* �p�����[�^ */
	slong	nprm_xsizeBlockW;
	slong	nprm_ysizeHalfScanRgn;
	slong	nprm_diffHaarLike;
	slong	nprm_thrHaarFtrTPnt;
	slong	nprm_distanceSrchRgn;
	float_t	fprm_coeffThrHaarLike;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptCrossPnt ) || ( NULL == ptDstPnt ) || ( NULL == pnNumDstPnt ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE;
//	nprm_ysizeHalfScanRgn = CB_FPE_PRM_YSIZE_HALF_SCAN_RGN;
	nprm_diffHaarLike = CB_FPE_PRM_DIFF_HAAR_LIKE;
	fprm_coeffThrHaarLike = CB_FPE_PRM_COEFF_THR_HAAR_LIKE;
	nprm_thrHaarFtrTPnt = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_LR;
	nprm_distanceSrchRgn = CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_LR;

	/* �T���̈�ݒ� */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	/* �N���A */
	nNumPnt = 0L;

	/* �T�������̌W�� */
	if ( CB_FPE_RIGHT_EDGE_POS_PATTERN == nEdgePos )
	{
		nCoeffDirX = (+1);
	}
	else if ( CB_FPE_LEFT_EDGE_POS_PATTERN == nEdgePos )
	{
		nCoeffDirX = (-1);
	}
	else
	{
		return ( CB_IMG_NG );
	}

	for ( ni = 0L; ni < nNumCrossPnt; ni++ )
	{
		ptDstPnt[ni].nX = -1L;
		ptDstPnt[ni].nY = -1L;

		if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) )
		{
			/* �p�����[�^�ݒ� */
			nprm_ysizeHalfScanRgn = CB_FPE_PRM_YSIZE_HALF_SCAN_RGN;

//			nXattn = ptCrossPnt[ni].nX + ( nCoeffDirX * nprm_xsizeBlockW );
			nYattn = ptCrossPnt[ni].nY;
			if ( 0L < nCoeffDirX )
			{
				nXattn = ptCrossPnt[ni].nX + ( nCoeffDirX * ( nprm_xsizeBlockW + nprm_distanceSrchRgn ) );
				nXSizeScanRgn = ( nExSrchRgn - nXattn ) + 1L;
			}
			else if ( nCoeffDirX < 0L )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6822	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6823
			{
				nXattn = ptCrossPnt[ni].nX + ( nCoeffDirX * nprm_distanceSrchRgn );
				nXSizeScanRgn = ( nXattn - ( nSxSrchRgn + nprm_xsizeBlockW ) ) + 1L;
			}
			else
			{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6824
				/* �G���[ */
				return ( CB_IMG_NG );
			}
			nYSizeScanRgn = ( 2L * nprm_ysizeHalfScanRgn ) + 1L;
			tPrevMaxPntInfo.nFtrValue = -1L;	/* �����l */
			tPrevMaxPntInfo.tPnt.nX = -1L;	/* �����l */
			tPrevMaxPntInfo.tPnt.nY = -1L;	/* �����l */
			tMaxPntInfo.nFtrValue = 0L;		/* �����l */
			tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
			tMaxPntInfo.tPnt.nY = -1L;		/* �����l */
			nFlgSrchComp = CB_FALSE;

			/* X�����ɒT����i�߂� */
			fThrFtrValue = CB_FPE_FLT_MIN;
			for ( nj = 0L; nj < nXSizeScanRgn; nj++ )
			{
				/* �T���̈���`�F�b�N */
				if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nXattn, nYattn, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn ) )
				{
					if ( 0L >= tPrevMaxPntInfo.nFtrValue )
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
					}
					break;
				}

				pnWkFtr = ptHaarLike->pnFtrValue + ( ( nYattn - nprm_ysizeHalfScanRgn ) * ptHaarLike->nXSize ) + nXattn;

				/* Haar-like�����ʂ̐�Βl���傫���_��T�� */
				tMaxPntInfo.nFtrValue = 0L;
				for ( nk = -nprm_ysizeHalfScanRgn; nk <= nprm_ysizeHalfScanRgn; nk++ )
				{
					if ( tMaxPntInfo.nFtrValue < labs( *pnWkFtr ) )
					{
						tMaxPntInfo.nFtrValue = labs( *pnWkFtr );
						tMaxPntInfo.tPnt.nX = nXattn;
						tMaxPntInfo.tPnt.nY = nYattn + nk;
					}

					/* Y������1pixel�V�t�g������ */
					pnWkFtr += ( ptHaarLike->nXSize );
				}

				/* ���ړ_�X�V */
				if ( -1 < tPrevMaxPntInfo.nFtrValue )	/* ����T���ȊO */
				{
	//				if ( tMaxPntInfo.nFtrValue < ( tPrevMaxPntInfo.nFtrValue - nprm_diffHaarLike ) )	/* �O��̒T���ʒu��Haar-like�����ʂ���}���ɓ����ʂ������� */
	//				if ( (float_t)( tMaxPntInfo.nFtrValue ) < ( (float_t)( tPrevMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike ) )	/* �O��̒T���ʒu��Haar-like�����ʂ���}���ɓ����ʂ������� */
					if ( (float_t)( tMaxPntInfo.nFtrValue ) < fThrFtrValue )	
					{
						nFlgSrchComp = CB_TRUE;	/* �T������ */
						break;
					}
					else
					{
						/* �T�����s(X������1�V�t�g�������ʒu�𒍖ڗ̈�Ƃ���) */
						nXattn = tMaxPntInfo.tPnt.nX + ( nCoeffDirX * 1L );
						nYattn = tMaxPntInfo.tPnt.nY;
						tPrevMaxPntInfo = tMaxPntInfo;
					}
				}
				else	/* ����T�� */
				{
					if ( 0L < tMaxPntInfo.nFtrValue )
					{
						/* �T�����s(X������1�V�t�g�������ʒu�𒍖ڗ̈�Ƃ���) */
						nXattn = tMaxPntInfo.tPnt.nX + ( nCoeffDirX * 1L );
						nYattn = tMaxPntInfo.tPnt.nY;
						tPrevMaxPntInfo = tMaxPntInfo;

						/* ����T������Haar-like�����ʂ���A�������l��ݒ� */
						fThrFtrValue = (float_t)( tMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike;
					}
					else
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
						break;
					}
				}

				/* 2��ڈȍ~�̒T���ł́A�T���̈�����߂� */
				/* �p�����[�^�ݒ� */
				nprm_ysizeHalfScanRgn = CB_FPE_PRM_YSIZE_REFINE_HALF_SCAN_RGN;
			}	/* for ( nj ) */
		
			/* �M���x�`�F�b�N */
			if ( labs( tPrevMaxPntInfo.nFtrValue ) < nprm_thrHaarFtrTPnt )
			{
				ptDstPnt[ni].nX = -1L;
				ptDstPnt[ni].nY = -1L;
			}
			else
			{
				/* ���_�i�[ */
				if ( 0L < nCoeffDirX )
				{
					ptDstPnt[ni] = tPrevMaxPntInfo.tPnt;
				}
				else if ( nCoeffDirX < 0L )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6825	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6826
				{
					ptDstPnt[ni].nX = tPrevMaxPntInfo.tPnt.nX - nprm_xsizeBlockW;
					ptDstPnt[ni].nY = tPrevMaxPntInfo.tPnt.nY;
				}
				else
				{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6827
					/* �G���[ */
					return ( CB_IMG_NG );
				}
			}
		}	/* if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) ) */

		/* �_�����C���N�������g */
		nNumPnt++;
	}	/* for ( ni ) */

	*pnNumDstPnt = nNumPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�g��
 *
 * @param[in,out]	ptSrchRsltPntInfo		:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.24	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ZoomIn4Coordinates( CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo )
{
	slong	ni;
	/* �p�����[�^ */
	slong	nprm_xmagZoomIn, nprm_ymagZoomIn;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRsltPntInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	nprm_xmagZoomIn = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_ymagZoomIn = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* �\�� */
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumCrossPnt; ni++ )
	{
		if ( ( 0L <= ptSrchRsltPntInfo->tCrossPntHalf[ni].nX ) && ( 0L <= ptSrchRsltPntInfo->tCrossPntHalf[ni].nY ) )
		{
			ptSrchRsltPntInfo->tCrossPnt[ni].nX = ptSrchRsltPntInfo->tCrossPntHalf[ni].nX * nprm_xmagZoomIn;
			ptSrchRsltPntInfo->tCrossPnt[ni].nY = ptSrchRsltPntInfo->tCrossPntHalf[ni].nY * nprm_ymagZoomIn;
		}
		else
		{
			ptSrchRsltPntInfo->tCrossPnt[ni].nX = -1L;
			ptSrchRsltPntInfo->tCrossPnt[ni].nY = -1L;
		}
	}
	/* �s��(�㑤) */
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumTPntUpper; ni++ )
	{
		if ( ( 0L <= ptSrchRsltPntInfo->tTPntUpperHalf[ni].nX ) && ( 0L <= ptSrchRsltPntInfo->tTPntUpperHalf[ni].nY ) )
		{
			ptSrchRsltPntInfo->tTPntUpper[ni].nX = ptSrchRsltPntInfo->tTPntUpperHalf[ni].nX * nprm_xmagZoomIn;
			ptSrchRsltPntInfo->tTPntUpper[ni].nY = ptSrchRsltPntInfo->tTPntUpperHalf[ni].nY * nprm_ymagZoomIn;
		}
		else
		{
			ptSrchRsltPntInfo->tTPntUpper[ni].nX = -1L;
			ptSrchRsltPntInfo->tTPntUpper[ni].nY = -1L;
		}
	}
	/* �s��(����) */
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumTPntLower; ni++ )
	{
		if ( ( 0L <= ptSrchRsltPntInfo->tTPntLowerHalf[ni].nX ) && ( 0L <= ptSrchRsltPntInfo->tTPntLowerHalf[ni].nY ) )
		{
			ptSrchRsltPntInfo->tTPntLower[ni].nX = ptSrchRsltPntInfo->tTPntLowerHalf[ni].nX * nprm_xmagZoomIn;
			ptSrchRsltPntInfo->tTPntLower[ni].nY = ptSrchRsltPntInfo->tTPntLowerHalf[ni].nY * nprm_ymagZoomIn;
		}
		else
		{
			ptSrchRsltPntInfo->tTPntLower[ni].nX = -1L;
			ptSrchRsltPntInfo->tTPntLower[ni].nY = -1L;
		}
	}
	/* �s��(����) */
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumTPntLeft; ni++ )
	{
		if ( ( 0L <= ptSrchRsltPntInfo->tTPntLeftHalf[ni].nX ) && ( 0L <= ptSrchRsltPntInfo->tTPntLeftHalf[ni].nY ) )
		{
			ptSrchRsltPntInfo->tTPntLeft[ni].nX = ptSrchRsltPntInfo->tTPntLeftHalf[ni].nX * nprm_xmagZoomIn;
			ptSrchRsltPntInfo->tTPntLeft[ni].nY = ptSrchRsltPntInfo->tTPntLeftHalf[ni].nY * nprm_ymagZoomIn;
		}
		else
		{
			ptSrchRsltPntInfo->tTPntLeft[ni].nX = -1L;
			ptSrchRsltPntInfo->tTPntLeft[ni].nY = -1L;
		}
	}
	/* �s��(�E��) */
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumTPntRight; ni++ )
	{
		if ( ( 0L <= ptSrchRsltPntInfo->tTPntRightHalf[ni].nX ) && ( 0L <= ptSrchRsltPntInfo->tTPntRightHalf[ni].nY ) )
		{
			ptSrchRsltPntInfo->tTPntRight[ni].nX = ptSrchRsltPntInfo->tTPntRightHalf[ni].nX * nprm_xmagZoomIn;
			ptSrchRsltPntInfo->tTPntRight[ni].nY = ptSrchRsltPntInfo->tTPntRightHalf[ni].nY * nprm_ymagZoomIn;
		}
		else
		{
			ptSrchRsltPntInfo->tTPntRight[ni].nX = -1L;
			ptSrchRsltPntInfo->tTPntRight[ni].nY = -1L;
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�����x�ʒu����(Checkerboard�p)
 *
 * @param[in]		nImgSrc					:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in,out]	ptSrchRsltPntInfo		:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptSrchRgnInfo			:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nCamDirection			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.12	K.Kato		�V�K�쐬
 *
 * @note			����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_PrecisionPositioning4Checkerboard( const IMPLIB_IMGID nImgSrc, CB_FPE_SRCH_RSLT_PNT_INFO *ptSrchRsltPntInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo, slong nCamDirection )
{
	slong	nRet = CB_IMG_NG;
	slong	ni;
	slong	nCrossX, nCrossY;
	/* �p�����[�^ */
//	slong	nprm_xmagZoomIn, nprm_ymagZoomIn;

	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRsltPntInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
//	nprm_xmagZoomIn = CB_FPE_PRM_ZOOMOUT_XMAG;
//	nprm_ymagZoomIn = CB_FPE_PRM_ZOOMOUT_YMAG;

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_4PNT_CROSS] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �s���p�^�[���̌�_�𒊏o */
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumCrossPnt; ni++ )
	{
		nCrossX = ptSrchRsltPntInfo->tCrossPnt[ni].nX;
		nCrossY = ptSrchRsltPntInfo->tCrossPnt[ni].nY;
		
		if ( ( 0L <= nCrossX ) && ( 0L <= nCrossY ) )
		{
			nRet = cb_FPE_methodFourPointCross( nImgSrc, nCrossX, nCrossY, &(ptSrchRsltPntInfo->tCrossPntPrecise[ni]) );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG_METHOD_4PNT_CROSS );
			}
		}
		else
		{
			ptSrchRsltPntInfo->tCrossPntPrecise[ni].w = -1.0;
			ptSrchRsltPntInfo->tCrossPntPrecise[ni].h = -1.0;
			ptSrchRsltPntInfo->tCrossPntPrecise[ni].nFlagPrecise = CB_FALSE;
		}
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_4PNT_CROSS] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_EST_PNT_EDGE_PTN] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* �p�^�[���̉��p */
	nRet = cb_FPE_EstimateFtrPoint4EdgeOfPattern( nImgSrc, ptSrchRsltPntInfo, ptSrchRgnInfo, nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_EST_EDGE_PNT_OF_PATTERN );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_EST_PNT_EDGE_PTN] );
#endif /* TIME_MEASUREMENT_CALIB */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�����_�ʒu�̐���(�p�^�[���̉��p)
 *
 * @param[in]		nImgSrc					:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in,out]	ptSrchRsltPntInfo		:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptSrchRgnInfo			:,�J�������Ƃ̒T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nCamDirection			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.16	K.Kato		�V�K�쐬
 * @date			2017.10.12	A.Honda		�T���͈͂̊ۂߍ��ݏ����ǉ�
 *
 * @note			����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_EstimateFtrPoint4EdgeOfPattern( const IMPLIB_IMGID nImgSrc, CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo, slong nCamDirection )
{
	slong						nRet = CB_IMG_NG;
	slong						ni = 0L, 
								nj = 0L, 
								nk = 0L;
	slong						nDirX = 0L, 
								nDirY = 0L;
	slong						nDirCoeffX = 0L, 
								nDirCoeffY = 0L;
	slong						nNumEdgePntUL = 0L, 
								nNumEdgePntLR = 0L, 
								nNumEdgePrecPntUL = 0L, 
								nNumEdgePrecPntLR = 0L;
	slong						yBase = 0L, 
								xBase = 0L;
	slong						nTempX = 0L, 
								nTempY = 0L;
	slong						nNumPntArray[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { 0L };
	slong						nSignHaarLikeFeature = 0L;
	ulong						nFlagChkRgn = CB_TRUE;
	enum enum_CB_FPE_SCAN_DIR_X	nScanDirX = CB_FPE_SCAN_DIR_X_PLUS;
	enum enum_CB_FPE_SCAN_DIR_Y	nScanDirY = CB_FPE_SCAN_DIR_Y_PLUS;
	CB_RECT_RGN					tSrchRgnUL = { 0L, 0L, 0L, 0L }, 
								tSrchRgnLR = { 0L, 0L, 0L, 0L };
	CB_RECT_RGN					tEdgeFLTRgnUL = { 0L, 0L, 0L, 0L }, 
								tEdgeFLTRgnLR = { 0L, 0L, 0L, 0L };
	CB_RECT_RGN					tEdgeNoUseRgnUL = { 0L, 0L, 0L, 0L }, 
								tEdgeNoUseRgnLR = { 0L, 0L, 0L, 0L };
	CB_FPE_IMG_ACCESS_INFO		tImgAccessInfo = { 0L, 0L, NULL, NULL, NULL, NULL };
	CB_IMG_POINT				*ptPntArray[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { NULL, NULL, NULL, NULL };
	t_cb_img_CenterPos			*ptRsltPntArray[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { NULL, NULL, NULL, NULL };
	CB_FPE_IMG_ACCESS_INFO		*ptPixelDataUL = NULL, 
								*ptPixelDataLR = NULL;
	CB_FPE_IMG_ACCESS_INFO		*ptEdgeRhoUL = NULL, 
								*ptEdgeRhoLR = NULL;
	CB_IMG_POINT				*ptEdgePntUL = NULL, 
								*ptEdgePntLR = NULL;
	t_cb_img_CenterPos			*ptEdgePrecPntUL = NULL, 
								*ptEdgePrecPntLR = NULL;
	/* �p�����[�^ */
	slong	nprm_distanceX = 0L, 
			nprm_distanceY = 0L;
	slong	nprm_WidthSrchRgn = 0L, 
			nprm_HeightSrchRgn = 0L;
	slong	nprm_halfWidthSrchRgn = 0L, 
			nprm_halfHeightSrchRgn = 0L;
	slong	nprm_sizeNoUse = 0L;

	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRsltPntInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	nprm_distanceX			= CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_X;
	nprm_distanceY			= CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_Y;
	nprm_WidthSrchRgn		= CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN;
	nprm_HeightSrchRgn		= CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN;
	nprm_halfWidthSrchRgn	= CB_FPE_PRM_HALF_WIDTH_PRECISE_SRCH_RGN;
	nprm_halfHeightSrchRgn	= CB_FPE_PRM_HALF_HEIGHT_PRECISE_SRCH_RGN;
	nprm_sizeNoUse			= CB_FPE_PRM_PERIPHERAL_NO_USE;

	/* �擪�|�C���^ */
	ptPixelDataUL		= &(m_FPE_tWorkInfo.tPixelDataUL);
	ptPixelDataLR		= &(m_FPE_tWorkInfo.tPixelDataLR);
	ptEdgeRhoUL			= &(m_FPE_tWorkInfo.tEdgeRhoUL);
	ptEdgeRhoLR			= &(m_FPE_tWorkInfo.tEdgeRhoLR);
	ptEdgePntUL			= &(m_FPE_tWorkInfo.tEdgePntUL[0L]);
	ptEdgePntLR			= &(m_FPE_tWorkInfo.tEdgePntLR[0L]);
	ptEdgePrecPntUL		= &(m_FPE_tWorkInfo.tEdgePrecPntUL[0L]);
	ptEdgePrecPntLR		= &(m_FPE_tWorkInfo.tEdgePrecPntLR[0L]);

	/* ===== �R�t�� ===== */
	/* ��f�l(�c����) */
	ptPixelDataUL->ptRgn = &tEdgeFLTRgnUL;
	/* ��f�l(������) */
	ptPixelDataLR->ptRgn = &tEdgeFLTRgnLR;
	/* �G�b�W���x(�c����) */
	ptEdgeRhoUL->ptRgn = &tEdgeNoUseRgnUL;
	/* �G�b�W���x(������) */
	ptEdgeRhoLR->ptRgn = &tEdgeNoUseRgnLR;

	/* �e�����_�̏���o�^ */
	ptPntArray[CB_FPE_UPPER_EDGE_POS_PATTERN]		= &( ptSrchRsltPntInfo[0L].tTPntUpper[0L] );
	ptPntArray[CB_FPE_LOWER_EDGE_POS_PATTERN]		= &( ptSrchRsltPntInfo[0L].tTPntLower[0L] );
	ptPntArray[CB_FPE_LEFT_EDGE_POS_PATTERN]		= &( ptSrchRsltPntInfo[0L].tTPntLeft[0L] );
	ptPntArray[CB_FPE_RIGHT_EDGE_POS_PATTERN]		= &( ptSrchRsltPntInfo[0L].tTPntRight[0L] );
	ptRsltPntArray[CB_FPE_UPPER_EDGE_POS_PATTERN]	= &( ptSrchRsltPntInfo[0L].tTPntUpperPrecise[0L] );
	ptRsltPntArray[CB_FPE_LOWER_EDGE_POS_PATTERN]	= &( ptSrchRsltPntInfo[0L].tTPntLowerPrecise[0L] );
	ptRsltPntArray[CB_FPE_LEFT_EDGE_POS_PATTERN]	= &( ptSrchRsltPntInfo[0L].tTPntLeftPrecise[0L] );
	ptRsltPntArray[CB_FPE_RIGHT_EDGE_POS_PATTERN]	= &( ptSrchRsltPntInfo[0L].tTPntRightPrecise[0L] );
	nNumPntArray[CB_FPE_UPPER_EDGE_POS_PATTERN]		= ptSrchRsltPntInfo[0L].nNumTPntUpper;
	nNumPntArray[CB_FPE_LOWER_EDGE_POS_PATTERN]		= ptSrchRsltPntInfo[0L].nNumTPntLower;
	nNumPntArray[CB_FPE_LEFT_EDGE_POS_PATTERN]		= ptSrchRsltPntInfo[0L].nNumTPntLeft;
	nNumPntArray[CB_FPE_RIGHT_EDGE_POS_PATTERN]		= ptSrchRsltPntInfo[0L].nNumTPntRight;

	/* �d�ݕt��Haar-like�����ʕ����̎擾 */
	nRet = cb_FPE_GetSignHaarLikeFeature( &( ptSrchRgnInfo->tPtnInfo ), nCamDirection, &nSignHaarLikeFeature );
	if( CB_IMG_OK != nRet )
	{
		return ( nRet );
	}

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgAccessInfo.nXSize ), &( tImgAccessInfo.nYSize ), (void**)&( tImgAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	for ( ni = 0L; ni < CB_FPE_EDGE_POS_PATTERN_MAXNUM; ni++ )
	{
		/*
		==================================================
			�̈�̐ݒ����
		==================================================
		*/
		nDirX = m_FPE_nDirX4PreciseSrchRgn[ni];
		nDirY = m_FPE_nDirY4PreciseSrchRgn[ni];

		/* �����_���ɂ��؂�ւ� */
		if ( 0L < nSignHaarLikeFeature )
		{
			if ( ( 3L == nNumPntArray[ni] ) && ( CB_FPE_UPPER_EDGE_POS_PATTERN == ni ) )
			{
				nDirX = nDirX * ( -1L );
			}
		}
		else if ( 0L > nSignHaarLikeFeature )
		{
			if ( ( 3L == nNumPntArray[ni] ) && ( CB_FPE_LOWER_EDGE_POS_PATTERN == ni ) )
			{
				nDirX = nDirX * ( -1L );
			}
			else if ( CB_FPE_LEFT_EDGE_POS_PATTERN == ni )
			{
				nDirY = nDirY * ( -1L );
			}
			else if ( CB_FPE_RIGHT_EDGE_POS_PATTERN == ni )
			{
				nDirY = nDirY * ( -1L );
			}
			else
			{
				/* ERROR�ł͂Ȃ� */
				/* NOP */
			}
		}
		else
		{
			return ( CB_IMG_NG );
		}

		for ( nj = 0L; nj < nNumPntArray[ni]; nj++ )
		{
			if ( ( 0L <= (ptPntArray[ni])[nj].nX ) && ( 0L <= (ptPntArray[ni])[nj].nY ) )
			{
				/* �X�L�����̈�̐ݒ� */
				/* �c�����ɐݒ肷��̈� */
				tSrchRgnUL.nSX = (ptPntArray[ni])[nj].nX - nprm_halfWidthSrchRgn;
				tSrchRgnUL.nSY = ( (ptPntArray[ni])[nj].nY + ( nDirY * nprm_distanceY ) ) - nprm_halfHeightSrchRgn;
				tSrchRgnUL.nEX = ( tSrchRgnUL.nSX + nprm_WidthSrchRgn ) - 1L;
				tSrchRgnUL.nEY = ( tSrchRgnUL.nSY + nprm_HeightSrchRgn ) - 1L;
				/* �������ɐݒ肷��̈� */
				tSrchRgnLR.nSX = ( (ptPntArray[ni])[nj].nX + ( nDirX * nprm_distanceX ) ) - nprm_halfHeightSrchRgn;
				tSrchRgnLR.nSY = (ptPntArray[ni])[nj].nY - nprm_halfWidthSrchRgn;
				tSrchRgnLR.nEX = ( tSrchRgnLR.nSX + nprm_HeightSrchRgn ) - 1L;
				tSrchRgnLR.nEY = ( tSrchRgnLR.nSY + nprm_WidthSrchRgn ) - 1L;

				/* �X�L�����̈�̃`�F�b�N */
				nFlagChkRgn = cb_FPE_CheckRegion( tSrchRgnUL.nSX, tSrchRgnUL.nSY, tSrchRgnUL.nEX, tSrchRgnUL.nEY, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
				if ( CB_TRUE != nFlagChkRgn )
				{
					/* �`�F�b�NNG���A�̈�Đݒ� */
					nRet = cb_FPE_ResetRegion( &tSrchRgnUL, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
					if ( CB_IMG_OK != nRet )
					{
						/* Open�����摜��������Close���� */
						nRet = implib_CloseImgDirect( nImgSrc );
						return ( CB_IMG_NG );
					}
				}
				nFlagChkRgn = cb_FPE_CheckRegion( tSrchRgnLR.nSX, tSrchRgnLR.nSY, tSrchRgnLR.nEX, tSrchRgnLR.nEY, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
				if ( CB_TRUE != nFlagChkRgn )
				{
					/* �`�F�b�NNG���A�̈�Đݒ� */
					nRet = cb_FPE_ResetRegion( &tSrchRgnLR, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
					if ( CB_IMG_OK != nRet )
					{
						/* Open�����摜��������Close���� */
						nRet = implib_CloseImgDirect( nImgSrc );
						return ( CB_IMG_NG );
					}
				}

				/* �X�L���������̐ݒ� */
				if ( 0L < nDirX )
				{
					nScanDirX = CB_FPE_SCAN_DIR_X_MINUS;
				}
				else if ( 0L > nDirX )
				{
					nScanDirX = CB_FPE_SCAN_DIR_X_PLUS;
				}
				else
				{
					/* �G���[ */
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}
				if ( 0L < nDirY )
				{
					nScanDirY = CB_FPE_SCAN_DIR_Y_MINUS;
				}
				else if ( 0L > nDirY )
				{
					nScanDirY = CB_FPE_SCAN_DIR_Y_PLUS;
				}
				else
				{
					/* �G���[ */
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ===== ��f�l�̎��o�� ===== */
				/*
					[MEMO]
					�c�����A�������ǂ���ł����L�p�^�[���ɔz�񂳂��悤�Ɏ��o���B

					�����E�E���������E�E����
					�����E�E���������E�E����
					�����E�E���������E�E����
					�����E�E���������E�E����
				*/

				/* �c���� */
				tImgAccessInfo.ptRgn = &tSrchRgnUL;
				nRet = cb_FPE_StorePixelValue( &tImgAccessInfo, ptPixelDataUL, nScanDirX, CB_FPE_SCAN_DIR_Y_PLUS, CB_FPE_NO_SHUFFLE );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ������ */
				tImgAccessInfo.ptRgn = &tSrchRgnLR;
				nRet = cb_FPE_StorePixelValue( &tImgAccessInfo, ptPixelDataLR, CB_FPE_SCAN_DIR_X_PLUS, nScanDirY, CB_FPE_SHUFFLE );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ===== ���C���G�b�W�t�B���^ ===== */
				/* �c���� */
				ptPixelDataUL->ptRgn->nSX = 0L;
				ptPixelDataUL->ptRgn->nSY = 0L;
				ptPixelDataUL->ptRgn->nEX = tSrchRgnUL.nEX - tSrchRgnUL.nSX;
				ptPixelDataUL->ptRgn->nEY = tSrchRgnUL.nEY - tSrchRgnUL.nSY;
				nRet = cb_FPE_LineEdgeFLT( ptPixelDataUL, ptEdgeRhoUL );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ������ */
				ptPixelDataLR->ptRgn->nSX = 0L;
				ptPixelDataLR->ptRgn->nSY = 0L;
				ptPixelDataLR->ptRgn->nEX = tSrchRgnLR.nEY - tSrchRgnLR.nSY;
				ptPixelDataLR->ptRgn->nEY = tSrchRgnLR.nEX - tSrchRgnLR.nSX;
				nRet = cb_FPE_LineEdgeFLT( ptPixelDataLR, ptEdgeRhoLR );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ===== �G�b�W�_���o ===== */
				/* �c���� */
				ptEdgeRhoUL->ptRgn->nSX = ptPixelDataUL->ptRgn->nSX + nprm_sizeNoUse;
				ptEdgeRhoUL->ptRgn->nSY = ptPixelDataUL->ptRgn->nSY;
				ptEdgeRhoUL->ptRgn->nEX = ptPixelDataUL->ptRgn->nEX - nprm_sizeNoUse;
				ptEdgeRhoUL->ptRgn->nEY = ptPixelDataUL->ptRgn->nEY;
				nRet = cb_FPE_ExtractEdgePoint( ptEdgeRhoUL, ptPixelDataUL, ptEdgePntUL, &nNumEdgePntUL );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ������ */
				ptEdgeRhoLR->ptRgn->nSX = ptPixelDataLR->ptRgn->nSX + nprm_sizeNoUse;
				ptEdgeRhoLR->ptRgn->nSY = ptPixelDataLR->ptRgn->nSY;
				ptEdgeRhoLR->ptRgn->nEX = ptPixelDataLR->ptRgn->nEX - nprm_sizeNoUse;
				ptEdgeRhoLR->ptRgn->nEY = ptPixelDataLR->ptRgn->nEY;
				nRet = cb_FPE_ExtractEdgePoint( ptEdgeRhoLR, ptPixelDataLR, ptEdgePntLR, &nNumEdgePntLR );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}
				
				/* �摜���_����(�_�̉���) */
			
				yBase = tSrchRgnUL.nSY;
				if ( CB_FPE_SCAN_DIR_X_MINUS == nScanDirX )
				{
					xBase = tSrchRgnUL.nEX - nprm_sizeNoUse;
					nDirCoeffX = (-1L);
				}
				else
				{
					xBase = tSrchRgnUL.nSX + nprm_sizeNoUse;
					nDirCoeffX = (+1L);
				}
				for ( nk = 0L; nk < nNumEdgePntUL; nk++ )
				{
					ptEdgePntUL[nk].nX = xBase + ( nDirCoeffX * ptEdgePntUL[nk].nX );
					ptEdgePntUL[nk].nY = yBase + ptEdgePntUL[nk].nY;
				}

				/* �摜���_����(�_�̍��E��) */
				xBase = tSrchRgnLR.nSX;
				if ( CB_FPE_SCAN_DIR_Y_PLUS == nScanDirY )
				{
					yBase = tSrchRgnLR.nSY + nprm_sizeNoUse;
					nDirCoeffY = (+1L);
				}
				else
				{
					yBase = tSrchRgnLR.nEY - nprm_sizeNoUse;
					nDirCoeffY = (-1L);
				}
				for ( nk = 0L; nk < nNumEdgePntLR; nk++ )
				{
					nTempX = xBase + ptEdgePntLR[nk].nY;
					nTempY = yBase + ( nDirCoeffY * ptEdgePntLR[nk].nX );
					ptEdgePntLR[nk].nX = nTempX;
					ptEdgePntLR[nk].nY = nTempY;
				}

				/* �����x�ʒu����(�_�̏㑤) */
				nRet = cb_FPE_ExtractEdgePrecisePoint( &tImgAccessInfo, ptEdgePntUL, nNumEdgePntUL, ptEdgePrecPntUL, &nNumEdgePrecPntUL, CB_FPE_LINE_FLT_VERT_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* �����x�ʒu����(�_�̉���) */
				nRet = cb_FPE_ExtractEdgePrecisePoint( &tImgAccessInfo, ptEdgePntLR, nNumEdgePntLR, ptEdgePrecPntLR, &nNumEdgePrecPntLR, CB_FPE_LINE_FLT_HORI_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ����/�����̒������ߎ����A��_���v�Z */
				nRet = cb_FPE_CalcIntersectionPoint( ptEdgePrecPntUL, ptEdgePrecPntLR, nNumEdgePrecPntUL, nNumEdgePrecPntLR, &( ptRsltPntArray[ni][nj] ) );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ��_�v�Z�����s���Ă���Ƃ��́A�\���_�̍r���T�����ʂ��i�[���� */
				if ( ptRsltPntArray[ni][nj].nFlagPrecise != CB_TRUE )
				{
					ptRsltPntArray[ni][nj].w = (double_t)( ptPntArray[ni][nj].nX );
					ptRsltPntArray[ni][nj].h = (double_t)( ptPntArray[ni][nj].nY );
					ptRsltPntArray[ni][nj].nFlagPrecise = CB_FALSE;
				}
				else
				{
					/* Not Operation */
				}

			}
			else
			{
				/* ���ʊi�[ */
				(ptRsltPntArray[ni])[nj].w = -1.0;
				(ptRsltPntArray[ni])[nj].h = -1.0;
				(ptRsltPntArray[ni])[nj].nFlagPrecise = CB_FALSE;
			}	/* if ( ( 0L <= (ptPntArray[ni])[nj].nX ) && ( 0L <= (ptPntArray[ni])[nj].nY ) ) */

			nDirX = nDirX * (-1L);
		}	/* for ( nj = 0; nj < nNumPntArray[ni]; nj++ ) */

	}	/* for ( ni = 0; ni < 2; ni++ ) */

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			�����x�ʒu����(Checkerboard�p)
 *
 * @param[in]		nImgSrc					:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCrossX					:,�s���p�^�[���̌�_X,-,[-],
 * @param[in]		nCrossY					:,�s���p�^�[���̌�_Y,-,[-],
 * @param[in]		ptPrecisePnt			:,�_���,-,[-],,
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.16	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_methodFourPointCross( IMPLIB_IMGID nImgSrc, slong nCrossX, slong nCrossY, t_cb_img_CenterPos *ptPrecisePnt )
{
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	ni;
	slong	nRet = CB_IMG_NG;							/* �߂�l�i�[�p�ϐ� */
	slong				nSxWin, nSyWin, nExWin, nEyWin;					/* �E�B���h�E�؂�o�����̍��W */
	double_t			dXUpArea, dYUpArea, dXDownArea, dYDownArea, dXLeftArea, dYLeftArea, dXRightArea, dYRightArea;	/* ��_�����߂�4�_�̍��W */
	slong					nMaxData;						/* ���Ԓl�̍ő�l */
	slong					nMinData;						/* ���Ԓl�̍ŏ��l */
	double_t				dResPoint;						/* �Z�x�ݐϒl���e���狁�߂��T�u�s�N�Z�� */
	slong					nResPointTemp;					/* �T�u�s�N�Z���Z�o�p���[�N�o�b�t�@ */
	slong					nResPointIdx;					/* �T�u�s�N�Z���Z�o���̒����ߕӂ𒊏o����ׂ̃C���f�b�N�X */
	double_t				dPrecisePoint[CB_FPE_PORJECT_AREA_MAXNUM];		/* 4�_�̃T�u�s�N�Z���i�[�e�[�u�� */
	double_t				dSlopeHori;					/* ���������㉺2�_�̒����W���i�X���j */
	double_t				dInterceptHori;					/* ���������㉺2�_�̒����W���i���Ӂj */
	double_t				dSlopeVert;						/* �����������E2�_�̒����W���i�X���j */
	double_t				dInterceptVert;						/* �����������E2�_�̒����W���i���Ӂj */
	slong					nProjectionIdx;					/* �Z�x�ݐϒl���e���s���G���A�̃��[�v�C���f�b�N�X */
	slong					nThrIntensity;
	slong					*pnTblMedian;
	slong					*pnTblProjGO;
	IMPLIB_IPGOFeatureTbl	*ptGOFtrTbl;
	/* �p�����[�^ */
	slong	nprm_NumProjectData = CB_FPE_PRM_PROJECTION_DATA_NUM;
	slong	nprm_NumProjectDataHalf = CB_FPE_PRM_PROJECTION_DATA_NUM_HALF;
	slong	nprm_ProjectCenterInt = CB_FPE_PRM_PROJECTION_CENTER_INTERVAL;
	slong	nprm_WidthProject = CB_FPE_PRM_PROJECTION_WIDTH;

	/* �����`�F�b�N */
	if ( NULL == ptPrecisePnt )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/***
	 * �ϐ�������
	 ***/
	memset( &( dPrecisePoint[0] ), 0x00, ( sizeof( double_t ) * CB_FPE_PORJECT_AREA_MAXNUM ) );
	dSlopeHori = 0.0;
	dInterceptHori = 0.0;
	dSlopeVert = 0.0;
	dInterceptVert = 0.0;

	/* �擪�|�C���^ */
	pnTblMedian = &(m_FPE_tWorkInfo.nTblMedian[0]);
	pnTblProjGO = &(m_FPE_tWorkInfo.nTblProjGO[0]);
	ptGOFtrTbl = &(m_FPE_tWorkInfo.tGOFtrTbl);

	/***
	 * ���߂����S���W�̏㉺���E4�ӏ��̔Z�x�ݐϒl���e���s���A
	 * 4�ӏ��̔Z�W���E�_�����߂āA����4�_�̌�_�����߂�B
	 ***/
	for ( nProjectionIdx = 0L; nProjectionIdx < CB_FPE_PORJECT_AREA_MAXNUM; nProjectionIdx++ )
	{
		/* �N���A */
		memset( pnTblProjGO, 0x00, ( sizeof( slong ) * ( size_t )nprm_NumProjectData ) );
		memset( pnTblMedian, 0x00, ( sizeof( slong ) * ( size_t )nprm_NumProjectData ) );

		switch ( nProjectionIdx )
		{
			case ( CB_FPE_PROJECT_UP_AREA ):
				/***
				 * �p�^�[���}�b�`���O�ŋ��߂����S�_���琅�������㕔�̓��e�p�E�B���h�E�؂�o��
				 ***/
				/* �i�؂�o�����W��"-1"�Ƃ��Ă���̂́A���_���W���܂ޒ������l�����Ă���ׁj */
				nSxWin = nCrossX - ( nprm_NumProjectDataHalf - 1L );
				nExWin = ( nSxWin + nprm_NumProjectData ) - 1L;
				nSyWin = nCrossY - ( nprm_ProjectCenterInt + nprm_WidthProject );
				nEyWin = ( nSyWin + nprm_WidthProject ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonX( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* �G���[���N���A */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			case ( CB_FPE_PROJECT_DOWN_AREA ):
				/***
				 * �p�^�[���}�b�`���O�ŋ��߂����S�_���琅�����������̓��e�p�E�B���h�E�؂�o��
				 ***/
				/* �i�؂�o�����W��"-1"�Ƃ��Ă���̂́A���_���W���܂ޒ������l�����Ă���ׁj */
				nSxWin = nCrossX - ( nprm_NumProjectDataHalf - 1L );
				nExWin = ( nSxWin + nprm_NumProjectData ) - 1L;
				nSyWin = nCrossY + nprm_ProjectCenterInt;
				nEyWin = ( nSyWin + nprm_WidthProject ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonX( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* �G���[���N���A */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			case ( CB_FPE_PROJECT_LEFT_AREA ):
				/***
				 * �p�^�[���}�b�`���O�ŋ��߂����S�_���琂�����������̓��e�p�E�B���h�E�؂�o��
				 ***/
				/* �i�؂�o�����W��"-1"�Ƃ��Ă���̂́A���_���W���܂ޒ������l�����Ă���ׁj */
				nSxWin = nCrossX - ( nprm_ProjectCenterInt + nprm_WidthProject );
				nExWin = ( nSxWin + nprm_WidthProject ) - 1L;
				nSyWin = nCrossY - ( nprm_NumProjectDataHalf - 1L );
				nEyWin = ( nSyWin + nprm_NumProjectData ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonY( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* �G���[���N���A */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			case ( CB_FPE_PROJECT_RIGHT_AREA ):
				/***
				 * �p�^�[���}�b�`���O�ŋ��߂����S�_���琂�������E���̓��e�p�E�B���h�E�؂�o��
				 ***/
				/* �i�؂�o�����W��"-1"�Ƃ��Ă���̂́A���_���W���܂ޒ������l�����Ă���ׁj */
				nSxWin = nCrossX - ( nprm_ProjectCenterInt + nprm_WidthProject );
				nExWin = ( nSxWin + nprm_WidthProject ) - 1L;
				nSyWin = nCrossY - ( nprm_NumProjectDataHalf - 1L );
				nEyWin = ( nSyWin + nprm_NumProjectData ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonY( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* �G���[���N���A */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			default:		// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6844
				break;
		}

		/***
		 * �Z�x�ݐϒl���e���ʂɑ΂��āA3��f�̒��Ԓl�����߂�B
		 * ���e���ʂ̃e�[�u��glTblW[]�ׂ̗荇��3�_glTblW[i],glTblW[i+1],glTblW[i+2]
		 * �̒l���r���A�^�񒆂̒l�𒊏o����B
		 ***/
		for ( ni = 1L; ni < ( nprm_NumProjectData - 1L ); ni++ )
		{
			if ( pnTblProjGO[ni - 1L] < pnTblProjGO[ni] )
			{
				if ( pnTblProjGO[ni + 1L] < pnTblProjGO[ni - 1L] )
				{
					pnTblMedian[ni] = pnTblProjGO[ni - 1L];	/* Median��ni-1 */
				}
				else
				{
					if ( pnTblProjGO[ni] < pnTblProjGO[ni + 1L] )
					{
						pnTblMedian[ni] = pnTblProjGO[ni];	/* Median��ni */
					}
					else
					{
						pnTblMedian[ni] = pnTblProjGO[ni + 1L];	/* Median��ni+1 */
					}
				}
			}
			else
			{
				if ( pnTblProjGO[ni - 1L] < pnTblProjGO[ni + 1L] )
				{
					pnTblMedian[ni] = pnTblProjGO[ni - 1L];	/* Median��ni-1 */
				}
				else
				{
					if ( pnTblProjGO[ni] < pnTblProjGO[ni + 1L] )
					{
						pnTblMedian[ni] = pnTblProjGO[ni + 1L];	/* Median��ni+1 */
					}
					else
					{
						pnTblMedian[ni] = pnTblProjGO[ni];	/* Median��ni */
					}
				}
			}
		}
		
		/***
		 * ���Ԓl�̍ő�l�ƍŏ��l�����߂āA�Z�W�ω��_��臒l���Z�o����B
		 ***/
		nMaxData = pnTblMedian[1];
		nMinData = pnTblMedian[1];
		for ( ni = 2L; ni < ( nprm_NumProjectData - 1L ); ni++ )
		{
			if ( nMaxData < pnTblMedian[ni] )
			{
				nMaxData = pnTblMedian[ni];
			}
			if ( pnTblMedian[ni] < nMinData )
			{
				nMinData = pnTblMedian[ni];
			}
		}
		/* �������l */
		nThrIntensity = ( nMinData + nMaxData ) / 2L;

		/***
		 * �Z�x�ݐϒl���e���ʂ���臒l������2�_�𒊏o����B
		 * ���o����2�_�̂����A臒l���傫�Ȓl�����_�̍��W�ɑ΂��āA
		 * 臒l�܂ł̃T�u�s�N�Z�����Z�o����B
		 ***/
		dResPoint = (double_t)nprm_NumProjectData;
		nResPointIdx = nprm_NumProjectDataHalf + 1L;
		for ( ni = 0L; ni < ( nprm_NumProjectData - 1L ); ni++ )
		{
			if ( ( pnTblProjGO[ni + 1L] < nThrIntensity ) && ( nThrIntensity < pnTblProjGO[ni] ) )
			{
				nResPointTemp = labs( nprm_NumProjectDataHalf - ni );
				if ( nResPointTemp < nResPointIdx )
				{
					nResPointIdx = nResPointTemp;
					/* pnTblProjGO[] �� nThrIntensity ��臒l�Ƃ��Ă��āA0�ɂ͂Ȃ�Ȃ��̂ŁA0���`�F�b�N�s�v */
					//dResPoint = (double_t)ni + ( (double_t)( pnTblProjGO[ni] - nThrIntensity ) / (double_t)( ( pnTblProjGO[ni] - nThrIntensity ) + ( nThrIntensity - pnTblProjGO[ni + 1L] ) ) );
					dResPoint = (double_t)ni + ( (double_t)( pnTblProjGO[ni] - nThrIntensity ) / (double_t)( ( pnTblProjGO[ni] - pnTblProjGO[ni + 1L] ) ) );
				}
			}
			else if ( ( pnTblProjGO[ni] < nThrIntensity ) && ( nThrIntensity < pnTblProjGO[ni + 1L] ) )
			{
				nResPointTemp = labs( nprm_NumProjectDataHalf - ni );
				if ( nResPointTemp < nResPointIdx )
				{
					nResPointIdx = nResPointTemp;
					/* pnTblProjGO[] �� nThrIntensity ��臒l�Ƃ��Ă��āA0�ɂ͂Ȃ�Ȃ��̂ŁA0���`�F�b�N�s�v */
					//dResPoint = (double_t)( ni + 1L ) - ( (double_t)( pnTblProjGO[ni + 1L] - nThrIntensity ) / (double_t)( ( pnTblProjGO[ni + 1L] - nThrIntensity ) + ( nThrIntensity - pnTblProjGO[ni] ) ) );
					dResPoint = (double_t)( ni + 1L ) - ( (double_t)( pnTblProjGO[ni + 1L] - nThrIntensity ) / (double_t)( ( pnTblProjGO[ni + 1L] -  pnTblProjGO[ni] ) ) );
				}
			}
			else if ( pnTblProjGO[ni] == nThrIntensity )
			{
				nResPointTemp = labs( nprm_NumProjectDataHalf - ni );
				if ( nResPointTemp < nResPointIdx )
				{
					nResPointIdx = nResPointTemp;
					dResPoint = (double_t)ni;
				}
			}
			else
			{
				/* �G���[�����ǉ�����I�I */;
			}
		}

		/***
		 * �T�u�s�N�Z�����i�[�B
		 ***/
		dPrecisePoint[nProjectionIdx] = dResPoint;
	}	/* for ( nProjectionIdx = 0; nProjectionIdx < CB_FPE_PORJECT_AREA_MAXNUM; nProjectionIdx++ ) */

	/***
	 * �T�u�s�N�Z����p���āA4�_�̍��W���Z�o�B
	 ***/
	dXUpArea = (double_t)( nCrossX - ( nprm_NumProjectDataHalf - 1L ) ) + dPrecisePoint[0];
	dYUpArea = (double_t)( nCrossY - ( nprm_ProjectCenterInt + nprm_WidthProject ) ) + ( (double_t)nprm_WidthProject / 2.0 );
	dXDownArea = (double_t)( nCrossX - ( nprm_NumProjectDataHalf - 1L ) ) + dPrecisePoint[1];
	dYDownArea = (double_t)( nCrossY + nprm_ProjectCenterInt ) + ( (double_t)nprm_WidthProject / 2.0 );
	dXLeftArea = (double_t)( nCrossX - ( nprm_ProjectCenterInt + nprm_WidthProject ) ) + ( (double_t)nprm_WidthProject / 2.0 );
	dYLeftArea = (double_t)( nCrossY - ( nprm_NumProjectDataHalf - 1L ) ) + dPrecisePoint[2];
	dXRightArea = (double_t)( nCrossX + nprm_ProjectCenterInt ) + ( (double_t)nprm_WidthProject / 2.0 );
	dYRightArea = (double_t)( nCrossY - ( nprm_NumProjectDataHalf - 1L ) ) + dPrecisePoint[3];

	/***
	 * 4�_�̍��W��p���āA4�_�̌�_���Z�o�B
	 ***/
	 /***
	  * ���������㉺2�_���W(up_x,up_y),(down_x,down_y)�����̕�������p����
	  * �����W���v�Zx=horizontal1*y+horizontal2���Z�o����B
	  ***/
	if ( CB_FPE_DBL_MIN <= fabs( dYUpArea - dYDownArea ) )
	{
		dSlopeHori = ( dXUpArea - dXDownArea ) / ( dYUpArea - dYDownArea );
		dInterceptHori = dXUpArea - ( dSlopeHori * dYUpArea );
	}
	else
	{
		/***
		 * �㉺2�_�̍��W��Y���W����v����ꍇ�́A�F�����s�ׁ̈A�G���[
		 ***/
		return ( CB_IMG_NG );
	}

	/***
	  * �����������E2�_���W(left_x,left_y),(right_x,right_y)�����̕�������p����
	  * �����W���v�Zx=vertical1*y+vertical2���Z�o����B
	  ***/
	if ( CB_FPE_DBL_MIN <= fabs( dYLeftArea - dYRightArea ) )
	{
		dSlopeVert = ( dXLeftArea - dXRightArea ) / ( dYLeftArea - dYRightArea );
		dInterceptVert = dXLeftArea - ( dSlopeVert * dYLeftArea );
		/***
		 * �����Ɛ�����2�����̌X��(horizontal1, vertical1)�ƌ���(horizontal2, vertical2)����
		 * ��_�����߂�B
		 ***/
		if ( CB_FPE_DBL_MIN <= fabs( dSlopeHori - dSlopeVert ) )
		{
			ptPrecisePnt->h = ( dInterceptVert - dInterceptHori ) / ( dSlopeHori - dSlopeVert );
			ptPrecisePnt->w = ( dSlopeHori * ptPrecisePnt->h ) + dInterceptHori;
			ptPrecisePnt->nFlagPrecise = CB_TRUE;
		}
		else
		{
			/***
			 * �����Ɛ�����2�����̌X��(horizontal1, vertical1)����v����ꍇ�́A
			 * ����������邱�Ƃ������ׁA�G���[
			 ***/
			return ( CB_IMG_NG );
		}
	}
	else
	{
		/***
		 * �����������E2�_��Y���W����v���Ă���ꍇ�́AY���W�͒萔�B
		 ***/
		if ( fabs( dSlopeHori ) > 0.0 )
		{
			ptPrecisePnt->h = dYLeftArea;
			ptPrecisePnt->w = ( dSlopeHori * ptPrecisePnt->h ) + dInterceptHori;
			ptPrecisePnt->nFlagPrecise = CB_TRUE;
		}
		else
		{
			/***
			 * �����������E2�_��Y���W����v���Ă��āA�����������㉺2�_��X���W��
			 * ��v���Ă���ꍇ�́AX���W�AY���W���ɒ萔�B
			 ***/
			ptPrecisePnt->h = dYLeftArea;
			ptPrecisePnt->w = dXUpArea;
			ptPrecisePnt->nFlagPrecise = CB_TRUE;
		}
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return CB_IMG_OK;
}

/******************************************************************************/
/**
 * @brief			Median Filter(Line)
 *
 * @param[in]		pnSrc					:,�P�x�l�̔z��ւ̃|�C���^(SRC),-,[-],
 * @param[out]		pnDst					:,�P�x�l�̔z��ւ̃|�C���^(DST),-,[-],
 * @param[in]		nNumData				:,�P�x�l�̔z��̃f�[�^��,0<value,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.18	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_MedianLineFilter( const slong* const pnSrc, slong* pnDst, slong nNumData )		// MISRA-C����̈�E [EntryAVM_QAC#3] M1.1.1  R-52, ID-6845
{
	slong	ni;

	/* �����`�F�b�N */
	if ( ( NULL == pnSrc ) || ( NULL == pnDst ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 1L; ni < ( nNumData - 1L ); ni++ )
	{
		if ( pnSrc[ni - 1L] < pnSrc[ni] )
		{
			if ( pnSrc[ni + 1L] < pnSrc[ni - 1L] )
			{
				pnDst[ni] = pnSrc[ni - 1L];	/* Median��ni-1 */
			}
			else
			{
				if ( pnSrc[ni] < pnSrc[ni + 1L] )
				{
					pnDst[ni] = pnSrc[ni];	/* Median��ni */
				}
				else
				{
					pnDst[ni] = pnSrc[ni + 1L];	/* Median��ni+1 */
				}
			}
		}
		else
		{
			if ( pnSrc[ni - 1L] < pnSrc[ni + 1L] )
			{
				pnDst[ni] = pnSrc[ni - 1L];	/* Median��ni-1 */
			}
			else
			{
				if ( pnSrc[ni] < pnSrc[ni + 1L] )
				{
					pnDst[ni] = pnSrc[ni + 1L];	/* Median��ni+1 */
				}
				else
				{
					pnDst[ni] = pnSrc[ni];	/* Median��ni */
				}
			}
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�\�[�g
 *
 * @param[out]		pnValue					:,�\�[�g�Ώۂ̃f�[�^�z��ւ̃|�C���^,-,[-],
 * @param[in]		nNumValue				:,�\�[�g�Ώۂ̃f�[�^�z��,0<value,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.24	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_Sort( slong* pnValue, slong nNumValue )
{
	slong	ni, nj;
	slong	nTempValue;

	/* �����`�F�b�N */
	if ( NULL == pnValue )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 0L; ni < ( nNumValue - 1L ); ni++ )
	{
		for ( nj = ( nNumValue - 2L ); nj >= ni; nj-- )
		{
			if ( pnValue[nj] > pnValue[nj + 1L] )
			{
				nTempValue = pnValue[nj];
				pnValue[nj] = pnValue[nj + 1L];
				pnValue[nj + 1L] = nTempValue;
			}
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�̈���̉�f�l�i�[
 *
 * @param[in]		ptImgAccessInfo			:,�摜�������A�N�Z�X���ւ̃|�C���^(SRC),-,[-],
 * @param[in,out]	ptStore					:,�摜�������A�N�Z�X���ւ̃|�C���^(DST),-,[-],
 * @param[in,out]	nScanDirX				:,�X�L��������(X����),CB_FPE_SCAN_DIR_X_PLUS<=value<=CB_FPE_SCAN_DIR_X_MINUS,[-],
 * @param[in,out]	nScanDirY				:,�X�L��������(Y����),CB_FPE_SCAN_DIR_Y_PLUS<=value<=CB_FPE_SCAN_DIR_Y_MINUS,[-],
 * @param[in,out]	nShuffleMode			:,X/Y�����̃V���b�t�����[�h,CB_FPE_NO_SHUFFLE<=value<=CB_FPE_SHUFFLE,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.12	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_StorePixelValue( const CB_FPE_IMG_ACCESS_INFO* const ptImgAccessInfo, const CB_FPE_IMG_ACCESS_INFO* const ptStore, enum enum_CB_FPE_SCAN_DIR_X nScanDirX, enum enum_CB_FPE_SCAN_DIR_Y nScanDirY, enum enum_CB_FPE_SHUFFLE_MODE nShuffleMode )
{
	slong	nx, ny;
	slong	nxSrc, nySrc;
	slong	nXSizeRgn, nYSizeRgn;
	slong	nCoeffDirX, nCoeffDirY;
	slong	nSX, nEX, nSY, nEY;
	uchar	*pnTopPixelSrc, *pnTopPixelDst, *pnPixelDst;

	/* �����`�F�b�N */
	if ( ( NULL == ptImgAccessInfo ) || ( NULL == ptStore ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �̈�̃T�C�Y */
	nXSizeRgn = ( ptImgAccessInfo->ptRgn->nEX - ptImgAccessInfo->ptRgn->nSX ) + 1L;
	nYSizeRgn = ( ptImgAccessInfo->ptRgn->nEY - ptImgAccessInfo->ptRgn->nSY ) + 1L;

	/* �擪�|�C���^ */
	pnTopPixelSrc = (uchar*)( ptImgAccessInfo->pnAddrY8 );
	pnTopPixelDst = (uchar*)( ptStore->pnAddrY8 );

	if ( CB_FPE_SCAN_DIR_X_PLUS == nScanDirX )
	{
		nSX = 0L;
		nEX = nXSizeRgn - 1L;
		nCoeffDirX = (+1);
	}
	else if ( CB_FPE_SCAN_DIR_X_MINUS == nScanDirX )
	{
		nSX = nXSizeRgn - 1L;
		nEX = 0L;
		nCoeffDirX = (-1);
	}
	else
	{
		/* �G���[ */
		return ( CB_IMG_NG );
	}
	if ( CB_FPE_SCAN_DIR_Y_PLUS == nScanDirY )
	{
		nSY = 0L;
		nEY = nYSizeRgn - 1L;
		nCoeffDirY = (+1);
	}
	else if ( CB_FPE_SCAN_DIR_Y_MINUS == nScanDirY )
	{
		nSY = nYSizeRgn - 1L;
		nEY = 0L;
		nCoeffDirY = (-1);
	}
	else
	{
		/* �G���[ */
		return ( CB_IMG_NG );
	}

	if ( CB_FPE_NO_SHUFFLE == nShuffleMode )
	{
		nySrc = nSY;
		for ( ny = 0L; ny < nYSizeRgn; ny++ )
		{
			nxSrc = nSX;
			pnPixelDst = pnTopPixelDst + ( ny * ptStore->nXSize );
			for ( nx = 0L; nx < nXSizeRgn; nx++ )
			{
				*pnPixelDst = *( pnTopPixelSrc + ( ( ( ptImgAccessInfo->ptRgn->nSY + nySrc ) * ptImgAccessInfo->nXSize ) + ( ptImgAccessInfo->ptRgn->nSX + nxSrc ) ) );

				/* ���̃A�N�Z�X��� */
				pnPixelDst++;
				nxSrc = nxSrc + nCoeffDirX;
			}

			/* ���̃A�N�Z�X��� */
			nySrc = nySrc + nCoeffDirY;
		}
	}
	else if ( CB_FPE_SHUFFLE == nShuffleMode )
	{
		nxSrc = nSX;
		for ( ny = 0L; ny < nXSizeRgn; ny++ )
		{
			nySrc = nSY;
			pnPixelDst = pnTopPixelDst + ( ny * ptStore->nXSize );
			for ( nx = 0L; nx < nYSizeRgn; nx++ )
			{
				*pnPixelDst = *( pnTopPixelSrc + ( ( ptImgAccessInfo->ptRgn->nSY + nySrc ) * ptImgAccessInfo->nXSize ) + ( ptImgAccessInfo->ptRgn->nSX + nxSrc ) );

				/* ���̃A�N�Z�X��� */
				pnPixelDst++;
				nySrc = nySrc + nCoeffDirY;
			}

			/* ���̃A�N�Z�X��� */
			nxSrc = nxSrc + nCoeffDirX;
		}
	}
	else
	{
		/* �G���[ */
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			���C���t�B���^
 *
 * @param[in]		nImgSrc					:,�摜�������A�N�Z�X���(SRC)�ւ̃|�C���^,-,[-],
 * @param[in,out]	ptEdgeDst				:,�G�b�W�摜�������A�N�Z�X���(DST)�ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.13	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_LineEdgeFLT( const CB_FPE_IMG_ACCESS_INFO* const ptImgSrc, const CB_FPE_IMG_ACCESS_INFO* const ptEdgeDst )
{
	slong	ni;
	slong	nx, ny;
	slong	nXSize, nYSize;
	slong	nCoeffEdgeFLT[CB_FPE_PRM_SIZE_LINE_FLT] = CB_FPE_PRM_COEFF_LINE_FLT;
	slong	nSizeNoUse;
	slong	nSum;
	uchar	*pnAddrPixelSrc;
	sshort	*pnAddrPixelDst;
	CB_RECT_RGN	*ptProcRgn;

	if ( ( NULL == ptImgSrc ) || ( NULL == ptEdgeDst ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nSizeNoUse = CB_FPE_PRM_PERIPHERAL_NO_USE;

	/* �擪�|�C���^ */
	ptProcRgn = ptImgSrc->ptRgn;

	/* �̈�T�C�Y */
	nXSize = ( ptProcRgn->nEX - ptProcRgn->nSX ) + 1L;
	nYSize = ( ptProcRgn->nEY - ptProcRgn->nSY ) + 1L;

	for ( ny = 0L; ny < nYSize; ny++ )
	{
		pnAddrPixelSrc = (uchar*)( ptImgSrc->pnAddrY8 + ( ( ptProcRgn->nSY + ny ) * ptImgSrc->nXSize ) + ( ptProcRgn->nSX + nSizeNoUse ) );
		pnAddrPixelDst = ptEdgeDst->pnAddrY16 + ( ny * ptEdgeDst->nXSize ) + nSizeNoUse;
			
		for ( nx = nSizeNoUse; nx < ( nXSize - nSizeNoUse ); nx++ )
		{
			nSum = 0L;
			for ( ni = -nSizeNoUse; ni <= nSizeNoUse; ni++ )
			{
				nSum += ( nCoeffEdgeFLT[nSizeNoUse + ni] * (slong)(*(pnAddrPixelSrc + ni)) );	// MISRA-C����̈�E [EntryAVM_QAC#3] O3.1  , ID-6855
			}
			
			/* �l�i�[ */
			*pnAddrPixelDst = (sshort)nSum;

			/* ���̃A�h���X */
			pnAddrPixelSrc++;
			pnAddrPixelDst++;
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�G�b�W�_���o
 *
 * @param[in]		ptEdgeSrc				:,�G�b�W�摜�������A�N�Z�X���ւ̃|�C���^,-,[-],
 * @param[in,out]	ptPixelInfo				:,�摜�������A�N�Z�X���ւ̃|�C���^,-,[-],
 * @param[in]		ptEdgePnt				:,�G�b�W�ʒu�ւ̃|�C���^,[-],
 * @param[in,out]	pnNumEdgePnt			:,�G�b�W�̐����i�[����̈�ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.14	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePoint( const CB_FPE_IMG_ACCESS_INFO* const ptEdgeSrc, const CB_FPE_IMG_ACCESS_INFO* ptPixelInfo, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt )
{
	slong	nx, ny;
	slong	nXSize, nYSize;
	slong	nMaxEdgeRho, nxMax;
	slong	nNumEdgePnt;
	slong	nCntDown;
	sshort	*pnAddrEdgeRho;
	slong	*pnMedian;
	uchar	*pnPixelValue;
	/* �p�����[�^ */
	slong	nprm_thrCntDown;
	slong	nprm_thrEdgeRho;

	/* �����`�F�b�N */
	if ( ( NULL == ptEdgeSrc ) || ( NULL == ptPixelInfo ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}
	
	/* �p�����[�^ */
	nprm_thrEdgeRho = CB_FPE_PRM_THR_EDGE_RHO;
	nprm_thrCntDown = CB_FPE_PRM_THR_EDGE_CNT_DOWN;
	
	/* �̈�T�C�Y */
	nXSize = ( ptEdgeSrc->ptRgn->nEX - ptEdgeSrc->ptRgn->nSX ) + 1L;
	nYSize = ( ptEdgeSrc->ptRgn->nEY - ptEdgeSrc->ptRgn->nSY ) + 1L;

	/* �擪�|�C���^ */
	pnPixelValue = (uchar*)&( ptPixelInfo->pnAddrY8 );
	pnMedian = &(m_FPE_tWorkInfo.nTblMedian4EdgeOfPtn[0]);

	nNumEdgePnt = 0L;

	for ( ny = 0L; ny < nYSize; ny++ )
	{
		pnAddrEdgeRho = ( ptEdgeSrc->pnAddrY16 + ( ( ( ptEdgeSrc->ptRgn->nSY + ny ) * ptEdgeSrc->nXSize ) + ptEdgeSrc->ptRgn->nSX ) );
			
		nxMax = -1;
		nMaxEdgeRho = nprm_thrEdgeRho;
		nCntDown = 0L;
		for ( nx = 0L; nx < nXSize; nx++ )
		{
			/* �o�^�ς݂̃G�b�W���x���傫���G�b�W���x�� */
			if ( nMaxEdgeRho < (*pnAddrEdgeRho) )
			{
				nMaxEdgeRho = (*pnAddrEdgeRho);
				nxMax = nx;

				nCntDown = 0L;
			}
			/* �ő�G�b�W���x���o�^�ς� AND ���ڂ̃G�b�W���x���ő�G�b�W���x��菬���� */
			else if ( ( nprm_thrEdgeRho < nMaxEdgeRho ) && ( nMaxEdgeRho > (*pnAddrEdgeRho) ) )
			{
				nCntDown++;
			}
			else
			{
				/* �������Ȃ� */;
			}

			/* �ő�G�b�W���x����A�����ăG�b�W���x���������Ă��邩 */
			if ( nprm_thrCntDown <= nCntDown )
			{
				break;
			}

			/* ���̃A�h���X */
			pnAddrEdgeRho++;
		}

		if ( 0L <= nxMax )
		{
			ptEdgePnt[nNumEdgePnt].nX = nxMax;
			ptEdgePnt[nNumEdgePnt].nY = ny;
			nNumEdgePnt++;
		}
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�G�b�W�_���o(�����x�ʒu����)
 *
 * @param[in]		ptImgSrc				:,�\�[�X�摜�������A�N�Z�X���ւ̃|�C���^,-,[-],
 * @param[in]		ptEdgePnt				:,�G�b�W�ʒu�ւ̃|�C���^,-,[-],
 * @param[in]		nNumEdgePnt				:,�G�b�W�̐�,0<value,[-],
 * @param[out]		ptEdgePrecPnt			:,�G�b�W�ʒu(�����x)�ւ̃|�C���^,-,[-],
 * @param[in]		nFltMode				:,�G�b�W���,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE,[-],
 * @param[in]		nScanDirX				:,�X�L��������(X����),CB_FPE_SCAN_DIR_X_PLUS<=value<=CB_FPE_SCAN_DIR_X_MINUS,[-],
 * @param[in]		nScanDirY				:,�X�L��������(Y����),CB_FPE_SCAN_DIR_Y_PLUS<=value<=CB_FPE_SCAN_DIR_Y_MINUS,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.21	K.Kato		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePrecisePoint( const CB_FPE_IMG_ACCESS_INFO* const ptImgSrc, const CB_IMG_POINT* const ptEdgePnt, slong nNumEdgePnt, t_cb_img_CenterPos* ptEdgePrecPnt, slong* pnNumEdgePrecPnt, enum enum_CB_FPE_LINE_FLT_MODE nFltMode, enum enum_CB_FPE_SCAN_DIR_X nScanDirX, enum enum_CB_FPE_SCAN_DIR_Y nScanDirY )
{
	slong	nRet = CB_IMG_NG;
	slong	ni = 0L, 
			nj = 0L;
	slong	nPntX = 0L, 
			nPntY = 0L;
	slong	nPntXBlack = 0L, 
			nPntYBlack = 0L;
	slong	nCoeffDirX = 0L, 
			nCoeffDirY = 0L;
	slong	nIntensityBlack = 0L, 
			nIntensityWhite = 0L;
	slong	nIdxMedian = 0L;
	slong	nDistanceIntensity = 0L;
	slong	nDistancePixel = 0L;
	slong	nPixelValueCur = 0L, 
			nPixelValuePrev = 0L;
	slong	nNumEdgePrecPnt = 0L;
	slong	nTblIntensityBlack[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM] = { 0L }, 
			nTblIntensityWhite[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM] = { 0L };
	double_t	dDistanceIntensityBlack = 0.0, 
				dDistanceIntensityWhite = 0.0;
	double_t	dThrIntensity = 0.0;
	double_t	dRatioBlack = 0.0, 
				dRatioWhite = 0.0;
	uchar	*pnTopPixel = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == ptImgSrc ) || ( NULL == ptEdgePnt ) || ( NULL == ptEdgePrecPnt ) || ( NULL == pnNumEdgePrecPnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	pnTopPixel = (uchar*)( ptImgSrc->pnAddrY8 );

	if ( CB_FPE_LINE_FLT_HORI_EDGE == nFltMode )
	{
		if ( CB_FPE_SCAN_DIR_Y_PLUS == nScanDirY )
		{
			nCoeffDirY = (+1L);
		}
		else if ( CB_FPE_SCAN_DIR_Y_MINUS == nScanDirY )
		{
			nCoeffDirY = (-1L);
		}
		else
		{
			/* �G���[ */
			return ( CB_IMG_NG );
		}

		/* <<<<<�������l����>>>>> */
		/* ��f�l�̎擾 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntX = ptEdgePnt[ni].nX;

			/* ���̈摤 */
			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 6L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[1] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			/* ���̈摤 */
			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 5L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[2] */	// MISRA-C����̈�E [EntryAVM_QAC#4] O4.1  R-4, ID-6853
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}
		}
		/* SORT */
		nRet = cb_FPE_Sort( &(nTblIntensityBlack[0L]), nNumEdgePnt );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		nRet = cb_FPE_Sort( &(nTblIntensityWhite[0L]), nNumEdgePnt );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		/* ��\�l */
		nIdxMedian = nNumEdgePnt / 2L;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];
		/* �������l */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<�T�u�s�N�Z������>>>>> */
		nNumEdgePrecPnt = 0L;
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntYBlack = -1L;
			if ( nCoeffDirY < 0L )
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[1],[2]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirY�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				for ( nj = ( ptEdgePnt[ni].nY - ( nCoeffDirY * 3L ) ); nj >= ( ptEdgePnt[ni].nY + ( nCoeffDirY * 3L ) ); nj = nj + nCoeffDirY )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ( nj - ( nCoeffDirY * 1L ) ) * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( nj * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						nPntYBlack =  nj - ( nCoeffDirY * 1L );
						break;
					}
				}
			}
			else if ( 0L < nCoeffDirY )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6861	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6862
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[1],[2]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirY�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				for ( nj = ( ptEdgePnt[ni].nY - ( nCoeffDirY * 3L ) ); nj <= ( ptEdgePnt[ni].nY + ( nCoeffDirY * 3L ) ); nj = nj + nCoeffDirY )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ( nj - ( nCoeffDirY * 1L ) ) * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( nj * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						nPntYBlack =  nj - ( nCoeffDirY * 1L );
						break;
					}
				}
			}
			else
			{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6863
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntYBlack )
			{
				nDistanceIntensity = ( nPixelValueCur - nPixelValuePrev );
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrev;
				dDistanceIntensityWhite = (double_t)nPixelValueCur - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0���`�F�b�N�ǉ� */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0�����̏��� */
					/* �����ł�0�����̓K�؂ȏ����ɂ��ẮA�v�Č��� */
					/* 
					   => ���L������0.5�Ƃ���B
					      nPixelValueCur��nPixelValuePrev�͗אڂ��Ă���̂ŁA�����P�x�l�̏ꍇ�́A���̒��Ԉʒu�Ƃ���
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* �����x */
				ptEdgePrecPnt[nNumEdgePrecPnt].w = (double_t)ptEdgePnt[ni].nX;
				ptEdgePrecPnt[nNumEdgePrecPnt].h = (double_t)nPntYBlack + ( (double_t)nCoeffDirY * ( ( dRatioBlack * (double_t)nDistancePixel ) /*- 1.0*/ ) );

				nNumEdgePrecPnt++;
			}
		}

	}
	else if ( CB_FPE_LINE_FLT_VERT_EDGE == nFltMode )
	{

		if ( CB_FPE_SCAN_DIR_X_PLUS == nScanDirX )
		{
			nCoeffDirX = (+1L);
		}
		else if ( CB_FPE_SCAN_DIR_X_MINUS == nScanDirX )
		{
			nCoeffDirX = (-1L);
		}
		else
		{
			/* �G���[ */
			return ( CB_IMG_NG );
		}

		/* <<<<<�������l����>>>>> */
		/* ��f�l�̎擾 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntY = ptEdgePnt[ni].nY;

			/* ���̈摤 */
			nPntX = ptEdgePnt[ni].nX - ( nCoeffDirX * 6L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[3] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			/* ���̈摤 */
			nPntX = ptEdgePnt[ni].nX + ( nCoeffDirX * 5L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[4] */	// MISRA-C����̈�E [EntryAVM_QAC#4] O4.1  R-4, ID-6857
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}
		}
		/* SORT */
		nRet = cb_FPE_Sort( &(nTblIntensityBlack[0L]), nNumEdgePnt );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		nRet = cb_FPE_Sort( &(nTblIntensityWhite[0L]), nNumEdgePnt );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		/* ��\�l */
		nIdxMedian = nNumEdgePnt / 2L;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];
		/* �������l */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<�T�u�s�N�Z������>>>>> */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntXBlack = -1L;
			if ( nCoeffDirX < 0L )
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[3],[4]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirX�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				for ( nj = ( ptEdgePnt[ni].nX - ( nCoeffDirX * 3L ) ); nj >= ( ptEdgePnt[ni].nX + ( nCoeffDirX * 3L ) ); nj = nj + nCoeffDirX )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + ( nj - ( nCoeffDirX * 1L ) ) ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + nj ));

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						nPntXBlack =  nj - ( nCoeffDirX * 1L );
						break;
					}
				}
			}
			else if ( 0L < nCoeffDirX )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6866	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6867
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[3],[4]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirX�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				for ( nj = ( ptEdgePnt[ni].nX - ( nCoeffDirX * 3L ) ); nj <= ( ptEdgePnt[ni].nX + ( nCoeffDirX * 3L ) ); nj = nj + nCoeffDirX )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + ( nj - ( nCoeffDirX * 1L ) ) ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + nj ));

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						nPntXBlack =  nj - ( nCoeffDirX * 1L );
						break;
					}
				}
			}
			else
			{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6868
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntXBlack )
			{
				nDistanceIntensity = ( nPixelValueCur - nPixelValuePrev );
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrev;
				dDistanceIntensityWhite = (double_t)nPixelValueCur - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0���`�F�b�N�ǉ� */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0�����̏��� */
					/* �����ł�0�����̓K�؂ȏ����ɂ��ẮA�v�Č��� */
					/* 
					   => ���L������0.5�Ƃ���B
					      nPixelValueCur��nPixelValuePrev�͗אڂ��Ă���̂ŁA�����P�x�l�̏ꍇ�́A���̒��Ԉʒu�Ƃ���
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* �����x */
				ptEdgePrecPnt[nNumEdgePrecPnt].w = (double_t)nPntXBlack + ( (double_t)nCoeffDirX * ( ( dRatioBlack * (double_t)nDistancePixel ) /*- 1.0*/ ) );
				ptEdgePrecPnt[nNumEdgePrecPnt].h = (double_t)ptEdgePnt[ni].nY;

				nNumEdgePrecPnt++;
			}
		}
	}
	else
	{
		/* �G���[ */
		return ( CB_IMG_NG );
	}

	/* �����x�ŋ��߂�ꂽ�_���i�[ */
	*pnNumEdgePrecPnt = nNumEdgePrecPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		���`��A
 *
 * @param[in]	ptPoint				:,�_���,-,[-],
 * @param[in]	nNumPoint			:,�_�̐�,0<=value<?,[-],
 * @param[out]	pdSlope				:,�����̌X���i�[��,-,[-],
 * @param[out]	pdIntercept			:,�����̐ؕЊi�[��,-,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.14	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_LinearRegression4VertLine( const t_cb_img_CenterPos* const ptPoint, slong nNumPoint, double_t* pdSlope, double_t* pdIntercept )
{
	slong	ni;
	double_t	dSum_YX, dSum_X, dSum_Y, dSum_Ypower2;
	double_t	dDenominator, dNumerator;

	/* �����`�F�b�N */
	if ( ( NULL == ptPoint ) || ( NULL == pdSlope ) || ( NULL == pdIntercept ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	if ( 2L <= nNumPoint )
	{
		dSum_X = 0.0;
		dSum_Y = 0.0;
		dSum_YX = 0.0;
		dSum_Ypower2 = 0.0;
		for ( ni = 0L; ni < nNumPoint; ni++ )
		{
			dSum_YX = dSum_YX + ( ptPoint[ni].w * ptPoint[ni].h );
			dSum_X = dSum_X + ptPoint[ni].w;
			dSum_Y = dSum_Y + ptPoint[ni].h;
			dSum_Ypower2 = dSum_Ypower2 + ( ptPoint[ni].h * ptPoint[ni].h );
		}

		dNumerator = ( (double_t)nNumPoint * dSum_YX ) - ( dSum_X * dSum_Y );
		dDenominator = ( (double_t)nNumPoint * dSum_Ypower2 ) - ( dSum_Y * dSum_Y );

		/* �X���E�ؕ� */
		if ( CB_FPE_DBL_MIN <= fabs( dDenominator ) )
		{
			*pdSlope = dNumerator / dDenominator;
			*pdIntercept = ( dSum_X - ( (*pdSlope) * dSum_Y ) ) / (double_t)nNumPoint;
		}
		else
		{
			/* X���ɐ����Ȓ��� */
			*pdSlope = 0.0;
			*pdIntercept = 0.0;
			return ( CB_IMG_NG_DIV0 );
		}
	}
	else
	{
		/* 0�����̏��� */
		/* �����ł�0�����̓K�؂ȏ����ɂ��ẮA�v�Č����i����CB_IMG_NG��Ԃ��j */
		/* 
		   => �{�֐��Ƃ��ẮA�v�Z�ł��Ȃ��̂ł����NG��Ԃ��݂̂Ƃ���B
		      NG���̑Ή��͏�ʂɂčs�����ƂƂ���B
			  (2013.08.07 K.Kato)
	    */
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		���`��A
 *
 * @param[in]	ptPoint			:,�_���,-,[-],
 * @param[in]	nNumPoint		:,�_�̐�,0<=value<?,[-],
 * @param[out]	pdSlope			:,�����̌X���i�[��,-,[-],
 * @param[out]	pdIntercept		:,�����̐ؕЊi�[��,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.14	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_LinearRegression4HoriLine( const t_cb_img_CenterPos* const ptPoint, slong nNumPoint, double_t* pdSlope, double_t* pdIntercept )
{
	slong	ni;
	double_t	dSum_XY, dSum_X, dSum_Y, dSum_Xpower2;
	double_t	dDenominator, dNumerator;

	/* �����`�F�b�N */
	if ( ( NULL == ptPoint ) || ( NULL == pdSlope ) || ( NULL == pdIntercept ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	if ( 2L <= nNumPoint )	/* 0���`�F�b�N�ǉ� */
	{
		dSum_X = 0.0;
		dSum_Y = 0.0;
		dSum_XY = 0.0;
		dSum_Xpower2 = 0.0;
		for ( ni = 0L; ni < nNumPoint; ni++ )
		{
			dSum_XY = dSum_XY + ( ptPoint[ni].w * ptPoint[ni].h );
			dSum_X = dSum_X + ptPoint[ni].w;
			dSum_Y = dSum_Y + ptPoint[ni].h;
			dSum_Xpower2 = dSum_Xpower2 + ( ptPoint[ni].w * ptPoint[ni].w );
		}

		dNumerator = ( (double_t)nNumPoint * dSum_XY ) - ( dSum_X * dSum_Y );
		dDenominator = ( (double_t)nNumPoint * dSum_Xpower2 ) - ( dSum_X * dSum_X );

		/* �X���E�ؕ� */
		if ( CB_FPE_DBL_MIN <= fabs( dDenominator ) )
		{
			*pdSlope = dNumerator / dDenominator;
			*pdIntercept = ( dSum_Y - ( (*pdSlope) * dSum_X ) ) / (double_t)nNumPoint;
		}
		else
		{
			/* X���ɐ����Ȓ��� */
			*pdSlope = 0.0;
			*pdIntercept = 0.0;
			return ( CB_IMG_NG_DIV0 );
		}
	}
	else
	{
		/* 0�����̏��� */
		/* �����ł�0�����̓K�؂ȏ����ɂ��ẮA�v�Č����i����CB_IMG_NG��Ԃ��j */
		/* 
		   => �{�֐��Ƃ��ẮA�v�Z�ł��Ȃ��̂ł����NG��Ԃ��݂̂Ƃ���B
		      NG���̑Ή��͏�ʂɂčs�����ƂƂ���B
			  (2013.08.07 K.Kato)
	    */
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����̌�_
 *
 * @param[in]	ptLineCoeff1	:,�����W���\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptLineCoeff2	:,�����W���\���̂ւ̃|�C���^,-,[-],
 * @param[out]	pdIntercept		:,�����̌�_�i�[��,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.14	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcIntersection( const CB_FPE_LINE_COEFF* const ptLineCoeff1, const CB_FPE_LINE_COEFF* const ptLineCoeff2, t_cb_img_CenterPos* ptIntersectPnt )
{
	/* �����`�F�b�N */
	if ( ( NULL == ptLineCoeff1 ) || ( NULL == ptLineCoeff2 ) || ( NULL == ptIntersectPnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	if ( CB_FPE_DBL_MIN <= fabs( ptLineCoeff1->dSlope - ptLineCoeff2->dSlope ) )	/* 0���`�F�b�N�ǉ� */
	{
		ptIntersectPnt->h = ( ptLineCoeff2->dIntercept - ptLineCoeff1->dIntercept ) / ( ptLineCoeff1->dSlope - ptLineCoeff2->dSlope );
		ptIntersectPnt->w = ( ptLineCoeff1->dSlope * ptIntersectPnt->h ) + ptLineCoeff1->dIntercept;
	}
	else
	{
		/* 0�����̏��� */
		/* ���蓾�Ȃ��p�^�[���Ȃ̂�NG��Ԃ� */
		return ( CB_IMG_NG_DIV0 );
	}
	
	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����̌X���E�ؕЂ��Z�o
 *
 * @param[in]	pfCoord1		:,������̓_1�ւ̃|�C���^,-,[-],
 * @param[in]	pfCoord2		:,������̓_2�ւ̃|�C���^,-,[-],
 * @param[out]	pfSlope			:,�����̌X���i�[��,-,[-],
 * @param[out]	pfIntercept		:,�����̐ؕЊi�[��,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.05.27	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcLineCoeff_F( const float_t* const pfCoord1, const float_t* const pfCoord2, float_t* pfSlope, float_t* pfIntercept )
{
	float_t	fDiffY = 0.0f;

	/* �����`�F�b�N */
	if ( ( NULL == pfCoord1 ) || ( NULL == pfCoord2 ) || ( NULL == pfSlope ) || ( NULL == pfIntercept ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* Y�����̕ω��� */
	fDiffY = pfCoord1[1L] - pfCoord2[1L];
	/* �X�� */
	if ( CB_FPE_FLT_MIN <= fabsf( fDiffY ) )	/* 0����`�F�b�N */
	{
		*pfSlope = ( pfCoord1[0L] - pfCoord2[0L] ) / fDiffY;
	}
	else
	{
		*pfSlope = 0.0f;
	}
	/* �ؕ� */
	*pfIntercept = pfCoord1[0L] - ( (*pfSlope) * pfCoord1[1L] );

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�������ʃf�[�^�̊i�[
 *
 * @param[in]	ptSrchRsltInfo		:,�T�����ʏ��\���̂ւ̃|�C���^,-,[-],
 * @param[out]	ptCenterPos			:,�����_���o���ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgn			:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.18	K.Kato			�V�K�쐬
 *
 * @note		����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_StoreResultData( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo, t_cb_img_CenterPosEx* const ptCenterPos, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgn, slong nCamDirection )
{
	slong							nRet = CB_IMG_OK;
	slong							nNumPnt = 0L;
	slong							nSignHaarLikeFeature;
	CB_FPE_SRCH_RSLT_PNT_INFO		*ptSrchRsltPnt = NULL;
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO	*ptSrchRsltAddPnt = NULL;

	CB_FPE_SRCH_RSLT_PNT_INFO		*ptSrchRsltAddChkPnt = NULL;
	enum enum_CB_IMG_PATTERN_TYPE	nPtnType_Left = CB_IMG_PATTERN_TYPE_NONE ,
									nPtnType_Right = CB_IMG_PATTERN_TYPE_NONE;
	enum enum_CB_IMG_ALGORITHM				nTypeAlgorithm;
	/* �p�����[�^ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;

	/* �����`�F�b�N */
	if ( ( NULL == ptSrchRsltInfo ) || ( NULL == ptCenterPos ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* �擪�|�C���^ */
	ptSrchRsltPnt = &(ptSrchRsltInfo->tSrchRsltPntInfo[0]);
	ptSrchRsltAddPnt = &(ptSrchRsltInfo->tSrchRsltAddPntInfo[0]);
	ptSrchRsltAddChkPnt = &(ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]);

	/* Center */
	nNumPnt = 0L;
	if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
	{
		( ptCenterPos->Center )[0] = ( ptSrchRsltPnt->tTPntUpperPrecise )[1];
		( ptCenterPos->Center )[1] = ( ptSrchRsltPnt->tCrossPntPrecise )[1];
		( ptCenterPos->Center )[2] = ( ptSrchRsltPnt->tTPntLowerPrecise )[1];
		( ptCenterPos->Center )[3] = ( ptSrchRsltPnt->tTPntUpperPrecise )[0];
		( ptCenterPos->Center )[4] = ( ptSrchRsltPnt->tCrossPntPrecise )[0];
		( ptCenterPos->Center )[5] = ( ptSrchRsltPnt->tTPntLowerPrecise )[0];
		( ptCenterPos->Center )[6] = ( ptSrchRsltPnt->tTPntUpperPrecise )[2];
		( ptCenterPos->Center )[7] = ( ptSrchRsltPnt->tCrossPntPrecise )[2];
		( ptCenterPos->Center )[8] = ( ptSrchRsltPnt->tTPntLowerPrecise )[2];
	}
	else if ( ( E_CB_SELFCALIB_CAMPOS_LEFT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_RIGHT == nCamDirection ) )
	{
		if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ptSrchRgn->tSrchRgnInfo4Pattern[0].tPtnInfo.nTypePattern )
		{
			( ptCenterPos->Center )[0] = ( ptSrchRsltPnt->tTPntUpperPrecise )[1];
			( ptCenterPos->Center )[1] = ( ptSrchRsltPnt->tCrossPntPrecise )[0];
			( ptCenterPos->Center )[2] = ( ptSrchRsltPnt->tTPntLowerPrecise )[0];
			( ptCenterPos->Center )[3] = ( ptSrchRsltPnt->tTPntUpperPrecise )[0];
			( ptCenterPos->Center )[4] = ( ptSrchRsltPnt->tTPntLeftPrecise )[0];
			( ptCenterPos->Center )[5] = ( ptSrchRsltPnt->tTPntRightPrecise )[0];
			( ptCenterPos->Center )[6] = ( ptSrchRsltPnt->tTPntLowerPrecise )[1];
		}
		else
		{
			nRet = cb_FPE_GetSignHaarLikeFeature( &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ), nCamDirection, &nSignHaarLikeFeature );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}

			/* �A���S���Y���^�C�v */
			nRet = cb_FPE_GetTypeAlgorithm( &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ), &nTypeAlgorithm );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
			
			if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
			{
				if ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					( ptCenterPos->Center )[0] = ( ptSrchRsltPnt->tTPntUpperPrecise )[1];
					( ptCenterPos->Center )[1] = ( ptSrchRsltPnt->tCrossPntPrecise )[0];	
					( ptCenterPos->Center )[2] = ( ptSrchRsltPnt->tTPntLowerPrecise )[0];
					( ptCenterPos->Center )[3] = ( ptSrchRsltPnt->tTPntUpperPrecise )[0];
					( ptCenterPos->Center )[4] = ( ptSrchRsltPnt->tTPntLeftPrecise )[0];
					( ptCenterPos->Center )[5] = ( ptSrchRsltPnt->tTPntUpperPrecise )[2];
					( ptCenterPos->Center )[6] = ( ptSrchRsltPnt->tCrossPntPrecise )[1];
					( ptCenterPos->Center )[7] = ( ptSrchRsltPnt->tTPntLowerPrecise )[1];
				}
				else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					( ptCenterPos->Center )[0] = ( ptSrchRsltPnt->tTPntUpperPrecise )[0];
					( ptCenterPos->Center )[1] = ( ptSrchRsltPnt->tCrossPntPrecise )[0];	
					( ptCenterPos->Center )[2] = ( ptSrchRsltPnt->tTPntLowerPrecise )[1];
					( ptCenterPos->Center )[3] = ( ptSrchRsltPnt->tTPntLeftPrecise )[0];
					( ptCenterPos->Center )[4] = ( ptSrchRsltPnt->tTPntLowerPrecise )[0];
					( ptCenterPos->Center )[5] = ( ptSrchRsltPnt->tTPntUpperPrecise )[1];
					( ptCenterPos->Center )[6] = ( ptSrchRsltPnt->tCrossPntPrecise )[1];
					( ptCenterPos->Center )[7] = ( ptSrchRsltPnt->tTPntLowerPrecise )[2];
				}
				else
				{
					return ( CB_IMG_NG );
				}
			}
			else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
			{
				if ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					( ptCenterPos->Center )[0] = ( ptSrchRsltPnt->tTPntUpperPrecise )[1];
					( ptCenterPos->Center )[1] = ( ptSrchRsltPnt->tCrossPntPrecise )[1];
					( ptCenterPos->Center )[2] = ( ptSrchRsltPnt->tTPntLowerPrecise )[1];
					( ptCenterPos->Center )[3] = ( ptSrchRsltPnt->tTPntUpperPrecise )[0];
					( ptCenterPos->Center )[4] = ( ptSrchRsltPnt->tCrossPntPrecise )[0];
					( ptCenterPos->Center )[5] = ( ptSrchRsltPnt->tTPntLowerPrecise )[0];
					( ptCenterPos->Center )[6] = ( ptSrchRsltPnt->tTPntUpperPrecise )[2];
					( ptCenterPos->Center )[7] = ( ptSrchRsltPnt->tTPntRightPrecise )[0];
				}
				else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					( ptCenterPos->Center )[0] = ( ptSrchRsltPnt->tTPntUpperPrecise )[1];
					( ptCenterPos->Center )[1] = ( ptSrchRsltPnt->tCrossPntPrecise )[1];
					( ptCenterPos->Center )[2] = ( ptSrchRsltPnt->tTPntLowerPrecise )[1];
					( ptCenterPos->Center )[3] = ( ptSrchRsltPnt->tTPntUpperPrecise )[0];
					( ptCenterPos->Center )[4] = ( ptSrchRsltPnt->tCrossPntPrecise )[0];
					( ptCenterPos->Center )[5] = ( ptSrchRsltPnt->tTPntLowerPrecise )[0];
					( ptCenterPos->Center )[6] = ( ptSrchRsltPnt->tTPntRightPrecise )[0];
					( ptCenterPos->Center )[7] = ( ptSrchRsltPnt->tTPntLowerPrecise )[2];
				}
				else
				{
					return ( CB_IMG_NG );
				}
			}
			else
			{
				return ( CB_IMG_NG );
			}
		}
	}
	else
	{
		/* �������Ȃ� */;
	}

	/* �_�� */
	ptCenterPos->CenterNum = (ulong)( ptSrchRsltPnt->nNumCrossPnt + ptSrchRsltPnt->nNumTPntLower + ptSrchRsltPnt->nNumTPntUpper + ptSrchRsltPnt->nNumTPntLeft + ptSrchRsltPnt->nNumTPntRight );

	/* �ǉ��p�^�[���̒T���̈�ݒ� */ /* �����I�ɒǉ��p�^�[���͍��E�ʂɔ��肵�č쐬���� */
	/* �p�^�[����Type���擾 */
	nPtnType_Left = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT].tPtnInfo.nTypePattern;
	nPtnType_Right = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT].tPtnInfo.nTypePattern;

	/* �ǉ��p�^�[���̔��� */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Right ) )
	{
		/* ���F�Ȃ��@�@�E�F�Ȃ��@�@�F*/
		/* NO OPERATION */
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Right ) )
	{
		/* ���F���ہ@�@�E�F���ہ@�@�F*/

		/* �ǉ��p�^�[��(Left) */
		ptCenterPos->LeftNum = (ulong)( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_LEFT].nNumCenterPnt );
		/* (Left)1�_��/2�_�� */
		( ptCenterPos->Left )[0] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_LEFT].tCenterPntPrecise )[0];
		( ptCenterPos->Left )[1] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_LEFT].tCenterPntPrecise )[1];
		/* �ǉ��p�^�[��(Right) */
		ptCenterPos->RightNum = (ulong)( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_RIGHT].nNumCenterPnt );
		/* (Right)1�_��/2�_�� */
		( ptCenterPos->Right )[0] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_RIGHT].tCenterPntPrecise )[0];
		( ptCenterPos->Right )[1] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_RIGHT].tCenterPntPrecise )[1];
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Right ) )
	{
		/* ���F�s��(2x2) �E�F�s��(2x2) �F*/

		/* �ǉ��p�^�[��(Left) */
		ptCenterPos->LeftNum = (ulong)(	  ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].nNumTPntUpper
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].nNumTPntLower
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].nNumCrossPnt);
		/* (Left)1�_��/2�_��/3�_��(���������x�_) */
		( ptCenterPos->Left )[0] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tTPntUpperPrecise )[0];
		( ptCenterPos->Left )[1] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tTPntLowerPrecise )[0];
		( ptCenterPos->Left )[2] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tCrossPntPrecise )[0];

		/* �ǉ��p�^�[��(Right) */
		ptCenterPos->RightNum = (ulong)(  ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].nNumTPntUpper
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].nNumTPntLower
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].nNumCrossPnt);
		/* (Right)1�_��/2�_��/3�_��(���������x�_) */
		( ptCenterPos->Right )[0] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].tTPntUpperPrecise )[0];
		( ptCenterPos->Right )[1] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].tTPntLowerPrecise )[0];
		( ptCenterPos->Right )[2] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].tCrossPntPrecise )[0];

#ifdef PC_DEVELOPMENT
		ptCenterPos->LeftCross.w = (double_t)ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tCrossPnt[0].nX;
		ptCenterPos->LeftCross.h = (double_t)ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tCrossPnt[0].nY;

		ptCenterPos->RightCross.w = (double_t)ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].tCrossPnt[0].nX;
		ptCenterPos->RightCross.h = (double_t)ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].tCrossPnt[0].nY;
#endif
	}
	else
	{
		/* ���E�̃p�^�[�����قȂ�ꍇ */
		/* ����ł͂��肦�Ȃ��p�^�[���Ȃ̂ŁA����̓G���[�Ƃ���B�����I�ɂ͑Ή�����B  */
		return ( CB_IMG_NG_COMBI_ADD_PATTERN );
	}

	/* �����_�Ԃ̈ʒu�֌W�`�F�b�N */
	nRet = cb_FPE_CheckFtrPointPos( ptCenterPos, ptSrchRgn, nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�����_�Ԃ̈ʒu�֌W�`�F�b�N
 *
 * @param[in,out]	ptCenterPos			:,�����_���o���ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptSrchRgn			:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK			:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.08.20			K.Kato			�V�K�쐬
 *
 * @note			����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_CheckFtrPointPos( t_cb_img_CenterPosEx* const ptCenterPos, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgn, slong nCamDirection )
{
	slong				nRet;
	slong				ni = 0L;
	slong				nFlagChkOK = CB_TRUE;
	slong				nIdxChk = 0L, 
						nIdxFtrPnt2 = 0L;
	slong				nIdxFtrPnt[CB_FPE_CENTER_PNT_MAXNUM];
	slong				nIdx;
	slong				nFtrPntValidTbl[CB_FPE_CENTER_PNT_MAXNUM];
	double_t			nAttnX = 0.0, 
						nAttnY = 0.0;
	enum enum_CB_IMG_ALGORITHM				nTypeAlgorithm;
	t_cb_img_CenterPos	tWkFtrPntPos[CB_FPE_CENTER_PNT_MAXNUM];
	CB_RECT_RGN			*ptSrchRgnChkboard = NULL;
	t_cb_img_CenterPos	*ptFtrPntPos = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == ptCenterPos ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �T���̈���ւ̃|�C���^ */
	ptSrchRgnChkboard = &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tRgnImg );

	/* �����_���ւ̃|�C���^ */
	ptFtrPntPos = &( (ptCenterPos->Center)[0] );

	/* �ʒu�֌W�`�F�b�N */
	/* �s��l���l�����ă��[�N�l��ݒ� */
	for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
	{
		/* ���W�l */
		tWkFtrPntPos[ni] = ptFtrPntPos[ni];

		/* �L���l�`�F�b�N */
		nFlagChkOK = CB_TRUE;
		if ( ( 0.0 <= ptFtrPntPos[ni].w ) && ( 0.0 <= ptFtrPntPos[ni].h ) )
		{
			/* �T���̈���O�`�F�b�N */
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( ptFtrPntPos[ni].w, ptFtrPntPos[ni].h, 
													(double_t)( ptSrchRgnChkboard->nSX ), 
													(double_t)( ptSrchRgnChkboard->nSY ), 
													(double_t)( ptSrchRgnChkboard->nEX ), 
													(double_t)( ptSrchRgnChkboard->nEY ) ) )
			{
				nFlagChkOK = CB_TRUE;
			}
			else
			{
				nFlagChkOK = CB_FALSE;
			}
		}
		else
		{
			nFlagChkOK = CB_FALSE;
		}

		/* ��L�`�F�b�N��Error�����łȂ������ꍇ */
		if ( CB_TRUE == nFlagChkOK )
		{
			nFtrPntValidTbl[ni] = CB_TRUE;
		}
		else
		{
			nFtrPntValidTbl[ni] = CB_FALSE;
		}
	}	/* for ( ni ) */

	/* Index�ݒ� */
	if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
	{
		nIdxFtrPnt[0] = CB_FPE_CHK_FTR_PNT_1;
		nIdxFtrPnt[1] = CB_FPE_CHK_FTR_PNT_2;
		nIdxFtrPnt[2] = CB_FPE_CHK_FTR_PNT_3;
		nIdxFtrPnt[3] = CB_FPE_CHK_FTR_PNT_4;
		nIdxFtrPnt[4] = CB_FPE_CHK_FTR_PNT_5;
		nIdxFtrPnt[5] = CB_FPE_CHK_FTR_PNT_6;
		nIdxFtrPnt[6] = CB_FPE_CHK_FTR_PNT_7;
		nIdxFtrPnt[7] = CB_FPE_CHK_FTR_PNT_8;
		nIdxFtrPnt[8] = CB_FPE_CHK_FTR_PNT_9;
	}
	else
	{
		if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ( ( ptSrchRgn->tSrchRgnInfo4Pattern )[0].tPtnInfo ).nTypePattern )
		{
			nIdxFtrPnt[0] = CB_FPE_CHK_FTR_PNT_SC_1;
			nIdxFtrPnt[1] = CB_FPE_CHK_FTR_PNT_SC_2;
			nIdxFtrPnt[2] = CB_FPE_CHK_FTR_PNT_SC_3;
			nIdxFtrPnt[3] = CB_FPE_CHK_FTR_PNT_SC_4;
			nIdxFtrPnt[4] = CB_FPE_CHK_FTR_PNT_SC_5;
			nIdxFtrPnt[5] = CB_FPE_CHK_FTR_PNT_SC_INVALID;
			nIdxFtrPnt[6] = CB_FPE_CHK_FTR_PNT_SC_INVALID;
			nIdxFtrPnt[7] = CB_FPE_CHK_FTR_PNT_SC_8;
			nIdxFtrPnt[8] = CB_FPE_CHK_FTR_PNT_SC_9;
		}
		else
		{

			/* �A���S���Y���^�C�v */
			nRet = cb_FPE_GetTypeAlgorithm( &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ), &nTypeAlgorithm );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
			
			nIdx = CB_FPE_CHK_FTR_PNT_SC_1;
			nIdxFtrPnt[0] = nIdx;
			nIdx++;
			nIdxFtrPnt[1] = nIdx;
			nIdx++;
			nIdxFtrPnt[2] = nIdx;
			nIdx++;
			if (   ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
				&& ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( ( ptSrchRgn->tSrchRgnInfo4Pattern )[0].tPtnInfo ).nFlagPlacement ) )
			{
				nIdxFtrPnt[3] = CB_FPE_CHK_FTR_PNT_SC_INVALID;
			}
			else
			{
				nIdxFtrPnt[3] = nIdx;
				nIdx++;
			}
			nIdxFtrPnt[4] = nIdx;
			nIdx++;
			if (   ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
				&& ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( ( ptSrchRgn->tSrchRgnInfo4Pattern )[0].tPtnInfo ).nFlagPlacement ) )
			{
				nIdxFtrPnt[5] = CB_FPE_CHK_FTR_PNT_SC_INVALID;
			}
			else
			{
				nIdxFtrPnt[5] = nIdx;
				nIdx++;
			}
			if (   ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
				&& ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( ( ptSrchRgn->tSrchRgnInfo4Pattern )[0].tPtnInfo ).nFlagPlacement ) )
			{
				nIdxFtrPnt[6] = CB_FPE_CHK_FTR_PNT_SC_INVALID;
			}
			else
			{
				nIdxFtrPnt[6] = nIdx;
				nIdx++;
			}
			nIdxFtrPnt[7] = nIdx;
			nIdx++;
			if (   ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
				&& ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( ( ptSrchRgn->tSrchRgnInfo4Pattern )[0].tPtnInfo ).nFlagPlacement ) )
			{
				nIdxFtrPnt[8] = CB_FPE_CHK_FTR_PNT_SC_INVALID;
			}
			else
			{
				nIdxFtrPnt[8] = nIdx;
				nIdx++;
			}
		}
	}

	/* �����_[2]�������l�̏ꍇ�́A�ʒu�֌W�`�F�b�N�����{���Ȃ� */
	nIdxFtrPnt2 = nIdxFtrPnt[1];
	if ( CB_TRUE == nFtrPntValidTbl[nIdxFtrPnt2] )
	{
		nIdx = 0L;

		/* �S�J�����ɑ΂��ėL���ȓ����_ */
		/* �����_[1] */
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* Y���� */
			if ( tWkFtrPntPos[nIdxFtrPnt2].h <= nAttnY )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* �����_[2] => ���̃`�F�b�N���Ɋ܂܂�Ă��邽�߁A�����I�ɂ̓`�F�b�N���Ȃ� */
		nIdx++;

		/* �����_[3] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* Y���� */
			if ( nAttnY <= tWkFtrPntPos[nIdxFtrPnt2].h )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* �����_[4] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y���� */
				if ( tWkFtrPntPos[nIdxFtrPnt2].h <= nAttnY )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}

		/* �����_[5] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* X���� */
			if ( tWkFtrPntPos[nIdxFtrPnt2].w <= nAttnX )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* �����_[6] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y���� */
				if ( nAttnY <= tWkFtrPntPos[nIdxFtrPnt2].h )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}

		/* �����_[7] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y���� */
				if ( tWkFtrPntPos[nIdxFtrPnt2].h <= nAttnY )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}

		/* �����_[8] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* X���� */
			if ( nAttnX <= tWkFtrPntPos[nIdxFtrPnt2].w )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* �����_[9] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y���� */
				if ( nAttnY <= tWkFtrPntPos[nIdxFtrPnt2].h )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}
	}
	else
	{
		/* �S�����_�𖳌������Ƃ��� */
		for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
		{
			nFtrPntValidTbl[ni] = CB_FALSE;
		}	/* for ( ni ) */
	}

	/* �`�F�b�N���ʂ��i�[ */
	for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
	{
		/* �L���l�`�F�b�N */
		if ( CB_TRUE == nFtrPntValidTbl[ni] )
		{
			ptFtrPntPos[ni] = tWkFtrPntPos[ni];
		}
		else
		{
			ptFtrPntPos[ni].w = -1.0;
			ptFtrPntPos[ni].h = -1.0;
		}
	}	/* for ( ni ) */

	return ( CB_IMG_OK );
}

#ifdef CB_FPE_CSW_ENABLE_TEST_MODE

/******************************************************************************/
/**
 * @brief		�e�X�g�f�[�^�쐬
 *
 * @param[in]	nImgID				:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	ptSrchRgnInfo		:,�T���̈���\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection		:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK			:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.07	K.Kato			�V�K�쐬
 *
 * @note		����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
*/
/******************************************************************************/
static slong cb_FPE_TEST_GenerateTestData( const IMPLIB_IMGID nImgID, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, uchar nCamDirection )
{
	slong	nRet = CB_IMG_NG;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSxWin, nSyWin, nExWin, nEyWin;
	slong	nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp;
	slong	nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo;
	CB_RECT_RGN	*ptSrchRgn, *ptSrchRgnZoomOut;
	slong	nXSizePeriRgn = 20, nYSizePeriRgn = 20;
	slong	nXSizeBlock, nYSizeBlock;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );

	nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );

	nRet = implib_SetWindow( IMPLIB_DST_WIN, 0, 0, 719, 479 );
	nRet = implib_IP_Const( nImgID, 255 );

	ptSrchRgnZoomOut = &(ptSrchRgnInfo->tSrchRgnInfo4Pattern[0].tRgnImgHLsrc);
	nRet = implib_SetWindow( IMPLIB_DST_WIN, ptSrchRgnZoomOut->nSX, ptSrchRgnZoomOut->nSY, ptSrchRgnZoomOut->nEX, ptSrchRgnZoomOut->nEY );
	nRet = implib_IP_Const( nImgID, 255 );

	nXSizeSrchRgn = ptSrchRgnZoomOut->nEX - ptSrchRgnZoomOut->nSX + 1;
	nYSizeSrchRgn = ptSrchRgnZoomOut->nEY - ptSrchRgnZoomOut->nSY + 1;

	switch ( nCamDirection )
	{
		case ( E_CB_SELFCALIB_CAMPOS_FRONT ):
			nXSizeBlock = ( nXSizeSrchRgn - ( 2 * nXSizePeriRgn ) ) / 4;
			nYSizeBlock = ( nYSizeSrchRgn - ( 2 * nYSizePeriRgn ) ) / 2;

			/* �u���b�N(0,0) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(0,2) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 2 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(1,1) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 1 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(1,3) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 3 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;

		case ( E_CB_SELFCALIB_CAMPOS_LEFT ):
		case ( E_CB_SELFCALIB_CAMPOS_RIGHT ):
			nXSizeBlock = ( nXSizeSrchRgn - ( 2 * nXSizePeriRgn ) ) / 2;
			nYSizeBlock = ( nYSizeSrchRgn - ( 2 * nYSizePeriRgn ) ) / 2;

			/* �u���b�N(0,0) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(1,1) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 1 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;
		case ( E_CB_SELFCALIB_CAMPOS_REAR ):
			nXSizeBlock = ( nXSizeSrchRgn - ( 2 * nXSizePeriRgn ) ) / 4;
			nYSizeBlock = ( nYSizeSrchRgn - ( 2 * nYSizePeriRgn ) ) / 2;

			/* �u���b�N(0,1) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 1 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(0,3) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 3 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(1,0) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* �u���b�N(1,2) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 2 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;
		default:
			break;
	}

	/* �ǉ��p�^�[��(Left) */
	ptSrchRgnZoomOut = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[0].tRgnImgRRF);
	nRet = implib_SetWindow( IMPLIB_DST_WIN, ptSrchRgnZoomOut->nSX, ptSrchRgnZoomOut->nSY, ptSrchRgnZoomOut->nEX, ptSrchRgnZoomOut->nEY );
	nRet = implib_IP_Const( nImgID, 255 );

	nXSizeSrchRgn = ptSrchRgnZoomOut->nEX - ptSrchRgnZoomOut->nSX + 1;
	nYSizeSrchRgn = ptSrchRgnZoomOut->nEY - ptSrchRgnZoomOut->nSY + 1;

	switch ( nCamDirection )
	{
		case ( E_CB_SELFCALIB_CAMPOS_FRONT ):
			nXSizeBlock = 7;
			nYSizeBlock = 7;

			/* ��̊� */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ���̊� */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ������ */
			nSxWin = nSxWinUp;
			nSyWin = nEyWinUp + ( ( nSyWinLo - nEyWinUp + 1 ) / 2 ) - 7;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;

		case ( E_CB_SELFCALIB_CAMPOS_LEFT ):
		case ( E_CB_SELFCALIB_CAMPOS_RIGHT ):
		case ( E_CB_SELFCALIB_CAMPOS_REAR ):
			nXSizeBlock = 7;
			nYSizeBlock = 7;

			/* ��̊� */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ���̊� */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ������ */
			nSxWin = nSxWinUp;
			nSyWin = nEyWinUp + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWinLo - nYSizePeriRgn;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;
		default:
			break;
	}

	/* �ǉ��p�^�[��(Right) */
	ptSrchRgnZoomOut = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[1].tRgnImgRRF);
	nRet = implib_SetWindow( IMPLIB_DST_WIN, ptSrchRgnZoomOut->nSX, ptSrchRgnZoomOut->nSY, ptSrchRgnZoomOut->nEX, ptSrchRgnZoomOut->nEY );
	nRet = implib_IP_Const( nImgID, 255 );

	nXSizeSrchRgn = ptSrchRgnZoomOut->nEX - ptSrchRgnZoomOut->nSX + 1;
	nYSizeSrchRgn = ptSrchRgnZoomOut->nEY - ptSrchRgnZoomOut->nSY + 1;

	switch ( nCamDirection )
	{
		case ( E_CB_SELFCALIB_CAMPOS_FRONT ):
			nXSizeBlock = 7;
			nYSizeBlock = 7;

			/* ��̊� */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ���̊� */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ������ */
			nSxWin = nSxWinUp;
			nSyWin = nEyWinUp + ( ( nSyWinLo - nEyWinUp + 1 ) / 2 ) - 7;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;

		case ( E_CB_SELFCALIB_CAMPOS_LEFT ):
		case ( E_CB_SELFCALIB_CAMPOS_RIGHT ):
		case ( E_CB_SELFCALIB_CAMPOS_REAR ):
			nXSizeBlock = 7;
			nYSizeBlock = 7;

			/* ��̊� */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ���̊� */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ������ */
			nSxWin = nSxWinUp;
			nSyWin = nEyWinUp + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWinLo - nYSizePeriRgn;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			break;
		default:
			break;
	}

//	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, 719, 479 );
//	nRet = implib_SaveBMPFile( nImgID, "C:/work/work_kato/07_EntryAVM/SELF_CB_DBG_IMG_TEST_DATA.bmp", IMPLIB_BW_BITMAP );

	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

#endif /* CB_FPE_CSW_ENABLE_TEST_MODE */


#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY											/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */
/******************************************************************************/
/**
 * @brief		�ǉ��p�^�[��_�P�x�̗ގ�����p�����̗p����
 *
 * @param[in]	
 * @param[out]	
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		-1				:,�ُ�I��,value=-1,[-],
 *
 * @date		2013.08.02	S.Suzuki			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcBrightnessSimilarity( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_INPUT* ptBrInput, float_t* pfSimilarity, float_t* pfContrast )
{
	CB_FPE_CBR_BR_INFO		tBrInfoTbl;
	slong					nRet = CB_IMG_NG;
	slong					ni = 0L;
	slong					nDiffBrMode = 0L;
	slong					nUpperModeID, nLowerModeID;
	slong					nHistBinNum;
	float_t					fUpperMode, fLowerMode;
	float_t					fDenom	=0.0F,			/* ���K�����ݑ��ց@���q */
							fNumerUpper	=0.0F,		/* ���K�����ݑ��ց@����(��[�����̈�̕W���΍�) */
							fNumerLower	=0.0F;		/* ���K�����ݑ��ց@����(���[�����̈�̕W���΍�) */
	float_t					fNumer = 0.0F;			/* ���K�����ݑ��ց@���� */
	float_t					fContrastUpper = 0.0F, fContrastLower = 0.0F;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgAddr ) || ( NULL == pnRRFAddr ) || ( NULL == ptBrInput ) || ( NULL == pfSimilarity ) || ( NULL == pfContrast ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 0�N���A */
	memset( &tBrInfoTbl, 0x00, sizeof( CB_FPE_CBR_BR_INFO ) );

	/* �q�X�g�O�����̃r�������i�[ */
	nHistBinNum = CB_FPE_PRM_HIST_BIN_NUM;

	/*
	============================================
		�@���K���q�X�g�O�������P�x���ς��v�Z
	============================================
	*/
	nRet = cb_FPE_CalcBrightnessHistogram( pnImgAddr, pnRRFAddr, &tBrInfoTbl, ptBrInput );

	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();

		return ( CB_IMG_NG_CALC_BR_SIMILARITY );
	}

	/*
	============================================
		�A�q�X�g�O�����̗ގ��x���v�Z
	============================================
	*/
	/* �ŕp�l�v�Z�p������ */
	nUpperModeID = 0L;
	nLowerModeID = 0L;
	fUpperMode = 0.0F;
	fLowerMode = 0.0F;

	/* �q�X�g�O������BIN���������[�v */
	for( ni = 0L; ni < nHistBinNum; ni++)
	{
		/* �q�X�g�O�����̍ő�l��T���A�ŕp�l�Ƃ���ID�ƒl��o�^ */
		/* �㑤���̈�̍ŕp�l�T�� */
		if( fUpperMode < tBrInfoTbl.fBrTblUpper[ ni ] )
		{
			nUpperModeID = ni;
			fUpperMode = tBrInfoTbl.fBrTblUpper[ ni ];
		}

		/* �������̈�̍ŕp�l�T�� */
		if( fLowerMode < tBrInfoTbl.fBrTblLower[ ni ] )
		{
			nLowerModeID = ni;
			fLowerMode = tBrInfoTbl.fBrTblLower[ ni ];
		}
	}
	/* �ŕp�l�̍��������߂� */
	nDiffBrMode	= nUpperModeID - nLowerModeID;

	/* �ŕp�l�̍��������ƂɁA�q�X�g�O�����̃s�[�N�����킹�A���K�����ݑ��ւ��v�Z */
	for( ni = 0L; ni < nHistBinNum; ni++)
	{
		/* �q�X�g�O�����̃s�[�N�����킹�邱�ƂŁA�z��O�ƂȂ�Ή��t�����ł��Ȃ��Ȃ�ӏ��͈���Ȃ� */
		if(		( ( ni - nDiffBrMode ) < 0L )
			||	( ( ni - nDiffBrMode ) >= nHistBinNum ) )
		{
			/* Not Operation */
		}
		else
		{
			/* ���K�����ݑ��ւ��v�Z���邽�߂̒l���i�[���Ă��� */
			fDenom		+= tBrInfoTbl.fBrTblUpper[ ni ] * tBrInfoTbl.fBrTblLower[ ni - nDiffBrMode ];
			fNumerUpper	+= tBrInfoTbl.fBrTblUpper[ ni ] * tBrInfoTbl.fBrTblUpper[ ni ];
			fNumerLower	+= tBrInfoTbl.fBrTblLower[ ni - nDiffBrMode ] * tBrInfoTbl.fBrTblLower[ ni - nDiffBrMode ];

		}
	}
	/* ������v�Z */
	fNumer = sqrtf(fNumerUpper) * sqrtf(fNumerLower);

	/* ���K�����ݑ��֌v�Z����0���`�F�b�N */
	if( CB_FPE_FLT_MIN > fNumer )	/* 0���`�F�b�N */
	{
		/* �ގ��x(���֒l)��0�Ƃ��Ĉ��� */
		(*pfSimilarity) = 0.0F;
	}
	else
	{
		/* �ގ��x���v�Z */
		(*pfSimilarity) = fDenom / fNumer;
	}

	/*
	============================================
		�B�R���g���X�g���v�Z
	============================================
	*/
	/* �㉺�[�̍��̈�Ɣ��̈�̋P�x���ς̍�(���R���g���X�g)���v�Z */
	/* ���ϒl�ŃR���g���X�g�v�Z */
	fContrastUpper = tBrInfoTbl.fAvgUpper - ptBrInput->fAvgBlackArea;
	fContrastLower = tBrInfoTbl.fAvgLower - ptBrInput->fAvgBlackArea;

	/* ���������̒l���i�[�����^�[�� */
	if( fContrastUpper < fContrastLower )
	{
		(*pfContrast) = fContrastUpper;
	}
	else
	{
		(*pfContrast) = fContrastLower;
	}

	/* �R���g���X�g�����̒l�̎��A0���i�[ */
	if( (*pfContrast) < 0.0F )
	{
		(*pfContrast) = 0.0F;
	}
	else
	{
		/* Not Operation */
	}

	return CB_IMG_OK;
}


/******************************************************************************/
/**
 * @brief		�ǉ��p�^�[��_�q�X�g�O�������Z�o
 *
 * @param[in]	
 * @param[out]	
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		-1				:,�ُ�I��,value=-1,[-],
 *
 * @date		2013.08.02	S.Suzuki			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcBrightnessHistogram( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_BR_INFO* ptBrInfoTbl, CB_FPE_CBR_INPUT* ptBrInput )
{
	slong		nPixSrchX, nPixSrchY;                           /* ��f�T��_���[�v�p�ϐ�                 */
	slong		nSrchMax       = 0L;                            /* ��f�T��_�������[�v�p�ő�l           */
	slong		nPixSrchHeight = 0L;                            /* ��f�T��_����                         */
	slong		nPos           = 0L;                            /* �ΏےT���_                            */
	slong		nPosX          = 0L;                            /* �T�����̊�ƂȂ�X���W               */
	slong       nPosY          = 0L;                            /* �T�����̊�ƂȂ�Y���W               */
	slong		nSumBrightness = 0L;                            /* �P�x�݌v(���όv�Z�p)                  */
	slong		nCntPixel      = 0L;                            /* �s�N�Z����                            */
	slong		nSrchPnt       = 0L;                            /* �T���_                                */
	slong		nUpperFlg      = 0L;                            /* ���̈�H���ݕ␳�����t���O(��[)      */
	slong		nLowerFlg      = 0L;                            /* ���̈�H���ݕ␳�����t���O(���[)      */
	slong		nIdxUpperRRF   = 0L;
	slong		nIdxLowerRRF   = 0L;
	slong		nPosUpperX     = 0L;                            /* �������̈�ω��_���W(��[X���W)       */
	slong		nPosUpperY     = 0L;                            /* �������̈�ω��_���W(��[Y���W)       */
	slong		nPosLowerX     = 0L;                            /* �������̈�ω��_���W(���[X���W)       */
	slong		nPosLowerY     = 0L;                            /* �������̈�ω��_���W(���[Y���W)       */
	slong		nUpperY        = 0L;                            /* ���̈�H���ݕ␳���f�T��(��[Y���W) */
	slong		nLowerY        = 0L;                            /* ���̈�H���ݕ␳���f�T��(���[Y���W) */
	slong		nUpper         = CB_ADD_PATTERN_PNT_POS_UPPER;  /* �Ώۗ̈�(���ۏ�[��)                  */
	slong		nLower         = CB_ADD_PATTERN_PNT_POS_LOWER;  /* �Ώۗ̈�(���ۉ��[��)                  */
	float_t		fCoefBinWidth;

	CB_FPE_CBR_POINT_INFO*	ptBrPointTbl = NULL;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgAddr ) || ( NULL == pnRRFAddr ) || (NULL == ptBrInfoTbl) || ( NULL == ptBrInput ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	ptBrPointTbl = &(ptBrInput->tBrPointTbl[0]);
	/* �q�X�g�O�����i�[���ɂǂ̃r���ɊY�����邩���v�Z���邽�߂̌W�� */
	fCoefBinWidth = 1.0F / (float_t)( CB_FPE_PRM_HIST_BIN_WIDTH );

	/*
	============================================
		���ۏ�[��
	============================================
	*/
	/* ��f�T���̈捂���Z�o */
	nPixSrchHeight = ( (slong)( ptBrPointTbl[nUpper].tLabelLowerPnt.fY + 0.5F ) - (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) ) + 1L;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6876

	/* ������ */
	nUpperY = 0L;
	nLowerY = nPixSrchHeight;

	/* ��f�T�������ݒ� */
	nSrchMax = nPixSrchHeight / 2L;     /* ���̈�̏�[/���[�o������T�����邽�ߒT�������𔼕��Ƃ��� */

	/* ���̈�H���ݕ␳ */
	for( nPixSrchY = 0L; nPixSrchY < nSrchMax; nPixSrchY++ )
	{
		if( nUpperFlg != 1L )
		{
			/* ��[���x���������E���W�ݒ� */
			if( ptBrPointTbl[nUpper].nFlagVertical == 1L )  /* �T�����C���������̏ꍇ */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + 0.5F );					// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6877
				nPosUpperY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6878
			}
			else    /* �T�����C���������łȂ��ꍇ */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nUpper].fSrchSlope ) + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6879
				nPosUpperY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6880
			}

			/* �T���Ώۉ�f�C���f�b�N�X�ݒ� */
			nIdxUpperRRF = ( nPosUpperY * ptBrInput->nImgRRFXSize ) + nPosUpperX;

			/* �ی�⁨���̈�̕ω��_Y���W�ݒ�(�ی�≺�[�ʒu�␳) */
			if( ( nUpperFlg == 0L ) && ( (slong)pnRRFAddr[nIdxUpperRRF] == 0L ) )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6881
			{
				/* �������̈�ɕω������ʒu�ɍX�V */
				nUpperY = nPixSrchY;
				/* �␳�ς݃t���O��ON */
				nUpperFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
		if( nLowerFlg != 1L )
		{
			/* ���[���x���������E���W�ݒ� */
			if( ptBrPointTbl[nUpper].nFlagVertical == 1L )  /* �T�����C���������̏ꍇ */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6882
				nPosLowerY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6883
			}
			else    /* �T�����C���������łȂ��ꍇ */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + ( (float_t)( nPixSrchHeight - nPixSrchY ) / ptBrPointTbl[nUpper].fSrchSlope ) + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6884
				nPosLowerY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6885
			}

			/* �T���Ώۉ�f�C���f�b�N�X�ݒ� */
			nIdxLowerRRF = ( nPosLowerY * ptBrInput->nImgRRFXSize ) + nPosLowerX;

			/* ����⁨���̈�̕ω��_Y���W�ݒ�(������[�ʒu�␳) */
			if( ( nLowerFlg == 0L ) && ( (slong)pnRRFAddr[nIdxLowerRRF] == 0L ) )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6886
			{
				/* �������̈�ɕω������ʒu�ɍX�V */
				nLowerY = ( nPixSrchHeight - nPixSrchY );
				/* �␳�ς݃t���O��ON */
				nLowerFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
	}

	/* �T���̈�����s�N�Z�����Z�o */
	/* nLowerY < nUpperY �ɂȂ邱�Ƃ͂Ȃ��̂ŁA0���͔������Ȃ� */
	nCntPixel = ptBrPointTbl[nUpper].nSrchWidth * ( ( nLowerY - nUpperY ) + 1L );

	/* �T���s�N�Z�������s���l�̏ꍇ�A�G���[���� */
	if( nCntPixel <= 0L )
	{
		return ( CB_IMG_NG_CALC_BR_SIMILARITY );
	}

	/* �P�x�݌v�v�Z */
	for( nPixSrchY = nUpperY; nPixSrchY <= nLowerY; nPixSrchY++ )
	{
		/* �T����_���W�v�Z */
		if( ptBrPointTbl[nUpper].nFlagVertical == 1L )  /* �T�����C���������̏ꍇ */
		{
			nPosX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6889
			nPosY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6890
		}
		else    /* �T�����C���������łȂ��ꍇ */
		{
			nPosX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nUpper].fSrchSlope ) + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6891
			nPosY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6892
		}

		for( nPixSrchX = 0L; nPixSrchX < ptBrPointTbl[nUpper].nSrchWidth; nPixSrchX++ )
		{
			/* �T���_�̃A�h���X���Z�o */
			nPos = ( nPosY * ptBrInput->nImgSrcXSize ) + ( ( nPosX - ( ptBrPointTbl[nUpper].nSrchWidth / 2L ) ) + nPixSrchX );		// [EntryAVM_QAC#4]ID-6877 ���Z�����w�E�Ή�
			nSrchPnt = (slong)( (float_t)( pnImgAddr[nPos] ) * fCoefBinWidth );		//�؂�̂ď���

			/* ���K�����ăq�X�g�O�����e�[�u���Ɋi�[ */
			ptBrInfoTbl->fBrTblUpper[nSrchPnt] += ( 1.0F / (float_t)nCntPixel ) ;

			/* �P�x�l��݌v */
			nSumBrightness += (slong)( pnImgAddr[nPos] );
		}
	}

	/* ���ϒl���i�[ */
	ptBrInfoTbl->fAvgUpper = (float_t)( nSumBrightness ) / (float_t)( nCntPixel );

	/*
	============================================
		���ۉ��[��
	============================================
	*/
	/* ��f�T���̈捂���Z�o */
	nPixSrchHeight = ( (slong)( ptBrPointTbl[nLower].tLabelLowerPnt.fY + 0.5F ) - (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) ) + 1L;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6897

	/* ������ */
	nSumBrightness = 0L;
	nUpperFlg = 0L;
	nLowerFlg = 0L;
	nUpperY = 0L;
	nLowerY = nPixSrchHeight;

	/* ��f�T�������ݒ� */
	nSrchMax = nPixSrchHeight / 2L;     /* ���̈�̏�[/���[�o������T�����邽�ߒT�������𔼕��Ƃ��� */

	/* ���̈�H���ݕ␳ */
	for( nPixSrchY = 0L; nPixSrchY < nSrchMax; nPixSrchY++ )
	{
		if( nUpperFlg != 1L )
		{
			/* ��[���x���������E���W�ݒ� */
			if( ptBrPointTbl[nLower].nFlagVertical == 1L )  /* �T�����C���������̏ꍇ */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6898
				nPosUpperY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6899
			}
			else    /* �T�����C���������łȂ��ꍇ */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nLower].fSrchSlope ) + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6900
				nPosUpperY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6901
			}

			/* �T���Ώۉ�f�C���f�b�N�X�ݒ� */
			nIdxUpperRRF = ( nPosUpperY * ptBrInput->nImgRRFXSize ) + nPosUpperX;

			/* ����⁨���̈�̕ω��_Y���W�ݒ�(����≺�[�ʒu�␳) */
			if( ( nUpperFlg == 0L ) && ( (slong)pnRRFAddr[nIdxUpperRRF] == 0L ) )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6902
			{
				/* �������̈�ɕω������ʒu�ɍX�V */
				nUpperY = nPixSrchY;
				/* �␳�ς݃t���O��ON */
				nUpperFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
		if( nLowerFlg != 1L )
		{
			/* ���[���x���������E���W�ݒ� */
			if( ptBrPointTbl[nLower].nFlagVertical == 1L )  /* �T�����C���������̏ꍇ */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6903
				nPosLowerY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6904
			}
			else    /* �T�����C���������łȂ��ꍇ */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + ( (float_t)( nPixSrchHeight - nPixSrchY ) / ptBrPointTbl[nLower].fSrchSlope ) + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6905
				nPosLowerY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6906
			}

			/* �T���Ώۉ�f�C���f�b�N�X�ݒ� */
			nIdxLowerRRF = ( nPosLowerY * ptBrInput->nImgRRFXSize ) + nPosLowerX;

			/* �ی�⁨���̈�̕ω��_Y���W�ݒ�(�ی���[�ʒu�␳) */
			if( ( nLowerFlg == 0L ) && ( (slong)pnRRFAddr[nIdxLowerRRF] == 0L ) )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6907
			{
				/* �������̈�ɕω������ʒu�ɍX�V */
				nLowerY = ( nPixSrchHeight - nPixSrchY );
				/* �␳�ς݃t���O��ON */
				nLowerFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
	}

	/* �T���̈�����s�N�Z�����Z�o */
	/* nLowerY < nUpperY �ɂȂ邱�Ƃ͂Ȃ��̂ŁA0���͔������Ȃ� */
	nCntPixel = ptBrPointTbl[nLower].nSrchWidth * ( ( nLowerY - nUpperY ) + 1L );

	/* �T���s�N�Z�������s���l�̏ꍇ�A�G���[���� */
	if( nCntPixel <= 0L )
	{
		return ( CB_IMG_NG_CALC_BR_SIMILARITY );
	}

	/* �P�x�݌v�v�Z */
	for( nPixSrchY = nUpperY; nPixSrchY <= nLowerY; nPixSrchY++ )
	{
		/* �T����_���W�v�Z */
		if( ptBrPointTbl[nLower].nFlagVertical == 1L )  /* �T�����C���������̏ꍇ */
		{
			nPosX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6910
			nPosY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6911
		}
		else    /* �T�����C���������łȂ��ꍇ */
		{
			nPosX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nLower].fSrchSlope ) + 0.5F );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6912
			nPosY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6913
		}

		for( nPixSrchX = 0L; nPixSrchX < ptBrPointTbl[nLower].nSrchWidth; nPixSrchX++ )
		{
			/* �T���_�̃A�h���X���Z�o */
			nPos = ( nPosY * ptBrInput->nImgSrcXSize ) + ( ( nPosX - ( ptBrPointTbl[nLower].nSrchWidth / 2L ) ) + nPixSrchX );
			nSrchPnt = (slong)( (float_t)( pnImgAddr[nPos] ) * fCoefBinWidth );		//�؂�̂ď���
			/* ���K�����ăq�X�g�O�����e�[�u���Ɋi�[ */
			ptBrInfoTbl->fBrTblLower[nSrchPnt] += ( 1.0F / (float_t)nCntPixel ) ;

			/* �P�x�̗݌v���v�Z */
			nSumBrightness += (slong)( pnImgAddr[nPos] );
		}
	}

	/* ���ϒl���i�[ */
	ptBrInfoTbl->fAvgLower = (float_t)( nSumBrightness ) / (float_t)( nCntPixel );

	return CB_IMG_OK;
}
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */										/* �����_���o�댟�m�΍�(�P�x����) <CHG> 2013.08.08 �� */


/********************************************************************
 * �O���֐�(�f�o�b�O)�@��`
 ********************************************************************/
#ifdef CB_FPE_CSW_DEBUG_ON

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�������ʂ̕`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptCenterPos		:,�����_�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.18	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawResult( const IMPLIB_IMGID nImgDst, t_cb_img_CenterPosEx* const ptCenterPos, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	IMPLIB_IMGTBL	tImgTbl;

	/* �����`�F�b�N */
	if ( NULL == ptCenterPos )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜���擾 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �`��ݒ� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* �O���b�h�� */
//	cb_FPE_DBG_DrawGridLine( nImgDst, nCamPos );
	/* �T���̈� */
//	cb_FPE_DBG_DrawSearchRegionInfo( nImgDst, nCamPos, 0 );
	/* �����_�ʒu */
	cb_FPE_DBG_DrawFtrPoint( nImgDst, ptCenterPos, nCamPos );

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�O���b�h���̕`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.10	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawGridLine( const IMPLIB_IMGID nImgDst, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nOffset;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nSxLine, nSyLine, nExLine, nEyLine;
	IMPLIB_IMGTBL	tImgTbl;
	CB_IMG_POINT	*ptPntImg;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜���擾 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �`��ݒ� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* �擪�|�C���^ */
	ptPntImg = &( (m_FPE_tDbgInfo.tGridInfo[nCamPos].tPntImg)[0][0] );

	/* ���E���W�n��X���ɐ����Ȑ� */
	for ( ni = 0; ni < ( CB_FPE_GRID_NUM_WLD_Y - 1 ); ni++ )
	{
		for ( nj = 0; nj < CB_FPE_GRID_NUM_WLD_X; nj++ )
		{
			nOffset = ( ni * CB_FPE_GRID_NUM_WLD_X ) + nj;
			nSxLine = ( ptPntImg + nOffset )->nX;
			nSyLine = ( ptPntImg + nOffset )->nY;
			
			nOffset = ( ( ni + 1 ) * CB_FPE_GRID_NUM_WLD_X ) + nj;
			nExLine = ( ptPntImg + nOffset )->nX;
			nEyLine = ( ptPntImg + nOffset )->nY;

			if (   ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nSxLine, nSyLine, 0L, 0L, ( tImgTbl.xlng - 1L ), ( tImgTbl.ylng - 1L ) ) )
				&& ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nExLine, nEyLine, 0L, 0L, ( tImgTbl.xlng - 1L ), ( tImgTbl.ylng - 1L ) ) ) )
			{
				nRet = implib_DrawLine( nSxLine, nSyLine, nExLine, nEyLine );
			}
		}
	}

	/* ���E���W�n��X���ɐ����Ȑ� */
	for ( ni = 0; ni < CB_FPE_GRID_NUM_WLD_Y; ni++ )
	{
		for ( nj = 0; nj < ( CB_FPE_GRID_NUM_WLD_X - 1 ); nj++ )
		{
			nOffset = ( ni * CB_FPE_GRID_NUM_WLD_X ) + nj;
			nSxLine = ( ptPntImg + nOffset )->nX;
			nSyLine = ( ptPntImg + nOffset )->nY;
			
			nOffset = ( ni * CB_FPE_GRID_NUM_WLD_X ) + ( nj + 1 );
			nExLine = ( ptPntImg + nOffset )->nX;
			nEyLine = ( ptPntImg + nOffset )->nY;

			if (   ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nSxLine, nSyLine, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) ) )
				&& ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nExLine, nEyLine, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) ) ) )
			{
				nRet = implib_DrawLine( nSxLine, nSyLine, nExLine, nEyLine );
			}
		}
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�T���̈�`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in]		nZoomMode		:,�g��/�k�����[�h,0<=value<=1,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.10	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawSearchRegionInfo( const IMPLIB_IMGID nImgDst, slong nCamPos, slong nZoomMode )
{
	slong	nRet = CB_IMG_NG;
	slong	ni;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nSxRect, nSyRect, nExRect, nEyRect;
	IMPLIB_IMGTBL	tImgTbl;
	CB_RECT_RGN	*ptRgn;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜���擾 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �`��ݒ� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_BLACK/*WHITE*/ );

	for ( ni = 0; ni < CB_PATTERN_POS_MAXNUM; ni++ )
	{	
		if ( 0 == nZoomMode )
		{
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[ni].tRgnImg);
		}
		else if ( 1 == nZoomMode )
		{
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[ni].tRgnImgZoomOut);
		}
		else
		{
			/* default */
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[ni].tRgnImg);
		}

		nSxRect = ptRgn->nSX;
		nSyRect = ptRgn->nSY;
		nExRect = ptRgn->nEX;
		nEyRect = ptRgn->nEY;

		if (   ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nSxRect, nSyRect, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) ) )
			&& ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nExRect, nEyRect, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) ) ) )
		{
			nRet = implib_DrawRectangle( nSxRect, nSyRect, ( nExRect - nSxRect + 1 ), ( nEyRect - nSyRect ) );
			if ( 0L > nRet )
			{
				nRet = implib_ClearIPError();
			}
		}
	}


	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�ǉ��p�^�[���ɂ�����T���̈�`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in]		nZoomMode		:,�g��/�k�����[�h,0<=value<=1,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.01	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawSearchRegionInfo4AddPattern( const IMPLIB_IMGID nImgDst, slong nCamPos, slong nZoomMode )
{
	slong	nRet = CB_IMG_NG;
	slong	ni;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nSxRect, nSyRect, nExRect, nEyRect;
	IMPLIB_IMGTBL	tImgTbl;
	CB_RECT_RGN	*ptRgn;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptPntImg;			/* �ǉ��p�^�[���ɑ΂���T���̈�\���̂ւ̃|�C���^ */
	slong	nSxTopRect, nSyTopRect, nExTopRect, nEyTopRect;		/* �`���`�n�I�_���W                             */
	slong	nl = 0L;											/* ���ے��S�_(�J�����덷�l��)�`��p���[�v�J�E���^ */
	slong	nPos;												/* ���ے��S�ʒu�p���[�v�J�E���^                   */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */


	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜���擾 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �`��ݒ� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_BLACK/*WHITE*/ );

	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{	
		if ( 0 == nZoomMode )
		{
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tRgnImg);
		}
		else if ( 1 == nZoomMode )
		{
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[ni].tRgnImgZoomOut);
		}
		else
		{
			/* default */
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[ni].tRgnImg);
		}

		nSxRect = ptRgn->nSX;
		nSyRect = ptRgn->nSY;
		nExRect = ptRgn->nEX;
		nEyRect = ptRgn->nEY;

		if (   ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nSxRect, nSyRect, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) ) )
			&& ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nExRect, nEyRect, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) ) ) )
		{
			nRet = implib_DrawRectangle( nSxRect, nSyRect, ( nExRect - nSxRect + 1 ), ( nEyRect - nSyRect ) );
			if ( 0L > nRet )
			{
				nRet = implib_ClearIPError();
			}
		}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
		if( 0L == nZoomMode )
		{
			ptPntImg = &( m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni] );
		}

		/*--------------------------------*/
		/* ���ےT���̈��`�`��           */
		/*--------------------------------*/

		/* �`��ݒ� */
		nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

		/* ���ې������[�v */
		for( nPos = 0; nPos < CB_ADD_PATTERN_PNT_POS_MAXNUM; nPos++ )
		{
			nSxTopRect = ptPntImg->tPntImgCircle[nPos].tCircleSrchRgnTL.nX;
			nSyTopRect = ptPntImg->tPntImgCircle[nPos].tCircleSrchRgnTL.nY;
			nExTopRect = ptPntImg->tPntImgCircle[nPos].tCircleSrchRgnBR.nX;
			nEyTopRect = ptPntImg->tPntImgCircle[nPos].tCircleSrchRgnBR.nY;

			nRet = implib_DrawRectangle( ( nSxTopRect - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ),
										 ( nSyTopRect - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ),
										 ( ( nExTopRect - nSxTopRect + 1 ) + ( CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN * 2 ) ),
										 ( ( nEyTopRect - nSyTopRect ) + ( CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN * 2 ) ) );
			if ( 0L > nRet )
			{
				nRet = implib_ClearIPError();
			}
		}

		/*--------------------------------*/
		/* ���ے��S�_(�J�����덷�l��)�`�� */
		/*--------------------------------*/

		/* �`��ݒ� */
		nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_BLACK );

		for( nl = 0; nl < CB_FPE_ERR_TBL_MAXNUM; nl++ )
		{
			/* ���ې������[�v */
			for( nPos = 0; nPos < CB_ADD_PATTERN_PNT_POS_MAXNUM; nPos++ )
			{
				nSxTopRect = ptPntImg->tPntImgCircle[nPos].tCircleErrPntTbl[nl].nX;
				nSyTopRect = ptPntImg->tPntImgCircle[nPos].tCircleErrPntTbl[nl].nY;

				nRet = implib_DrawRectangle( nSxTopRect, nSyTopRect, 1, 1 );
				if ( 0L > nRet )
				{
					nRet = implib_ClearIPError();
				}
			}
		}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* �����_���o�댟�m�΍�(�T���̈����) <CHG> 2013.08.01 �� */
	}


	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]Haar-like�����ʂ̕`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.17	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawHaarLikeInfo( const IMPLIB_IMGID nImgDst, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nR_RGB, nG_RGB, nB_RGB;
	slong	nY_YUV, nU_YUV, nV_YUV;
	slong	nXSizeImgY, nYSizeImgY, nXSizeImgUV, nYSizeImgUV;
	slong	nMaxValue;
	slong	*pnWkFtr;
	uchar	*pnPixelImgY, *pnWkPixelImgY;
	uchar	*pnPixelImgUV, *pnWkPixelImgUV;
	CB_FPE_HAAR_LIKE_FTR	*ptHaarLike;
	CB_RECT_RGN	*ptSrchRgn;
	/* �p�����[�^ */
	slong	nprm_xMagZoomOut, nprm_yMagZoomOut;
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �p�����[�^ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE;

	/* �f�o�b�O�摜���N���A */
//	nRet = implib_IP_ClearImg( nImgDst );

	/* Haar-Like����`�� */
	/* �擪�|�C���^ */
	ptHaarLike = &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]);
//	ptHaarLike = &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]);
//	ptHaarLike = &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]);
	/* �ő�l */
	nMaxValue = 2 * ( ( nprm_xsizeBlockW * nprm_ysizeBlockW ) * 255 );

	/* �T���̈� */
	ptSrchRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[0].tRgnImgZoomOut);

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgDst, &nXSizeImgY, &nYSizeImgY, (schar**)&pnPixelImgY );
	nRet = implib_OpenImgDirect( implib_GetUVImgID( nImgDst ), &nXSizeImgUV, &nYSizeImgUV, (schar**)&pnPixelImgUV );

	for ( ni = ptSrchRgn->nSY; ni <= ptSrchRgn->nEY; ni++ )
	{
		pnWkFtr = ptHaarLike->pnFtrValue + ( ni * ptHaarLike->nXSize ) + ptSrchRgn->nSX;
		pnWkPixelImgY = pnPixelImgY + ( ni * nXSizeImgY ) + ptSrchRgn->nSX;
		pnWkPixelImgUV = pnPixelImgUV + ( ni * nXSizeImgUV ) + ptSrchRgn->nSX;

		for ( nj = ptSrchRgn->nSX; nj <= ptSrchRgn->nEX; nj+=2 )
		{
#if 0
			if ( (*pnWkFtr) < 0 )
			{
				nR_RGB = 0;
				nG_RGB = 0;
				nB_RGB = 0;
			}
			else
			{
				nR_RGB = 0;
				nG_RGB = (slong)( ( ( (float_t)(*pnWkFtr) / (float_t)10000 ) * 255.0f ) + 0.5f );
				nB_RGB = 0;
			}
#else
			if ( (*pnWkFtr) < 0 )
			{
				nR_RGB = 0;
				nG_RGB = (slong)( ( ( (float_t)labs( (*pnWkFtr) ) / (float_t)10000 ) * 255.0f ) + 0.5f );
				nB_RGB = 0;
			}
			else
			{
				nR_RGB = 0;
				nG_RGB = (slong)( ( ( (float_t)(*pnWkFtr) / (float_t)10000 ) * 255.0f ) + 0.5f );
				nB_RGB = 0;
			}
#endif

			if ( 255 < nG_RGB )
			{
				nG_RGB = 255;
			}
			if ( nG_RGB < 0 )
			{
				nG_RGB = 0;
			}

			/* RGB to YUV */
			nY_YUV = (slong)( (  0.299f * (float_t)nR_RGB ) + (  0.587f * (float_t)nG_RGB ) + (  0.114f * (float_t)nB_RGB ) );
			nU_YUV = (slong)( ( -0.169f * (float_t)nR_RGB ) + ( -0.331f * (float_t)nG_RGB ) + (  0.500f * (float_t)nB_RGB ) );
			nV_YUV = (slong)( (  0.500f * (float_t)nR_RGB ) + ( -0.419f * (float_t)nG_RGB ) + ( -0.081f * (float_t)nB_RGB ) );

			/* UV�͈͕̔ϊ� */
			nU_YUV += 128;
			nV_YUV += 128;

			/* �͈̓`�F�b�N */
			if ( 255 < nY_YUV )
			{
				nY_YUV = 255;
			}
			if ( nY_YUV < 0 )
			{
				nY_YUV = 0;
			}
			if ( 255 < nU_YUV )
			{
				nU_YUV = 255;
			}
			if ( nU_YUV < 0 )
			{
				nU_YUV = 0;
			}
			if ( 255 < nV_YUV )
			{
				nV_YUV = 255;
			}
			if ( nV_YUV < 0 )
			{
				nV_YUV = 0;
			}

			/* �l�i�[ */
//			*pnWkPixelImgY = (uchar)nY_YUV;
//			*( pnWkPixelImgY + 1 ) = (uchar)nY_YUV;
			
			if ( 0 == ( nj & 0x00000001 ) )	/* ���� */
			{
				*pnWkPixelImgUV = (uchar)nU_YUV;
				*(pnWkPixelImgUV + 1 ) = (uchar)nV_YUV;
			}
			else	/* � */
			{
				*(pnWkPixelImgUV - 1) = (uchar)nU_YUV;
				*pnWkPixelImgUV = (uchar)nV_YUV;
			}

			pnWkFtr += 2;
			pnWkPixelImgY += 2;
			pnWkPixelImgUV += 2;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nImgDst );
	nRet = implib_CloseImgDirect( implib_GetUVImgID( nImgDst ) );
	
	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�T������(���)�_�`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.17	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawSearchResultCandPnt( const IMPLIB_IMGID nImgDst, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nXpoint, nYpoint;
	IMPLIB_IMGTBL	tImgTbl;
	CB_FPE_SRCH_RSLT_PNT_INFO	*ptSrchRsltPntInfo;
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO	*ptSrchRsltAddPntInfo;
	/* �p�����[�^ */
	slong	nprm_xMagZoomOut, nprm_yMagZoomOut;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �p�����[�^ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* �摜��� */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �f�o�b�O�摜���N���A */
//	nRet = implib_IP_ClearImg( nImgDst );

	/* �T������(���)��`�� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* �`��ݒ� */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* �擪�|�C���^ */
	ptSrchRsltPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltPntInfo[0]);
	ptSrchRsltAddPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddPntInfo[0]);

#if 0	/* �F���� */
	for ( ni = 0; ni < CB_PATTERN_POS_MAXNUM; ni++ )
	{
		/* �\���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumCrossPnt; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tCrossPntHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tCrossPntHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		}

		/* �����s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntLower; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntLowerHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntLowerHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nU );
		}

		/* �㑤�s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntUpper; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntUpperHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntUpperHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* �E�����s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntRight; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntRightHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntRightHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nU );
		}

		/* �������s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntLeft; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntLeftHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntLeftHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK].nU );
		}
	}
#else
	for ( ni = 0; ni < CB_PATTERN_POS_MAXNUM; ni++ )
	{
		/* �\���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumCrossPnt; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tCrossPnt[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tCrossPnt[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* �����s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntLower; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntLower[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntLower[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* �㑤�s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntUpper; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntUpper[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntUpper[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* �E�����s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntRight; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntRight[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntRight[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* �������s���_ */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntLeft; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntLeft[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntLeft[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}
	}

	/* �ǉ��p�^�[�� */
	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		for ( nj = 0; nj < ptSrchRsltAddPntInfo[ni].nNumCenterPnt; nj++ )
		{
			/* �ʏ� */
			nXpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nX;
			nYpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}
	}
#endif

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�����_�ʒu�`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptCenterPos		:,�����_�\���̂ւ̃|�C���^,-,[-], 
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.18	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawFtrPoint( const IMPLIB_IMGID nImgDst, t_cb_img_CenterPosEx* const ptCenterPos, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	ni;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nXpoint, nYpoint;
	IMPLIB_IMGTBL	tImgTbl;

	/* �����`�F�b�N */
	if ( NULL == ptCenterPos )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜��� */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �T������(���)��`�� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* �`��ݒ� */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	for ( ni = 0; ni < ptCenterPos->CenterNum; ni++ )
	{
		nXpoint = (slong)( ptCenterPos->Center[ni].w + 0.5 );
		nYpoint = (slong)( ptCenterPos->Center[ni].h + 0.5 );

		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	for ( ni = 0; ni < ptCenterPos->LeftNum; ni++ )
	{
		nXpoint = (slong)( ptCenterPos->Left[ni].w + 0.5 );
		nYpoint = (slong)( ptCenterPos->Left[ni].h + 0.5 );

		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	for ( ni = 0; ni < ptCenterPos->RightNum; ni++ )
	{
		nXpoint = (slong)( ptCenterPos->Right[ni].w + 0.5 );
		nYpoint = (slong)( ptCenterPos->Right[ni].h + 0.5 );

		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

#if 0
	nXpoint = (slong)( ptCenterPos->LeftCross.w + 0.5 );
	nYpoint = (slong)( ptCenterPos->LeftCross.h + 0.5 );

	nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 3 ), nYpoint, ( nXpoint + 3 ), nYpoint
						, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
	nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 3 ), nXpoint, ( nYpoint + 3 )
						, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

	nXpoint = (slong)( ptCenterPos->RightCross.w + 0.5 );
	nYpoint = (slong)( ptCenterPos->RightCross.h + 0.5 );

	nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 3 ), nYpoint, ( nXpoint + 3 ), nYpoint
						, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
	nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 3 ), nXpoint, ( nYpoint + 3 )
						, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

#endif

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]���ے��S�ʒu�`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.01	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawCircleCenterPoint( const IMPLIB_IMGID nImgDst, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nXpoint, nYpoint;
	IMPLIB_IMGTBL	tImgTbl;
	CB_IMG_POINT	*ptPoint;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜��� */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �T������(���)��`�� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* �`��ݒ� */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		ptPoint = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddPntInfo[ni].tCenterPnt[0]);
		for ( nj = 0; nj < m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddPntInfo[ni].nNumCenterPnt; nj++ )
		{
			nXpoint = ptPoint[nj].nX;
			nYpoint = ptPoint[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		}
	}

	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�����x�ʒu���ߏ��`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.22	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawPrecisePoint( const IMPLIB_IMGID nImgDst, slong nCamPos )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nXpoint, nYpoint;
	IMPLIB_IMGTBL	tImgTbl;
	CB_FPE_SRCH_RSLT_PNT_INFO	*ptSrchRsltPntInfo;
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO	*ptSrchRsltAddPntInfo;
	CB_FPE_SRCH_RSLT_PNT_INFO	*ptSrchRsltAddChkPntInfo;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜��� */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �T������(���)��`�� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* �`��ݒ� */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* �擪�|�C���^ */
	ptSrchRsltPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltPntInfo[0]);
	ptSrchRsltAddPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddPntInfo[0]);
	ptSrchRsltAddChkPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddChkPntInfo[0]);

	/* �s���p�^�[����̓_ */
	/* �\�� */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumCrossPnt; ni++ )
	{
		/* �ʏ� */
		nXpoint = ptSrchRsltPntInfo->tCrossPnt[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tCrossPnt[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* �����x */
		nXpoint = (slong)( ptSrchRsltPntInfo->tCrossPntPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tCrossPntPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* �s��(��) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntUpper; ni++ )
	{
		/* �ʏ� */
		nXpoint = ptSrchRsltPntInfo->tTPntUpper[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntUpper[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* �����x */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntUpperPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntUpperPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* �s��(��) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntLower; ni++ )
	{
		/* �ʏ� */
		nXpoint = ptSrchRsltPntInfo->tTPntLower[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntLower[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* �����x */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntLowerPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntLowerPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* �s��(��) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntLeft; ni++ )
	{
		/* �ʏ� */
		nXpoint = ptSrchRsltPntInfo->tTPntLeft[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntLeft[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* �����x */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntLeftPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntLeftPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* �s��(�E) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntRight; ni++ )
	{
		/* �ʏ� */
		nXpoint = ptSrchRsltPntInfo->tTPntRight[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntRight[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* �����x */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntRightPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntRightPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* �ǉ��p�^�[�� */
	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		for ( nj = 0; nj < ptSrchRsltAddPntInfo[ni].nNumCenterPnt; nj++ )
		{
			/* �ʏ� */
			nXpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nX;
			nYpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

			/* �����x */
			nXpoint = (slong)( ptSrchRsltAddPntInfo[ni].tCenterPntPrecise[nj].w + 0.5 );
			nYpoint = (slong)( ptSrchRsltAddPntInfo[ni].tCenterPntPrecise[nj].h + 0.5 );
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		}
	}


	/* �ǉ��p�^�[�� */
	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* �\���_ */
		/* �ʏ�̂� */
		nXpoint = ptSrchRsltAddChkPntInfo[ni].tCrossPnt[0].nX;
		nYpoint = ptSrchRsltAddChkPntInfo[ni].tCrossPnt[0].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* �����x */
		nXpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tCrossPntPrecise[0].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tCrossPntPrecise[0].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );

		/* ��T���_ */
		for ( nj = 0; nj < ptSrchRsltAddChkPntInfo[ni].nNumTPntUpper; nj++ )
		{
			/* �ʏ� */
			nXpoint = ptSrchRsltAddChkPntInfo[ni].tTPntUpper[nj].nX;
			nYpoint = ptSrchRsltAddChkPntInfo[ni].tTPntUpper[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

			/* �����x */
			nXpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tTPntUpperPrecise[nj].w + 0.5 );
			nYpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tTPntUpperPrecise[nj].h + 0.5 );
			if( nXpoint >= 0L ){
				nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
									, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
				nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
									, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			}
		}

		/* ��T���_ */
		for ( nj = 0; nj < ptSrchRsltAddChkPntInfo[ni].nNumTPntLower; nj++ )
		{
			/* �ʏ� */
			nXpoint = ptSrchRsltAddChkPntInfo[ni].tTPntLower[nj].nX;
			nYpoint = ptSrchRsltAddChkPntInfo[ni].tTPntLower[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

			/* �����x */
			nXpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tTPntLowerPrecise[nj].w + 0.5 );
			nYpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tTPntLowerPrecise[nj].h + 0.5 );
			if( nXpoint >= 0L ){
				nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
									, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
				nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
									, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			}
		}
	}


	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�P�x����X�R�A&���`��
 *
 * @param[in,out]	
 * @param[in]		 
 * @param[in]		
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			-1				:,�ُ�I��,value=-1,[-],
 *
 * @date		2013.08.01	S.Suzuki		�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawCbrScoreResult( const IMPLIB_IMGID nImgDst )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nXpoint, nYpoint;
	IMPLIB_IMGTBL	tImgTbl;

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜��� */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �T������(���)��`�� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( 720 - 1 ), ( 720 - 1 ) );

	/* �`��ݒ� */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );
	nRet = implib_SetStringAttributes( IMPLIB_STRING_16x16, 0, 0 );


	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		for ( nj = 0L; nj < m_FPE_tDbgInfo.nNum[ni]; nj++ )
		{

			if( m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX < 0L )
			{
				/* Not Operation */
			}
			else
			{
				/* ���ʂ�Ԓ����ŕ`�� */
				nRet = IPM_DrawLineColor( nImgDst, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY,
													m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY,
													m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
				/* ��[�̈ʒu���`�ŕ`�� */
				nRet = IPM_DrawRectangleColor( nImgDst, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY,
														3, 3,
														m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
				/* ���[�̈ʒu���`�ŕ`�� */
				nRet = IPM_DrawRectangleColor( nImgDst, m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY,
														3, 3,
														m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
				/* �����̒��S������ɐ�ID��`�� */
				nRet = IPEXT_DrawString( (	m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX + m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX ) / 2L,
										 (	m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY + m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY ) / 2L,
										"[%.2d]",nj );
				/* ��ʉ��̍��E�ɁA�X�R�A����`�� */
				nRet = IPEXT_DrawString( 10 + (360*ni), 280 + ( 20*nj ), "[%d]Score:%.3f \t Sim:%.3f \t Cont:%.3f ", nj, m_FPE_tDbgInfo.tScore[ni][nj], m_FPE_tDbgInfo.tSimilarity[ni][nj], m_FPE_tDbgInfo.tContrast[ni][nj] );
			}

		}
	}


	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

#endif /* CB_FPE_CSW_DEBUG_ON */

/********************************************************************
 * �����֐�(�f�o�b�O)�@��`
 ********************************************************************/

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]������
 *
 * @param		�Ȃ�
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.14	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_DBG_Init( void )
{

#ifdef CB_FPE_CSW_DEBUG_ON
	CB_COLOR_YUV	*ptColorYUV;
#endif /* CB_FPE_CSW_DEBUG_ON */

	/* 0�N���A */
	memset( &m_FPE_tDbgInfo, 0x00, sizeof( CB_FPE_DBG_INFO ) );

#ifdef CB_FPE_CSW_DEBUG_ON
	/* �F */
	/* �� */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED]);
	ptColorYUV->nY = CB_COLOR_RED_YUV_Y;
	ptColorYUV->nU = CB_COLOR_RED_YUV_U;
	ptColorYUV->nV = CB_COLOR_RED_YUV_V;
	/* �� */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE]);
	ptColorYUV->nY = CB_COLOR_BLUE_YUV_Y;
	ptColorYUV->nU = CB_COLOR_BLUE_YUV_U;
	ptColorYUV->nV = CB_COLOR_BLUE_YUV_V;
	/* �� */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN]);
	ptColorYUV->nY = CB_COLOR_GREEN_YUV_Y;
	ptColorYUV->nU = CB_COLOR_GREEN_YUV_U;
	ptColorYUV->nV = CB_COLOR_GREEN_YUV_V;
	/* �s���N */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK]);
	ptColorYUV->nY = CB_COLOR_PINK_YUV_Y;
	ptColorYUV->nU = CB_COLOR_PINK_YUV_U;
	ptColorYUV->nV = CB_COLOR_PINK_YUV_V;
	/* �I�����W */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE]);
	ptColorYUV->nY = CB_COLOR_ORANGE_YUV_Y;
	ptColorYUV->nU = CB_COLOR_ORANGE_YUV_U;
	ptColorYUV->nV = CB_COLOR_ORANGE_YUV_V;
	/* �V�A�� */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_CYAN]);
	ptColorYUV->nY = CB_COLOR_CYAN_YUV_Y;
	ptColorYUV->nU = CB_COLOR_CYAN_YUV_U;
	ptColorYUV->nV = CB_COLOR_CYAN_YUV_V;
	/* �� */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_YELLOW]);
	ptColorYUV->nY = CB_COLOR_YELLOW_YUV_Y;
	ptColorYUV->nU = CB_COLOR_YELLOW_YUV_U;
	ptColorYUV->nV = CB_COLOR_YELLOW_YUV_V;
	/* �}�[���^ */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_MAGENTA]);
	ptColorYUV->nY = CB_COLOR_MAGENTA_YUV_Y;
	ptColorYUV->nU = CB_COLOR_MAGENTA_YUV_U;
	ptColorYUV->nV = CB_COLOR_MAGENTA_YUV_V;
#endif

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�O���b�h���̐ݒ�
 *
 * @param[out]	ptGridInfo		:,�O���b�h���\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptCamPrm		:,�J�����p�����[�^�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection	:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.15	K.Kato			�V�K�쐬
 *
 * @note		����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_DBG_SetGridInfo( CB_FPE_DBG_GRID_INFO* ptGridInfo, const CB_FPE_CAM_PRM* const ptCamPrm, slong nCamDirection )
{
	slong	nRet = CB_IMG_NG;
	slong	nj, nk;
	slong	nNumCam;
	double_t	dTempValue;
	double_t	dStartPosX, dStartPosY;
	CB_CG_PNT_WORLD	tTempWldPnt, tTempWldPntVehicle;		// MISRA-C����̈�E [EntryAVM_QAC#3] R1.1.1  , ID-6702
	CB_CG_PNT_IMG	tTempImgPnt;
	CB_CG_PNT_WORLD	*ptPntWld;
	CB_IMG_POINT	*ptPntImg;

	/* �����`�F�b�N */
	if ( ( NULL == ptGridInfo ) || ( NULL == ptCamPrm ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nNumCam = E_CB_SELFCALIB_CAMPOS_MAX;

	/* ���g�p�ϐ���� ���[�j���O�΍� */
	CALIB_UNUSED_VARIABLE( tTempWldPntVehicle );	/* �ϐ��g�p���ɂ͍폜���Ă������� */		// MISRA-C����̈�E [EntryAVM_QAC#3] R1.1.1  R-30, ID-6704	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.3  R-53, ID-6703

	/* �擪�|�C���^ */
	ptPntWld = &(ptGridInfo->tPntWld[0][0]);
	ptPntImg = &(ptGridInfo->tPntImg[0][0]);

	/* �O���b�h�J�n�ʒu�̐ݒ� */
	switch ( nCamDirection )
	{
		case ( E_CB_SELFCALIB_CAMPOS_FRONT ):
			dStartPosX = -5.0;
			dStartPosY = -9.0;
			break;
		case ( E_CB_SELFCALIB_CAMPOS_LEFT ):
			dStartPosX = -10.0;
			dStartPosY = -4.0;
			break;
		case ( E_CB_SELFCALIB_CAMPOS_RIGHT ):
			dStartPosX =  0.0;
			dStartPosY = -4.0;
			break;
		case ( E_CB_SELFCALIB_CAMPOS_REAR ):
			dStartPosX = -5.0;
			dStartPosY = 4.0;
			break;
		default:
			return ( CB_IMG_NG );
	}

	/* Y�����̃��[�v */
	for ( nj = 0L; nj < CB_FPE_GRID_NUM_WLD_Y; nj++ )
	{
		/* X�����̃��[�v */
		for ( nk = 0L; nk < CB_FPE_GRID_NUM_WLD_X; nk++ )
		{
			dTempValue = dStartPosX + ( (double_t)nk * CB_FPE_GRID_INTERVAL_WLD_X );
			tTempWldPnt.dX = dTempValue;
			dTempValue = dStartPosY + ( (double_t)nj * CB_FPE_GRID_INTERVAL_WLD_Y );
			tTempWldPnt.dY = dTempValue;
			tTempWldPnt.dZ = 0.0;

			/* World To Image */
			nRet = cb_CG_CalcW2C2I( ptCamPrm->nCamID, &tTempWldPnt, &tTempImgPnt );

			/* World�i�[ */
			*( ptPntWld + ( ( nj * CB_FPE_GRID_NUM_WLD_X ) + nk ) ) = tTempWldPnt;
			/* Image�i�[ */
			(*( ptPntImg + ( ( nj * CB_FPE_GRID_NUM_WLD_X ) + nk ) )).nX = (slong)( tTempImgPnt.dW + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6705
			(*( ptPntImg + ( ( nj * CB_FPE_GRID_NUM_WLD_X ) + nk ) )).nY = (slong)( tTempImgPnt.dH + 0.5 );		// MISRA-C����̈�E [EntryAVM_QAC#3] R2.4.1  , ID-6706
		}	/* for ( nk ) */
	}	/* for ( nj ) */

	return ( CB_IMG_OK );
}

#ifdef CB_FPE_CSW_DEBUG_ON

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]Haar-like�����ʂ̃t�@�C�������o��
 *
 * @param[in]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	pFilePath		:,�o�͐�t�@�C�����ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.01.17	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_DBG_SaveHaarLikeInfo( CB_FPE_HAAR_LIKE_FTR* ptHaarLike, schar *pFilePath )
{
	slong	ni, nj;
	slong	*pnWkFtr;
	FILE	*pFile;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == pFilePath ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �t�@�C���I�[�v�� */
	pFile = fopen( pFilePath, "w" );
	if ( NULL == pFile )
	{
		/* �t�@�C���I�[�v�����s */
		return ( CB_IMG_NG );
	}

	fprintf( pFile, "Y_X," );
	for ( nj = 0; nj < ptHaarLike->nXSize; nj++ )
	{
		fprintf( pFile, "%d,", nj );
	}
	fprintf( pFile, "\n" );

	for ( ni = 0; ni < ptHaarLike->nYSize; ni++ )
	{
		pnWkFtr = ptHaarLike->pnFtrValue + ( ni * ptHaarLike->nXSize );

		fprintf( pFile, "%d,", ni );

		for ( nj = 0; nj < ptHaarLike->nXSize; nj++ )
		{
			fprintf( pFile, "%d,", (*pnWkFtr) );
			pnWkFtr++;
		}

		fprintf( pFile, "\n" );
	}

	/* �t�@�C���N���[�Y */
	fclose( pFile );

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[�f�o�b�O]��f�l�̃t�@�C�������o��
 *
 * @param[in]	nImgID			:,���ID,-,[-],
 * @param[in]	pFilePath		:,�o�͐�t�@�C�����ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.02.25	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_DBG_SavePixelValue( IMPLIB_IMGID nImgID, schar *pFilePath )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	FILE	*pFile;
	slong	nXSizeImg, nYSizeImg;
	schar	*pnPixelValue;
	IMPLIB_IMGTBL	tImgTbl;

	/* �����`�F�b�N */
	if ( NULL == pFilePath )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �t�@�C���I�[�v�� */
	pFile = fopen( pFilePath, "w" );
	if ( NULL == pFile )
	{
		/* �t�@�C���I�[�v�����s */
		return ( CB_IMG_NG );
	}

	nRet = implib_ReadImgTable( nImgID, &tImgTbl );

	nRet = implib_OpenImgDirect( nImgID, &nXSizeImg, &nYSizeImg, (void**)&pnPixelValue );

	fprintf( pFile, "Y_X," );
	for ( nj = 0; nj < nXSizeImg; nj++ )
	{
		fprintf( pFile, "%d,", nj );
	}
	fprintf( pFile, "\n" );

	if ( ( IMPLIB_UNSIGN8_DATA == tImgTbl.dtyp ) || ( IMPLIB_BINARY_DATA == tImgTbl.dtyp ) )
	{
		for ( ni = 0; ni < nYSizeImg; ni++ )
		{
			fprintf( pFile, "%d,", ni );

			for ( nj = 0; nj < nXSizeImg; nj++ )
			{
				fprintf( pFile, "%d,", (*((uchar*)(pnPixelValue + (ni * nXSizeImg) + nj))) );
			}

			fprintf( pFile, "\n" );
		}
	}
	else if ( IMPLIB_SIGN8_DATA == tImgTbl.dtyp )
	{
		for ( ni = 0; ni < nYSizeImg; ni++ )
		{
			fprintf( pFile, "%d,", ni );

			for ( nj = 0; nj < nXSizeImg; nj++ )
			{
				fprintf( pFile, "%d,", (*(pnPixelValue + (ni * nXSizeImg) + nj)) );
			}

			fprintf( pFile, "\n" );
		}
	}
	else
	{
		;
	}

	/* �t�@�C���N���[�Y */
	fclose( pFile );

	nRet = implib_CloseImgDirect( nImgID );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CB_FPE_CSW_DEBUG_ON */


/******************************************************************************/
/**
 * @brief			���E�s���p�^�[���ɂ���������_���o
 *
 * @param[in]		nImgSrc			:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptFPEInfo		:,�������\���̂ւ̃|�C���^,-,[-],
 * @param[out]		nCamDirection	:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]		nCalibType		:,�L�����u���(�H��E�̎�),CB_CALIB_REAR_KIND_FACTORY<=value<=CB_CALIB_REAR_KIND_RUNTIME,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.05	S.Suzuki			�V�K�쐬
 * @date			2015.08.26	S.Morita			�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�
 *
 * @note			����nCamDirection�́A��ʊ֐��Ŕ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_ExtractFtrPoint4AddChk( const IMPLIB_IMGID nImgSrc, CB_FPE_INFO *ptFPEInfo, slong nCamDirection, slong nCalibType )
{
	slong									nRet = CB_IMG_NG;
	enum enum_CB_ADD_PATTERN_POS			ni = CB_ADD_PATTERN_POS_LEFT;
	slong									nSxSrc0 = 0L, 
											nSySrc0 = 0L, 
											nExSrc0 = 0L, 
											nEySrc0 = 0L;
	slong									nSxSrc1 = 0L, 
											nSySrc1 = 0L, 
											nExSrc1 = 0L, 
											nEySrc1 = 0L;
	slong									nSxDst = 0L, 
											nSyDst = 0L, 
											nExDst = 0L, 
											nEyDst = 0L;
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo = NULL;
	CB_FPE_SRCH_RSLT_INFO					*ptSrchRsltInfo = NULL;
	CB_FPE_SRCH_RSLT_PNT_INFO				*ptSrchRsltPntInfo = NULL;
	CB_FPE_HAAR_LIKE_FTR					*ptHaarLike = NULL;
	/* �p�����[�^ */
	slong	nprm_thrHaarFtrCrossPnt = 0L;		/* �s���^�[�Q�b�g�����̓����_���o���ɁA�����_���Ƃ�������ʂɑ΂��邵�����l�i�[�p */


	/* �����`�F�b�N */
	if ( NULL == ptFPEInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT;

	/* ���g�p�ϐ���� ���[�j���O�΍� */
	CALIB_UNUSED_VARIABLE( nCalibType );	/* �ϐ��g�p���ɂ͍폜���Ă������� */		// MISRA-C����̈�E [EntryAVM_QAC#3] O1.3  R-53, ID-6724

	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �擪�|�C���^ */
	ptSrchRgnInfo = &(ptFPEInfo->tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]);
	ptSrchRsltInfo = &(ptFPEInfo->tRslt.tSrchRsltInfo[nCamDirection]);
	ptSrchRsltPntInfo = &(ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]);

	/* �N���A */
	nRet = cb_FPE_ClearSearchResultPointInfo( CB_FPE_NELEMS( ptSrchRsltInfo->tSrchRsltAddChkPntInfo ), &( ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* Haar-like�����ʗp�̉摜���������蓖��(�摜�������A�N�Z�X�J�n) */
	/* Haar-like�摜�������N���A */
	nRet = implib_IP_Const( ptFPEInfo->nImg[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL], 0L );

	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}


	/* ���E�}�[�J�[�Ń��[�v */
	for ( ni = CB_ADD_PATTERN_POS_LEFT; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/*
		==============================
			�\���_�T��
		==============================
		*/
		/* Integral Image�쐬 */
		nRet = cb_FPE_IntegralImage( &( ptFPEInfo->nImg[0] ),  &( ptSrchRgnInfo[ni].tRgnImgNormalHLsrc ), CB_FPE_PRM_INTIMG_FLG_SIDE );
		if ( CB_IMG_OK != nRet )
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_INTEGRAL_IMG );
		}
#ifdef CALIB_PARAM_P32S_EUR_UK
		if ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
		{
			/* �J����������ʂ��t�����g�J�����ł���ہA���}�[�J�̓����_�擾�ɂ͏C���s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(�t�����g�J�������s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FR_LEFT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
		else if ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) 
		{
			/* �J����������ʂ����A�J�����ł���ہA���}�[�J�̓����_�擾�ɂ͏C���s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(���A�J�������s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_RR_LEFT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
		else if ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
		{
			/* �J����������ʂ��t�����g�J�����ł���ہA�E�}�[�J�̓����_�擾�ɂ͏C���s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(�t�����g�J�����E�s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FR_RIGHT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
		else if ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) 
		{
			/* �J����������ʂ����A�J�����ł���ہA�E�}�[�J�̓����_�擾�ɂ͏C���s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(���A�J�����E�s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_RR_RIGHT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
#else	/* CALIB_PARAM_P32S_EUR_UK */
		if ( ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
			|| ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) )
		{
			/* �J����������ʂ��t�����g�J�����E�܂��̓��A�J�����ł���ہA���}�[�J�̓����_�擾�ɂ͏C���s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(�t�����g�J�����E���A�J�������s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FRRR_LEFT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ),nCamDirection );
		}
		else if ( ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
			|| ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) )
		{
			/* �J����������ʂ��t�����g�J�����E�܂��̓��A�J�����ł���ہA�E�}�[�J�̓����_�擾�ɂ͏C���s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(�t�����g�J�����E���A�J�����E�s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FRRR_RIGHT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ),nCamDirection );
		}
#endif /* CALIB_PARAM_P32S_EUR_UK */
		else
		{
			/* �J����������ʂ����J�����܂��͉E�J�����ł���ہA�����ʎ擾�ɂ͎s���p�^�[����p���� */

			/* �d�ݕt��Haar-like(�s��) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}

		if ( CB_IMG_OK != nRet )
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_HAAR_LIKE_FTR_CHK_EDGE );
		}

/* [DEBUG] */
#ifdef CB_FPE_CSW_DEBUG_ON
#if 0
	cb_FPE_DBG_SaveHaarLikeInfo( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), "C:/SELF_CB_DBG_WEIGHTED_HAAR_LIKE_CHK_EDGE.csv" );
#endif
#endif /* CB_FPE_CSW_DEBUG_ON */

		/* �\���_�T���p��臒l�ݒ� */
		if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
		{
			nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT_FRRR;
		}
		else
		{
			nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT;
		}

		/* �\���_�T�� */
		
		/* ���E�s���p�̏\���_�T���Ȃ̂ŁA��5����(�T���_��)�� 1 �ŌŒ� */
		nRet = cb_FPE_SearchCandCrossPnt( &( ptFPEInfo->nImg[0] ), &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &(ptSrchRsltPntInfo[ni].tCrossPnt[0]), &(ptSrchRsltPntInfo[ni].nNumCrossPnt), 
										&(ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize.tAddChkSrchRgn), 1L, nCamDirection, nprm_thrHaarFtrCrossPnt, &(ptSrchRgnInfo[ni].tPtnInfo), 0U );
		if ( CB_IMG_OK != nRet )
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_SRCH_CAND_CROSS_PNT );
		}

		/*
		==============================
			�㉺T���R�[�i�[�_�T��
		==============================
		*/
		/* �G�b�W�R�[�h���̒��o */
		nRet = cb_FPE_ExtractEdgeCodeInfo( ( ptFPEInfo->nImg )[CB_FPE_IMG_NORMAL], 
											( ptFPEInfo->nImg )[CB_FPE_IMG_EDGE_RHO_NORMAL], 
											( ptFPEInfo->nImg )[CB_FPE_IMG_EDGE_ANGLE_NORMAL], 
											( ptFPEInfo->nImg )[CB_FPE_IMG_EDGE_AC_NORMAL], 
											&( ptSrchRgnInfo[ni].tRgnImgNormalSize ), 
											nCamDirection );
		if ( CB_IMG_OK != nRet )
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_EDGE_CODE_INFO_ZOOMOUT );
		}

		/* ����T���_�T�� */
		/* �k���摜��ŁA�G�b�W�_���o(�����ō��E�s���}�[�J�[�ŏ�������ł���悤�ɁAni�������Ƃ��Ēǉ�) */
		nRet = cb_FPE_SearchCandTPntUL4AddChk_byEdgeCode( &( ptFPEInfo->nImg[0] ), 
														&(ptSrchRsltPntInfo[ni].tCrossPnt[0]), 1L, 
														&(ptSrchRsltPntInfo[ni].tTPntLower[0]), &(ptSrchRsltPntInfo[ni].nNumTPntLower), 
														&(ptSrchRgnInfo[ni].tRgnImgNormalSize), CB_FPE_LOWER_EDGE_POS_PATTERN, ni, nCamDirection, &(ptSrchRgnInfo->tPtnInfo) );	
		if ( CB_IMG_OK != nRet )
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_SRCH_CAND_T_PNT_LOWER_ADD_CHK_PATTERN );
		}

		/* �㑤�s���_�T�� */
		nRet = cb_FPE_SearchCandTPntUL4AddChk_byEdgeCode( &( ptFPEInfo->nImg[0] ), 
														&(ptSrchRsltPntInfo[ni].tCrossPnt[0]), 1L, 
														&(ptSrchRsltPntInfo[ni].tTPntUpper[0]), &(ptSrchRsltPntInfo[ni].nNumTPntUpper),
														&(ptSrchRgnInfo[ni].tRgnImgNormalSize), CB_FPE_UPPER_EDGE_POS_PATTERN, ni, nCamDirection, &(ptSrchRgnInfo->tPtnInfo) );
		if ( CB_IMG_OK != nRet )
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_SRCH_CAND_T_PNT_UPPER_ADD_CHK_PATTERN );
		}

		/*
		==============================
			�����x�ʒu����
		==============================
		*/

		/* ���E�s���}�[�J�[�̏㉺�����_�𒊏o */
		nRet = cb_FPE_EstimateFtrPoint4AddChk( nImgSrc, &(ptFPEInfo->nImg[0]), &(ptSrchRgnInfo[ni]) , &(ptSrchRsltPntInfo[ni]), nCamDirection );

		/* ���E�s���}�[�J�[�̒����\���_�𒊏o */
		nRet = cb_FPE_EstimateFtrPoint4AddCrossPnt( nImgSrc, &(ptFPEInfo->nImg[0]), &(ptSrchRgnInfo[ni]) , &(ptSrchRsltPntInfo[ni]), nCamDirection );


	}

	/* Haar-like�����ʗp�̉摜���������蓖�ĉ���(�摜�������A�N�Z�X�I��) */
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}



	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMP�̃G���[���܂Ƃ߂ă`�F�b�N */
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(���E�s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.11.05	S.Suzuki			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	float_t	fprm_RcpHaarArea;		/* Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_NormHaarWeight;	/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;	/* Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;	/* Haar-like����`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_xsizeBlockW * nprm_ysizeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;


	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �擪�|�C���^ */
		/* ����̃u���b�N */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6767
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6812
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6814
		/* �E��̃u���b�N */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6772
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6773
		/* �����̃u���b�N */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̃u���b�N */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6774
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6775
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6776
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6777

		/* ����̏��u���b�N */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );			// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6767
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6812
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#4] O3.1  , ID-6814
		/* �E��̏��u���b�N */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6772
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6773
		/* �����̏��u���b�N */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��u���b�N */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6774
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6775
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6776
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.2  , ID-6777

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �e�u���b�N���̋P�x�l���v */
			nSumBlockLU = ( ( (*pnWkSumRL_BlockLU) - (*pnWkSumRU_BlockLU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU);
			nSumBlockRU = ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockRU) ) + (*pnWkSumLU_BlockRU);
			nSumBlockLL = ( ( (*pnWkSumRL_BlockLL) - (*pnWkSumRU_BlockLL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL);
			nSumBlockRL = ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockRL) ) + (*pnWkSumLU_BlockRL);

			nSumSmallBlockLU = ( ( (*pnWkSumRL_SmallBlockLU) - (*pnWkSumRU_SmallBlockLU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU);
			nSumSmallBlockRU = ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockRU) ) + (*pnWkSumLU_SmallBlockRU);
			nSumSmallBlockLL = ( ( (*pnWkSumRL_SmallBlockLL) - (*pnWkSumRU_SmallBlockLL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL);
			nSumSmallBlockRL = ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockRL) ) + (*pnWkSumLU_SmallBlockRL);

			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j
			                     �@      �������������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumBlockLL - nSumBlockRU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumBlockLU - nSumBlockRL ) ) * fprm_CoeffHaarWeight;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumSmallBlockLL - nSumSmallBlockRU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumSmallBlockLU - nSumSmallBlockRL ) ) * fprm_CoeffSmallHaarWeight;

			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
//			*pnWkFtr = ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL );
//			*pnWkFtr =	(slong)( (float_t)( ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU);
			fHaar = ( (float_t)( ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			fSmallHaar = ( (float_t)( ( nSumSmallBlockLU + nSumSmallBlockRL ) - ( nSumSmallBlockRU + nSumSmallBlockLL ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				*pnWkFtr = (slong)( fHaar * fabsf( fSmallHaar ) );
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(�t�����g�J�������s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.08.01	S.Morita			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge_FR_LEFT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	slong	Modify_Leng, Modify_SmallLeng;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	CB_FPE_BRIGHT_CALC_INFO stCalcInfo;
	CB_FPE_BRIGHT_CALC_INFO stCalcInfo_small;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* ��̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpSmallHaarArea;			/* ���̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeHaarArea;			/* ��̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeSmallHaarArea;		/* ���̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */

	float_t	fprm_NormHaarWeight;			/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;			/* ��̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;		/* ���̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeHaarWeight;		/* ��̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* ���̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �\���̏����� */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FR;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FR;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FR;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FR;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeBlockW) || (0L ==  nprm_areaSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}

	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_areaSmallBlockW ) );
	fprm_RcpLargeHaarArea = ( 1.0F / (float_t)( nprm_areaLargeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeSmallBlockW) || (0L ==  nprm_areaSmallSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpSmallHaarArea = ( 1.0F / (float_t)( nprm_areaSmallSmallBlockW ) );
	fprm_RcpLargeSmallHaarArea = ( 1.0F / (float_t)( nprm_areaLargeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �P�x�l���v�̏����� */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �C���s���p�^�[���쐬���쐬����ׂ̒l  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* �擪�|�C���^ */
		/* ����̗̈� */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW - Modify_Leng;
		/* �E��̗̈� */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		/* �����̗̈� */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̗̈� */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* ����̏��̈� */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* �E��̏��̈� */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* �����̏��̈� */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��̈� */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;
	
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �����ʊi�[�ϐ������� */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* ���}�[�J�[�̓����_���擾����ꍇ */
	
			/* �e�̈���̋P�x�l���v */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumLL_BlockRU;
			stCalcInfo.pnFtrValue[1] = pnWkSumRU_BlockLL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumLL_SmallBlockRU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumRU_SmallBlockLL;

			/* �ʏ�̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* ���̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			nSumBlockRU = stCalcInfo.CalcBrightness[0];
			nSumBlockLL = stCalcInfo.CalcBrightness[1];
			nSumSmallBlockRU = stCalcInfo_small.CalcBrightness[0];
			nSumSmallBlockLL = stCalcInfo_small.CalcBrightness[1];
			
			nSumBlockLU = ( ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU) ) - nSumBlockRU;
			nSumBlockRL = ( ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL) ) - nSumBlockLL;
			nSumSmallBlockLU = ( ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU) ) - nSumSmallBlockRU;
			nSumSmallBlockRL = ( ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL) ) - nSumSmallBlockLL;
			
			/* ���}�[�J�̓����ʂ��Z�o����p�^�[����p�����ꍇ�̏d�ݐݒ� */
			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			
			/*                   �@    ���������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeSmallHaarWeight;

			/* ���}�[�J�̓����ʂ��Z�o����p�^�[���́A���̈�̖ʐς̑��a�����̈�̖ʐς̑��a�����傫�� */
			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );

			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(���A�J�������s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.08.01	S.Morita			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge_RR_LEFT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	slong	Modify_Leng, Modify_SmallLeng;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	CB_FPE_BRIGHT_CALC_INFO stCalcInfo;
	CB_FPE_BRIGHT_CALC_INFO stCalcInfo_small;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* ��̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpSmallHaarArea;			/* ���̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeHaarArea;			/* ��̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeSmallHaarArea;		/* ���̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */

	float_t	fprm_NormHaarWeight;			/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;			/* ��̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;		/* ���̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeHaarWeight;		/* ��̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* ���̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �\���̏����� */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeBlockW) || (0L ==  nprm_areaSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}

	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_areaSmallBlockW ) );
	fprm_RcpLargeHaarArea = ( 1.0F / (float_t)( nprm_areaLargeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeSmallBlockW) || (0L ==  nprm_areaSmallSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpSmallHaarArea = ( 1.0F / (float_t)( nprm_areaSmallSmallBlockW ) );
	fprm_RcpLargeSmallHaarArea = ( 1.0F / (float_t)( nprm_areaLargeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �P�x�l���v�̏����� */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �C���s���p�^�[���쐬���쐬����ׂ̒l  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* �擪�|�C���^ */
		/* ����̗̈� */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW - Modify_Leng;
		/* �E��̗̈� */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		/* �����̗̈� */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̗̈� */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* ����̏��̈� */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* �E��̏��̈� */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* �����̏��̈� */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��̈� */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;
	
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �����ʊi�[�ϐ������� */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* ���}�[�J�[�̓����_���擾����ꍇ */
	
			/* �e�̈���̋P�x�l���v */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumLL_BlockRU;
			stCalcInfo.pnFtrValue[1] = pnWkSumRU_BlockLL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumLL_SmallBlockRU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumRU_SmallBlockLL;

			/* �ʏ�̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* ���̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			nSumBlockRU = stCalcInfo.CalcBrightness[0];
			nSumBlockLL = stCalcInfo.CalcBrightness[1];
			nSumSmallBlockRU = stCalcInfo_small.CalcBrightness[0];
			nSumSmallBlockLL = stCalcInfo_small.CalcBrightness[1];
			
			nSumBlockLU = ( ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU) ) - nSumBlockRU;
			nSumBlockRL = ( ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL) ) - nSumBlockLL;
			nSumSmallBlockLU = ( ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU) ) - nSumSmallBlockRU;
			nSumSmallBlockRL = ( ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL) ) - nSumSmallBlockLL;
			
			/* ���}�[�J�̓����ʂ��Z�o����p�^�[����p�����ꍇ�̏d�ݐݒ� */
			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			/*                   �@    ���������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeSmallHaarWeight;

			/* ���}�[�J�̓����ʂ��Z�o����p�^�[���́A���̈�̖ʐς̑��a�����̈�̖ʐς̑��a�����傫�� */
			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			
			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(�t�����g�J�����E�s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.08.01	S.Morita			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge_FR_RIGHT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	slong	Modify_Leng, Modify_SmallLeng;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	CB_FPE_BRIGHT_CALC_INFO stCalcInfo;
	CB_FPE_BRIGHT_CALC_INFO stCalcInfo_small;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* ��̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpSmallHaarArea;			/* ���̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeHaarArea;			/* ��̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeSmallHaarArea;		/* ���̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */

	float_t	fprm_NormHaarWeight;			/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;			/* ��̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;		/* ���̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeHaarWeight;		/* ��̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* ���̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �\���̏����� */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FR;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FR;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FR;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FR;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeBlockW) || (0L ==  nprm_areaSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}

	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_areaSmallBlockW ) );
	fprm_RcpLargeHaarArea = ( 1.0F / (float_t)( nprm_areaLargeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeSmallBlockW) || (0L ==  nprm_areaSmallSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpSmallHaarArea = ( 1.0F / (float_t)( nprm_areaSmallSmallBlockW ) );
	fprm_RcpLargeSmallHaarArea = ( 1.0F / (float_t)( nprm_areaLargeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �P�x�l���v�̏����� */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* �J������ʂ��t�����g�J�����ł���ꍇ�͓����_�擾�ɗp����p�^�[����ύX���� */
	/* �E�}�[�J�[�̓����_���擾����ꍇ�ɂ́A����ƉE�������ԑΊp����p�����C���s���p�^�[����p���� */

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �C���s���p�^�[���쐬���쐬����ׂ̒l  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* �擪�|�C���^ */
		/* ����̗̈� */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW + Modify_Leng;
		/* �E��̗̈� */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		/* �����̗̈� */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̗̈� */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* ����̏��̈� */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* �E��̏��̈� */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* �����̏��̈� */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��̈� */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �����ʊi�[�ϐ������� */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* �E�}�[�J�[�̓����_���擾����ꍇ */

			/* �e�̈���̋P�x�l���v */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumRL_BlockLU;
			stCalcInfo.pnFtrValue[1] = pnWkSumLU_BlockRL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumRL_SmallBlockLU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumLU_SmallBlockRL;

			/* �ʏ�̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* ���̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			nSumBlockLU = stCalcInfo.CalcBrightness[0];
			nSumBlockRL = stCalcInfo.CalcBrightness[1];
			nSumSmallBlockLU = stCalcInfo_small.CalcBrightness[0];
			nSumSmallBlockRL = stCalcInfo_small.CalcBrightness[1];

			nSumBlockRU = ( ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU) ) - nSumBlockLU;
			nSumBlockLL = ( ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL) ) - nSumBlockRL;
			nSumSmallBlockRU = ( ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU) ) - nSumSmallBlockLU;
			nSumSmallBlockLL = ( ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL) ) - nSumSmallBlockRL;

			/* �E�}�[�J�̓����ʂ��Z�o����p�^�[����p�����ꍇ�̏d�ݐݒ� */
			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			/*                   �@    ���������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffSmallHaarWeight;


			/* �E�}�[�J�̓����ʂ��Z�o����p�^�[���́A���̈�̖ʐς̑��a�����̈�̖ʐς̑��a�����傫�� */
			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( (float_t)tempLU_RL * fprm_RcpHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU;

			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( (float_t)tempLU_RL * fprm_RcpSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU;

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(���A�J�����E�s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.08.01	S.Morita			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge_RR_RIGHT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	slong	Modify_Leng, Modify_SmallLeng;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	CB_FPE_BRIGHT_CALC_INFO stCalcInfo;
	CB_FPE_BRIGHT_CALC_INFO stCalcInfo_small;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* ��̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpSmallHaarArea;			/* ���̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeHaarArea;			/* ��̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeSmallHaarArea;		/* ���̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */

	float_t	fprm_NormHaarWeight;			/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;			/* ��̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;		/* ���̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeHaarWeight;		/* ��̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* ���̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �\���̏����� */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeBlockW) || (0L ==  nprm_areaSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}

	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_areaSmallBlockW ) );
	fprm_RcpLargeHaarArea = ( 1.0F / (float_t)( nprm_areaLargeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeSmallBlockW) || (0L ==  nprm_areaSmallSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpSmallHaarArea = ( 1.0F / (float_t)( nprm_areaSmallSmallBlockW ) );
	fprm_RcpLargeSmallHaarArea = ( 1.0F / (float_t)( nprm_areaLargeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �P�x�l���v�̏����� */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* �J������ʂ����A�J�����ł���ꍇ�͓����_�擾�ɗp����p�^�[����ύX���� */
	/* �E�}�[�J�[�̓����_���擾����ꍇ�ɂ́A����ƉE�������ԑΊp����p�����C���s���p�^�[����p���� */

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �C���s���p�^�[���쐬���쐬����ׂ̒l  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* �擪�|�C���^ */
		/* ����̗̈� */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW + Modify_Leng;
		/* �E��̗̈� */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		/* �����̗̈� */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̗̈� */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* ����̏��̈� */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* �E��̏��̈� */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* �����̏��̈� */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��̈� */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �����ʊi�[�ϐ������� */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* �E�}�[�J�[�̓����_���擾����ꍇ */

			/* �e�̈���̋P�x�l���v */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumRL_BlockLU;
			stCalcInfo.pnFtrValue[1] = pnWkSumLU_BlockRL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumRL_SmallBlockLU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumLU_SmallBlockRL;

			/* �ʏ�̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* ���̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			nSumBlockLU = stCalcInfo.CalcBrightness[0];
			nSumBlockRL = stCalcInfo.CalcBrightness[1];
			nSumSmallBlockLU = stCalcInfo_small.CalcBrightness[0];
			nSumSmallBlockRL = stCalcInfo_small.CalcBrightness[1];

			nSumBlockRU = ( ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU) ) - nSumBlockLU;
			nSumBlockLL = ( ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL) ) - nSumBlockRL;
			nSumSmallBlockRU = ( ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU) ) - nSumSmallBlockLU;
			nSumSmallBlockLL = ( ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL) ) - nSumSmallBlockRL;

			/* �E�}�[�J�̓����ʂ��Z�o����p�^�[����p�����ꍇ�̏d�ݐݒ� */
			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			/*                   �@      �������������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffSmallHaarWeight;

			/* �E�}�[�J�̓����ʂ��Z�o����p�^�[���́A���̈�̖ʐς̑��a�����̈�̖ʐς̑��a�����傫�� */
			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( (float_t)tempLU_RL * fprm_RcpHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU;

			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( (float_t)tempLU_RL * fprm_RcpSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU;

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifndef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(�t�����g�J�����E���A�J�������s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.08.01	S.Morita			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge_FRRR_LEFT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo ,slong nCamDirection)
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	slong	Modify_Leng, Modify_SmallLeng;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	CB_FPE_BRIGHT_CALC_INFO stCalcInfo;
	CB_FPE_BRIGHT_CALC_INFO stCalcInfo_small;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* ��̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpSmallHaarArea;			/* ���̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeHaarArea;			/* ��̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeSmallHaarArea;		/* ���̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */

	float_t	fprm_NormHaarWeight;			/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;			/* ��̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;		/* ���̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeHaarWeight;		/* ��̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* ���̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �\���̏����� */
	memset( &stCalcInfo, 0x00, sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, sizeof(stCalcInfo_small));
	
#ifdef CB_FPE_MIZUSHIMA_FACTORY

	if(E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection){
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 1;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 2;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 3;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 4;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 4;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 5;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 6;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 7;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 7;														

		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 1;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 2;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 3;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 4;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 4;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 5;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 6;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 7;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 7;	
	}else{
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 2;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 3;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 4;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 5;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 6;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 7;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 8;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 9;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 10;														

		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 2;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 3;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 4;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 5;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 6;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 7;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 8;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 9;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 10;
	}
	
#endif

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;

#ifdef CB_FPE_MIZUSHIMA_FACTORY
	if(E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection){
		nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FRONT;
		nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FRONT;
		nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FRONT;
		nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FRONT;
	}else{
#endif
		nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE;
		nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL;
		nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL;
		nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL;
#ifdef CB_FPE_MIZUSHIMA_FACTORY
	}
#endif

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeBlockW) || (0L ==  nprm_areaSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}

	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_areaSmallBlockW ) );
	fprm_RcpLargeHaarArea = ( 1.0F / (float_t)( nprm_areaLargeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeSmallBlockW) || (0L ==  nprm_areaSmallSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpSmallHaarArea = ( 1.0F / (float_t)( nprm_areaSmallSmallBlockW ) );
	fprm_RcpLargeSmallHaarArea = ( 1.0F / (float_t)( nprm_areaLargeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �P�x�l���v�̏����� */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �C���s���p�^�[���쐬���쐬����ׂ̒l  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* �擪�|�C���^ */
		/* ����̗̈� */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW - Modify_Leng;
		/* �E��̗̈� */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		/* �����̗̈� */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̗̈� */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* ����̏��̈� */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* �E��̏��̈� */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* �����̏��̈� */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��̈� */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;
	
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �����ʊi�[�ϐ������� */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* ���}�[�J�[�̓����_���擾����ꍇ */
	
			/* �e�̈���̋P�x�l���v */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumLL_BlockRU;
			stCalcInfo.pnFtrValue[1] = pnWkSumRU_BlockLL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumLL_SmallBlockRU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumRU_SmallBlockLL;

			/* �ʏ�̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo, tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT]  );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* ���̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo_small, tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT]  );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			nSumBlockRU = stCalcInfo.CalcBrightness[0];
			nSumBlockLL = stCalcInfo.CalcBrightness[1];
			nSumSmallBlockRU = stCalcInfo_small.CalcBrightness[0];
			nSumSmallBlockLL = stCalcInfo_small.CalcBrightness[1];
			
			nSumBlockLU = ( ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU) ) - nSumBlockRU;
			nSumBlockRL = ( ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL) ) - nSumBlockLL;
			nSumSmallBlockLU = ( ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU) ) - nSumSmallBlockRU;
			nSumSmallBlockRL = ( ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL) ) - nSumSmallBlockLL;
			
			/* ���}�[�J�̓����ʂ��Z�o����p�^�[����p�����ꍇ�̏d�ݐݒ� */
			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j
			                     �@      �������������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumBlockLL - nSumBlockRU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumBlockLU - nSumBlockRL ) ) * fprm_CoeffLargeHaarWeight;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumSmallBlockLL - nSumSmallBlockRU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumSmallBlockLU - nSumSmallBlockRL ) ) * fprm_CoeffLargeSmallHaarWeight;

			/* ���}�[�J�̓����ʂ��Z�o����p�^�[���́A���̈�̖ʐς̑��a�����̈�̖ʐς̑��a�����傫�� */
			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
			fHaar = ( (float_t)( ( ( nSumBlockLU + nSumBlockRL ) * fprm_RcpLargeHaarArea ) - ( ( nSumBlockRU + nSumBlockLL ) * fprm_RcpHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			fSmallHaar = ( (float_t)( ( ( nSumSmallBlockLU + nSumSmallBlockRL ) * fprm_RcpLargeSmallHaarArea ) - ( ( nSumSmallBlockRU + nSumSmallBlockLL ) * fprm_RcpSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				*pnWkFtr = (slong)( fHaar * fabsf( fSmallHaar ) );
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifndef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		�d�ݕt��Haar-like�����ʂ̌v�Z(�t�����g�J�����E���A�J�����E�s���p)
 *
 * @param[in]	nIntegralImg	:,�C���e�O�����C���[�W���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like�����ʍ\���̂ւ̃|�C���^,-,[-],
 * @param[in]	ptSrchRgnInfo	:,�����̈�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2014.08.01	S.Morita			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_WeightedHaarLikeChkEdge_FRRR_RIGHT( IMPLIB_IMGID nIntegralImg, const CB_FPE_HAAR_LIKE_FTR* const ptHaarLike, const CB_FPE_ADD_CHK_SRCH_RGN_INFO* const ptSrchRgnInfo ,slong nCamDirection)
{
	slong	nRet = CB_IMG_NG;
	slong	ni, nj;
	slong	nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn;
	slong	nXSizeSrchRgn, nYSizeSrchRgn;
	slong	nSumBlockLU, nSumBlockRU, nSumBlockLL, nSumBlockRL;
	slong	nSumSmallBlockLU, nSumSmallBlockRU, nSumSmallBlockLL, nSumSmallBlockRL;
	slong	nXSizeIImg, nYSizeIImg;
	slong	nSxProc, nSyProc, nExProc, nEyProc;
	slong	*pnAddrIImg;
	slong	*pnWkFtr;
	slong	*pnWkSumLU_BlockLU, *pnWkSumRU_BlockLU, *pnWkSumLL_BlockLU, *pnWkSumRL_BlockLU;
	slong	*pnWkSumLU_BlockRU, *pnWkSumRU_BlockRU, *pnWkSumLL_BlockRU, *pnWkSumRL_BlockRU;
	slong	*pnWkSumLU_BlockLL, *pnWkSumRU_BlockLL, *pnWkSumLL_BlockLL, *pnWkSumRL_BlockLL;
	slong	*pnWkSumLU_BlockRL, *pnWkSumRU_BlockRL, *pnWkSumLL_BlockRL, *pnWkSumRL_BlockRL;
	slong	*pnWkSumLU_SmallBlockLU, *pnWkSumRU_SmallBlockLU, *pnWkSumLL_SmallBlockLU, *pnWkSumRL_SmallBlockLU;
	slong	*pnWkSumLU_SmallBlockRU, *pnWkSumRU_SmallBlockRU, *pnWkSumLL_SmallBlockRU, *pnWkSumRL_SmallBlockRU;
	slong	*pnWkSumLU_SmallBlockLL, *pnWkSumRU_SmallBlockLL, *pnWkSumLL_SmallBlockLL, *pnWkSumRL_SmallBlockLL;
	slong	*pnWkSumLU_SmallBlockRL, *pnWkSumRU_SmallBlockRL, *pnWkSumLL_SmallBlockRL, *pnWkSumRL_SmallBlockRL;

	slong	Modify_Leng, Modify_SmallLeng;

	float_t fHaarWeight_LU_RL,fHaarWeight_LL_RU;
	float_t fSmallHaarWeight_LU_RL,fSmallHaarWeight_LL_RU;

	CB_FPE_BRIGHT_CALC_INFO stCalcInfo;
	CB_FPE_BRIGHT_CALC_INFO stCalcInfo_small;

	/* �p�����[�^ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* ��̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpSmallHaarArea;			/* ���̈�Haar-like��`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeHaarArea;			/* ��̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */
	float_t	fprm_RcpLargeSmallHaarArea;		/* ���̈�Haar-like���`�̖ʐς̋t���A�P�x���ϒl�̌v�Z�Ɏg�p */

	float_t	fprm_NormHaarWeight;			/* �d�݂̐��K���p�p�����[�^(255[�P�x�l]�̋t��) */

	float_t	fprm_CoeffHaarWeight;			/* ��̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffSmallHaarWeight;		/* ���̈�Haar-like��`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeHaarWeight;		/* ��̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* ���̈�Haar-like���`�̖ʐς̋t���~�d�ݒl */

	float_t	fHaar,fSmallHaar;

	/* �����`�F�b�N */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �\���̏����� */
	memset( &stCalcInfo, 0x00, sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, sizeof(stCalcInfo_small));

#ifdef CB_FPE_MIZUSHIMA_FACTORY

	if(E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection){
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 1;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 2;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 3;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 4;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 4;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 5;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 6;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 7;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 7;														

		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 1;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 2;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 3;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 4;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 4;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 5;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 6;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 7;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 7;	
	}else{
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 2;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 3;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 4;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 5;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 6;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 7;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 8;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 9;														
		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 10;														

		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][0] = 1;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][1] = 2;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][2] = 3;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][3] = 4;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][4] = 5;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][5] = 6;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][6] = 7;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][7] = 8;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][8] = 9;														
		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT][9] = 10;
	}													
#endif

	/* �����̈�擾 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* �p�����[�^�擾 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;

#ifdef CB_FPE_MIZUSHIMA_FACTORY
	if(E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection){
		nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FRONT;
		nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FRONT;
		nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FRONT;
		nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FRONT;
	}else{
#endif
		nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE;
		nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL;
		nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL;
		nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL;
#ifdef CB_FPE_MIZUSHIMA_FACTORY		
	}
#endif

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeBlockW) || (0L ==  nprm_areaSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}

	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_areaSmallBlockW ) );
	fprm_RcpLargeHaarArea = ( 1.0F / (float_t)( nprm_areaLargeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* ���K���̒l���v�Z */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0���`�F�b�N */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	if( (0L ==  nprm_areaLargeSmallBlockW) || (0L ==  nprm_areaSmallSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpSmallHaarArea = ( 1.0F / (float_t)( nprm_areaSmallSmallBlockW ) );
	fprm_RcpLargeSmallHaarArea = ( 1.0F / (float_t)( nprm_areaLargeSmallBlockW ) );

	/* ���K���̒l���v�Z */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* �������̈敪���l�����������̈� */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* �����T�C�Y */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �P�x�l���v�̏����� */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* �J������ʂ��t�����g�J�����A�������̓��A�J�����ł���ꍇ�͓����_�擾�ɗp����p�^�[����ύX���� */
	/* �E�}�[�J�[�̓����_���擾����ꍇ�ɂ́A����ƉE�������ԑΊp����p�����C���s���p�^�[����p���� */

	/* Haar-like */
	/* �E������_�ɂ��Čv�Z */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* �C���s���p�^�[���쐬���쐬����ׂ̒l  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* �擪�|�C���^ */
		/* ����̗̈� */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW + Modify_Leng;
		/* �E��̗̈� */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		/* �����̗̈� */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* �E���̗̈� */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* ����̏��̈� */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* �E��̏��̈� */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* �����̏��̈� */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* �E���̏��̈� */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* �d�݌v�Z�p�ϐ������� */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* �����ʊi�[�ϐ������� */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* �E�}�[�J�[�̓����_���擾����ꍇ */

			/* �e�̈���̋P�x�l���v */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumRL_BlockLU;
			stCalcInfo.pnFtrValue[1] = pnWkSumLU_BlockRL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumRL_SmallBlockLU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumLU_SmallBlockRL;

			/* �ʏ�̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo,tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT] );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* ���̈�ɂ����ăp�^�[���̏C���ɂ��ʐς����������̈�̋P�x�l���v���v�Z���� */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo_small, tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT] );
			if ( 0L > nRet )
			{
				/* �G���[���N���A */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			nSumBlockLU = stCalcInfo.CalcBrightness[0];
			nSumBlockRL = stCalcInfo.CalcBrightness[1];
			nSumSmallBlockLU = stCalcInfo_small.CalcBrightness[0];
			nSumSmallBlockRL = stCalcInfo_small.CalcBrightness[1];

			nSumBlockRU = ( ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU) ) - nSumBlockLU;
			nSumBlockLL = ( ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL) ) - nSumBlockRL;
			nSumSmallBlockRU = ( ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU) ) - nSumSmallBlockLU;
			nSumSmallBlockLL = ( ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL) ) - nSumSmallBlockRL;

			/* �E�}�[�J�̓����ʂ��Z�o����p�^�[����p�����ꍇ�̏d�ݐݒ� */
			/*
			 �P�x���ς̍��������ɁA�d�݂��v�Z
			 �@�F�召�t�]�����邽�߂̌��Z(1�ɋ߂��قǁA�d�݂��傫���Ȃ�ϊ�)
			 �A�F����ƉE�� / �����ƉE�� �̋P�x���ς̍�
			 �B�F�A��255(�P�x���̍ő�l)�Ŋ��邱�ƂŁA0�`1�ɐ��K���i���炩���ߌv�Z�����l�j
			                     �@      �������������������������������A������������              �B        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumBlockLL - nSumBlockRU ) ) * fprm_CoeffLargeHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumBlockLU - nSumBlockRL ) ) * fprm_CoeffHaarWeight;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumSmallBlockLL - nSumSmallBlockRU ) ) * fprm_CoeffLargeSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumSmallBlockLU - nSumSmallBlockRL ) ) * fprm_CoeffSmallHaarWeight;


			/* �E�}�[�J�̓����ʂ��Z�o����p�^�[���́A���̈�̖ʐς̑��a�����̈�̖ʐς̑��a�����傫�� */
			/* ������ */
			/* Haar-like�����ʂɏ�L�d�݂��|�����킹�A�s���炵���ӏ��̓����ʂ��傫���Ȃ�悤�ɒ���*/
			fHaar = ( (float_t)( ( ( nSumBlockLU + nSumBlockRL ) * fprm_RcpHaarArea ) - ( ( nSumBlockRU + nSumBlockLL ) * fprm_RcpLargeHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			fSmallHaar = ( (float_t)( ( ( nSumSmallBlockLU + nSumSmallBlockRL ) * fprm_RcpSmallHaarArea ) - ( ( nSumSmallBlockRU + nSumSmallBlockLL ) * fprm_RcpLargeSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* ���̈�ƒʏ�̈��Haar�����������̎� */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				*pnWkFtr = (slong)( fHaar * fabsf( fSmallHaar ) );
			}
			else
			{
				/* Haar�̒l���i�[���Ȃ� */
				*pnWkFtr = 0L;
			}

			/* �|�C���^��i�߂� */
			/* ���� */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* �E�� */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* ���� */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* �E�� */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* ���� */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* �E�� */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* ���� */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* �E�� */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

/******************************************************************************/
/**
 * @brief		�P�x�l�v�Z(�t�����g�J�����E���A�J�������̍��}�[�J�ɑ΂��Ďg�p����)
 *
 * @param[in out]	pnCalcBrightness		:,�P�x�l�v�Z���ʊi�[�z��ւ̃|�C���^,-,[-],
 * @param[in]		ptCalcInfo				:,�P�x�l�v�Z�ɗp����������\����,-,[-],
 *
 * @retval		CB_IMG_OK					:,����I��,value=0,[-],
 * @retval		CB_IMG_NG_ARG_NULL			:,�ُ�I��(����NULL)
 *
 * @date		2014.08.01	S.Morita		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcLeftBrightness( CB_FPE_BRIGHT_CALC_INFO *ptCalcInfo , const slong *pYPos)
{
	slong na;
	slong nloopsize;
	slong nSumBlockRU, nSumBlockLL;
	slong nXSizeIImg;

	slong *pnWkSumLL_BlockRU, *pnWkSumLL_BlockRU1, *pnWkSumLL_BlockRU2, *pnWkSumLL_BlockRU3, *pnWkSumLL_BlockRU4;
	slong *pnWkSumRU_BlockLL, *pnWkSumRU_BlockLL1, *pnWkSumRU_BlockLL2, *pnWkSumRU_BlockLL3, *pnWkSumRU_BlockLL4;
	
	nloopsize = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nXSizeIImg = 0L;

	/* �����`�F�b�N */
	if ( NULL == ptCalcInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nloopsize = ptCalcInfo->nHaarXSize;
	pnWkSumLL_BlockRU = ptCalcInfo->pnFtrValue[0];
	pnWkSumRU_BlockLL = ptCalcInfo->pnFtrValue[1];
	nXSizeIImg = ptCalcInfo->nXSize;

	/* �e�u���b�N���̋P�x�l���v */
#ifdef CB_FPE_MIZUSHIMA_FACTORY
	for ( na = 0L; na < nloopsize; na++ )
	{
		pnWkSumLL_BlockRU1 = pnWkSumLL_BlockRU + ( na + 1L );
		pnWkSumLL_BlockRU2 = pnWkSumLL_BlockRU + na;
		pnWkSumLL_BlockRU3 = ( pnWkSumLL_BlockRU + ( na + 1L ) ) - ( pYPos[na] * nXSizeIImg ) ;
		pnWkSumLL_BlockRU4 = ( pnWkSumLL_BlockRU + na ) - ( pYPos[na] * nXSizeIImg );

		pnWkSumRU_BlockLL1 = ( pnWkSumRU_BlockLL - na ) + ( pYPos[na] * nXSizeIImg );
		pnWkSumRU_BlockLL2 = pnWkSumRU_BlockLL - na;
		pnWkSumRU_BlockLL3 = ( pnWkSumRU_BlockLL - ( na + 1L ) ) + ( pYPos[na] * nXSizeIImg );
		pnWkSumRU_BlockLL4 = pnWkSumRU_BlockLL - ( na + 1L );

		nSumBlockRU += ( ( ( *pnWkSumLL_BlockRU1 ) - ( *pnWkSumLL_BlockRU2 ) ) - ( *pnWkSumLL_BlockRU3 ) ) + ( *pnWkSumLL_BlockRU4 );
		nSumBlockLL += ( ( ( *pnWkSumRU_BlockLL1 ) - ( *pnWkSumRU_BlockLL2 ) ) - ( *pnWkSumRU_BlockLL3 ) ) + ( *pnWkSumRU_BlockLL4 );
	}
#else
	for ( na = 0L; na < nloopsize; na++ )
	{
		pnWkSumLL_BlockRU1 = pnWkSumLL_BlockRU + ( na + 1L );
		pnWkSumLL_BlockRU2 = pnWkSumLL_BlockRU + na;
		pnWkSumLL_BlockRU3 = ( pnWkSumLL_BlockRU + ( na + 1L ) ) - ( ( na + 1L ) * nXSizeIImg ) ;
		pnWkSumLL_BlockRU4 = ( pnWkSumLL_BlockRU + na ) - ( ( na + 1L ) * nXSizeIImg );

		pnWkSumRU_BlockLL1 = ( pnWkSumRU_BlockLL - na ) + ( ( na + 1L ) * nXSizeIImg );
		pnWkSumRU_BlockLL2 = pnWkSumRU_BlockLL - na;
		pnWkSumRU_BlockLL3 = ( pnWkSumRU_BlockLL - ( na + 1L ) ) + ( ( na + 1L ) * nXSizeIImg );
		pnWkSumRU_BlockLL4 = pnWkSumRU_BlockLL - ( na + 1L );

		nSumBlockRU += ( ( ( *pnWkSumLL_BlockRU1 ) - ( *pnWkSumLL_BlockRU2 ) ) - ( *pnWkSumLL_BlockRU3 ) ) + ( *pnWkSumLL_BlockRU4 );
		nSumBlockLL += ( ( ( *pnWkSumRU_BlockLL1 ) - ( *pnWkSumRU_BlockLL2 ) ) - ( *pnWkSumRU_BlockLL3 ) ) + ( *pnWkSumRU_BlockLL4 );
	}
#endif

	ptCalcInfo->CalcBrightness[0] = nSumBlockRU;
	ptCalcInfo->CalcBrightness[1] = nSumBlockLL;

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief		�P�x�l�v�Z(�t�����g�J�����E���A�J�������̉E�}�[�J�ɑ΂��Ďg�p����)
 *
 * @param[in out]	pnCalcBrightness		:,�P�x�l�v�Z���ʊi�[�z��ւ̃|�C���^,-,[-],
 * @param[in]		ptCalcInfo				:,�P�x�l�v�Z�ɗp����������\����,-,[-],
 *
 * @retval		CB_IMG_OK					:,����I��,value=0,[-],
 * @retval		CB_IMG_NG_ARG_NULL			:,�ُ�I��(����NULL)
 *
 * @date		2014.08.01	S.Morita		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcRightBrightness( CB_FPE_BRIGHT_CALC_INFO *ptCalcInfo, const slong *pYPos )
{
	slong na;
	slong nloopsize;
	slong nSumBlockLU, nSumBlockRL;
	slong nXSizeIImg;

	slong *pnWkSumRL_BlockLU, *pnWkSumRL_BlockLU1, *pnWkSumRL_BlockLU2, *pnWkSumRL_BlockLU3, *pnWkSumRL_BlockLU4;
	slong *pnWkSumLU_BlockRL, *pnWkSumLU_BlockRL1, *pnWkSumLU_BlockRL2, *pnWkSumLU_BlockRL3, *pnWkSumLU_BlockRL4;

	nloopsize = 0L;
	nSumBlockLU = 0L;
	nSumBlockRL = 0L;

	nXSizeIImg = 0L;

	/* �����`�F�b�N */
	if ( NULL == ptCalcInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nloopsize = ptCalcInfo->nHaarXSize;
	pnWkSumRL_BlockLU = ptCalcInfo->pnFtrValue[0];
	pnWkSumLU_BlockRL = ptCalcInfo->pnFtrValue[1];
	nXSizeIImg = ptCalcInfo->nXSize;

	/* �e�u���b�N���̋P�x�l���v */
#ifdef CB_FPE_MIZUSHIMA_FACTORY
	for ( na = 0L; na < nloopsize; na++ )
	{
		pnWkSumRL_BlockLU1 = pnWkSumRL_BlockLU - na;
		pnWkSumRL_BlockLU2 = ( pnWkSumRL_BlockLU - na ) - ( pYPos[na] * nXSizeIImg );
		pnWkSumRL_BlockLU3 = pnWkSumRL_BlockLU - ( na + 1L );
		pnWkSumRL_BlockLU4 = ( pnWkSumRL_BlockLU - ( na + 1L ) ) - ( pYPos[na] * nXSizeIImg );

		pnWkSumLU_BlockRL1 = ( pnWkSumLU_BlockRL + ( na + 1L ) ) + ( pYPos[na] * nXSizeIImg );
		pnWkSumLU_BlockRL2 = pnWkSumLU_BlockRL + ( na + 1L );
		pnWkSumLU_BlockRL3 = ( pnWkSumLU_BlockRL + na ) + ( pYPos[na] * nXSizeIImg );
		pnWkSumLU_BlockRL4 = pnWkSumLU_BlockRL + na;

		nSumBlockLU += ( ( ( *pnWkSumRL_BlockLU1 ) - ( *pnWkSumRL_BlockLU2 ) ) - ( *pnWkSumRL_BlockLU3 ) ) + ( *pnWkSumRL_BlockLU4 );
		nSumBlockRL += ( ( ( *pnWkSumLU_BlockRL1 ) - ( *pnWkSumLU_BlockRL2 ) ) - ( *pnWkSumLU_BlockRL3 ) ) + ( *pnWkSumLU_BlockRL4 );
	}
#else
	for ( na = 0L; na < nloopsize; na++ )
	{
		pnWkSumRL_BlockLU1 = pnWkSumRL_BlockLU - na;
		pnWkSumRL_BlockLU2 = ( pnWkSumRL_BlockLU - na ) - ( ( na + 1L ) * nXSizeIImg );
		pnWkSumRL_BlockLU3 = pnWkSumRL_BlockLU - ( na + 1L );
		pnWkSumRL_BlockLU4 = ( pnWkSumRL_BlockLU - ( na + 1L ) ) - ( ( na + 1L ) * nXSizeIImg );

		pnWkSumLU_BlockRL1 = ( pnWkSumLU_BlockRL + ( na + 1L ) ) + ( ( na + 1L ) * nXSizeIImg );
		pnWkSumLU_BlockRL2 = pnWkSumLU_BlockRL + ( na + 1L );
		pnWkSumLU_BlockRL3 = ( pnWkSumLU_BlockRL + na ) + ( ( na + 1L ) * nXSizeIImg );
		pnWkSumLU_BlockRL4 = pnWkSumLU_BlockRL + na;

		nSumBlockLU += ( ( ( *pnWkSumRL_BlockLU1 ) - ( *pnWkSumRL_BlockLU2 ) ) - ( *pnWkSumRL_BlockLU3 ) ) + ( *pnWkSumRL_BlockLU4 );
		nSumBlockRL += ( ( ( *pnWkSumLU_BlockRL1 ) - ( *pnWkSumLU_BlockRL2 ) ) - ( *pnWkSumLU_BlockRL3 ) ) + ( *pnWkSumLU_BlockRL4 );
	}
#endif
	ptCalcInfo->CalcBrightness[0] = nSumBlockLU;
	ptCalcInfo->CalcBrightness[1] = nSumBlockRL;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�����l�Z�o
 *
 * @param[out]	nMedianValue				:,�Z�o���������l,-,[-],
 * @param[in]	nValArray[3]				:,�l���i�[����Ă���z��,-,[-],
 * @param[in]	nNumValue					:,�z��̃T�C�Y,-,[-],
 * @retval		CB_IMG_OK					:,����I��,value=0,[-],
 * @retval		CB_IMG_NG_ARG_NULL			:,�ُ�I��(����NULL)
 *
 * @date		2014.08.05	S.Morita		�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_CalcMedian( slong* nMedianValue, const slong* const nValArray, const slong nNumValue)
{
	slong	nRet = CB_IMG_NG;

	/* �����`�F�b�N */
	if (( NULL == nMedianValue ) || ( NULL == nValArray ))
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �z����\�[�g���� */
	nRet = cb_FPE_Sort( nValArray, nNumValue );
	if ( CB_IMG_OK != nRet ) 
	{
		return ( CB_IMG_NG );
	}
	
	/* �����l���擾����i�z��j*/
	/* ���z��T�C�Y�������̏ꍇ�͔z��̐^�񒆂̂P�������v�f���𒆉��l�ɂ��� */
	*nMedianValue = nValArray[ ( nNumValue - 1L ) / 2L ];
	
	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�G�b�W�p�x => �p�x�R�[�h�ϊ��e�[�u���̍쐬
 *
 * @param[out]	pnTbl4FrRr					:,�ϊ��e�[�u��(Fr/Rr�p)�ւ̃|�C���^,-,[-],
 * @param[out]	pnTbl4LR					:,�ϊ��e�[�u��(SL/SR�p)�ւ̃|�C���^,-,[-],
 *
 * @retval		CB_IMG_OK					:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)				:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.12.02	K.Kato			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_SetEdgeACTable( IMPLIB_CNVLUT* pnTbl4FrRr, IMPLIB_CNVLUT* pnTbl4LR )
{
	slong	ni, 
			nj;
	slong	nMaxEdgeCodeAngle;
	slong	nEdgeAngle, 
			nEdgeAC, 
			nCntIns;
	IMPLIB_CNVLUT	*pnTopAddrCnvLUT[CB_FPE_EDGE_AC_TBL_MAXNUM],
					*pnCnvLUT;
	/* �p�����[�^ */
	slong	nprm_maxEdgeAC;
	slong	nprm_startEdgeAngle[CB_FPE_EDGE_AC_TBL_MAXNUM];
	slong	nprm_rangeEdgeAngleLR[CB_FPE_EDGE_AC_TBL_MAXNUM], 
			nprm_rangeEdgeAngleUL[CB_FPE_EDGE_AC_TBL_MAXNUM];

	/* �����`�F�b�N */
	if ( ( NULL == pnTbl4FrRr ) || ( NULL == pnTbl4LR ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nMaxEdgeCodeAngle = 181L;

	/* �p�����[�^ */
	nprm_maxEdgeAC = CB_FPE_EDGE_AC_MAXNUM;
	nprm_startEdgeAngle[CB_FPE_EDGE_AC_TBL_FR_RR] = CB_FPE_PRM_START_EDGE_ANGLE_FR_RR;
	nprm_startEdgeAngle[CB_FPE_EDGE_AC_TBL_SL_SR] = CB_FPE_PRM_START_EDGE_ANGLE_SL_SR;
	nprm_rangeEdgeAngleLR[CB_FPE_EDGE_AC_TBL_FR_RR] = CB_FPE_PRM_RANGE_EDGE_ANGLE_FR_RR;
	nprm_rangeEdgeAngleLR[CB_FPE_EDGE_AC_TBL_SL_SR] = CB_FPE_PRM_RANGE_EDGE_ANGLE_SL_SR;

	/* �ϊ��e�[�u�� */
	pnTopAddrCnvLUT[CB_FPE_EDGE_AC_TBL_FR_RR] = pnTbl4FrRr;
	pnTopAddrCnvLUT[CB_FPE_EDGE_AC_TBL_SL_SR] = pnTbl4LR;
	
	for ( nj = 0L; nj < CB_FPE_EDGE_AC_TBL_MAXNUM; nj++ )
	{
		pnCnvLUT = pnTopAddrCnvLUT[nj];
		
		/* �����l */
		for ( ni = 0L; ni < 256L; ni++ )
		{
			pnCnvLUT[ni] = nprm_maxEdgeAC;
		}	/* for ( ni ) */

		/* �e�[�u���쐬 */
		nEdgeAngle = nprm_startEdgeAngle[nj];	/* �J�n�ʒu */
		nprm_rangeEdgeAngleUL[nj] = ( ( nMaxEdgeCodeAngle - 1L ) / 2L ) - nprm_rangeEdgeAngleLR[nj];
		nEdgeAC = CB_FPE_EDGE_AC_RIGHT;	/* �J�n�R�[�h */
		nCntIns = 0L;

		for ( ni = 0L; ni < ( nMaxEdgeCodeAngle - 1L ); ni++ )
		{
			pnCnvLUT[nEdgeAngle] = nEdgeAC;
			nCntIns++;

			if (   ( ( CB_FPE_EDGE_AC_RIGHT == nEdgeAC ) || ( CB_FPE_EDGE_AC_LEFT == nEdgeAC ) )
				&& ( nprm_rangeEdgeAngleLR[nj] < nCntIns ) )
			{
				nCntIns = 0L;
				nEdgeAC++;
			}
			else if (  ( ( CB_FPE_EDGE_AC_LOWER == nEdgeAC ) || ( CB_FPE_EDGE_AC_UPPER == nEdgeAC ) )
					&& ( nprm_rangeEdgeAngleUL[nj] < nCntIns ) )
			{
				nCntIns = 0L;
				nEdgeAC++;
			}
			else
			{
				/* �G���[ */
			}

			nEdgeAngle++;
			if ( ( nMaxEdgeCodeAngle - 1L ) < nEdgeAngle )
			{
				nEdgeAngle = 1L;
			}
		}	/* for ( ni ) */
		pnCnvLUT[nMaxEdgeCodeAngle] = CB_FPE_EDGE_AC_RIGHT;	/* �p�x360�x������0�x�Ɠ��������ɂ��� */
	}	/* for ( nj ) */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		�G�b�W�R�[�h���̒��o
 *
 * @param[in]	nImgSrc						:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	nImgEdgeRho					:,�G�b�W���x���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	nImgEdgeAngle				:,�G�b�W�p�x���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	nImgEdgeAC					:,�G�b�W�p�x�R�[�h���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	ptSrchRgn					:,�T���̈�ւ̃|�C���^,-,[-],
 * @param[in]	nCamDirection				:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK					:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)				:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.11.29	K.Kato			�V�K�쐬
 * @date		2013.11.29	S.Suzuki		�G�b�W�p�x��ʐ�����ǉ�
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgeCodeInfo( IMPLIB_IMGID nImgSrc, IMPLIB_IMGID nImgEdgeRho, IMPLIB_IMGID nImgEdgeAngle, IMPLIB_IMGID nImgEdgeAC, const CB_RECT_RGN* const ptSrchRgn, slong nCamDirection )
{
	slong	nRet;
	slong	ni;
	slong	nSxSrc0, 
			nSySrc0, 
			nExSrc0, 
			nEySrc0;
	slong	nSxDst, 
			nSyDst, 
			nExDst, 
			nEyDst;
	slong	nSxSrchRgn, 
			nSySrchRgn, 
			nExSrchRgn, 
			nEySrchRgn;
	slong	nNumEdgeCode;
	slong	nNumEdgeCodeTbl;
	slong	nXedge, 
			nYedge;
	uchar	nEdgeRho, 
			nEdgeAngle;
	slong	nXSizeEdgeRho, 
			nYSizeEdgeRho, 
			nXSizeEdgeAngle, 
			nYSizeEdgeAngle,
			nXSizeEdgeAC, 
			nYSizeEdgeAC;
	slong	nOffsetImg;
	uchar	*pnAddrEdgeRho, 
			*pnWkAddrEdgeRho, 
			*pnAddrEdgeAngle, 
			*pnWkAddrEdgeAngle,
			*pnAddrEdgeAC, 
			*pnWkAddrEdgeAC;
	IMPLIB_CNVLUT		*pnCnvLUT;
	IMPLIB_IPEdgePoint	*ptEdgeCodeTbl, 
						*ptWkEdgeCodeTbl;
	/* �p�����[�^ */
	slong	nprm_edgeShiftDown, 
			nprm_edgeThrMin, 
			nprm_edgeThrMax;
	slong	*pnprm_EdgeCoeff_H, 
			*pnprm_EdgeCoeff_V;

	/* �����`�F�b�N */
	if ( NULL == ptSrchRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^ */
	nprm_edgeShiftDown = CB_FPE_PRM_EDGE_CODE_SHIFT_DOWN;
	nprm_edgeThrMin = CB_FPE_PRM_EDGE_CODE_THR_MIN;
	nprm_edgeThrMax = CB_FPE_PRM_EDGE_CODE_THR_MAX;
	pnprm_EdgeCoeff_H = &( m_FPE_nprm_EdgeCodeCoeff_H[0] );
	pnprm_EdgeCoeff_V = &( m_FPE_nprm_EdgeCodeCoeff_V[0] );

	/* �E�B���h�E���̑ޔ� */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );

	/* �T���̈� */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;

	/*
	==================================================
		�G�b�W�R�[�h
	==================================================
	*/
	ptEdgeCodeTbl = &( m_FPE_tEdgeCodeTbl[0] );
	nNumEdgeCodeTbl = CB_FPE_EDGE_CODE_TBL_MAXNUM;

	/* �G�b�W�R�[�h�ݒ�̏������F�p�x�Z�o�p�ϊ��e�[�u���������� */
	nRet = implib_SetEdgeCodeTbl( &( CB_atanTbl[0] ), &( CB_thetaTbl[0] ) );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	/* �G�b�W�R�[�h�ݒ�̏������F���ݒ� */
	nRet = implib_SetEdgeCodeConfig( IMPLIB_EC_BIN, IMPLIB_RHO_H_V_APPR, IMPLIB_EC_VOLUME3 );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �G�b�W�R�[�h�Z�o */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );
	nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
	nNumEdgeCode = implib_IP_EdgeCode(	nImgSrc,
										ptEdgeCodeTbl,
										nprm_edgeShiftDown,
										pnprm_EdgeCoeff_H,
										pnprm_EdgeCoeff_V,
										nprm_edgeThrMin,
										nprm_edgeThrMax,
										nNumEdgeCodeTbl, 
										(enum IMPLIB_ECBufferMode)IMPLIB_ECB_NORMAL  );
	if ( 0L > nNumEdgeCode )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}


#if 0
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );
	nRet = implib_SetIPDataType( IMPLIB_SIGN8_DATA );
	nRet = implib_IP_EdgeFLT( nImgSrc, nImgEdgeAC, nprm_edgeShiftDown, pnprm_EdgeCoeff_V );

	{
		slong	nSxSys, nSySys, nExSys, nEySys;

		nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );
		
		nRet =implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, 359, 239 );
		nRet = implib_SaveBMPFile( nImgEdgeAC, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeRho_test.bmp", IMPLIB_BW_BITMAP );
		nRet = cb_FPE_DBG_SavePixelValue( nImgEdgeAC, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeRho_test.csv" );

		nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );
	}
#endif

#if 0
	{
		slong	nXSizeEdgeAngle, nYSizeEdgeAngle;
		uchar	*pnAddrEdgeAngle, *pnWkAddrEdgeAngle;

		nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );
		nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
		nRet = implib_IP_Const( nImgEdgeAC, 0L );

		nRet = implib_OpenImgDirect( nImgEdgeAC, &nXSizeEdgeAngle, &nYSizeEdgeAngle, (void**)&pnAddrEdgeAngle );

		ptWkEdgeCodeTbl = ptEdgeCodeTbl;
		for ( ni = 0L; ni < nNumEdgeCode; ni++ )
		{
			nXedge = nSxSrchRgn + ptWkEdgeCodeTbl->x;
			nYedge = nSySrchRgn + ptWkEdgeCodeTbl->y;
			nEdgeAngle = ptWkEdgeCodeTbl->theta;

			nOffsetImg = ( nYedge * nXSizeEdgeAngle ) + nXedge;
			pnWkAddrEdgeAngle = pnAddrEdgeAngle + nOffsetImg;
			*pnWkAddrEdgeAngle = (uchar)nEdgeAngle;

			ptWkEdgeCodeTbl++;
		}

		nRet = implib_CloseImgDirect( nImgEdgeAC );

		nRet = cb_FPE_DBG_SavePixelValue( nImgEdgeAC, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeAngle.csv" );
	}
#endif


	/*
	==================================================
		�G�b�W���x���p�x�R�[�h
	==================================================
	*/
	if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
	{
		pnCnvLUT = &( m_FPE_nCnvLUT_EdgeAC[CB_FPE_EDGE_AC_TBL_FR_RR][0] );
	}
	else if ( ( E_CB_SELFCALIB_CAMPOS_LEFT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_RIGHT == nCamDirection ) )
	{
		pnCnvLUT = &( m_FPE_nCnvLUT_EdgeAC[CB_FPE_EDGE_AC_TBL_SL_SR][0] );
	}
	else 
	{
		/* �ʏ킠�肦�Ȃ��p�X */
		return ( CB_IMG_NG );
	}

	/* �摜�N���A */
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );
	nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
	nRet = implib_IP_Const( nImgEdgeRho, 0L );
	nRet = implib_IP_Const( nImgEdgeAngle, 0L );
	nRet = implib_IP_Const( nImgEdgeAC, 0L );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgEdgeRho, &nXSizeEdgeRho, &nYSizeEdgeRho, (void**)&pnAddrEdgeRho );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( nImgEdgeAngle, &nXSizeEdgeAngle, &nYSizeEdgeAngle, (void**)&pnAddrEdgeAngle );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( nImgEdgeRho );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( nImgEdgeAC, &nXSizeEdgeAC, &nYSizeEdgeAC, (void**)&pnAddrEdgeAC );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( nImgEdgeAngle );
		nRet = implib_CloseImgDirect( nImgEdgeRho );
		return ( CB_IMG_NG_IMPLIB );
	}

	ptWkEdgeCodeTbl = ptEdgeCodeTbl;
	for ( ni = 0L; ni < nNumEdgeCode; ni++ )
	{
		/* ���W�l�����x���p�x */
		nXedge = nSxSrchRgn + (slong)ptWkEdgeCodeTbl->x;
		nYedge = nSySrchRgn + (slong)ptWkEdgeCodeTbl->y;
		nEdgeRho = ptWkEdgeCodeTbl->rho;
		nEdgeAngle = ptWkEdgeCodeTbl->theta;

		/* ���x�l�i�[ */
		nOffsetImg = ( nYedge * nXSizeEdgeRho ) + nXedge;
		pnWkAddrEdgeRho = pnAddrEdgeRho + nOffsetImg;
		*pnWkAddrEdgeRho = nEdgeRho;

		/* �G�b�W�p�x�l�i�[ */
		nOffsetImg = ( nYedge * nXSizeEdgeAngle ) + nXedge;
		pnWkAddrEdgeAngle = pnAddrEdgeAngle + nOffsetImg;
		*pnWkAddrEdgeAngle = nEdgeAngle;

		/* �G�b�W�p�x�R�[�h�i�[ */
		nOffsetImg = ( nYedge * nXSizeEdgeAC ) + nXedge;
		pnWkAddrEdgeAC = pnAddrEdgeAC + nOffsetImg;
		*pnWkAddrEdgeAC = (uchar)( pnCnvLUT[nEdgeAngle] );

		/* ���̃G�b�W�R�[�h */
		ptWkEdgeCodeTbl++;
	}	/* for ( ni ) */

	/* �摜�������A�N�Z�X�I�� */
	nRet = implib_CloseImgDirect( nImgEdgeAC );
	nRet = implib_CloseImgDirect( nImgEdgeAngle );
	nRet = implib_CloseImgDirect( nImgEdgeRho );

	/* ��ʃf�[�^�^�C�v�������I�ɐݒ� */
	nRet = implib_ChangeImgDataType( nImgEdgeRho, IMPLIB_UNSIGN8_DATA );
	nRet = implib_ChangeImgDataType( nImgEdgeAngle, IMPLIB_UNSIGN8_DATA );
	nRet = implib_ChangeImgDataType( nImgEdgeAC, IMPLIB_UNSIGN8_DATA );

	/* �֐������ŃV�X�e���f�[�^��ύX�����肵�Ă���̂ŁAUNSIGN8_DATA��RETURN���� */
	nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );

	/* �E�B���h�E���̕��A */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

#ifdef _WINDOWS
#if 0
	{
		slong	nSxSys, nSySys, nExSys, nEySys;

		nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );
		
		nRet =implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, 359, 239 );
		nRet = implib_SaveBMPFile( nImgEdgeRho, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeRho.bmp", IMPLIB_BW_BITMAP );
		nRet = cb_FPE_DBG_SavePixelValue( nImgEdgeRho, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeRho.csv" );
		nRet = implib_SaveBMPFile( nImgEdgeAC, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeAC.bmp", IMPLIB_BW_BITMAP );
		nRet = cb_FPE_DBG_SavePixelValue( nImgEdgeAC, (char_t*)"C:/work/work_dbg/SelfCalib_Dbg_ImgEdgeAC.csv" );

		nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );
	}
#endif
#endif /* _WINDOWS */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		��/�����s���_(���)�̒T��(���E�s���p)
 *
 * @param[in]	pnImgTbl		:,�\�[�X���ID�ւ̃|�C���^,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	ptCrossPnt		:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nNumCrossPnt	:,�摜���W(�\���ʒu)�\���̂̐�,-,[-],
 * @param[out]	ptDstPnt		:,�摜���W(�s���ʒu)�\���̂ւ̃|�C���^,-,[-],
 * @param[out]	pnNumDstPnt		:,�摜���W(�s���ʒu)�\���̂̐�,-,[-],
 * @param[in]	ptSrchRgn		:,��`�̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in]	nEdgePos		:,�T������,CB_FPE_UPPER_EDGE_POS_PATTERN<=value<=CB_FPE_LOWER_EDGE_POS_PATTERN,[-],
 * @param[in]	nAddPatternPos	:,�ǉ��p�^�[���̈ʒu,CB_ADD_PATTERN_POS_LEFT<=value<=CB_ADD_PATTERN_POS_RIGHT,[-],
 * @param[in]	nCamDirection	:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.11.28	K.Kato			�V�K�쐬
 * @date		2015.08.26	S.Morita		�T�C�h�}�[�J�[�摜�T�C�Y�C���Ή�/�T���̈�X�V�ʒu�C��
 */
/******************************************************************************/
static slong cb_FPE_SearchCandTPntUL4AddChk_byEdgeCode( const IMPLIB_IMGID* const pnImgTbl, const CB_IMG_POINT* const ptCrossPnt, slong nNumCrossPnt, 
														CB_IMG_POINT* ptDstPnt, slong* pnNumDstPnt, const CB_RECT_RGN* const ptSrchRgn, enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos, enum enum_CB_ADD_PATTERN_POS nAddPatternPos, slong nCamDirection,  const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo )
{
	slong	nRet;
	slong	ni, 
			nj, 
			nk;
	slong	nEdgeRho, 
			nEdgeAC,
			nEdgeTheta;
	
	slong	nSxSrchRgn, 
			nSySrchRgn, 
			nExSrchRgn, 
			nEySrchRgn;
	slong	nYSrchRgn;
	slong	nYSizeScanRgn;
	slong	nXSizeSrchRgn, 
			nYSizeSrchRgn;
	slong	nXattn, 
			nYattn;
	slong	nFlgSrchComp;
	slong	nNumDstPnt;
	slong	nCoeffDirX,
			nCoeffDirY;
	slong	nSrchAngleCode;
	slong	nXSizeEdgeRho, 
			nYSizeEdgeRho, 
			nXSizeEdgeAC, 
			nYSizeEdgeAC,
			nXSizeEdgeTheta, 
			nYSizeEdgeTheta;
	slong	nSignHaarLikeFeature = 0L;

	IMPLIB_IMGID			nImgEdgeRho, 
							nImgEdgeAC,
							nImgEdgeTheta;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo, 
							tPrevMaxPntInfo;
	uchar	*pnAddrEdgeRho, 
			*pnWkAddrEdgeRho, 
			*pnAddrEdgeAC, 
			*pnWkAddrEdgeAC,
			*pnAddrEdgeTheta, 
			*pnWkAddrEdgeTheta;
	/* �p�����[�^ */
	slong	nprm_xsizeHalfScanRgn;
	slong	nprm_distanceSrchRgn;
	slong	nprm_thrEdgeRho;
	
	slong	nmedianEdgeRho;
	slong	EdgeRhoArray[CB_FPE_MAX_ARREY_EGDE_MEDIAN];
	slong	nmedianEdgeTheta;
	slong	EdgeThetaArray[CB_FPE_MAX_ARREY_EGDE_MEDIAN];

	slong	nEndCheckEdgeTheta;
	slong	nEndCheckEdgeRho;

	slong	nMaxPntEdgeTheta;

	/* �����`�F�b�N */
	if ( ( NULL == pnImgTbl ) || ( NULL == ptCrossPnt ) || ( NULL == ptDstPnt ) || ( NULL == pnNumDstPnt ) || ( NULL == ptSrchRgn ) || ( NULL == ptPatternInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
	nprm_distanceSrchRgn = CB_FPE_PRM_DISTANCE_SRCH_RGN_ADD_CHK_T_PNT_UL;
	nprm_thrEdgeRho = CB_FPE_PRM_THR_EDGE_RHO_SRCH_CAND_T_PNT_UL_ADD_CHK;

	/* ���ID */
	nImgEdgeRho = pnImgTbl[ CB_FPE_IMG_EDGE_RHO_NORMAL ];
	nImgEdgeAC = pnImgTbl[ CB_FPE_IMG_EDGE_AC_NORMAL ];
	nImgEdgeTheta = pnImgTbl[ CB_FPE_IMG_EDGE_ANGLE_NORMAL ];

	/* �T���̈�ݒ� */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	/* �T�������̌W�����擾 */
	nRet = cb_FPE_GetSearchCoeffDir( nEdgePos, nAddPatternPos, &nCoeffDirX, &nCoeffDirY );
	if ( 0L > nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �ݒ肳�ꂽ�W������A�T��������ݒ� */
	if ( 0L < nCoeffDirY )
	{
		/* ���̐��̏ꍇ�́A��������*/
		nYSrchRgn = nEySrchRgn;
	}
	else
	{
		/* ���̐��̏ꍇ�́A�������*/
		nYSrchRgn = nSySrchRgn;
	}

	/*
	==================================================
		�����_�T��
	==================================================
	*/
	/* �d�ݕt��Haar-like�����ʕ����̎擾 */
	nRet = cb_FPE_GetSignHaarLikeFeature( ptPatternInfo, nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �T���Ώۂ̊p�x�R�[�h��ݒ�(�e�[�u�������Ȃ�) */
	nSrchAngleCode = -1L;
//	if ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection )
	if ( CB_IMG_CHECKBOARD_SIGN_PLUS == nSignHaarLikeFeature )
	{
		if ( CB_FPE_UPPER_EDGE_POS_PATTERN == nEdgePos )
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_LEFT;
		}
		else
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_RIGHT;
		}
	}
	else
	{
		if ( CB_FPE_UPPER_EDGE_POS_PATTERN == nEdgePos )
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_RIGHT;
		}
		else
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_LEFT;
		}
	}

	/* ���x���p�x�R�[�h���p�x���g���ĒT�����܂� */
	nRet = implib_OpenImgDirect( nImgEdgeRho, &nXSizeEdgeRho, &nYSizeEdgeRho, (void**)&pnAddrEdgeRho );
	if ( 0L > nRet ) 
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( nImgEdgeAC, &nXSizeEdgeAC, &nYSizeEdgeAC, (void**)&pnAddrEdgeAC );
	if ( 0L > nRet ) 
	{
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( nImgEdgeRho );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( nImgEdgeTheta, &nXSizeEdgeTheta, &nYSizeEdgeTheta, (void**)&pnAddrEdgeTheta );
	if ( 0L > nRet ) 
	{
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( nImgEdgeRho );
		nRet = implib_CloseImgDirect( nImgEdgeAC );
		return ( CB_IMG_NG_IMPLIB );
	}

	/* �T�� */
	/* ������ */
	nNumDstPnt = 0L;

	/* �G�b�W���x�E�G�b�W�p�x�R�[�h�E�G�b�W�p�x */
	/* ������ */
	nEdgeRho = 0L;
	nEdgeAC = 0L;
	nEdgeTheta = 0L;

	pnWkAddrEdgeTheta =NULL;

	/* �G�b�W���x�����l */
	/* ������ */
	memset( EdgeRhoArray, 0, sizeof( EdgeRhoArray ) );
	nmedianEdgeRho = 0L;

	/* �G�b�W�p�x�����l */
	/* ������ */
	memset( EdgeThetaArray, 0, sizeof( EdgeThetaArray ) );
	nmedianEdgeTheta = 0L;

	/* �T���I����������ɗp����l(�b��I�ɐݒ肵�Ă���B�e�X�g���o�ďC�����s���l) */
	nEndCheckEdgeTheta = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_THETA_LIMIT;
	nEndCheckEdgeRho = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_RHO_LIMIT;

	/* ���E�s���ł���΁AnNumCrossPnt��1�ŌŒ�̂͂��Ȃ̂ŁA�s�v��Loop? */
	for ( ni = 0L; ni < nNumCrossPnt; ni++ )
	{
		ptDstPnt[ni].nX = -1L;
		ptDstPnt[ni].nY = -1L;

		if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) )
		{
			/* �p�����[�^�ݒ� */
			nprm_xsizeHalfScanRgn = CB_FPE_PRM_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK;

			/* �T����_���v�Z */
			/* Y���W�́A�s���\���_�����/�������ɏ��蕝���������ꏊ */
			nXattn = ptCrossPnt[ni].nX;
			nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * nprm_distanceSrchRgn );

			/* �T���͈͂ƏƂ炵���킹�AY�����̍ő�T�������v�Z */
			nYSizeScanRgn = ( ( nYSrchRgn - nYattn ) * nCoeffDirY ) + 1L;

			tPrevMaxPntInfo.nFtrValue = -1L;	/* �����l */
			tPrevMaxPntInfo.tPnt.nX = -1L;	/* �����l */
			tPrevMaxPntInfo.tPnt.nY = -1L;	/* �����l */
			tMaxPntInfo.nFtrValue = 0L;		/* �����l */
			tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
			tMaxPntInfo.tPnt.nY = -1L;		/* �����l */
			nFlgSrchComp = CB_FALSE;

			/* �ȉ��AY�����ɒT����i�߂� */

			for ( nj = 0L; nj < nYSizeScanRgn; nj++ )
			{
				/* �T���̈���`�F�b�N */
				/* ���C���t�B���^��(nSizeNoUse)���AX�����ɔ��f */
				if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nXattn, nYattn, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn ) )
				{
					if ( 0L >= tPrevMaxPntInfo.nFtrValue )
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
					}
					break;
				}

				/* �T����_�̃A�h���X���v�Z */
				pnWkAddrEdgeRho = pnAddrEdgeRho + ( nYattn * nXSizeEdgeRho ) + nXattn;
				pnWkAddrEdgeAC = pnAddrEdgeAC + ( nYattn * nXSizeEdgeAC ) + nXattn;

				if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
				{
					pnWkAddrEdgeTheta = pnAddrEdgeTheta +  ( nYattn * nXSizeEdgeTheta ) + nXattn;
				}

				tMaxPntInfo.nFtrValue = 0L;		/* �����l */
				tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
				tMaxPntInfo.tPnt.nY = -1L;		/* �����l */
				nMaxPntEdgeTheta = 0L;			/* �����l */

				/* �G�b�W���x���傫���_���p�x�R�[�h������̃R�[�h�̓_��T�� */
				for ( nk = -nprm_xsizeHalfScanRgn; nk <= nprm_xsizeHalfScanRgn; nk++ )
				{
					/* �G�b�W���x�Ɗp�x�R�[�h�ƃG�b�W�p�x */
					nEdgeRho = (slong)( *( pnWkAddrEdgeRho + nk ) );
					nEdgeAC = (slong)( *( pnWkAddrEdgeAC + nk ) );

					if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
					{
						nEdgeTheta = (slong)( *( pnWkAddrEdgeTheta + nk ) );
					}

					if (   ( tMaxPntInfo.nFtrValue < nEdgeRho )
						&& ( nSrchAngleCode == nEdgeAC ) )
					{
						tMaxPntInfo.nFtrValue = nEdgeRho;
						tMaxPntInfo.tPnt.nX = nXattn + nk;
						tMaxPntInfo.tPnt.nY = nYattn;
				
						nMaxPntEdgeTheta = nEdgeTheta;

						if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
						{
							/* �G�b�W���x�ƃG�b�W�p�x�̒����l�擾�ׂ̈ɏ����i�[���� */
							if ( nj < CB_FPE_MAX_ARREY_EGDE_MEDIAN )
							{
								EdgeRhoArray[nj] = nEdgeRho;
								EdgeThetaArray[nj] = nEdgeTheta;
							}

						}
					}
				}	/* for ( nk ) */

				/* ���ړ_�X�V */
				if ( -1L < tPrevMaxPntInfo.nFtrValue )	/* ����T���ȊO */
				{
					if (   ( nprm_thrEdgeRho < tMaxPntInfo.nFtrValue )
						&& ( ( -1L < tMaxPntInfo.tPnt.nX ) && ( -1L < tMaxPntInfo.tPnt.nY ) ) )
					{
						if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
						{
							/* CB_FPE_MAX_ARREY_EGDE_MEDIAN��ڂ̒T���ȍ~�A�����I������ɃG�b�W���x�����l�ƃG�b�W�p�x�����l��p���� */
							if ( (CB_FPE_MAX_ARREY_EGDE_MEDIAN - 1L) == nj )
							{
								/* �G�b�W���x�̒����l���Z�o */
								nRet = cb_FPE_CalcMedian( &nmedianEdgeRho, EdgeRhoArray, CB_FPE_MAX_ARREY_EGDE_MEDIAN );
								if ( 0L > nRet )
								{
									/* �G���[���N���A */
									nRet = implib_ClearIPError();
									nRet = implib_CloseImgDirect( nImgEdgeRho );
									nRet = implib_CloseImgDirect( nImgEdgeAC );
									nRet = implib_CloseImgDirect( nImgEdgeTheta );
									return ( CB_IMG_NG );
								}

								/* �G�b�W�p�x�̒����l���Z�o */
								nRet = cb_FPE_CalcMedian( &nmedianEdgeTheta, EdgeThetaArray, CB_FPE_MAX_ARREY_EGDE_MEDIAN );
								if ( 0L > nRet )
								{
									/* �G���[���N���A */
									nRet = implib_ClearIPError();
									nRet = implib_CloseImgDirect( nImgEdgeRho );
									nRet = implib_CloseImgDirect( nImgEdgeAC );
									nRet = implib_CloseImgDirect( nImgEdgeTheta );
									return ( CB_IMG_NG );
								}
							}
						
							/* CB_FPE_MAX_ARREY_EGDE_MEDIAN��ڂ̒T���ȍ~�A�G�b�W���x�ƃG�b�W���x�����l�̍��ƃG�b�W�p�x�ƃG�b�W�p�x�����l�̍������ꂼ��덷�ȓ��Ɏ��܂�Ȃ��Ȃ�΁A�T����ł��؂� */
							if ( nj >= (CB_FPE_MAX_ARREY_EGDE_MEDIAN -1L) )
							{
								if ( ( labs( nMaxPntEdgeTheta - nmedianEdgeTheta ) > nEndCheckEdgeTheta ) || ( labs( tMaxPntInfo.nFtrValue - nmedianEdgeRho ) > nEndCheckEdgeRho ) )
								{
									nFlgSrchComp = CB_TRUE;	/* �T������ */
									break;
								}
							}
						}

						/* �T�����s(Y������1�V�t�g�������ʒu�𒍖ڗ̈�Ƃ���) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;
					}
					else
					{
						nFlgSrchComp = CB_TRUE;	/* �T������ */
						break;
					}
				}
				else	/* ����T�� */
				{
					if (   ( nprm_thrEdgeRho < tMaxPntInfo.nFtrValue )
						&& ( ( -1L < tMaxPntInfo.tPnt.nX ) && ( -1L < tMaxPntInfo.tPnt.nY ) ) )
					{
						/* �T�����s(Y������1�V�t�g�������ʒu�𒍖ڗ̈�Ƃ���) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;
					}
					else
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
						break;
					}
				}

				/* 2��ڈȍ~�̒T���ł́A�T���̈�����߂� */
				/* �p�����[�^�ݒ� */
				nprm_xsizeHalfScanRgn = CB_FPE_PRM_REFINE_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK;
			}	/* for ( nj ) */

			/* �M���x�`�F�b�N���邩�Ȃ� */
			if ( nprm_thrEdgeRho < tPrevMaxPntInfo.nFtrValue )
			{
				ptDstPnt[ni].nX = tPrevMaxPntInfo.tPnt.nX;
				ptDstPnt[ni].nY = tPrevMaxPntInfo.tPnt.nY;
			}

		}	/* if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) ) */

		/* �_�����C���N�������g */
		nNumDstPnt++;

	}	/* for ( ni ) */

	nRet = implib_CloseImgDirect( nImgEdgeRho );
	if ( 0L > nRet ) 
	{
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( nImgEdgeAC );
		nRet = implib_CloseImgDirect( nImgEdgeTheta );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_CloseImgDirect( nImgEdgeAC );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( nImgEdgeTheta );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_CloseImgDirect( nImgEdgeTheta );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	*pnNumDstPnt= nNumDstPnt;

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief		�T�������̌W���ݒ�
 *
 * ��������param[input/output]�͗v�C��������
 *
 * @param[in]	nEdgePos		:,�T������,CB_FPE_UPPER_EDGE_POS_PATTERN<=value<=CB_FPE_LOWER_EDGE_POS_PATTERN,[-],
 * @param[in]	nAddPatternPos	:,�ǉ��p�^�[���̈ʒu��`,CB_ADD_PATTERN_POS_LEFT<=value<=CB_ADD_PATTERN_POS_RIGHT,[-],
 * @param[out]	nCoeffDirX		:,X�������̌W��,-1<=value<=+1,[-],
 * @param[out]	nCoeffDirY		:,Y�������̌W��,-1<=value<=+1,[-],
 *
 * @retval		CB_IMG_OK		:,����I��,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.11.21	F.Sano			�V�K�쐬
 */
/******************************************************************************/
static slong cb_FPE_GetSearchCoeffDir( enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos, enum enum_CB_ADD_PATTERN_POS nAddPatternPos, slong* nCoeffDirX, slong* nCoeffDirY)
{
	/* �����`�F�b�N */
	if ( ( NULL == nCoeffDirX ) || ( NULL == nCoeffDirY ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ������*/
	*nCoeffDirX = 0L;
	*nCoeffDirY = 0L;
	
	/* �T�������̌W�� */
	if ( CB_FPE_LOWER_EDGE_POS_PATTERN == nEdgePos )
	{
		*nCoeffDirY = (+1L);

		/* ���E�ǂ���̃}�[�J�[���ɂ���āA�T���ɂ��X�̕ω��ʂ̐������ς�� */
		if( CB_ADD_PATTERN_POS_LEFT == nAddPatternPos )
		{
			*nCoeffDirX = (-1L);
		}
		else if( CB_ADD_PATTERN_POS_RIGHT == nAddPatternPos )
		{
			*nCoeffDirX = (+1L);
		}
		else
		{
			return ( CB_IMG_NG );
		}
	}
	else if ( CB_FPE_UPPER_EDGE_POS_PATTERN == nEdgePos )
	{
		*nCoeffDirY = (-1L);

		/* ���E�ǂ���̃}�[�J�[���ɂ���āA�T���ɂ��X�̕ω��ʂ̐������ς�� */
		if( CB_ADD_PATTERN_POS_LEFT == nAddPatternPos )
		{
			*nCoeffDirX = (+1L);
		}
		else if( CB_ADD_PATTERN_POS_RIGHT == nAddPatternPos )
		{
			*nCoeffDirX = (-1L);
		}
		else
		{
			return ( CB_IMG_NG );
		}
	}
	else
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

#ifdef CB_FPE_CSW_DEBUG_ON
/******************************************************************************/
/**
 * @brief		[�f�o�b�O]�ǉ��p�^�[��(�s��)�ɂ�����T���̈�`��
 *
 * @param[in,out]	nImgDst			:,�o�͉��ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in]		nZoomMode		:,�g��/�k�����[�h,0<=value<=1,[-],
 *
 * @retval			CB_IMG_OK		:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date		2013.03.01	K.Kato			�V�K�쐬
 */
/******************************************************************************/
slong cb_FPE_DBG_DrawSearchRegionInfo4AddChk( const IMPLIB_IMGID nImgDst, slong nCamPos, slong nZoomMode )
{
	slong	nRet = CB_IMG_NG;
	slong	ni;
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxSrc1, nSySrc1, nExSrc1, nEySrc1;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	nSxSys, nSySys, nExSys, nEySys;
	slong	nSxRect, nSyRect, nExRect, nEyRect;
	IMPLIB_IMGTBL	tImgTbl;
	CB_RECT_RGN	*ptRgn;


	/* �E�B���h�E���ޔ�(�S�E�B���h�E) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* �摜���擾 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* �`��ݒ� */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0L, 0L, ( tImgTbl.xlng - 1L ), ( tImgTbl.ylng - 1L ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_BLACK/*WHITE*/ );

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{	
		if ( 0L == nZoomMode )
		{
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tRgnImgHLsrc);
		}
		else if ( 1L == nZoomMode )
		{
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tRgnImgZoomOutHLsrc);
		}
		else
		{
			/* default */
			ptRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tRgnImgHLsrc);
		}

		nSxRect = ptRgn->nSX;
		nSyRect = ptRgn->nSY;
		nExRect = ptRgn->nEX;
		nEyRect = ptRgn->nEY;

		if (   ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nSxRect, nSyRect, 0L, 0L, ( tImgTbl.xlng - 1L ), ( tImgTbl.ylng - 1L ) ) )
			&& ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nExRect, nEyRect, 0L, 0L, ( tImgTbl.xlng - 1L ), ( tImgTbl.ylng - 1L ) ) ) )
		{
			nRet = implib_DrawRectangle( nSxRect, nSyRect, ( nExRect - nSxRect + 1L ), ( nEyRect - nSyRect ) );
			if ( 0L > nRet )
			{
				nRet = implib_ClearIPError();
			}
		}

		/*--------------------------------*/
		/* �\���_�T���̈��`�`��         */
		/*--------------------------------*/

		nSxRect = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tPntImgSideChkCenter.tAddChkSrchRgn.nSX;
		nSyRect = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tPntImgSideChkCenter.tAddChkSrchRgn.nSY;
		nExRect = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tPntImgSideChkCenter.tAddChkSrchRgn.nEX;
		nEyRect = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni].tPntImgSideChkCenter.tAddChkSrchRgn.nEY;

		nRet = implib_DrawRectangle( nSxRect, nSyRect, ( nExRect - nSxRect + 1L ), ( nEyRect - nSyRect ) );
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
		}
	}


	/* �E�B���h�E��񕜋A(�S�E�B���h�E) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* �f�o�b�O����implib���C�u�����G���[�m�F���� */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* �G���[������ :implib_ReadIPErrorTable�̓G���[�N���A�������܂܂�Ă��邽�߁A�G���[�N���A�����͏ȗ����� */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CB_FPE_CSW_DEBUG_ON */


/******************************************************************************/
/**
 * @brief			�����_�ʒu�̐���(�ǉ��s���̉��p)
 *
 * @param[in]		nImgSrc					:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		pnImgWk					:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[in]		ptSrchRgnInfo			:,�����̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in,out]	ptSrchRsltAddPntInfo	:,�T�����ʓ_���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nCamDirection			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.27	S.Suzuki		�V�K�쐬
 * @date			2015.09.14	S.Morita		��_�v�Z���s���ɁA�����l���i�[����悤�C��
 *
 * @note			����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_EstimateFtrPoint4AddChk( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, const CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* const ptSrchRgnInfo,
												CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltAddPntInfo, slong nCamDirection )
{
	slong						nRet = CB_IMG_NG;
	slong						ni = 0L, 
								nj = 0L;
	slong						nDirX = 0L, 
								nDirY = 0L;
	slong						nNumEdgePntUL = 0L, 
								nNumEdgePntLR = 0L, 
								nNumEdgePrecPntUL = 0L, 
								nNumEdgePrecPntLR = 0L;
	slong						nNumPntArray[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { 0L };
	enum enum_CB_FPE_SCAN_DIR_X	nScanDirX = CB_FPE_SCAN_DIR_X_PLUS;
	enum enum_CB_FPE_SCAN_DIR_Y	nScanDirY = CB_FPE_SCAN_DIR_Y_PLUS;
	CB_FPE_IMG_ACCESS_INFO		tImgAccessInfo = { 0L, 0L, NULL, NULL, NULL, NULL };
	CB_IMG_POINT				*ptPntArray[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { NULL, NULL, NULL, NULL };
	t_cb_img_CenterPos			*ptRsltPntArray[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { NULL, NULL, NULL, NULL };
	CB_IMG_POINT				*ptEdgePntUL = NULL, 
								*ptEdgePntLR = NULL;
	t_cb_img_CenterPos			*ptEdgePrecPntUL = NULL, 
								*ptEdgePrecPntLR = NULL;

	CB_FPE_IMG_ACCESS_INFO		tEdgeCodeRho = { 0L, 0L, NULL, NULL, NULL, NULL };			/* �����x�ʒu���ߗp�@�G�b�W���x�摜(work�摜1�𗘗p) */
	CB_FPE_IMG_ACCESS_INFO		tEdgeCodeTheta = { 0L, 0L, NULL, NULL, NULL, NULL };		/* �����x�ʒu���ߗp�@�G�b�W�p�x�摜(work�摜2�𗘗p) */
	CB_FPE_IMG_ACCESS_INFO		tEdgeCodeAC = { 0L, 0L, NULL, NULL, NULL, NULL };		/* �����x�ʒu���ߗp�@�G�b�W�p�x�摜(work�摜2�𗘗p) */


	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk )  || ( NULL == ptSrchRgnInfo ) || ( NULL == ptSrchRsltAddPntInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	ptEdgePntUL			= &(m_FPE_tWorkInfo.tEdgePntUL[0L]);
	ptEdgePntLR			= &(m_FPE_tWorkInfo.tEdgePntLR[0L]);
	ptEdgePrecPntUL		= &(m_FPE_tWorkInfo.tEdgePrecPntUL[0L]);
	ptEdgePrecPntLR		= &(m_FPE_tWorkInfo.tEdgePrecPntLR[0L]);


	/* �e�����_�̏���o�^ */
	ptPntArray[CB_ADD_PATTERN_PNT_POS_UPPER]		= &( ptSrchRsltAddPntInfo->tTPntUpper[0L] );
	ptPntArray[CB_ADD_PATTERN_PNT_POS_LOWER]		= &( ptSrchRsltAddPntInfo->tTPntLower[0L] );
	ptRsltPntArray[CB_ADD_PATTERN_PNT_POS_UPPER]	= &( ptSrchRsltAddPntInfo->tTPntUpperPrecise[0L] );
	ptRsltPntArray[CB_ADD_PATTERN_PNT_POS_LOWER]	= &( ptSrchRsltAddPntInfo->tTPntLowerPrecise[0L] );
	nNumPntArray[CB_ADD_PATTERN_PNT_POS_UPPER]		= ptSrchRsltAddPntInfo->nNumTPntUpper;
	nNumPntArray[CB_ADD_PATTERN_PNT_POS_LOWER]		= ptSrchRsltAddPntInfo->nNumTPntLower;


	/* �G�b�W�R�[�h���̒��o */
	nRet = cb_FPE_ExtractEdgeCodeInfo( nImgSrc, 
										pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], 
										pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], 
										pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL], 
										&( ptSrchRgnInfo->tRgnImg ),
										nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}


	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgAccessInfo.nXSize ), &( tImgAccessInfo.nYSize ), (void**)&( tImgAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	/* �G�b�W���x�E�G�b�W�X���摜�̃��������蓖��(�摜�������A�N�Z�X�J�n) */
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], &( tEdgeCodeRho.nXSize ), &( tEdgeCodeRho.nYSize ), (void**)&( tEdgeCodeRho.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], &( tEdgeCodeTheta.nXSize ), &( tEdgeCodeTheta.nYSize ), (void**)&( tEdgeCodeTheta.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL], &( tEdgeCodeAC.nXSize ), &( tEdgeCodeAC.nYSize ), (void**)&( tEdgeCodeAC.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}


	for ( ni = 0L; ni < CB_FPE_EDGE_POS_PATTERN_MAXNUM; ni++ )
	{
		/* �̈�̐ݒ���� */
		nDirX = m_FPE_nDirX4PreciseSrchRgnAddChk[nCamDirection][ni];
		nDirY = m_FPE_nDirY4PreciseSrchRgnAddChk[nCamDirection][ni];

		if( ptSrchRgnInfo->tPtnInfo.nFlagPlacement == CB_IMG_CHKBOARD_NEG_PLACEMENT )
		{
			nDirX = nDirX * ( -1L );
		}
		else
		{
			/* No Operation */
		}

		for ( nj = 0L; nj < nNumPntArray[ni]; nj++ )
		{
			if ( ( 0L <= (ptPntArray[ni])[nj].nX ) && ( 0L <= (ptPntArray[ni])[nj].nY ) )
			{

				/* �X�L���������̐ݒ� */
				if ( 0L < nDirX )
				{
					nScanDirX = CB_FPE_SCAN_DIR_X_MINUS;
				}
				else if ( 0L > nDirX )
				{
					nScanDirX = CB_FPE_SCAN_DIR_X_PLUS;
				}
				else
				{
					/* �G���[ */
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}
				if ( 0L < nDirY )
				{
					nScanDirY = CB_FPE_SCAN_DIR_Y_MINUS;
				}
				else if ( 0L > nDirY )
				{
					nScanDirY = CB_FPE_SCAN_DIR_Y_PLUS;
				}
				else
				{
					/* �G���[ */
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* �G�b�W�_�Q���o(�����G�b�W) */
//				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, ptPntArray[ni][nj], ptEdgePntUL, &nNumEdgePntUL, CB_FPE_LINE_FLT_VERT_EDGE, nDirX, nDirY );
				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, &tEdgeCodeAC, ptPntArray[ni][nj], ptEdgePntUL, &nNumEdgePntUL, CB_FPE_LINE_FLT_VERT_EDGE, nDirX, nDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* �����x�ʒu����(�����G�b�W) */
				nRet = cb_FPE_ExtractEdgePrecisePoint4AddChk( &tImgAccessInfo, ptEdgePntUL, nNumEdgePntUL, ptEdgePrecPntUL, &nNumEdgePrecPntUL, CB_FPE_LINE_FLT_VERT_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}


				/* �G�b�W�_�Q���o(�����G�b�W) */
//				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, ptPntArray[ni][nj], ptEdgePntLR, &nNumEdgePntLR, CB_FPE_LINE_FLT_HORI_EDGE, nDirX, nDirY );
				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, &tEdgeCodeAC, ptPntArray[ni][nj], ptEdgePntLR, &nNumEdgePntLR, CB_FPE_LINE_FLT_HORI_EDGE, nDirX, nDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* �����x�ʒu����(�����G�b�W) */
				nRet = cb_FPE_ExtractEdgePrecisePoint4AddChk( &tImgAccessInfo, ptEdgePntLR, nNumEdgePntLR, ptEdgePrecPntLR, &nNumEdgePrecPntLR, CB_FPE_LINE_FLT_HORI_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}


				/* ����/�����̒������ߎ����A��_���v�Z */
				nRet = cb_FPE_CalcIntersectionPoint( ptEdgePrecPntUL, ptEdgePrecPntLR, nNumEdgePrecPntUL, nNumEdgePrecPntLR, &( ptRsltPntArray[ni][nj] ) );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}
				
				/* ��_�v�Z�����s���Ă���Ƃ��́A���ʂɖ����l���i�[���� */
				if ( ptRsltPntArray[ni][nj].nFlagPrecise != CB_TRUE )
				{
					/* �s�N�Z�����x�̍��W�l�ɁA�����l��ݒ肷�� */
					( (ptPntArray[ni])[nj].nX ) = -1L;
					( (ptPntArray[ni])[nj].nY ) = -1L;
			
					/* �T�u�s�N�Z�����x�̍��W�l�ɁA�����l��ݒ肷�� */
					(ptRsltPntArray[ni])[nj].w = (double_t)( (ptPntArray[ni])[nj].nX );	//-1.0;
					(ptRsltPntArray[ni])[nj].h = (double_t)( (ptPntArray[ni])[nj].nY );	//-1.0;
					(ptRsltPntArray[ni])[nj].nFlagPrecise = CB_FALSE;
				}
				else
				{
					/* Not Operation */
				}

			}
			else
			{
				/* �s�N�Z�����x�̍��W�l�ɁA�����l��ݒ肷�� */
				( (ptPntArray[ni])[nj].nX ) = -1L;
				( (ptPntArray[ni])[nj].nY ) = -1L;
		
				/* �T�u�s�N�Z�����x�̍��W�l�ɁA�����l��ݒ肷�� */
				(ptRsltPntArray[ni])[nj].w = (double_t)( (ptPntArray[ni])[nj].nX );	//-1.0;
				(ptRsltPntArray[ni])[nj].h = (double_t)( (ptPntArray[ni])[nj].nY );	//-1.0;
				(ptRsltPntArray[ni])[nj].nFlagPrecise = CB_FALSE;
			}	/* if ( ( 0L <= (ptPntArray[ni])[nj].nX ) && ( 0L <= (ptPntArray[ni])[nj].nY ) ) */

			nDirX = nDirX * (-1L);
		}	/* for ( nj = 0; nj < nNumPntArray[ni]; nj++ ) */

	}	/* for ( ni = 0; ni < 2; ni++ ) */

	/* �摜�������A�N�Z�X�܂Ƃ߂ďI�� */
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			�G�b�W�_���o
 *
 * @param[in]		ptImgRho				:,�����x�ʒu���ߗp�@�G�b�W���x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		ptImgTheta				:,�����x�ʒu���ߗp�@�G�b�W�p�x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		tTPnt					:,�����̈�\���̂ւ̃|�C���^,-,[-],
 * @param[out]		ptEdgePnt				:,�����x�ʒu���ߗp�@�G�b�W�_�z��,-,[-],
 * @param[out]		pnNumEdgePnt			:,�����x�ʒu���ߗp�@�G�b�W�_��,-,[-]
 * @param[in]		nFltMode				:,Line�t�B���^�̃��[�h,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE
 * @param[in]		nDirX					:,�X�L�����������(X��),CB_FPE_SCAN_DIR_X_MINUS<=value<=CB_FPE_SCAN_DIR_X_PLUS,[-],
 * @param[in]		nDirY					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.27	S.Suzuki	�V�K�쐬
 * @date			2015.09.07	S.Morita	�c�G�b�W�i�s�N�Z�����x�j�T�����̕ύX
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
										    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
										    enum enum_CB_FPE_LINE_FLT_MODE nFltMode, slong nDirX, slong nDirY)
{
	slong	nRet = CB_IMG_NG;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	slong	nThrThetaMin;
	slong	nThrThetaMax;
	slong	nThrRho;

	slong	nNumEdgePnt;

	slong	nSrchAngleCode;
	
	nNumEdgePnt = 0L;
	tMaxPntInfo.nFtrValue = 0L;		/* �����l */
	tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
	tMaxPntInfo.tPnt.nY = -1L;		/* �����l */


	/* �����`�F�b�N */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �̈�`�F�b�N */
	if ( ( 0L > tTPnt.nX ) || ( 0L > tTPnt.nY ) )
	{
		*pnNumEdgePnt = 0L;
		return ( CB_IMG_OK );
	}

	if ( CB_FPE_LINE_FLT_HORI_EDGE == nFltMode )
	{
		nSrchAngleCode = -1L;
		/* �T������p�x�R�[�h��ݒ� */
		if( nDirY > 0L )
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_UPPER;
		}
		else
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_LOWER;
		}

		/*
		===================================
			�@T���_����V�t�g�����_��T��
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( ptImgRho, ptImgTheta, ptImgAC, tTPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirX, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* ��ƂȂ�_��������Ȃ��ꍇ�A���ʂȂ��Ƃ���return */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			�A�T�������_���獶�ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Hori( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nDirY, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			�B�T�������_����E�ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Hori( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nDirY, nThrThetaMin, nThrThetaMax, nThrRho, +1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
	}
	else if ( CB_FPE_LINE_FLT_VERT_EDGE == nFltMode )
	{
		nSrchAngleCode = -1L;
		/* �T������p�x�R�[�h��ݒ� */
		if( nDirX > 0L )
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_LEFT;
		}
		else
		{
			nSrchAngleCode = CB_FPE_EDGE_AC_RIGHT;
		}

		/*
		===================================
			�@T���_����V�t�g�����_��T��
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( ptImgRho, ptImgTheta, ptImgAC, tTPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* ��ƂȂ�_��������Ȃ��ꍇ�A���ʂȂ��Ƃ���return */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			�A�T�������_�����ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Vert( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			�B�T�������_���牺�ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Vert( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nThrThetaMin, nThrThetaMax, nThrRho, +1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
	}
	else
	{
		/* �R�R�ɗ���͖̂{���L�蓾�Ȃ��p�^�[��*/
		return ( CB_IMG_NG );
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			�G�b�W�_���o�iT���_���W������ʐ��������ɃV�t�g�������ړ_�̍��W�l���擾
 *
 * @param[in]		ptImgRho				:,�����x�ʒu���ߗp�@�G�b�W���x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		ptImgTheta				:,�����x�ʒu���ߗp�@�G�b�W�p�x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		tTPnt					:,�����̈�\���̂ւ̃|�C���^,-,[-],
 * @param[out]		ptEdgePnt				:,�����x�ʒu���ߗp�@�G�b�W�_�z��,-,[-],
 * @param[out]		pnNumEdgePnt			:,�����x�ʒu���ߗp�@�G�b�W�_��,-,[-]
 * @param[out]		nThrThetaMin			:,�G�b�W�p�x�̍ŏ�臒l,-,[-]
 * @param[out]		nThrThetaMax			:,�G�b�W�p�x�̍ő�臒l,-,[-]
 * @param[out]		pnThrRho				:,�G�b�W���x��臒l,-,[-]
 * @param[in]		nDirX					:,�X�L�����������(X��),CB_FPE_SCAN_DIR_X_MINUS<=value<=CB_FPE_SCAN_DIR_X_PLUS,[-],
 * @param[in]		nDirY					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.01.06	F.Sano	�V�K�쐬(�֐�cb_FPE_ExtractEdgePnt4AddChk���ו���) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
														    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
										   					slong* pnThrThetaMin, slong* pnThrThetaMax, slong* pnThrRho,
															slong nDirX, slong nDirY, slong nSrchAngleCode, slong nprm_HalfScanRgn)
{
	slong	ni;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* �p�����[�^ */
/*	slong	nprm_HalfScanRgn;*/
	slong	nprm_distanceSrchRgn;
	slong	nprm_thrEdgeFtrTPnt;
	slong	nprm_thrRangeTheta;
	float_t	nprm_coeffRho;

	uchar*	pnAddrRho;
	uchar*	pnAddrTheta;
	uchar*	pnAddrAC;
	slong	nTargetRho;
	slong	nTargetTheta;
	slong	nTargetAC;
	slong	nTargetX;
	slong	nTargetY;

	slong	nNumEdgePnt;
	
	nNumEdgePnt = 0L;
	tMaxPntInfo.nFtrValue = 0L;		/* �����l */
	tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
	tMaxPntInfo.tPnt.nY = -1L;		/* �����l */


	/* �����`�F�b�N */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt )
	  || ( NULL == pnThrThetaMin ) || ( NULL == pnThrThetaMax ) || ( NULL == pnThrRho )  )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_distanceSrchRgn	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_SRCH_BASE_PNT;
	nprm_thrEdgeFtrTPnt		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_EDGE_FTR_T_PNT;
	nprm_thrRangeTheta		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_RANGE_THETA;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	nTargetTheta = -999L;

	/*
	===================================
		�@T���_����V�t�g�����_��T��
	===================================
	*/
	/* T���_���W������ʃV�t�g�������ړ_�̍��W�l */
	nTargetX = tTPnt.nX + ( nDirX * nprm_distanceSrchRgn );
	nTargetY = tTPnt.nY;

	/* ���ړ_�̃A�h���X */
	/* ptImgRho��ptImgTheta���AnXSize���̉�ʃT�C�Y���͓��������A�Ƃ肠�����g�������Ă��� */
	pnAddrRho = (uchar*)( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX;
	pnAddrTheta = (uchar*)( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX;
	pnAddrAC = (uchar*)( ptImgAC->pnAddrY8 ) + ( nTargetY * ptImgAC->nXSize ) + nTargetX;

	/* �G�b�W���x��Βl���ő�̓_��T�� */
	for ( ni = -nprm_HalfScanRgn; ni <= nprm_HalfScanRgn; ni++ )
	{
		/* �T��������A�h���X���v�Z */
		nTargetRho = (slong)*( pnAddrRho - ( ( nDirY * ni ) * ptImgRho->nXSize ) );
		nTargetAC = (slong)*( pnAddrAC - ( ( nDirY * ni ) * ptImgAC->nXSize ) );

		/* �G�b�W���x���ő�ƂȂ��f�́A���W�E�G�b�W�p�x��ۑ����A�G�b�W�_�J�E���g���C���N�������g */
		/* ���̃G�b�W�p�x���A�ȍ~�̓_�Q���莞��臒l�ƂȂ� */
		if (	( tMaxPntInfo.nFtrValue < nTargetRho )
			&&	( nTargetAC == nSrchAngleCode ) )
		{
			tMaxPntInfo.nFtrValue = nTargetRho;
			nTargetTheta = (slong)*(uchar*)( pnAddrTheta - ( ( nDirY * ni )  * ptImgTheta->nXSize ) );
			tMaxPntInfo.tPnt.nX = nTargetX;
			tMaxPntInfo.tPnt.nY = nTargetY - ( nDirY * ni );
		}
	}

	/* �M���x�`�F�b�N */
	if ( tMaxPntInfo.nFtrValue < nprm_thrEdgeFtrTPnt )
	{
		/* ��ƂȂ�_��������Ȃ��ꍇ�A���ʂȂ��Ƃ���return */
		ptEdgePnt[nNumEdgePnt].nX = -1L;
		ptEdgePnt[nNumEdgePnt].nY = -1L;
	}
	else
	{
		/* ��ƂȂ�_�����������ꍇ�A�ȍ~�̃G�b�W�_�T���𑱍s */
		*pnThrThetaMin = nTargetTheta - nprm_thrRangeTheta;
		*pnThrThetaMax = nTargetTheta + nprm_thrRangeTheta;
		*pnThrRho		 = (slong)( (float_t)tMaxPntInfo.nFtrValue * nprm_coeffRho );
		ptEdgePnt[nNumEdgePnt].nX = tMaxPntInfo.tPnt.nX;
		ptEdgePnt[nNumEdgePnt].nY = tMaxPntInfo.tPnt.nY;
		nNumEdgePnt ++;
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );

}

/******************************************************************************/
/**
 * @brief			�G�b�W�_���o�iT���_���W������ʐ��������ɃV�t�g�������ړ_�̍��W�l���擾�j
 *
 * @param[in]		ptImgRho				:,�����x�ʒu���ߗp�@�G�b�W���x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		ptImgTheta				:,�����x�ʒu���ߗp�@�G�b�W�p�x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		tTPnt					:,�����̈�\���̂ւ̃|�C���^,-,[-],
 * @param[out]		ptEdgePnt				:,�����x�ʒu���ߗp�@�G�b�W�_�z��,-,[-],
 * @param[out]		pnNumEdgePnt			:,�����x�ʒu���ߗp�@�G�b�W�_��,-,[-]
 * @param[out]		nThrThetaMin			:,�G�b�W�p�x�̍ŏ�臒l,-,[-]
 * @param[out]		nThrThetaMax			:,�G�b�W�p�x�̍ő�臒l,-,[-]
 * @param[out]		pnThrRho				:,�G�b�W���x��臒l,-,[-]
 * @param[in]		nDirY					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.01.06	F.Sano	�V�K�쐬(�֐�cb_FPE_ExtractEdgePnt4AddChk���ו���) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,  const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
														    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
															slong* pnThrThetaMin, slong* pnThrThetaMax, slong* pnThrRho, 
														slong nDirY, slong nSrchAngleCode, slong nprm_HalfScanRgn)
{
	slong	ni;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* �p�����[�^ */
/*	slong	nprm_HalfScanRgn;*/
	slong	nprm_distanceSrchRgn;
	slong	nprm_thrEdgeFtrTPnt;
	slong	nprm_thrRangeTheta;
	float_t	nprm_coeffRho;

	uchar*	pnAddrRho;
	uchar*	pnAddrTheta;
	uchar*	pnAddrAC;
	slong	nTargetRho;
	slong	nTargetTheta;
	slong	nTargetAC;
	slong	nTargetX;
	slong	nTargetY;

	slong	nNumEdgePnt;
	
	nNumEdgePnt = 0L;
	tMaxPntInfo.nFtrValue = 0L;		/* �����l */
	tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
	tMaxPntInfo.tPnt.nY = -1L;		/* �����l */


	/* �����`�F�b�N */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt )
	  || ( NULL == pnThrThetaMin ) || ( NULL == pnThrThetaMax ) || ( NULL == pnThrRho )  )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �p�����[�^�擾 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_distanceSrchRgn	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_SRCH_BASE_PNT;
	nprm_thrEdgeFtrTPnt		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_EDGE_FTR_T_PNT;
	nprm_thrRangeTheta		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_RANGE_THETA;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	nTargetTheta = -999L;

	/*
	===================================
		�@T���_����V�t�g�����_��T��
	===================================
	*/
	/* T���_���W������ʃV�t�g�������ړ_�̍��W�l */
	nTargetX = tTPnt.nX;
	nTargetY = tTPnt.nY + ( nDirY * nprm_distanceSrchRgn );

	/* ���ړ_�̃A�h���X */
	/* ptImgRho��ptImgTheta���AnXSize���̉�ʃT�C�Y���͓��������A�Ƃ肠�����g�������Ă��� */
	pnAddrRho = (uchar*)( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX;
	pnAddrTheta = (uchar*)( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX;
	pnAddrAC = (uchar*)( ptImgAC->pnAddrY8 ) + ( nTargetY * ptImgAC->nXSize ) + nTargetX;

	/* �G�b�W���x��Βl���ő�̓_��T�� */
	for ( ni = -nprm_HalfScanRgn; ni <= nprm_HalfScanRgn; ni++ )
	{
		/* �T��������A�h���X���v�Z */
		nTargetRho = (slong)*( pnAddrRho + ni );
		nTargetAC = (slong)*( pnAddrAC + ni );

		/* �G�b�W���x���ő�ƂȂ��f�́A���W�E�G�b�W�p�x��ۑ����A�G�b�W�_�J�E���g���C���N�������g */
		/* ���̃G�b�W�p�x���A�ȍ~�̓_�Q���莞��臒l�ƂȂ� */
		if (	( tMaxPntInfo.nFtrValue < nTargetRho )
			&&	( nTargetAC == nSrchAngleCode ) )
		{
			tMaxPntInfo.nFtrValue = nTargetRho;
			nTargetTheta = (slong)*(uchar*)( pnAddrTheta + ni );
			tMaxPntInfo.tPnt.nX = nTargetX + ni;
			tMaxPntInfo.tPnt.nY = nTargetY;
		}
	}

	/* �M���x�`�F�b�N */
	if ( tMaxPntInfo.nFtrValue < nprm_thrEdgeFtrTPnt )
	{
		/* ��ƂȂ�_��������Ȃ��ꍇ�A���ʂȂ��Ƃ���return */
		ptEdgePnt[nNumEdgePnt].nX = -1L;
		ptEdgePnt[nNumEdgePnt].nY = -1L;
	}
	else
	{
		/* ��ƂȂ�_�����������ꍇ�A�ȍ~�̃G�b�W�_�T���𑱍s */
		*pnThrThetaMin = nTargetTheta - nprm_thrRangeTheta;
		*pnThrThetaMax = nTargetTheta + nprm_thrRangeTheta;
		*pnThrRho		 = (slong)( (float_t)tMaxPntInfo.nFtrValue * nprm_coeffRho );
		ptEdgePnt[nNumEdgePnt].nX = tMaxPntInfo.tPnt.nX;
		ptEdgePnt[nNumEdgePnt].nY = tMaxPntInfo.tPnt.nY;
		nNumEdgePnt ++;
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );

}



/******************************************************************************/
/**
 * @brief			�G�b�W�_���o�i���������T���p�j
 *
 * @param[in]		ptImgRho				:,�����x�ʒu���ߗp�@�G�b�W���x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		ptImgTheta				:,�����x�ʒu���ߗp�@�G�b�W�p�x�摜�ւ̃|�C���^,-,[-],
 * @param[out]		ptEdgePnt				:,�����x�ʒu���ߗp�@�G�b�W�_�z��,-,[-],
 * @param[out]		pnNumEdgePnt			:,�����x�ʒu���ߗp�@�G�b�W�_��,-,[-]
 * @param[in]		nDirY					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nThrThetaMin			:,�̗p����G�b�W�p�x�͈́i�ŏ��l�j,-,[-],
 * @param[in]		nThrThetaMax			:,�̗p����G�b�W�p�x�͈́i�ő�l�j,-,[-],
 * @param[in]		nThrRho					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nDirectNum				:,�T�������i�}�C�i�X�i�������j or �v���X(�E����)�j,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.01.06	F.Sano	�V�K�쐬(�֐�cb_FPE_ExtractEdgePnt4AddChk���ו���) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_Search_Hori(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,
										    			CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt, slong nDirY, 
													slong nThrThetaMin, slong nThrThetaMax, slong nThrRho, slong nDirectNum, slong nprm_HalfScanRgn)
{
	slong	ni, nj;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* �p�����[�^ */
/*	slong	nprm_HalfScanRgn;*/
	slong	nprm_MaxSearchRetry;
	slong	nprm_MaxScanDistance;
	float_t	nprm_coeffRho;

	uchar*	pnAddrRho;
	uchar*	pnAddrTheta;
	slong	nTargetRho;
	slong	nTargetTheta;
	slong	nTargetX;
	slong	nTargetY;
	slong	nRetryNum;

	slong	nNumEdgePnt;

	/* �����`�F�b�N */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �G�b�W�_�����擾 */
	nNumEdgePnt = *pnNumEdgePnt;

	tMaxPntInfo.nFtrValue = 0L;		/* �����l */
	tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
	tMaxPntInfo.tPnt.nY = -1L;		/* �����l */

	/* �p�����[�^�擾 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_MaxSearchRetry		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_MAX_RETRY;
	nprm_MaxScanDistance	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	/*
	====================================
		�A�T�������_���獶(�܂��͉E)�ɓ_�Q�T��
		nDirectNum���}�C�i�X�l�F�F������
		nDirectNum���v���X�l�@�F�F�E����
	====================================
	*/

	nRetryNum = 0L;

	/* �T���_����}�C�i�X�܂��̓v���X�����ɃV�t�g�������W�l */
	nTargetX = ptEdgePnt[0].nX + nDirectNum;
	nTargetY = ptEdgePnt[0].nY;

	/* �y�w�肵�������֒T���z */
	for( ni = 0L; ni < nprm_MaxScanDistance; ni ++ )
	{
		/* �N���A */
		nTargetRho = 0L;
		nTargetTheta = -999L;	/* �G�b�W�R�[�h���ʂƂ��Ă͂��蓾�Ȃ��l */
		tMaxPntInfo.nFtrValue = 0L;		/* �����l */
		tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
		tMaxPntInfo.tPnt.nY = -1L;		/* �����l */


		for ( nj = -nprm_HalfScanRgn; nj <= nprm_HalfScanRgn; nj++ )
		{
			/* ���ړ_�̃A�h���X */
			/* ptImgRho��ptImgTheta���AnXSize���̉�ʃT�C�Y���͓��������A�Ƃ肠�����g�������Ă��� */
			pnAddrRho = (uchar*)( ( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX );
			pnAddrTheta = (uchar*)( ( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX );

			/* �G�b�W���x���v�Z */
			nTargetRho = (slong)*( pnAddrRho - ( ( nDirY * nj ) * ptImgRho->nXSize ) );
			nTargetTheta = (slong)*( pnAddrTheta - ( ( nDirY * nj ) * ptImgTheta->nXSize ) );

			/* �G�b�W�p�x��臒l�͈͂̉�f�̂����A�ł��G�b�W���x���������W��z��Ɋi�[ */
			if( nThrThetaMin < 1L )
			{
				if(		( ( 180L + nThrThetaMin ) < nTargetTheta )
					||	( nThrThetaMax > nTargetTheta ) )
				{
					if ( tMaxPntInfo.nFtrValue < nTargetRho )
					{
						tMaxPntInfo.nFtrValue = nTargetRho;
						tMaxPntInfo.tPnt.nX = nTargetX;
						tMaxPntInfo.tPnt.nY = nTargetY - ( nDirY * nj );
					}
				}
			}
			else if( nThrThetaMax > 180L )
			{
				if(		( nThrThetaMin < nTargetTheta )
					||	( ( nThrThetaMax - 180L ) > nTargetTheta ) )
				{
					if ( tMaxPntInfo.nFtrValue < nTargetRho )
					{
						tMaxPntInfo.nFtrValue = nTargetRho;
						tMaxPntInfo.tPnt.nX = nTargetX;
						tMaxPntInfo.tPnt.nY = nTargetY - ( nDirY * nj );
					}
				}
			}
			else
			{
				if (	( nThrThetaMin < nTargetTheta ) 
					&&	( nThrThetaMax > nTargetTheta ) )
				{
					if ( tMaxPntInfo.nFtrValue < nTargetRho )
					{
						tMaxPntInfo.nFtrValue = nTargetRho;
						tMaxPntInfo.tPnt.nX = nTargetX;
						tMaxPntInfo.tPnt.nY = nTargetY - ( nDirY * nj );
					}
				}
			}
		}

		/* �G�b�W���x���������l�𒴂���ꍇ�͍̗p */
		if ( tMaxPntInfo.nFtrValue > nThrRho )
		{
			ptEdgePnt[nNumEdgePnt].nX = tMaxPntInfo.tPnt.nX;
			ptEdgePnt[nNumEdgePnt].nY = tMaxPntInfo.tPnt.nY;
			nTargetX += nDirectNum;
			nTargetY = tMaxPntInfo.tPnt.nY;
			nNumEdgePnt ++;
		}
		else
		{
			if( nRetryNum < nprm_MaxSearchRetry )
			{
				/* ���g���C�񐔏���܂ł́A���g���C�񐔂��C���N�����ėׂ̃��C����T�� */
				nTargetX += nDirectNum;
				if( tMaxPntInfo.tPnt.nX == -1L )
				{
					/* Not Operation */
				}
				else
				{
					nTargetY = tMaxPntInfo.tPnt.nY;
				}
				nRetryNum ++;
			}
			else
			{
				/* ���g���C����𒴂�����A�T���I�� */
				break;
			}
		}
	}

	/* �G�b�W�_�����X�V */
	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );

}


/******************************************************************************/
/**
 * @brief			�G�b�W�_���o�i���������T���p�j
 *
 * @param[in]		ptImgRho				:,�����x�ʒu���ߗp�@�G�b�W���x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		ptImgTheta				:,�����x�ʒu���ߗp�@�G�b�W�p�x�摜�ւ̃|�C���^,-,[-],
 * @param[out]		ptEdgePnt				:,�����x�ʒu���ߗp�@�G�b�W�_�z��,-,[-],
 * @param[out]		pnNumEdgePnt			:,�����x�ʒu���ߗp�@�G�b�W�_��,-,[-]
 * @param[in]		nDirY					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nThrThetaMin			:,�̗p����G�b�W�p�x�͈́i�ŏ��l�j,-,[-],
 * @param[in]		nThrThetaMax			:,�̗p����G�b�W�p�x�͈́i�ő�l�j,-,[-],
 * @param[in]		nThrRho					:,�X�L�����������(Y��),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nDirectNum				:,�T�������i�}�C�i�X�i�������j or �v���X(�E����)�j,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.01.06	F.Sano	�V�K�쐬(�֐�cb_FPE_ExtractEdgePnt4AddChk���ו���) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_Search_Vert(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,
										    			CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt, slong nThrThetaMin, 
														slong nThrThetaMax, slong nThrRho, slong nDirectNum, slong nprm_HalfScanRgn)
{
	slong	ni, nj;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* �p�����[�^ */
/*	slong	nprm_HalfScanRgn;*/
	slong	nprm_MaxSearchRetry;
	slong	nprm_MaxScanDistance;
	float_t	nprm_coeffRho;

	uchar*	pnAddrRho;
	uchar*	pnAddrTheta;
	slong	nTargetRho;
	slong	nTargetTheta;
	slong	nTargetX;
	slong	nTargetY;
	slong	nRetryNum;

	slong	nNumEdgePnt;

	/* �����`�F�b�N */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �G�b�W�_�����擾 */
	nNumEdgePnt = *pnNumEdgePnt;

	tMaxPntInfo.nFtrValue = 0L;		/* �����l */
	tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
	tMaxPntInfo.tPnt.nY = -1L;		/* �����l */

	/* �p�����[�^�擾 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_MaxSearchRetry		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_MAX_RETRY;
	nprm_MaxScanDistance	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	/*
	====================================
		�A�T�������_�����(�܂��͉�)�ɓ_�Q�T��
		nDirectNum���}�C�i�X�l�F�F�����
		nDirectNum���v���X�l�@�F�F������
	====================================
	*/

	nRetryNum = 0L;

	/* �T���_����}�C�i�X�܂��̓v���X�����ɃV�t�g�������W�l */
	nTargetX = ptEdgePnt[0].nX;
	nTargetY = ptEdgePnt[0].nY + nDirectNum;

	/* �y�w�肵�������֒T���z */
	for( ni = 0L; ni < nprm_MaxScanDistance; ni ++ )
	{
		/* �N���A */
		nTargetRho = 0L;
		nTargetTheta = -999L;	/* �G�b�W�R�[�h���ʂƂ��Ă͂��蓾�Ȃ��l */
		tMaxPntInfo.nFtrValue = 0L;		/* �����l */
		tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
		tMaxPntInfo.tPnt.nY = -1L;		/* �����l */


		for ( nj = -nprm_HalfScanRgn; nj <= nprm_HalfScanRgn; nj++ )
		{
			/* ���ړ_�̃A�h���X */
			/* ptImgRho��ptImgTheta���AnXSize���̉�ʃT�C�Y���͓��������A�Ƃ肠�����g�������Ă��� */
			pnAddrRho = (uchar*)( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX;
			pnAddrTheta = (uchar*)( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX;

			/* �G�b�W���x���v�Z */
			nTargetRho = (slong)*( pnAddrRho + nj );
			nTargetTheta = (slong)*( pnAddrTheta + nj );

			/* �G�b�W�p�x��臒l�͈͂̉�f�̂����A�ł��G�b�W���x���������W��z��Ɋi�[ */
			if( nThrThetaMin < 1L )
			{
				if(		( ( 180L + nThrThetaMin ) < nTargetTheta )
					||	( nThrThetaMax > nTargetTheta ) )
				{
					if ( tMaxPntInfo.nFtrValue < nTargetRho )
					{
						tMaxPntInfo.nFtrValue = nTargetRho;
						tMaxPntInfo.tPnt.nX = nTargetX + nj;
						tMaxPntInfo.tPnt.nY = nTargetY;
					}
				}
			}
			else if( nThrThetaMax > 180L )
			{
				if(		( nThrThetaMin < nTargetTheta )
					||	( ( nThrThetaMax - 180L ) > nTargetTheta ) )
				{
					if ( tMaxPntInfo.nFtrValue < nTargetRho )
					{
						tMaxPntInfo.nFtrValue = nTargetRho;
						tMaxPntInfo.tPnt.nX = nTargetX + nj;
						tMaxPntInfo.tPnt.nY = nTargetY;
					}
				}
			}
			else
			{
				if (	( nThrThetaMin < nTargetTheta ) 
					&&	( nThrThetaMax > nTargetTheta ) )
				{
					if ( tMaxPntInfo.nFtrValue < nTargetRho )
					{
						tMaxPntInfo.nFtrValue = nTargetRho;
						tMaxPntInfo.tPnt.nX = nTargetX + nj;
						tMaxPntInfo.tPnt.nY = nTargetY;
					}
				}
			}
		}

		/* �G�b�W���x���������l�𒴂���ꍇ�͍̗p */
		if ( tMaxPntInfo.nFtrValue > nThrRho )
		{
			ptEdgePnt[nNumEdgePnt].nX = tMaxPntInfo.tPnt.nX;
			ptEdgePnt[nNumEdgePnt].nY = tMaxPntInfo.tPnt.nY;
			nTargetX = tMaxPntInfo.tPnt.nX;
			nTargetY += nDirectNum;
			nNumEdgePnt ++;
		}
		else
		{
			if( nRetryNum < nprm_MaxSearchRetry )
			{
				/* ���g���C�񐔏���܂ł́A���g���C�񐔂��C���N�����ėׂ̃��C����T�� */
				if( tMaxPntInfo.tPnt.nX == -1L )
				{
					/* Not Operation */
				}
				else
				{
					nTargetX = tMaxPntInfo.tPnt.nX;
				}
				nTargetY += nDirectNum;
				nRetryNum ++;
			}
			else
			{
				/* ���g���C����𒴂�����A�T���I�� */
				break;
			}
		}
	}

	/* �G�b�W�_�����X�V */
	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );

}


/******************************************************************************/
/**
 * @brief			�d�ݕt��Haar-like�����ʕ�������֐�
 *
 * @param[in]		ptPatternInfo			:,�����p�^�[�����\����,-,[-],
 * @param[in]		nCamDirection			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in,out]	nSignHaarLikeFeature	:,�摜���W(�\���ʒu)�\���̂ւ̃|�C���^,-,[-],
 *
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.27	F.Sano	�V�K�쐬
 * @date			2016.06.24	M.Ando	�G���u�����ɂ��P�����Ή�
 *
 * @note			����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.11.27 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_GetSignHaarLikeFeature( const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, slong nCamDirection, slong* nSignHaarLikeFeature)
{

	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfo ) || ( NULL == nSignHaarLikeFeature ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �s���p�^�[���̃^�C�v�m�F */
	if ( ( ptPatternInfo->nTypePattern < CB_IMG_PATTERN_TYPE_NONE ) || ( CB_IMG_PATTERN_TYPE_CHKBOARD1x4 < ptPatternInfo->nTypePattern ) )
	{
		return ( CB_IMG_NG );
	}

	/* �s���p�^�[���̌����m�F */
	if ( (ptPatternInfo->nFlagPlacement < CB_IMG_CHKBOARD_POS_PLACEMENT ) || (CB_IMG_CHKBOARD_NEG_PLACEMENT < ptPatternInfo->nFlagPlacement ) )
	{
		return ( CB_IMG_NG );
	}

	/* �s���p�^�[���̃A���S���Y���m�F */
	if ( (ptPatternInfo->nTypeAlgorithm < CB_IMG_ALGORITHM_DEFAULT ) || (CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM < ptPatternInfo->nTypeAlgorithm ) )
	{
		return ( CB_IMG_NG );
	}

	/* ��������e�[�u�����擾 */
	*nSignHaarLikeFeature = SignHaarLikeFeatureTable[ ptPatternInfo->nTypePattern ][ ptPatternInfo->nFlagPlacement ][ ptPatternInfo->nTypeAlgorithm ]; 

	/* ���A�J�����̏ꍇ�A�����𔽓]������ */
	if( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection )
	{
		*nSignHaarLikeFeature *= CB_IMG_CHECKBOARD_SIGN_CHANGE;
	}
	else
	{
		/* �t�����g�E�T�C�h���t�g�E�T�C�h���C�g�̏ꍇ�͓��ɏ����Ȃ� */
		/* Not Operation */
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			�A���S���Y���^�C�v����֐�
 *
 * @param[in]		ptPatternInfo			:,�����p�^�[�����\����,-,[-],
 * @param[in,out]	pnTypeAlgorithm			:,�A���S���Y���^�C�v�i�[��ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.01.07				K.Kato		�V�K�쐬
 * @date			2016.06.24				M.Ando		�G���u�����ɂ��P�����Ή�
 */
/******************************************************************************/
static slong cb_FPE_GetTypeAlgorithm( const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, enum enum_CB_IMG_ALGORITHM* pnTypeAlgorithm )
{

	/* �����`�F�b�N */
	if ( ( NULL == ptPatternInfo ) || ( NULL == pnTypeAlgorithm ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �s���p�^�[���̃^�C�v�m�F */
	if ( ( ptPatternInfo->nTypePattern < CB_IMG_PATTERN_TYPE_NONE ) || ( CB_IMG_PATTERN_TYPE_CHKBOARD1x4 < ptPatternInfo->nTypePattern ) )
	{
		return ( CB_IMG_NG );
	}

	/* �s���p�^�[���̌����m�F */
	if ( (ptPatternInfo->nFlagPlacement < CB_IMG_CHKBOARD_POS_PLACEMENT ) || (CB_IMG_CHKBOARD_NEG_PLACEMENT < ptPatternInfo->nFlagPlacement ) )
	{
		return ( CB_IMG_NG );
	}

	/* �s���p�^�[���̃A���S���Y���m�F */
	if ( (ptPatternInfo->nTypeAlgorithm < CB_IMG_ALGORITHM_DEFAULT ) || (CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM < ptPatternInfo->nTypeAlgorithm ) )
	{
		return ( CB_IMG_NG );
	}

	/* ��������e�[�u�����擾 */
	*pnTypeAlgorithm = m_FPE_nFtrPntPosTypeTbl4Chkboard[ ptPatternInfo->nTypePattern ][ ptPatternInfo->nFlagPlacement ][ ptPatternInfo->nTypeAlgorithm ]; 

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			�G�b�W�_���o(���E�s���p�����x�ʒu����)
 *
 * @param[in]		ptImgSrc				:,�\�[�X�摜�������A�N�Z�X���ւ̃|�C���^,-,[-],
 * @param[in]		ptEdgePnt				:,�G�b�W�ʒu�ւ̃|�C���^,-,[-],
 * @param[in]		nNumEdgePnt				:,�G�b�W�̐�,0<value,[-],
 * @param[out]		ptEdgePrecPnt			:,�G�b�W�ʒu(�����x)�ւ̃|�C���^,-,[-],
 * @param[in]		nFltMode				:,�G�b�W���,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE,[-],
 * @param[in]		nScanDirX				:,�X�L��������(X����),CB_FPE_SCAN_DIR_X_PLUS<=value<=CB_FPE_SCAN_DIR_X_MINUS,[-],
 * @param[in]		nScanDirY				:,�X�L��������(Y����),CB_FPE_SCAN_DIR_Y_PLUS<=value<=CB_FPE_SCAN_DIR_Y_MINUS,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.03.21	K.Kato		�V�K�쐬
 * @date			2015.09.14	S.Morita	臒l����̔���ǉ��E�����x�G�b�W�_�擾������break�ɂ�鏈�����C��
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePrecisePoint4AddChk( const CB_FPE_IMG_ACCESS_INFO* const ptImgSrc, const CB_IMG_POINT* const ptEdgePnt, slong nNumEdgePnt, t_cb_img_CenterPos* ptEdgePrecPnt, slong* pnNumEdgePrecPnt, enum enum_CB_FPE_LINE_FLT_MODE nFltMode, enum enum_CB_FPE_SCAN_DIR_X nScanDirX, enum enum_CB_FPE_SCAN_DIR_Y nScanDirY )
{
	slong	nRet = CB_IMG_NG;
	slong	ni = 0L, 
			nj = 0L;
	slong	nPntX = 0L, 
			nPntY = 0L;
	slong	nPntXBlack = 0L, 
			nPntYBlack = 0L;
	slong	nCoeffDirX = 0L, 
			nCoeffDirY = 0L;
	slong	nIntensityBlack = 0L, 
			nIntensityWhite = 0L;
	slong	nIdxMedian = 0L;
	slong	nDistanceIntensity = 0L;
	slong	nDistancePixel = 0L;
	slong	nPixelValueCur = 0L, 
			nPixelValuePrev = 0L;
	slong	nNumEdgePrecPnt = 0L;
	slong	nTblIntensityBlack[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM] = { 0L }, 
			nTblIntensityWhite[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM] = { 0L };
	double_t	dDistanceIntensityBlack = 0.0, 
				dDistanceIntensityWhite = 0.0;
	double_t	dThrIntensity = 0.0;
	double_t	dRatioBlack = 0.0, 
				dRatioWhite = 0.0;
	uchar	*pnTopPixel = NULL;

	slong	nPixelValueDiff = 0L; 
	slong	nPixelValueDiffMax = 0L; 
	slong	nPixelValueCurMax = 0L; 
	slong	nPixelValuePrevMax = 0L; 

	/* �����`�F�b�N */
	if ( ( NULL == ptImgSrc ) || ( NULL == ptEdgePnt ) || ( NULL == ptEdgePrecPnt ) || ( NULL == pnNumEdgePrecPnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	pnTopPixel = (uchar*)( ptImgSrc->pnAddrY8 );

	if ( CB_FPE_LINE_FLT_HORI_EDGE == nFltMode )
	{
		if ( CB_FPE_SCAN_DIR_Y_PLUS == nScanDirY )
		{
			nCoeffDirY = (+1L);
		}
		else if ( CB_FPE_SCAN_DIR_Y_MINUS == nScanDirY )
		{
			nCoeffDirY = (-1L);
		}
		else
		{
			/* �G���[ */
			return ( CB_IMG_NG );
		}

		/* <<<<<�������l����>>>>> */
		/* ��f�l�̎擾 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntX = ptEdgePnt[ni].nX;

			/* ���̈摤 */
//			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 6L );
			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 4L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[1] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			/* ���̈摤 */
//			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 5L );
			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 3L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[2] */	// MISRA-C����̈�E [EntryAVM_QAC#4] O4.1  R-4, ID-6853
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}
		}

		/* 臒l���߂̃T���v���������Ή� */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntX = ptEdgePnt[ni].nX;

			/* ���̈摤 臒l���߂̃T���v���������Ή� */
			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 3L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )
			{
				nTblIntensityBlack[ni + nNumEdgePnt] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			/* ���̈摤 臒l���߂̃T���v���������Ή��@*/
			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 2L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )
			{
				nTblIntensityWhite[ni + nNumEdgePnt] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

		}

		/* SORT */
		nRet = cb_FPE_Sort( &(nTblIntensityBlack[0L]), (nNumEdgePnt * 2L) );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		nRet = cb_FPE_Sort( &(nTblIntensityWhite[0L]), (nNumEdgePnt * 2L) );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		/* ��\�l */
		/* 臒l���߂̃T���v������2�{�ɑ��₵�����߁A�ݒ肷��l���unNumEdgePnt / 2L�v����unNumEdgePnt�v�ɏC�����Ă��� */
		nIdxMedian = nNumEdgePnt;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];

		if(nIntensityBlack > nIntensityWhite)
		{
			/* ���̈�̋P�x�l���A���̈�̋P�x�l�����傫���ꍇ�ɂ̓G���[�Ƃ��� */
			return ( CB_IMG_NG );
		}
		
		/* �������l */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<�T�u�s�N�Z������>>>>> */
		nNumEdgePrecPnt = 0L;
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntYBlack = -1L;
			if ( nCoeffDirY < 0L )
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[1],[2]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirY�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				nPixelValueDiffMax = -1L;
				for ( nj = ( ptEdgePnt[ni].nY - ( nCoeffDirY * 3L ) ); nj >= ( ptEdgePnt[ni].nY + ( nCoeffDirY * 3L ) ); nj = nj + nCoeffDirY )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ( nj - ( nCoeffDirY * 1L ) ) * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( nj * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));
					nPixelValueDiff = nPixelValueCur - nPixelValuePrev;

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						if( nPixelValueDiffMax < nPixelValueDiff )
						{
							/* �P�x�����傫���_���̗p���� */
							nPntYBlack = nj - ( nCoeffDirY * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;						
						}
					}
				}
			}
			else if ( 0L < nCoeffDirY )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6861	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6862
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[1],[2]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirY�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				nPixelValueDiffMax = -1L;
				for ( nj = ( ptEdgePnt[ni].nY - ( nCoeffDirY * 3L ) ); nj <= ( ptEdgePnt[ni].nY + ( nCoeffDirY * 3L ) ); nj = nj + nCoeffDirY )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ( nj - ( nCoeffDirY * 1L ) ) * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( nj * ptImgSrc->nXSize ) + ptEdgePnt[ni].nX ));
					nPixelValueDiff = nPixelValueCur - nPixelValuePrev;

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						if( nPixelValueDiffMax < nPixelValueDiff )
						{
							/* �P�x�����傫���_���̗p���� */
							nPntYBlack = nj - ( nCoeffDirY * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;	
						}
					}
				}
			}
			else
			{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6863
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntYBlack )
			{
				nDistanceIntensity = nPixelValueDiffMax;
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrevMax;
				dDistanceIntensityWhite = (double_t)nPixelValueCurMax - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0���`�F�b�N�ǉ� */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0�����̏��� */
					/* �����ł�0�����̓K�؂ȏ����ɂ��ẮA�v�Č��� */
					/* 
					   => ���L������0.5�Ƃ���B
					      nPixelValueCur��nPixelValuePrev�͗אڂ��Ă���̂ŁA�����P�x�l�̏ꍇ�́A���̒��Ԉʒu�Ƃ���
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* �����x */
				ptEdgePrecPnt[nNumEdgePrecPnt].w = (double_t)ptEdgePnt[ni].nX;
				ptEdgePrecPnt[nNumEdgePrecPnt].h = (double_t)nPntYBlack + ( (double_t)nCoeffDirY * ( ( dRatioBlack * (double_t)nDistancePixel ) /*- 1.0*/ ) );

				nNumEdgePrecPnt++;
			}
		}

	}
	else if ( CB_FPE_LINE_FLT_VERT_EDGE == nFltMode )
	{

		if ( CB_FPE_SCAN_DIR_X_PLUS == nScanDirX )
		{
			nCoeffDirX = (+1L);
		}
		else if ( CB_FPE_SCAN_DIR_X_MINUS == nScanDirX )
		{
			nCoeffDirX = (-1L);
		}
		else
		{
			/* �G���[ */
			return ( CB_IMG_NG );
		}

		/* <<<<<�������l����>>>>> */
		/* ��f�l�̎擾 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntY = ptEdgePnt[ni].nY;

			/* ���̈摤 */
//			nPntX = ptEdgePnt[ni].nX - ( nCoeffDirX * 6L );
			nPntX = ptEdgePnt[ni].nX - ( nCoeffDirX * 4L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[3] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			/* ���̈摤 */
//			nPntX = ptEdgePnt[ni].nX + ( nCoeffDirX * 5L );
			nPntX = ptEdgePnt[ni].nX + ( nCoeffDirX * 3L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* �摜�������͈͊O�`�F�b�N[4] */	// MISRA-C����̈�E [EntryAVM_QAC#4] O4.1  R-4, ID-6857
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}
		}
		/* SORT */
		nRet = cb_FPE_Sort( &(nTblIntensityBlack[0L]), nNumEdgePnt );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		nRet = cb_FPE_Sort( &(nTblIntensityWhite[0L]), nNumEdgePnt );
		if ( CB_IMG_OK != nRet ) 
		{
			return ( CB_IMG_NG );
		}
		/* ��\�l */
		nIdxMedian = nNumEdgePnt / 2L;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];

		if(nIntensityBlack > nIntensityWhite)
		{
			/* ���̈�̋P�x�l���A���̈�̋P�x�l�����傫���ꍇ�ɂ̓G���[�Ƃ��� */
			return ( CB_IMG_NG );
		}

		/* �������l */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<�T�u�s�N�Z������>>>>> */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntXBlack = -1L;
			if ( nCoeffDirX < 0L )
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[3],[4]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirX�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				nPixelValueDiffMax = -1L;
				for ( nj = ( ptEdgePnt[ni].nX - ( nCoeffDirX * 3L ) ); nj >= ( ptEdgePnt[ni].nX + ( nCoeffDirX * 3L ) ); nj = nj + nCoeffDirX )
//				for ( nj = ( ptEdgePnt[ni].nX - ( nCoeffDirX * 5L ) ); nj >= ( ptEdgePnt[ni].nX + ( nCoeffDirX * 5L ) ); nj = nj + nCoeffDirX )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + ( nj - ( nCoeffDirX * 1L ) ) ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + nj ));
					nPixelValueDiff = nPixelValueCur - nPixelValuePrev;

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						if( nPixelValueDiffMax < nPixelValueDiff )
						{
							/* �P�x�����傫���_���̗p���� */
							nPntXBlack = nj - ( nCoeffDirX * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;
						}
					}
				}
			}
			else if ( 0L < nCoeffDirX )		// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6866	// MISRA-C����̈�E [EntryAVM_QAC#3] O4.1  R-4, ID-6867
			{
				/*
					�摜�������O�`�F�b�N�ɂ��ẮA�w�摜�������͈͊O�`�F�b�N[3],[4]�x�ɂă`�F�b�N�ς݂̂��ߖ{�����ł̓`�F�b�N���Ȃ����ƂƂ���B
					�������AnCoeffDirX�̌W���ɂ���Ă̓`�F�b�N�K�v�ƂȂ邽�߁A�W���ύX���ɂ̓������O�A�N�Z�X�������Ȃ����m�F���邱�ƁB
					(2013.08.20 K.Kato)
				*/
				nPixelValueDiffMax = -1L;
				for ( nj = ( ptEdgePnt[ni].nX - ( nCoeffDirX * 3L ) ); nj <= ( ptEdgePnt[ni].nX + ( nCoeffDirX * 3L ) ); nj = nj + nCoeffDirX )
//				for ( nj = ( ptEdgePnt[ni].nX - ( nCoeffDirX * 5L ) ); nj <= ( ptEdgePnt[ni].nX + ( nCoeffDirX * 5L ) ); nj = nj + nCoeffDirX )
				{
					nPixelValuePrev = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + ( nj - ( nCoeffDirX * 1L ) ) ));
					nPixelValueCur = (slong)(*( pnTopPixel + ( ptEdgePnt[ni].nY * ptImgSrc->nXSize ) + nj ));
					nPixelValueDiff = nPixelValueCur - nPixelValuePrev;

					if ( ( (double_t)nPixelValuePrev <= dThrIntensity ) && ( dThrIntensity <= (double_t)nPixelValueCur ) )
					{
						if( nPixelValueDiffMax < nPixelValueDiff )
						{
							/* �P�x�����傫���_���̗p���� */
							nPntXBlack = nj - ( nCoeffDirX * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;
						}
					}
				}
			}
			else
			{	// MISRA-C����̈�E [EntryAVM_QAC#3] O1.1  R-52, ID-6868
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntXBlack )
			{
				nDistanceIntensity = nPixelValueDiffMax;
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrevMax;
				dDistanceIntensityWhite = (double_t)nPixelValueCurMax - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0���`�F�b�N�ǉ� */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0�����̏��� */
					/* �����ł�0�����̓K�؂ȏ����ɂ��ẮA�v�Č��� */
					/* 
					   => ���L������0.5�Ƃ���B
					      nPixelValueCur��nPixelValuePrev�͗אڂ��Ă���̂ŁA�����P�x�l�̏ꍇ�́A���̒��Ԉʒu�Ƃ���
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* �����x */
				ptEdgePrecPnt[nNumEdgePrecPnt].w = (double_t)nPntXBlack + ( (double_t)nCoeffDirX * ( ( dRatioBlack * (double_t)nDistancePixel ) /*- 1.0*/ ) );
				ptEdgePrecPnt[nNumEdgePrecPnt].h = (double_t)ptEdgePnt[ni].nY;

				nNumEdgePrecPnt++;
			}
		}
	}
	else
	{
		/* �G���[ */
		return ( CB_IMG_NG );
	}

	/* �����x�ŋ��߂�ꂽ�_���i�[ */
	*pnNumEdgePrecPnt = nNumEdgePrecPnt;

	return ( CB_IMG_OK );
}




/******************************************************************************/
/**
 * @brief			�G�b�W�_���o
 *
 * @param[in]		ptImgRho				:,�����x�ʒu���ߗp�@�G�b�W���x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		ptImgTheta				:,�����x�ʒu���ߗp�@�G�b�W�p�x�摜�ւ̃|�C���^,-,[-],
 * @param[in]		tCrossPnt				:,�\���_���W�\����,-,[-],
 * @param[out]		ptEdgePnt				:,�����x�ʒu���ߗp�@�G�b�W�_�z��,-,[-],
 * @param[out]		pnNumEdgePnt			:,�����x�ʒu���ߗp�@�G�b�W�_��,-,[-]
 * @param[in]		nFltMode				:,Line�t�B���^�̃��[�h,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE
 * @param[in]		nDirX					:,�X�L�����������(X��),-1<=value<=+1,[-],
 * @param[in]		nDirY					:,�X�L�����������(Y��),-1<=value<=+1,[-],
 * @param[in]		nSrchAngleCode			:,�X�L�������ɒT������p�x�R�[�h,CB_FPE_EDGE_AC_RIGHT<=value<=CB_FPE_EDGE_AC_UPPER,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2013.11.27	S.Suzuki	�V�K�쐬
 *					2014.02.13	S.Suzuki		�P�̃e�X�g�w�E�C��
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddCross(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
												CB_IMG_POINT tCrossPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
												enum enum_CB_FPE_LINE_FLT_MODE nFltMode, slong nDirX, slong nDirY, slong nSrchAngleCode )
{
	slong	nRet = CB_IMG_NG;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	slong	nThrThetaMin;
	slong	nThrThetaMax;
	slong	nThrRho;

	slong	nNumEdgePnt;

//	slong	nSrchAngleCode;
	
	nNumEdgePnt = 0L;
	tMaxPntInfo.nFtrValue = 0L;		/* �����l */
	tMaxPntInfo.tPnt.nX = -1L;		/* �����l */
	tMaxPntInfo.tPnt.nY = -1L;		/* �����l */


	/* �����`�F�b�N */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �̈�`�F�b�N */
	if ( ( 0L > tCrossPnt.nX ) || ( 0L > tCrossPnt.nY ) )
	{
		*pnNumEdgePnt = 0L;
		return ( CB_IMG_OK );
	}

	if ( CB_FPE_LINE_FLT_HORI_EDGE == nFltMode )
	{
		/*
		===================================
			�@T���_����V�t�g�����_��T��
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( ptImgRho, ptImgTheta, ptImgAC, tCrossPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirX, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* ��ƂȂ�_��������Ȃ��ꍇ�A���ʂȂ��Ƃ���return */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			�A�T�������_���獶�ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Hori( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nDirY, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			�B�T�������_����E�ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Hori( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nDirY, nThrThetaMin, nThrThetaMax, nThrRho, +1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
	}
	else if ( CB_FPE_LINE_FLT_VERT_EDGE == nFltMode )
	{
		/*
		===================================
			�@T���_����V�t�g�����_��T��
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( ptImgRho, ptImgTheta, ptImgAC, tCrossPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* ��ƂȂ�_��������Ȃ��ꍇ�A���ʂȂ��Ƃ���return */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			�A�T�������_�����ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Vert( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			�B�T�������_���牺�ɓ_�Q�T��
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Vert( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nThrThetaMin, nThrThetaMax, nThrRho, +1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
	}
	else
	{
		/* �R�R�ɗ���͖̂{���L�蓾�Ȃ��p�^�[��*/
		return ( CB_IMG_NG );
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );
}

/*@}*/



/******************************************************************************/
/**
 * @brief			�����_�ʒu�̐���(�ǉ��s���̒����\���_�p)
 *
 * @param[in]		nImgSrc					:,�\�[�X���ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		pnImgWk					:,���[�N���ID�ւ̃|�C���^,-,[-],
 * @param[in]		ptSrchRgnInfo			:,�����̈�\���̂ւ̃|�C���^,-,[-],
 * @param[in,out]	ptSrchRsltAddPntInfo	:,�T�����ʓ_���\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nCamDirection			:,�J�����������,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.02.07	S.Suzuki		�V�K�쐬
 * @date			2015.09.14	S.Morita		��_�v�Z���s���ɁA�����l���i�[����悤�C��
 *
 * @note			����nCamDirection�́A�O������̎擾���ɔ͈̓`�F�b�N���s���Ă��邽�߁A�{�֐����ł͈̔̓`�F�b�N�͏ȗ�����(2013.07.31 Sano�j
 *
 */
/******************************************************************************/
static slong cb_FPE_EstimateFtrPoint4AddCrossPnt( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo,
												CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltAddPntInfo, slong nCamDirection )
{
	slong						nRet = CB_IMG_NG;
	slong						ni = 0L, 
								nj = 0L,
								nk = 0L;
	slong						nDirX = 0L, 
								nDirY = 0L;
	slong						nNumEdgePnt = 0L, 
								nNumEdgePrecPnt = 0L;
	slong						nNumPntArray = 0L;
	enum enum_CB_FPE_SCAN_DIR_X	nScanDirX = CB_FPE_SCAN_DIR_X_PLUS;
	enum enum_CB_FPE_SCAN_DIR_Y	nScanDirY = CB_FPE_SCAN_DIR_Y_PLUS;
	CB_FPE_IMG_ACCESS_INFO		tImgAccessInfo = { 0L, 0L, NULL, NULL, NULL, NULL };
	CB_IMG_POINT				*ptPntArray = NULL;
	t_cb_img_CenterPos			*ptRsltPntArray = NULL;
	CB_IMG_POINT				*ptEdgePnt = NULL;
	t_cb_img_CenterPos			*ptEdgePrecPnt = NULL;

	slong						nSignHaarLikeFeature = 0L;
	CB_FPE_IMG_PATTERN_INFO		*ptPatternInfo = NULL;
	enum enum_CB_FPE_LINE_FLT_MODE	nEdgeType = CB_FPE_LINE_FLT_HORI_EDGE;
	slong						nSrchAngleCode = 0L;

	slong						nTotalNumEdgePrecPntUL = 0L,
								nTotalNumEdgePrecPntLR = 0L;
	t_cb_img_CenterPos			tTotalEdgePrecPntUL[ CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS ],
								tTotalEdgePrecPntLR[ CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS ];


	CB_FPE_IMG_ACCESS_INFO					tEdgeCodeRho = { 0L, 0L, NULL, NULL, NULL, NULL };			/* �����x�ʒu���ߗp�@�G�b�W���x�摜(work�摜1�𗘗p) */
	CB_FPE_IMG_ACCESS_INFO					tEdgeCodeTheta = { 0L, 0L, NULL, NULL, NULL, NULL };		/* �����x�ʒu���ߗp�@�G�b�W�p�x�摜(work�摜2�𗘗p) */
	CB_FPE_IMG_ACCESS_INFO					tEdgeCodeAC = { 0L, 0L, NULL, NULL, NULL, NULL };		/* �����x�ʒu���ߗp�@�G�b�W�p�x�摜(work�摜2�𗘗p) */



	/* �����`�F�b�N */
	if ( ( NULL == pnImgWk )  || ( NULL == ptSrchRgnInfo ) || ( NULL == ptSrchRsltAddPntInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	ptEdgePnt			= &(m_FPE_tWorkInfo.tEdgePntUL[0L]);
	ptEdgePrecPnt		= &(m_FPE_tWorkInfo.tEdgePrecPntUL[0L]);


	/* �e�����_�̏���o�^ */
	ptPntArray = &( ptSrchRsltAddPntInfo->tCrossPnt[0L] );
	ptRsltPntArray = &( ptSrchRsltAddPntInfo->tCrossPntPrecise[0L] );
	nNumPntArray = ptSrchRsltAddPntInfo->nNumCrossPnt;

	ptPatternInfo =  &(ptSrchRgnInfo->tPtnInfo );


	/* �G�b�W�R�[�h���̒��o */
	nRet = cb_FPE_ExtractEdgeCodeInfo( nImgSrc, 
										pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], 
										pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], 
										pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL], 
										&( ptSrchRgnInfo->tRgnImg ),
										nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* �z�񏉊��� */
	for( ni = 0L; ni < CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS; ni++ )
	{
		tTotalEdgePrecPntUL[ni].w = -1.0;
		tTotalEdgePrecPntUL[ni].h = -1.0;
		tTotalEdgePrecPntUL[ni].nFlagPrecise = CB_FALSE;

		tTotalEdgePrecPntLR[ni].w = -1.0;
		tTotalEdgePrecPntLR[ni].h = -1.0;
		tTotalEdgePrecPntLR[ni].nFlagPrecise = CB_FALSE;
	}


	/* �摜�������A�N�Z�X�J�n */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgAccessInfo.nXSize ), &( tImgAccessInfo.nYSize ), (void**)&( tImgAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	/* �G�b�W���x�E�G�b�W�X���摜�̃��������蓖��(�摜�������A�N�Z�X�J�n) */
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], &( tEdgeCodeRho.nXSize ), &( tEdgeCodeRho.nYSize ), (void**)&( tEdgeCodeRho.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], &( tEdgeCodeTheta.nXSize ), &( tEdgeCodeTheta.nYSize ), (void**)&( tEdgeCodeTheta.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL], &( tEdgeCodeAC.nXSize ), &( tEdgeCodeAC.nYSize ), (void**)&( tEdgeCodeAC.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}


	/* �d�ݕt��Haar-like�����ʕ����̎擾 */
	nRet = cb_FPE_GetSignHaarLikeFeature( ptPatternInfo, nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( nRet );
	}


	for ( ni = 0L; ni < CB_FPE_EDGE_POS_PATTERN_MAXNUM; ni++ )
	{
		/* �T���̈�̃V�t�g�����ƒT�������̐ݒ� */
		/* �@�㉺�̐����G�b�W�T����
			 nDirX�F�T������(��������)
			 nDirY�F�T���̈�V�t�g����(��or��)
		   �A���E�̐����G�b�W�T����
			 nDirX�F�T���̈�V�t�g����(��or�E)
			 nDirY�F�T������(��������) 
		   ...�����ꂼ��\�� */
		nDirX = m_FPE_nDirX4PreciseSrchRgnAddCenter[ni];
		nDirY = m_FPE_nDirY4PreciseSrchRgnAddCenter[ni];

		/* HaarLike�����ʂ̕��������̎�(��ʏ�ō����F���̎�) */
		if ( 0L > nSignHaarLikeFeature )
		{
			/* �؂�ւ��s�v */
		}
		/* HaarLike�����ʂ̕��������̎�(��ʏ�ō����F���̎�) */
		else if ( 0L < nSignHaarLikeFeature )
		{
			/* �㉺�̐����G�b�W��T�����鎞�ɂ́AX�����̒T���������ς�� */
			if		( ( CB_FPE_UPPER_EDGE_POS_PATTERN == ni )
				||	  ( CB_FPE_LOWER_EDGE_POS_PATTERN == ni ) )
			{
				nDirX = nDirX * ( -1L );
			}
			/* ���E�̐����G�b�W��T�����鎞�ɂ́AY�����̒T���������ς�� */
			else	/* if	( ( CB_FPE_LEFT_EDGE_POS_PATTERN == ni )
						||	  ( CB_FPE_RIGHT_EDGE_POS_PATTERN == ni ) ) */
			{
				nDirY = nDirY * ( -1L );
			}
		}
		else
		{
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG );
		}


		/* �X�L���������̐ݒ� */
		if ( 0L < nDirX )
		{
			nScanDirX = CB_FPE_SCAN_DIR_X_MINUS;
		}
		else if ( 0L > nDirX )
		{
			nScanDirX = CB_FPE_SCAN_DIR_X_PLUS;
		}
		else
		{
			/* �G���[ */
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG );
		}
		if ( 0L < nDirY )
		{
			nScanDirY = CB_FPE_SCAN_DIR_Y_PLUS;
		}
		else if ( 0L > nDirY )
		{
			nScanDirY = CB_FPE_SCAN_DIR_Y_MINUS;
		}
		else
		{
			/* �G���[ */
			/* Open�����摜��������Close���� */
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG );
		}

		/* �T������G�b�W�^�C�v�̐ݒ� */
		if		( ( CB_FPE_UPPER_EDGE_POS_PATTERN == ni )
			||	  ( CB_FPE_LOWER_EDGE_POS_PATTERN == ni ) )
		{
			nEdgeType = CB_FPE_LINE_FLT_VERT_EDGE;

			/* �����G�b�W�T�����̊p�x���� */
			if( nDirX > 0L )
			{
				nSrchAngleCode = CB_FPE_EDGE_AC_LEFT;
			}
			else
			{
				nSrchAngleCode = CB_FPE_EDGE_AC_RIGHT;
			}
		}
		else	/*	 if ( ( CB_FPE_LEFT_EDGE_POS_PATTERN == ni )
					||	  ( CB_FPE_RIGHT_EDGE_POS_PATTERN == ni ) )	*/
		{
			nEdgeType = CB_FPE_LINE_FLT_HORI_EDGE;

			/* �����G�b�W�T�����̊p�x���� */
			if( nDirY > 0L )
			{
				nSrchAngleCode = CB_FPE_EDGE_AC_LOWER;
			}
			else
			{
				nSrchAngleCode = CB_FPE_EDGE_AC_UPPER;
			}
		}



		for ( nj = 0L; nj < nNumPntArray; nj++ )
		{
			if ( ( 0L <= ptPntArray->nX ) && ( 0L <= ptPntArray->nY ) )
			{

				/* �G�b�W�_�Q���o */
				nRet = cb_FPE_ExtractEdgePnt4AddCross( &tEdgeCodeRho, &tEdgeCodeTheta, &tEdgeCodeAC, *ptPntArray, ptEdgePnt, &nNumEdgePnt, nEdgeType, nDirX, nDirY, nSrchAngleCode );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* �����x�ʒu���� */
				nRet = cb_FPE_ExtractEdgePrecisePoint4AddChk( &tImgAccessInfo, ptEdgePnt, nNumEdgePnt, ptEdgePrecPnt, &nNumEdgePrecPnt, nEdgeType, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Open�����摜��������Close���� */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ����/�����������ɁA�܂Ƃߔz��Ɋi�[ */
				if( nEdgeType == CB_FPE_LINE_FLT_VERT_EDGE )
				{
					/* �����x�ʒu���߂��o�����_�Q�����܂Ƃ߂� */
					for( nk = 0L; nk < nNumEdgePrecPnt; nk++ )
					{
						tTotalEdgePrecPntUL[nTotalNumEdgePrecPntUL + nk].w = ptEdgePrecPnt[nk].w;
						tTotalEdgePrecPntUL[nTotalNumEdgePrecPntUL + nk].h = ptEdgePrecPnt[nk].h;
					}

					/* �_�Q�������Z */
					nTotalNumEdgePrecPntUL += nNumEdgePrecPnt;
				}
				else	/*	 if ( ( nEdgeType == CB_FPE_LINE_FLT_HORI_EDGE ) */
				{
					/* �����x�ʒu���߂��o�����_�Q�����܂Ƃ߂� */
					for( nk = 0L; nk < nNumEdgePrecPnt; nk++ )
					{
						tTotalEdgePrecPntLR[nTotalNumEdgePrecPntLR + nk].w = ptEdgePrecPnt[nk].w;
						tTotalEdgePrecPntLR[nTotalNumEdgePrecPntLR + nk].h = ptEdgePrecPnt[nk].h;
					}

					/* �_�Q�������Z */
					nTotalNumEdgePrecPntLR += nNumEdgePrecPnt;
				}
			}
			else
			{
				/* Not Operation */
			}
		} /* for ( nj = 0L; nj < nNumPntArray[ni]; nj++ ) */
	}

	/* ����/�����̒������ߎ����A��_���v�Z */
	nRet = cb_FPE_CalcIntersectionPoint( tTotalEdgePrecPntUL, tTotalEdgePrecPntLR, nTotalNumEdgePrecPntUL, nTotalNumEdgePrecPntLR, ptRsltPntArray );
	if ( CB_IMG_OK != nRet )
	{
		/* Open�����摜��������Close���� */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG );
	}
	
	/* ��_�v�Z�����s���Ă���Ƃ��́A�����l���i�[���� */
	if ( ptRsltPntArray->nFlagPrecise != CB_TRUE )
	{
		/* �s�N�Z�����x�̍��W�l�ɁA�����l��ݒ肷�� */
		ptPntArray->nX = -1L;
		ptPntArray->nY = -1L;
		
		/* �T�u�s�N�Z�����x�̍��W�l�ɁA�����l��ݒ肷�� */
		ptRsltPntArray->w = (double_t)( ptPntArray->nX );
		ptRsltPntArray->h = (double_t)( ptPntArray->nY );
		ptRsltPntArray->nFlagPrecise = CB_FALSE;
	}
	else
	{
		/* Not Operation */
	}

	/* �摜�������A�N�Z�X�܂Ƃ߂ďI�� */
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		/* �G���[���N���A */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}







/******************************************************************************/
/**
 * @brief			�����x�ʒu����_�ߎ������̌�_�ʒu�̎Z�o
 *
 * @param[in]		ptEdgePrecPntUL			:,�����G�b�W�̍\���_���W�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		ptEdgePrecPntLR			:,�����G�b�W�̍\���_���W�\���̂ւ̃|�C���^,-,[-],
 * @param[in]		nNumEdgePrecPntUL		:,�����G�b�W�̍\���_��,0<=value<=CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS,[-],
 * @param[in]		nNumEdgePrecPntLR		:,�����G�b�W�̍\���_��,0<=value<=CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS,[-],
 * @param[out]		ptRsltPntArray			:,���o���ʍ\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,�ُ�I��,value=-1�`-**,[-],
 *
 * @date			2014.02.07	S.Suzuki		�V�K�쐬
 *					2014.02.13	S.Suzuki		�P�̃e�X�g�w�E�C��
 *
 * @note			-
 *
 */
/******************************************************************************/
static slong cb_FPE_CalcIntersectionPoint(	const t_cb_img_CenterPos* const ptEdgePrecPntUL, const t_cb_img_CenterPos* const ptEdgePrecPntLR, slong nNumEdgePrecPntUL, slong nNumEdgePrecPntLR,
												t_cb_img_CenterPos* ptRsltPntArray )
{
	slong						nRet = CB_IMG_NG;
	slong						nFlgLinearRegUL = 0L, 
								nFlgLinearRegLR = 0L;
	double_t					dTempSlope = 0.0, 
								dTempIntercept = 0.0;
	CB_FPE_LINE_COEFF			tLineCoeffUL = { 0.0, 0.0, CB_FPE_LINE_SLOPE_DIAG_TYPE }, 
								tLineCoeffLR = { 0.0, 0.0, CB_FPE_LINE_SLOPE_DIAG_TYPE };
	t_cb_img_CenterPos			tIntersectPnt = { -1.0, -1.0, CB_FALSE };	/* s.suzuki �����l�ŏ���������悤�ɕύX_20140213 */


	/* �����`�F�b�N */
	if ( ( NULL == ptEdgePrecPntUL )  || ( NULL == ptEdgePrecPntLR ) || ( NULL == ptRsltPntArray ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �ߎ�����(����) */
	nFlgLinearRegUL = CB_FALSE;
	if ( 2L <= nNumEdgePrecPntUL )
	{
		nRet = cb_FPE_LinearRegression4VertLine( ptEdgePrecPntUL, nNumEdgePrecPntUL, &(tLineCoeffUL.dSlope), &(tLineCoeffUL.dIntercept) );
		if ( CB_IMG_OK == nRet )
		{
			if ( CB_FPE_DBL_MIN <= fabs( tLineCoeffUL.dSlope ) )	/* double�f�[�^�̂Ƃ��́Afabs()�ŗǂ����v�m�F */ /* s.suzuki ���l�ł̔�����C��_20140213 */
			{
				tLineCoeffUL.nTypeSlope = CB_FPE_LINE_SLOPE_DIAG_TYPE;
			}
			else
			{
				tLineCoeffUL.nTypeSlope = CB_FPE_LINE_SLOPE_VERT_TYPE;
			}
		}
		else if ( CB_IMG_NG_DIV0 == nRet )
		{
			tLineCoeffUL.nTypeSlope = CB_FPE_LINE_SLOPE_HORI_TYPE;
		}
		else
		{
			/* �G���[ */
			return ( CB_IMG_NG );
		}

		nFlgLinearRegUL = CB_TRUE;	/* �����Z�o�ς݃t���O��ݒ� */
	}

	/* �ߎ�����(����) */
	nFlgLinearRegLR = CB_FALSE;
	if ( 2L <= nNumEdgePrecPntLR )
	{
		nRet = cb_FPE_LinearRegression4HoriLine( ptEdgePrecPntLR, nNumEdgePrecPntLR, &dTempSlope, &dTempIntercept );
		if ( CB_IMG_OK == nRet )
		{
			if ( CB_FPE_DBL_MIN <= fabs( dTempSlope ) )	/* double�f�[�^�̂Ƃ��́Afabs()�ŗǂ����v�m�F */
			{
				tLineCoeffLR.dSlope = 1.0 / dTempSlope;		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.1  , ID-6840
				tLineCoeffLR.dIntercept = -( dTempIntercept / dTempSlope );		// MISRA-C����̈�E [EntryAVM_QAC#3] R3.2.1  , ID-6841
				tLineCoeffLR.nTypeSlope = CB_FPE_LINE_SLOPE_DIAG_TYPE;
			}
			else
			{
				tLineCoeffLR.dSlope = 0.0;
				tLineCoeffLR.dIntercept = ptEdgePrecPntLR[0L].h;
				tLineCoeffLR.nTypeSlope = CB_FPE_LINE_SLOPE_HORI_TYPE;
			}
		}
		else if ( CB_IMG_NG_DIV0 == nRet )
		{
			tLineCoeffLR.dSlope = 0.0;
			tLineCoeffLR.dIntercept = ptEdgePrecPntLR[0L].w;
			tLineCoeffLR.nTypeSlope = CB_FPE_LINE_SLOPE_VERT_TYPE;
		}
		else
		{
			/* �G���[ */
			return ( CB_IMG_NG );
		}

		nFlgLinearRegLR = CB_TRUE;	/* �����Z�o�ς݃t���O��ݒ� */
	}

	if ( ( CB_TRUE == nFlgLinearRegUL ) && ( CB_TRUE == nFlgLinearRegLR ) )	/* �������Z�o�ł������ */
	{

		/* �����̌�_ */
		if (   ( CB_FPE_LINE_SLOPE_HORI_TYPE == tLineCoeffUL.nTypeSlope ) /* �c�������������̂ɉ��� */ 
			|| ( CB_FPE_LINE_SLOPE_VERT_TYPE == tLineCoeffLR.nTypeSlope ) /* ���������������̂ɏc�� */ )	
		{
			/* ���ʊi�[ */
			ptRsltPntArray->w = -1.0;
			ptRsltPntArray->h = -1.0;
			ptRsltPntArray->nFlagPrecise = CB_FALSE;
		}
		else
		{
			if ( CB_FPE_LINE_SLOPE_DIAG_TYPE == tLineCoeffLR.nTypeSlope )	/* ���E���̒������X���Ă��� */
			{
				/* ��_�v�Z */
				nRet = cb_FPE_CalcIntersection( &tLineCoeffUL, &tLineCoeffLR, &tIntersectPnt );
				if ( CB_IMG_OK != nRet )
				{
					/* �G���[ */
					return ( CB_IMG_NG );
				}

				tIntersectPnt.nFlagPrecise = CB_TRUE;	/* s.suzuki �����xOK�t���O�𐬌����̂ݗ��Ă�悤�C��_20140213 */
			}
			else if ( CB_FPE_LINE_SLOPE_HORI_TYPE == tLineCoeffLR.nTypeSlope )	/* ���E���̒��������� */
			{
				tIntersectPnt.w = ( tLineCoeffUL.dSlope * ptEdgePrecPntLR[0].h ) + tLineCoeffUL.dIntercept;
				tIntersectPnt.h = ptEdgePrecPntLR[0].h;
				tIntersectPnt.nFlagPrecise = CB_TRUE;	/* s.suzuki �����xOK�t���O�𐬌����̂ݗ��Ă�悤�C��_20140213 */
			}
			else
			{
				/* �G���[ */
				return ( CB_IMG_NG );
			}

			/* ���ʊi�[ */
			*ptRsltPntArray = tIntersectPnt;	/* s.suzuki ���ʊi�[�����̈ʒu���ړ�_20140213 */
		}
	}
	else
	{
		/* ���ʊi�[ */
		ptRsltPntArray->w = -1.0;
		ptRsltPntArray->h = -1.0;
		ptRsltPntArray->nFlagPrecise = CB_FALSE;
	}	/* if ( ( CB_TRUE == nFlgLinearRegUL ) && ( CB_TRUE == nFlgLinearRegLR ) ) */

	return ( CB_IMG_OK );

}

/******************************************************************************/
/**
 * @brief			���E�}�[�J�[�̃s�N�Z�����x���W�l�ƃT�u�s�N�Z�����x���W�l�̒���������p��������
 *
 * @param[in, out]	ptSrchRsltInfo			:,�����_���o���ʊi�[�\���̂ւ̃|�C���^,-,[-],
 *
 * @retval			CB_IMG_OK				:,����I��,value=0,[-],
 * @retval			CB_IMG_NG_ARG_NULL		:,�ُ�I��,value=-4,[-],
 *
 * @date			2015.09.14	S.Morita	�V�K�쐬
 *
 * @note			-
 *
 */
/******************************************************************************/
static slong cb_FPE_CalcDistCheck( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo )
{
	enum enum_CB_ADD_PATTERN_POS	ni;								/* ���[�v�J�E���^�[ */
	double_t						nxdist;							/* X�������� */
	double_t						nydist;							/* Y�������� */
	double_t						ndistStraight;					/* �������� */
	CB_FPE_SRCH_RSLT_PNT_INFO		*ptSrchRsltAddChkPnt = NULL;	/* �T�����ʓ_��� */

	/* �����`�F�b�N */
	if ( NULL == ptSrchRsltInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* �擪�|�C���^ */
	ptSrchRsltAddChkPnt = &(ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]);

	for( ni = CB_ADD_PATTERN_POS_LEFT; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* ��T���_ */
		/* �T�u�s�N�Z�����x���o�������ɔ�����s��*/
		if( CB_TRUE == (ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].nFlagPrecise) )
		{
			nxdist = (ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].w) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nX);
			nydist = (ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].h) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nY);
			
			/* �s�N�Z�����x�����_�ƃT�u�s�N�Z�����x�����_�̒������������߂� */
			ndistStraight = sqrt((nxdist * nxdist)+(nydist * nydist));
			
			if( CB_FPE_PRM_TOLERANCE_DIST <= ndistStraight )
			{
				/* �������������e�l�𒴂����ꍇ�́A�����_���o���ʂɖ����l��ݒ肷�� */
				ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nX					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nY					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].w				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].h				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].nFlagPrecise	= CB_FALSE;
			}
		}
		
		/* ��T���_ */
		/* �T�u�s�N�Z�����x���o�������ɔ�����s��*/
		if( CB_TRUE == (ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].nFlagPrecise) )
		{
			nxdist = (ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].w) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntLower[0].nX);
			nydist = (ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].h) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntLower[0].nY);
			
			/* �s�N�Z�����x�����_�ƃT�u�s�N�Z�����x�����_�̒������������߂� */
			ndistStraight = sqrt((nxdist * nxdist)+(nydist * nydist));
			
			if( CB_FPE_PRM_TOLERANCE_DIST <= ndistStraight )
			{
				/* �������������e�l�𒴂����ꍇ�́A�����_���o���ʂɖ����l��ݒ肷�� */
				ptSrchRsltAddChkPnt[ni].tTPntLower[0].nX					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntLower[0].nY					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].w				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].h				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].nFlagPrecise	= CB_FALSE;
			}
		}
		
		/* �����\���_ */
		/* �T�u�s�N�Z�����x���o�������ɔ�����s��*/
		if( CB_TRUE == (ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].nFlagPrecise) )
		{
			nxdist = (ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].w) - (double_t)(ptSrchRsltAddChkPnt[ni].tCrossPnt[0].nX);
			nydist = (ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].h) - (double_t)(ptSrchRsltAddChkPnt[ni].tCrossPnt[0].nY);
			
			/* �s�N�Z�����x�����_�ƃT�u�s�N�Z�����x�����_�̒������������߂� */
			ndistStraight = sqrt((nxdist * nxdist)+(nydist * nydist));
			
			if( CB_FPE_PRM_TOLERANCE_DIST <= ndistStraight )
			{
				/* �������������e�l�𒴂����ꍇ�́A�����_���o���ʂɖ����l��ݒ肷�� */
				ptSrchRsltAddChkPnt[ni].tCrossPnt[0].nX						= -1L;
				ptSrchRsltAddChkPnt[ni].tCrossPnt[0].nY						= -1L;
				ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].w				= -1.0;
				ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].h				= -1.0;
				ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].nFlagPrecise	= CB_FALSE;
			}
		}

	}

	return ( CB_IMG_OK );

}

/*@}*/