
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


## 1) Produit matrice-matrice

### Effet de la taille de la matrice

  n            | MFlops  | time
---------------|---------|---------
1024 (origine) | 59.7862 | 35.9194
1023           | 651.215 | 3.28801
1025           | 660.288 | 3.26188
1026           | 873.928 | 2.4717

1.1 *Expliquer les résultats.*

On remarque que la performance du produit matrice-matrice diminue drastiquement pour n=1024. Cela est du à la manière dont on accède aux différents éléments de la matrice A.  
Tous les coefficients de la matrice A vont être stockés dans la mémoire RAM. Nous pouvons voir dans le fichier Matrix.hpp comment ces coefficients sont stockés:  

  double operator() (int i, int j) const  
  {  
    return m_arr_coefs[i+j*nbRows];  
  }  

Ainsi, on commence par parcourir la 1ère colonne de haut en bas puis on passe à la suivante. On itère pour chaque colonne.
Cependant, dans l'implémentation de notre produit matrice-matrice, on parcourt la matrice A ligne par ligne et non colonne par colonne. Dès lors, un jump de 1024 cases est effectué pour accéder à l'élément suivant.  
De plus, lorsque la mémoire cache remplit une ligne de cache, elle accède à l'élément souhaité dans la mémoire RAM et effectue une opération modulo pour attribuer une adresse de cache à cet élément. Le problème est que du à ce modulo, la ligne de cache de l'élément 1024 a la même adresse que celle du 1er élément. La ligne de cache du 1er élément est donc supprimé. Ainsi, la même ligne de cache est à chaque fois remplie avec de nouvelles valeurs, ce qui fait qu'on ne garde pas beaucoup d'éléments dans la mémoire cache. Il y aura donc beaucoup d'accès aux éléments qui nécessiteront d'aller accéder à la mémoire RAM, or cet accès est moins rapide.

Ce problème n'apparaît plus lorsque la dimension est différente de 1024. En effet, après l'opération de modulo, la ligne de cache de l'élément suivant obtient une nouvelle adresse. Dès lors, la mémoire cache peut stocker bien plus d'éléments. Les accès aux éléments seront donc plus rapide car beaucoup plus d'éléments seront gardés en mémoire cache.

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


1.2 *Discuter les résultats.*

Nous pouvons constater que le meilleur résultat est obtenu pour l'ordre j,k,i. Dans cet ordre, les deux matrices A et B sont parcourues colonne par colonne. Or, commme nous l'avons vu, les coefficients des matrices sont stockés colonne par colonne. Ainsi, avec cet ordre nous pouvons éviter les problèmes rencontrés ci-dessus. Lors du remplissage des lignes de cache, l'opération modulo aboutira moins souvent à une adresse déjà utilisée que pour les autres ordres.  



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

1.3 *Tracer les courbes de speedup (pour chaque valeur de n), discuter les résultats.*

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q3%20n%3D1024.png)  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q3%20n%3D2048.png)  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q3%20n%3D512.png)  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q3%20n%3D4096.png)  

Nous pouvons constater que globalement, utiliser plusieurs coeurs au lieu d'un seul permet d'améliorer la performance de notre produit matriciel. En effet, en utilisant plusieurs coeurs, nous pouvons parlléliser le programme en mémoire partagée, ce qui permet de faire plus de calculs en même temps. Cependant, au bout d'un certain nombre de coeurs, nous n'obtenons plus de gain significatif en continuant de rajouter des coeurs. Cela est peut-être du au fait que la gestion des threads prend trop de temps lorsqu'un nombre important de coeurs est utilisé.  

1.4 Il est sûrement possible d'améliorer les résultats obtenus puisque jusqu'à maintenant, nous avons utiliser un algorithme assez naïf pour effectuer le produit matrice-matrice. D'autres algorithmes plus efficaces existent. Dans la suite du TP, nous allons utiliser le produit matriciel par blocs.  

### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    | 2337.38 | 2518.04        | 3434.94        | 2188.18
32                | 578.513 | 3480.38        | 3492.44        | 1934.75
64                | 3566.63 | 1950.08        | 3755.39        | 2042.27
128               | 3384.91 | 3377.11        | 3429.89        | 2263.29
256               | 4050.96 | 3898.61        | 4336.79        | 2575.59
512               | 2008.94 | 1404.88        | 3067.57        | 2016.78
1024              | 2530.45 | 2123.28        | None           | 2079.34


  szBlock         | time     | time(n=2048)   | time(n=512)    | time(n=4096)
------------------|----------|----------------|----------------|---------------
origine (=max)    | 0.918759 | 6.82271        | 0.0781486      | 62.8097
32                | 3.71207  | 4.9362         | 0.0768663      | 71.0371
64                | 0.602105 | 8.80984        | 0.0714801      | 67.2971
128               | 0.634429 | 5.08716        | 0.0782636      | 60.7253
256               | 0.530117 | 4.40667        | 0.0618973      | 53.3622
512               | 1.06896  | 12.2287        | 0.0875075      | 68.1478
1024              | 0.848655 | 8.09118        | None           | 66.0974

1.5 Nous pouvons voir que pour toutes les dimensions testées, le meilleur résultat est obtenu pour une taille de bloc de 256.  

1.6 *Discuter les résultats.*

Nous constatons que les résultats obtenus avec le produit matriciel par blocs sont parfois mitigés. Selon la taille des blocs, les performances obtenues peuvent être meilleures ou moins bonnes que celles du produit matrice-matrice scalaire. Cela peut être expliqué par la taille de mon 1er cache. En effet, mon 1er cache est de taille 256 KiB, ce qui correspond à la taille de block optimale. Ainsi, nous pouvons garder les blocs dans la mémoire cache pour y accéder plus rapidement. Pour des blocs plus gros, la mémoire cache sera trop petite pour garder en mémoire tout un bloc.  


### Bloc + OMP


  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)|
---------------|---------|---------|----------------|----------------|---------------|
1024           |  1      | 2770.98 | 2331.41        | None           | 2046.14       |
1024           |  8      | 9566.07 | 12238.4        | None           | 9285.06       |
512            |  1      | 2611.7  | 2056.84        | 3034.65        | 2005.9        |
512            |  8      | 6351.16 | 7690.57        | 6079.77        | 8744.91       |


  szBlock      | OMP_NUM | time     | time(n=2048)   | time(n=512)    | time(n=4096)  |
---------------|---------|----------|----------------|----------------|---------------|
1024           |  1      | 0.772015 | 7.36886        | None           | 67.17         |
1024           |  8      | 0.22449  | 1.40377        | None           | 14.8022       |
512            |  1      | 0.822254 | 8.35256        | 0.0884569      | 68.5173       |
512            |  8      | 0.338124 | 2.23389        | 0.0441523      | 15.7165       |

1.7 *Discuter les résultats.*

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q7%20n%3D1024%20et%20szBlock%3D1024.png)  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q7%20n%3D1024%20et%20szBlock%3D512.png)  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20Q7%20n%3D512%20et%20szBlock%3D512.png)  

Ces résultats peuvent être expliqués avec les différents points que nous avons vus précédemment. Tout d'abord, utiliser plusieurs coeurs permet d'augmenter significativement les performances de notre produit matriciel. En effet, la parallélisation du calcul permet de gagner du temps de manière efficace. Quant à l'influence de la taille des blocs, nous avons vu plus haut que la taille 1024 donnaient des résultats légèrement meilleurs que pour la taille 512. Nous retrouvons à nouveau ce résultat avec la parallélisation en mémoire partagée.  

### Comparaison avec BLAS, Eigen et numpy

1.8 *Comparer les performances avec un calcul similaire utilisant les bibliothèques d'algèbre linéaire BLAS, Eigen et/ou numpy.*

Pour szBlock=1024, n=1024 et OMP_NUM=8 on avait obtenu MFlops=9566.07 et time=0.22449s,  
Dans les mêmes conditions mais en utilisant BLAS, on obtient MFlops=2611.38 et time=0.822357s.  
Le rapport de temps est de 3.66322 en faveur de la version précédente. La version sans BLAS est plus de 3 fois plus rapide que la version utilisant BLAS. Ce rapport est assez surprenant, normalement la bibliothèque BLAS est censée optimiser le produit matrice-matrice. Peut-être que l'influence de la parallélisation est plus importante que celle de l'optimisation fournie par BLAS.  


## 2) Parallélisation MPI

### 2.1 Circulation d'un jeton dans un anneau

Le code est contenu dans jeton.cpp.  

### 2.2 Calcul très approché de pi

J'ai utilisé 100000000 points pour chaque programme approximant pi.
J'ai tout d'abord parallélisé le programme en mémoire partagée dans le fichier calcul_pi_openmp.cpp. Voici les résultats:  

Nombre de coeurs | Valeur de pi approximée  | time
-----------------|--------------------------|---------
1                | 3.14163                  | 8.52427
2                | 3.19595                  | 10.6799
3                | 3.22487                  | 10.2832
4                | 3.2817                   | 11.1445
5                | 3.31585                  | 14.4748
6                | 3.21528                  | 12.9168
7                | 3.35955                  | 9.77158
8                | 3.35594                  | 12.809


![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20pi%20OpenMP.png)  

Comme vous pouvez le voir, la parallélisation n'a pas été efficace. Augmenter le nombre de coeurs n'a pas permi d'améliorer les performances, bien au contraire. Cela est assez surprenant, il se peut qu'il y ait une erreur dans mon code, à moins que la gestion des threads soit trop coûteuse.  
J'ai remarqué que lorsque j'utilise plusieurs coeurs, tous les 100000000 points ne sont pas générés. Cependant, je ne pense pas que cela soit la source principale de l'inefficacité de ma parallélisation. Même en générant moins de points, le programme de base permet d'obtenir de meilleurs résultats.  

J'ai également parallélisé le programme en mémoire distribuée à l'aide de MPI. On obtient les résultats suivants:

Nombre de threads | Valeur de pi approximée  | time
-----------------|--------------------------|---------
1                | 3.14163                  | 8.52427
2                | 3.14133                  | 4.8319
3                | 3.14151                  | 3.40191
4                | 3.14156                  | 5.53834
5                | 3.14173                  | 2.28216
6                | 3.14143                  | 2.00158
7                | 3.14124                  | 1.81286
8                | 3.14147                  | 1.44691


![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp1/sources/Speedup%20pi%20MPI.png)  

Avec cette parallélisation, nous parvenons à gagner en efficacité. L'accélération augmente bien, et les valeurs de pi restent satisfaisantes. Cette parallélisation semble donc être réussie.  

### 2.3 Diffusion d'un entier dans un réseau hypercube

Le code est contenu dans les fichiers hypercube_dimx.cpp.  
Concernant l'accélération je n'ai pas compris quel était le programme de base auquel on doit comparer celui demandé dans les questions.  

# Tips

```
	env
	OMP_NUM_THREADS=4 ./produitMatriceMatrice.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```
