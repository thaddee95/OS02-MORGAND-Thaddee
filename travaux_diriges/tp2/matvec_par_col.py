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
A_loc = np.array([[((i+rank*Nloc)+j) % dim+1. for i in range(Nloc)] for j in range(dim)])
print(f"A_loc = {A_loc}")

# Initialisation du vecteur u
u_loc = np.array([i+rank*Nloc+1. for i in range(Nloc)])
print(f"u_loc = {u_loc}")

# Produit matrice-vecteur
v=np.empty(dim,dtype=np.double)
deb=time()
v_loc = A_loc.dot(np.transpose(u_loc))
globCom.Allreduce([v_loc,MPI.DOUBLE], [v,MPI.DOUBLE], MPI.SUM)
fin=time()
print(f"v = {v}")
globCom.Barrier()
if (rank==0):
    print(f"Temps du calcul du produit matrice-vecteur par colonnes : {fin-deb}")