# VFX Shader

## 作业目的

1. 了解 shader 和其在游戏中的应用；
2. 学会编写 shader 来实现一些简单的视觉特效；
3. 理解基于物理的渲染模型。

## 作业要求

请同学们在开发本次作业时，注意项目的鲁棒性和可扩展性。提交完本次作业后，保留此次作业的项目。此次作业会和后续几次作业合并为一个完整的游戏。

本次作业需要完成的内容：

1. 本次作业将在“简单 shader”作业的基础上进行，为游戏添加更丰富的渲染效果
2. 参考[作业文档](http://dalab.se.sjtu.edu.cn/gp/hw-custom-shader/manual.html)，实现基于物理的渲染模型
	- 了解双向反射分布函数（BRDF）的原理以及代码实现
	- 在[样例代码](http://dalab.se.sjtu.edu.cn/gp/hw-custom-shader/gp2020-hw-brdf.unitypackage)基础上，参照[作业文档](http://dalab.se.sjtu.edu.cn/gp/hw-custom-shader/manual.html)实现 Cook-Torrance 模型
	- 请在实验报告中使用**材质球矩阵图**展示效果，简要描述实现思路
3. 非真实感渲染（下面几种中，至少实现一种。多实现会有额外加分）
	- 卡通风格渲染（参考文献：[Illustrative Rendering in Team Fortress 2](http://dalab.se.sjtu.edu.cn/gp2019/reference/NPAR07_IllustrativeRenderingInTeamFortress2.pdf)）
	- 风格化卡通高光渲染（参考文献：[Stylized Highlights For Cartoon Rendering And Animation](http://dalab.se.sjtu.edu.cn/gp2019/reference/stylized_highlight.pdf)）
	- 素描风格渲染（参考文献：[Real-Time Hatching](http://dalab.se.sjtu.edu.cn/gp2019/reference/hatching.pdf)）
	- 请在实验报告中描述实现思路、方法，并展示效果
4. 屏幕后处理效果（下面几种中，至少实现一种。多实现会有额外加分）
	- Bloom 效果（参考文献：[GPU Gems: Real-Time Glow](https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch21.html)）
	- 运动模糊（参考文献：[GPU Gems3: Motion Blur as a Post-Processing Effect](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch27.html)）
	- 全局雾效（参考文献：[Real-Time Fog using Post-processing in OpenGL](http://dalab.se.sjtu.edu.cn/gp2019/reference/fog.pdf), [Reconstructing positions from the depth buffer](http://www.derschmale.com/2014/01/26/reconstructing-positions-from-the-depth-buffer/)）
	- 景深效果（参考文献：[GPU Gems: Depth of Field: A Survey of Techniques](https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch23.html), [GPU Gems3: Practical Post-Process Depth of Field](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch28.html)）
5. 请将作业要求 1/2/3 中实现的 shader 应用在「简单 shader」作业中搭建的游戏上，使得用户可以在场景中放置不同渲染效果的物品，在一定条件下（如设置界面中选择）触发切换屏幕后处理特效。
6. 编写实验报告，其中包括：
	1. 基于物理的 shader 的实现过程、实现结果展示；
	2. 非真实感渲染、屏幕后处理效果 shader 的实现原理、过程和实现结果展示
	3. 游戏操作指南

## 作业提示

1. 本次作业为个人作业，艺术资源（音乐、图片、字体等）可以直接从网络获取。常用素材获取网站：[Magictools](https://github.com/ellisonleao/magictools)，[Unity Asset Store](https://assetstore.unity.com/)。
2. 本次作业需要提交的内容包括：**实验报告**(导出为`.pdf`)、**工程文件**（导出为`.unitypackage`）、**可执行文件**。提交时请将所有文件打包成一个压缩文件，命名为 `学号_姓名全拼_作业.xxx`，如：`517xxxxxxxxx_ZhangSan_hw5.rar`。
3. 在最终提交的作业中，请去除多余的文件（如没有用到的图片素材），以免作业文件占用空间过大。
4. 本次作业的**截止时间**为 4 月 20 日 23:59，并提供了一天的作业补交时间，**补交截止时间**为 4 月 21 日 23:59，补交的作业会扣除 10% 的分数。

## 作业参考资料

1. 作业文档（pdf版）
	- [custom-shader-tutorial.pdf](https://oc.sjtu.edu.cn/courses/17606/files/659392/download?verifier=ChIWVuguDWS0FENprD4wTHqIXY6v3VX9zPTeAtvB&wrap=1)
2. brdf样例代码
	- [gp2020-hw-brdf.unitypackage](https://oc.sjtu.edu.cn/courses/17606/files/731142/download?verifier=twxmT1HDmbV0EEHCfUzOG5VRogpDj8YvbGD0AzBA&wrap=1)
3. 作业ppt
	- [custom-shader-slides.pdf](https://oc.sjtu.edu.cn/courses/17606/files/709193/download?verifier=8xQd8HDBmnkVQOEAvvCZrgRItTKGF5qvXA7Jb8Vs&wrap=1)
4. Team fortress 模型
	- [team-fortress.unitypackage](https://oc.sjtu.edu.cn/courses/17606/files/727401/download?verifier=Qj635U1Qzr3RVr0j3qYf4a1vTKOQDOO4VIiEjbee&wrap=1)
5. Websites 
	- [Unity build-in shaders](https://unity3d.com/get-unity/download/archive)
	- [Brief BRDF Summary](http://user.ceng.metu.edu.tr/~kadircenk/raytracing/brdf.pdf), Kadircenk Blog
	- [Siggraph 2015 shading course](https://blog.selfshadow.com/publications/s2015-shading-course/)
6. Papers
	- [Cook Tarrance BRDF](http://inst.eecs.berkeley.edu/~cs283/sp13/lectures/cookpaper.pdf)
	- [GGX Distribuition term](https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf)
	- [Schlick Fresnel term](http://www.cs.virginia.edu/~jdl/bib/appearance/analytic models/schlick94b.pdf)
	- [Cook Torrance Geometry term](http://graphics.pixar.com/library/ReflectanceModel/)
	- [PBS at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)
7. Books
	- [Physically Based Rendering: From Theory To Implementation](https://www.pbrt.org/)

