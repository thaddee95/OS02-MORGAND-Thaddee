# Rapport de projet
# Parallélisation d'une simulation de feu de forêt

## Première étape

Le nombre de coeurs physiques sur ma machine et la taille des différentes mémoires cache s'obtient avec la commande *lscpu* :  

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

  J'ai tout d'abord mesuré les temps moyens pris pour l'affichage, l'avancement en temps et le temps total du programme séquentiel :  

Temps d'affichage moyen | Temps d'avancement en temps moyen | Temps total
------------------------|-----------------------------------|------------
0.0218935               | 0.914479                          | 30.448

Ces mesures serveront donc de référence pour la suite.  
Pour paralléliser l'avancement en temps, j'ai utilisé un tableau pour lister toutes les clefs contenues dans le dictionnaire *m_fire_front*.
Cela m'a permis de paralléliser la mise à jour de *m_vegetation_map*, qui représente la végétation.
Cependant, je n'ai pas paralléliser la mise à jour de *m_fire_map* et *next_front*. En effet, j'ai rencontré un problème et ai obtenu un segmentation fault.
L'hypothèse la plus probable est que la modification par un thread de *next_front* peut changer sa taille (notamment lorsqu'on efface une clé), taille qui serait utilisée ensuite par un autre thread, d'où l'erreur.
Lorsque je met *next_front* en privé, le segmentation fault disparaît mais évidemment, la simulation ne progresse pas car il faut pouvoir communiquer *next_front*.
Il serait envisageable de créer un dictionnaire local, puis d'effectuer une opération de réduction pour résoudre ce problème.  

Les résultats suivants sont donc obtenus en parallélisant la mise à jour de la végétation : 

Nombre de threads |Temps d'affichage moyen | Temps d'avancement en temps moyen | Temps total
------------------|------------------------|-----------------------------------|------------
1                 | 0.0222011              | 0.961778                          | 32.0187
2                 | 0.0193775              | 0.970843                          | 32.3207
3                 | 0.0192415              | 0.876211                          | 29.2893
4                 | 0.0192656              | 0.962721                          | 32.0671
5                 | 0.0219884              | 0.962152                          | 32.037
6                 | 0.0222613              | 0.983458                          | 32.7392
7                 | 0.0224599              | 0.987636                          | 32.8871
8                 | 0.022299               | 0.975182                          | 32.4777  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/projet/src/images/Q1_Avancement_en_temps.png)

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/projet/src/images/Q1_Temps_total.png)

Nous pouvons voir que cette parallélisation n'est pas très satisfaisante. De part le fait que la partie parallélisée est petite et que je dois créer un tableau contenant toutes les clés, la performance s'en retrouve dégradée.
La parallélisation fait perdre en temps au lieu d'en gagner. Le temps d'avancement en temps a légèrement augmenté. Le temps d'affichage est inchangé.  

Pour chaque partie, afin de m'assurer que les simulations obtenues sont les mêmes, je calcule la somme des 5 1ères valeurs de végétation à chaque étape.
La clé affichée est la somme totale de ces éléments à la fin de la modélisation. Cette méthode manque peut-être un peu de robustesse, mais je voulais un compromis pour ne pas perdre en temps de calcul. La clé que j'ai obtenue de cette manière est : 732360.  


## Deuxième étape

Je suis reparti de la version originale du code et j'ai mis en place l'environnement MPI. J'ai séparé l'affichage, qui sera effectué par le processus 0, du calcul qui est ici effectué par le processus 1. Le processus 1 envoie au processus 0 les grilles de végétation et de feu après les avoir calculées. Voici le résultat que j'obtient :  

Temps d'affichage moyen| Temps d'avancement en temps moyen | Temps total
-----------------------|-----------------------------------|------------
0.0209088              | 0.653228                          | 22.3772  

Cette parallélisation plus satisfaisante. Nous pouvons voir que le temps total a diminué. En rendant possible la simultanéité de l'affichage et de l'avancement en temps, nous parvenons à gagner du temps. Le temps d'affichage, lui, n'est pas affecté. Ce qui est plus surprenant est que le temps d'avancement en temps a également diminué. Le temps d'avancement en temps ne devrait pas être affecté puisque cet avancement est calculé de la même manière, je trouve donc cette amélioration un peu surprenante.  

## Troisième étape  

Pour cette étape, j'ai tout simplement combiné les deux codes que j'ai écrit dans les étapes ci-dessus. J'ai donc utilisé la parallélisation OpenMP pour le calcul de la végétation, et ai séparé l'affichage et l'avancement en temps. Personnellement je n'ai pas eu besoin de rajouter l'option *--bind-to none* pour faire fonctionner mes codes. Voici mes résultats : 

Nombre de threads |Temps d'affichage moyen | Temps d'avancement en temps moyen | Temps total
------------------|------------------------|-----------------------------------|------------
1                 | 0.0239895              | 0.767318                          | 26.0465
2                 | 0.0213066              | 0.679766                          | 23.2708
3                 | 0.02531                | 0.810582                          | 27.4318
4                 | 0.0213307              | 0.680043                          | 23.2564
5                 | 0.023966               | 0.767781                          | 26.061
6                 | 0.0243406              | 0.779905                          | 26.4797
7                 | 0.0241371              | 0.769648                          | 26.1329
8                 | 0.0256032              | 0.819275                          | 27.8038  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/projet/src/images/Q3_Avancement_en_temps.png)

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/projet/src/images/Q3_Temps_total.png)

Ces résultats peuvent être expliqués avec les parties précédentes. Nous pouvons voir en effet que la parallélisation MPI permet encore de gagner en temps, mais comparé à la 2ème étape, les résultats sont moins bons, et ce à cause de la parallélisation avec OpenMP qui n'est pas efficace.  

## Quatrième étape

La dermière étape consiste à effecter une décomposition de domaine. Le processus 0 s'occupe toujours de l'affichage, mais maintenant les autres processus se répartissent la grille pour paralléliser les calculs. Chaque processus non nul a une grille locale de plus petite taille. Ces petites grilles possèdent des cellules fantômes à gauche et à droite. En effet le calcul d'un pixel nécessite de connaître ses voisins.  
Le principal problème que j'ai rencontré est le fait que les grilles de végétation et de feu sont représentées par des vecteurs. Cela implique que lors du calcul des cellules fantômes, pour savoir quels valeurs prendre il faut convertir les coordonnées de chaque point en indice, et ce pour les deux grilles, et pour les colonnes de gauche et de droite. De même, lors de l'envoi des deux grilles au processus 0, comme la grille contient des cellules fantômes, il faut les enlever avant d'envoyer le message. Cela m'oblige encore une fois à parcourir les deux grilles de la simulation. Tous ces calculs risquent grandement de coûter très cher et de faire perdre beaucoup de temps.


