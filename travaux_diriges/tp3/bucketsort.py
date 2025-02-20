import numpy as np
import time
from mpi4py import MPI
from math import floor


globCom = MPI.COMM_WORLD.Dup()
nbp     = globCom.size
rank    = globCom.rank
name    = MPI.Get_processor_name()

# Bucket sort
dim=100
local_size=dim//nbp
local_values=np.random.rand(local_size)
print(f"Valeurs locales initiales pour le rang {rank}: {local_values}\n")
local_values.sort()

min_global=globCom.allreduce(local_values[0],op=MPI.MIN)
max_global=globCom.allreduce(local_values[-1],op=MPI.MAX)

buckets = np.linspace( min_global, max_global, nbp+1)
my_bucket = buckets[rank:rank+2]

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

sorted_list=None
if (rank==0):
    sorted_list=np.empty(local_size*nbp,dtype=np.float64)
globCom.Gatherv(local_values2,[sorted_list,loc_sizes],root=0)
if (rank==0):
    print(f"Liste triée : {sorted_list}\n")