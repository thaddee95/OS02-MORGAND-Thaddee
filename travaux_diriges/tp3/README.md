# TD n°3 - parallélisation du Bucket Sort

*Ce TD peut être réalisé au choix, en C++ ou en Python*

Implémenter l'algorithme "bucket sort" tel que décrit sur les deux dernières planches du cours n°3 :

- le process 0 génère un tableau de nombres arbitraires,
- il les dispatch aux autres process,
- tous les process participent au tri en parallèle,
- le tableau trié est rassemblé sur le process 0.

Pour tous les tris que j'ai implémenté, j'utilise une liste de 1000 valeurs.  
Le programme `tri_sequentiel.py` contient un tri par insertion effectué de manière séquentiel. 
J'ai mesuré un temps de 0.07348465919494629 s pour ce tri séquentiel.  

J'ai tout d'abord implémenté le tri bucketsort avec des buckets réguliers. Vous pouvez trouver ce tri dans `bucketsort.py`.  

 nbProcessus | temps tri de la liste 
 ----------|----------------------------
  1        | 0.003768444061279297        
  2        | 0.003971099853515625       
  3        | 0.0032138824462890625       
  4        | 0.0032660961151123047       
  5        | 0.0032143592834472656       
  6        | 0.003574848175048828      
  7        | 0.004536628723144531       
  8        | 0.004792690277099609  

Nous pouvons remarquer que le tri bucketsort est plus efficace que le tri par insertion. Le temps de calcul est divisé par plus de 10.  

![Alt text](https://github.com/thaddee95/OS02-MORGAND-Thaddee/blob/main/travaux_diriges/tp3/images/Buckets%20r%C3%A9guliers.png)

Cependant, la courbe de speed-up n'est pas satisfaisante. Le temps effectué pour faire le tri augmente un peu lorsque nous augmentons le nombre de processus, alors que nous voudrions le voir diminuer. Cela est sûrement du aux nombreuses communications inter-processus que j'utilise dans mon code.  

J'ai ensuite implémenté le tri bucketsort avec des buckets calculés en fonction des quantiles d'ordre 1/nbp. Vous pouvez trouver ce tri dans `bucketsort_quantile.py`.  

 nbProcessus | temps tri de la liste 
 ----------|----------------------------
  1        | 0.0014493465423583984        
  2        | 0.004538536071777344       
  3        | 0.0032012462615966797      
  4        | 0.0057561397552490234       
  5        | 0.00403594970703125       
  6        | 0.002230405807495117      
  7        | 0.002183198928833008       
  8        | 0.0027344226837158203  

Pour un nombre important de processus, cette nouvelle version de bucketsort devient plus efficace. Lorsque nous affichons le contenu de chaque bucket, nous pouvons voir que les données sont mieux réparties entre les buckets pour le programme `bucketsort_quantile.py`, notamment lorsque nous concentrons les valeurs autour de 0.  

![Alt text]()  

Malheureusement, la courbe de speed-up reste mauvaise, sûrement pour les mêmes raisons. Cependant, nous pouvons voir tout de même que le temps de calcul diminue en augmentant le nombre de processus, si nous excluons le cas avec un unique processus. Le tri `bucketsort_quantile.py` reste le plus efficace pour un grand nombre de processus, ce qui est satisfaisant.
