# gdres系统

[TOC]



## 一、总体介绍

##### gdres含义：

~~~~c
g  : guo shuaiwei   郭帅伟
d  : design         设计
re : restructure    重构
s  : stable         稳定
g(gsw)-d(design)-re(restructure)-s(stable)
~~~~

​		一个程序的生命周期，从首次设计、开发到为了适应新需求的变化而变得臃肿腐化，然后进行重构、优化，提取出稳定的部分，封装成库，从朦胧的变化到趋于完美的具化；

​		一个程序员的生命周期，从模仿借鉴到沉淀积累，随着经验不断丰富，对各种技术的驾驭游刃有余，对框架的掌握随心所欲，能针对不同场景和需求给出不同的解决方案，进一步**总结创新**继续升华。

​		**我当前尚处于模仿借鉴的初级阶段，需要不断总结积淀。**这个gdres系统会是我技术成长的见证，随着我对编程的理解和使用经验的增加，会不断优化、完善这个系统。希望在未来的某一天，他能成为一套可以被多数人认可的基础框架。” 勤于总结思考，敢于坚持到底，就能创造奇迹 ！“

​		感谢在我成长道路上对我有过帮助启迪的UP主和源码大神！                                             （于  2021.8.10）

------

##### gdres背景：

​		从研究生到现在（2021.8），我独立完成过6个自动化测控装备项目，虽然都不算大项目，但麻雀虽小五脏俱全。从项目需求分析，到给出解决方案；从机械结构设计，到联系工厂加工制造和装配调试；从器件选型到采购；从软件逻辑编写到界面设计；从实验室到生产线；自认为积累了丰富的工程项目经验。然而内视自我的时候才发现，”**杂而不精**"，不是物适的生存之道。**这种斑杂的系统性经验的东西，没有一定资历和关键的技术水平做支撑，很难找到用武之地。**但这个过程所带给我的系统性思维是潜移默化和具有普适性的。

​		所以，在这个万物互联的时代，掌握一门不错的**大众化的**编程技术，对于工程者来说似乎是一个想做点什么事情的基础。于是决定从小众的Labview编程转向大众的c++编程，并以此为技术突破口加上我那些斑杂的系统性经验的东西去实现一直以来都有的那些想法。从labview转向c++面临的第一个问题就是似乎什么都没有了，从面向项目过程编程，到不知道怎么出手。只能自己先造轮子了。所以决定完成一个自己的轮子系统。目的有二：其一、借鉴行业大佬的经验，提升自己的编程技术；其二、从无到有为自己做粮草储备。

​		一套框架的选择，要适合需求才是最优的。但就我目前的阶段而言，还不涉及框架适配，能完成一套可用的框架系统，就是当前的目标。不过从内心里，对技术的发展方向，有个朦胧的方向指导：

~~~
		第一阶段：完成高性能服务器框架，实现数据采集，到云端的传输、存储、管理和取回，打通数据链路；

​		第二阶段：集成数据处理算法，提供算法接口，方便支持数据处理（图像、声音、视频等的数据）

​		第三阶段：......
~~~



------

## 二、开发实施

##### 基本指导思想：

~~~
先从无到有实现基本功能，再在项目实战中进行完善、优化和性能提升。
~~~

##### 基本实现思路：

~~~
参照工作中使用的程序框架，基于使用中的理解，实现一版功能相似的框架，包含两个部分：
1. 基于消息的模块管理系统
2. 基于协程的webserver服务系统
~~~

------



### 1. 基于消息的模块管理系统的实现

------

##### 系统的基本功能：

~~~c++
1. 支持业务模块的注册和注销；
2. 为业务模块创建工作线程，并对业务模块线程进行管理；
3. 支持业务模块注册定时任务；
4. 支持业务模块间消息的传递；
5. 支持windows和linux环境下运行
6. （之后继续补充）
~~~

##### 系统的基本组成：

​		系统暂时应该包含以下几个部分：

（1） 日志模块：实现日志器的管理和日志输出到文件；

（2） 线程封装：跨平台支持线程命名（方便日志打印）和线程状态获取；

（3） 线程池：基本组件

（4） 定时器：添加定时任务

（5） 模块类：业务模块的运行主体

（6） 模块管理类：实现对模块类的管理和消息的路由

（7） 各种锁：读写锁、自旋锁、信号量的实现以支持跨平台

（8） ...



------

#### 1.1 日志模块的实现

##### （1） 日志模块支持：

  - [x] 日志到文件或者控制台的输出；
  - [x] 方便日志输出格式的设置和调整；

##### （2） 日志模块的实现方案：

​		（学习借鉴大佬sylar的日志系统实现方案）

​		参见：http://www.sylar.top/blog/?p=147

------

#### 1.2 线程池类的实现

##### （1）线程池支持：

- [ ] 动态线程池：有管理线程，根据任务量对工作线程数量进行动态增减
- [ ] 固定线程池：不需要管理线程，创建固定数量的工作线程
- [ ] 向线程池添加任务并自动执行任务

##### （2）线程池的实现类图：		

<img src="C:\Users\86187\AppData\Roaming\Typora\typora-user-images\image-20211010105145323.png" alt="image-20211010105145323" style="zoom:50%;" />

​	

------

#### 1.3 定时器的实现

##### （1）定时器支持：

- [ ] 支持添加任意时长的定时器
- [ ] 支持循环定时器或者一次性定时器的创建
- [ ] 支持定时器的取消、刷新和重置

##### （2）定时器的实现方案：

​		Timer类：记录创建的定时器的基本信息

​		TimerMgr类：管理创建的全部定时器，定时触发到时间的定时器

​		ModulTimerMgr类：继承TimerMgr类，根据模块管理的需求，自带线程池，支持一个时间管理线程和N个定时任务执行的工作线程

![image-20211010175152607](C:\Users\86187\AppData\Roaming\Typora\typora-user-images\image-20211010175152607.png)



------
#### 1.4 业务模块类的实现

##### （1）业务模块类支持：

- [ ] 在没有消息处理时陷入休眠，当有消息到来时被唤醒；
- [ ] 优先处理优先级较高的消息；
- [ ] 支持处理定时任务；

##### （2）业务模块类的实现方案：

​		MsgQueue类：支持放入消息和取出消息，取出消息时会优先弹出优先级高的消息；

​		ModulBase类：

​				a. 模块ID是模块的唯一标识；

​				b. 将模块Run()函数注册给管理模块，为模块线程的入口函数；

​				c. 每个业务模块类维护一个自己的消息队列，并将投递消息的接口注册给模块管理；

​				d. 向管理模块注册定时任务；

​		ModulTest类：继承ModulBase类，并实现基类的虚方法；

![image-20211010174858136](C:\Users\86187\AppData\Roaming\Typora\typora-user-images\image-20211010174858136.png)



------


#### 1.5 模块管理类的实现

##### （1）模块管理类支持：

- [ ] 支持业务模块的注册和注销
- [ ] 支持模块间消息的路由
- [ ] 支持模块注册定时任务

##### （2）模块管理类的实现方案：

![image-20211010174528533](C:\Users\86187\AppData\Roaming\Typora\typora-user-images\image-20211010174528533.png)



------

#### 1.6 基于消息的模块管理系统的总体实现

![image-20211010174231409](C:\Users\86187\AppData\Roaming\Typora\typora-user-images\image-20211010174231409.png)



------

### 2. 基于协程的webserver服务系统的实现

​		暂时借鉴学习，完成或正在实现以下几个部分：

​		（1）完成了协程的封装：linux环境基于ucontext实现，windows环境基于纤程实现

​		（2）实现了协程调度器：在N个线程和M个协程之间进行协程的调度

​		（3）socket基础函数封装：主要用作练习，后期要集成到server，需重新进行封装

​		（4）内存管理的实现：进行后暂时中断了，后期继续实现

​		（5）对象池的实现：暂未执行

​		（6）.....





------

### 3. gdres的目录结构说明

​		gdres通过CMake组织编译，支持windows和Linux环境下运行。

​		在不同环境下直接执行根目录下的编译脚本build.sh文件实现编译（当前编译效率不太满意，后期需对cmake进行优化）

​		目录结构说明如下：

~~~
bin    ： 放置生成的可执行文件及动态库文件，测试运行中日志文件默认在该目录中生成
build  ： 编译过程生成的中间件在该目录中
lib    ： 生成的静态库文件在此处
src    ： 源文件目录
~~~





























​			



