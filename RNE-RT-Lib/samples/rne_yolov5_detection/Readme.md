# 操作说明
## step 1: 确保配置好交叉编译工具链

## step 2: 编译 sample

`make clean; make`

## step 3: 将资源copy至板端

* 将输出 build_linux_a7_uclibc/Release/rne_yolov5_detection.elf 传输到板端
* 将 resource 目录传输到板端

## step 4: 运行sample

- ./rne_yolov5_detection.elf 参数1 参数2 参数3 参数4

`./build_linux_a7_uclibc/Release/rne_yolov5_detection.elf resource/yolov5_quantize_r.cfg resource/yolov5_quantize_r.weight resource/1.jpg resource/result.jpg`

**注: 此yolov5的模型为一体化sample下onnx框架带relue模型。**

# sample 参数说明
| 参数 | 示例 | 说明|
| :- |:- |:- |
| 参数1|resource/yolov5_quantize_r.cfg| 网络配置文件|
| 参数2|resource/yolov5_quantize_r.weight| 网络权重文件|
| 参数3|resource/1.jpg 或 resource/2.jpg 或 resource/3.jpg 或 resource/4.jpg| 网络输入文件|
| 参数4|resource/result.jpg|  检测结果图片保存文件名（必须以图片格式结尾）|
