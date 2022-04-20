###文件结构
  1. 主函数  
     > main.cpp
  2. 文件选择类  
     > DataMerge.h  
     DataMerge.cpp
  3. 配置文件读取类  
     > ReadInitFile.h  
     ReadInitFile.cpp  
     ParamStruct.h  
     dataInit.ini  
  4. 工具类  
     > Util.h  
     Util.cpp
  5. CMakeLists.txt  

###使用说明
  1. 修改配置文件 dataInit.ini，修改pcd文件路径、图像img文件路径。  
  2. 将配置文件放入编译后的文件夹内（如./build/）。
  3. 程序运行后，选中点云显示窗口，键盘“n/N”——显示下一帧（next），“b/B”——返回上一帧（back），“s/S”——保存当前显示帧并自动显示下一帧（save）。
  4. 停止程序后，可手动下次启动的帧序号（点云、图像上已显示），修改配置文件 startFramePos 字段值，下次运行程序后从该帧序号处开始显示。
