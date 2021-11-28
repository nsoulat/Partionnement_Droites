---
title:  "Partitionnement de droites"
category: projet GIG
authors:
- William Templeton
- Nathanaël Soulat
---

# Projet FIG : Partitionnement de droites

- [Enoncé](#enoncé)
- [Résolutions](#résolutions)
    - [Algorithmes Naïfs](#algorithmes-naïfs)
    - [Tentatives non naïves](#tentatives-non-naïves)
    - [Algorithme aléatoire](#algorithme-aléatoire)
- [Conclusion](#conclusion)
- [Installer et lancer le projet](#installer-et-lancer-le-projet)

## Enoncé

![image](https://user-images.githubusercontent.com/75546258/141136749-815f4440-a7a6-46ea-abd9-3dc462c15c71.png)

## Résolutions

On présente ici les différentes pistes explorées pour la résolution du problème. La solution retenue est l'[algorithme aléatoire](#algorithme-aléatoire).

Toutes les solutions proposées sont gérénalisables pour prendre un sous-ensemble de <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/> droites de l'ensemble de <img src="svgs/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/>.
On aura par défault <img src="svgs/fb8f3e2aa44d2cd7b5065cc512b87e47.svg?invert_in_darkmode" align=middle width=53.49304949999999pt height=22.465723500000017pt/> et <img src="svgs/e1fb72afe90507b54b1c080565b8075d.svg?invert_in_darkmode" align=middle width=86.23285604999998pt height=22.465723500000017pt/> (d'après la taille du fichier json).

### Algorithmes Naïfs

#### Algorithme naïf basique

L'algorithme va tester toutes les combinaisons possibles de <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/> droites parmi <img src="svgs/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> droites et calculer à la volée toutes les distances des droites à ce sous-ensemble, pour au final afficher quelle combinaison a eu le meilleur score (*ie* la distance la plus faible aux restes des droites).

**Résultats:** Cette méthode est impraticable pour le sujet initial. La complexité est en <img src="svgs/4785354b601502ca96d2434fefef3626.svg?invert_in_darkmode" align=middle width=58.84233134999999pt height=27.94539330000001pt/> et il faudrait des années pour savoir quel est le meilleur ensemble. Juste pour <img src="svgs/4485568012854cb7ff9d8f6f6bcff06d.svg?invert_in_darkmode" align=middle width=53.49304949999999pt height=22.465723500000017pt/> et <img src="svgs/db03ea18b1de52a4907905053e09cbaf.svg?invert_in_darkmode" align=middle width=53.35601864999999pt height=22.465723500000017pt/>, il lui faut environ 30s. Conseil: éviter d'attendre la fin de l'algorithme s'il existe trop de combinaisons.

#### Algorithme naïf - Amélioré (tableau des distances)

L'algorithme va tester toutes les combinaisons possibles de <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/> droites parmi `N` droites mais au lieu de re-calculer à chaque fois les distances à la volée, va remplir un tableau <img src="svgs/a964749a6b635295960fe89162eda4de.svg?invert_in_darkmode" align=middle width=50.091150449999994pt height=22.465723500000017pt/> contenant les distances d'une droite à l'autre.  

**Résultats:** Cette méthode est bien plus rapide que la précédente mais reste impraticable pour le sujet initial car stocker un tel tableau de `double` demanderait de bien trop grandes ressources. Cet algorithme est donc préférable au précédent pour des <img src="svgs/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> raisonnables.

#### Algorithme naïf - Optimisation

Si on reprend le premier algorithme naïf, on peut constater que calculer le maximum des minima pour ensuite prendre le minimum de ces maxima, peut se révéler être redondant. En effet, une amélioration est de passer à la combinaison suivante dès lors qu'une distance d'une droite au sous-ensemble actuel est supérieure au minimal actuel (*ie* la distance pour la meilleure combinaison actuelle), car le max sera forcément supérieure à cette distance donc la combinaison ne sera pas retenue. Cela permet donc de sauter rapidement des combinaisons et donc de réduire la complexité. Cependant il faut toujours tester toutes les combinaisons possibles...

**Résultats:** Bien qu'optimisé, il faudrait facilement plusieurs trillions d'années pour parcourir toutes les combinaisons (*ie* <img src="svgs/4f6a7e562fc9ab519df3fa80b7315c72.svg?invert_in_darkmode" align=middle width=118.6304064pt height=29.419440600000005pt/> combinaisons).

### Tentatives non naïves

Nous avons donc réfléchi à des méthodes mathématiques de résolution... afin de ne pas à avoir à parcourir toutes les combinaisons pour avoir la solution optimale.

#### Un énoncé proche

Voici l'idée derrière l'algorithme:
Imaginons qu'on ait un tableau des distances (+ simple pour la visualisation), on parcourt chaque colonne (ie chaque droite) et on récupère les <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/> plus grandes distances. On compare ensuite la <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/>-ème plus grande valeur de chaque colonne et on prend la colonne ayant celle la plus faible. La droite associée à cette colonne est la première dans la combinaison et les <img src="svgs/877f17da0d7c756840fdc976d3c9e264.svg?invert_in_darkmode" align=middle width=43.44740069999999pt height=22.465723500000017pt/> autres sont les droites les plus loins de cette droite (*ie* les <img src="svgs/877f17da0d7c756840fdc976d3c9e264.svg?invert_in_darkmode" align=middle width=43.44740069999999pt height=22.465723500000017pt/> plus grandes valeurs de sa colonne).
L'algorithme est en <img src="svgs/039750d598df867f46247ef0c8b2e03b.svg?invert_in_darkmode" align=middle width=35.64773519999999pt height=24.65753399999998pt/> car il suffit de parcourir le pseudo tableau des distances une seule fois pour avoir les droites qui composent le meilleur sous-ensemble.

**Résultats:** L'algorithme donne une réponse très rapidement (moins d'une minute, voire en quelques secondes avec optimisation). Cependant cet algorithme ne donne pas tout à fait le résultat attendu. En effet, les droites dans le sous-ensemble ne sont pas forcément proches, ainsi, cet algorithme répond seulement à l'énoncé où on minimise <img src="svgs/f2c20a5dce73fe62a09c51ca070636e6.svg?invert_in_darkmode" align=middle width=260.96494215pt height=24.7161288pt/>.

Une preuve par l'absurde peut se faire pour démontrer que cette combinaison est la meilleure (laissée au lecteur). Et la distance donnée par cet algorithme est loin d'être la meilleure (environ 6)... (en parcourant au hasard quelques combinaisons on trouve très facilement mieux).

#### Division de l'espace

On réalise qu'un sous-ensemble de 10 droites est un nombre arbitraire (surtout au vu du nombre important de droites). On commence donc évaluer les cas <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>, <img src="svgs/9ef148a4e8883871a96554228e19ff08.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>, ... pour en déduire une méthode de calcul qui n'est pas combinatoire. Le principe repose ici sur *construire la meilleure combinaison*, en faisant une analogie avec un nuage de points.

##### Cas <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>

Cela revient à trouver, parmi toutes les droites, la droite dont la distance maximale est la plus petite.
Pour simplifier la visualisation, on suppose qu'on dispose d'un tableau stockant toutes les distances calculées.
Pour chaque droite (donc chaque ligne du tableau), on ne garde que la valeur max. À chaque droite est associée sa valeur max. Le sous-ensemble correspond à la droite ayant la plus petite valeur max.

Nommons cette valeur <img src="svgs/90085a0c43d72e4deebf6ed4a8d9e014.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/>. Intuitivement, on voit ici que le sous-ensemble (composée d'une seule droite) est le "centre" de toutes les autres droites: il ne peut pas s'agir d'une droite plus excentrée, sinon sa distance max serait plus grande.

*note: ça a été plus facile de visualiser ça soit avec un nuage de points, soit des images d'électromagnétique avec des centres chargées et des lignes de niveau; mais l'idée s'applique ici aussi, on ne parle que de distances.*

> Conjecture: <img src="svgs/8d006484ae0dea7b93dba73d05172781.svg?invert_in_darkmode" align=middle width=94.16078099999999pt height=22.831056599999986pt/> ...

##### Cas <img src="svgs/9ef148a4e8883871a96554228e19ff08.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>

On cherche <img src="svgs/25eda7b7741f869a00061a631b356db9.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/>. Pour un sous-ensemble de deux droites donné, la distance d'une droite au sous-ensemble correspond à la plus petite distance entre cette droite et les deux droites du sous-ensemble. Parmi toutes les distances calculées, on prend la valeur max : c'est la distance au sous-ensemble. <img src="svgs/25eda7b7741f869a00061a631b356db9.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/> correspond à la plus petite de ces distances.

Intuitivement, on voit que le sous-ensemble constituent deux "centres". Notons <img src="svgs/eb4779c5fded13881cb5f169b1f10c73.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/> et <img src="svgs/9f0028b414617caf75a357cfb98e7497.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/> les deux droites du sous-ensemble final. Une partie des droites de <img src="svgs/78ec2b7008296ce0561cf83393cb746d.svg?invert_in_darkmode" align=middle width=14.06623184999999pt height=22.465723500000017pt/> aura une distance minimale avec <img src="svgs/eb4779c5fded13881cb5f169b1f10c73.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/> et une autre partie aura une distance minimale avec <img src="svgs/9f0028b414617caf75a357cfb98e7497.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/>. L'espace est donc partitionné en deux, ce qui signifie qu'on peut se ramener au cas <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/> suivant cette partition de l'espace. Comment réaliser cette partition ?

On part de la droite trouvée au cas précédent, et on se place sur la droite la plus éloignée de celle-ci. La première partition correspond à toutes les droites situées à moins de <img src="svgs/90085a0c43d72e4deebf6ed4a8d9e014.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/> de cette droite; la deuxième partition regroupe toutes les autres. Chaque nouveau "centre" <img src="svgs/eb4779c5fded13881cb5f169b1f10c73.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/> et <img src="svgs/9f0028b414617caf75a357cfb98e7497.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/> sera inclut dans chaque partition et correpond au "centre" de chaque partition.

Le sous-ensemble final correspond donc à la réunion des deux droites trouvées dans chaque sous-ensemble. On dispose maintenant de <img src="svgs/25eda7b7741f869a00061a631b356db9.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/> (qui est en fait la plus petite distance des deux "sous-sous-ensembles" : pas besoin de recalculer).

##### Cas <img src="svgs/6fd71b9db9f2f55921401dbdd689eb90.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>

On suit la même méthode: on se place sur la droite la plus éloignée d'un des deux centres. La première partition correpond aux droites de distances inférieures à <img src="svgs/25eda7b7741f869a00061a631b356db9.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/>. On fait de même pour l'autre centre: on obtient la deuxième partition. Les autres droites qui ne sont pas dans une des deux partitions constituent la troisième partition.

*note: il faudrait démontrer qu'utiliser <img src="svgs/90085a0c43d72e4deebf6ed4a8d9e014.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/> fournit deux partition, utiliser <img src="svgs/25eda7b7741f869a00061a631b356db9.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/> fournit trois partitions, etc.*

Pour chaque partition, on se ramène au cas <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>.

Le sous-ensemble final est la réunion des trois droites trouvées.

##### Cas général

On discerne déjà l'algorithme itératif utilisée:

- À chaque incrémentation, on dispose d'une distance <img src="svgs/852e27f19a205399dc63eb61502d7449.svg?invert_in_darkmode" align=middle width=15.82199354999999pt height=22.831056599999986pt/>
- On calcule les <img src="svgs/6b44835ef9c9df90c1ab13fe002f5bf9.svg?invert_in_darkmode" align=middle width=37.38576269999999pt height=22.831056599999986pt/> partitions à l'incrémentation d'après grâce à <img src="svgs/852e27f19a205399dc63eb61502d7449.svg?invert_in_darkmode" align=middle width=15.82199354999999pt height=22.831056599999986pt/> pour se ramener à <img src="svgs/63bb9849783d01d91403bc9a5fea12a2.svg?invert_in_darkmode" align=middle width=9.075367949999992pt height=22.831056599999986pt/> problèmes de type <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>. On dispose maintenant de <img src="svgs/ed78a318831fea1cd556da75c313f1c0.svg?invert_in_darkmode" align=middle width=32.46591314999999pt height=22.831056599999986pt/>
- On continue jusqu'à atteindre le nombre d'éléments dans le sous-enseble souhaité

##### Complexité

À chaque itération, on teste tous les distances une fois, pour trouver le centre dans chaque partition, donc complexité en <img src="svgs/a511a7422a9182316c69002e13dacf93.svg?invert_in_darkmode" align=middle width=50.78473454999999pt height=24.65753399999998pt/>.

##### Algorithme récursif

On pourrait améliorer la rapidité de l'algorithme récursivement: au lieu d'incrémenter, on diviser toujours la partition en deux:

- Cas <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>: on se retrouve avec deux partitions
- Cas <img src="svgs/9ef148a4e8883871a96554228e19ff08.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>: on trouve le "centre" de chaque partition et on l'utilise pour diviser chaque partition en deux
- etc

Inconvient: on obtient des sous-ensembles avec un nombre d'éléments correpondant à une puissance de 2 uniquement. Comme on cherche un sous-ensemble avec 10 éléments, cette méthode ne convient pas.

##### Résultats

Malheureusement, tout repose sur une analogie avec des points. Cependant la distance manipulée ici (distance entre deux droites), n'est pas une distance...

En effet une distance est une application <img src="svgs/2103f85b8b1477f430fc407cad462224.svg?invert_in_darkmode" align=middle width=8.55596444999999pt height=22.831056599999986pt/> de <img src="svgs/fc0bb3f7da2937a10b9d2eee661d0afb.svg?invert_in_darkmode" align=middle width=46.255563749999986pt height=22.465723500000017pt/> dans <img src="svgs/a80a691c1cc6accff9e6da3e7bf4e800.svg?invert_in_darkmode" align=middle width=22.57284809999999pt height=22.465723500000017pt/> vérifiant:

- <img src="svgs/d926e346bb160a501650e6ee6435d3f5.svg?invert_in_darkmode" align=middle width=115.30057934999999pt height=24.65753399999998pt/> *(symétrie)*
- <img src="svgs/276cddc3bd3a504e243738f154af6e20.svg?invert_in_darkmode" align=middle width=165.19170029999998pt height=24.65753399999998pt/>
- <img src="svgs/539843f116d56abe77b1b5e9b6d5a53e.svg?invert_in_darkmode" align=middle width=180.77428874999998pt height=24.65753399999998pt/> *(inégalité triangulaire)*

Hors ici, la "distance" entre deux droites ne vérifient pas les propriétés:

- 2\) car deux droites distinctes s'intersectant donnent une distance nulle
- 3\) car si on prend les droites <img src="svgs/332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.39498779999999pt height=14.15524440000002pt/> et <img src="svgs/f93ce33e511096ed626b4719d50f17d2.svg?invert_in_darkmode" align=middle width=8.367621899999993pt height=14.15524440000002pt/> qui ne s'intersectent pas (<img src="svgs/6de12225304ce7ef3d35ab6f1b15d56a.svg?invert_in_darkmode" align=middle width=76.54672739999998pt height=24.65753399999998pt/>), et qu'on prend une droite <img src="svgs/deceeaf6940a8c7a5a02373728002b0f.svg?invert_in_darkmode" align=middle width=8.649225749999989pt height=14.15524440000002pt/> qui intersecte à la fois <img src="svgs/332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.39498779999999pt height=14.15524440000002pt/> et <img src="svgs/f93ce33e511096ed626b4719d50f17d2.svg?invert_in_darkmode" align=middle width=8.367621899999993pt height=14.15524440000002pt/> alors on aurait <img src="svgs/c978b1a6420676fab546bc304c032ac7.svg?invert_in_darkmode" align=middle width=142.5836115pt height=24.65753399999998pt/> et donc on a contradiction de l'inégalité triangulaire.

Nous avons donc choisi de ne pas implémenter cette méthode.

#### Inclusion des solutions

Au lieu de diviser l'espace pour construire la meilleure combinaison, nous sommes partis du principe que l'espace des solutions est croissant avec <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/>. Cela signifie que pour trouver le sous-ensemble de taille <img src="svgs/63db01447b7c391e8f82f6f72e09fd36.svg?invert_in_darkmode" align=middle width=43.44740069999999pt height=22.465723500000017pt/>, il suffit de trouver la bonne droite à ajouter au sous-ensemble optimal de taille K.

On cherche donc la première droite <img src="svgs/eb4779c5fded13881cb5f169b1f10c73.svg?invert_in_darkmode" align=middle width=20.16214364999999pt height=22.465723500000017pt/> (cas <img src="svgs/6548debba4b05edacf375ef470d3292a.svg?invert_in_darkmode" align=middle width=45.273840149999984pt height=22.465723500000017pt/>). Cela se fait en complexité <img src="svgs/3987120c67ed5a9162aa9841b531c3a9.svg?invert_in_darkmode" align=middle width=43.02219404999999pt height=26.76175259999998pt/> car on a juste à parcourir le tableau des distances. On dispose de <img src="svgs/90085a0c43d72e4deebf6ed4a8d9e014.svg?invert_in_darkmode" align=middle width=15.10851044999999pt height=22.831056599999986pt/>.

Concrètement, cette droite est la "meilleure" droite: n'importe quelle autre droite augmente la distance au sous-ensemble. Il faut donc conserver cette droite et ajouter des droites pour améliorer le résultat.

Trouver la meilleure droite à ajouter se fait au pire en <img src="svgs/90846c243bb784093adbb6d2d0b2b9d0.svg?invert_in_darkmode" align=middle width=43.02219404999999pt height=26.76175259999998pt/> si on teste toutes les droites. Et on répète jusqu'à avoir un sous-ensemble de taille <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/>.

**Résultats:** Encore malheureusement, il n'y a pas inclusion entre les espaces de solution, ce qui rends faux la construction de cette "solution optimale".

> Par ailleurs, l'algorithme peut vite gagner en complexité car en réalité, il peut y avoir plusieurs solutions optimales à chaque <img src="svgs/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/>, il faut donc toutes les garder en mémoire pour tester toutes les combinaisons, et donc la complexité peut exploser en <img src="svgs/689cf0247b5199e1cdc5f5747a59e35e.svg?invert_in_darkmode" align=middle width=20.070877199999988pt height=27.6567522pt/>.

### Algorithme aléatoire

Au lieu de chercher la solution optimale à ce problème d'[optimisation combinoire](https://en.wikipedia.org/wiki/Combinatorial_optimization), en parcourant les combinaisons dans l'ordre, on peut parcourir les combinaisons dans un désordre aléatoire et retenir la meilleure solution au fil du temps.

En effet, le problème de parcourir les combinaisons dans l'ordre est que les mêmes droites sont toujours testés (il y a <img src="svgs/e7903f0773828eb319535639ccd7b93a.svg?invert_in_darkmode" align=middle width=29.54351234999999pt height=26.76175259999998pt/> combinaisons qui commencent avec la droite <img src="svgs/29632a9bf827ce0200454dd32fc3be82.svg?invert_in_darkmode" align=middle width=8.219209349999991pt height=21.18721440000001pt/>).

Parcourir aléatoirement toutes les combinaisons permet de laisser une chance à n'importe quelle combinaison.

**Résultats:** Cette méthode permet facilement d'obtenir de meilleurs résultats rapidement que les méthodes précédentes. Cependant, elle reste aussi lente que les algorithmes naïfs. On effleure une minime partie des combinaisons donc tout repose sur l'aléatoire de trouver une des meilleures solutions.

## Conclusion

Après avoir pris conscience qu'il serait vain de trouver la solution optimale, nous avons décidé de faire tourner l'algorithme aléatoire (optimisé pour les calculs et déterminer si la combinaison est meilleure) pendant plusieurs jours.

L'algorithme aléatoire a été modifié de tel sorte à s'arrêter au bout d'un certain temps ou d'un certain nombre d'itérations (le temps max et le nombre d'itération peuvent être infinis cependant, si on souhaite que cela ne s'arrête jamais).

L'algorithme commence avec la combinaison trouvé dans `résultat.json` (la meilleure combinaison actuelle). Et si jamais il trouve une meilleure combinaison, il modifie ce fichier pour y stocker la nouvelle meilleure combinaison.

> Résultat final:
>
> ```JSON
> "distance": 1,
> "droites": [1, 2, 3, 4, 5, 6, 7, 8, 9]
> ```

## Installer et lancer le projet

Cloner le projet puis

```bash
cmake .
make
```

Vous obtiendrez les *executables* `main` et `verif`.

`.\main` permet de lancer le programme principal, vous pouvez modifier à votre guise la fonction *main* pour lancer les algorithmes souhaités et régler le temps maximal / nombre d'itérations maximal pour la recherche aléatoire.

`.\verif` permet de vérifier que le fichier `resultat.json` est correct (*ie* la distance correspond bien à la distance du sous-ensemble donné dans "droites").

#### Utiliser le repo
À cause des nombreuses formules utilisées, on utilise des formules LaTeX dans le markdown. Pour la compilation du fichier `README.md`, on utilise [`readme2tex`](https://github.com/leegao/readme2tex). Il faut éditer `README_latex.md` et compiler avec: 

``` bash
python -m readme2tex --output README.md INPUT.md
```

