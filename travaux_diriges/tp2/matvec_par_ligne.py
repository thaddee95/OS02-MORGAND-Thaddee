# Produit matrice-vecteur v = A.u
import numpy as np
from mpi4py import MPI
from time import time

globCom = MPI.COMM_WORLD.Dup()
nbp     = globCom.size
rank    = globCom.rank

# Dimension du problème (peut-être changé)
dim = 840
Nloc=dim//nbp
# Initialisation de la matrice
A_loc = np.array([[(i+j+rank*Nloc) % dim+1. for i in range(dim)] for j in range(Nloc)])
print(f"A_loc = {A_loc}")

# Initialisation du vecteur u
u = np.array([i+1. for i in range(dim)])
print(f"u = {u}")

# Produit matrice-vecteur
v=np.empty(dim,dtype=np.double)
deb=time()
v_loc = A_loc.dot(u)
globCom.Allgather(v_loc,v)
fin=time()
print(f"v = {v}")
globCom.Barrier()
if (rank==0):
    print(f"Temps du calcul du produit matrice-vecteur par lignes : {fin-deb}")