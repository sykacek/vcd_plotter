#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static struct stat st = {0};


#include "vcd.h"

/* 
 * VCD_SIG_T FUNCTIONS
*/

static const char *keyword[] = {"$date\n", "$version\n", "$timescale\n"};
static const size_t offset[] = {0, 64, 128, 192, 192};

vcd_sig_t *vcd_sig_init(void){
	vcd_sig_t *ret = (vcd_sig_t *)calloc(1, sizeof(vcd_sig_t));

	return ret;
}

// resize values array
static void vcd_sig_resize(vcd_sig_t *sig, int vlen){
	for(int i = 0; i < sig->count; ++i){
		sig->val[i] = (char *)realloc(sig->val[i], sizeof(char) * vlen);
	}
}

void vcd_sig_update_values(vcd_sig_t *sig, char *val, size_t len, float time){
	if(len != sig->count){
#ifdef DEBUG
		fprintf(stderr, "len and count do not match! %ld %d, %s %s\n", len, sig->count, val, sig->name);
#endif
	} else {
		sig->vlen++;
		vcd_sig_resize(sig, sig->vlen);
		for(int i = 0; i < sig->count; ++i){
			if(sig->vlen > 1)
				sig->val[i][sig->vlen - 1] = sig->val[i][sig->vlen - 2];
			else
				sig->val[i][sig->vlen - 1] = '0';
		}

		//update time
		sig->time = realloc(sig->time, sizeof(float) * sig->vlen);
		sig->time[sig->vlen - 1] = time - 0.01f;

		sig->vlen++;
		vcd_sig_resize(sig, sig->vlen);
		for(int i = 0; i < sig->count; ++i)
			sig->val[i][sig->vlen - 1] = val[i];

		//update time
		sig->time = realloc(sig->time, sizeof(int) * sig->vlen);
		sig->time[sig->vlen - 1] = time;
	}
}

void vcd_sig_free(vcd_sig_t **sig){
	if(*sig && sig){
		for(int i = 0; i < (*sig)->count; ++i){
			if((*sig)->val[i])
				free((*sig)->val[i]);
		}

		free((*sig)->val);
		free((*sig)->time);
	}

	free(*sig);

	*sig = NULL;
}

void vcd_sig_dump(vcd_sig_t *sig){
	printf("\ttype      %s\n", sig->type);
	printf("\tcount     %d\n", sig->count);
	printf("\tid        %c\n", sig->id);
	printf("\tname      %s\n", sig->name);
	printf("\trecords   %ld\n", sig->vlen);

	puts("");
}

void vcd_sig_fprint(vcd_sig_t *sig, FILE * fd){
	fprintf(fd, "%s\n", sig->name);
	for(int j = 0; j < sig->count; ++j){
		for(int i = 0; i < sig->vlen; ++i)
			fprintf(fd, "    %c", sig->val[j][i]);

		fprintf(fd, "\n");
	}

	for(int i = 0; i < sig->vlen; ++i)
		fprintf(fd, " %4d", (int)sig->time[i]);

	fprintf(fd, "\n");
}

/* 
 * VCD_MOD_T FUNCTIONS
*/

vcd_mod_t *vcd_mod_init(const char *name){
	vcd_mod_t *ret = (vcd_mod_t *)calloc(1, sizeof(vcd_mod_t));
	memcpy(ret->id, name, strlen(name) > 63 ? 63 : strlen(name));

	return ret;
}

void vcd_mod_free(vcd_mod_t **mod){
	if(*mod && mod){
		for(int i = 0; i < (*mod)->len; ++i)
			vcd_sig_free(&(*mod)->sig[i]);
	}

	free((*mod)->sig);
	free(*mod);
	*mod = NULL;
}

void vcd_mod_dump(vcd_mod_t *mod){
	printf("module name %s\n", mod->id);
	puts("Module signals:");

	for(int i = 0; i < mod->len; ++i)
		printf("\t%s\n", mod->sig[i]->name);
}

void vcd_mod_dump_rec(vcd_mod_t *mod){
	vcd_mod_dump(mod);

	puts("\nMore detail:\n");

	for(int i = 0; i < mod->len; ++i){
		printf("Signal %d\n", i);
		vcd_sig_dump(mod->sig[i]);
	}
}

void vcd_mod_fprint_short(vcd_mod_t *mod, FILE *fd){
	for(int i = 0; i < mod->len; ++i)
		vcd_sig_fprint(mod->sig[i], fd);
}

void vcd_mod_fprint(vcd_mod_t *mod){
	vcd_sig_t *sig = NULL;
	//FILE *fd = NULL;
	char fname[128] = {0};
	if (stat(".data", &st) == -1) {
		mkdir(".data", 0744);
	}


	for(int i = 0; i < mod->len; ++i){
		sig = mod->sig[i];
		memset(fname, 0x00, 128);
		sprintf(fname, ".data/%s.txt", sig->name);

		FILE *fd = fopen(fname, "w");
		fprintf(fd, "#time");
		for(int l = 0; l < sig->count; ++l)
			fprintf(fd, "\t%d", l);

		fprintf(fd, "\n");

		// Z translates to -1
		for(int k = 0; k < sig->vlen; ++k){
			fprintf(fd, "%.2f", sig->time[k]);
			for(int l = 0; l < sig->count; ++l){
				if(sig->val[l][k] == 'z'){
					fprintf(fd, "\t%d", -1 + STEP * i);
				} else
					fprintf(fd, "\t%d", sig->val[l][k] - 0x30 + STEP * i);
			}

			fprintf(fd, "\n");
		}

		fclose(fd);
	}
}

/* 
 * VCD_T FUNCTIONS
*/

vcd_t *vcd_read(char *file){
	FILE *fd = fopen(file, "r");
	if(fd == NULL){
		fprintf(stderr, "Error: file %s does not exist!\n", file);
		return NULL;
	}

	vcd_t *ret = (vcd_t *)calloc(1, sizeof(vcd_t));
	memcpy(ret->file, file, strlen(file) > 64 ? 63 : strlen(file));

	size_t n;
	char *line = NULL;
	char mod[32] = {0};
	int now;

	vcd_mod_t *mod_p = NULL;
	vcd_sig_t *sig_p = NULL;
	while(getline(&line, &n, fd) > -1){
		// get keywords
		for(int i = 0; i < 3; ++i){
			if(strcmp(keyword[i], line) == 0){
				getline(&line, &n, fd);
				memcpy((char *)ret + offset[i], line, strlen(line) > 64 ? 63 : strlen(line) - 1);
				getline(&line, &n, fd);
			}
		}

		// ADD MODULE  TO VCD
		if(strncmp(line, "$scope module", 13) == 0){
			memset(mod, 0x00, 32);
			memcpy(mod, line + 14, strlen(line) - 20);
			//puts(mod);

			ret->len++;
			ret->mod = (vcd_mod_t **)realloc(ret->mod, sizeof(vcd_mod_t *) * ret->len);
			ret->mod[ret->len - 1] = vcd_mod_init(mod);
			mod_p = ret->mod[ret->len - 1];
		} else

		// ADD SIGNAL TO MODULE
		if(strncmp(line, "$var", 4) == 0){
			mod_p->len++;
			mod_p->sig = (vcd_sig_t **)realloc(mod_p->sig, sizeof(vcd_sig_t *) * mod_p->len);
			mod_p->sig[mod_p->len - 1] = vcd_sig_init();

			sig_p = mod_p->sig[mod_p->len - 1];

			char empty[16];
			sscanf(line, "%s %s %d %c %s", empty, sig_p->type, &(sig_p->count), &(sig_p->id), sig_p->name);
			sig_p->val = (char **)calloc(1, sizeof(char *) * sig_p->count);

		} else

		// skip
		if(strncmp(line, "$upscope", 8) == 0){
			;
		} else

		// skip
		if(strncmp(line, "$enddefinitions", 15) == 0){
			;
		} else

		// skip
		if(strncmp(line, "$comment", 8) == 0){
			;
		} else

		// skip
		if(strncmp(line, "$end", 4) == 0){
			;
		} else

		// skip
		if(strncmp(line, "$scope begin", 12) == 0){
			;
		} else

		// skip for now
		if(strncmp(line, "$dumpall", 8) == 0){
			while(strncmp(line, "$end", 4) != 0)
				getline(&line, &n, fd);
		} else

		// skip for now
		if(strncmp(line, "$dumpvars", 9) == 0){
			//while(strncmp(line, "$end", 4) != 0){
				//getline(&line, &n, fd);
				//printf("%s", line);
			//}
		} 

		// time update
		else if(line[0] == '#'){
			memset(mod, 0x00, 32);
			memcpy(mod, line + 1, strlen(line) - 1);
			now = atoi(mod);
		}

		// value update
		else {
			// TODO better bus signals
			if(line[0] == 'b'){
				char val[64] = {0};
				char id;
				sscanf(line, "%s %c", val, &id);

				vcd_update_sig(ret, id, val + 1, strlen(val) - 1, (float)now);
			} else {
				vcd_update_sig(ret, line[1], line, 1, now);
			}

			//printf("%s", line);
		}
	}

	if(line)
		free(line);
	fclose(fd);

	return ret;
}

void vcd_free(vcd_t **vcd){
	if(vcd && *vcd){
		for(int i = 0; i < (*vcd)->len; ++i)
			vcd_mod_free(&(*vcd)->mod[i]);

		free((*vcd)->mod);
		free(*vcd);

		*vcd = NULL;
	}
}

void vcd_dump(vcd_t *vcd){
	printf("**** VCD **** \n");
	printf("date      %s\n", vcd->date);
	printf("version   %s\n", vcd->version);
	printf("timescale %s\n", vcd->timescale);

	puts("Modules:");
	for(int i = 0; i < vcd->len; ++i){
		printf("module %s\n", vcd->mod[i]->id);
	}
}

void vcd_dump_rec(vcd_t *vcd){
	vcd_dump(vcd);
	for(int i = 0; i < vcd->len; ++i)
		vcd_mod_dump_rec(vcd->mod[i]);
}

void vcd_print_list(vcd_t *vcd){
	vcd_mod_t *mod = NULL;
	vcd_sig_t *sig = NULL;

	for(int i = 0; i < vcd->len; ++i){
		mod = vcd->mod[i];
		printf("Module  %s\n", mod->id);

		for(int j = 0; j < mod->len; ++j){
			sig = mod->sig[j];
			printf("    %d %d: %s\n", i, j, sig->name);

		}

		puts("");
	}

}

void vcd_update_sig(vcd_t *vcd, char c, char *val, size_t n, float time){
	vcd_mod_t *mod = NULL;
	vcd_sig_t *sig = NULL;

	// find signal which corresponds to c
	for(int i = 0; i < vcd->len; ++i){
		mod = vcd->mod[i];

		for(int j = 0; j < mod->len; ++j){
			sig = mod->sig[j];

			if(c == sig->id){
				vcd_sig_update_values(sig, val, n, time);
			}
		}
	}
}
