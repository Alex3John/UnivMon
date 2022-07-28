# UnivMon
Cardinality , Entropy and Heavy hitter
主要思路是使用多Sketch，每个Sketch实现为一个CountHeap，用来记录频繁项，UnivMon首先计算L2 HH，然后从得到的L2 HH中反推得到HH。
