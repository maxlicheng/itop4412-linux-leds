# 说明
嵌入式 linux leds驱动及应用程序

## 功能介绍
- 1.leds 字符设备驱动；
- 2.配套 c应用程序；
- 3.配套 qt应用程序，其中qt应用程序含ADC采集显示，蜂鸣器控制，LED使能及闪烁频率控制；
- 4.其他待补充
  
## 使用方式
### 1.下载
```
git clone https://github.com/maxlicheng/itop4412-linux-leds.git 
```
### 2.编译
- 驱动和c应用程序，修改Makefile交叉编译器名称，交叉编译器版本需要与编译嵌入式linux内核时的版本一致；
- qt应用程序，需要交叉编译qt源码，qt源码交叉编译完成后，用交叉编译成的qmake编译qt应用程序，具体博客文章参考：[《交叉编译QT到嵌入式arm Linux系统》](https://www.maxlicheng.com/embedded/523.html "Title")；
- 将交叉编译好的驱动、c应用程序及qt应用程序拷贝至嵌入式linux开发板；
### 3.加载驱动
```
insmod leds.ko
```
### 4.c应用程序
```
./leds_app /dev/led0 0 1	//点亮LED0
./leds_app /dev/led0 0 0	//关闭LED0
./leds_app /dev/led0 1 0	//获取LED0状态
./leds_app /dev/led0 2 10	//设置LED0闪烁频率
./leds_app /dev/led0 3 0	//获取LED0闪烁频率
```
### 5.qt应用程序
```
./qtleds &
```

## 效果图
### 1.c应用程序效果图
![Image text](http://www.maxlicheng.com/wp-content/uploads/2019/07/LED应用程序.jpg)
### 2.qt应用程序效果图
![Image text](https://raw.githubusercontent.com/maxlicheng/itop4412-linux-leds/master/view/%E5%8A%9F%E8%83%BD%E5%9B%BE.jpg)

## 视频教程
- 1.[《嵌入式Linux QT应用程序功能演示》](https://www.bilibili.com/video/av60787366/)
- 2.[《嵌入式Linux QT应用程序交叉编译方法》](https://www.bilibili.com/video/av60787366/?p=2)

## 博客文章
- 1.[《QT5开发环境在ubuntu下的安装》](https://www.maxlicheng.com/embedded/513.html)
- 2.[《交叉编译QT到嵌入式arm Linux系统》](https://www.maxlicheng.com/embedded/523.html)

# 其他
## 嵌入式Linux字符设备驱动开发流程——以LED为例
- 1.[《设备模块加载及卸载》](https://www.maxlicheng.com/embedded/561.html "Title")
- 2.[《静态申请设备号》](https://www.maxlicheng.com/embedded/563.html "Title")
- 3.[《动态申请设备号》](https://www.maxlicheng.com/embedded/566.html "Title")
- 4.[《注册字符类设备》](https://www.maxlicheng.com/embedded/568.html "Title")
- 5.[《生成字符设备节点》](https://www.maxlicheng.com/embedded/569.html "Title")
- 6.[《完善字符类设备驱动》](https://www.maxlicheng.com/embedded/572.html "Title")
- 7.[《根据完善后的模板编写相关设备驱动》](https://www.maxlicheng.com/embedded/575.html "Title")

