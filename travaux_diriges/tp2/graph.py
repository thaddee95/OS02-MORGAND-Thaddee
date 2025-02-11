import matplotlib.pyplot as plt
import numpy as np

x=[0 for i in range(8)]
x[0]=0.02459120750427246
x[1]=0.020023822784423828
x[2]=0.018400192260742188
x[3]=0.012806892395019531
x[4]=0.0025763511657714844
x[5]=0.03521919250488281
x[6]=0.019542694091796875
x[7]=0.003913402557373047

speedup=[x[0]/x[i] for i in range(8)]
asymptote=[i+1 for i in range(8)]
nbThreads=[i+1 for i in range(8)]

plt.plot(nbThreads,asymptote,color='red')
plt.plot(nbThreads,speedup,color='green')
plt.title("Speedup Produit Matrice-vecteur par lignes")
plt.xlabel("Nombre de Threads")
plt.ylabel("Speedup")
plt.xticks(np.arange(1,9,1))
plt.yticks(np.arange(0,asymptote[7],1))
plt.grid()
plt.savefig("images/Produit par lignes.png")
plt.show()