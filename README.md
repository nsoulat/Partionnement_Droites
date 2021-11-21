---
title:  "Partitionnement de droites"
category: projet GIG
authors:
- "William Templeton"
- "Nathanaël Soulat"
---

# Projet FIG : Partitionnement de droites

![image](https://user-images.githubusercontent.com/75546258/141136749-815f4440-a7a6-46ea-abd9-3dc462c15c71.png)

## Solutions proposée

Toutes les solutions proposées sont gérénalisables pour prendre un sous-ensemble de `K` droites de l'ensemble de `N`.
On aura par défault `K = 10` et `N = 100000` (d'après la taille du fichier json).

### Algorithme Naïf

L'algorithme va tester toutes les combinaisons possibles de `K` droites parmi `N` droites et calculer à la volée toutes les distances des droites à ce sous-ensemble, pour au final afficher quelle combinaison a eu le meilleur score (*ie* la distance la plus faible aux restes des droites).

**Résultats:** Cette méthode est impraticable pour le sujet initial. La complexité est en *O(n x (k parmi n))* et il faudrait des années pour savoir quel est le meilleur ensemble. Juste pour K=10 et N=23, il lui faut environ 30s. Eviter donc d'attendre la fin de l'algorithme s'il existe trop de combinaisons.

### Algorithme Naïf - Amélioré

L'algorithme va tester toutes les combinaisons possibles de `K` droites parmi `N` droites mais au lieu de re-calculer à chaque fois les distances à la volée, va remplir un tableau `N x N` contenant les distances d'une droite à l'autre.  

**Résultats:** Cette méthode est bien plus rapide que la précédente mais reste impraticable pour le sujet initial car stocker un tel tableau de `double` demanderait de bien trop grandes ressources. Cet algorithme est donc préférable au précédent pour des N raisonnables.

### Algorithme naïf - Optimisation possible

Si on reprend le premier algorithme naïf, on peut constater que calculer le maximum des minima pour ensuite prendre le minimum de ces maxima, peut se révéler être redondant. En effet, une amélioration est de passer à la combinaison suivante dès lors qu'une distance d'une droite au sous-ensemble actuel est supérieure au minimal actuel (*ie* la distance pour la meilleure combinaison actuelle), car le max sera forcément supérieure à cette distance donc la combinaison ne sera pas retenue. Cela permet donc de sauter rapidement des combinaisons et donc de réduire la complexité. Cependant il faut toujours tester toutes les combinaisons possibles...

### Première tentative non naïve

Puisque stocker toutes les distances n'est pas envisageable, les distances sont calculées à la volée à chaque fois.
L'algorithme est en *O(n²)* car il suffit de parcourir le pseudo tableau des distances une seule fois pour avoir les droites qui composent le meilleur sous-ensemble.
Voici l'idée derrière l'algorithme:
Imaginons qu'on ait un tableau des distances (+ simple pour la visualisation), on parcourt chaque colonne (ie chaque droite) et on récupère les `K` plus grandes distances. On compare ensuite la `K`-ème plus grande valeur de chaque colonne et on prend la colonne ayant celle la plus faible. La droite associée à cette colonne est la première dans la combinaison et les `K-1` autres sont les droites les plus loins de cette droite (*ie* les `K-1` plus grandes valeurs de sa colonne).
Cependant cet algorithme ne donne pas tout à fait le résultat attendu. En effet, les droites dans le sous-ensemble ne sont pas forcément proches, ainsi, cet algorithme répond seulement à l'énoncé où on minimise f2(D') = max{d(D_i, D'), D_i appartenant à D/D'}
Une preuve par l'absurde peut se faire pour démontrer que cette combinaison est la meilleure (laissée au lecteur).

### Algorithme itératif

On réalise qu'un sous-ensemble de 10 droites est un nombre arbitraire (surtout au vu du nombre important de droites). On commence donc évaluer les cas `K = 1`, `K = 2`, ... pour en déduire une méthode de calcul qui n'est pas combinatoire.

#### Cas `K = 1`
Cela revient à trouver, parmi toutes les droites, la droite dont la distance maximale est la plus petite. 
Pour simplifier la visualisation, on suppose qu'on dispose d'un tableau stockant toutes les distances calculées. 
Pour chaque droite (donc chaque ligne du tableau), on ne garde que la valeur max. À chaque droite est associée sa valeur max. Le sous-ensemble correspond à la droite ayant la plus petite valeur max. 

Nommons cette valeur `d1`. Intuitivement, on voit ici que le sous-ensemble (composée d'une seule droite) est le "centre" de toutes les autres droites: il ne peut pas s'agir d'une droite plus excentrée, sinon sa distance max serait plus grande. 

_(note: ça a été plus facile de visualiser ça soit avec un nuage de points, soit des images d'électromagnétique avec des centres chargées et des lignes de niveau; mais l'idée s'applique ici aussi, on ne parle que de distances)_

> Conjecture: `d1 > d2 > d3` ...

#### Cas `K = 2`
On cherche `d2`. Pour un sous-ensemble de deux droites donné, la distance d'une droite au sous-ensemble correspond à la plus petite distance entre cette droite et les deux droites du sous-ensemble. Parmi toutes les distances calculées, on prend la valeur max : c'est la distance au sous-ensemble. `d2` correspond à la plus petite de ces distances. 

Intuitivement, on voit que le sous-ensemble constituent deux "centres". Notons `D1` et `D2` les deux droites du sous-ensemble final. Une partie des droites de `D` aura une distance minimale avec `D1` et une autre partie aura une distance minimale avec `D2`. L'espace est donc partitionné en deux, ce qui signifie qu'on peut se ramener au cas `K = 1` suivant cette partition de l'espace. Comment réaliser cette partition ? 

On part de la droite trouvée au cas précédent, et on se place sur la droite la plus éloignée de celle-ci. La première partition correspond à toutes les droites situées à moins de `d1` de cette droite; la deuxième partition regroupe toutes les autres. Chaque nouveau "centre" `D1` et `D2` sera inclut dans chaque partition et correpond au "centre" de chaque partition. 

Le sous-ensemble final correspond donc à la réunion des deux droites trouvées dans chaque sous-ensemble. On dispose maintenant de `d2` (qui est en fait la plus petite distance des deux "sous-sous-ensembles" : pas besoin de recalculer).

#### Cas `K = 3`

On suit la même méthode: on se place sur la droite la plus éloignée d'un des deux centres. La première partition correpond aux droites de distances inférieures à `d2`. On fait de même pour l'autre centre: on obtient la deuxième partition. Les autres droites qui ne sont pas dans une des deux partitions constituent la troisième partition. 

_(note: il faudrait démontrer qu'utiliser `d1` fournit deux partition, utiliser `d2` fournit trois partitions, etc)_

Pour chaque partition, on se ramène au cas `K = 1`.

Le sous-ensemble final est la réunion des trois droites trouvées. 

#### Cas général

On discerne déjà l'algorithme itératif utilisée:
- À chaque incrémentation, on dispose d'une distance `dk`
- On calcule les `k + 1` partitions à l'incrémentation d'après grâce à `dk` pour se ramener à `k` problèmes de type `K = 1`. On dispose maintenant de `dk+1`
- On continue jusqu'à atteindre le nombre d'éléments dans le sous-enseble souhaité

#### Complexité

_J'ai pas posé proprement mais quelque chose comme du `O(Kn²)` ?_

_En tout cas on voit que c'est une méthode qui ne marche que pour des petites valeurs de K_

### Algorithme récursif

On pourrait améliorer la rapidité de l'algorithme récursivement: au lieu d'incrémenter, on diviser toujours la partition en deux:

- Cas `K = 1`: on se retrouve avec deux partitions
- Cas `K = 2`: on trouve le "centre" de chaque partition et on l'utilise pour diviser chaque partition en deux
- etc

Inconvient: on obtient des sous-ensembles avec un nombre d'éléments correpondant à une puissance de 2 uniquement. Comme on cherche un sous-ensemble avec 10 éléments, cette méthode ne convient pas.

_Complexité: comme on obtient pas ce qu'on veut je laisse ce calcul au lecteur_

### Algorithme inclusion

On calcule la première droite `D1` (cas `K = 1`). On dispose de `d1`.

Concrètement, cette droite est la "meilleure" droite: n'importe quelle autre droite augmente la distance au sous-ensemble. Il faut donc conserver cette droite et ajouter des droites pour améliorer le résultat. On suppose donc qu'il y a une inclusion: le sous-ensemble de l'iteration `K` est inclut dans l'itération `K+1`. Dans les faits, il suffit donc de rajouter une droite au sous-ensemble à chaque itération. 

Il faut aussi que les droites du sous-ensemble soit proches l'une de l'autre. On part de l'idée suivante: 

- puisque `D1` est la meilleure droite, on cherche simplement la droite `D2` qui est la plus proche de `D1` (complexité `O(n)`: on balaye la ligne de `D1` et on récupère le min). 
    - Règle: si `d(D1, D2) > d1`, on élimine la droite candidate et on passe à la deuxième valeur min, jusqu'à trouver la bonne droite `D2`.
    - On calcule `d2` qui est la distance au sous-ensemble `(D1, D2)`.
- On cherche la droite `D3` qui est la plus proche de `(D1, D2)` telle que `d(D3, (D1, D2)) < d2`, puisque on calcule la distance au sous-ensemble `(D1, D2, D3)`
- etc jusqu'à 10

>Problèmes:
>- est-ce que ça marche vraiment ?
>- est-ce que c'est la solution optimale ?


### Algorithme principal

> En cours de recherche ...
