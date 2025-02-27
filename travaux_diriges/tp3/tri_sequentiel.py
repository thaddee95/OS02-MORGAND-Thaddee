import numpy as np
from time import time

def tri_insertion(L):
    N = len(L)
    for n in range(1,N):
        elemt = L[n]
        j = n-1
        while j>=0 and L[j] > elemt:
            L[j+1] = L[j]
            j = j-1
        L[j+1] = elemt

dim=1000
values=np.random.rand(dim)
for i in range(dim):
    values[i]=values[i]**4
deb=time()
tri_insertion(values)
fin=time()
print(f"Temps de tri de la liste : {fin-deb}\n")