import matplotlib.pyplot as plt
import numpy as np

x=[0 for i in range(8)]
x[0]=0.0014493465423583984
x[1]=0.004538536071777344
x[2]=0.0032012462615966797
x[3]=0.0057561397552490234
x[4]=0.00403594970703125
x[5]=0.002230405807495117
x[6]=0.002183198928833008
x[7]=0.0027344226837158203

speedup=[x[0]/x[i] for i in range(8)]
asymptote=[i+1 for i in range(8)]
nbThreads=[i+1 for i in range(8)]

plt.plot(nbThreads,asymptote,color='red')
plt.plot(nbThreads,speedup,color='green')
plt.title("Speedup Tri buckets avec quantiles")
plt.xlabel("Nombre de Threads")
plt.ylabel("Speedup")
plt.xticks(np.arange(1,9,1))
plt.yticks(np.arange(0,asymptote[7],1))
plt.grid()
plt.savefig("images/Buckets avec quantiles.png")
plt.show()