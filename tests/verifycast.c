#include <../src/eegdev-common.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef	float	scaled_t;
unsigned int orignumch = 64;
unsigned int innumch = 72;
unsigned int chunklen = 72;
unsigned int inbuff_offset = 0;
#define NS	8192
#define NPOINT	(orignumch*NS)
#define INNPOINT	(innumch*NS)
#define NSEL	3



struct eegdev_operations ops = {.close_device = NULL};

void init_selch(struct selected_channels* selch, int nsel)
{
	int i, in_off = 0;
	unsigned int lench, remch = orignumch;

	for (i=0; i<nsel; i++) {
		lench = remch / (nsel-i);
		remch -= lench;
		selch[i].len = lench*sizeof(scaled_t);
		selch[i].in_offset = in_off+inbuff_offset*sizeof(scaled_t);
		selch[i].buff_offset=in_off;
		selch[i].cast_fn = get_cast_fn(EGD_FLOAT, EGD_FLOAT, 0);
		in_off += selch[i].len;
	}
}


void init_buffer(scaled_t* buffer)
{
	unsigned int ich,is;

	for (ich=0; ich<orignumch; ich++) {
		for (is=0; is<NS; is++) {
			buffer[ich+is*orignumch] = (is % 17)+ich*3;
		}
	}
}


void copy_buffers(scaled_t* restrict out, scaled_t* restrict in, unsigned int ns)
{
	unsigned int i;

	out += inbuff_offset;
	for (i=0; i<ns; i++) {
		memcpy(out, in, orignumch*sizeof(scaled_t));
		out += innumch;
		in += orignumch;
	}
}


int main(int argc, char* argv[])
{
	int opt, retval = 0;
	size_t len;
	unsigned int i;

	struct eegdev dev;
	scaled_t* inbuffer, *origbuffer;
	char* ref, *test;

	// Parse option
	while ((opt = getopt(argc, argv, "s:S:o:c:")) != -1) {
		if (opt == 's')
			orignumch = atoi(optarg);
		else if (opt == 'S')
			innumch = atoi(optarg);
		else if (opt == 'o')
			inbuff_offset = atoi(optarg);
		else if (opt == 'c')
			chunklen = atoi(optarg);
		else {
			fprintf(stderr, 
"Usage: verifycast [-s orignumch] [-S innumch]\n"
"                  [-o inbuff_offset] [-c chunklen]\n");
		   	exit(EXIT_FAILURE);
		}
	}
	


	origbuffer = malloc(NS*orignumch*sizeof(scaled_t));
	inbuffer = malloc(NS*innumch*sizeof(scaled_t));
	init_buffer(origbuffer);
	copy_buffers(inbuffer, origbuffer, NS);

	init_eegdev(&dev, &ops);
	dev.selch = malloc(NSEL*sizeof(*(dev.selch)));
	dev.nsel = NSEL;
	init_selch(dev.selch, NSEL);
	dev.acq = 1;
	dev.buffer = malloc(NS*orignumch*sizeof(scaled_t));
	dev.strides = NULL;
	dev.arrconf = NULL;
	dev.in_samlen = innumch*sizeof(scaled_t);
	dev.buff_samlen = orignumch*sizeof(scaled_t);
	dev.buffsize = NPOINT*sizeof(scaled_t);

	i = 0;
	while (i<INNPOINT) {
		len = (i + chunklen < INNPOINT) ? chunklen : INNPOINT-i;
		update_ringbuffer(&dev, inbuffer + i, len*sizeof(scaled_t));
		i+=chunklen;
	}

	ref = (char*)origbuffer;
	test = (char*)dev.buffer;
	for (i=0; i<NS; i++) {
		if (memcmp(ref, test, dev.buff_samlen)) {
			fprintf(stderr, "mismatch at sample %i\n", i);
			retval = 1;
			break;
		}
		ref += dev.buff_samlen;
		test += dev.buff_samlen;
	}

	destroy_eegdev(&dev);
	free(origbuffer);
	free(inbuffer);

	return retval;
}