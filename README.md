# cuda_word_split
This project is an old code for Chinese words split. It is written by CUDA at 2010, so it could not run well directly under you platform without an GPU card. 
郑重提示：
本开源是作者研究生期间所做，是国内外原创，全部知识产权归华中科技大学智能与分布式计算实验室所有。
最新的代码和改进算法，请联系华中科技大学计算机学院《智能与分布式计算实验室》李瑞轩教授！
下面公开的算法和说明，与作者目前所在公司不存在任何知识产权纠纷和泄密等问题。
 
任何个人、团队、研究结构、商业结构可以对本算法进行使用、修改、扩展、传播。 使用本算法不当而造成的损失，责任有使用者自行负责。
请在你的论文撰写或者技术研讨交流中，如果引用或者参考过本项目思路或者源码，建议做引用说明，非常感谢！


1. 实验结果
   Microsoft Windows Server 2003 Intel(R) Pentium(R) Dual CPU E2180 @2.00GHz 2.00GHz,2.0Gb memory
GTS250,CUDA3.0,Microsoft Visual C++ 2008 Expression Edition

64 thread perDoc
docNum | all_global  all_blend  | max_global  max_blend   | min_global    min_blend   | size        |
    48 | 44.48MB/s   47.618MB/s | 53.36MB/s   56.348MB/s  | 58.918MB/s    62.106MB/s  | 119104 words|
    96 | 77.738MB/s  89.66MB/s  | 104.55MB/s  121.071MB/s | 127.0456MB/s  146.038MB/s | 473990 words|
   112 | 78.79MB/s   91.848MB/s | 108.95MB/s  126.95MB/s  | 133.917MB/s   154.876MB/s | 559031 words|
   128 | 80.435MB/s  93.03MB/s  | 112.57MB/s  129.97MB/s  | 141.1996MB/s  161.439MB/s | 644470 words|

128 threead/perDoc
donNum | all_global   all_blend | max_global   max_blend  | min_global    min_blend   | size
    48 | 62.14MB/s    56.71MB/s | 79.26MB/s    72.22MB/s  | 91.268MB/s    79.08MB/s   | 119104 words|
    96 | 86.00MB/s    100.06MB/s| 120.32MB/s   138.176MB/s| 155.29MB/s    170.42MB/s  | 473990 words|
   112 | 83.19MB/s    91.87MB/s | 113.437MB/s  132.478MB/s| 138.755MB/s   160.086MB/s | 559031 words|
   128 | 84.68MB/s    90.08MB/s | 116.90MB/s   137.557MB/s| 144.765MB/s   166.77MB/s  | 644470 words|

CPU
 docNum=1, size= 935063 bytes
 all  27.86MB/s              
 max  55.73MB/s 
 min  55.73Mb/s

另外，昨早公开的文档链接 http://blog.sina.com.cn/s/blog_4d58e3c001010ufv.html
