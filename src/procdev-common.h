/*
    Copyright (C) 2011  EPFL (Ecole Polytechnique Fédérale de Lausanne)
    Nicolas Bourdaud <nicolas.bourdaud@epfl.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PROCDEV_COMMON_H
#define PROCDEV_COMMON_H 

#include "eegdev-common.h"
#include "eegdev-procdev.h"

#define PDEV_REPORT_ERROR		0
#define PDEV_SET_SAMLEN			1
#define PDEV_SET_INPUT_GROUPS		2
#define PDEV_UPDATE_CAPABILITIES	3
#define PDEV_CLOSE_DEVICE		4
#define PDEV_SET_CHANNEL_GROUPS		5
#define PDEV_START_ACQ			6
#define PDEV_STOP_ACQ			7
#define PDEV_FILL_CHINFO		8
#define PDEV_OPEN_DEVICE		9
#define PDEV_CLOSE_INTERFACE		10

#pragma pack(push, 1)
struct egd_procdev_chinfo {
	char label[EGD_LABEL_LEN];
	char unit[EGD_UNIT_LEN];
	char transducter[EGD_TRANSDUCTER_LEN];
	char prefiltering[EGD_PREFILTERING_LEN];
	int32_t isint;
	int32_t dtype;
	union gval min, max;
};

struct egd_procdev_caps {
	uint32_t sampling_freq;
	uint32_t type_nch[EGD_NUM_STYPE];
	uint32_t devtype_len, devid_len;
};
#pragma pack(pop)

// Definition of the file descriptor used by the child
#define PIPIN	STDIN_FILENO
#define PIPOUT	7 /*STDOUT_FILENO*/
#define PIPDATA	4

#endif //PROCDEV_COMMON_H
