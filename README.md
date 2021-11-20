---
title:  "Partitionnement de droites"
category: projet GIG
authors:
- "William Templeton"
- "Nathanaël Soulat"
---

Projet FIG : Partitionnement de droites

![image](https://user-images.githubusercontent.com/75546258/141136749-815f4440-a7a6-46ea-abd9-3dc462c15c71.png)

## Solutions proposées

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

### Algorithme principal

> En cours de recherche ...
