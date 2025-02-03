
# TD1

`pandoc -s --toc README.md --css=./github-pandoc.css -o README.html`

## lscpu

*lscpu donne des infos utiles sur le processeur : nb core, taille de cache :*

```
CPU(s):                   16
    Thread(s) per core:   2
    Core(s) per socket:   8
    Socket(s):            1
Caches (sum of all):      
  L1d:                    256 KiB (8 instances)
  L1i:                    256 KiB (8 instances)
  L2:                     4 MiB (8 instances)
  L3:                     4 MiB (1 instance)
```


## Produit matrice-matrice

### Effet de la taille de la matrice

  n            | MFlops  | time
---------------|---------|---------
1024 (origine) | 59.7862 | 35.9194
1023           | 651.215 | 3.28801
1025           | 660.288 | 3.26188
1026           | 873.928 | 2.4717

*Expliquer les résultats.*


### Permutation des boucles

*Expliquer comment est compilé le code (ligne de make ou de gcc) : on aura besoin de savoir l'optim, les paramètres, etc. Par exemple :*

`make TestProduct.exe && ./TestProduct.exe 1024`


  ordre           | time     | MFlops  | time(n=2048)| MFlops(n=2048)
------------------|----------|---------|-------------|----------------
i,j,k (origine)   | 9.45441  | 227.141 | 186.454     | 92.1397
j,i,k             | 10.1557  | 211.457 | 182.204     | 94.289
i,k,j             | 36.0308  | 59.6013 | 247.34      | 69.4585
k,i,j             | 36.7611  | 58.4172 | 247.502     | 69.4132
j,k,i             | 0.533584 | 4024.15 | 4.25019     | 4042.15
k,j,i             | 1.03374  | 2077.4  | 8.28672     | 2073.18


*Discuter les résultats.*



### OMP sur la meilleure boucle

`make TestProduct.exe && OMP_NUM_THREADS=8 ./TestProduct.exe 1024`

  OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 | 2781.52 | 2635.51        | 3543.13        | 2530.75
2                 | 5149.2  | 4864.32        | 7287.85        | 4136.93
3                 | 6713.52 | 6993.19        | 13200.5        | 6657.09
4                 | 9140.42 | 7825.27        | 3451.98        | 6661.67
5                 | 7812.16 | 3636.8         | 6265.38        | 9262.44
6                 | 8403.45 | 10566.1        | 5841.05        | 8999.85
7                 | 9539.93 | 11204.8        | 4550.79        | 10946.9
8                 | 9303.44 | 3955.92        | 4574.75        | 10961.5

  OMP_NUM         | time     | time(n=2048)   | time(n=512)    | time(n=4096)
------------------|----------|----------------|----------------|---------------
1                 | 0.772055 | 6.51862        | 0.0757623      | 54.3076
2                 | 0.417052 | 3.53182        | 0.0368333      | 33.2225
3                 | 0.319875 | 2.45666        | 0.0203353      | 20.6455
4                 | 0.234944 | 2.19543        | 0.0777627      | 20.6313
5                 | 0.27489  | 4.72389        | 0.0428442      | 14.8383
6                 | 0.255548 | 1.62594        | 0.0459567      | 15.2712
7                 | 0.225105 | 1.53326        | 0.0589865      | 12.5551
8                 | 0.230827 | 4.34282        | 0.0586776      | 12.5384

*Tracer les courbes de speedup (pour chaque valeur de n), discuter les résultats.*



### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    |
32                |
64                |
128               |
256               |
512               |
1024              |

*Discuter les résultats.*



### Bloc + OMP


  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)|
---------------|---------|---------|----------------|----------------|---------------|
1024           |  1      |         |                |                |               |
1024           |  8      |         |                |                |               |
512            |  1      |         |                |                |               |
512            |  8      |         |                |                |               |

*Discuter les résultats.*


### Comparaison avec BLAS, Eigen et numpy

*Comparer les performances avec un calcul similaire utilisant les bibliothèques d'algèbre linéaire BLAS, Eigen et/ou numpy.*


# Tips

```
	env
	OMP_NUM_THREADS=4 ./produitMatriceMatrice.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```
