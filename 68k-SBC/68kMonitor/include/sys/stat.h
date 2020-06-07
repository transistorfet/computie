
#ifndef _STAT_H
#define _STAT_H


#define S_IFBLK  060000		/* block special */
#define S_IFDIR  040000  	/* directory */
#define S_IFCHR  020000		/* character special */
#define S_IFIFO  010000		/* this is a FIFO */
#define S_ISUID  004000		/* set user id on execution */
#define S_ISGID  002000		/* set group id on execution */
#define S_ISVTX   01000		/* stick bit */

#define S_IRWXU   00700		/* owner:  rwx------ */
#define S_IRUSR   00400		/* owner:  r-------- */
#define S_IWUSR   00200		/* owner:  -w------- */
#define S_IXUSR   00100		/* owner:  --x------ */

#define S_IRWXG   00070		/* group:  ---rwx--- */
#define S_IRGRP   00040		/* group:  ---r----- */
#define S_IWGRP   00020		/* group:  ----w---- */
#define S_IXGRP   00010		/* group:  -----x--- */

#define S_IRWXO   00007		/* others: ------rwx */
#define S_IROTH   00004		/* others: ------r-- */ 
#define S_IWOTH   00002		/* others: -------w- */
#define S_IXOTH   00001		/* others: --------x */


#endif
