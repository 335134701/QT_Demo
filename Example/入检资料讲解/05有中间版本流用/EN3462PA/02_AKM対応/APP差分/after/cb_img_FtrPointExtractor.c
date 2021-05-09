/**
 *	@addtogroup Calibコンポーネント
 */
/*@{*/

/******************************************************************************/
/**
 * @file		cb_img_FtrPointExtractor.c
 * @brief		特徴点抽出
 * @author		K.Kato
 * @date		2013.01.10	K.Kato			新規作成
 * @date		2016.06.24	M.Ando			エンブレムによるケラレ対応
 * @date		2017.10.04	A.Honda			線丸の検知能力向上(#2-案2)
 * @note		None
 */
/******************************************************************************/

/********************************************************************
 * インクルードファイル
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

/* 共通ヘッダ */
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

/* 処理時間計測用include */
#ifndef _WINDOWS
#include "RG_ModeConfig.h"
#include "RG_Dbg_TimeMeasurement.h"
#endif /* _WINDOWS */

/* デバッグ用include */
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
 * 内部マクロ定義
 ********************************************************************/

#ifdef SOFTVP
	#define	implib_IP_IntegralImage	implib_IntegralImage	/* 画認LIB側のAPI名間違いに対する暫定対策 */
#endif /* SOFTVP */

/* コンパイルスイッチ */
//#define	CB_FPE_CSW_ENABLE_TEST_MODE
//#define	CB_FPE_CSW_ENABLE_SRCH_ADD_PATTERN_FAST
//#define	CB_FPE_CSW_DEMO
#define	CB_FPE_CSW_CIRCLE_SRCH_RGN		/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 */
#define	CB_FPE_CSW_JUDGE_BR_SIMILARITY	/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 */
#define CB_FPE_SEARCH_REGION_NARROW	/* 左右探索範囲縮小対策 <CHG> 2015.01.21 */

#define	CB_GEO_PI	( (float_t)CB_CG_PI )	/*!< ,円周率,value=3.141593,-,[-], */

#define	CB_FPE_CHK_OUTSIDE_RGN( nX, nY, nSxRgn, nSyRgn, nExRgn, nEyRgn )	(    ( ( ( nSxRgn ) <= ( nX ) ) && ( ( nX ) <= ( nExRgn ) ) ) \
																			  && ( ( ( nSyRgn ) <= ( nY ) ) && ( ( nY ) <= ( nEyRgn ) ) ) )	/*!< ,領域外チェックマクロ定義,-,[-], */
#define	CB_FPE_CONVERT_m2mm_double( nValue )	( ( nValue ) * 1000.0 )	/*!< ,mmからmへの変換(double用),-,[-], */
#define	CB_FPE_SWAP( a, b )	{	( a ) = ( a ) ^ ( b );		\
								( b ) = ( a ) ^ ( b );		\
								( a ) = ( a ) ^ ( b );		\
							}	/*!< ,SWAP用マクロ定義,-,[-], */
#define	CB_FPE_CONVERT_DEG2RAD( dDeg )	( ( (dDeg) * CB_GEO_PI ) / 180.0 )	/*!< ,Degree to Radian,-,[-], */
#define	CB_FPE_CONVERT_RAD2DEG_F( dRad )	( ( (dRad) * 180.0F ) / CB_GEO_PI )	/*!< ,Radian to Degree,-,[-], */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY
#define	CB_FPE_COMP( a, b )		( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )		/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */
#define CB_FPE_NELEMS( a ) ( sizeof( ( a ) ) / sizeof( ( a )[0] ) )					/* 配列の要素数取得 参考元定義はCMN_NELEMS <ADD> 2013.11.25 */

/* --- 未使用変数ワーニング回避 --- */
#define	CALIB_UNUSED_VARIABLE(value)		(void)((value))		/*!< ,未使用変数のWarning回避用,-,[-], */
#define	CALIB_UNUSED_VARIABLE_P(value)		(void*)((value))	/*!< ,未使用ポインタ変数のWarning回避用,-,[-], */

#define CB_FPE_FLT_MIN	( 1.0e-5F )		/*!< ,本最小値よりも小さい値は「0」とみなす,value=1.0e-5F,[-], */
#define CB_FPE_DBL_MIN	( 1.0e-14L )	/*!< ,本最小値よりも小さい値は「0」とみなす,value=1.0e-14L,[-], */

#define	CB_FPE_MAXNUM_SRCH_RGN_PNT			( 4L )	/*!< ,探索領域を構成する最大点数,value=4,[-], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CHKBOARD		( 10L )	/*!< ,市松ターゲット用探索領域X方向のマージン,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CHKBOARD		( 10L )	/*!< ,市松ターゲット用探索領域Y方向のマージン,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CIRCLE_LINE	( 10L )	/*!< ,線丸ターゲット用探索領域X方向のマージン,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CIRCLE_LINE	( 10L )	/*!< ,線丸ターゲット用探索領域Y方向のマージン,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_ADDCHKBOARD		( 10L )	/*!< ,左右市松ターゲット用探索領域X方向のマージン,value=10,[pixel], */
#define	CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_ADDCHKBOARD		( 10L )	/*!< ,左右市松ターゲット用探索領域Y方向のマージン,value=10,[pixel], */

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN
#define	CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN	( 3L )	/*!< ,黒丸探索領域用マージン,value=3,[pixel], *//* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */

#define	CB_FPE_GRID_INTERVAL_WLD_X			( 1.0 )		/*!< ,グリッド線の間隔(X方向),value=1.0,[-], */
#define	CB_FPE_GRID_INTERVAL_WLD_Y			( 1.0 )		/*!< ,グリッド線の間隔(Y方向),value=1.0,[-], */
#define	CB_FPE_GRID_HIGH_ORDER_LEFT_WLD		( -5.0 )	/*!< ,グリッド線左端位置(World),value=-5.0,[m], */
#define	CB_FPE_GRID_HIGH_ORDER_RIGHT_WLD	( +5.0 )	/*!< ,グリッド線右端位置(World),value=+5.0,[m], */
#define	CB_FPE_GRID_HIGH_ORDER_NEAR_WLD		( +0.0 )	/*!< ,グリッド線左端位置(World),value=-5.0,[m], */
#define	CB_FPE_GRID_HIGH_ORDER_FAR_WLD		( +8.0 )	/*!< ,グリッド線右端位置(World),value=+5.0,[m], */

#define	CB_FPE_PRM_ZOOMOUT_XMAG		( 2L )		/*!< ,縮小倍率,value=2,[-], */
#define	CB_FPE_PRM_ZOOMOUT_YMAG		( 2L )		/*!< ,縮小倍率,value=2,[-], */
#define	CB_FPE_IMG_XSIZE_ZOOMOUTx2	( 360L )	/*!< ,縮小時の画像X方向サイズ,value=360,[pixel], */
#define	CB_FPE_IMG_YSIZE_ZOOMOUTx2	( 240L )	/*!< ,縮小時の画像Y方向サイズ,value=240,[pixel], */

#define	CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE	( 10L/*6L/*10L*/ )	/*!< ,Haar-like検出窓(市松エッジ)、ブロックのX方向サイズ,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE	(  5L/*4L/*5L*/ )	/*!< ,Haar-like検出窓(市松エッジ)、ブロックのY方向サイズ,value=5,[pixel], */
#define	CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_XSIZE	( 10L )	/*!< ,Haar-like検出窓(縦エッジ)、ブロックのX方向サイズ,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE	(  5L )	/*!< ,Haar-like検出窓(縦エッジ)、ブロックのY方向サイズ,value=5,[pixel], */
#define	CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE	(  5L )	/*!< ,Haar-like検出窓(横エッジ)、ブロックのX方向サイズ,value=5,[pixel], */
#define	CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_YSIZE	( 10L )	/*!< ,Haar-like検出窓(横エッジ)、ブロックのY方向サイズ,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE	( 10L )	/*!< ,Haar-like検出窓(左右市松エッジ)、ブロックのX方向サイズ,value=10,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE	( 10L )	/*!< ,Haar-like検出窓(左右市松エッジ)、ブロックのY方向サイズ,value=10,[pixel], */
#define CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL	( 6L )	/*!< ,Haar-like小領域検出窓(左右市松エッジ)、ブロックのX方向サイズ,value=6,[pixel], */
#define CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL	( 6L )	/*!< ,Haar-like小領域検出窓(左右市松エッジ)、ブロックのY方向サイズ,value=6,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE	( 290L )	/*!< ,Haar-like検出窓(左右市松エッジ)、大領域の面積,value=290,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL	( 110L )	/*!< ,Haar-like検出窓(左右市松エッジ)、小領域の面積,value=110,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL	( 102L )	/*!< ,Haar-like小検出窓(左右市松エッジ)、大領域の面積,value=102,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL	( 42L )	/*!< ,Haar-like小検出窓(左右市松エッジ)、小領域の面積,value=42,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FRONT	( 320L )	/*!< ,Haar-like検出窓(左右市松エッジ)、大領域の面積,value=320,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FRONT	( 80L )	/*!< ,Haar-like検出窓(左右市松エッジ)、小領域の面積,value=80,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FRONT	( 112L )	/*!< ,Haar-like小検出窓(左右市松エッジ)、大領域の面積,value=112,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FRONT	( 30L )	/*!< ,Haar-like小検出窓(左右市松エッジ)、小領域の面積,value=30,[pixel], */

#ifdef CALIB_PARAM_P32S_EUR_UK
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FR		( 318L )	/*!< ,Haar-like検出窓(フロントカメラ・左右市松エッジ)、大領域の面積,value=290,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FR		(  82L )	/*!< ,Haar-like検出窓(フロントカメラ・左右市松エッジ)、小領域の面積,value=110,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FR	( 114L )	/*!< ,Haar-like小検出窓(フロントカメラ・左右市松エッジ)、大領域の面積,value=102,[pixel], */
#define	CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FR	(  30L )	/*!< ,Haar-like小検出窓(フロントカメラ・左右市松エッジ)、小領域の面積,value=42,[pixel], */
#endif /* CALIB_PARAM_P32S_EUR_UK */

#define	CB_FPE_PRM_COEFF_SMOOTH_FLT	\
						1L, 2L, 1L, \
						2L, 4L, 1L, \
						1L, 2L, 1L			/*!< ,平滑化フィルタの係数,-,[-], */
#define	CB_FPE_PRM_SCALE_SMOOTH_FLT	( 4L )	/*!< ,平滑化フィルタのシフトダウン量,value=4,[-], */

static slong	CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN = 0L;			/*!< ,高精度位置決め用の探索領域(X方向),3≦value≦255,[pixel], */
static slong	CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN= 0L;			/*!< ,高精度位置決め用の探索領域(Y方向),3≦value≦255,[pixel], */
static slong	CB_FPE_PRM_HALF_WIDTH_PRECISE_SRCH_RGN = 0L;	/*!< ,高精度位置決め用の探索領域(1/2、X方向),1≦value≦127,[pixel], */
static slong	CB_FPE_PRM_HALF_HEIGHT_PRECISE_SRCH_RGN = 0L;	/*!< ,高精度位置決め用の探索領域(1/2、Y方向),1≦value≦127,[pixel], */
static slong	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_X = 0L;		/*!< ,探索領域設定時のベース点までの距離(X),0≦value≦255,[pixel], */
static slong	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_Y = 0L;		/*!< ,探索領域設定時のベース点までの距離(Y),0≦value≦255,[pixel], */

static slong	CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN = 0L;	/*!< ,左右市松高精度位置決め、エッジ点探索回数,value=CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN/2L,[-], */

#define	CB_FPE_PRM_SIZE_LINE_FLT				(  5 )						/*!< ,ラインフィルタサイズ,value=5,[pixel], */
#define	CB_FPE_PRM_COEFF_LINE_FLT				{ -1L, -1L, 0L, +1L, +1L }	/*!< ,ラインフィルタの係数,?<=value<=?,[-], */
#define	CB_FPE_PRM_PERIPHERAL_NO_USE			(  2L )						/*!< ,未処理領域,value=2,[pixel], */

#define	CB_FPE_PRM_PROJECTION_DATA_NUM			( 20L )									/*!< ,投影サイズ,value=20,[pixel], */
#define	CB_FPE_PRM_PROJECTION_DATA_NUM_HALF		( CB_FPE_PRM_PROJECTION_DATA_NUM / 2L )	/*!< ,投影サイズ(1/2),value=10,[pixel], */
#define	CB_FPE_PRM_PROJECTION_CENTER_INTERVAL	( 5L )									/*!< ,注目点からの距離,value=5,[pixel], */
#define	CB_FPE_PRM_PROJECTION_WIDTH				( 5L )									/*!< ,投影幅,value=5,[pixel], */

#define	CB_FPE_PRM_RRF_LENGTH_REACH					( 10L )						/*!< ,リーチの長さ最大値,value=10,[-], */
#define	CB_FPE_PRM_RRF_DIFF_INTENSITY				( 45L/*30L/*60L/*80L*/ )	/*!< ,注目点とリーチ先の輝度値の差,value=80,[-], */
#define	CB_FPE_PRM_RRF_DIFF_INTENSITY_RETRY_DIFF	( -8L )						/*!< ,注目点とリーチ先の輝度値の差,value=80,[-], */
#define	CB_FPE_PRM_RRF_START_REACH					(  2L )						/*!< ,リーチ開始距離,value=2,[-], */
#define	CB_FPE_PRM_RRG_THR_REACH_FOR_MAG			(  5L )						/*!< ,間引きを行うときの注目点のリーチ数,value=5,[-], */

#define	CB_FPE_PRM_THR_CNT_REACH			( 6L )		/*!< ,円上のリーチ数と判定するしきい値,value=7,[-], */
#define	CB_FPE_PRM_THR_MIN_LABEL_AREA		( 7L )		/*!< ,ラベル最小しきい値,value=16,[-], */
#define	CB_FPE_PRM_THR_MAX_LABEL_AREA		( 10000L )	/*!< ,ラベル最大しきい値,value=10000,[-], */
#define	CB_FPE_PRM_ASPECT_RATIO				( 0.25F )	/*!< ,ラベルのアスペクト比,value=0.3,[-], */
#define	CB_FPE_PRM_LENGTH_RATIO				( 0.6F )	/*!< ,長さ比,value=0.6,[-], */
#define	CB_FPE_PRM_THR_AREA_RATIO			( 0.45F )	/*!< ,面積比,value=0.45,[-], */
#define	CB_FPE_PRM_THR_SIZE_LABEL			( 3L )		/*!< ,ラベルサイズに対する閾値,value=3,[-], */
#define	CB_FPE_PRM_THR_DISTANCE_RATIO		( 0.5F )	/*!< ,長さ比に対する閾値,value=0.5,[-], */

#define	CB_FPE_PRM_MERGE_DISTANCE_CAND_CROSS_PNT	( 15 * 15 )	/*!< ,十字点のマージ距離,value=15*15,[-], */
#define	CB_FPE_PRM_XSIZE_HALF_SCAN_RGN				( 5L )		/*!< ,T字点探索時のスキャンサイズ(X),value=5,[pixel], */
#define	CB_FPE_PRM_XSIZE_REFINE_HALF_SCAN_RGN		( 2L )		/*!< ,T字点探索時のスキャンサイズ(絞込み)(X),value=2,[pixel], */
#define	CB_FPE_PRM_YSIZE_HALF_SCAN_RGN				( 5L )		/*!< ,T字点探索時のスキャンサイズ(X),value=5,[pixel], */
#define	CB_FPE_PRM_YSIZE_REFINE_HALF_SCAN_RGN		( 2L )		/*!< ,T字点探索時のスキャンサイズ(絞込み)(Y),value=2,[pixel], */
#define	CB_FPE_PRM_DIFF_HAAR_LIKE					( 4000L )	/*!< ,前回点と今回点におけるHaar-like特徴量の差,value=4000,[-], */
#define	CB_FPE_PRM_COEFF_THR_HAAR_LIKE				( 0.80F )	/*!< ,前回点と今回点におけるHaar-like特徴量の差チェック時の今回Haar-like特徴量に対する係数,value=0.85,[-], */

#define	CB_FPE_PRM_XSIZE_HALF_SCAN_RGN_ADD_CHK			( 2L )			/*!< ,左右市松T字点探索時のスキャンサイズ(X),value=2,[pixel], */
#define	CB_FPE_PRM_COEFF_THR_EDGE						( 0.60F )		/*!< ,前回点と今回点におけるエッジ強度特徴量の差チェック時の,今回エッジ強度特徴量に対する係数,value=0.60,[-], */
#define	CB_FPE_PRM_THR_EDGE_FTR_ADD_CHK_T_PNT_UL		( 50L )			/*!< ,左右市松ターゲット縁(上下)上の特徴点抽出時に、特徴点候補とする特徴量に対するエッジ強度しきい値,value=100,[-], */
#define	CB_FPE_PRM_DISTANCE_SRCH_RGN_ADD_CHK_T_PNT_UL	( 3L/*2L*/ )			/*!< ,左右市松ターゲット縁(上下)上の特徴点抽出時、探索領域までの距離,value=3(適当),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN					( 10L/*3L/*5L/*6L*/ )		/*!< ,左右市松高精度位置決め、エッジ点探索幅,value=3(適当),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL				( 5L/*3L/*5L/*6L*/ )		/*!< ,左右市松高精度位置決め、エッジ点探索幅,value=3(適当),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_SRCH_BASE_PNT	( 5L/*6L*/ )		/*!< ,左右市松高精度位置決め、探索開始点の基準点からの距離,value=3(適当),[pixel], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_THR_EDGE_FTR_T_PNT		( 10L/*50L*/ )		/*!< ,左右市松高精度位置決め、特徴点候補とするエッジ強度しきい値,value=10(適当),[-], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_THR_RANGE_THETA			( 10L/*5L*/ )		/*!< ,左右市松高精度位置決め、特徴点候補とするエッジ角度許容範囲,0<value<90,[2deg], */
#define CB_FPE_PRM_ADD_CHK_PRECISE_THR_MAX_RETRY			( 3L )		/*!< ,左右市松高精度位置決め、エッジ点探索リトライ回数上限,value=2(適当),[-], */
#define	CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE			( 0.60F/*0.75F*/ )	/*!< ,左右市松高精度位置決め、エッジ強度特徴量に対する係数,value=0.80(適当),[-], */

#define	CB_FPE_PRM_THR_LINE_LABEL		( 100L )	/*!< ,線・丸の線部分として扱うラベル面積しきい値,value=100,[-], */
#define	CB_FPE_PRM_THR_CIRCLE_LABEL		( 100L )	/*!< ,線・丸の丸部分として扱うラベル面積しきい値,value=100,[-], */
#define	CB_FPE_PRM_SCAN_HEIGHT_LINE_UL	(   2L )	/*!< ,線・丸の線部分における上端・下端のスキャン高さ,value=2,[pixel], */
#define	CB_FPE_PRM_THR_DIFF_EST_X		( 5.0f )	/*!< ,推定位置(X)との差に対するしきい値,value=5.0,[pixel], */
#define	CB_FPE_PRM_COEFF_LINE_LENGTH	( 2.0f )	/*!< ,線・丸のペアとみなすときの線の長さに対する係数,value=2.0,[-], */

#define	CB_FPE_PRM_RRF_DIFF_INTENSITY_PRECISE	( 80L )	/*!< ,円の特徴点を高精度抽出するときの輝度の差に対するしきい値,value=80,[-], */

#define	CB_FPE_PRM_THR_HAAR_FTR_CROSS_PNT		( 3200L )	/*!< ,市松ターゲット内部の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値,value=6400,[-], */
#define	CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT	( 1000L )	/*!< ,左右市松ターゲット内部の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値,value=6400,[-], */
#define	CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT_FRRR	( 400L )	/*!< ,フロントカメラ・リアカメラの左右市松ターゲット内部の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値 */

#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_UL		( 1600L )	/*!< ,市松ターゲット縁(上下)上の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値,value=3200,[-], */
#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_LR		( 1600L )	/*!< ,市松ターゲット縁(左右)上の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値,value=3200,[-], */
#define	CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_UL	( 2L )		/*!< ,市松ターゲット縁(上下)上の特徴点抽出時、探索領域までの距離,value=2,[pixel], */
#define	CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_LR	( 2L )		/*!< ,市松ターゲット縁(左右)上の特徴点抽出時、探索領域までの距離,value=2,[pixel], */
#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_RHO_LIMIT		( 170L )	/*!< ,市松ターゲット縁(上下)上の特徴点抽出時に、特徴点探索処理終了判定に用いるエッジ強度のしきい値 */
#define	CB_FPE_PRM_THR_HAAR_FTR_T_PNT_THETA_LIMIT	( 12L )	/*!< ,市松ターゲット縁(上下)上の特徴点抽出時に、特徴点探索処理終了判定に用いるエッジ角度のしきい値 */

#define	CB_FPE_PRM_THR_EDGE_RHO				( 128L )	/*!< ,高精度位置決め時のエッジ抽出のためのエッジ強度に対するしきい値,value=128,[-], */
#define	CB_FPE_PRM_THR_EDGE_CNT_DOWN		(   4L )	/*!< ,エッジ強度が小さくなったと判定するときのカウンタに対するしきい値,value=4,[-], */

#define	CB_FPE_ERR_TBL_MAXNUM	( 256L )	/*!< ,許容誤差の組み合わせテーブルのサイズ,value=256,[-], */

#define	CB_FPE_CIRCLE_CAND_POINT_MAXNUM	( 20L )	/*!< ,線特徴点の候補位置の点数最大値,value=20,[-], */

#define	CB_FPE_MAX_SLOPE				( 4096.0F )		/*!< ,垂線になったときの傾き仮設定値,value=4096.0F,[-], */
#define CB_FPE_DBG_BR_RESULT_MAXNUM		( 10L )			/*!< ,輝度情報デバッグで描画する最大候補数,value=10L,[-],  */

#define	CB_FPE_CNT_RETRY_CIRCLE_LINE_MAXNUM		( 3L )		/*!< ,丸領域内特徴点抽出処理のリトライ回数(最大),value=3,[-], */
#define	CB_FPE_CNT_RETRY_LABELING_MAXNUM		( 3L )		/*!< ,ラベリング時のリトライ回数(最大),value=3,[-], */
#define	CB_FPE_CNT_RETRY_LABELING_THR_AREA_DIFF	( 2L )		/*!< ,リトライ時の基準しきい値から変動ささせる値,value=2,[-], */
#define	CB_FPE_CODE_LABEL_OVERFLOW				( 0x7FFFL )	/*!< ,ラベルOverflow時のコード,value=0x7FFF,[-], */

#define CB_FPE_PRM_HISTTBL_MAXNUM		( 256L )		/*!< ,輝度情報ヒストグラムのテーブルサイズ,value=256L,[-],  */
#define CB_FPE_PRM_HIST_BIN_WIDTH		( 4L )			/*!< ,輝度情報ヒストグラムのビン幅,value=4L,[-],  */
#define CB_FPE_PRM_HIST_BIN_NUM			( CB_FPE_PRM_HISTTBL_MAXNUM / CB_FPE_PRM_HIST_BIN_WIDTH )	/*!< ,輝度情報ヒストグラムのビン数,value=64L,[-],  */

#define	CB_FPE_PROC_MAXSIZE_X		( 256L )	/*!< ,EdgeCode処理X方向最大サイズ,value=256,[pixel], */
#define	CB_FPE_PROC_MAXSIZE_Y		( 256L )	/*!< ,EdgeCode処理Y方向最大サイズ,value=256,[pixel], */
#define	CB_FPE_EDGE_CODE_TBL_MAXNUM	( CB_FPE_PROC_MAXSIZE_X * CB_FPE_PROC_MAXSIZE_Y )	/*!< ,EdgeCodeTableの最大数,value=65536,[-], */

#define	CB_FPE_PRM_START_EDGE_ANGLE_FR_RR					( 148L )	/*!< ,角度コード設定開始位置のエッジ角度(Fr/Rr),value=148,[-], */
#define	CB_FPE_PRM_START_EDGE_ANGLE_SL_SR					( 158L )	/*!< ,角度コード設定開始位置のエッジ角度(SL/SR),value=158,[-], */
#define	CB_FPE_PRM_RANGE_EDGE_ANGLE_FR_RR					( 65L )		/*!< ,角度範囲(Fr/Rr),value=65,[-], */
#define	CB_FPE_PRM_RANGE_EDGE_ANGLE_SL_SR					( 45L )		/*!< ,角度範囲(SL/SR),value=45,[-], */
#define	CB_FPE_PRM_EDGE_CODE_SHIFT_DOWN						( 3L )		/*!< ,エッジコード時のダウンシフト量,value=3,[-], */
#define	CB_FPE_PRM_EDGE_CODE_THR_MIN						( 10L )		/*!< ,エッジコード時のしきい値(最小)value=10,[-], */
#define	CB_FPE_PRM_EDGE_CODE_THR_MAX						( 255L )	/*!< ,エッジコード時のしきい値(最大)value=255,[-], */
#define	CB_FPE_PRM_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK		( 6L/*3L*/ )		/*!< ,左右市松上の上下T字探索時の探索領域(半分),value=6,[pixel], */
#define	CB_FPE_PRM_REFINE_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK	( 3L )		/*!< ,左右市松上の上下T字探索時の探索領域(半分),value=3,[pixel], */
#define	CB_FPE_PRM_THR_EDGE_RHO_SRCH_CAND_T_PNT_UL_ADD_CHK	( 20L/*10L*/ )		/*!< ,左右市松上の上下T字探索時のエッジ強度に対するしきい値,value=10,[-], */

/* 左右市松パターンHaar-like特徴量符号判定用定数 */
#define CB_IMG_CHECKBOARD_SIGN_PLUS		(  +1L )		/*!< , 左右市松パターンHaar-like特徴量符号（プラス）  ,value= 1,[-], */
#define CB_IMG_CHECKBOARD_SIGN_MINUS	(  -1L )		/*!< , 左右市松パターンHaar-like特徴量符号（マイナス）,value=-1,[-], */
#define CB_IMG_CHECKBOARD_SIGN_NONE		(   0L )		/*!< , 左右市松パターンHaar-like特徴量符号（符号なし（ダミー））,value= 0,[-], */
#define CB_IMG_CHECKBOARD_SIGN_CHANGE	(  -1L )		/*!< , 左右市松パターンHaar-like特徴量符号（符号反転用定数）,value=-1,[-], */


/* 左右市松パターン中央十字点の高精度位置決め用の探索点の最大サイズ	*
 * (上+下) or (左+右)で探索するので、								*
 * CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM(20) の2倍を設定		*/
#define CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS	( 40L )

#define CB_FPE_PRM_HAAR_LABEL_MAX_NUM				( 5L )			/*!< ,マーカ併設時の十字点ラベル候補の最大数, value=5,[-], */
#define	CB_FPE_PRM_THR_HAAR_MIN_LABEL_AREA			( 5L )			/*!< ,マーカ併設時の十字点ラベル最小面積しきい値,value=5,[-], */
#define	CB_FPE_PRM_THR_HAAR_MAX_LABEL_AREA			( 10000L )		/*!< ,マーカ併設時の十字点ラベル最大面積しきい値,value=10000,[-], */
#define CB_FPE_PRM_SRCH_WIDTH_CHECK_HAAR_UL			( 3L )			/*!< ,マーカ併設時の十字点_左右探索(検証)時の上下幅, value=3,[pixel], */
#define CB_FPE_PRM_COEFF_THR_HAAR_CROSS_FTR			( 0.65F )		/*!< ,マーカ併設時の十字点_Haar-Like特徴量閾値算出用の係数, value=0.65,[-], */

#define CB_FPE_MAX_ARREY_EGDE_MEDIAN						( 3L )			/*!< ,エッジ中央値を求める配列サイズ, value=3,[pixel], */

#define CB_FPE_PRM_ERR_DPITCHDEG	( 0.0 )		/*!< ,ピッチ角誤差, value=2.0,[-], */
#define CB_FPE_PRM_ERR_DROLLDEG		( 0.0 )		/*!< ,ロール角誤差, value=2.0,[-], */
#define CB_FPE_PRM_ERR_DYAWDEG		( 0.0 )		/*!< ,ヨー角誤差, value=2.0,[-], */
#define CB_FPE_PRM_ERR_DSHIFTX		( 0.00 )	/*!< ,カメラ位置X座標誤差, value=0.05,[-], */
#define CB_FPE_PRM_ERR_DSHIFTY		( 0.00 )	/*!< ,カメラ位置Y座標誤差, value=0.05,[-], */
#define CB_FPE_PRM_ERR_DSHIFTZ		( 0.00 )	/*!< ,カメラ位置Z座標誤差, value=0.05,[-], */
#define CB_FPE_PRM_ERR_DPOSHORI 	( 30.0 )	/*!< ,水平光軸位置誤差, value=20.0,[-], */
#define CB_FPE_PRM_ERR_DPOSVERT 	( 30.0 )	/*!< ,垂直光軸位置誤差, value=20.0,[-], */

#define CB_FPE_PRM_INTIMG_FLG_CENTER 	( 0U )	/*!< ,中央マーカーのインテグラルイメージ作成フラグ　　サイドマーカー画像サイズ修正対応, value=0,[-], */
#define CB_FPE_PRM_INTIMG_FLG_SIDE	 	( 1U )	/*!< ,サイドマーカーのインテグラルイメージ作成フラグ　サイドマーカー画像サイズ修正対応, value=1,[-], */

#define CB_FPE_PRM_TOLERANCE_DIST	 	( 7.0 )	/*!< ,ピクセル精度座標値とサブピクセル精度座標値の直線距離許容値　ピクセル精度座標値除外対応, value=7.0,[-], */

/********************************************************************
 * 内部列挙型　定義
 ********************************************************************/

/**
 * @brief	画像タイプ
 */
enum enum_CB_FPE_IMG_TYPE
{
	CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL = 0L,								/*!< ,放射画像(入力画像と同サイズ),value=0,[-], */
	CB_FPE_IMG_Y_WORK01_NORMAL,												/*!< ,ワーク画像1(入力画像と同サイズ),value=1,[-], */
	CB_FPE_IMG_Y_WORK02_NORMAL,												/*!< ,ワーク画像2(入力画像と同サイズ),value=2,[-], */
	CB_FPE_IMG_Y_WORK03_NORMAL,												/*!< ,ワーク画像3(入力画像と同サイズ),value=3,[-], */
	CB_FPE_IMG_Y_SIZE_NORMAL_MAXNUM,										/*!< ,入力画像と同サイズかつY画面の数,value=4,[-], */

	CB_FPE_IMG_ZOOMOUT2x2 = CB_FPE_IMG_Y_SIZE_NORMAL_MAXNUM,				/*!< ,入力画像1/2サイズ,value=4,[-], */
	CB_FPE_IMG_EDGE_RHO,													/*!< ,エッジ強度画像,value=5,[-], */
	CB_FPE_IMG_EDGE_ANGLE,													/*!< ,エッジ角度画像,value=6,[-], */
	CB_FPE_IMG_EDGE_AC,														/*!< ,エッジ角度コード画像,value=7,[-], */
	CB_FPE_IMG_HAAR_NORMALIZE,												/*!< ,Haar-Like特徴量(市松エッジ)正規化画像,value=8,[-], */
	CB_FPE_IMG_HAAR_BINARIZE,												/*!< ,Haar-Like特徴量(市松エッジ)二値化画像,value=9,[-], */
	CB_FPE_IMG_HAAR_LABELING,												/*!< ,Haar-Like特徴量(市松エッジ)ラベル画像,value=10,[-], */
	CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM,									/*!< ,入力画像と同サイズかつY画面の数,value=11,[-], */
	
	CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2 = CB_FPE_IMG_Y_SIZE_ZOOMOUT2x2_MAXNUM,	/*!< ,IntegralImage(入力画像サイズ縦横1/2),value=11,[-], */
	CB_FPE_HAAR_LIKE_FTR_CHK_EDGE,											/*!< ,Haar-like特徴量(市松エッジ),value=12,[-], */
	CB_FPE_HAAR_LIKE_FTR_VERT_EDGE,											/*!< ,Haar-like特徴量(縦エッジ),value=13,[-], */
	CB_FPE_HAAR_LIKE_FTR_HORI_EDGE,											/*!< ,Haar-like特徴量(横エッジ),value=14,[-], */
	CB_FPE_IMG_Y32_SIZE_ZOOMOUT2x2_MAXNUM,									/*!< ,入力画像サイズ縦横1/2,value=15,[-], */

	CB_FPE_IMG_NORMAL = CB_FPE_IMG_Y32_SIZE_ZOOMOUT2x2_MAXNUM,				/*!< ,入力画像と同じサイズ,value=15,[-], */
	CB_FPE_INTEGRAL_IMG_NORMAL,												/*!< ,IntegralImage(入力画像と同じサイズ),value=16,[-], */
	CB_FPE_IMG_EDGE_RHO_NORMAL,												/*!< ,エッジ強度画像(入力画像と同じサイズ),value=17,[-], */
	CB_FPE_IMG_EDGE_ANGLE_NORMAL,											/*!< ,エッジ角度画像(入力画像と同じサイズ),value=18,[-], */
	CB_FPE_IMG_EDGE_AC_NORMAL,												/*!< ,エッジ角度コード画像(入力画像と同じサイズ),value=19,[-], */
	CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL,									/*!< ,Haar-like特徴量(市松エッジ)(入力画像と同じサイズ),value=20,[-], */
	CB_FPE_IMG_SIDEMARKER_SIZE_CHG_MAXNUM,									/*!< ,サイドマーカー画像サイズ修正対応用の画像タイプ最大値,value=21,[-], */

	CB_FPE_IMG_MAXNUM = CB_FPE_IMG_SIDEMARKER_SIZE_CHG_MAXNUM				/*!< ,画像最大数,value=21,[-], */
};

/**
 * @brief	Haar-like特徴量のタイプ
 */
enum enum_CB_FPE_HAAR_LIKE_TYPE
{
	CB_FPE_HAAR_LIKE_CHK_EDGE = 0,	/*!< ,市松エッジのHaar-like特徴量,value=0,[-], */
	CB_FPE_HAAR_LIKE_VERT_EDGE,		/*!< ,縦エッジのHaar-like特徴量,value=1,[-], */
	CB_FPE_HAAR_LIKE_HORI_EDGE,		/*!< ,横エッジのHaar-like特徴量,value=2,[-], */
	
	CB_FPE_HAAR_LIKE_MAXNUM			/*!< ,Haar-likeタイプの最大数,value=2,[-], */
};

/**
 * @brief	スキャン方向(Y方向)
 */
enum enum_CB_FPE_SCAN_DIR_Y
{
	CB_FPE_SCAN_DIR_Y_PLUS = 0,	/*!< ,Y方向のプラス側にスキャン,value=0,[-], */
	CB_FPE_SCAN_DIR_Y_MINUS,	/*!< ,Y方向のマイナス側にスキャン,value=1,[-], */

	CB_FPE_SCAN_DIR_Y_MAXNUM	/*!< ,スキャン方向(Y方向)の最大数,value=2,[-], */
};

/**
 * @brief	スキャン方向(X方向)
 */
enum enum_CB_FPE_SCAN_DIR_X
{
	CB_FPE_SCAN_DIR_X_PLUS = 0,	/*!< ,X方向のプラス側にスキャン,value=0,[-], */
	CB_FPE_SCAN_DIR_X_MINUS,	/*!< ,X方向のマイナス側にスキャン,value=1,[-], */

	CB_FPE_SCAN_DIR_X_MAXNUM	/*!< ,スキャン方向(X方向)の最大数,value=,2,[-], */
};

/**
 * @brief	Lineフィルタのモード
 */
enum enum_CB_FPE_LINE_FLT_MODE
{
	CB_FPE_LINE_FLT_HORI_EDGE = 0,	/*!< ,横エッジ用のLineフィルタ,value=0,[-], */
	CB_FPE_LINE_FLT_VERT_EDGE,		/*!< ,縦エッジ用のLineフィルタ,value=1,[-], */

	CB_PFE_LINE_FLT_MODE_MAXNUM		/*!< ,Lineフィルタのモード数,value=2,[-], */
};

/**
 * @brief	直線の傾きタイプ
 */
enum enum_CB_FPE_LINE_SLOPE_TYPE
{
	CB_FPE_LINE_SLOPE_DIAG_TYPE = 0,	/*!< ,斜め,value=0,[-], */
	CB_FPE_LINE_SLOPE_HORI_TYPE,		/*!< ,水平,value=1,[-], */
	CB_FPE_LINE_SLOPE_VERT_TYPE,		/*!< ,垂直,value=2,[-], */
	
	CB_FPE_LINE_SLOPE_TYPE_MAXNUM		/*!< ,直線の傾きタイプ数,value=3,[-], */
};

/**
 * @brief	投影領域のID
 */
enum enum_CB_FPE_PROJECT_AREA_ID
{
	CB_FPE_PROJECT_UP_AREA = 0,	/*!< ,上側の領域ID,value=0,[-], */
	CB_FPE_PROJECT_DOWN_AREA,	/*!< ,下側の領域ID,value=1,[-], */
	CB_FPE_PROJECT_LEFT_AREA,	/*!< ,左側の領域ID,value=2,[-], */
	CB_FPE_PROJECT_RIGHT_AREA,	/*!< ,右側の領域ID,value=3,[-], */

	CB_FPE_PORJECT_AREA_MAXNUM	/*!< ,投影領域のID最大数,value=4,[-], */
};

/**
 * @brief	画素値のX,Y方向入れ替えモード
 */
enum enum_CB_FPE_SHUFFLE_MODE
{
	CB_FPE_NO_SHUFFLE = 0,			/*!< ,入れ替えなし,value=0,[-], */
	CB_FPE_SHUFFLE,					/*!< ,入れ替えあり,value=1,[-], */

	CB_FPE_SHUFFLE_MODE_MAXNUM		/*!< ,入れ替えモードの最大数,value=2,[-], */
};

/**
 * @brief	市松ターゲット上のエッジ位置
 */
enum enum_CB_FPE_EDGE_POS_PATTERN
{
	CB_FPE_UPPER_EDGE_POS_PATTERN = 0,	/*!< ,上側のエッジ,value=0,[-], */
	CB_FPE_LOWER_EDGE_POS_PATTERN,		/*!< ,下側のエッジ,value=1,[-], */
	CB_FPE_LEFT_EDGE_POS_PATTERN,		/*!< ,左側のエッジ,value=2,[-], */
	CB_FPE_RIGHT_EDGE_POS_PATTERN,		/*!< ,右側のエッジ,value=3,[-], */

	CB_FPE_EDGE_POS_PATTERN_MAXNUM		/*!< ,市松ターゲット上のエッジ位置の最大数,value=4,[-], */
};

/**
 * @brief	特徴点位置Index(市松ターゲット)
 */
enum enum_CB_FPE_CHK_FTR_PNT_INDEX
{
	CB_FPE_CHK_FTR_PNT_1 = 0L,	/*!< ,特徴点1,value=0,[-], */
	CB_FPE_CHK_FTR_PNT_2,		/*!< ,特徴点2,value=1,[-], */
	CB_FPE_CHK_FTR_PNT_3,		/*!< ,特徴点3,value=2,[-], */
	CB_FPE_CHK_FTR_PNT_4,		/*!< ,特徴点4,value=3,[-], */
	CB_FPE_CHK_FTR_PNT_5,		/*!< ,特徴点5,value=4,[-], */
	CB_FPE_CHK_FTR_PNT_6,		/*!< ,特徴点6,value=5,[-], */
	CB_FPE_CHK_FTR_PNT_7,		/*!< ,特徴点7,value=6,[-], */
	CB_FPE_CHK_FTR_PNT_8,		/*!< ,特徴点8,value=7,[-], */
	CB_FPE_CHK_FTR_PNT_9,		/*!< ,特徴点9,value=8,[-], */

	CB_FPE_CHK_FTR_PNT_MAXNUM	/*!< ,特徴点の最大数,value=9,[-], */
};

/**
 * @brief	特徴点位置Index(市松ターゲット(Side Camera))
 */
enum enum_CB_FPE_CHK_FTR_PNT_INDEX_SIDE_CAM
{
	CB_FPE_CHK_FTR_PNT_SC_INVALID = -1L,
	CB_FPE_CHK_FTR_PNT_SC_1 = 0L,	/*!< ,特徴点1,value=0,[-], */
	CB_FPE_CHK_FTR_PNT_SC_2,		/*!< ,特徴点2,value=1,[-], */
	CB_FPE_CHK_FTR_PNT_SC_3,		/*!< ,特徴点3,value=2,[-], */
	CB_FPE_CHK_FTR_PNT_SC_4,		/*!< ,特徴点4,value=3,[-], */
	CB_FPE_CHK_FTR_PNT_SC_5,		/*!< ,特徴点5,value=4,[-], */
	CB_FPE_CHK_FTR_PNT_SC_8,		/*!< ,特徴点8,value=5,[-], */
	CB_FPE_CHK_FTR_PNT_SC_9,		/*!< ,特徴点9,value=6,[-], */

	CB_FPE_CHK_FTR_PNT_SC_MAXNUM	/*!< ,特徴点の最大数,value=7,[-], */
};

/**
 * @brief	角度コード変換テーブルINDEX
 */
enum enum_CB_FPE_EDGE_AC_TBL_INDEX
{
	CB_FPE_EDGE_AC_TBL_FR_RR = 0L,	/*!< ,角度コード変換テーブル(Fr/Rr用),value=0,[-], */
	CB_FPE_EDGE_AC_TBL_SL_SR,		/*!< ,角度コード変換テーブル(SL/SR用),value=1,[-], */

	CB_FPE_EDGE_AC_TBL_MAXNUM		/*!< ,角度コード変換テーブルの最大数,value=2,[-], */
};

/**
 * @brief	角度コード
 */
enum enum_CB_FPE_EDGE_AC
{
	CB_FPE_EDGE_AC_RIGHT = 0L,	/*!< ,右側(0degree周辺),value=0,[-], */
	CB_FPE_EDGE_AC_LOWER,		/*!< ,下側(90degree周辺),value=1,[-], */
	CB_FPE_EDGE_AC_LEFT,		/*!< ,左側(180degree周辺),value=2,[-], */
	CB_FPE_EDGE_AC_UPPER,		/*!< ,上側(270degree周辺),value=3,[-], */

	CB_FPE_EDGE_AC_MAXNUM		/*!< ,角度コードの数,value=4,[-], */
};

/**
 * @brief	処理時間計測位置
 */
enum enum_CALIB_TIME_TYPE
{
	/* 0-9 */
	E_CALIB_TIME_IMG_TOTAL				= 0,	/*!< ,全体,value=0,[-], */
	E_CALIB_TIME_CHECKERBOARD_TOTAL,			/*!< ,市松特徴点抽出,value=1,[-], */
	E_CALIB_TIME_CIRCLE_LINE_TOTAL,				/*!< ,線丸特徴点抽出,value=2,[-], */
	E_CALIB_TIME_STORE_RESULT,					/*!< ,結果格納,value=3,[-], */
	E_CALIB_TIME_INTEGRAL_IMG,					/*!< ,Integral Image,value=4,[-], */
	E_CALIB_TIME_HAAR_LIKE_CHK_EDGE,			/*!< ,Haar-like特徴量(市松エッジ)算出,value=5,[-], */
	E_CALIB_TIME_HAAR_LIKE_VERT_EDGE,			/*!< ,Haar-like特徴量(縦エッジ)算出,value=6,[-], */
	E_CALIB_TIME_HAAR_LIKE_HORI_EDGE,			/*!< ,Haar-like特徴量(横エッジ)算出,value=7,[-], */
	E_CALIB_TIME_SRCH_CROSS_PNT,				/*!< ,市松エッジ上の特徴点探索,value=8,[-], */
	E_CALIB_TIME_SRCH_T_PNT_RI,					/*!< ,右側エッジ上のの特徴点探索,value=9,[-], */
	/* 10-19 */
	E_CALIB_TIME_SRCH_T_PNT_LE,					/*!< ,左側エッジ上のの特徴点探索,value=10,[-], */
	E_CALIB_TIME_SRCH_T_PNT_LO,					/*!< ,下側エッジ上のの特徴点探索,value=11,[-], */
	E_CALIB_TIME_SRCH_T_PNT_UP,					/*!< ,上側エッジ上のの特徴点探索,value=12,[-], */
	E_CALIB_TIME_PREC_POS_CHK,					/*!< ,高精度位置決め(市松特徴点),value=13,[-], */
	E_CALIB_TIME_4PNT_CROSS,					/*!< ,高精度位置決め(市松エッジ上の特徴点),value=14,[-], */
	E_CALIB_TIME_EST_PNT_EDGE_PTN,				/*!< ,高精度位置決め(市松エッジ上の特徴点以外),value=15,[-], */
	E_CALIB_TIME_SMOOTH,						/*!< ,平滑化,value=16,[-], */
	E_CALIB_TIME_RRF,							/*!< ,Radial Reach Feature,value=17,[-], */
	E_CALIB_TIME_CIRC_RGN,						/*!< ,線丸特徴点抽出,value=18,[-], */
	E_CALIB_TIME_CIRC_RGN_FR,					/*!< ,線丸特徴点抽出(Fr用),value=19,[-], */
	/* 20-29 */
	E_CALIB_TIME_SET_SRCH_RGN,					/*!< ,探索領域設定,value=20,[-], */
	
	E_CALIB_TIME_MAXNUM							/*!< ,処理時間計測位置の最大数,value=21,[-], */
};

/********************************************************************
 * 内部構造体　定義
 ********************************************************************/

/**
 * @brief	画像メモリアクセス情報
 */
typedef struct tagCB_FPE_IMG_ACCESS_INFO
{
	slong			nXSize;				/*!< ,画像メモリX方向サイズ,1<=value<?,[pixel], */
	slong			nYSize;				/*!< ,画像メモリY方向サイズ,1<=value<?,[pixel], */
	schar			*pnAddrY8;			/*!< ,8bitY画像メモリの先頭アドレス,-,[-], */
	sshort			*pnAddrY16;			/*!< ,16bitY画像メモリの先頭アドレス,-,[-], */
	slong			*pnAddrY32;			/*!< ,32bitY画像メモリの先頭アドレス,-,[-], */
	CB_RECT_RGN		*ptRgn;				/*!< ,処理領域構造体へのポインタ,-,[-], */
} CB_FPE_IMG_ACCESS_INFO;

/**
 * @brief	Vector
 */
typedef struct tagCB_FPE_VECTOR_2D
{
	slong	nX;	/*!< ,X成分,?<=value<=?,[-], */
	slong	nY;	/*!< ,Y成分,?<=value<=?,[-], */
} CB_FPE_VECTOR_2D;

/**
 * @brief	Vector(float)
 */
typedef struct tagCB_FPE_VECTOR_2D_F
{
	float_t	fX;	/*!< ,X成分,?<=value<=?,[-], */
	float_t	fY;	/*!< ,Y成分,?<=value<=?,[-], */
} CB_FPE_VECTOR_2D_F;

/**
 * @brief	直線係数
 */
typedef struct tagCB_FPE_LINE_COEFF
{
	double_t							dSlope;		/*!< ,傾き,?<=value<=?,[-], */
	double_t							dIntercept;	/*!< ,切片,?<=value<=?,[-], */
	enum enum_CB_FPE_LINE_SLOPE_TYPE	nTypeSlope;	/*!< ,直線の傾きタイプ,?,[-], */
} CB_FPE_LINE_COEFF;

/**
 * @brief	直線係数(float)
 */
typedef struct tagCB_FPE_LINE_COEFF_F
{
	float_t								fSlope;		/*!< ,傾き,?<=value<=?,[-], */
	float_t								fIntercept;	/*!< ,切片,?<=value<=?,[-], */
	enum enum_CB_FPE_LINE_SLOPE_TYPE	nTypeSlope;	/*!< ,直線の傾きタイプ,?,[-], */
} CB_FPE_LINE_COEFF_F;

/**
 * @brief	カメラパラメータ情報
 */
typedef struct tagCB_FPE_CAM_PRM
{
	slong				nCamID;			/*!< ,カメラID,0<value<=?,[-], */
	CB_CG_EXTRINSIC_PRM	tExtrinsicPrm;	/*!< ,外部パラメータ,-,[-], */
	CB_CG_INTRINSIC_PRM	tIntrinsicPrm;	/*!< ,内部パラメータ,-,[-], */
	CB_CG_DIST_PRM		tDistPrm;		/*!< ,歪みパラメータ,-,[-], */
} CB_FPE_CAM_PRM;

/**
 * @brief	カメラパラメータ、誤差テーブル
 */
typedef struct tagCB_FPE_CAM_ERR_TBL
{
	double_t	dErrPitch;			/*!< ,Pitchの誤差,?<=value<=?,[deg], */
	double_t	dErrRoll;			/*!< ,Rollの誤差,?<=value<=?,[deg], */
	double_t	dErrYaw;			/*!< ,Yawの誤差,?<=value<=?,[deg], */
	double_t	dErrOffsetWldX;		/*!< ,取り付け位置(X)の誤差,?<=value<=?,[m], */
	double_t	dErrOffsetWldY;		/*!< ,取り付け位置(Y)の誤差,?<=value<=?,[m], */
	double_t	dErrOffsetWldZ;		/*!< ,取り付け位置(Z)の誤差,?<=value<=?,[m], */
	double_t	dErrCntPntHori;		/*!< ,水平光軸位置の誤差,?<=value<=?,[pixel], */
	double_t	dErrCntPntVert;		/*!< ,垂直光軸位置の誤差,?<=value<=?,[pixel], */
} CB_FPE_CAM_ERR_TBL;

/**
 * @brief	カメラパラメータ(誤差)情報
 */
typedef struct tagCB_FPE_CAM_ERR_INFO
{
	CB_FPE_CAM_ERR_TBL	tErrTbl[CB_FPE_ERR_TBL_MAXNUM];	/*!< ,許容誤差テーブル,-,[-], */
} CB_FPE_CAM_ERR_INFO;


/**
 * @brief	特徴パターン情報構造体
 */
typedef struct tagCB_FPE_IMG_PATTERN_INFO
{
	enum enum_CB_IMG_CHKBOARD_PLACEMENT		nFlagPlacement;		/*!< ,パターンの向き,-,[-], */
	enum enum_CB_IMG_PATTERN_TYPE			nTypePattern;		/*!< ,パターンのType,-,[-], */
	enum enum_CB_IMG_ALGORITHM				nTypeAlgorithm;		/*!< ,パターン抽出アルゴリズム,-,[-], */
	uchar	nDoubleType;
	uchar									nlayoutType;		/*!< ,レイアウト種別,-,[-], */											
	uchar									nCenterOffsetType;	/*!< ,オフセットタイプ,-,[-], */											
} CB_FPE_IMG_PATTERN_INFO;

/**
 * @brief	各パターンに対する探索領域
 */
typedef struct tagCB_FPE_SRCH_RGN_INFO_EACH_PATTERN
{
	slong				nNumPnt;								/*!< ,探索領域を構成する点の数,1<=value<?,[-], */
	CB_CG_PNT_WORLD		tPntWld[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,探索領域を構成する点座標,-,[-], */
	CB_IMG_POINT		tPntImg[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,探索領域を構成する点座標,-,[-], */
	CB_RECT_RGN			tRgnImg;								/*!< ,探索領域,-,[-], */
	CB_RECT_RGN			tRgnImgZoomOut;							/*!< ,探索領域(縮小時),-,[-], */
	CB_RECT_RGN			tRgnImgHLsrc;							/*!< ,探索領域(Haar-like考慮),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLsrc;					/*!< ,探索領域(縮小時のソース),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLChkEdge;				/*!< ,探索領域(縮小時、Haar-like(市松)考慮),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLVertEdge;				/*!< ,探索領域(縮小時、Haar-like(縦エッジ)考慮),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLHoriEdge;				/*!< ,探索領域(縮小時、Haar-like(横エッジ)考慮),-,[-], */
	ulong				nNumCenterPnt;							/*!< ,格納座標数(カメラパラメータ推定用),-,[-], */
	CB_CG_PNT_WORLD		tCenterPntWld[CB_FPE_CENTER_PNT_MAXNUM];	/*!< ,格納座標(カメラパラメータ推定用),-,[-], */
	CB_FPE_IMG_PATTERN_INFO		tPtnInfo;						/*!< ,特徴パターン情報,-,[-], */
} CB_FPE_SRCH_RGN_INFO_EACH_PATTERN;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */

/**
 * @brief	黒丸に対する探索領域
 */
typedef struct tagCB_FPE_CIRCLE_SRCH_RGN_INFO
{
	CB_IMG_POINT		tCircleSrchRgnTL;						/*!< ,黒丸探索領域(左上),-,[-], */
	CB_IMG_POINT		tCircleSrchRgnBR;						/*!< ,黒丸探索領域(右下),-,[-], */

#ifdef CB_FPE_CSW_DEBUG_ON
	CB_IMG_POINT		tCircleErrPntTbl[CB_FPE_ERR_TBL_MAXNUM];/*!< ,黒丸中心座標(カメラ誤差考慮),-,[-], */
#endif /* CB_FPE_CSW_DEBUG_ON */

} CB_FPE_CIRCLE_SRCH_RGN_INFO;

#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */

/**
 * @brief	左右市松に対する探索領域
 */
typedef struct tagCB_FPE_ADD_CHK_SRCH_RGN_INFO
{
	CB_RECT_RGN		tAddChkSrchRgn;						/*!< ,探索領域,-,[-], */
#ifdef CB_FPE_CSW_DEBUG_ON
	CB_IMG_POINT		tAddChkErrPntTbl[CB_FPE_ERR_TBL_MAXNUM];/*!< ,中心座標(カメラ誤差考慮),-,[-], */
#endif /* CB_FPE_CSW_DEBUG_ON */

} CB_FPE_ADD_CHK_SRCH_RGN_INFO;

/**
 * @brief	追加パターンに対する探索領域
 */
typedef struct tagCB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN
{
	slong				nNumPnt;								/*!< ,探索領域を構成する点の数,1<=value<?,[-], */
	CB_CG_PNT_WORLD		tPntWld[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,探索領域を構成する点座標(World),-,[-], */
	CB_IMG_POINT		tPntImg[CB_FPE_MAXNUM_SRCH_RGN_PNT];	/*!< ,探索領域を構成する点座標(Image),-,[-], */
	CB_RECT_RGN			tRgnImg;								/*!< ,探索領域,-,[-], */
	CB_RECT_RGN			tRgnImgZoomOut;							/*!< ,探索領域(縮小時),-,[-], */
	CB_RECT_RGN			tRgnImgNormalSize;						/*!< ,探索領域　サイドマーカー画像サイズ修正対応（tRgnImgと同一の値を設定する）,-,[-], */
	CB_RECT_RGN			tRgnImgRRF;								/*!< ,RRF用探索領域,-,[-], */
	CB_RECT_RGN			tRgnImgRRFZoomOut;						/*!< ,RRF用探索領域(縮小時),-,[-], */

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN													/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
	CB_CG_PNT_WORLD					tPntWldCircle[CB_ADD_PATTERN_PNT_POS_MAXNUM];	/*!< ,黒丸中心座標(世界座標系),-,[-], */
	CB_FPE_CIRCLE_SRCH_RGN_INFO		tPntImgCircle[CB_ADD_PATTERN_PNT_POS_MAXNUM];	/*!< ,黒丸における探索領域,-,[-], */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */												/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */

	CB_RECT_RGN			tRgnImgHLsrc;												/*!< ,左右市松探索領域(Haar-like考慮),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLsrc;										/*!< ,左右市松探索領域(縮小時のソース),-,[-], */
	CB_RECT_RGN			tRgnImgZoomOutHLChkEdge;									/*!< ,左右市松探索領域(縮小時、Haar-like(市松)考慮),-,[-], */
	CB_RECT_RGN			tRgnImgNormalHLsrc;											/*!< ,左右市松探索領域　サイドマーカー画像サイズ修正対応（tRgnImgHLsrcと同一の値を設定する）,-,[-], */
	CB_CG_PNT_WORLD					tPntWldSideChkCenter;							/*!< ,左右市松中心座標(世界座標系),-,[-], */
	CB_FPE_ADD_CHK_SRCH_RGN_INFO	tPntImgSideChkCenter;							/*!< ,左右市松上下点探索領域,-,[-], */
	CB_FPE_ADD_CHK_SRCH_RGN_INFO	tPntImgSideChkCenterZoomOut;					/*!< ,左右市松上下点探索領域,-,[-], */
	CB_FPE_ADD_CHK_SRCH_RGN_INFO	tPntImgSideChkCenterNormalSize;					/*!< ,左右市松上下点探索領域　サイドマーカー画像サイズ修正対応（tPntImgSideChkCenterと同一の値を設定する）,-,[-], */
	CB_FPE_IMG_PATTERN_INFO		tPtnInfo;											/*!< ,特徴パターン情報,-,[-], */

} CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN;

/*
 * @brief	各カメラにおける探索領域
 */
typedef struct tagCB_FPE_SRCH_RGN_INFO_EACH_CAM
{
	slong									nFlagValidInfo4Pattern;								/*!< ,市松パターン情報の有効フラグ,0<=value<=1,[-], */
	slong									nFlagValidInfo4AddPattern;							/*!< ,線と丸パターン情報の有効フラグ,0<=value<=1,[-], */
	CB_FPE_SRCH_RGN_INFO_EACH_PATTERN		tSrchRgnInfo4Pattern[CB_PATTERN_POS_MAXNUM];		/*!< ,各パターンにおける探索領域,-,[-], */
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_MAXNUM];	/*!< ,追加パターンにおける探索領域,-,[-], */
} CB_FPE_SRCH_RGN_INFO_EACH_CAM;

/**
 * @brief	Haar-Like情報
 */
typedef struct tagCB_FPE_HAAR_LIKE_FTR
{
	slong	nXSize;		/*!< ,X方向サイズ,1<=value<?,[pixel], */
	slong	nYSize;		/*!< ,Y方向サイズ,1<=value<?,[pixel], */
	slong*	pnFtrValue;	/*!< ,特徴量へのポインタ,-,[-], */
} CB_FPE_HAAR_LIKE_FTR;

/**
 * @brief	輝度値計算情報(フロントカメラ・リアカメラ使用時の左右マーカに対して使用)
 */
typedef struct tagCB_FPE_BRIGHT_CALC_INFO
{
	slong*		pnFtrValue[2];		/*!< ,輝度計算に用いるポインタ, */
	slong		nXSize;				/*!< ,X方向サイズ,1<=value<?,[pixel], */
	slong		nHaarXSize;			/*!< ,Haar-like矩形X方向サイズ,1<=value<?,[pixel], */
	slong		CalcBrightness[2];	/*!< ,設定領域内の輝度値合計, */
} CB_FPE_BRIGHT_CALC_INFO;

/**
 * @brief	探索結果点情報
 */
typedef struct tagCB_FPE_SRCH_RSLT_PNT_INFO
{
	slong				nNumCrossPnt;									/*!< ,十字点数,0<=value<?,[-], */
	slong				nNumTPntLower;									/*!< ,下側Ｔ字点数,0<=value<?,[-], */
	slong				nNumTPntUpper;									/*!< ,上側Ｔ字点数,0<=value<?,[-], */
	slong				nNumTPntLeft;									/*!< ,左側Ｔ字点数,0<=value<?,[-], */
	slong				nNumTPntRight;									/*!< ,右側Ｔ字点数,0<=value<?,[-], */
	CB_IMG_POINT		tCrossPnt[CB_FPE_CROSS_PNT_MAXNUM];				/*!< ,十字点座標,-,[-], */
	CB_IMG_POINT		tTPntLower[CB_FPE_T_PNT_LOWER_MAXNUM];			/*!< ,下側Ｔ字点座標,-,[-], */
	CB_IMG_POINT		tTPntUpper[CB_FPE_T_PNT_UPPER_MAXNUM];			/*!< ,上側Ｔ字点座標,-,[-], */
	CB_IMG_POINT		tTPntLeft[CB_FPE_T_PNT_LEFT_MAXNUM];			/*!< ,左側Ｔ字点座標,-,[-], */
	CB_IMG_POINT		tTPntRight[CB_FPE_T_PNT_RIGHT_MAXNUM];			/*!< ,右側Ｔ字点座標,-,[-], */
	CB_IMG_POINT		tCrossPntHalf[CB_FPE_CROSS_PNT_MAXNUM];			/*!< ,十字点座標(1/2サイズ),-,[-], */
	CB_IMG_POINT		tTPntLowerHalf[CB_FPE_T_PNT_LOWER_MAXNUM];		/*!< ,下側Ｔ字点座標(1/2サイズ),-,[-], */
	CB_IMG_POINT		tTPntUpperHalf[CB_FPE_T_PNT_UPPER_MAXNUM];		/*!< ,上側Ｔ字点座標(1/2サイズ),-,[-], */
	CB_IMG_POINT		tTPntLeftHalf[CB_FPE_T_PNT_LEFT_MAXNUM];		/*!< ,左側Ｔ字点座標(1/2サイズ),-,[-], */
	CB_IMG_POINT		tTPntRightHalf[CB_FPE_T_PNT_RIGHT_MAXNUM];		/*!< ,右側Ｔ字点座標(1/2サイズ),-,[-], */
	t_cb_img_CenterPos	tCrossPntPrecise[CB_FPE_CROSS_PNT_MAXNUM];		/*!< ,十字点座標(サブピクセル),-,[-], */
	t_cb_img_CenterPos	tTPntLowerPrecise[CB_FPE_T_PNT_LOWER_MAXNUM];	/*!< ,下側Ｔ字点座標(サブピクセル),-,[-], */
	t_cb_img_CenterPos	tTPntUpperPrecise[CB_FPE_T_PNT_UPPER_MAXNUM];	/*!< ,上側Ｔ字点座標(サブピクセル),-,[-], */
	t_cb_img_CenterPos	tTPntLeftPrecise[CB_FPE_T_PNT_LEFT_MAXNUM];		/*!< ,左側Ｔ字点座標(サブピクセル),-,[-], */
	t_cb_img_CenterPos	tTPntRightPrecise[CB_FPE_T_PNT_RIGHT_MAXNUM];	/*!< ,右側Ｔ字点座標(サブピクセル),-,[-], */
} CB_FPE_SRCH_RSLT_PNT_INFO;

/**
 * @brief	探索結果情報(追加パターン)
 */
typedef struct tagCB_FPE_SRCH_RSLT_ADD_PNT_INFO
{
	slong				nNumCenterPnt;										/*!< ,円の中点の数,0<=value<,[-], */
	CB_IMG_POINT		tCenterPnt[CB_ADD_PATTERN_PNT_POS_MAXNUM];			/*!< ,円の中点,-,[-], */
	t_cb_img_CenterPos	tCenterPntPrecise[CB_ADD_PATTERN_PNT_POS_MAXNUM];	/*!< ,円の中点(サブピクセル),-,[-], */
} CB_FPE_SRCH_RSLT_ADD_PNT_INFO;

/**
 * @brief	探索結果情報
 */
typedef struct tagCB_FPE_SRCH_RSLT_INFO
{
	CB_FPE_SRCH_RSLT_PNT_INFO			tSrchRsltPntInfo[CB_PATTERN_POS_MAXNUM];			/*!< ,探索結果点情報,-,[-], */
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO		tSrchRsltAddPntInfo[CB_ADD_PATTERN_POS_MAXNUM];		/*!< ,探索結果点(追加パターン：：線丸),-,[-], */
	CB_FPE_SRCH_RSLT_PNT_INFO			tSrchRsltAddChkPntInfo[CB_ADD_PATTERN_POS_MAXNUM];	/*!< ,探索結果点(追加パターン：：市松),-,[-], */
} CB_FPE_SRCH_RSLT_INFO;

/**
 * @brief	候補点情報
 */
typedef struct tagCB_FPE_CAND_PNT_INFO
{
	slong			nFtrValue;	/*!< ,特徴量,?<=value<=?,[-], */
	CB_IMG_POINT	tPnt;		/*!< ,候補点座標,-,[-], */
} CB_FPE_CAND_PNT_INFO;

/**
 * @brief	候補円領域情報
 */
typedef struct tagCB_FPE_CAND_CIRCLE_INFO
{
	slong	nIdxUpperLabel;	/*!< ,上側丸を示すラベルIndex,1<=value<=255,[-], */
	slong	nIdxLowerLabel;	/*!< ,下側丸を示すラベルIndex,1<=value<=255,[-], */
	float_t	fDistance;		/*!< ,距離,0.0<=value,[-], */
} CB_FPE_CAND_CIRCLE_INFO;

/**
 * @brief	線と丸に関する組み合わせ情報
 */
typedef struct
{
	uchar	nIdxUpperCircle;	/*!< ,上側丸を示すIndex,?<=value<=?,[-], */
	uchar	nIdxLowerCircle;	/*!< ,下側丸を示すIndex,?<=value<=?,[-], */
	uchar	nIdxLine;			/*!< ,線を示すIndex,?<=value<=?,[-], */
} CB_FPE_CIRCLE_LINE_COMB;

/**
 * @brief	線パターンにおける線情報
 */
typedef struct
{
	uchar				nIdxLine;			/*!< ,線を示すIndex,?<=value<=?,[-], */
	float_t				fLength;			/*!< ,長さ,0.0<=value,[-], */
	CB_IMG_POINT_F		tUpperCenterPnt;	/*!< ,上側丸の中心位置,-,[-], */
	CB_IMG_POINT_F		tLowerCenterPnt;	/*!< ,下側丸の中心位置,-,[-], */
	CB_IMG_POINT_F		tMiddleCenterPnt;	/*!< ,中央の中心位置,-,[-], */
	CB_FPE_LINE_COEFF_F	tUpperLineCoeff;	/*!< ,上側直線係数,-,[-], */
	CB_FPE_LINE_COEFF_F	tLowerLineCoeff;	/*!< ,下側直線係数,-,[-], */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY		/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
	slong				nWidthUpper;		/*!< ,線ラベル幅(上端),?<=value<=?,[-], */
	slong				nWidthLower;		/*!< ,線ラベル幅(下端),?<=value<=?,[-], */
	slong				nWidthMiddle;		/*!< ,線ラベル幅(中央),?<=value<=?,[-], */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */	/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */
} CB_FPE_LINE_PART_INFO;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY		/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
/**
 * @brief	輝度判定の座標情報
 */
typedef struct tagCB_FPE_CBR_POINT_INFO
{
	CB_IMG_POINT_F		tLabelUpperPnt;		/*!< ,ラベル上端中心座標,-,[-], */
	CB_IMG_POINT_F		tLabelLowerPnt;		/*!< ,ラベル下端中心座標,-,[-], */
	slong				nSrchWidth;			/*!< ,輝度判定探索領域幅,-,[-], */
	slong				nFlagVertical;		/*!< ,輝度判定探索ライン垂直フラグ,0<=value<=1,[-], */
	float_t				fSrchSlope;			/*!< ,輝度判定探索ライン傾き,-,[-], */
} CB_FPE_CBR_POINT_INFO;

/**
 * @brief	輝度判定の輝度情報
 */
typedef struct tagCB_FPE_CBR_BR_INFO
{
	float_t				fBrTblUpper[ CB_FPE_PRM_HIST_BIN_NUM ];	/*!< ,探索点輝度値(上端),-,[-], */
	float_t				fBrTblLower[ CB_FPE_PRM_HIST_BIN_NUM ];	/*!< ,探索点輝度値(下端),-,[-], */
	float_t				fAvgUpper;			/*!< ,探索領域平均輝度値(上端),-,[-], */
	float_t				fAvgLower;			/*!< ,探索領域平均輝度値(下端),-,[-], */
} CB_FPE_CBR_BR_INFO;

/**
 * @brief	輝度判定の輝度情報
 */
typedef struct tagCB_FPE_CBR_INPUT
{
	CB_FPE_CBR_POINT_INFO	tBrPointTbl[CB_ADD_PATTERN_POS_MAXNUM];
	slong					nImgSrcXSize;
	slong					nImgRRFXSize;
	float_t					fAvgBlackArea;		/*!< ,黒領域平均輝度値(上端丸 + 線 + 下端丸),-,[-], */
} CB_FPE_CBR_INPUT;

#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */	/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

/*
 * @brief	制御情報
 */
typedef struct tagCB_FPE_CTRL
{
	CB_FPE_CAM_PRM					tCamPrm[E_CB_SELFCALIB_CAMPOS_MAX];				/*!< ,カメラパラメータ,-,[-], */
	CB_FPE_CAM_ERR_INFO				tCamErrInfo[E_CB_SELFCALIB_CAMPOS_MAX];			/*!< ,カメラパラメータ誤差,-,[-], */
	CB_FPE_SRCH_RGN_INFO_EACH_CAM	tSrchRgnInfo4Cam[E_CB_SELFCALIB_CAMPOS_MAX];	/*!< ,各カメラにおける探索領域,-,[-], */
} CB_FPE_CTRL;

/**
 * @brief	出力情報
 */
typedef struct tagCB_FPE_RESULT
{
	CB_FPE_SRCH_RSLT_INFO	tSrchRsltInfo[E_CB_SELFCALIB_CAMPOS_MAX];	/*!< ,探索結果情報,-,[-], */
} CB_FPE_RESULT;

/**
 * @brief	パラメータ
 */
typedef struct tagCB_FPE_PRM
{
	slong	nXmagZoomOut;	/*!< ,縮小率(X方向),1<=value<?,[-], */
	slong	nYmagZoomOut;	/*!< ,縮小率(Y方向),1<=value<?,[-], */
	/* ===== Haar-like ===== */
	slong	nXSizeHaarChkEdgeBlock;	/*!< ,市松Haar-likeの1ブロックにおけるX方向サイズ,1<=value<?,[pixel], */
	slong	nYSizeHaarChkEdgeBlock;	/*!< ,市松Haar-likeの1ブロックにおけるY方向サイズ,1<=value<?,[pixel], */
	slong	nXSizeHaarVertEdgeBlock;	/*!< ,垂直Haar-likeの1ブロックにおけるX方向サイズ,1<=value<?,[pixel], */
	slong	nYSizeHaarVertEdgeBlock;	/*!< ,垂直Haar-likeの1ブロックにおけるY方向サイズ,1<=value<?,[pixel], */
	slong	nXSizeHaarHoriEdgeBlock;	/*!< ,水平Haar-likeの1ブロックにおけるX方向サイズ,1<=value<?,[pixel], */
	slong	nYSizeHaarHoriEdgeBlock;	/*!< ,水平Haar-likeの1ブロックにおけるY方向サイズ,1<=value<?,[pixel], */
	/* ===== Radial Reach ===== */
	slong	nLengthReach;			/*!< ,リーチの最大長さ,1<=value<?,[pixel], */
	slong	nDiffIntensity;			/*!< ,輝度値の差,0<=value<?,[-], */
	slong	nDiffIntensity4Retry;	/*!< ,輝度値の差(リトライ時),0<=value<?,[-], */
	/* ===== 円の中点抽出 ===== */
	slong	nCntReach;				/*!< ,所定条件を満たしたリーチの数,0<=value<?,[-], */
	float_t	fAspectRatio;			/*!< ,図形のアスペクト比,0.0<=value<=1.0,[-], */
	float_t	fLengthRatio;			/*!< ,長さの比,0.0<=value<=1.0,[-], */
	/* ===== 市松ターゲット内の特徴点抽出 ===== */
	slong	nXSizePeri;				/*!< ,周辺領域のX方向サイズ,0<=value<?,[pixel], */
	slong	nYSizePeri;				/*!< ,周辺領域のY方向サイズ,0<=value<?,[pixel], */
	slong	nXSizeScanRgn;			/*!< ,スキャン領域のX方向サイズ,1<value<?,[pixel], */
} CB_FPE_PRM;

/**
 * @brief	内部情報
 */
typedef struct tagCB_FPE_INFO
{
	IMPLIB_IMGID			nImg[CB_FPE_IMG_MAXNUM];			/*!< ,画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-], */
	CB_FPE_HAAR_LIKE_FTR	tHaarLike[CB_FPE_HAAR_LIKE_MAXNUM];	/*!< ,Haar-like特徴量,-,[-], */
	CB_FPE_CTRL				tCtrl;								/*!< ,制御情報,-,[-], */
	CB_FPE_RESULT			tRslt;								/*!< ,結果情報,-,[-], */
} CB_FPE_INFO;

/**
 * @brief	ワーク情報
 */
typedef struct tagCB_FPE_WORK_INFO
{
	slong					nNumBasePnt4Upper;											/*!< ,上/下側T字点探索時の基点数,-,[-], */
	CB_IMG_POINT			tBasePnt4Upper[3];											/*!< ,上/下側T字点探索時の基点,-,[-], */
	slong					nNumBasePnt4Lower;											/*!< ,上/下側T字点探索時の基点数,-,[-], */
	CB_IMG_POINT			tBasePnt4Lower[3];											/*!< ,上/下側T字点探索時の基点,-,[-], */
	slong					nTblLabelMinX[256];											/*!< ,ラベルの最小X座標,0<=value<?,[pixel], */
	slong					nTblLabelMaxX[256];											/*!< ,ラベルの最大X座標,0<=value<?,[pixel], */
	slong					nTblLabelMinY[256];											/*!< ,ラベルの最小Y座標,0<=value<?,[pixel], */
	slong					nTblLabelMaxY[256];											/*!< ,ラベルの最大Y座標,0<=value<?,[pixel], */
	slong					nTblAreaLabel[256];											/*!< ,ラベルの面積,0<=value<?,[pixel^2], */
	IMPLIB_IPLOGravityTbl	tTblGravity[256];											/*!< ,ラベルごと重心座標,-,[-], */
	slong					nTblMedian[CB_FPE_PRM_PROJECTION_DATA_NUM];					/*!< ,Median,0<=value<?,[-], */
	slong					nTblProjGO[CB_FPE_PRM_PROJECTION_DATA_NUM];					/*!< ,投影値,0<=value<?,[-], */
	slong					nTblMedian4EdgeOfPtn[CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];	/*!< ,Median,0<=value<=255,[-], */
	IMPLIB_IPGOFeatureTbl	tGOFtrTbl;													/*!< ,濃淡特徴量テーブル,-,[-], */

	uchar					nPixelDataUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];	/*!< ,高精度位置決め用の画素値配列(縦方向),?,[-], */
	uchar					nPixelDataLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];	/*!< ,高精度位置決め用の画素値配列(横方向),?,[-], */
	sshort					nEdgeRhoUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];		/*!< ,高精度位置決め用のエッジ強度配列(縦方向),?,[-], */
	sshort					nEdgeRhoLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM][CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN_MAXNUM];		/*!< ,高精度位置決め用のエッジ強度配列(横方向),?,[-], */
	CB_IMG_POINT			tEdgePntUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];											/*!< ,高精度位置決め用のエッジ点配列(縦方向),-,[-], */
	CB_IMG_POINT			tEdgePntLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];											/*!< ,高精度位置決め用のエッジ点配列(横方向),-,[-], */
	t_cb_img_CenterPos		tEdgePrecPntUL[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];										/*!< ,高精度位置決め用のエッジ点配列(縦方向),-,[-], */
	t_cb_img_CenterPos		tEdgePrecPntLR[CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN_MAXNUM];										/*!< ,高精度位置決め用のエッジ点配列(横方向),-,[-], */
	CB_FPE_IMG_ACCESS_INFO	tPixelDataUL,																			/*!< ,輝度値格納画像メモリアクセス情報(縦方向),-,[-], */
							tPixelDataLR;																			/*!< ,輝度値格納画像メモリアクセス情報(横方向),-,[-], */
	CB_FPE_IMG_ACCESS_INFO	tEdgeRhoUL,																				/*!< ,エッジ強度格納画像メモリアクセス情報(縦方向),-,[-], */
							tEdgeRhoLR;																				/*!< ,エッジ強度格納画像メモリアクセス情報(横方向),-,[-], */

	CB_IMG_POINT_F	tCircleCandPnt[CB_FPE_CIRCLE_CAND_POINT_MAXNUM];	/*!< ,線丸特徴点候補位置格納テーブル,-,[-], */
	slong			nIdxLabelCandPnt[CB_FPE_CIRCLE_CAND_POINT_MAXNUM];	/*!< ,線丸候補位置ラベルIndexテーブル,1<=value<=255,[-], */

	CB_FPE_CAND_CIRCLE_INFO	tCircleCandInfo[CB_FPE_CIRCLE_CAND_POINT_MAXNUM];	/*!< ,線丸候補位置情報テーブル,-,[-], */

	CB_FPE_LINE_PART_INFO	tLinePartInfo[256L];	/*!< ,線に該当するラベルNo.格納用テーブル,1<=value<=255,[-], */
	CB_FPE_CIRCLE_LINE_COMB	tCircleLineComb[256L];	/*!< ,線丸組み合わせテーブル,-,[-], */

	IMPLIB_IPErrorTbl	tErrTbl;	/*!< ,IMPエラー情報管理テーブル,-,[-], */
} CB_FPE_WORK_INFO;


/**
 * @brief	デバッグ情報(グリッド情報)
 */
typedef struct tagCB_FPE_DBG_GRID_INFO
{
	CB_CG_PNT_WORLD	tPntWld[CB_FPE_GRID_NUM_WLD_Y][CB_FPE_GRID_NUM_WLD_X];	/*!< ,グリッド線描画用のポイント(World),-,[-], */
	CB_IMG_POINT	tPntImg[CB_FPE_GRID_NUM_WLD_Y][CB_FPE_GRID_NUM_WLD_X];	/*!< ,グリッド線描画用のポイント(Image),-,[-], */
} CB_FPE_DBG_GRID_INFO;

/**
 * @brief	デバッグ情報
 */
typedef struct tagCB_FPE_DBG_INFO
{
#ifdef CB_FPE_CSW_DEBUG_ON
	CB_COLOR_PALETTE_YUV	tColorYUV;	/*!< ,YUV色,-,[-], */
#endif /* CB_FPE_CSW_DEBUG_ON */
	CB_FPE_DBG_GRID_INFO	tGridInfo[E_CB_SELFCALIB_CAMPOS_MAX];	/*!< ,グリッド情報,-,[-], */
	CB_IMG_POINT	tPntCandidate[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM][2];		/*!< ,パターン候補の上下端座標(3次元目が0：上,1：下),-,[-], */
	float_t			tScore[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM];					/*!< ,パターン候補の輝度判定スコア,-,[-], */
	float_t			tSimilarity[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM];			/*!< ,パターン候補の白領域類似度,-,[-], */
	float_t			tContrast[CB_ADD_PATTERN_POS_MAXNUM][CB_FPE_DBG_BR_RESULT_MAXNUM];				/*!< ,パターン候補の白・黒領域間コントラスト,-,[-], */
	slong			nNum[CB_ADD_PATTERN_POS_MAXNUM];												/*!< ,パターン候補数,-,[-], */
} CB_FPE_DBG_INFO;


/********************************************************************
 * 内部グローバル変数　宣言
 ********************************************************************/

/* 内部 */
static CB_FPE_INFO	m_FPE_tInfo;	/*!< ,内部データ,-,[-], */
static CB_FPE_PRM	m_FPE_tPrm;		/*!< ,パラメータ情報,-,[-], */

/* ワーク */
static CB_FPE_WORK_INFO	m_FPE_tWorkInfo;	/*!< ,ワークデータ,-,[-], */
static slong	m_FPE_nDirX4PreciseSrchRgn[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { 
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Upper */
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Lower */
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Left  */
																				CB_IMG_CHECKBOARD_SIGN_MINUS  /* Right */
																				};	/*!< ,探索領域を設定する方向(X),?,[-], */
static slong	m_FPE_nDirY4PreciseSrchRgn[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = {
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Upper */
																				CB_IMG_CHECKBOARD_SIGN_MINUS, /* Lower */
																				CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Left  */
																				CB_IMG_CHECKBOARD_SIGN_MINUS  /* Right */
																				};	/*!< ,探索領域を設定する方向(Y),?,[-], */
static slong	m_FPE_nFlagInit = CB_FALSE;	/*!< ,初期化済みフラグ,value=0,[-], */
static float_t	m_FPE_fSimilarityMAX;	/*!< ,類似度のMAX値,value=0.0<value<?,[-], */


/* 左右市松パターン用 */
static slong	m_FPE_nDirX4PreciseSrchRgnAddChk[E_CB_SELFCALIB_CAMPOS_MAX][4] = {
																					/* Upper, Lower, Left , Right */
																					{    -1L,   +1L,    0L,    0L },	/* FRONT */
																					{    -1L,   +1L,    0L,    0L },	/* LEFT */ 
																					{    -1L,   +1L,    0L,    0L },	/* RIGHT */
																					{    +1L,   -1L,    0L,    0L }		/* REAR */
																				 };	/*!< ,探索領域を設定する方向(X),?,[-], */
static slong	m_FPE_nDirY4PreciseSrchRgnAddChk[E_CB_SELFCALIB_CAMPOS_MAX][4] = {
																					/* Upper, Lower, Left , Right */
																					{    +1L,   -1L,    0L,    0L },	/* FRONT */
																					{    +1L,   -1L,    0L,    0L },	/* LEFT */ 
																					{    +1L,   -1L,    0L,    0L },	/* RIGHT */
																					{    +1L,   -1L,    0L,    0L }		/* REAR */
																				 };	/*!< ,探索領域を設定する方向(X),?,[-], */


static slong	m_FPE_nDirX4PreciseSrchRgnAddCenter[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = { 
																						CB_IMG_CHECKBOARD_SIGN_MINUS,  /* Upper */
																						CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Lower */
																						CB_IMG_CHECKBOARD_SIGN_MINUS,  /* Left  */
																						CB_IMG_CHECKBOARD_SIGN_PLUS  /* Right */
																						};	/*!< ,探索領域を設定する方向(X),?,[-], */
static slong	m_FPE_nDirY4PreciseSrchRgnAddCenter[CB_FPE_EDGE_POS_PATTERN_MAXNUM] = {
																						CB_IMG_CHECKBOARD_SIGN_MINUS,  /* Upper */
																						CB_IMG_CHECKBOARD_SIGN_PLUS, /* Lower */
																						CB_IMG_CHECKBOARD_SIGN_PLUS,  /* Left  */
																						CB_IMG_CHECKBOARD_SIGN_MINUS  /* Right */
																						};	/*!< ,探索領域を設定する方向(Y),?,[-], */


/* デバッグ */
static CB_FPE_DBG_INFO	m_FPE_tDbgInfo;	/*!< ,デバッグ情報,-,[-], */

static IMPLIB_CNVLUT	m_FPE_nCnvLUT_EdgeAC[CB_FPE_EDGE_AC_TBL_MAXNUM][256];	/*!< ,エッジ角度コード生成用LUT,0<=value<=255,[-], */
static slong	m_FPE_nprm_EdgeCodeCoeff_H[9] = {	-1L, -2L, -1L, 
													 0L,  0L,  0L, 
													 1L,  2L,  1L };	/*!< ,エッジコード時の係数(水平エッジ用),-,[-], */
static slong	m_FPE_nprm_EdgeCodeCoeff_V[9] = {	-1L,  0L,  1L, 
													-2L,  0L,  2L, 
													-1L,  0L,  1L };	/*!< ,エッジコード時の係数(垂直エッジ用),-,[-], */
static IMPLIB_IPEdgePoint	m_FPE_tEdgeCodeTbl[CB_FPE_EDGE_CODE_TBL_MAXNUM];	/*!< ,エッジコードテーブル,-,[-], */

/********************************************************
 * エッジコード角度変換関係を他プロジェクトから引き抜き *
 ********************************************************/

/* エッジコード角度変換テーブル(12bitから7bitへの変換で使用) T.Arano 03/04/04 */
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
 * エッジコード角度変換関係を他プロジェクトから引き抜き *
 ********************************************************/

/**
 * @brief	市松Haar-like特徴量符号判定用テーブル
 */
/* テーブルのルールを下記に示す */
/* (1)CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER（中央寄り）が選択された場合は、市松の形にかかわらず、CB_IMG_ALGORITHM_DEFAULTと同様の動作とする */
/* (2)CB_IMG_PATTERN_TYPE_CHKBOARD2x2 の場合に、CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT／CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHTが選択されても、CB_IMG_ALGORITHM_DEFAULTと同様の動作とする*/
/* (3)CB_IMG_PATTERN_TYPE_CHKBOARD2x3 の場合に、CB_IMG_ALGORITHM_DEFAULT／CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTERが選択された場合、スマーナ・キャントン工場の判定（車両のFr側で判定）をデフォルトとする*/
/* (4)CB_IMG_PATTERN_TYPE_CHKBOARD2x4 の場合に、CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT／CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHTが選択されても、CB_IMG_ALGORITHM_DEFAULTと同様の動作とする*/
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
	{	/* CB_IMG_PATTERN_TYPE_CIRCLE_LINE */ /* CB_IMG_PATTERN_TYPE_CIRCLE_LINEの場合は無効にしておく */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{ CB_IMG_CHECKBOARD_SIGN_NONE,		CB_IMG_CHECKBOARD_SIGN_NONE, 			CB_IMG_CHECKBOARD_SIGN_NONE, 				CB_IMG_CHECKBOARD_SIGN_NONE },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{ CB_IMG_CHECKBOARD_SIGN_NONE,		CB_IMG_CHECKBOARD_SIGN_NONE, 			CB_IMG_CHECKBOARD_SIGN_NONE, 				CB_IMG_CHECKBOARD_SIGN_NONE },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	}
};

/**
 * @brief	特徴点位置タイプ定義テーブル
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
	{	/* CB_IMG_PATTERN_TYPE_CIRCLE_LINE */ /* CB_IMG_PATTERN_TYPE_CIRCLE_LINEの場合は無効にしておく */
	/* 		CB_IMG_ALGORITHM_DEFAULT,		          CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT,	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT,  CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_CENTER */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_POS_PLACEMENT */
		{	CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM, CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM },	/* CB_IMG_CHKBOARD_NEG_PLACEMENT */
	}
};
/* 重み付けHaar-like特徴量計算時のブロックサイズ */																																												
#define		CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MAX		(   10L )				/*!< ,修正市松パターン領域面積算出時のブロックサイズ最大値, value=10, [pixel], */																							
#define		CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MIN		(    6L )				/*!< ,修正市松パターン領域面積算出時のブロックサイズ最小値, value=6, [pixel], */																							
																																												
/**																																												
 * @brief			マーカーの傾き種別																																									
 */																																												
enum enum_CB_FPE_SLOPE_TYPE																																												
{																																												
	CB_FPE_SLOPE_TYPE_VERT = 0,									/*!< ,垂直方向,value=0,[-], */																																		
	CB_FPE_SLOPE_TYPE_HORI,									/*!< ,水平方向,value=1,[-], */																																		
	CB_FPE_SLOPE_TYPE_MAX									/*!< ,最大値,value=2,[-], */																																		
};																																												

/*Y座標位置設定テーブル*/																												
slong		tLargeAreaYPos[CB_FPE_SLOPE_TYPE_MAX][CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MAX];																										
slong		tSmallAreaYPos[CB_FPE_SLOPE_TYPE_MAX][CB_FPE_HAAR_ADD_CHK_EDGE_BLOCK_SIZE_MAX];																										


/********************************************************************
 * 内部関数　プロトタイプ宣言
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
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																													/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
static slong cb_FPE_CalcBrightnessSimilarity( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_INPUT* ptBrInput, float_t* pfSimilarity, float_t* pfContrast );
static slong cb_FPE_CalcBrightnessHistogram( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_BR_INFO* ptBrInfoTbl, CB_FPE_CBR_INPUT* ptBrInput );
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																												/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

#ifdef CB_FPE_CSW_ENABLE_TEST_MODE

static slong cb_FPE_TEST_GenerateTestData( const IMPLIB_IMGID nImgID, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, uchar nCamDirection );

#endif /* CB_FPE_CSW_ENABLE_TEST_MODE */

/* デバッグ関連 */
static slong cb_FPE_DBG_Init( void );
static slong cb_FPE_DBG_SetGridInfo( CB_FPE_DBG_GRID_INFO* ptGridInfo, const CB_FPE_CAM_PRM* const ptCamPrm, slong nCamDirection );

#ifdef CB_FPE_CSW_DEBUG_ON
static slong cb_FPE_DBG_SaveHaarLikeInfo( CB_FPE_HAAR_LIKE_FTR* ptHaarLike, schar *pFilePath );
static slong cb_FPE_DBG_SavePixelValue( IMPLIB_IMGID nImgID, schar *pFilePath );
#endif /* CB_FPE_CSW_DEBUG_ON */

static void cb_img_FtrPointExtractor_ParamInit( void );

static slong cb_FPE_CalcDistCheck( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo );

/********************************************************************
 * 外部関数　定義
 ********************************************************************/

/******************************************************************************/
/**
 * @brief		特徴点抽出処理(初期化)
 *
 * @param		なし
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.10	K.Kato			新規作成
 */
/******************************************************************************/
slong cb_img_FtrPointExtractor_Init( void )
{
	slong					nRet = CB_IMG_NG;

	if ( CB_TRUE == m_FPE_nFlagInit )
	{
		/* 多重初期化のためNG */
		return ( CB_IMG_NG_MULTIPLE_INIT );
	}

	/* 0クリア */
	memset( &m_FPE_tInfo, 0x00, sizeof( CB_FPE_INFO ) );
	memset( &m_FPE_tWorkInfo, 0x00, sizeof( CB_FPE_WORK_INFO ) );
	memset( &m_FPE_tPrm, 0x00, sizeof( CB_FPE_PRM ) );

	/* 外部パラメータ値の取得 */
	nRet = cb_img_ParamInit();
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* 外部パラメータの初期化 */
	cb_img_FtrPointExtractor_ParamInit();
	
	/* 高精度位置決めと探索領域情報有効フラグの初期化 */
	cb_img_FtrPointExtractor_Reinit();

	/* メモリ確保 */
	nRet = cb_FPE_AllocImg( &(m_FPE_tInfo.nImg[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_ALLOC_IMG );
	}

	/* カメラ幾何モジュールの初期化 */
	nRet = cb_CG_Init();
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG_CG_INIT );
	}

	/* 各種フラグ設定 */
	/* 初期化フラグ */
	m_FPE_nFlagInit = CB_TRUE;

	/* ヒストグラム類似度のMAX値 */
	m_FPE_fSimilarityMAX = sqrtf( 2.0f );

	/* 角度コード変換テーブルの設定 */
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

	/* 処理時間格納用メモリ */
#ifdef TIME_MEASUREMENT_CALIB
	memset( &(RG_Tim_CALIB[0L]), 0x00, ( 30L * sizeof( RG_Time_Measure ) ) );
#endif /* TIME_MEASUREMENT_CALIB */
	
	return ( CB_IMG_OK );
}



/******************************************************************************/
/**
 * @brief			外部パラメータの取得と初期化関数（外部向け）
 *
 * @param[in,out]	なし			:,,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.12.20	F.Sano	新規作成
 *
 */
/******************************************************************************/
slong cb_img_FtrPointExtractor_MMParam_Init( void )
{
	slong	nRet = CB_IMG_NG;

	/* 外部パラメータ値の取得 */
	nRet = cb_img_ParamInit();
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* 外部パラメータの初期化 */
	cb_img_FtrPointExtractor_ParamInit();
	
	/* 高精度位置決めと探索領域情報有効フラグの再初期化 */
	cb_img_FtrPointExtractor_Reinit();

	return ( CB_IMG_NG_MM_OBJECT_READ );
}


/******************************************************************************/
/**
 * @brief			外部パラメータ値取得関数
 *
 * @param[in,out]	なし			:,,-,[-],
 *
 * @retval			なし			:,正常終了,value=0,[-],
 *
 * @date			2013.12.06	F.Sano	新規作成
 *
 */
/******************************************************************************/
static void cb_img_FtrPointExtractor_ParamInit( void )
{

	/* 共有OBJから取得したパラメータ値をセット */
	CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_Width();
	CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_Height();
	CB_FPE_PRM_HALF_WIDTH_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_HalfWidth();
	CB_FPE_PRM_HALF_HEIGHT_PRECISE_SRCH_RGN = (slong)cb_img_GetParam_PreciseSrchRgn_HalfHeight();
	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_X = (slong)cb_img_GetParam_PreciseSrchRgn_DistanceBasePnt_X();
	CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_Y = (slong)cb_img_GetParam_PreciseSrchRgn_DistanceBasePnt_Y();

	/* 取得したパラメータ値を利用する値をセット */
	/* 左右市松高精度位置決め、エッジ点探索回数 */
//	CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN = CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN / 2L;
	CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN = 2L;

	return;
}


/******************************************************************************/
/**
 * @brief		特徴点抽出処理(再初期化)
 *
 * @param		なし
 *
 * @retval		なし
 *
 * @date		2013.12.12	F.Sano			新規作成
 *
 * @note		初期化処理のうち、「高精度位置決め」と、「探索領域情報有効フラグ」の再初期化を行う
 *				本変数の初期化は車両パラメータが更新されたタイミングで初期化したいため、
 *				起動時にのみ初期化する特徴点抽出処理(初期化)とは別途作成。
 */
/******************************************************************************/

static void cb_img_FtrPointExtractor_Reinit( void )
{
	slong					ni = 0L;
	CB_FPE_IMG_ACCESS_INFO	*ptImgAccessInfo = NULL;
	/* パラメータ */
	slong	nprm_WidthSrchRgn = 0L, 
			nprm_HeightSrchRgn = 0L;

	/* パラメータ取得 */
	nprm_WidthSrchRgn = CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN;
	nprm_HeightSrchRgn = CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN;

	/* 高精度位置決めの再設定 */
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

	/* 探索領域情報有効フラグ */
	for ( ni = 0L; ni < E_CB_SELFCALIB_CAMPOS_MAX; ni++ )
	{
		( (m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam )[ni].nFlagValidInfo4Pattern		= CB_FALSE;
		( (m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam )[ni].nFlagValidInfo4AddPattern	= CB_FALSE;
	}

	return;
}

/******************************************************************************/
/**
 * @brief		探索領域の設定
 *
 * @param[in]	ptPatternInfo	:,特徴パターン情報構造体へのポインタ,-,[-],
 * @param[in]	ptCamPrm		:,カメラパラメータ構造体へのポインタ,-,[-],
 * @param[in]	ptInfo			:,特徴点抽出入力情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.20	K.Kato			新規作成
 */
/******************************************************************************/
slong cb_img_SetSearchRegion( const CB_IMG_PATTERN_INFO* const ptPatternInfo, const CB_IMG_CAMERA_PRM* const ptCamPrm, const CB_IMG_INFO* const ptInfo )
{
	slong	nRet = CB_IMG_NG;
	slong	nCamDirection = E_CB_SELFCALIB_CAMPOS_MAX;

	/* 引数チェック */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* カメラ方向種別 */
	nCamDirection = ptInfo->nCamDirection;

	/* カメラ方向の範囲チェック */
	if ( ( nCamDirection < E_CB_SELFCALIB_CAMPOS_FRONT ) || ( E_CB_SELFCALIB_CAMPOS_MAX <= nCamDirection   ) )
	{
		return ( CB_IMG_NG_INVALID_CAMDIRECTION );
	}

	/* 探索領域情報有効フラグ */
	/* cb_img_SetSearchRegion()の開始部分で無効状態にし、正常終了時点で有効状態にする */
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4Pattern	= CB_FALSE;
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4AddPattern	= CB_FALSE;

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SET_SRCH_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* カメラパラメータ設定 */
	nRet = cb_FPE_SetCameraParam( ptCamPrm, &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		if ( 0L < m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].nCamID )
		{
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		}
		return ( CB_IMG_NG_SET_CAM_PRM );
	}

	/* カメラパラメータ、誤差量設定 */
	nRet = cb_FPE_SetCameraErrParam( ptCamPrm, &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_CAM_ERR_PRM );
	}

	/* 市松の探索領域設定 */
	/* 探索領域(World)設定 */
	nRet = cb_FPE_SetSearchRegionWorld( ptPatternInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_RGN_WLD_CHK_PATTERN );
	}

	/* 探索領域(Image)設定 */
	nRet = cb_FPE_SetSearchRegionImg( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_CNV_W2I_RGN_CHK_PATTERN );
	}

	/* 探索領域設定 */
	nRet = cb_FPE_SetSearchRegion( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[0]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_RGN_CHK_PATTERN );
	}

	/* 追加パターンの探索領域設定 */ /* 将来的に追加パターンは左右個別に判定して作成する */
	/* 追加パターンの判別 */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfo->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfo->nTypeRightPattern ) )
	{
		/* 左：なし　　右：なし　　：*/
		/* NO OPERATION */
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfo->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfo->nTypeRightPattern ) )
	{
		/* 左：線丸　　右：線丸　　：*/

		/* 探索領域(World)設定(追加パターン) */
		nRet = cb_FPE_SetSearchRegionWorld4AddPattern( ptPatternInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_LINE_CIRC_PATTERN );
		}

		/* 探索領域(Image)設定(追加パターン) */
		nRet = cb_FPE_SetSearchRegionImg4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_LINE_CIRC_PATTERN );
		}

		/* 探索領域設定(追加パターン) */
		nRet = cb_FPE_SetSearchRegion4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_LINE_CIRC_PATTERN );
		}
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfo->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfo->nTypeRightPattern ) )
	{
		/* 左：市松(2x2) 右：市松(2x2) ：*/
		/* 探索領域(World)設定(追加パターン) */
		nRet = cb_FPE_SetSearchRegionWorld4AddChkboard( ptPatternInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), nCamDirection );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_ADD_CHK_PATTERN );
		}

		/* 探索領域(Image)設定(追加パターン) */
		nRet = cb_FPE_SetSearchRegionImg4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_ADD_CHK_PATTERN );
		}

		/* 探索領域設定(追加パターン) */
		nRet = cb_FPE_SetSearchRegion4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_ADD_CHK_PATTERN );
		}
	}
	else
	{
		/* 左右のパターンが異なる場合 */
		/* 現状ではありえないパターンなので、現状はエラーとする。将来的には対応する。  */
		return ( CB_IMG_NG_COMBI_ADD_PATTERN );
	}

	/* [DEBUG] */
	/* グリッド情報の設定 */
	nRet = cb_FPE_DBG_SetGridInfo( &(m_FPE_tDbgInfo.tGridInfo[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG );
	}

	/* カメラパラメータの削除 */
	nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_DEL_CAM_PRM );
	}

	/* 探索領域情報有効フラグ */
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4Pattern	= CB_TRUE;
	m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4AddPattern	= CB_TRUE;

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SET_SRCH_RGN] );
#endif /* TIME_MEASUREMENT_CALIB */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		探索領域の設定(左右ターゲット)
 *
 * @param[in]	ptPatternInfoAdd	:,特徴パターン情報構造体へのポインタ,-,[-],
 * @param[in]	ptCamPrmAdd			:,カメラパラメータ構造体へのポインタ,-,[-],
 * @param[in]	ptInfoAdd			:,特徴点抽出入力情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.09.02	S.Morita		新規作成
 * @date		2014.12.01	S.Morita		コメント追記
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
	slong							ret_estimate_1st = E_CB_SELFCALIB_NG;	/* 1回目の推定結果 */
	slong							ret_check_1st = E_CB_SELFCALIB_NG;		/* 1回目の推定値の範囲チェック結果 */	
	CB_RECOG_CENT_PAT_LAYOUT_INFO	CCentPatLayoutInfo;						/* 中央パターン配置情報 */

	/* 特徴点抽出可能最大数 */
	ulong							ulCentEssNumMax = 0UL;					/* 中央必須点の抽出可能最大数 */
	ulong							ulCentOtherNumMax = 0UL;				/* 中央その他点の抽出可能最大数 */

	/* 引数チェック */
	if ( ( NULL == ptPatternInfoAdd ) || ( NULL == ptCamPrmAdd ) || ( NULL == ptInfoAdd ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* カメラパラメータをコピー */
	camPrmAdd = *ptCamPrmAdd;

	/* パラメータ取得 */
	/* カメラパラメータの各種許容誤差情報 */
	ptErrInfoSrcAdd = &( camPrmAdd.tErrInfo );

	/* 外部パラメータ推定処理に使用する構造体を初期化 */
	memset(&estimateCoordinates, 0L, sizeof(CB_EST_PARAM_COORD));

	/* カメラ方向種別 */
	nCamDirection = ptInfoAdd->nCamDirection;

	/* カメラ方向の範囲チェック */
	if ( ( nCamDirection < E_CB_SELFCALIB_CAMPOS_FRONT ) || ( E_CB_SELFCALIB_CAMPOS_MAX <= nCamDirection   ) )
	{
		return ( CB_IMG_NG_INVALID_CAMDIRECTION );
	}

	/* セルフキャリブ情報構造体にデータを格納  (外部パラメータ推定のための処理)*/
	switch ( nCamDirection )
	{
		case E_CB_SELFCALIB_CAMPOS_FRONT:
			/* フロントカメラ */
			camPos = E_CB_SELFCALIB_CAMPOS_FRONT;
			mapType = E_CB_MAPDATA_TYPE_TOP_FRONT;
			break;
		case E_CB_SELFCALIB_CAMPOS_LEFT:
			/* 左側カメラ */
			camPos = E_CB_SELFCALIB_CAMPOS_LEFT;
			mapType = E_CB_MAPDATA_TYPE_TOP_LEFT;
			break;
		case E_CB_SELFCALIB_CAMPOS_RIGHT:
			/* 右側カメラ */
			camPos = E_CB_SELFCALIB_CAMPOS_RIGHT;
			mapType = E_CB_MAPDATA_TYPE_TOP_RIGHT;
			break;
		case E_CB_SELFCALIB_CAMPOS_REAR:
			/* リアカメラ */
			camPos = E_CB_SELFCALIB_CAMPOS_REAR;
			mapType = E_CB_MAPDATA_TYPE_TOP_REAR;
			break;
		default:
			/* defaultの場合はカメラ方向の範囲チェックエラーであるため、処理なし */
			break;
	}

	/* 外部パラメータ推定のため、セルフキャリブ構造体に値を設定 */
	nRet = cb_FPE_SettingForEstimate(&selfCalibInfo, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[CB_PATTERN_POS_CENTER]));
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 中央必須点・中央その他点の取得上限個数を設定する */
	/* 「selfCalibInfo.centerTargetPatternType」は"uchar"型ですが、実際には"CALIB_CHECKBOARD_PAT_TYPE"型の値が入っています */
	nRet = cb_recog_getCentPointNumMax(&ulCentEssNumMax, &ulCentOtherNumMax, selfCalibInfo.centerTargetPatternType);
	if(E_CB_RECOG_OK != nRet)
	{
		return ( CB_IMG_NG );
	}

	/* 中央パターン配置情報初期化 */
	memset(&CCentPatLayoutInfo, 0L, sizeof(CB_RECOG_CENT_PAT_LAYOUT_INFO));

	/* 中央パターン配置情報をCB_SELF_CALIBから取得 */
	nRet = cb_selfCalib_getCentPatLayoutInfoFromCbSelfCalib(&(CCentPatLayoutInfo), &selfCalibInfo, camPos, (ulCentEssNumMax + ulCentOtherNumMax));
	if(E_CB_RECOG_OK != nRet)
	{
		return ( CB_IMG_NG );
	}

	/* 中央必須点を５点取得出来ているかを判定 */
	nRet = cb_FPE_CheckCenterEssCount(&CCentPatLayoutInfo, ulCentEssNumMax, ulCentOtherNumMax);
	if( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 共有オブジェクトから実カメラパラメータ情報を取得 (外部パラメータ推定のための処理) */
	nRet = cb_getShare_RealCamParam( camPos, &camParamReal );
	if( E_CB_SHARE_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* 共有オブジェクトから仮想カメラパラメータ情報を取得 (外部パラメータ推定のための処理) */
	nRet = cb_getShare_VirtualCamParam( mapType, &camParamVirtual );
	if( E_CB_SHARE_OK != nRet )
	{
		return ( CB_IMG_NG_MM_OBJECT_READ );
	}

	/* 中央市松特徴点データの格納 (外部パラメータ推定のための処理) */ 
	nRet = cb_FPE_StoreResultCenterData( &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection]), &selfCalibInfo, &( ((m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam)[nCamDirection] ), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 左右ターゲットは使用しない */
	selfCalibInfo.sideTargetNumDef = 0UL;

	/* 外部カメラパラメータ推定に用いる情報を格納する  (外部パラメータ推定のための処理) */
	nRet = cb_selfCalib_getEstimateLogicKind(&EstimateLogicKind, &estimateCoordinates, &selfCalibInfo, camPos);
	if( E_CB_SELFCALIB_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 外部カメラパラメータ推定 */
	ret_estimate_1st = 	cb_selfCalib_estimateCamParam_ExtOnly(&estimateCoordinates, &camParamReal, &camParamVirtual, &camEstParamReal, &camEstParamVirtual);
	if( E_CB_SELFCALIB_OK == ret_estimate_1st )
	{
		/* パラメータ推定範囲チェック */
		ret_check_1st = cb_checkEstimatResult(&camEstParamReal, &camEstParamVirtual, &camParamReal, &camParamVirtual);
	}

	if((ret_estimate_1st != E_CB_SELFCALIB_OK) || (ret_check_1st != E_CB_SELFCALIB_OK))
	{
		/* 中央その他点を無効値に設定する */
		nRet = cb_FPE_InvalidSetting(&selfCalibInfo, &CCentPatLayoutInfo, ulCentEssNumMax, ulCentOtherNumMax);
		if( CB_IMG_OK != nRet )
		{
			/* 再推定が正常終了しなかった場合は、既存処理で設定された探索領域を用いる */
			return ( CB_IMG_NG );
		}

		/* 中央その他点を無効値に設定しているため、中央必須点５点が格納される */
		nRet = cb_selfCalib_getEstimateLogicKind(&EstimateLogicKind, &estimateCoordinates, &selfCalibInfo, camPos);
		if( E_CB_SELFCALIB_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* 外部カメラパラメータ推定 */
		nRet = 	cb_selfCalib_estimateCamParam_ExtOnly(&estimateCoordinates, &camParamReal, &camParamVirtual, &camEstParamReal, &camEstParamVirtual);
		if( E_CB_SELFCALIB_OK != nRet )
		{
			/* 再推定が正常終了しなかった場合は、既存処理で設定された探索領域を用いる */
			return ( CB_IMG_NG );
		}

		/* パラメータ推定範囲チェック */
		nRet = cb_checkEstimatResult(&camEstParamReal, &camEstParamVirtual, &camParamReal, &camParamVirtual);
		if ( E_CB_SELFCALIB_OK != nRet)
		{
			/* 再推定結果が範囲外である場合は、既存処理で設定された探索領域を用いる */
			return ( CB_IMG_NG );
		}
	}

	/* カメラパラメータ設定 */
	nRet = cb_FPE_SetCameraParam( &camPrmAdd, &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		if ( 0L < m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].nCamID )
		{
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		}
		return ( CB_IMG_NG_SET_CAM_PRM );
	}

	//* 外部カメラパラメータ推定、または再推定が正常終了した場合は、許容誤差を更新する */
	ptErrInfoSrcAdd->dPitchDeg = CB_FPE_PRM_ERR_DPITCHDEG;	/* ピッチ角誤差 */
	ptErrInfoSrcAdd->dRollDeg = CB_FPE_PRM_ERR_DROLLDEG;	/* ロール角誤差 */
	ptErrInfoSrcAdd->dYawDeg = CB_FPE_PRM_ERR_DYAWDEG;		/* ヨー角誤差 */
	ptErrInfoSrcAdd->dShiftX = CB_FPE_PRM_ERR_DSHIFTX;		/* カメラ位置X座標誤差 */
	ptErrInfoSrcAdd->dShiftY = CB_FPE_PRM_ERR_DSHIFTY;		/* カメラ位置Y座標誤差 */
	ptErrInfoSrcAdd->dShiftZ = CB_FPE_PRM_ERR_DSHIFTZ;		/* カメラ位置X座標誤差 */
	ptErrInfoSrcAdd->dPosHori = CB_FPE_PRM_ERR_DPOSHORI;	/* 水平光軸位置誤差 */
	ptErrInfoSrcAdd->dPosVert = CB_FPE_PRM_ERR_DPOSVERT;	/* 垂直光軸位置誤差 */

	/* カメラパラメータ構造体に推定結果を格納する */
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dShiftX = camEstParamReal.px;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dShiftY = camEstParamReal.py;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dShiftZ = camEstParamReal.pz;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dPitchDeg = camEstParamReal.alpha;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dRollDeg = camEstParamReal.gamma;
	m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection].tExtrinsicPrm.dYawDeg = camEstParamReal.beta;

	/* カメラパラメータ、誤差量設定 */
	nRet = cb_FPE_SetCameraErrParam( &camPrmAdd, &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_SET_CAM_ERR_PRM );
	}

	/* 左右ターゲットの探索領域設定 */ /* 将来的に左右ターゲットは左右個別に判定して作成する */
	/* 左右ターゲットの判別 */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfoAdd->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_NONE == ptPatternInfoAdd->nTypeRightPattern ) )
	{
		/* 左：なし　　右：なし　　：*/
		/* NO OPERATION */
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfoAdd->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == ptPatternInfoAdd->nTypeRightPattern ) )
	{
		/* 左：線丸　　右：線丸　　：*/

		/* 探索領域(World)設定(左右ターゲット) */
		nRet = cb_FPE_SetSearchRegionWorld4AddPattern( ptPatternInfoAdd, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_LINE_CIRC_PATTERN );
		}

		/* 探索領域(Image)設定(左右ターゲット) */
		nRet = cb_FPE_SetSearchRegionImg4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_LINE_CIRC_PATTERN );
		}

		/* 探索領域設定(左右ターゲット) */
		nRet = cb_FPE_SetSearchRegion4AddPattern( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_LINE_CIRC_PATTERN );
		}
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfoAdd->nTypeLeftPattern ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == ptPatternInfoAdd->nTypeRightPattern ) )
	{
		/* 左：市松(2x2) 右：市松(2x2) ：*/
		/* 探索領域(World)設定(左右ターゲット) */
		nRet = cb_FPE_SetSearchRegionWorld4AddChkboard( ptPatternInfoAdd, &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), nCamDirection );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_WLD_ADD_CHK_PATTERN );
		}

		/* 探索領域(Image)設定(左右ターゲット) */
		nRet = cb_FPE_SetSearchRegionImg4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamErrInfo[nCamDirection]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_CNV_W2I_RGN_ADD_CHK_PATTERN );
		}

		/* 探索領域設定(左右ターゲット) */
		nRet = cb_FPE_SetSearchRegion4AddChkboard( &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]) );
		if ( CB_IMG_OK != nRet )
		{
			/* カメラパラメータの削除 */
			nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
			return ( CB_IMG_NG_SET_RGN_ADD_CHK_PATTERN );
		}
	}
	else
	{
		/* 左右ターゲットの形状が異なる場合 */

		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG_COMBI_ADD_PATTERN );
	}

	/* [DEBUG] */
	/* グリッド情報の設定 */
	nRet = cb_FPE_DBG_SetGridInfo( &(m_FPE_tDbgInfo.tGridInfo[nCamDirection]), &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		/* カメラパラメータの削除 */
		nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
		return ( CB_IMG_NG );
	}

	/* カメラパラメータの削除 */
	nRet = cb_FPE_DeleteCameraParam( &(m_FPE_tInfo.tCtrl.tCamPrm[nCamDirection]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_DEL_CAM_PRM );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		中央市松特徴点データの格納
 *
 * @param[in]	ptSrchRsltInfo		:,探索結果情報構造体へのポインタ,-,[-],
 * @param[out]	ptselfCalibInfo		:,特徴点抽出結果を格納するセルフキャリブ結果情報構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgn			:,探索領域情報構造体へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.09.03	S.Morita		新規作成
 *
 * @note		引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する
 *
 */
/******************************************************************************/
static slong cb_FPE_StoreResultCenterData( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo, CB_SELF_CALIB* ptselfCalibInfo, const CB_FPE_SRCH_RGN_INFO_EACH_CAM* const ptSrchRgn, slong nCamDirection )
{
	slong							nRet = CB_IMG_OK;
	CB_FPE_SRCH_RSLT_PNT_INFO		*ptSrchRsltPnt = NULL;
	enum enum_CB_IMG_ALGORITHM		nTypeAlgorithm = CB_IMG_ALGORITHM_DEFAULT;
	
	/* 引数チェック */
	if ( ( NULL == ptSrchRsltInfo ) || ( NULL == ptselfCalibInfo ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 中央市松の特徴点座標を格納している構造体にポインタを設定 */
	ptSrchRsltPnt = &(ptSrchRsltInfo->tSrchRsltPntInfo[0]);
	
	/* フロントカメラ、またはリアカメラの場合 */
	if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
	{
		/* 9点の特徴点座標をptselfCalibInfoに格納する */
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
	/* 左側カメラ、または右側カメラの場合 */
	else if ( ( E_CB_SELFCALIB_CAMPOS_LEFT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_RIGHT == nCamDirection ) )
	{
		/* 中央市松が"2x3の市松"以外の形状である場合 */
		if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ptSrchRgn->tSrchRgnInfo4Pattern[0].tPtnInfo.nTypePattern )
		{
			/* 7点の特徴点座標をptselfCalibInfoに格納する */
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
			/* アルゴリズムタイプの判定 */
			nRet = cb_FPE_GetTypeAlgorithm( &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ), &nTypeAlgorithm );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
			
			/* アルゴリズムタイプが"左寄りの特徴点抽出アルゴリズム"である場合 */
			if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
			{
				/* 市松パターンの向きが"正の向き(市松模様の最左下の色が白)"である場合 */
				if ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8点の特徴点座標をptselfCalibInfoに格納する */
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
				/* 市松パターンの向きが"負の向き(市松模様の最左下の色が黒)"である場合 */
				else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8点の特徴点座標をptselfCalibInfoに格納する */
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
				/* 市松パターンの向きが"正の向き"でも"負の向き"でもない場合 */
				else
				{
					return ( CB_IMG_NG );
				}
			}
			/* アルゴリズムタイプが"右寄りの特徴点抽出アルゴリズム"である場合 */
			else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
			{
				/* 市松パターンの向きが"正の向き(市松模様の最左下の色が白)"である場合 */
				if ( CB_IMG_CHKBOARD_POS_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8点の特徴点座標をptselfCalibInfoに格納する */
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
				/* 市松パターンの向きが"負の向き(市松模様の最左下の色が黒)"である場合 */
				else if ( CB_IMG_CHKBOARD_NEG_PLACEMENT == ( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tPtnInfo ).nFlagPlacement )
				{
					/* 8点の特徴点座標をptselfCalibInfoに格納する */
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
				/* 市松パターンの向きが"正の向き"でも"負の向き"でもない場合 */
				else
				{
					return ( CB_IMG_NG );
				}
			}
			/* アルゴリズムタイプが"左寄りの特徴点抽出アルゴリズム"でも"右寄りの特徴点抽出アルゴリズム"でもない場合 */
			else
			{
				return ( CB_IMG_NG );
			}
		}
	}
	/* フロントカメラ・リアカメラ・左側カメラ・右側カメラのどれでも無い場合 */
	else
	{
		/* 何もしない */
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		中央その他点の無効値設定
 *
 * @param[in,out]	ptselfCalibInfo			:,セルフキャリブ情報構造体,-,[-],
 * @param[in]		ptCCentPatLayoutInfo	:,中央特徴パターン配置情報,-,[-],
 * @param[in]		ulCentEssNum			:,中央必須点の個数,-,[-],
 * @param[in]		ulCentOtherNum			:,中央その他点の個数,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG			:,異常終了,value=-1,[-],
 *
 * @date		2014.11.28	S.Morita		新規作成
 * @date		2014.12.01	S.Morita		引数の修正
 *
 * @note		
 *
 */
/******************************************************************************/
static slong cb_FPE_InvalidSetting(CB_SELF_CALIB *ptselfCalibInfo, CB_RECOG_CENT_PAT_LAYOUT_INFO *ptCCentPatLayoutInfo,  const ulong ulCentEssNum,  const ulong ulCentOtherNum)
{
	ulong	targetCount;							/* ループ用 */
	slong	nRet = E_CB_RECOG_NG;
	BOOL	bIsCentEssPoint = FALSE;				/* 中央必須点か否か */
	ulong	ulCenterOhterNgNum = 0UL;				/* 中央その他点であり、無効値である特徴点の個数 */

	/* 引数チェック */
	if ( ( NULL == ptselfCalibInfo ) || ( NULL == ptCCentPatLayoutInfo ) )
	{
		return ( CB_IMG_NG );
	}

	for(targetCount = 0UL; targetCount < (ulCentEssNum+ulCentOtherNum); targetCount++)
	{
		/* 中央必須点か否かを取得 */
		nRet = cb_recog_isCentEssPoint(&bIsCentEssPoint, ptCCentPatLayoutInfo, targetCount);
		if(E_CB_RECOG_OK != nRet)
		{
			return ( CB_IMG_NG );
		}
		else if(FALSE == bIsCentEssPoint)
		{	/* その他点 */
			if(TRUE == cb_recog_isValidPickupResult(ptselfCalibInfo->centerTarget[targetCount].px, ptselfCalibInfo->centerTarget[targetCount].py))
			{
				/* 中央その他点を無効値に設定する */
				ptselfCalibInfo->centerTarget[targetCount].px = -1.0;
				ptselfCalibInfo->centerTarget[targetCount].py = -1.0;
			}
			else
			{
				ulCenterOhterNgNum++;
			}
		}
		else{	/* 必須 */
			/* 何もしない */
		}
	}

	/* １度目の推定を行った際に中央その他点が全て無効値であった場合は、特徴点が変わらないので、再推定は行わずに終了。 */
	if( ulCenterOhterNgNum == ulCentOtherNum )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		中央必須点の取得個数判定
 *
 * @param[in]		ptCCentPatLayoutInfo	:,中央特徴パターン配置情報,-,[-],
 * @param[in]		ulCentEssNum			:,中央必須点の個数,-,[-],
 * @param[in]		ulCentOtherNum			:,中央その他点の個数,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG			:,異常終了,value=-1,[-],
 *
 * @date		2014.11.28	S.Morita		新規作成
 * @date		2014.12.01	S.Morita		引数の修正等
 *
 * @note		
 *
 */
/******************************************************************************/
static slong cb_FPE_CheckCenterEssCount(CB_RECOG_CENT_PAT_LAYOUT_INFO *ptCCentPatLayoutInfo, const ulong ulCentEssNum, const ulong ulCentOtherNum)
{
	ulong			targetCount;
	slong			nRet = E_CB_RECOG_NG;
	ulong			centerEsscount = 0UL;					/* 中央必須点の個数 */
	BOOL			bIsCentEssPoint = FALSE;				/* 中央必須点か否か */

	/* 引数チェック */
	if( NULL == ptCCentPatLayoutInfo )
	{
		return ( CB_IMG_NG );
	}

	for(targetCount = 0UL; targetCount < (ulCentEssNum+ulCentOtherNum); targetCount++)
	{
		/* 中央必須点か否かを取得 */
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

	/* 中央必須点５点を取得できていない場合は処理を終了する */
	if( ulCentEssNum != centerEsscount )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		外部パラメータ推定のため、セルフキャリブ構造体に値を設定する
 *
 * @param[in,out]	ptselfCalibInfo			:,セルフキャリブ情報構造体,-,[-],
 * @param[in]		ptSrchRgnInfo			:,探索領域情報構造体,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG			:,異常終了,value=-1,[-],
 *
 * @date		2014.11.28	S.Morita		新規作成
 * @date		2014.12.01	S.Morita		引数の修正等
 *
 * @note		
 *
 */
/******************************************************************************/
static slong cb_FPE_SettingForEstimate(CB_SELF_CALIB *ptselfCalibInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo)
{
	ulong	i = 0UL;

	/* 引数チェック */
	if( (NULL == ptselfCalibInfo) || (NULL == ptSrchRgnInfo) )
	{
		return ( CB_IMG_NG );
	}

	/* 市松ターゲット中心座標を取得する */
	ptselfCalibInfo->centerTargetNumDef = ptSrchRgnInfo->nNumCenterPnt;
	for( i = 0UL; i < (ptselfCalibInfo->centerTargetNumDef); i++ )
	{
		ptselfCalibInfo->centerTargetDef[i].px = ptSrchRgnInfo->tCenterPntWld[i].dX;
		ptselfCalibInfo->centerTargetDef[i].py = ptSrchRgnInfo->tCenterPntWld[i].dY;
		ptselfCalibInfo->centerTargetDef[i].pz = ptSrchRgnInfo->tCenterPntWld[i].dZ;
	}

	/* 中央(格子)パターンタイプ */
	ptselfCalibInfo->centerTargetPatternType = ptSrchRgnInfo->tPtnInfo.nTypePattern;			/* "enum_CB_IMG_PATTERN_TYPE"型をキャストしています。0～5までしか取らないので問題なし */

	/* オフセットタイプ(中央/右寄せ/左寄せ等) */
	ptselfCalibInfo->centerTargetPointOffset = ptSrchRgnInfo->tPtnInfo.nCenterOffsetType;

	/* 正像/鏡像 */
	ptselfCalibInfo->centerTargetPlacement = ptSrchRgnInfo->tPtnInfo.nFlagPlacement;			/* "enum_CB_IMG_CHKBOARD_PLACEMENT"型をキャストしています。0～2までしか取らないので問題なし */

	/* レイアウト種別 */
	ptselfCalibInfo->layoutType = ptSrchRgnInfo->tPtnInfo.nlayoutType;

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief		特徴点抽出処理(本体)
 *
 * @param[in]	nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptResult		:,特徴点抽出出力情報構造体へのポインタ,-,[-],
 * @param[in]	ptInfo			:,特徴点抽出入力情報構造体へのポインタ,-,[-],
 * @param[in]	ptPatternInfo	:,特徴パターン情報構造体へのポインタ,-,[-],
 * @param[in]	ptCamPrm		:,カメラパラメータ構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.10	K.Kato			新規作成
 * @date		2013.11.15	F.Sano			追加パターンの判別処理追加
 * @date		2015.09.14	S.Morita		ピクセル精度・サブピクセル精度座標値間の直線距離を用いた判定を追加
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
	/* パラメータ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	enum enum_CB_IMG_PATTERN_TYPE		nPtnType_Left = CB_IMG_PATTERN_TYPE_NONE ,
										nPtnType_Right = CB_IMG_PATTERN_TYPE_NONE;

	/* 引数チェック */
	if ( ( NULL == ptResult ) || ( NULL == ptInfo ) || ( NULL == ptPatternInfo ) || ( NULL == ptCamPrm ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_IMG_TOTAL] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* パラメータ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* カメラ方向 */ 
	nCamDirection = ptInfo->nCamDirection;
	/* カメラ方向の範囲チェック */
	if ( ( nCamDirection < E_CB_SELFCALIB_CAMPOS_FRONT ) || ( E_CB_SELFCALIB_CAMPOS_MAX <= nCamDirection ) )
	{
		return ( CB_IMG_NG_INVALID_CAMDIRECTION );
	}

	/* キャリブ種別 */
	nCalibType = ptInfo->nCalibType;

	/* 実行条件チェック */
	nFlagExec = cb_FPE_CheckExecCondition( m_FPE_nFlagInit, &( ( (m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam )[nCamDirection] ) );
	if ( CB_TRUE != nFlagExec )
	{
		return ( CB_IMG_NG_CHK_EXEC_CONDITION );
	}

	/* 結果クリア */
	nRet = cb_FPE_ClearResult( ptResult );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_CLEAR_RESULT );
	}

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
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

	/* 縮小画像 */
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
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMG_ZOOM_OUT );
	}

	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, 0L, 0L, 719L, 479L );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, 0L, 0L, 719L, 479L );
	nRet = implib_IP_ZoomOutExt( nImgSrc, m_FPE_tInfo.nImg[CB_FPE_IMG_NORMAL], 1L, 1L );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
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

	/* Checkerboardにおける特徴点抽出 */
	nRet = cb_FPE_ExtractFtrPoint4Checkerboard( nImgSrc, &m_FPE_tInfo, nCamDirection, nCalibType );
	if ( CB_IMG_OK != nRet )
	{
		/* 内部関数のエラーコードをそのまま返すこと!(エラー状態を詳細に切り分けるため) */
		return ( nRet );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_CHECKERBOARD_TOTAL] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_CIRCLE_LINE_TOTAL] );
#endif /* TIME_MEASUREMENT_CALIB */

#ifdef CB_FPE_SEARCH_REGION_NARROW
	/* 探索範囲の設定(左右ターゲット) */
	/* 正常終了しない場合は既に設定されている探索範囲を用いるため、戻り値の判定は行わない */
	nRet = cb_FPE_SetSearchRegionAddPattern(ptPatternInfo, ptCamPrm, ptInfo);
#endif /* CB_FPE_SEARCH_REGION_NARROW */

	/* パターンのTypeを取得 */
	nPtnType_Left = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT].tPtnInfo.nTypePattern;
	nPtnType_Right = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT].tPtnInfo.nTypePattern;

	/* 追加パターンの探索領域設定 */ /* 将来的に追加パターンは左右個別に判定して作成する */
	/* 追加パターンの判別 */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Right ) )
	{
		/* 左：なし　　右：なし　　：*/
		/* NO OPERATION */
		nRet = CB_IMG_OK;
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Right ) )
	{
		/* 左：線丸　　右：線丸　　：*/
		/* 円と直線から構成されるパターンにおける特徴点抽出 */
		nRet = cb_FPE_ExtractFtrPoint4CircleLine( nImgSrc, &m_FPE_tInfo, nCamDirection, nCalibType );

	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Right ) )
	{
		/* 左：市松(2x2) 右：市松(2x2) ：*/
		/* 左右市松パターン特徴点抽出 */
		nRet = cb_FPE_ExtractFtrPoint4AddChk( nImgSrc, &m_FPE_tInfo, nCamDirection, nCalibType );
		if ( CB_IMG_OK != nRet )
		{
			/* 内部関数のエラーコードをそのまま返すこと!(エラー状態を詳細に切り分けるため) */
			return ( nRet );
		}

		/* 左右マーカーのピクセル精度座標値とサブピクセル精度座標値の直線距離を用いた判定 */
		nRet = cb_FPE_CalcDistCheck( &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection]) );

	}
	else
	{
		/* 左右のパターンが異なる場合 */
		/* 本来ではありえないパターンなので、これが発生した場合はエラーを返す  */
		return( CB_IMG_NG_COMBI_ADD_PATTERN );
	}
	
	if ( CB_IMG_OK != nRet )
	{
		/* 内部関数のエラーコードをそのまま返すこと!(エラー状態を詳細に切り分けるため) */
		return ( nRet );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_CIRCLE_LINE_TOTAL] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_STORE_RESULT] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* 出力データ格納 */
	nRet = cb_FPE_StoreResultData( &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection]), &(ptResult->tFtrPoint), &( ((m_FPE_tInfo.tCtrl).tSrchRgnInfo4Cam)[nCamDirection] ), nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_STORE_RESULT_DATA );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_STORE_RESULT] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
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
 * @brief		探索領域(市松に対する)情報取得
 *
 * @param[out]	ptSrchRgn			:,領域構造体へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.17	K.Kato			新規作成
 *
 * @note		デバッグ用の取得関数のため、本関数内での引数nCamDirectionの範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
slong cb_img_getSearchRgnInfo4Chkboard( CB_RECT_RGN* ptSrchRgn, slong nCamDirection )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6707
{
	slong	ni = 0L;
	slong	nPatternPosMaxNum = 0L;
	CB_FPE_SRCH_RGN_INFO_EACH_CAM	*ptSrchRgnInfo = NULL;

	/* 引数チェック */
	if ( NULL == ptSrchRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パターン数 */
	nPatternPosMaxNum = CB_PATTERN_POS_MAXNUM;

	/* 先頭ポインタ */
	ptSrchRgnInfo = &( m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection] );

	for ( ni = 0L; ni < nPatternPosMaxNum; ni++ )
	{
		/* 座標値セット */
		ptSrchRgn[ni] = ptSrchRgnInfo->tSrchRgnInfo4Pattern[ni].tRgnImg;
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		探索領域(線・丸に対する)情報取得
 *
 * @param[out]	ptSrchRgn			:,領域構造体へのポインタ(CB_ADD_PATTERN_POS_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.17	K.Kato			新規作成
 *
 * @note		デバッグ用の取得関数のため、本関数内での引数nCamDirectionの範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
slong cb_img_getSearchRgnInfo4LineCircle( CB_RECT_RGN* ptSrchRgn, slong nCamDirection )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6708
{
	slong	ni = 0L;
	slong	nPatternPosMaxNum = 0L;
	CB_FPE_SRCH_RGN_INFO_EACH_CAM	*ptSrchRgnInfo = NULL;

	/* 引数チェック */
	if ( NULL == ptSrchRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パターン数 */
	nPatternPosMaxNum = CB_ADD_PATTERN_POS_MAXNUM;

	/* 先頭ポインタ */
	ptSrchRgnInfo = &( m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection] );

	for ( ni = 0L; ni < nPatternPosMaxNum; ni++ )
	{
		/* 座標値セット */
		ptSrchRgn[ni] = ptSrchRgnInfo->tSrchRgnInfoAddPattern[ni].tRgnImg;
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		特徴点位置(市松に対する)取得
 *
 * @param[out]	ptCrossPnt			:,十字点位置構造体へのポインタ(CB_FPE_CROSS_PNT_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumCrossPnt		:,十字点位置の数を格納する領域へのポインタ,-,[-],
 * @param[out]	ptTPntUpper			:,Ｔ字位置(上側)構造体へのポインタ(CB_FPE_T_PNT_UPPER_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumTPntUpper		:,Ｔ字位置(上側)の数を格納する領域へのポインタ,-,[-],
 * @param[out]	ptTPntLower			:,Ｔ字位置(下側)構造体へのポインタ(CB_FPE_T_PNT_LOWER_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumTPntLower		:,Ｔ字位置(下側)の数を格納する領域へのポインタ,-,[-],
 * @param[out]	ptTPntLeft			:,Ｔ字位置(左側)構造体へのポインタ(CB_FPE_T_PNT_LEFT_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumTPntLeft		:,Ｔ字位置(左側)の数を格納する領域へのポインタ,-,[-],
 * @param[out]	ptTPntRight			:,Ｔ字位置(右側)構造体へのポインタ(CB_FPE_T_PNT_RIGHT_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumTPntRight		:,Ｔ字位置(右側)の数を格納する領域へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.17	K.Kato			新規作成
 *
 * @note		デバッグ用の取得関数のため、本関数内での引数nCamDirectionの範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
slong cb_img_getFtrPntPos4Chkboard( t_cb_img_CenterPos* ptCrossPnt, slong* pnNumCrossPnt, 						// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6709
								   t_cb_img_CenterPos* ptTPntUpper, slong* pnNumTPntUpper, 
								   t_cb_img_CenterPos* ptTPntLower, slong* pnNumTPntLower, 
								   t_cb_img_CenterPos* ptTPntLeft, slong* pnNumTPntLeft, 
								   t_cb_img_CenterPos* ptTPntRight, slong* pnNumTPntRight, slong nCamDirection )
{
	slong	ni = 0L;
	slong	nNumPnt = 0L;
	CB_FPE_SRCH_RSLT_PNT_INFO	*ptSrchRsltPntInfo = NULL;

	/* 引数チェック */
	if (   ( NULL == ptCrossPnt  ) || ( NULL == pnNumCrossPnt  )
		|| ( NULL == ptTPntUpper ) || ( NULL == pnNumTPntUpper )
		|| ( NULL == ptTPntLower ) || ( NULL == pnNumTPntLower )
		|| ( NULL == ptTPntLeft  ) || ( NULL == pnNumTPntLeft  )
		|| ( NULL == ptTPntRight ) || ( NULL == pnNumTPntRight ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
	ptSrchRsltPntInfo = &( m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltPntInfo[CB_PATTERN_POS_CENTER] );

	/* 十字点 */
	nNumPnt = ptSrchRsltPntInfo->nNumCrossPnt;
	*pnNumCrossPnt = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptCrossPnt[ni] = (ptSrchRsltPntInfo->tCrossPntPrecise)[ni];
	}

	/* Ｔ字点(上側) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntUpper;
	*pnNumTPntUpper = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntUpper[ni] = (ptSrchRsltPntInfo->tTPntUpperPrecise)[ni];
	}

	/* Ｔ字点(下側) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntLower;
	*pnNumTPntLower = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntLower[ni] = (ptSrchRsltPntInfo->tTPntLowerPrecise)[ni];
	}

	/* Ｔ字点(左側) */
	nNumPnt = ptSrchRsltPntInfo->nNumTPntLeft;
	*pnNumTPntLeft = nNumPnt;
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptTPntLeft[ni] = (ptSrchRsltPntInfo->tTPntLeftPrecise)[ni];
	}

	/* Ｔ字点(右側) */
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
 * @brief		特徴点位置(線・丸に対する)取得
 *
 * @param[out]	ptCenterPntL		:,丸の中点位置(左パターン)構造体へのポインタ(CB_ADD_PATTERN_PNT_POS_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumCenterPntL		:,丸の中点位置(左パターン)の数を格納する領域へのポインタ,-,[-],
 * @param[out]	ptCenterPntR		:,丸の中点位置(右パターン)構造体へのポインタ(CB_ADD_PATTERN_PNT_POS_MAXNUM個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumCenterPntR		:,丸の中点位置(右パターン)の数を格納する領域へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.17	K.Kato			新規作成
 *
 * @note		デバッグ用の取得関数のため、本関数内での引数nCamDirectionの範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
slong cb_img_getFtrPntPos4LineCircle( t_cb_img_CenterPos* ptCenterPntL, slong* pnNumCenterPntL, 						// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6710
									 t_cb_img_CenterPos* ptCenterPntR, slong* pnNumCenterPntR, slong nCamDirection )
{
	slong	ni = 0L;
	CB_FPE_SRCH_RSLT_ADD_PNT_INFO	*ptSrchRsltPntInfo = NULL;

	/* 引数チェック */
	if (   ( NULL == ptCenterPntL ) || ( NULL == pnNumCenterPntL )
		|| ( NULL == ptCenterPntR ) || ( NULL == pnNumCenterPntR ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 左側のパターン */
	/* 先頭ポインタ */
	ptSrchRsltPntInfo = &( m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltAddPntInfo[CB_ADD_PATTERN_POS_LEFT] );
	*pnNumCenterPntL = ptSrchRsltPntInfo->nNumCenterPnt;
	for ( ni = 0L; ni < ptSrchRsltPntInfo->nNumCenterPnt; ni++ )
	{
		ptCenterPntL[ni] = (ptSrchRsltPntInfo->tCenterPntPrecise)[ni];
	}

	/* 右側のパターン */
	/* 先頭ポインタ */
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
 * @brief		グリッド線上の頂点取得
 *
 * @param[out]	ptPeak				:,頂点構造体へのポインタ(CB_FPE_GRID_NUM_WLD_Y*CB_FPE_GRID_NUM_WLD_X個分のテーブルが確保されていること),-,[-],
 * @param[out]	pnNumGridVert		:,グリッド線の数(垂直方向)を格納する領域へのポインタ,-,[-],
 * @param[out]	pnNumGridHori		:,グリッド線の数(水平方向)を格納する領域へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.17	K.Kato			新規作成
 *
 * @note		デバッグ用の取得関数のため、本関数内での引数nCamDirectionの範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
slong cb_img_getGridLinePeak( CB_IMG_POINT* ptPeak, slong* pnNumGridVert, slong* pnNumGridHori, slong nCamDirection )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6711
{
	slong	ni = 0L, 
			nj = 0L;
	slong	nOffset = 0L;
	slong	nNumGridVert = 0L, 
			nNumGridHori = 0L;
	CB_FPE_DBG_GRID_INFO	*ptGridInfo = NULL;

	/* 引数チェック */
	if ( ( NULL == ptPeak ) || ( NULL == pnNumGridVert ) || ( NULL == pnNumGridHori ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* グリッド情報有効チェック */
	if ( ( CB_TRUE != m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4Pattern ) || ( CB_TRUE != m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].nFlagValidInfo4AddPattern ) )
	{
		return ( CB_IMG_NG );
	}

	/* 先頭ポインタ */
	ptGridInfo = &( m_FPE_tDbgInfo.tGridInfo[nCamDirection] );

	/* グリッド線の本数 */
	nNumGridVert = CB_FPE_GRID_NUM_WLD_X;
	nNumGridHori = CB_FPE_GRID_NUM_WLD_Y;
	*pnNumGridVert = nNumGridVert;
	*pnNumGridHori = nNumGridHori;

	/* グリッド線上の点を格納 */
	for ( ni = 0L; ni < nNumGridHori; ni++ )
	{
		for ( nj = 0L; nj < nNumGridVert; nj++ )
		{
			/* オフセット */
			nOffset = ( ni * nNumGridVert ) + nj;

			/* 格納 */
			*( ptPeak + nOffset ) = ( ptGridInfo->tPntImg )[ni][nj];
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		認識実行状態取得
 *
 * @param[out]	pnState				:,実行状態を格納する領域へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.17	K.Kato			新規作成
 *
 * @note		デバッグ用の取得関数のため、本関数内での引数nCamDirectionの範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
slong cb_img_getExecState( slong* pnState, slong nCamDirection )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6712
{
	/* 引数チェック */
	if ( NULL == pnState )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 未使用変数回避 ワーニング対策 */
	CALIB_UNUSED_VARIABLE( nCamDirection );	/* 変数使用時には削除してください */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.3  R-53, ID-6713
	
	*pnState = CB_IMG_OK;

	return ( CB_IMG_OK );
}


/********************************************************************
 * 内部関数　定義
 ********************************************************************/

/******************************************************************************/
/**
 * @brief		画像メモリ確保
 *
 * @param[out]	pnImg			:,画面IDへのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.14	K.Kato			新規作成
 * @date		2015.08.26	S.Morita		サイドマーカー画像サイズ修正対応
 */
/******************************************************************************/
static slong cb_FPE_AllocImg( IMPLIB_IMGID* pnImg )
{
	slong	nRet = CB_IMG_NG;
	sint	ni = 0;

	/* 引数チェック */
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

	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		カメラパラメータの設定
 *
 * @param[in]	ptCamPrmSrc		:,カメラパラメータ(ソース)へのポインタ,-,[-],
 * @param[out]	ptCamPrmDst		:,カメラパラメータ(デスティネーション)へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.08	K.Kato			新規作成
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
	
	/* 引数チェック */
	if ( ( NULL == ptCamPrmSrc ) || ( NULL == ptCamPrmDst ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
	/* 外部パラメータ */
	ptExtrinsicPrmSrc = &( ptCamPrmSrc->tExtrinsicPrm );
	ptExtrinsicPrmDst = &( ptCamPrmDst->tExtrinsicPrm );
	ptExtrinsicPrmDst->dPitchDeg	= ptExtrinsicPrmSrc->dPitchDeg;
	ptExtrinsicPrmDst->dRollDeg		= ptExtrinsicPrmSrc->dRollDeg;
	ptExtrinsicPrmDst->dYawDeg		= ptExtrinsicPrmSrc->dYawDeg;
	ptExtrinsicPrmDst->dShiftX		= ptExtrinsicPrmSrc->dShiftX;
	ptExtrinsicPrmDst->dShiftY		= ptExtrinsicPrmSrc->dShiftY;
	ptExtrinsicPrmDst->dShiftZ		= ptExtrinsicPrmSrc->dShiftZ;
	/* 内部パラメータ */
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
	/* 歪みパラメータ */
	ptDistPrmDst = &( ptCamPrmDst->tDistPrm );
	ptDistPrmDst->nHeightTblMaxNum		= ptIntrinsicPrmSrc->nNumImgHeightTbl;
	ptDistPrmDst->dMaxAngleDeg			= ptIntrinsicPrmSrc->dMaxAngle;
	ptDistPrmDst->pdHeightTbl			= ptIntrinsicPrmSrc->pdImgHeightTbl;
	ptDistPrmDst->nFlgDistortion		= (slong)ptIntrinsicPrmSrc->nFlagDistortion;

	/* パラメータ登録 */

	/* 先頭ポインタ */
	ptExtrinsicPrmDst = &( ptCamPrmDst->tExtrinsicPrm );
	ptIntrinsicPrmDst = &( ptCamPrmDst->tIntrinsicPrm );
	ptDistPrmDst = &( ptCamPrmDst->tDistPrm );
	/* 登録 */
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
 * @brief		カメラパラメータの削除
 *
 * @param[in]	ptCamPrm		:,カメラパラメータ(ソース)へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.06.25		K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_DeleteCameraParam( CB_FPE_CAM_PRM* ptCamPrm )
{
	slong						nRet = CB_IMG_NG;
	
	/* 引数チェック */
	if ( NULL == ptCamPrm )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ削除 */
	nRet = cb_CG_DeleteCameraPrm( ptCamPrm->nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 無効値設定 */
	ptCamPrm->nCamID = -1L;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		カメラパラメータ(許容誤差)の設定
 *
 * @param[in]	ptCamPrmSrc			:,カメラパラメータ(ソース)へのポインタ,-,[-],
 * @param[out]	ptCamErrInfo		:,許容誤差情報構造体(デスティネーション)へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.04.15	K.Kato			新規作成
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

	/* 引数チェック */
	if ( ( NULL == ptCamPrmSrc ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
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

	/* 光軸位置(水平) */
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

	/* 光軸位置(垂直) */
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
 * @brief		探索領域設定(World)
 *
 * @param[in]	ptPatternInfo		:,特徴パターン情報構造体へのポインタ,-,[-],
 * @param[out]	ptSrchRgnInfo		:,探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.07	K.Kato			新規作成
 * @date		2014.12.01	S.Morita		キャリブ内パラメータ推定処理のため修正(キャスト修正)
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionWorld( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo )
{
	ulong								nj = 0UL;
	ulong								nk = 0UL;
	CB_FPE_SRCH_RGN_INFO_EACH_PATTERN	*ptSrchRgnInfo4Ptn = NULL;
	CB_IMG_PATTERN_CORNER_INFO			*ptCornerInfo = NULL;
	CB_IMG_PATTERN_FTR_PNT_INFO			*ptFtrPntInfo = NULL;

	/* 引数チェック */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* コーナーポイントの数をチェック */
	if ( 4UL != ptPatternInfo->tCornerInfo.nNumCenter )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}

	/* 先頭ポインタ */
	ptCornerInfo = &( ptPatternInfo->tCornerInfo );
	ptFtrPntInfo = &( ptPatternInfo->tFtrPntInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfo4Pattern[CB_PATTERN_POS_CENTER]);
	/* 点数と座標値設定 */
	ptSrchRgnInfo4Ptn->nNumPnt = (slong)ptCornerInfo->nNumCenter;
	for ( nj = 0UL; nj < ptCornerInfo->nNumCenter; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWld[nj].dX = ( ptCornerInfo->tCenter )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dY = ( ptCornerInfo->tCenter )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dZ = ( ptCornerInfo->tCenter )[nj].dZ;
	}	/* for ( nj ) */

	/* カメラパラメータ推定用に座標を格納する */
	ptSrchRgnInfo4Ptn->nNumCenterPnt = ptFtrPntInfo->nNumCenter;
	for ( nk = 0UL; nk < ptFtrPntInfo->nNumCenter; nk++ )
	{
		ptSrchRgnInfo4Ptn->tCenterPntWld[nk].dX = ( ptFtrPntInfo->tCenter )[nk].dX;
		ptSrchRgnInfo4Ptn->tCenterPntWld[nk].dY = ( ptFtrPntInfo->tCenter )[nk].dY;
		ptSrchRgnInfo4Ptn->tCenterPntWld[nk].dZ = ( ptFtrPntInfo->tCenter )[nk].dZ;
	}	/* for ( nk ) */

	/* 特徴パターン情報(パターンの向き) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagCenterPlacement;
	/* 特徴パターン情報(パターンのType) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeCenterPattern;
	/* 特徴パターン情報(パターン抽出アルゴリズム) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeCenterAlgorithm;
	/* 特徴パターン情報(パターン併設状態フラグ) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nDoubleType = ptPatternInfo->nDoubleType;

	/* 特徴パターン情報(レイアウト種別) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nlayoutType = ptPatternInfo->nlayoutType;
	/* 特徴パターン情報(オフセットタイプ) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nCenterOffsetType = ptPatternInfo->nCenterOffsetType;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		追加パターンに対する探索領域設定(World)
 *
 * @param[in]	ptPatternInfo		:,特徴パターン情報構造体へのポインタ,-,[-],
 * @param[out]	ptSrchRgnInfo		:,探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.03	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionWorld4AddPattern( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo )
{
	ulong									nj = 0UL;
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo4Ptn = NULL;
	CB_IMG_PATTERN_CORNER_INFO				*ptCornerInfo = NULL;
#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN
	CB_IMG_PATTERN_FTR_PNT_INFO				*ptFtrPntInfo = NULL;				/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */

	/* 引数チェック */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* コーナーポイントの数をチェック */
	if (   ( 4UL != ptPatternInfo->tCornerInfo.nNumLeft )
		|| ( 4UL != ptPatternInfo->tCornerInfo.nNumRight ) )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN												/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
	/* 線丸マーカーの特徴点数をチェック */
	if (   ( 2UL != ptPatternInfo->tFtrPntInfo.nNumLeft )
		|| ( 2UL != ptPatternInfo->tFtrPntInfo.nNumRight ) )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */											/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */

	/* 先頭ポインタ */
	ptCornerInfo = &( ptPatternInfo->tCornerInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);
	/* 点数と座標値設定 */
	ptSrchRgnInfo4Ptn->nNumPnt = (slong)ptCornerInfo->nNumLeft;
	for ( nj = 0UL; nj < ptCornerInfo->nNumLeft; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWld[nj].dX = ( ptCornerInfo->tLeft )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dY = ( ptCornerInfo->tLeft )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dZ = ( ptCornerInfo->tLeft )[nj].dZ;
	}	/* for ( nj ) */

	/* 先頭ポインタ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);
	/* 点数と座標値設定 */
	ptSrchRgnInfo4Ptn->nNumPnt = (slong)ptCornerInfo->nNumRight;
	for ( nj = 0UL; nj < ptCornerInfo->nNumRight; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWld[nj].dX = ( ptCornerInfo->tRight )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dY = ( ptCornerInfo->tRight )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWld[nj].dZ = ( ptCornerInfo->tRight )[nj].dZ;
	}	/* for ( nj ) */
	
#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN												/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
	/* 先頭ポインタ */
	ptFtrPntInfo = &( ptPatternInfo->tFtrPntInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);
	/* 座標値設定 */
	for ( nj = 0UL; nj < ptFtrPntInfo->nNumLeft; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dX = ( ptFtrPntInfo->tLeft )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dY = ( ptFtrPntInfo->tLeft )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dZ = ( ptFtrPntInfo->tLeft )[nj].dZ;
	}	/* for ( nj ) */

	/* 先頭ポインタ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);
	/* 座標値設定 */
	for ( nj = 0UL; nj < ptFtrPntInfo->nNumRight; nj++ )
	{
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dX = ( ptFtrPntInfo->tRight )[nj].dX;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dY = ( ptFtrPntInfo->tRight )[nj].dY;
		ptSrchRgnInfo4Ptn->tPntWldCircle[nj].dZ = ( ptFtrPntInfo->tRight )[nj].dZ;
	}	/* for ( nj ) */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */											/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */

	/* 先頭ポインタ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);
	/* 特徴パターン情報(パターンの向き) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagLeftPlacement;
	/* 特徴パターン情報(パターンのType) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeLeftPattern;
	/* 特徴パターン情報(パターン抽出アルゴリズム) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeLeftAlgorithm;

	/* 先頭ポインタ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);
	/* 特徴パターン情報(パターンの向き) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagRightPlacement;
	/* 特徴パターン情報(パターンのType) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeRightPattern;
	/* 特徴パターン情報(パターン抽出アルゴリズム) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeRightAlgorithm;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		追加パターン(左右市松)に対する探索領域設定(World)
 *
 * @param[in]	ptPatternInfo		:,特徴パターン情報構造体へのポインタ,-,[-],
 * @param[out]	ptSrchRgnInfo		:,探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.11.12	F.Sano			新規作成
 */
/******************************************************************************/
static slong cb_FPE_SetSearchRegionWorld4AddChkboard( const CB_IMG_PATTERN_INFO* const ptPatternInfo, CB_FPE_SRCH_RGN_INFO_EACH_CAM* ptSrchRgnInfo, slong nCamDirection )
{
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptSrchRgnInfo4Ptn = NULL;
	CB_IMG_PATTERN_ADD_INFO					*ptAddInfo = NULL;			/* 追加情報座標取得用 */

	slong nSignHaarLikeFeature = 0L;
	slong nRet = 0L;


	/* 引数チェック */
	if ( ( NULL == ptPatternInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 左右市松の追加制御点数をチェック */
	if (   ( CB_IMG_ADD_PNT_MAXNUM_LEFT  != ptPatternInfo->tAddInfo.nNumLeft )
		|| ( CB_IMG_ADD_PNT_MAXNUM_RIGHT != ptPatternInfo->tAddInfo.nNumRight ) )
	{
		return ( CB_IMG_NG_ARG_INCORRECT_VALUE );
	}

	/* 先頭ポインタ */
	ptAddInfo = &( ptPatternInfo->tAddInfo );
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT]);

	/* 左右市松（左）十字点の座標値設定 */
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_CC].dX;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_CC].dY;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_CC].dZ;

	/* 左右市松(左)の点数と座標値設定 */
	ptSrchRgnInfo4Ptn->nNumPnt = 4L;


	/* 重み付きHaar-like特徴量符号の取得 */
	nRet = cb_FPE_GetSignHaarLikeFeature( &( ptSrchRgnInfo->tSrchRgnInfoAddPattern[0].tPtnInfo ), nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		return ( nRet );
	}

	/* 画像上で、左下が白の場合 */
	if( nSignHaarLikeFeature == CB_IMG_CHECKBOARD_SIGN_MINUS )
	{
		/* 市松左上のコーナー点(追加情報の上側－左) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UL].dZ;
		/* 十字点下側のT字点(追加情報の下側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* 十字点上側のT字点(追加情報の上側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* 市松右下のコーナー点(追加情報の下側－右) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LR].dZ;
	}
	/* 画像上で、左下が黒の場合 */
	else
	{
		/* 市松左下のコーナー点(追加情報の下側－左) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LL].dZ;
		/* 十字点下側のT字点(追加情報の下側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* 十字点上側のT字点(追加情報の上側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* 市松右上のコーナー点(追加情報の上側－右) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tLeft )[CB_IMG_CHKBOARD_PNT_UR].dZ;
	}


	/* 特徴パターン情報(パターンの向き) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagLeftPlacement;
	/* 特徴パターン情報(パターンのType) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeLeftPattern;
	/* 特徴パターン情報(パターン抽出アルゴリズム) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeLeftAlgorithm;

	/* 左右市松（右）十字点の座標値設定 */
	/* 先頭ポインタ */
	ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT]);

	/* 左右市松（右）十字点の座標値設定 */
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_CC].dX;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_CC].dY;
	ptSrchRgnInfo4Ptn->tPntWldSideChkCenter.dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_CC].dZ;
	
	/* 左右市松(右)の点数と座標値設定 */
	ptSrchRgnInfo4Ptn->nNumPnt = 4L;

	/* 画像上で、左下が白の場合 */
	if( nSignHaarLikeFeature == CB_IMG_CHECKBOARD_SIGN_MINUS )
	{
		/* 市松左上のコーナー点(追加情報の上側－左) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UL].dZ;
		/* 十字点下側のT字点(追加情報の下側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* 十字点上側のT字点(追加情報の上側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* 市松右下のコーナー点(追加情報の下側－右) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LR].dZ;
	}
	/* 画像上で、左下が黒の場合 */
	else
	{
		/* 市松左下のコーナー点(追加情報の下側－左) */
		ptSrchRgnInfo4Ptn->tPntWld[0].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LL].dX;
		ptSrchRgnInfo4Ptn->tPntWld[0].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LL].dY;
		ptSrchRgnInfo4Ptn->tPntWld[0].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LL].dZ;
		/* 十字点下側のT字点(追加情報の下側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[1].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[1].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[1].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_LC].dZ;
		/* 十字点上側のT字点(追加情報の上側－中央) */
		ptSrchRgnInfo4Ptn->tPntWld[2].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dX;
		ptSrchRgnInfo4Ptn->tPntWld[2].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dY;
		ptSrchRgnInfo4Ptn->tPntWld[2].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UC].dZ;
		/* 市松右上のコーナー点(追加情報の上側－右) */
		ptSrchRgnInfo4Ptn->tPntWld[3].dX = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UR].dX;
		ptSrchRgnInfo4Ptn->tPntWld[3].dY = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UR].dY;
		ptSrchRgnInfo4Ptn->tPntWld[3].dZ = ( ptAddInfo->tRight )[CB_IMG_CHKBOARD_PNT_UR].dZ;
	}
	/* 特徴パターン情報(パターンの向き) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nFlagPlacement = ptPatternInfo->nFlagRightPlacement;
	/* 特徴パターン情報(パターンのType) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypePattern = ptPatternInfo->nTypeRightPattern;
	/* 特徴パターン情報(パターン抽出アルゴリズム) */
	ptSrchRgnInfo4Ptn->tPtnInfo.nTypeAlgorithm = ptPatternInfo->nTypeRightAlgorithm;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			探索領域設定(Image)
 *
 * @param[in,out]	ptSrchRgnInfo		:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 * @param[in]		ptCamPrm			:,カメラパラメータ構造体へのポインタ,-,[-],
 * @param[in]		ptCamErrInfo		:,許容誤差情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.02.07	K.Kato			新規作成
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

	/* 引数チェック */
	if ( ( NULL == ptSrchRgnInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nNumPtn = CB_PATTERN_POS_MAXNUM;
	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;

	/* とりあえずパラメータ設定(更新関数を実行するため) */
	nRet = cb_CG_SetCameraPrm( &(ptCamPrm->tExtrinsicPrm), &(ptCamPrm->tIntrinsicPrm), &(ptCamPrm->tDistPrm) );
	if ( 0L >= nRet )
	{
		return ( CB_IMG_NG );
	}
	nCamID = nRet;

	/* 初期化 */
	for ( nj = 0L; nj < nNumPtn; nj++ )	/* 市松パターン数 */
	{
		/* 先頭ポインタ */
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

	for ( nl = 0L; nl < nTblMaxNum; nl++ )	/* 許容誤差のパターン */
	{
		/* パラメータコピー */
		tExtrinsicPrm = ptCamPrm->tExtrinsicPrm;
		tIntrinsicPrm = ptCamPrm->tIntrinsicPrm;
		tDistPrm = ptCamPrm->tDistPrm;
		/* 誤差を考慮させる */
		tExtrinsicPrm.dPitchDeg = ( ptCamPrm->tExtrinsicPrm ).dPitchDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrPitch;
		tExtrinsicPrm.dRollDeg = ( ptCamPrm->tExtrinsicPrm ).dRollDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrRoll;
		tExtrinsicPrm.dYawDeg = ( ptCamPrm->tExtrinsicPrm ).dYawDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrYaw;
		tExtrinsicPrm.dShiftX = ( ptCamPrm->tExtrinsicPrm ).dShiftX + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldX;
		tExtrinsicPrm.dShiftY = ( ptCamPrm->tExtrinsicPrm ).dShiftY + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldY;
		tExtrinsicPrm.dShiftZ = ( ptCamPrm->tExtrinsicPrm ).dShiftZ + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldZ;
		tIntrinsicPrm.dCentPntW = ( ptCamPrm->tIntrinsicPrm ).dCentPntW + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntHori;
		tIntrinsicPrm.dCentPntH = ( ptCamPrm->tIntrinsicPrm ).dCentPntH + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntVert;
		/* パラメータ更新 */
		nRet = cb_CG_UpdateCameraPrm( nCamID, &tExtrinsicPrm, &tIntrinsicPrm, &tDistPrm );
		if ( CB_CG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* パターンのループ */
		for ( nj = 0L; nj < nNumPtn; nj++ )
		{
			/* 先頭ポインタ */
			ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfo4Pattern[nj]);

			/* 初期値 */
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

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6714
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6715

				/* 最小値・最大値比較 */
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

			/* 外接矩形を探索領域として設定 */
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

	/* パラメータ削除 */
	nRet = cb_CG_DeleteCameraPrm( nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			追加パターンに対する探索領域設定(Image)
 *
 * @param[in,out]	ptSrchRgnInfo		:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 * @param[in]		ptCamPrm			:,カメラパラメータ構造体へのポインタ,-,[-],
 * @param[in]		ptCamErrInfo		:,許容誤差情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.04	K.Kato			新規作成
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
	slong									ni = 0L;							/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */

	/* 引数チェック */
	if ( ( NULL == ptSrchRgnInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nNumPtn = CB_ADD_PATTERN_POS_MAXNUM;
	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;

	/* とりあえずパラメータ設定(更新関数を実行するため) */
	nRet = cb_CG_SetCameraPrm( &(ptCamPrm->tExtrinsicPrm), &(ptCamPrm->tIntrinsicPrm), &(ptCamPrm->tDistPrm) );
	if ( 0L >= nRet )
	{
		return ( CB_IMG_NG );
	}
	nCamID = nRet;

	/* 初期化 */
	for ( nj = 0L; nj < nNumPtn; nj++ )
	{
		/* 先頭ポインタ */
		ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);
		ptSrchRgnInfo4Ptn->tPntImg[0L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[0L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nY = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nY = -1L;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN					/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
		/* 上側の丸 */
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nY = -1L;
		/* 下側の丸 */
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nY = -1L;

#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */				/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */
	}

	for ( nl = 0L; nl < nTblMaxNum; nl++ )
	{
		/* パラメータコピー */
		tExtrinsicPrm = ptCamPrm->tExtrinsicPrm;
		tIntrinsicPrm = ptCamPrm->tIntrinsicPrm;
		tDistPrm = ptCamPrm->tDistPrm;
		/* 誤差を考慮させる */
		tExtrinsicPrm.dPitchDeg = ( ptCamPrm->tExtrinsicPrm ).dPitchDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrPitch;
		tExtrinsicPrm.dRollDeg = ( ptCamPrm->tExtrinsicPrm ).dRollDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrRoll;
		tExtrinsicPrm.dYawDeg = ( ptCamPrm->tExtrinsicPrm ).dYawDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrYaw;
		tExtrinsicPrm.dShiftX = ( ptCamPrm->tExtrinsicPrm ).dShiftX + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldX;
		tExtrinsicPrm.dShiftY = ( ptCamPrm->tExtrinsicPrm ).dShiftY + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldY;
		tExtrinsicPrm.dShiftZ = ( ptCamPrm->tExtrinsicPrm ).dShiftZ + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldZ;
		tIntrinsicPrm.dCentPntW = ( ptCamPrm->tIntrinsicPrm ).dCentPntW + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntHori;
		tIntrinsicPrm.dCentPntH = ( ptCamPrm->tIntrinsicPrm ).dCentPntH + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntVert;
		/* パラメータ更新 */
		nRet = cb_CG_UpdateCameraPrm( nCamID, &tExtrinsicPrm, &tIntrinsicPrm, &tDistPrm );
		if ( CB_CG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* パターンのループ */
		for ( nj = 0L; nj < nNumPtn; nj++ )
		{
			/* 先頭ポインタ */
			ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);

			/* 初期値 */
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

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6716
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6717

				/* 最小値・最大値比較 */
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

			/* 外接矩形を探索領域として設定 */
			ptSrchRgnInfo4Ptn->tPntImg[0L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[0L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nY = nYmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nY = nYmax;

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN					/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
			for( ni = 0L; ni < CB_ADD_PATTERN_PNT_POS_MAXNUM; ni++ )
			{
				/* World to Image */
				nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWldCircle[ni]), &tTempSrchPntImg );
				if ( CB_CG_OK != nRet )
				{
					return ( CB_IMG_NG );
				}

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6718
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6719

				/* 黒丸探索領域として最小値(始点)を設定 */
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nX > tSrchPntImg.nX )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nX = tSrchPntImg.nX;
				}
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nY > tSrchPntImg.nY )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnTL.nY = tSrchPntImg.nY;
				}
				/* 黒丸探索領域として最大値(終点)を設定 */
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nX < tSrchPntImg.nX )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nX = tSrchPntImg.nX;
				}
				if( ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nY < tSrchPntImg.nY )
				{
					ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleSrchRgnBR.nY = tSrchPntImg.nY;
				}

#ifdef CB_FPE_CSW_DEBUG_ON
				/* デバッグ描画用 */
				ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleErrPntTbl[nl].nX = tSrchPntImg.nX;
				ptSrchRgnInfo4Ptn->tPntImgCircle[ni].tCircleErrPntTbl[nl].nY = tSrchPntImg.nY;
#endif /* CB_FPE_CSW_DEBUG_ON */
			}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */				/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */
		}	/* for ( nj ) */
	}	/* for ( nl ) */

	/* パラメータ削除 */
	nRet = cb_CG_DeleteCameraPrm( nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			追加パターン(左右市松)に対する探索領域設定(Image)
 *
 * @param[in,out]	ptSrchRgnInfo		:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 * @param[in]		ptCamPrm			:,カメラパラメータ構造体へのポインタ,-,[-],
 * @param[in]		ptCamErrInfo		:,許容誤差情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.12	F.Sano			新規作成
 * @date			2015.08.26	S.Morita		サイドマーカー画像サイズ修正対応
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


	/* 引数チェック */
	if ( ( NULL == ptSrchRgnInfo ) || ( NULL == ptCamPrm ) || ( NULL == ptCamErrInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nNumPtn = CB_ADD_PATTERN_POS_MAXNUM;
	nTblMaxNum = CB_FPE_ERR_TBL_MAXNUM;

	/* とりあえずパラメータ設定(更新関数を実行するため) */
	nRet = cb_CG_SetCameraPrm( &(ptCamPrm->tExtrinsicPrm), &(ptCamPrm->tIntrinsicPrm), &(ptCamPrm->tDistPrm) );
	if ( 0L >= nRet )
	{
		return ( CB_IMG_NG );
	}
	nCamID = nRet;

	/* 初期化 */
	for ( nj = 0L; nj < nNumPtn; nj++ )
	{
		/* 先頭ポインタ */
		ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);
		ptSrchRgnInfo4Ptn->tPntImg[0L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[0L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[1L].nY = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[2L].nY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nX = -1L;
		ptSrchRgnInfo4Ptn->tPntImg[3L].nY = -1L;

		/* 十字点 */
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSX = CB_GET_IMG_XSIZE;
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nSY = CB_GET_IMG_YSIZE;
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEX = -1L;
		ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkSrchRgn.nEY = -1L;

	}

	for ( nl = 0L; nl < nTblMaxNum; nl++ )
	{
		/* パラメータコピー */
		tExtrinsicPrm = ptCamPrm->tExtrinsicPrm;
		tIntrinsicPrm = ptCamPrm->tIntrinsicPrm;
		tDistPrm = ptCamPrm->tDistPrm;
		/* 誤差を考慮させる */
		tExtrinsicPrm.dPitchDeg = ( ptCamPrm->tExtrinsicPrm ).dPitchDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrPitch;
		tExtrinsicPrm.dRollDeg = ( ptCamPrm->tExtrinsicPrm ).dRollDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrRoll;
		tExtrinsicPrm.dYawDeg = ( ptCamPrm->tExtrinsicPrm ).dYawDeg + ( ptCamErrInfo->tErrTbl )[nl].dErrYaw;
		tExtrinsicPrm.dShiftX = ( ptCamPrm->tExtrinsicPrm ).dShiftX + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldX;
		tExtrinsicPrm.dShiftY = ( ptCamPrm->tExtrinsicPrm ).dShiftY + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldY;
		tExtrinsicPrm.dShiftZ = ( ptCamPrm->tExtrinsicPrm ).dShiftZ + ( ptCamErrInfo->tErrTbl )[nl].dErrOffsetWldZ;
		tIntrinsicPrm.dCentPntW = ( ptCamPrm->tIntrinsicPrm ).dCentPntW + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntHori;
		tIntrinsicPrm.dCentPntH = ( ptCamPrm->tIntrinsicPrm ).dCentPntH + ( ptCamErrInfo->tErrTbl )[nl].dErrCntPntVert;
		/* パラメータ更新 */
		nRet = cb_CG_UpdateCameraPrm( nCamID, &tExtrinsicPrm, &tIntrinsicPrm, &tDistPrm );
		if ( CB_CG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}

		/* パターンのループ */
		for ( nj = 0L; nj < nNumPtn; nj++ )
		{
			/* 先頭ポインタ */
			ptSrchRgnInfo4Ptn = &(ptSrchRgnInfo->tSrchRgnInfoAddPattern[nj]);

			/* 初期値 */
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

				tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6716
				tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6717

				/* 最小値・最大値比較 */
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

			/* 外接矩形を探索領域として設定 */
			ptSrchRgnInfo4Ptn->tPntImg[0L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[0L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nX = nXmin;
			ptSrchRgnInfo4Ptn->tPntImg[1L].nY = nYmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[2L].nY = nYmin;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nX = nXmax;
			ptSrchRgnInfo4Ptn->tPntImg[3L].nY = nYmax;

			/* 中心点 */
			/* World to Image */
			nRet = cb_CG_CalcW2C2I( nCamID, &(ptSrchRgnInfo4Ptn->tPntWldSideChkCenter), &tTempSrchPntImg );
			if ( CB_CG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}

			tSrchPntImg.nX = (slong)( tTempSrchPntImg.dW + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6718
			tSrchPntImg.nY = (slong)( tTempSrchPntImg.dH + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6719

			/* 左右市松中心点探索領域として最小値(始点)を設定 */
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
			/* 左右市松中心点探索領域として最大値(終点)を設定 */
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
			/* デバッグ描画用 */
			ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkErrPntTbl[nl].nX = tSrchPntImg.nX;
			ptSrchRgnInfo4Ptn->tPntImgSideChkCenter.tAddChkErrPntTbl[nl].nY = tSrchPntImg.nY;
#endif /* CB_FPE_CSW_DEBUG_ON */
		}	/* for ( nj ) */
	}	/* for ( nl ) */

	/* パラメータ削除 */
	nRet = cb_CG_DeleteCameraPrm( nCamID );
	if ( CB_CG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			探索領域設定
 *
 * @param[in,out]	ptSrchRgnInfo		:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.02.16	K.Kato			新規作成
 * @date			2017.10.12	A.Honda			探索範囲の丸め込み処理追加
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
	/* パラメータ */
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

	/* 引数チェック */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
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

	/* 画像サイズ */
	nXSizeImg = CB_GET_IMG_XSIZE;
	nYSizeImg = CB_GET_IMG_YSIZE;
	nHalfXSizeImg = CB_GET_IMG_XSIZE_HALF;
	nHalfYSizeImg = CB_GET_IMG_YSIZE_HALF;

	/* 先頭ポインタ */
	ptSrchRgnSrc				= &(ptSrchRgnInfo->tPntImg[0L]);
	ptSrchRgn					= &(ptSrchRgnInfo->tRgnImg);
	ptSrchRgnZoomOut			= &(ptSrchRgnInfo->tRgnImgZoomOut);
	ptSrchRgnHLsrc				= &(ptSrchRgnInfo->tRgnImgHLsrc);
	ptSrchRgnZoomOutHLChkEdge	= &(ptSrchRgnInfo->tRgnImgZoomOutHLChkEdge);
	ptSrchRgnZoomOutHLVertEdge	= &(ptSrchRgnInfo->tRgnImgZoomOutHLVertEdge);
	ptSrchRgnZoomOutHLHoriEdge	= &(ptSrchRgnInfo->tRgnImgZoomOutHLHoriEdge);
	ptSrchRgnZoomOutHLsrc		= &(ptSrchRgnInfo->tRgnImgZoomOutHLsrc);

	/* 通常 */
	ptSrchRgn = &(ptSrchRgnInfo->tRgnImg);
	ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
	ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
	ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
	ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* 縮小後サイズ */
	nXSizeZoomOut = ( ( ptSrchRgn->nEX - ptSrchRgn->nSX ) + 1L ) / nprm_xMagZoomOut;
	nYSizeZoomOut = ( ( ptSrchRgn->nEY - ptSrchRgn->nSY ) + 1L ) / nprm_yMagZoomOut;

	/* 縮小版 */
	ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
	ptSrchRgnZoomOut->nEX = ( ptSrchRgnZoomOut->nSX + nXSizeZoomOut ) - 1L;
	ptSrchRgnZoomOut->nEY = ( ptSrchRgnZoomOut->nSY + nYSizeZoomOut ) - 1L;
	/* 通常版で領域チェック／丸め込みを行っているため、領域チェックは不要 */

	/* Haar-like考慮 */
	/* 市松(縮小時) */
	ptSrchRgnZoomOutHLChkEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLChkEdge;
	ptSrchRgnZoomOutHLChkEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLChkEdge;
	ptSrchRgnZoomOutHLChkEdge->nEX = ptSrchRgnZoomOut->nEX + nprm_xsizeBlockHLChkEdge;
	ptSrchRgnZoomOutHLChkEdge->nEY = ptSrchRgnZoomOut->nEY + nprm_ysizeBlockHLChkEdge;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLChkEdge->nSX, ptSrchRgnZoomOutHLChkEdge->nSY, ptSrchRgnZoomOutHLChkEdge->nEX, ptSrchRgnZoomOutHLChkEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLChkEdge, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* 縦エッジ(縮小版) */
	ptSrchRgnZoomOutHLVertEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLVertEdge;
	ptSrchRgnZoomOutHLVertEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLVertEdge;
	ptSrchRgnZoomOutHLVertEdge->nEX = ptSrchRgnZoomOut->nEX + nprm_xsizeBlockHLVertEdge;
	ptSrchRgnZoomOutHLVertEdge->nEY = ptSrchRgnZoomOut->nEY;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLVertEdge->nSX, ptSrchRgnZoomOutHLVertEdge->nSY, ptSrchRgnZoomOutHLVertEdge->nEX, ptSrchRgnZoomOutHLVertEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLVertEdge, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* 横エッジ(縮小版) */
	ptSrchRgnZoomOutHLHoriEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLHoriEdge;
	ptSrchRgnZoomOutHLHoriEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLHoriEdge;
	ptSrchRgnZoomOutHLHoriEdge->nEX = ptSrchRgnZoomOut->nEX;
	ptSrchRgnZoomOutHLHoriEdge->nEY = ptSrchRgnZoomOut->nEY + nprm_ysizeBlockHLHoriEdge;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLHoriEdge->nSX, ptSrchRgnZoomOutHLHoriEdge->nSY, ptSrchRgnZoomOutHLHoriEdge->nEX, ptSrchRgnZoomOutHLHoriEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLHoriEdge, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* 全体を含む領域(縮小版) */
	ptSrchRgnZoomOutHLsrc->nSX = ( ptSrchRgnZoomOutHLChkEdge->nSX < ptSrchRgnZoomOutHLVertEdge->nSX ) ? ptSrchRgnZoomOutHLChkEdge->nSX : ptSrchRgnZoomOutHLVertEdge->nSX;
	ptSrchRgnZoomOutHLsrc->nSX = ( ptSrchRgnZoomOutHLHoriEdge->nSX < ptSrchRgnZoomOutHLsrc->nSX ) ? ptSrchRgnZoomOutHLHoriEdge->nSX : ptSrchRgnZoomOutHLsrc->nSX;
	ptSrchRgnZoomOutHLsrc->nSY = ( ptSrchRgnZoomOutHLChkEdge->nSY < ptSrchRgnZoomOutHLVertEdge->nSY ) ? ptSrchRgnZoomOutHLChkEdge->nSY : ptSrchRgnZoomOutHLVertEdge->nSY;
	ptSrchRgnZoomOutHLsrc->nSY = ( ptSrchRgnZoomOutHLHoriEdge->nSY < ptSrchRgnZoomOutHLsrc->nSY ) ? ptSrchRgnZoomOutHLHoriEdge->nSY : ptSrchRgnZoomOutHLsrc->nSY;
	ptSrchRgnZoomOutHLsrc->nEX = ( ptSrchRgnZoomOutHLChkEdge->nEX > ptSrchRgnZoomOutHLVertEdge->nEX ) ? ptSrchRgnZoomOutHLChkEdge->nEX : ptSrchRgnZoomOutHLVertEdge->nEX;
	ptSrchRgnZoomOutHLsrc->nEX = ( ptSrchRgnZoomOutHLHoriEdge->nEX > ptSrchRgnZoomOutHLsrc->nEX ) ? ptSrchRgnZoomOutHLHoriEdge->nEX : ptSrchRgnZoomOutHLsrc->nEX;
	ptSrchRgnZoomOutHLsrc->nEY = ( ptSrchRgnZoomOutHLChkEdge->nEY > ptSrchRgnZoomOutHLVertEdge->nEY ) ? ptSrchRgnZoomOutHLChkEdge->nEY : ptSrchRgnZoomOutHLVertEdge->nEY;
	ptSrchRgnZoomOutHLsrc->nEY = ( ptSrchRgnZoomOutHLHoriEdge->nEY > ptSrchRgnZoomOutHLsrc->nEY ) ? ptSrchRgnZoomOutHLHoriEdge->nEY : ptSrchRgnZoomOutHLsrc->nEY;
	/* Haar-like(考慮)、横エッジ、縦エッジで領域チェック／丸め込みを行っているため、領域チェックは不要 */

	/* 全体を含む領域(通常) */
	nXSize = ( ( ptSrchRgnZoomOutHLsrc->nEX - ptSrchRgnZoomOutHLsrc->nSX ) + 1L ) * nprm_xMagZoomOut;
	nYSize = ( ( ptSrchRgnZoomOutHLsrc->nEY - ptSrchRgnZoomOutHLsrc->nSY ) + 1L ) * nprm_yMagZoomOut;
	ptSrchRgnHLsrc->nSX = ptSrchRgnZoomOutHLsrc->nSX * nprm_xMagZoomOut;
	ptSrchRgnHLsrc->nSY = ptSrchRgnZoomOutHLsrc->nSY * nprm_yMagZoomOut;
	ptSrchRgnHLsrc->nEX = ( ptSrchRgnHLsrc->nSX + nXSize ) - 1L;
	ptSrchRgnHLsrc->nEY = ( ptSrchRgnHLsrc->nSY + nYSize ) - 1L;
	/* Haar-like(考慮)、横エッジ、縦エッジで領域チェック／丸め込みを行っているため、領域チェックは不要 */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			追加パターンに対する探索領域設定
 *
 * @param[in,out]	ptSrchRgnInfo		:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.04	K.Kato			新規作成
 * @date			2017.10.12	A.Honda			探索範囲の丸め込み処理追加
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
	/* パラメータ */
	slong	nprm_lengthReach = 10L;
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	slong	nprm_marginX4SrchRgn = 0L, 
			nprm_marginY4SrchRgn = 0L;

	/* 引数チェック */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_lengthReach = CB_FPE_PRM_RRF_LENGTH_REACH;
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_marginX4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_CIRCLE_LINE;
	nprm_marginY4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_CIRCLE_LINE;

	/* 画像サイズ */
	nXSizeImg = CB_GET_IMG_XSIZE;
	nYSizeImg = CB_GET_IMG_YSIZE;
	nHalfXSizeImg = CB_GET_IMG_XSIZE_HALF;
	nHalfYSizeImg = CB_GET_IMG_YSIZE_HALF;

	/* Left */
	/* 先頭ポインタ */
	ptSrchRgnSrc = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tPntImg[0L]);

	/* 通常 */
	ptSrchRgn = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImg);
	ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
	ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
	ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
	ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* 通常(縮小) */
	ptSrchRgnZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImgZoomOut);
	ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
	ptSrchRgnZoomOut->nEX = ptSrchRgn->nEX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nEY = ptSrchRgn->nEY / nprm_yMagZoomOut;
	/* 通常版で領域チェック／丸め込みを行っているため、領域チェックは不要 */

	/* RRF用 */
	ptSrchRgnRRF = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImgRRF);
	ptSrchRgnRRF->nSX = ptSrchRgn->nSX - nprm_lengthReach;
	ptSrchRgnRRF->nSY = ptSrchRgn->nSY - nprm_lengthReach;
	ptSrchRgnRRF->nEX = ptSrchRgn->nEX + nprm_lengthReach;
	ptSrchRgnRRF->nEY = ptSrchRgn->nEY + nprm_lengthReach;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnRRF->nSX, ptSrchRgnRRF->nSY, ptSrchRgnRRF->nEX, ptSrchRgnRRF->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgnRRF, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* RRF用(縮小) */
	ptSrchRgnRRFZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_LEFT].tRgnImgRRFZoomOut);
	ptSrchRgnRRFZoomOut->nSX = ptSrchRgnRRF->nSX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nSY = ptSrchRgnRRF->nSY / nprm_yMagZoomOut;
	ptSrchRgnRRFZoomOut->nEX = ptSrchRgnRRF->nEX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nEY = ptSrchRgnRRF->nEY / nprm_yMagZoomOut;
	/* RRF(通常版)で領域チェック／丸め込みを行っているため、領域チェックは不要 */

	/* Right */
	/* 先頭ポインタ */
	ptSrchRgnSrc = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tPntImg[0L]);

	/* 通常 */
	ptSrchRgn = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImg);
	ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
	ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
	ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
	ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* 通常(縮小) */
	ptSrchRgnZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImgZoomOut);
	ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
	ptSrchRgnZoomOut->nEX = ptSrchRgn->nEX / nprm_xMagZoomOut;
	ptSrchRgnZoomOut->nEY = ptSrchRgn->nEY / nprm_yMagZoomOut;
	/* 通常版で領域チェック／丸め込みを行っているため、領域チェックは不要 */

	/* RRF用 */
	ptSrchRgnRRF = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImgRRF);
	ptSrchRgnRRF->nSX = ptSrchRgn->nSX - nprm_lengthReach;
	ptSrchRgnRRF->nSY = ptSrchRgn->nSY - nprm_lengthReach;
	ptSrchRgnRRF->nEX = ptSrchRgn->nEX + nprm_lengthReach;
	ptSrchRgnRRF->nEY = ptSrchRgn->nEY + nprm_lengthReach;
	/* 領域チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnRRF->nSX, ptSrchRgnRRF->nSY, ptSrchRgnRRF->nEX, ptSrchRgnRRF->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		/* チェックNG時、領域再設定 */
		nRet = cb_FPE_ResetRegion( ptSrchRgnRRF, nXSizeImg, nYSizeImg );
		if ( CB_IMG_OK != nRet )
		{
			return ( CB_IMG_NG );
		}
	}

	/* RRF用(縮小) */
	ptSrchRgnRRFZoomOut = &(ptSrchRgnInfo[CB_ADD_PATTERN_POS_RIGHT].tRgnImgRRFZoomOut);
	ptSrchRgnRRFZoomOut->nSX = ptSrchRgnRRF->nSX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nSY = ptSrchRgnRRF->nSY / nprm_yMagZoomOut;
	ptSrchRgnRRFZoomOut->nEX = ptSrchRgnRRF->nEX / nprm_xMagZoomOut;
	ptSrchRgnRRFZoomOut->nEY = ptSrchRgnRRF->nEY / nprm_yMagZoomOut;
	/* RRF(通常版)で領域チェック／丸め込みを行っているため、領域チェックは不要 */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			追加パターン(左右市松)に対する探索領域設定
 *
 * @param[in,out]	ptSrchRgnInfo		:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.12	F.Sanoo			新規作成
 * @date			2015.08.26	S.Morita		サイドマーカー画像サイズ修正対応
 * @date			2017.10.12	A.Honda			探索範囲の丸め込み処理追加
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
	/* パラメータ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;
	slong	nprm_xsizeBlockHLChkEdge = 0L, 
			nprm_ysizeBlockHLChkEdge = 0L;
	slong	nprm_marginX4SrchRgn = 0L, 
			nprm_marginY4SrchRgn = 0L;

	/* 引数チェック */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_xsizeBlockHLChkEdge = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockHLChkEdge = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_marginX4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_X_ADDCHKBOARD;
	nprm_marginY4SrchRgn = CB_FPE_PRM_SRCH_RGN_IMG_MARGIN_Y_ADDCHKBOARD;

	/* 画像サイズ */
	nXSizeImg = CB_GET_IMG_XSIZE;
	nYSizeImg = CB_GET_IMG_YSIZE;
	nHalfXSizeImg = CB_GET_IMG_XSIZE_HALF;
	nHalfYSizeImg = CB_GET_IMG_YSIZE_HALF;

	/* LeftとRightはループで回す */
	for( ni = 0L ; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ ) {

		/* 先頭ポインタ */
		ptSrchRgnSrc				= &(ptSrchRgnInfo[ni].tPntImg[0L]);
		ptSrchRgn					= &(ptSrchRgnInfo[ni].tRgnImg);
		ptSrchRgnZoomOut			= &(ptSrchRgnInfo[ni].tRgnImgZoomOut);
		ptSrchRgnNormalSize			= &(ptSrchRgnInfo[ni].tRgnImgNormalSize);
		ptSrchRgnHLsrc				= &(ptSrchRgnInfo[ni].tRgnImgHLsrc);
		ptSrchRgnNormalHLsrc		= &(ptSrchRgnInfo[ni].tRgnImgNormalHLsrc);
		ptSrchRgnZoomOutHLChkEdge	= &(ptSrchRgnInfo[ni].tRgnImgZoomOutHLChkEdge);
		ptSrchRgnZoomOutHLsrc		= &(ptSrchRgnInfo[ni].tRgnImgZoomOutHLsrc);

		/* 通常 */
		ptSrchRgn->nSX = ptSrchRgnSrc[0L].nX - nprm_marginX4SrchRgn;
		ptSrchRgn->nSY = ptSrchRgnSrc[0L].nY - nprm_marginY4SrchRgn;
		ptSrchRgn->nEX = ptSrchRgnSrc[3L].nX + nprm_marginX4SrchRgn;
		ptSrchRgn->nEY = ptSrchRgnSrc[3L].nY + nprm_marginY4SrchRgn;
		/* 領域チェック */
		nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY, nXSizeImg, nYSizeImg );
		if ( CB_TRUE != nFlagChkRgn )
		{
			/* チェックNG時、領域再設定 */
			nRet = cb_FPE_ResetRegion( ptSrchRgn, nXSizeImg, nYSizeImg );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
		}

		/* サイドマーカー特徴点抽出用探索範囲 */
		/* ptSrchRgn と同一の値を設定する */
		ptSrchRgnNormalSize->nSX = ptSrchRgn->nSX;
		ptSrchRgnNormalSize->nSY = ptSrchRgn->nSY;
		ptSrchRgnNormalSize->nEX = ptSrchRgn->nEX;
		ptSrchRgnNormalSize->nEY = ptSrchRgn->nEY;

		/* 縮小後サイズ */
		nXSizeZoomOut = ( ( ptSrchRgn->nEX - ptSrchRgn->nSX ) + 1L ) / nprm_xMagZoomOut;
		nYSizeZoomOut = ( ( ptSrchRgn->nEY - ptSrchRgn->nSY ) + 1L ) / nprm_yMagZoomOut;

		/* 縮小版 */
		ptSrchRgnZoomOut->nSX = ptSrchRgn->nSX / nprm_xMagZoomOut;
		ptSrchRgnZoomOut->nSY = ptSrchRgn->nSY / nprm_yMagZoomOut;
		ptSrchRgnZoomOut->nEX = ( ptSrchRgnZoomOut->nSX + nXSizeZoomOut ) - 1L;
		ptSrchRgnZoomOut->nEY = ( ptSrchRgnZoomOut->nSY + nYSizeZoomOut ) - 1L;
		/* 通常版で領域チェック／丸め込みを行っているため、領域チェックは不要 */

		/* Haar-like考慮 */
		/* 市松(縮小時) */
		ptSrchRgnZoomOutHLChkEdge->nSX = ptSrchRgnZoomOut->nSX - nprm_xsizeBlockHLChkEdge;
		ptSrchRgnZoomOutHLChkEdge->nSY = ptSrchRgnZoomOut->nSY - nprm_ysizeBlockHLChkEdge;
		ptSrchRgnZoomOutHLChkEdge->nEX = ptSrchRgnZoomOut->nEX + nprm_xsizeBlockHLChkEdge;
		ptSrchRgnZoomOutHLChkEdge->nEY = ptSrchRgnZoomOut->nEY + nprm_ysizeBlockHLChkEdge;
		/* 領域チェック */
		nFlagChkRgn = cb_FPE_CheckRegion( ptSrchRgnZoomOutHLChkEdge->nSX, ptSrchRgnZoomOutHLChkEdge->nSY, ptSrchRgnZoomOutHLChkEdge->nEX, ptSrchRgnZoomOutHLChkEdge->nEY, nHalfXSizeImg, nHalfYSizeImg );
		if ( CB_TRUE != nFlagChkRgn )
		{
			/* チェックNG時、領域再設定 */
			nRet = cb_FPE_ResetRegion( ptSrchRgnZoomOutHLChkEdge, nHalfXSizeImg, nHalfYSizeImg );
			if ( CB_IMG_OK != nRet )
			{
				return ( CB_IMG_NG );
			}
		}

		/* 全体を含む領域(縮小版) */
		ptSrchRgnZoomOutHLsrc->nSX = ptSrchRgnZoomOutHLChkEdge->nSX;
		ptSrchRgnZoomOutHLsrc->nSY = ptSrchRgnZoomOutHLChkEdge->nSY;
		ptSrchRgnZoomOutHLsrc->nEX = ptSrchRgnZoomOutHLChkEdge->nEX;
		ptSrchRgnZoomOutHLsrc->nEY = ptSrchRgnZoomOutHLChkEdge->nEY;
		/* Haar-likeを考慮した領域チェック／丸め込みを行っているため、領域チェックは不要 */

		/* 全体を含む領域(通常) */
		nXSize = ( ( ptSrchRgnZoomOutHLsrc->nEX - ptSrchRgnZoomOutHLsrc->nSX ) + 1L ) * nprm_xMagZoomOut;
		nYSize = ( ( ptSrchRgnZoomOutHLsrc->nEY - ptSrchRgnZoomOutHLsrc->nSY ) + 1L ) * nprm_yMagZoomOut;
		ptSrchRgnHLsrc->nSX = ptSrchRgnZoomOutHLsrc->nSX * nprm_xMagZoomOut;
		ptSrchRgnHLsrc->nSY = ptSrchRgnZoomOutHLsrc->nSY * nprm_yMagZoomOut;
		ptSrchRgnHLsrc->nEX = ( ptSrchRgnHLsrc->nSX + nXSize ) - 1L;
		ptSrchRgnHLsrc->nEY = ( ptSrchRgnHLsrc->nSY + nYSize ) - 1L;
		/* Haar-likeを考慮した領域チェック／丸め込みを行っているため、領域チェックは不要 */

		/* サイドマーカー特徴点抽出用探索範囲 */
		/* ptSrchRgnHLsrc と同一の値を設定する */
		ptSrchRgnNormalHLsrc->nSX = ptSrchRgnHLsrc->nSX;
		ptSrchRgnNormalHLsrc->nSY = ptSrchRgnHLsrc->nSY;
		ptSrchRgnNormalHLsrc->nEX = ptSrchRgnHLsrc->nEX;
		ptSrchRgnNormalHLsrc->nEY = ptSrchRgnHLsrc->nEY;

	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		領域チェック
 *
 * @param[in]	nSxRgn				:,始点x座標,?<=value<=?,[pixel],
 * @param[in]	nSyRgn				:,始点y座標,?<=value<=?,[pixel],
 * @param[in]	nExRgn				:,終点x座標,?<=value<=?,[pixel],
 * @param[in]	nEyRgn				:,終点y座標,?<=value<=?,[pixel],
 * @param[in]	nXSizeImg			:,画像メモリのX方向サイズ,0<value<?,[pixel],
 * @param[in]	nYSizeImg			:,画像メモリのY方向サイズ,0<value<?,[pixel],
 *
 * @retval		CB_TRUE				:,TRUE,value=1,[-],
 * @retval		CB_FALSE			:,FALSE,value=0,[-],
 *
 * @date		2013.08.20	K.Kato			新規作成
 */
/******************************************************************************/
static ulong cb_FPE_CheckRegion( slong nSxRgn, slong nSyRgn, slong nExRgn, slong nEyRgn, slong nXSizeImg, slong nYSizeImg )
{
	ulong	nFlagChk = CB_TRUE;

	/* 始点チェック */
	if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nSxRgn, nSyRgn, 0L, 0L, ( nXSizeImg - 1L ), ( nYSizeImg - 1L ) ) )
	{
		nFlagChk = CB_FALSE;
	}
	/* 終点チェック */
	if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nExRgn, nEyRgn, 0L, 0L, ( nXSizeImg - 1L ), ( nYSizeImg - 1L ) ) )
	{
		nFlagChk = CB_FALSE;
	}
	/* 始終点チェック */
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
 * @brief		領域再設定
 *
 * @param[in,out]	ptSetRgn		:,再設定領域ポインタ,-,[-],
 * @param[in]		nXSizeImg		:,画像メモリのX方向サイズ,0<value<?,[pixel],
 * @param[in]		nYSizeImg		:,画像メモリのY方向サイズ,0<value<?,[pixel],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2017.10.12	A.Honda			新規作成
 */
/******************************************************************************/
static slong cb_FPE_ResetRegion( CB_RECT_RGN *ptSetRgn, slong nXSizeImg, slong nYSizeImg )
{
	slong	minSX = 0L;
	slong	minSY = 0L;
	slong	maxEX = nXSizeImg - 1L;
	slong	maxEY = nYSizeImg - 1L;
	ulong	nFlagChkRgn = CB_TRUE;

	/* 引数チェック */
	if ( NULL == ptSetRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}
	/* 始点チェック */
	if( minSX > ptSetRgn->nSX )
	{
		ptSetRgn->nSX = minSX;
	}
	if( minSY > ptSetRgn->nSY )
	{
		ptSetRgn->nSY = minSY;
	}
	/* 終点チェック */
	if( maxEX < ptSetRgn->nEX )
	{
		ptSetRgn->nEX = maxEX;
	}
	if( maxEY < ptSetRgn->nEY )
	{
		ptSetRgn->nEY = maxEY;
	}
	/* 再設定範囲チェック */
	nFlagChkRgn = cb_FPE_CheckRegion( ptSetRgn->nSX, ptSetRgn->nSY, ptSetRgn->nEX, ptSetRgn->nEY, nXSizeImg, nYSizeImg );
	if ( CB_TRUE != nFlagChkRgn )
	{
		return ( CB_IMG_NG );
	}
	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		特徴点抽出実行条件チェック
 *
 * @param[in]	nFlagInit			:,初期化フラグ,0<=value<=1,[-],
 * @param[in]	ptSrchRgnInfo		:,探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_TRUE				:,TRUE,value=1,[-],
 * @retval		CB_FALSE			:,FALSE,value=0,[-],
 * @retval		CB_IMG_NG			:,異常終了,value=-1,[-],
 *
 * @date		2013.06.03	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_CheckExecCondition( slong nFlagInit, const CB_FPE_SRCH_RGN_INFO_EACH_CAM* const ptSrchRgnInfo )
{
	slong	nFlagExec = CB_FALSE;

	/* 引数チェック */
	if ( NULL == ptSrchRgnInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nFlagExec = CB_FALSE;
	/* 初期化状態に関するチェック */
	if ( CB_TRUE != nFlagInit )
	{
		return ( nFlagExec );
	}
	/* 探索領域設定状態(市松パターン)に関するチェック */
	if ( CB_TRUE != ptSrchRgnInfo->nFlagValidInfo4Pattern )
	{
		return ( nFlagExec );
	}
	/* 探索領域設定状態(線丸パターン)に関するチェック */
	if ( CB_TRUE != ptSrchRgnInfo->nFlagValidInfo4AddPattern )
	{
		return ( nFlagExec );
	}

	/* 実行可能状態 */
	nFlagExec = CB_TRUE;

	return ( nFlagExec );
}

/******************************************************************************/
/**
 * @brief			抽出結果のクリア
 *
 * @param[out]		ptResult		:,特徴点抽出出力情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.08.04	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_ClearResult( CB_IMG_RESULT* const ptResult )
{
	slong					ni = 0L;
	t_cb_img_CenterPosEx	*ptCenterPosEx = NULL;
	t_cb_img_CenterPos		*ptCenterPos = NULL;

	/* 引数チェック */
	if ( NULL == ptResult )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	ptCenterPosEx = &( ptResult->tFtrPoint );
	/* 特徴点数 */
	ptCenterPosEx->CenterNum	= 0UL;
	ptCenterPosEx->LeftNum		= 0UL;
	ptCenterPosEx->RightNum		= 0UL;
	/* 座標 */
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
 * @brief			Checkerboardにおける特徴点抽出
 *
 * @param[in]		nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptFPEInfo		:,内部情報構造体へのポインタ,-,[-],
 * @param[out]		nCamDirection	:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]		nCalibType		:,キャリブ種別(工場・販社),CB_CALIB_REAR_KIND_FACTORY<=value<=CB_CALIB_REAR_KIND_RUNTIME,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.08	K.Kato			新規作成
 * @date			2016.06.24	M.Ando			エンブレムによるケラレ対応
 *
 * @note			引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	/* パラメータ */
	slong	nprm_nMaxNumPnt = 0L;
	slong	nprm_thrHaarFtrCrossPnt = 0L;	/* 市松ターゲット内部の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値格納用 */

	/* 引数チェック */
	if ( NULL == ptFPEInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_FTR_CROSS_PNT;

	/* 未使用変数回避 ワーニング対策 */
	CALIB_UNUSED_VARIABLE( nCalibType );	/* 変数使用時には削除してください */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.3  R-53, ID-6724

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 先頭ポインタ */
	ptSrchRgnInfo = &(ptFPEInfo->tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfo4Pattern[0]);
	ptSrchRsltInfo = &(ptFPEInfo->tRslt.tSrchRsltInfo[nCamDirection]);

	/* パラメータ取得 */
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
		/* エラー情報クリア */
		return ( CB_IMG_NG );
	}

	/* クリア */
	nRet = cb_FPE_ClearSearchResultPointInfo( CB_FPE_NELEMS(ptSrchRsltInfo->tSrchRsltPntInfo), &(ptSrchRsltInfo->tSrchRsltPntInfo[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_INTEGRAL_IMG] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Integral Image作成 */
	nRet = cb_FPE_IntegralImage( &(ptFPEInfo->nImg[0]), &( ptSrchRgnInfo->tRgnImgZoomOutHLsrc), CB_FPE_PRM_INTIMG_FLG_CENTER );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_INTEGRAL_IMG );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_INTEGRAL_IMG] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like特徴量用の画像メモリ割り当て(画像メモリアクセス開始) */
	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		return ( CB_IMG_NG_IMPLIB );
	}
	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_ClearIPError();
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		return ( CB_IMG_NG_IMPLIB );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_CHK_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like(市松) */
	nRet = cb_FPE_HaarLikeChkEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &(ptSrchRgnInfo->tRgnImgZoomOutHLChkEdge) );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_HAAR_LIKE_FTR_CHK_EDGE );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_CHK_EDGE] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_VERT_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like(縦エッジ) */
	nRet = cb_FPE_HaarLikeVertEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), &(ptSrchRgnInfo->tRgnImgZoomOutHLVertEdge) );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_HAAR_LIKE_FTR_VERT_EDGE );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_VERT_EDGE] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_HAAR_LIKE_HORI_EDGE] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like(横エッジ) */
	nRet = cb_FPE_HaarLikeHoriEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), &(ptSrchRgnInfo->tRgnImgZoomOutHLHoriEdge) );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
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

	/* 十字点探索 */
	nRet = cb_FPE_SearchCandCrossPnt( &(ptFPEInfo->nImg[0]), &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt), 
									&(ptSrchRgnInfo->tRgnImgZoomOut), nprm_nMaxNumPnt, nCamDirection, nprm_thrHaarFtrCrossPnt, &(ptSrchRgnInfo->tPtnInfo), ptSrchRgnInfo->tPtnInfo.nDoubleType );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
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

	/* カメラ位置がLeft or Rightの場合、右側/左側T字点探索 */
	if ( ( E_CB_SELFCALIB_CAMPOS_LEFT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_RIGHT == nCamDirection ) )
	{
	
		if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != (ptSrchRgnInfo->tPtnInfo).nTypePattern )
		{

#ifdef TIME_MEASUREMENT_CALIB
			RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_RI] );
#endif /* TIME_MEASUREMENT_CALIB */

			/* 右側Ｔ字点探索 */
			nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt, 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntRightHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntRight), 
											&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_RIGHT_EDGE_POS_PATTERN );
			if ( CB_IMG_OK != nRet )
			{
				/* Openした画像メモリをCloseする */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG_SRCH_CAND_T_PNT_RIGHT );
			}
		
#ifdef TIME_MEASUREMENT_CALIB
			RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_RI] );
			RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LE] );
#endif /* TIME_MEASUREMENT_CALIB */

			/* 左側Ｔ字点探索 */
			nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt, 
											&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLeftHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLeft), 
											&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_LEFT_EDGE_POS_PATTERN );
			if ( CB_IMG_OK != nRet )
			{
				/* Openした画像メモリをCloseする */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG_SRCH_CAND_T_PNT_LEFT );
			}
		
#ifdef TIME_MEASUREMENT_CALIB
		RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LE] );
#endif /* TIME_MEASUREMENT_CALIB */

			/* ベース点追加 */
			/* UPPER */
			m_FPE_tWorkInfo.tBasePnt4Upper[m_FPE_tWorkInfo.nNumBasePnt4Upper] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tTPntLeftHalf[0];
			m_FPE_tWorkInfo.nNumBasePnt4Upper++;
			/* ベース点設定 */
			for ( ni = 0L; ni < ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt; ni++ )
			{
				m_FPE_tWorkInfo.tBasePnt4Upper[m_FPE_tWorkInfo.nNumBasePnt4Upper + ni] = ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].tCrossPntHalf[ni];
			}
			m_FPE_tWorkInfo.nNumBasePnt4Upper += ptSrchRsltInfo->tSrchRsltPntInfo[CB_PATTERN_POS_CENTER].nNumCrossPnt;
			/* LOWER */
			/* ベース点設定 */
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
			/* アルゴリズムタイプ */
			nRet = cb_FPE_GetTypeAlgorithm( &( ptSrchRgnInfo->tPtnInfo ), &nTypeAlgorithm );
			if ( CB_IMG_OK != nRet )
			{
				/* Openした画像メモリをCloseする */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG );
			}
			
			if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )
			{
				/* 右側Ｔ字点探索 */
				nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[1]), 1L, 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntRightHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntRight), 
												&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_RIGHT_EDGE_POS_PATTERN );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
					return ( CB_IMG_NG_SRCH_CAND_T_PNT_RIGHT );
				}

				/* 結果領域のアドレスを取得 */
				ptPntStore[0]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0] );
				pnNumPntStore[0]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt );
				ptPntStore[1]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntRightHalf[0] );
				pnNumPntStore[1]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntRight );
			}
			else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
			{
				/* 左側Ｔ字点探索 */
				nRet = cb_FPE_SearchCandTPntLR( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]), 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0]), 1L, 
												&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLeftHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLeft), 
												&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_LEFT_EDGE_POS_PATTERN );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
					nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
					return ( CB_IMG_NG_SRCH_CAND_T_PNT_LEFT );
				}

				/* 結果領域のアドレスを取得 */
				ptPntStore[0]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLeftHalf[0] );
				pnNumPntStore[0]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLeft );
				ptPntStore[1]		= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].tCrossPntHalf[0] );
				pnNumPntStore[1]	= &( ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumCrossPnt );
			}
			else
			{
				/* Openした画像メモリをCloseする */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG );
			}

			/* ベース点追加 */
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
				/* Openした画像メモリをCloseする */
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
				nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
				return ( CB_IMG_NG );
			}
		}
	}
	else
	{
		/* ベース点設定 */
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

	/* 下側T字点探索 */
	nRet = cb_FPE_SearchCandTPntUL( &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), 
									&(m_FPE_tWorkInfo.tBasePnt4Lower[0]), m_FPE_tWorkInfo.nNumBasePnt4Lower, 
									&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntLowerHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntLower), 
									&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_LOWER_EDGE_POS_PATTERN );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_SRCH_CAND_T_PNT_LOWER );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_LO] );
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_UP] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* 上側Ｔ字点探索 */
	nRet = cb_FPE_SearchCandTPntUL( &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]), 
									&(m_FPE_tWorkInfo.tBasePnt4Upper[0]), m_FPE_tWorkInfo.nNumBasePnt4Upper, 
									&(ptSrchRsltInfo->tSrchRsltPntInfo[0].tTPntUpperHalf[0]), &(ptSrchRsltInfo->tSrchRsltPntInfo[0].nNumTPntUpper), 
									&(ptSrchRgnInfo->tRgnImgZoomOut), CB_FPE_UPPER_EDGE_POS_PATTERN );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
		nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
		return ( CB_IMG_NG_SRCH_CAND_T_PNT_UPPER );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_SRCH_T_PNT_UP] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* Haar-like特徴量用の画像メモリ割り当て解除(画像メモリアクセス終了) */
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE] );
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_VERT_EDGE] );
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_HORI_EDGE] );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 拡大(座標値) */
	nRet = cb_FPE_ZoomIn4Coordinates( &(ptSrchRsltInfo->tSrchRsltPntInfo[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG_ZOOM_IN_CAND_COORD );
	}


#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_PREC_POS_CHK] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* 高精度位置決め */
	nRet = cb_FPE_PrecisionPositioning4Checkerboard( nImgSrc, &(ptSrchRsltInfo->tSrchRsltPntInfo[0]), ptSrchRgnInfo, nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		/* 内部関数のエラーコードをそのまま返すこと!(エラー状態を詳細に切り分けるため) */
		return ( nRet );
	}
	
#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_FINISH, &RG_Tim_CALIB[E_CALIB_TIME_PREC_POS_CHK] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			円＆直線パターンにおける特徴点抽出
 *
 * @param[in]		nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptFPEInfo		:,内部情報構造体へのポインタ,-,[-],
 * @param[out]		nCamDirection	:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]		nCalibType		:,キャリブ種別(工場・販社),CB_CALIB_REAR_KIND_FACTORY<=value<=CB_CALIB_REAR_KIND_RUNTIME,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.10	K.Kato			新規作成
 *
 * @note			引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	/* パラメータ */
	slong	nprm_cntRetryCircleLineMaxNum = 0L;
	slong	nprm_retryDiff4RRFdiffIntensity = 0L;

#ifndef	_WINDOWS
	ER	ercd = E_OK;
#endif	// _WINDOWS

	/* 引数チェック */
	if ( NULL == ptFPEInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 未使用変数回避 ワーニング対策 */
	CALIB_UNUSED_VARIABLE( nCalibType );	/* 変数使用時には削除してください */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.3  R-53, ID-6725

	/* パラメータ取得 */
	nprm_cntRetryCircleLineMaxNum = CB_FPE_CNT_RETRY_CIRCLE_LINE_MAXNUM;
	nprm_retryDiff4RRFdiffIntensity = CB_FPE_PRM_RRF_DIFF_INTENSITY_RETRY_DIFF;

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 先頭ポインタ */
	ptSrchRgnInfo = &(ptFPEInfo->tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]);
	ptSrchRsltPntInfo = &(ptFPEInfo->tRslt.tSrchRsltInfo[nCamDirection].tSrchRsltAddPntInfo[0]);

	/* クリア */
	nRet = cb_FPE_ClearSearchResultAddPointInfo( ptSrchRsltPntInfo );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_SMOOTH] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* 平滑化 */
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

	/* パラメータ設定 */
	m_FPE_tPrm.nDiffIntensity4Retry = CB_FPE_PRM_RRF_DIFF_INTENSITY;
	/* フラグ設定 */
	nFlagProcOK = CB_FALSE;
	for ( nCntRetry = 0L; nCntRetry < nprm_cntRetryCircleLineMaxNum; nCntRetry++ )
	{
		/* Radial Reach特徴 */
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

		/* 円領域の抽出 */
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
			/* 特徴点が存在しないという状態以外は、ここでRETURNする */
			if ( CB_IMG_NG_NO_CIRCLE_FTR_PNT != nRet )
			{
				return ( CB_IMG_NG_EXTRACT_CIRC_RGN );
			}
		}

		/* パラメータ設定(リトライ時) */
		m_FPE_tPrm.nDiffIntensity4Retry = m_FPE_tPrm.nDiffIntensity4Retry + nprm_retryDiff4RRFdiffIntensity;

#ifndef _WINDOWS
		ercd = cl_dly_tsk( 30UL );		/* 処理毎に30msのディレイを挿入 */
		if( E_OK != ercd )
		{
			/* Not Operation */	
		}
#endif	//_WINDOWS

	}	/* for ( nCntRetry ) */

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			Checkerboard用特徴点抽出情報のクリア
 *
 * @param[in]		nLoopIdx			:,初期化する構造体の配列数,-,[-],
 * @param[out]		ptSrchRsltPntInfo	:,探索結果情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.10	K.Kato			新規作成
 * @date			2013.11.22	F.Sano			引数nLoopIdxを追加(初期化する配列サイズを可変に変更)
 */
/******************************************************************************/
static slong cb_FPE_ClearSearchResultPointInfo( slong nLoopIdx, CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo )
{
	slong			ni = 0L, 
					nj = 0L;
	CB_IMG_POINT	*ptImgPnt = NULL;
	t_cb_img_CenterPos	*ptImgPntPrecise = NULL;

	/* 引数チェック */
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
 * @brief			円＆直線パターン用特徴点抽出情報のクリア
 *
 * @param[out]		ptSrchRsltPntInfo	:,探索結果情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.10	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_ClearSearchResultAddPointInfo( CB_FPE_SRCH_RSLT_ADD_PNT_INFO* ptSrchRsltPntInfo )
{
	slong			ni = 0L, 
					nj = 0L;
	CB_IMG_POINT	*ptImgPnt = NULL;
	t_cb_img_CenterPos	*ptCenterPntPrecise = NULL;

	/* 引数チェック */
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
 * @brief		Integral Image作成
 *
 * @param[in,out]	pnImgWk			:,ワーク画面IDへのポインタ,-,[-],
 * @param[in]		ptSrchRgnInfo	:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.16	K.Kato			新規作成
 * @date		2015.08.26	S.Morita		サイドマーカー画像サイズ修正対応
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

	/* 引数チェック */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}


	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, ptSrchRgn->nSX, ptSrchRgn->nSY, ptSrchRgn->nEX, ptSrchRgn->nEY );

	if( CB_FPE_PRM_INTIMG_FLG_CENTER == nIntgralImgFlg )
	{
		/* Integral Image(縮小) */
		nRet = implib_IP_IntegralImage( pnImgWk[CB_FPE_IMG_ZOOMOUT2x2], pnImgWk[CB_FPE_INTEGRAL_IMG_ZOOMOUT2x2] );
	}
	else if( CB_FPE_PRM_INTIMG_FLG_SIDE == nIntgralImgFlg )
	{
		/* Integral Image(入力画像と同じサイズ) */
		nRet = implib_IP_IntegralImage( pnImgWk[CB_FPE_IMG_NORMAL], pnImgWk[CB_FPE_INTEGRAL_IMG_NORMAL] );
	}
	else
	{
		nRet = CB_IMG_NG;
	}

	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		平滑化
 *
 * @param[in]	nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	pnImgWk			:,ワーク画面IDへのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,カメラごとの探索領域情報構造体へのポインタ(追加パターン),-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.11	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_Smoothing( const IMPLIB_IMGID nImgSrc, const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6727
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
	/* パラメータ */
	slong	nprm_coeffSmoothFLT[9] = { CB_FPE_PRM_COEFF_SMOOTH_FLT };
	slong	nprm_scaleSmoothFLT = CB_FPE_PRM_SCALE_SMOOTH_FLT;

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 引数チェック */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* 先頭ポインタ */
		ptRgn = &(ptSrchRgnInfo[ni].tRgnImgRRF);

		/* 平滑化 */
		nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ( ptRgn->nSX - 1L ), ( ptRgn->nSY - 1L ), ( ptRgn->nEX + 1L ), ( ptRgn->nEY + 1L ) );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ( ptRgn->nSX - 1L ), ( ptRgn->nSY - 1L ), ( ptRgn->nEX + 1L ), ( ptRgn->nEY + 1L ) );
		nRet = implib_IP_SmoothFLT( nImgSrc, pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], nprm_scaleSmoothFLT, &(nprm_coeffSmoothFLT[0]) );
		if ( 0L > nRet )
		{
			/* エラー情報クリア */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}
	}

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		放射リーチ特徴
 *
 * @param[in]	nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	pnImgWk			:,ワーク画面IDへのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,カメラごとの探索領域情報構造体へのポインタ(追加パターン),-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.28	K.Kato			新規作成
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
											{ +0L, -1L },	/* [0]上 */
											{ +1L, -1L },	/* [1]右上 */
											{ +1L, +0L },	/* [2]右 */
											{ +1L, +1L },	/* [3]右下 */
											{ +0L, +1L },	/* [4]下 */
											{ -1L, +1L },	/* [5]左下 */
											{ -1L, +0L },	/* [6]左 */
											{ -1L, -1L },	/* [7]左上 */
										};
	uchar				*pnAddrPixelArray[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	uchar				*pnTopAddrPixel = NULL, 
						*pnAddrPixel = NULL;
//	uchar				*pnTopAddrPixelSmooth = NULL, 
//						*pnAddrPixelSmooth = NULL;
	uchar				*pnTopAddrPixelDst = NULL, 
						*pnAddrPixelDst = NULL;
	CB_RECT_RGN			*ptSrchRgn = NULL;
	/* パラメータ */
	slong	nprm_lengthReach = CB_FPE_PRM_RRF_LENGTH_REACH;
	slong	nprm_diffIntensity = m_FPE_tPrm.nDiffIntensity4Retry;
	slong	nprm_startReach = CB_FPE_PRM_RRF_START_REACH;
	slong	nprm_thrReach4Mag = CB_FPE_PRM_RRG_THR_REACH_FOR_MAG;

	/* 引数チェック */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nImgSrc, &nXSizeImg, &nYSizeImg, (void**)&pnTopAddrPixel );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
#if 0
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], &nXSizeImgSmooth, &nYSizeImgSmooth, (void**)&pnTopAddrPixelSmooth );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();	/* implib_CloseImgDirect()発行のために、エラーをクリア */
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
#endif
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], &nXSizeImgDst, &nYSizeImgDst, (void**)&pnTopAddrPixelDst );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();	/* implib_CloseImgDirect()発行のために、エラーをクリア */
//		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}

	for ( nm = 0L; nm < CB_ADD_PATTERN_POS_MAXNUM; nm++ )	/* パターンの数でループ */
	{
		ptSrchRgn = &(ptSrchRgnInfo[nm].tRgnImgRRF);
		nXSizeSrchRgn = ( ptSrchRgn->nEX - ptSrchRgn->nSX ) + 1L;
		nYSizeSrchRgn = ( ptSrchRgn->nEY - ptSrchRgn->nSY ) + 1L;

		for ( ni = nprm_lengthReach; ni < ( nYSizeSrchRgn - nprm_lengthReach ); ni++ )	/* Y方向 */
		{
			/* アクセス開始アドレス */
//			pnAddrPixelSmooth = pnTopAddrPixelSmooth + ( ( ptSrchRgn->nSY + ni ) * nXSizeImgSmooth ) + ( ptSrchRgn->nSX + nprm_lengthReach );
			pnAddrPixel = pnTopAddrPixel + ( ( ptSrchRgn->nSY + ni ) * nXSizeImg ) + ( ptSrchRgn->nSX + nprm_lengthReach );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6732
			pnAddrPixelDst = pnTopAddrPixelDst + ( ( ptSrchRgn->nSY + ni ) * nXSizeImgDst ) + ( ptSrchRgn->nSX + nprm_lengthReach );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6733

#if 0
			for ( nj = nprm_lengthReach; nj < ( nXSizeSrchRgn - nprm_lengthReach ); nj++ )	/* X方向 */
#else
			nOffset = 1L;
			for ( nj = nprm_lengthReach; nj < ( nXSizeSrchRgn - nprm_lengthReach ); nj+=nOffset )	/* X方向 */
#endif
			{
				/* 中心付近の平均 */
				nAveValue = (slong)(*pnAddrPixel);

				/* 初期化 */
				nCntReachOK = 0L;
				memset( &(nFlgReachOK[0]), 0x00, ( 8UL * sizeof( slong ) ) );
				memset( &(nLenReach[0]), 0x00, ( 8UL * sizeof( slong ) ) );
				
				for ( nCntReach = nprm_startReach; nCntReach <= nprm_lengthReach; nCntReach++ )
				{
					for ( nk = 0L; nk < 8L; nk++ )
					{
						/* 参照先更新 */
						pnAddrPixelArray[nk] = pnAddrPixel + ( ( nCntReach * tVecArray[nk].nY ) * nXSizeImg ) + ( nCntReach * tVecArray[nk].nX );

						/* 輝度値の差をチェック */
						if ( ( CB_FALSE == nFlgReachOK[nk] ) && ( ( nAveValue + nprm_diffIntensity ) < (slong)(*(pnAddrPixelArray[nk])) ) )
						{
							nFlgReachOK[nk] = CB_TRUE;
							nLenReach[nk] = nCntReach;
							nCntReachOK++;
						}
					}
				}

				/* 2013.06.11 K.Kato add */
				if ( 0L == nm )	/* 左 */
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

				/* 放射カウンタを格納 */
				*pnAddrPixelDst = (uchar)nCntReachOK;

				/* X方向にシフト */
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
					pnAddrPixel++;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6734
					pnAddrPixelDst++;
				}
#endif
			}
		}
	}
	
	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nImgSrc );
//	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );

	/* 画面データタイプを変更 */
	nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], IMPLIB_UNSIGN8_DATA );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		円領域の中点抽出
 *
 * @param[in]	nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	pnImgWk			:,ワーク画面IDへのポインタ,-,[-],
 * @param[out]	ptSrchRsltInfo	:,探索結果情報構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,探索領域情報構造体へのポインタ,-,[-],
 * @param[in]	nCamDirection	:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.01	K.Kato			新規作成
 * @date		2017.10.04	A.Honda			線丸の検知能力向上(#2-案2)
 *
 * @note		引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	float_t	fUpperLineLength;			/* 上用近似直線長さ(d_1) */
	float_t	fLowerLineLength;			/* 下用近似直線長さ(d_2) */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY													/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
	CB_FPE_CBR_INPUT			tBrInput;	
	CB_FPE_CBR_POINT_INFO		*ptBrPointTbl = NULL;                                   /* 輝度判定用座標情報                 */
	CB_FPE_IMG_ACCESS_INFO		tImgRRFAccessInfo;
	slong						nBrTbl[256]              = { 0L };                      /*                                    */
	slong						nIdTbl[256]              = { 0L };                      /* ★                                 */
	slong						nWidthUpper              = 0L;                          /*                                    */
	slong						nWidthLower              = 0L;                          /*                                    */
	slong						nWidthMiddle             = 0L;                          /*                                    */
	slong						nIdxLinePart             = 0L;                          /* ★                                 */
	slong						nSrchUpperMinX           = 0L;                          /* QAC対応 上側丸探索最小X            */
	slong						nSrchLowerMinX           = 0L;                          /* QAC対応 下側丸探索最小X            */
	slong						nSrchUpperMaxX           = 0L;                          /* QAC対応 上側丸探索最大X            */
	slong						nSrchLowerMaxX           = 0L;                          /* QAC対応 下側丸探索最大X            */
	slong						nSrchMinX                = 0L;                          /* QAC対応 処理領域外チェック用変数   */
	slong						nSrchMaxX                = 0L;                          /* QAC対応 処理領域外チェック用変数   */
	uchar						nIdxUpperCircle;                                        /* 丸候補インデックス(上端)           */
	uchar						nIdxLowerCircle;                                        /* 丸候補インデックス(下端)           */
	uchar						nIdxLine;                                               /* 線候補インデックス                 */
	float_t						fSimilarity              = 0.0F;                        /*                                    */
	float_t						fContrast                = 0.0F;                        /*                                    */
	float_t						fSlopeUpper,fSlopeLower  = 0.0F;                        /*                                    */
	float_t 					fMaxScore                = 0.0F;                        /*                                    */
	float_t						fShiftXUpper             = 0.0F;                        /* ★                                 */
	float_t						fShiftXLower             = 0.0F;                        /* ★                                 */
	float_t						fSumBlackArea            = 0.0F;                        /* ★                                 */
	float_t						fScoreTbl[256]           = { 0.0F };                    /* ★                                 */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R1.2.1  R-31, ID-6735
	float_t						fSumDistance             = 0.0F;                        /* ★                                 */
	float_t						fDiffXUpper, fDiffYUpper, fDiffXLower, fDiffYLower;     /*                                    */
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */												/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

	/* パラメータ */
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

	/* 引数チェック */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRsltInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 未使用変数回避 ワーニング対策 */
	CALIB_UNUSED_VARIABLE( nCamDirection );	/* 変数使用時には削除してください */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.3  R-53, ID-6736

	/* パラメータ取得 */
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 先頭ポインタ */
	pnTblLabelMinX = &(m_FPE_tWorkInfo.nTblLabelMinX[0]);
	pnTblLabelMaxX = &(m_FPE_tWorkInfo.nTblLabelMaxX[0]);
	pnTblLabelMinY = &(m_FPE_tWorkInfo.nTblLabelMinY[0]);
	pnTblLabelMaxY = &(m_FPE_tWorkInfo.nTblLabelMaxY[0]);
	pnTblAreaLabel = &(m_FPE_tWorkInfo.nTblAreaLabel[0]);
	ptLOGravTbl = &(m_FPE_tWorkInfo.tTblGravity[0]);
	ptLinePartInfo = &(m_FPE_tWorkInfo.tLinePartInfo[0L]);
	ptCircleLineComb = &(m_FPE_tWorkInfo.tCircleLineComb[0L]);
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																	/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
	ptBrPointTbl = &(tBrInput.tBrPointTbl[0]);
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																	/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
	/* 0クリア */
	memset( &tBrInput, 0x00, sizeof( CB_FPE_CBR_INPUT ) );
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgSrcAccessInfo.nXSize ), &( tImgSrcAccessInfo.nYSize ), (void**)&( tImgSrcAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* 処理領域 */
		ptRectRgn = &(ptSrchRgnInfo[ni].tRgnImg);
		/* 候補点格納先 */
		ptCandPoint = &(ptSrchRsltInfo[ni].tCenterPnt[0]);
		/* クリア */
		nCntCandPntUpper = 0L;
		nCntCandPntLower = 0L;
		memset( pnTblLabelMinX, 0x00, ( 256UL * sizeof( slong ) ) );
		memset( pnTblLabelMaxX, 0x00, ( 256UL * sizeof( slong ) ) );
		memset( pnTblAreaLabel, 0x00, ( 256UL * sizeof( slong ) ) );
		nDistanceCandPntUpper = ( 720 * 720 ) + ( 480 * 480 );
		nDistanceCandPntLower = ( 720 * 720 ) + ( 480 * 480 );

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY		/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
		m_FPE_tDbgInfo.nNum[ni] = 0L;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */	/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

		/* しきい値処理して2値化 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_Binarize( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], nprm_cntReach );

		/* ノイズ除去(仮ラベル付けでのoverflowを防止するため追加 2013.09.30 K.Kato) */
//		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
//		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = IP_PickNoise4( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );

		/* [DEBUG] */
#ifdef CB_FPE_CSW_DEBUG_ON
//		cb_FPE_DBG_SavePixelValue( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], "C:/work/work_kato/SELF_CB_DBG_RRF_IMG_withPickNoise.csv" );
#endif /* CB_FPE_CSW_DEBUG_ON */

		/* Labeling */
		/* Window設定 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );

		/* パラメータ設定 */
		nprm_thrminLabelArea = CB_FPE_PRM_THR_MIN_LABEL_AREA;
		nFlagProcOK = CB_FALSE;
		for ( nCntRetry = 0L; nCntRetry < nprm_cntRetryLabelingMaxNum; nCntRetry++ )
		{	
			nNumLabel = implib_IP_Label8withAreaFLT( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], nprm_thrminLabelArea, nprm_thrmaxLabelArea, IMPLIB_LABEL_OBJ );
			if ( CB_FPE_CODE_LABEL_OVERFLOW == nNumLabel )
			{
				nRet = implib_ClearIPError();
				/* リトライ時のしきい値設定 */
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
		/* ラベリング失敗の場合、特徴点位置が見つからなかった場合の振る舞いになるようにする */
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

		/* Labelごとに最小/最大x,y座標 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_ExtractLORegionX( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinX, pnTblLabelMaxX );
		nRet = implib_IP_ExtractLORegionY( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinY, pnTblLabelMaxY );

		/* Labelごと面積 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
//		nRet = implib_IP_ExtractLOArea( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblAreaLabel );
		nRet = implib_IP_ExtractLOGravity( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], ptLOGravTbl );

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY																				/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
		/* Labelごとに輝度値累計 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_SRC1_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_ProjectLabelGO( nImgSrc, pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], (int32_t*)(nBrTbl) );
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */																			/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

		/* IMPのエラーをまとめてチェック */
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		/*
		==============================
			線候補の抽出
		==============================
		*/

		/* 画像メモリアクセス開始 */
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
			if ( nprm_thrLineLabel <= ptLOGravTbl[nj].AREA )	/* 線候補として扱うラベルの面積チェック */
			{
				/* 上側スキャン */
				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				for ( nk = 0L; nk < nprm_scanHeightLineUL; nk++ )
				{
					/* スキャンするY座標設定 */
					nScanY = ( ptRectRgn->nSY + pnTblLabelMinY[nj] ) + nk;
					/* X方向にスキャン */
					nScanWidth = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
					pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6737
					for ( nl = 0L; nl < nScanWidth; nl++ )
					{
						if ( nj == (slong)(*pnWkAddrPixel) )
						{
							nSumX = nSumX + ( ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) + nl );
							nSumY = nSumY + nScanY;
							/* サンプル数 */
							nCntCoord++;
						}
						pnWkAddrPixel++;
					}	/* for ( nl ) */
				}	/* for ( nk ) */
				/* 代表座標 */
				if ( 0L < nCntCoord )	/* 0割チェック追加 */
				{
					fCoordUpper[0L] = (float_t)nSumX / (float_t)nCntCoord;
					fCoordUpper[1L] = (float_t)nSumY / (float_t)nCntCoord;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY														/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
					if ( 0L < nprm_scanHeightLineUL )	/* 0割チェック追加 */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6738	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6739
					{
						nWidthUpper = (slong)( nCntCoord / nprm_scanHeightLineUL );
					}
					else
					{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6740
						/* Openした画像メモリをCloseする */
						nRet = implib_CloseImgDirect( nImgSrc );
						nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
						/* 0割時の処理 */
						return ( CB_IMG_NG_DIV0 );
					}
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */													/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */
				}
				else
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					/* 0割時の処理 */
					return ( CB_IMG_NG_DIV0 );
				}


				/* 下側スキャン */
				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				for ( nk = 0L; nk < nprm_scanHeightLineUL; nk++ )
				{
					/* スキャンするY座標設定 */
					nScanY = ( ptRectRgn->nSY + pnTblLabelMaxY[nj] ) - nk;
					/* X方向にスキャン */
					nScanWidth = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
					pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) );
					for ( nl = 0L; nl < nScanWidth; nl++ )
					{
						if ( nj == (slong)(*pnWkAddrPixel) )
						{
							nSumX = nSumX + ( ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) + nl );
							nSumY = nSumY + nScanY;
							/* サンプル数 */
							nCntCoord++;
						}
						pnWkAddrPixel++;
					}	/* for ( nl ) */
				}	/* for ( nk ) */
				/* 代表座標 */
				if ( 0L < nCntCoord )	/* 0割チェック追加 */
				{
					fCoordLower[0L] = (float_t)nSumX / (float_t)nCntCoord;
					fCoordLower[1L] = (float_t)nSumY / (float_t)nCntCoord;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY														/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
					if ( 0L < nprm_scanHeightLineUL )	/* 0割チェック追加 */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6741	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6742
					{
						nWidthLower = (slong)( nCntCoord / nprm_scanHeightLineUL );
					}
					else
					{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6743
						/* Openした画像メモリをCloseする */
						nRet = implib_CloseImgDirect( nImgSrc );
						nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
						/* 0割時の処理 */
						return ( CB_IMG_NG_DIV0 );
					}
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */													/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */
				}
				else
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					/* 0割時の処理 */
					return ( CB_IMG_NG_DIV0 );
				}

				/* 中央スキャン */
				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				/* スキャンするY座標設定 */
				nScanY = ( ptRectRgn->nSY + pnTblLabelMinY[nj] ) + ( (pnTblLabelMaxY[nj] - pnTblLabelMinY[nj]) / 2L) + 1L;
				/* X方向にスキャン */
				nScanWidth = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
				pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6737
				for ( nl = 0L; nl < nScanWidth; nl++ )
				{
					if ( nj == (slong)(*pnWkAddrPixel) )
					{
						nSumX = nSumX + ( ( ptRectRgn->nSX + pnTblLabelMinX[nj] ) + nl );
						nSumY = nSumY + nScanY;
						/* サンプル数 */
						nCntCoord++;
					}
					pnWkAddrPixel++;
				}	/* for ( nl ) */
				/* 代表座標 */
				if ( 0L < nCntCoord )	/* 0割チェック追加 */
				{
					fCoordMiddle[0L] = (float_t)nSumX / (float_t)nCntCoord;
					fCoordMiddle[1L] = (float_t)nSumY / (float_t)nCntCoord;

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY														/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
					nWidthMiddle = nCntCoord;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */													/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */
				}
				else
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					/* 0割時の処理 */
					return ( CB_IMG_NG_DIV0 );
				}

				/* 上用近似直線(傾き・切片) */
				nRet = cb_FPE_CalcLineCoeff_F( &(fCoordUpper[0L]), &(fCoordMiddle[0L]), &( tUpperLineCoeff4Line.fSlope ), &( tUpperLineCoeff4Line.fIntercept ) );
				if ( CB_IMG_OK != nRet )
				{
					nRet = implib_ClearIPError();
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					return ( CB_IMG_NG );
				}

				/* 下近似直線(傾き・切片) */
				nRet = cb_FPE_CalcLineCoeff_F( &(fCoordMiddle[0L]), &(fCoordLower[0L]), &( tLowerLineCoeff4Line.fSlope ), &( tLowerLineCoeff4Line.fIntercept ) );
				if ( CB_IMG_OK != nRet )
				{
					nRet = implib_ClearIPError();
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					return ( CB_IMG_NG );
				}

				if ( CB_ADD_PATTERN_POS_LEFT == ni )
				{
					//if (1)// 0.0f > tLineCoeff4Line.fSlope )//変更
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

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY												/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
						ptLinePartInfo[nCntCandLine].nWidthUpper = nWidthUpper;
						ptLinePartInfo[nCntCandLine].nWidthLower = nWidthLower;
						ptLinePartInfo[nCntCandLine].nWidthMiddle = nWidthMiddle;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */											/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

						nCntCandLine++;
					//}
				}
				else
				{
					//if (1)// 0.0f < tLineCoeff4Line.fSlope )//変更
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

#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY												/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
						ptLinePartInfo[nCntCandLine].nWidthUpper = nWidthUpper;
						ptLinePartInfo[nCntCandLine].nWidthLower = nWidthLower;
						ptLinePartInfo[nCntCandLine].nWidthMiddle = nWidthMiddle;
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */											/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

						nCntCandLine++;
					//}
				}
			}
		}

		/* 画像メモリアクセス終了 */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		/*
		==============================
			丸候補の抽出
		==============================
		*/
		nCntCircleLineComb = 0L;
		for ( nj = 0L; nj < nCntCandLine; nj++ )
		{
			/* Index */
			nIdxCandLine = (slong)( ptLinePartInfo[nj].nIdxLine );

			for ( nIdxUpper = 1L; nIdxUpper <= nNumLabel; nIdxUpper++ )	/* 上側の丸 */
			{
				if ( nIdxCandLine == nIdxUpper )	/* 同じラベルでないか */
				{
					continue;
				}
				if ( nprm_thrCircleLabel < ptLOGravTbl[nIdxUpper].AREA )	/* 大きいものは対象にしない */
				{
					continue;
				}
				if ( pnTblLabelMinY[nIdxCandLine] <= pnTblLabelMaxY[nIdxUpper] )	/* 線の上側端点より下側にあるラベルは無視 */
				{
					continue;
				}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
				/* 黒丸探索領域外にあるラベルは無視 */
				if (   ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nX - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxUpper].X ) ) )
					|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnTL.nY - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxUpper].Y ) ) )
					|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nX + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxUpper].X ) ) )
					|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_UPPER].tCircleSrchRgnBR.nY + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxUpper].Y ) ) ) )
				{
					continue;
				}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */

				for ( nIdxLower = 1L; nIdxLower <= nNumLabel; nIdxLower++ )	/* 下側の丸 */
				{
					if ( ( nIdxCandLine == nIdxLower ) || ( nIdxUpper == nIdxLower ) )	/* 同じラベルでないか */
					{
						continue;
					}
					if ( nprm_thrCircleLabel < ptLOGravTbl[nIdxLower].AREA )	/* 大きいものは対象にしない */
					{
						continue;
					}
					if ( pnTblLabelMaxY[nIdxCandLine] >= pnTblLabelMinY[nIdxLower] )	/* 線の下側端点より上側にあるラベルは無視 */
					{
						continue;
					}

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
					/* 黒丸探索領域外にあるラベルは無視 */
					if (   ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nX - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxLower].X ) ) )
						|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnTL.nY - CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) > ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxLower].Y ) ) )
						|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nX + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSX + (slong)( ptLOGravTbl[nIdxLower].X ) ) )
						|| ( ( ptSrchRgnInfo[ni].tPntImgCircle[CB_ADD_PATTERN_PNT_POS_LOWER].tCircleSrchRgnBR.nY + CB_FPE_PRM_CIRCLE_SRCH_RGN_MARGIN ) < ( ptRectRgn->nSY + (slong)( ptLOGravTbl[nIdxLower].Y ) ) ) )
					{
						continue;
					}
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */

					if ( CB_ADD_PATTERN_POS_LEFT == ni )
					{
						if ( ptLOGravTbl[nIdxUpper].X <= ptLOGravTbl[nIdxLower].X )	/* X方向の位置関係 */
						{
							//continue;//変更
						}
					}
					else
					{
						if ( ptLOGravTbl[nIdxLower].X <= ptLOGravTbl[nIdxUpper].X )	/* X方向の位置関係 */
						{
							//continue;//変更
						}
					}

					/* X座標を推定 */
					fEstUpperX = ( ptLinePartInfo[nj].tUpperLineCoeff.fSlope * ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxUpper].Y ) ) + ptLinePartInfo[nj].tUpperLineCoeff.fIntercept;
					fEstLowerX = ( ptLinePartInfo[nj].tLowerLineCoeff.fSlope * ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxLower].Y ) ) + ptLinePartInfo[nj].tLowerLineCoeff.fIntercept;

					if (   ( fprm_thrDiffEstX < fabsf( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxUpper].X ) - fEstUpperX ) )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.8.1  R-33, ID-6744
						|| ( fprm_thrDiffEstX < fabsf( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLower].X ) - fEstLowerX ) ) )
					{
						continue;
					}

					/* 長さの比較 */
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

		/* 絞込み */
		nIdxCircleLineComb = -1L;
		fMinDiffDistance = sqrtf( ( 720.0f * 720.0f ) + ( 480.0f * 480.0f ) );
		fMinFarDistance = sqrtf( ( 720.0f * 720.0f ) + ( 480.0f * 480.0f ) );
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY											/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
		memset( &nIdTbl, 0xFF, ( 256UL * sizeof( slong ) ) );
		memset( &fScoreTbl, 0x00, (  256UL * sizeof( float_t ) ) );
		fSumDistance = sqrtf( ( 720.0f * 720.0f ) + ( 480.0f * 480.0f ) );

		/* RRF画像オープン */
		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], &( tImgRRFAccessInfo.nXSize ), &( tImgRRFAccessInfo.nYSize ), (void**)&( tImgRRFAccessInfo.pnAddrY8 ) );
		if ( 0L > nRet )
		{
			/* エラー情報クリア */
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

		/* ソース画像・RRF画像x座標幅を格納 */
		tBrInput.nImgSrcXSize = tImgSrcAccessInfo.nXSize;
		tBrInput.nImgRRFXSize = tImgRRFAccessInfo.nXSize;


#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */										/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

		for ( nj = 0L; nj < nCntCircleLineComb; nj++ )
		{
#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY											/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
			/*
			==============================
				輝度判定
			==============================
			*/
			/* 0クリア */
			fSimilarity	= 0.0F;
			fContrast	= 0.0F;

			/* インデックス設定 */
			nIdxUpperCircle = ptCircleLineComb[nj].nIdxUpperCircle;     /* 上端丸候補インデックス */
			nIdxLowerCircle = ptCircleLineComb[nj].nIdxLowerCircle;     /* 下端丸候補インデックス */
			nIdxLine = ptCircleLineComb[nj].nIdxLine;                   /* 線候補インデックス     */

			/* 差分値算出(上端) */
			fDiffXUpper = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxUpperCircle].X ) - ptLinePartInfo[nIdxLine].tUpperCenterPnt.fX;
			fDiffYUpper = ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxUpperCircle].Y ) - ptLinePartInfo[nIdxLine].tUpperCenterPnt.fY;
			/* 差分値算出(下端) */
			fDiffXLower = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLowerCircle].X ) - ptLinePartInfo[nIdxLine].tLowerCenterPnt.fX;
			fDiffYLower = ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[nIdxLowerCircle].Y ) - ptLinePartInfo[nIdxLine].tLowerCenterPnt.fY;

			/*--------------------------------*/
			/* 傾き設定                       */
			/*--------------------------------*/

			/* 傾き設定(上端) */
			if( CB_FPE_FLT_MIN > fabsf(fDiffXUpper) )
			{
				fSlopeUpper = (float_t) CB_FPE_MAX_SLOPE;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nFlagVertical = 1L;    /* 垂直状態設定 */
			}
			else
			{
				fSlopeUpper = fDiffYUpper / fDiffXUpper;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nFlagVertical = 0L;    /* 非垂直状態設定 */
			}

			/* 傾き設定(下端) */
			if( CB_FPE_FLT_MIN > fabsf(fDiffXLower) )
			{
				fSlopeLower = (float_t) CB_FPE_MAX_SLOPE;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nFlagVertical = 1L;    /* 垂直状態設定 */
			}
			else
			{
				fSlopeLower = fDiffYLower / fDiffXLower;
				ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nFlagVertical = 0L;    /* 非垂直状態設定 */
			}

			/*--------------------------------*/
			/* 輝度判定用座標情報設定         */
			/*--------------------------------*/

			fShiftXUpper = ( (float_t)pnTblLabelMaxY[nIdxUpperCircle] - ptLOGravTbl[nIdxUpperCircle].Y ) / fSlopeUpper;

			/* ラベル(丸候補)上端中心座標設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelUpperPnt.fX = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxUpperCircle].X ) + fShiftXUpper;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelUpperPnt.fY = (float_t)( ptRectRgn->nSY + pnTblLabelMaxY[nIdxUpperCircle] );

			/* ラベル(丸候補)下端中心座標設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelLowerPnt.fX = ptLinePartInfo[nIdxLine].tUpperCenterPnt.fX;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelLowerPnt.fY = ptLinePartInfo[nIdxLine].tUpperCenterPnt.fY;

			/* 輝度判定探索領域幅設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nSrchWidth = ptLinePartInfo[nIdxLine].nWidthUpper;

			/* 輝度判定探索ライン傾き設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].fSrchSlope = fSlopeUpper;


			fShiftXLower = ( ( ptLOGravTbl[nIdxLowerCircle].Y ) - (float_t)pnTblLabelMinY[nIdxLowerCircle] ) / fSlopeLower;

			/* ラベル(丸候補)上端中心座標設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelUpperPnt.fX = ptLinePartInfo[nIdxLine].tLowerCenterPnt.fX;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelUpperPnt.fY = ptLinePartInfo[nIdxLine].tLowerCenterPnt.fY;

			/* ラベル(線候補)下端中心座標設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelLowerPnt.fX = ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[nIdxLowerCircle].X ) - fShiftXLower;
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelLowerPnt.fY = (float_t)( ptRectRgn->nSY + pnTblLabelMinY[nIdxLowerCircle] );

			/* 輝度判定探索領域幅設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nSrchWidth = ptLinePartInfo[nIdxLine].nWidthLower;

			/* 輝度判定探索ライン傾き設定 */
			ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].fSrchSlope = fSlopeLower;


			/* -s 20130827[QAC指摘]処理領域外アクセス */
			/* 探索領域が処理範囲外になる線丸候補は、候補から除外する */

			/* 上下の探索領域の最小X・最大Xを計算 */
			nSrchUpperMinX = (slong)( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].tLabelUpperPnt.fX + 0.5F ) - ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nSrchWidth / 2L );		// MISRA-Cからの逸脱 [EntryAVM_QAC#4] R2.4.1  , ID-6794
			nSrchLowerMinX = (slong)( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].tLabelLowerPnt.fX + 0.5F ) - ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nSrchWidth / 2L );		// MISRA-Cからの逸脱 [EntryAVM_QAC#4] R2.4.1  , ID-6795
			nSrchUpperMaxX = nSrchUpperMinX + ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_UPPER].nSrchWidth - 1L );
			nSrchLowerMaxX = nSrchLowerMinX + ( ptBrPointTbl[CB_ADD_PATTERN_PNT_POS_LOWER].nSrchWidth - 1L );

			/* 探索領域最小X座標を計算 */
			if( nSrchUpperMinX < nSrchLowerMinX )
			{
				nSrchMinX = nSrchUpperMinX;
			}
			else
			{
				nSrchMinX = nSrchLowerMinX;
			}

			/* 探索領域最大X座標を計算 */
			if( nSrchUpperMaxX > nSrchLowerMinX )
			{
				nSrchMaxX = nSrchUpperMaxX;
			}
			else
			{
				nSrchMaxX = nSrchLowerMaxX;
			}

			/* 探索時に処理領域外に出る場合は、無効候補として処理を飛ばす */
			if(		( nSrchMinX < ptRectRgn->nSX )
				||	( nSrchMaxX > ptRectRgn->nEX ) )
			{

#ifdef CB_FPE_CSW_DEBUG_ON
				if( nj <= CB_FPE_DBG_BR_RESULT_MAXNUM )
				{
					/* 上側推定丸位置に無効な値を登録(DBG描画時に無効候補として描画しないようにする)*/
					m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX = -1L;
					/* パターン候補数 */
					m_FPE_tDbgInfo.nNum[ni] ++;
				}
				else
				{
					/* Not Operation */
				}
#endif /* CB_FPE_CSW_DEBUG_ON */

				continue;
			}

			/* -e 20130827[QAC指摘]処理領域外アクセス */


			/*--------------------------------*/
			/* 黒領域平均輝度値設定           */
			/*--------------------------------*/

			/* 線候補ラベルインデックス設定 */
			nIdxLinePart = (slong)( ptLinePartInfo[nIdxLine].nIdxLine );
			/* 黒領域(上端丸 + 線 + 下端丸)の合計面積算出 */
			fSumBlackArea = (float_t)( ptLOGravTbl[nIdxLinePart].AREA + ptLOGravTbl[nIdxUpperCircle].AREA + ptLOGravTbl[nIdxLowerCircle].AREA );

			/* 黒領域平均輝度値算出 */
			if( CB_FPE_FLT_MIN > fSumBlackArea )	/* 0割チェック */
			{
				/* ここに来るのはあり得ないパターン */
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
			/* 類似度計算                     */
			/*--------------------------------*/

			/* 輝度の類似度計算 */
			nRet = cb_FPE_CalcBrightnessSimilarity( tImgSrcAccessInfo.pnAddrY8, tImgRRFAccessInfo.pnAddrY8, &tBrInput, &fSimilarity, &fContrast );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R1.1.1  R-30, ID-6745	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R1.1.1  R-30, ID-6746

			if ( 0L > nRet )
			{
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nImgSrc );
				nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );
				return ( CB_IMG_NG_CALC_BR_SIMILARITY );
			}

			/*--------------------------------*/
			/* 距離判定                       */
			/*--------------------------------*/

			/* ラベル間距離算出(上端) */
			fDistanceUpper = sqrtf( ( fDiffXUpper * fDiffXUpper ) + ( fDiffYUpper * fDiffYUpper ) );
			/* ラベル間距離算出(下端) */
			fDistanceLower = sqrtf( ( fDiffXLower * fDiffXLower ) + ( fDiffYLower * fDiffYLower ) );

			/* 候補が見つかっていない状態 */
			if ( 0L > nIdxCircleLineComb )
			{
				/* インデックス設定 */
				nIdxCircleLineComb = nj;    /* 線丸ペア候補インデックス   */
				nIdTbl[nIdxLine] = nj;      /* 線ラベルインデックス       */

				/* ラベル間距離(上下端合計値) */
				fSumDistance = fDistanceUpper + fDistanceLower;

				/* 類似度スコア */
				fScoreTbl[nIdxLine] = fSimilarity * ( fContrast / 255.0F );
			}
			/* 候補がすでに見つかっている状態 */
			else
			{
				/***
				* 以下の条件の時、インデックスを更新する
				*  ①同じ直線部分を持つペアではない場合
				*  ②同じ直線部分を持つペア 且つ ラベル間距離(上下端合計値)が最短の場合
				****/
				if (   ( nIdxLine != ptCircleLineComb[nIdxCircleLineComb].nIdxLine )
					|| ( fSumDistance > ( fDistanceUpper + fDistanceLower ) ) )
				{
					/* インデックス設定 */
					nIdxCircleLineComb = nj;    /* 線丸ペア候補インデックス   */
					nIdTbl[nIdxLine] = nj;      /* 線ラベルインデックス       */

					/* ラベル間距離(上下端合計値) */
					fSumDistance = fDistanceUpper + fDistanceLower;

					/* 類似度スコア */
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
				/* 上側推定丸位置 */
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX = (slong)( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ nIdxUpperCircle ].X + 0.5F );
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY = (slong)( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ nIdxUpperCircle ].Y + 0.5F );
				/* 下側推定丸位置 */
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX = (slong)( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ nIdxLowerCircle ].X + 0.5F );
				m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY = (slong)( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ nIdxLowerCircle ].Y + 0.5F );
				/* 類似度・コントラスト */
				m_FPE_tDbgInfo.tSimilarity[ni][nj] = fSimilarity;
				m_FPE_tDbgInfo.tContrast[ni][nj] = fContrast / 255.0F;
				/* パターン候補数 */
				m_FPE_tDbgInfo.nNum[ni] ++;
				/* 輝度判定スコア */
				/* ( 正規化相互相関 ) * ( コントラスト(正規化) ) */
				m_FPE_tDbgInfo.tScore[ni][nj] = fScoreTbl[nIdxLine];

			}
			else
			{
				/* Not Operation */
			}
#endif /* CB_FPE_CSW_DEBUG_ON */
		} /* nj */

		/*--------------------------------*/
		/* 類似度判定                     */
		/*--------------------------------*/

		/* 初期化 */
		fMaxScore = 0.0F;

		for( nj = 0L; nj < nCntCandLine; nj++ )
		{
			if( fMaxScore < fScoreTbl[nj] )
			{
				fMaxScore = fScoreTbl[nj];          /* スコアが高いものに更新 */
				nIdxCircleLineComb = nIdTbl[nj];    /* その時の線丸ペアインデックスを設定 */
			}
			else
			{
				/* Not Operation */
			}
		}


		/* 荒い探索結果を格納 */
		/* Index */
		nIdxCircle[0] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxUpperCircle );
		nIdxCircle[1] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxLowerCircle );

		/* 上側推定丸位置 */
		ptSrchRsltInfo[ni].tCenterPnt[0].nX = ptRectRgn->nSX + (slong)( ptLOGravTbl[ nIdxCircle[0] ].X + 0.5f );
		ptSrchRsltInfo[ni].tCenterPnt[0].nY = ptRectRgn->nSY + (slong)( ptLOGravTbl[ nIdxCircle[0] ].Y + 0.5f );
		/* 下側推定丸位置 */
		ptSrchRsltInfo[ni].tCenterPnt[1].nX = ptRectRgn->nSX + (slong)( ptLOGravTbl[ nIdxCircle[1] ].X + 0.5f );
		ptSrchRsltInfo[ni].tCenterPnt[1].nY = ptRectRgn->nSY + (slong)( ptLOGravTbl[ nIdxCircle[1] ].Y + 0.5f );


		/* RRF画像メモリアクセス終了 */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL] );
		if ( 0L > nRet )
		{
			nRet = implib_ClearIPError();
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG_IMPLIB );
		}

#else		/* 距離判定(既存) */
			fDiffX = ( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxUpperCircle ].X ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tUpperCenterPnt.fX );
			fDiffY = ( ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxUpperCircle ].Y ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tUpperCenterPnt.fY );
			fDistanceUpper = sqrtf( ( fDiffX * fDiffX ) + ( fDiffY * fDiffY ) );
			fDiffX = ( ( (float_t)( ptRectRgn->nSX ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxLowerCircle ].X ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tLowerCenterPnt.fX );
			fDiffY = ( ( (float_t)( ptRectRgn->nSY ) + ptLOGravTbl[ ptCircleLineComb[nj].nIdxLowerCircle ].Y ) - ptLinePartInfo[ ptCircleLineComb[nj].nIdxLine ].tLowerCenterPnt.fY );
			fDistanceLower = sqrtf( ( fDiffX * fDiffX ) + ( fDiffY * fDiffY ) );
			fDiffDistance = fabsf( fDistanceUpper - fDistanceLower );
			fFarDistance = ( fDistanceUpper > fDistanceLower ) ? fDistanceUpper : fDistanceLower;

			if ( 0L > nIdxCircleLineComb )	/* 候補が見つかっていない状態 */
			{
				if ( fMinDiffDistance > fDiffDistance )
				{
					nIdxCircleLineComb = nj;
					fMinDiffDistance = fDiffDistance;
					fMinFarDistance = fFarDistance;
				}
			}
			else	/* 候補がすでに見つかっている状態 */
			{
				if ( ptCircleLineComb[nj].nIdxLine != ptCircleLineComb[nIdxCircleLineComb].nIdxLine )	/* 同じ直線部分を持つペアではない場合 */
				{
					if ( fMinDiffDistance > fDiffDistance )
					{
						nIdxCircleLineComb = nj;
						fMinDiffDistance = fDiffDistance;
						fMinFarDistance = fFarDistance;
					}
				}
				else		/* 同じ直線部分を持つペアの場合 */
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
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */										/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */

		/*
		==============================
			高精度位置決め
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
			/* 画像メモリアクセス情報設定 */
			pnTopAddrPixel = (uchar*)( tImgSrcAccessInfo.pnAddrY8 );
			nXSizeImg = tImgSrcAccessInfo.nXSize;
			nYSizeImg = tImgSrcAccessInfo.nYSize;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R1.1.1  R-30, ID-6747

			/* Index */
			nIdxCircle[0] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxUpperCircle );
			nIdxCircle[1] = (slong)( ptCircleLineComb[nIdxCircleLineComb].nIdxLowerCircle );

			for ( nj = 0L; nj < CB_ADD_PATTERN_PNT_POS_MAXNUM; nj++ )
			{
				/* 注目位置 */
				nAttnX = ptRectRgn->nSX + (slong)( ptLOGravTbl[ nIdxCircle[nj] ].X + 0.5f );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6748
				nAttnY = ptRectRgn->nSY + (slong)( ptLOGravTbl[ nIdxCircle[nj] ].Y + 0.5f );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6749

				/* 注目アドレス */
				pnWkAddrPixel = pnTopAddrPixel + ( nAttnY * nXSizeImg ) + nAttnX;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6750

				/* 代表画素値 */
				nSumValue = (slong)(   ( *( pnWkAddrPixel - ( nXSizeImg + 1L ) ) )
									 + ( *( pnWkAddrPixel - ( nXSizeImg      ) ) )
									 + ( *( pnWkAddrPixel - ( nXSizeImg - 1L ) ) )
									 + ( *( pnWkAddrPixel - (           + 1L ) ) )
									 + ( *( pnWkAddrPixel                      ) )
									 + ( *( pnWkAddrPixel + (           + 1L ) ) )
									 + ( *( pnWkAddrPixel + ( nXSizeImg - 1L ) ) )
									 + ( *( pnWkAddrPixel + ( nXSizeImg      ) ) )
									 + ( *( pnWkAddrPixel + ( nXSizeImg + 1L ) ) ) );	// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6802
				nRepValue = (slong)( ( (float_t)nSumValue / 9.0f ) + 0.5f );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6752

				nSumX = 0L;
				nSumY = 0L;
				nCntCoord = 0L;
				for ( nk = ( pnTblLabelMinY[ nIdxCircle[nj] ] - 1L ); nk <= ( pnTblLabelMaxY[ nIdxCircle[nj] ] + 1L ); nk++ )
				{
					/* スキャンするY座標設定 */
					nScanY = ( ptRectRgn->nSY + nk );

					for ( nl = ( pnTblLabelMinX[ nIdxCircle[nj] ] - 1L ); nl <= ( pnTblLabelMaxX[ nIdxCircle[nj] ] + 1L ); nl++ )
					{
						/* スキャンするX座標設定 */
						nScanX = ( ptRectRgn->nSX + nl );
						/* 参照アドレス */
						pnWkAddrPixel = pnTopAddrPixel + ( ( nScanY * nXSizeImg ) + nScanX );

						if ( ( nRepValue + nprm_diffIntensityPrecise ) >= (slong)(*pnWkAddrPixel) )
						{
							nSumX = nSumX + nScanX;
							nSumY = nSumY + nScanY;
							/* サンプル数 */
							nCntCoord++;
						}
					}	/* for ( nl ) */
				}	/* for ( nk ) */

				if ( 0L < nCntCoord )	/* 代表画素値との類似画素値が存在するか */
				{
					dCoeffAve = 1.0 / (double_t)nCntCoord;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].w = (double_t)nSumX * dCoeffAve;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].h = (double_t)nSumY * dCoeffAve;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].nFlagPrecise = CB_TRUE;
				}
				else
				{
					/* 代表画素値との類似画素値が存在しない -> 0割り発生 */
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].w = (double_t)ptSrchRsltInfo[ni].tCenterPnt[nj].nX;//-1.0;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].h = (double_t)ptSrchRsltInfo[ni].tCenterPnt[nj].nY;//-1.0;
					ptSrchRsltInfo[ni].tCenterPntPrecise[nj].nFlagPrecise = CB_FALSE;
				}
			}	/* for ( nj ) */

			/* 点数 */
			ptSrchRsltInfo[ni].nNumCenterPnt = CB_ADD_PATTERN_PNT_POS_MAXNUM;
		}

	}	/* for ( ni ) */

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	
	/* 抽出できたかチェック */
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
 * @brief		円領域の中点抽出(フロントカメラ用、真ん中の線が短い場合)
 *
 * @param[in]	pnImgWk			:,ワーク画面IDへのポインタ,-,[-],
 * @param[out]	ptSrchRsltInfo	:,探索結果情報構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,探索領域情報構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.04	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_ExtractCircleRegion4FrontCAM( const IMPLIB_IMGID* const pnImgWk, CB_FPE_SRCH_RSLT_ADD_PNT_INFO *ptSrchRsltInfo, CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN* ptSrchRgnInfo )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6753
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
	/* パラメータ */
	slong	nprm_cntReach;
	slong	nprm_thrminLabelArea, nprm_thrmaxLabelArea;
	slong	nprm_thrSizeLabel;
	float_t	/*fprm_aspectRatioVert, fprm_aspectRatioHori, */fprm_aspectRatio, fprm_aspectRatioInv;
	float_t	fprm_lengthRatio, fprm_lengthRatioInv;
	float_t	fprm_thrAreaRatio;
	float_t	fprm_thrDistanceRatio, fprm_thrDistanceRatioInv;

	/* 引数チェック */
	if ( ( NULL == pnImgWk ) || ( NULL == ptSrchRsltInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 先頭ポインタ */
	pnTblLabelMinX = &(m_FPE_tWorkInfo.nTblLabelMinX[0]);
	pnTblLabelMaxX = &(m_FPE_tWorkInfo.nTblLabelMaxX[0]);
	pnTblLabelMinY = &(m_FPE_tWorkInfo.nTblLabelMinY[0]);
	pnTblLabelMaxY = &(m_FPE_tWorkInfo.nTblLabelMaxY[0]);
	ptLOGravTbl = &(m_FPE_tWorkInfo.tTblGravity[0]);
	pnIdxLabelCandPnt = &( m_FPE_tWorkInfo.nIdxLabelCandPnt[0] );
	ptCandPoint = &( m_FPE_tWorkInfo.tCircleCandPnt[0] );

	for ( ni = 0L; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* 処理領域 */
		ptRectRgn = &(ptSrchRgnInfo[ni].tRgnImg);
		/* 候補点格納先 */
		ptCenterPnt = &(ptSrchRsltInfo[ni].tCenterPnt[0]);
		/* クリア */
		nCntCandPoint = 0L;
		nCntCandPntUpper = 0L;
		nCntCandPntLower = 0L;
		memset( pnTblLabelMinX, 0x00, ( 256UL * sizeof( slong ) ) );
		memset( pnTblLabelMaxX, 0x00, ( 256UL * sizeof( slong ) ) );

		/* しきい値処理して2値化 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_Binarize( pnImgWk[CB_FPE_IMG_RADIAL_REACH_IMG_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], nprm_cntReach );

		/* Labeling */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nNumLabel = implib_IP_Label8withAreaFLT( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], nprm_thrminLabelArea, nprm_thrmaxLabelArea, IMPLIB_LABEL_OBJ );

		/* Labelごとに最小/最大x,y座標 */
		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, ptRectRgn->nSX, ptRectRgn->nSY, ptRectRgn->nEX, ptRectRgn->nEY );
		nRet = implib_IP_ExtractLORegionX( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinX, pnTblLabelMaxX );
		nRet = implib_IP_ExtractLORegionY( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], pnTblLabelMinY, pnTblLabelMaxY );

		/* 重心 */
		nRet = implib_IP_ExtractLOGravity( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], ptLOGravTbl );

		/* IMPのエラーをまとめてチェック */
		if ( 0L > nRet )
		{
			/* エラー情報クリア */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}

		if ( CB_FPE_CIRCLE_CAND_POINT_MAXNUM < nNumLabel )
		{
			return ( CB_IMG_NG );
		}

		/* 縦横比を使って、絞込み */
		for ( nj = 1L; nj <= nNumLabel; nj++ )
		{
			nXSizeLabel = ( pnTblLabelMaxX[nj] - pnTblLabelMinX[nj] ) + 1L;
			nYSizeLabel = ( pnTblLabelMaxY[nj] - pnTblLabelMinY[nj] ) + 1L;

			if( ( 0L == nXSizeLabel ) || ( 0L == nYSizeLabel ) )
			{
					/* 0割時の処理 *//* ここに来るのはあり得ないため、エラー*/
					return ( CB_IMG_NG_DIV0 );
			}

			/* 縦横比 */
			nFlgAspectChk = CB_FALSE;
			fAspectRatio = (float_t)nYSizeLabel / (float_t)nXSizeLabel;
			if (   ( ( fprm_aspectRatio <= fAspectRatio ) && ( fAspectRatio <= fprm_aspectRatioInv ) ) 
				&& ( ( nprm_thrSizeLabel <= nXSizeLabel ) && ( nprm_thrSizeLabel <= nYSizeLabel ) ) )
			{
				nFlgAspectChk = CB_TRUE;
			}
			/* 面積比 */
			nAreaBoundRect = nXSizeLabel * nYSizeLabel;
			nFlgAreaChk = CB_FALSE;
			if ( fprm_thrAreaRatio <= ( (float_t)( ptLOGravTbl[nj].AREA ) / (float_t)nAreaBoundRect ) )
			{
				nFlgAreaChk = CB_TRUE;
			}

			/* 縦横比から絞込み */
			if ( ( CB_TRUE == nFlgAspectChk ) && ( CB_TRUE == nFlgAreaChk ) )
			{
				/* 中点 */
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
		for ( nj = 0L; nj < nCntCandPoint; nj++ )	/* 注目点 */
		{
			for ( nk = 0L; nk < nCntCandPoint; nk++ )	/* 対応点1 */
			{
				if ( nj == nk )	/* 同じデータを参照しない */
				{
					continue;
				}

				/* 方向 */
				tVector[0].fX = ( ptCandPoint[nk].fX - ptCandPoint[nj].fX );
				tVector[0].fY = ( ptCandPoint[nk].fY - ptCandPoint[nj].fY );
				fDistanceCand1 = sqrtf( ( tVector[0].fX * tVector[0].fX ) + ( tVector[0].fY * tVector[0].fY ) );

				if ( CB_FPE_FLT_MIN > fDistanceCand1 )	/* 0割チェック追加 */
				{
					/* ここに来るのはあり得ないパターン */
					continue;
				}

				for ( nl = 0L; nl < nCntCandPoint; nl++ )	/* 対応点2 */
				{
					if ( ( nj == nl ) || ( nk == nl ) )	/* 同じデータを参照しない */
					{
						continue;
					}

					/* 方向 */
					tVector[1].fX = ( ptCandPoint[nl].fX - ptCandPoint[nj].fX );
					tVector[1].fY = ( ptCandPoint[nl].fY - ptCandPoint[nj].fY );
					fDistanceCand2 = sqrtf( ( tVector[1].fX * tVector[1].fX ) + ( tVector[1].fY * tVector[1].fY ) );

					if ( CB_FPE_FLT_MIN > fDistanceCand2 )	/* 0割チェック追加 */
					{
						/* ここに来るのはあり得ないパターン */
						continue;
					}

					fDistanceRatio = fDistanceCand2 / fDistanceCand1;
					if ( ( fprm_thrDistanceRatio > fDistanceRatio ) || ( fDistanceRatio > fprm_thrDistanceRatioInv ) )
					{
						continue;
					}
				
					/* tVector[0]、tVector[1]のX成分またはY成分のいづれかが0 */ /* 0割チェック */
					if (   ( CB_FPE_FLT_MIN >= fabsf( tVector[0].fX ) ) || ( CB_FPE_FLT_MIN >= fabsf( tVector[0].fY ) )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.8.1  R-33, ID-6758
						|| ( CB_FPE_FLT_MIN >= fabsf( tVector[1].fX ) ) || ( CB_FPE_FLT_MIN >= fabsf( tVector[1].fY ) ) )
					{
						continue;
					}
					
					/* X成分が同符号 */
					if (   ( ( tVector[0].fX < CB_FPE_FLT_MIN ) && ( tVector[1].fX < CB_FPE_FLT_MIN ) )
						|| ( ( tVector[0].fX > CB_FPE_FLT_MIN ) && ( tVector[1].fX > CB_FPE_FLT_MIN ) ) )
					{
						continue;
					}
					/* Y成分が同符号 */
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

					/* X成分の比 */
					fLengthRatio = (float_t)fabsf( tVector[1].fX ) / (float_t)fabsf( tVector[0].fX );

					/* X成分の比をしきい値処理 */
					if ( ( fLengthRatio < fprm_lengthRatio ) || ( fprm_lengthRatioInv < fLengthRatio ) )
					{
						continue;
					}

					/* Y成分の比 */
					fLengthRatio = (float_t)fabsf( tVector[1].fY ) / (float_t)fabsf( tVector[0].fY );

					/* Y成分の比をしきい値処理 */
					if ( ( fLengthRatio < fprm_lengthRatio ) || ( fprm_lengthRatioInv < fLengthRatio ) )
					{
						continue;
					}

					/* 角度による比較 */
					fCosCand1 = fabsf( tVector[0].fX ) / fDistanceCand1;
					fThetaCand1 = CB_FPE_CONVERT_RAD2DEG_F( acosf( fCosCand1 ) );
					fCosCand2 = fabsf( tVector[1].fX ) / fDistanceCand2;
					fThetaCand2 = CB_FPE_CONVERT_RAD2DEG_F( acosf( fCosCand2 ) );
					if ( 10.0F < fabsf( fThetaCand1 - fThetaCand2 ) )
					{
						continue;
					}

					/* 探索成功 */
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

				if ( CB_TRUE == nFlagSearch )	/* 探索成功なら抜ける */
				{
					break;
				}
			}

			if ( CB_TRUE == nFlagSearch )	/* 探索成功なら抜ける */
			{
				break;
			}
		}

		/* 探索結果を格納 */
//		ptCandPoint[0] = tCandPoint[0];
//		ptCandPoint[1] = tCandPoint[1];
		ptCenterPnt[0].nX = (slong)( tCandPoint[0].fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6759
		ptCenterPnt[0].nY = (slong)( tCandPoint[0].fY + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6760
		ptCenterPnt[1].nX = (slong)( tCandPoint[1].fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6761
		ptCenterPnt[1].nY = (slong)( tCandPoint[1].fY + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6762
		
		/* 候補数格納 */
		ptSrchRsltInfo[ni].nNumCenterPnt = nCntCandPntUpper + nCntCandPntLower;

		/* 高精度版 */
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

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Haar-like特徴量の計算(市松タイプ)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgn		:,矩形領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.17	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 先頭ポインタ */
		/* 左上のブロック */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6767
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6812
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6814
		/* 右上のブロック */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6772
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6773
		/* 左下のブロック */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下のブロック */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6774
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6775
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6776
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6777
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 各ブロック内の輝度値合計 */
			nSumBlockLU = ( ( (*pnWkSumRL_BlockLU) - (*pnWkSumRU_BlockLU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU);
			nSumBlockRU = ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockRU) ) + (*pnWkSumLU_BlockRU);
			nSumBlockLL = ( ( (*pnWkSumRL_BlockLL) - (*pnWkSumRU_BlockLL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL);
			nSumBlockRL = ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockRL) ) + (*pnWkSumLU_BlockRL);

			/* 特徴量 */
			*pnWkFtr = ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL );

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;
			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Haar-like特徴量の計算(縦エッジタイプ)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgn		:,矩形領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.06	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 先頭ポインタ */
		/* 左のブロック */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6790
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6823
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6825
		/* 右のブロック */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6795
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6796

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 各ブロック内の輝度値合計 */
			nSumBlockLU = ( ( (*pnWkSumRL_BlockLU) - (*pnWkSumRU_BlockLU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU);
			nSumBlockRU = ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockRU) ) + (*pnWkSumLU_BlockRU);

			/* 特徴量 */
			*pnWkFtr = nSumBlockLU - nSumBlockRU;

			/* ポインタを進める */
			/* 左 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		Haar-like特徴量の計算(横エッジタイプ)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgn		:,矩形領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.09	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_YSIZE;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 先頭ポインタ */
		/* 上のブロック */
		pnWkSumRL_BlockU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6805
		pnWkSumLL_BlockU = pnWkSumRL_BlockU - nprm_xsizeBlockW;									// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6830
		pnWkSumRU_BlockU = pnWkSumRL_BlockU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockU = pnWkSumRU_BlockU - nprm_xsizeBlockW;									// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6832
		/* 下のブロック */
		pnWkSumRL_BlockL = pnWkSumRL_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLL_BlockL = pnWkSumLL_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );				// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6810
		pnWkSumRU_BlockL = pnWkSumRU_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockL = pnWkSumLU_BlockU + ( nprm_ysizeBlockW * nXSizeIImg );				// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6811

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 各ブロック内の輝度値合計 */
			nSumBlockU = ( ( (*pnWkSumRL_BlockU) - (*pnWkSumRU_BlockU) ) - (*pnWkSumLL_BlockU) ) + (*pnWkSumLU_BlockU);
			nSumBlockL = ( ( (*pnWkSumRL_BlockL) - (*pnWkSumRU_BlockL) ) - (*pnWkSumLL_BlockL) ) + (*pnWkSumLU_BlockL);

			/* 特徴量 */
			*pnWkFtr = nSumBlockU - nSumBlockL;

			/* ポインタを進める */
			/* 左 */
			pnWkSumRL_BlockU++;
			pnWkSumLL_BlockU++;
			pnWkSumRU_BlockU++;
			pnWkSumLU_BlockU++;
			/* 右 */
			pnWkSumRL_BlockL++;
			pnWkSumLL_BlockL++;
			pnWkSumRU_BlockL++;
			pnWkSumLU_BlockL++;
			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		十字点(候補)の探索
 *
 * @param[in]	ptHaarLike			:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptCrossPnt			:,画像座標(十字位置)構造体へのポインタ,-,[-],
 * @param[out]	pnNumCrossPnt		:,画像座標(十字位置)構造体の数へのポインタ,-,[-],
 * @param[in]	ptSrchRgn			:,矩形領域構造体へのポインタ,-,[-],
 * @param[in]	nMaxNumPnt			:,探索対象の最大点数,1<=value<=3,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]	nThrHaarFtrCrossPnt	:,市松ターゲット内部の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値,-,[-]
 * @param[in]	ptPatternInfo		:,市松ターゲットの特徴パターン情報,-,[-]
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.17	K.Kato			新規作成
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
	CB_FPE_CAND_PNT_INFO	tPntAry[3];	/* 本関数内に初期設定があるため、ここでは初期化しない */
	slong	*pnWkFtrAry[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	slong	nSignHaarLikeFeature = 0L;
	slong	nSignHaarLikeFeature2 = 0L;
	enum enum_CB_IMG_ALGORITHM				nTypeAlgorithm;
	/* パラメータ */
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
	CB_FPE_CAND_PNT_INFO	tHaarLabelAry[CB_FPE_PRM_HAAR_LABEL_MAX_NUM + 1L];	/* 本関数内に初期設定があるため、ここでは初期化しない */
	CB_FPE_CAND_PNT_INFO	tTempAry;


	/* 引数チェック */
	if ( ( NULL == pnImgWk ) || ( NULL == ptHaarLike ) || ( NULL == ptCrossPnt ) || ( NULL == pnNumCrossPnt ) || ( NULL == ptSrchRgn ) || ( NULL == ptPatternInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_xSizePeri = 1L;
	nprm_ySizePeri = 1L;
	nprm_distancePnt = CB_FPE_PRM_MERGE_DISTANCE_CAND_CROSS_PNT;
//	nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_FTR_CROSS_PNT;
	nprm_HaarLabelMinArea = CB_FPE_PRM_THR_HAAR_MIN_LABEL_AREA;
	nprm_HaarLabelMaxArea = CB_FPE_PRM_THR_HAAR_MAX_LABEL_AREA;
	nprm_HaarLabelMaxID = CB_FPE_PRM_HAAR_LABEL_MAX_NUM + 1L;
	nprm_SrchWidthUL = CB_FPE_PRM_SRCH_WIDTH_CHECK_HAAR_UL;

	fprm_CoeffHaarThr = CB_FPE_PRM_COEFF_THR_HAAR_CROSS_FTR;
	/* 市松Haar-Likeの最大値or最小値は、[ 255 * ( 1ブロックの面積 ) * 2 ] なので	*/
	/* この値を-128～127(符号付き8bit)の範囲に正規化するため、						*/
	/* [ ( 1ブロックの面積 ) * 2 * 2 ] で除算する。									*/
	fprm_CoeffNormHaarFtr = (float_t)( CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE * CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE * 2L * 2L );

	/* 異常値チェック( 0割もしくは、Haarの係数が負のとき )	*/
	/* 通常は起こりえないパターンなので、NGを返す			*/
	if ( CB_FPE_DBL_MIN <= fprm_CoeffNormHaarFtr )
	{
		/* No Operation */
	}
	else
	{
		/* 0割時の処理 */
		/* あり得ないパターンなのでNGを返す */
		return ( CB_IMG_NG );
	}


	/* 先頭ポインタ */
	pnTblLabelMinX = &(m_FPE_tWorkInfo.nTblLabelMinX[0]);
	pnTblLabelMaxX = &(m_FPE_tWorkInfo.nTblLabelMaxX[0]);
	pnTblLabelMinY = &(m_FPE_tWorkInfo.nTblLabelMinY[0]);
	pnTblLabelMaxY = &(m_FPE_tWorkInfo.nTblLabelMaxY[0]);

	/* 探索領域 */
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


	/* 重み付きHaar-like特徴量符号の取得 */
	nRet = cb_FPE_GetSignHaarLikeFeature( ptPatternInfo, nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		return ( nRet );
	}


	if( nDoubleType == 0U )
	{
		/********************************************
		 *　キャリブマーカが併設されていない場合	*
		 *　⇒既存処理を実行						*
		 ********************************************/
		/* 市松の真ん中の点を探索 */
		for ( ni = nprm_ySizePeri; ni < ( nYSizeSrchRgn - nprm_ySizePeri ); ni++ )
		{
			/* 先頭ポインタ */
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

		/* 信頼度チェック */
		if ( labs( tPntAry[0].nFtrValue ) < nThrHaarFtrCrossPnt )
		{
			tPntAry[0].tPnt.nX = -1L;
			tPntAry[0].tPnt.nY = -1L;
		}
	}
	else
	{
		/********************************************
		 *　キャリブマーカが併設されている場合		*
		 *　⇒新規処理(複数点対応)を実行			*
		 ********************************************/

		/*----------------------------------*/
		/* ①下準備							*/
		/*----------------------------------*/

		/* 画像処理動作タイプをSIGN8_DATAに変更 */
		nRet = implib_SetIPDataType( IMPLIB_SIGN8_DATA );
		/* 画面データタイプを強制的に設定 */
		nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], IMPLIB_SIGN8_DATA );
		nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], IMPLIB_SIGN8_DATA );

		/* マスク用2値画像をオープン */
		nRet = implib_IP_Const( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], 0L );
		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE], &( nXSizeImg ), &( nYSizeImg ), (void**)&( pnAddrImg ) );
		if ( 0L > nRet )
		{
			/* エラー情報クリア */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}

		/*----------------------------------*/
		/* ②十字候補領域のラベリング		*/
		/*----------------------------------*/

		/* 市松の真ん中の点を探索 */
		/* 並行して、Haar-Like特徴量の正規化を行い、work画像に格納 */
		for ( ni = nprm_ySizePeri; ni < ( nYSizeSrchRgn - nprm_ySizePeri ); ni++ )
		{
			/* 先頭ポインタ */
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
				/* Defineから、Haar-Like特徴量を-127～127の範囲に平滑化 */
				*(pnWkNormAry) = (schar)( (float_t)( *(pnWkFtrAry[0L]) ) / fprm_CoeffNormHaarFtr );

				(pnWkFtrAry[0])++;
				(pnWkNormAry)++;
			}
		}

		/* 画像をClose */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE] );

		/* 処理領域内の最高or最低Haar-Like特徴量から、2値画像作成用の閾値を計算 */
		nThrHaarFtr = (slong)( ( fMaxHaarLikeFeature / fprm_CoeffNormHaarFtr ) * fprm_CoeffHaarThr );

		nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );
		nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn );

		/* Haarの符号に応じて、閾値を設定 */
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
			/* 本来はありえないパス */
			return ( CB_IMG_NG );
		}

		/* 正規化したHaar-Like画像(符号付き8bit)を、算出した閾値で2値画像化する */
		nRet = IP_BinarizeExt( pnImgWk[CB_FPE_IMG_HAAR_NORMALIZE],  pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], nThrHaarFtr_Min, nThrHaarFtr_Max, 0 );
		if ( 0L > nRet )
		{
			/* エラー情報クリア */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}


		/* 画像処理動作タイプをUNSIGN8_DATAに変更 */
		nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );
		nRet = implib_ChangeImgDataType( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], IMPLIB_UNSIGN8_DATA );

		/* ２値画像をラベリング(面積順にソート) */
		nNumLabel = implib_IP_Label8withAreaFLTSort( pnImgWk[CB_FPE_IMG_HAAR_BINARIZE], pnImgWk[CB_FPE_IMG_HAAR_LABELING], nprm_HaarLabelMinArea, nprm_HaarLabelMaxArea, IMPLIB_LABEL_OBJ, IMPLIB_LABEL_SORT_MAX );
		if ( 0L > nNumLabel )
		{
			/* エラー情報クリア */
			nRet = implib_ClearIPError();
			return ( CB_IMG_NG_IMPLIB );
		}

		/*----------------------------------*/
		/* ③各ラベルの代表点算出			*/
		/*----------------------------------*/

		/* ラベル毎に、外接矩形X,Y座標を取得 */
		nRet = implib_IP_ExtractLORegionX( pnImgWk[CB_FPE_IMG_HAAR_LABELING], pnTblLabelMinX, pnTblLabelMaxX );
		nRet = implib_IP_ExtractLORegionY( pnImgWk[CB_FPE_IMG_HAAR_LABELING], pnTblLabelMinY, pnTblLabelMaxY );

		nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_HAAR_LABELING], &( nXSizeImg ), &( nYSizeImg ), (void**)&( pnAddrImg ) );


		/* ラベル数でループ */
		for( nCnt = 1L; nCnt < nprm_HaarLabelMaxID ; nCnt++ )
		{
			nXSizeLabelSrchRgn = ( pnTblLabelMaxX[nCnt] - pnTblLabelMinX[nCnt] ) + 1L;
			nYSizeLabelSrchRgn = ( pnTblLabelMaxY[nCnt] - pnTblLabelMinY[nCnt] ) + 1L;

			/* 当該ラベルで、Haar-Like特徴量の最も高いor低い点を探索 */
			for ( ni = 0L; ni < nYSizeLabelSrchRgn; ni++ )
			{
				/* Haar-Like特徴量/ラベル画像の先頭ポインタ */
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

		/* 画像をClose */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_HAAR_LABELING] );

		/* HaarLike特徴量が高い順に配列をバブルソート */
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
		/* ④代表点に対して、左右を探索する検定処理			*/
		/*--------------------------------------------------*/

		/* ラベル数でループ */
		for( nCnt = 1L; nCnt < ( nNumLabel+ 1L ) ; nCnt++ )
		{
			/* ラベル毎の候補点を、基準値として格納 */
			tPntAry[0].nFtrValue = tHaarLabelAry[nCnt].nFtrValue;
			tPntAry[0].tPnt.nX = tHaarLabelAry[nCnt].tPnt.nX;
			tPntAry[0].tPnt.nY = tHaarLabelAry[nCnt].tPnt.nY;

			/* 信頼度チェック */
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

					/* 左側 */
					tPntAry[1].nFtrValue = 0L;
					tPntAry[1].tPnt.nX = -1L;
					tPntAry[1].tPnt.nY = -1L;

					nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

					if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
					{
						for ( ni = ( tPntAry[0].tPnt.nY - nprm_SrchWidthUL ); ni <= ( tPntAry[0].tPnt.nY + nprm_SrchWidthUL ); ni++ )
						{
							/* 先頭ポインタ */
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

						/* 信頼度チェック */
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


					/* 右側 */
					tPntAry[2].nFtrValue = 0L;
					tPntAry[2].tPnt.nX = -1L;
					tPntAry[2].tPnt.nY = -1L;

					if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
					{
						for ( ni = ( tPntAry[0].tPnt.nY - nprm_SrchWidthUL ); ni <= ( tPntAry[0].tPnt.nY + nprm_SrchWidthUL ); ni++ )
						{
							/* 先頭ポインタ */
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


						/* 信頼度チェック */
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

					/* 特徴点が最大数探索できた時(≒探索成功と思しき)場合は、Breakして処理を抜ける */
					if( nSuccessCnt == nMaxNumPnt )
					{
						break;
					}
					else
					{
						/* クリアして次のラベルへ */
						/* 座標/パラメータ */
						tPntAry[0].nFtrValue = 0L;
						tPntAry[0].tPnt.nX = -1L;
						tPntAry[0].tPnt.nY = -1L;
						tPntAry[1].nFtrValue = 0L;
						tPntAry[1].tPnt.nX = -1L;
						tPntAry[1].tPnt.nY = -1L;
						tPntAry[2].nFtrValue = 0L;
						tPntAry[2].tPnt.nX = -1L;
						tPntAry[2].tPnt.nY = -1L;

						/* カウンタ */
						nNumPnt = 0L;
						nSuccessCnt = 0L;
					}

				}
				else if ( 2L == nMaxNumPnt )
				{
					if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
					{
						/* アルゴリズムタイプ */
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

						/* 符号設定 */
						nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

						/* 初期値 */
						tPntAry[1].nFtrValue = 0L;
						tPntAry[1].tPnt.nX = -1L;
						tPntAry[1].tPnt.nY = -1L;

						for ( ni = nScanSY; ni <= nScanEY; ni++ )
						{
							/* 先頭ポインタ */
							pnWkFtrAry[0] = ptHaarLike->pnFtrValue + ( ( ni * ptHaarLike->nXSize ) + nScanSX );

							for ( nj = nScanSX; nj <= nScanEX; nj++ )
							{
								if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2 ) > tPntAry[1].nFtrValue )
								{
									tPntAry[1].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
									tPntAry[1].tPnt.nX = nj;
									tPntAry[1].tPnt.nY = ni;
								}

								/* 次のアクセス先へ */
								(pnWkFtrAry[0])++;
							}	/* for ( nj ) */
						}	/* for ( ni ) */

						/* 信頼度チェック */
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

					/* 特徴点が最大数探索できた時(≒探索成功と思しき)場合は、Breakして処理を抜ける */
					if( nSuccessCnt == nMaxNumPnt )
					{
						break;
					}
					else
					{
						/* クリアして次のラベルへ */
						/* 座標/パラメータ */
						tPntAry[0].nFtrValue = 0L;
						tPntAry[0].tPnt.nX = -1L;
						tPntAry[0].tPnt.nY = -1L;
						tPntAry[1].nFtrValue = 0L;
						tPntAry[1].tPnt.nX = -1L;
						tPntAry[1].tPnt.nY = -1L;
						tPntAry[2].nFtrValue = 0L;
						tPntAry[2].tPnt.nX = -1L;
						tPntAry[2].tPnt.nY = -1L;

						/* カウンタ */
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
			/* デバッグ用_Work画像を出力 */
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



	/* 点数をインクリメント */
	nNumPnt++;

	if ( 1L < nMaxNumPnt )
	{
		if ( 3L == nMaxNumPnt )
		{

			/* Index:0 -> Index:1に入れ替え */
			tPntAry[1] = tPntAry[0];

			/* 左側 */
			tPntAry[0].nFtrValue = 0L;
			tPntAry[0].tPnt.nX = -1L;
			tPntAry[0].tPnt.nY = -1L;

			nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

			if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) )
			{
				for ( ni = tPntAry[1].tPnt.nY - 10L; ni <= ( tPntAry[1].tPnt.nY + 10L ); ni++ )
				{
					/* 先頭ポインタ */
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

				/* 信頼度チェック */
				if ( labs( tPntAry[0].nFtrValue ) < nThrHaarFtrCrossPnt )
				{
					tPntAry[0].nFtrValue = 0L;
					tPntAry[0].tPnt.nX = -1L;
					tPntAry[0].tPnt.nY = -1L;
				}
			}	/* if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) ) */

			/* 点数インクリメント */
			nNumPnt++;


			/* 右側 */
			tPntAry[2].nFtrValue = 0L;
			tPntAry[2].tPnt.nX = -1L;
			tPntAry[2].tPnt.nY = -1L;

			if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) )
			{
				for ( ni = tPntAry[1].tPnt.nY - 10L; ni <= ( tPntAry[1].tPnt.nY + 10L ); ni++ )
				{
					/* 先頭ポインタ */
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


				/* 信頼度チェック */
				if ( labs( tPntAry[2].nFtrValue ) < nThrHaarFtrCrossPnt )
				{
					tPntAry[2].nFtrValue = 0L;
					tPntAry[2].tPnt.nX = -1L;
					tPntAry[2].tPnt.nY = -1L;
				}
			}	/* if ( ( 0L <= tPntAry[1].tPnt.nX ) && ( 0L <= tPntAry[1].tPnt.nY ) ) */

			/* 点数をインクリメント */
			nNumPnt++;

		}
		else if ( 2L == nMaxNumPnt )
		{
			if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) )
			{
				/* アルゴリズムタイプ */
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

				/* 符号設定 */
				nSignHaarLikeFeature2 = -nSignHaarLikeFeature;

				/* 初期値 */
				tPntAry[1].nFtrValue = 0L;
				tPntAry[1].tPnt.nX = -1L;
				tPntAry[1].tPnt.nY = -1L;

				for ( ni = nScanSY; ni <= nScanEY; ni++ )
				{
					/* 先頭ポインタ */
					pnWkFtrAry[0] = ptHaarLike->pnFtrValue + ( ( ni * ptHaarLike->nXSize ) + nScanSX );

					for ( nj = nScanSX; nj <= nScanEX; nj++ )
					{
						if ( ( (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2 ) > tPntAry[1].nFtrValue )
						{
							tPntAry[1].nFtrValue = (*(pnWkFtrAry[0])) * nSignHaarLikeFeature2;
							tPntAry[1].tPnt.nX = nj;
							tPntAry[1].tPnt.nY = ni;
						}

						/* 次のアクセス先へ */
						(pnWkFtrAry[0])++;
					}	/* for ( nj ) */
				}	/* for ( ni ) */

				/* 信頼度チェック */
				if ( labs( tPntAry[1].nFtrValue ) < nThrHaarFtrCrossPnt )
				{
					tPntAry[1].nFtrValue = 0L;
					tPntAry[1].tPnt.nX = -1L;
					tPntAry[1].tPnt.nX = -1L;
				}


				if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_RIGHT == nTypeAlgorithm )	/* 右寄りの場合のみ、配列の入れ替え */
				{
					/* 入れ替え */
					tPntAry[2] = tPntAry[0];
					tPntAry[0] = tPntAry[1];
					tPntAry[1] = tPntAry[2];
				}
				/*
				左寄りの場合は、配列の入れ替えは不要のため、コメントアウト
				else if ( CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_LEFT == nTypeAlgorithm )
				{

				}
				else
				{
				}
				*/
			}	/* if ( ( 0L <= tPntAry[0].tPnt.nX ) && ( 0L <= tPntAry[0].tPnt.nY ) ) */

			/* 点数をインクリメント */
			nNumPnt++;
		}
		else
		{
			return ( CB_IMG_NG );
		}/* if ( CB_IMG_PATTERN_TYPE_CHKBOARD2x3 != ptPatternInfo->nTypePattern ) */
	}

	/* 結果格納 */
	for ( ni = 0L; ni < nNumPnt; ni++ )
	{
		ptCrossPnt[ni] = tPntAry[ni].tPnt;
	}
	*pnNumCrossPnt = nNumPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		上/下側Ｔ字点(候補)の探索
 *
 * @param[in]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptCrossPnt		:,画像座標(十字位置)構造体へのポインタ,-,[-],
 * @param[in]	nNumCrossPnt	:,画像座標(十字位置)構造体の数,-,[-],
 * @param[out]	ptDstPnt		:,画像座標(Ｔ字位置)構造体へのポインタ,-,[-],
 * @param[out]	pnNumDstPnt		:,画像座標(Ｔ字位置)構造体の数,-,[-],
 * @param[in]	ptSrchRgn		:,矩形領域構造体へのポインタ,-,[-],
 * @param[in]	nEdgePos		:,探索方向,CB_FPE_UPPER_EDGE_POS_PATTERN<=value<=CB_FPE_LOWER_EDGE_POS_PATTERN,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.12	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_ysizeBlockW;
	slong	nprm_xsizeHalfScanRgn;
	slong	nprm_diffHaarLike;
	slong	nprm_thrHaarFtrTPnt;
	slong	nprm_distanceSrchRgn;
	float_t	fprm_coeffThrHaarLike;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptCrossPnt ) || ( NULL == ptDstPnt ) || ( NULL == pnNumDstPnt ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_VERT_EDGE_BLOCK_YSIZE;
//	nprm_xsizeHalfScanRgn = CB_FPE_PRM_XSIZE_HALF_SCAN_RGN;
	nprm_diffHaarLike = CB_FPE_PRM_DIFF_HAAR_LIKE;
	fprm_coeffThrHaarLike = CB_FPE_PRM_COEFF_THR_HAAR_LIKE;
	nprm_thrHaarFtrTPnt = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_UL;
	nprm_distanceSrchRgn = CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_UL;

	/* 探索領域設定 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	/* 初期化 */
	nNumDstPnt = 0L;

	/* 探索方向の係数 */
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

	/* 探索 */
	for ( ni = 0L; ni < nNumCrossPnt; ni++ )
	{
		ptDstPnt[ni].nX = -1L;
		ptDstPnt[ni].nY = -1L;

		if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) )
		{
			/* パラメータ設定 */
			nprm_xsizeHalfScanRgn = CB_FPE_PRM_XSIZE_HALF_SCAN_RGN;

			nXattn = ptCrossPnt[ni].nX;
//			nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * nprm_ysizeBlockW );
			nXSizeScanRgn = ( 2L * nprm_xsizeHalfScanRgn ) + 1L;
			if ( nCoeffDirY < 0L )
			{
				nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * nprm_distanceSrchRgn );
				nYSizeScanRgn = ( nYattn - ( nSySrchRgn + nprm_ysizeBlockW ) ) + 1L;
			}
			else if ( 0L < nCoeffDirY )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6816	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6817
			{
				nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * ( nprm_ysizeBlockW + nprm_distanceSrchRgn ) );
				nYSizeScanRgn = ( nEySrchRgn - nYattn ) + 1L;
			}
			else
			{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6818
				return ( CB_IMG_NG );
			}

			tPrevMaxPntInfo.nFtrValue = -1L;	/* 無効値 */
			tPrevMaxPntInfo.tPnt.nX = -1L;	/* 無効値 */
			tPrevMaxPntInfo.tPnt.nY = -1L;	/* 無効値 */
			tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
			tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
			tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */
			nFlgSrchComp = CB_FALSE;

			/* Y方向に探索を進める */
			fThrFtrValue = CB_FPE_FLT_MIN;
			for ( nj = 0L; nj < nYSizeScanRgn; nj++ )
			{
				/* 探索領域内チェック */
				if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nXattn, nYattn, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn ) )
				{
					if ( 0L >= tPrevMaxPntInfo.nFtrValue )
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
					}
					break;
				}

				pnWkFtr = ptHaarLike->pnFtrValue + ( nYattn * ptHaarLike->nXSize ) + ( nXattn - nprm_xsizeHalfScanRgn );

				/* Haar-like特徴量の絶対値が大きい点を探索 */
				tMaxPntInfo.nFtrValue = 0L;
				for ( nk = -nprm_xsizeHalfScanRgn; nk <= nprm_xsizeHalfScanRgn; nk++ )
				{
					if ( tMaxPntInfo.nFtrValue < labs( *pnWkFtr ) )
					{
						tMaxPntInfo.nFtrValue = labs( *pnWkFtr );
						tMaxPntInfo.tPnt.nX = nXattn + nk;
						tMaxPntInfo.tPnt.nY = nYattn;
					}

					/* X方向に1pixelシフトさせる */
					pnWkFtr++;
				}

				/* 注目点更新 */
				if ( -1 < tPrevMaxPntInfo.nFtrValue )	/* 初回探索以外 */
				{
	//				if ( tMaxPntInfo.nFtrValue < ( tPrevMaxPntInfo.nFtrValue - nprm_diffHaarLike ) )	/* 前回の探索位置のHaar-like特徴量から急激に特徴量が落ちた */
	//				if ( labs( tMaxPntInfo.nFtrValue - tPrevMaxPntInfo.nFtrValue ) > nprm_diffHaarLike )	/* 前回の探索位置のHaar-like特徴量から急激に特徴量が落ちた */
	//				if ( (float_t)( tMaxPntInfo.nFtrValue ) < ( (float_t)( tPrevMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike ) )	/* 前回の探索位置のHaar-like特徴量から急激に特徴量が落ちた */
					if ( (float_t)( tMaxPntInfo.nFtrValue ) < fThrFtrValue )
					{
						nFlgSrchComp = CB_TRUE;	/* 探索完了 */
						break;
					}
					else
					{
						/* 探索続行(Y方向に1シフトさせた位置を注目領域とする) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;
					}
				}
				else	/* 初回探索 */
				{
					if ( 0L < tMaxPntInfo.nFtrValue )
					{
						/* 探索続行(Y方向に1シフトさせた位置を注目領域とする) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;

						/* 初回探索時のHaar-like特徴量から、しきい値を設定 */
						fThrFtrValue = (float_t)( tMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike;
					}
					else
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
						break;
					}
				}

				/* 2回目以降の探索では、探索領域を狭める */
				/* パラメータ設定 */
				nprm_xsizeHalfScanRgn = CB_FPE_PRM_XSIZE_REFINE_HALF_SCAN_RGN;
			}	/* for ( nj ) */
		
			/* 信頼度チェック */
			if ( labs( tPrevMaxPntInfo.nFtrValue ) < nprm_thrHaarFtrTPnt )
			{
				ptDstPnt[ni].nX = -1L;
				ptDstPnt[ni].nY = -1L;
			}
			else
			{
				/* 候補点格納 */
				if ( nCoeffDirY < 0L )
				{
					ptDstPnt[ni].nX = tPrevMaxPntInfo.tPnt.nX;
					ptDstPnt[ni].nY = tPrevMaxPntInfo.tPnt.nY - nprm_ysizeBlockW;
				}
				else if ( 0L < nCoeffDirY )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6819	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6820
				{
					ptDstPnt[ni] = tPrevMaxPntInfo.tPnt;
				}
				else
				{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6821
					return ( CB_IMG_NG );
				}
			}
		}	/* if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) ) */

		/* 点数をインクリメント */
		nNumDstPnt++;
	}	/* for ( ni ) */

	*pnNumDstPnt= nNumDstPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		右/下側Ｔ字点(候補)の探索
 *
 * @param[in]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptCrossPnt		:,画像座標(十字位置)構造体へのポインタ,-,[-],
 * @param[in]	nNumCrossPnt	:,画像座標(十字位置)構造体の数,-,[-],
 * @param[out]	ptDstPnt		:,画像座標(Ｔ字位置)構造体へのポインタ,-,[-],
 * @param[out]	pnNumDstPnt		:,画像座標(Ｔ字位置)構造体の数,-,[-],
 * @param[in]	ptSrchRgn		:,矩形領域構造体へのポインタ,-,[-],
 * @param[in]	nEdgePos		:,探索方向,CB_FPE_LEFT_EDGE_POS_PATTERN<=value<=CB_FPE_RIGHT_EDGE_POS_PATTERN,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.18	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_xsizeBlockW;
	slong	nprm_ysizeHalfScanRgn;
	slong	nprm_diffHaarLike;
	slong	nprm_thrHaarFtrTPnt;
	slong	nprm_distanceSrchRgn;
	float_t	fprm_coeffThrHaarLike;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptCrossPnt ) || ( NULL == ptDstPnt ) || ( NULL == pnNumDstPnt ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_HORI_EDGE_BLOCK_XSIZE;
//	nprm_ysizeHalfScanRgn = CB_FPE_PRM_YSIZE_HALF_SCAN_RGN;
	nprm_diffHaarLike = CB_FPE_PRM_DIFF_HAAR_LIKE;
	fprm_coeffThrHaarLike = CB_FPE_PRM_COEFF_THR_HAAR_LIKE;
	nprm_thrHaarFtrTPnt = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_LR;
	nprm_distanceSrchRgn = CB_FPE_PRM_DISTANCE_SRCH_RGN_T_PNT_LR;

	/* 探索領域設定 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	/* クリア */
	nNumPnt = 0L;

	/* 探索方向の係数 */
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
			/* パラメータ設定 */
			nprm_ysizeHalfScanRgn = CB_FPE_PRM_YSIZE_HALF_SCAN_RGN;

//			nXattn = ptCrossPnt[ni].nX + ( nCoeffDirX * nprm_xsizeBlockW );
			nYattn = ptCrossPnt[ni].nY;
			if ( 0L < nCoeffDirX )
			{
				nXattn = ptCrossPnt[ni].nX + ( nCoeffDirX * ( nprm_xsizeBlockW + nprm_distanceSrchRgn ) );
				nXSizeScanRgn = ( nExSrchRgn - nXattn ) + 1L;
			}
			else if ( nCoeffDirX < 0L )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6822	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6823
			{
				nXattn = ptCrossPnt[ni].nX + ( nCoeffDirX * nprm_distanceSrchRgn );
				nXSizeScanRgn = ( nXattn - ( nSxSrchRgn + nprm_xsizeBlockW ) ) + 1L;
			}
			else
			{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6824
				/* エラー */
				return ( CB_IMG_NG );
			}
			nYSizeScanRgn = ( 2L * nprm_ysizeHalfScanRgn ) + 1L;
			tPrevMaxPntInfo.nFtrValue = -1L;	/* 無効値 */
			tPrevMaxPntInfo.tPnt.nX = -1L;	/* 無効値 */
			tPrevMaxPntInfo.tPnt.nY = -1L;	/* 無効値 */
			tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
			tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
			tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */
			nFlgSrchComp = CB_FALSE;

			/* X方向に探索を進める */
			fThrFtrValue = CB_FPE_FLT_MIN;
			for ( nj = 0L; nj < nXSizeScanRgn; nj++ )
			{
				/* 探索領域内チェック */
				if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nXattn, nYattn, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn ) )
				{
					if ( 0L >= tPrevMaxPntInfo.nFtrValue )
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
					}
					break;
				}

				pnWkFtr = ptHaarLike->pnFtrValue + ( ( nYattn - nprm_ysizeHalfScanRgn ) * ptHaarLike->nXSize ) + nXattn;

				/* Haar-like特徴量の絶対値が大きい点を探索 */
				tMaxPntInfo.nFtrValue = 0L;
				for ( nk = -nprm_ysizeHalfScanRgn; nk <= nprm_ysizeHalfScanRgn; nk++ )
				{
					if ( tMaxPntInfo.nFtrValue < labs( *pnWkFtr ) )
					{
						tMaxPntInfo.nFtrValue = labs( *pnWkFtr );
						tMaxPntInfo.tPnt.nX = nXattn;
						tMaxPntInfo.tPnt.nY = nYattn + nk;
					}

					/* Y方向に1pixelシフトさせる */
					pnWkFtr += ( ptHaarLike->nXSize );
				}

				/* 注目点更新 */
				if ( -1 < tPrevMaxPntInfo.nFtrValue )	/* 初回探索以外 */
				{
	//				if ( tMaxPntInfo.nFtrValue < ( tPrevMaxPntInfo.nFtrValue - nprm_diffHaarLike ) )	/* 前回の探索位置のHaar-like特徴量から急激に特徴量が落ちた */
	//				if ( (float_t)( tMaxPntInfo.nFtrValue ) < ( (float_t)( tPrevMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike ) )	/* 前回の探索位置のHaar-like特徴量から急激に特徴量が落ちた */
					if ( (float_t)( tMaxPntInfo.nFtrValue ) < fThrFtrValue )	
					{
						nFlgSrchComp = CB_TRUE;	/* 探索完了 */
						break;
					}
					else
					{
						/* 探索続行(X方向に1シフトさせた位置を注目領域とする) */
						nXattn = tMaxPntInfo.tPnt.nX + ( nCoeffDirX * 1L );
						nYattn = tMaxPntInfo.tPnt.nY;
						tPrevMaxPntInfo = tMaxPntInfo;
					}
				}
				else	/* 初回探索 */
				{
					if ( 0L < tMaxPntInfo.nFtrValue )
					{
						/* 探索続行(X方向に1シフトさせた位置を注目領域とする) */
						nXattn = tMaxPntInfo.tPnt.nX + ( nCoeffDirX * 1L );
						nYattn = tMaxPntInfo.tPnt.nY;
						tPrevMaxPntInfo = tMaxPntInfo;

						/* 初回探索時のHaar-like特徴量から、しきい値を設定 */
						fThrFtrValue = (float_t)( tMaxPntInfo.nFtrValue ) * fprm_coeffThrHaarLike;
					}
					else
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
						break;
					}
				}

				/* 2回目以降の探索では、探索領域を狭める */
				/* パラメータ設定 */
				nprm_ysizeHalfScanRgn = CB_FPE_PRM_YSIZE_REFINE_HALF_SCAN_RGN;
			}	/* for ( nj ) */
		
			/* 信頼度チェック */
			if ( labs( tPrevMaxPntInfo.nFtrValue ) < nprm_thrHaarFtrTPnt )
			{
				ptDstPnt[ni].nX = -1L;
				ptDstPnt[ni].nY = -1L;
			}
			else
			{
				/* 候補点格納 */
				if ( 0L < nCoeffDirX )
				{
					ptDstPnt[ni] = tPrevMaxPntInfo.tPnt;
				}
				else if ( nCoeffDirX < 0L )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6825	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6826
				{
					ptDstPnt[ni].nX = tPrevMaxPntInfo.tPnt.nX - nprm_xsizeBlockW;
					ptDstPnt[ni].nY = tPrevMaxPntInfo.tPnt.nY;
				}
				else
				{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6827
					/* エラー */
					return ( CB_IMG_NG );
				}
			}
		}	/* if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) ) */

		/* 点数をインクリメント */
		nNumPnt++;
	}	/* for ( ni ) */

	*pnNumDstPnt = nNumPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			拡大
 *
 * @param[in,out]	ptSrchRsltPntInfo		:,画像座標(十字位置)構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.24	K.Kato		新規作成
 */
/******************************************************************************/
static slong cb_FPE_ZoomIn4Coordinates( CB_FPE_SRCH_RSLT_PNT_INFO* ptSrchRsltPntInfo )
{
	slong	ni;
	/* パラメータ */
	slong	nprm_xmagZoomIn, nprm_ymagZoomIn;

	/* 引数チェック */
	if ( NULL == ptSrchRsltPntInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
	nprm_xmagZoomIn = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_ymagZoomIn = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* 十字 */
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
	/* Ｔ字(上側) */
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
	/* Ｔ字(下側) */
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
	/* Ｔ字(左側) */
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
	/* Ｔ字(右側) */
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
 * @brief			高精度位置決め(Checkerboard用)
 *
 * @param[in]		nImgSrc					:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in,out]	ptSrchRsltPntInfo		:,画像座標(十字位置)構造体へのポインタ,-,[-],
 * @param[in]		ptSrchRgnInfo			:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 * @param[in]		nCamDirection			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.12	K.Kato		新規作成
 *
 * @note			引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
 *
 */
/******************************************************************************/
static slong cb_FPE_PrecisionPositioning4Checkerboard( const IMPLIB_IMGID nImgSrc, CB_FPE_SRCH_RSLT_PNT_INFO *ptSrchRsltPntInfo, const CB_FPE_SRCH_RGN_INFO_EACH_PATTERN* const ptSrchRgnInfo, slong nCamDirection )
{
	slong	nRet = CB_IMG_NG;
	slong	ni;
	slong	nCrossX, nCrossY;
	/* パラメータ */
//	slong	nprm_xmagZoomIn, nprm_ymagZoomIn;

	/* 引数チェック */
	if ( ( NULL == ptSrchRsltPntInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
//	nprm_xmagZoomIn = CB_FPE_PRM_ZOOMOUT_XMAG;
//	nprm_ymagZoomIn = CB_FPE_PRM_ZOOMOUT_YMAG;

#ifdef TIME_MEASUREMENT_CALIB
	RG_TimeMeasurement( D_TM_START, &RG_Tim_CALIB[E_CALIB_TIME_4PNT_CROSS] );
#endif /* TIME_MEASUREMENT_CALIB */

	/* 市松パターンの交点を抽出 */
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

	/* パターンの縁用 */
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
 * @brief			特徴点位置の推定(パターンの縁用)
 *
 * @param[in]		nImgSrc					:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in,out]	ptSrchRsltPntInfo		:,画像座標(十字位置)構造体へのポインタ,-,[-],
 * @param[in]		ptSrchRgnInfo			:,カメラごとの探索領域情報構造体へのポインタ,-,[-],
 * @param[in]		nCamDirection			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.16	K.Kato		新規作成
 * @date			2017.10.12	A.Honda		探索範囲の丸め込み処理追加
 *
 * @note			引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	/* パラメータ */
	slong	nprm_distanceX = 0L, 
			nprm_distanceY = 0L;
	slong	nprm_WidthSrchRgn = 0L, 
			nprm_HeightSrchRgn = 0L;
	slong	nprm_halfWidthSrchRgn = 0L, 
			nprm_halfHeightSrchRgn = 0L;
	slong	nprm_sizeNoUse = 0L;

	/* 引数チェック */
	if ( ( NULL == ptSrchRsltPntInfo ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
	nprm_distanceX			= CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_X;
	nprm_distanceY			= CB_FPE_PRM_DISTANCE_SRCH_BASE_PNT_Y;
	nprm_WidthSrchRgn		= CB_FPE_PRM_WIDTH_PRECISE_SRCH_RGN;
	nprm_HeightSrchRgn		= CB_FPE_PRM_HEIGHT_PRECISE_SRCH_RGN;
	nprm_halfWidthSrchRgn	= CB_FPE_PRM_HALF_WIDTH_PRECISE_SRCH_RGN;
	nprm_halfHeightSrchRgn	= CB_FPE_PRM_HALF_HEIGHT_PRECISE_SRCH_RGN;
	nprm_sizeNoUse			= CB_FPE_PRM_PERIPHERAL_NO_USE;

	/* 先頭ポインタ */
	ptPixelDataUL		= &(m_FPE_tWorkInfo.tPixelDataUL);
	ptPixelDataLR		= &(m_FPE_tWorkInfo.tPixelDataLR);
	ptEdgeRhoUL			= &(m_FPE_tWorkInfo.tEdgeRhoUL);
	ptEdgeRhoLR			= &(m_FPE_tWorkInfo.tEdgeRhoLR);
	ptEdgePntUL			= &(m_FPE_tWorkInfo.tEdgePntUL[0L]);
	ptEdgePntLR			= &(m_FPE_tWorkInfo.tEdgePntLR[0L]);
	ptEdgePrecPntUL		= &(m_FPE_tWorkInfo.tEdgePrecPntUL[0L]);
	ptEdgePrecPntLR		= &(m_FPE_tWorkInfo.tEdgePrecPntLR[0L]);

	/* ===== 紐付け ===== */
	/* 画素値(縦方向) */
	ptPixelDataUL->ptRgn = &tEdgeFLTRgnUL;
	/* 画素値(横方向) */
	ptPixelDataLR->ptRgn = &tEdgeFLTRgnLR;
	/* エッジ強度(縦方向) */
	ptEdgeRhoUL->ptRgn = &tEdgeNoUseRgnUL;
	/* エッジ強度(横方向) */
	ptEdgeRhoLR->ptRgn = &tEdgeNoUseRgnLR;

	/* 各特徴点の情報を登録 */
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

	/* 重み付きHaar-like特徴量符号の取得 */
	nRet = cb_FPE_GetSignHaarLikeFeature( &( ptSrchRgnInfo->tPtnInfo ), nCamDirection, &nSignHaarLikeFeature );
	if( CB_IMG_OK != nRet )
	{
		return ( nRet );
	}

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgAccessInfo.nXSize ), &( tImgAccessInfo.nYSize ), (void**)&( tImgAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	for ( ni = 0L; ni < CB_FPE_EDGE_POS_PATTERN_MAXNUM; ni++ )
	{
		/*
		==================================================
			領域の設定方向
		==================================================
		*/
		nDirX = m_FPE_nDirX4PreciseSrchRgn[ni];
		nDirY = m_FPE_nDirY4PreciseSrchRgn[ni];

		/* 特徴点数による切り替え */
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
				/* ERRORではない */
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
				/* スキャン領域の設定 */
				/* 縦方向に設定する領域 */
				tSrchRgnUL.nSX = (ptPntArray[ni])[nj].nX - nprm_halfWidthSrchRgn;
				tSrchRgnUL.nSY = ( (ptPntArray[ni])[nj].nY + ( nDirY * nprm_distanceY ) ) - nprm_halfHeightSrchRgn;
				tSrchRgnUL.nEX = ( tSrchRgnUL.nSX + nprm_WidthSrchRgn ) - 1L;
				tSrchRgnUL.nEY = ( tSrchRgnUL.nSY + nprm_HeightSrchRgn ) - 1L;
				/* 横方向に設定する領域 */
				tSrchRgnLR.nSX = ( (ptPntArray[ni])[nj].nX + ( nDirX * nprm_distanceX ) ) - nprm_halfHeightSrchRgn;
				tSrchRgnLR.nSY = (ptPntArray[ni])[nj].nY - nprm_halfWidthSrchRgn;
				tSrchRgnLR.nEX = ( tSrchRgnLR.nSX + nprm_HeightSrchRgn ) - 1L;
				tSrchRgnLR.nEY = ( tSrchRgnLR.nSY + nprm_WidthSrchRgn ) - 1L;

				/* スキャン領域のチェック */
				nFlagChkRgn = cb_FPE_CheckRegion( tSrchRgnUL.nSX, tSrchRgnUL.nSY, tSrchRgnUL.nEX, tSrchRgnUL.nEY, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
				if ( CB_TRUE != nFlagChkRgn )
				{
					/* チェックNG時、領域再設定 */
					nRet = cb_FPE_ResetRegion( &tSrchRgnUL, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
					if ( CB_IMG_OK != nRet )
					{
						/* Openした画像メモリをCloseする */
						nRet = implib_CloseImgDirect( nImgSrc );
						return ( CB_IMG_NG );
					}
				}
				nFlagChkRgn = cb_FPE_CheckRegion( tSrchRgnLR.nSX, tSrchRgnLR.nSY, tSrchRgnLR.nEX, tSrchRgnLR.nEY, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
				if ( CB_TRUE != nFlagChkRgn )
				{
					/* チェックNG時、領域再設定 */
					nRet = cb_FPE_ResetRegion( &tSrchRgnLR, tImgAccessInfo.nXSize, tImgAccessInfo.nYSize );
					if ( CB_IMG_OK != nRet )
					{
						/* Openした画像メモリをCloseする */
						nRet = implib_CloseImgDirect( nImgSrc );
						return ( CB_IMG_NG );
					}
				}

				/* スキャン方向の設定 */
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
					/* エラー */
					/* Openした画像メモリをCloseする */
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
					/* エラー */
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ===== 画素値の取り出し ===== */
				/*
					[MEMO]
					縦方向、横方向どちらでも下記パターンに配列されるように取り出す。

					■■・・■■□□・・□□
					■■・・■■□□・・□□
					■■・・■■□□・・□□
					■■・・■■□□・・□□
				*/

				/* 縦方向 */
				tImgAccessInfo.ptRgn = &tSrchRgnUL;
				nRet = cb_FPE_StorePixelValue( &tImgAccessInfo, ptPixelDataUL, nScanDirX, CB_FPE_SCAN_DIR_Y_PLUS, CB_FPE_NO_SHUFFLE );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 横方向 */
				tImgAccessInfo.ptRgn = &tSrchRgnLR;
				nRet = cb_FPE_StorePixelValue( &tImgAccessInfo, ptPixelDataLR, CB_FPE_SCAN_DIR_X_PLUS, nScanDirY, CB_FPE_SHUFFLE );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ===== ラインエッジフィルタ ===== */
				/* 縦方向 */
				ptPixelDataUL->ptRgn->nSX = 0L;
				ptPixelDataUL->ptRgn->nSY = 0L;
				ptPixelDataUL->ptRgn->nEX = tSrchRgnUL.nEX - tSrchRgnUL.nSX;
				ptPixelDataUL->ptRgn->nEY = tSrchRgnUL.nEY - tSrchRgnUL.nSY;
				nRet = cb_FPE_LineEdgeFLT( ptPixelDataUL, ptEdgeRhoUL );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 横方向 */
				ptPixelDataLR->ptRgn->nSX = 0L;
				ptPixelDataLR->ptRgn->nSY = 0L;
				ptPixelDataLR->ptRgn->nEX = tSrchRgnLR.nEY - tSrchRgnLR.nSY;
				ptPixelDataLR->ptRgn->nEY = tSrchRgnLR.nEX - tSrchRgnLR.nSX;
				nRet = cb_FPE_LineEdgeFLT( ptPixelDataLR, ptEdgeRhoLR );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* ===== エッジ点抽出 ===== */
				/* 縦方向 */
				ptEdgeRhoUL->ptRgn->nSX = ptPixelDataUL->ptRgn->nSX + nprm_sizeNoUse;
				ptEdgeRhoUL->ptRgn->nSY = ptPixelDataUL->ptRgn->nSY;
				ptEdgeRhoUL->ptRgn->nEX = ptPixelDataUL->ptRgn->nEX - nprm_sizeNoUse;
				ptEdgeRhoUL->ptRgn->nEY = ptPixelDataUL->ptRgn->nEY;
				nRet = cb_FPE_ExtractEdgePoint( ptEdgeRhoUL, ptPixelDataUL, ptEdgePntUL, &nNumEdgePntUL );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 横方向 */
				ptEdgeRhoLR->ptRgn->nSX = ptPixelDataLR->ptRgn->nSX + nprm_sizeNoUse;
				ptEdgeRhoLR->ptRgn->nSY = ptPixelDataLR->ptRgn->nSY;
				ptEdgeRhoLR->ptRgn->nEX = ptPixelDataLR->ptRgn->nEX - nprm_sizeNoUse;
				ptEdgeRhoLR->ptRgn->nEY = ptPixelDataLR->ptRgn->nEY;
				nRet = cb_FPE_ExtractEdgePoint( ptEdgeRhoLR, ptPixelDataLR, ptEdgePntLR, &nNumEdgePntLR );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}
				
				/* 画像原点相対(点の下側) */
			
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

				/* 画像原点相対(点の左右側) */
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

				/* 高精度位置決め(点の上側) */
				nRet = cb_FPE_ExtractEdgePrecisePoint( &tImgAccessInfo, ptEdgePntUL, nNumEdgePntUL, ptEdgePrecPntUL, &nNumEdgePrecPntUL, CB_FPE_LINE_FLT_VERT_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 高精度位置決め(点の下側) */
				nRet = cb_FPE_ExtractEdgePrecisePoint( &tImgAccessInfo, ptEdgePntLR, nNumEdgePntLR, ptEdgePrecPntLR, &nNumEdgePrecPntLR, CB_FPE_LINE_FLT_HORI_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 水平/垂直の直線を近似し、交点を計算 */
				nRet = cb_FPE_CalcIntersectionPoint( ptEdgePrecPntUL, ptEdgePrecPntLR, nNumEdgePrecPntUL, nNumEdgePrecPntLR, &( ptRsltPntArray[ni][nj] ) );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 交点計算が失敗しているときは、十字点の荒い探索結果を格納する */
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
				/* 結果格納 */
				(ptRsltPntArray[ni])[nj].w = -1.0;
				(ptRsltPntArray[ni])[nj].h = -1.0;
				(ptRsltPntArray[ni])[nj].nFlagPrecise = CB_FALSE;
			}	/* if ( ( 0L <= (ptPntArray[ni])[nj].nX ) && ( 0L <= (ptPntArray[ni])[nj].nY ) ) */

			nDirX = nDirX * (-1L);
		}	/* for ( nj = 0; nj < nNumPntArray[ni]; nj++ ) */

	}	/* for ( ni = 0; ni < 2; ni++ ) */

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			高精度位置決め(Checkerboard用)
 *
 * @param[in]		nImgSrc					:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCrossX					:,市松パターンの交点X,-,[-],
 * @param[in]		nCrossY					:,市松パターンの交点Y,-,[-],
 * @param[in]		ptPrecisePnt			:,点情報,-,[-],,
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.16	K.Kato		新規作成
 */
/******************************************************************************/
static slong cb_FPE_methodFourPointCross( IMPLIB_IMGID nImgSrc, slong nCrossX, slong nCrossY, t_cb_img_CenterPos *ptPrecisePnt )
{
	slong	nSxSrc0, nSySrc0, nExSrc0, nEySrc0;
	slong	nSxDst, nSyDst, nExDst, nEyDst;
	slong	ni;
	slong	nRet = CB_IMG_NG;							/* 戻り値格納用変数 */
	slong				nSxWin, nSyWin, nExWin, nEyWin;					/* ウィンドウ切り出し時の座標 */
	double_t			dXUpArea, dYUpArea, dXDownArea, dYDownArea, dXLeftArea, dYLeftArea, dXRightArea, dYRightArea;	/* 交点を求める4点の座標 */
	slong					nMaxData;						/* 中間値の最大値 */
	slong					nMinData;						/* 中間値の最小値 */
	double_t				dResPoint;						/* 濃度累積値投影から求めたサブピクセル */
	slong					nResPointTemp;					/* サブピクセル算出用ワークバッファ */
	slong					nResPointIdx;					/* サブピクセル算出時の中央近辺を抽出する為のインデックス */
	double_t				dPrecisePoint[CB_FPE_PORJECT_AREA_MAXNUM];		/* 4点のサブピクセル格納テーブル */
	double_t				dSlopeHori;					/* 水平方向上下2点の直線係数（傾き） */
	double_t				dInterceptHori;					/* 水平方向上下2点の直線係数（欠辺） */
	double_t				dSlopeVert;						/* 垂直方向左右2点の直線係数（傾き） */
	double_t				dInterceptVert;						/* 垂直方向左右2点の直線係数（欠辺） */
	slong					nProjectionIdx;					/* 濃度累積値投影を行うエリアのループインデックス */
	slong					nThrIntensity;
	slong					*pnTblMedian;
	slong					*pnTblProjGO;
	IMPLIB_IPGOFeatureTbl	*ptGOFtrTbl;
	/* パラメータ */
	slong	nprm_NumProjectData = CB_FPE_PRM_PROJECTION_DATA_NUM;
	slong	nprm_NumProjectDataHalf = CB_FPE_PRM_PROJECTION_DATA_NUM_HALF;
	slong	nprm_ProjectCenterInt = CB_FPE_PRM_PROJECTION_CENTER_INTERVAL;
	slong	nprm_WidthProject = CB_FPE_PRM_PROJECTION_WIDTH;

	/* 引数チェック */
	if ( NULL == ptPrecisePnt )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/***
	 * 変数初期化
	 ***/
	memset( &( dPrecisePoint[0] ), 0x00, ( sizeof( double_t ) * CB_FPE_PORJECT_AREA_MAXNUM ) );
	dSlopeHori = 0.0;
	dInterceptHori = 0.0;
	dSlopeVert = 0.0;
	dInterceptVert = 0.0;

	/* 先頭ポインタ */
	pnTblMedian = &(m_FPE_tWorkInfo.nTblMedian[0]);
	pnTblProjGO = &(m_FPE_tWorkInfo.nTblProjGO[0]);
	ptGOFtrTbl = &(m_FPE_tWorkInfo.tGOFtrTbl);

	/***
	 * 求めた中心座標の上下左右4箇所の濃度累積値投影を行い、
	 * 4箇所の濃淡境界点を求めて、その4点の交点を求める。
	 ***/
	for ( nProjectionIdx = 0L; nProjectionIdx < CB_FPE_PORJECT_AREA_MAXNUM; nProjectionIdx++ )
	{
		/* クリア */
		memset( pnTblProjGO, 0x00, ( sizeof( slong ) * ( size_t )nprm_NumProjectData ) );
		memset( pnTblMedian, 0x00, ( sizeof( slong ) * ( size_t )nprm_NumProjectData ) );

		switch ( nProjectionIdx )
		{
			case ( CB_FPE_PROJECT_UP_AREA ):
				/***
				 * パターンマッチングで求めた中心点から水平方向上部の投影用ウィンドウ切り出し
				 ***/
				/* （切り出し座標で"-1"としているのは、原点座標を含む長さを考慮している為） */
				nSxWin = nCrossX - ( nprm_NumProjectDataHalf - 1L );
				nExWin = ( nSxWin + nprm_NumProjectData ) - 1L;
				nSyWin = nCrossY - ( nprm_ProjectCenterInt + nprm_WidthProject );
				nEyWin = ( nSyWin + nprm_WidthProject ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonX( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* エラー情報クリア */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			case ( CB_FPE_PROJECT_DOWN_AREA ):
				/***
				 * パターンマッチングで求めた中心点から水平方向下部の投影用ウィンドウ切り出し
				 ***/
				/* （切り出し座標で"-1"としているのは、原点座標を含む長さを考慮している為） */
				nSxWin = nCrossX - ( nprm_NumProjectDataHalf - 1L );
				nExWin = ( nSxWin + nprm_NumProjectData ) - 1L;
				nSyWin = nCrossY + nprm_ProjectCenterInt;
				nEyWin = ( nSyWin + nprm_WidthProject ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonX( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* エラー情報クリア */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			case ( CB_FPE_PROJECT_LEFT_AREA ):
				/***
				 * パターンマッチングで求めた中心点から垂直方向左部の投影用ウィンドウ切り出し
				 ***/
				/* （切り出し座標で"-1"としているのは、原点座標を含む長さを考慮している為） */
				nSxWin = nCrossX - ( nprm_ProjectCenterInt + nprm_WidthProject );
				nExWin = ( nSxWin + nprm_WidthProject ) - 1L;
				nSyWin = nCrossY - ( nprm_NumProjectDataHalf - 1L );
				nEyWin = ( nSyWin + nprm_NumProjectData ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonY( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* エラー情報クリア */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			case ( CB_FPE_PROJECT_RIGHT_AREA ):
				/***
				 * パターンマッチングで求めた中心点から垂直方向右部の投影用ウィンドウ切り出し
				 ***/
				/* （切り出し座標で"-1"としているのは、原点座標を含む長さを考慮している為） */
				nSxWin = nCrossX - ( nprm_ProjectCenterInt + nprm_WidthProject );
				nExWin = ( nSxWin + nprm_WidthProject ) - 1L;
				nSyWin = nCrossY - ( nprm_NumProjectDataHalf - 1L );
				nEyWin = ( nSyWin + nprm_NumProjectData ) - 1L;

				nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxWin, nSyWin, nExWin, nEyWin );
				nRet = implib_IP_ProjectGOonY( nImgSrc, pnTblProjGO, ptGOFtrTbl );
				if ( 0L > nRet )
				{
					/* エラー情報クリア */
					nRet = implib_ClearIPError();
					return ( CB_IMG_NG_IMPLIB );
				}

				break;
			default:		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6844
				break;
		}

		/***
		 * 濃度累積値投影結果に対して、3画素の中間値を求める。
		 * 投影結果のテーブルglTblW[]の隣り合う3点glTblW[i],glTblW[i+1],glTblW[i+2]
		 * の値を比較し、真ん中の値を抽出する。
		 ***/
		for ( ni = 1L; ni < ( nprm_NumProjectData - 1L ); ni++ )
		{
			if ( pnTblProjGO[ni - 1L] < pnTblProjGO[ni] )
			{
				if ( pnTblProjGO[ni + 1L] < pnTblProjGO[ni - 1L] )
				{
					pnTblMedian[ni] = pnTblProjGO[ni - 1L];	/* Medianはni-1 */
				}
				else
				{
					if ( pnTblProjGO[ni] < pnTblProjGO[ni + 1L] )
					{
						pnTblMedian[ni] = pnTblProjGO[ni];	/* Medianはni */
					}
					else
					{
						pnTblMedian[ni] = pnTblProjGO[ni + 1L];	/* Medianはni+1 */
					}
				}
			}
			else
			{
				if ( pnTblProjGO[ni - 1L] < pnTblProjGO[ni + 1L] )
				{
					pnTblMedian[ni] = pnTblProjGO[ni - 1L];	/* Medianはni-1 */
				}
				else
				{
					if ( pnTblProjGO[ni] < pnTblProjGO[ni + 1L] )
					{
						pnTblMedian[ni] = pnTblProjGO[ni + 1L];	/* Medianはni+1 */
					}
					else
					{
						pnTblMedian[ni] = pnTblProjGO[ni];	/* Medianはni */
					}
				}
			}
		}
		
		/***
		 * 中間値の最大値と最小値を求めて、濃淡変化点の閾値を算出する。
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
		/* しきい値 */
		nThrIntensity = ( nMinData + nMaxData ) / 2L;

		/***
		 * 濃度累積値投影結果から閾値を挟む2点を抽出する。
		 * 抽出した2点のうち、閾値より大きな値を持つ点の座標に対して、
		 * 閾値までのサブピクセルを算出する。
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
					/* pnTblProjGO[] は nThrIntensity を閾値としていて、0にはならないので、0割チェック不要 */
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
					/* pnTblProjGO[] は nThrIntensity を閾値としていて、0にはならないので、0割チェック不要 */
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
				/* エラー処理追加する！！ */;
			}
		}

		/***
		 * サブピクセルを格納。
		 ***/
		dPrecisePoint[nProjectionIdx] = dResPoint;
	}	/* for ( nProjectionIdx = 0; nProjectionIdx < CB_FPE_PORJECT_AREA_MAXNUM; nProjectionIdx++ ) */

	/***
	 * サブピクセルを用いて、4点の座標を算出。
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
	 * 4点の座標を用いて、4点の交点を算出。
	 ***/
	 /***
	  * 水平方向上下2点座標(up_x,up_y),(down_x,down_y)を解の方程式を用いて
	  * 直線係数計算x=horizontal1*y+horizontal2を算出する。
	  ***/
	if ( CB_FPE_DBL_MIN <= fabs( dYUpArea - dYDownArea ) )
	{
		dSlopeHori = ( dXUpArea - dXDownArea ) / ( dYUpArea - dYDownArea );
		dInterceptHori = dXUpArea - ( dSlopeHori * dYUpArea );
	}
	else
	{
		/***
		 * 上下2点の座標でY座標が一致する場合は、認識失敗の為、エラー
		 ***/
		return ( CB_IMG_NG );
	}

	/***
	  * 垂直方向左右2点座標(left_x,left_y),(right_x,right_y)を解の方程式を用いて
	  * 直線係数計算x=vertical1*y+vertical2を算出する。
	  ***/
	if ( CB_FPE_DBL_MIN <= fabs( dYLeftArea - dYRightArea ) )
	{
		dSlopeVert = ( dXLeftArea - dXRightArea ) / ( dYLeftArea - dYRightArea );
		dInterceptVert = dXLeftArea - ( dSlopeVert * dYLeftArea );
		/***
		 * 水平と垂直の2直線の傾き(horizontal1, vertical1)と欠辺(horizontal2, vertical2)から
		 * 交点を求める。
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
			 * 水平と垂直の2直線の傾き(horizontal1, vertical1)が一致する場合は、
			 * 直線が交わることが無い為、エラー
			 ***/
			return ( CB_IMG_NG );
		}
	}
	else
	{
		/***
		 * 垂直方向左右2点のY座標が一致している場合は、Y座標は定数。
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
			 * 垂直方向左右2点のY座標が一致していて、かつ水平方向上下2点のX座標が
			 * 一致している場合は、X座標、Y座標共に定数。
			 ***/
			ptPrecisePnt->h = dYLeftArea;
			ptPrecisePnt->w = dXUpArea;
			ptPrecisePnt->nFlagPrecise = CB_TRUE;
		}
	}

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return CB_IMG_OK;
}

/******************************************************************************/
/**
 * @brief			Median Filter(Line)
 *
 * @param[in]		pnSrc					:,輝度値の配列へのポインタ(SRC),-,[-],
 * @param[out]		pnDst					:,輝度値の配列へのポインタ(DST),-,[-],
 * @param[in]		nNumData				:,輝度値の配列のデータ数,0<value,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.18	K.Kato		新規作成
 */
/******************************************************************************/
static slong cb_FPE_MedianLineFilter( const slong* const pnSrc, slong* pnDst, slong nNumData )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] M1.1.1  R-52, ID-6845
{
	slong	ni;

	/* 引数チェック */
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
				pnDst[ni] = pnSrc[ni - 1L];	/* Medianはni-1 */
			}
			else
			{
				if ( pnSrc[ni] < pnSrc[ni + 1L] )
				{
					pnDst[ni] = pnSrc[ni];	/* Medianはni */
				}
				else
				{
					pnDst[ni] = pnSrc[ni + 1L];	/* Medianはni+1 */
				}
			}
		}
		else
		{
			if ( pnSrc[ni - 1L] < pnSrc[ni + 1L] )
			{
				pnDst[ni] = pnSrc[ni - 1L];	/* Medianはni-1 */
			}
			else
			{
				if ( pnSrc[ni] < pnSrc[ni + 1L] )
				{
					pnDst[ni] = pnSrc[ni + 1L];	/* Medianはni+1 */
				}
				else
				{
					pnDst[ni] = pnSrc[ni];	/* Medianはni */
				}
			}
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			ソート
 *
 * @param[out]		pnValue					:,ソート対象のデータ配列へのポインタ,-,[-],
 * @param[in]		nNumValue				:,ソート対象のデータ配列数,0<value,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.24	K.Kato		新規作成
 */
/******************************************************************************/
static slong cb_FPE_Sort( slong* pnValue, slong nNumValue )
{
	slong	ni, nj;
	slong	nTempValue;

	/* 引数チェック */
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
 * @brief			領域内の画素値格納
 *
 * @param[in]		ptImgAccessInfo			:,画像メモリアクセス情報へのポインタ(SRC),-,[-],
 * @param[in,out]	ptStore					:,画像メモリアクセス情報へのポインタ(DST),-,[-],
 * @param[in,out]	nScanDirX				:,スキャン方向(X方向),CB_FPE_SCAN_DIR_X_PLUS<=value<=CB_FPE_SCAN_DIR_X_MINUS,[-],
 * @param[in,out]	nScanDirY				:,スキャン方向(Y方向),CB_FPE_SCAN_DIR_Y_PLUS<=value<=CB_FPE_SCAN_DIR_Y_MINUS,[-],
 * @param[in,out]	nShuffleMode			:,X/Y方向のシャッフルモード,CB_FPE_NO_SHUFFLE<=value<=CB_FPE_SHUFFLE,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.12	K.Kato		新規作成
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

	/* 引数チェック */
	if ( ( NULL == ptImgAccessInfo ) || ( NULL == ptStore ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 領域のサイズ */
	nXSizeRgn = ( ptImgAccessInfo->ptRgn->nEX - ptImgAccessInfo->ptRgn->nSX ) + 1L;
	nYSizeRgn = ( ptImgAccessInfo->ptRgn->nEY - ptImgAccessInfo->ptRgn->nSY ) + 1L;

	/* 先頭ポインタ */
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
		/* エラー */
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
		/* エラー */
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

				/* 次のアクセス先へ */
				pnPixelDst++;
				nxSrc = nxSrc + nCoeffDirX;
			}

			/* 次のアクセス先へ */
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

				/* 次のアクセス先へ */
				pnPixelDst++;
				nySrc = nySrc + nCoeffDirY;
			}

			/* 次のアクセス先へ */
			nxSrc = nxSrc + nCoeffDirX;
		}
	}
	else
	{
		/* エラー */
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			ラインフィルタ
 *
 * @param[in]		nImgSrc					:,画像メモリアクセス情報(SRC)へのポインタ,-,[-],
 * @param[in,out]	ptEdgeDst				:,エッジ画像メモリアクセス情報(DST)へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.13	K.Kato		新規作成
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

	/* パラメータ */
	nSizeNoUse = CB_FPE_PRM_PERIPHERAL_NO_USE;

	/* 先頭ポインタ */
	ptProcRgn = ptImgSrc->ptRgn;

	/* 領域サイズ */
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
				nSum += ( nCoeffEdgeFLT[nSizeNoUse + ni] * (slong)(*(pnAddrPixelSrc + ni)) );	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O3.1  , ID-6855
			}
			
			/* 値格納 */
			*pnAddrPixelDst = (sshort)nSum;

			/* 次のアドレス */
			pnAddrPixelSrc++;
			pnAddrPixelDst++;
		}
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			エッジ点抽出
 *
 * @param[in]		ptEdgeSrc				:,エッジ画像メモリアクセス情報へのポインタ,-,[-],
 * @param[in,out]	ptPixelInfo				:,画像メモリアクセス情報へのポインタ,-,[-],
 * @param[in]		ptEdgePnt				:,エッジ位置へのポインタ,[-],
 * @param[in,out]	pnNumEdgePnt			:,エッジの数を格納する領域へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.14	K.Kato		新規作成
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
	/* パラメータ */
	slong	nprm_thrCntDown;
	slong	nprm_thrEdgeRho;

	/* 引数チェック */
	if ( ( NULL == ptEdgeSrc ) || ( NULL == ptPixelInfo ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}
	
	/* パラメータ */
	nprm_thrEdgeRho = CB_FPE_PRM_THR_EDGE_RHO;
	nprm_thrCntDown = CB_FPE_PRM_THR_EDGE_CNT_DOWN;
	
	/* 領域サイズ */
	nXSize = ( ptEdgeSrc->ptRgn->nEX - ptEdgeSrc->ptRgn->nSX ) + 1L;
	nYSize = ( ptEdgeSrc->ptRgn->nEY - ptEdgeSrc->ptRgn->nSY ) + 1L;

	/* 先頭ポインタ */
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
			/* 登録済みのエッジ強度より大きいエッジ強度か */
			if ( nMaxEdgeRho < (*pnAddrEdgeRho) )
			{
				nMaxEdgeRho = (*pnAddrEdgeRho);
				nxMax = nx;

				nCntDown = 0L;
			}
			/* 最大エッジ強度が登録済み AND 注目のエッジ強度が最大エッジ強度より小さい */
			else if ( ( nprm_thrEdgeRho < nMaxEdgeRho ) && ( nMaxEdgeRho > (*pnAddrEdgeRho) ) )
			{
				nCntDown++;
			}
			else
			{
				/* 処理しない */;
			}

			/* 最大エッジ強度から連続してエッジ強度が減少しているか */
			if ( nprm_thrCntDown <= nCntDown )
			{
				break;
			}

			/* 次のアドレス */
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
 * @brief			エッジ点抽出(高精度位置決め)
 *
 * @param[in]		ptImgSrc				:,ソース画像メモリアクセス情報へのポインタ,-,[-],
 * @param[in]		ptEdgePnt				:,エッジ位置へのポインタ,-,[-],
 * @param[in]		nNumEdgePnt				:,エッジの数,0<value,[-],
 * @param[out]		ptEdgePrecPnt			:,エッジ位置(高精度)へのポインタ,-,[-],
 * @param[in]		nFltMode				:,エッジ種別,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE,[-],
 * @param[in]		nScanDirX				:,スキャン方向(X方向),CB_FPE_SCAN_DIR_X_PLUS<=value<=CB_FPE_SCAN_DIR_X_MINUS,[-],
 * @param[in]		nScanDirY				:,スキャン方向(Y方向),CB_FPE_SCAN_DIR_Y_PLUS<=value<=CB_FPE_SCAN_DIR_Y_MINUS,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.21	K.Kato		新規作成
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

	/* 引数チェック */
	if ( ( NULL == ptImgSrc ) || ( NULL == ptEdgePnt ) || ( NULL == ptEdgePrecPnt ) || ( NULL == pnNumEdgePrecPnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
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
			/* エラー */
			return ( CB_IMG_NG );
		}

		/* <<<<<しきい値決め>>>>> */
		/* 画素値の取得 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntX = ptEdgePnt[ni].nX;

			/* 黒領域側 */
			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 6L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[1] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}

			/* 白領域側 */
			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 5L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[2] */	// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O4.1  R-4, ID-6853
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
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
		/* 代表値 */
		nIdxMedian = nNumEdgePnt / 2L;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];
		/* しきい値 */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<サブピクセル推定>>>>> */
		nNumEdgePrecPnt = 0L;
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntYBlack = -1L;
			if ( nCoeffDirY < 0L )
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[1],[2]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirYの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
			else if ( 0L < nCoeffDirY )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6861	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6862
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[1],[2]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirYの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
			{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6863
				/* エラー */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntYBlack )
			{
				nDistanceIntensity = ( nPixelValueCur - nPixelValuePrev );
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrev;
				dDistanceIntensityWhite = (double_t)nPixelValueCur - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0割チェック追加 */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0割時の処理 */
					/* ここでの0割時の適切な処理については、要再検討 */
					/* 
					   => 下記割合は0.5とする。
					      nPixelValueCurとnPixelValuePrevは隣接しているので、同じ輝度値の場合は、その中間位置とする
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* 高精度 */
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
			/* エラー */
			return ( CB_IMG_NG );
		}

		/* <<<<<しきい値決め>>>>> */
		/* 画素値の取得 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntY = ptEdgePnt[ni].nY;

			/* 黒領域側 */
			nPntX = ptEdgePnt[ni].nX - ( nCoeffDirX * 6L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[3] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}

			/* 白領域側 */
			nPntX = ptEdgePnt[ni].nX + ( nCoeffDirX * 5L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[4] */	// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O4.1  R-4, ID-6857
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
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
		/* 代表値 */
		nIdxMedian = nNumEdgePnt / 2L;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];
		/* しきい値 */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<サブピクセル推定>>>>> */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntXBlack = -1L;
			if ( nCoeffDirX < 0L )
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[3],[4]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirXの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
			else if ( 0L < nCoeffDirX )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6866	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6867
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[3],[4]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirXの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
			{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6868
				/* エラー */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntXBlack )
			{
				nDistanceIntensity = ( nPixelValueCur - nPixelValuePrev );
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrev;
				dDistanceIntensityWhite = (double_t)nPixelValueCur - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0割チェック追加 */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0割時の処理 */
					/* ここでの0割時の適切な処理については、要再検討 */
					/* 
					   => 下記割合は0.5とする。
					      nPixelValueCurとnPixelValuePrevは隣接しているので、同じ輝度値の場合は、その中間位置とする
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* 高精度 */
				ptEdgePrecPnt[nNumEdgePrecPnt].w = (double_t)nPntXBlack + ( (double_t)nCoeffDirX * ( ( dRatioBlack * (double_t)nDistancePixel ) /*- 1.0*/ ) );
				ptEdgePrecPnt[nNumEdgePrecPnt].h = (double_t)ptEdgePnt[ni].nY;

				nNumEdgePrecPnt++;
			}
		}
	}
	else
	{
		/* エラー */
		return ( CB_IMG_NG );
	}

	/* 高精度で求められた点数格納 */
	*pnNumEdgePrecPnt = nNumEdgePrecPnt;

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		線形回帰
 *
 * @param[in]	ptPoint				:,点情報,-,[-],
 * @param[in]	nNumPoint			:,点の数,0<=value<?,[-],
 * @param[out]	pdSlope				:,直線の傾き格納先,-,[-],
 * @param[out]	pdIntercept			:,直線の切片格納先,-,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.14	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_LinearRegression4VertLine( const t_cb_img_CenterPos* const ptPoint, slong nNumPoint, double_t* pdSlope, double_t* pdIntercept )
{
	slong	ni;
	double_t	dSum_YX, dSum_X, dSum_Y, dSum_Ypower2;
	double_t	dDenominator, dNumerator;

	/* 引数チェック */
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

		/* 傾き・切片 */
		if ( CB_FPE_DBL_MIN <= fabs( dDenominator ) )
		{
			*pdSlope = dNumerator / dDenominator;
			*pdIntercept = ( dSum_X - ( (*pdSlope) * dSum_Y ) ) / (double_t)nNumPoint;
		}
		else
		{
			/* X軸に水平な直線 */
			*pdSlope = 0.0;
			*pdIntercept = 0.0;
			return ( CB_IMG_NG_DIV0 );
		}
	}
	else
	{
		/* 0割時の処理 */
		/* ここでの0割時の適切な処理については、要再検討（仮でCB_IMG_NGを返す） */
		/* 
		   => 本関数としては、計算できないのであればNGを返すのみとする。
		      NG時の対応は上位にて行うこととする。
			  (2013.08.07 K.Kato)
	    */
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		線形回帰
 *
 * @param[in]	ptPoint			:,点情報,-,[-],
 * @param[in]	nNumPoint		:,点の数,0<=value<?,[-],
 * @param[out]	pdSlope			:,直線の傾き格納先,-,[-],
 * @param[out]	pdIntercept		:,直線の切片格納先,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.14	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_LinearRegression4HoriLine( const t_cb_img_CenterPos* const ptPoint, slong nNumPoint, double_t* pdSlope, double_t* pdIntercept )
{
	slong	ni;
	double_t	dSum_XY, dSum_X, dSum_Y, dSum_Xpower2;
	double_t	dDenominator, dNumerator;

	/* 引数チェック */
	if ( ( NULL == ptPoint ) || ( NULL == pdSlope ) || ( NULL == pdIntercept ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	if ( 2L <= nNumPoint )	/* 0割チェック追加 */
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

		/* 傾き・切片 */
		if ( CB_FPE_DBL_MIN <= fabs( dDenominator ) )
		{
			*pdSlope = dNumerator / dDenominator;
			*pdIntercept = ( dSum_Y - ( (*pdSlope) * dSum_X ) ) / (double_t)nNumPoint;
		}
		else
		{
			/* X軸に垂直な直線 */
			*pdSlope = 0.0;
			*pdIntercept = 0.0;
			return ( CB_IMG_NG_DIV0 );
		}
	}
	else
	{
		/* 0割時の処理 */
		/* ここでの0割時の適切な処理については、要再検討（仮でCB_IMG_NGを返す） */
		/* 
		   => 本関数としては、計算できないのであればNGを返すのみとする。
		      NG時の対応は上位にて行うこととする。
			  (2013.08.07 K.Kato)
	    */
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		直線の交点
 *
 * @param[in]	ptLineCoeff1	:,直線係数構造体へのポインタ,-,[-],
 * @param[in]	ptLineCoeff2	:,直線係数構造体へのポインタ,-,[-],
 * @param[out]	pdIntercept		:,直線の交点格納先,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.14	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_CalcIntersection( const CB_FPE_LINE_COEFF* const ptLineCoeff1, const CB_FPE_LINE_COEFF* const ptLineCoeff2, t_cb_img_CenterPos* ptIntersectPnt )
{
	/* 引数チェック */
	if ( ( NULL == ptLineCoeff1 ) || ( NULL == ptLineCoeff2 ) || ( NULL == ptIntersectPnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	if ( CB_FPE_DBL_MIN <= fabs( ptLineCoeff1->dSlope - ptLineCoeff2->dSlope ) )	/* 0割チェック追加 */
	{
		ptIntersectPnt->h = ( ptLineCoeff2->dIntercept - ptLineCoeff1->dIntercept ) / ( ptLineCoeff1->dSlope - ptLineCoeff2->dSlope );
		ptIntersectPnt->w = ( ptLineCoeff1->dSlope * ptIntersectPnt->h ) + ptLineCoeff1->dIntercept;
	}
	else
	{
		/* 0割時の処理 */
		/* あり得ないパターンなのでNGを返す */
		return ( CB_IMG_NG_DIV0 );
	}
	
	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		直線の傾き・切片を算出
 *
 * @param[in]	pfCoord1		:,直線上の点1へのポインタ,-,[-],
 * @param[in]	pfCoord2		:,直線上の点2へのポインタ,-,[-],
 * @param[out]	pfSlope			:,直線の傾き格納先,-,[-],
 * @param[out]	pfIntercept		:,直線の切片格納先,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.05.27	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_CalcLineCoeff_F( const float_t* const pfCoord1, const float_t* const pfCoord2, float_t* pfSlope, float_t* pfIntercept )
{
	float_t	fDiffY = 0.0f;

	/* 引数チェック */
	if ( ( NULL == pfCoord1 ) || ( NULL == pfCoord2 ) || ( NULL == pfSlope ) || ( NULL == pfIntercept ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* Y成分の変化量 */
	fDiffY = pfCoord1[1L] - pfCoord2[1L];
	/* 傾き */
	if ( CB_FPE_FLT_MIN <= fabsf( fDiffY ) )	/* 0割りチェック */
	{
		*pfSlope = ( pfCoord1[0L] - pfCoord2[0L] ) / fDiffY;
	}
	else
	{
		*pfSlope = 0.0f;
	}
	/* 切片 */
	*pfIntercept = pfCoord1[0L] - ( (*pfSlope) * pfCoord1[1L] );

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		処理結果データの格納
 *
 * @param[in]	ptSrchRsltInfo		:,探索結果情報構造体へのポインタ,-,[-],
 * @param[out]	ptCenterPos			:,特徴点抽出結果構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgn			:,探索領域情報構造体へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.18	K.Kato			新規作成
 *
 * @note		引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	/* パラメータ */
	slong	nprm_xMagZoomOut = 0L, 
			nprm_yMagZoomOut = 0L;

	/* 引数チェック */
	if ( ( NULL == ptSrchRsltInfo ) || ( NULL == ptCenterPos ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* 先頭ポインタ */
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

			/* アルゴリズムタイプ */
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
		/* 何もしない */;
	}

	/* 点数 */
	ptCenterPos->CenterNum = (ulong)( ptSrchRsltPnt->nNumCrossPnt + ptSrchRsltPnt->nNumTPntLower + ptSrchRsltPnt->nNumTPntUpper + ptSrchRsltPnt->nNumTPntLeft + ptSrchRsltPnt->nNumTPntRight );

	/* 追加パターンの探索領域設定 */ /* 将来的に追加パターンは左右個別に判定して作成する */
	/* パターンのTypeを取得 */
	nPtnType_Left = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_LEFT].tPtnInfo.nTypePattern;
	nPtnType_Right = m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[CB_ADD_PATTERN_POS_RIGHT].tPtnInfo.nTypePattern;

	/* 追加パターンの判別 */
	if ( ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_NONE == nPtnType_Right ) )
	{
		/* 左：なし　　右：なし　　：*/
		/* NO OPERATION */
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CIRCLE_LINE == nPtnType_Right ) )
	{
		/* 左：線丸　　右：線丸　　：*/

		/* 追加パターン(Left) */
		ptCenterPos->LeftNum = (ulong)( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_LEFT].nNumCenterPnt );
		/* (Left)1点目/2点目 */
		( ptCenterPos->Left )[0] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_LEFT].tCenterPntPrecise )[0];
		( ptCenterPos->Left )[1] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_LEFT].tCenterPntPrecise )[1];
		/* 追加パターン(Right) */
		ptCenterPos->RightNum = (ulong)( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_RIGHT].nNumCenterPnt );
		/* (Right)1点目/2点目 */
		( ptCenterPos->Right )[0] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_RIGHT].tCenterPntPrecise )[0];
		( ptCenterPos->Right )[1] = ( ptSrchRsltAddPnt[CB_ADD_PATTERN_POS_RIGHT].tCenterPntPrecise )[1];
	}
	else if ( ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Left ) && ( CB_IMG_PATTERN_TYPE_CHKBOARD2x2 == nPtnType_Right ) )
	{
		/* 左：市松(2x2) 右：市松(2x2) ：*/

		/* 追加パターン(Left) */
		ptCenterPos->LeftNum = (ulong)(	  ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].nNumTPntUpper
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].nNumTPntLower
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].nNumCrossPnt);
		/* (Left)1点目/2点目/3点目(中央高精度点) */
		( ptCenterPos->Left )[0] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tTPntUpperPrecise )[0];
		( ptCenterPos->Left )[1] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tTPntLowerPrecise )[0];
		( ptCenterPos->Left )[2] = ( ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_LEFT].tCrossPntPrecise )[0];

		/* 追加パターン(Right) */
		ptCenterPos->RightNum = (ulong)(  ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].nNumTPntUpper
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].nNumTPntLower
										+ ptSrchRsltAddChkPnt[CB_ADD_PATTERN_POS_RIGHT].nNumCrossPnt);
		/* (Right)1点目/2点目/3点目(中央高精度点) */
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
		/* 左右のパターンが異なる場合 */
		/* 現状ではありえないパターンなので、現状はエラーとする。将来的には対応する。  */
		return ( CB_IMG_NG_COMBI_ADD_PATTERN );
	}

	/* 特徴点間の位置関係チェック */
	nRet = cb_FPE_CheckFtrPointPos( ptCenterPos, ptSrchRgn, nCamDirection );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			特徴点間の位置関係チェック
 *
 * @param[in,out]	ptCenterPos			:,特徴点抽出結果構造体へのポインタ,-,[-],
 * @param[in]		ptSrchRgn			:,探索領域情報構造体へのポインタ,-,[-],
 * @param[in]		nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.08.20			K.Kato			新規作成
 *
 * @note			引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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

	/* 引数チェック */
	if ( ( NULL == ptCenterPos ) || ( NULL == ptSrchRgn ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 探索領域情報へのポインタ */
	ptSrchRgnChkboard = &( (ptSrchRgn->tSrchRgnInfo4Pattern)[0].tRgnImg );

	/* 特徴点情報へのポインタ */
	ptFtrPntPos = &( (ptCenterPos->Center)[0] );

	/* 位置関係チェック */
	/* 不定値を考慮してワーク値を設定 */
	for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
	{
		/* 座標値 */
		tWkFtrPntPos[ni] = ptFtrPntPos[ni];

		/* 有効値チェック */
		nFlagChkOK = CB_TRUE;
		if ( ( 0.0 <= ptFtrPntPos[ni].w ) && ( 0.0 <= ptFtrPntPos[ni].h ) )
		{
			/* 探索領域内外チェック */
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

		/* 上記チェックでError扱いでなかった場合 */
		if ( CB_TRUE == nFlagChkOK )
		{
			nFtrPntValidTbl[ni] = CB_TRUE;
		}
		else
		{
			nFtrPntValidTbl[ni] = CB_FALSE;
		}
	}	/* for ( ni ) */

	/* Index設定 */
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

			/* アルゴリズムタイプ */
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

	/* 特徴点[2]が無効値の場合は、位置関係チェックを実施しない */
	nIdxFtrPnt2 = nIdxFtrPnt[1];
	if ( CB_TRUE == nFtrPntValidTbl[nIdxFtrPnt2] )
	{
		nIdx = 0L;

		/* 全カメラに対して有効な特徴点 */
		/* 特徴点[1] */
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* Y方向 */
			if ( tWkFtrPntPos[nIdxFtrPnt2].h <= nAttnY )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* 特徴点[2] => 他のチェック内に含まれてくるため、明示的にはチェックしない */
		nIdx++;

		/* 特徴点[3] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* Y方向 */
			if ( nAttnY <= tWkFtrPntPos[nIdxFtrPnt2].h )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* 特徴点[4] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y方向 */
				if ( tWkFtrPntPos[nIdxFtrPnt2].h <= nAttnY )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}

		/* 特徴点[5] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* X方向 */
			if ( tWkFtrPntPos[nIdxFtrPnt2].w <= nAttnX )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* 特徴点[6] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y方向 */
				if ( nAttnY <= tWkFtrPntPos[nIdxFtrPnt2].h )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}

		/* 特徴点[7] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y方向 */
				if ( tWkFtrPntPos[nIdxFtrPnt2].h <= nAttnY )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}

		/* 特徴点[8] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
		{
			nAttnX = tWkFtrPntPos[nIdxChk].w;
			nAttnY = tWkFtrPntPos[nIdxChk].h;
			/* X方向 */
			if ( nAttnX <= tWkFtrPntPos[nIdxFtrPnt2].w )
			{
				nFtrPntValidTbl[nIdxChk] = CB_FALSE;
			}
		}

		/* 特徴点[9] */
		nIdx++;
		nIdxChk = nIdxFtrPnt[nIdx];
		if ( CB_FPE_CHK_FTR_PNT_SC_INVALID != nIdxChk )
		{
			if ( CB_TRUE == nFtrPntValidTbl[nIdxChk] )
			{
				nAttnX = tWkFtrPntPos[nIdxChk].w;
				nAttnY = tWkFtrPntPos[nIdxChk].h;
				/* Y方向 */
				if ( nAttnY <= tWkFtrPntPos[nIdxFtrPnt2].h )
				{
					nFtrPntValidTbl[nIdxChk] = CB_FALSE;
				}
			}
		}
	}
	else
	{
		/* 全特徴点を無効扱いとする */
		for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
		{
			nFtrPntValidTbl[ni] = CB_FALSE;
		}	/* for ( ni ) */
	}

	/* チェック結果を格納 */
	for ( ni = 0L; ni < CB_FPE_CENTER_PNT_MAXNUM; ni++ )
	{
		/* 有効値チェック */
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
 * @brief		テストデータ作成
 *
 * @param[in]	nImgID				:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	ptSrchRgnInfo		:,探索領域情報構造体へのポインタ,-,[-],
 * @param[in]	nCamDirection		:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK			:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)		:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.07	K.Kato			新規作成
 *
 * @note		引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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

	/* 引数チェック */
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

			/* ブロック(0,0) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(0,2) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 2 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(1,1) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 1 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(1,3) */
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

			/* ブロック(0,0) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(1,1) */
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

			/* ブロック(0,1) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 1 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(0,3) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn + 3 * nXSizeBlock;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(1,0) */
			nSxWin = ptSrchRgnZoomOut->nSX + nXSizePeriRgn;
			nSyWin = ptSrchRgnZoomOut->nSY + nYSizePeriRgn + 1 * nYSizeBlock;
			nExWin = nSxWin + nXSizeBlock - 1;
			nEyWin = nSyWin + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWin, nSyWin, nExWin, nEyWin );
			nRet = implib_IP_Const( nImgID, 0 );

			/* ブロック(1,2) */
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

	/* 追加パターン(Left) */
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

			/* 上の丸 */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 下の丸 */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 長い線 */
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

			/* 上の丸 */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 下の丸 */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 長い線 */
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

	/* 追加パターン(Right) */
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

			/* 上の丸 */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 下の丸 */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 長い線 */
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

			/* 上の丸 */
			nSxWinUp = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nSyWinUp = ptSrchRgnZoomOut->nSY + nYSizePeriRgn;
			nExWinUp = nSxWinUp + nXSizeBlock - 1;
			nEyWinUp = nSyWinUp + nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinUp, nSyWinUp, nExWinUp, nEyWinUp );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 下の丸 */
			nSxWinLo = ptSrchRgnZoomOut->nSX + ( nXSizeSrchRgn / 2 ) - 7;
			nEyWinLo = ptSrchRgnZoomOut->nEY - nYSizePeriRgn;
			nExWinLo = nSxWinLo + nXSizeBlock - 1;
			nSyWinLo = nEyWinLo - nYSizeBlock - 1;
			nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxWinLo, nSyWinLo, nExWinLo, nEyWinLo );
			nRet = implib_IP_Const( nImgID, 0 );

			/* 長い線 */
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

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

#endif /* CB_FPE_CSW_ENABLE_TEST_MODE */


#ifdef CB_FPE_CSW_JUDGE_BR_SIMILARITY											/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↓ */
/******************************************************************************/
/**
 * @brief		追加パターン_輝度の類似性を用いた採用判定
 *
 * @param[in]	
 * @param[out]	
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		-1				:,異常終了,value=-1,[-],
 *
 * @date		2013.08.02	S.Suzuki			新規作成
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
	float_t					fDenom	=0.0F,			/* 正規化相互相関　分子 */
							fNumerUpper	=0.0F,		/* 正規化相互相関　分母(上端側白領域の標準偏差) */
							fNumerLower	=0.0F;		/* 正規化相互相関　分母(下端側白領域の標準偏差) */
	float_t					fNumer = 0.0F;			/* 正規化相互相関　分母 */
	float_t					fContrastUpper = 0.0F, fContrastLower = 0.0F;

	/* 引数チェック */
	if ( ( NULL == pnImgAddr ) || ( NULL == pnRRFAddr ) || ( NULL == ptBrInput ) || ( NULL == pfSimilarity ) || ( NULL == pfContrast ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 0クリア */
	memset( &tBrInfoTbl, 0x00, sizeof( CB_FPE_CBR_BR_INFO ) );

	/* ヒストグラムのビン数を格納 */
	nHistBinNum = CB_FPE_PRM_HIST_BIN_NUM;

	/*
	============================================
		①正規化ヒストグラム＆輝度平均を計算
	============================================
	*/
	nRet = cb_FPE_CalcBrightnessHistogram( pnImgAddr, pnRRFAddr, &tBrInfoTbl, ptBrInput );

	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();

		return ( CB_IMG_NG_CALC_BR_SIMILARITY );
	}

	/*
	============================================
		②ヒストグラムの類似度を計算
	============================================
	*/
	/* 最頻値計算用初期化 */
	nUpperModeID = 0L;
	nLowerModeID = 0L;
	fUpperMode = 0.0F;
	fLowerMode = 0.0F;

	/* ヒストグラムのBIN数だけループ */
	for( ni = 0L; ni < nHistBinNum; ni++)
	{
		/* ヒストグラムの最大値を探し、最頻値としてIDと値を登録 */
		/* 上側白領域の最頻値探索 */
		if( fUpperMode < tBrInfoTbl.fBrTblUpper[ ni ] )
		{
			nUpperModeID = ni;
			fUpperMode = tBrInfoTbl.fBrTblUpper[ ni ];
		}

		/* 下側白領域の最頻値探索 */
		if( fLowerMode < tBrInfoTbl.fBrTblLower[ ni ] )
		{
			nLowerModeID = ni;
			fLowerMode = tBrInfoTbl.fBrTblLower[ ni ];
		}
	}
	/* 最頻値の差分を求める */
	nDiffBrMode	= nUpperModeID - nLowerModeID;

	/* 最頻値の差分をもとに、ヒストグラムのピークを合わせ、正規化相互相関を計算 */
	for( ni = 0L; ni < nHistBinNum; ni++)
	{
		/* ヒストグラムのピークを合わせることで、配列外となり対応付けができなくなる箇所は扱わない */
		if(		( ( ni - nDiffBrMode ) < 0L )
			||	( ( ni - nDiffBrMode ) >= nHistBinNum ) )
		{
			/* Not Operation */
		}
		else
		{
			/* 正規化相互相関を計算するための値を格納していく */
			fDenom		+= tBrInfoTbl.fBrTblUpper[ ni ] * tBrInfoTbl.fBrTblLower[ ni - nDiffBrMode ];
			fNumerUpper	+= tBrInfoTbl.fBrTblUpper[ ni ] * tBrInfoTbl.fBrTblUpper[ ni ];
			fNumerLower	+= tBrInfoTbl.fBrTblLower[ ni - nDiffBrMode ] * tBrInfoTbl.fBrTblLower[ ni - nDiffBrMode ];

		}
	}
	/* 分母を計算 */
	fNumer = sqrtf(fNumerUpper) * sqrtf(fNumerLower);

	/* 正規化相互相関計算時の0割チェック */
	if( CB_FPE_FLT_MIN > fNumer )	/* 0割チェック */
	{
		/* 類似度(相関値)を0として扱う */
		(*pfSimilarity) = 0.0F;
	}
	else
	{
		/* 類似度を計算 */
		(*pfSimilarity) = fDenom / fNumer;
	}

	/*
	============================================
		③コントラストを計算
	============================================
	*/
	/* 上下端の黒領域と白領域の輝度平均の差(≒コントラスト)を計算 */
	/* 平均値でコントラスト計算 */
	fContrastUpper = tBrInfoTbl.fAvgUpper - ptBrInput->fAvgBlackArea;
	fContrastLower = tBrInfoTbl.fAvgLower - ptBrInput->fAvgBlackArea;

	/* 小さい方の値を格納しリターン */
	if( fContrastUpper < fContrastLower )
	{
		(*pfContrast) = fContrastUpper;
	}
	else
	{
		(*pfContrast) = fContrastLower;
	}

	/* コントラストが負の値の時、0を格納 */
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
 * @brief		追加パターン_ヒストグラムを算出
 *
 * @param[in]	
 * @param[out]	
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		-1				:,異常終了,value=-1,[-],
 *
 * @date		2013.08.02	S.Suzuki			新規作成
 */
/******************************************************************************/
static slong cb_FPE_CalcBrightnessHistogram( const uchar* const pnImgAddr, const uchar* const pnRRFAddr, CB_FPE_CBR_BR_INFO* ptBrInfoTbl, CB_FPE_CBR_INPUT* ptBrInput )
{
	slong		nPixSrchX, nPixSrchY;                           /* 画素探索_ループ用変数                 */
	slong		nSrchMax       = 0L;                            /* 画素探索_高さループ用最大値           */
	slong		nPixSrchHeight = 0L;                            /* 画素探索_高さ                         */
	slong		nPos           = 0L;                            /* 対象探索点                            */
	slong		nPosX          = 0L;                            /* 探索時の基準となるX座標               */
	slong       nPosY          = 0L;                            /* 探索時の基準となるY座標               */
	slong		nSumBrightness = 0L;                            /* 輝度累計(平均計算用)                  */
	slong		nCntPixel      = 0L;                            /* ピクセル数                            */
	slong		nSrchPnt       = 0L;                            /* 探索点                                */
	slong		nUpperFlg      = 0L;                            /* 黒領域食込み補正完了フラグ(上端)      */
	slong		nLowerFlg      = 0L;                            /* 黒領域食込み補正完了フラグ(下端)      */
	slong		nIdxUpperRRF   = 0L;
	slong		nIdxLowerRRF   = 0L;
	slong		nPosUpperX     = 0L;                            /* 黒→白領域変化点座標(上端X座標)       */
	slong		nPosUpperY     = 0L;                            /* 黒→白領域変化点座標(上端Y座標)       */
	slong		nPosLowerX     = 0L;                            /* 黒→白領域変化点座標(下端X座標)       */
	slong		nPosLowerY     = 0L;                            /* 黒→白領域変化点座標(下端Y座標)       */
	slong		nUpperY        = 0L;                            /* 黒領域食込み補正後画素探索(上端Y座標) */
	slong		nLowerY        = 0L;                            /* 黒領域食込み補正後画素探索(下端Y座標) */
	slong		nUpper         = CB_ADD_PATTERN_PNT_POS_UPPER;  /* 対象領域(線丸上端側)                  */
	slong		nLower         = CB_ADD_PATTERN_PNT_POS_LOWER;  /* 対象領域(線丸下端側)                  */
	float_t		fCoefBinWidth;

	CB_FPE_CBR_POINT_INFO*	ptBrPointTbl = NULL;

	/* 引数チェック */
	if ( ( NULL == pnImgAddr ) || ( NULL == pnRRFAddr ) || (NULL == ptBrInfoTbl) || ( NULL == ptBrInput ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
	ptBrPointTbl = &(ptBrInput->tBrPointTbl[0]);
	/* ヒストグラム格納時にどのビンに該当するかを計算するための係数 */
	fCoefBinWidth = 1.0F / (float_t)( CB_FPE_PRM_HIST_BIN_WIDTH );

	/*
	============================================
		線丸上端側
	============================================
	*/
	/* 画素探索領域高さ算出 */
	nPixSrchHeight = ( (slong)( ptBrPointTbl[nUpper].tLabelLowerPnt.fY + 0.5F ) - (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) ) + 1L;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6876

	/* 初期化 */
	nUpperY = 0L;
	nLowerY = nPixSrchHeight;

	/* 画素探索高さ設定 */
	nSrchMax = nPixSrchHeight / 2L;     /* 白領域の上端/下端双方から探索するため探索高さを半分とする */

	/* 黒領域食込み補正 */
	for( nPixSrchY = 0L; nPixSrchY < nSrchMax; nPixSrchY++ )
	{
		if( nUpperFlg != 1L )
		{
			/* 上端ラベル黒白境界座標設定 */
			if( ptBrPointTbl[nUpper].nFlagVertical == 1L )  /* 探索ラインが垂直の場合 */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + 0.5F );					// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6877
				nPosUpperY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6878
			}
			else    /* 探索ラインが垂直でない場合 */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nUpper].fSrchSlope ) + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6879
				nPosUpperY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6880
			}

			/* 探索対象画素インデックス設定 */
			nIdxUpperRRF = ( nPosUpperY * ptBrInput->nImgRRFXSize ) + nPosUpperX;

			/* 丸候補→白領域の変化点Y座標設定(丸候補下端位置補正) */
			if( ( nUpperFlg == 0L ) && ( (slong)pnRRFAddr[nIdxUpperRRF] == 0L ) )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6881
			{
				/* 黒→白領域に変化した位置に更新 */
				nUpperY = nPixSrchY;
				/* 補正済みフラグをON */
				nUpperFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
		if( nLowerFlg != 1L )
		{
			/* 下端ラベル黒白境界座標設定 */
			if( ptBrPointTbl[nUpper].nFlagVertical == 1L )  /* 探索ラインが垂直の場合 */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6882
				nPosLowerY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6883
			}
			else    /* 探索ラインが垂直でない場合 */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + ( (float_t)( nPixSrchHeight - nPixSrchY ) / ptBrPointTbl[nUpper].fSrchSlope ) + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6884
				nPosLowerY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6885
			}

			/* 探索対象画素インデックス設定 */
			nIdxLowerRRF = ( nPosLowerY * ptBrInput->nImgRRFXSize ) + nPosLowerX;

			/* 線候補→白領域の変化点Y座標設定(線候補上端位置補正) */
			if( ( nLowerFlg == 0L ) && ( (slong)pnRRFAddr[nIdxLowerRRF] == 0L ) )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6886
			{
				/* 黒→白領域に変化した位置に更新 */
				nLowerY = ( nPixSrchHeight - nPixSrchY );
				/* 補正済みフラグをON */
				nLowerFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
	}

	/* 探索領域内総ピクセル数算出 */
	/* nLowerY < nUpperY になることはないので、0割は発生しない */
	nCntPixel = ptBrPointTbl[nUpper].nSrchWidth * ( ( nLowerY - nUpperY ) + 1L );

	/* 探索ピクセル数が不正値の場合、エラー処理 */
	if( nCntPixel <= 0L )
	{
		return ( CB_IMG_NG_CALC_BR_SIMILARITY );
	}

	/* 輝度累計計算 */
	for( nPixSrchY = nUpperY; nPixSrchY <= nLowerY; nPixSrchY++ )
	{
		/* 探索基準点座標計算 */
		if( ptBrPointTbl[nUpper].nFlagVertical == 1L )  /* 探索ラインが垂直の場合 */
		{
			nPosX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6889
			nPosY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6890
		}
		else    /* 探索ラインが垂直でない場合 */
		{
			nPosX = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nUpper].fSrchSlope ) + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6891
			nPosY = (slong)( ptBrPointTbl[nUpper].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6892
		}

		for( nPixSrchX = 0L; nPixSrchX < ptBrPointTbl[nUpper].nSrchWidth; nPixSrchX++ )
		{
			/* 探索点のアドレスを算出 */
			nPos = ( nPosY * ptBrInput->nImgSrcXSize ) + ( ( nPosX - ( ptBrPointTbl[nUpper].nSrchWidth / 2L ) ) + nPixSrchX );		// [EntryAVM_QAC#4]ID-6877 演算順序指摘対応
			nSrchPnt = (slong)( (float_t)( pnImgAddr[nPos] ) * fCoefBinWidth );		//切り捨て処理

			/* 正規化してヒストグラムテーブルに格納 */
			ptBrInfoTbl->fBrTblUpper[nSrchPnt] += ( 1.0F / (float_t)nCntPixel ) ;

			/* 輝度値を累計 */
			nSumBrightness += (slong)( pnImgAddr[nPos] );
		}
	}

	/* 平均値を格納 */
	ptBrInfoTbl->fAvgUpper = (float_t)( nSumBrightness ) / (float_t)( nCntPixel );

	/*
	============================================
		線丸下端側
	============================================
	*/
	/* 画素探索領域高さ算出 */
	nPixSrchHeight = ( (slong)( ptBrPointTbl[nLower].tLabelLowerPnt.fY + 0.5F ) - (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) ) + 1L;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6897

	/* 初期化 */
	nSumBrightness = 0L;
	nUpperFlg = 0L;
	nLowerFlg = 0L;
	nUpperY = 0L;
	nLowerY = nPixSrchHeight;

	/* 画素探索高さ設定 */
	nSrchMax = nPixSrchHeight / 2L;     /* 白領域の上端/下端双方から探索するため探索高さを半分とする */

	/* 黒領域食込み補正 */
	for( nPixSrchY = 0L; nPixSrchY < nSrchMax; nPixSrchY++ )
	{
		if( nUpperFlg != 1L )
		{
			/* 上端ラベル黒白境界座標設定 */
			if( ptBrPointTbl[nLower].nFlagVertical == 1L )  /* 探索ラインが垂直の場合 */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6898
				nPosUpperY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6899
			}
			else    /* 探索ラインが垂直でない場合 */
			{
				nPosUpperX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nLower].fSrchSlope ) + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6900
				nPosUpperY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6901
			}

			/* 探索対象画素インデックス設定 */
			nIdxUpperRRF = ( nPosUpperY * ptBrInput->nImgRRFXSize ) + nPosUpperX;

			/* 線候補→白領域の変化点Y座標設定(線候補下端位置補正) */
			if( ( nUpperFlg == 0L ) && ( (slong)pnRRFAddr[nIdxUpperRRF] == 0L ) )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6902
			{
				/* 黒→白領域に変化した位置に更新 */
				nUpperY = nPixSrchY;
				/* 補正済みフラグをON */
				nUpperFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
		if( nLowerFlg != 1L )
		{
			/* 下端ラベル黒白境界座標設定 */
			if( ptBrPointTbl[nLower].nFlagVertical == 1L )  /* 探索ラインが垂直の場合 */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6903
				nPosLowerY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6904
			}
			else    /* 探索ラインが垂直でない場合 */
			{
				nPosLowerX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + ( (float_t)( nPixSrchHeight - nPixSrchY ) / ptBrPointTbl[nLower].fSrchSlope ) + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6905
				nPosLowerY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + ( nPixSrchHeight - nPixSrchY );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6906
			}

			/* 探索対象画素インデックス設定 */
			nIdxLowerRRF = ( nPosLowerY * ptBrInput->nImgRRFXSize ) + nPosLowerX;

			/* 丸候補→白領域の変化点Y座標設定(丸候補上端位置補正) */
			if( ( nLowerFlg == 0L ) && ( (slong)pnRRFAddr[nIdxLowerRRF] == 0L ) )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6907
			{
				/* 黒→白領域に変化した位置に更新 */
				nLowerY = ( nPixSrchHeight - nPixSrchY );
				/* 補正済みフラグをON */
				nLowerFlg = 1L;
			}
			else
			{
				/* Not Operation */
			}
		}
	}

	/* 探索領域内総ピクセル数算出 */
	/* nLowerY < nUpperY になることはないので、0割は発生しない */
	nCntPixel = ptBrPointTbl[nLower].nSrchWidth * ( ( nLowerY - nUpperY ) + 1L );

	/* 探索ピクセル数が不正値の場合、エラー処理 */
	if( nCntPixel <= 0L )
	{
		return ( CB_IMG_NG_CALC_BR_SIMILARITY );
	}

	/* 輝度累計計算 */
	for( nPixSrchY = nUpperY; nPixSrchY <= nLowerY; nPixSrchY++ )
	{
		/* 探索基準点座標計算 */
		if( ptBrPointTbl[nLower].nFlagVertical == 1L )  /* 探索ラインが垂直の場合 */
		{
			nPosX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6910
			nPosY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6911
		}
		else    /* 探索ラインが垂直でない場合 */
		{
			nPosX = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fX + ( (float_t)nPixSrchY / ptBrPointTbl[nLower].fSrchSlope ) + 0.5F );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6912
			nPosY = (slong)( ptBrPointTbl[nLower].tLabelUpperPnt.fY + 0.5F ) + nPixSrchY;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6913
		}

		for( nPixSrchX = 0L; nPixSrchX < ptBrPointTbl[nLower].nSrchWidth; nPixSrchX++ )
		{
			/* 探索点のアドレスを算出 */
			nPos = ( nPosY * ptBrInput->nImgSrcXSize ) + ( ( nPosX - ( ptBrPointTbl[nLower].nSrchWidth / 2L ) ) + nPixSrchX );
			nSrchPnt = (slong)( (float_t)( pnImgAddr[nPos] ) * fCoefBinWidth );		//切り捨て処理
			/* 正規化してヒストグラムテーブルに格納 */
			ptBrInfoTbl->fBrTblLower[nSrchPnt] += ( 1.0F / (float_t)nCntPixel ) ;

			/* 輝度の累計を計算 */
			nSumBrightness += (slong)( pnImgAddr[nPos] );
		}
	}

	/* 平均値を格納 */
	ptBrInfoTbl->fAvgLower = (float_t)( nSumBrightness ) / (float_t)( nCntPixel );

	return CB_IMG_OK;
}
#endif /* CB_FPE_CSW_JUDGE_BR_SIMILARITY */										/* 特徴点抽出誤検知対策(輝度判定) <CHG> 2013.08.08 ↑ */


/********************************************************************
 * 外部関数(デバッグ)　定義
 ********************************************************************/
#ifdef CB_FPE_CSW_DEBUG_ON

/******************************************************************************/
/**
 * @brief		[デバッグ]処理結果の描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptCenterPos		:,特徴点構造体へのポインタ,-,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.18	K.Kato			新規作成
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

	/* 引数チェック */
	if ( NULL == ptCenterPos )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報取得 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 描画設定 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* グリッド線 */
//	cb_FPE_DBG_DrawGridLine( nImgDst, nCamPos );
	/* 探索領域 */
//	cb_FPE_DBG_DrawSearchRegionInfo( nImgDst, nCamPos, 0 );
	/* 特徴点位置 */
	cb_FPE_DBG_DrawFtrPoint( nImgDst, ptCenterPos, nCamPos );

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]グリッド線の描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.10	K.Kato			新規作成
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報取得 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 描画設定 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* 先頭ポインタ */
	ptPntImg = &( (m_FPE_tDbgInfo.tGridInfo[nCamPos].tPntImg)[0][0] );

	/* 世界座標系のX軸に垂直な線 */
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

	/* 世界座標系のX軸に水平な線 */
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

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]探索領域描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in]		nZoomMode		:,拡大/縮小モード,0<=value<=1,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.10	K.Kato			新規作成
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報取得 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 描画設定 */
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


	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]追加パターンにおける探索領域描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in]		nZoomMode		:,拡大/縮小モード,0<=value<=1,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.01	K.Kato			新規作成
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

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
	CB_FPE_SRCH_RGN_INFO_EACH_ADD_PATTERN	*ptPntImg;			/* 追加パターンに対する探索領域構造体へのポインタ */
	slong	nSxTopRect, nSyTopRect, nExTopRect, nEyTopRect;		/* 描画矩形始終点座標                             */
	slong	nl = 0L;											/* 黒丸中心点(カメラ誤差考慮)描画用ループカウンタ */
	slong	nPos;												/* 黒丸中心位置用ループカウンタ                   */
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */


	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報取得 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 描画設定 */
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

#ifdef CB_FPE_CSW_CIRCLE_SRCH_RGN								/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↓ */
		if( 0L == nZoomMode )
		{
			ptPntImg = &( m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfoAddPattern[ni] );
		}

		/*--------------------------------*/
		/* 黒丸探索領域矩形描画           */
		/*--------------------------------*/

		/* 描画設定 */
		nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

		/* 黒丸数分ループ */
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
		/* 黒丸中心点(カメラ誤差考慮)描画 */
		/*--------------------------------*/

		/* 描画設定 */
		nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_BLACK );

		for( nl = 0; nl < CB_FPE_ERR_TBL_MAXNUM; nl++ )
		{
			/* 黒丸数分ループ */
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
#endif /* CB_FPE_CSW_CIRCLE_SRCH_RGN */							/* 特徴点抽出誤検知対策(探索領域限定) <CHG> 2013.08.01 ↑ */
	}


	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]Haar-like特徴量の描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.17	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_xMagZoomOut, nprm_yMagZoomOut;
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* パラメータ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_CHK_EDGE_BLOCK_YSIZE;

	/* デバッグ画像をクリア */
//	nRet = implib_IP_ClearImg( nImgDst );

	/* Haar-Like情報を描画 */
	/* 先頭ポインタ */
	ptHaarLike = &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_VERT_EDGE]);
//	ptHaarLike = &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]);
//	ptHaarLike = &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_HORI_EDGE]);
	/* 最大値 */
	nMaxValue = 2 * ( ( nprm_xsizeBlockW * nprm_ysizeBlockW ) * 255 );

	/* 探索領域 */
	ptSrchRgn = &(m_FPE_tInfo.tCtrl.tSrchRgnInfo4Cam[nCamPos].tSrchRgnInfo4Pattern[0].tRgnImgZoomOut);

	/* 画像メモリアクセス開始 */
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

			/* UVの範囲変換 */
			nU_YUV += 128;
			nV_YUV += 128;

			/* 範囲チェック */
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

			/* 値格納 */
//			*pnWkPixelImgY = (uchar)nY_YUV;
//			*( pnWkPixelImgY + 1 ) = (uchar)nY_YUV;
			
			if ( 0 == ( nj & 0x00000001 ) )	/* 偶数 */
			{
				*pnWkPixelImgUV = (uchar)nU_YUV;
				*(pnWkPixelImgUV + 1 ) = (uchar)nV_YUV;
			}
			else	/* 奇数 */
			{
				*(pnWkPixelImgUV - 1) = (uchar)nU_YUV;
				*pnWkPixelImgUV = (uchar)nV_YUV;
			}

			pnWkFtr += 2;
			pnWkPixelImgY += 2;
			pnWkPixelImgUV += 2;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nImgDst );
	nRet = implib_CloseImgDirect( implib_GetUVImgID( nImgDst ) );
	
	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]探索結果(候補)点描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.17	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_xMagZoomOut, nprm_yMagZoomOut;

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* パラメータ */
	nprm_xMagZoomOut = CB_FPE_PRM_ZOOMOUT_XMAG;
	nprm_yMagZoomOut = CB_FPE_PRM_ZOOMOUT_YMAG;

	/* 画像情報 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* デバッグ画像をクリア */
//	nRet = implib_IP_ClearImg( nImgDst );

	/* 探索結果(候補)を描画 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* 描画設定 */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* 先頭ポインタ */
	ptSrchRsltPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltPntInfo[0]);
	ptSrchRsltAddPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddPntInfo[0]);

#if 0	/* 色分け */
	for ( ni = 0; ni < CB_PATTERN_POS_MAXNUM; ni++ )
	{
		/* 十字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumCrossPnt; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tCrossPntHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tCrossPntHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		}

		/* 下側Ｔ字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntLower; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntLowerHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntLowerHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN].nU );
		}

		/* 上側Ｔ字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntUpper; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntUpperHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntUpperHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* 右側側Ｔ字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntRight; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntRightHalf[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntRightHalf[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE].nU );
		}

		/* 左側側Ｔ字点 */
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
		/* 十字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumCrossPnt; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tCrossPnt[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tCrossPnt[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* 下側Ｔ字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntLower; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntLower[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntLower[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* 上側Ｔ字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntUpper; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntUpper[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntUpper[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* 右側側Ｔ字点 */
		for ( nj = 0; nj < ptSrchRsltPntInfo->nNumTPntRight; nj++ )
		{
			nXpoint = ptSrchRsltPntInfo[ni].tTPntRight[nj].nX;
			nYpoint = ptSrchRsltPntInfo[ni].tTPntRight[nj].nY;

			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}

		/* 左側側Ｔ字点 */
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

	/* 追加パターン */
	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		for ( nj = 0; nj < ptSrchRsltAddPntInfo[ni].nNumCenterPnt; nj++ )
		{
			/* 通常 */
			nXpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nX;
			nYpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		}
	}
#endif

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]特徴点位置描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptCenterPos		:,特徴点構造体へのポインタ,-,[-], 
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.18	K.Kato			新規作成
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

	/* 引数チェック */
	if ( NULL == ptCenterPos )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 探索結果(候補)を描画 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* 描画設定 */
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

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]黒丸中心位置描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.01	K.Kato			新規作成
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 探索結果(候補)を描画 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* 描画設定 */
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

	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]高精度位置決め情報描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.22	K.Kato			新規作成
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 探索結果(候補)を描画 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( tImgTbl.xlng - 1 ), ( tImgTbl.ylng - 1 ) );

	/* 描画設定 */
	nRet = implib_SetDrawMode( nImgDst, IMPLIB_DRAW_DIRECT, IMPLIB_COLOR_WHITE );

	/* 先頭ポインタ */
	ptSrchRsltPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltPntInfo[0]);
	ptSrchRsltAddPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddPntInfo[0]);
	ptSrchRsltAddChkPntInfo = &(m_FPE_tInfo.tRslt.tSrchRsltInfo[nCamPos].tSrchRsltAddChkPntInfo[0]);

	/* 市松パターン上の点 */
	/* 十字 */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumCrossPnt; ni++ )
	{
		/* 通常 */
		nXpoint = ptSrchRsltPntInfo->tCrossPnt[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tCrossPnt[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* 高精度 */
		nXpoint = (slong)( ptSrchRsltPntInfo->tCrossPntPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tCrossPntPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* Ｔ字(上) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntUpper; ni++ )
	{
		/* 通常 */
		nXpoint = ptSrchRsltPntInfo->tTPntUpper[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntUpper[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* 高精度 */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntUpperPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntUpperPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* Ｔ字(下) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntLower; ni++ )
	{
		/* 通常 */
		nXpoint = ptSrchRsltPntInfo->tTPntLower[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntLower[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* 高精度 */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntLowerPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntLowerPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* Ｔ字(左) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntLeft; ni++ )
	{
		/* 通常 */
		nXpoint = ptSrchRsltPntInfo->tTPntLeft[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntLeft[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* 高精度 */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntLeftPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntLeftPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* Ｔ字(右) */
	for ( ni = 0; ni < ptSrchRsltPntInfo->nNumTPntRight; ni++ )
	{
		/* 通常 */
		nXpoint = ptSrchRsltPntInfo->tTPntRight[ni].nX;
		nYpoint = ptSrchRsltPntInfo->tTPntRight[ni].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* 高精度 */
		nXpoint = (slong)( ptSrchRsltPntInfo->tTPntRightPrecise[ni].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltPntInfo->tTPntRightPrecise[ni].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
	}

	/* 追加パターン */
	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		for ( nj = 0; nj < ptSrchRsltAddPntInfo[ni].nNumCenterPnt; nj++ )
		{
			/* 通常 */
			nXpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nX;
			nYpoint = ptSrchRsltAddPntInfo[ni].tCenterPnt[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

			/* 高精度 */
			nXpoint = (slong)( ptSrchRsltAddPntInfo[ni].tCenterPntPrecise[nj].w + 0.5 );
			nYpoint = (slong)( ptSrchRsltAddPntInfo[ni].tCenterPntPrecise[nj].h + 0.5 );
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		}
	}


	/* 追加パターン */
	for ( ni = 0; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* 十字点 */
		/* 通常のみ */
		nXpoint = ptSrchRsltAddChkPntInfo[ni].tCrossPnt[0].nX;
		nYpoint = ptSrchRsltAddChkPntInfo[ni].tCrossPnt[0].nY;
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
							, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

		/* 高精度 */
		nXpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tCrossPntPrecise[0].w + 0.5 );
		nYpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tCrossPntPrecise[0].h + 0.5 );
		nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
		nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );

		/* 上T字点 */
		for ( nj = 0; nj < ptSrchRsltAddChkPntInfo[ni].nNumTPntUpper; nj++ )
		{
			/* 通常 */
			nXpoint = ptSrchRsltAddChkPntInfo[ni].tTPntUpper[nj].nX;
			nYpoint = ptSrchRsltAddChkPntInfo[ni].tTPntUpper[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

			/* 高精度 */
			nXpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tTPntUpperPrecise[nj].w + 0.5 );
			nYpoint = (slong)( ptSrchRsltAddChkPntInfo[ni].tTPntUpperPrecise[nj].h + 0.5 );
			if( nXpoint >= 0L ){
				nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
									, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
				nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
									, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
			}
		}

		/* 下T字点 */
		for ( nj = 0; nj < ptSrchRsltAddChkPntInfo[ni].nNumTPntLower; nj++ )
		{
			/* 通常 */
			nXpoint = ptSrchRsltAddChkPntInfo[ni].tTPntLower[nj].nX;
			nYpoint = ptSrchRsltAddChkPntInfo[ni].tTPntLower[nj].nY;
			nRet = IPM_DrawLineColor( nImgDst, ( nXpoint - 5 ), nYpoint, ( nXpoint + 5 ), nYpoint
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
			nRet = IPM_DrawLineColor( nImgDst, nXpoint, ( nYpoint - 5 ), nXpoint, ( nYpoint + 5 )
								, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );

			/* 高精度 */
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


	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]輝度判定スコア&候補描画
 *
 * @param[in,out]	
 * @param[in]		 
 * @param[in]		
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			-1				:,異常終了,value=-1,[-],
 *
 * @date		2013.08.01	S.Suzuki		新規作成
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

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 探索結果(候補)を描画 */
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, 0, 0, ( 720 - 1 ), ( 720 - 1 ) );

	/* 描画設定 */
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
				/* 結果を赤直線で描画 */
				nRet = IPM_DrawLineColor( nImgDst, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY,
													m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY,
													m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED].nU );
				/* 上端の位置を青矩形で描画 */
				nRet = IPM_DrawRectangleColor( nImgDst, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY,
														3, 3,
														m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
				/* 下端の位置を青矩形で描画 */
				nRet = IPM_DrawRectangleColor( nImgDst, m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX, m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY,
														3, 3,
														m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nY, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nV, m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE].nU );
				/* 直線の中心あたりに線IDを描画 */
				nRet = IPEXT_DrawString( (	m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nX + m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nX ) / 2L,
										 (	m_FPE_tDbgInfo.tPntCandidate[ni][nj][0].nY + m_FPE_tDbgInfo.tPntCandidate[ni][nj][1].nY ) / 2L,
										"[%.2d]",nj );
				/* 画面下の左右に、スコア等を描画 */
				nRet = IPEXT_DrawString( 10 + (360*ni), 280 + ( 20*nj ), "[%d]Score:%.3f \t Sim:%.3f \t Cont:%.3f ", nj, m_FPE_tDbgInfo.tScore[ni][nj], m_FPE_tDbgInfo.tSimilarity[ni][nj], m_FPE_tDbgInfo.tContrast[ni][nj] );
			}

		}
	}


	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

#endif /* CB_FPE_CSW_DEBUG_ON */

/********************************************************************
 * 内部関数(デバッグ)　定義
 ********************************************************************/

/******************************************************************************/
/**
 * @brief		[デバッグ]初期化
 *
 * @param		なし
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.14	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_DBG_Init( void )
{

#ifdef CB_FPE_CSW_DEBUG_ON
	CB_COLOR_YUV	*ptColorYUV;
#endif /* CB_FPE_CSW_DEBUG_ON */

	/* 0クリア */
	memset( &m_FPE_tDbgInfo, 0x00, sizeof( CB_FPE_DBG_INFO ) );

#ifdef CB_FPE_CSW_DEBUG_ON
	/* 色 */
	/* 赤 */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_RED]);
	ptColorYUV->nY = CB_COLOR_RED_YUV_Y;
	ptColorYUV->nU = CB_COLOR_RED_YUV_U;
	ptColorYUV->nV = CB_COLOR_RED_YUV_V;
	/* 青 */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_BLUE]);
	ptColorYUV->nY = CB_COLOR_BLUE_YUV_Y;
	ptColorYUV->nU = CB_COLOR_BLUE_YUV_U;
	ptColorYUV->nV = CB_COLOR_BLUE_YUV_V;
	/* 緑 */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_GREEN]);
	ptColorYUV->nY = CB_COLOR_GREEN_YUV_Y;
	ptColorYUV->nU = CB_COLOR_GREEN_YUV_U;
	ptColorYUV->nV = CB_COLOR_GREEN_YUV_V;
	/* ピンク */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_PINK]);
	ptColorYUV->nY = CB_COLOR_PINK_YUV_Y;
	ptColorYUV->nU = CB_COLOR_PINK_YUV_U;
	ptColorYUV->nV = CB_COLOR_PINK_YUV_V;
	/* オレンジ */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_ORANGE]);
	ptColorYUV->nY = CB_COLOR_ORANGE_YUV_Y;
	ptColorYUV->nU = CB_COLOR_ORANGE_YUV_U;
	ptColorYUV->nV = CB_COLOR_ORANGE_YUV_V;
	/* シアン */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_CYAN]);
	ptColorYUV->nY = CB_COLOR_CYAN_YUV_Y;
	ptColorYUV->nU = CB_COLOR_CYAN_YUV_U;
	ptColorYUV->nV = CB_COLOR_CYAN_YUV_V;
	/* 黄 */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_YELLOW]);
	ptColorYUV->nY = CB_COLOR_YELLOW_YUV_Y;
	ptColorYUV->nU = CB_COLOR_YELLOW_YUV_U;
	ptColorYUV->nV = CB_COLOR_YELLOW_YUV_V;
	/* マゼンタ */
	ptColorYUV = &(m_FPE_tDbgInfo.tColorYUV.tYUV[CB_COLOR_MAGENTA]);
	ptColorYUV->nY = CB_COLOR_MAGENTA_YUV_Y;
	ptColorYUV->nU = CB_COLOR_MAGENTA_YUV_U;
	ptColorYUV->nV = CB_COLOR_MAGENTA_YUV_V;
#endif

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]グリッド線の設定
 *
 * @param[out]	ptGridInfo		:,グリッド情報構造体へのポインタ,-,[-],
 * @param[in]	ptCamPrm		:,カメラパラメータ構造体へのポインタ,-,[-],
 * @param[in]	nCamDirection	:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.15	K.Kato			新規作成
 *
 * @note		引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	CB_CG_PNT_WORLD	tTempWldPnt, tTempWldPntVehicle;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R1.1.1  , ID-6702
	CB_CG_PNT_IMG	tTempImgPnt;
	CB_CG_PNT_WORLD	*ptPntWld;
	CB_IMG_POINT	*ptPntImg;

	/* 引数チェック */
	if ( ( NULL == ptGridInfo ) || ( NULL == ptCamPrm ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nNumCam = E_CB_SELFCALIB_CAMPOS_MAX;

	/* 未使用変数回避 ワーニング対策 */
	CALIB_UNUSED_VARIABLE( tTempWldPntVehicle );	/* 変数使用時には削除してください */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R1.1.1  R-30, ID-6704	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.3  R-53, ID-6703

	/* 先頭ポインタ */
	ptPntWld = &(ptGridInfo->tPntWld[0][0]);
	ptPntImg = &(ptGridInfo->tPntImg[0][0]);

	/* グリッド開始位置の設定 */
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

	/* Y方向のループ */
	for ( nj = 0L; nj < CB_FPE_GRID_NUM_WLD_Y; nj++ )
	{
		/* X方向のループ */
		for ( nk = 0L; nk < CB_FPE_GRID_NUM_WLD_X; nk++ )
		{
			dTempValue = dStartPosX + ( (double_t)nk * CB_FPE_GRID_INTERVAL_WLD_X );
			tTempWldPnt.dX = dTempValue;
			dTempValue = dStartPosY + ( (double_t)nj * CB_FPE_GRID_INTERVAL_WLD_Y );
			tTempWldPnt.dY = dTempValue;
			tTempWldPnt.dZ = 0.0;

			/* World To Image */
			nRet = cb_CG_CalcW2C2I( ptCamPrm->nCamID, &tTempWldPnt, &tTempImgPnt );

			/* World格納 */
			*( ptPntWld + ( ( nj * CB_FPE_GRID_NUM_WLD_X ) + nk ) ) = tTempWldPnt;
			/* Image格納 */
			(*( ptPntImg + ( ( nj * CB_FPE_GRID_NUM_WLD_X ) + nk ) )).nX = (slong)( tTempImgPnt.dW + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6705
			(*( ptPntImg + ( ( nj * CB_FPE_GRID_NUM_WLD_X ) + nk ) )).nY = (slong)( tTempImgPnt.dH + 0.5 );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R2.4.1  , ID-6706
		}	/* for ( nk ) */
	}	/* for ( nj ) */

	return ( CB_IMG_OK );
}

#ifdef CB_FPE_CSW_DEBUG_ON

/******************************************************************************/
/**
 * @brief		[デバッグ]Haar-like特徴量のファイル書き出し
 *
 * @param[in]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	pFilePath		:,出力先ファイル名へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.01.17	K.Kato			新規作成
 */
/******************************************************************************/
static slong cb_FPE_DBG_SaveHaarLikeInfo( CB_FPE_HAAR_LIKE_FTR* ptHaarLike, schar *pFilePath )
{
	slong	ni, nj;
	slong	*pnWkFtr;
	FILE	*pFile;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == pFilePath ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ファイルオープン */
	pFile = fopen( pFilePath, "w" );
	if ( NULL == pFile )
	{
		/* ファイルオープン失敗 */
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

	/* ファイルクローズ */
	fclose( pFile );

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		[デバッグ]画素値のファイル書き出し
 *
 * @param[in]	nImgID			:,画面ID,-,[-],
 * @param[in]	pFilePath		:,出力先ファイル名へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.02.25	K.Kato			新規作成
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

	/* 引数チェック */
	if ( NULL == pFilePath )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* ファイルオープン */
	pFile = fopen( pFilePath, "w" );
	if ( NULL == pFile )
	{
		/* ファイルオープン失敗 */
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

	/* ファイルクローズ */
	fclose( pFile );

	nRet = implib_CloseImgDirect( nImgID );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CB_FPE_CSW_DEBUG_ON */


/******************************************************************************/
/**
 * @brief			左右市松パターンにおける特徴点抽出
 *
 * @param[in]		nImgSrc			:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		ptFPEInfo		:,内部情報構造体へのポインタ,-,[-],
 * @param[out]		nCamDirection	:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 * @param[in]		nCalibType		:,キャリブ種別(工場・販社),CB_CALIB_REAR_KIND_FACTORY<=value<=CB_CALIB_REAR_KIND_RUNTIME,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.05	S.Suzuki			新規作成
 * @date			2015.08.26	S.Morita			サイドマーカー画像サイズ修正対応
 *
 * @note			引数nCamDirectionは、上位関数で範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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
	/* パラメータ */
	slong	nprm_thrHaarFtrCrossPnt = 0L;		/* 市松ターゲット内部の特徴点抽出時に、特徴点候補とする特徴量に対するしきい値格納用 */


	/* 引数チェック */
	if ( NULL == ptFPEInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT;

	/* 未使用変数回避 ワーニング対策 */
	CALIB_UNUSED_VARIABLE( nCalibType );	/* 変数使用時には削除してください */		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.3  R-53, ID-6724

	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 先頭ポインタ */
	ptSrchRgnInfo = &(ptFPEInfo->tCtrl.tSrchRgnInfo4Cam[nCamDirection].tSrchRgnInfoAddPattern[0]);
	ptSrchRsltInfo = &(ptFPEInfo->tRslt.tSrchRsltInfo[nCamDirection]);
	ptSrchRsltPntInfo = &(ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]);

	/* クリア */
	nRet = cb_FPE_ClearSearchResultPointInfo( CB_FPE_NELEMS( ptSrchRsltInfo->tSrchRsltAddChkPntInfo ), &( ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]) );
	if ( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* Haar-like特徴量用の画像メモリ割り当て(画像メモリアクセス開始) */
	/* Haar-like画像メモリクリア */
	nRet = implib_IP_Const( ptFPEInfo->nImg[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL], 0L );

	ptHaarLike = &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]);
	nRet = implib_OpenImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL], &( ptHaarLike->nXSize ), &( ptHaarLike->nYSize ), (void**)&( ptHaarLike->pnFtrValue ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}


	/* 左右マーカーでループ */
	for ( ni = CB_ADD_PATTERN_POS_LEFT; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/*
		==============================
			十字点探索
		==============================
		*/
		/* Integral Image作成 */
		nRet = cb_FPE_IntegralImage( &( ptFPEInfo->nImg[0] ),  &( ptSrchRgnInfo[ni].tRgnImgNormalHLsrc ), CB_FPE_PRM_INTIMG_FLG_SIDE );
		if ( CB_IMG_OK != nRet )
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_INTEGRAL_IMG );
		}
#ifdef CALIB_PARAM_P32S_EUR_UK
		if ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
		{
			/* カメラ方向種別がフロントカメラである際、左マーカの特徴点取得には修正市松パターンを用いる */

			/* 重み付きHaar-like(フロントカメラ左市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FR_LEFT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
		else if ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) 
		{
			/* カメラ方向種別がリアカメラである際、左マーカの特徴点取得には修正市松パターンを用いる */

			/* 重み付きHaar-like(リアカメラ左市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_RR_LEFT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
		else if ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
		{
			/* カメラ方向種別がフロントカメラである際、右マーカの特徴点取得には修正市松パターンを用いる */

			/* 重み付きHaar-like(フロントカメラ右市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FR_RIGHT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
		else if ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) 
		{
			/* カメラ方向種別がリアカメラである際、右マーカの特徴点取得には修正市松パターンを用いる */

			/* 重み付きHaar-like(リアカメラ右市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_RR_RIGHT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}
#else	/* CALIB_PARAM_P32S_EUR_UK */
		if ( ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
			|| ( ( CB_ADD_PATTERN_POS_LEFT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) )
		{
			/* カメラ方向種別がフロントカメラ・またはリアカメラである際、左マーカの特徴点取得には修正市松パターンを用いる */

			/* 重み付きHaar-like(フロントカメラ・リアカメラ左市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FRRR_LEFT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ),nCamDirection );
		}
		else if ( ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) ) 
			|| ( ( CB_ADD_PATTERN_POS_RIGHT == ni ) && ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) ) )
		{
			/* カメラ方向種別がフロントカメラ・またはリアカメラである際、右マーカの特徴点取得には修正市松パターンを用いる */

			/* 重み付きHaar-like(フロントカメラ・リアカメラ右市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge_FRRR_RIGHT( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ),nCamDirection );
		}
#endif /* CALIB_PARAM_P32S_EUR_UK */
		else
		{
			/* カメラ方向種別が左カメラまたは右カメラである際、特徴量取得には市松パターンを用いる */

			/* 重み付きHaar-like(市松) */
			nRet = cb_FPE_WeightedHaarLikeChkEdge( ptFPEInfo->nImg[CB_FPE_INTEGRAL_IMG_NORMAL], &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &( ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize ) );
		}

		if ( CB_IMG_OK != nRet )
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_HAAR_LIKE_FTR_CHK_EDGE );
		}

/* [DEBUG] */
#ifdef CB_FPE_CSW_DEBUG_ON
#if 0
	cb_FPE_DBG_SaveHaarLikeInfo( &(m_FPE_tInfo.tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), "C:/SELF_CB_DBG_WEIGHTED_HAAR_LIKE_CHK_EDGE.csv" );
#endif
#endif /* CB_FPE_CSW_DEBUG_ON */

		/* 十字点探索用の閾値設定 */
		if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
		{
			nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT_FRRR;
		}
		else
		{
			nprm_thrHaarFtrCrossPnt = CB_FPE_PRM_THR_HAAR_ADD_FTR_CROSS_PNT;
		}

		/* 十字点探索 */
		
		/* 左右市松用の十字点探索なので、第5引数(探索点数)は 1 で固定 */
		nRet = cb_FPE_SearchCandCrossPnt( &( ptFPEInfo->nImg[0] ), &(ptFPEInfo->tHaarLike[CB_FPE_HAAR_LIKE_CHK_EDGE]), &(ptSrchRsltPntInfo[ni].tCrossPnt[0]), &(ptSrchRsltPntInfo[ni].nNumCrossPnt), 
										&(ptSrchRgnInfo[ni].tPntImgSideChkCenterNormalSize.tAddChkSrchRgn), 1L, nCamDirection, nprm_thrHaarFtrCrossPnt, &(ptSrchRgnInfo[ni].tPtnInfo), 0U );
		if ( CB_IMG_OK != nRet )
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_SRCH_CAND_CROSS_PNT );
		}

		/*
		==============================
			上下T字コーナー点探索
		==============================
		*/
		/* エッジコード情報の抽出 */
		nRet = cb_FPE_ExtractEdgeCodeInfo( ( ptFPEInfo->nImg )[CB_FPE_IMG_NORMAL], 
											( ptFPEInfo->nImg )[CB_FPE_IMG_EDGE_RHO_NORMAL], 
											( ptFPEInfo->nImg )[CB_FPE_IMG_EDGE_ANGLE_NORMAL], 
											( ptFPEInfo->nImg )[CB_FPE_IMG_EDGE_AC_NORMAL], 
											&( ptSrchRgnInfo[ni].tRgnImgNormalSize ), 
											nCamDirection );
		if ( CB_IMG_OK != nRet )
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_EDGE_CODE_INFO_ZOOMOUT );
		}

		/* 下側T字点探索 */
		/* 縮小画像上で、エッジ点検出(内部で左右市松マーカーで条件分岐できるように、niを引数として追加) */
		nRet = cb_FPE_SearchCandTPntUL4AddChk_byEdgeCode( &( ptFPEInfo->nImg[0] ), 
														&(ptSrchRsltPntInfo[ni].tCrossPnt[0]), 1L, 
														&(ptSrchRsltPntInfo[ni].tTPntLower[0]), &(ptSrchRsltPntInfo[ni].nNumTPntLower), 
														&(ptSrchRgnInfo[ni].tRgnImgNormalSize), CB_FPE_LOWER_EDGE_POS_PATTERN, ni, nCamDirection, &(ptSrchRgnInfo->tPtnInfo) );	
		if ( CB_IMG_OK != nRet )
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_SRCH_CAND_T_PNT_LOWER_ADD_CHK_PATTERN );
		}

		/* 上側Ｔ字点探索 */
		nRet = cb_FPE_SearchCandTPntUL4AddChk_byEdgeCode( &( ptFPEInfo->nImg[0] ), 
														&(ptSrchRsltPntInfo[ni].tCrossPnt[0]), 1L, 
														&(ptSrchRsltPntInfo[ni].tTPntUpper[0]), &(ptSrchRsltPntInfo[ni].nNumTPntUpper),
														&(ptSrchRgnInfo[ni].tRgnImgNormalSize), CB_FPE_UPPER_EDGE_POS_PATTERN, ni, nCamDirection, &(ptSrchRgnInfo->tPtnInfo) );
		if ( CB_IMG_OK != nRet )
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
			return ( CB_IMG_NG_SRCH_CAND_T_PNT_UPPER_ADD_CHK_PATTERN );
		}

		/*
		==============================
			高精度位置決め
		==============================
		*/

		/* 左右市松マーカーの上下特徴点を抽出 */
		nRet = cb_FPE_EstimateFtrPoint4AddChk( nImgSrc, &(ptFPEInfo->nImg[0]), &(ptSrchRgnInfo[ni]) , &(ptSrchRsltPntInfo[ni]), nCamDirection );

		/* 左右市松マーカーの中央十字点を抽出 */
		nRet = cb_FPE_EstimateFtrPoint4AddCrossPnt( nImgSrc, &(ptFPEInfo->nImg[0]), &(ptSrchRgnInfo[ni]) , &(ptSrchRsltPntInfo[ni]), nCamDirection );


	}

	/* Haar-like特徴量用の画像メモリ割り当て解除(画像メモリアクセス終了) */
	nRet = implib_CloseImgDirect( ( ptFPEInfo->nImg )[CB_FPE_HAAR_LIKE_FTR_CHK_EDGE_NORMAL] );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}



	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	/* IMPのエラーをまとめてチェック */
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(左右市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.11.05	S.Suzuki			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	float_t	fprm_RcpHaarArea;		/* Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_NormHaarWeight;	/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;	/* Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;	/* Haar-like小矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;

	/* 0割チェック */
	if( 0L == ( nprm_xsizeBlockW * nprm_ysizeBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_xsizeBlockW * nprm_ysizeBlockW ) );
	fprm_NormHaarWeight = ( 1.0F / 255.0F );

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
	if( 0L == ( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW) )
	{
		return ( CB_IMG_NG_DIV0 );
	}
	fprm_RcpHaarArea = ( 1.0F / (float_t)( nprm_xsizeSmallBlockW * nprm_ysizeSmallBlockW ) );

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;


	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 先頭ポインタ */
		/* 左上のブロック */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6767
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6812
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6814
		/* 右上のブロック */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6772
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6773
		/* 左下のブロック */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下のブロック */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6774
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6775
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6776
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6777

		/* 左上の小ブロック */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );			// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6767
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6812
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O3.1  , ID-6814
		/* 右上の小ブロック */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6772
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6773
		/* 左下の小ブロック */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小ブロック */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6774
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6775
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6776
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;								// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.2  , ID-6777

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 各ブロック内の輝度値合計 */
			nSumBlockLU = ( ( (*pnWkSumRL_BlockLU) - (*pnWkSumRU_BlockLU) ) - (*pnWkSumLL_BlockLU) ) + (*pnWkSumLU_BlockLU);
			nSumBlockRU = ( ( (*pnWkSumRL_BlockRU) - (*pnWkSumRU_BlockRU) ) - (*pnWkSumLL_BlockRU) ) + (*pnWkSumLU_BlockRU);
			nSumBlockLL = ( ( (*pnWkSumRL_BlockLL) - (*pnWkSumRU_BlockLL) ) - (*pnWkSumLL_BlockLL) ) + (*pnWkSumLU_BlockLL);
			nSumBlockRL = ( ( (*pnWkSumRL_BlockRL) - (*pnWkSumRU_BlockRL) ) - (*pnWkSumLL_BlockRL) ) + (*pnWkSumLU_BlockRL);

			nSumSmallBlockLU = ( ( (*pnWkSumRL_SmallBlockLU) - (*pnWkSumRU_SmallBlockLU) ) - (*pnWkSumLL_SmallBlockLU) ) + (*pnWkSumLU_SmallBlockLU);
			nSumSmallBlockRU = ( ( (*pnWkSumRL_SmallBlockRU) - (*pnWkSumRU_SmallBlockRU) ) - (*pnWkSumLL_SmallBlockRU) ) + (*pnWkSumLU_SmallBlockRU);
			nSumSmallBlockLL = ( ( (*pnWkSumRL_SmallBlockLL) - (*pnWkSumRU_SmallBlockLL) ) - (*pnWkSumLL_SmallBlockLL) ) + (*pnWkSumLU_SmallBlockLL);
			nSumSmallBlockRL = ( ( (*pnWkSumRL_SmallBlockRL) - (*pnWkSumRU_SmallBlockRL) ) - (*pnWkSumLL_SmallBlockRL) ) + (*pnWkSumLU_SmallBlockRL);

			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）
			                     ①      ←──────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumBlockLL - nSumBlockRU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumBlockLU - nSumBlockRL ) ) * fprm_CoeffHaarWeight;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumSmallBlockLL - nSumSmallBlockRU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumSmallBlockLU - nSumSmallBlockRL ) ) * fprm_CoeffSmallHaarWeight;

			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
//			*pnWkFtr = ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL );
//			*pnWkFtr =	(slong)( (float_t)( ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU);
			fHaar = ( (float_t)( ( nSumBlockLU + nSumBlockRL ) - ( nSumBlockRU + nSumBlockLL ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			fSmallHaar = ( (float_t)( ( nSumSmallBlockLU + nSumSmallBlockRL ) - ( nSumSmallBlockRU + nSumSmallBlockLL ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				*pnWkFtr = (slong)( fHaar * fabsf( fSmallHaar ) );
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(フロントカメラ左市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.08.01	S.Morita			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* 大領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpSmallHaarArea;			/* 小領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeHaarArea;			/* 大領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeSmallHaarArea;		/* 小領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */

	float_t	fprm_NormHaarWeight;			/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;			/* 大領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;		/* 小領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeHaarWeight;		/* 大領域Haar-like大矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* 小領域Haar-like大矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 構造体初期化 */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FR;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FR;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FR;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FR;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 輝度値合計の初期化 */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 修正市松パターン作成を作成する為の値  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* 先頭ポインタ */
		/* 左上の領域 */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW - Modify_Leng;
		/* 右上の領域 */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		/* 左下の領域 */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下の領域 */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* 左上の小領域 */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* 右上の小領域 */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* 左下の小領域 */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小領域 */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;
	
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 特徴量格納変数初期化 */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* 左マーカーの特徴点を取得する場合 */
	
			/* 各領域内の輝度値合計 */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumLL_BlockRU;
			stCalcInfo.pnFtrValue[1] = pnWkSumRU_BlockLL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumLL_SmallBlockRU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumRU_SmallBlockLL;

			/* 通常領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* 小領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
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
			
			/* 左マーカの特徴量を算出するパターンを用いた場合の重み設定 */
			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			
			/*                   ①    ←────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeSmallHaarWeight;

			/* 左マーカの特徴量を算出するパターンは、白領域の面積の総和が黒領域の面積の総和よりも大きい */
			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );

			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(リアカメラ左市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.08.01	S.Morita			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* 大領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpSmallHaarArea;			/* 小領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeHaarArea;			/* 大領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeSmallHaarArea;		/* 小領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */

	float_t	fprm_NormHaarWeight;			/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;			/* 大領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;		/* 小領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeHaarWeight;		/* 大領域Haar-like大矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* 小領域Haar-like大矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 構造体初期化 */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 輝度値合計の初期化 */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 修正市松パターン作成を作成する為の値  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* 先頭ポインタ */
		/* 左上の領域 */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW - Modify_Leng;
		/* 右上の領域 */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		/* 左下の領域 */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下の領域 */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* 左上の小領域 */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* 右上の小領域 */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* 左下の小領域 */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小領域 */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;
	
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 特徴量格納変数初期化 */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* 左マーカーの特徴点を取得する場合 */
	
			/* 各領域内の輝度値合計 */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumLL_BlockRU;
			stCalcInfo.pnFtrValue[1] = pnWkSumRU_BlockLL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumLL_SmallBlockRU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumRU_SmallBlockLL;

			/* 通常領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* 小領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
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
			
			/* 左マーカの特徴量を算出するパターンを用いた場合の重み設定 */
			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			/*                   ①    ←────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffLargeSmallHaarWeight;

			/* 左マーカの特徴量を算出するパターンは、白領域の面積の総和が黒領域の面積の総和よりも大きい */
			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			
			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( ( (float_t)tempLU_RL * fprm_RcpLargeSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(フロントカメラ右市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.08.01	S.Morita			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* 大領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpSmallHaarArea;			/* 小領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeHaarArea;			/* 大領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeSmallHaarArea;		/* 小領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */

	float_t	fprm_NormHaarWeight;			/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;			/* 大領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;		/* 小領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeHaarWeight;		/* 大領域Haar-like大矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* 小領域Haar-like大矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 構造体初期化 */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_FR;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_FR;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL_FR;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL_FR;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 輝度値合計の初期化 */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* カメラ種別がフロントカメラである場合は特徴点取得に用いるパターンを変更する */
	/* 右マーカーの特徴点を取得する場合には、左上と右下を結ぶ対角線を用いた修正市松パターンを用いる */

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 修正市松パターン作成を作成する為の値  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* 先頭ポインタ */
		/* 左上の領域 */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW + Modify_Leng;
		/* 右上の領域 */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		/* 左下の領域 */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下の領域 */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* 左上の小領域 */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* 右上の小領域 */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* 左下の小領域 */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小領域 */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 特徴量格納変数初期化 */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* 右マーカーの特徴点を取得する場合 */

			/* 各領域内の輝度値合計 */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumRL_BlockLU;
			stCalcInfo.pnFtrValue[1] = pnWkSumLU_BlockRL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumRL_SmallBlockLU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumLU_SmallBlockRL;

			/* 通常領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* 小領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
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

			/* 右マーカの特徴量を算出するパターンを用いた場合の重み設定 */
			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			/*                   ①    ←────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffSmallHaarWeight;


			/* 右マーカの特徴量を算出するパターンは、黒領域の面積の総和が白領域の面積の総和よりも大きい */
			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( (float_t)tempLU_RL * fprm_RcpHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU;

			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( (float_t)tempLU_RL * fprm_RcpSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU;

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifdef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(リアカメラ右市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.08.01	S.Morita			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* 大領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpSmallHaarArea;			/* 小領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeHaarArea;			/* 大領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeSmallHaarArea;		/* 小領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */

	float_t	fprm_NormHaarWeight;			/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;			/* 大領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;		/* 小領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeHaarWeight;		/* 大領域Haar-like大矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* 小領域Haar-like大矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	slong	tempLL_RU;
	slong	tempLU_RL;
	float_t	calctemp;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 構造体初期化 */
	memset( &stCalcInfo, 0x00, (size_t)sizeof(stCalcInfo));
	memset( &stCalcInfo_small, 0x00, (size_t)sizeof(stCalcInfo_small));

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
	nprm_xsizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
	nprm_ysizeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE;
	nprm_xsizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;
	nprm_ysizeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_YSIZE_SMALL;
	nprm_areaLargeBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE;
	nprm_areaSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL;
	nprm_areaLargeSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_LARGE_SMALL;
	nprm_areaSmallSmallBlockW = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_AREA_SMALL_SMALL;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 輝度値合計の初期化 */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* カメラ種別がリアカメラである場合は特徴点取得に用いるパターンを変更する */
	/* 右マーカーの特徴点を取得する場合には、左上と右下を結ぶ対角線を用いた修正市松パターンを用いる */

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 修正市松パターン作成を作成する為の値  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* 先頭ポインタ */
		/* 左上の領域 */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW + Modify_Leng;
		/* 右上の領域 */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		/* 左下の領域 */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下の領域 */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* 左上の小領域 */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* 右上の小領域 */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* 左下の小領域 */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小領域 */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 特徴量格納変数初期化 */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* 右マーカーの特徴点を取得する場合 */

			/* 各領域内の輝度値合計 */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumRL_BlockLU;
			stCalcInfo.pnFtrValue[1] = pnWkSumLU_BlockRL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumRL_SmallBlockLU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumLU_SmallBlockRL;

			/* 通常領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* 小領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo_small );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
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

			/* 右マーカの特徴量を算出するパターンを用いた場合の重み設定 */
			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）*/
			tempLL_RU = nSumBlockLL - nSumBlockRU;
			tempLU_RL = nSumBlockLU - nSumBlockRL;
			/*                   ①      ←──────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffHaarWeight;

			tempLL_RU = nSumSmallBlockLL - nSumSmallBlockRU;
			tempLU_RL = nSumSmallBlockLU - nSumSmallBlockRL;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( (double_t)tempLL_RU ) ) * fprm_CoeffLargeSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( (double_t)tempLU_RL ) ) * fprm_CoeffSmallHaarWeight;

			/* 右マーカの特徴量を算出するパターンは、黒領域の面積の総和が白領域の面積の総和よりも大きい */
			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
			tempLU_RL = nSumBlockLU + nSumBlockRL;
			tempLL_RU = nSumBlockRU + nSumBlockLL;
			fHaar = ( ( (float_t)tempLU_RL * fprm_RcpHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU;

			tempLU_RL = nSumSmallBlockLU + nSumSmallBlockRL;
			tempLL_RU = nSumSmallBlockRU + nSumSmallBlockLL;
			fSmallHaar = ( ( (float_t)tempLU_RL * fprm_RcpSmallHaarArea ) - ( (float_t)tempLL_RU * fprm_RcpLargeSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU;

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				calctemp = fHaar * fabsf( fSmallHaar );
				*pnWkFtr = (slong)calctemp;
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifndef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(フロントカメラ・リアカメラ左市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.08.01	S.Morita			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* 大領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpSmallHaarArea;			/* 小領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeHaarArea;			/* 大領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeSmallHaarArea;		/* 小領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */

	float_t	fprm_NormHaarWeight;			/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;			/* 大領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;		/* 小領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeHaarWeight;		/* 大領域Haar-like大矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* 小領域Haar-like大矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 構造体初期化 */
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

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
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

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 輝度値合計の初期化 */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 修正市松パターン作成を作成する為の値  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* 先頭ポインタ */
		/* 左上の領域 */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW - Modify_Leng;
		/* 右上の領域 */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		/* 左下の領域 */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下の領域 */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* 左上の小領域 */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* 右上の小領域 */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* 左下の小領域 */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小領域 */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;
	
		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 特徴量格納変数初期化 */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* 左マーカーの特徴点を取得する場合 */
	
			/* 各領域内の輝度値合計 */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumLL_BlockRU;
			stCalcInfo.pnFtrValue[1] = pnWkSumRU_BlockLL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumLL_SmallBlockRU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumRU_SmallBlockLL;

			/* 通常領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo, tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT]  );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* 小領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcLeftBrightness( &stCalcInfo_small, tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT]  );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
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
			
			/* 左マーカの特徴量を算出するパターンを用いた場合の重み設定 */
			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）
			                     ①      ←──────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumBlockLL - nSumBlockRU ) ) * fprm_CoeffHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumBlockLU - nSumBlockRL ) ) * fprm_CoeffLargeHaarWeight;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumSmallBlockLL - nSumSmallBlockRU ) ) * fprm_CoeffSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumSmallBlockLU - nSumSmallBlockRL ) ) * fprm_CoeffLargeSmallHaarWeight;

			/* 左マーカの特徴量を算出するパターンは、白領域の面積の総和が黒領域の面積の総和よりも大きい */
			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
			fHaar = ( (float_t)( ( ( nSumBlockLU + nSumBlockRL ) * fprm_RcpLargeHaarArea ) - ( ( nSumBlockRU + nSumBlockLL ) * fprm_RcpHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			fSmallHaar = ( (float_t)( ( ( nSumSmallBlockLU + nSumSmallBlockRL ) * fprm_RcpLargeSmallHaarArea ) - ( ( nSumSmallBlockRU + nSumSmallBlockLL ) * fprm_RcpSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				*pnWkFtr = (slong)( fHaar * fabsf( fSmallHaar ) );
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

#ifndef CALIB_PARAM_P32S_EUR_UK
/******************************************************************************/
/**
 * @brief		重み付きHaar-like特徴量の計算(フロントカメラ・リアカメラ右市松用)
 *
 * @param[in]	nIntegralImg	:,インテグラルイメージ画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	ptHaarLike		:,Haar-like特徴量構造体へのポインタ,-,[-],
 * @param[in]	ptSrchRgnInfo	:,処理領域構造体へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2014.08.01	S.Morita			新規作成
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

	/* パラメータ */
	slong	nprm_xsizeBlockW, nprm_ysizeBlockW;
	slong	nprm_xsizeSmallBlockW, nprm_ysizeSmallBlockW;
	slong	nprm_areaLargeBlockW, nprm_areaSmallBlockW;
	slong	nprm_areaLargeSmallBlockW, nprm_areaSmallSmallBlockW;
	float_t	fprm_RcpHaarArea;				/* 大領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpSmallHaarArea;			/* 小領域Haar-like矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeHaarArea;			/* 大領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */
	float_t	fprm_RcpLargeSmallHaarArea;		/* 小領域Haar-like大矩形の面積の逆数、輝度平均値の計算に使用 */

	float_t	fprm_NormHaarWeight;			/* 重みの正規化用パラメータ(255[輝度値]の逆数) */

	float_t	fprm_CoeffHaarWeight;			/* 大領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffSmallHaarWeight;		/* 小領域Haar-like矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeHaarWeight;		/* 大領域Haar-like大矩形の面積の逆数×重み値 */
	float_t	fprm_CoeffLargeSmallHaarWeight;	/* 小領域Haar-like大矩形の面積の逆数×重み値 */

	float_t	fHaar,fSmallHaar;

	/* 引数チェック */
	if ( ( NULL == ptHaarLike ) || ( NULL == ptSrchRgnInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 構造体初期化 */
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

	/* 処理領域取得 */
	nSxSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSX;
	nSySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nSY;
	nExSrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEX;
	nEySrchRgn = ptSrchRgnInfo->tAddChkSrchRgn.nEY;
	
	/* パラメータ取得 */
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

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffHaarWeight = fprm_RcpHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeHaarWeight = fprm_RcpLargeHaarArea * fprm_NormHaarWeight;

	/* 0割チェック */
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

	/* 正規化の値を計算 */
	fprm_CoeffSmallHaarWeight = fprm_RcpSmallHaarArea * fprm_NormHaarWeight;
	fprm_CoeffLargeSmallHaarWeight = fprm_RcpLargeSmallHaarArea * fprm_NormHaarWeight;

	/* 未処理領域分を考慮した処理領域 */
	nSxProc = nSxSrchRgn + nprm_xsizeBlockW;
	nSyProc = nSySrchRgn + nprm_ysizeBlockW;
	nExProc = nExSrchRgn - nprm_xsizeBlockW;
	nEyProc = nEySrchRgn - nprm_ysizeBlockW;

	/* 処理サイズ */
	nXSizeSrchRgn = ( nExProc - nSxProc ) + 1L;
	nYSizeSrchRgn = ( nEyProc - nSyProc ) + 1L;

	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nIntegralImg, &nXSizeIImg, &nYSizeIImg, (void**)&pnAddrIImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* 輝度値合計の初期化 */
	nSumBlockLU = 0L;
	nSumBlockRU = 0L;
	nSumBlockLL = 0L;
	nSumBlockRL = 0L;
	nSumSmallBlockLU = 0L;
	nSumSmallBlockRU = 0L;
	nSumSmallBlockLL = 0L;
	nSumSmallBlockRL = 0L;

	/* カメラ種別がフロントカメラ、もしくはリアカメラである場合は特徴点取得に用いるパターンを変更する */
	/* 右マーカーの特徴点を取得する場合には、左上と右下を結ぶ対角線を用いた修正市松パターンを用いる */

	/* Haar-like */
	/* 右下を基点にして計算 */
	for ( ni = 0L; ni < nYSizeSrchRgn; ni++ )
	{
		/* 修正市松パターン作成を作成する為の値  */
		Modify_Leng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE;
		Modify_SmallLeng = CB_FPE_PRM_HAAR_ADD_CHK_EDGE_BLOCK_XSIZE_SMALL;

		/* 先頭ポインタ */
		/* 左上の領域 */
		pnWkSumRL_BlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_BlockLU = pnWkSumRL_BlockLU - nprm_xsizeBlockW;
		pnWkSumRU_BlockLU = pnWkSumRL_BlockLU - ( nprm_ysizeBlockW * nXSizeIImg ) - Modify_Leng;
		pnWkSumLU_BlockLU = pnWkSumRU_BlockLU - nprm_xsizeBlockW + Modify_Leng;
		/* 右上の領域 */
		pnWkSumRL_BlockRU = pnWkSumRL_BlockLU + nprm_xsizeBlockW;
		pnWkSumLL_BlockRU = pnWkSumLL_BlockLU + nprm_xsizeBlockW;
		pnWkSumRU_BlockRU = pnWkSumRU_BlockLU + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumLU_BlockRU = pnWkSumLU_BlockLU + nprm_xsizeBlockW - Modify_Leng;
		/* 左下の領域 */
		pnWkSumRL_BlockLL = pnWkSumRL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLL_BlockLL = pnWkSumLL_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		pnWkSumRU_BlockLL = pnWkSumRU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg ) + Modify_Leng;
		pnWkSumLU_BlockLL = pnWkSumLU_BlockLU + ( nprm_ysizeBlockW * nXSizeIImg );
		/* 右下の領域 */
		pnWkSumRL_BlockRL = pnWkSumRL_BlockLL + nprm_xsizeBlockW - Modify_Leng;
		pnWkSumLL_BlockRL = pnWkSumLL_BlockLL + nprm_xsizeBlockW + Modify_Leng;
		pnWkSumRU_BlockRL = pnWkSumRU_BlockLL + nprm_xsizeBlockW;
		pnWkSumLU_BlockRL = pnWkSumLU_BlockLL + nprm_xsizeBlockW;

		/* 左上の小領域 */
		pnWkSumRL_SmallBlockLU = pnAddrIImg + ( ( ( nSyProc + ni ) * nXSizeIImg ) + nSxProc );
		pnWkSumLL_SmallBlockLU = pnWkSumRL_SmallBlockLU - nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockLU = pnWkSumRL_SmallBlockLU - ( nprm_ysizeSmallBlockW * nXSizeIImg ) - Modify_SmallLeng;
		pnWkSumLU_SmallBlockLU = pnWkSumRU_SmallBlockLU - nprm_xsizeSmallBlockW + Modify_SmallLeng;
		/* 右上の小領域 */
		pnWkSumRL_SmallBlockRU = pnWkSumRL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumLL_SmallBlockRU = pnWkSumLL_SmallBlockLU + nprm_xsizeSmallBlockW;
		pnWkSumRU_SmallBlockRU = pnWkSumRU_SmallBlockLU + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumLU_SmallBlockRU = pnWkSumLU_SmallBlockLU + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		/* 左下の小領域 */
		pnWkSumRL_SmallBlockLL = pnWkSumRL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLL_SmallBlockLL = pnWkSumLL_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		pnWkSumRU_SmallBlockLL = pnWkSumRU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg ) + Modify_SmallLeng;
		pnWkSumLU_SmallBlockLL = pnWkSumLU_SmallBlockLU + ( nprm_ysizeSmallBlockW * nXSizeIImg );
		/* 右下の小領域 */
		pnWkSumRL_SmallBlockRL = pnWkSumRL_SmallBlockLL + nprm_xsizeSmallBlockW - Modify_SmallLeng;
		pnWkSumLL_SmallBlockRL = pnWkSumLL_SmallBlockLL + nprm_xsizeSmallBlockW + Modify_SmallLeng;
		pnWkSumRU_SmallBlockRL = pnWkSumRU_SmallBlockLL + nprm_xsizeSmallBlockW;
		pnWkSumLU_SmallBlockRL = pnWkSumLU_SmallBlockLL + nprm_xsizeSmallBlockW;

		/* Haar-like */
		pnWkFtr = ptHaarLike->pnFtrValue + ( ( ( nSyProc + ni ) * ptHaarLike->nXSize ) + nSxProc );

		for ( nj = 0L; nj < nXSizeSrchRgn; nj++ )
		{
			/* 重み計算用変数初期化 */
			fHaarWeight_LL_RU = 0.0F;
			fHaarWeight_LU_RL = 0.0F;

			/* 特徴量格納変数初期化 */
			fHaar = 0.0F;
			fSmallHaar = 0.0F;

			/* 右マーカーの特徴点を取得する場合 */

			/* 各領域内の輝度値合計 */
			stCalcInfo.nHaarXSize = nprm_xsizeBlockW;
			stCalcInfo.nXSize = nXSizeIImg;
			stCalcInfo.pnFtrValue[0] = pnWkSumRL_BlockLU;
			stCalcInfo.pnFtrValue[1] = pnWkSumLU_BlockRL;

			stCalcInfo_small.nHaarXSize = nprm_xsizeSmallBlockW;
			stCalcInfo_small.nXSize = nXSizeIImg;
			stCalcInfo_small.pnFtrValue[0] = pnWkSumRL_SmallBlockLU;
			stCalcInfo_small.pnFtrValue[1] = pnWkSumLU_SmallBlockRL;

			/* 通常領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo,tLargeAreaYPos[CB_FPE_SLOPE_TYPE_VERT] );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
				nRet = implib_ClearIPError();
				nRet = implib_CloseImgDirect( nIntegralImg );
				return ( CB_IMG_NG );
			}

			/* 小領域においてパターンの修正により面積が減少した領域の輝度値合計を計算する */
			nRet = cb_FPE_CalcRightBrightness( &stCalcInfo_small, tSmallAreaYPos[CB_FPE_SLOPE_TYPE_VERT] );
			if ( 0L > nRet )
			{
				/* エラー情報クリア */
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

			/* 右マーカの特徴量を算出するパターンを用いた場合の重み設定 */
			/*
			 輝度平均の差分を元に、重みを計算
			 ①：大小逆転させるための減算(1に近いほど、重みが大きくなる変換)
			 ②：左上と右下 / 左下と右上 の輝度平均の差
			 ③：②を255(輝度差の最大値)で割ることで、0～1に正規化（あらかじめ計算した値）
			                     ①      ←──────────────②─────→              ③        */
			fHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumBlockLL - nSumBlockRU ) ) * fprm_CoeffLargeHaarWeight;
			fHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumBlockLU - nSumBlockRL ) ) * fprm_CoeffHaarWeight;

			fSmallHaarWeight_LL_RU = 1.0F - (float_t)( fabs( nSumSmallBlockLL - nSumSmallBlockRU ) ) * fprm_CoeffLargeSmallHaarWeight;
			fSmallHaarWeight_LU_RL = 1.0F - (float_t)( fabs( nSumSmallBlockLU - nSumSmallBlockRL ) ) * fprm_CoeffSmallHaarWeight;


			/* 右マーカの特徴量を算出するパターンは、黒領域の面積の総和が白領域の面積の総和よりも大きい */
			/* 特徴量 */
			/* Haar-like特徴量に上記重みを掛けあわせ、市松らしい箇所の特徴量が大きくなるように調整*/
			fHaar = ( (float_t)( ( ( nSumBlockLU + nSumBlockRL ) * fprm_RcpHaarArea ) - ( ( nSumBlockRU + nSumBlockLL ) * fprm_RcpLargeHaarArea ) ) * fHaarWeight_LU_RL * fHaarWeight_LL_RU );
			fSmallHaar = ( (float_t)( ( ( nSumSmallBlockLU + nSumSmallBlockRL ) * fprm_RcpSmallHaarArea ) - ( ( nSumSmallBlockRU + nSumSmallBlockLL ) * fprm_RcpLargeSmallHaarArea ) ) * fSmallHaarWeight_LU_RL * fSmallHaarWeight_LL_RU );

			/* 小領域と通常領域のHaarが同じ符号の時 */
			if( ( fSmallHaar * fHaar ) > 0.0F )
			{
				*pnWkFtr = (slong)( fHaar * fabsf( fSmallHaar ) );
			}
			else
			{
				/* Haarの値を格納しない */
				*pnWkFtr = 0L;
			}

			/* ポインタを進める */
			/* 左上 */
			pnWkSumRL_BlockLU++;
			pnWkSumLL_BlockLU++;
			pnWkSumRU_BlockLU++;
			pnWkSumLU_BlockLU++;
			/* 右上 */
			pnWkSumRL_BlockRU++;
			pnWkSumLL_BlockRU++;
			pnWkSumRU_BlockRU++;
			pnWkSumLU_BlockRU++;
			/* 左下 */
			pnWkSumRL_BlockLL++;
			pnWkSumLL_BlockLL++;
			pnWkSumRU_BlockLL++;
			pnWkSumLU_BlockLL++;
			/* 右下 */
			pnWkSumRL_BlockRL++;
			pnWkSumLL_BlockRL++;
			pnWkSumRU_BlockRL++;
			pnWkSumLU_BlockRL++;

			/* 左上 */
			pnWkSumRL_SmallBlockLU++;
			pnWkSumLL_SmallBlockLU++;
			pnWkSumRU_SmallBlockLU++;
			pnWkSumLU_SmallBlockLU++;
			/* 右上 */
			pnWkSumRL_SmallBlockRU++;
			pnWkSumLL_SmallBlockRU++;
			pnWkSumRU_SmallBlockRU++;
			pnWkSumLU_SmallBlockRU++;
			/* 左下 */
			pnWkSumRL_SmallBlockLL++;
			pnWkSumLL_SmallBlockLL++;
			pnWkSumRU_SmallBlockLL++;
			pnWkSumLU_SmallBlockLL++;
			/* 右下 */
			pnWkSumRL_SmallBlockRL++;
			pnWkSumLL_SmallBlockRL++;
			pnWkSumRU_SmallBlockRL++;
			pnWkSumLU_SmallBlockRL++;


			/* Haar-Like */
			pnWkFtr++;
		}
	}

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nIntegralImg );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CALIB_PARAM_P32S_EUR_UK */

/******************************************************************************/
/**
 * @brief		輝度値計算(フロントカメラ・リアカメラ時の左マーカに対して使用する)
 *
 * @param[in out]	pnCalcBrightness		:,輝度値計算結果格納配列へのポインタ,-,[-],
 * @param[in]		ptCalcInfo				:,輝度値計算に用いる情報を持つ構造体,-,[-],
 *
 * @retval		CB_IMG_OK					:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG_ARG_NULL			:,異常終了(引数NULL)
 *
 * @date		2014.08.01	S.Morita		新規作成
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

	/* 引数チェック */
	if ( NULL == ptCalcInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nloopsize = ptCalcInfo->nHaarXSize;
	pnWkSumLL_BlockRU = ptCalcInfo->pnFtrValue[0];
	pnWkSumRU_BlockLL = ptCalcInfo->pnFtrValue[1];
	nXSizeIImg = ptCalcInfo->nXSize;

	/* 各ブロック内の輝度値合計 */
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
 * @brief		輝度値計算(フロントカメラ・リアカメラ時の右マーカに対して使用する)
 *
 * @param[in out]	pnCalcBrightness		:,輝度値計算結果格納配列へのポインタ,-,[-],
 * @param[in]		ptCalcInfo				:,輝度値計算に用いる情報を持つ構造体,-,[-],
 *
 * @retval		CB_IMG_OK					:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG_ARG_NULL			:,異常終了(引数NULL)
 *
 * @date		2014.08.01	S.Morita		新規作成
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

	/* 引数チェック */
	if ( NULL == ptCalcInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nloopsize = ptCalcInfo->nHaarXSize;
	pnWkSumRL_BlockLU = ptCalcInfo->pnFtrValue[0];
	pnWkSumLU_BlockRL = ptCalcInfo->pnFtrValue[1];
	nXSizeIImg = ptCalcInfo->nXSize;

	/* 各ブロック内の輝度値合計 */
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
 * @brief		中央値算出
 *
 * @param[out]	nMedianValue				:,算出した中央値,-,[-],
 * @param[in]	nValArray[3]				:,値が格納されている配列,-,[-],
 * @param[in]	nNumValue					:,配列のサイズ,-,[-],
 * @retval		CB_IMG_OK					:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG_ARG_NULL			:,異常終了(引数NULL)
 *
 * @date		2014.08.05	S.Morita		新規作成
 */
/******************************************************************************/
static slong cb_FPE_CalcMedian( slong* nMedianValue, const slong* const nValArray, const slong nNumValue)
{
	slong	nRet = CB_IMG_NG;

	/* 引数チェック */
	if (( NULL == nMedianValue ) || ( NULL == nValArray ))
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 配列をソートする */
	nRet = cb_FPE_Sort( nValArray, nNumValue );
	if ( CB_IMG_OK != nRet ) 
	{
		return ( CB_IMG_NG );
	}
	
	/* 中央値を取得する（配列）*/
	/* ※配列サイズが偶数の場合は配列の真ん中の１つ小さい要素数を中央値にする */
	*nMedianValue = nValArray[ ( nNumValue - 1L ) / 2L ];
	
	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		エッジ角度 => 角度コード変換テーブルの作成
 *
 * @param[out]	pnTbl4FrRr					:,変換テーブル(Fr/Rr用)へのポインタ,-,[-],
 * @param[out]	pnTbl4LR					:,変換テーブル(SL/SR用)へのポインタ,-,[-],
 *
 * @retval		CB_IMG_OK					:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)				:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.12.02	K.Kato			新規作成
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
	/* パラメータ */
	slong	nprm_maxEdgeAC;
	slong	nprm_startEdgeAngle[CB_FPE_EDGE_AC_TBL_MAXNUM];
	slong	nprm_rangeEdgeAngleLR[CB_FPE_EDGE_AC_TBL_MAXNUM], 
			nprm_rangeEdgeAngleUL[CB_FPE_EDGE_AC_TBL_MAXNUM];

	/* 引数チェック */
	if ( ( NULL == pnTbl4FrRr ) || ( NULL == pnTbl4LR ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	nMaxEdgeCodeAngle = 181L;

	/* パラメータ */
	nprm_maxEdgeAC = CB_FPE_EDGE_AC_MAXNUM;
	nprm_startEdgeAngle[CB_FPE_EDGE_AC_TBL_FR_RR] = CB_FPE_PRM_START_EDGE_ANGLE_FR_RR;
	nprm_startEdgeAngle[CB_FPE_EDGE_AC_TBL_SL_SR] = CB_FPE_PRM_START_EDGE_ANGLE_SL_SR;
	nprm_rangeEdgeAngleLR[CB_FPE_EDGE_AC_TBL_FR_RR] = CB_FPE_PRM_RANGE_EDGE_ANGLE_FR_RR;
	nprm_rangeEdgeAngleLR[CB_FPE_EDGE_AC_TBL_SL_SR] = CB_FPE_PRM_RANGE_EDGE_ANGLE_SL_SR;

	/* 変換テーブル */
	pnTopAddrCnvLUT[CB_FPE_EDGE_AC_TBL_FR_RR] = pnTbl4FrRr;
	pnTopAddrCnvLUT[CB_FPE_EDGE_AC_TBL_SL_SR] = pnTbl4LR;
	
	for ( nj = 0L; nj < CB_FPE_EDGE_AC_TBL_MAXNUM; nj++ )
	{
		pnCnvLUT = pnTopAddrCnvLUT[nj];
		
		/* 初期値 */
		for ( ni = 0L; ni < 256L; ni++ )
		{
			pnCnvLUT[ni] = nprm_maxEdgeAC;
		}	/* for ( ni ) */

		/* テーブル作成 */
		nEdgeAngle = nprm_startEdgeAngle[nj];	/* 開始位置 */
		nprm_rangeEdgeAngleUL[nj] = ( ( nMaxEdgeCodeAngle - 1L ) / 2L ) - nprm_rangeEdgeAngleLR[nj];
		nEdgeAC = CB_FPE_EDGE_AC_RIGHT;	/* 開始コード */
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
				/* エラー */
			}

			nEdgeAngle++;
			if ( ( nMaxEdgeCodeAngle - 1L ) < nEdgeAngle )
			{
				nEdgeAngle = 1L;
			}
		}	/* for ( ni ) */
		pnCnvLUT[nMaxEdgeCodeAngle] = CB_FPE_EDGE_AC_RIGHT;	/* 角度360度相当を0度と同じ扱いにする */
	}	/* for ( nj ) */

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief		エッジコード情報の抽出
 *
 * @param[in]	nImgSrc						:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	nImgEdgeRho					:,エッジ強度画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	nImgEdgeAngle				:,エッジ角度画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[out]	nImgEdgeAC					:,エッジ角度コード画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	ptSrchRgn					:,探索領域へのポインタ,-,[-],
 * @param[in]	nCamDirection				:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK					:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)				:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.11.29	K.Kato			新規作成
 * @date		2013.11.29	S.Suzuki		エッジ角度画面生成を追加
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
	/* パラメータ */
	slong	nprm_edgeShiftDown, 
			nprm_edgeThrMin, 
			nprm_edgeThrMax;
	slong	*pnprm_EdgeCoeff_H, 
			*pnprm_EdgeCoeff_V;

	/* 引数チェック */
	if ( NULL == ptSrchRgn )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ */
	nprm_edgeShiftDown = CB_FPE_PRM_EDGE_CODE_SHIFT_DOWN;
	nprm_edgeThrMin = CB_FPE_PRM_EDGE_CODE_THR_MIN;
	nprm_edgeThrMax = CB_FPE_PRM_EDGE_CODE_THR_MAX;
	pnprm_EdgeCoeff_H = &( m_FPE_nprm_EdgeCodeCoeff_H[0] );
	pnprm_EdgeCoeff_V = &( m_FPE_nprm_EdgeCodeCoeff_V[0] );

	/* ウィンドウ情報の退避 */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );

	/* 探索領域 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;

	/*
	==================================================
		エッジコード
	==================================================
	*/
	ptEdgeCodeTbl = &( m_FPE_tEdgeCodeTbl[0] );
	nNumEdgeCodeTbl = CB_FPE_EDGE_CODE_TBL_MAXNUM;

	/* エッジコード設定の初期化：角度算出用変換テーブル書き込み */
	nRet = implib_SetEdgeCodeTbl( &( CB_atanTbl[0] ), &( CB_thetaTbl[0] ) );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	/* エッジコード設定の初期化：環境設定 */
	nRet = implib_SetEdgeCodeConfig( IMPLIB_EC_BIN, IMPLIB_RHO_H_V_APPR, IMPLIB_EC_VOLUME3 );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	/* エッジコード算出 */
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
		エッジ強度＆角度コード
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
		/* 通常ありえないパス */
		return ( CB_IMG_NG );
	}

	/* 画像クリア */
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

	/* 画像メモリアクセス開始 */
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
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( nImgEdgeRho );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( nImgEdgeAC, &nXSizeEdgeAC, &nYSizeEdgeAC, (void**)&pnAddrEdgeAC );
	if ( 0L > nRet )
	{
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( nImgEdgeAngle );
		nRet = implib_CloseImgDirect( nImgEdgeRho );
		return ( CB_IMG_NG_IMPLIB );
	}

	ptWkEdgeCodeTbl = ptEdgeCodeTbl;
	for ( ni = 0L; ni < nNumEdgeCode; ni++ )
	{
		/* 座標値＆強度＆角度 */
		nXedge = nSxSrchRgn + (slong)ptWkEdgeCodeTbl->x;
		nYedge = nSySrchRgn + (slong)ptWkEdgeCodeTbl->y;
		nEdgeRho = ptWkEdgeCodeTbl->rho;
		nEdgeAngle = ptWkEdgeCodeTbl->theta;

		/* 強度値格納 */
		nOffsetImg = ( nYedge * nXSizeEdgeRho ) + nXedge;
		pnWkAddrEdgeRho = pnAddrEdgeRho + nOffsetImg;
		*pnWkAddrEdgeRho = nEdgeRho;

		/* エッジ角度値格納 */
		nOffsetImg = ( nYedge * nXSizeEdgeAngle ) + nXedge;
		pnWkAddrEdgeAngle = pnAddrEdgeAngle + nOffsetImg;
		*pnWkAddrEdgeAngle = nEdgeAngle;

		/* エッジ角度コード格納 */
		nOffsetImg = ( nYedge * nXSizeEdgeAC ) + nXedge;
		pnWkAddrEdgeAC = pnAddrEdgeAC + nOffsetImg;
		*pnWkAddrEdgeAC = (uchar)( pnCnvLUT[nEdgeAngle] );

		/* 次のエッジコード */
		ptWkEdgeCodeTbl++;
	}	/* for ( ni ) */

	/* 画像メモリアクセス終了 */
	nRet = implib_CloseImgDirect( nImgEdgeAC );
	nRet = implib_CloseImgDirect( nImgEdgeAngle );
	nRet = implib_CloseImgDirect( nImgEdgeRho );

	/* 画面データタイプを強制的に設定 */
	nRet = implib_ChangeImgDataType( nImgEdgeRho, IMPLIB_UNSIGN8_DATA );
	nRet = implib_ChangeImgDataType( nImgEdgeAngle, IMPLIB_UNSIGN8_DATA );
	nRet = implib_ChangeImgDataType( nImgEdgeAC, IMPLIB_UNSIGN8_DATA );

	/* 関数内部でシステムデータを変更したりしているので、UNSIGN8_DATAでRETURNする */
	nRet = implib_SetIPDataType( IMPLIB_UNSIGN8_DATA );

	/* ウィンドウ情報の復帰 */
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
 * @brief		上/下側Ｔ字点(候補)の探索(左右市松用)
 *
 * @param[in]	pnImgTbl		:,ソース画面IDへのポインタ,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]	ptCrossPnt		:,画像座標(十字位置)構造体へのポインタ,-,[-],
 * @param[in]	nNumCrossPnt	:,画像座標(十字位置)構造体の数,-,[-],
 * @param[out]	ptDstPnt		:,画像座標(Ｔ字位置)構造体へのポインタ,-,[-],
 * @param[out]	pnNumDstPnt		:,画像座標(Ｔ字位置)構造体の数,-,[-],
 * @param[in]	ptSrchRgn		:,矩形領域構造体へのポインタ,-,[-],
 * @param[in]	nEdgePos		:,探索方向,CB_FPE_UPPER_EDGE_POS_PATTERN<=value<=CB_FPE_LOWER_EDGE_POS_PATTERN,[-],
 * @param[in]	nAddPatternPos	:,追加パターンの位置,CB_ADD_PATTERN_POS_LEFT<=value<=CB_ADD_PATTERN_POS_RIGHT,[-],
 * @param[in]	nCamDirection	:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<E_CB_SELFCALIB_CAMPOS_MAX,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.11.28	K.Kato			新規作成
 * @date		2015.08.26	S.Morita		サイドマーカー画像サイズ修正対応/探索領域更新位置修正
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
	/* パラメータ */
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

	/* 引数チェック */
	if ( ( NULL == pnImgTbl ) || ( NULL == ptCrossPnt ) || ( NULL == ptDstPnt ) || ( NULL == pnNumDstPnt ) || ( NULL == ptSrchRgn ) || ( NULL == ptPatternInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
	nprm_distanceSrchRgn = CB_FPE_PRM_DISTANCE_SRCH_RGN_ADD_CHK_T_PNT_UL;
	nprm_thrEdgeRho = CB_FPE_PRM_THR_EDGE_RHO_SRCH_CAND_T_PNT_UL_ADD_CHK;

	/* 画面ID */
	nImgEdgeRho = pnImgTbl[ CB_FPE_IMG_EDGE_RHO_NORMAL ];
	nImgEdgeAC = pnImgTbl[ CB_FPE_IMG_EDGE_AC_NORMAL ];
	nImgEdgeTheta = pnImgTbl[ CB_FPE_IMG_EDGE_ANGLE_NORMAL ];

	/* 探索領域設定 */
	nSxSrchRgn = ptSrchRgn->nSX;
	nSySrchRgn = ptSrchRgn->nSY;
	nExSrchRgn = ptSrchRgn->nEX;
	nEySrchRgn = ptSrchRgn->nEY;
	nXSizeSrchRgn = ( nExSrchRgn - nSxSrchRgn ) + 1L;
	nYSizeSrchRgn = ( nEySrchRgn - nSySrchRgn ) + 1L;

	/* 探索方向の係数を取得 */
	nRet = cb_FPE_GetSearchCoeffDir( nEdgePos, nAddPatternPos, &nCoeffDirX, &nCoeffDirY );
	if ( 0L > nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 設定された係数から、探索方向を設定 */
	if ( 0L < nCoeffDirY )
	{
		/* 正の数の場合は、下方向に*/
		nYSrchRgn = nEySrchRgn;
	}
	else
	{
		/* 負の数の場合は、上方向に*/
		nYSrchRgn = nSySrchRgn;
	}

	/*
	==================================================
		特徴点探索
	==================================================
	*/
	/* 重み付きHaar-like特徴量符号の取得 */
	nRet = cb_FPE_GetSignHaarLikeFeature( ptPatternInfo, nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		return ( CB_IMG_NG );
	}

	/* 探索対象の角度コードを設定(テーブル化かなぁ) */
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

	/* 強度＆角度コード＆角度を使って探索します */
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

	/* 探索 */
	/* 初期化 */
	nNumDstPnt = 0L;

	/* エッジ強度・エッジ角度コード・エッジ角度 */
	/* 初期化 */
	nEdgeRho = 0L;
	nEdgeAC = 0L;
	nEdgeTheta = 0L;

	pnWkAddrEdgeTheta =NULL;

	/* エッジ強度中央値 */
	/* 初期化 */
	memset( EdgeRhoArray, 0, sizeof( EdgeRhoArray ) );
	nmedianEdgeRho = 0L;

	/* エッジ角度中央値 */
	/* 初期化 */
	memset( EdgeThetaArray, 0, sizeof( EdgeThetaArray ) );
	nmedianEdgeTheta = 0L;

	/* 探索終了条件判定に用いる値(暫定的に設定している。テストを経て修正を行う値) */
	nEndCheckEdgeTheta = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_THETA_LIMIT;
	nEndCheckEdgeRho = CB_FPE_PRM_THR_HAAR_FTR_T_PNT_RHO_LIMIT;

	/* 左右市松であれば、nNumCrossPntは1で固定のはずなので、不要なLoop? */
	for ( ni = 0L; ni < nNumCrossPnt; ni++ )
	{
		ptDstPnt[ni].nX = -1L;
		ptDstPnt[ni].nY = -1L;

		if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) )
		{
			/* パラメータ設定 */
			nprm_xsizeHalfScanRgn = CB_FPE_PRM_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK;

			/* 探索基準点を計算 */
			/* Y座標は、市松十字点から上/下方向に所定幅動かした場所 */
			nXattn = ptCrossPnt[ni].nX;
			nYattn = ptCrossPnt[ni].nY + ( nCoeffDirY * nprm_distanceSrchRgn );

			/* 探索範囲と照らし合わせ、Y方向の最大探索幅を計算 */
			nYSizeScanRgn = ( ( nYSrchRgn - nYattn ) * nCoeffDirY ) + 1L;

			tPrevMaxPntInfo.nFtrValue = -1L;	/* 無効値 */
			tPrevMaxPntInfo.tPnt.nX = -1L;	/* 無効値 */
			tPrevMaxPntInfo.tPnt.nY = -1L;	/* 無効値 */
			tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
			tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
			tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */
			nFlgSrchComp = CB_FALSE;

			/* 以下、Y方向に探索を進める */

			for ( nj = 0L; nj < nYSizeScanRgn; nj++ )
			{
				/* 探索領域内チェック */
				/* ラインフィルタ幅(nSizeNoUse)を、X方向に反映 */
				if ( CB_TRUE != CB_FPE_CHK_OUTSIDE_RGN( nXattn, nYattn, nSxSrchRgn, nSySrchRgn, nExSrchRgn, nEySrchRgn ) )
				{
					if ( 0L >= tPrevMaxPntInfo.nFtrValue )
					{
						tPrevMaxPntInfo.nFtrValue = 0L;
					}
					break;
				}

				/* 探索基準点のアドレスを計算 */
				pnWkAddrEdgeRho = pnAddrEdgeRho + ( nYattn * nXSizeEdgeRho ) + nXattn;
				pnWkAddrEdgeAC = pnAddrEdgeAC + ( nYattn * nXSizeEdgeAC ) + nXattn;

				if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
				{
					pnWkAddrEdgeTheta = pnAddrEdgeTheta +  ( nYattn * nXSizeEdgeTheta ) + nXattn;
				}

				tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
				tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
				tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */
				nMaxPntEdgeTheta = 0L;			/* 無効値 */

				/* エッジ強度が大きい点＆角度コードが所定のコードの点を探索 */
				for ( nk = -nprm_xsizeHalfScanRgn; nk <= nprm_xsizeHalfScanRgn; nk++ )
				{
					/* エッジ強度と角度コードとエッジ角度 */
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
							/* エッジ強度とエッジ角度の中央値取得の為に情報を格納する */
							if ( nj < CB_FPE_MAX_ARREY_EGDE_MEDIAN )
							{
								EdgeRhoArray[nj] = nEdgeRho;
								EdgeThetaArray[nj] = nEdgeTheta;
							}

						}
					}
				}	/* for ( nk ) */

				/* 注目点更新 */
				if ( -1L < tPrevMaxPntInfo.nFtrValue )	/* 初回探索以外 */
				{
					if (   ( nprm_thrEdgeRho < tMaxPntInfo.nFtrValue )
						&& ( ( -1L < tMaxPntInfo.tPnt.nX ) && ( -1L < tMaxPntInfo.tPnt.nY ) ) )
					{
						if ( ( E_CB_SELFCALIB_CAMPOS_FRONT == nCamDirection ) || ( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection ) )
						{
							/* CB_FPE_MAX_ARREY_EGDE_MEDIAN回目の探索以降、処理終了判定にエッジ強度中央値とエッジ角度中央値を用いる */
							if ( (CB_FPE_MAX_ARREY_EGDE_MEDIAN - 1L) == nj )
							{
								/* エッジ強度の中央値を算出 */
								nRet = cb_FPE_CalcMedian( &nmedianEdgeRho, EdgeRhoArray, CB_FPE_MAX_ARREY_EGDE_MEDIAN );
								if ( 0L > nRet )
								{
									/* エラー情報クリア */
									nRet = implib_ClearIPError();
									nRet = implib_CloseImgDirect( nImgEdgeRho );
									nRet = implib_CloseImgDirect( nImgEdgeAC );
									nRet = implib_CloseImgDirect( nImgEdgeTheta );
									return ( CB_IMG_NG );
								}

								/* エッジ角度の中央値を算出 */
								nRet = cb_FPE_CalcMedian( &nmedianEdgeTheta, EdgeThetaArray, CB_FPE_MAX_ARREY_EGDE_MEDIAN );
								if ( 0L > nRet )
								{
									/* エラー情報クリア */
									nRet = implib_ClearIPError();
									nRet = implib_CloseImgDirect( nImgEdgeRho );
									nRet = implib_CloseImgDirect( nImgEdgeAC );
									nRet = implib_CloseImgDirect( nImgEdgeTheta );
									return ( CB_IMG_NG );
								}
							}
						
							/* CB_FPE_MAX_ARREY_EGDE_MEDIAN回目の探索以降、エッジ強度とエッジ強度中央値の差とエッジ角度とエッジ角度中央値の差がそれぞれ誤差以内に収まらないならば、探索を打ち切る */
							if ( nj >= (CB_FPE_MAX_ARREY_EGDE_MEDIAN -1L) )
							{
								if ( ( labs( nMaxPntEdgeTheta - nmedianEdgeTheta ) > nEndCheckEdgeTheta ) || ( labs( tMaxPntInfo.nFtrValue - nmedianEdgeRho ) > nEndCheckEdgeRho ) )
								{
									nFlgSrchComp = CB_TRUE;	/* 探索完了 */
									break;
								}
							}
						}

						/* 探索続行(Y方向に1シフトさせた位置を注目領域とする) */
						nXattn = tMaxPntInfo.tPnt.nX;
						nYattn = tMaxPntInfo.tPnt.nY + ( nCoeffDirY * 1L );
						tPrevMaxPntInfo = tMaxPntInfo;
					}
					else
					{
						nFlgSrchComp = CB_TRUE;	/* 探索完了 */
						break;
					}
				}
				else	/* 初回探索 */
				{
					if (   ( nprm_thrEdgeRho < tMaxPntInfo.nFtrValue )
						&& ( ( -1L < tMaxPntInfo.tPnt.nX ) && ( -1L < tMaxPntInfo.tPnt.nY ) ) )
					{
						/* 探索続行(Y方向に1シフトさせた位置を注目領域とする) */
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

				/* 2回目以降の探索では、探索領域を狭める */
				/* パラメータ設定 */
				nprm_xsizeHalfScanRgn = CB_FPE_PRM_REFINE_SCAN_RGN_SRCH_CAND_T_PNT_UL_ADD_CHK;
			}	/* for ( nj ) */

			/* 信頼度チェックするかなぁ */
			if ( nprm_thrEdgeRho < tPrevMaxPntInfo.nFtrValue )
			{
				ptDstPnt[ni].nX = tPrevMaxPntInfo.tPnt.nX;
				ptDstPnt[ni].nY = tPrevMaxPntInfo.tPnt.nY;
			}

		}	/* if ( ( 0L <= ptCrossPnt[ni].nX ) && ( 0L <= ptCrossPnt[ni].nY ) ) */

		/* 点数をインクリメント */
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
 * @brief		探索方向の係数設定
 *
 * ★★★↓param[input/output]は要修正★★★
 *
 * @param[in]	nEdgePos		:,探索方向,CB_FPE_UPPER_EDGE_POS_PATTERN<=value<=CB_FPE_LOWER_EDGE_POS_PATTERN,[-],
 * @param[in]	nAddPatternPos	:,追加パターンの位置定義,CB_ADD_PATTERN_POS_LEFT<=value<=CB_ADD_PATTERN_POS_RIGHT,[-],
 * @param[out]	nCoeffDirX		:,X軸方向の係数,-1<=value<=+1,[-],
 * @param[out]	nCoeffDirY		:,Y軸方向の係数,-1<=value<=+1,[-],
 *
 * @retval		CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval		CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.11.21	F.Sano			新規作成
 */
/******************************************************************************/
static slong cb_FPE_GetSearchCoeffDir( enum enum_CB_FPE_EDGE_POS_PATTERN nEdgePos, enum enum_CB_ADD_PATTERN_POS nAddPatternPos, slong* nCoeffDirX, slong* nCoeffDirY)
{
	/* 引数チェック */
	if ( ( NULL == nCoeffDirX ) || ( NULL == nCoeffDirY ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 初期化*/
	*nCoeffDirX = 0L;
	*nCoeffDirY = 0L;
	
	/* 探索方向の係数 */
	if ( CB_FPE_LOWER_EDGE_POS_PATTERN == nEdgePos )
	{
		*nCoeffDirY = (+1L);

		/* 左右どちらのマーカーかによって、探索によるXの変化量の正負が変わる */
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

		/* 左右どちらのマーカーかによって、探索によるXの変化量の正負が変わる */
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
 * @brief		[デバッグ]追加パターン(市松)における探索領域描画
 *
 * @param[in,out]	nImgDst			:,出力画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		nCamPos			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in]		nZoomMode		:,拡大/縮小モード,0<=value<=1,[-],
 *
 * @retval			CB_IMG_OK		:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)	:,異常終了,value=-1～-**,[-],
 *
 * @date		2013.03.01	K.Kato			新規作成
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


	/* ウィンドウ情報退避(全ウィンドウ) */
	nRet = implib_ReadWindow( IMPLIB_SRC0_WIN, &nSxSrc0, &nSySrc0, &nExSrc0, &nEySrc0 );
	nRet = implib_ReadWindow( IMPLIB_SRC1_WIN, &nSxSrc1, &nSySrc1, &nExSrc1, &nEySrc1 );
	nRet = implib_ReadWindow( IMPLIB_DST_WIN, &nSxDst, &nSyDst, &nExDst, &nEyDst );
	nRet = implib_ReadWindow( IMPLIB_SYS_WIN, &nSxSys, &nSySys, &nExSys, &nEySys );

	/* 画像情報取得 */
	nRet = implib_ReadImgTable( nImgDst, &tImgTbl );

	/* 描画設定 */
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
		/* 十字点探索領域矩形描画         */
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


	/* ウィンドウ情報復帰(全ウィンドウ) */
	nRet = implib_SetWindow( IMPLIB_SRC0_WIN, nSxSrc0, nSySrc0, nExSrc0, nEySrc0 );
	nRet = implib_SetWindow( IMPLIB_SRC1_WIN, nSxSrc1, nSySrc1, nExSrc1, nEySrc1 );
	nRet = implib_SetWindow( IMPLIB_DST_WIN, nSxDst, nSyDst, nExDst, nEyDst );
	nRet = implib_SetWindow( IMPLIB_SYS_WIN, nSxSys, nSySys, nExSys, nEySys );

	/* デバッグ時のimplibライブラリエラー確認処理 */
	nRet = implib_ReadIPErrorTable( &m_FPE_tWorkInfo.tErrTbl );
	if( 0L != m_FPE_tWorkInfo.tErrTbl.ErrorCode )
	{
		/* エラー発生時 :implib_ReadIPErrorTableはエラークリア処理が含まれているため、エラークリア処理は省略する */
		// nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}
#endif /* CB_FPE_CSW_DEBUG_ON */


/******************************************************************************/
/**
 * @brief			特徴点位置の推定(追加市松の縁用)
 *
 * @param[in]		nImgSrc					:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		pnImgWk					:,ワーク画面IDへのポインタ,-,[-],
 * @param[in]		ptSrchRgnInfo			:,処理領域構造体へのポインタ,-,[-],
 * @param[in,out]	ptSrchRsltAddPntInfo	:,探索結果点情報構造体へのポインタ,-,[-],
 * @param[in]		nCamDirection			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.27	S.Suzuki		新規作成
 * @date			2015.09.14	S.Morita		交点計算失敗時に、無効値を格納するよう修正
 *
 * @note			引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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

	CB_FPE_IMG_ACCESS_INFO		tEdgeCodeRho = { 0L, 0L, NULL, NULL, NULL, NULL };			/* 高精度位置決め用　エッジ強度画像(work画像1を利用) */
	CB_FPE_IMG_ACCESS_INFO		tEdgeCodeTheta = { 0L, 0L, NULL, NULL, NULL, NULL };		/* 高精度位置決め用　エッジ角度画像(work画像2を利用) */
	CB_FPE_IMG_ACCESS_INFO		tEdgeCodeAC = { 0L, 0L, NULL, NULL, NULL, NULL };		/* 高精度位置決め用　エッジ角度画像(work画像2を利用) */


	/* 引数チェック */
	if ( ( NULL == pnImgWk )  || ( NULL == ptSrchRgnInfo ) || ( NULL == ptSrchRsltAddPntInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
	ptEdgePntUL			= &(m_FPE_tWorkInfo.tEdgePntUL[0L]);
	ptEdgePntLR			= &(m_FPE_tWorkInfo.tEdgePntLR[0L]);
	ptEdgePrecPntUL		= &(m_FPE_tWorkInfo.tEdgePrecPntUL[0L]);
	ptEdgePrecPntLR		= &(m_FPE_tWorkInfo.tEdgePrecPntLR[0L]);


	/* 各特徴点の情報を登録 */
	ptPntArray[CB_ADD_PATTERN_PNT_POS_UPPER]		= &( ptSrchRsltAddPntInfo->tTPntUpper[0L] );
	ptPntArray[CB_ADD_PATTERN_PNT_POS_LOWER]		= &( ptSrchRsltAddPntInfo->tTPntLower[0L] );
	ptRsltPntArray[CB_ADD_PATTERN_PNT_POS_UPPER]	= &( ptSrchRsltAddPntInfo->tTPntUpperPrecise[0L] );
	ptRsltPntArray[CB_ADD_PATTERN_PNT_POS_LOWER]	= &( ptSrchRsltAddPntInfo->tTPntLowerPrecise[0L] );
	nNumPntArray[CB_ADD_PATTERN_PNT_POS_UPPER]		= ptSrchRsltAddPntInfo->nNumTPntUpper;
	nNumPntArray[CB_ADD_PATTERN_PNT_POS_LOWER]		= ptSrchRsltAddPntInfo->nNumTPntLower;


	/* エッジコード情報の抽出 */
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


	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgAccessInfo.nXSize ), &( tImgAccessInfo.nYSize ), (void**)&( tImgAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	/* エッジ強度・エッジ傾き画像のメモリ割り当て(画像メモリアクセス開始) */
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], &( tEdgeCodeRho.nXSize ), &( tEdgeCodeRho.nYSize ), (void**)&( tEdgeCodeRho.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], &( tEdgeCodeTheta.nXSize ), &( tEdgeCodeTheta.nYSize ), (void**)&( tEdgeCodeTheta.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL], &( tEdgeCodeAC.nXSize ), &( tEdgeCodeAC.nYSize ), (void**)&( tEdgeCodeAC.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}


	for ( ni = 0L; ni < CB_FPE_EDGE_POS_PATTERN_MAXNUM; ni++ )
	{
		/* 領域の設定方向 */
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

				/* スキャン方向の設定 */
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
					/* エラー */
					/* Openした画像メモリをCloseする */
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
					/* エラー */
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* エッジ点群抽出(垂直エッジ) */
//				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, ptPntArray[ni][nj], ptEdgePntUL, &nNumEdgePntUL, CB_FPE_LINE_FLT_VERT_EDGE, nDirX, nDirY );
				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, &tEdgeCodeAC, ptPntArray[ni][nj], ptEdgePntUL, &nNumEdgePntUL, CB_FPE_LINE_FLT_VERT_EDGE, nDirX, nDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 高精度位置決め(垂直エッジ) */
				nRet = cb_FPE_ExtractEdgePrecisePoint4AddChk( &tImgAccessInfo, ptEdgePntUL, nNumEdgePntUL, ptEdgePrecPntUL, &nNumEdgePrecPntUL, CB_FPE_LINE_FLT_VERT_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}


				/* エッジ点群抽出(水平エッジ) */
//				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, ptPntArray[ni][nj], ptEdgePntLR, &nNumEdgePntLR, CB_FPE_LINE_FLT_HORI_EDGE, nDirX, nDirY );
				nRet = cb_FPE_ExtractEdgePnt4AddChk( &tEdgeCodeRho, &tEdgeCodeTheta, &tEdgeCodeAC, ptPntArray[ni][nj], ptEdgePntLR, &nNumEdgePntLR, CB_FPE_LINE_FLT_HORI_EDGE, nDirX, nDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 高精度位置決め(水平エッジ) */
				nRet = cb_FPE_ExtractEdgePrecisePoint4AddChk( &tImgAccessInfo, ptEdgePntLR, nNumEdgePntLR, ptEdgePrecPntLR, &nNumEdgePrecPntLR, CB_FPE_LINE_FLT_HORI_EDGE, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}


				/* 水平/垂直の直線を近似し、交点を計算 */
				nRet = cb_FPE_CalcIntersectionPoint( ptEdgePrecPntUL, ptEdgePrecPntLR, nNumEdgePrecPntUL, nNumEdgePrecPntLR, &( ptRsltPntArray[ni][nj] ) );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}
				
				/* 交点計算が失敗しているときは、結果に無効値を格納する */
				if ( ptRsltPntArray[ni][nj].nFlagPrecise != CB_TRUE )
				{
					/* ピクセル精度の座標値に、無効値を設定する */
					( (ptPntArray[ni])[nj].nX ) = -1L;
					( (ptPntArray[ni])[nj].nY ) = -1L;
			
					/* サブピクセル精度の座標値に、無効値を設定する */
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
				/* ピクセル精度の座標値に、無効値を設定する */
				( (ptPntArray[ni])[nj].nX ) = -1L;
				( (ptPntArray[ni])[nj].nY ) = -1L;
		
				/* サブピクセル精度の座標値に、無効値を設定する */
				(ptRsltPntArray[ni])[nj].w = (double_t)( (ptPntArray[ni])[nj].nX );	//-1.0;
				(ptRsltPntArray[ni])[nj].h = (double_t)( (ptPntArray[ni])[nj].nY );	//-1.0;
				(ptRsltPntArray[ni])[nj].nFlagPrecise = CB_FALSE;
			}	/* if ( ( 0L <= (ptPntArray[ni])[nj].nX ) && ( 0L <= (ptPntArray[ni])[nj].nY ) ) */

			nDirX = nDirX * (-1L);
		}	/* for ( nj = 0; nj < nNumPntArray[ni]; nj++ ) */

	}	/* for ( ni = 0; ni < 2; ni++ ) */

	/* 画像メモリアクセスまとめて終了 */
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			エッジ点抽出
 *
 * @param[in]		ptImgRho				:,高精度位置決め用　エッジ強度画像へのポインタ,-,[-],
 * @param[in]		ptImgTheta				:,高精度位置決め用　エッジ角度画像へのポインタ,-,[-],
 * @param[in]		tTPnt					:,処理領域構造体へのポインタ,-,[-],
 * @param[out]		ptEdgePnt				:,高精度位置決め用　エッジ点配列,-,[-],
 * @param[out]		pnNumEdgePnt			:,高精度位置決め用　エッジ点数,-,[-]
 * @param[in]		nFltMode				:,Lineフィルタのモード,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE
 * @param[in]		nDirX					:,スキャン方向種別(X軸),CB_FPE_SCAN_DIR_X_MINUS<=value<=CB_FPE_SCAN_DIR_X_PLUS,[-],
 * @param[in]		nDirY					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.27	S.Suzuki	新規作成
 * @date			2015.09.07	S.Morita	縦エッジ（ピクセル精度）探索幅の変更
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
	tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
	tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
	tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */


	/* 引数チェック */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 領域チェック */
	if ( ( 0L > tTPnt.nX ) || ( 0L > tTPnt.nY ) )
	{
		*pnNumEdgePnt = 0L;
		return ( CB_IMG_OK );
	}

	if ( CB_FPE_LINE_FLT_HORI_EDGE == nFltMode )
	{
		nSrchAngleCode = -1L;
		/* 探索する角度コードを設定 */
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
			①T字点からシフトした点を探索
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( ptImgRho, ptImgTheta, ptImgAC, tTPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirX, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* 基準となる点が見つからない場合、結果なしとしてreturn */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			②探索した点から左に点群探索
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Hori( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nDirY, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			③探索した点から右に点群探索
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
		/* 探索する角度コードを設定 */
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
			①T字点からシフトした点を探索
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( ptImgRho, ptImgTheta, ptImgAC, tTPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* 基準となる点が見つからない場合、結果なしとしてreturn */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			②探索した点から上に点群探索
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Vert( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			③探索した点から下に点群探索
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
		/* ココに来るのは本来有り得ないパターン*/
		return ( CB_IMG_NG );
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			エッジ点抽出（T字点座標から一定量水平方向にシフトした注目点の座標値を取得
 *
 * @param[in]		ptImgRho				:,高精度位置決め用　エッジ強度画像へのポインタ,-,[-],
 * @param[in]		ptImgTheta				:,高精度位置決め用　エッジ角度画像へのポインタ,-,[-],
 * @param[in]		tTPnt					:,処理領域構造体へのポインタ,-,[-],
 * @param[out]		ptEdgePnt				:,高精度位置決め用　エッジ点配列,-,[-],
 * @param[out]		pnNumEdgePnt			:,高精度位置決め用　エッジ点数,-,[-]
 * @param[out]		nThrThetaMin			:,エッジ角度の最小閾値,-,[-]
 * @param[out]		nThrThetaMax			:,エッジ角度の最大閾値,-,[-]
 * @param[out]		pnThrRho				:,エッジ強度の閾値,-,[-]
 * @param[in]		nDirX					:,スキャン方向種別(X軸),CB_FPE_SCAN_DIR_X_MINUS<=value<=CB_FPE_SCAN_DIR_X_PLUS,[-],
 * @param[in]		nDirY					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.01.06	F.Sano	新規作成(関数cb_FPE_ExtractEdgePnt4AddChkを細分化) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta, const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
														    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
										   					slong* pnThrThetaMin, slong* pnThrThetaMax, slong* pnThrRho,
															slong nDirX, slong nDirY, slong nSrchAngleCode, slong nprm_HalfScanRgn)
{
	slong	ni;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* パラメータ */
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
	tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
	tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
	tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */


	/* 引数チェック */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt )
	  || ( NULL == pnThrThetaMin ) || ( NULL == pnThrThetaMax ) || ( NULL == pnThrRho )  )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_distanceSrchRgn	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_SRCH_BASE_PNT;
	nprm_thrEdgeFtrTPnt		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_EDGE_FTR_T_PNT;
	nprm_thrRangeTheta		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_RANGE_THETA;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	nTargetTheta = -999L;

	/*
	===================================
		①T字点からシフトした点を探索
	===================================
	*/
	/* T字点座標から一定量シフトした注目点の座標値 */
	nTargetX = tTPnt.nX + ( nDirX * nprm_distanceSrchRgn );
	nTargetY = tTPnt.nY;

	/* 注目点のアドレス */
	/* ptImgRhoもptImgThetaも、nXSize等の画面サイズ情報は同じだが、とりあえず使い分けておく */
	pnAddrRho = (uchar*)( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX;
	pnAddrTheta = (uchar*)( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX;
	pnAddrAC = (uchar*)( ptImgAC->pnAddrY8 ) + ( nTargetY * ptImgAC->nXSize ) + nTargetX;

	/* エッジ強度絶対値が最大の点を探索 */
	for ( ni = -nprm_HalfScanRgn; ni <= nprm_HalfScanRgn; ni++ )
	{
		/* 探索幅からアドレスを計算 */
		nTargetRho = (slong)*( pnAddrRho - ( ( nDirY * ni ) * ptImgRho->nXSize ) );
		nTargetAC = (slong)*( pnAddrAC - ( ( nDirY * ni ) * ptImgAC->nXSize ) );

		/* エッジ強度が最大となる画素の、座標・エッジ角度を保存し、エッジ点カウントをインクリメント */
		/* このエッジ角度が、以降の点群判定時の閾値となる */
		if (	( tMaxPntInfo.nFtrValue < nTargetRho )
			&&	( nTargetAC == nSrchAngleCode ) )
		{
			tMaxPntInfo.nFtrValue = nTargetRho;
			nTargetTheta = (slong)*(uchar*)( pnAddrTheta - ( ( nDirY * ni )  * ptImgTheta->nXSize ) );
			tMaxPntInfo.tPnt.nX = nTargetX;
			tMaxPntInfo.tPnt.nY = nTargetY - ( nDirY * ni );
		}
	}

	/* 信頼度チェック */
	if ( tMaxPntInfo.nFtrValue < nprm_thrEdgeFtrTPnt )
	{
		/* 基準となる点が見つからない場合、結果なしとしてreturn */
		ptEdgePnt[nNumEdgePnt].nX = -1L;
		ptEdgePnt[nNumEdgePnt].nY = -1L;
	}
	else
	{
		/* 基準となる点が見つかった場合、以降のエッジ点探索を続行 */
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
 * @brief			エッジ点抽出（T字点座標から一定量垂直方向にシフトした注目点の座標値を取得）
 *
 * @param[in]		ptImgRho				:,高精度位置決め用　エッジ強度画像へのポインタ,-,[-],
 * @param[in]		ptImgTheta				:,高精度位置決め用　エッジ角度画像へのポインタ,-,[-],
 * @param[in]		tTPnt					:,処理領域構造体へのポインタ,-,[-],
 * @param[out]		ptEdgePnt				:,高精度位置決め用　エッジ点配列,-,[-],
 * @param[out]		pnNumEdgePnt			:,高精度位置決め用　エッジ点数,-,[-]
 * @param[out]		nThrThetaMin			:,エッジ角度の最小閾値,-,[-]
 * @param[out]		nThrThetaMax			:,エッジ角度の最大閾値,-,[-]
 * @param[out]		pnThrRho				:,エッジ強度の閾値,-,[-]
 * @param[in]		nDirY					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.01.06	F.Sano	新規作成(関数cb_FPE_ExtractEdgePnt4AddChkを細分化) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,  const CB_FPE_IMG_ACCESS_INFO* const ptImgAC,
														    CB_IMG_POINT tTPnt, CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt,
															slong* pnThrThetaMin, slong* pnThrThetaMax, slong* pnThrRho, 
														slong nDirY, slong nSrchAngleCode, slong nprm_HalfScanRgn)
{
	slong	ni;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* パラメータ */
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
	tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
	tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
	tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */


	/* 引数チェック */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt )
	  || ( NULL == pnThrThetaMin ) || ( NULL == pnThrThetaMax ) || ( NULL == pnThrRho )  )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* パラメータ取得 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_distanceSrchRgn	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_SRCH_BASE_PNT;
	nprm_thrEdgeFtrTPnt		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_EDGE_FTR_T_PNT;
	nprm_thrRangeTheta		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_RANGE_THETA;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	nTargetTheta = -999L;

	/*
	===================================
		①T字点からシフトした点を探索
	===================================
	*/
	/* T字点座標から一定量シフトした注目点の座標値 */
	nTargetX = tTPnt.nX;
	nTargetY = tTPnt.nY + ( nDirY * nprm_distanceSrchRgn );

	/* 注目点のアドレス */
	/* ptImgRhoもptImgThetaも、nXSize等の画面サイズ情報は同じだが、とりあえず使い分けておく */
	pnAddrRho = (uchar*)( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX;
	pnAddrTheta = (uchar*)( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX;
	pnAddrAC = (uchar*)( ptImgAC->pnAddrY8 ) + ( nTargetY * ptImgAC->nXSize ) + nTargetX;

	/* エッジ強度絶対値が最大の点を探索 */
	for ( ni = -nprm_HalfScanRgn; ni <= nprm_HalfScanRgn; ni++ )
	{
		/* 探索幅からアドレスを計算 */
		nTargetRho = (slong)*( pnAddrRho + ni );
		nTargetAC = (slong)*( pnAddrAC + ni );

		/* エッジ強度が最大となる画素の、座標・エッジ角度を保存し、エッジ点カウントをインクリメント */
		/* このエッジ角度が、以降の点群判定時の閾値となる */
		if (	( tMaxPntInfo.nFtrValue < nTargetRho )
			&&	( nTargetAC == nSrchAngleCode ) )
		{
			tMaxPntInfo.nFtrValue = nTargetRho;
			nTargetTheta = (slong)*(uchar*)( pnAddrTheta + ni );
			tMaxPntInfo.tPnt.nX = nTargetX + ni;
			tMaxPntInfo.tPnt.nY = nTargetY;
		}
	}

	/* 信頼度チェック */
	if ( tMaxPntInfo.nFtrValue < nprm_thrEdgeFtrTPnt )
	{
		/* 基準となる点が見つからない場合、結果なしとしてreturn */
		ptEdgePnt[nNumEdgePnt].nX = -1L;
		ptEdgePnt[nNumEdgePnt].nY = -1L;
	}
	else
	{
		/* 基準となる点が見つかった場合、以降のエッジ点探索を続行 */
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
 * @brief			エッジ点抽出（水平方向探索用）
 *
 * @param[in]		ptImgRho				:,高精度位置決め用　エッジ強度画像へのポインタ,-,[-],
 * @param[in]		ptImgTheta				:,高精度位置決め用　エッジ角度画像へのポインタ,-,[-],
 * @param[out]		ptEdgePnt				:,高精度位置決め用　エッジ点配列,-,[-],
 * @param[out]		pnNumEdgePnt			:,高精度位置決め用　エッジ点数,-,[-]
 * @param[in]		nDirY					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nThrThetaMin			:,採用するエッジ角度範囲（最小値）,-,[-],
 * @param[in]		nThrThetaMax			:,採用するエッジ角度範囲（最大値）,-,[-],
 * @param[in]		nThrRho					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nDirectNum				:,探索方向（マイナス（左方向） or プラス(右方向)）,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.01.06	F.Sano	新規作成(関数cb_FPE_ExtractEdgePnt4AddChkを細分化) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_Search_Hori(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,
										    			CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt, slong nDirY, 
													slong nThrThetaMin, slong nThrThetaMax, slong nThrRho, slong nDirectNum, slong nprm_HalfScanRgn)
{
	slong	ni, nj;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* パラメータ */
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

	/* 引数チェック */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* エッジ点数を取得 */
	nNumEdgePnt = *pnNumEdgePnt;

	tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
	tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
	tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */

	/* パラメータ取得 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_MaxSearchRetry		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_MAX_RETRY;
	nprm_MaxScanDistance	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	/*
	====================================
		②探索した点から左(または右)に点群探索
		nDirectNumがマイナス値：：左方向
		nDirectNumがプラス値　：：右方向
	====================================
	*/

	nRetryNum = 0L;

	/* 探索点からマイナスまたはプラス方向にシフトした座標値 */
	nTargetX = ptEdgePnt[0].nX + nDirectNum;
	nTargetY = ptEdgePnt[0].nY;

	/* 【指定した方向へ探索】 */
	for( ni = 0L; ni < nprm_MaxScanDistance; ni ++ )
	{
		/* クリア */
		nTargetRho = 0L;
		nTargetTheta = -999L;	/* エッジコード結果としてはあり得ない値 */
		tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
		tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
		tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */


		for ( nj = -nprm_HalfScanRgn; nj <= nprm_HalfScanRgn; nj++ )
		{
			/* 注目点のアドレス */
			/* ptImgRhoもptImgThetaも、nXSize等の画面サイズ情報は同じだが、とりあえず使い分けておく */
			pnAddrRho = (uchar*)( ( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX );
			pnAddrTheta = (uchar*)( ( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX );

			/* エッジ強度を計算 */
			nTargetRho = (slong)*( pnAddrRho - ( ( nDirY * nj ) * ptImgRho->nXSize ) );
			nTargetTheta = (slong)*( pnAddrTheta - ( ( nDirY * nj ) * ptImgTheta->nXSize ) );

			/* エッジ角度が閾値範囲の画素のうち、最もエッジ強度が強い座標を配列に格納 */
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

		/* エッジ強度がしきい値を超える場合は採用 */
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
				/* リトライ回数上限までは、リトライ回数をインクリして隣のラインを探索 */
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
				/* リトライ上限を超えたら、探索終了 */
				break;
			}
		}
	}

	/* エッジ点数を更新 */
	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );

}


/******************************************************************************/
/**
 * @brief			エッジ点抽出（垂直方向探索用）
 *
 * @param[in]		ptImgRho				:,高精度位置決め用　エッジ強度画像へのポインタ,-,[-],
 * @param[in]		ptImgTheta				:,高精度位置決め用　エッジ角度画像へのポインタ,-,[-],
 * @param[out]		ptEdgePnt				:,高精度位置決め用　エッジ点配列,-,[-],
 * @param[out]		pnNumEdgePnt			:,高精度位置決め用　エッジ点数,-,[-]
 * @param[in]		nDirY					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nThrThetaMin			:,採用するエッジ角度範囲（最小値）,-,[-],
 * @param[in]		nThrThetaMax			:,採用するエッジ角度範囲（最大値）,-,[-],
 * @param[in]		nThrRho					:,スキャン方向種別(Y軸),CB_FPE_SCAN_DIR_Y_MINUS<=value<=CB_FPE_SCAN_DIR_Y_PLUS,[-],
 * @param[in]		nDirectNum				:,探索方向（マイナス（左方向） or プラス(右方向)）,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.01.06	F.Sano	新規作成(関数cb_FPE_ExtractEdgePnt4AddChkを細分化) 
 */
/******************************************************************************/
static slong cb_FPE_ExtractEdgePnt4AddChk_Search_Vert(	const CB_FPE_IMG_ACCESS_INFO* const ptImgRho, const CB_FPE_IMG_ACCESS_INFO* const ptImgTheta,
										    			CB_IMG_POINT* ptEdgePnt, slong* pnNumEdgePnt, slong nThrThetaMin, 
														slong nThrThetaMax, slong nThrRho, slong nDirectNum, slong nprm_HalfScanRgn)
{
	slong	ni, nj;
	CB_FPE_CAND_PNT_INFO	tMaxPntInfo;

	/* パラメータ */
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

	/* 引数チェック */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* エッジ点数を取得 */
	nNumEdgePnt = *pnNumEdgePnt;

	tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
	tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
	tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */

	/* パラメータ取得 */
/*	nprm_HalfScanRgn		= CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN;*/
	nprm_MaxSearchRetry		= CB_FPE_PRM_ADD_CHK_PRECISE_THR_MAX_RETRY;
	nprm_MaxScanDistance	= CB_FPE_PRM_ADD_CHK_PRECISE_DISTANCE_MAX_SCAN;
	nprm_coeffRho			= CB_FPE_PRM_ADD_CHK_PRECISE_COEFF_THR_EDGE;

	/*
	====================================
		②探索した点から上(または下)に点群探索
		nDirectNumがマイナス値：：上方向
		nDirectNumがプラス値　：：下方向
	====================================
	*/

	nRetryNum = 0L;

	/* 探索点からマイナスまたはプラス方向にシフトした座標値 */
	nTargetX = ptEdgePnt[0].nX;
	nTargetY = ptEdgePnt[0].nY + nDirectNum;

	/* 【指定した方向へ探索】 */
	for( ni = 0L; ni < nprm_MaxScanDistance; ni ++ )
	{
		/* クリア */
		nTargetRho = 0L;
		nTargetTheta = -999L;	/* エッジコード結果としてはあり得ない値 */
		tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
		tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
		tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */


		for ( nj = -nprm_HalfScanRgn; nj <= nprm_HalfScanRgn; nj++ )
		{
			/* 注目点のアドレス */
			/* ptImgRhoもptImgThetaも、nXSize等の画面サイズ情報は同じだが、とりあえず使い分けておく */
			pnAddrRho = (uchar*)( ptImgRho->pnAddrY8 ) + ( nTargetY * ptImgRho->nXSize ) + nTargetX;
			pnAddrTheta = (uchar*)( ptImgTheta->pnAddrY8 ) + ( nTargetY * ptImgTheta->nXSize ) + nTargetX;

			/* エッジ強度を計算 */
			nTargetRho = (slong)*( pnAddrRho + nj );
			nTargetTheta = (slong)*( pnAddrTheta + nj );

			/* エッジ角度が閾値範囲の画素のうち、最もエッジ強度が強い座標を配列に格納 */
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

		/* エッジ強度がしきい値を超える場合は採用 */
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
				/* リトライ回数上限までは、リトライ回数をインクリして隣のラインを探索 */
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
				/* リトライ上限を超えたら、探索終了 */
				break;
			}
		}
	}

	/* エッジ点数を更新 */
	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );

}


/******************************************************************************/
/**
 * @brief			重み付きHaar-like特徴量符号判定関数
 *
 * @param[in]		ptPatternInfo			:,特徴パターン情報構造体,-,[-],
 * @param[in]		nCamDirection			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 * @param[in,out]	nSignHaarLikeFeature	:,画像座標(十字位置)構造体へのポインタ,-,[-],
 *
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.27	F.Sano	新規作成
 * @date			2016.06.24	M.Ando	エンブレムによるケラレ対応
 *
 * @note			引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.11.27 Sano）
 *
 */
/******************************************************************************/
static slong cb_FPE_GetSignHaarLikeFeature( const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, slong nCamDirection, slong* nSignHaarLikeFeature)
{

	/* 引数チェック */
	if ( ( NULL == ptPatternInfo ) || ( NULL == nSignHaarLikeFeature ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 市松パターンのタイプ確認 */
	if ( ( ptPatternInfo->nTypePattern < CB_IMG_PATTERN_TYPE_NONE ) || ( CB_IMG_PATTERN_TYPE_CHKBOARD1x4 < ptPatternInfo->nTypePattern ) )
	{
		return ( CB_IMG_NG );
	}

	/* 市松パターンの向き確認 */
	if ( (ptPatternInfo->nFlagPlacement < CB_IMG_CHKBOARD_POS_PLACEMENT ) || (CB_IMG_CHKBOARD_NEG_PLACEMENT < ptPatternInfo->nFlagPlacement ) )
	{
		return ( CB_IMG_NG );
	}

	/* 市松パターンのアルゴリズム確認 */
	if ( (ptPatternInfo->nTypeAlgorithm < CB_IMG_ALGORITHM_DEFAULT ) || (CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM < ptPatternInfo->nTypeAlgorithm ) )
	{
		return ( CB_IMG_NG );
	}

	/* 符号判定テーブルを取得 */
	*nSignHaarLikeFeature = SignHaarLikeFeatureTable[ ptPatternInfo->nTypePattern ][ ptPatternInfo->nFlagPlacement ][ ptPatternInfo->nTypeAlgorithm ]; 

	/* リアカメラの場合、符号を反転させる */
	if( E_CB_SELFCALIB_CAMPOS_REAR == nCamDirection )
	{
		*nSignHaarLikeFeature *= CB_IMG_CHECKBOARD_SIGN_CHANGE;
	}
	else
	{
		/* フロント・サイドレフト・サイドライトの場合は特に処理なし */
		/* Not Operation */
	}

	return ( CB_IMG_OK );
}


/******************************************************************************/
/**
 * @brief			アルゴリズムタイプ判定関数
 *
 * @param[in]		ptPatternInfo			:,特徴パターン情報構造体,-,[-],
 * @param[in,out]	pnTypeAlgorithm			:,アルゴリズムタイプ格納先へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.01.07				K.Kato		新規作成
 * @date			2016.06.24				M.Ando		エンブレムによるケラレ対応
 */
/******************************************************************************/
static slong cb_FPE_GetTypeAlgorithm( const CB_FPE_IMG_PATTERN_INFO* const ptPatternInfo, enum enum_CB_IMG_ALGORITHM* pnTypeAlgorithm )
{

	/* 引数チェック */
	if ( ( NULL == ptPatternInfo ) || ( NULL == pnTypeAlgorithm ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 市松パターンのタイプ確認 */
	if ( ( ptPatternInfo->nTypePattern < CB_IMG_PATTERN_TYPE_NONE ) || ( CB_IMG_PATTERN_TYPE_CHKBOARD1x4 < ptPatternInfo->nTypePattern ) )
	{
		return ( CB_IMG_NG );
	}

	/* 市松パターンの向き確認 */
	if ( (ptPatternInfo->nFlagPlacement < CB_IMG_CHKBOARD_POS_PLACEMENT ) || (CB_IMG_CHKBOARD_NEG_PLACEMENT < ptPatternInfo->nFlagPlacement ) )
	{
		return ( CB_IMG_NG );
	}

	/* 市松パターンのアルゴリズム確認 */
	if ( (ptPatternInfo->nTypeAlgorithm < CB_IMG_ALGORITHM_DEFAULT ) || (CB_IMG_ALGORITHM_CHKBOARD_FTR_PNT_MAXNUM < ptPatternInfo->nTypeAlgorithm ) )
	{
		return ( CB_IMG_NG );
	}

	/* 符号判定テーブルを取得 */
	*pnTypeAlgorithm = m_FPE_nFtrPntPosTypeTbl4Chkboard[ ptPatternInfo->nTypePattern ][ ptPatternInfo->nFlagPlacement ][ ptPatternInfo->nTypeAlgorithm ]; 

	return ( CB_IMG_OK );
}

/******************************************************************************/
/**
 * @brief			エッジ点抽出(左右市松用高精度位置決め)
 *
 * @param[in]		ptImgSrc				:,ソース画像メモリアクセス情報へのポインタ,-,[-],
 * @param[in]		ptEdgePnt				:,エッジ位置へのポインタ,-,[-],
 * @param[in]		nNumEdgePnt				:,エッジの数,0<value,[-],
 * @param[out]		ptEdgePrecPnt			:,エッジ位置(高精度)へのポインタ,-,[-],
 * @param[in]		nFltMode				:,エッジ種別,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE,[-],
 * @param[in]		nScanDirX				:,スキャン方向(X方向),CB_FPE_SCAN_DIR_X_PLUS<=value<=CB_FPE_SCAN_DIR_X_MINUS,[-],
 * @param[in]		nScanDirY				:,スキャン方向(Y方向),CB_FPE_SCAN_DIR_Y_PLUS<=value<=CB_FPE_SCAN_DIR_Y_MINUS,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.03.21	K.Kato		新規作成
 * @date			2015.09.14	S.Morita	閾値決定の判定追加・高精度エッジ点取得処理のbreakによる処理を修正
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

	/* 引数チェック */
	if ( ( NULL == ptImgSrc ) || ( NULL == ptEdgePnt ) || ( NULL == ptEdgePrecPnt ) || ( NULL == pnNumEdgePrecPnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
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
			/* エラー */
			return ( CB_IMG_NG );
		}

		/* <<<<<しきい値決め>>>>> */
		/* 画素値の取得 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntX = ptEdgePnt[ni].nX;

			/* 黒領域側 */
//			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 6L );
			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 4L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[1] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}

			/* 白領域側 */
//			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 5L );
			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 3L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[2] */	// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O4.1  R-4, ID-6853
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}
		}

		/* 閾値決めのサンプル数増加対応 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntX = ptEdgePnt[ni].nX;

			/* 黒領域側 閾値決めのサンプル数増加対応 */
			nPntY = ptEdgePnt[ni].nY - ( nCoeffDirY * 3L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )
			{
				nTblIntensityBlack[ni + nNumEdgePnt] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}

			/* 白領域側 閾値決めのサンプル数増加対応　*/
			nPntY = ptEdgePnt[ni].nY + ( nCoeffDirY * 2L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )
			{
				nTblIntensityWhite[ni + nNumEdgePnt] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
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
		/* 代表値 */
		/* 閾値決めのサンプル数を2倍に増やしたため、設定する値を「nNumEdgePnt / 2L」から「nNumEdgePnt」に修正している */
		nIdxMedian = nNumEdgePnt;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];

		if(nIntensityBlack > nIntensityWhite)
		{
			/* 黒領域の輝度値が、白領域の輝度値よりも大きい場合にはエラーとする */
			return ( CB_IMG_NG );
		}
		
		/* しきい値 */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<サブピクセル推定>>>>> */
		nNumEdgePrecPnt = 0L;
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntYBlack = -1L;
			if ( nCoeffDirY < 0L )
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[1],[2]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirYの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
							/* 輝度差が大きい点を採用する */
							nPntYBlack = nj - ( nCoeffDirY * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;						
						}
					}
				}
			}
			else if ( 0L < nCoeffDirY )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6861	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6862
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[1],[2]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirYの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
							/* 輝度差が大きい点を採用する */
							nPntYBlack = nj - ( nCoeffDirY * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;	
						}
					}
				}
			}
			else
			{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6863
				/* エラー */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntYBlack )
			{
				nDistanceIntensity = nPixelValueDiffMax;
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrevMax;
				dDistanceIntensityWhite = (double_t)nPixelValueCurMax - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0割チェック追加 */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0割時の処理 */
					/* ここでの0割時の適切な処理については、要再検討 */
					/* 
					   => 下記割合は0.5とする。
					      nPixelValueCurとnPixelValuePrevは隣接しているので、同じ輝度値の場合は、その中間位置とする
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* 高精度 */
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
			/* エラー */
			return ( CB_IMG_NG );
		}

		/* <<<<<しきい値決め>>>>> */
		/* 画素値の取得 */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntY = ptEdgePnt[ni].nY;

			/* 黒領域側 */
//			nPntX = ptEdgePnt[ni].nX - ( nCoeffDirX * 6L );
			nPntX = ptEdgePnt[ni].nX - ( nCoeffDirX * 4L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[3] */
			{
				nTblIntensityBlack[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}

			/* 白領域側 */
//			nPntX = ptEdgePnt[ni].nX + ( nCoeffDirX * 5L );
			nPntX = ptEdgePnt[ni].nX + ( nCoeffDirX * 3L );
			if ( CB_TRUE == CB_FPE_CHK_OUTSIDE_RGN( nPntX, nPntY, 0L, 0L, ( ptImgSrc->nXSize - 1L ), ( ptImgSrc->nYSize - 1L ) ) )	/* 画像メモリ範囲外チェック[4] */	// MISRA-Cからの逸脱 [EntryAVM_QAC#4] O4.1  R-4, ID-6857
			{
				nTblIntensityWhite[ni] = (slong)(*( pnTopPixel + ( ( nPntY * ptImgSrc->nXSize ) + nPntX ) ));
			}
			else
			{
				/* エラー */
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
		/* 代表値 */
		nIdxMedian = nNumEdgePnt / 2L;
		nIntensityBlack = nTblIntensityBlack[nIdxMedian];
		nIntensityWhite = nTblIntensityWhite[nIdxMedian];

		if(nIntensityBlack > nIntensityWhite)
		{
			/* 黒領域の輝度値が、白領域の輝度値よりも大きい場合にはエラーとする */
			return ( CB_IMG_NG );
		}

		/* しきい値 */
		dThrIntensity = 0.5 * (double_t)( nIntensityWhite + nIntensityBlack );

		/* <<<<<サブピクセル推定>>>>> */
		for ( ni = 0L; ni < nNumEdgePnt; ni++ )
		{
			nPntXBlack = -1L;
			if ( nCoeffDirX < 0L )
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[3],[4]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirXの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
							/* 輝度差が大きい点を採用する */
							nPntXBlack = nj - ( nCoeffDirX * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;
						}
					}
				}
			}
			else if ( 0L < nCoeffDirX )		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6866	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O4.1  R-4, ID-6867
			{
				/*
					画像メモリ外チェックについては、『画像メモリ範囲外チェック[3],[4]』にてチェック済みのため本処理ではチェックしないこととする。
					ただし、nCoeffDirXの係数によってはチェック必要となるため、係数変更時にはメモリ外アクセスがおきないか確認すること。
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
							/* 輝度差が大きい点を採用する */
							nPntXBlack = nj - ( nCoeffDirX * 1L );
							nPixelValueDiffMax = nPixelValueCur - nPixelValuePrev;
							nPixelValueCurMax = nPixelValueCur;
							nPixelValuePrevMax = nPixelValuePrev;
						}
					}
				}
			}
			else
			{	// MISRA-Cからの逸脱 [EntryAVM_QAC#3] O1.1  R-52, ID-6868
				/* エラー */
				return ( CB_IMG_NG );
			}

			if ( 0L <= nPntXBlack )
			{
				nDistanceIntensity = nPixelValueDiffMax;
				dDistanceIntensityBlack = dThrIntensity - (double_t)nPixelValuePrevMax;
				dDistanceIntensityWhite = (double_t)nPixelValueCurMax - dThrIntensity;

				if ( 0L != nDistanceIntensity )	/* 0割チェック追加 */
				{
					dRatioBlack = dDistanceIntensityBlack / (double_t)nDistanceIntensity;
					dRatioWhite = dDistanceIntensityWhite / (double_t)nDistanceIntensity;
				}
				else
				{
					/* 0割時の処理 */
					/* ここでの0割時の適切な処理については、要再検討 */
					/* 
					   => 下記割合は0.5とする。
					      nPixelValueCurとnPixelValuePrevは隣接しているので、同じ輝度値の場合は、その中間位置とする
						  (2013.08.07 K.Kato)
					*/
					dRatioBlack = 0.5;
					dRatioWhite = 0.5;
				}

				nDistancePixel = 1L;

				/* 高精度 */
				ptEdgePrecPnt[nNumEdgePrecPnt].w = (double_t)nPntXBlack + ( (double_t)nCoeffDirX * ( ( dRatioBlack * (double_t)nDistancePixel ) /*- 1.0*/ ) );
				ptEdgePrecPnt[nNumEdgePrecPnt].h = (double_t)ptEdgePnt[ni].nY;

				nNumEdgePrecPnt++;
			}
		}
	}
	else
	{
		/* エラー */
		return ( CB_IMG_NG );
	}

	/* 高精度で求められた点数格納 */
	*pnNumEdgePrecPnt = nNumEdgePrecPnt;

	return ( CB_IMG_OK );
}




/******************************************************************************/
/**
 * @brief			エッジ点抽出
 *
 * @param[in]		ptImgRho				:,高精度位置決め用　エッジ強度画像へのポインタ,-,[-],
 * @param[in]		ptImgTheta				:,高精度位置決め用　エッジ角度画像へのポインタ,-,[-],
 * @param[in]		tCrossPnt				:,十字点座標構造体,-,[-],
 * @param[out]		ptEdgePnt				:,高精度位置決め用　エッジ点配列,-,[-],
 * @param[out]		pnNumEdgePnt			:,高精度位置決め用　エッジ点数,-,[-]
 * @param[in]		nFltMode				:,Lineフィルタのモード,CB_FPE_LINE_FLT_HORI_EDGE<=value<=CB_FPE_LINE_FLT_VERT_EDGE
 * @param[in]		nDirX					:,スキャン方向種別(X軸),-1<=value<=+1,[-],
 * @param[in]		nDirY					:,スキャン方向種別(Y軸),-1<=value<=+1,[-],
 * @param[in]		nSrchAngleCode			:,スキャン時に探索する角度コード,CB_FPE_EDGE_AC_RIGHT<=value<=CB_FPE_EDGE_AC_UPPER,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2013.11.27	S.Suzuki	新規作成
 *					2014.02.13	S.Suzuki		単体テスト指摘修正
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
	tMaxPntInfo.nFtrValue = 0L;		/* 無効値 */
	tMaxPntInfo.tPnt.nX = -1L;		/* 無効値 */
	tMaxPntInfo.tPnt.nY = -1L;		/* 無効値 */


	/* 引数チェック */
	if ( ( NULL == ptImgRho ) || ( NULL == ptImgTheta ) || ( NULL == ptImgAC ) || ( NULL == ptEdgePnt ) || ( NULL == pnNumEdgePnt ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 領域チェック */
	if ( ( 0L > tCrossPnt.nX ) || ( 0L > tCrossPnt.nY ) )
	{
		*pnNumEdgePnt = 0L;
		return ( CB_IMG_OK );
	}

	if ( CB_FPE_LINE_FLT_HORI_EDGE == nFltMode )
	{
		/*
		===================================
			①T字点からシフトした点を探索
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Hori( ptImgRho, ptImgTheta, ptImgAC, tCrossPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirX, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* 基準となる点が見つからない場合、結果なしとしてreturn */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			②探索した点から左に点群探索
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Hori( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nDirY, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN_UL);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			③探索した点から右に点群探索
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
			①T字点からシフトした点を探索
		===================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_SrchEdgePnt_Vert( ptImgRho, ptImgTheta, ptImgAC, tCrossPnt, ptEdgePnt, &nNumEdgePnt,
										   					  &nThrThetaMin, &nThrThetaMax, &nThrRho, nDirY, nSrchAngleCode, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return( CB_IMG_NG );
		}
		/* 基準となる点が見つからない場合、結果なしとしてreturn */
		if ( 0L == nNumEdgePnt ) {
			*pnNumEdgePnt = 0L;
			return( CB_IMG_OK );
		}
		/*
		====================================
			②探索した点から上に点群探索
		====================================
		*/
		nRet = cb_FPE_ExtractEdgePnt4AddChk_Search_Vert( ptImgRho, ptImgTheta, ptEdgePnt, &nNumEdgePnt, nThrThetaMin, nThrThetaMax, nThrRho, -1L, CB_FPE_PRM_ADD_CHK_PRECISE_SRCH_RGN);
		if ( CB_IMG_OK != nRet )
		{
			return(CB_IMG_NG);
		}
		/*
		====================================
			③探索した点から下に点群探索
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
		/* ココに来るのは本来有り得ないパターン*/
		return ( CB_IMG_NG );
	}

	*pnNumEdgePnt = nNumEdgePnt;

	return ( CB_IMG_OK );
}

/*@}*/



/******************************************************************************/
/**
 * @brief			特徴点位置の推定(追加市松の中央十字点用)
 *
 * @param[in]		nImgSrc					:,ソース画面ID,0<value<=IMPLIB_CFG_MAXIMGID,[-],
 * @param[in]		pnImgWk					:,ワーク画面IDへのポインタ,-,[-],
 * @param[in]		ptSrchRgnInfo			:,処理領域構造体へのポインタ,-,[-],
 * @param[in,out]	ptSrchRsltAddPntInfo	:,探索結果点情報構造体へのポインタ,-,[-],
 * @param[in]		nCamDirection			:,カメラ方向種別,E_CB_SELFCALIB_CAMPOS_FRONT<=value<=E_CB_SELFCALIB_CAMPOS_REAR,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.02.07	S.Suzuki		新規作成
 * @date			2015.09.14	S.Morita		交点計算失敗時に、無効値を格納するよう修正
 *
 * @note			引数nCamDirectionは、外部からの取得時に範囲チェックを行っているため、本関数内での範囲チェックは省略する(2013.07.31 Sano）
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


	CB_FPE_IMG_ACCESS_INFO					tEdgeCodeRho = { 0L, 0L, NULL, NULL, NULL, NULL };			/* 高精度位置決め用　エッジ強度画像(work画像1を利用) */
	CB_FPE_IMG_ACCESS_INFO					tEdgeCodeTheta = { 0L, 0L, NULL, NULL, NULL, NULL };		/* 高精度位置決め用　エッジ角度画像(work画像2を利用) */
	CB_FPE_IMG_ACCESS_INFO					tEdgeCodeAC = { 0L, 0L, NULL, NULL, NULL, NULL };		/* 高精度位置決め用　エッジ角度画像(work画像2を利用) */



	/* 引数チェック */
	if ( ( NULL == pnImgWk )  || ( NULL == ptSrchRgnInfo ) || ( NULL == ptSrchRsltAddPntInfo ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
	ptEdgePnt			= &(m_FPE_tWorkInfo.tEdgePntUL[0L]);
	ptEdgePrecPnt		= &(m_FPE_tWorkInfo.tEdgePrecPntUL[0L]);


	/* 各特徴点の情報を登録 */
	ptPntArray = &( ptSrchRsltAddPntInfo->tCrossPnt[0L] );
	ptRsltPntArray = &( ptSrchRsltAddPntInfo->tCrossPntPrecise[0L] );
	nNumPntArray = ptSrchRsltAddPntInfo->nNumCrossPnt;

	ptPatternInfo =  &(ptSrchRgnInfo->tPtnInfo );


	/* エッジコード情報の抽出 */
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

	/* 配列初期化 */
	for( ni = 0L; ni < CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS; ni++ )
	{
		tTotalEdgePrecPntUL[ni].w = -1.0;
		tTotalEdgePrecPntUL[ni].h = -1.0;
		tTotalEdgePrecPntUL[ni].nFlagPrecise = CB_FALSE;

		tTotalEdgePrecPntLR[ni].w = -1.0;
		tTotalEdgePrecPntLR[ni].h = -1.0;
		tTotalEdgePrecPntLR[ni].nFlagPrecise = CB_FALSE;
	}


	/* 画像メモリアクセス開始 */
	nRet = implib_OpenImgDirect( nImgSrc, &( tImgAccessInfo.nXSize ), &( tImgAccessInfo.nYSize ), (void**)&( tImgAccessInfo.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}
	/* エッジ強度・エッジ傾き画像のメモリ割り当て(画像メモリアクセス開始) */
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL], &( tEdgeCodeRho.nXSize ), &( tEdgeCodeRho.nYSize ), (void**)&( tEdgeCodeRho.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL], &( tEdgeCodeTheta.nXSize ), &( tEdgeCodeTheta.nYSize ), (void**)&( tEdgeCodeTheta.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}
	nRet = implib_OpenImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL], &( tEdgeCodeAC.nXSize ), &( tEdgeCodeAC.nYSize ), (void**)&( tEdgeCodeAC.pnAddrY8 ) );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG_IMPLIB );
	}


	/* 重み付きHaar-like特徴量符号の取得 */
	nRet = cb_FPE_GetSignHaarLikeFeature( ptPatternInfo, nCamDirection, &nSignHaarLikeFeature);
	if( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( nRet );
	}


	for ( ni = 0L; ni < CB_FPE_EDGE_POS_PATTERN_MAXNUM; ni++ )
	{
		/* 探索領域のシフト方向と探索向きの設定 */
		/* ①上下の垂直エッジ探索時
			 nDirX：探索向き(黒→白を正)
			 nDirY：探索領域シフト方向(上or下)
		   ②左右の水平エッジ探索時
			 nDirX：探索領域シフト方向(左or右)
			 nDirY：探索向き(黒→白を正) 
		   ...をそれぞれ表す */
		nDirX = m_FPE_nDirX4PreciseSrchRgnAddCenter[ni];
		nDirY = m_FPE_nDirY4PreciseSrchRgnAddCenter[ni];

		/* HaarLike特徴量の符号が負の時(画面上で左下：白の時) */
		if ( 0L > nSignHaarLikeFeature )
		{
			/* 切り替え不要 */
		}
		/* HaarLike特徴量の符号が正の時(画面上で左下：黒の時) */
		else if ( 0L < nSignHaarLikeFeature )
		{
			/* 上下の垂直エッジを探索する時には、X方向の探索向きが変わる */
			if		( ( CB_FPE_UPPER_EDGE_POS_PATTERN == ni )
				||	  ( CB_FPE_LOWER_EDGE_POS_PATTERN == ni ) )
			{
				nDirX = nDirX * ( -1L );
			}
			/* 左右の水平エッジを探索する時には、Y方向の探索向きが変わる */
			else	/* if	( ( CB_FPE_LEFT_EDGE_POS_PATTERN == ni )
						||	  ( CB_FPE_RIGHT_EDGE_POS_PATTERN == ni ) ) */
			{
				nDirY = nDirY * ( -1L );
			}
		}
		else
		{
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG );
		}


		/* スキャン方向の設定 */
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
			/* エラー */
			/* Openした画像メモリをCloseする */
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
			/* エラー */
			/* Openした画像メモリをCloseする */
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
			nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
			nRet = implib_CloseImgDirect( nImgSrc );
			return ( CB_IMG_NG );
		}

		/* 探索するエッジタイプの設定 */
		if		( ( CB_FPE_UPPER_EDGE_POS_PATTERN == ni )
			||	  ( CB_FPE_LOWER_EDGE_POS_PATTERN == ni ) )
		{
			nEdgeType = CB_FPE_LINE_FLT_VERT_EDGE;

			/* 垂直エッジ探索時の角度方向 */
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

			/* 水平エッジ探索時の角度方向 */
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

				/* エッジ点群抽出 */
				nRet = cb_FPE_ExtractEdgePnt4AddCross( &tEdgeCodeRho, &tEdgeCodeTheta, &tEdgeCodeAC, *ptPntArray, ptEdgePnt, &nNumEdgePnt, nEdgeType, nDirX, nDirY, nSrchAngleCode );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 高精度位置決め */
				nRet = cb_FPE_ExtractEdgePrecisePoint4AddChk( &tImgAccessInfo, ptEdgePnt, nNumEdgePnt, ptEdgePrecPnt, &nNumEdgePrecPnt, nEdgeType, nScanDirX, nScanDirY );
				if ( CB_IMG_OK != nRet )
				{
					/* Openした画像メモリをCloseする */
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
					nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
					nRet = implib_CloseImgDirect( nImgSrc );
					return ( CB_IMG_NG );
				}

				/* 水平/垂直方向毎に、まとめ配列に格納 */
				if( nEdgeType == CB_FPE_LINE_FLT_VERT_EDGE )
				{
					/* 高精度位置決めが出来た点群情報をまとめる */
					for( nk = 0L; nk < nNumEdgePrecPnt; nk++ )
					{
						tTotalEdgePrecPntUL[nTotalNumEdgePrecPntUL + nk].w = ptEdgePrecPnt[nk].w;
						tTotalEdgePrecPntUL[nTotalNumEdgePrecPntUL + nk].h = ptEdgePrecPnt[nk].h;
					}

					/* 点群数を加算 */
					nTotalNumEdgePrecPntUL += nNumEdgePrecPnt;
				}
				else	/*	 if ( ( nEdgeType == CB_FPE_LINE_FLT_HORI_EDGE ) */
				{
					/* 高精度位置決めが出来た点群情報をまとめる */
					for( nk = 0L; nk < nNumEdgePrecPnt; nk++ )
					{
						tTotalEdgePrecPntLR[nTotalNumEdgePrecPntLR + nk].w = ptEdgePrecPnt[nk].w;
						tTotalEdgePrecPntLR[nTotalNumEdgePrecPntLR + nk].h = ptEdgePrecPnt[nk].h;
					}

					/* 点群数を加算 */
					nTotalNumEdgePrecPntLR += nNumEdgePrecPnt;
				}
			}
			else
			{
				/* Not Operation */
			}
		} /* for ( nj = 0L; nj < nNumPntArray[ni]; nj++ ) */
	}

	/* 水平/垂直の直線を近似し、交点を計算 */
	nRet = cb_FPE_CalcIntersectionPoint( tTotalEdgePrecPntUL, tTotalEdgePrecPntLR, nTotalNumEdgePrecPntUL, nTotalNumEdgePrecPntLR, ptRsltPntArray );
	if ( CB_IMG_OK != nRet )
	{
		/* Openした画像メモリをCloseする */
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
		nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
		nRet = implib_CloseImgDirect( nImgSrc );
		return ( CB_IMG_NG );
	}
	
	/* 交点計算が失敗しているときは、無効値を格納する */
	if ( ptRsltPntArray->nFlagPrecise != CB_TRUE )
	{
		/* ピクセル精度の座標値に、無効値を設定する */
		ptPntArray->nX = -1L;
		ptPntArray->nY = -1L;
		
		/* サブピクセル精度の座標値に、無効値を設定する */
		ptRsltPntArray->w = (double_t)( ptPntArray->nX );
		ptRsltPntArray->h = (double_t)( ptPntArray->nY );
		ptRsltPntArray->nFlagPrecise = CB_FALSE;
	}
	else
	{
		/* Not Operation */
	}

	/* 画像メモリアクセスまとめて終了 */
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK03_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK02_NORMAL] );
	nRet = implib_CloseImgDirect( pnImgWk[CB_FPE_IMG_Y_WORK01_NORMAL] );
	nRet = implib_CloseImgDirect( nImgSrc );
	if ( 0L > nRet )
	{
		/* エラー情報クリア */
		nRet = implib_ClearIPError();
		return ( CB_IMG_NG_IMPLIB );
	}

	return ( CB_IMG_OK );
}







/******************************************************************************/
/**
 * @brief			高精度位置決め_近似直線の交点位置の算出
 *
 * @param[in]		ptEdgePrecPntUL			:,垂直エッジの構成点座標構造体へのポインタ,-,[-],
 * @param[in]		ptEdgePrecPntLR			:,水平エッジの構成点座標構造体へのポインタ,-,[-],
 * @param[in]		nNumEdgePrecPntUL		:,垂直エッジの構成点数,0<=value<=CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS,[-],
 * @param[in]		nNumEdgePrecPntLR		:,水平エッジの構成点数,0<=value<=CB_FPE_PRM_ADD_CHK_PRECISE_TOTAL_EDGE_PNT_NUM_CROSS,[-],
 * @param[out]		ptRsltPntArray			:,抽出結果構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG(_***)			:,異常終了,value=-1～-**,[-],
 *
 * @date			2014.02.07	S.Suzuki		新規作成
 *					2014.02.13	S.Suzuki		単体テスト指摘修正
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
	t_cb_img_CenterPos			tIntersectPnt = { -1.0, -1.0, CB_FALSE };	/* s.suzuki 無効値で初期化するように変更_20140213 */


	/* 引数チェック */
	if ( ( NULL == ptEdgePrecPntUL )  || ( NULL == ptEdgePrecPntLR ) || ( NULL == ptRsltPntArray ) )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 近時直線(垂直) */
	nFlgLinearRegUL = CB_FALSE;
	if ( 2L <= nNumEdgePrecPntUL )
	{
		nRet = cb_FPE_LinearRegression4VertLine( ptEdgePrecPntUL, nNumEdgePrecPntUL, &(tLineCoeffUL.dSlope), &(tLineCoeffUL.dIntercept) );
		if ( CB_IMG_OK == nRet )
		{
			if ( CB_FPE_DBL_MIN <= fabs( tLineCoeffUL.dSlope ) )	/* doubleデータのときは、fabs()で良いか要確認 */ /* s.suzuki 直値での判定を修正_20140213 */
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
			/* エラー */
			return ( CB_IMG_NG );
		}

		nFlgLinearRegUL = CB_TRUE;	/* 直線算出済みフラグを設定 */
	}

	/* 近時直線(水平) */
	nFlgLinearRegLR = CB_FALSE;
	if ( 2L <= nNumEdgePrecPntLR )
	{
		nRet = cb_FPE_LinearRegression4HoriLine( ptEdgePrecPntLR, nNumEdgePrecPntLR, &dTempSlope, &dTempIntercept );
		if ( CB_IMG_OK == nRet )
		{
			if ( CB_FPE_DBL_MIN <= fabs( dTempSlope ) )	/* doubleデータのときは、fabs()で良いか要確認 */
			{
				tLineCoeffLR.dSlope = 1.0 / dTempSlope;		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.1  , ID-6840
				tLineCoeffLR.dIntercept = -( dTempIntercept / dTempSlope );		// MISRA-Cからの逸脱 [EntryAVM_QAC#3] R3.2.1  , ID-6841
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
			/* エラー */
			return ( CB_IMG_NG );
		}

		nFlgLinearRegLR = CB_TRUE;	/* 直線算出済みフラグを設定 */
	}

	if ( ( CB_TRUE == nFlgLinearRegUL ) && ( CB_TRUE == nFlgLinearRegLR ) )	/* 直線が算出できた状態 */
	{

		/* 直線の交点 */
		if (   ( CB_FPE_LINE_SLOPE_HORI_TYPE == tLineCoeffUL.nTypeSlope ) /* 縦線を見つけたいのに横線 */ 
			|| ( CB_FPE_LINE_SLOPE_VERT_TYPE == tLineCoeffLR.nTypeSlope ) /* 横線を見つけたいのに縦線 */ )	
		{
			/* 結果格納 */
			ptRsltPntArray->w = -1.0;
			ptRsltPntArray->h = -1.0;
			ptRsltPntArray->nFlagPrecise = CB_FALSE;
		}
		else
		{
			if ( CB_FPE_LINE_SLOPE_DIAG_TYPE == tLineCoeffLR.nTypeSlope )	/* 左右側の直線が傾いている */
			{
				/* 交点計算 */
				nRet = cb_FPE_CalcIntersection( &tLineCoeffUL, &tLineCoeffLR, &tIntersectPnt );
				if ( CB_IMG_OK != nRet )
				{
					/* エラー */
					return ( CB_IMG_NG );
				}

				tIntersectPnt.nFlagPrecise = CB_TRUE;	/* s.suzuki 高精度OKフラグを成功時のみ立てるよう修正_20140213 */
			}
			else if ( CB_FPE_LINE_SLOPE_HORI_TYPE == tLineCoeffLR.nTypeSlope )	/* 左右側の直線が水平 */
			{
				tIntersectPnt.w = ( tLineCoeffUL.dSlope * ptEdgePrecPntLR[0].h ) + tLineCoeffUL.dIntercept;
				tIntersectPnt.h = ptEdgePrecPntLR[0].h;
				tIntersectPnt.nFlagPrecise = CB_TRUE;	/* s.suzuki 高精度OKフラグを成功時のみ立てるよう修正_20140213 */
			}
			else
			{
				/* エラー */
				return ( CB_IMG_NG );
			}

			/* 結果格納 */
			*ptRsltPntArray = tIntersectPnt;	/* s.suzuki 結果格納処理の位置を移動_20140213 */
		}
	}
	else
	{
		/* 結果格納 */
		ptRsltPntArray->w = -1.0;
		ptRsltPntArray->h = -1.0;
		ptRsltPntArray->nFlagPrecise = CB_FALSE;
	}	/* if ( ( CB_TRUE == nFlgLinearRegUL ) && ( CB_TRUE == nFlgLinearRegLR ) ) */

	return ( CB_IMG_OK );

}

/******************************************************************************/
/**
 * @brief			左右マーカーのピクセル精度座標値とサブピクセル精度座標値の直線距離を用いた判定
 *
 * @param[in, out]	ptSrchRsltInfo			:,特徴点抽出結果格納構造体へのポインタ,-,[-],
 *
 * @retval			CB_IMG_OK				:,正常終了,value=0,[-],
 * @retval			CB_IMG_NG_ARG_NULL		:,異常終了,value=-4,[-],
 *
 * @date			2015.09.14	S.Morita	新規作成
 *
 * @note			-
 *
 */
/******************************************************************************/
static slong cb_FPE_CalcDistCheck( CB_FPE_SRCH_RSLT_INFO* ptSrchRsltInfo )
{
	enum enum_CB_ADD_PATTERN_POS	ni;								/* ループカウンター */
	double_t						nxdist;							/* X方向距離 */
	double_t						nydist;							/* Y方向距離 */
	double_t						ndistStraight;					/* 直線距離 */
	CB_FPE_SRCH_RSLT_PNT_INFO		*ptSrchRsltAddChkPnt = NULL;	/* 探索結果点情報 */

	/* 引数チェック */
	if ( NULL == ptSrchRsltInfo )
	{
		return ( CB_IMG_NG_ARG_NULL );
	}

	/* 先頭ポインタ */
	ptSrchRsltAddChkPnt = &(ptSrchRsltInfo->tSrchRsltAddChkPntInfo[0]);

	for( ni = CB_ADD_PATTERN_POS_LEFT; ni < CB_ADD_PATTERN_POS_MAXNUM; ni++ )
	{
		/* 上T字点 */
		/* サブピクセル精度抽出成功時に判定を行う*/
		if( CB_TRUE == (ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].nFlagPrecise) )
		{
			nxdist = (ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].w) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nX);
			nydist = (ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].h) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nY);
			
			/* ピクセル精度特徴点とサブピクセル精度特徴点の直線距離を求める */
			ndistStraight = sqrt((nxdist * nxdist)+(nydist * nydist));
			
			if( CB_FPE_PRM_TOLERANCE_DIST <= ndistStraight )
			{
				/* 直線距離が許容値を超えた場合は、特徴点抽出結果に無効値を設定する */
				ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nX					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntUpper[0].nY					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].w				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].h				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntUpperPrecise[0].nFlagPrecise	= CB_FALSE;
			}
		}
		
		/* 下T字点 */
		/* サブピクセル精度抽出成功時に判定を行う*/
		if( CB_TRUE == (ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].nFlagPrecise) )
		{
			nxdist = (ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].w) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntLower[0].nX);
			nydist = (ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].h) - (double_t)(ptSrchRsltAddChkPnt[ni].tTPntLower[0].nY);
			
			/* ピクセル精度特徴点とサブピクセル精度特徴点の直線距離を求める */
			ndistStraight = sqrt((nxdist * nxdist)+(nydist * nydist));
			
			if( CB_FPE_PRM_TOLERANCE_DIST <= ndistStraight )
			{
				/* 直線距離が許容値を超えた場合は、特徴点抽出結果に無効値を設定する */
				ptSrchRsltAddChkPnt[ni].tTPntLower[0].nX					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntLower[0].nY					= -1L;
				ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].w				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].h				= -1.0;
				ptSrchRsltAddChkPnt[ni].tTPntLowerPrecise[0].nFlagPrecise	= CB_FALSE;
			}
		}
		
		/* 中央十字点 */
		/* サブピクセル精度抽出成功時に判定を行う*/
		if( CB_TRUE == (ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].nFlagPrecise) )
		{
			nxdist = (ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].w) - (double_t)(ptSrchRsltAddChkPnt[ni].tCrossPnt[0].nX);
			nydist = (ptSrchRsltAddChkPnt[ni].tCrossPntPrecise[0].h) - (double_t)(ptSrchRsltAddChkPnt[ni].tCrossPnt[0].nY);
			
			/* ピクセル精度特徴点とサブピクセル精度特徴点の直線距離を求める */
			ndistStraight = sqrt((nxdist * nxdist)+(nydist * nydist));
			
			if( CB_FPE_PRM_TOLERANCE_DIST <= ndistStraight )
			{
				/* 直線距離が許容値を超えた場合は、特徴点抽出結果に無効値を設定する */
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