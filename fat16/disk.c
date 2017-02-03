#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include "disk.h"


/* ######################################################
   VARIÁVEIS LOCAIS
   ######################################################
*/

/* Guarda o file descriptor associado à disquete
   Esta variável é privada a este módulo (efeito de 'static')
*/
static int fd;


/* ######################################################
   VARIÁVEIS GLOBAIS
   ######################################################
*/

/* Guarda informação relevante do bbot sector
   Acrescente os novos campos que considerar necessários
   Esta variável é visível de outros módulos (basta incluir "floppy.h")
*/
boot_sector_t boot_sector;


/* ######################################################
   MACROS
   ######################################################
*/

/* Aborta o programa com a mensagem de error dada se a avaliação da
 * expressão for verdadeira
 */
#define TESTE(exp,msg)          \
if (exp) {                      \
    fprintf (stderr, "[%s (%d)]: ", __FILE__, __LINE__);  \
    perror (msg);               \
    exit (1);                   \
}


#define BYTES_INT_1(var,d)      (var[d])
#define BYTES_INT_2(var,d)      (var[d]) + 256 * (var[d+1])
#define BYTES_INT_4(var,d)      (var[d]) + 256 * (var[d+1]) + 65536 * (var[d+2]) + 16777216 * (var[d+3]) 


/* ######################################################
   FUNÇÕES
   ######################################################
*/

/* =================================================================
   Abre a floppy para leitura
*/
void openDisk (const char* device)
{
    unsigned char buffer[512];
    int err;
    
    fd = open (device, O_RDONLY);
    TESTE (fd < 0, "openDisk::open()");
    err = read (fd, buffer, 512);
    TESTE (err != 512, "openDisk::Reading master boot sector...");


    /* Extrair o tamanho do sector (deslocamentos LSB=0x0b e MSB=0x0c)
       LSB = Least Significant Byte
       MSB = Most Significant Byte
    */
    boot_sector.sector_size =           BYTES_INT_2 (buffer, 0x0b);
    boot_sector.sectores_por_cluster =  BYTES_INT_1 (buffer, 0x0d);
    boot_sector.sectores_reservados =   BYTES_INT_2 (buffer, 0x0e);
    boot_sector.n_fats =                BYTES_INT_1 (buffer, 0x10);
    boot_sector.n_entrdas_dir_raiz =    BYTES_INT_2 (buffer, 0x11);
    boot_sector.n_total_sectores =      BYTES_INT_2 (buffer, 0x13);
    if (boot_sector.n_total_sectores == 0)
      boot_sector.n_total_sectores =      BYTES_INT_4 (buffer, 0x20);
    boot_sector.n_sectores_por_fat =    BYTES_INT_2 (buffer, 0x16);
    boot_sector.sectores_escondidos =   BYTES_INT_4 (buffer, 0x1c);
}


/* =================================================================
   Fecha o acesso à floppy
*/
void closeDisk(void)
{
    int err = close (fd);
    TESTE (err < 0, "closeDisk::close()");
}

/* =================================================================
   Lê 'count' sectores, a partir do sector 'offset' (inclusive) para 'buf'
   É necessário que 'buf' tenha, pelo menos,
        'count * boot_sector.sector_size' bytes
*/
void readDisk (char *buf, int count, int offset)
{
    int r, n;

    r = lseek (fd, offset * boot_sector.sector_size, SEEK_SET);
    TESTE (r < 0, "readDisk::lseek()");

    n = read (fd, buf, count * boot_sector.sector_size);
    TESTE (n < count * boot_sector.sector_size, "readDisk::read()");
}
