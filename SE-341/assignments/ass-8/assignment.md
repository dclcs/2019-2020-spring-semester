# Cardboard VR

## 作业目的

1. 了解 Unity 中 VR 程序的开发；
2. 尝试使用 Google VR SDK 开发一款移动端的 Google Cardboard 游戏。

## 作业要求

1. 配置移动端开发环境
   - Windows/Mac电脑+Android手机：参考[作业文档](http://dalab.se.sjtu.edu.cn/gp/hw-cardboard-vr/cardboard tutorial/gvr-cardboard-tutorial.html)配置 Android VR 项目；
   - Windows电脑+IOS手机：参考[作业文档](http://dalab.se.sjtu.edu.cn/gp/hw-cardboard-vr/cardboard tutorial/gvr-cardboard-tutorial.html)配置 Android VR 项目；（建议在Editor中调试完成后，提前打包为apk，交由助教测试游戏功能）
   - Mac电脑+IOS手机：参考[作业文档](http://dalab.se.sjtu.edu.cn/gp/hw-cardboard-vr/cardboard tutorial/gvr-cardboard-tutorial.html)配置 IOS VR 项目；
2. 在之前的shader、physics作业基础上，使用 Google VR SDK 设计开发一个基于 Google Cardboard 的 VR游戏，其中要求包含：
   - 使用之前作业里的内容（简单shader、自定义特效shader、Game Physics）；
     - 出于手机性能和 Google Cardboard 交互方式的限制，且保证游戏风格的统一，可以适当去除之前作业中的一些功能，如Geometry Shader等
   - 设计实现基于 Google Cardboard 的游戏机制，需要使用到头盔位姿信息和屏幕点击事件；
     - 可以参考文末列出的参考游戏进行设计
   - 游戏开始和结束条件；
   - 游戏计分机制；
   - 用户界面 (UI)，包括分数统计、开始界面等；
3. 编写实验报告，其中包括：
   - 游戏设计及各个功能的实现情况，可适当插入截图及代码。如果项目中用到了非自己制作的内容（如美术资源、代码、插件），应在报告中注明
   - 游戏操作指南
4. 按照参考游戏中的视频形式，录制2分钟以内的移动端的游戏视频（Windows+IOS的同学请另行联系助教）。
5. 如果项目发布在Android端，请同时上交.apk可执行文件。

## 作业提示

1. 为避免不必要的麻烦，请使用Unity 2018进行开发。
2. 本次作业需要提交的内容包括：**实验报告**（导出为.pdf）、**工程文件**（导出为.unitypackage）、**游戏视频**（建议mp4格式）、**可执行文件**（仅Android平台，导出为.apk）。提交时请将所有文件打包成一个压缩文件，命名为 学号_姓名全拼_作业.xxx，如：517xxxxxxxxx_ZhangSan_hw7.rar。
3. 在最终提交的作业中，请去除多余的文件（如没有用到的图片素材），以免作业文件占用空间过大。
4. 本次作业的**截止时间**为5月15日23:59，并提供了一天的作业补交时间，**补交截止时间**为5月16日23:59，补交的作业会扣除10%的分数。

## 作业参考资料

- 作业文档
  - [作业文档](http://dalab.se.sjtu.edu.cn/gp/hw-cardboard-vr/cardboard tutorial/gvr-cardboard-tutorial.html)
- Google VR SDK
  - [官方文档](https://developers.google.com/vr/develop/unity/get-started-android)
  - [Unity Package](https://oc.sjtu.edu.cn/courses/17606/files/928503/download?verifier=SUSsquqyl0cS6CGf8iptnOKhegj7s5Js2NrXqp3N&wrap=1)
- 参考游戏
  - Sunken Treasure Expedition
    - 潜水艇驾驶模拟
  - Mineforge
    - Minecraft游戏
  - Crazy Ojek
    - 跑酷游戏
  - Space Rustlers
    - 飞行模拟
  - Little Vr Shooter
    - 射击游戏
  - Perfect Angle Zen
    - 视觉错位游戏

