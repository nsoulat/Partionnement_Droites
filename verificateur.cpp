#include <iostream>
#include <fstream>
#include <chrono>
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


/* distance d'une droite à un ensemble de droites */
double distance(std::vector<Droite>& droites, double px, double py, double pz, double ux, double uy, double uz, int id, int max) {
	double min = std::numeric_limits<double>::max();
	for(Droite droite: droites) {
		if(id != droite.id) {
			double* p = droite.getPoint();
			double* v = droite.getVect();
			double dist = distance(p[0], p[1], p[2], v[0], v[1], v[2], px, py, pz, ux, uy, uz);
			if(dist < min) {
				min = dist;
                if(dist < max){
                    break;
                }
			}
		}
	}
	return min;
}

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
			i,
            max
		);
		if(dist > max){
			max = dist;
		}
	}
	return max;
}

int main()
{
    auto t_1 = std::chrono::high_resolution_clock::now();
    const std::string file_resultat = "resultat.json";
    const std::string file_json = "sujet5.instance.json";

	std::cout << "\n---*** Vérification du résultat ***---" << std::endl;
    auto t_json1 = std::chrono::high_resolution_clock::now();
    std::ifstream ifs(file_json);
    json js = json::parse(ifs);
    const int N = js["droites"].size();

    auto t_json2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationJson = t_json2 - t_json1;
	std::cout << "lecture du fichier " << file_json <<": " << durationJson.count() << "s" << std::endl;
	std::cout << "nombre de droites: " << N << std::endl;

    auto t_resultat1 = std::chrono::high_resolution_clock::now();
    std::ifstream ifs_resultat(file_resultat);
    json result = json::parse(ifs_resultat);
    int K = result["droites"].size();
    double distance = result["distance"].get<double>();

    auto t_resultat2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationResultat = t_resultat2 - t_resultat1;
    std::cout << "\nlecture du fichier " << file_resultat << ": " << durationResultat.count() << "s"  << std::endl;
    std::cout << "distance: " << distance << std::endl;
    std::cout << "droites: (" << K << ")" << std::endl;
    std::cout << "[";

    std::vector<Droite> droites;
    int id;
    for(int k=0; k<K; k++){
        id = result["droites"][k].get<int>();
		Droite droite = Droite(
			js["droites"][id]["point"]["x"].get<double>(),
			js["droites"][id]["point"]["y"].get<double>(),
			js["droites"][id]["point"]["z"].get<double>(),
			js["droites"][id]["vecteur"][0].get<double>(),
			js["droites"][id]["vecteur"][1].get<double>(),
			js["droites"][id]["vecteur"][2].get<double>(),
			id);
		droites.push_back(droite);
        if(k<K-1) std::cout << id << ", ";
    }
    std::cout << id << "]" << std::endl;

    auto t_calcul1 = std::chrono::high_resolution_clock::now();
    double expected = f(js, N, droites);
    auto t_calcul2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationCalcul = t_calcul2 - t_calcul1;
    std::cout << "\nDistance réelle des droites du résultat: " << expected << std::endl;
    
    double diff = abs(expected-distance);
    double eps = 0.00001;

    std::string is_verified = diff < eps ? "TRUE" : "FALSE";
    std::cout << "le fichier résultat est-il correct: " << is_verified << std::endl;
    std::cout << "différence: " << abs(expected-distance) << std::endl; 
    
    std::cout << "temps de calcul: " << durationCalcul.count() << "s" << std::endl;

    auto t_2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = t_2 - t_1;
    std::cout << "\ntemps total de vérification: " << duration.count() << "s" << std::endl;

    return 0;
}