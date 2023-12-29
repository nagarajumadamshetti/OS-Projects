//Requirements: 
Environment: VCL- XINU+QEMU (CSC501)
//clone the repo or download repo as zip file
$git clone https://github.com/ncsu-csc-501/project4-nagarajumadamshetti.git


//STEPS TO RUN
//first type MAKE OR 'gcc -o defrag defrag.c disk_image.c'
$make


// running the code for a image, eg: ./defrag images_frag/disk_frag_1
$./defrag 'path-to-fragmented-image'
output file is named: "disk_defrag" in the home folder

//test or compare, eg: cmp disk_defrag images_defrag/disk_defrag_1
$cmp disk_defrag 'path-to-defragmented-image'


//to clean
$make clean 