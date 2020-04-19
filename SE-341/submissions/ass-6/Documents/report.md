# Assignment 6

## Bidirectional Reflectance Distribution Function

### Implementations

没有太多的代码需要写。

#### Distribution Term

根据
$$
D_{GGX} = \dfrac {\alpha^2} {\pi((n·h)^2(\alpha^2 - 1) + 1) ^2}
$$
可以写出代码

```glsl
pow(roughness, 2.0) / (UNITY_PI * pow( pow(NdotH, 2.0) * (pow(roughness, 2.0) - 1) + 1, 2));
```

注意这里不能使用 C# 标准库中的 `System.Math.Pow` 进行幂指数运算，而要采用 Unity Shader 语言中预定义的 `pow` 函数。

#### Fresnel Term

根据 Schilick 模型的修正，我们需要通过
$$
F_{Schlick} = C_{spec} + (1 - C_{spec})(1 - l·h)^5
$$
进行计算。为了方便地得到反射光颜色 $C_{spec}$ 和 $1 - l·h$，我们可以通过调用内置 `DiffuseAndSpecularFromMetallic` 函数来简化代码。这个函数的前两个参数是传入参数，和 `Schlick_F` 的两个参数对应；而後两个参数是传出参数，调用後会将值存入其中。

得到 `specColor` 和 `oneMinusReclectivity` 之後就直接返回就好了。

```glsl
specColor + (1 - specColor) * oneMinusReflectivity;
```

#### Geometry Term

Cook-Torrance 告诉我们，几何系数
$$
G_{Cook - Torrance} = \min\lbrace 1, \dfrac {2(n·h)(n·v)} {v·h}, \dfrac {2(n·h)(n·l)} {v·h}\rbrace
$$
式中的 $n·l$、$n·v$、$v·h$、$n·h$ 都作为参数给出了。因此只需要做简单的乘除法及比较即可返回结果。

同样地，这里不能使用 `System.Math.Min`，而要使用 Unity Shader 提供的 `min`。

#### Finale

最後，在 Fragment Shader 末尾将除错代码注释掉，并将最终计算的颜色向量返回即可得到结果。

### Results

#### Demo

![image-20200419123721476](report.assets/image-20200419123721476.png)

> 注：调整了摄像机位置为 `(20, 20, 50)`，角度为 `(0, 0, 0)`。

#### In-Game

将上面的天空盒和 BRDF 效果应用到 `Minecrappy` 游戏中，得到的效果如下：

![image-20200419124357670](report.assets/image-20200419124357670.png)

## Unrealistic Rendering

## Post Processing

