## 作业目的

1. 了解游戏中对物理现象的模拟方法
2. 了解几种常微分方程的求解方法
3. 学会基本的弹簧模型的实现

## 作业要求

请同学们在开发本次作业时，注意项目的鲁棒性和可扩展性。提交完本次作业后，保留此次作业的项目。此次作业会和后续几次作业合并为一个完整的游戏。

1. 本次作业将在「自定义shader」作业的基础上进行，为游戏添加更丰富的物理效果
2. 单摆模拟 (40%)
	- 参考 [Homework slides](https://oc.sjtu.edu.cn/courses/17606/files/778272/download?verifier=bcAjoMEE8qVd4iUcyfGVpzXR1sUFmN5es4xuOAjb&wrap=1)，实现 Explicit Euler 、 Explicit Midpoint 、 Trapezoid 方法的单摆模拟
	- 将模拟结果和解析解进行对比，并通过折线图或表格展示
		- 由于单摆没有简单的解析解，可以用时间精度较高的模拟结果作为解析解
3. 头发模拟 (60%)
	- 参考爱丽丝的发丝，实现基于 Verlet 方法的弹簧模型
		- 构建简单的模型，以球体作为头部模型，以质点弹簧作为头发模型
		- 利用 Verlet 积分方法实现头发的动态效果
		- 实现头发上的弹簧约束、头发和球体的碰撞约束
	- 实现简单的交互
		- 通过 UI 调整头发的重量、长度、数量等参数
		- 通过游戏操作，移动模型产生头发晃动效果
		- 在界面显示当前帧率
	-  实现更复杂的效果（可选加分项）
		- 实现头发渲染效果
		- 通过实现更多维度的弹簧约束，模拟布料或 Jelly Cube
		- 实现隐式欧拉方法的头发模拟
4. 请将作业要求 1、2、3 中实现的仿真应用在「自定义shader」作业中搭建的游戏上，使得用户可以在场景中操作实现的物理仿真模型。
5. 编写实验报告，其中包括：
	1. 单摆模拟的实现过程、实现结果展示；
	2. 基于 Verlet 的头发仿真的实现原理、过程和实现结果展示
	3. 游戏操作指南

## 作业提示

1. 本次作业为个人作业，艺术资源（音乐、图片、字体等）可以直接从网络获取。常用素材获取网站：[Magictools](https://github.com/ellisonleao/magictools)，[Unity Asset Store](https://assetstore.unity.com/)。
2. 本次作业需要提交的内容包括：**实验报告**(导出为 `.pdf`)、**工程文件**（导出为 `.unitypackage`）、**可执行文件**。提交时请将所有文件打包成一个压缩文件，命名为「学号_姓名全拼_作业.xxx」，如：`517xxxxxxxxx_ZhangSan_hw6.rar`。
3. 在最终提交的作业中，请去除多余的文件（如没有用到的图片素材），以免作业文件占用空间过大。
4. 本次作业的**截止时间**为 5 月 4 日 23:59，并提供了一天的作业补交时间，**补交截止时间**为 5 月 5 日 23:59，补交的作业会扣除 10% 的分数。

## 作业参考资料

- Homework Slides
	- [Homework slides](https://oc.sjtu.edu.cn/courses/17606/files/778272/download?verifier=bcAjoMEE8qVd4iUcyfGVpzXR1sUFmN5es4xuOAjb&wrap=1)
- Technical blog
	- [爱丽丝的发丝](http://miloyip.com/2011/alice-hair/) (Milo Yip)
-  Notes/Books
	- [Differential Equation Basics](http://dalab.se.sjtu.edu.cn/gp2019/hw5doc/notes-ODE.pdf) (Pixar course note)
- Further reading
	- Mass spring simulation 
		- [A Mass Spring Model for Hair Simulation](http://dalab.se.sjtu.edu.cn/gp2019/hw5doc/2008-A Mass Spring Model for Hair Simulation.pdf)
		- [Fast Simulation of Mass-Spring Systems](http://dalab.se.sjtu.edu.cn/gp2019/hw5doc/2013-Fast Simulation of Mass-Spring Systems.pdf)
		- [A Reduced Model for Interactive Hairs](http://dalab.se.sjtu.edu.cn/gp2019/hw5doc/2014-A Reduced Model for Interactive Hairs.pdf)
	- Implicit solver
		- [Implicit Euler method](http://dalab.se.sjtu.edu.cn/gp2019/hw5doc/PixarCourse-ImplicitMethods.pdf) (Pixar course note)
		- [Large Steps in Cloth Simulation](https://www.cs.cmu.edu/~baraff/papers/sig98.pdf)
	- Jelly cube
		- [Case study: the jelly cube](http://www.cs.cmu.edu/~barbic/jellocube_bw.pdf)
	- Hair rendering
		- Light Scattering from Human Hair Fibers
			- [Summary of the paper](https://hairrendering.wordpress.com/2010/06/26/marschner-shader-part-i/)
		- [An Energy-Conserving Hair Reﬂectance Model](http://www.eugenedeon.com/wp-content/uploads/2014/04/egsrhair.pdf)