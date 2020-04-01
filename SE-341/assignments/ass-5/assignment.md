# Custom Shader

## 作业目的

1. 了解shader和其在游戏中的应用；
2. 学会编写shader来实现一些简单的视觉特效；
3. 能够使用shader来实现一些特别的渲染效果；
4. 学会使用shader debug工具；
5. 为后续作业搭建基本框架。

## 作业要求

请同学们在开发本次作业时，注意项目的鲁棒性和可扩展性。提交完本次作业后，保留此次作业的项目。此次作业会和后续几次作业合并为一个完整的游戏。

本次作业需要完成的内容：

1. 本次作业要搭建一个类似 Minecraft 的游戏，实现一个支持人物移动、从物品栏选择物品、放置物品于地面的 3D 游戏；

2. 参考

	作业文档。实现基础的 shader，并且支持使用这些 shader 来渲染游戏中的物体（在放置物品时，支持使用自己实现的 shader 来渲染该物体）： 

	- 实现展示模型法线方向的 shader
	- 实现基于 Blinn-Phong 的 shader

3. 参考

	作业文档。编写 shader 的 GUI：

	- 通过继承 Unity 自带的 `ShaderGUI` 类，实现自定义的 Shader GUI；
	- 实现在自定义 GUI 中调整各项参数 Shader 参数，如高光颜色、镜面高光系数等；
	- 实现在自定义 GUI 中的 Shader 效果切换，使得用户可以通过下拉框选取不同的渲染效果，包含法向 Shader、Blinn-Phong 光照 Shader。

4. 了解、尝试至少一个 Debug 工具：

	- Unity Frame Debugger（Unity 自带工具）
	- RenderDoc（推荐，但 Mac 可能不支持）
	- Nvidia Nsight（Nvidia 显卡专用）
	- Visual Studio Frame Debugger（Visual Studio 工具）
	- Xcode Instruments（Xcode 工具）

5. 实现一些更复杂的 Shader 效果（下面 4 种中，至少实现一种。多实现会有额外的加分）

	- 描边shader，参考：[Hull Outlines (链接到外部网站。)链接到外部网站。](https://www.ronja-tutorials.com/2018/07/21/hull-outline.html), [Outline Shader (链接到外部网站。)链接到外部网站。](https://roystan.net/articles/outline-shader.html)
	- 海浪shader，参考：[Waves (链接到外部网站。)链接到外部网站。](https://catlikecoding.com/unity/tutorials/flow/waves/)
	- 毛发shader，参考：[Unity的PBR拓展——皮毛材质 (链接到外部网站。)链接到外部网站。](https://zhuanlan.zhihu.com/p/57897827)，[妲己尾巴毛发制作分享 (链接到外部网站。)链接到外部网站。](https://mp.weixin.qq.com/s/aIWMEO5Qa2gNn2yCmnHbOg)
	- 草地shader，参考：[利用GPU实现无尽草地的实时渲染 (链接到外部网站。)链接到外部网站。](https://zhuanlan.zhihu.com/p/29632347)，[Grass Shader (链接到外部网站。)链接到外部网站。](https://roystan.net/articles/grass-shader.html)

6. 请将作业要求 2/3/5中实现的 shader 应用在要求 1 的游戏中，使得用户可以在场景中放置不同 shader 实现的物品。

7. 编写实验报告，其中包括：

	1. Shader 的实现过程、实现结果展示；
	2. Shader GUI 的实现过程、实现结果展示
	3. Debug 工具的使用过程；
	4. 复杂 shader 的实现原理、过程和实现结果展示
	5. 游戏操作指南

## 作业提示

1. 本次作业为个人作业，艺术资源（音乐、图片、字体等）可以直接从网络获取。常用素材获取网站：[Magictools (链接到外部网站。)链接到外部网站。](https://github.com/ellisonleao/magictools)，[Unity Asset Store (链接到外部网站。)链接到外部网站。](https://assetstore.unity.com/)。
2. 本次作业需要提交的内容包括：**实验报告**(导出为 `.pdf`)、**工程文件**（导出为 `.unitypackage`）、**可执行文件**。提交时请将所有文件打包成一个压缩文件，命名为 学号_姓名全拼_作业.xxx，如：`517xxxxxxxxx_ZhangSan_hw4.rar`。
3. 在最终提交的作业中，请去除多余的文件（如没有用到的图片素材），以免作业文件占用空间过大。
4. 本次作业的**截止时间**为 4 月 6 日 23:59，并提供了一天的作业补交时间，**补交截止时间**为 4 月 7 日 23:59，补交的作业会扣除 10% 的分数。