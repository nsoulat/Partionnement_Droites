#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void save_result(const std::string name_file, double distance, std::vector<int> droites_id) {

	json j;
	j["distance"] = distance;
	j["droites"] = droites_id;

	// write prettified JSON to another file
	std::ofstream o(name_file);
	o << std::setw(4) << j << std::endl;
	std::cout << "résultat sauvegardé dans le fichier " << name_file << std::endl;
}

void save_result(const std::string name_file, double distance, int* droites_id, int K) {
	std::vector<int> vect_id;
	for(int k=0; k<K; k++){
		vect_id.push_back(droites_id[k]);
	}
	save_result(name_file, distance, vect_id);
}

class Droite {
private:
	double p[3];
	double v[3];
public:
	int id;
	Droite(double px, double py, double pz, double ux, double uy, double uz, int id)
	: id{id}
	{
		p[0] = px; p[1] = py; p[2] = pz;
		v[0] = ux; v[1] = uy; v[2] = uz;
	}
	double* getPoint(){return p;}
	double* getVect(){return v;}
};

double pscalaire(double ux, double uy, double uz, double vx, double vy, double vz) {
	return ux*vx + uy*vy + uz*vz;
}

void pvectoriel(double* w, double ux, double uy, double uz, double vx, double vy, double vz) {
	w[0] = uy*vz - uz*vy;
	w[1] = uz*vx - ux*vz;
	w[2] = ux*vy - uy*vx;
}

double norme(double ux, double uy, double uz) {
	return sqrt(ux*ux + uy*uy + uz*uz);
}

/* distance entre deux droites */
double distance(double px, double py, double pz, double ux, double uy, double uz,
			double qx, double qy, double qz, double vx, double vy, double vz) {
	double w[3];
	pvectoriel(w, ux, uy, uz, vx, vy, vz);
	double w_norme = norme(w[0], w[1], w[2]);
	double eps = 0.000000000001;
	if(w_norme < eps && w_norme > -eps)	return 0;
	else return abs(pscalaire(qx-px, qy-py, qz-pz, w[0], w[1], w[2]) / w_norme);
}

/* distance entre deux droites */
double distance(Droite& d1, Droite& d2) {
	double* p = d1.getPoint();
	double* u = d1.getVect();
	double* q = d2.getPoint();
	double* v = d2.getVect();
	double w[3];
	pvectoriel(w, u[0], u[1], u[2], v[0], v[1], v[2]);
	double w_norme = norme(w[0], w[1], w[2]);
	double eps = 0.000000000001;
	if(w_norme < eps && w_norme > -eps) return 0;
	else return abs(pscalaire(q[0]-p[0], q[1]-p[1], q[2]-p[2], w[0], w[1], w[2]) / w_norme);
}

/* distance d'une droite à un ensemble de droites */
double distance(std::vector<Droite>& droites, double px, double py, double pz, double ux, double uy, double uz, int id) {
	double min = std::numeric_limits<double>::max();
	for(Droite droite: droites) {
		if(id != droite.id) {
			double* p = droite.getPoint();
			double* v = droite.getVect();
			double dist = distance(p[0], p[1], p[2], v[0], v[1], v[2], px, py, pz, ux, uy, uz);
			if(dist < min) {
				min = dist;
			}
		}
	}
	return min;
}

/* distance d'une droite à un ensemble de droites */
double distance(std::vector<Droite>& droites, Droite d) {
	double min = std::numeric_limits<double>::max();
	for(Droite droite: droites) {
		if(d.id != droite.id) {
			double* p = droite.getPoint();
			double* v = droite.getVect();
			double* q = d.getPoint();
			double* u = d.getVect();
			double dist = distance(p[0], p[1], p[2], v[0], v[1], v[2], q[0], q[1], q[2], u[0], u[1], u[2]);
			if(dist < min) {
				min = dist;
			}
		}
	}
	return min;
}

/* fonction à minimiser -> max des distances au sous-ensemble */
double f(json& js, int N, std::vector<Droite>& droites) {
	double max = 0;
	for(int i=0; i<N; i++) {
		double dist = distance(droites,
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i
		);
		if(dist > max){
			max = dist;
		}
	}
	return max;
}

/* fonction à minimiser -> max des distances au sous-ensemble */
double f(std::vector<Droite>& droites, std::vector<Droite>& subset, double min) {
	double max = 0;
	for(Droite droite : droites) {
		double dist = distance(subset, droite);
		if(dist > max){
			max = dist;
			if(max > min){
				break;
			}
		}
	}
	return max;
}

/* fonction à minimiser -> max des distances au sous-ensemble 
dans le cas où on souhaite trouver que des meilleures (strictes) combinaisons */
double f_strict(std::vector<Droite>& droites, std::vector<Droite>& subset, double min) {
	double max = 0;
	for(Droite droite : droites) {
		double dist = distance(subset, droite);
		if(dist > max){
			max = dist;
			if(max >= min){
				break;
			}
		}
	}
	return max;
}

void copy(int* src, int* dest, int length) {
	for(int i = 0; i < length; i++) {
		dest[i] = src[i];
	}
}

void add_combination(std::vector<std::vector<int>>& best_combinations, int* combination, int K) {
	std::vector<int> new_combination;
	for (int k=0; k<K; k++) {
		new_combination.push_back(combination[k]);
	}
	best_combinations.push_back(new_combination);
}

/* algo naïf -> on teste tous les sous-ensembles
 - on recalcule les distances à la volée - */
void naif(json& js) {

	// unsigned long long compteur_quadrillion = 0;
	// unsigned long long compteur = 0;

	// unsigned long long max_compteur_quadrillon = 263409;
	// unsigned long long max_compteur = 560461970212832400;

	const int N = js["droites"].size();
	const int K = 3; // 10 droites parmi N

	std::vector<std::vector<int>> best_combinations;
    int combination[K];
	std::vector<Droite> droites;

    for (int i = 0; i < K; i++) {
        combination[i] = i;
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
    }

	add_combination(best_combinations, combination, K);
	double min = f(js, N, droites);

	// std::cout << "combinaison: ";
	// for(int i = 0; i < K; i++) {
	// 	std::cout << combination[i] << " ";
	// }
	// std::cout << ", distance: " << min << std::endl;

	unsigned long long compteur = 1;

    while (combination[0] < N-K) {
		droites.clear();
         // generate next combination in lexicographic order
        int t = K-1;
        while (t != 0 && combination[t] == N-K+t) {
            t--;
        }
        combination[t]++;
        for (int i = t+1; i < K; i++) {
            combination[i] = combination[i-1] + 1;
        }

		for (int i = 0; i < K; i++) {
			int id = combination[i];
			Droite droite = Droite(
				js["droites"][id]["point"]["x"].get<double>(),
				js["droites"][id]["point"]["y"].get<double>(),
				js["droites"][id]["point"]["z"].get<double>(),
				js["droites"][id]["vecteur"][0].get<double>(),
				js["droites"][id]["vecteur"][1].get<double>(),
				js["droites"][id]["vecteur"][2].get<double>(),
				id);
			droites.push_back(droite);
		}

		double dist = f(js, N, droites);
		if (dist == min) {
			add_combination(best_combinations, combination, K);
		}

		else if (dist < min){
			best_combinations.clear();
			add_combination(best_combinations, combination, K);
			min = dist;
			// std::cout << "combinaison: ";
			// for(int i = 0; i < K; i++) {
			// 	std::cout << best_combination[i] << " ";
			// }
			// std::cout << ", distance: " << min << std::endl;
		}

		// std::cout << "combinaison: ";
		// for(int i = 0; i < K; i++) {
		// 	std::cout << combination[i] << " ";
		// }
		// std::cout << ", distance: " << dist << std::endl;

		compteur++;
    }

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: (" << best_combinations.size() << " solutions)" << std::endl;
	for (std::vector<int> best_combination : best_combinations) {
		std::cout << "[";
		for(int k = 0; k < K-1; k++) {
			std::cout << best_combination[k] << ", ";
		}
		std::cout << best_combination[K-1] << "]" << std::endl;
	}
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << " (N=" << N << ", K=" << K << ")" << std::endl;
}

/* algo naïf -> on teste tous les sous-ensembles
 - on recalcule les distances à la volée -
 - petite amélioration : on stocke les droites au lieu de les re-récupérer dans le json à chaque fois */
void naif2(json& js) {

	// unsigned long long compteur_quadrillion = 0;
	// unsigned long long compteur = 0;

	// unsigned long long max_compteur_quadrillon = 263409;
	// unsigned long long max_compteur = 560461970212832400;

	const int N = js["droites"].size();
	const int K = 3; // 10 droites parmi N

	std::vector<std::vector<int>> best_combinations;
    int combination[K];
	std::vector<Droite> droites;

    for (int i = 0; i < K; i++) {
        combination[i] = i;
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
    }

	add_combination(best_combinations, combination, K);
	double min = f(js, N, droites);

	// std::cout << "combinaison: ";
	// for(int i = 0; i < K; i++) {
	// 	std::cout << best_combination[i] << " ";
	// }
	// std::cout << ", distance: " << min << std::endl;

	unsigned long long compteur = 1;

    while (combination[0] < N-K) {
		droites.clear();
         // generate next combination in lexicographic order
        int t = K-1;
        while (t != 0 && combination[t] == N-K+t) {
            t--;
        }
        combination[t]++;
        for (int i = t+1; i < K; i++) {
            combination[i] = combination[i-1] + 1;
        }

		for (int i = 0; i < K; i++) {
			int id = combination[i];
			Droite droite = Droite(
				js["droites"][id]["point"]["x"].get<double>(),
				js["droites"][id]["point"]["y"].get<double>(),
				js["droites"][id]["point"]["z"].get<double>(),
				js["droites"][id]["vecteur"][0].get<double>(),
				js["droites"][id]["vecteur"][1].get<double>(),
				js["droites"][id]["vecteur"][2].get<double>(),
				id);
			droites.push_back(droite);
		}
		double dist = f(js, N, droites);

		if (dist == min) {
			add_combination(best_combinations, combination, K);
		}

		else if (dist < min){
			best_combinations.clear();
			add_combination(best_combinations, combination, K);
			min = dist;
			// std::cout << "combinaison: ";
			// for(int i = 0; i < K; i++) {
			// 	std::cout << best_combination[i] << " ";
			// }
			// std::cout << ", distance: " << min << std::endl;
		}

		compteur++;
    }

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: (" << best_combinations.size() << " solutions)" << std::endl;
	for (std::vector<int> best_combination : best_combinations) {
		std::cout << "[";
		for(int k = 0; k < K-1; k++) {
			std::cout << best_combination[k] << ", ";
		}
		std::cout << best_combination[K-1] << "]" << std::endl;
	}
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << " (N=" << N << ", K=" << K << ")" << std::endl;
}

/* algo naïf -> on teste tous les sous-ensembles
 - on recalcule les distances à la volée -
 - on stocke les droites au lieu de les re-récupérer dans le json à chaque fois
 - on passe à la combinaison suivante dès que c'est sûr que c'est perdu */
void naif3(json& js) {

	// unsigned long long compteur_quadrillion = 0;
	// unsigned long long compteur = 0;

	// unsigned long long max_compteur_quadrillon = 263409;
	// unsigned long long max_compteur = 560461970212832400;

	// const int N = js["droites"].size();
	const int N = 50;
	const int K = 5; // 10 droites parmi N

	std::vector<std::vector<int>> best_combinations;
    int combination[K];
	std::vector<Droite> droites;
	std::vector<Droite> subset;

	for(int i=0; i<N; i++) {
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
	}

    for (int i = 0; i < K; i++) {
        combination[i] = i;
		subset.push_back(droites[i]);
    }

	add_combination(best_combinations, combination, K);
	double min = f(droites, subset, std::numeric_limits<double>::max());

	// std::cout << "combinaison: ";
	// for(int i = 0; i < K; i++) {
	// 	std::cout << best_combination[i] << " ";
	// }
	// std::cout << ", distance: " << min << std::endl;

	unsigned long long compteur = 1;

    while (combination[0] < N-K) {
		subset.clear();
         // generate next combination in lexicographic order
        int t = K-1;
        while (t != 0 && combination[t] == N-K+t) {
            t--;
        }
        combination[t]++;
        for (int i = t+1; i < K; i++) {
            combination[i] = combination[i-1] + 1;
        }

		for (int i = 0; i < K; i++) {
			int id = combination[i];
			subset.push_back(droites[id]);
		}

		double dist = f(droites, subset, min);

		if (dist == min) {
			add_combination(best_combinations, combination, K);
		}

		else if (dist < min){
			best_combinations.clear();
			add_combination(best_combinations, combination, K);
			min = dist;
			// std::cout << "combinaison: ";
			// for(int i = 0; i < K; i++) {
			// 	std::cout << best_combination[i] << " ";
			// }
			// std::cout << ", distance: " << min << std::endl;
		}

		compteur++;
    }

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: (" << best_combinations.size() << " solutions)" << std::endl;
	for (std::vector<int> best_combination : best_combinations) {
		std::cout << "[";
		for(int k = 0; k < K-1; k++) {
			std::cout << best_combination[k] << ", ";
		}
		std::cout << best_combination[K-1] << "]" << std::endl;
	}
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << " (N=" << N << ", K=" << K << ")" << std::endl;
}


/* stocke une matrice des distances + test tous les sous-ensembles
 - 100 000 x 100 000 est trop important - 
 - mais cette technique est plus optimisée pour des N plus petits */
void naif_opti(json& js) {

	// const int N = js["droites"].size();
	const int N = 50;
	const int K = 10; // 10 droites parmi N

	std::vector<Droite> droites;
	for (int i = 0; i < N; i++){
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
    }

	//std::cout << "vecteur de " << N << " droites créé" << std::endl;

	std::vector<std::vector<double>> distances;
	distances.resize(N, std::vector<double>(N, 0));
	//std::cout << "matrice " << N << "x" << N << " de distance créée avec max size: " << distances.max_size() << std::endl;
	
	for (Droite droite1 : droites) {
		for (Droite droite2 : droites) {
			distances[droite1.id][droite2.id] = distance(droite1, droite2);
		}
	}

	// std::cout << "matrice " << N << "x" << N << " de distance remplie" << std::endl;

	std::vector<std::vector<int>> best_combinations;
    int combination[K];

    for (int i = 0; i < K; i++) {
        combination[i] = i;
    }

	double max = 0;
	for(int i=0; i<N; i++) {
		double min_ensemble = std::numeric_limits<double>::max();
		for(int k=0; k<K; k++) {
			if(i != combination[k]) {
				double dist = distances[i][combination[k]];
				if(dist < min_ensemble) {
					min_ensemble = dist;
				}
			}
		}
		if(min_ensemble > max && min_ensemble != std::numeric_limits<double>::max()){
			max = min_ensemble;
		}
	}

	add_combination(best_combinations, combination, K);
	double min = max;

	// std::cout << "combinaison: ";
	// for(int i = 0; i < K; i++) {
	// 	std::cout << best_combination[i] << " ";
	// }
	// std::cout << ", distance: " << min << std::endl;

	unsigned long long compteur = 1;

    while (combination[0] < N-K) {
         // generate next combination in lexicographic order
        int t = K-1;
        while (t != 0 && combination[t] == N-K+t) {
            t--;
        }
        combination[t]++;
        for (int i = t+1; i < K; i++) {
            combination[i] = combination[i-1] + 1;
        }

		max = 0;
		for(int i=0; i<N; i++) {
			double min_ensemble = std::numeric_limits<double>::max();
			for(int k=0; k<K; k++) {
				if(i != combination[k]) {
					double dist = distances[i][combination[k]];
					if(dist < min_ensemble) {
						min_ensemble = dist;
					}
				}
			}
			if(min_ensemble > max && min_ensemble != std::numeric_limits<double>::max()){
				max = min_ensemble;
				if(max > min) {
					// aucune chance que ça change, on peut passer à la combinaison suivante
					i = N;
				}
			}
		}
		// std::cout << "combinaison: ";
		// for(int i = 0; i < K; i++) {
		// 	std::cout << combination[i] << " ";
		// }
		// std::cout << ", distance: " << max << std::endl;

		if(max == min) {
			add_combination(best_combinations, combination, K);
		}

		else if(max < min){
			best_combinations.clear();
			add_combination(best_combinations, combination, K);
			min = max;
			// std::cout << "combinaison: ";
			// for(int i = 0; i < K; i++) {
			// 	std::cout << best_combination[i] << " ";
			// }
			// std::cout << ", distance: " << min << std::endl;
		}

		compteur++;
    }

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: (" << best_combinations.size() << " solutions)" << std::endl;
	for (std::vector<int> best_combination : best_combinations) {
		std::cout << "[";
		for(int k = 0; k < K-1; k++) {
			std::cout << best_combination[k] << ", ";
		}
		std::cout << best_combination[K-1] << "]" << std::endl;
	}

	std::cout << "nombre de combinaisons: " << compteur << " (N=" << N << ", K=" << K << ")" << std::endl;

	// std::cout << "\nTableau des distances" << std::endl;
	// for(int i = 0; i < N; i++) {
	// 	for(int j = 0; j < N-1; j++) {
	// 		std::cout << distances[i][j] << ", ";
	// 	}
	// 	std::cout << distances[i][N-1] << std::endl;
	// }
}

/* Un algorithme pas naïf en O(n²) */
void tentative_1_pas_naif(json& js) {

	const int N = js["droites"].size();
	const int K = 10; // 10 droites parmi N

	int best_combination[K];
	double min = std::numeric_limits<double>::max();
	double max_colonne[K];
	int max_index[K];
	double dist;
	
	for (int i = 0; i < N; i++) {
		for (int k = 0; k < K; k++) {
			// remise à zéro
			max_colonne[k] = 0;
			max_index[k] = -1;
		}
		for (int j = 0; j < N; j++) {
			if (j != i) {
				// on ne veut pas ajouter i (au cas où il y ait que des droites qui s'intersectent par exemple)
				dist = distance(
					js["droites"][i]["point"]["x"].get<double>(),
					js["droites"][i]["point"]["y"].get<double>(),
					js["droites"][i]["point"]["z"].get<double>(),
					js["droites"][i]["vecteur"][0].get<double>(),
					js["droites"][i]["vecteur"][1].get<double>(),
					js["droites"][i]["vecteur"][2].get<double>(),
					js["droites"][j]["point"]["x"].get<double>(),
					js["droites"][j]["point"]["y"].get<double>(),
					js["droites"][j]["point"]["z"].get<double>(),
					js["droites"][j]["vecteur"][0].get<double>(),
					js["droites"][j]["vecteur"][1].get<double>(),
					js["droites"][j]["vecteur"][2].get<double>());
				
				if(dist > max_colonne[K-1]) {
					// insertion de dist dans max_colonne
					bool has_been_inserted = false;
					int k = 0;
					while(!has_been_inserted && k < K-1) {
						if (max_colonne[k] <= dist) {
							has_been_inserted = true;
						}
						else {
							k++;
						}
					}
					// k vaut l'indice où insérer dist dans max_colonne
					// on veut que max_colonne et max_index reste de taille K
					// on décale donc les valeurs d'index > k vers la droite (pour faire une place)
					for (int m = K-1; m > k; m--) {
						max_colonne[m] = max_colonne[m-1];
						max_index[m] = max_index[m-1];
					}
					max_colonne[k] = dist;
					max_index[k] = j;
				}
				
				if (max_colonne[K-1] >= min) {
					// cette colonne ne pourra pas avoir sa K-ieme plus grosse valeur valeur minimale
					j = N;
				}
			}
		}
		if (max_colonne[K-1] < min) {
			for(int k = 0; k < K-1; k++) {
				best_combination[k] = max_index[k];
			}
			best_combination[K-1] = i;
			min = max_colonne[K-1];
		}
	}

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: [";
	for(int i = 0; i < K-1; i++) {
		std::cout << best_combination[i] << ", ";
	}
	std::cout << best_combination[K-1] << "]" << std::endl;
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
}

/* Un algorithme pas naïf en O(n²) - on stocke les droites dans un vecteur */
void tentative_1_pas_naif2(json& js) {

	const int N = js["droites"].size();
	const int K = 10; // 10 droites parmi N

	int best_combination[K];
	double min = std::numeric_limits<double>::max();
	double max_colonne[K];
	int max_index[K];
	double dist;

	std::vector<Droite> droites;
	for (int i = 0; i < N; i++){
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
    }
	
	for (int i = 0; i < N; i++) {
		for (int k = 0; k < K; k++) {
			// remise à zéro
			max_colonne[k] = 0;
			max_index[k] = -1;
		}
		for (int j = 0; j < N; j++) {
			if (j != i) {
				// on ne veut pas ajouter i (au cas où il y ait que des droites qui s'intersectent par exemple)
				dist = distance(droites[i], droites[j]);
				
				if(dist > max_colonne[K-1]) {
					// insertion de dist dans max_colonne
					bool has_been_inserted = false;
					int k = 0;
					while(!has_been_inserted && k < K-1) {
						if (max_colonne[k] <= dist) {
							has_been_inserted = true;
						}
						else {
							k++;
						}
					}
					// k vaut l'indice où insérer dist dans max_colonne
					// on veut que max_colonne et max_index reste de taille K
					// on décale donc les valeurs d'index > k vers la droite (pour faire une place)
					for (int m = K-1; m > k; m--) {
						max_colonne[m] = max_colonne[m-1];
						max_index[m] = max_index[m-1];
					}
					max_colonne[k] = dist;
					max_index[k] = j;
				}
				
				if (max_colonne[K-1] >= min) {
					// cette colonne ne pourra pas avoir sa K-ieme plus grosse valeur valeur minimale
					j = N;
				}
			}
		}
		if (max_colonne[K-1] < min) {
			for(int k = 0; k < K-1; k++) {
				best_combination[k] = max_index[k];
			}
			best_combination[K-1] = i;
			min = max_colonne[K-1];
		}
	}

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: [";
	for(int i = 0; i < K-1; i++) {
		std::cout << best_combination[i] << ", ";
	}
	std::cout << best_combination[K-1] << "]" << std::endl;
}

bool is_in_combination(int id, std::vector<int>& combination) {
	for(int i: combination){
		if(id == i){
			return true;
		}
	}
	return false;
}

void copy(std::vector<std::vector<int>>& src, std::vector<std::vector<int>>& dest) {
	dest.clear();
	for(int i=0; i<src.size(); i++){
		std::vector<int> new_vect;
		for(int el: src[i]){
			new_vect.push_back(el);
		}
		dest.push_back(new_vect);
	}
}

/* /!\ les vecteurs de src doivent être triés */
bool is_new_combination(std::vector<std::vector<int>>& src, std::vector<int> new_comb) {
	std::sort(new_comb.begin(), new_comb.end());
	for (std::vector<int> comb : src) {
		if(comb == new_comb) return false;
	}
	return true;
}


/* Un algorithme avec itération en O(n^3) */
void tentative_2_iteration(json& js) {

	const int N = js["droites"].size();
	const int K = 5; // 10 droites parmi N

	std::vector<std::vector<int>> best_combinations;
	std::vector<std::vector<int>> old_best_combinations;
	double min = std::numeric_limits<double>::max();
	double dist, max;

	std::vector<Droite> droites;
	for (int i = 0; i < N; i++){
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
    }

	std::cout << "**itération " << 1 << "**" << std::endl;	
	// on cherche la colonne dont son max est le min parmi les colonnes
	for (int i = 0; i < N; i++) {
		max = 0;
		for (int j = 0; j < N; j++) {
			if (j != i) {
				// on ne veut pas ajouter i (au cas où il y ait que des droites qui s'intersectent par exemple)
				dist = distance(droites[i], droites[j]);
				if (dist > max) {
					max = dist;
					if (max > min) {
						// ce n'est pas la peine d'aller plus loin
						j = N;
					}
				}
			}
		}
		if (max == min) {
			int best_col[1] = {i};
			add_combination(best_combinations, best_col, 1);
		}
		else if (max < min) {
			int best_col[1] = {i};
			best_combinations.clear();
			add_combination(best_combinations, best_col, 1);
			min = max;
		}
	}

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites:";
	for (std::vector<int> best_combination : best_combinations) {
		std::cout << "[";
		for(int k = 0; k < 0; k++) {
			std::cout << best_combination[k] << ", ";
		}
		std::cout << best_combination[0] << "]" << std::endl;
	}

	std::vector<Droite> subset;

	// on réalise les itérations k=2 à k=K (on vient de faire k=1)
	for(int k=2; k<=K; k++) {
		std::cout << "**itération " << k << "**" << std::endl;
		copy(best_combinations, old_best_combinations);
		best_combinations.clear();
		for(std::vector<int> combination_before : old_best_combinations) {
			subset.clear();
			for (int iter=0; iter<combination_before.size(); iter++) {
				int id = combination_before[iter];
				subset.push_back(droites[id]);
			}

			// chaque droite qui n'est pas dans la combinaison tente d'être la nouvelle recrue
			for (int i=0; i<N; i++) {
				if (!is_in_combination(i, combination_before)) {
					combination_before.push_back(i);
					subset.push_back(droites[i]);
					if (is_new_combination(best_combinations, combination_before)) {
						dist = f(droites, subset, min);
						
						if(dist == min) {
							std::vector<int> combination_sorted = combination_before;
							std::sort(combination_sorted.begin(), combination_sorted.end());
							best_combinations.push_back(combination_sorted);
							std::cout << "la combinaison est tout aussi bien: ";
							std::cout << "[";
							for(int k1 = 0; k1 < k-1; k1++) {
								std::cout << combination_sorted[k1] << ", ";
							}
							std::cout << combination_sorted[k-1] << "]" << std::endl;
						}

						else if(dist < min){
							best_combinations.clear();
							std::vector<int> combination_sorted = combination_before;
							std::sort(combination_sorted.begin(), combination_sorted.end());
							best_combinations.push_back(combination_sorted);
							min = dist;
							std::cout << "nouvelle plus courte distance: " << min << std::endl;
							std::cout << "[";
							for(int k1 = 0; k1 < k-1; k1++) {
								std::cout << combination_sorted[k1] << ", ";
							}
							std::cout << combination_sorted[k-1] << "]" << std::endl;
						}
					}
					combination_before.pop_back();
					subset.pop_back();
				}
			}
		}
	}

	std::sort(best_combinations.begin(), best_combinations.end()); // pour un plus bel affichage

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: (" << best_combinations.size() << " solutions)" << std::endl;
	
	// std::cout << "[";
	// for(int k = 0; k < K-1; k++) {
	// 	std::cout << best_combinations[0][k] << ", ";
	// }
	// std::cout << best_combinations[0][K-1] << "]" << std::endl;

	for (std::vector<int> best_combination : best_combinations) {
		std::cout << "[";
		for(int k = 0; k < K-1; k++) {
			std::cout << best_combination[k] << ", ";
		}
		std::cout << best_combination[K-1] << "]" << std::endl;
	}
}

/* Un algorithme avec itération en O(n^3) - sans les égalités, on prend le premier meilleur venu */
void tentative_2_iteration_2(json& js) {

	const int N = js["droites"].size();
	const int K = 3; // 10 droites parmi N

	std::vector<std::vector<int>> best_combinations;
	std::vector<std::vector<int>> old_best_combinations;
	double min = std::numeric_limits<double>::max();
	double dist, max;

	std::vector<Droite> droites;
	for (int i = 0; i < N; i++){
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
    }

	// on cherche la colonne dont son max est le min parmi les colonnes
	for (int i = 0; i < N; i++) {
		max = 0;
		for (int j = 0; j < N; j++) {
			if (j != i) {
				// on ne veut pas ajouter i (au cas où il y ait que des droites qui s'intersectent par exemple)
				dist = distance(droites[i], droites[j]);
				if (dist > max) {
					max = dist;
					if (max > min) {
						// ce n'est pas la peine d'aller plus loin
						j = N;
					}
				}
			}
		}
		if (max < min) {
			int best_col[1] = {i};
			best_combinations.clear();
			add_combination(best_combinations, best_col, 1);
			min = max;
		}
	}

	// std::cout << "distance: " << min << std::endl;
	// std::cout << "droites:";
	// for (std::vector<int> best_combination : best_combinations) {
	// 	std::cout << "[";
	// 	for(int k = 0; k < 0; k++) {
	// 		std::cout << best_combination[k] << ", ";
	// 	}
	// 	std::cout << best_combination[0] << "]" << std::endl;
	// }

	std::vector<Droite> subset;

	// on réalise les itérations k=2 à k=K (on vient de faire k=1)
	for(int k=2; k<=K; k++) {
		std::cout << "**itération " << k << "**" << std::endl;
		copy(best_combinations, old_best_combinations);
		best_combinations.clear();
		for(std::vector<int> combination_before : old_best_combinations) {
			subset.clear();
			for (int iter=0; iter<combination_before.size(); iter++) {
				int id = combination_before[iter];
				subset.push_back(droites[id]);
			}

			// chaque droite qui n'est pas dans la combinaison tente d'être la nouvelle recrue
			for (int i=0; i<N; i++) {
				if (!is_in_combination(i, combination_before)) {
					combination_before.push_back(i);
					subset.push_back(droites[i]);
					if (is_new_combination(best_combinations, combination_before)) {
						dist = f(droites, subset, min);

						if(dist < min){
							best_combinations.clear();
							std::vector<int> combination_sorted = combination_before;
							std::sort(combination_sorted.begin(), combination_sorted.end());
							best_combinations.push_back(combination_sorted);
							min = dist;
							// std::cout << "nouvelle plus courte distance: " << min << std::endl;
							// std::cout << "[";
							// for(int k1 = 0; k1 < k-1; k1++) {
							// 	std::cout << combination_sorted[k1] << ", ";
							// }
							// std::cout << combination_sorted[k-1] << "]" << std::endl;
						}
					}
					combination_before.pop_back();
					subset.pop_back();
				}
			}
		}
	}

	std::sort(best_combinations.begin(), best_combinations.end()); // pour un plus bel affichage

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites: (" << best_combinations.size() << " solutions)" << std::endl;
	std::cout << "[";
	for(int k = 0; k < K-1; k++) {
		std::cout << best_combinations[0][k] << ", ";
	}
	std::cout << best_combinations[0][K-1] << "]" << std::endl;

	// for (std::vector<int> best_combination : best_combinations) {
	// 	std::cout << "[";
	// 	for(int k = 0; k < K-1; k++) {
	// 		std::cout << best_combination[k] << ", ";
	// 	}
	// 	std::cout << best_combination[K-1] << "]" << std::endl;
	// }
}

void full_random(json& js, const double t_max, const unsigned long long iter_max) {
	
	std::cout << "\n---*** Full Random ***--" << std::endl;
	
	std::cout << "temps max: ";
	if(t_max <= 0) std::cout << "INFINI";
	else std::cout << t_max << "s";
	std::cout << ", iteration max: ";
	if(iter_max == 0) std::cout << "INFINI";
	else std::cout << iter_max;
	std::cout << std::endl;

	auto start = std::chrono::high_resolution_clock::now();

	const int N = js["droites"].size();
	const int K = 10; // 10 droites parmi N
	const std::string file_resultat = "resultat.json";

	int best_combination[K];
    int combination[K];
	std::vector<Droite> droites;
	std::vector<Droite> subset;
	int random;

	for(int i=0; i<N; i++) {
		Droite droite = Droite(
			js["droites"][i]["point"]["x"].get<double>(),
			js["droites"][i]["point"]["y"].get<double>(),
			js["droites"][i]["point"]["z"].get<double>(),
			js["droites"][i]["vecteur"][0].get<double>(),
			js["droites"][i]["vecteur"][1].get<double>(),
			js["droites"][i]["vecteur"][2].get<double>(),
			i);
		droites.push_back(droite);
	}

	try{
		std::ifstream ifs_resultat(file_resultat);
		json result = json::parse(ifs_resultat);

		for(int k=0; k<K; k++){
			int id = result["droites"][k].get<int>();
			combination[k] = id;
			subset.push_back(droites[id]);
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		for (int i = 0; i < K; i++) {
			random = rand() % N;
			combination[i] = random;
			subset.push_back(droites[random]);
		}
	}

	copy(combination, best_combination, K);
	double min = f(droites, subset, std::numeric_limits<double>::max());

	std::cout << "combinaison de départ: ";
	for(int i = 0; i < K; i++) {
		std::cout << combination[i] << " ";
	}
	std::cout << ", distance: " << min << std::endl;

	unsigned long long compteur = 1;
	std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
	double time = elapsed.count();

    while ((iter_max == 0 || compteur < iter_max) && (t_max <= 0 || time < t_max)) {
		subset.clear();
        
		for (int i = 0; i < K; i++) {
			random = rand() % N;
			combination[i] = random;
			subset.push_back(droites[random]);
		}

		double dist = f_strict(droites, subset, min);

		if (dist < min){
			copy(combination, best_combination, K);
			min = dist;
			std::cout << "nouvelle meilleure combinaison: ";
			for(int i = 0; i < K; i++) {
				std::cout << combination[i] << " ";
			}
			std::cout << ", distance: " << min << std::endl;
			save_result("resultat.json", min, best_combination, K);
		}

		compteur++;
		elapsed = std::chrono::high_resolution_clock::now() - start;
		time = elapsed.count(); 
    }

	std::sort(std::begin(best_combination), std::end(best_combination)); // pour un plus bel affichage

	std::cout << "distance: " << min << std::endl;
	std::cout << "droites:" << std::endl;
	std::cout << "[";
	for(int k = 0; k < K-1; k++) {
		std::cout << best_combination[k] << ", ";
	}
	std::cout << best_combination[K-1] << "]" << std::endl;
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << " (N=" << N << ", K=" << K << ")" << std::endl;
	save_result("resultat.json", min, best_combination, K);
}

int main()
{
	// durée maximale (secondes) pour les algo de recherches
	const double t_max = 10.0 * 60; // infini si <= 0

	// nombre maximal de combinaison à tester pour les algo de recherches
	const unsigned long long iter_max = 0; // infini si 0

	/* initialize random seed: */
	srand(time(NULL));

	std::cout << "\n---*** Lecture fichier JSON ***---" << std::endl;
    auto t_json1 = std::chrono::high_resolution_clock::now();
    std::ifstream ifs("sujet5.instance.json");
	// std::ifstream ifs("test.json");
    json js = json::parse(ifs);

    // js = {
    // 		"droites": [
	// 			{
	// 				"point": {x, y, z},
	// 				"vecteur": [v_x, v_y, v_z]
	// 			}, ...
	// 		]
    // }

    auto t_json2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationJson = t_json2 - t_json1;
	std::cout << "lecture du fichier JSON: " << durationJson.count() << "s" << std::endl;
	std::cout << "nombre de droites: " << js["droites"].size() << std::endl;

	// std::cout << "\n---*** Algorithme Naïf ***--" << std::endl;
	// auto t_naif1 = std::chrono::high_resolution_clock::now();
	// naif(js);
    // auto t_naif2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationNaif = t_naif2 - t_naif1;
   	// std::cout << "algorithme naïf: " << durationNaif.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme Naïf 2 ***--" << std::endl;
	// auto t_naif21 = std::chrono::high_resolution_clock::now();
	// naif2(js);
    // auto t_naif22 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationNaif2 = t_naif22 - t_naif21;
   	// std::cout << "algorithme naïf: " << durationNaif2.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme Naïf 3 ***--" << std::endl;
	// auto t_naif31 = std::chrono::high_resolution_clock::now();
	// naif3(js);
    // auto t_naif32 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationNaif3 = t_naif32 - t_naif31;
   	// std::cout << "algorithme naïf: " << durationNaif3.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme Naïf Opti ***--" << std::endl;
	// auto t_naif_opti1 = std::chrono::high_resolution_clock::now();
	// naif_opti(js);
    // auto t_naif_opti2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationNaifOpti = t_naif_opti2 - t_naif_opti1;
   	// std::cout << "algorithme naïf: " << durationNaifOpti.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme non fonctionnel Pas Naïf ***--" << std::endl;
	// auto t_pas_naif1 = std::chrono::high_resolution_clock::now();
	// tentative_1_pas_naif(js);
    // auto t_pas_naif2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationPasNaif = t_pas_naif2 - t_pas_naif1;
   	// std::cout << "algorithme pas naïf: " << durationPasNaif.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme non fonctionnel Pas Naïf 2 ***--" << std::endl;
	// auto t_pas_naif21 = std::chrono::high_resolution_clock::now();
	// tentative_1_pas_naif2(js);
    // auto t_pas_naif22 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationPasNaif2 = t_pas_naif22 - t_pas_naif21;
   	// std::cout << "algorithme pas naïf: " << durationPasNaif2.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme Itératif ***--" << std::endl;
	// auto t_iteration1 = std::chrono::high_resolution_clock::now();
	// tentative_2_iteration(js);
    // auto t_iteration2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationIteration = t_iteration2 - t_iteration1;
   	// std::cout << "algorithme itératif: " << durationIteration.count() << "s" << std::endl;

	// std::cout << "\n---*** Algorithme Itératif sans égalité ***--" << std::endl;
	// auto t_iteration21 = std::chrono::high_resolution_clock::now();
	// tentative_2_iteration_2(js);
    // auto t_iteration22 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> durationIteration2 = t_iteration22 - t_iteration21;
   	// std::cout << "algorithme itératif sans égalité: " << durationIteration2.count() << "s" << std::endl;

	auto t_random1 = std::chrono::high_resolution_clock::now();
	full_random(js, t_max, iter_max);
    auto t_random2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationRandom = t_random2 - t_random1;
   	std::cout << "algorithme random: " << durationRandom.count() << "s" << std::endl;

    //std::cout << "---*** Résultats ***---" << std::endl;
    return 0;
}