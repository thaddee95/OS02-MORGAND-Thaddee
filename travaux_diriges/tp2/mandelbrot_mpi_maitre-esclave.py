# Calcul de l'ensemble de Mandelbrot en python
import numpy as np
from dataclasses import dataclass
from PIL import Image
from math import ceil
from math import log
from time import time
import matplotlib.cm
from mpi4py import MPI

@dataclass
class MandelbrotSet:
    max_iterations: int
    escape_radius:  float = 2.0

    def __contains__(self, c: complex) -> bool:
        return self.stability(c) == 1

    def convergence(self, c: complex, smooth=False, clamp=True) -> float:
        value = self.count_iterations(c, smooth)/self.max_iterations
        return max(0.0, min(value, 1.0)) if clamp else value

    def count_iterations(self, c: complex,  smooth=False) -> int:
        z:    complex
        iter: int

        # On vérifie dans un premier temps si le complexe
        # n'appartient pas à une zone de convergence connue :
        #   1. Appartenance aux disques  C0{(0,0),1/4} et C1{(-1,0),1/4}
        if c.real*c.real+c.imag*c.imag < 0.0625:
            return self.max_iterations
        if (c.real+1)*(c.real+1)+c.imag*c.imag < 0.0625:
            return self.max_iterations
        #  2.  Appartenance à la cardioïde {(1/4,0),1/2(1-cos(theta))}
        if (c.real > -0.75) and (c.real < 0.5):
            ct = c.real-0.25 + 1.j * c.imag
            ctnrm2 = abs(ct)
            if ctnrm2 < 0.5*(1-ct.real/max(ctnrm2, 1.E-14)):
                return self.max_iterations
        # Sinon on itère
        z = 0
        for iter in range(self.max_iterations):
            z = z*z + c
            if abs(z) > self.escape_radius:
                if smooth:
                    return iter + 1 - log(log(abs(z)))/log(2)
                return iter
        return self.max_iterations

# Definition d'une tâche prenant un sous paquet de samples à traiter :
def mandelbrot_task(iPack : int, nbPacks : int, packSize : int, maxIter : int, width : int, height : int ):
    # print("Tâche ",iPack,"\n")
    y_begin=iPack*packSize
    scaleX = 3./width
    scaleY = 2.25/height
    image = np.zeros((width, height),dtype=np.double)
    mandelbrot_set = MandelbrotSet(max_iterations=maxIter,escape_radius=10)
    packSizeRest = height%packSize
    y_end=y_begin+packSize
    if (iPack==nbPacks-1) and (packSizeRest!=0):
        y_end=y_begin+packSizeRest
    for y in range(y_begin,y_end):
        for x in range(width):
            c = complex(-2. + scaleX*x, -1.125 + scaleY * y)
            image[x, y] += mandelbrot_set.convergence(c, smooth=True)
    return image

# Mandelbrot to test the chronometer
def mandelbrot ( maxIter : int, width : int, height : int, comm : MPI.Comm ):
    packSize = 40 # on fait des paquets de lignes
    # Attention: il faut qu'il y ait plus de paquets que de threads
    nbp      = comm.size
    rank     = comm.rank
    nbPacks = ceil(height/packSize)
    image     = np.zeros((width, height),dtype=np.double)
    image_loc = np.zeros((width, height),dtype=np.double)
    # Algorithme maître-escalve :
    if rank==0: # Algorithme maître distribuant les tâches

        iPack : int = 0
        for iProc in range(1,nbp):
            comm.send(iPack, iProc)
            iPack += 1
        stat : MPI.Status = MPI.Status()
        while iPack < nbPacks:
            done = comm.recv(status=stat)# On reçoit du premier process à envoyer un message
            slaveRk = stat.source
            comm.send(iPack, dest=slaveRk)
            iPack += 1
        iPack = -1 # iPack vaut maintenant -1 pour signaler aux autres procs qu'il n'y a plus de tâches à exécuter
        for iProc in range(1,nbp):
            status = MPI.Status()
            done = comm.recv(status=status)# On reçoit du premier process à envoyer un message
            slaveRk : int = status.source
            comm.send(iPack, dest=slaveRk)
        comm.Reduce([image_loc,MPI.INT64_T], [image,MPI.INT64_T], op=MPI.SUM, root=0)
    else:
        status : MPI.Status = MPI.Status()
        iPack : int
        res   : int = 1

        iPack = comm.recv(source=0) # On reçoit un n° de tâche à effectuer
        while iPack != -1:          # Tant qu'il y a une tâche à faire
            image_loc = mandelbrot_task(iPack, nbPacks, packSize, maxIter, width, height )
            req : MPI.Request = comm.isend(res,0)
            image += image_loc
            iPack = comm.recv(source=0) # On reçoit un n° de tâche à effectuer
            req.wait()
        comm.Reduce([image,MPI.INT64_T], None, op=MPI.SUM, root=0)
    return image

globCom = MPI.COMM_WORLD.Dup()
nbp     = globCom.size
rank    = globCom.rank
name    = MPI.Get_processor_name()

# On peut changer les paramètres des deux prochaines lignes
width, height = 1024,1024

# Calcul de Mandelbrot
deb = time()
image=mandelbrot(50, width , height , globCom)
fin = time()

if (rank==0):
    print(f"Temps du calcul de l'ensemble de Mandelbrot avec une stratégie maître-esclave : {fin-deb}")
    # Constitution de l'image résultante :
    deb = time()
    image = Image.fromarray(np.uint8(matplotlib.cm.plasma(image.T)*255))
    fin = time()
    print(f"Temps de constitution de l'image : {fin-deb}")
    image.show()
    image.save("images/Mandel_MPI_Maitre-esclave.png")