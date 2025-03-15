import matplotlib.pyplot as plt
import numpy as np

x=[0 for i in range(8)]
x[0]=30.448
x[1]=26.0465
x[2]=23.2708
x[3]=27.4318
x[4]=23.2564
x[5]=26.061
x[6]=26.1329
x[7]=27.8038 

speedup=[x[0]/x[i] for i in range(8)]
asymptote=[i+1 for i in range(8)]
nbThreads=[i+1 for i in range(8)]

plt.plot(nbThreads,asymptote,color='red')
plt.plot(nbThreads,speedup,color='green')
plt.title("Speedup Etape 3 Temps total")
plt.xlabel("Nombre de Threads")
plt.ylabel("Speedup")
plt.xticks(np.arange(1,9,1))
plt.yticks(np.arange(0,asymptote[7],1))
plt.grid()
plt.savefig("images/Q3_Temps_total.png")
plt.show()