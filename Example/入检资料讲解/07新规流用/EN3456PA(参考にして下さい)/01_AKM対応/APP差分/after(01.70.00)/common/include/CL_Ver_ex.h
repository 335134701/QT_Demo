/*==========================================================================================================
 * �V�X�e����  �FSH7766 �W����PF �v���W�F�N�g
 * �t�@�C����  �FCL_ver_ex.h
 *             �F
 * �t�@�C���T�v�FOS�� ���b�Z�[�W��` �t�@�C�� 
 *             �F
 * �S����      �F�R��
 * �쐬��      �F2012/11/20
 * ���l        �F
 *==========================================================================================================
 */
 /*_EDIT_
 *==========================================================================================================
 * ----------------------------------------------------------------------------------------------
 * �ύX����
 * ----------------------------------------------------------------------------------------------
 * ���t       �S��           �@��                 �ύXTAG      �����T�v
 * ----------------------------------------------------------------------------------------------
 * 2012.11.10 K.YAMAUCHI    SH7766 �W��PF          ����
 * 2012.12.25 Y.KODATE      EntryAVM								Software Version��EntryAVM�� Ver00.14 �ɕύX
 * 2013.05.13 (HIENG)SAKATA EntryAVM								�@�킲�Ƃ̏�񂪋L�ڂ���Ă���t�@�C�����C���N���[�h
 * 2021.03.09 xul           EntryAVM								���Y�����H������̃T�C�h�J�����̃L�����u�G���A����
 *==========================================================================================================
_EDIT_*/

#ifndef _CL_VER_H_
#define _CL_VER_H_

#include "CL_Ver_Parts_ex.h"

/* ======================================================== */
/*    �o�[�W�����y�ѕ��i�ԍ�                                */
/* ======================================================== */
/*--------------------------------------------------------
 *  Software Version
 *--------------------------------------------------------*/
#define		E_SOFTVER	0x0170U									// 2012�N���܂ł͏�ʃo�C�g��AA�Ƃ���B����ۂɒ�o����ۂ�Ver.0000�Ƃ���B
/*--------------------------------------------------------
 *  MainerVersion
 *--------------------------------------------------------*/
#define		E_MINORVER	0x00U									// �}�C�i�[�o�[�W����(�N�����I���Г������p) �ʏ�g�p����
/*--------------------------------------------------------
 *  Hardware Version
 *--------------------------------------------------------*/
#define		E_HARDVER	0x0000000000U 		  					// Ver00.00 �n�[�h�o�[�W����(�ʎY���ɏ������ނ��A�����ł͏����l�[���Œ���w�肷��)
/*--------------------------------------------------------
 *  Version���̊���t��Addres
 *--------------------------------------------------------*/
#define	E_CMN_ROMADDR_OSD_START_ADDR			0x00F80000										/* OSD���e�[�u��(���i�ԍ��y�уo�[�W������)			*/
#define	E_CMN_ROMADDR_MAP_START_ADDR			0x00500000										/* MAP���e�[�u��(���i�ԍ��y�уo�[�W������)			*/
#define	E_CMN_ROMADDR_CAR_START_ADDR			0x004A0000										/* �ԗ��p�����[�^�e�[�u��(���i�ԍ��y�уo�[�W������)			*/
#define	E_CMN_ROMADDR_FACTORYCONFIG_START_ADDR	0x015A0000										/* �H��R���t�B�O���e�[�u��(���i�ԍ��y�уo�[�W������)	*/
#define	E_CMN_ROMADDR_OSD_INFO_TBL				(E_CMN_ROMADDR_OSD_START_ADDR+0x10)				/* OSD���e�[�u��(���i�ԍ��y�уo�[�W������)			*/
#define	E_CMN_ROMADDR_MAP_INFO_TBL				(E_CMN_ROMADDR_MAP_START_ADDR+0x10)				/* MAP���e�[�u��(���i�ԍ��y�уo�[�W������)			*/
#define	E_CMN_ROMADDR_CAR_INFO_TBL				(E_CMN_ROMADDR_CAR_START_ADDR+0x10)				/* �ԗ��p�����[�^�e�[�u��(���i�ԍ��y�уo�[�W������)			*/
#define	E_CMN_ROMADDR_FACTORYCONFIG_INFO_TBL	(E_CMN_ROMADDR_FACTORYCONFIG_START_ADDR+0x10)	/* �H��R���t�B�O���e�[�u��(���i�ԍ��y�уo�[�W������)	*/

/*--------------------------------------------------------
 *  �\�t�g�E�f�[�^���e�[�u��(soft,OSD,MAP,�ԗ����)
 *--------------------------------------------------------*/
#define	E_CMN_LEN_INFO_TBL			48						/* �\�t�g�E�G�A(�f�[�^)���̒���	64 -> 48��				*/
#define	E_CMN_LEN_UNIT_PARTSNO		8						/* �N�����I���@��ԍ� 8Byte ��)EN3302PA 					*/
#define	E_CMN_LEN_SOFT_PARTSNO		5						/* �\�t�g�E�G�A���i�ԍ� 5byte ��)470-2345-670, 485-9876-540	*/
#define	E_CMN_LEN_VER				2						/* �\�t�g�o�[�W���� 2byte HEX�ŕ\������						*/
#define	E_CMN_LEN_MINOR				1						/* �}�C�i�[�o�[�W���� 1Byte HEX�ŕ\������					*/
#define	E_CMN_LEN_CUSTOMER_PARTSNO	5						/* �ڋq���� 												*/
#define	E_CMN_LEN_VEHICLE_REGION	16						/* �Ԏ�/�d����												*/
#define	E_CMN_LEN_RESERVE			(E_CMN_LEN_INFO_TBL-(E_CMN_LEN_UNIT_PARTSNO+E_CMN_LEN_SOFT_PARTSNO+E_CMN_LEN_VER+E_CMN_LEN_MINOR+E_CMN_LEN_CUSTOMER_PARTSNO+E_CMN_LEN_VEHICLE_REGION))

typedef struct {
	unsigned char ucUnitPartsNo[E_CMN_LEN_UNIT_PARTSNO];			/* �N�����I���@��ԍ� ��)EN3302PA */
	unsigned char ucSoftPartsNo[E_CMN_LEN_SOFT_PARTSNO];			/* �\�t�g�E�G�A���i�ԍ�(5byteHEX)		PA�}�ʂ̏��Ԃɍ��킹��(���i�ԍ�����) 470������485	*/
	unsigned char ucVer[E_CMN_LEN_VER];								/* �o�[�W�����ԍ�(2byteHEX) PA�}�ʂ̏��Ԃɍ��킹��(�o�[�W�����ԍ����� / �]���ł̏ꍇ�ɂ� ��ʂ�0xa�`0xf�ƂȂ�ꍇ����	*/
	unsigned char ucMinor;											/* �}�C�i�[�o�[�W����(��{�I�Ɏg�p����/�N�����I�����Г������p) 	*/
	unsigned char ucCustomerPartsNo[E_CMN_LEN_CUSTOMER_PARTSNO];	/* �ڋq���� 													*/
	unsigned char ucVehicleRegion[E_CMN_LEN_VEHICLE_REGION];		/* �Ԏ�/�d����													*/
	unsigned char ucReserve[E_CMN_LEN_RESERVE];						/* Reserve														*/
} SOFT_DATA_INFO_TBL;

/*--------------------------------------------------------
 *  Version���擾�֐�
 *--------------------------------------------------------*/
signed short ver_SoftwareInfo_Get(SOFT_DATA_INFO_TBL *pReadBuff);

/*--------------------------------------------------------
 *  �ȉ��A�@�킲�Ƃ̏��(�R���p�C���X�C�b�`�Ő؂�ւ���
 *--------------------------------------------------------*/
#define	E_CUSTOMER_PARTSNO	{ E_N_PARTNO_01,E_N_PARTNO_02,E_N_PARTNO_03,E_N_PARTNO_04,E_N_PARTNO_05			 }	/* ���Y���� */

#endif	/* _CL_VER_H_ */

