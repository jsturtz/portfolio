#!/bin/bash

CUDA_PATH="/usr/local/apps/cuda/cuda-9.2"
CUDA_BIN_PATH="$CUDA_PATH/bin"
CUDA_NVCC="$CUDA_BIN_PATH/nvcc"

echo -e "BlockSize\tSize\tNumHits\tMegaTrialsPerSec\n"
SIZE=16000
BLOCKSIZE=16
for x in  1 2 3
do
  for y in {1..6}
  do
    $CUDA_NVCC -o  monteCarlo monteCarlo.cu -DSIZE=$SIZE -DBLOCKSIZE=$BLOCKSIZE
    ./monteCarlo
    SIZE=$(expr $SIZE \* 2)
  done
  BLOCKSIZE=$(expr $BLOCKSIZE \* 2)
  SIZE=16000
done
