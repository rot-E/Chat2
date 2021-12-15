# Chat2
Chat server/client written in C.
<br>
<br>

## ダウンロード&ビルド&実行
```sh
git clone https://github.com/RotationE/Chat2.git
cd Chat2
git clone https://github.com/RotationE/NeoC.git
cd NeoC
cp -r include/ ../src
mkdir build
cd build
cmake ..
make -j                                                                        
cp lib/NeoC/libNeoC.a ../../src
cd ../../src
make -j
./Main -h
```
<br>

## 利用許諾
[`AGPLv3`]（LICENSE)

