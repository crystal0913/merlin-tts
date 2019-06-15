## merlin合成相关代码的c++版本
包含：
- 前端工具【参考https://github.com/Jackiexiao/MTTS】
- DNN合成【参考https://github.com/CSTR-Edinburgh/merlin】

duration model和acoustic model均使用6层的全连接网络，训练好的参数在`tts_data`中。

### Requirement
- cmake
- WORLD
- cppjieba
- regex

### Run
- 程序入口：inference.cpp
- Input: 汉字串（不包括标点符号等其他非汉字字符）
- Output：3个音频特征文件，分别为：test/1.mgc、test/1.lf0、test/1.bap
- 再利用Vocoder：音频特征生成wav，`python ~/merlin/src/gen_test.py test_synth.conf`
