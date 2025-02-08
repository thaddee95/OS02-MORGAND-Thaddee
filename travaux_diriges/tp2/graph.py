import matplotlib.pyplot as plt

x=[0 for i in range(8)]
x[0]=2.7888474464416504
x[1]=1.4285612106323242
x[2]=0.9951529502868652
x[3]=0.7784037590026855
x[4]=0.9519078731536865
x[5]=0.5735557079315186
x[6]=0.6897034645080566
x[7]=0.5875773429870605

speedup=[x[0]/x[i] for i in range(8)]
asymptote=[x[0]*(i+1) for i in range(8)]
nbThreads=[i+1 for i in range(8)]

plt.plot(nbThreads,asymptote,color='red')
plt.plot(nbThreads,speedup,color='green')
plt.savefig("images/Mandelbrot_Q1.png")
plt.show()