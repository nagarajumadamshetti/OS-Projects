/**
 * @file disk_image.c
 * @brief Implementation of functions related to disk image manipulation.
 */

#include "disk_image.h"

int blocksize=512;
int no_inodes=10;
void *input_buffer;
void *new_inodes;
int no_blocks=0;
int bs;
int print_true=0;
int swap_offset=0;

/**
 * @brief Prints the contents of the inodes.
 * 
 * @param nodes The pointer to the inodes.
 */
void printInodes(void *nodes) {
    printf("\nInode contents: \n");
    int i = 0;
    for (i = 0; i < no_inodes; i++){
        struct inode* node = (struct inode *)((char*) nodes + i * sizeof(struct inode));
        printf("next_inode: %d\n", node->next_inode);
        printf("protect:    %d\n", node->protect);
        printf("nlink:      %d\n", node->nlink);
        printf("size:       %d\n", node->size);
        printf("uid:        %d\n", node->uid);
        printf("gid:        %d\n", node->gid);
        printf("ctime:      %d\n", node->ctime);
        printf("mtime:      %d\n", node->mtime);
        printf("atime:      %d\n", node->atime);
        printf("dblocks:    ");
        int j;
        for (j = 0; j < N_DBLOCKS; ++j) {
            printf("%d ", node->dblocks[j]);
        }
        printf("\n");
        printf("iblocks:    ");
        for (j = 0; j < N_IBLOCKS; ++j) {
            printf("%d ", node->iblocks[j]);
        }
        printf("\n");
        printf("i2block:    %d\n", node->i2block);
        printf("i3block:    %d\n", node->i3block);
    }
    perror("22");
    return;
}


/**
 * @brief Prints the contents of a single inode.
 * 
 * @param node The pointer to the inode.
 */
void printInode(struct inode *node) {
    printf("\nInode contents: \n");
    printf("next_inode: %d\n", node->next_inode);
    printf("protect:    %d\n", node->protect);
    printf("nlink:      %d\n", node->nlink);
    printf("size:       %d\n", node->size);
    printf("uid:        %d\n", node->uid);
    printf("gid:        %d\n", node->gid);
    printf("ctime:      %d\n", node->ctime);
    printf("mtime:      %d\n", node->mtime);
    printf("atime:      %d\n", node->atime);
    printf("dblocks:    ");
    int j;
    for (j = 0; j < N_DBLOCKS; ++j) {
        printf("%d ", node->dblocks[j]);
    }
    printf("\n");
    printf("iblocks:    ");
    
    for (j= 0; j < N_IBLOCKS; ++j) {
        printf("%d ", node->iblocks[j]);
    }
    printf("\n");
    printf("i2block:    %d\n", node->i2block);
    printf("i3block:    %d\n", node->i3block);
    return;
}

/**
 * @brief Prints the contents of the superblock.
 * 
 * @param sb The pointer to the superblock.
 */
void printSuperBlock(struct superblock *sb){
    printf("\nSuperblock contents: \n");
    printf("Block size: %d\n", sb->blocksize);
    printf("Inode offset: %d\n", sb->inode_offset);
    printf("Data offset: %d\n", sb->data_offset);
    printf("Swap offset: %d\n", sb->swap_offset);
    printf("Free inode: %d\n", sb->free_inode);
    printf("Free block: %d\n", sb->free_block);
}


/**
 * @brief Reads the disk image from a file.
 * 
 * @param file The file pointer to the disk image file.
 * @param di The diskImage structure to store the read data.
 */
void readDiskImage(FILE *file, diskImage *di) {
    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    input_buffer = malloc(filesize);
    fread(input_buffer, 1, filesize, file);
    temp_output_file = fopen(outfile_name, "w+");
    di->sb = (struct superblock*) (input_buffer + 512);
    bs = di->sb->blocksize;

    // printSuperBlock(di->sb);

    data_offset = di->sb->data_offset;
    inode_offset = di->sb->inode_offset;
    swap_offset = di->sb->swap_offset;

    fseek(temp_output_file, 1024 + bs * data_offset, SEEK_SET);

    no_inodes = (data_offset - inode_offset) * bs / sizeof(struct inode);

    new_inodes = malloc(sizeof(struct inode) * no_inodes);

    memcpy(new_inodes, input_buffer + 1024 + bs * inode_offset, sizeof(struct inode) * no_inodes);
}

/**
 * @brief Reads the data from a specific block in the disk image.
 * 
 * @param input_buffer The input_buffer containing the disk image data.
 * @param blockNumber The block number to read.
 * @param outputBuffer The input_buffer to store the read data.
 */
void readBlockData(void *input_buffer, int blockNumber, char *outputBuffer) {
    size_t offset = 1024 + bs * blockNumber;
    memcpy(outputBuffer, input_buffer + offset, bs);
}

/**
 * @brief Writes the index block to the disk image.
 * 
 * @param start The starting index of the data blocks.
 * @param end The ending index of the data blocks.
 * @param index The index of the index block.
 */
void updateIBlock(int start, int end,int index){
    int indices[bs/4];
    int j = 0;
    int i=start;
    int k;
    for (k = 0; k < bs && i<=end ; k += 4) {
        int* ptr = (int*)(input_buffer + 1024 + (bs * data_offset) + (bs * index) + k);

        if (*ptr == -1) {
            indices[j] = -1;
        } else {
            indices[j] = start + (k / 4);
        }
        i++;
        j++;
    }
    fwrite(indices, 1, bs, temp_output_file);
    fseek(temp_output_file, 0, SEEK_END);
}

void handleDirectBlocks(struct inode *node, struct inode *temp, int *remainingBlockSize) {
    int i;
    for (i = 0; i < N_DBLOCKS && node->dblocks[i] != -1; i++) {
        if (*remainingBlockSize <= 0)
            break;

        //write data block to output file
        void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * node->dblocks[i]);
        fwrite(block, 1, bs, temp_output_file);

        temp->dblocks[i] = no_blocks;
        no_blocks++;
        *remainingBlockSize = *remainingBlockSize - bs;
    }
}

/**
 * @brief Handles the indirect blocks of an inode.
 * 
 * @param node The pointer to the inode.
 * @param temp The pointer to the temporary inode.
 * @param remainingBlockSize The remaining data size to be written.
 */
void handleIndirectBlocks(struct inode *node, struct inode *temp, int *remainingBlockSize) {
    int i;
    for (i = 0; i < N_IBLOCKS; i++) {
        if (*remainingBlockSize <= 0)
            break;
        int ival = node->iblocks[i];
        updateIBlock(no_blocks + 1, no_blocks + bs / 4, ival);
        temp->iblocks[i] = no_blocks;
        no_blocks++;
        int j;
        for (j = 0; j < bs; j += 4) {
            if (*remainingBlockSize <= 0)
                break;
            void *dadd = input_buffer + 1024 + bs * data_offset + bs * ival + j;
            int dval = *((int *)dadd);

            //write data block to output file
            void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * dval);
            fwrite(block, 1, bs, temp_output_file);

            no_blocks++;
            *remainingBlockSize = *remainingBlockSize - bs;
        }
    }
}

/**
 * @brief Handles the double indirect blocks of an inode.
 * 
 * @param node The pointer to the inode.
 * @param temp The pointer to the temporary inode.
 * @param remainingBlockSize The remaining data size to be written.
 */
void handleDoubleIndirectBlocks(struct inode *node, struct inode *temp, int *remainingBlockSize) {
    int i2val = node->i2block;

    //write i2block to output file
    void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * i2val);
    fwrite(block, 1, bs, temp_output_file);

    temp->i2block = no_blocks;
    no_blocks++;

    int iblock_tracker[bs / 4];
    memset(iblock_tracker, -1, sizeof(iblock_tracker));

    int i;
    for (i = 0; i < bs; i += 4) {
        if (*remainingBlockSize <= 0)
            break;

        void *iadd = input_buffer + 1024 + (bs * data_offset) + (bs * i2val) + i;
        int ival = *((int *)iadd);
        updateIBlock(no_blocks + 1, no_blocks + bs / 4, ival);

        iblock_tracker[i / 4] = no_blocks;
        no_blocks++;

        int j;
        for (j = 0; j < bs; j += 4) {
            if (*remainingBlockSize <= 0)
                break;

            void *dadd = input_buffer + 1024 + bs * data_offset + bs * ival + j;
            int dval = *((int *)dadd);

            //write data block to output file
            void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * dval);
            fwrite(block, 1, bs, temp_output_file);

            no_blocks++;
            *remainingBlockSize = *remainingBlockSize - bs;
        }
    }
    int offset=1024 + bs * data_offset + bs * temp->i2block;
    fseek(temp_output_file, offset, SEEK_SET);
    fwrite(iblock_tracker, 1, bs, temp_output_file);
    fseek(temp_output_file, 0, SEEK_END);
}

/**
 * @brief Handles the triple indirect blocks of an inode.
 * 
 * @param node The pointer to the inode.
 * @param temp The pointer to the temporary inode.
 * @param remainingBlockSize The remaining data size to be written.
 */
void handleTripleIndirectBlocks(struct inode *node, struct inode *temp, int *remainingBlockSize) {
    int i3val = node->i3block;

    //write i3block to output file
    void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * i3val);
    fwrite(block, 1, bs, temp_output_file);

    temp->i3block = no_blocks;
    no_blocks++;
    int i2block_tracker[bs/4];
    memset(i2block_tracker, -1, sizeof(i2block_tracker));

    int k;
    for(k=0;k<bs;k+=4){
        if (*remainingBlockSize <= 0)
            break;

        void *i2add = input_buffer + 1024 + (bs * data_offset) + (bs * i3val) + k;
        int i2val = *((int *)i2add);

        //write i2block to output file

        void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * i2val);
        fwrite(block, 1, bs, temp_output_file);

        int temp_i2val = no_blocks;
        i2block_tracker[k/4]=no_blocks;
        no_blocks++;
        int iblock_tracker[bs / 4];
        memset(iblock_tracker, -1, sizeof(iblock_tracker));

        int i;
        for (i = 0; i < bs; i += 4) {
            if (*remainingBlockSize <= 0)
                break;

            void *iadd = input_buffer + 1024 + (bs * data_offset) + (bs * i2val) + i;
            int ival = *((int *)iadd);
            updateIBlock(no_blocks + 1, no_blocks + bs / 4, ival);

            iblock_tracker[i / 4] = no_blocks;
            no_blocks++;

            int j;
            for (j = 0; j < bs; j += 4) {
                if (*remainingBlockSize <= 0)
                    break;

                void *dadd = input_buffer + 1024 + bs * data_offset + bs * ival + j;
                int dval = *((int *)dadd);

                //write block to output file
                void *block = input_buffer + 1024 + ( bs * data_offset ) + (bs * dval);
                fwrite(block, 1, bs, temp_output_file);

                no_blocks++;
                *remainingBlockSize = *remainingBlockSize - bs;
            }
        }
        int offset=1024 + bs * data_offset + bs * temp_i2val;
        fseek(temp_output_file, offset, SEEK_SET);
        fwrite(iblock_tracker, 1, bs, temp_output_file);
        fseek(temp_output_file, 0, SEEK_END);
    }
    int offset=1024 + bs * data_offset + bs * temp->i3block;
    fseek(temp_output_file, offset, SEEK_SET);
    fwrite(i2block_tracker, 1, bs, temp_output_file);
    fseek(temp_output_file, 0, SEEK_END);
}


/**
 * @brief Defragments the disk image by rearranging the inodes and data blocks.
 * 
 * @param input The input diskImage structure.
 * @return The defragmented diskImage structure.
 */
diskImage *defragDiskImage(diskImage *input) {
    fseek(temp_output_file, 0, SEEK_SET);

    fwrite(input_buffer, 1, 1024, temp_output_file);

    fseek(temp_output_file, 0, SEEK_END);
    
    int i; 
    for (i= 0; i < no_inodes; i++){
        struct inode* primary_inode = (struct inode *)(input_buffer + 1024 + (bs * input->sb->inode_offset) + (i * sizeof(struct inode)));

        if(primary_inode->nlink == 1){
            int offset = 1024 + (bs * input->sb->data_offset) + (bs * no_blocks);
            fseek(temp_output_file, offset, SEEK_SET);
            struct inode* secondary_inode = (struct inode*)(new_inodes + i * sizeof(struct inode));

            int remainingBlockSize = primary_inode->size;
            handleDirectBlocks(primary_inode, secondary_inode, &remainingBlockSize);
                if(remainingBlockSize>0){
                    handleIndirectBlocks(primary_inode, secondary_inode, &remainingBlockSize);
                    if(remainingBlockSize>0){
                        handleDoubleIndirectBlocks(primary_inode, secondary_inode, &remainingBlockSize);
                        if(remainingBlockSize>0){
                            handleTripleIndirectBlocks(primary_inode, secondary_inode, &remainingBlockSize);
                        }
                    }
                }

            // writing inodes
            long add = 1024 + bs * inode_offset + i * sizeof(struct inode);
            fseek(temp_output_file, add, SEEK_SET);
            fwrite(secondary_inode, 1, sizeof(struct inode), temp_output_file);
            fseek(temp_output_file, 0, SEEK_END);
            
        } else {
            // writing inodes
            long add = 1024 + bs * inode_offset + i * sizeof(struct inode);
            fseek(temp_output_file, add, SEEK_SET);
            fwrite(primary_inode, 1, sizeof(struct inode), temp_output_file);
            fseek(temp_output_file, 0, SEEK_END);
        }
    
    }

    void* swapAddress = input_buffer + 1024 + (bs * swap_offset);

    void* freeBlockLimit = swapAddress - 1;

    int availableFreeBlocks = (freeBlockLimit - (input_buffer + 1024 + bs*(data_offset + no_blocks))) / bs;

    int zero = 0;
    for (i = 0; i < availableFreeBlocks; i+=1) {
        no_blocks++;
        fwrite(&no_blocks, 1, 4, temp_output_file);

        int j;
        for (j = 0; j < bs - 4; j++) {
            fwrite(&zero, 1, 1, temp_output_file);
        }
    }


    int end = -1;
    fwrite(&end, 1, 4, temp_output_file);

    int j;
    for (j = 0; j < bs - 4; j++) {
        fwrite(&zero, 1, 1, temp_output_file);
    }

    fwrite(swapAddress, 1, input_buffer + filesize - swapAddress, temp_output_file);

    return input;
}

/**
 * @brief Writes the disk image to a file.
 * 
 * @param file The file pointer to write the disk image.
 * @param di The diskImage structure to write.
 */
void writeDiskImage(FILE *file, const diskImage *di) {
     // Close the temporary output file
    fclose(temp_output_file);
    // remove(outfile_name);

    // // Rename the temporary file to the target filename
    // if (rename("./temp_output_file", outfile_name) != 0) {
    //     perror("Error renaming file");
    //     return;
    // }
}

/**
 * @brief Frees the memory allocated for the disk image.
 * 
 * @param di The diskImage structure to free.
 */
void freeDiskImage(diskImage *di) {
    free(di->inodes);
    free(di->data_blocks);
    // free(new_inodes);
}
