# Calcul de l'ensemble de Mandelbrot en python
import numpy as np
from dataclasses import dataclass
from PIL import Image
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

globCom = MPI.COMM_WORLD.Dup()
nbp     = globCom.size
rank    = globCom.rank
name    = MPI.Get_processor_name()

# On peut changer les paramètres des deux prochaines lignes
mandelbrot_set = MandelbrotSet(max_iterations=50, escape_radius=10)
width, height = 1024,1024
#width, height = 10,10

scaleX = 3./width
scaleY = 2.25/height
convergence_local=np.empty((width, height//nbp), dtype=np.double)
print(type(convergence_local),convergence_local.shape)
# Calcul de l'ensemble de mandelbrot :
deb = time()
if (rank==0):
    szBlock=height-(height//nbp)*(nbp-1)
else:
    szBlock=height//nbp
for y_local in range(szBlock):
    for x in range(width):
        y_reel=y_local+rank*height/nbp
        c = complex(-2. + scaleX*x, -1.125 + scaleY * y_reel)
        convergence_local[x, y_local] = mandelbrot_set.convergence(c, smooth=True)
convergence_local=convergence_local.T
image_local = Image.fromarray(np.uint8(matplotlib.cm.plasma(convergence_local)*255))
image_local.show()
convergence_global=None
if (rank==0):
    convergence_global = np.empty((width, height), dtype=np.double)
globCom.Gather([convergence_local,MPI.DOUBLE],[convergence_global,MPI.DOUBLE],root=0)

fin = time()

if (rank==0):
    print(type(convergence_global),convergence_global.dtype,convergence_global.shape)
    print(f"Temps du calcul de l'ensemble de Mandelbrot : {fin-deb}")

# Constitution de l'image résultante :
if (rank==0):
    deb = time()
    image = Image.fromarray(np.uint8(matplotlib.cm.plasma(convergence_global.T)*255))
    fin = time()
    print(f"Temps de constitution de l'image : {fin-deb}")
    image.show()
    image.save("images/Mandel_MPI.png")
