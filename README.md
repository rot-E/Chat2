# Chat2
Chat server/client written in C.

## ビルド&実行
```sh
$ git clone https://github.com/RotationE/Chat2.git
$ cd Chat2
$ git clone https://github.com/RotationE/NeoC.git
$ cd NeoC
$ cp -r include/ ../src
$ mkdir build
$ cd build
$ cmake ..
$ make -j                                                                        
$ cp lib/NeoC/libNeoC.a ../../src
$ cd ../../src
$ make -j
$ ./Main -h
```
<br>

## 利用許諾
**このリポジトリ内の一切の成果物は[AGPL3](https://www.gnu.org/licenses/agpl-3.0.ja.html)でライセンスされています。**

