# TD n° 2 - 27 Janvier 2025

##  1. Parallélisation ensemble de Mandelbrot

L'ensensemble de Mandebrot est un ensemble fractal inventé par Benoit Mandelbrot permettant d'étudier la convergence ou la rapidité de divergence dans le plan complexe de la suite récursive suivante :
$$
\left\{
\begin{array}{l}
    c\,\,\textrm{valeurs\,\,complexe\,\,donnée}\\
    z_{0} = 0 \\
    z_{n+1} = z_{n}^{2} + c
\end{array}
\right.
$$
dépendant du paramètre $c$.

Il est facile de montrer que si il existe un $N$ tel que $\mid z_{N} \mid > 2$, alors la suite $z_{n}$ diverge. Cette propriété est très utile pour arrêter le calcul de la suite puisqu'on aura détecter que la suite a divergé. La rapidité de divergence est le plus petit $N$ trouvé pour la suite tel que $\mid z_{N} \mid > 2$.

On fixe un nombre d'itérations maximal $N_{\textrm{max}}$. Si jusqu'à cette itération, aucune valeur de $z_{N}$ ne dépasse en module 2, on considère que la suite converge.

L'ensemble de Mandelbrot sur le plan complexe est l'ensemble des valeurs de $c$ pour lesquels la suite converge.

Pour l'affichage de cette suite, on calcule une image de $W\times H$ pixels telle qu'à chaque pixel $(p_{i},p_{j})$, de l'espace image, on associe une valeur complexe  $c = x_{min} + p_{i}.\frac{x_{\textrm{max}}-x_{\textrm{min}}}{W} + i.\left(y_{\textrm{min}} + p_{j}.\frac{y_{\textrm{max}}-y_{\textrm{min}}}{H}\right)$. Pour chacune des valeurs $c$ associées à chaque pixel, on teste si la suite converge ou diverge.

- Si la suite converge, on affiche le pixel correspondant en noir
- Si la suite diverge, on affiche le pixel avec une couleur correspondant à la rapidité de divergence.

1. À partir du code séquentiel `mandelbrot.py`, faire une partition équitable par bloc suivant les lignes de l'image pour distribuer le calcul sur `nbp` processus  puis rassembler l'image sur le processus zéro pour la sauvegarder. Calculer le temps d'exécution pour différents nombre de tâches et calculer le speedup. Comment interpréter les résultats obtenus ?  

 nbThreads | temps calcul de l'ensemble | temps constitution de l'image
 ----------|----------------------------|------------------------------
  1        | 2.7888474464416504         | 0.04216599464416504
  2        | 1.4285612106323242         | 0.048128366470336914
  3        | 0.9951529502868652         | 0.049051761627197266
  4        | 0.7784037590026855         | 0.05345034599304199
  5        | 0.9519078731536865         | 0.05475974082946777
  6        | 0.5735557079315186         | 0.06935572624206543
  7        | 0.6897034645080566         | 0.05202174186706543
  8        | 0.5875773429870605         | 0.05259251594543457  

Nous pouvons remarquer que le temps de calcul de l'ensemble diminue quand on rajoute des threads. Par contre, celui pour la constitution de l'image augmente: en effet, plus on découpe l'image en un nombre important de paquets, plus la constitution prendra du temps. Il faudra rassembler plus de paquets. Cependant, l'augmentation du temps dû à cette constitution reste négligeable par rapport au temps nécessaire pour calculer l'ensemble.  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp2/images/Mandelbrot_Q1.png)

2. Réfléchissez à une meilleur répartition statique des lignes au vu de l'ensemble obtenu sur notre exemple et mettez la en œuvre. Calculer le temps d'exécution pour différents nombre de tâches et calculer le speedup et comparez avec l'ancienne répartition. Quel problème pourrait se poser avec une telle stratégie ?  

Lorsque nous calculons l'ensemble de Mandelbrot, tous les pixels ne demandent pas forcément la même quantité de calcul. En effet, selon la vitesse de divergence ou de convergence des suites, il y aura un nombre plus ou moins important d'itérations à effectuer, et donc d'opérations à exécuter. Ainsi, les différents threads ont des tâches de durée inégales à exécuter. Une meilleure répartition statique des lignes permettrait alors de s'assurer que la durée des tâches à exécuter soit la même pour chaque thread.  
Comme il y a une certaine continuité dans l'image représentant l'ensemble de Mandelbrot, je propose de répartir les lignes en alternant entre chaque thread:  
la ligne 1 est prise par le thread 1  
la ligne 2 est prise par le thread 2  
etc..  
la ligne nbp+1 est prise par le thread 1  
la ligne nbp21 est prise par le thread 2  
etc..  
Cela permet de s'assurer que les tâches exécutées par les threads sont de durée égales. Voyons si cela est efficace:  

 nbThreads | temps calcul de l'ensemble | temps constitution de l'image
 ----------|----------------------------|------------------------------
  1        | 2.7279422283172607         | 0.05819845199584961
  2        | 1.4280922412872314         | 0.06397175788879395
  3        | 1.476961374282837          | 0.0612490177154541
  4        | 0.7343153953552246         | 0.05973482131958008
  5        | 0.6141350269317627         | 0.061002492904663086
  6        | 0.5107924938201904         | 0.06510758399963379
  7        | 0.6348967552185059         | 0.058724403381347656
  8        | 0.5105395317077637         | 0.05483412742614746

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp2/images/Mandelbrot_Q2.png)  

Visiblement, la nouvelle stratégie n'a pas été aussi efficace que ce que j'espérais. Nous avons une légère amélioration du speedup, cependant la différence est assez négligeable. 

3. Mettre en œuvre une stratégie maître-esclave pour distribuer les différentes lignes de l'image à calculer. Calculer le speedup avec cette approche et comparez  avec les solutions différentes. Qu'en concluez-vous ?

## 2. Produit matrice-vecteur

On considère le produit d'une matrice carrée $A$ de dimension $N$ par un vecteur $u$ de même dimension dans $\mathbb{R}$. La matrice est constituée des cœfficients définis par $A_{ij} = (i+j) \mod N$. 

Par soucis de simplification, on supposera $N$ divisible par le nombre de tâches `nbp` exécutées.

### a - Produit parallèle matrice-vecteur par colonne

Afin de paralléliser le produit matrice–vecteur, on décide dans un premier temps de partitionner la matrice par un découpage par bloc de colonnes. Chaque tâche contiendra $N_{\textrm{loc}}$ colonnes de la matrice. 

- Calculer en fonction du nombre de tâches la valeur de Nloc
- Paralléliser le code séquentiel `matvec.py` en veillant à ce que chaque tâche n’assemble que la partie de la matrice utile à sa somme partielle du produit matrice-vecteur. On s’assurera que toutes les tâches à la fin du programme contiennent le vecteur résultat complet.
- Calculer le speed-up obtenu avec une telle approche

### b - Produit parallèle matrice-vecteur par ligne

Afin de paralléliser le produit matrice–vecteur, on décide dans un deuxième temps de partitionner la matrice par un découpage par bloc de lignes. Chaque tâche contiendra $N_{\textrm{loc}}$ lignes de la matrice.

- Calculer en fonction du nombre de tâches la valeur de Nloc
- paralléliser le code séquentiel `matvec.py` en veillant à ce que chaque tâche n’assemble que la partie de la matrice utile à son produit matrice-vecteur partiel. On s’assurera que toutes les tâches à la fin du programme contiennent le vecteur résultat complet.
- Calculer le speed-up obtenu avec une telle approche

## 3. Entraînement pour l'examen écrit

Alice a parallélisé en partie un code sur machine à mémoire distribuée. Pour un jeu de données spécifiques, elle remarque que la partie qu’elle exécute en parallèle représente en temps de traitement 90% du temps d’exécution du programme en séquentiel.

En utilisant la loi d’Amdhal, pouvez-vous prédire l’accélération maximale que pourra obtenir Alice avec son code (en considérant n ≫ 1) ?

À votre avis, pour ce jeu de donné spécifique, quel nombre de nœuds de calcul semble-t-il raisonnable de prendre pour ne pas trop gaspiller de ressources CPU ?

En effectuant son cacul sur son calculateur, Alice s’aperçoit qu’elle obtient une accélération maximale de quatre en augmentant le nombre de nœuds de calcul pour son jeu spécifique de données.

En doublant la quantité de donnée à traiter, et en supposant la complexité de l’algorithme parallèle linéaire, quelle accélération maximale peut espérer Alice en utilisant la loi de Gustafson ?

