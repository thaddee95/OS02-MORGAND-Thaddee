import numpy as np
from time import time
from mpi4py import MPI
from math import ceil


globCom = MPI.COMM_WORLD.Dup()
nbp     = globCom.size
rank    = globCom.rank
name    = MPI.Get_processor_name()

# Bucket sort
dim=1000
local_size=dim//nbp
local_values=np.random.rand(local_size)
for i in range(local_size):
    local_values[i]=local_values[i]**4
print(f"Valeurs locales initiales pour le rang {rank}: {local_values}\n")
deb=time()
local_values.sort()

quantile_loc = np.empty(nbp+1,dtype=np.float64)
quantile_loc[0] = local_values[0]
for q in range(1,nbp):
    index = ceil(local_size*q/nbp)-1
    quantile_loc[q] = local_values[index]
quantile_loc[-1] = local_values[-1]

quantile_glob = np.empty(nbp*(nbp+1),dtype=np.float64)
globCom.Allgather(quantile_loc,quantile_glob)
# print(f"Quantiles locaux trouvés: {quantile_glob}\n")

quantile_glob.sort()
buckets = np.empty(nbp+1,dtype=np.float64)
buckets[0] = quantile_glob[0]
for q in range(1,nbp):
    index = ceil((nbp+1)*q)-1
    buckets[q] = quantile_glob[index]
buckets[-1] = quantile_glob[-1]

dest=[0 for i in range(local_size)]
local_source=np.zeros((nbp,nbp))
for i in range(local_size):
    for i_bucket in range(nbp):
        if (local_values[i]>=buckets[i_bucket]) and (local_values[i]<buckets[i_bucket+1]):
            dest[i]=i_bucket
            local_source[i_bucket,rank]+=1
    if (local_values[i]==buckets[-1]):
        dest[i]=nbp-1
        local_source[nbp-1,rank]+=1

global_source=np.zeros((nbp,nbp))
globCom.Allreduce([local_source,MPI.INT],[global_source,MPI.INT],op=MPI.SUM)

local_values2=[]
for i in range(local_size):
    if (dest[i]!=rank):
        globCom.send(local_values[i],dest[i])
    else:
        local_values2.append(local_values[i])
for current_source in range(nbp):
    if (current_source!=rank):
        for i in range(int(global_source[rank,current_source])):
            temp=globCom.recv(source=current_source)
            local_values2.append(temp)

local_values2.sort()
local_values2=np.asarray(local_values2)
loc_sizes = globCom.gather(len(local_values2), root=0)
# print(f"Bucket de rang {rank}: {local_values2}\n")

sorted_list=None
if (rank==0):
    sorted_list=np.empty(local_size*nbp,dtype=np.float64)
globCom.Gatherv(local_values2,[sorted_list,loc_sizes],root=0)
fin=time()
if (rank==0):
    print(f"Liste triée : {sorted_list}\n")
    print(f"Temps de tri de la liste : {fin-deb}\n")