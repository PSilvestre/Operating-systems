#ifndef __DISK_H
#define __DISK_H

#define ReadOnly	0x01 /* 00000001 */
#define Hidden		0x02 /* 00000010 */
#define System		0x04 /* 00000100 */
#define VolumeLabel	0x08 /* 00001000 */
#define Directory	0x10 /* 00010000 */
#define Archive		0x20 /* 00100000 */


#define BS(f) boot_sector.f

	


/* Guarda informação relevante do bbot sector
   Acrescente os novos campos que considerar necessários */
typedef struct boot_sector_s {
    unsigned char name[8];  
    unsigned short sector_size;			    /* 2 bytes */
	unsigned char sectores_por_cluster;		/* 1 byte */
	unsigned short sectores_reservados;		/* 2 bytes */
	unsigned char n_fats;					/* 1 byte */
	unsigned short n_entrdas_dir_raiz;		/* 2 bytes */
	unsigned int n_total_sectores;		   /* 2 or 4 bytes */
	unsigned short n_sectores_por_fat;		/* 2 bytes */
	unsigned int sectores_escondidos;		/* 4 bytes */
} boot_sector_t;




typedef struct directory_s {      // Short 8.3 names
    unsigned char name[8];        // file name
    unsigned char ext[3];         // file extension
    unsigned char attr;           // attribute byte
    unsigned char lcase;          // Case for base and extension
    unsigned char ctime_ms;       // Creation time, milliseconds
    unsigned short ctime;		  // Creation time
    unsigned short cdate;         // Creation date
    unsigned short adate;         // Last access date
    unsigned short reserved;      // reserved values (ignored)
    unsigned short time;          // time stamp
    unsigned short date;          // date stamp
    unsigned short start;         // starting cluster number
    unsigned int size;            // size of the file
} directory_t;

extern boot_sector_t boot_sector;

void openDisk (const char*);

void closeDisk (void);

void readDisk (char *buf, int count, int offset);
	
#endif /* __DISK_H */
