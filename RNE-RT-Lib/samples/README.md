**注: 此处以rne_simple_forward为例。**

`./build_linux_a7_uclibc/Release/rne_simple_forward.elf  ../compiled_model/test_gp/model_r.cfg  ../compiled_model/test_gp/model_r.weight  ../compiled_model/test_gp/model_input.bin  int_chw`

# sample 参数说明
| 参数 | 示例 | 说明|
| :- |:- |:- |
| 参数1|../compiled_model/test_gp/model_r.cfg| 网络配置文件|
| 参数2|../compiled_model/test_gp/model_r.weight| 网络权重文件|
| 参数3|../compiled_model/test_gp/model_input.bin | 网络输入文件|
| 参数4|int_hwc|int_chw|float_hwc|float_chw|输入数据的格式|



