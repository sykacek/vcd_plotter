#ifndef __PARAM_H__
#define __PARAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	char in[64];
	bool fd;		// use different fd than stdout
	char out[64];
	bool list;
	bool ok;
	bool signal;
	int x; // select only those signals
	int y;
	bool module;		// specify whole module
	int module_i;		// specify whole module
} param_t;

typedef enum {RES_OK, RES_FAILED} res_t;

void param_read(param_t *par, int argc, char **argv){
	for(int i = 1; i < argc; ++i){
		if(strcmp(argv[i], "-i") == 0){
			memset(par->in, 0x00, 64);
			memcpy(par->in, argv[i + 1], strlen(argv[i + 1]));

			par->ok = true;
			i++;
		}

		if(strcmp(argv[i], "-m") == 0){
			par->module_i = atoi(argv[i + 1]);

			par->module = true;
			i++;
		}

		if(strcmp(argv[i], "-s") == 0){
			par->x = atoi(argv[i + 1]);
			par->y = atoi(argv[i + 2]);

			par->signal = true;
			i += 2;
		}

		if(strcmp(argv[i], "-o") == 0){
			memset(par->out, 0x00, 64);
			memcpy(par->out, argv[i + 1], strlen(argv[i + 1]));
			par->fd = true;

			i++;
		}

		if(strcmp(argv[i], "-l") == 0){
			par->list = true;
			par->ok = true;
		}
	}
};

void param_help(){
	puts("Options:\n\t-i input_file\n\t-o output_file");
	puts("\t-l list available signals");
	puts("\t-m x plot whole module");
	puts("\t-s x y - specify signals");
}

void param_dump(param_t *par){
	printf("in: %s\nout: %s\n", par->in, par->out);
}

#endif //__PARAM_H__
