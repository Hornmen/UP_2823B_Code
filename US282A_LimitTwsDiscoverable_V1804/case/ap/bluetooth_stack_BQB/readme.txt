
使用BQB模式操作步骤:
1、打开case_independent.h ，找到下面语句去掉注释
//是否进入BQB测试模式
//#define __BQB_TEST_PROFILE_

2、用\bluetooth_stack_BQB\btstack\bt_src_lib里面的.o文件替换掉原来btstack\bt_src_lib里面的.o文件

3、全部重新编译
