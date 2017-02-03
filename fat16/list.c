#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"

int startFAT(void) {
	int hs = 0, rs;
	hs = boot_sector.sectores_escondidos;	// numero de sectores escondidos
	rs = boot_sector.sectores_reservados;   // numero de sectores reservados
	return hs + rs;
}

int endFAT( int nFats ) {
	int i, spf;
	spf = boot_sector.n_sectores_por_fat;	// numero de sectores por FAT
	return startFAT() + (nFats * spf);
}

void printNonNullTerminatedInfo(char* name, char* ext, int size){
	for(int j = 0; j < 8; j++){
		if(name[j] != ' ')
			putchar(name[j]);
	}
	putchar(' ');
	for(int j = 0; j < 3; j++){
		if(ext[j] != ' ')
		putchar(ext[j]);
	}
	printf("\t%d\tclusters:", size);

}

void listDir(unsigned short* fat, directory_t* dir_entries) {
	int cluster;
	int endOfDirFlag = 0;
	char attr;
	int i = 0;

	while( i < boot_sector.n_entrdas_dir_raiz && !endOfDirFlag){
		attr = dir_entries[i].attr;
		if(dir_entries[i].name[0] != '\0'){ //if not end of dir
			if(!(attr & System == System)){ //if not a system file
				if(dir_entries[i].name[0] != 0xE5){ //if not marked for deletion
					if(dir_entries[i].start >= 0x0003){ //if first cluster is valid
						printNonNullTerminatedInfo(dir_entries[i].name, dir_entries[i].ext, dir_entries[i].size);
						cluster = dir_entries[i].start;
						while(cluster <= 0xFFF8){
							printf(" %d", cluster);
							cluster = *(fat + cluster);
						}
						printf("\n");
					}
				}
			}
		} else {
			endOfDirFlag = 1;
		}
		i++;
	}

 }

  unsigned short freeBlocks(unsigned short* fat, unsigned short size) {
  	int result = 0;

  	for(int i = 0; i < size; i++){
		if(fat[i] == 0x0000)
			result++;
	}
	return result;
  }


void readRoot(void) {

	// Ler FAT
	int n_fat_sectors = boot_sector.n_sectores_por_fat * boot_sector.n_fats;
	unsigned short* fat = (unsigned short *) malloc (n_fat_sectors * boot_sector.sector_size);
	readDisk((char *) fat, n_fat_sectors, startFAT() );

	// Ler directorio
	int n_rootdir_sectors = boot_sector.n_entrdas_dir_raiz * sizeof(directory_t) / boot_sector.sector_size;
	directory_t dir_entries[boot_sector.n_entrdas_dir_raiz];
	readDisk ((char*) dir_entries, n_rootdir_sectors, endFAT(boot_sector.n_fats) );

	// listar directorio
	listDir(fat, dir_entries);

	// Imprimir o numero de cluster livres

	unsigned short nfat_entries = ((boot_sector.n_total_sectores - (boot_sector.sectores_escondidos + n_rootdir_sectors + n_fat_sectors)) / boot_sector.sectores_por_cluster) -1;
	//-1 to account for the boot sector
	printf ("\nFree blocks: %d\n", freeBlocks(fat, nfat_entries));

	free(fat);
}


void printBoot(void) {
	printf("\n***** Bootsector *****\n");
	printf("sector_size -> %d\n", boot_sector.sector_size);
	printf("sectores_por_cluster -> %d\n", boot_sector.sectores_por_cluster);
        printf("sectores_reservados -> %d\n", boot_sector.sectores_reservados);
	printf("n_fats -> %d\n", boot_sector.n_fats);
	printf("n_entrdas_dir_raiz -> %d\n", boot_sector.n_entrdas_dir_raiz);
	printf("n_total_sectores -> %d\n", boot_sector.n_total_sectores);
	printf("n_sectores_por_fat -> %d\n", boot_sector.n_sectores_por_fat);
	printf("sectores_escondidos -> %d\n", boot_sector.sectores_escondidos);
	printf("**********************\n\n");
}


int main(int argc, char* argv[]) {

	if (argc < 2 || (argc ==3 &&  strcmp(argv[2], "-b"))) {
		printf ("usage: %s image [-b]\n", argv[0]);
		return 1;
	}

	/* Leitura do BootSector */
	openDisk(argv[1]); // inicia o disco, preenche os valores do bootSector
	if (argc == 3) {
		printBoot(); // imprime o bootSector
	}
	readRoot();
	closeDisk();
	return 0;
}
