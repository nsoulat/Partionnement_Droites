#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <limits>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

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

void copy(int* src, int* dest, int length) {
	for(int i = 0; i < length; i++) {
		dest[i] = src[i];
	}
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

	int best_combination[K];
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

	copy(combination, best_combination, K);
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
		if(dist < min){
			copy(combination, best_combination, K);
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
	std::cout << "droites: [";
	for(int i = 0; i < K-1; i++) {
		std::cout << best_combination[i] << ", ";
	}
	std::cout << best_combination[K-1] << "]" << std::endl;
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << std::endl;
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

	int best_combination[K];
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

	copy(combination, best_combination, K);
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
		if(dist < min){
			copy(combination, best_combination, K);
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
	std::cout << "droites: [";
	for(int i = 0; i < K-1; i++) {
		std::cout << best_combination[i] << ", ";
	}
	std::cout << best_combination[K-1] << "]" << std::endl;
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << std::endl;
}

/* stocke une matrice des distances + test tous les sous-ensembles
 - 100 000 x 100 000 est trop important - 
 - mais cette technique est plus optimisée pour des N plus petits */
void naif_opti(json& js) {

	const int N = js["droites"].size();
	const int K = 3; // 10 droites parmi N

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

	// std::cout << "vecteur de " << N << " droites créé" << std::endl;

	std::vector<std::vector<double>> distances;
	distances.resize(N, std::vector<double>(N, 0));
	// std::cout << "matrice " << N << "x" << N << " de distance créée avec max size: " << distances.max_size() << std::endl;
	
	for (Droite droite1 : droites) {
		for (Droite droite2 : droites) {
			distances[droite1.id][droite2.id] = distance(droite1, droite2);
		}
	}

	// std::cout << "matrice " << N << "x" << N << " de distance remplie" << std::endl;

	int best_combination[K];
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
		if(min_ensemble > max){
			max = min_ensemble;
		}
	}

	copy(combination, best_combination, K);
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
			if(min_ensemble > max){
				max = min_ensemble;
			}
		}
		// std::cout << "combinaison: ";
		// for(int i = 0; i < K; i++) {
		// 	std::cout << combination[i] << " ";
		// }
		// std::cout << ", distance: " << max << std::endl;

		if(max < min){
			copy(combination, best_combination, K);
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
	std::cout << "droites: [";
	for(int i = 0; i < K-1; i++) {
		std::cout << best_combination[i] << ", ";
	}
	std::cout << best_combination[K-1] << "]" << std::endl;
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
	std::cout << "nombre de combinaisons: " << compteur << std::endl;

	// std::cout << "\nTableau des distances" << std::endl;
	// for(int i = 0; i < N; i++) {
	// 	for(int j = 0; j < N-1; j++) {
	// 		std::cout << distances[i][j] << ", ";
	// 	}
	// 	std::cout << distances[i][N-1] << std::endl;
	// }
}

/* Un algorithme pas naïf en O(n²) */
void pas_naif(json& js) {

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
void pas_naif2(json& js) {

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
	//	{
    //		"distance": x,
	//		"droites": [id1, id2, id3, ...]
    //	}
}

int main()
{
	std::cout << "\n---*** Lecture fichier JSON ***---" << std::endl;
    auto t_json1 = std::chrono::high_resolution_clock::now();
    std::ifstream ifs("sujet5.instance.json");
	// std::ifstream ifs("test2.json");
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

	std::cout << "\n---*** Algorithme Naïf ***--" << std::endl;
	auto t_naif1 = std::chrono::high_resolution_clock::now();
	// naif(js);
    auto t_naif2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationNaif = t_naif2 - t_naif1;
   	std::cout << "algorithme naïf: " << durationNaif.count() << "s" << std::endl;

	std::cout << "\n---*** Algorithme Naïf 2 ***--" << std::endl;
	auto t_naif21 = std::chrono::high_resolution_clock::now();
	// naif2(js);
    auto t_naif22 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationNaif2 = t_naif22 - t_naif21;
   	std::cout << "algorithme naïf: " << durationNaif2.count() << "s" << std::endl;

	std::cout << "\n---*** Algorithme Naïf Opti ***--" << std::endl;
	auto t_naif_opti1 = std::chrono::high_resolution_clock::now();
	// naif_opti(js);
    auto t_naif_opti2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationNaifOpti = t_naif_opti2 - t_naif_opti1;
   	std::cout << "algorithme naïf: " << durationNaifOpti.count() << "s" << std::endl;

	std::cout << "\n---*** Algorithme Pas Naïf ***--" << std::endl;
	auto t_pas_naif1 = std::chrono::high_resolution_clock::now();
	//pas_naif(js);
    auto t_pas_naif2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationPasNaif = t_pas_naif2 - t_pas_naif1;
   	std::cout << "algorithme pas naïf: " << durationPasNaif.count() << "s" << std::endl;

	std::cout << "\n---*** Algorithme Pas Naïf 2 ***--" << std::endl;
	auto t_pas_naif21 = std::chrono::high_resolution_clock::now();
	pas_naif2(js);
    auto t_pas_naif22 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationPasNaif2 = t_pas_naif22 - t_pas_naif21;
   	std::cout << "algorithme pas naïf: " << durationPasNaif2.count() << "s" << std::endl;


    //std::cout << "---*** Résultats ***---" << std::endl;
    return 0;

}