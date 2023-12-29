#ifndef DISK_IMAGE_H
#define DISK_IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#define N_DBLOCKS 10 
#define N_IBLOCKS 4  
#define BLOCKSIZE 

#define N_INODES

extern  char *outfile_name;
// extern diskImage *defragged;
extern int no_inodes;
extern int blocksize;
FILE* temp_output_file;
// int no_blocks=0;
int filesize;
int data_offset;
int inode_offset;
// Superblock structure
struct superblock {
    int blocksize;      /* size of blocks in bytes */
    int inode_offset;   /* offset of inode region in blocks */
    int data_offset;    /* data region offset in blocks */
    int swap_offset;    /* swap region offset in blocks */
    int free_inode;     /* head of free inode list */
    int free_block;     /* head of free block list */
};

// Inode structure
struct inode {
    int next_inode;         /* list for free inodes */
    int protect;            /* protection field */
    int nlink;              /* Number of links to this file */
    int size;               /* Number of bytes in file */
    int uid;                /* Owner's user ID */
    int gid;                /* Owner's group ID */
    int ctime;              /* Time field */
    int mtime;              /* Time field */
    int atime;              /* Time field */
    int dblocks[N_DBLOCKS]; /* Pointers to data blocks */
    int iblocks[N_IBLOCKS]; /* Pointers to indirect blocks */
    int i2block;            /* Pointer to doubly indirect block */
    int i3block;            /* Pointer to triply indirect block */
};

// #define BLOCKSIZE  
// 
typedef struct {
    char data[512];
} dataBlock;

#undef INODESIZE
#define INODESIZE sizeof(struct inode)

// Disk image structure
typedef struct {
    struct superblock *sb;   /* Superblock */
    struct inode *inodes;   /* Inode array */
    dataBlock *data_blocks;       /* Data blocks array */
    char* inode_buffer;
    char* sb_buffer;
    char* data_buffer;
    char* defrag_image;
    char* frag_image;
} diskImage;

// Function prototypes
void readDiskImage(FILE *file, diskImage *di);
diskImage *defragDiskImage(diskImage *input);
void writeDiskImage(FILE *file, const diskImage *di);
void freeDiskImage(diskImage *di);

#endif /* DISK_IMAGE_H */
