export MNN_Path=/root/Amin/libs/MNN_libs/MNN
mkdir MNN
mkdir MNN/lib
cp -r $MNN_Path/include  MNN
cp -r $MNN_Path/tools/cv/include MNN
cp $MNN_Path/build/libMNN.a MNN/lib