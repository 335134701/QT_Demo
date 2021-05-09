/*==========================================================================================================
 * システム名  ：SH7766 標準化PF プロジェクト
 * ファイル名  ：CL_ver_ex.h
 *             ：
 * ファイル概要：OS部 メッセージ定義 ファイル 
 *             ：
 * 担当者      ：山内
 * 作成日      ：2012/11/20
 * 備考        ：
 *==========================================================================================================
 */
 /*_EDIT_
 *==========================================================================================================
 * ----------------------------------------------------------------------------------------------
 * 変更履歴
 * ----------------------------------------------------------------------------------------------
 * 日付       担当           機種                 変更TAG      履歴概要
 * ----------------------------------------------------------------------------------------------
 * 2012.11.10 K.YAMAUCHI    SH7766 標準PF          初版
 * 2012.12.25 Y.KODATE      EntryAVM								Software VersionをEntryAVM向 Ver00.14 に変更
 * 2013.05.13 (HIENG)SAKATA EntryAVM								機種ごとの情報が記載されているファイルをインクルード
 * 2021.03.09 xul           EntryAVM								日産水島工場向けのサイドカメラのキャリブエリア調整
 *==========================================================================================================
_EDIT_*/

#ifndef _CL_VER_H_
#define _CL_VER_H_

#include "CL_Ver_Parts_ex.h"

/* ======================================================== */
/*    バージョン及び部品番号                                */
/* ======================================================== */
/*--------------------------------------------------------
 *  Software Version
 *--------------------------------------------------------*/
#define		E_SOFTVER	0x0170U									// 2012年末までは上位バイトをAAとする。試作課に提出する際にVer.0000とする。
/*--------------------------------------------------------
 *  MainerVersion
 *--------------------------------------------------------*/
#define		E_MINORVER	0x00U									// マイナーバージョン(クラリオン社内調整用) 通常使用せず
/*--------------------------------------------------------
 *  Hardware Version
 *--------------------------------------------------------*/
#define		E_HARDVER	0x0000000000U 		  					// Ver00.00 ハードバージョン(量産時に書き込むが、ここでは初期値ゼロ固定を指定する)
/*--------------------------------------------------------
 *  Version情報の割り付けAddres
 *--------------------------------------------------------*/
#define	E_CMN_ROMADDR_OSD_START_ADDR			0x00F80000										/* OSD情報テーブル(部品番号及びバージョン等)			*/
#define	E_CMN_ROMADDR_MAP_START_ADDR			0x00500000										/* MAP情報テーブル(部品番号及びバージョン等)			*/
#define	E_CMN_ROMADDR_CAR_START_ADDR			0x004A0000										/* 車両パラメータテーブル(部品番号及びバージョン等)			*/
#define	E_CMN_ROMADDR_FACTORYCONFIG_START_ADDR	0x015A0000										/* 工場コンフィグ情報テーブル(部品番号及びバージョン等)	*/
#define	E_CMN_ROMADDR_OSD_INFO_TBL				(E_CMN_ROMADDR_OSD_START_ADDR+0x10)				/* OSD情報テーブル(部品番号及びバージョン等)			*/
#define	E_CMN_ROMADDR_MAP_INFO_TBL				(E_CMN_ROMADDR_MAP_START_ADDR+0x10)				/* MAP情報テーブル(部品番号及びバージョン等)			*/
#define	E_CMN_ROMADDR_CAR_INFO_TBL				(E_CMN_ROMADDR_CAR_START_ADDR+0x10)				/* 車両パラメータテーブル(部品番号及びバージョン等)			*/
#define	E_CMN_ROMADDR_FACTORYCONFIG_INFO_TBL	(E_CMN_ROMADDR_FACTORYCONFIG_START_ADDR+0x10)	/* 工場コンフィグ情報テーブル(部品番号及びバージョン等)	*/

/*--------------------------------------------------------
 *  ソフト・データ情報テーブル(soft,OSD,MAP,車両情報等)
 *--------------------------------------------------------*/
#define	E_CMN_LEN_INFO_TBL			48						/* ソフトウエア(データ)情報の長さ	64 -> 48へ				*/
#define	E_CMN_LEN_UNIT_PARTSNO		8						/* クラリオン機種番号 8Byte 例)EN3302PA 					*/
#define	E_CMN_LEN_SOFT_PARTSNO		5						/* ソフトウエア部品番号 5byte 例)470-2345-670, 485-9876-540	*/
#define	E_CMN_LEN_VER				2						/* ソフトバージョン 2byte HEXで表現する						*/
#define	E_CMN_LEN_MINOR				1						/* マイナーバージョン 1Byte HEXで表現する					*/
#define	E_CMN_LEN_CUSTOMER_PARTSNO	5						/* 顧客部番 												*/
#define	E_CMN_LEN_VEHICLE_REGION	16						/* 車種/仕向け												*/
#define	E_CMN_LEN_RESERVE			(E_CMN_LEN_INFO_TBL-(E_CMN_LEN_UNIT_PARTSNO+E_CMN_LEN_SOFT_PARTSNO+E_CMN_LEN_VER+E_CMN_LEN_MINOR+E_CMN_LEN_CUSTOMER_PARTSNO+E_CMN_LEN_VEHICLE_REGION))

typedef struct {
	unsigned char ucUnitPartsNo[E_CMN_LEN_UNIT_PARTSNO];			/* クラリオン機種番号 例)EN3302PA */
	unsigned char ucSoftPartsNo[E_CMN_LEN_SOFT_PARTSNO];			/* ソフトウエア部品番号(5byteHEX)		PA図面の順番に合わせる(部品番号が先) 470或いは485	*/
	unsigned char ucVer[E_CMN_LEN_VER];								/* バージョン番号(2byteHEX) PA図面の順番に合わせる(バージョン番号が後 / 評価版の場合には 上位が0xa～0xfとなる場合あり	*/
	unsigned char ucMinor;											/* マイナーバージョン(基本的に使用せず/クラリオン内社内調整用) 	*/
	unsigned char ucCustomerPartsNo[E_CMN_LEN_CUSTOMER_PARTSNO];	/* 顧客部番 													*/
	unsigned char ucVehicleRegion[E_CMN_LEN_VEHICLE_REGION];		/* 車種/仕向け													*/
	unsigned char ucReserve[E_CMN_LEN_RESERVE];						/* Reserve														*/
} SOFT_DATA_INFO_TBL;

/*--------------------------------------------------------
 *  Version情報取得関数
 *--------------------------------------------------------*/
signed short ver_SoftwareInfo_Get(SOFT_DATA_INFO_TBL *pReadBuff);

/*--------------------------------------------------------
 *  以下、機種ごとの情報(コンパイルスイッチで切り替える
 *--------------------------------------------------------*/
#define	E_CUSTOMER_PARTSNO	{ E_N_PARTNO_01,E_N_PARTNO_02,E_N_PARTNO_03,E_N_PARTNO_04,E_N_PARTNO_05			 }	/* 日産部番 */

#endif	/* _CL_VER_H_ */

